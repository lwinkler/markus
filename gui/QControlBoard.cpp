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
#include "Controller.h"
#include "Module.h"
#include "Manager.h"
#include <QBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>

using namespace std;

	 
QControlBoard::QControlBoard(Module& x_module, QWidget *parent):
	m_currentModule(x_module)
{
	QBoxLayout * mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
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


/// Update the list of controls associated with the module
void QControlBoard::updateControl(Controller* x_control)
{
	mp_currentControl = x_control;

	// Create the control buttons
	// TODO: remove buttons and delete widgets
	vector<string> actions;
	mp_currentControl->ListActions(actions);
	for(vector<string>::const_iterator it = actions.begin() ; it != actions.end() ; it++)
	{
		// note : names must match between buttons and actions
		QPushButton* button = new QPushButton(it->c_str());
		mp_buttonLayout->addWidget(button);
		connect(button, SIGNAL(pressed()), this, SLOT(callAction(void)));
	}


	/// Create new control screen
	mp_gbControls->setWidgetResizable(true);
	QGridLayout * vbox = new QGridLayout;

	QLabel * lab = new QLabel(x_control->GetName().c_str());
	vbox->addWidget(lab, 0, 0);
	QWidget* wid = x_control->CreateWidget();
	if(wid != NULL)
		vbox->addWidget(wid, 0, 1);

	QWidget *viewport = new QWidget;
	viewport->setLayout(vbox);
	viewport->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	mp_gbControls->setWidget(viewport);

	mp_gbControls->show();
	mp_gbButtons->show();
}


void QControlBoard::callAction()
{
	QPushButton* button = dynamic_cast<QPushButton*>(sender());
	assert(button != NULL);
	/*map<string, const px_action>::const_iterator it = mp_currentControl->GetActions().find(button->text().toStdString());
	if(it == mp_currentControl->GetActions().end())
		throw MkException("Cannot find action in controller " + button->text().toStdString(), LOC);
	*/

	// Call the function pointer associated with the action
	//(*(it->second))(mp_currentControl, NULL);


	m_currentModule.LockForWrite();
	LOG_DEBUG(Manager::Logger(), "Call control on module "<<m_currentModule.GetName());
	mp_currentControl->CallAction(button->text().toStdString(), NULL);
	m_currentModule.Unlock();
}
