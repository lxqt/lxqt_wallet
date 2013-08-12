#include <QCoreApplication>

#include "main_application.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	main_application app ;

	app.start() ;

	return a.exec();
}
