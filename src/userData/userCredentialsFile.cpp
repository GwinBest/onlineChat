#include "userCredentialsFile.h"

namespace UserData
{
	bool UserCredentialsFile::IsFileExists() noexcept
	{
		_credentialsFile.open(_fileName, std::ios::in | std::ios::binary);
		if (_credentialsFile.is_open())
		{
			return true;
		}

		return false;
	}

	bool UserCredentialsFile::CreateNewFile() noexcept
	{
		_credentialsFile.open(_fileName, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
		if (_credentialsFile.is_open())
		{
			SetFileAttributesA(_fileName, FILE_ATTRIBUTE_HIDDEN);

			return true;
		}

		return false;
	}

	void UserCredentialsFile::WriteCredentials(const std::string& name, const std::string& login, const std::string& password) noexcept
	{
		const size_t nameLength = strlen(name.c_str());
		const size_t loginLength = strlen(login.c_str());
		const size_t passwordLength = strlen(password.c_str());

		_credentialsFile.write(name.c_str(), nameLength);
		_credentialsFile.write(" ", 1);

		_credentialsFile.write(login.c_str(), loginLength);
		_credentialsFile.write(" ", 1);

		_credentialsFile.write(password.c_str(), passwordLength);


	}

	void UserCredentialsFile::CloseFile() noexcept
	{
		if (_credentialsFile.is_open())
		{
			_credentialsFile.close();
		}

	}

} // !namespace UserData
