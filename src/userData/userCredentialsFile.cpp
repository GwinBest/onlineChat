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
		_credentialsFile.open(_fileName, std::ios::in | std::ios::out  | std::ios::binary);
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

		if (_credentialsFile.is_open())
		{
			_credentialsFile.close();
		}
	}

	void UserCredentialsFile::WriteCredentials(const std::string& name, const std::string& login, const std::string& password) noexcept
	{
		const size_t nameLength = strlen(name.c_str());
		const size_t loginLength = strlen(login.c_str());
		const size_t passwordLength = strlen(password.c_str());

		_credentialsFile.write(name.c_str(), name.size());
		_credentialsFile.write(" ", sizeof(char));

		_credentialsFile.write(login.c_str(), login.size());
		_credentialsFile.write(" ", sizeof(char));

		_credentialsFile.write(password.c_str(), password.size());
	}

	void UserCredentialsFile::ReadCredentials(std::string& name, std::string& login, std::string& password) noexcept
	{
		std::getline(_credentialsFile, name, ' ');
		std::getline(_credentialsFile, login, ' ');
		std::getline(_credentialsFile, password);
	}

} // !namespace UserData
