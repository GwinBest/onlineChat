#pragma once

#include <filesystem>
#include <fstream>

#include "userData/user.h"

namespace UserData
{
    class UserCredentialsFile final
    {
    public:
        [[nodiscard]] static bool IsFileExists();
        [[nodiscard]] static bool CreateNewFile();
        static void RemoveFile();
        static void CloseFile();

        static void WriteCredentials(const User& user);
        [[nodiscard]] static User ReadCredentials();

    private:
        static inline std::filesystem::path _fileName = ".loginFile.dat";
        static inline std::fstream _credentialsFile;

        static inline bool _isFileExists = false;
    };
}   // namespace UserData