#pragma once

#include "Application/InfrastructureServices/ICryptoPPManager.h"
#include <string>
#include <cryptopp/aes.h>


using namespace std;
namespace Infrastructure::CryptoPP {

class CryptoPPManager: public Application::InfrastructureServices::ICryptoPPManager {
    
public:
    CryptoPPManager(const string& secret);
    string encrypt(const string& plainText) override;
    string decrypt(const string& encryptedText) override;

private:
    string _key;
    static const size_t SALT_SIZE = 16;               
    static const size_t KEY_SIZE = 32;                
    static const size_t IV_SIZE = ::CryptoPP::AES::BLOCKSIZE; 
    static const unsigned int PBKDF2_ITERATIONS = 100000;    
};
}
