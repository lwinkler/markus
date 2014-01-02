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

#include "QControlBoard.h"
#include "Control.h"
#include <QBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>

using namespace std;

#define CLEAN_DELETE(x) if((x) != NULL){delete((x));(x) = NULL;}
	 
//QControlBoard::QControlBoard(const std::string& x_name, const std::string& x_description):
QControlBoard::QControlBoard(Module * x_module, QWidget *parent) :
	m_name("bla"),
	m_description("TODO")
{
	QBoxLayout * mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	// m_currentModule         = x_module;
	mp_gbControls		= new QScrollArea;
	mp_gbButtons		= new QGroupBox(tr("Actions"));

	// Create the group with settings buttons	
	mp_buttonLayout = new QHBoxLayout;

	mp_gbButtons->setLayout(mp_buttonLayout);
	mainLayout->addWidget(mp_gbControls, 0);
	mainLayout->addWidget(mp_gbButtons, 1);
	setLayout(mainLayout);
}

QControlBoard::~QControlBoard()
{
	CLEAN_DELETE(mp_gbControls);
	// for (vector<Controller*>::iterator it = m_controllers.begin() ; it != m_controllers.end() ; it++)
		// delete *it;
}

void QControlBoard::resizeEvent(QResizeEvent * e)
{
}

void QControlBoard::paintEvent(QPaintEvent * e) 
{
}


void QControlBoard::updateControl(Controller* x_control)
{
	m_currentControl = x_control;

	// ParameterControl   * controlParam = dynamic_cast<ParameterControl*>(x_control);
	// InputStreamControl * controlInput = dynamic_cast<InputStreamControl*>(x_control);

	
	// if(controlParam)
		// controlParam->SetParameterStructure(m_currentModule->RefParameter());
	/* else if(controlInput)
	{
		VideoFileReader * module = dynamic_cast<VideoFileReader*>(m_currentModule);
		assert(module);
		controlInput->SetModule(*module);
	}
	else assert(false); */

	// Create the control buttons
	// TODO: remove buttons and delete widgets
	for(map<string, const px_action>::const_iterator it = m_currentControl->GetActions().begin() ; it != m_currentControl->GetActions().end() ; it++)
	{
		// note : names must match between buttons and actions
		QPushButton* button = new QPushButton(it->first.c_str());
		mp_buttonLayout->addWidget(button);
		connect(button, SIGNAL(pressed()), this, SLOT(callAction(void)));
	}


	/// Create new control screen
	mp_gbControls->setWidgetResizable(true);
	QGridLayout * vbox = new QGridLayout;

	// Add controls (= parameters)
	/*int cpt = 0;
	for(vector<Controller*>::iterator it = RefListControllers().begin() ; it != RefListControllers().end() ; it++)
	{
		QLabel * lab = new QLabel((*it)->GetName().c_str());
		vbox->addWidget(lab, cpt, 0);
		vbox->addWidget((*it)->RefWidget(), cpt, 1);
		cpt++;
	}*/
	QLabel * lab = new QLabel(x_control->GetName().c_str());
	vbox->addWidget(lab, 0, 0);
	vbox->addWidget(x_control->RefWidget(), 0, 1);

	QWidget *viewport = new QWidget;
	viewport->setLayout(vbox);
	viewport->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	mp_gbControls->setWidget(viewport);

	//mp_widEmpty->hide();
	mp_gbControls->show();
	mp_gbButtons->show();
}


void QControlBoard::callAction()
{
	QPushButton* button = dynamic_cast<QPushButton*>(sender());
	assert(button != NULL);
	map<string, const px_action>::const_iterator it = m_currentControl->GetActions().find(button->text().toStdString());
	if(it == m_currentControl->GetActions().end())
		throw MkException("Cannot find action in controller " + button->text().toStdString(), LOC);
	(*(it->second))(m_currentControl);
	// cout<<"Action called "<<button->text().toStdString()<<endl;	
}
