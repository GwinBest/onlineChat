#include "messagesModel.h"

#include <QDateTime>
#include <QLocale>

#include "userData/user.h"
#include "userData/userRepository.h"

Q_DECLARE_METATYPE(MessageBuffer::MessageNode)

namespace Gui::Model
{
    using namespace CoroutineUtils;

    int MessageModel::rowCount(const QModelIndex&) const
    {
        return static_cast<int>(_messages.size());
    }

    QVariant MessageModel::data(const QModelIndex& index, int role) const
    {
        if (!index.isValid() || _messages.empty()) return {};

        int row = index.row();
        std::lock_guard<std::mutex> lock(_messagesMutex);
        if (row < 0 || row >= static_cast<int>(_messages.size())) return {};

        if (role == Qt::DisplayRole)
        {
            auto it = std::next(_messages.begin(), row);
            if (it == _messages.end()) return {};
            return QVariant::fromValue(*it);
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

        std::lock_guard<std::mutex> lock(_messagesMutex);

        _messages.clear();

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