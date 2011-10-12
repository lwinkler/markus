#include "Manager.h"
#include "Module.h"

#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <assert.h>

#include "ObjectTracker.h"
#include "SlitCam.h"

#include "timer.h"

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
	//m_workIsColor = (m_workChannels==3);	
	//cout<<"Create Manager : Work image ("<<m_workWidth<<"x"<<m_workHeight<<" depth="<<m_workDepth<<" channels="<<m_workChannels<<")"<<endl;
	m_capture = NULL;
	m_frameCount = 0;
	
	cout<<"Input "<< m_param.input<<endl;

	if(m_param.input == "cam")
	{
		cout<<"Capture from Cam"<<endl;
		m_capture = cvCaptureFromCAM( CV_CAP_ANY );
	}
	else
	{
		m_capture = cvCaptureFromFile(m_param.input.c_str());
	}
	
	if(m_capture == NULL)
	{
		throw("Error : Input or input file not found ! : " + m_param.input);
	}
	assert(m_capture != NULL);
	
	// Get capture device information
	cvQueryFrame(m_capture); // this call is necessary to get correct capture properties
	//int frameHc    = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_HEIGHT);
	//int frameWc    = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_WIDTH);
	int fpsc       = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FPS);
	//int numFramesc = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_COUNT);
	
	
	//Initializing a video writer:

	m_writer = NULL;
	
	int fps     = fpsc;  // or 30
	
	if(m_param.mode == "benchmark")
	{
		m_writer = cvCreateVideoWriter("out.avi", CV_FOURCC('D','I','V','3'), fps, cvSize(m_param.width, m_param.height), m_param.channels == 3);
		assert(m_writer != NULL);
		//not working writer=cvCreateVideoWriter("out.mpg",CV_FOURCC('D', 'I', 'V', '3'), fps,cvSize(frameW,frameH),m_workIsColor);
	}
	
	m_key=0;
	
	// Create timers
	m_timerConv.start();
			
	m_modules.clear();
	
	m_configReader.ReadConfigModules();
	std::list<ParameterValue> moduleList =  m_configReader.m_parameterList;
	for(std::list<ParameterValue>::const_iterator it = moduleList.begin() ; it != moduleList.end() ; it++)
	{
		if(it->m_name.compare("SlitCamera") == 0)
		{
			AddModule(new SlitCam(it->m_value, m_configReader));
		}
		else if(it->m_name.compare("ObjectTracker") == 0)
		{
			AddModule(new ObjectTracker(it->m_value, m_configReader));
		}
		else throw("Module type unknown : " + it->m_name);
	}
}

Manager::~Manager()
{
	cvReleaseCapture(&m_capture );
	// Releasing the video writer:
	if(m_writer != NULL) cvReleaseVideoWriter(&m_writer);

	for(list<Module*>::iterator it = m_modules.begin(); it != m_modules.end(); it++)
	{
		delete(*it);
	}
}

void Manager::CaptureInput()
{
	
	
}
void Manager::Process()
{
	IplImage *img = cvCreateImage( cvSize(m_param.width, m_param.height), m_param.depth, m_param.channels);	
	
	// Main loop
	cvGrabFrame(m_capture);
	//while(cvGrabFrame(m_capture) && m_key != 27)
	{
		IplImage* source=cvRetrieveFrame(m_capture);           // retrieve the captured frame
		
		static IplImage* tmp1=NULL;
		static IplImage* tmp2=NULL;
		adjust(source, img, tmp1, tmp2);
		
		//Get frame information:
		static double posMsec   =       cvGetCaptureProperty(m_capture, CV_CAP_PROP_POS_MSEC);
		static int posFrames    = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_POS_FRAMES);
		static double posRatio  =       cvGetCaptureProperty(m_capture, CV_CAP_PROP_POS_AVI_RATIO);
		
		//printf("Processing frame %d (%dx%d) with %d channels\n", frame, width, height, channels); 
		// declare a destination IplImage object with correct size, depth and channels			
		
		m_timerConv.stop();
		timerProc.start();
		
		for(list<Module*>::iterator it = m_modules.begin(); it != m_modules.end(); it++)
		{
			(*it)->ProcessFrame(img);
		}
		
		timerProc.stop();
		m_timerConv.start();
		
		if(m_param.mode == "benchmark")
		{
			static IplImage* output = cvCreateImage( cvSize(m_param.width, m_param.height), IPL_DEPTH_8U, m_param.channels);
			static IplImage* tmp1 = NULL;
			static IplImage* tmp2 = NULL;
			
			//adjust(detect.GetBackground(), output, tmp1, tmp2);
			//if(m_workIsColor) cvWriteFrame(m_writer,output); // proble for B&W
		}
		else
		{
			// Write output to screen
			/*static IplImage *output = cvCreateImage( cvSize(m_param.width, m_param.height), IPL_DEPTH_8U, m_param.channels);
			static IplImage* tmp1_c1 = NULL;
			static IplImage* tmp2_c1 = NULL;
			static IplImage* tmp1_c3 = NULL;
			static IplImage* tmp2_c3 = NULL;
			
			for(list<Module*>::iterator it = m_modules.begin(); it != m_modules.end(); it++)
			{
				const std::list<OutputStream> streamList((*it)->GetOutputStreamList());
				for(list<OutputStream>::const_iterator it2 = streamList.begin(); it2 != streamList.end(); it2++)
				{
					if((*it2).GetImageRef()->nChannels == 3)
						adjust((*it2).GetImageRef(), output, tmp1_c3, tmp2_c3);
					else
						adjust((*it2).GetImageRef(), output, tmp1_c1, tmp2_c1);

					cvShowImage((*it2).GetName().c_str(), output);
				}
			}*/
			//m_key= (char) cvWaitKey(5);           // wait 20 ms
		}

		m_frameCount++;
		if(m_frameCount % 100 == 0)
		{
			m_timerConv.stop();
			cout<<m_frameCount<<" frames processed in "<<timerProc.value<<" s "<<m_frameCount/timerProc.value<<" frames/s"<<endl;
			cout<<"Time for Input/Output conversion "<<m_timerConv.value<<" s "<<m_frameCount/m_timerConv.value<<" frames/s"<<endl;
			cout<<"Total time "<<timerProc.value + m_timerConv.value<<" s"<<endl;
			m_timerConv.start();
		}

	}// end main loop
	
	/*m_timerConv.stop();
	cout<<"--------- end of execution -------------"<<endl;
	cout<<frame<<" frames processed in "<<timerProc.value<<" s "<<frame/timerProc.value<<" frames/s"<<endl;
	cout<<"Time for Input/Output conversion "<<m_timerConv.value<<" s "<<frame/m_timerConv.value<<" frames/s"<<endl;
	cout<<"Total time "<<timerProc.value + m_timerConv.value<<" s"<<endl;
	m_timerConv.start();*/
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


