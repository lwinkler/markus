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

#include "TempDiff.h"
#include "StreamImage.h"

// for debug
#include "util.h"
// #include <opencv2/highgui/highgui.hpp>

namespace mk {
using namespace cv;

log4cxx::LoggerPtr TempDiff::m_logger(log4cxx::Logger::getLogger("TempDiff"));

TempDiff::TempDiff(ParameterStructure& xr_params) :
	Module(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params)),
	m_input(Size(m_param.width, m_param.height), m_param.type),
	m_temporalDiff(Size(m_param.width, m_param.height), CV_8UC1),
	// m_output(Size(m_param.width, m_param.height), m_param.type),
	m_lastImg(Size(m_param.width, m_param.height), m_param.type)
{
	AddInputStream(0, new StreamImage("image", m_input, *this,             "Video input"));
	AddOutputStream(0, new StreamImage("tempDiff", m_temporalDiff, *this, "Temporal difference"));
}

void TempDiff::Reset()
{
	Module::Reset();
	m_tmp = Mat(Size(m_input.cols, m_input.rows), m_input.depth(), m_input.channels());
	m_emptyTemporalDiff = true;
}

void TempDiff::ProcessFrame()
{
	// Main part of the program
	if(m_emptyTemporalDiff)
	{
		m_input.copyTo(m_lastImg);
		m_emptyTemporalDiff = false;
	}
	else
	{
		subtract(m_input, m_lastImg, m_tmp);
		absdiff(m_input,  m_lastImg, m_tmp);
		adjustChannels(m_tmp, m_temporalDiff);

		m_input.copyTo(m_lastImg);
	}
};

} // namespace mk
