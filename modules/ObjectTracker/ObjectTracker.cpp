
#include "ObjectTracker.h"

// for debug
#include "util.h"
#include "cv.h"
#include "highgui.h"

const char * ObjectTracker::m_type = "ObjectTracker";


ObjectTracker::ObjectTracker(const std::string& x_name, ConfigReader& x_configReader) :
	detect(m_param.detector, m_param.width, m_param.height, m_param.depth, m_param.channels),
	track(m_param.tracker, m_param.width, m_param.height, m_param.depth, m_param.channels),
	m_param(x_configReader, x_name), Module(x_name, x_configReader)
{
	m_img_blur = NULL;
	m_img_blur = cvCreateImage(cvSize(m_param.width, m_param.height), IPL_DEPTH_8U, m_param.channels);
	m_output   = cvCreateImage(cvSize(m_param.width, m_param.height), m_param.depth, m_param.channels);

	cvSet( m_img_blur, cvScalar(0,0,0));
	detect.Reset();
	track.Reset();
	m_outputStreams.push_back(OutputStream("background", STREAM_DEBUG, detect.GetBackground()));
	m_outputStreams.push_back(OutputStream("foreground", STREAM_DEBUG, detect.GetForeground()));
	m_outputStreams.push_back(OutputStream("foreground_rff", STREAM_DEBUG, detect.GetForegroundRff()));
	//m_outputStreams.push_back(OutputStream("blobs", STREAM_DEBUG, track.GetBlobsImg()));
	m_outputStreams.push_back(OutputStream("tracker", STREAM_OUTPUT, m_output));
}

ObjectTracker::~ObjectTracker(void )
{
	cvReleaseImage(&m_img_blur);
	cvReleaseImage(&m_output);
}

void ObjectTracker::ProcessFrame(const IplImage* x_img)
{
	// Main part of the program
	detect.BlurInput(x_img, m_img_blur);
	detect.UpdateBackground(m_img_blur);
	//detect.UpdateBackgroundMask(img, detect.GetForegroundRff());
	detect.ExtractForeground(m_img_blur);
	//detect.ExtractForegroundMax(img);
	//detect.RemoveFalseForegroundNeigh();
	detect.RemoveFalseForegroundMorph();
	detect.TemporalDiff(m_img_blur);
	
	track.ExtractBlobs(detect.GetForegroundRff());
	//track.PrintRegions();
	track.MatchTemplates();
	track.CleanTemplates();
	track.DetectNewTemplates();
	track.UpdateTemplates();
	
	cvCopy(track.GetBlobsImg(), m_output);
	//track.PrintTrackedRegions();
};

