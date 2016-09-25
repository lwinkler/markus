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
#include "Manager.h"
#include <QBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>

using namespace std;


QControlBoard::QControlBoard(Manager& rx_manager, const string& x_moduleName, const string& x_controllerName, QWidget *xp_parent)
	: mr_manager(rx_manager),
	m_moduleName(x_moduleName),
	m_controllerName(x_controllerName)
{
	auto  mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	mp_gbControls		= new QScrollArea;
	mp_gbButtons		= new QGroupBox(tr("Actions"));

	// Create the group with settings buttons
	mp_buttonLayout = new QHBoxLayout;

	mp_gbButtons->setLayout(mp_buttonLayout);
	mainLayout->addWidget(mp_gbControls, 0);
	mainLayout->addWidget(mp_gbButtons, 1);
	setLayout(mainLayout);

	// Create the control buttons
	vector<string> actions;
	mr_manager.GetModuleByName(m_moduleName).FindController(x_controllerName).ListActions(actions);
	for(const auto& elem : actions)
	{
		// note : names must match between buttons and actions
		QPushButton* button = new QPushButton(elem.c_str());
		mp_buttonLayout->addWidget(button);
		connect(button, SIGNAL(pressed()), this, SLOT(callAction(void)));
	}

	/// Create new control screen
	mp_gbControls->setWidgetResizable(true);
	auto  vbox = new QGridLayout;

	QLabel * lab = new QLabel(x_controllerName.c_str());
	vbox->addWidget(lab, 0, 0);
	QWidget* wid = mr_manager.CreateControllerWidget(x_moduleName, x_controllerName);
	if(wid != nullptr)
		vbox->addWidget(wid, 0, 1);

	auto viewport = new QWidget;
	viewport->setLayout(vbox);
	viewport->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	mp_gbControls->setWidget(viewport);

	mp_gbControls->show();
	mp_gbButtons->show();
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


void QControlBoard::callAction()
{
	QPushButton* button = dynamic_cast<QPushButton*>(sender());
	assert(button != nullptr);

	// LOG_DEBUG(Manager::Logger(), "Call control on module "<<m_currentModule.GetName());
	mr_manager.SendCommand(m_moduleName + "." + m_controllerName + "." + button->text().toStdString());
}
