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

#ifndef MODULE_ASYNC_H
#define MODULE_ASYNC_H

#include "cv.h"
#include "Module.h"

enum ThreadStatus
{
	ThreadStatusRunning,
	ThreadStatusStopped,
	ThreadStatusWaiting
};

class ModuleAsyncParameterStructure : public ModuleParameterStructure
{
public:
	ModuleAsyncParameterStructure(ConfigReader& x_confReader, const std::string& x_moduleName) : ModuleParameterStructure(x_confReader, x_moduleName)
	{
		/*
		m_list.push_back(new ParameterT<std::string>(0, "class", 		"", 	PARAM_STR, 			&objClass));
		m_list.push_back(new ParameterT<int>(0, "width", 		640, 	PARAM_INT, 	0, 	4000,	&width));
		m_list.push_back(new ParameterT<int>(0, "height", 	480, 	PARAM_INT, 	0, 	3000,		&height));
		m_list.push_back(new ParameterT<int>(0, "depth", 	IPL_DEPTH_8U, PARAM_INT, 	0, 	32,	&depth));
		m_list.push_back(new ParameterT<int>(0, "channels", 	3, 	PARAM_INT, 	1, 	3,		&channels));
		m_list.push_back(new ParameterT<double>(0, "fps", 	10, 	PARAM_DOUBLE, 	0, 	100,		&fps));
		*/		
	};

public:
};

class ModuleAsync : public Module
{
public:
	ModuleAsync(const std::string& x_name, ConfigReader& x_confReader);
	virtual ~ModuleAsync();
	
	virtual void ProcessFrame(const IplImage * m_input, const double x_timeSinceLastProcessing);
	//const std::string& GetName(){return m_name;};
	//const IplImage * GetOutput(){return m_output;}
	
	inline int GetWidth() const {return GetRefParameter().width;}
	inline int GetHeight() const {return GetRefParameter().height;}
	inline int GetDepth() const {return GetRefParameter().depth;}
	inline int GetNbChannels() const {return GetRefParameter().channels;}
	inline int GetFps() const {return GetRefParameter().fps;}
		
protected:
//	IplImage * m_output;
//	const std::string m_name;
//	std::vector<OutputStream *> m_outputStreams;
	ThreadStatus m_threadStatus;
	virtual const ModuleAsyncParameterStructure & GetRefParameter() const = 0;
	
	virtual void ThreadProcess(const IplImage* m_input, const double x_timeSinceLastProcessing) = 0;
	virtual void PreProcess(const IplImage * img, const double x_timeSinceLastProcessing) = 0;
	virtual void PostProcess(const IplImage * img, const double x_timeSinceLastProcessing) = 0;
};

#endif