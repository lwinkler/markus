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


void setCursor(Controller* x_ctr)
{
	InputStreamControl* ctr = dynamic_cast<InputStreamControl*>(x_ctr);
	assert(ctr != NULL);
	ctr->module.SetMsec(ctr->parameterSlider->GetValue());
}

void getCursor(Controller* x_ctr)
{
	InputStreamControl* ctr = dynamic_cast<InputStreamControl*>(x_ctr);
	assert(ctr != NULL);
	ctr->parameterSlider->SetValue(ctr->module.GetMsec());
}

InputStreamControl::InputStreamControl(VideoFileReader& rx_module) :
	Controller("File reader"),
	module(rx_module)
{
	m_widget = parameterSlider = new QParameterSlider(0, 0, module.GetMaxMsec(), 0);
	m_actions.insert(std::make_pair("Get", &getCursor));
	m_actions.insert(std::make_pair("Set", &setCursor));
}

InputStreamControl::~InputStreamControl()
{
}
