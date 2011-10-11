#include <QtGui/QApplication>
#include "markus.h"

#include "Manager.h"


int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	ConfigReader conf("config.xml");
	Manager manager(conf);

	markus gui(conf, manager);
	gui.show();
	return app.exec();
}
