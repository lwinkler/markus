#include "markus.h"

#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QAction>

//#include <QVBoxLayout>
//#include <QGridLayout>
#include <QTimer>
//#include <QGroupBox>
#include <QMessageBox>
//#include <QComboBox>

#include "Manager.h"
#include "Module.h"

using namespace std;

markus::markus(ConfigReader & rx_configReader, Manager& rx_manager)
	: m_manager(rx_manager), m_configReader(rx_configReader)
{
	nbLines = 1;
	nbCols = 1;

	startTimer(10);  // 100 -> 0.1-second timer

	//QTimer *timer = new QTimer(this);
	//QWidget* mainWidget = new QWidget;
	//QGridLayout* mainLayout = new QGridLayout;
	
	setWindowState(Qt::WindowMaximized);
	
	m_scroll.clear();
	m_moduleViewer.clear();
	updateModuleViewers();
	m_mainWidget.setLayout(&m_mainLayout);
	setCentralWidget(&m_mainWidget);

	
	//mainWidget->setLayout(mainLayout);

	//setCentralWidget(mainWidget);
	
	//layout()->addWidget(quit);
	//	layout->addWidget(quit);//, 0 , 1 );
	//timer->start(100);
	createActions();
	createMenus();

	setWindowTitle(tr("Markus"));
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
	for(int i = 0 ; i < nbCols * nbLines ; i++)
		m_moduleViewer[i]->putImage();
}

markus::~markus()
{}


QLabel *markus::createLabel(const QString &text)
{
	QLabel *label = new QLabel(text);
	label->setAlignment(Qt::AlignCenter);
	label->setMargin(2);
	label->setFrameStyle(QFrame::Box | QFrame::Sunken);
	return label;
}

void markus::about()
{
	QMessageBox::about(this, tr("About Image Viewer"),
				    tr("<p>The <b>Markus</b> image processing environment lets "
				    "the user experiment with the different elements of computer "
				    "vision.</p> "
				    "<p><b>Author : Laurent Winkler</b></p>"));
}

void markus::createActions()
{
	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
	
	aboutAct = new QAction(tr("&About"), this);
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
	
	view1x1Act = new QAction(tr("View 1x1"), this);
	connect(view1x1Act, SIGNAL(triggered()), this, SLOT(view1x1()));
	view1x2Act = new QAction(tr("View 2x1"), this);
	connect(view1x2Act, SIGNAL(triggered()), this, SLOT(view1x2()));
	view2x2Act = new QAction(tr("View 2x2"), this);
	connect(view2x2Act, SIGNAL(triggered()), this, SLOT(view2x2()));
	view2x3Act = new QAction(tr("View 2x3"), this);
	connect(view2x3Act, SIGNAL(triggered()), this, SLOT(view2x3()));
}

void markus::createMenus()
{
	fileMenu = new QMenu(tr("&File"), this);
	//fileMenu->addAction(cleanAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);
	
	
	viewMenu = new QMenu(tr("&View"), this);
	viewMenu->addAction(view1x1Act);
	viewMenu->addAction(view1x2Act);
	viewMenu->addAction(view2x2Act);
	viewMenu->addAction(view2x3Act);
	
	helpMenu = new QMenu(tr("&Help"), this);
	helpMenu->addAction(aboutAct);
	
	menuBar()->addMenu(fileMenu);
	menuBar()->addMenu(viewMenu);
	menuBar()->addMenu(helpMenu);
}

/*void Window::updateActions()
{
	zoomInAct->setEnabled(!fitToWindowAct->isChecked());
	zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
	normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}*/

void markus::view1x1()
{
	nbLines = 1;
	nbCols = 1;
	updateModuleViewers();
}

void markus::view1x2()
{
	nbLines = 1;
	nbCols = 2;
	updateModuleViewers();
}

void markus::view2x2()
{
	nbLines = 2;
	nbCols = 2;
	updateModuleViewers();
}

void markus::view2x3()
{
	nbLines = 2;
	nbCols = 3;
	updateModuleViewers();
}

void markus::updateModuleViewers()
{
	int size = m_moduleViewer.size();
	for(int ind = 0 ; ind < size; ind++)
	{
		m_mainLayout.removeWidget(m_moduleViewer[ind]);
		m_mainLayout.removeWidget(m_moduleViewer[ind]);
	}
	
	for(int ind = size ; ind < nbLines * nbCols ; ind++)
	{
		m_scroll.push_back(new QScrollArea);
		m_moduleViewer.push_back(new QOpenCVWidget(&m_manager));
		
		m_scroll[ind]->setWidget(m_moduleViewer[ind]);
		//imageViewer[i]->setGeometry(i * width()/2, 0, (i + 1) * width()/2, height()/2);
		//m_scroll[i]->setBaseSize(width()/2, width()/2);
		m_scroll[ind]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		//m_moduleViewer[i]->resize(1000, 1000);
		
		//imageViewer[i]->setBaseSize(100, 100);
		//m_scroll[i]->setBaseSize(200, 400);
		m_moduleViewer[ind]->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	}
	//m_mainLayout.deleteAllItems();
	//.deleteAllItems();
	
	for (int ii = 0; ii < nbLines; ++ii) 
	{
		for (int jj = 0; jj < nbCols; ++jj) 
		{
			int ind = ii * nbCols + jj;
			/*m_scroll[ind]->setWidget(m_moduleViewer[ind]);
			//imageViewer[i]->setGeometry(i * width()/2, 0, (i + 1) * width()/2, height()/2);
			//m_scroll[i]->setBaseSize(width()/2, width()/2);
			m_scroll[ind]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			//m_moduleViewer[i]->resize(1000, 1000);
			
			//imageViewer[i]->setBaseSize(100, 100);
			//m_scroll[i]->setBaseSize(200, 400);
			m_moduleViewer[ind]->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);*/
			m_mainLayout.addWidget(m_scroll[ind], ii, jj);
			
			//imageViewer[i]->setAntialiased(j != 0);
			//imageViewer[i]->setFloatBased(i != 0);
			
			//QCheckBox* cbDispImage=new QCheckBox(tr("Display image"));
			//connect(cbDispImage, SIGNAL(toggled(bool)),imageViewer[i], SLOT(setDispImage(bool)));
			//cbDispImage->setChecked(true);
			//mainLayout->addWidget(cbDispImage, 2, i);
			//connect(imageViewer[i], SIGNAL(dispImageSetted(bool)),cbDispImage, SLOT(setChecked(bool)));
			
			//QCheckBox* cbDispKP=new QCheckBox(tr("Display key points"));
			//connect(cbDispKP, SIGNAL(toggled(bool)),imageViewer[i], SLOT(setDispKP(bool)));
			//cbDispKP->setChecked(true);

			//QCheckBox* cbDispMatch=new QCheckBox(tr("Display matches"));
			//connect(cbDispMatch, SIGNAL(toggled(bool)),imageViewer[i], SLOT(setDispMatch(bool)));
			//cbDispMatch->setChecked(true);
			
			//m_mainLayout.addWidget(gbSettings, 2 * ii + 1, jj);
			
			//connect(timer, SIGNAL(timeout()),imageViewer[i], SLOT(update()));
			//QPushButton *quit = new QPushButton(tr("&Quit"));
			//mainLayout->addWidget(quit, 1, i);
		}
	}
}


#include "markus.moc"
