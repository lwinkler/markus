#include <QtGui/QApplication>
#include "markus.h"

#include "Manager.h"

#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
	try
	{
		QApplication app(argc, argv);
		ConfigReader conf("config.xml");
		Manager manager(conf);

		markus gui(conf, manager);
		gui.setWindowTitle("OpenCV --> QtImage");
		gui.show();
		return app.exec();
	}
	catch(cv::Exception& e)
	{
		cout << "Exception raised (std::exception) : " << e.what() <<endl;
	}
	catch(std::exception& e)
	{
		cout << "Exception raised (std::exception) : " << e.what() <<endl;
	}
	catch(std::string str)
	{
		cout << "Exception raised (string) : " << str <<endl;
	}
	catch(const char* str)
	{
		cout << "Exception raised (const char*) : " << str <<endl;
	}
	catch(...)
	{
		cout << "Unknown exception raised: "<<endl;
	}
	return -1;
}
