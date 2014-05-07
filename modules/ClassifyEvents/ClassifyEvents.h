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
		Init();
	}
	double validityThres;
};


/**
* @brief Filter events based on user feedback
*/
class ClassifyEvents : public Module // TODO: Should probably be in dir bulk/
{
public:
	ClassifyEvents(const ConfigReader& x_configReader);
	~ClassifyEvents();
	MKCLASS("ClassifyEvents")
	MKDESCR("Filter events based on user feedback")
	
	virtual void ProcessFrame();
	void Reset();
	virtual void InvalidateLastEvent();
	virtual void ValidateLastEvent();
	virtual float PredictEventValidity(const Event& x_event);
	void PushEvent();
	void PopEvent();
	
protected:

	// input
	Event m_eventIn;

	// output
	Event m_eventOut;
	Event m_eventToValidate;

	// state variables
	std::list<Event> m_events;

private:
	ClassifyEventsParameterStructure m_param;
	inline virtual const ModuleParameterStructure& GetParameters() const { return m_param;}
	static log4cxx::LoggerPtr m_logger;
};


#endif
