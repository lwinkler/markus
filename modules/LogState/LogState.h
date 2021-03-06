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

#ifndef LOG_STATE_H
#define LOG_STATE_H

#include "Module.h"
#include "Parameter.h"
#include "Timer.h"
#include <fstream>



namespace mk {
/**
* @brief Read a state stream and log it to .srt file
*/
class LogState : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) :
			Module::Parameters(x_name)
		{
			AddParameter(new ParameterString("file", "state.%d.srt", &file, "Name of the .srt file with extension"));
		}
		std::string file;
	};

	explicit LogState(ParameterStructure& xr_params);
	~LogState() override;
	MKCLASS("LogState")
	MKCATEG("Output")
	MKDESCR("Read a state stream and log it to .srt file")

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	void Reset() override;
	void ProcessFrame() override;
	void WriteState();

	// input
	bool m_state;

	// state
	bool m_oldState;
	int m_subId;
	std::string m_startTime;
	std::string m_endTime;
	std::string m_srtFileName;
	std::ofstream m_file;
};

} // namespace mk
#endif

