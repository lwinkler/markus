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
#include "util.h"

#include <list>

using namespace std;

Module::Module(const std::string& x_name, ConfigReader& x_configReader) :
	Configurable(x_configReader),
	m_name(x_name)
{
	cout<<endl<<"*** Create object Module : "<<x_name<<" ***"<<endl;
	m_input = NULL;
	m_img_tmp1 = NULL;
	m_img_tmp2 = NULL;
};


Module::~Module()
{
	if(m_img_tmp1 != NULL) delete(m_img_tmp1);
	if(m_img_tmp2 != NULL) delete(m_img_tmp2);

	//TODO : delete m_outputStreams
};

void Module::SetInput(const cv::Mat* x_img)
{
	adjust(x_img, m_input, m_img_tmp1, m_img_tmp2);
}

/*void Module::AddStream(const std::string& x_name, StreamType x_type, Mat* x_image)
{
	m_outputStreams.push_back(new Stream(x_name, x_type, x_image));
}*/

/// Describe the module with name, parameters, inputs, outputs to an output stream (in xml)
void Module::DescribeModule(ostream& rx_os)
{
	rx_os<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"<<endl;
	rx_os<<"<module name=\""<<m_name<<"\" description=\""<<"descr!!!TODO"<<"\">"<<endl;

	rx_os<<"<parameters>"<<endl;
	for(vector<Parameter*>::const_iterator it = GetRefParameter().GetList().begin() ; it != GetRefParameter().GetList().end() ; it++)
		(*it)->Describe(rx_os);
	rx_os<<"</parameters>"<<endl;

	rx_os<<"<inputs>"<<endl;
	rx_os<<"</inputs>"<<endl;
	rx_os<<"<outputs>"<<endl;
	for(vector<Stream*>::const_iterator it = m_outputStreams.begin() ; it != m_outputStreams.end() ; it++)
		(*it)->Describe(rx_os);
	rx_os<<"</outputs>"<<endl;
	rx_os<<"</module>"<<endl;
}
