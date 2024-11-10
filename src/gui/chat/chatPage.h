#pragma once

#include <QWidget>

// forward declaration
namespace Ui { class ChatPage; };

namespace Gui
{
    class ChatPage final : public QWidget
    {
        Q_OBJECT

    public:
        explicit ChatPage(QWidget* parent = nullptr);
        ~ChatPage() override;

    private:
        Ui::ChatPage* ui = nullptr;
    };
} // !namespace Gui