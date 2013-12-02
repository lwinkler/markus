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

#include "QParameterControlBoard.h"
#include <cstdio>

#include <QComboBox>
#include <QGroupBox>
#include <QGridLayout>
// #include <QMenu>
#include <QScrollBar>
#include <QPushButton>
#include <QLabel>
#include <qevent.h>

// #include <QPixmap>
#include <QScrollArea>
// #include <QListWidget>
#include <QBoxLayout>

#include "Manager.h"
#include "Module.h"
#include "util.h"
#include "StreamImage.h"
#include "ParameterControl.h"
#include "InputStreamControl.h"
#include "VideoFileReader/VideoFileReader.h"

#define CLEAN_DELETE(x) if((x) != NULL){delete((x));(x) = NULL;}

using namespace std;
using namespace cv;

// Constructor
QParameterControlBoard::QParameterControlBoard(Module *x_module, QWidget *parent) : QWidget(parent)
{
	QBoxLayout * mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	m_currentModule         = x_module;
	mp_gbControls		= new QScrollArea;
	mp_gbButtons		= new QGroupBox(tr("Parameter options"));


	// Create the group with control buttons (for settings)
	mp_buttonGetCurrentControl  = new QPushButton(tr("Get current"));
	mp_buttonGetDefaultControl  = new QPushButton(tr("Get default"));
	mp_buttonSetControl         = new QPushButton(tr("Set"));
	mp_buttonResetModule        = new QPushButton(tr("Reset module"));


	// Create the group with settings buttons	
	QHBoxLayout * buttonLayout = new QHBoxLayout;
	mp_gbButtons->setLayout(buttonLayout);

	mainLayout->addWidget(mp_gbControls, 0);
	mainLayout->addWidget(mp_gbButtons, 1);

	buttonLayout->addWidget(mp_buttonGetCurrentControl);
	buttonLayout->addWidget(mp_buttonGetDefaultControl);
	buttonLayout->addWidget(mp_buttonSetControl);
	buttonLayout->addWidget(mp_buttonResetModule);

	connect(mp_buttonGetCurrentControl, SIGNAL(pressed()), this, SLOT(getCurrentControl(void)));
	connect(mp_buttonGetDefaultControl, SIGNAL(pressed()), this, SLOT(getDefaultControl(void)));
	connect(mp_buttonSetControl       , SIGNAL(pressed()), this, SLOT(SetControlledValue(void)));
	connect(mp_buttonResetModule      , SIGNAL(pressed()), this, SLOT(resetModule(void)));

	setLayout(mainLayout); // TODO fix this
}

QParameterControlBoard::~QParameterControlBoard(void) 
{
	CLEAN_DELETE(mp_gbControls);
	CLEAN_DELETE(mp_gbButtons);
	// if(mp_widEmpty != NULL) delete mp_widEmpty;
}

void QParameterControlBoard::resizeEvent(QResizeEvent * e)
{
}

void QParameterControlBoard::paintEvent(QPaintEvent * e) 
{
}


void QParameterControlBoard::updateControl(ControlBoard* x_control)
{
	m_currentControl = x_control;

	ParameterControl   * controlParam = dynamic_cast<ParameterControl*>(m_currentControl);
	InputStreamControl * controlInput = dynamic_cast<InputStreamControl*>(m_currentControl);

	
	if(controlParam)
		controlParam->SetParameterStructure(m_currentModule->RefParameter());
	else if(controlInput)
	{
		VideoFileReader * module = dynamic_cast<VideoFileReader*>(m_currentModule);
		assert(module);
		controlInput->SetModule(*module);
	}
	else assert(false);


	/// Create new control screen
	mp_gbControls->setWidgetResizable(true);
	QGridLayout * vbox = new QGridLayout;

	// Add controls (= parameters)
	int cpt = 0;
	for(vector<Controller*>::iterator it = m_currentControl->RefListControllers().begin() ; it != m_currentControl->RefListControllers().end() ; it++)
	{
		QLabel * lab = new QLabel((*it)->GetName().c_str());
		vbox->addWidget(lab, cpt, 0);
		vbox->addWidget((*it)->RefWidget(), cpt, 1);
		cpt++;
	}

	QWidget *viewport = new QWidget;
	viewport->setLayout(vbox);
	viewport->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	mp_gbControls->setWidget(viewport);

	//mp_widEmpty->hide();
	mp_gbControls->show();
	mp_gbButtons->show();
}

void QParameterControlBoard::SetControlledValue()
{
	if(m_currentControl != NULL)
	{
		m_currentControl->SetControlledValue();
	}
}

void QParameterControlBoard::resetModule()
{
	m_currentModule->Reset();
}

void QParameterControlBoard::getCurrentControl()
{
	if(m_currentControl != NULL)
	{
		m_currentControl->GetCurrent();
	}
}

void QParameterControlBoard::getDefaultControl()
{
	if(m_currentControl != NULL)
	{
		m_currentControl->GetDefault();
	}
}


// #include "QParameterControlBoard.moc"
