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
#include "StreamDebug.h"
#include "StreamImage.h"

// for debug
#include "util.h"
#include "cv.h"
#include "highgui.h"

using namespace cv;

const char * ObjectTracker::m_type = "ObjectTracker";


ObjectTracker::ObjectTracker(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader), 
	detect(m_param.detector, m_param.width, m_param.height, m_param.type)
{
	m_description = "Track moving objects on video by using backgroung subtraction.";
	//m_img_blur = NULL;
	//m_img_blur = new Mat(cvSize(m_param.width, m_param.height), m_param.type);//IPL_DEPTH_8U, m_param.channels);
	m_output   = new Mat(cvSize(m_param.width, m_param.height), m_param.type);
	m_input    = new Mat(cvSize(m_param.width, m_param.height), m_param.type);

	//m_img_blur->setTo(cvScalar(0,0,0));
	detect.Reset();
	
	//m_inputStreams.push_back(new StreamDebug(0, "input", m_img_blur, *this,					"Video input"));
	m_inputStreams.push_back(new StreamImage(0, "input", m_input, *this,					"Video input"));

	m_outputStreams.push_back(new StreamImage(0, "foreground", detect.GetForegroundRff(), *this,				"Foreground"));
	m_outputStreams.push_back(new StreamImage(1, "background", detect.GetBackground(), *this,		"Background"));
	
	m_debugStreams.push_back(new StreamDebug(0, "foreground", detect.GetForeground(), *this,		""));
	m_debugStreams.push_back(new StreamDebug(1, "foreground_rff", detect.GetForegroundRff(), *this,	""));
	//m_debugStreams.push_back(StreamDebug("blobs", track.GetBlobsImg()));
}

ObjectTracker::~ObjectTracker(void )
{
	//delete(m_img_blur);
	delete(m_input);
	delete(m_output);
}

void ObjectTracker::ProcessFrame(const double /*x_timeSinceLastProcessing*/)
{
	// Main part of the program
	//detect.BlurInput(m_input, m_img_blur);
	//detect.UpdateBackground(m_img_blur);
	detect.UpdateBackground(m_input);
	
	//detect.UpdateBackgroundMask(img, detect.GetForegroundRff());
	
	//detect.ExtractForeground(m_img_blur);
	detect.ExtractForeground(m_input);
	
	//detect.ExtractForegroundMax(img);
	//detect.RemoveFalseForegroundNeigh();
	detect.RemoveNoiseForeground();

	//detect.TemporalDiff(m_img_blur);
	detect.TemporalDiff(m_input);
};

