#pragma once

#include <cstddef>
#include <filesystem>

using namespace std;
namespace Application::Services{

    class IFolderEnryptionService {
    public:
        virtual ~IFolderEnryptionService() = default;

        virtual size_t encryptFiles(const filesystem::path& folderPath) = 0;
        virtual size_t decryptFiles(const filesystem::path& folderPath) = 0;
    };
}
