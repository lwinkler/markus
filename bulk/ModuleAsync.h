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
		m_list.push_back(new ParameterT<double>(0, "detectionFps", 	10, 	PARAM_DOUBLE, 	0, 	100,		&detectionFps,	"Frames per seconds for the asynchronous detection (normally smaller that module)"));
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
	cv::Mat * m_input;
	cv::Mat * m_lastInput; // TODO : Is this used ?
	double m_timeSinceLastThread;
	bool m_resultsCopied;

	virtual const QThread & GetRefThread() = 0;
	virtual void LaunchThread(const cv::Mat* m_input, const double x_timeSinceLastProcessing) = 0;
	virtual void NormalProcess(const cv::Mat * img, const double x_timeSinceLastProcessing) = 0;
	virtual void CopyResults() = 0;
	
private:
	virtual const ModuleAsyncParameterStructure & GetRefParameter() const = 0;
};

#endif