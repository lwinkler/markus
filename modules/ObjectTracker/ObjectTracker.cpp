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
	m_input = cvCreateImage(cvSize(m_param.width, m_param.height), m_param.depth, m_param.channels);

	cvSet( m_img_blur, cvScalar(0,0,0));
	detect.Reset();
	track.Reset();
	m_outputStreams.push_back(new OutputStream("input", STREAM_DEBUG, m_img_blur));
	m_outputStreams.push_back(new OutputStream("background", STREAM_DEBUG, detect.GetBackground()));
	m_outputStreams.push_back(new OutputStream("foreground", STREAM_DEBUG, detect.GetForeground()));
	m_outputStreams.push_back(new OutputStream("foreground_rff", STREAM_DEBUG, detect.GetForegroundRff()));
	//m_outputStreams.push_back(OutputStream("blobs", STREAM_DEBUG, track.GetBlobsImg()));
	m_outputStreams.push_back(new OutputStream("tracker", STREAM_OUTPUT, m_output));
}

ObjectTracker::~ObjectTracker(void )
{
	cvReleaseImage(&m_img_blur);
	cvReleaseImage(&m_output);
}

void ObjectTracker::ProcessFrame(const IplImage* x_img, const double /*x_timeSinceLastProcessing*/)
{
	cvCopy(x_img, m_input);
	
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

