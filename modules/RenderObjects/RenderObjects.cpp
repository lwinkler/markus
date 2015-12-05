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
#include "MultipleStreamT.h"
#include "StreamObject.h"
#include "StreamImage.h"

using namespace cv;

log4cxx::LoggerPtr RenderObjects::m_logger(log4cxx::Logger::getLogger("RenderObjects"));

RenderObjects::RenderObjects(ParameterStructure& xr_params) :
	Module(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params)),
	m_imageInput(Size(m_param.width, m_param.height), m_param.type),
	m_imageOutput(Size(m_param.width, m_param.height), m_param.type)
{
	AddInputStream(0, new StreamImage( "image", m_imageInput, *this,	"Input video stream"));
	m_objectInputs.resize(1);
	AddInputStream(1, new MultipleStreamT<vector<Object>>("objects0", m_objectInputs, *this,	"Object stream"));

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
		/*
	for(auto& objects : m_objectInputs)
	{
		for(auto& obj : objects)
		{
			// cout <<  input.first << endl;
			obj.RenderTo(m_imageOutput, Scalar(i * 30,100 - i * 30,0));
		}
		i++;
	}
		*/
}
