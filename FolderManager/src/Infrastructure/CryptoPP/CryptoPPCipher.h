#pragma once

#include <string>

#include "Application/InfrastructureServices/ICipher.h"

namespace Infrastructure::CryptoPP {

class CryptoPPCipher final : public Application::InfrastructureServices::ICipher {
public:
    explicit CryptoPPCipher(const std::string& secret);

    std::string encrypt(const std::string& plainText) const override;

private:
    std::string key_;
};

}  // namespace Infrastructure::CryptoPP
