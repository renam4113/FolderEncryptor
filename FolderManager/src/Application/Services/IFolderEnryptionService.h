#pragma once

#include <cstddef>
#include <filesystem>

namespace Application::Services{

    class IFolderEnryptionService {
    public:
        virtual ~IFolderEnryptionService() = default;

        virtual std::size_t encryptFiles(const std::filesystem::path& folderPath) = 0;
    };
}
 // namespace Application::Services
