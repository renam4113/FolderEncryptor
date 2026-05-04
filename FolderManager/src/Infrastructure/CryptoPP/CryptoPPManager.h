#pragma once

#include <string>

#include "Application/InfrastructureServices/ICryptoPPManager.h"

using namespace std;
namespace Infrastructure::CryptoPP {

class CryptoPPManager: public Application::InfrastructureServices::ICryptoPPManager {
    
public:
    CryptoPPManager(const string& secret);
    string encrypt(const string& plainText) override;
    string decrypt(const string& encryptedText) override;

private:
    string _key;
};

}
