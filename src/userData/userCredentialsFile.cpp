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

	void UserCredentialsFile::WriteCredentials(const std::string& name, const std::string& login, const size_t& password) noexcept
	{
		const size_t nameLength = strlen(name.c_str());
		const size_t loginLength = strlen(login.c_str());

		_credentialsFile.write(name.c_str(), name.size());
		_credentialsFile.write(" ", sizeof(char));

		_credentialsFile.write(login.c_str(), login.size());
		_credentialsFile.write(" ", sizeof(char));

		_credentialsFile.write(reinterpret_cast<char*>(&const_cast<size_t&>(password)), sizeof(password));
	}

	void UserCredentialsFile::ReadCredentials(std::string& name, std::string& login, size_t& password) noexcept
	{
		std::getline(_credentialsFile, name, ' ');
		std::getline(_credentialsFile, login, ' ');
		_credentialsFile.read(reinterpret_cast<char*>(&password), sizeof(password));
	}

} // !namespace UserData
