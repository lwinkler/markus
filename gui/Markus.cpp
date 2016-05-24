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

MarkusWindow::MarkusWindow(ParameterStructure& rx_param, Manager& rx_manager)
	: Configurable(rx_param),
	  m_param(dynamic_cast<MarkusWindow::Parameters&>(rx_param)),
	  m_manager(rx_manager)
{
	// Call to manager process each 10 ms
	//if(!m_centralized)
	startTimer(static_cast<int>(MARKUS_TIMER_S * 1000));  // 10 -> 0.01-second timer

	setWindowState(Qt::WindowMaximized);

	//m_scroll.clear();
	m_paramsViewer.clear();
	m_moduleViewer.clear();
	//resizeEvent()();
	m_mainWidget.setLayout(&m_mainLayout);
	setCentralWidget(&m_mainWidget);

	createActionsAndMenus();
	resizeEvent(nullptr);

	setWindowTitle(tr("Markus"));
}


void MarkusWindow::timerEvent(QTimerEvent* px_event)
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

void MarkusWindow::UpdateConfig()
{
	for(auto & elem : m_paramsViewer)
		elem->UpdateConfig();
	m_param.UpdateConfig();
}

QLabel *MarkusWindow::createLabel(const QString &text)
{
	auto label = new QLabel(text);
	label->setAlignment(Qt::AlignCenter);
	label->setMargin(2);
	label->setFrameStyle(QFrame::Box | QFrame::Sunken);
	return label;
}


//----------------------------------------------------------------------------------------------------
// Print about text
//----------------------------------------------------------------------------------------------------
void MarkusWindow::about()
{
	QMessageBox::about(this, tr("About Image Viewer"),
					   tr("<p>The <b>Markus</b> image processing environment lets "
						  "the user experiment with the different elements of computer "
						  "vision.</p> "
						  "<p><b>Author : Laurent Winkler</b></p>"));
}



//----------------------------------------------------------------------------------------------------
// Create actions and menus
//----------------------------------------------------------------------------------------------------
void MarkusWindow::createActionsAndMenus()
{
	// Action for file menu
	QAction* exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	// Actions for view menu
	QAction* viewDisplayOptionsAct = new QAction(tr("Show display options"), this);
	viewDisplayOptionsAct->setCheckable(true);
	connect(viewDisplayOptionsAct, SIGNAL(triggered(bool)), this, SLOT(viewDisplayOptions(bool)));
	QAction* view1x1Act = new QAction(tr("View 1x1"), this);
	connect(view1x1Act, SIGNAL(triggered()), this, SLOT(view1x1()));
	QAction* view1x2Act = new QAction(tr("View 2x1"), this);
	connect(view1x2Act, SIGNAL(triggered()), this, SLOT(view1x2()));
	QAction* view2x2Act = new QAction(tr("View 2x2"), this);
	connect(view2x2Act, SIGNAL(triggered()), this, SLOT(view2x2()));
	QAction* view2x3Act = new QAction(tr("View 2x3"), this);
	connect(view2x3Act, SIGNAL(triggered()), this, SLOT(view2x3()));
	QAction* view3x3Act = new QAction(tr("View 3x3"), this);
	connect(view3x3Act, SIGNAL(triggered()), this, SLOT(view3x3()));
	QAction* view3x4Act = new QAction(tr("View 3x4"), this);
	connect(view3x4Act, SIGNAL(triggered()), this, SLOT(view3x4()));
	QAction* view4x4Act = new QAction(tr("View 4x4"), this);
	connect(view4x4Act, SIGNAL(triggered()), this, SLOT(view4x4()));


	// Action for help menu
	QAction* aboutAct = new QAction(tr("&About"), this);
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	// File Menu
	QMenu* fileMenu = new QMenu(tr("&File"), this);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);

	// View menu
	QMenu* viewMenu = new QMenu(tr("&View"), this);
	viewMenu->addAction(viewDisplayOptionsAct);
	viewMenu->addAction(view1x1Act);
	viewMenu->addAction(view1x2Act);
	viewMenu->addAction(view2x2Act);
	viewMenu->addAction(view2x3Act);
	viewMenu->addAction(view3x3Act);
	viewMenu->addAction(view3x4Act);
	viewMenu->addAction(view4x4Act);

	// Manager menu
	QMenu* managerMenu = new QMenu(tr("&Manager"), this);

	// Action for manager menu
	//    list all commands available in the manager controller
	vector<string> actions;
	m_manager.FindController("manager").ListActions(actions);
	for(const auto& elem : actions)
	{
		QAction* action = new QAction(elem.c_str(), this);
		if(elem == "Stop")
			action->setShortcut(QKeySequence("p"));
		else if(elem == "Start")
			action->setShortcut(QKeySequence("Shift+p"));
		else if(elem == "Reset")
			action->setShortcut(QKeySequence("Shift+r"));
		else if(elem == "ResetExceptInputs")
			action->setShortcut(QKeySequence("r"));
		else if(elem == "ProcessOne")
			action->setShortcut(QKeySequence("o"));
		connect(action, SIGNAL(triggered()), this, SLOT(callManagerCommand()));
		managerMenu->addAction(action);
	}

	// Help menu
	QMenu* helpMenu = new QMenu(tr("&Help"), this);
	helpMenu->addAction(aboutAct);

	menuBar()->addMenu(fileMenu);
	menuBar()->addMenu(viewMenu);
	menuBar()->addMenu(managerMenu);
	menuBar()->addMenu(helpMenu);
}

