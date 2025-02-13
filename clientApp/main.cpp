#include <QApplication>

#include "gui/mainWindow.h"
#include "userData/user.h"

UserData::User currentUser;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QApplication::setStyle("fusion");

    Gui::MainWindow window;
    window.show();

    return QApplication::exec();
}