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

using namespace std;
using namespace cv;

VideoFileWriter::VideoFileWriter(const ConfigReader& x_configReader): 
	Module(x_configReader),
	m_param(x_configReader)
{
	m_description = "Write output to a video file.";

	m_input    = new Mat(cvSize(m_param.width, m_param.height), m_param.type);

	m_inputStreams.push_back(new StreamImage(0, "input",             m_input, *this,   "Video input"));
}

VideoFileWriter::~VideoFileWriter()
{
	// cout<<"Release FileWriter"<<endl;
	// m_writer.release();
}

void VideoFileWriter::Reset()
{
	Module::Reset();
	// m_writer.release();
	if(m_param.fourcc.size() != 4)
		throw("Error in parameter: fourcc must have 4 characters in VideoFileWriter::Reset");
	const char * s = m_param.fourcc.c_str();
	bool isColor = false;
	if(m_param.type == CV_8UC3) // TODO : find a way to restrain parameters values
		isColor = true;
	else if(m_param.type == CV_8UC1)
		isColor = false;
	else assert(false);

	const string filename = m_param.file  + (m_param.timeStamp ? ("." + timeStamp()) : ".") + "." + ExtensionFromFourcc(m_param.fourcc);

cout<<"Opening "<<filename<<endl;
	// TODO: Manage fps for stream recording
	m_writer.open(filename, CV_FOURCC(s[0], s[1], s[2], s[3]), /*m_param.fps > 0 ? m_param.fps :*/ 12, Size(m_param.width, m_param.height), isColor); // TODO: compute last param. Iscolor
	if(!m_writer.isOpened())
	{
		cout<<"Failed to open output video file in VideoFileWriter::Reset"<<endl;
		throw("Failed to open output video file in VideoFileWriter::Reset");
	}
}

void VideoFileWriter::ProcessFrame()
{
	cout << "write frame " << m_input->cols << "x" << m_input->rows << endl;
	// m_writer << *m_input;
	m_writer.write(*m_input);
}


const string VideoFileWriter::ExtensionFromFourcc(const string& x_fourcc) 
{
	if(x_fourcc.compare("PIM1") == 0)
		return "PIM1.mpeg";
	if(x_fourcc.compare("MJPG") == 0)
		return "mjpg";
	if(x_fourcc.compare("MP42") == 0)
		return "MP42.avi";
	if(x_fourcc.compare("DIV3") == 0)
		return "DIV3.avi";
	if(x_fourcc.compare("DIVX") == 0)
		return "DIVX.avi";
	if(x_fourcc.compare("H263") == 0)
		return "h263";
	if(x_fourcc.compare("I263") == 0) // TODO: not working ?
		return "I263.avi";
	if(x_fourcc.compare("FLV1") == 0) 
		return "flv1.avi";
	
	cout<<"WARNING: Unknown fourcc code, cannot find a matching extension in VideoFileWriter::ExtensionFromFourcc"<<endl;
	return "avi";
}
