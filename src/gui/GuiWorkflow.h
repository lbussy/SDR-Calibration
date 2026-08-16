#pragma once

#include "cli/ProductionCli.h"

#include <atomic>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace sdrcal::gui {

inline constexpr std::uint64_t review_file_limit = 8U * 1024U * 1024U;

struct RunSelection {
    std::filesystem::path request_path;
    std::filesystem::path trust_path;
    std::filesystem::path output_directory;
};

struct RunResult {
    cli::ProductExit exit = cli::ProductExit::input;
    std::string terminal_json;
    std::string diagnostics;
};

using ProgressCallback = std::function<void(std::string)>;

struct ReviewedFile {
    std::filesystem::path path;
    std::string contents;
    std::string error;
    [[nodiscard]] bool ok() const noexcept {
        return error.empty();
    }
};

class CancellationToken {
  public:
    void cancel() noexcept;
    [[nodiscard]] bool cancelled() const noexcept;

  private:
    std::atomic_bool cancelled_{false};
};

[[nodiscard]] cli::ProductArguments productArguments(const RunSelection& selection);
[[nodiscard]] RunResult runCalibration(const RunSelection& selection,
                                       const std::shared_ptr<CancellationToken>& token,
                                       ProgressCallback progress = {},
                                       cli::LiveBoundaryFactory live_boundary_factory = {});
[[nodiscard]] ReviewedFile reviewFile(const std::filesystem::path& path,
                                      std::uint64_t limit = review_file_limit);
[[nodiscard]] std::vector<ReviewedFile> reviewResultDirectory(const std::filesystem::path& path);

} // namespace sdrcal::gui
