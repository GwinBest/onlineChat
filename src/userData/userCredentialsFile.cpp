#include "userCredentialsFile.h"

namespace UserData
{
	bool UserCredentialsFile::IsFileExists() noexcept
	{
		_credentialsFile.open(_fileName, std::ios::in | std::ios::out | std::ios::binary);								// if file is already exist we can reopen it 			
		if (_credentialsFile.is_open())
		{
			return true;
		}

		_credentialsFile.open(_fileName, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);			// if file is not exist we need to create it
		if (_credentialsFile.is_open())
		{
			SetFileAttributesA(_fileName, FILE_ATTRIBUTE_HIDDEN);

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

		return false;
	}

	void UserCredentialsFile::CloseFile() noexcept
	{
		if (_credentialsFile.is_open())
		{
			_credentialsFile.close();
		}
	}

	void UserCredentialsFile::WriteCredentials(const std::string& userName, const std::string& userLogin, const size_t& userPassword) noexcept
	{
		const size_t nameLength = strlen(userName.c_str());
		const size_t loginLength = strlen(userLogin.c_str());

		_credentialsFile.write(userName.c_str(), userName.size());
		_credentialsFile.write(" ", sizeof(char));

		_credentialsFile.write(userLogin.c_str(), userLogin.size());
		_credentialsFile.write(" ", sizeof(char));

		_credentialsFile.write(reinterpret_cast<char*>(&const_cast<size_t&>(userPassword)), sizeof(userPassword));
	}

	void UserCredentialsFile::ReadCredentials(std::string& userName, std::string& userLogin, size_t& userPassword) noexcept
	{
		std::getline(_credentialsFile, userName, ' ');
		std::getline(_credentialsFile, userLogin, ' ');
		_credentialsFile.read(reinterpret_cast<char*>(&userPassword), sizeof(userPassword));
	}

} // !namespace UserData
