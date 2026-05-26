#pragma once
#include <memory>
#include <filesystem>
#include "Application/InfrastructureServices/ICryptoPPManager.h"
#include "Application/Services/IFolderEnryptionService.h"

namespace Application::Services::Implementation {

    class FolderEncryptionService : public IFolderEnryptionService {
    public:
        static FolderEncryptionService& GetInstance(
            std::shared_ptr<Application::InfrastructureServices::ICryptoPPManager> cipher = nullptr);

        FolderEncryptionService(const FolderEncryptionService&) = delete;
        FolderEncryptionService& operator=(const FolderEncryptionService&) = delete;

        size_t encryptFiles(const std::filesystem::path& folderPath) override;
        size_t decryptFiles(const std::filesystem::path& folderPath) override;

    private:
        explicit FolderEncryptionService(std::shared_ptr<Application::InfrastructureServices::ICryptoPPManager> cipher);
        std::shared_ptr<Application::InfrastructureServices::ICryptoPPManager> _cipher;
        static std::unique_ptr<FolderEncryptionService> s_instance;
    };
}