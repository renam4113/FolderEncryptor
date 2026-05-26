cmake_minimum_required(VERSION 3.20)
project(FolderManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(FolderManager
    FolderManager/main.cpp
    FolderManager/src/Application/Services/Implementation/FolderEncryptionService.cpp 
    FolderManager/src/Infrastructure/CryptoPP/CryptoPPManager.cpp
)

target_include_directories(FolderManager PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/FolderManager/src
    "C:/ProgramData/vcpkg/vcpkg/installed/x64-mingw-dynamic/include"
)


target_link_libraries(FolderManager PRIVATE
    "C:/ProgramData/vcpkg/vcpkg/installed/x64-mingw-dynamic/lib/libcryptopp.a"
)