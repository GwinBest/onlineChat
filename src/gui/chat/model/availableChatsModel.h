#pragma once

#include <QAbstractListModel>
#include <QKeyEvent>
#include <QList>
#include <QString>
#include <cstdint>
#include <string>

#include "chatSystem/chatInfo.h"
#include "coroutineUtils/coroutineUtils.h"

namespace Gui::Model
{
    bool IsChatInModel(const size_t chatId);

    class AvailableChatsModel final : public QAbstractListModel
    {
    public:
        enum AvailableChatsRole : uint16_t
        {
            kChatIdRole = Qt::UserRole + 1,
            kChatNameRole,
            kLastMessageRole,
            kLastMessageSendTimeRole,
        };

    public:
        using QAbstractListModel::QAbstractListModel;

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        bool setData(const QModelIndex& index,
                     const QVariant& value,
                     int role = Qt::EditRole) override;

        [[nodiscard]] inline bool IsEmpty() const noexcept { return _availableChats.empty(); }

        CoroutineUtils::coroutine_void SetAllAvailableChats();
        CoroutineUtils::coroutine_void SetMatchingChats(const std::string& pattern);

        bool IsChatInModel(const size_t chatId);

        void AddChatById(const ChatSystem::ChatInfo&& chatInfo);

    private:
        QList<ChatSystem::ChatInfo> _availableChats;
    };
}   // namespace Gui::Model