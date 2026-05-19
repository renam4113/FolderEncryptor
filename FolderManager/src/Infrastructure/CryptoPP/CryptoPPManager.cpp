#include "Infrastructure/CryptoPP/CryptoPPManager.h"

#include <stdexcept>
#include <algorithm>
#include <cryptopp/aes.h>
#include <cryptopp/base64.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/sha.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/osrng.h>

using namespace std;
namespace Infrastructure::CryptoPP {

CryptoPPManager::CryptoPPManager(const std::string& secret) {
    if (secret.empty()) {
        throw std::runtime_error("Ключ шифрования не может быть пустым");
    }
    _key = secret;
}

std::string CryptoPPManager::encrypt(const std::string& plainText) {
    ::CryptoPP::AutoSeededRandomPool rng;
    ::CryptoPP::byte salt[SALT_SIZE];
    ::CryptoPP::byte iv[IV_SIZE];
    rng.GenerateBlock(salt, sizeof(salt));
    rng.GenerateBlock(iv, sizeof(iv));

    ::CryptoPP::byte key[KEY_SIZE];
    ::CryptoPP::PKCS5_PBKDF2_HMAC<::CryptoPP::SHA256> kdf;
    kdf.DeriveKey(
        key, sizeof(key),
        0,
        reinterpret_cast<const ::CryptoPP::byte*>(_key.data()), _key.size(),
        salt, sizeof(salt),
        PBKDF2_ITERATIONS
    );

    std::string cipherBinary;
    try {
        ::CryptoPP::CBC_Mode<::CryptoPP::AES>::Encryption encryptor(key, sizeof(key), iv);
        ::CryptoPP::StringSource(
            plainText, true,
            new ::CryptoPP::StreamTransformationFilter(
                encryptor,
                new ::CryptoPP::StringSink(cipherBinary),
                ::CryptoPP::StreamTransformationFilter::PKCS_PADDING
            )
        );
    } catch (const ::CryptoPP::Exception& e) {
        throw std::runtime_error("Encryption failed: " + std::string(e.what()));
    }

    // Собираем результат: соль + IV + шифротекст
    std::string result;
    result.reserve(SALT_SIZE + IV_SIZE + cipherBinary.size());
    result.append(reinterpret_cast<const char*>(salt), SALT_SIZE);
    result.append(reinterpret_cast<const char*>(iv), IV_SIZE);
    result.append(cipherBinary);

    // Кодируем в Base64
    std::string resultBase64;
    ::CryptoPP::StringSource(
        result, true,
        new ::CryptoPP::Base64Encoder(new ::CryptoPP::StringSink(resultBase64), false)
    );
    return resultBase64;
}

std::string CryptoPPManager::decrypt(const std::string& encryptedTextBase64) {
    // Декодируем из Base64
    std::string encryptedBinary;
    ::CryptoPP::StringSource(
        encryptedTextBase64, true,
        new ::CryptoPP::Base64Decoder(new ::CryptoPP::StringSink(encryptedBinary))
    );

    if (encryptedBinary.size() < SALT_SIZE + IV_SIZE) {
        throw std::runtime_error("Invalid ciphertext: too short");
    }

    const ::CryptoPP::byte* data = reinterpret_cast<const ::CryptoPP::byte*>(encryptedBinary.data());
    ::CryptoPP::byte salt[SALT_SIZE];
    ::CryptoPP::byte iv[IV_SIZE];
    std::copy(data, data + SALT_SIZE, salt);
    std::copy(data + SALT_SIZE, data + SALT_SIZE + IV_SIZE, iv);
    std::string cipherText(encryptedBinary.begin() + SALT_SIZE + IV_SIZE, encryptedBinary.end());

    ::CryptoPP::byte key[KEY_SIZE];
    ::CryptoPP::PKCS5_PBKDF2_HMAC<::CryptoPP::SHA256> kdf;
    kdf.DeriveKey(
        key, sizeof(key),
        0,
        reinterpret_cast<const ::CryptoPP::byte*>(_key.data()), _key.size(),
        salt, sizeof(salt),
        PBKDF2_ITERATIONS
    );

    std::string plainText;
    try {
        ::CryptoPP::CBC_Mode<::CryptoPP::AES>::Decryption decryptor(key, sizeof(key), iv);
        ::CryptoPP::StringSource(
            cipherText, true,
            new ::CryptoPP::StreamTransformationFilter(
                decryptor,
                new ::CryptoPP::StringSink(plainText),
                ::CryptoPP::StreamTransformationFilter::PKCS_PADDING
            )
        );
    } catch (const ::CryptoPP::Exception& e) {
        throw std::runtime_error("Decryption failed: " + std::string(e.what()));
    }

    return plainText;
}

} // namespace Infrastructure::CryptoPP