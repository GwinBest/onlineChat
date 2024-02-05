#pragma once

#include <fstream>
#include <string>

namespace UserData
{
	class UserCredentialsFile final
	{
	public:
		UserCredentialsFile() = delete;
		UserCredentialsFile(const UserCredentialsFile&) = delete;
		UserCredentialsFile& operator=(const UserCredentialsFile&) = delete;
		UserCredentialsFile(const UserCredentialsFile&&) = delete;
		UserCredentialsFile& operator=(const UserCredentialsFile&&) = delete;
		~UserCredentialsFile() = delete;

		static bool IsFileExists() noexcept;

		[[nodiscard]] static bool CreateNewFile() noexcept;
		static void CloseFile() noexcept;

		static void WriteCredentials(const std::string& userName, const std::string& userLogin, const size_t userPassword, const size_t userId) noexcept;
		static void ReadCredentials(std::string& userName, std::string& userLogin, size_t& userPassword, size_t& usedId) noexcept;

	private:
		static inline const char* _fileName = ".loginFile.dat";
		static inline std::fstream _credentialsFile;

		static inline bool _isFileExists = false;
	};

} // !namespace UserData