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

#include "VideoOutput.h"
#include "StreamDebug.h"
#include "StreamObject.h"
#include "StreamImage.h"

//#include "util.h"

using namespace cv;

const char * VideoOutput::m_type = "VideoOutput";


VideoOutput::VideoOutput(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader)
{
	m_description = "Output video stream with additional object streams.";
	m_imageInput = new Mat(cvSize(m_param.width, m_param.height), m_param.type);
	m_imageOutput = new Mat(cvSize(m_param.width, m_param.height), m_param.type);
	
	m_inputStreams.push_back(new StreamImage(0, "input", m_imageInput, *this,	"Input video stream"));
	m_inputStreams.push_back(new StreamObject(1, "input", m_param.width, m_param.height, m_objectInput1, *this,	"Object stream 1"));
	m_inputStreams.push_back(new StreamObject(2, "input", m_param.width, m_param.height, m_objectInput2, *this,	"Object stream 1"));
	m_inputStreams.push_back(new StreamObject(3, "input", m_param.width, m_param.height, m_objectInput3, *this,	"Object stream 1"));
	m_inputStreams.push_back(new StreamObject(4, "input", m_param.width, m_param.height, m_objectInput4, *this,	"Object stream 1"));
	m_inputStreams.push_back(new StreamObject(5, "input", m_param.width, m_param.height, m_objectInput5, *this,	"Object stream 1"));
	
	m_outputStreams.push_back(new StreamImage(0, "output", m_imageOutput, *this,	"Output video stream"));
}

VideoOutput::~VideoOutput(void )
{
	delete(m_imageInput);
	delete(m_imageOutput);
}


void VideoOutput::Reset()
{
	Module::Reset();
}

void VideoOutput::ProcessFrame()
{
	for(int i = 0 ; i < 5 ; i++)
		m_inputStreams[i]->RenderTo(m_imageOutput);
}
