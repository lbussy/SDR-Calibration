#include "profile/ProfileStorage.h"

#include "profile/CanonicalJson.h"

#include <atomic>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <system_error>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

namespace sdrcal::profile {
namespace {

bool syncFile(const std::filesystem::path& path) {
#ifdef _WIN32
    const int descriptor = _open(path.string().c_str(), _O_RDONLY | _O_BINARY);
    if (descriptor < 0)
        return false;
    const bool ok = _commit(descriptor) == 0;
    _close(descriptor);
#else
    const int descriptor = ::open(path.c_str(), O_RDONLY);
    if (descriptor < 0)
        return false;
    const bool ok = ::fsync(descriptor) == 0;
    ::close(descriptor);
#endif
    return ok;
}

std::uint64_t processIdentifier() {
#ifdef _WIN32
    return static_cast<std::uint64_t>(GetCurrentProcessId());
#else
    return static_cast<std::uint64_t>(::getpid());
#endif
}

} // namespace

ReplacementResult replaceProfileAtomically(const std::filesystem::path& destination,
                                           const CalibrationProfile& profile,
                                           const VerifyHook& verifier, bool require_signature) {
    if (destination.empty() || !validateProfile(profile).valid())
        return {ReplacementStatus::validation_failed,
                "profile failed semantic or digest validation"};
    std::string integrity_error;
    if (!verifyIntegrity(profile, verifier, require_signature, &integrity_error))
        return {ReplacementStatus::validation_failed, integrity_error};
    const std::string serialized = serializeProfile(profile, true);
    if (serialized.empty())
        return {ReplacementStatus::validation_failed, "profile serialization failed"};
    static std::atomic<std::uint64_t> sequence{0};
    const auto nonce =
        static_cast<std::uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count()) ^
        (processIdentifier() << 32U) ^ sequence.fetch_add(1U);
    const auto temporary =
        destination.parent_path() /
        (destination.filename().string() + ".tmp-native-profile-" + std::to_string(nonce));
    std::error_code ignored;
    if (std::filesystem::exists(temporary, ignored))
        return {ReplacementStatus::write_failed, "unique temporary profile path already exists"};
    {
        std::ofstream stream(temporary, std::ios::binary | std::ios::trunc);
        if (!stream ||
            !stream.write(serialized.data(), static_cast<std::streamsize>(serialized.size())) ||
            !stream.put('\n') || !stream.flush()) {
            stream.close();
            std::filesystem::remove(temporary, ignored);
            return {ReplacementStatus::write_failed, "temporary profile write failed"};
        }
    }
    if (!syncFile(temporary)) {
        std::filesystem::remove(temporary, ignored);
        return {ReplacementStatus::sync_failed, "temporary profile synchronization failed"};
    }
#ifdef _WIN32
    bool replaced = false;
    if (std::filesystem::exists(destination)) {
        replaced = ReplaceFileW(destination.c_str(), temporary.c_str(), nullptr,
                                REPLACEFILE_WRITE_THROUGH, nullptr, nullptr) != 0;
    } else {
        replaced = MoveFileExW(temporary.c_str(), destination.c_str(), MOVEFILE_WRITE_THROUGH) != 0;
    }
    if (!replaced) {
#else
    std::filesystem::rename(temporary, destination, ignored);
    if (ignored) {
#endif
        std::filesystem::remove(temporary, ignored);
        return {ReplacementStatus::replace_failed, "atomic rename failed"};
    }
    return {ReplacementStatus::success, "profile atomically replaced"};
}

} // namespace sdrcal::profile
