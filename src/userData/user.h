#pragma once

#include <string>

namespace UserData
{
    class User final
    {
    public:
        [[nodiscard]] const size_t GetUserId() const noexcept { return _userId; }
        [[nodiscard]] const std::string& GetUserName() const noexcept { return _userName; }
        [[nodiscard]] const std::string& GetUserLogin() const noexcept { return _userLogin; }
        [[nodiscard]] const size_t GetUserPassword() const noexcept { return _userPassword; }

        void SetUserId(const size_t userId) noexcept { _userId = userId; }
        void SetUserName(std::string_view userName) noexcept { _userName = userName; }
        void SetUserLogin(std::string_view userLogin) noexcept { _userLogin = userLogin; }
        void SetUserPassword(const size_t userPassword) noexcept { _userPassword = userPassword; }

    private:
        size_t _userId = 0;
        std::string _userName;
        std::string _userLogin;
        size_t _userPassword = 0;
    };

} // !namespace UserData
