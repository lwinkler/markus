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
#include "StreamObject.h"

// for debug
#include "util.h"

using namespace cv;

const char * SimpleTracker::m_type = "SimpleTracker";


SimpleTracker::SimpleTracker(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader), 
	track(m_param.tracker, m_param.width, m_param.height, m_param.type)
{
	m_description = "Track any detect objects (from StreamObject).";
	track.Reset();
	
	m_inputStreams.push_back(new StreamObject(0, "input", 	m_param.width, m_param.height, track.m_objects, cvScalar(255,255,255), *this,	"Input objects"));

	m_outputStreams.push_back(new StreamObject(0, "tracker", m_param.width, m_param.height, track.m_objects, cvScalar(255,255,255), *this,	"Tracked objects"));

	// TODO : Output template + check if ok in/out same
	
}

SimpleTracker::~SimpleTracker(void )
{
}

void SimpleTracker::ProcessFrame()
{
	// Main part of the program
	//track.ExtractBlobs(detect.GetForegroundRff());
	//track.PrintRegions();
	//int cpt = 0;
	
	/*track.m_objects.clear();
	for(vector<Object>::const_iterator it = m_trackerInput.begin()  ; it != m_trackerInput.end() ; it++ )
	{
		TrackedRegion reg;
		const Rect& rect = it->GetRect();
		reg.m_posX = rect.x;
		reg.m_posY = rect.y;
		
		/*if(Feature::m_names.size() == 0)
		{
			// Write names once
			Feature::m_names.push_back("area");
			Feature::m_names.push_back("perimeter");
			Feature::m_names.push_back("position x");
			Feature::m_names.push_back("position y");
		}* /
		reg.AddFeature(rect.x);
		reg.AddFeature(rect.y);
		reg.AddFeature(rect.width);
		reg.AddFeature(rect.height);
		
		track.m_objects.push_back(reg);
		//cpt++;
	}*/
	
	
	track.MatchTemplates();
	track.CleanTemplates();
	track.DetectNewTemplates();
	track.UpdateTemplates();
	
	/*m_trackerOutput.clear();
	for(list<Template>::const_iterator it = track.m_templates.begin()  ; it != track.m_templates.end() ; it++ )
	{
		Rect rect;
		rect.x = it->m_posX;
		rect.y = it->m_posY;
		rect.width = 10;
		rect.height = 30;//TODO
		Object obj;
		obj.SetRect(rect); // TODO : Objects must be centered !!!!
		m_trackerOutput.push_back(obj);
	}*/

	//cvCopy(track.GetBlobsImg(), m_output);
	//track.PrintTrackedRegions();
};

