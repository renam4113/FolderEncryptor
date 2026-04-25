#include "Infrastructure/CryptoPP/CryptoPPCipher.h"

#include <stdexcept>

#include <cryptopp/aes.h>
#include <cryptopp/base64.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>

namespace Infrastructure::CryptoPP {

CryptoPPCipher::CryptoPPCipher(const std::string& secret) : key_(secret) {
    if (key_.empty()) {
        throw std::invalid_argument("Secret key must not be empty");
    }
}

std::string CryptoPPCipher::encrypt(const std::string& plainText) const {
    (void)plainText;
    throw std::runtime_error("CryptoPPCipher::encrypt is not implemented yet");
}

}  // namespace Infrastructure::CryptoPP
