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
#include <QWidget>

#include "Control.h"

using namespace std;

#include <QScrollBar>


Controller::Controller()
{
	m_widget = NULL;
}

ControllerInt::ControllerInt(ParameterInt& x_param):
	Controller(),
	m_param(x_param)
{
	m_scrollBar = new QScrollBar;
	m_scrollBar->setOrientation(Qt::Horizontal);
	m_scrollBar->setMinimum(m_param.GetMin());
	m_scrollBar->setMaximum(m_param.GetMax());
	//0, 100, 1, 10, 42, Qt::Horizontal, 0, 0 );
	m_scrollBar->resize( 100, m_scrollBar->height() );

	m_widget = m_scrollBar;
}


Controller::~Controller()
{
	if(m_widget != NULL) delete m_widget;
}

void ControllerInt::SetControlledValue()
{
	m_param.SetValue(m_scrollBar->value());
}


Control::Control(const std::string& x_name, const std::string& x_description):
	m_name(x_name),
	m_description(x_description)
{

}

Control::~Control()
{
	for (vector<Controller*>::iterator it = m_controllers.begin() ; it != m_controllers.end() ; it++)
		delete *it;
}

ParameterControl::ParameterControl(const std::string& x_name, const std::string& x_description, ParameterStructure& x_param):
	Control(x_name, x_description),
	m_param(x_param)
{

	for(vector<Parameter*>::const_iterator it = m_param.GetList().begin(); it != m_param.GetList().end(); it++)
	{
		Controller * ctrr = NULL;
		switch((*it)->GetType())
		{
			case PARAM_BOOL:
			break;
			case PARAM_DOUBLE:
			break;
			case PARAM_FLOAT:
			break;
			case PARAM_IMAGE_TYPE:
			break;
			case PARAM_INT:
				ctrr = new ControllerInt(*dynamic_cast<ParameterInt*>(*it));
			break;
			case PARAM_STR:
			break;
		}
		if(ctrr != NULL)AddController(ctrr); // TODO : use asssert
	}
}

ParameterControl::~ParameterControl()
{
}

void Control::SetControlledValue()
{
	for(vector<Controller*>::iterator it = m_controllers.begin() ; it != m_controllers.end() ; it++)
		(*it)->SetControlledValue();
}
