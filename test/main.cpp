#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	MainWindow app ;

	app.start() ;

	return a.exec();
}
