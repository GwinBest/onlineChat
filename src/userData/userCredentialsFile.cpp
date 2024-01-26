#include "userCredentialsFile.h"

namespace UserData
{
	bool UserCredentialsFile::IsFileExists() noexcept
	{
		_credentialsFile.open(_fileName, std::ios::in | std::ios::out | std::ios::binary);	
		if (_credentialsFile.is_open())
		{
			return true;
		}

		return false;
	}

	bool UserCredentialsFile::CreateNewFile() noexcept
	{
		_credentialsFile.open(_fileName, std::ios::in | std::ios::out | std::ios::binary);						
		if (_credentialsFile.is_open())
		{
			SetFileAttributesA(_fileName, FILE_ATTRIBUTE_HIDDEN);

			return true;
		}

		_credentialsFile.open(_fileName, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);        // if file already exist - clear it
		if (_credentialsFile.is_open())
		{
			SetFileAttributesA(_fileName, FILE_ATTRIBUTE_HIDDEN);

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

	void UserCredentialsFile::WriteCredentials(const std::string& userName, const std::string& userLogin, const size_t userPassword) noexcept
	{
		if (_credentialsFile.is_open())
		{
			_credentialsFile.write(userName.c_str(), userName.size());
			_credentialsFile.write(" ", sizeof(char));

			_credentialsFile.write(userLogin.c_str(), userLogin.size());
			_credentialsFile.write(" ", sizeof(char));

			_credentialsFile.write(reinterpret_cast<const char*>(&userPassword), sizeof(userPassword));
		}
	}

	void UserCredentialsFile::ReadCredentials(std::string& userName, std::string& userLogin, size_t& userPassword) noexcept
	{
		if (_credentialsFile.is_open())
		{
			std::getline(_credentialsFile, userName, ' ');
			std::getline(_credentialsFile, userLogin, ' ');
			_credentialsFile.read(reinterpret_cast<char*>(&userPassword), sizeof(userPassword));
		}
	}

} // !namespace UserData
