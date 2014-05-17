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

#include "ParameterString.h"

using namespace std;

void ParameterString::Export(std::ostream& rx_os, int x_tabs)
{
	std::string tabs(x_tabs, '\t');
	rx_os<<tabs<<"<param name=\""<<m_name<<"\">"<<std::endl;
	tabs = std::string(x_tabs + 1, '\t');
	rx_os<<tabs<<"<type>"<<GetTypeString()<<"</type>"<<std::endl;
	rx_os<<tabs<<"<value default=\""<<m_default<<"\">"<<GetValue()<<"</value>"<<std::endl;
	rx_os<<tabs<<"<description>"<<m_description<<"</description>"<<std::endl;
	tabs = std::string(x_tabs, '\t');
	rx_os<<tabs<<"</param>"<<std::endl;
}
