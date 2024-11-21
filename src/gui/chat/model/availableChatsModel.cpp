#include "availableChatsModel.h"

#include "userData/userRepository.h"

extern UserData::User currentUser;

namespace Gui::Model
{
    int AvailableChatsModel::rowCount(const QModelIndex& parent) const
    {
        if (parent.isValid()) return 0;
        return _availableChats.size();
    }

    QVariant AvailableChatsModel::data(const QModelIndex& index, const int role) const
    {
        if (!index.isValid() || index.row() < 0 || index.row() >= _availableChats.size())
            return QVariant();

        const ChatSystem::ChatInfo& chatInfo = _availableChats.at(index.row());

        switch (role)
        {
        case kChatIdRole:               return chatInfo.id;
        case kChatNameRole:             return chatInfo.name;
        case kLastMessageRole:          return chatInfo.lastMessage;
        case kLastMessageSendTimeRole:  return chatInfo.lastMessageSendTime;
        default:                        return QVariant();
        }
    }

    void AvailableChatsModel::SetAllAvailableChats() noexcept
    {
        beginResetModel();

        _availableChats.clear();

        std::optional<std::vector<ChatSystem::ChatInfo>> chats =
            UserData::UserRepository::GetAvailableChatsForUser(currentUser.GetUserId());

        if (!chats.has_value()) return;

        for (const auto& item : chats.value())
        {
            _availableChats.append(item);
        }

        endResetModel();
    }

    void AvailableChatsModel::SetMatchingChats(const std::string& pattern) noexcept
    {
        beginResetModel();

        _availableChats.clear();

        std::optional<std::vector<ChatSystem::ChatInfo>> chats =
            UserData::UserRepository::FindMatchingChats(currentUser.GetUserId(), pattern);

        if (!chats.has_value()) return;

        for (const auto& item : chats.value())
        {
            _availableChats.append(item);
        }

        endResetModel();
    }
} // !namespace Gui::Model