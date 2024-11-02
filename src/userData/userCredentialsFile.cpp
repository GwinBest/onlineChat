#include "userCredentialsFile.h"

#ifdef WIN32
#include <Windows.h>		// for SetFileAttributesA

#ifdef GetUserName
#undef GetUserName
#endif // GetUserName

#endif // WIN32

namespace UserData
{
    bool UserCredentialsFile::IsFileExists() noexcept
    {
        _credentialsFile.open(_fileName, std::ios::in | std::ios::out | std::ios::binary);
        return _credentialsFile.is_open();
    }

    bool UserCredentialsFile::CreateNewFile() noexcept
    {
        _credentialsFile.open(_fileName, std::ios::in | std::ios::out | std::ios::binary);
        if (_credentialsFile.is_open())
        {
#ifdef WIN32
            SetFileAttributesA(_fileName, FILE_ATTRIBUTE_HIDDEN);
#endif
            return true;
        }

        _credentialsFile.open(_fileName, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);        // if file already exist - clear it
        if (_credentialsFile.is_open())
        {
#ifdef WIN32
            SetFileAttributesA(_fileName, FILE_ATTRIBUTE_HIDDEN);
#endif
            return true;
        }

        return false;
    }

    void UserCredentialsFile::CloseFile() noexcept
    {
        if (_credentialsFile.is_open())
        {
            _credentialsFile.close();
        }
    }

    void UserCredentialsFile::WriteCredentials(const User& user) noexcept
    {
        if (_credentialsFile.is_open())
        {
            const std::string userName = user.GetUserName();
            _credentialsFile.write(userName.c_str(), userName.size());
            _credentialsFile.write(" ", sizeof(char));

            const std::string userLogin = user.GetUserLogin();
            _credentialsFile.write(userLogin.c_str(), userLogin.size());
            _credentialsFile.write(" ", sizeof(char));

            const size_t userPassword = user.GetUserPassword();
            _credentialsFile.write(reinterpret_cast<const char*>(&userPassword), sizeof(userPassword));

            const size_t userId = user.GetUserId();
            _credentialsFile.write(reinterpret_cast<const char*>(&userId), sizeof(userId));
        }
    }

    User UserCredentialsFile::ReadCredentials() noexcept
    {
        User user;

        if (_credentialsFile.is_open())
        {
            std::string userName;
            std::string userLogin;
            size_t userPassword = 0;
            size_t userId = 0;

            std::getline(_credentialsFile, userName, ' ');
            std::getline(_credentialsFile, userLogin, ' ');
            _credentialsFile.read(reinterpret_cast<char*>(&userPassword), sizeof(userPassword));
            _credentialsFile.read(reinterpret_cast<char*>(&userId), sizeof(userId));

            user.SetUserId(userId);
            user.SetUserName(userName);
            user.SetUserLogin(userLogin);
            user.SetUserPassword(userPassword);
        }

        return user;
    }

} // !namespace UserData
