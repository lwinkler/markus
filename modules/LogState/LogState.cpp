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

#include "LogState.h"
#include "StreamState.h"
#include "util.h"
#include "Manager.h"

using namespace std;
using namespace cv;

log4cxx::LoggerPtr LogState::m_logger(log4cxx::Logger::getLogger("LogState"));

LogState::LogState(ParameterStructure& xr_params)
	: Module(xr_params), m_param(dynamic_cast<Parameters&>(xr_params))
{
	// Init input images
	AddInputStream(0, new StreamState("input", m_state, *this, 	"Input state to be logged"));
}

LogState::~LogState()
{
	WriteState();
	// delete(m_input);
}

void LogState::Reset()
{
	if(m_file.is_open())
		WriteState();
	Module::Reset();
	m_state = m_oldState = 0;
	m_subId = 0;
	m_startTime = "00:00:00,000";

	// write time stamp in log filename
	/*time_t now = time(0);
	struct tm tm_struct;
	assert(gmtime(&tm_struct, &now) == 0);
	m_srtFileName = "log." + gmtime(&tm_struct, &now) + ".srt";
	*/

	m_file.close();
	m_file.open(RefContext().ReserveFile(m_param.file, m_nbReset), std::ios_base::app);
	if(! m_file.is_open())
		throw MkException("Impossible to open file " + m_srtFileName, LOC);
}

void LogState::ProcessFrame()
{
	if(m_state != m_oldState)
	{
		LOG_DEBUG(m_logger, "state change");
		// Log the change in state
		WriteState();
	}
}

/// Write the subtitle in log file
void LogState::WriteState()
{
	m_endTime = msToTimeStamp(m_currentTimeStamp);

	if(m_oldState)
	{
		m_file<<m_subId<<endl;
		m_file<<m_startTime<<" --> "<<m_endTime<<endl;
		m_file<<"state_"<<m_oldState<<endl<<endl;
		m_subId++;
	}
	m_startTime = m_endTime;
	m_oldState = m_state;
}
