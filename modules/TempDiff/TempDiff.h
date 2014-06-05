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

#ifndef TEMPDIFF_H
#define TEMPDIFF_H

#include "Module.h"

class ConfigReader;


/**
* @brief Perform temporal differencing: compare frame with previous frame by subtraction
*/
class TempDiff : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : Module::Parameters(x_confReader)
		{
			Init();
		};
	};

	TempDiff(const ConfigReader& x_configReader);
	~TempDiff();
	MKCLASS("TempDiff")
	MKDESCR("Perform temporal differencing: compare frame with previous frame by subtraction")
	
	inline virtual const Module::Parameters& GetParameters() const { return m_param;}
	virtual void ProcessFrame();
	void Reset();

private:
	inline virtual Parameters & RefParameters() {return m_param;}
	Parameters m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	// input
	cv::Mat m_input;

	// output
	cv::Mat m_temporalDiff;

	// state
	cv::Mat m_lastImg;
	bool m_emptyTemporalDiff;

	// temporary
	cv::Mat* m_tmp;
};


#endif
