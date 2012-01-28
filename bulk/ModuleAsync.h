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

#include <cv.h>

#include "Module.h"
#include <QThread>

/// This class represents a parent module that can process data asynchronously (in a separate thread)

class ModuleAsyncParameterStructure : public ModuleParameterStructure
{
public:
	ModuleAsyncParameterStructure(const ConfigReader& x_confReader) : ModuleParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterDouble(0, "detectionFps", 	10, 	PARAM_DOUBLE, 	0, 	100,		&detectionFps,	"Frames per seconds for the asynchronous detection (normally smaller that module)"));
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
	double m_timeSinceLastThread;
	bool m_resultsCopied;
	
	long long m_timerThread;
	long long m_countFramesThread;

	inline virtual bool IsInputUsed(double x_timeCount) const {return m_timeSinceLastThread + m_processingTime >= 1 / GetRefParameter().detectionFps;};
	virtual const QThread & GetRefThread() = 0;
	virtual void LaunchThread() = 0;
	virtual void NormalProcess() = 0;
	virtual void CopyResults() = 0;
	virtual void PrintStatistics(std::ostream& os) const;
	
private:
	virtual const ModuleAsyncParameterStructure & GetRefParameter() const = 0;
};

#endif