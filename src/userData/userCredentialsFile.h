#pragma once

// for FILE_ATTRIBUTE_HIDDEN
#include <Windows.h>

#include <fstream>


namespace UserData
{
	static class UserCredentialsFile final
	{
	public:
		UserCredentialsFile() = default;
		static bool IsFileExists() noexcept;

		[[nodiscard]] static bool CreateNewFile() noexcept;
		static void WriteCredentials(const std::string& name, const std::string& login, const std::string& password) noexcept;
		static void CloseFile() noexcept;

	private:
		static inline const char* _fileName = ".loginFile.dat";
		static inline std::fstream _credentialsFile;

		static inline bool _isFileExists = false;
	};

} // !namespace UserData