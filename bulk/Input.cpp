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

#include "Input.h"
#include "Stream.h"

#include "util.h"

using namespace std;


Input::Input(const ConfigReader& x_configReader): 
	Module(x_configReader)
{
	LOG4CXX_INFO(m_logger, "Create object "<<m_name<<" of type Input");
	m_endOfStream = false;
}

Input::~Input()
{
}

/// Set the timestamps of all the outputs
void Input::SetTimeStampToOutputs(TIME_STAMP x_timeStamp)
{
	for(vector<Stream*>::iterator it = m_outputStreams.begin() ; it != m_outputStreams.end() ; it++)
		(*it)->SetTimeStamp(x_timeStamp);
	
}
