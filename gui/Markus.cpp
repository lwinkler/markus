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

#include "Markus.h"

#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QAction>

#include <QTimer>
#include <QMessageBox>
#include <QComboBox>
#include <qevent.h>

#include "Manager.h"
#include "Module.h"
#include "ModuleTimer.h"


using namespace std;

//---------------------------------------------------------------------------------------------------- 
// Main constructor
//---------------------------------------------------------------------------------------------------- 

Markus::Markus(ConfigReader & rx_configReader, Manager& rx_manager)
	: Configurable(rx_configReader),
 	m_param(rx_configReader),
	m_manager(rx_manager)
{
	// Call to manager process each 10 ms
	//if(!m_centralized)
	startTimer(static_cast<int>(MARKUS_TIMER_S * 1000));  // 10 -> 0.01-second timer

	setWindowState(Qt::WindowMaximized);
	
	//m_scroll.clear();
	m_moduleViewer.clear();
	//resizeEvent()();
	m_mainWidget.setLayout(&m_mainLayout);
	setCentralWidget(&m_mainWidget);

	createActions();
	createMenus();
	resizeEvent(NULL);

	setWindowTitle(tr("Markus"));
}


void Markus::timerEvent(QTimerEvent*)
{
	/*if(m_centralized)
	{
		// at each increment, call the general Process method
		m_manager.Process();
	}*/

	for(int i = 0 ; i < m_param.nbCols * m_param.nbRows ; i++)
		m_moduleViewer[i]->update();
	
	//update();
}

Markus::~Markus()
{
}

void Markus::UpdateConfig()
{
	for(vector<QModuleViewer *>::iterator it = m_moduleViewer.begin() ; it != m_moduleViewer.end() ; it++)
		(*it)->UpdateConfig();
	Configurable::UpdateConfig();
}

QLabel *Markus::createLabel(const QString &text)
{
	QLabel *label = new QLabel(text);
	label->setAlignment(Qt::AlignCenter);
	label->setMargin(2);
	label->setFrameStyle(QFrame::Box | QFrame::Sunken);
	return label;
}


//---------------------------------------------------------------------------------------------------- 
// Print about text
//---------------------------------------------------------------------------------------------------- 
void Markus::about()
{
	QMessageBox::about(this, tr("About Image Viewer"),
				    tr("<p>The <b>Markus</b> image processing environment lets "
				    "the user experiment with the different elements of computer "
				    "vision.</p> "
				    "<p><b>Author : Laurent Winkler</b></p>"));
}



//---------------------------------------------------------------------------------------------------- 
// Create actions for menus
//---------------------------------------------------------------------------------------------------- 
void Markus::createActions()
{
	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
	
	aboutAct = new QAction(tr("&About"), this);
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
	
	viewDisplayOptionsAct = new QAction(tr("Show display options"), this);
	viewDisplayOptionsAct->setCheckable(true);
	connect(viewDisplayOptionsAct, SIGNAL(triggered(bool)), this, SLOT(viewDisplayOptions(bool)));
	view1x1Act = new QAction(tr("View 1x1"), this);
	connect(view1x1Act, SIGNAL(triggered()), this, SLOT(view1x1()));
	view1x2Act = new QAction(tr("View 2x1"), this);
	connect(view1x2Act, SIGNAL(triggered()), this, SLOT(view1x2()));
	view2x2Act = new QAction(tr("View 2x2"), this);
	connect(view2x2Act, SIGNAL(triggered()), this, SLOT(view2x2()));
	view2x3Act = new QAction(tr("View 2x3"), this);
	connect(view2x3Act, SIGNAL(triggered()), this, SLOT(view2x3()));
	view3x3Act = new QAction(tr("View 3x3"), this);
	connect(view3x3Act, SIGNAL(triggered()), this, SLOT(view3x3()));
	view3x4Act = new QAction(tr("View 3x4"), this);
	connect(view3x4Act, SIGNAL(triggered()), this, SLOT(view3x4()));
	view4x4Act = new QAction(tr("View 4x4"), this);
	connect(view4x4Act, SIGNAL(triggered()), this, SLOT(view4x4()));
}

