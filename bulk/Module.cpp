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

#include "Module.h"
#include "ConfigReader.h"

#include <list>

using namespace std;

Module::Module(const std::string& x_name, ConfigReader& x_configReader) :
	Configurable(x_configReader),
	m_name(x_name)
{
	cout<<endl<<"*** Create object Module : "<<x_name<<" ***"<<endl;
};


Module::~Module()
{
	//TODO : delete m_outputStreams
};

/*void Module::AddStream(const std::string& x_name, StreamType x_type, Mat* x_image)
{
	m_outputStreams.push_back(new Stream(x_name, x_type, x_image));
}*/
