#include "Application/Services/Implementation/FolderEncryptionService.h"
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <filesystem>

using namespace std;

namespace Application::Services::Implementation {

FolderEncryptionService::FolderEncryptionService(shared_ptr<Application::InfrastructureServices::ICipher> cipher) {
    _cipher = cipher;
}

size_t FolderEncryptionService::encryptFiles(const filesystem::path& folderPath) {
    if (!filesystem::exists(folderPath) || !filesystem::is_directory(folderPath)) {
        throw runtime_error("Указанный путь не существует или не является директорией");
    }
    size_t fileCount = 0;

    for (const auto& entry : filesystem::recursive_directory_iterator(folderPath)) {
        if (!filesystem::is_regular_file(entry.status())) {
            continue;
        }

        ifstream inFile(entry.path(), ios::binary);
        if (!inFile.is_open()) {
            cerr << "Не удалось открыть файл: " << entry.path() << "\n";
            continue;
        }

        string content((istreambuf_iterator<char>(inFile)),
                            istreambuf_iterator<char>());
        inFile.close();

        string encrypted = _cipher->encrypt(content);  // используем ->

        ofstream outFile(entry.path(), ios::binary | ios::trunc);
        if (!outFile.is_open()) {
            std::cerr << "Не удалось открыть файл для записи: " << entry.path() << "\n";
            continue;
        }

        outFile.write(encrypted.data(), encrypted.size());
        outFile.close();

        ++fileCount;
    }
    return fileCount;
}

} // namespace Application::Services::Implementation