#include <QApplication>
#include <QMetaType>

#include "gui/mainWindow.h"
#include "messageBuffer/messageBuffer.h"
#include "userData/user.h"

UserData::User currentUser;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QApplication::setStyle("fusion");

    Gui::MainWindow window;
    window.show();

    qRegisterMetaType<MessageBuffer::MessageNode>("MessageBuffer::MessageNode");

    return QApplication::exec();
}