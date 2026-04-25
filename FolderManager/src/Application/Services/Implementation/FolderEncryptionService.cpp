#include "Application/Services/Implementation/FolderEncryptionService.h"
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <filesystem>

using namespace std;

namespace Application::Services::Implementation {
namespace {
const filesystem::path kEncryptionMarker = ".foldermanager.encrypted";
}

    FolderEncryptionService::FolderEncryptionService(shared_ptr<Application::InfrastructureServices::ICipher> cipher) {
        _cipher = cipher;
    }

    size_t FolderEncryptionService::encryptFiles(const filesystem::path& folderPath) {
        if (!filesystem::exists(folderPath) || !filesystem::is_directory(folderPath)) {
            throw runtime_error("Указанный путь не существует или не является директорией");
        }

        const filesystem::path markerPath = folderPath / kEncryptionMarker;
        if (filesystem::exists(markerPath)) {
            throw runtime_error("Папка уже помечена как зашифрованная");
        }

        size_t fileCount = 0;

        for (const auto& entry : filesystem::recursive_directory_iterator(folderPath)) {
            if (!filesystem::is_regular_file(entry.status())) {
                continue;
            }
            if (entry.path() == markerPath) {
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

            string encrypted = _cipher->encrypt(content);

            ofstream outFile(entry.path(), ios::binary | ios::trunc);
            if (!outFile.is_open()) {
                std::cerr << "Не удалось открыть файл для записи: " << entry.path() << "\n";
                continue;
            }

            outFile.write(encrypted.data(), encrypted.size());
            outFile.close();

            ++fileCount;
        }

        ofstream markerFile(markerPath, ios::binary | ios::trunc);
        if (!markerFile.is_open()) {
            throw runtime_error("Не удалось создать маркер зашифрованной папки");
        }
        markerFile << "encrypted";
        markerFile.close();

        return fileCount;
    }

    size_t FolderEncryptionService::decryptFiles(const filesystem::path& folderPath) {
        if (!filesystem::exists(folderPath) || !filesystem::is_directory(folderPath)) {
            throw runtime_error("Указанный путь не существует или не является директорией");
        }

        const filesystem::path markerPath = folderPath / kEncryptionMarker;
        if (!filesystem::exists(markerPath)) {
            throw runtime_error("Папка не помечена как зашифрованная");
        }

        size_t fileCount = 0;
        for (const auto& entry : filesystem::recursive_directory_iterator(folderPath)) {
            if (!filesystem::is_regular_file(entry.status())) {
                continue;
            }
            if (entry.path() == markerPath) {
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

            string decrypted;
            try {
                decrypted = _cipher->decrypt(content);
            } catch (const exception& ex) {
                cerr << "Не удалось расшифровать файл: " << entry.path() << ". Причина: " << ex.what() << "\n";
                continue;
            }

            ofstream outFile(entry.path(), ios::binary | ios::trunc);
            if (!outFile.is_open()) {
                cerr << "Не удалось открыть файл для записи: " << entry.path() << "\n";
                continue;
            }

            outFile.write(decrypted.data(), decrypted.size());
            outFile.close();
            ++fileCount;
        }

        filesystem::remove(markerPath);
        return fileCount;
    }

}