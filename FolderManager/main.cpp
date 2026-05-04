#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include <windows.h>

#include "Application/Services/Implementation/FolderEncryptionService.h"
#include "Infrastructure/CryptoPP/CryptoPPManager.h"

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    try {
        std::cout << "Введите путь к директории для шифрования: ";
        std::string inputPath;
        std::getline(std::cin, inputPath);

        std::filesystem::path startPath;
        if (inputPath.empty()) {
            startPath = std::filesystem::current_path();
            std::cout << "Путь не введён, используется текущая директория: " << startPath << "\n";
        } else {
            startPath = std::filesystem::path(inputPath);
        }
        std::cout << "Введите секретный ключ (пароль): ";
        std::string secret;
        std::getline(std::cin, secret);

        auto cipher = std::make_shared<Infrastructure::CryptoPP::CryptoPPManager>(secret);
        Application::Services::Implementation::FolderEncryptionService service(std::move(cipher));
        const std::size_t encryptedFiles = service.encryptFiles(startPath);

        std::cout << "Шифрование завершено. Обработано файлов: " << encryptedFiles << "\n";
    } catch (const std::exception& ex) {
        std::cerr << "Ошибка: " << ex.what() << "\n";
        return 1;
    }
    return 0;
}
