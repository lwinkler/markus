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

#include "MotionDetector.h"
#include "StreamImage.h"
#include "StreamDebug.h"
#include "StreamState.h"
#include "StreamEvent.h"
#include "StreamNum.h"

#include <opencv2/highgui/highgui.hpp>


using namespace std;
using namespace cv;


const Scalar MotionDetector::m_colorPlotBack  = cv::Scalar(255, 255, 240);
const Scalar MotionDetector::m_colorPlotValue = cv::Scalar(10, 255, 30);
const Scalar MotionDetector::m_colorPlotThres = cv::Scalar(0, 0, 0);

log4cxx::LoggerPtr MotionDetector::m_logger(log4cxx::Logger::getLogger("MotionDetector"));

MotionDetector::MotionDetector(ParameterStructure& xr_params)
	: Module(xr_params), m_param(dynamic_cast<Parameters&>(xr_params)),
	  m_input(Size(m_param.width, m_param.height), m_param.type),
	  m_value(0)
{
	// Init output images
	AddInputStream(0, new StreamImage("input", m_input, *this, 	"Video input"));
	AddOutputStream(0, new StreamState("motion", m_state,  *this, 	"Motion is detected"));
	AddOutputStream(1, new StreamEvent("motion", m_event,  *this, 	"Motion is detected"));
	mp_streamValues = new StreamNum<double>("value", m_value,  *this,      "Scalar representing the motion level");
	AddOutputStream(2, mp_streamValues);

#ifdef MARKUS_DEBUG_STREAMS
	m_debug = Mat(Size(m_param.width, m_param.height), CV_8UC3);
	AddDebugStream(0, new StreamDebug("debug", m_debug, *this, 	"Motion percentage"));
#endif
}

MotionDetector::~MotionDetector(void)
{
}

void MotionDetector::Reset()
{
	Module::Reset();
	m_state = false;
#ifdef MARKUS_DEBUG_STREAMS
	m_debug.setTo(m_colorPlotBack);
#endif
}

void MotionDetector::ProcessFrame()
{
	vector<Mat> channels;
	split(m_input, channels);
	m_value = 0;

	m_event.Clean();

	for(int i = 0 ; i < m_input.channels() ; i++)
	{
		Scalar m = mean(channels[i]);
		m /= 255.0;
		m_value += sum(m)[i];
	}
	m_value /= m_input.channels();
	bool oldState = m_state;
	m_state = (m_value >= m_param.motionThres);
	if(m_state == true && oldState == false)
		m_event.Raise("motion");
	
	LOG_DEBUG(m_logger, "Motion state " << m_state);
	

#ifdef MARKUS_DEBUG_STREAMS
	mp_streamValues->Store();

	// Debug image: moving plot displaying threshold and current value
	// Mat col(m_debug->rows, 1, CV_8UC3);
	Mat col = m_debug.col(m_debug.cols - 1); // (*m_debug)(Rect(m_debug->cols - 1 / 2, 0, 1, m_debug->rows));
	col.setTo(m_colorPlotBack);
	line(col, Point(0, m_debug.rows * (1 - m_value / 3 / m_param.motionThres)), Point(0, m_debug.cols - 1), m_colorPlotValue);
	circle(col, Point(0, m_debug.rows * (1 - 0.333)),                         0, m_colorPlotThres, -1);

	// m_debug->row(m_debug->rows -1) = col.row(0);


	Rect roi;
	roi.x = 1;
	roi.y = 0;
	roi.width  = m_debug.cols - 1;
	roi.height = m_debug.rows;

	// Mat crop;
	Mat crop = (m_debug)(roi);

	// Move the left boundary to the right
	//*m_debug = Scalar::all(0);
	m_debug.adjustROI(0, 0, 0, -1);
	crop.copyTo(m_debug); // TODO: warning valgrind : Source and destination overlap in memcpy. Is this fixed ?
	m_debug.adjustROI(0, 0, 0, 1);
#endif
}

