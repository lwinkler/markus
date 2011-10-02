// +--------------------------------------------------------------------------+
// | File      : SlitCam.cpp                                                  |
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

/*void detectorBarCallback4(int pos) 
{
	SlitCam::SetTimeInterval(pos);	
	cout<<"Time interval "<<SlitCam::GetTimeInterval()<<endl;
}*/

/*void SlitCam::CreateParamWindow()
{
	//cvNamedWindow("Parameters", CV_WINDOW_AUTOSIZE); 
	//cvMoveWindow("Parameters", 0, 0);
	
	//cvCreateTrackbar( "Time interval ", "Parameters", &m_time_interval, 7, detectorBarCallback4 );

}*/

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
	int aperture = m_param.aperture;
	char * pDst = m_output->imageData + m_position * m_output->nChannels * aperture;// * img->nChannels;
	char * pSrc = img->imageData + img->widthStep * img->nChannels / 2;
	int size = m_output->depth * aperture;
	assert(m_output->imageSize == img->imageSize);
	
	for(int negCount = img->height ; negCount ; negCount--)
	{
		memcpy(pDst, pSrc, size);// * img->nChannels);
		//memset(pDst, *pSrc, m_image->depth);
		pSrc += widthStep;
		pDst += widthStep;
	}
	m_position = m_position == img->width - 1 ? 0 : m_position + 1;
}

