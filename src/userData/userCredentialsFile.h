#pragma once

#include <fstream>
#include <string>

#include "userData/user.h"

namespace UserData
{
    class UserCredentialsFile final
    {
    public:
        [[nodiscard]] static bool IsFileExists() noexcept;

        [[nodiscard]] static bool CreateNewFile() noexcept;
        static void CloseFile() noexcept;

        static void WriteCredentials(const User& user) noexcept;
        static User ReadCredentials() noexcept;

    private:
        static constexpr const char* _fileName = ".loginFile.dat";
        static inline std::fstream _credentialsFile;

        static inline bool _isFileExists = false;
    };

} // !namespace UserData