#pragma once

#include <fstream>

#include "userData/user.h"

namespace UserData
{
    class UserCredentialsFile final
    {
    public:
        [[nodiscard]] static bool IsFileExists();

        [[nodiscard]] static bool CreateNewFile();
        static void CloseFile();

        static void WriteCredentials(const User& user);
        static User ReadCredentials();

    private:
        static constexpr const char* const _fileName = ".loginFile.dat";
        static inline std::fstream _credentialsFile;

        static inline bool _isFileExists = false;
    };
} // !namespace UserData