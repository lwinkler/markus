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

#include "ControllerInputStream.h"
#include "VideoFileReader/VideoFileReader.h"

#ifndef MARKUS_NO_GUI
#include "QParameterSlider.h"
#endif

using namespace std;

void setCursor(Controller* xp_ctr, string* xp_value)
{
	InputStreamControl* ctr = dynamic_cast<InputStreamControl*>(xp_ctr);
	assert(ctr != NULL);
	if(xp_value != NULL)
		ctr->module.SetMsec(PSTR2FLOAT(xp_value));
#ifndef MARKUS_NO_GUI
	else ctr->module.SetMsec(ctr->parameterSlider->GetValue());
#else
	else throw MkException("This function can only be called with a GUI or along with a string pointer for input/output", LOC);
#endif
}

void getCursor(Controller* xp_ctr, string* xp_value)
{
	InputStreamControl* ctr = dynamic_cast<InputStreamControl*>(xp_ctr);
	assert(ctr != NULL);
	if(xp_value != NULL)
	{
		FLOAT2PSTR(ctr->module.GetMsec(), xp_value);
	}
#ifndef MARKUS_NO_GUI
	else ctr->parameterSlider->SetValue(ctr->module.GetMsec());
#else
	else throw MkException("This function can only be called with a GUI or along with a string pointer for input/output", LOC);
#endif
}

InputStreamControl::InputStreamControl(VideoFileReader& rx_module) :
	Controller("reader", "inputStream"),
	module(rx_module)
{
	m_actions.insert(std::make_pair("Get", &getCursor));
	m_actions.insert(std::make_pair("Set", &setCursor));
}

InputStreamControl::~InputStreamControl()
{
}

QWidget* InputStreamControl::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	return parameterSlider = new QParameterSlider(0, 0, module.GetMaxMsec(), 0);
#else
	return NULL;
#endif
}
