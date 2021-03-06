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

namespace mk {
using namespace std;

/**
* @brief Command: Set the value of the cursor to the given position in ms
*
* @param rx_value
*/
void ControllerInputStream::SetCursor(mkjson& rx_value)
{
	Processable::WriteLock lock(m_module.RefLock());
	if(! rx_value.is_null())
		m_module.SetMsec(rx_value.get<double>());
#ifndef MARKUS_NO_GUI
	else m_module.SetMsec(m_parameterSlider->GetValue());
#else
	else throw MkException("This function can only be called with a GUI or along with a string pointer for input/output", LOC);
#endif
}

/**
* @brief Command: Get the position of the cursor in ms
*
* @param rx_value
*/
void ControllerInputStream::GetCursor(mkjson& rx_value)
{
	Processable::ReadLock lock(m_module.RefLock());
	if(! rx_value.is_null())
	{
		rx_value = m_module.GetMsec();
	}
#ifndef MARKUS_NO_GUI
	else m_parameterSlider->SetValue(m_module.GetMsec());
#else
	else throw MkException("This function can only be called with a GUI or along with a string pointer for input/output", LOC);
#endif
}

ControllerInputStream::ControllerInputStream(mk::VideoFileReader& rx_module) :
	Controller("reader"),
	m_module(rx_module)
{
	m_actions.insert(make_pair("Get", &ControllerInputStream::GetCursor));
	m_actions.insert(make_pair("Set", &ControllerInputStream::SetCursor));
	m_parameterSlider = nullptr;
}

ControllerInputStream::~ControllerInputStream()
{
}

QWidget* ControllerInputStream::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	return m_parameterSlider = new QParameterSlider(0, 0, m_module.GetMaxMsec(), 0);
#else
	return nullptr;
#endif
}
} // namespace mk
