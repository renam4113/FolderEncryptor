#pragma once

#include <string>

using namespace std;
namespace Application::InfrastructureServices{

    class ICipher {
    public:
        virtual ~ICipher() = default;
        virtual string encrypt(const string& plainText) = 0;
        virtual string decrypt(const string& encryptedText) = 0;
    };
}