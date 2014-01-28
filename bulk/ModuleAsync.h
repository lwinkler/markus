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

#include "Module.h"
#include <QThread>

/// This class represents a parent module that can process data asynchronously (in a separate thread)

class ModuleAsyncParameterStructure : public ModuleParameterStructure
{
public:
	ModuleAsyncParameterStructure(const ConfigReader& x_confReader) : ModuleParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterDouble("detection_fps", 	10, 	0, 	1000,		&detectionFps,	"Frames per seconds for the asynchronous detection (normally smaller that module)"));
		Init();
	};

public:
	double detectionFps;
};

class ModuleAsync : public Module
{
public:
	ModuleAsync(const ConfigReader& x_confReader);
	virtual ~ModuleAsync();
	
	virtual void ProcessFrame();
	
protected:
	TIME_STAMP m_timeStampLastThread;
	bool m_resultsCopied;
	
	long long m_timerThread;
	long long m_countFramesThread;

	// For an async module, this method is used to check if we need to process the input
	inline virtual bool IsInputProcessed() {return (m_currentTimeStamp - m_timeStampLastThread) * RefParameter().detectionFps >= 1000;};
	virtual const QThread & GetRefThread() = 0;
	virtual void LaunchThread() = 0;
	virtual void NormalProcess() = 0;
	virtual void CopyResults() = 0;
	virtual void PrintStatistics() const;
	
private:
	virtual ModuleAsyncParameterStructure & RefParameter() = 0;
};

#endif
