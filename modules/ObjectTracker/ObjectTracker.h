#ifndef OBJECTTRACKER_H
#define OBJECTTRACKER_H

#include "Module.h"
#include "Detector.h"
#include "Tracker.h"

class ConfigReader;

class ObjectTrackerParameter : public ModuleParameterStructure
{
public:
	ObjectTrackerParameter(ConfigReader& x_confReader, const std::string& x_moduleName) : ModuleParameterStructure(x_confReader, x_moduleName)
	{
		m_list.push_back(new ParameterT<int>(0, "input_blur_size",	1, 	PARAM_INT, 1, 7,	&detector.inputBlurSize));
		m_list.push_back(new ParameterT<float>(0, "background_alpha",	0.02, 	PARAM_FLOAT, 0, 1,	&detector.backgroundAlpha));
		m_list.push_back(new ParameterT<float>(0, "foreground_thres", 	0.2, 	PARAM_FLOAT, 0, 1,	&detector.foregroundThres));
		m_list.push_back(new ParameterT<int>(0, "foreground_filter_size", 3, 	PARAM_INT, 1, 7,	&detector.foregroundFilterSize));

		ParameterStructure::Init();
	};
	DetectorParameter detector;
	TrackerParameter tracker;
};

class ObjectTracker : public Module
{
public:
	ObjectTracker(const std::string& x_name, ConfigReader& x_configReader);
	~ObjectTracker(void);
	
	virtual void ProcessFrame(const IplImage* m_img);
	
private:
	ObjectTrackerParameter m_param;
	IplImage* m_img_blur;
	Detector detect;
	Tracker track;
	static const char * m_type;
protected:
	virtual ModuleParameterStructure& GetRefParameter(){ return m_param;};
};


#endif