#pragma once
#include <memory>
#include "Application/InfrastructureServices/ICipher.h"
#include "Application/Services/IFolderEnryptionService.h"

using namespace std;
namespace Application::Services::Implementation {
    class FolderEncryptionService: public Application::Services::IFolderEnryptionService {
    public:
        FolderEncryptionService(shared_ptr<Application::InfrastructureServices::ICipher> cipher);
        size_t encryptFiles(const filesystem::path& folderPath) override;

    private:
        shared_ptr<Application::InfrastructureServices::ICipher> _cipher;
    };
}  // namespace Application::Services::Implementation
