#include "mainwindow.h"

#include <QApplication>
#include <QMetaType>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	a.setApplicationName("AudioDatenbank");
	a.setOrganizationName("Leonhard Suckau");
	a.setOrganizationDomain("AudioDatenbank");

	MainWindow w;
    w.show();

    return a.exec();
}
