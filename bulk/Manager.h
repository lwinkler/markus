#ifndef MANAGER_H
#define MANAGER_H

#include <list>
#include <cstring>

#include <cv.h>
//#include <highgui.h>
#include "timer.h"

#include "ConfigReader.h"
#include "Input.h"

class ManagerParameter : public ParameterStructure
{
public:
	ManagerParameter(ConfigReader& x_confReader, const std::string& x_moduleName) : ParameterStructure(x_confReader, "Manager", "")
	{
		m_list.push_back(new ParameterT<std::string>(0, "mode",	"", 	PARAM_STR, &mode));
		//		m_list.push_back(new ParameterT<std::string>(1, "input",	"cam", 	PARAM_STR, &input));
		m_list.push_back(new ParameterT<int>(2, "width", 	640, 	PARAM_INT, 0, 4000,	&width));
		m_list.push_back(new ParameterT<int>(3, "height", 	480, 	PARAM_INT, 0, 3000,	&height));
		m_list.push_back(new ParameterT<int>(4, "depth", IPL_DEPTH_8U, PARAM_INT, 0, 32,	&depth));
		m_list.push_back(new ParameterT<int>(5, "channels", 	3, 	PARAM_INT, 1, 3,	&channels));

		ParameterStructure::Init();
	};
	std::string mode;
	//std::string input;
	int width;
	int height;
	int depth;
	int channels;
};


class Manager : public Configurable
{
public:
	Manager(ConfigReader & x_configReader);
	~Manager();
	void CaptureInput();
	void Process();
	void AddInput(Input * x_input);
	void AddModule(Module * x_mod);
	const std::vector<Input*> & GetinputList()  const {return m_inputs; };
	const std::vector<Module*>& GetModuleList() const {return m_modules; };
private:
	ManagerParameter m_param;
	CvVideoWriter * m_writer;
	int m_key;
	flann::StartStopTimer m_timerConv;
	flann::StartStopTimer timerProc;
	
	std::vector<Module *> m_modules;
	std::vector<Input  *> m_inputs;
	long long m_frameCount;
protected:
	virtual const ParameterStructure& GetRefParameter(){return m_param;};
};
#endif