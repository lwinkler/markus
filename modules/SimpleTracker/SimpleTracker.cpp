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

#include "SimpleTracker.h"
#include "StreamDebug.h"
#include "StreamRect.h"

// for debug
#include "util.h"

using namespace cv;

const char * SimpleTracker::m_type = "SimpleTracker";


SimpleTracker::SimpleTracker(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader), 
	track(m_param.tracker, m_param.width, m_param.height, m_param.type)
{
	m_description = "Track any detect objects (from StreamRect).";
	track.Reset();
	
	m_inputStreams.push_back(new StreamRect(0, "input", 	m_param.width, m_param.height, m_trackerInput, cvScalar(255,255,255), *this,	"Input objects"));

	m_outputStreams.push_back(new StreamRect(0, "tracker", m_param.width, m_param.height, m_trackerOutput, cvScalar(255,255,255), *this,	"Tracked objects"));
}

SimpleTracker::~SimpleTracker(void )
{
}

void SimpleTracker::ProcessFrame(const double /*x_timeSinceLastProcessing*/)
{
	// Main part of the program
	//track.ExtractBlobs(detect.GetForegroundRff());
	//track.PrintRegions();
	track.MatchTemplates();
	track.CleanTemplates();
	track.DetectNewTemplates();
	track.UpdateTemplates();
	
	//cvCopy(track.GetBlobsImg(), m_output);
	//track.PrintTrackedRegions();
};

