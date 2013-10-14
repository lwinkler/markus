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

#include "ParameterControl.h"
#include <cstdio>

using namespace std;

#include <QWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>

#include "QParameterSlider.h"


#define PRECISION_DOUBLE 2


ParameterControl::ParameterControl(const std::string& x_name, const std::string& x_description):
	Control(x_name, x_description)
{

}

ParameterControl::~ParameterControl()
{
	// Delete all controllers
	for(vector<Controller*>::iterator it = m_controllers.begin() ; it != m_controllers.end() ; it++)
	{
		delete(*it);
	}
	m_controllers.clear();
}

void ParameterControl::SetParameterStructure(ParameterStructure& rx_param)
{
	mp_param = &rx_param;

	// Delete all controllers
	for(vector<Controller*>::iterator it = m_controllers.begin() ; it != m_controllers.end() ; it++)
	{
		delete(*it);
	}
	m_controllers.clear();

	for(vector<Parameter*>::const_iterator it = mp_param->GetList().begin(); it != mp_param->GetList().end(); it++)
	{
		Controller * ctrr = NULL;
		switch((*it)->GetType())
		{
			case PARAM_BOOL:
				ctrr = new ControllerBool(*dynamic_cast<ParameterBool*>(*it));
				break;
			case PARAM_DOUBLE:
				ctrr = new ControllerDouble(*dynamic_cast<ParameterDouble*>(*it));
				break;
			case PARAM_FLOAT:
				ctrr = new ControllerFloat(*dynamic_cast<ParameterFloat*>(*it));
				break;
			case PARAM_IMAGE_TYPE:
				ctrr = new ControllerEnum(*dynamic_cast<ParameterEnum*>(*it)); 
				break;
			case PARAM_INT:
				ctrr = new ControllerInt(*dynamic_cast<ParameterInt*>(*it));
				break;
			case PARAM_STR:
				ctrr = new ControllerString(*dynamic_cast<ParameterString*>(*it));
				break;
		}
		if(ctrr == NULL) throw("Controller creation failed");
		else AddController(ctrr);
	}
}

void ParameterControl::CleanParameterStructure()
{
	// Delete all controllers
	for(vector<Controller*>::iterator it = m_controllers.begin() ; it != m_controllers.end() ; it++)
	{
		delete(*it);
	}
	m_controllers.clear();
}
