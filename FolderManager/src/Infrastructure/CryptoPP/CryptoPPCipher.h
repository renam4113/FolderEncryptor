#pragma once

#include <string>

#include "Application/InfrastructureServices/ICipher.h"

using namespace std;
namespace Infrastructure::CryptoPP {

class CryptoPPCipher: public Application::InfrastructureServices::ICipher {
    
public:
    CryptoPPCipher(const string& secret);
    string encrypt(const string& plainText) override;
    string decrypt(const string& encryptedText) override;

private:
    string _key;
};

}
