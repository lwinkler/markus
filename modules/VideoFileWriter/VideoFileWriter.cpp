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

#include "VideoFileWriter.h"
#include "StreamImage.h"
#include "util.h"
#include "Manager.h"

namespace mk {
using namespace std;
using namespace cv;

log4cxx::LoggerPtr VideoFileWriter::m_logger(log4cxx::Logger::getLogger("VideoFileWriter"));

VideoFileWriter::VideoFileWriter(ParameterStructure& xr_params):
	Module(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params)),
	m_input(Size(m_param.width, m_param.height), m_param.type)
{
	AddInputStream(0, new StreamImage("image", m_input, *this,   "Video input"));
	m_index = 0;
}

VideoFileWriter::~VideoFileWriter()
{
	// cout<<"Release FileWriter"<<endl;
	// m_writer.release();
}

void VideoFileWriter::Reset()
{
	Module::Reset();
	if(!m_writer.isOpened())
		m_writer.release();
	// m_writer.release();
	if(m_param.fourcc.size() != 4)
		throw MkException("Error in parameter: fourcc must have 4 characters in VideoFileWriter::Reset", LOC);
	const char * s = m_param.fourcc.c_str();
	// The color flag seem to be supported on Windows only
	// http://docs.opencv.org/modules/highgui/doc/reading_and_writing_images_and_video.html#videowriter-videowriter
	bool isColor = true;
	assert(m_param.type == CV_8UC3);

	stringstream ss;
	ss << m_param.file  << "." << m_index++ << "." << ExtensionFromFourcc(m_param.fourcc);
	const string filename = RefContext().RefOutputDir().ReserveFile(ss.str());
	double fps = 12;

	try
	{
		fps = GetRecordingFps();
		if(fps == 0)
			fps = 8; // default 8 fps
	}
	catch(exception& e)
	{
		// This may happen if the module is not connected
		LOG_WARN(m_logger, "Impossible to acquire the fps value for recording in VideoFileWriter::Reset. Set to default value " << fps << ". Reason: " << e.what());
	}

	LOG_DEBUG(m_logger, "Start recording file "<<filename<<" with fps="<<fps<<" and size "<<m_param.width<<"x"<<m_param.height);
	m_writer.open(filename, CV_FOURCC(s[0], s[1], s[2], s[3]), fps, Size(m_param.width, m_param.height), isColor);
	if(!m_writer.isOpened())
	{
		throw MkException("Failed to open output video file " + filename + " in VideoFileWriter::Reset", LOC);
	}
}

void VideoFileWriter::ProcessFrame()
{
	// cout << "write frame " << m_input->cols << "x" << m_input->rows << endl;
	// m_writer << *m_input;
	m_writer.write(m_input);
}


const string VideoFileWriter::ExtensionFromFourcc(const string& x_fourcc)
{
	if(x_fourcc.compare("PIM1") == 0)
		return "PIM1.mpeg";
	if(x_fourcc.compare("MJPG") == 0)
		return "MJPG.avi";
	if(x_fourcc.compare("MP42") == 0)
		return "MP42.avi";
	if(x_fourcc.compare("DIV3") == 0)
		return "DIV3.avi";
	if(x_fourcc.compare("DIVX") == 0)
		return "DIVX.avi";
	if(x_fourcc.compare("H263") == 0)
		return "h263.avi";
	if(x_fourcc.compare("I263") == 0) // note: seems not to be working
		return "I263.avi";
	if(x_fourcc.compare("FLV1") == 0)
		return "flv1.avi";

	LOG_WARN(m_logger, "Unknown fourcc code, cannot find a matching extension in VideoFileWriter::ExtensionFromFourcc");

	return "avi";
}
} // namespace mk
