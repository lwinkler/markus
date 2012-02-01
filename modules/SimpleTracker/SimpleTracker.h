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

#ifndef SIMPLETRACKER_H
#define SIMPLETRACKER_H

#include "Module.h"
#include "Tracker.h"
#include "StreamObject.h"

class ConfigReader;

class SimpleTrackerParameterStructure : public ModuleParameterStructure
{
public:
	SimpleTrackerParameterStructure(const ConfigReader& x_confReader) : ModuleParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterDouble(0, "maxMatchingDistance", 100, 	PARAM_DOUBLE, 0, MAX_WIDTH + MAX_HEIGHT, &tracker.maxMatchingDistance,		"Tolerance of the tracker."));
		m_list.push_back(new ParameterInt   (0, "maxNbFramesDisappearance", 10, PARAM_INT, 1, 1000,			&tracker.maxNbFramesDisappearance,	"Time before disappearence of an object")); // TODO : should be in seconds
		m_list.push_back(new ParameterBool  (0, "symetricMatch", 	true, 	PARAM_BOOL, 0, 1, 			&tracker.symetricMatch,			"Each match between objects and templates must be symetrical"));

		ParameterStructure::Init();
	};
	TrackerParameter tracker;
};

class SimpleTracker : public Module
{
public:
	SimpleTracker(const ConfigReader& x_configReader);
	~SimpleTracker();
	
	virtual void ProcessFrame();
	inline virtual int GetInputWidth() const {return m_param.width;};
	inline virtual int GetInputHeight() const {return m_param.height;};
	
private:
	SimpleTrackerParameterStructure m_param;

	Tracker track;
	static const char * m_type;
protected:
	inline virtual ModuleParameterStructure& RefParameter() { return m_param;};
};


#endif