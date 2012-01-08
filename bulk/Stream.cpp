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

#include "Stream.h"

using namespace std;

Stream::Stream(int x_id, const std::string& x_name, StreamType x_type, int x_width, int x_height, Module& rx_module, const string& rx_description) :
	m_name(x_name),
	m_id(x_id),
	m_type(x_type),
	m_width(x_width),
	m_height(x_height),
	mr_module(rx_module),
	m_description(rx_description)
{
	m_connected = NULL;
}

Stream::~Stream()
{
}

void Stream::Export(ostream& rx_os, int x_indentation, bool x_isInput)
{
	string tabs(x_indentation , '\t');
	string inout = "output";
	if(x_isInput) inout = "input";
	rx_os<<tabs<<"<"<<inout<<" id=\""<<m_id<<"\">"<<endl;
	tabs = std::string(x_indentation + 1, '\t');
	rx_os<<tabs<<"<type>"<<GetTypeString()<<"</type>"<<endl;
	rx_os<<tabs<<"<name>"<<m_name<<"</name>"<<endl;
	rx_os<<tabs<<"<description>"<<GetDescription()<<"</description>"<<endl;
	tabs = std::string(x_indentation, '\t');
	rx_os<<tabs<<"</"<<inout<<">"<<endl;
}


void Stream::Connect(const Stream* x_stream)
{
	m_connected = x_stream;
}

