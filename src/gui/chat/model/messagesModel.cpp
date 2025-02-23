#include "messagesModel.h"

#include <QDateTime>
#include <QLocale>

#include "userData/user.h"
#include "userData/userRepository.h"

Q_DECLARE_METATYPE(MessageBuffer::MessageNode)

namespace Gui::Model
{
    using namespace CoroutineUtils;

    int MessageModel::rowCount(const QModelIndex& parent) const
    {
        return _messages.size();
    }

    QVariant MessageModel::data(const QModelIndex& index, int role) const
    {
        if (!index.isValid() || index.row() >= _messages.size()) return {};

        if (role == Qt::DisplayRole)
        {
            auto it = _messages.begin();
            std::advance(it, index.row());

            if (it != _messages.end()) return QVariant::fromValue(*it);
        }

        return {};
    }

    coroutine_void MessageModel::GetAllMessages(const int chatId, const UserData::User& user)
    {
        if (chatId == 0) co_return;

        const std::optional<std::vector<MessageBuffer::MessageNode>>
            chatMessage = co_await UserData::UserRepository::GetAvailableChatMessagesAsync(
                user.GetUserId(),
                chatId);

        if (!chatMessage.has_value()) co_return;

        beginResetModel();

        for (const auto& message : chatMessage.value()) _messages.push_back(message);

        endResetModel();
    }

    void MessageModel::AddMessage(const MessageBuffer::MessageNode& message)
    {
        const int row = _messages.size();

        beginInsertRows(QModelIndex(), row, row);

        _messages.push_back(message);

        endInsertRows();
    }
}   // namespace Gui::Model