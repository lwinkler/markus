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

#ifndef BGRSUBRUNAVG_H
#define BGRSUBRUNAVG_H

#include "Module.h"


/**
* @brief Perform a background subtraction using a running average
*/
class BgrSubRunAvg : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		Parameters(const std::string& x_name) : Module::Parameters(x_name)
		{
			AddParameter(new ParameterFloat("backgroundAlpha",	0.02, 	0, 1,	&backgroundAlpha,	"Defines the speed at which the background adapts"));
			AddParameter(new ParameterFloat("foregroundThres", 	0.2, 	0, 1,	&foregroundThres,	"Threshold to accept a pixel as foreground"));

			RefParameterByName("type").SetDefaultAndValue("CV_32FC3");
			RefParameterByName("type").SetRange("[CV_8UC1,CV_8UC3,CV_32FC1,CV_32FC3]");
		};
		float backgroundAlpha;
		float foregroundThres;
	};

	BgrSubRunAvg(ParameterStructure& xr_params);
	virtual ~BgrSubRunAvg();
	MKCLASS("BgrSubRunAvg")
	MKCATEG("BackgroundSubtraction")
	MKDESCR("Perform a background subtraction using a running average")

	void ProcessFrame() override;
	void Reset() override;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	// input
	cv::Mat m_input;

	// output
	cv::Mat m_background;
	cv::Mat m_foreground;

	// temporary
	cv::Mat m_foreground_tmp;

	// state variables
	bool m_emptyBackgroundSubtractor;
	cv::Mat m_accumulator;
};


#endif
