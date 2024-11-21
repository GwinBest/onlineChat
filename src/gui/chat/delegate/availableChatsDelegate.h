#pragma once

#include <QStyledItemDelegate>

namespace Gui::Delegate
{
    class AvailableChatsDelegate final : public QStyledItemDelegate
    {
    public:
        explicit AvailableChatsDelegate(QObject* parent = nullptr)
            : QStyledItemDelegate(parent) {}

        void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    };
} // !namespace Gui::Delegate