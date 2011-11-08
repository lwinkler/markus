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
#include "ImageProcessor.h"

#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <assert.h>

//#include "ObjectTracker.h"
//#include "SlitCam.h"

//#include "timer.h"

	#include <sstream>
	#include <string.h>
	using namespace std;

#if defined( WIN32 ) && defined( TUNE )
	#include <crtdbg.h>
	_CrtMemState startMemState;
	_CrtMemState endMemState;
#endif


using namespace std;


Manager::Manager(ConfigReader& x_configReader) : 
	m_param(m_configReader, "Manager"),
	Configurable(x_configReader)
{
	cout<<"*** Create object Manager ***"<<endl;
	//m_workIsColor = (m_workChannels==3);	
	//cout<<"Create Manager : Work image ("<<m_workWidth<<"x"<<m_workHeight<<" depth="<<m_workDepth<<" channels="<<m_workChannels<<")"<<endl;
	m_frameCount = 0;
	
	//Initializing a video writer:

	m_writer = NULL;
	
	int fps     = 30; // FIXME fpsc;  // or 30
	
	if(m_param.mode == "benchmark")
	{
		//m_writer = cvCreateVideoWriter("out.avi", CV_FOURCC('D','I','V','3'), fps, cvSize(m_param.width, m_param.height), m_param.channels == 3);
		assert(m_writer != NULL);
		//not working writer=cvCreateVideoWriter("out.mpg",CV_FOURCC('D', 'I', 'V', '3'), fps,cvSize(frameW,frameH),m_workIsColor);
	}
	
	m_key=0;
	
	// Create timers
	m_timerConv.start();
			
	m_inputs.clear();
	m_modules.clear();
	
	int tot = m_configReader.ReadConfigGetVectorSize("ImageProcessors", "ImageProcessor");

	for(int i = 0 ; i < tot; i++)
	{
		ImageProcessor * ip = new ImageProcessor("ip", i, m_configReader, m_inputs);
		m_imageProcessors.push_back(ip);
		//m_inputs.push_back(&ip->GetInput());
		m_modules.push_back(&ip->GetModule());
	}
		//m_configReader.ReadConfigObjectFromVect("ImageProcessor", "imageprocessor", i);
}

Manager::~Manager()
{
	for(vector<ImageProcessor*>::iterator it = m_imageProcessors.begin(); it != m_imageProcessors.end(); it++)
	{
		delete(*it);
	}
	for(vector<Input*>::iterator it = m_inputs.begin(); it != m_inputs.end(); it++)
	{
		delete(*it);
	}
	
	// Releasing the video writer:
	if(m_writer != NULL) cvReleaseVideoWriter(&m_writer);
}


void Manager::Process()
{
	m_timerConv.stop();
	timerProc.start();
	
	// Aquire input images and process
	/*for(vector<Input*>::iterator it = m_inputs.begin(); it != m_inputs.end(); it++)
	{
		(*it)->Capture();
	}*/
	for(vector<ImageProcessor*>::iterator it = m_imageProcessors.begin(); it != m_imageProcessors.end(); it++)
	{
		(*it)->Process();
	}
	
	timerProc.stop();
	m_timerConv.start();

	m_frameCount++;
	if(m_frameCount % 100 == 0)
	{
		m_timerConv.stop();
		cout<<m_frameCount<<" frames processed in "<<timerProc.value<<" s "<<m_frameCount/timerProc.value<<" frames/s"<<endl;
		cout<<"Time for Input/Output conversion "<<m_timerConv.value<<" s "<<m_frameCount/m_timerConv.value<<" frames/s"<<endl;
		cout<<"Total time "<<timerProc.value + m_timerConv.value<<" s"<<endl;
		m_timerConv.start();
	}
}

void Manager::AddModule(Module * x_mod)
{
	int cpt = 0;
	m_modules.push_back(x_mod);
	/*const std::list<OutputStream> streamList(x_mod->GetOutputStreamList());
	for(list<OutputStream>::const_iterator it2 = streamList.begin(); it2 != streamList.end(); it2++)
	{
		// Create windows for output streams
		cvNamedWindow(it2->GetName().c_str(), CV_WINDOW_AUTOSIZE);
		cvMoveWindow(it2->GetName().c_str(), 100 * cpt, 30 * cpt);
		cpt++;
	}*/
}


void Manager::AddInput(Input* x_input)
{
	int cpt = 0;
	m_inputs.push_back(x_input);
}
