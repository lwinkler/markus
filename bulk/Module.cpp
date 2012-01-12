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

#include "Stream.h"
#include "StreamImage.h"
#include "StreamDebug.h"
#include "StreamRect.h"

#include <list>

using namespace std;

Module::Module(const ConfigReader& x_configReader) :
	Configurable(x_configReader)
{
	m_name 	= x_configReader.GetAttribute("name");
	m_id	= atoi(x_configReader.GetAttribute("id").c_str());
	cout<<endl<<"*** Create object Module : "<<m_name<<" id:"<<m_id<<" ***"<<endl;
	m_processingTime = 0;
};


Module::~Module()
{
	// Delete all streams
	for(std::vector<Stream* >::iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; it++)
		delete(*it);
	for(std::vector<Stream* >::iterator it = m_outputStreams.begin() ; it != m_outputStreams.end() ; it++)
		delete(*it);
	for(std::vector<Stream* >::iterator it = m_debugStreams.begin() ; it != m_debugStreams.end() ; it++)
		delete(*it);
};

void Module::ReadAndConvertInput()//const cv::Mat* x_img)
{
	//m_input->m_lock.lockForRead();
	
	// adjust(x_img, m_input, m_img_tmp1, m_img_tmp2);
	for(vector<Stream*>::iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; it++)
	{
		(*it)->LockForRead();
		(*it)->ConvertInput();
		(*it)->UnLock();
	}
	//m_input->m_lock.unlock();
}

/*void Module::AddStream(const std::string& x_name, StreamType x_type, Mat* x_image)
{
	m_outputStreams.push_back(new Stream(x_name, x_type, x_image));
}*/

/// Describe the module with name, parameters, inputs, outputs to an output stream (in xml)
void Module::Export(ostream& rx_os, int x_indentation)
{
	string tabs(x_indentation, '\t');
	tabs = string(x_indentation, '\t');
	rx_os<<tabs<<"<module name=\""<<m_name<<"\" description=\""<<GetDescription()<<"\">"<<endl;
	tabs = string(x_indentation + 1, '\t');
	rx_os<<tabs<<"<parameters>"<<endl;
	for(vector<Parameter*>::const_iterator it = GetRefParameter().GetList().begin() ; it != GetRefParameter().GetList().end() ; it++)
		(*it)->Export(rx_os, x_indentation + 2);
	rx_os<<tabs<<"</parameters>"<<endl;

	rx_os<<tabs<<"<inputs>"<<endl;
	for(vector<Stream*>::const_iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; it++)
		(*it)->Export(rx_os, x_indentation + 2, true);
	rx_os<<tabs<<"</inputs>"<<endl;

	rx_os<<tabs<<"<outputs>"<<endl;
	for(vector<Stream*>::const_iterator it = m_outputStreams.begin() ; it != m_outputStreams.end() ; it++)
		(*it)->Export(rx_os, x_indentation + 2, false);
	rx_os<<tabs<<"</outputs>"<<endl;
	tabs = string(x_indentation, '\t');
	rx_os<<tabs<<"</module>"<<endl;
}

/// Get a stream by its id
Stream* Module::GetInputStreamById(int x_id) const
{
	for(vector<Stream *>::const_iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; it++)
		if((*it)->GetId() == x_id) return *it;
	assert(false);
	return NULL;
}

/// Get a stream by its id
Stream* Module::GetOutputStreamById(int x_id) const
{
	for(vector<Stream *>::const_iterator it = m_outputStreams.begin() ; it != m_outputStreams.end() ; it++)
		if((*it)->GetId() == x_id) return *it;
	assert(false);
	return NULL;
}


