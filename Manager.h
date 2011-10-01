#ifndef MANAGER_H
#define MANAGER_H

#include <list>
#include <cstring>

#include <cv.h>
#include <highgui.h>
#include "timer.h"

#include "SlitCam.h"
#include "Detector.h"

#include "ConfigReader.h"

class Manager
{
	public:
	Manager(ConfigReader & m_configReader);
	~Manager();
	void ReadConfig(const char *, Detector& detect);
	void CaptureInput();
	void Process();
	void AddModule(Module & x_mod);
	
	std::string m_mode;
	std::string m_input;
	
	private:
	ConfigReader& m_configReader;
	CvCapture * m_capture;
	CvVideoWriter * m_writer;
	int m_key;
	flann::StartStopTimer m_timerConv;
	flann::StartStopTimer timerProc;
	
	std::list<Module *> m_modules;

	int m_workWidth;//384;//640;
	int m_workHeight;//288;//320;
	int m_workDepth;//IPL_DEPTH_8U IPL_DEPTH_32F;
	int m_workChannels;
	bool m_workIsColor;
};
#endif