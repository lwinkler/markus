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

#ifndef MK_BACKGROUND_SUB_H
#define MK_BACKGROUND_SUB_H

#include "ConfigReader.h"
#include "Parameter.h"
#include "Module.h"

/// Class representing an module used for input (camera, video file, network stream, ...)
class BackgroundSubtraction : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : Module::Parameters(x_confReader)
		{
			AddParameter(new ParameterDouble( "online_learn_time",            0.0,       0, 600,    &onlineLearnTime,            "Learn the background on the first seconds of the video"));
			AddParameter(new ParameterDouble( "online_learn_ratio",           5.0,       1, 100,    &onlineLearnRatio,           "Increase the learning rate by this ratio at initialization"));

			Init();
		}
		double onlineLearnTime;
		double onlineLearnRatio;
	};

	BackgroundSubtraction(ParameterStructure& xr_params);
	virtual ~BackgroundSubtraction();

	virtual void Reset() override;

	virtual void StartOnlineLearning(bool x_cutOutput);

protected:
	int  m_onlineLearnTimeMs;
	bool m_cutOutput;

private:
	Parameters& m_param;
};

#endif
