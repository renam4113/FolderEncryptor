#pragma once
#include <memory>
#include "Application/InfrastructureServices/ICryptoPPManager.h"
#include "Application/Services/IFolderEnryptionService.h"

using namespace std;
namespace Application::Services::Implementation {
    class FolderEncryptionService: public Application::Services::IFolderEnryptionService {
    public:
        FolderEncryptionService(shared_ptr<Application::InfrastructureServices::ICryptoPPManager> cipher);
        size_t encryptFiles(const filesystem::path& folderPath) override;
        size_t decryptFiles(const filesystem::path& folderPath) override;

    private:
        shared_ptr<Application::InfrastructureServices::ICryptoPPManager> _cipher;
    };
}
