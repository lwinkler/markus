/*----------------------------------------------------------------------------------
*
*    MARKUS : a manager for video analysis modules
*
*    author : Loïc Monney <loic.monney@hefr.ch>
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

#include "CompareVideo.h"

#include "StreamImage.h"
#include "StreamDebug.h"
#include "MkException.h"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

log4cxx::LoggerPtr CompareVideo::m_logger(log4cxx::Logger::getLogger("CompareVideo"));

CompareVideo::CompareVideo(const ConfigReader &x_configReader) :
	Module(x_configReader),
	m_param(x_configReader),
	m_video1(Size(m_param.width, m_param.height), m_param.type),
	m_video2(Size(m_param.width, m_param.height), m_param.type)
#ifdef MARKUS_DEBUG_STREAMS
	,
	m_video1_out(Size(m_param.width, m_param.height), m_param.type),
	m_video2_out(Size(m_param.width, m_param.height), m_param.type)
#endif
{

	AddInputStream(0, new StreamImage("video1", m_video1, *this, "Video 1"));
	AddInputStream(1, new StreamImage("video2", m_video2, *this, "Video 2"));

#ifdef MARKUS_DEBUG_STREAMS
	AddDebugStream(0, new StreamImage("video1", m_video1_out, *this, "Video1"));
	AddDebugStream(1, new StreamImage("video2", m_video2_out, *this, "Video2"));
#endif
}

CompareVideo::~CompareVideo()
{
}

void CompareVideo::Reset()
{
	Module::Reset();
	m_frameCount = 0;
}

void CompareVideo::ProcessFrame()
{

	/* Check size of images */
	if(!(
				m_video1.rows > 0 &&
				m_video1.cols > 0 &&
				m_video1.rows == m_video2.rows &&
				m_video1.cols == m_video2.cols &&
				m_video1.channels() == m_video2.channels()
			))
	{
		LOG_ERROR(m_logger, "Incoming videos must have the same format");
		return;
	}

	/* Compute dissimilarity */
	Mat temp;
	absdiff(m_video1, m_video2, temp);
#ifdef MARKUS_DEBUG_STREAMS
	m_video1_out = m_video1 - m_video2;
	m_video2_out = temp;
#endif
	Scalar rgb = mean(temp); // compute a mean for each channel (from 1 to 4)
	double sum = 0;
	for(int c = 0; c < m_video1.channels(); c++)
	{
		sum += rgb[c];
	}

	/*
	 * divide by m_video1.channels(): number of channels
	 * divide by 255: divide by max value for a channel --> get a number between 0..1 (require CV_8xCx)
	 */
	double e = static_cast<double>(sum) / (m_video1.channels() * 255);
	// cout<<rgb<<" "<<sum<<" "<<e<<endl;

	/* Log */
	LOG_DEBUG(m_logger, "Frame[" << m_frameCount << "]: error = " << e << " (" << e * 100 << "%)");

	/* Exception when exceeds threshold */
	if (e > m_param.threshold)
	{
		LOG_ERROR(m_logger, "Frame[" << m_frameCount << "]: " << e << " > " << m_param.threshold << "!");
	}

	m_frameCount++;
}


