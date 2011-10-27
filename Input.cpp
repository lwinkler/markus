#include "Input.h"

using namespace std;

Input::Input(const std::string& x_name, ConfigReader& x_configReader): 
	m_param(x_configReader, x_name), 
	Configurable(x_configReader),
	m_name(x_name)
{

	m_capture = NULL;
	cout<<"Input "<< m_param.source<<endl;

	if(m_param.source == "cam")
	{
		cout<<"Capture from Cam"<<endl;
		m_capture = cvCaptureFromCAM( CV_CAP_ANY );
	}
	else
	{
		m_capture = cvCaptureFromFile(m_param.source.c_str());
	}
	
	if(m_capture == NULL)
	{
		throw("Error : Input or input file not found ! : " + m_param.source);
	}
	assert(m_capture != NULL);
	
	// Get capture device information
	cvQueryFrame(m_capture); // this call is necessary to get correct capture properties
	m_width    = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_WIDTH);
	m_height   = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_HEIGHT);
	m_fps     = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FPS);
	//int numFramesc = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_COUNT);
	
	m_input = cvCreateImage( cvSize(m_width, m_height), IPL_DEPTH_8U, 3);
}

Input::~Input()
{
	cvReleaseCapture(&m_capture );

}


void Input::Capture()
{
	//Get frame information:
	double posMsec   =       cvGetCaptureProperty(m_capture, CV_CAP_PROP_POS_MSEC);
	int posFrames    = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_POS_FRAMES);
	double posRatio  =       cvGetCaptureProperty(m_capture, CV_CAP_PROP_POS_AVI_RATIO);

	m_input = cvRetrieveFrame(m_capture);           // retrieve the captured frame

}
