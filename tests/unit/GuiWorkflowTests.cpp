#include "gui/GuiWorkflow.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace {
int failures = 0;
#define CHECK(condition)                                                                           \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            std::cerr << __FILE__ << ':' << __LINE__ << ": check failed: " #condition "\n";        \
            ++failures;                                                                            \
        }                                                                                          \
    } while (false)

void write(const std::filesystem::path& path, std::string_view value) {
    std::ofstream stream(path, std::ios::binary);
    stream.write(value.data(), static_cast<std::streamsize>(value.size()));
}
} // namespace

int main() {
    using namespace sdrcal::gui;
    const RunSelection selection{"request.json", "trust.json", "output"};
    const auto arguments = productArguments(selection);
    CHECK(arguments.action == sdrcal::cli::ProductAction::calibrate);
    CHECK(arguments.request_path == selection.request_path);
    CHECK(arguments.trust_path == selection.trust_path);
    CHECK(arguments.output_directory == selection.output_directory);

    auto token = std::make_shared<CancellationToken>();
    CHECK(!token->cancelled());
    token->cancel();
    CHECK(token->cancelled());

    const auto root = std::filesystem::temp_directory_path() / "sdrcal-gui-workflow-tests";
    std::error_code ignored;
    std::filesystem::remove_all(root, ignored);
    std::filesystem::create_directories(root);
    write(root / "small.json", "{}");
    CHECK(reviewFile(root / "small.json").contents == "{}");
    CHECK(!reviewFile(root / "small.json", 1).ok());
    CHECK(!reviewFile(root / "missing.json").ok());
    std::filesystem::create_symlink(root / "small.json", root / "link.json", ignored);
    if (!ignored)
        CHECK(!reviewFile(root / "link.json").ok());

    for (const auto* name : {"profile.json", "evidence.json", "summary.json"})
        write(root / name, name);
    auto reviewed = reviewResultDirectory(root);
    CHECK(reviewed.size() == 3U);
    CHECK(reviewed[0].ok() && reviewed[1].ok() && reviewed[2].ok());
    write(root / "wsjtx.ini", "[Configuration]");
    reviewed = reviewResultDirectory(root);
    CHECK(reviewed.size() == 4U);
    std::filesystem::remove(root / "profile.json", ignored);
    reviewed = reviewResultDirectory(root);
    CHECK(!reviewed[0].ok());

    const RunSelection invalid{root / "missing-request.json", root / "missing-trust.json",
                               root / "new-output"};
    std::string progress;
    const auto result = runRecordedCalibration(invalid, std::make_shared<CancellationToken>(),
                                               [&](std::string text) { progress += text; });
    CHECK(result.exit == sdrcal::cli::ProductExit::input);
    CHECK(result.terminal_json.find("\"status\":\"input_error\"") != std::string::npos);
    CHECK(progress.ends_with(result.diagnostics));
    CHECK(!progress.empty());

    std::filesystem::create_directory_symlink(root, root / "directory-link", ignored);
    if (!ignored) {
        const auto linked = reviewResultDirectory(root / "directory-link");
        CHECK(linked.size() == 3U);
        CHECK(!linked[0].ok() && !linked[1].ok() && !linked[2].ok());
    }

    std::filesystem::remove_all(root, ignored);
    return failures == 0 ? 0 : 1;
}
