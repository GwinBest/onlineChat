#pragma once

#include <Windows.h>		// for SetFileAttributesA

#include <fstream>
#include <string>

namespace UserData
{
	class UserCredentialsFile final
	{
	public:
		UserCredentialsFile() = default;

		static bool IsFileExists() noexcept;

		[[nodiscard]] static bool CreateNewFile() noexcept;
		static void CloseFile() noexcept;

		static void WriteCredentials(const std::string& userName, const std::string& userLogin, const size_t& userPassword) noexcept;
		static void ReadCredentials(std::string& name, std::string& login, size_t& password) noexcept;

		~UserCredentialsFile();

	private:
		static inline const char* _fileName = ".loginFile.dat";
		static inline std::fstream _credentialsFile;

		static inline bool _isFileExists = false;
	};

} // !namespace UserData