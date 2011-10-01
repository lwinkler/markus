// +--------------------------------------------------------------------------+
// | File      : SlitCam.h                                                    |
// | Utility   : One-Slit camera module                                       |
// | Author    : Laurent Winkler                                              |
// | Date      : Sept 2010                                                    |
// | Remarque  : none                                                         |
// +--------------------------------------------------------------------------+

#include "SlitCam.h"

#include <iostream>
#include <cstdio>
#include <highgui.h>

using namespace std;


float SlitCam::m_time_interval = 0;
const char * SlitCam::m_name = "SlitCamera";


SlitCam::SlitCam(ConfigReader& x_configReader, int width, int height, int depth, int channels) 
	 : Module(x_configReader, width, height, depth, channels)
{
	Init();
}

void detectorBarCallback4(int pos) 
{
	SlitCam::SetTimeInterval(pos);	
	cout<<"Time interval "<<SlitCam::GetTimeInterval()<<endl;
}

void SlitCam::CreateParamWindow()
{
	cvNamedWindow("Parameters", CV_WINDOW_AUTOSIZE); 
	cvMoveWindow("Parameters", 0, 0);
	
	//cvCreateTrackbar( "Time interval ", "Parameters", &m_time_interval, 7, detectorBarCallback4 );

}

SlitCam::~SlitCam(void)
{
}

void SlitCam::Init()
{
	Module::Init();
	m_time_interval = 0;
	m_position = 0;
}

void SlitCam::ProcessFrame(const IplImage * img)
{
	int widthStep = img->widthStep;
	char * pDst = m_output->imageData + m_position * m_output->depth;
	char * pSrc = img->imageData + img->widthStep / 2;
	for(int i = 0 ; i < img->height ; i++)
	{
		memcpy(pDst, pSrc, m_output->depth * sizeof(char));
		
		//memset(pDst, *pSrc, m_image->depth);
		pSrc += img->widthStep;
		pDst += m_output->widthStep;
	}
	m_position = m_position == img->width ? 0 : m_position + 1;
	//cout<<"m_pos "<< m_position<<endl;
}

