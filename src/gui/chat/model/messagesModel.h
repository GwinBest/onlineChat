#pragma once

#include <QAbstractListModel>
#include <list>
#include <mutex>

#include "coroutineUtils/coroutineUtils.h"
#include "messageBuffer/messageBuffer.h"

// forward declaration
namespace UserData
{
    class User;
}   // namespace UserData

namespace Gui::Model
{
    class MessageModel final : public QAbstractListModel
    {
    public:
        using QAbstractListModel::QAbstractListModel;

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

        CoroutineUtils::coroutine_void GetAllMessages(const int chatId, const UserData::User& user);

        void AddMessage(const MessageBuffer::MessageNode& message);

    private:
        std::list<MessageBuffer::MessageNode> _messages = {};

        static constexpr uint8_t maxMessages = UINT8_MAX;

        mutable std::mutex _messagesMutex;
    };
}   // namespace Gui::Model