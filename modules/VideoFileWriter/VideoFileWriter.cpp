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

using namespace std;
using namespace cv;

VideoFileWriter::VideoFileWriter(const ConfigReader& x_configReader): 
	Module(x_configReader),
	m_param(x_configReader)
{
	m_description = "Write output to a video file.";

	m_input    = new Mat(cvSize(m_param.width, m_param.height), m_param.type);

	m_inputStreams.push_back(new StreamImage(0, "input",             m_input, *this,   "Video input"));
	/*
	CV_FOURCC('P','I','M','1') = MPEG-1 codec
	CV_FOURCC('M','J','P','G') = motion-jpeg codec (does not work well)
	CV_FOURCC('M','P','4','2') = MPEG-4.2 codec
	CV_FOURCC('D','I','V','3') = MPEG-4.3 codec
	CV_FOURCC('D','I','V','X') = MPEG-4 codec
	CV_FOURCC('U','2','6','3') = H263 codec
	CV_FOURCC('I','2','6','3') = H263I codec
	CV_FOURCC('F','L','V','1') = FLV1 codec 
	*/
}

VideoFileWriter::~VideoFileWriter()
{
}

void VideoFileWriter::Reset()
{
	Module::Reset();
	m_writer.release();
	m_writer.open(m_param.file, CV_FOURCC('M','J','P','G'), m_param.fps, Size(m_param.width, m_param.height), true); // TODO: compute last param. Iscolor
}

void VideoFileWriter::ProcessFrame()
{
	cout << "write frame " << m_input->cols << "x" << m_input->rows << endl;
	m_writer << *m_input;
}

