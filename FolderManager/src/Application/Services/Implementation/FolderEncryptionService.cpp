#include "Application/Services/Implementation/FolderEncryptionService.h"
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <filesystem>
#include <vector>
#include <utility>

namespace Application::Services::Implementation {
namespace {

const std::filesystem::path kEncryptionMarker = ".foldermanager.encrypted";
const std::filesystem::path kBackupDirName = ".foldermanager_backup";

void RestoreFromBackup(const std::filesystem::path& backupPath,
                       const std::filesystem::path& originalPath) {
    std::error_code ec;
    std::filesystem::rename(backupPath, originalPath, ec);
    if (!ec) {
        return;
    }
    std::ifstream bak(backupPath, std::ios::binary);
    if (!bak) {
        return;
    }
    std::string content((std::istreambuf_iterator<char>(bak)),
                        std::istreambuf_iterator<char>());
    bak.close();
    std::ofstream orig(originalPath, std::ios::binary | std::ios::trunc);
    if (orig) {
        orig.write(content.data(), content.size());
    }
    std::filesystem::remove(backupPath, ec);
}

}

std::unique_ptr<FolderEncryptionService> FolderEncryptionService::s_instance = nullptr;

FolderEncryptionService& FolderEncryptionService::GetInstance(
    std::shared_ptr<Application::InfrastructureServices::ICryptoPPManager> cipher) {
    if (!s_instance) {
        if (!cipher) {
            throw std::invalid_argument("Cipher must not be null for initial creation");
        }
        s_instance = std::unique_ptr<FolderEncryptionService>(new FolderEncryptionService(cipher));
    }
    return *s_instance;
}


FolderEncryptionService::FolderEncryptionService(
    std::shared_ptr<Application::InfrastructureServices::ICryptoPPManager> cipher)
    : _cipher(std::move(cipher)) {}

size_t FolderEncryptionService::encryptFiles(const std::filesystem::path& folderPath) {
    using std::filesystem::exists;
    using std::filesystem::is_directory;
    using std::filesystem::is_regular_file;
    using std::filesystem::path;

    if (!exists(folderPath) || !is_directory(folderPath)) {
        throw std::runtime_error("Указанный путь не существует или не является директорией");
    }

    const path markerPath = folderPath / kEncryptionMarker;
    if (exists(markerPath)) {
        throw std::runtime_error("Папка уже помечена как зашифрованная");
    }

    const path backupDir = folderPath / kBackupDirName;
    if (exists(backupDir)) {
        throw std::runtime_error(
            "Обнаружена папка временных бекапов " + backupDir.string() +
            ". Возможно, предыдущая операция была прервана. "
            "Удалите её вручную перед повторной попыткой.");
    }

    std::error_code ec;
    if (!std::filesystem::create_directory(backupDir, ec)) {
        throw std::runtime_error("Не удалось создать папку для бекапов: " + backupDir.string());
    }

    std::vector<std::pair<path, path>> modifiedFiles;
    size_t fileCount = 0;

    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(folderPath)) {
            if (!is_regular_file(entry.status()))
                continue;
            if (entry.path() == markerPath)
                continue;
            if (entry.path().string().find(backupDir.string()) == 0)
                continue;

            const path originalPath = entry.path();
            const path relativePath = std::filesystem::relative(originalPath, folderPath);
            const path backupPath = backupDir / relativePath;

            std::filesystem::create_directories(backupPath.parent_path(), ec);
            if (ec) {
                throw std::runtime_error("Не удалось создать директорию для бекапа: " + backupPath.parent_path().string());
            }
            std::filesystem::copy_file(originalPath, backupPath, std::filesystem::copy_options::overwrite_existing, ec);
            if (ec) {
                throw std::runtime_error("Не удалось скопировать файл в бекап: " + originalPath.string());
            }

            modifiedFiles.emplace_back(originalPath, backupPath);

            std::ifstream inFile(originalPath, std::ios::binary);
            if (!inFile.is_open()) {
                throw std::runtime_error("Не удалось открыть файл для чтения: " + originalPath.string());
            }
            std::string plain((std::istreambuf_iterator<char>(inFile)),
                              std::istreambuf_iterator<char>());
            inFile.close();

            std::string encrypted = _cipher->encrypt(plain);

            std::ofstream outFile(originalPath, std::ios::binary | std::ios::trunc);
            if (!outFile.is_open()) {
                throw std::runtime_error("Не удалось открыть файл для записи: " + originalPath.string());
            }
            outFile.write(encrypted.data(), encrypted.size());
            outFile.close();

            ++fileCount;
        }