void MarkusWindow::viewDisplayOptions(bool x_isChecked)
{
	int size = m_moduleViewer.size();
	for(int ind = 0 ; ind < size; ind++)
	{
		m_moduleViewer.at(ind)->showDisplayOptions(x_isChecked);
	}
}

void MarkusWindow::view1x1()
{
	m_param.nbRows = 1;
	m_param.nbCols = 1;
	resizeEvent(nullptr);
}

void MarkusWindow::view1x2()
{
	m_param.nbRows = 1;
	m_param.nbCols = 2;
	resizeEvent(nullptr);
}

void MarkusWindow::view2x2()
{
	m_param.nbRows = 2;
	m_param.nbCols = 2;
	resizeEvent(nullptr);
}

void MarkusWindow::view2x3()
{
	m_param.nbRows = 2;
	m_param.nbCols = 3;
	resizeEvent(nullptr);
}

void MarkusWindow::view3x3()
{
	m_param.nbRows = 3;
	m_param.nbCols = 3;
	resizeEvent(nullptr);
}

void MarkusWindow::view3x4()
{
	m_param.nbRows = 3;
	m_param.nbCols = 4;
	resizeEvent(nullptr);
}

void MarkusWindow::view4x4()
{
	m_param.nbRows = 4;
	m_param.nbCols = 4;
	resizeEvent(nullptr);
}

void MarkusWindow::callManagerCommand()
{
	QAction* action = dynamic_cast<QAction*>(sender());
	assert(action != nullptr);

	m_manager.SendCommand("manager.manager." + string(action->text().toStdString()), "");
}

void MarkusWindow::resizeEvent(QResizeEvent* event)
{
	// hide all modules
	int size = m_moduleViewer.size();
	for(int ind = 0 ; ind < size; ind++)
	{
		m_mainLayout.removeWidget(m_moduleViewer.at(ind));
		m_mainLayout.removeWidget(m_moduleViewer.at(ind));
		m_moduleViewer.at(ind)->hide();
	}

	// Add new module viewers to config
	for(int ind = size ; ind < m_param.nbRows * m_param.nbCols ; ind++)
	{
		stringstream ss;
		ss<<"viewer"<<ind;
		ConfigReader conf(m_param.config.FindRef("viewer[name=\"" + ss.str() + "\"]", true));
		conf.FindRef("parameters", true);
		m_paramsViewer.push_back(new QModuleViewer::Parameters(conf));
// 		m_moduleViewer.push_back(new QModuleViewer(m_manager, *m_paramsViewer.back()));
		m_moduleViewer.at(ind)->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		// m_moduleViewer.at(ind)->showDisplayOptions(true);
	}

	// Remove extra modules
	for(int ind = m_param.nbRows * m_param.nbCols ; ind < size ; ind++)
	{
		delete(m_moduleViewer.at(ind));
		delete(m_paramsViewer.at(ind));
	}
	m_moduleViewer.resize(m_param.nbRows * m_param.nbCols);
	m_paramsViewer.resize(m_param.nbRows * m_param.nbCols);

	for (int ii = 0; ii < m_param.nbRows; ++ii)
	{
		for (int jj = 0; jj < m_param.nbCols; ++jj)
		{
			int ind = ii * m_param.nbCols + jj;
			m_mainLayout.addWidget(m_moduleViewer.at(ind), ii, jj);
			m_moduleViewer.at(ind)->show();
			//m_moduleViewer.at(ind)->toggleDisplayOptions(1);
		}
	}
}

// #include "markus.moc"
