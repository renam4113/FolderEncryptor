#include "Infrastructure/CryptoPP/CryptoPPManager.h"

#include <stdexcept>

#include <cryptopp/aes.h>
#include <cryptopp/base64.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>

using namespace std;
namespace Infrastructure::CryptoPP{
    CryptoPPManager::CryptoPPManager(const string& secret){
        if (secret.empty()) {
            throw std::runtime_error("Ключ шифрования не может быть пустым");
        }
        _key = secret;
    }

    string CryptoPPManager::encrypt(const string& plainText){
        string normalizedKey = _key;
        normalizedKey.resize(::CryptoPP::AES::DEFAULT_KEYLENGTH, '\0');

        ::CryptoPP::byte iv[::CryptoPP::AES::BLOCKSIZE] = {};
        ::CryptoPP::CBC_Mode<::CryptoPP::AES>::Encryption encryptor(
            reinterpret_cast<const ::CryptoPP::byte*>(normalizedKey.data()),
            ::CryptoPP::AES::DEFAULT_KEYLENGTH,
            iv);

        string encryptedText = "";
        ::CryptoPP::StringSource source(
            plainText,
            true,
            new ::CryptoPP::StreamTransformationFilter(
                encryptor,
                new ::CryptoPP::Base64Encoder(
                    new ::CryptoPP::StringSink(encryptedText),
                    false)));
        return encryptedText;
    }

    string CryptoPPManager::decrypt(const string& encryptedText){
        string normalizedKey = _key;
        normalizedKey.resize(::CryptoPP::AES::DEFAULT_KEYLENGTH, '\0');

        ::CryptoPP::byte iv[::CryptoPP::AES::BLOCKSIZE] = {};
        ::CryptoPP::CBC_Mode<::CryptoPP::AES>::Decryption decryptor(
            reinterpret_cast<const ::CryptoPP::byte*>(normalizedKey.data()),
            ::CryptoPP::AES::DEFAULT_KEYLENGTH,
            iv);

        string plainText = "";
         ::CryptoPP::StringSource source(
            encryptedText,
            true,
            new ::CryptoPP::StreamTransformationFilter(
                decryptor,
                new ::CryptoPP::Base64Decoder(
                    new ::CryptoPP::StringSink(plainText))));
        return plainText;
    }
}