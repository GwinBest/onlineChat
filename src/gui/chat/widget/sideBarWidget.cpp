#include "sideBarWidget.h"

#include "ui_sideBarWidget.h"

#include <QMouseEvent>

#include "userData/userCredentialsFile.h"

namespace Gui::Widget
{
    SideBarWidget::SideBarWidget(QWidget* parent)
        : QWidget(parent)
        , _ui(new Ui::SideBarWidget())
    {
        _ui->setupUi(this);
        _ui->sideBarFrame->setFixedWidth(sideBarWidth);
        _ui->transparentFrame->setMouseTracking(true);
        _ui->transparentFrame->installEventFilter(this);

        connect(_ui->logOutButton, &QPushButton::pressed, this, &SideBarWidget::OnLogOutButtonPressed);
    }

    SideBarWidget::~SideBarWidget()
    {
        delete _ui;
    }

    void SideBarWidget::UpdateUserName(const std::string_view name) const noexcept
    {
        _ui->userName->setText(name.data());
    }

    void SideBarWidget::OnLogOutButtonPressed() const noexcept
    {
        UserData::UserCredentialsFile::RemoveFile();

        emit LogOutButtonPressed();
    }

    bool SideBarWidget::eventFilter(QObject* obj, QEvent* event)
    {
        if (obj == _ui->transparentFrame)
        {
            if (event->type() == QEvent::MouseButtonPress) emit HideSideBar();
        }

        return QWidget::eventFilter(obj, event);
    }
} //!namespace Gui::Widget