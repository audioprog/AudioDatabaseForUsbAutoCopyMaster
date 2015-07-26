#include "mainwindow.h"

#include "lsVariantList2D.h"

#include <QApplication>
#include <QMetaType>

int main(int argc, char *argv[])
{
	qRegisterMetaType<lsVariantList2D>(QT_STRINGIFY(lsVariantList2D));
    QApplication a(argc, argv);

	a.setApplicationName("AudioDatenbank");
	a.setOrganizationName("Leonhard Suckau");
	a.setOrganizationDomain("AudioDatenbank");

	MainWindow w;
    w.show();

    return a.exec();
}
