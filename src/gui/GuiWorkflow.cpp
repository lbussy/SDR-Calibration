#include "gui/GuiWorkflow.h"

#include <fstream>
#include <sstream>
#include <system_error>

namespace sdrcal::gui {
namespace {
class ProgressBuffer final : public std::streambuf {
  public:
    explicit ProgressBuffer(ProgressCallback callback) : callback_(std::move(callback)) {}
    [[nodiscard]] const std::string& completeText() const noexcept {
        return complete_;
    }

  protected:
    int sync() override {
        emitPending();
        return 0;
    }
    int overflow(int character) override {
        if (character == traits_type::eof())
            return sync() == 0 ? traits_type::not_eof(character) : traits_type::eof();
        append(static_cast<char>(character));
        return character;
    }
    std::streamsize xsputn(const char* text, std::streamsize count) override {
        for (std::streamsize index = 0; index < count; ++index)
            append(text[index]);
        return count;
    }

  private:
    void append(char character) {
        complete_.push_back(character);
        pending_.push_back(character);
        if (character == '\n')
            emitPending();
    }
    void emitPending() {
        if (!pending_.empty() && callback_)
            callback_(pending_);
        pending_.clear();
    }

    ProgressCallback callback_;
    std::string complete_;
    std::string pending_;
};
} // namespace

void CancellationToken::cancel() noexcept {
    cancelled_.store(true);
}

bool CancellationToken::cancelled() const noexcept {
    return cancelled_.load();
}

cli::ProductArguments productArguments(const RunSelection& selection) {
    return {cli::ProductAction::calibrate,
            selection.request_path,
            selection.trust_path,
            selection.output_directory,
            {}};
}

RunResult runRecordedCalibration(const RunSelection& selection,
                                 const std::shared_ptr<CancellationToken>& token,
                                 ProgressCallback progress) {
    std::ostringstream output;
    if (progress)
        progress("Starting production recorded-input validation.\n");
    ProgressBuffer progressBuffer(progress);
    std::ostream diagnostics(&progressBuffer);
    const auto exit = cli::runProductCommand(productArguments(selection), output, diagnostics,
                                             [token] { return token && token->cancelled(); });
    diagnostics.flush();
    return {exit, output.str(), progressBuffer.completeText()};
}

ReviewedFile reviewFile(const std::filesystem::path& path, std::uint64_t limit) {
    ReviewedFile result;
    result.path = path;
    std::error_code error;
    const auto status = std::filesystem::symlink_status(path, error);
    if (error || !std::filesystem::is_regular_file(status) || std::filesystem::is_symlink(status)) {
        result.error = "not an available regular file";
        return result;
    }
    const auto size = std::filesystem::file_size(path, error);
    if (error || size > limit) {
        result.error = "file exceeds the review limit or its size is unavailable";
        return result;
    }
    std::ifstream stream(path, std::ios::binary);
    result.contents.resize(static_cast<std::size_t>(size));
    if (!stream.read(result.contents.data(),
                     static_cast<std::streamsize>(result.contents.size()))) {
        result.contents.clear();
        result.error = "file could not be read completely";
    }
    return result;
}

std::vector<ReviewedFile> reviewResultDirectory(const std::filesystem::path& path) {
    std::vector<ReviewedFile> result;
    std::error_code directoryError;
    const auto status = std::filesystem::symlink_status(path, directoryError);
    if (directoryError || !std::filesystem::is_directory(status) ||
        std::filesystem::is_symlink(status)) {
        for (const auto* name : {"profile.json", "evidence.json", "summary.json"})
            result.push_back(
                {path / name, {}, "result path is not an available regular directory"});
        return result;
    }
    for (const auto* name : {"profile.json", "evidence.json", "summary.json"})
        result.push_back(reviewFile(path / name));
    const auto wsjtx = path / "wsjtx.ini";
    std::error_code error;
    if (std::filesystem::exists(wsjtx, error) && !error)
        result.push_back(reviewFile(wsjtx));
    return result;
}

} // namespace sdrcal::gui
