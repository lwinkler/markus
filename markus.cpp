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
    	ConfigReader conf("../config.xml");
	Manager manager(conf);
	ObjectTracker detect(conf, 640, 480, IPL_DEPTH_8U, 3);
	//Tracker track(conf, 640, 480, IPL_DEPTH_8U, 3);
	SlitCam slitcam(conf, 640, 480, IPL_DEPTH_8U, 3);
	manager.AddModule((Module&)detect);
	manager.AddModule((Module&)slitcam);
	manager.Process();

    
}

markus::~markus()
{}

#include "markus.moc"
