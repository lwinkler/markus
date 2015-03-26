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

/**
* @brief Filter events based on user feedback
*/
class ClassifyEvents : public Module // TODO: Should probably be in dir bulk/
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : Module::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterDouble("validity_thres", 0.5, 0, 1, &validityThres, "Decision threshold to consider an event as valid [0 to 1]"));
			Init();
		}
		double validityThres;
	};

	ClassifyEvents(ParameterStructure& xr_params);
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
	Event   m_eventIn;
	cv::Mat m_imageIn;

	// output
	Event   m_eventOut;
	Event   m_eventToValidate;
	cv::Mat m_imageToValidate;

	// state variables
	std::list<Event> m_events;
	std::list<cv::Mat>   m_images;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;
};


#endif
