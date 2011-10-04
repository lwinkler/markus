#ifndef MANAGER_H
#define MANAGER_H

#include <list>
#include <cstring>

#include <cv.h>
#include <highgui.h>
#include "timer.h"

#include "ConfigReader.h"

class ManagerParameter : public ParameterStructure
{
public:
	ManagerParameter(ConfigReader& x_confReader, const std::string& x_moduleName) : ParameterStructure(x_confReader, x_moduleName)
	{
		m_list.push_back(Parameter(0, "mode",	"", 	PARAM_STR,		&mode));
		m_list.push_back(Parameter(1, "input",	"cam", 	PARAM_STR,		&input));
		m_list.push_back(Parameter(2, "width", 	640, 	PARAM_INT, 0, 4000,	&width));
		m_list.push_back(Parameter(3, "height", 	480, 	PARAM_INT, 0, 3000,	&height));
		m_list.push_back(Parameter(4, "depth", IPL_DEPTH_8U, PARAM_INT, 0, 32,	&depth));
		m_list.push_back(Parameter(5, "channels", 	3, 	PARAM_INT, 1, 3,	&channels));
	    
		ParameterStructure::Init();

	};
	std::string mode;
	std::string input;
	int width;//384;//640;
	int height;//288;//320;
	int depth;//IPL_DEPTH_8U IPL_DEPTH_32F;
	int channels;
//	bool workIsColor;
	
	//void Init(){};
};


class Manager
{
public:
	Manager(ConfigReader & x_configReader);
	~Manager();
	void CaptureInput();
	void Process();
	void AddModule(Module & x_mod);
	void Init();
	
private:
	ConfigReader& m_configReader;
	ManagerParameter m_param;
	CvCapture * m_capture;
	CvVideoWriter * m_writer;
	int m_key;
	flann::StartStopTimer m_timerConv;
	flann::StartStopTimer timerProc;
	
	std::list<Module *> m_modules;
};
#endif