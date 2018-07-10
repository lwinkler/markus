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

#include "Parameter.h"
#include "Module.h"

namespace mk {
/// Class representing an module used for input (camera, video file, network stream, ...)
class BackgroundSubtraction : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : Module::Parameters(x_name)
		{
			AddParameter(new ParameterDouble( "onlineLearnTime",            0.0,       0, 600,    &onlineLearnTime,            "Learn the background on the first seconds of the video"));
			AddParameter(new ParameterDouble( "onlineLearnRatio",           5.0,       1, 100,    &onlineLearnRatio,           "Increase the learning rate by this ratio at initialization"));
		}
		double onlineLearnTime;
		double onlineLearnRatio;
	};

	explicit BackgroundSubtraction(ParameterStructure& xr_params);
	~BackgroundSubtraction() override;

	virtual void StartOnlineLearning(bool x_cutOutput);

protected:
	void Reset() override;

	int  m_onlineLearnTimeMs;
	bool m_cutOutput;

private:
	Parameters& m_param;
};

} // namespace mk
#endif
