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

#ifndef INPUT_VIDEOFILEBUFFERWRITER_H
#define INPUT_VIDEOFILEBUFFERWRITER_H

#include <opencv2/highgui/highgui.hpp>
#include <list>

#include "modules/VideoFileWriter/VideoFileWriter.h"




/**
* @brief Write output to a buffer and exports it if an evenement occurs
*/
class VideoFileBufferWriter : public VideoFileWriter
{
public:
	class Parameters : public VideoFileWriter::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : 
		VideoFileWriter::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterDouble("buffer_duration", 120, 0, 600, &bufferDuration, "Length of one buffer block of video [s]"));
			m_list.push_back(new ParameterInt("nb_buffers",           0, 0,  10, &nbBuffers,      "Number of buffers blocks to keep before recording"));
			Init();
		};
		double bufferDuration;
		int nbBuffers;
	};

	VideoFileBufferWriter(const ConfigReader& x_confReader);
	MKCLASS("VideoFileBufferWriter")
	MKDESCR("Write output to a buffer and exports it if an evenement occurs")
	
	virtual void ProcessFrame();
	virtual void Reset();
	void AddImageToBuffer();
	void OpenNewFile();

private:
	Parameters m_param;
	inline virtual const Parameters& GetParameters() const {return m_param;}
	static log4cxx::LoggerPtr m_logger;

protected:
	// input
	bool m_trigger;

	// state
	bool m_buffering;

	// temporary
	std::list<cv::Mat> m_buffer;
	std::list<cv::Mat>::iterator m_currentFrame;
	TIME_STAMP m_timeBufferFull;
	bool m_bufferFull;
};

#endif
