#pragma once

#include "capture/CaptureTypes.h"

#include <filesystem>
#include <string>

namespace sdrcal::capture {

class CaptureManifestWriter {
public:
    [[nodiscard]] static std::string serialize(
        const CapturePlan& plan,
        const DeviceMetadata& device,
        const CaptureResult& result,
        const std::filesystem::path& rawFilename);
    [[nodiscard]] static bool write(
        const std::filesystem::path& path,
        const std::string& json,
        std::string& error);
};

} // namespace sdrcal::capture
