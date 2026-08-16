#pragma once

#include "application/CalibrationWorkflow.h"

#include <chrono>
#include <filesystem>
#include <functional>
#include <iosfwd>
#include <map>
#include <memory>

namespace sdrcal::cli {

enum class ProductAction { calibrate, help, version };
enum class ProductExit : int {
    success = 0,
    usage = 2,
    input = 3,
    workflow = 4,
    output = 5,
    cancelled = 130
};

struct ProductArguments {
    ProductAction action = ProductAction::calibrate;
    std::filesystem::path request_path;
    std::filesystem::path trust_path;
    std::filesystem::path output_directory;
    std::vector<std::string> errors;
    [[nodiscard]] bool ok() const noexcept {
        return errors.empty();
    }
};

struct RecordedObservation {
    application::ObservationRequest request;
    std::filesystem::path input_path;
    std::string sha256;
    std::uint64_t maximum_bytes = 0;
    std::uint64_t sample_count = 0;
    double duration_seconds = 0.0;
    double effective_indicated_center_frequency_hz = 0.0;
    double signal_to_noise_db = 0.0;
    std::uint64_t clipped_samples = 0;
    std::uint64_t missing_samples = 0;
    std::uint64_t discontinuities = 0;
    double frequency_instability_hz = 0.0;
    double interference_to_carrier_db = 0.0;
    bool reference_conditions_met = false;
};

enum class ProductInputMode { recorded, live };

struct LiveAcquisitionRequest {
    std::map<std::string, std::string> device_arguments;
    std::size_t rx_channel = 0;
    std::optional<double> gain_db;
    std::optional<double> duration_seconds;
    std::optional<std::uint64_t> sample_count;
    std::chrono::milliseconds read_timeout{100};
    std::uint64_t maximum_memory_bytes = 0;
};

struct ProductRequest {
    ProductInputMode input_mode = ProductInputMode::recorded;
    application::WorkflowRequest workflow;
    application::DeviceCandidate device;
    std::vector<RecordedObservation> observations;
    std::optional<LiveAcquisitionRequest> live_acquisition;
    std::map<std::string, std::string> trusted_registry_signatures;
};

struct ProductRequestResult {
    std::optional<ProductRequest> request;
    std::vector<std::string> errors;
    [[nodiscard]] bool ok() const noexcept {
        return request.has_value() && errors.empty();
    }
};

using ProductCancellationCheck = std::function<bool()>;
using LiveBoundaryFactory =
    std::function<std::unique_ptr<application::DeviceWorkflowBoundary>(const ProductRequest&)>;

[[nodiscard]] std::uint64_t recordedInputMaximumBytes() noexcept;

[[nodiscard]] ProductArguments parseProductArguments(const std::vector<std::string>& arguments);
[[nodiscard]] ProductRequestResult
parseProductRequest(std::string_view json, const std::filesystem::path& base_directory,
                    std::map<std::string, std::string> trusted_signatures);
[[nodiscard]] std::string productUsage();
[[nodiscard]] ProductExit runProductCommand(const ProductArguments& arguments, std::ostream& output,
                                            std::ostream& diagnostics,
                                            ProductCancellationCheck cancelled = {},
                                            LiveBoundaryFactory live_boundary_factory = {});

} // namespace sdrcal::cli
