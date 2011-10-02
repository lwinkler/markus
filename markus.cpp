#include "markus.h"

#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>

#include "Manager.h"
#include "ObjectTracker.h"
#include "SlitCam.h"

using namespace std;

markus::markus()
{
    /*QLabel* l = new QLabel( this );
    l->setText( "Hello World!" );
    setCentralWidget( l );
    QAction* a = new QAction(this);
    a->setText( "Quit" );
    connect(a, SIGNAL(triggered()), SLOT(close()) );
    menuBar()->addMenu( "File" )->addAction( a );*/
    try
    {
    	ConfigReader conf("config.xml");
	Manager manager(conf);
	ObjectTracker objtrack(conf, 640, 480, IPL_DEPTH_8U, 3);
	SlitCam slitcam(conf, 640, 480, IPL_DEPTH_8U, 3);
	manager.AddModule((Module&)objtrack);
	manager.AddModule((Module&)slitcam);
	manager.Process();
	}
	catch(std::exception e)
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
}

markus::~markus()
{}

#include "markus.moc"
