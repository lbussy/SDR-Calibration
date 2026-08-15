#pragma once

#include "soapy/SoapyApi.h"

namespace sdrcal::soapy {

class RealSoapyApi final : public SoapyApi {
public:
    [[nodiscard]] std::vector<KeywordMap> enumerate(const KeywordMap& filter) override;
    [[nodiscard]] SoapyDevice* make(const KeywordMap& arguments) override;
    void unmake(SoapyDevice* device) override;
    [[nodiscard]] std::string libraryVersion() const override;
    [[nodiscard]] std::string apiVersion() const override;
    [[nodiscard]] std::string abiVersion() const override;
};

} // namespace sdrcal::soapy
