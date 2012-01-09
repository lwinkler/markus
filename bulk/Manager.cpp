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
	//m_workIsColor = (m_workChannels==3);	
	//cout<<"Create Manager : Work image ("<<m_workWidth<<"x"<<m_workHeight<<" depth="<<m_workDepth<<" channels="<<m_workChannels<<")"<<endl;
	m_frameCount = 0;
	
	//Initializing a video writer:

	m_writer = NULL;
	
	//int fps     = 30; // FIXME fpsc;  // or 30
	
	if(m_param.mode == "benchmark")
	{
		//m_writer = cvCreateVideoWriter("out.avi", CV_FOURCC('D','I','V','3'), fps, cvSize(m_param.width, m_param.height), m_param.channels == 3);
		assert(m_writer != NULL);
		//not working writer=cvCreateVideoWriter("out.mpg",CV_FOURCC('D', 'I', 'V', '3'), fps,cvSize(frameW,frameH),m_workIsColor);
	}
	
	
	m_inputs.clear();
	m_modules.clear();
	
	//int tot = m_configReader.ReadConfigGetVectorSize("", "module");

	ConfigReader moduleConfig = m_configReader.SubConfig("module");
	
	while(! moduleConfig.IsEmpty())
	{
		// Read parameters
		if( moduleConfig.SubConfig("parameters").IsEmpty()) 
			throw("Impossible to find <parameters> section for module " +  moduleConfig.GetAttribute("name"));
		//vector<ParameterValue> paramList = moduleConfig.SubConfig("parameters").ReadParameters("param");
		//string moduleClass = ConfigReader::GetParameterValue("class", paramList).m_value;
		//const string moduleName = moduleConfig.GetAttribute("name");
		Module * tmp1 = createNewModule( moduleConfig);
		
		
		// Add to inputs if an input
		m_modules.push_back(tmp1);
		if(tmp1->IsInput()) m_inputs.push_back(dynamic_cast<Input* >(tmp1));
		moduleConfig = moduleConfig.NextSubConfig("module");
	}

	
	// Connect input and output streams
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
	/*for(vector<Input *>::const_iterator it = m_inputs.begin() ; it != m_inputs.end() ; it++)
	{
		m_modules.push_back(dynamic_cast<Module *>(*it));
		//cpt++;
	}*/
	
	// Create timers
	m_timerConv.reset();
	m_timerProc.reset();
	
	m_timerLastProcess = clock();
}

Manager::~Manager()
{
	PrintTimers();
	/*for(vector<Input*>::iterator it = m_inputs.begin(); it != m_inputs.end(); it++)
	{
		delete(*it);
	}*/
	
	for(vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
		delete *it;
	
	// Releasing the video writer:
	if(m_writer != NULL) cvReleaseVideoWriter(&m_writer);
}


void Manager::Process()
{
	m_lock.lockForWrite();
	//m_timerConv.stop();
	m_timerProc.start();
	
	clock_t tmp = clock();
	double timecount = ( (double)tmp - m_timerLastProcess) / CLOCKS_PER_SEC;
	m_timerLastProcess = tmp;
	
	try
	{
		/*for(vector<ImageProcessor*>::iterator it = m_imageProcessors.begin(); it != m_imageProcessors.end(); it++)
		{
			(*it)->Process(timecount);
		}*/
		//m_lock.lockForWrite();
		//m_timeSinceLastProcessing += x_timeSinceLast;
		//cout<<"Process "<<m_module->GetName()<<" "<<m_timeSinceLastProcessing<<" >= "<<m_timeInterval<<" "<<m_input->GetImage()<<endl;
		for(vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
		{
			//assert(m_input->GetImage() != NULL);
			if(timecount >= 1.0 / (*it)->GetFps())
			{
				//m_input->m_lock.lockForRead();
				(*it)->ReadAndConvertInput();
				//m_input->m_lock.unlock();
				(*it)->ProcessFrame(timecount);
				//m_input->ProcessFrame(timecount);
				//m_timeSinceLastProcessing = 0;
			}
		}
		//m_lock.unlock();
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

	
	m_timerProc.stop();
	//m_timerConv.start();

	//m_timerConv.start();
	//sleep(1);
	//m_timerConv.stop();	
	
	m_frameCount++;
	if(m_frameCount % 100 == 0)
	{
		PrintTimers();
	}
	m_lock.unlock();
}

void Manager::PrintTimers()
{
		cout<<m_frameCount<<" frames processed in "<<m_timerProc.value<<" s ("<<m_frameCount/m_timerProc.value<<" frames/s)"<<endl;
		cout<<"Time between calls to process method "<<m_timerConv.value<<" s ("<<m_frameCount/m_timerConv.value<<" frames/s)"<<endl;
		cout<<"Total time "<<m_timerProc.value + m_timerConv.value<<" s ("<<m_frameCount/(m_timerProc.value + m_timerConv.value)<<" frames/s)"<<endl;
}

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

/*void Manager::AddModule(Module * x_mod)
{
	int cpt = 0;
	m_modules.push_back(x_mod);
	/ *const std::list<Stream> streamList(x_mod->GetStreamList());
	for(list<Stream>::const_iterator it2 = streamList.begin(); it2 != streamList.end(); it2++)
	{
		// Create windows for output streams
		cvNamedWindow(it2->GetName().c_str(), CV_WINDOW_AUTOSIZE);
		cvMoveWindow(it2->GetName().c_str(), 100 * cpt, 30 * cpt);
		cpt++;
	}* /
	cout<<" manager adds module "<<x_mod->GetName()<<endl;
}


void Manager::AddInput(Input* x_input)
{
	int cpt = 0;
	m_inputs.push_back(x_input);
	m_modules.push_back(x_input);
	cout<<" manager adds input "<<x_input->GetName()<<endl;
}*/