        std::filesystem::remove_all(backupDir, ec);
        if (ec) {
            std::cerr << "Предупреждение: не удалось удалить папку бекапов: " << backupDir.string() << '\n';
        }

        std::ofstream markerFile(markerPath);
        if (!markerFile) {
            throw std::runtime_error("Не удалось создать маркер зашифрованной папки");
        }
        markerFile << "encrypted";
        markerFile.close();

        return fileCount;
    } catch (...) {
        for (const auto& [original, backup] : modifiedFiles) {
            RestoreFromBackup(backup, original);
        }
        std::error_code cleanupEc;
        std::filesystem::remove_all(backupDir, cleanupEc);
        throw;
    }
}

size_t FolderEncryptionService::decryptFiles(const std::filesystem::path& folderPath) {
    using std::filesystem::exists;
    using std::filesystem::is_directory;
    using std::filesystem::is_regular_file;
    using std::filesystem::path;

    if (!exists(folderPath) || !is_directory(folderPath)) {
        throw std::runtime_error("Указанный путь не существует или не является директорией");
    }

    const path markerPath = folderPath / kEncryptionMarker;
    if (!exists(markerPath)) {
        throw std::runtime_error("Папка не помечена как зашифрованная");
    }

    const path backupDir = folderPath / kBackupDirName;
    if (exists(backupDir)) {
        throw std::runtime_error(
            "Обнаружена папка временных бекапов " + backupDir.string() +
            ". Возможно, предыдущая операция была прервана. "
            "Удалите её вручную перед повторной попыткой.");
    }

    std::error_code ec;
    if (!std::filesystem::create_directory(backupDir, ec)) {
        throw std::runtime_error("Не удалось создать папку для бекапов: " + backupDir.string());
    }

    std::vector<std::pair<path, path>> modifiedFiles;
    size_t fileCount = 0;

    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(folderPath)) {
            if (!is_regular_file(entry.status()))
                continue;
            if (entry.path() == markerPath)
                continue;
            if (entry.path().string().find(backupDir.string()) == 0)
                continue;

            const path originalPath = entry.path();
            const path relativePath = std::filesystem::relative(originalPath, folderPath);
            const path backupPath = backupDir / relativePath;

            std::filesystem::create_directories(backupPath.parent_path(), ec);
            if (ec) {
                throw std::runtime_error("Не удалось создать директорию для бекапа: " + backupPath.parent_path().string());
            }
            std::filesystem::copy_file(originalPath, backupPath, std::filesystem::copy_options::overwrite_existing, ec);
            if (ec) {
                throw std::runtime_error("Не удалось скопировать файл в бекап: " + originalPath.string());
            }

            modifiedFiles.emplace_back(originalPath, backupPath);

            std::ifstream inFile(originalPath, std::ios::binary);
            if (!inFile.is_open()) {
                throw std::runtime_error("Не удалось открыть файл для чтения: " + originalPath.string());
            }
            std::string encrypted((std::istreambuf_iterator<char>(inFile)),
                                  std::istreambuf_iterator<char>());
            inFile.close();

            std::string decrypted = _cipher->decrypt(encrypted);

            std::ofstream outFile(originalPath, std::ios::binary | std::ios::trunc);
            if (!outFile.is_open()) {
                throw std::runtime_error("Не удалось открыть файл для записи: " + originalPath.string());
            }
            outFile.write(decrypted.data(), decrypted.size());
            outFile.close();

            ++fileCount;
        }

        std::filesystem::remove_all(backupDir, ec);
        if (ec) {
            std::cerr << "Предупреждение: не удалось удалить папку бекапов: " << backupDir.string() << '\n';
        }

        std::filesystem::remove(markerPath, ec);
        if (ec) {
            throw std::runtime_error("Не удалось удалить маркер шифрования");
        }

        return fileCount;
    } catch (...) {
        for (const auto& [original, backup] : modifiedFiles) {
            RestoreFromBackup(backup, original);
        }
        std::error_code ex;
        std::filesystem::remove_all(backupDir, ex);
        throw;
    }
}

}