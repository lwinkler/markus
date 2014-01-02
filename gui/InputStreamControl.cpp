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

#include "InputStreamControl.h"
#include "QParameterSlider.h"
#include "VideoFileReader/VideoFileReader.h"

using namespace std;

const std::string InputStreamControl::m_name = "StreamControl";



InputStreamControl::InputStreamControl(VideoFileReader& rx_module) :
	Controller(),
	module(rx_module)
{
	m_widget = parameterSlider = new QParameterSlider(0, 0, module.GetMaxMsec(), 0);
	//m_actions.insert(std::make_pair("Set", &setControlledValueFloat));
	//m_actions.insert(std::make_pair("GetCurrent", &getCurrentFloat));
	//m_actions.insert(std::make_pair("GetDefault", &getDefaultFloat));
}

InputStreamControl::~InputStreamControl()
{
}
/*
void InputStreamControl::SetControlledValue()
{
	module.SetMsec(parameterSlider->GetValue());
}

void InputStreamControl::GetCurrent()
{
	parameterSlider->SetValue(module.GetMsec());
}
		
void InputStreamControl::GetDefault()
{
	parameterSlider->SetValue(0);
}
*/
