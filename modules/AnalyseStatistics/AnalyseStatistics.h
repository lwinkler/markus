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

#ifndef ANALYSE_STATISTICS_H
#define ANALYSE_STATISTICS_H

#include <cv.h>
#include "Module.h"
#include "Parameter.h"
#include "Timer.h"


/*! \class AnalyseStatistics
 *  \brief Class containing methods/attributes of a slit camera
 *
 */


class AnalyseStatisticsParameterStructure : public ModuleParameterStructure
{
	
public:
	AnalyseStatisticsParameterStructure(const ConfigReader& x_confReader) : 
		ModuleParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterFloat(0, "motion_thres", 0.1, PARAM_FLOAT, 0, 1, &motionThres,	"Threshold for motion analysis"));

		ParameterStructure::Init();
	}
	
	float motionThres;
};

class AnalyseStatistics : public Module
{
protected:
	virtual void ProcessFrame();
	AnalyseStatisticsParameterStructure m_param;
	static const char * m_type;

	cv::Mat * m_input;

	bool m_status;
	long int m_subId;
	Timer m_timer;
	std::string m_startTime;
	std::string m_srtFileName;;

public:
	AnalyseStatistics(const ConfigReader& x_configReader);
	~AnalyseStatistics(void);
	void Reset();


protected:
	inline virtual AnalyseStatisticsParameterStructure& RefParameter() { return m_param;}
};

#endif

