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

#include "RenderObjects.h"
#include "StreamObject.h"
#include "StreamImage.h"

using namespace cv;

log4cxx::LoggerPtr RenderObjects::m_logger(log4cxx::Logger::getLogger("RenderObjects"));

RenderObjects::RenderObjects(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader),
	m_imageInput(Size(m_param.width, m_param.height), m_param.type),
	m_imageOutput(Size(m_param.width, m_param.height), m_param.type)
{
	AddInputStream(0, new StreamImage( "input", m_imageInput, *this,	"Input video stream"));
	AddInputStream(1, new StreamObject("input", m_objectInput1, *this,	"Object stream 1"));
	AddInputStream(2, new StreamObject("input", m_objectInput2, *this,	"Object stream 1"));
	AddInputStream(3, new StreamObject("input", m_objectInput3, *this,	"Object stream 1"));
	AddInputStream(4, new StreamObject("input", m_objectInput4, *this,	"Object stream 1"));
	AddInputStream(5, new StreamObject("input", m_objectInput5, *this,	"Object stream 1"));
	
	AddOutputStream(0, new StreamImage("output", m_imageOutput, *this,	"Output video stream"));
}

RenderObjects::~RenderObjects(void )
{
}


void RenderObjects::Reset()
{
	Module::Reset();
}

void RenderObjects::ProcessFrame()
{
	for(int i = 0 ; i < 5 ; i++)
		m_inputStreams[i]->RenderTo(m_imageOutput);
}
