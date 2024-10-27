#include "userCredentialsFile.h"

#include <Windows.h>		// for SetFileAttributesA

#ifdef GetUserName
#undef GetUserName
#endif // GetUserName

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
            _credentialsFile.write(user.GetUserName().c_str(), user.GetUserName().size());
            _credentialsFile.write(" ", sizeof(char));

            _credentialsFile.write(user.GetUserLogin().c_str(), user.GetUserLogin().size());
            _credentialsFile.write(" ", sizeof(char));

            _credentialsFile.write(reinterpret_cast<const char*>(user.GetUserPassword()), sizeof(user.GetUserPassword()));

            _credentialsFile.write(reinterpret_cast<const char*>(user.GetUserId()), sizeof(user.GetUserId()));
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
