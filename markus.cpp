#include "markus.h"

#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>

#include <QtGui/QVBoxLayout>
#include <QTimer>
#include <QGroupBox>
#include <QMessageBox>
#include <QComboBox>

#include "Manager.h"
#include "Module.h"

using namespace std;

markus::markus(ConfigReader & rx_configReader, Manager& rx_manager)
	: m_manager(rx_manager), m_configReader(rx_configReader)
{
	//QLabel* l = new QLabel( this );
	//l->setText( "Hello World!" );
	//setCentralWidget( l );

	//QVBoxLayout *layout = new QVBoxLayout;
	//layout->addWidget(&cvwidget);
	//layout->addWidget(l);
//	setCentralWidget(&cvwidget);
	
//	setCentralWidget( cvwidget );
//	resize(500, 400);

	startTimer(10);  // 100 -> 0.1-second timer

	
	// from hlfe
		
	//QTimer *timer = new QTimer(this);
	QWidget* mainWidget = new QWidget;
	QGridLayout* mainLayout = new QGridLayout;
	
	setWindowState(Qt::WindowMaximized);
	
	for (int i = 0; i < nbViewer; ++i) {
		scroll[i] = new QScrollArea;
		moduleViewer[i] = new QOpenCVWidget(&m_manager);
		scroll[i]->setWidget(moduleViewer[i]);
		//imageViewer[i]->setGeometry(i * width()/2, 0, (i + 1) * width()/2, height()/2);
		//scroll[i]->setBaseSize(width()/2, width()/2);
		scroll[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		//moduleViewer[i]->resize(1000, 1000);
		
		//imageViewer[i]->setBaseSize(100, 100);
		//scroll[i]->setBaseSize(200, 400);
		moduleViewer[i]->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		mainLayout->addWidget(scroll[i], 1, i);
		
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

		QGroupBox *groupBox = new QGroupBox(tr("Display options"));
		groupBox->setFlat(true);
		QGridLayout *vbox = new QGridLayout;
		//vbox->addStretch(1);
		
		QLabel* lab1 = new QLabel(tr("Module"));
		vbox->addWidget(lab1,0,0);
		QComboBox * combo1 = new QComboBox();
		for(std::list<Module*>::const_iterator it = m_manager.GetModuleList().begin(); it != m_manager.GetModuleList().end(); it++)
			combo1->addItem(QString((*it)->GetName().c_str()), 1);
		vbox->addWidget(combo1,0,1);
		
		QLabel* lab2 = new QLabel(tr("Out stream"));
		vbox->addWidget(lab2,1,0);
		QComboBox * combo2 = new QComboBox();
		//for(std::list<OutputStream>::const_iterator it = m_currentModule.begin(); it != m_manager.GetModuleList().end(); it++)
		//	combo1->addItem(QString((*it)->GetName().c_str()), 1);
		vbox->addWidget(combo1,0,1);
		
		groupBox->setLayout(vbox);
		
		mainLayout->addWidget(groupBox, 2, i);
		//connect(timer, SIGNAL(timeout()),imageViewer[i], SLOT(update()));
		//QPushButton *quit = new QPushButton(tr("&Quit"));
		//mainLayout->addWidget(quit, 1, i);
	}
	mainWidget->setLayout(mainLayout);

	setCentralWidget(mainWidget);
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
	for(int i = 0 ; i < nbViewer ; i++)
		moduleViewer[i]->putImage();
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

#include "markus.moc"
