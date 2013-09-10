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

#include "Manager.h"
#include "Module.h"
#include "Input.h"
#include "Stream.h"

#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <assert.h>

#include <sstream>
#include <string.h>
using namespace std;

#include "AllModules.h"

	
#if defined( WIN32 ) && defined( TUNE )
	#include <crtdbg.h>
	_CrtMemState startMemState;
	_CrtMemState endMemState;
#endif


using namespace std;


Manager::Manager(ConfigReader& x_configReader) : 
	Configurable(x_configReader),
	m_param(m_configReader, "Manager")
{
	cout<<endl<<"*** Create object Manager ***"<<endl;
	m_frameCount = 0;
	
	//Initializing a video writer:

	m_writer = NULL;
	
	if(m_param.mode == "benchmark")
	{
		//m_writer = cvCreateVideoWriter("out.avi", CV_FOURCC('D','I','V','3'), fps, cvSize(m_param.width, m_param.height), m_param.channels == 3);
		assert(m_writer != NULL);
		//not working writer=cvCreateVideoWriter("out.mpg",CV_FOURCC('D', 'I', 'V', '3'), fps,cvSize(frameW,frameH),m_workIsColor);
	}
	
	
	// m_inputs.clear();
	m_modules.clear();
	
	// Read the configuration of each module
	ConfigReader moduleConfig = m_configReader.SubConfig("module");
	
	while(! moduleConfig.IsEmpty())
	{
		// Read parameters
		if( moduleConfig.SubConfig("parameters").IsEmpty()) 
			throw("Impossible to find <parameters> section for module " +  moduleConfig.GetAttribute("name"));
		Module * tmp1 = createNewModule( moduleConfig);		
		
		// Add to inputs if an input
		m_modules.push_back(tmp1);
		// if(tmp1->IsInput()) m_inputs.push_back(dynamic_cast<Input* >(tmp1));
		moduleConfig = moduleConfig.NextSubConfig("module");
	}

	
	// Connect input and output streams (re-read the config once since we need all modules to be connected)
	moduleConfig = m_configReader.SubConfig("module");
	
	while(! moduleConfig.IsEmpty())
	{
		// Read conections of inputs
		if(!moduleConfig.SubConfig("inputs").IsEmpty()) 
		{
			ConfigReader inputConfig = moduleConfig.SubConfig("inputs").SubConfig("input");
			while(! inputConfig.IsEmpty())
			{
				int moduleId		= atoi(moduleConfig.GetAttribute("id").c_str());
				int inputId 		= atoi(inputConfig.GetAttribute("id").c_str());
				int outputModuleId 	= atoi(inputConfig.GetAttribute("moduleid").c_str());
				int outputId 		= atoi(inputConfig.GetAttribute("outputid").c_str());
				
				Stream * inputStream  = GetModuleById(moduleId)->GetInputStreamById(inputId);
				Stream * outputStream = GetModuleById(outputModuleId)->GetOutputStreamById(outputId);
				
				inputStream->Connect(outputStream);
				
				inputConfig = inputConfig.NextSubConfig("input");
			}
		}
		moduleConfig = moduleConfig.NextSubConfig("module");
	}

	// Set the module preceeding the current module
	for(vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
	{
		try
		{
			Stream& stream = (*it)->GetInputStreamById(0)->RefConnected();
			if(&stream == NULL) throw(0);
			Module& preceeding(stream.RefModule());
			(*it)->SetPreceedingModule(preceeding);
			if((*it)->GetFps() == 0)
				preceeding.AddDependingModule(**it);
		}
		catch(...){}
	}

	// Reset timers
	m_timerConvertion = 0;
	m_timerProcessing = 0;

	// Reset all modules (to set the module timer)
	for(vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
	{
		(*it)->Reset();
	}
	m_timer.Restart();
}

Manager::~Manager()
{
	PrintTimers();
	
	for(vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
		delete *it;
	
	// Releasing the video writer:
	if(m_writer != NULL) cvReleaseVideoWriter(&m_writer);
}

/// Process all modules

void Manager::Process()
{
	//m_lock.lockForWrite();
	if(!m_lock.tryLockForWrite())
	{
		cout<<"Warning : Manager too slow !"<<endl;
		return;
	}
	m_timer.Restart();
	usleep(100000);
	// double timecount = m_timer.GetSecDouble();
	m_timer.Restart();
	
	try
	{
		/*
		for(vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
		{
			(*it)->Process(timecount);
		}*/
	}
	catch(cv::Exception& e)
	{
		cout << "Exception raised (std::exception) : " << e.what() <<endl;
	}
	catch(std::exception& e)
	{
		cout << "Exception raised (std::exception) : " << e.what() <<endl;
	}
	catch(std::string str)
	{
		cout << "Exception raised (string) : " << str <<endl;
	}
	catch(const char* str)
	{
		cout << "Exception raised (const char*) : " << str <<endl;
	}
	catch(...)
	{
		cout << "Unknown exception raised: "<<endl;
	}
	m_frameCount++;
	if(m_frameCount % 100 == 0)
	{
		PrintTimers();
	}
	m_lock.unlock();
}

/// Print the results of timings

void Manager::PrintTimers()
{
		cout<<m_frameCount<<" frames processed in "<<m_timerProcessing<<" ms ("<<  (1000.0 * m_frameCount) / m_timerProcessing<<" frames/s)"<<endl;
		cout<<"input convertion "                  <<m_timerConvertion<<" ms ("<<(1000.0 * m_frameCount) / m_timerConvertion<<" frames/s)"<<endl;
		cout<<"Total time "<< m_timerProcessing + m_timerConvertion<<" ms ("<<     (1000.0 * m_frameCount) /(m_timerProcessing + m_timerConvertion)<<" frames/s)"<<endl;

		int cpt = 0;
		for(vector<Module*>::const_iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
		{
			cout<<cpt<<": ";
			(*it)->PrintStatistics(cout);
			cpt++;
		}
}

/// Export current configuration to xml

void Manager::Export()
{
	system("mkdir -p modules");
	for(vector<Module*>::const_iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
	{
		string file("modules/" + (*it)->GetName() + ".xml");
		ofstream os(file.c_str());
		os<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"<<endl;
		(*it)->Export(os, 0);
		os.close();
	}
}

Module* Manager::GetModuleById(int x_id) const
{
	for(vector<Module*>::const_iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
		if((*it)->GetId() == x_id) 
			return *it;
	assert(false);
	return NULL;
}
