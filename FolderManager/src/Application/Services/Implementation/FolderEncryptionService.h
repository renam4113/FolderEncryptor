#pragma once
#include <memory>
#include "Application/InfrastructureServices/ICipher.h"
#include "Application/Services/IFolderEnryptionService.h"

namespace Application::Services::Implementation {

class FolderEncryptionService final : public Application::Services::IFolderEnryptionService {
public:
    explicit FolderEncryptionService(std::shared_ptr<Application::InfrastructureServices::ICipher> cipher);

    std::size_t encryptFiles(const std::filesystem::path& folderPath) const override;

private:
    std::shared_ptr<Application::InfrastructureServices::ICipher> cipher_;
};

}  // namespace Application::Services::Implementation
