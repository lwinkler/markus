#include "markus.h"

#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>

#include <QtGui/QVBoxLayout>

#include "Manager.h"

using namespace std;

markus::markus(ConfigReader & rx_configReader, Manager& rx_manager)
	: m_manager(rx_manager), m_configReader(rx_configReader)
{
	QLabel* l = new QLabel( this );
	l->setText( "Hello World!" );
	setCentralWidget( l );
	QAction* a = new QAction(this);
	a->setText( "Quit" );

	QVBoxLayout *layout = new QVBoxLayout;
	cvwidget = new QOpenCVWidget(this);
	layout->addWidget(cvwidget);
	setLayout(layout);
	resize(500, 400);

	connect(a, SIGNAL(triggered()), SLOT(close()) );
	menuBar()->addMenu( "File" )->addAction( a );
	startTimer(100);  // 0.1-second timer

}


void markus::timerEvent(QTimerEvent*)
{

	try
	{
	//ConfigReader conf("config.xml");
	//Manager manager(conf);
	//ObjectTracker objtrack(conf);
	//SlitCam slitcam(conf);
	//manager.AddModule((Module&)objtrack);
	//manager.AddModule((Module&)slitcam);
	m_manager.Process();
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
}

markus::~markus()
{}

#include "markus.moc"
