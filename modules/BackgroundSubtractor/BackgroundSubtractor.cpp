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

#include "BackgroundSubtractor.h"
#include "StreamImage.h"
#include "StreamDebug.h"
#include <opencv2/opencv.hpp>

namespace mk {
using namespace cv;
using namespace std;

log4cxx::LoggerPtr BackgroundSubtractor::m_logger(log4cxx::Logger::getLogger("BackgroundSubtractor"));

BackgroundSubtractor::BackgroundSubtractor(ParameterStructure& xr_params) :
	Module(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params)),
	m_input(Size(m_param.width, m_param.height), m_param.type),
	m_foreground(Size(m_param.width, m_param.height), CV_8UC1),
	m_background(Size(m_param.width, m_param.height), m_param.type),
	m_foregroundWithShadows(Size(m_param.width, m_param.height), m_param.type)
{
	AddInputStream(0, new StreamImage("image",       m_input, *this,   "Video input"));

	AddOutputStream(0, new StreamImage("foreground", m_foreground,*this,      "Foreground"));
	AddOutputStream(1, new StreamImage("background", m_background, *this,		"Background"));

#ifdef MARKUS_DEBUG_STREAMS
	AddDebugStream(0, new StreamDebug("foregroundWithShadows", m_foregroundWithShadows, *this,	"Foreground with shadows"));
#endif
};


BackgroundSubtractor::~BackgroundSubtractor()
{
}

void BackgroundSubtractor::Reset()
{
	Module::Reset();
	mp_bsub.release();
	mp_bsub = create(m_param.create);
}

void BackgroundSubtractor::ProcessFrame()
{
	mp_bsub->apply(m_input, m_foregroundWithShadows, m_param.learningRate);
	// TODO crashes on Reset. Find out why.
	// mp_bsub->getBackgroundImage(m_background);

	// Threshold shadows (value=128) to 0
	threshold(m_foregroundWithShadows, m_foreground, 254, 255, cv::THRESH_BINARY);
}

} // namespace mk
