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
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  <See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with Markus.  If not, see <http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------------------*/

#ifndef MODULE_H
#define MODULE_H

#include "cv.h"
#include "ConfigReader.h"
#include "Parameter.h"
#include <QReadWriteLock>

#define MAX_WIDTH  6400
#define MAX_HEIGHT 4800

class Stream;
class Control;
class QModuleTimer;

class ModuleParameterStructure : public ParameterStructure
{
public:
	ModuleParameterStructure(const ConfigReader& x_confReader) : ParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterString(0, "class", 		"", 					&objClass,	"Class of the module (define the module's function)"));
		Parameter * param = new ParameterInt(0, "width", 	640, 	PARAM_INT, 	1, MAX_WIDTH,		&width,		"Width of the input");
		m_list.push_back(param);
		m_list.push_back(new ParameterInt(0, "height", 	480, 	PARAM_INT, 	1, MAX_HEIGHT,		&height,	"Height of the input"));
		m_list.push_back(new ParameterImageType(0, "type", 	CV_8UC1, 				&type,		"Format of the input image"));
		m_list.push_back(new ParameterDouble(0, "fps", 	10, 	PARAM_DOUBLE, 	0, 	1000,		&fps,		"Frames per seconds (processing speed)")); //TODO : Check that min max works
	}

public:
	int width;
	int height;
	int type;
	double fps;
	std::string objClass;
};

class Module : Configurable
{
public:
	Module(const ConfigReader& x_confReader);
	virtual ~Module();
	
	virtual void Reset() = 0;
        void Process(double x_timeCount);

	const std::string& GetName() const{return m_name;}
	const std::string& GetDescription() const{return m_description;}
	int GetId() const {return m_id;}

	const std::vector<Stream*>& GetInputStreamList() const {return m_inputStreams;}
	const std::vector<Stream*>& GetOutputStreamList() const {return m_outputStreams;}
	const std::vector<Stream*>& GetDebugStreamList() const {return m_debugStreams;}
	const std::vector<Control*>& GetControlList() const {return m_controls;}
	virtual ModuleParameterStructure & RefParameter() = 0;
	
	inline int GetWidth() {return RefParameter().width;}
	inline int GetHeight(){return RefParameter().height;}
	inline int GetType()  {return RefParameter().type;}
	inline int GetFps() {return RefParameter().fps;}
	
	inline void SetPreceedingModule(Module & x_module){m_modulePreceeding = &x_module;}
	inline void AddFollowingModule (Module & x_module){m_modulesFollowing.push_back(&x_module);}
	virtual void PrintStatistics(std::ostream& os) const;
	
	virtual inline bool IsInput() {return false;}
	void Export(std::ostream& rx_os, int x_indentation);
	Stream * GetInputStreamById(int x_id) const;
	Stream * GetOutputStreamById(int x_id) const;
	QReadWriteLock m_lock;
	
protected:
	long long m_timerConvertion;
	long long m_timerProcessing;
	long long m_timerWaiting;
	long long m_countProcessedFrames;
	
	virtual void ProcessFrame() = 0;
	inline virtual bool IsInputUsed(double x_timeCount) const {return true;}
	std::vector<Stream *> m_inputStreams;
	std::vector<Stream *> m_outputStreams;
	std::vector<Stream *> m_debugStreams;	
	std::vector<Control *> m_controls;	
	std::string m_name;
	std::string m_description; 
	int m_id;
	double m_processingTime;
	Module * m_modulePreceeding;
	std::vector<Module *> m_modulesFollowing;
	QModuleTimer * m_moduleTimer;
};

#endif
