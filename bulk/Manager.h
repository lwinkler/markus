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

#ifndef MANAGER_H
#define MANAGER_H

#include <list>
#include <cstring>

#include <cv.h>
#include "timer.h"

#include "ConfigReader.h"
//#include "ConfigReader.h"
#include "Input.h"
#include "Module.h"

class ImageProcessor;
//class Input;
//class Module;
class CvVideoWriter;

class ManagerParameter : public ParameterStructure
{
public:
	ManagerParameter(ConfigReader& x_confReader, const std::string& x_moduleName) : ParameterStructure(x_confReader, "Manager", "")
	{
		m_list.push_back(new ParameterT<std::string>(0, "mode",	"", 	PARAM_STR, &mode));
		//		m_list.push_back(new ParameterT<std::string>(1, "input",	"cam", 	PARAM_STR, &input));
	/*	m_list.push_back(new ParameterT<int>(2, "width", 	640, 	PARAM_INT, 0, 4000,	&width));
		m_list.push_back(new ParameterT<int>(3, "height", 	480, 	PARAM_INT, 0, 3000,	&height));
		m_list.push_back(new ParameterT<int>(4, "depth", IPL_DEPTH_8U, PARAM_INT, 0, 32,	&depth));
		m_list.push_back(new ParameterT<int>(5, "channels", 	3, 	PARAM_INT, 1, 3,	&channels));
*/
		ParameterStructure::Init();
	};
	std::string mode;
	//std::string input;
	//int width;
	//int height;
	//int depth;
	//int channels;
};


class Manager : public Configurable
{
public:
	Manager(ConfigReader & x_configReader);
	~Manager();
	//void CaptureInput();
	void Process();
	//void AddInput(Input * x_input);
	//void AddModule(Module * x_mod);
	const std::vector<Input*> & GetInputList()  const {return m_inputs; };
	const std::vector<Module*>& GetModuleList() const {return m_modules; };

 	std::vector<Input*> & GetInputListVar()  {return m_inputs; };
	std::vector<Module*>& GetModuleListVar() {return m_modules; };
private:
	ManagerParameter m_param;
	CvVideoWriter * m_writer;

	flann::StartStopTimer m_timerConv;
	flann::StartStopTimer m_timerProc;

	//flann::StartStopTimer 
	clock_t m_timerLastProcess;
	
	std::vector<Module *> 		m_modules;
	std::vector<Input  *> 		m_inputs;
	std::vector<ImageProcessor *> 	m_imageProcessors;
	long long m_frameCount;
protected:
	inline virtual const ParameterStructure& GetRefParameter() const{return m_param;};
};
#endif