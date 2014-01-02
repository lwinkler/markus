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
		ParameterStructure::Init();
	}
};


class ClassifyEvents : public Module
{
public:
	ClassifyEvents(const ConfigReader& x_configReader);
	~ClassifyEvents();
	
	virtual void ProcessFrame();
	void Reset();
	
protected:

	Event m_eventIn;
	Event m_eventOut;

	static const char * m_type;

private:
	ClassifyEventsParameterStructure m_param;
	inline virtual ModuleParameterStructure& RefParameter() { return m_param;}
};


#endif