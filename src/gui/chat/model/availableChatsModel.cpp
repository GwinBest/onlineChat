#include "availableChatsModel.h"

#include <algorithm>

#include "userData/userRepository.h"

extern UserData::User currentUser;

namespace Gui::Model
{
    using namespace CoroutineUtils;

    int AvailableChatsModel::rowCount(const QModelIndex& parent) const
    {
        if (parent.isValid()) return 0;
        return static_cast<int>(_availableChats.size());
    }

    QVariant AvailableChatsModel::data(const QModelIndex& index, const int role) const
    {
        if (!index.isValid() || index.row() < 0 || index.row() >= _availableChats.size()) return {};

        const ChatSystem::ChatInfo& chatInfo = _availableChats.at(index.row());

        switch (role)
        {
        case kChatIdRole:              return chatInfo.id;
        case kChatNameRole:            return chatInfo.name;
        case kLastMessageRole:         return chatInfo.lastMessage;
        case kLastMessageSendTimeRole: return chatInfo.lastMessageSendTime;
        default:                       return {};
        }
    }

    bool AvailableChatsModel::setData(const QModelIndex& index, const QVariant& value, int role)
    {
        if (!index.isValid() || index.row() < 0 || index.row() >= _availableChats.size())
        {
            return false;
        }

        ChatSystem::ChatInfo& chatInfo = _availableChats[index.row()];

        switch (role)
        {
        case kChatIdRole:              chatInfo.id = value.toInt(); return true;
        case kChatNameRole:            chatInfo.name = value.toString(); return true;
        case kLastMessageRole:         chatInfo.lastMessage = value.toString(); return true;
        case kLastMessageSendTimeRole: chatInfo.lastMessageSendTime = value.toString(); return true;
        default:                       return false;
        }
    }

    coroutine_void AvailableChatsModel::SetAllAvailableChats()
    {
        beginResetModel();

        _availableChats.clear();

        const std::optional<std::vector<ChatSystem::ChatInfo>>
            chats = co_await UserData::UserRepository::GetAvailableChatsForUserAsync(
                currentUser.GetUserId());

        if (!chats.has_value()) co_return;

        for (const auto& item : chats.value())
        {
            _availableChats.append(item);
        }

        endResetModel();
    }

    coroutine_void AvailableChatsModel::SetMatchingChats(const std::string& pattern)
    {
        beginResetModel();

        _availableChats.clear();

        const std::optional<std::vector<ChatSystem::ChatInfo>>
            chats = co_await UserData::UserRepository::FindMatchingChatsAsync(
                currentUser.GetUserId(),
                pattern);

        if (!chats.has_value()) co_return;

        for (const auto& item : chats.value())
        {
            _availableChats.append(item);
        }

        endResetModel();
    }

    bool AvailableChatsModel::IsChatInModel(const size_t chatId)
    {
        auto it = std::find_if(
            _availableChats.begin(),
            _availableChats.end(),
            [chatId](const ChatSystem::ChatInfo& chat) { return chat.id == chatId; });
        return it != _availableChats.end();
    }

    void AvailableChatsModel::AddChatById(const ChatSystem::ChatInfo&& chatInfo)
    {
        const int row = _availableChats.size();

        beginInsertRows(QModelIndex(), row, row);

        _availableChats.push_back(std::move(chatInfo));

        endInsertRows();
    }
}   // namespace Gui::Model