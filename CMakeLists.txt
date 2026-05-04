cmake_minimum_required(VERSION 3.20)
project(FolderManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(
    src/main.cpp
    Application/FolderEncryptionService.cpp
    Infrastructure/CryptoPPManager.cpp
)

target_include_directories(FolderManager PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(cryptopp CONFIG REQUIRED)

if(TARGET cryptopp::cryptopp)
    target_link_libraries(FolderManager PRIVATE cryptopp::cryptopp)
elseif(TARGET cryptopp-static)
    target_link_libraries(FolderManager PRIVATE cryptopp-static)
elseif(TARGET cryptopp)
    target_link_libraries(FolderManager PRIVATE cryptopp)
else()
    message(FATAL_ERROR
        "Пакет cryptopp найден, но экспортируемая CMake-цель не распознана. "
        "Проверьте вывод find_package(cryptopp)."
    )
endif()