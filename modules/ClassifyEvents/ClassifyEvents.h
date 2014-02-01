/*----------------------------------------------------------------------------------
*
*    author : Laurent Winkler <lwinkler888@gmail.com>
* 
* 
*    This file is not part of Markus.
-------------------------------------------------------------------------------------*/

#ifndef CLASSIFY_EVENTS
#define CLASSIFY_EVENTS

#include "Module.h"
#include "Event.h"
#include "StreamObject.h"

class ConfigReader;

class ClassifyEventsParameterStructure : public ModuleParameterStructure
{
public:
	ClassifyEventsParameterStructure(const ConfigReader& x_confReader) : ModuleParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterDouble("validity_thres", 0.5, 0, 1, &validityThres, "Decision threshold to consider an event as valid [0 to 1]"));
		Init(); // TODO: This causes a warning when inherited ! Check if still true
	}
	double validityThres;
};


class ClassifyEvents : public Module
{
public:
	ClassifyEvents(const ConfigReader& x_configReader);
	~ClassifyEvents();
	
	virtual void ProcessFrame();
	void Reset();
	virtual void InvalidateLastEvent();
	virtual void ValidateLastEvent();
	virtual float PredictEventValidity(const Event& x_event);
	
protected:

	Event m_eventIn;
	Event m_eventOut;

private:
	ClassifyEventsParameterStructure m_param;
	inline virtual const ModuleParameterStructure& RefParameter() const { return m_param;}
};


#endif
