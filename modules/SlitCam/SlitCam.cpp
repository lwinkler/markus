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


SlitCam::SlitCam(ConfigReader& x_configReader) 
	 : m_param(x_configReader, "SlitCamera"), Module(x_configReader)
{
	m_time_interval = 0;
	m_position = 0;
	
	// Init images
	m_output = cvCreateImage(cvSize(m_param.width, m_param.height),
				 m_param.depth, m_param.channels);

	// Init output images
	m_outputStreams.push_back(OutputStream("myslit", STREAM_OUTPUT, m_output));
}

SlitCam::~SlitCam(void)
{
	cvReleaseImage(&m_output);
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

