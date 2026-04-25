#pragma once

#include <string>

namespace Application::InfrastructureServices {

class ICipher {
public:
    virtual ~ICipher() = default;

    virtual std::string encrypt(const std::string& plainText) const = 0;
};

}  // namespace Application::InfrastructureServices
