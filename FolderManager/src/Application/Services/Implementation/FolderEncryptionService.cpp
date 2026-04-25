#include "Application/Services/Implementation/FolderEncryptionService.h"

#include <stdexcept>

namespace Application::Services::Implementation {

FolderEncryptionService::FolderEncryptionService(
    std::shared_ptr<Application::InfrastructureServices::ICipher> cipher)
    : cipher_(std::move(cipher)) {
    if (!cipher_) {
        throw std::invalid_argument("Cipher dependency must not be null");
    }
}

std::size_t FolderEncryptionService::encryptFiles(const std::filesystem::path& folderPath) const {
    (void)folderPath;

    // TODO: add recursive file processing and persist encrypted output.
    const auto encryptedSample = cipher_->encrypt("sample");
    (void)encryptedSample;

    return 0;
}

}  // namespace Application::Services::Implementation
