#pragma once

#include <QWidget>

// forward declaration
namespace Ui { class ChatPage; }
namespace Gui::Model { class AvailableChatsModel; }

namespace Gui
{
    class ChatPage final : public QWidget
    {
        Q_OBJECT

    public:
        explicit ChatPage(QWidget* parent = nullptr);
        ~ChatPage() override;

        void PreparePage() noexcept;

    private slots:
        void OnSearchInputTextChanged() const noexcept;

    private:
        Ui::ChatPage* _ui = nullptr;

        Model::AvailableChatsModel* _model = nullptr;
    };
} // !namespace Gui