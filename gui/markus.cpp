/*----------------------------------------------------------------------------------
*
*    MARKUS : a manager for video analysis modules
* 
*    author : Laurent Winkler <lwinkler888@gmail.com>
* 
* 
*    This file is part of Markus.
*
*    Markus is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Lesser General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Markus is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with Markus.  If not, see <http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------------------*/

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
#include <QComboBox>
#include <qevent.h>

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
	
	//m_scroll.clear();
	m_moduleViewer.clear();
	//resizeEvent()();
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
		m_moduleViewer[i]->update();
	
	//update();
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
	
	viewDisplayOptions1Act = new QAction(tr("Show display options"), this);
	connect(viewDisplayOptions1Act, SIGNAL(triggered()), this, SLOT(viewDisplayOptions1()));
	viewDisplayOptions0Act = new QAction(tr("Hide display options"), this);
	connect(viewDisplayOptions0Act, SIGNAL(triggered()), this, SLOT(viewDisplayOptions0()));
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
	viewMenu->addAction(viewDisplayOptions1Act);
	viewMenu->addAction(viewDisplayOptions0Act);
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

void markus::viewDisplayOptions1()
{
	int size = m_moduleViewer.size();
	for(int ind = 0 ; ind < size; ind++)
	{
		m_moduleViewer[ind]->toggleDisplayOptions(1);
	}
}

void markus::viewDisplayOptions0()
{
	int size = m_moduleViewer.size();
	for(int ind = 0 ; ind < size; ind++)
	{
		m_moduleViewer[ind]->toggleDisplayOptions(0);
	}
}


void markus::view1x1()
{
	nbLines = 1;
	nbCols = 1;
	resizeEvent(NULL);
}

void markus::view1x2()
{
	nbLines = 1;
	nbCols = 2;
	resizeEvent(NULL);
}

void markus::view2x2()
{
	nbLines = 2;
	nbCols = 2;
	resizeEvent(NULL);
}

void markus::view2x3()
{
	nbLines = 2;
	nbCols = 3;
	resizeEvent(NULL);
}


void markus::resizeEvent(QResizeEvent* event)
{
	int size = m_moduleViewer.size();
	for(int ind = 0 ; ind < size; ind++)
	{
		m_mainLayout.removeWidget(m_moduleViewer[ind]);
		m_mainLayout.removeWidget(m_moduleViewer[ind]);
		m_moduleViewer[ind]->hide();
	}
	
	for(int ind = size ; ind < nbLines * nbCols ; ind++)
	{
		m_moduleViewer.push_back(new QModuleViewer(&m_manager));
		m_moduleViewer[ind]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	}
	
	for (int ii = 0; ii < nbLines; ++ii) 
	{
		for (int jj = 0; jj < nbCols; ++jj) 
		{
			int ind = ii * nbCols + jj;
			m_mainLayout.addWidget(m_moduleViewer[ind], ii, jj);
			m_moduleViewer[ind]->show();
			m_moduleViewer[ind]->toggleDisplayOptions(1);
		}
	}
}


#include "markus.moc"