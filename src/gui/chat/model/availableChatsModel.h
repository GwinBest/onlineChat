#pragma once

#include <cstdint>
#include <string>

#include <QAbstractListModel>
#include <QList>
#include <QString>

#include <chatSystem/chatInfo.h>

namespace Gui::Model
{
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
        explicit AvailableChatsModel(QObject* parent = nullptr) noexcept
            : QAbstractListModel(parent)
        {}

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

        [[nodiscard]] inline bool IsEmpty() const noexcept { return _availableChats.empty(); }

        void SetAllAvailableChats();
        void SetMatchingChats(const std::string& pattern);

    private:
        QList<ChatSystem::ChatInfo> _availableChats;
    };
} // !namespace Gui::Model

