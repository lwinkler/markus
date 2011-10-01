#include "Manager.h"
#include "Module.h"

#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <assert.h>

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
	m_configReader(m_configReader)
{
	m_workWidth = 640;//384;//640;
	m_workHeight = 480;//288;//320;
	m_workDepth = IPL_DEPTH_8U;//IPL_DEPTH_8U IPL_DEPTH_32F;
	m_workChannels = 3;
	m_workIsColor = (m_workChannels==3);
	
	m_configReader.ReadConfig(this);

	cout<<"Create Manager : Work image ("<<m_workWidth<<"x"<<m_workHeight<<" depth="<<m_workDepth<<" channels="<<m_workChannels<<")"<<endl;
	m_capture = NULL;
	
	cout<<"Input "<< m_input<<endl;

	if(m_input == "cam")
	{
		cout<<"Capture from Cam"<<endl;
		m_capture = cvCaptureFromCAM( CV_CAP_ANY );
	}
	else
	{
		m_capture = cvCaptureFromFile(m_input.c_str());
	}
	
	if(m_capture == NULL)
	{
		cout<<"Error : Input or input file not found ! : "<<m_input<<endl;
		exit(-1);
	}
	assert(m_capture != NULL);
	// Get capture device information
	cvQueryFrame(m_capture); // this call is necessary to get correct capture properties
	int frameHc    = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_HEIGHT);
	int frameWc    = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_WIDTH);
	int fpsc       = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FPS);
	int numFramesc = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_COUNT);

	//Initializing a video writer:

	m_writer = NULL;
	
	int fps     = fpsc;  // or 30
	
	if(m_mode == "benchmark")
	{
		m_writer = cvCreateVideoWriter("out.avi", CV_FOURCC('D','I','V','3'), fps, cvSize(m_workWidth, m_workHeight), m_workIsColor);
		assert(m_writer != NULL);
		//not working writer=cvCreateVideoWriter("out.mpg",CV_FOURCC('D', 'I', 'V', '3'), fps,cvSize(frameW,frameH),m_workIsColor);
	}
	
	m_key=0;
	
	// Create timers
	m_timerConv.start();
	
	cvNamedWindow("background", CV_WINDOW_AUTOSIZE); 
	cvMoveWindow("background", 0, 000);
	cvNamedWindow("foreground", CV_WINDOW_AUTOSIZE); 
	cvMoveWindow("foreground", 650, 000);
	cvNamedWindow("foreground_rff", CV_WINDOW_AUTOSIZE); 
	cvMoveWindow("foreground_rff", 0, 650);
	//cvNamedWindow("temporaldiff", CV_WINDOW_AUTOSIZE); 
	//cvMoveWindow("temporaldiff", 650, 650);
	cvNamedWindow("blobs", CV_WINDOW_AUTOSIZE); 
	cvMoveWindow("blobs", 650, 650);
	
	cvNamedWindow("slit", CV_WINDOW_AUTOSIZE); 
	
	for(list<Module*>::iterator it = m_modules.begin(); it != m_modules.end(); it++)
	{
		//it->CreateParamWindow();
	}
	
	m_modules.clear();
}

Manager::~Manager()
{
	cvReleaseCapture(&m_capture );
	// Releasing the video writer:
	if(m_writer != NULL) cvReleaseVideoWriter(&m_writer);

	// TODO release images ...
}

void Manager::CaptureInput()
{
	
	
}
void Manager::Process()
{
	IplImage *img = cvCreateImage( cvSize(m_workWidth, m_workHeight), m_workDepth, m_workChannels);	
	int frame=0;
	
	try
	{
		// Main loop
		while(cvGrabFrame(m_capture) && m_key != 27)
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
			
			if(m_mode == "benchmark")
			{
				static IplImage* output = cvCreateImage( cvSize(m_workWidth, m_workHeight), IPL_DEPTH_8U, m_workChannels);
				static IplImage* tmp1 = NULL;
				static IplImage* tmp2 = NULL;
				
				//adjust(detect.GetBackground(), output, tmp1, tmp2);
				//if(m_workIsColor) cvWriteFrame(m_writer,output); // proble for B&W
			}
			else
			{
				// Write output to screen
				static IplImage *output = cvCreateImage( cvSize(m_workWidth, m_workHeight), IPL_DEPTH_8U, m_workChannels);
				static IplImage* tmp1_c1 = NULL;
				static IplImage* tmp2_c1 = NULL;
				static IplImage* tmp1_c3 = NULL;
				static IplImage* tmp2_c3 = NULL;
				
				for(list<Module*>::iterator it = m_modules.begin(); it != m_modules.end(); it++)
				{
					adjust((*it)->GetOutput(), output, tmp1_c3, tmp2_c3);
					cvShowImage((*it)->GetName(), output);
				}
			
				
				m_key= (char) cvWaitKey(5);           // wait 20 ms
			}

			frame++;
			if(frame % 100 == 0)
			{
				m_timerConv.stop();
				cout<<frame<<" frames processed in "<<timerProc.value<<" s "<<frame/timerProc.value<<" frames/s"<<endl;
				cout<<"Time for Input/Output conversion "<<m_timerConv.value<<" s "<<frame/m_timerConv.value<<" frames/s"<<endl;
				cout<<"Total time "<<timerProc.value + m_timerConv.value<<" s"<<endl;
				m_timerConv.start();
			}

		}// end main loop
		
		m_timerConv.stop();
		cout<<"--------- end of execution -------------"<<endl;
		cout<<frame<<" frames processed in "<<timerProc.value<<" s "<<frame/timerProc.value<<" frames/s"<<endl;
		cout<<"Time for Input/Output conversion "<<m_timerConv.value<<" s "<<frame/m_timerConv.value<<" frames/s"<<endl;
		cout<<"Total time "<<timerProc.value + m_timerConv.value<<" s"<<endl;
		m_timerConv.start();
	}
	catch(const char* str)
	{
		cout << "Exception raised : " << str <<endl;
	}
	catch(...)
	{
		cout << "Unknown exception raised: "<<endl;
	}

}

void Manager::AddModule(Module& x_mod)
{
	m_modules.push_back(&x_mod);
}

