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

#ifndef TEMPORALDIFFERENCING_H
#define TEMPORALDIFFERENCING_H

#include "Module.h"

class ConfigReader;


class TemporalDifferencingParameterStructure : public ModuleParameterStructure
{
public:
	TemporalDifferencingParameterStructure(const ConfigReader& x_confReader) : ModuleParameterStructure(x_confReader)
	{
		// m_list.push_back(new ParameterFloat(0, "background_alpha",	0.02, 	PARAM_FLOAT, 0, 1,	&backgroundAlpha,	"Defines the speed at which the background adapts"));
		// RefParameterByName("type").SetDefault("CV_32FC3");

		ParameterStructure::Init();
	};
	//float backgroundAlpha;
};

class TemporalDifferencing : public Module
{
public:
	TemporalDifferencing(const ConfigReader& x_configReader);
	~TemporalDifferencing();
	
	virtual void ProcessFrame();
	void Reset();
		
private:
	TemporalDifferencingParameterStructure m_param;
	inline virtual ModuleParameterStructure& RefParameter() { return m_param;};

	// Temporal differencing
	cv::Mat* m_lastImg;
	cv::Mat* m_temporalDiff;
	bool m_emptyTemporalDiff;

protected:
	cv::Mat * m_input;
	cv::Mat * m_output;
	
	static const char * m_type;
};


#endif