//---------------------------------------------------------------------------------------------------- 
// Create the menus themselves
//---------------------------------------------------------------------------------------------------- 
void Markus::createMenus()
{
	fileMenu = new QMenu(tr("&File"), this);
	//fileMenu->addAction(cleanAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);
	
	
	viewMenu = new QMenu(tr("&View"), this);
	viewMenu->addAction(viewDisplayOptionsAct);
	viewMenu->addAction(view1x1Act);
	viewMenu->addAction(view1x2Act);
	viewMenu->addAction(view2x2Act);
	viewMenu->addAction(view2x3Act);
	viewMenu->addAction(view3x3Act);
	viewMenu->addAction(view3x4Act);
	viewMenu->addAction(view4x4Act);
	
	helpMenu = new QMenu(tr("&Help"), this);
	helpMenu->addAction(aboutAct);
	
	menuBar()->addMenu(fileMenu);
	menuBar()->addMenu(viewMenu);
	menuBar()->addMenu(helpMenu);
}

void Markus::viewDisplayOptions(bool x_isChecked)
{
	int size = m_moduleViewer.size();
	for(int ind = 0 ; ind < size; ind++)
	{
		m_moduleViewer[ind]->showDisplayOptions(x_isChecked);
	}
}

void Markus::view1x1()
{
	m_param.nbRows = 1;
	m_param.nbCols = 1;
	resizeEvent(NULL);
}

void Markus::view1x2()
{
	m_param.nbRows = 1;
	m_param.nbCols = 2;
	resizeEvent(NULL);
}

void Markus::view2x2()
{
	m_param.nbRows = 2;
	m_param.nbCols = 2;
	resizeEvent(NULL);
}

void Markus::view2x3()
{
	m_param.nbRows = 2;
	m_param.nbCols = 3;
	resizeEvent(NULL);
}

void Markus::view3x3()
{
	m_param.nbRows = 3;
	m_param.nbCols = 3;
	resizeEvent(NULL);
}

void Markus::view3x4()
{
	m_param.nbRows = 3;
	m_param.nbCols = 4;
	resizeEvent(NULL);
}

void Markus::view4x4()
{
	m_param.nbRows = 4;
	m_param.nbCols = 4;
	resizeEvent(NULL);
}


void Markus::resizeEvent(QResizeEvent* event)
{
	// hide all modules
	int size = m_moduleViewer.size();
	for(int ind = 0 ; ind < size; ind++)
	{
		m_mainLayout.removeWidget(m_moduleViewer[ind]);
		m_mainLayout.removeWidget(m_moduleViewer[ind]);
		m_moduleViewer[ind]->hide();
	}
	
	// Add new module viewers
	for(int ind = size ; ind < m_param.nbRows * m_param.nbCols ; ind++)
	{
		stringstream ss;
		ss<<"viewer"<<ind;
		ConfigReader conf = m_configReader.RefSubConfig("viewer", ss.str(), true);
		conf.RefSubConfig("parameters", "", true);
		m_moduleViewer.push_back(new QModuleViewer(&m_manager, conf));
		m_moduleViewer[ind]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		// m_moduleViewer[ind]->showDisplayOptions(true);
	}
	
	// Remove extra modules
	for(int ind = m_param.nbRows * m_param.nbCols ; ind < size ; ind++)
	{
		delete(m_moduleViewer[ind]);
	}
	m_moduleViewer.resize(m_param.nbRows * m_param.nbCols);

	for (int ii = 0; ii < m_param.nbRows; ++ii) 
	{
		for (int jj = 0; jj < m_param.nbCols; ++jj) 
		{
			int ind = ii * m_param.nbCols + jj;
			m_mainLayout.addWidget(m_moduleViewer[ind], ii, jj);
			m_moduleViewer[ind]->show();
			//m_moduleViewer[ind]->toggleDisplayOptions(1);
		}
	}
}

// #include "markus.moc"