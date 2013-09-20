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

#include "Morph.h"
#include "StreamDebug.h"
#include "StreamImage.h"

// for debug
#include "util.h"
#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

const char * Morph::m_type = "Morph";


Morph::Morph(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader)
{
	m_description = "Apply a morphological operator to an image";
	m_output   = new Mat(cvSize(m_param.width, m_param.height), m_param.type);
	m_input    = new Mat(cvSize(m_param.width, m_param.height), m_param.type);

	
	m_inputStreams.push_back(new StreamImage( 0, "input" , m_input , *this, "Video input"));

	m_outputStreams.push_back(new StreamImage(0, "output", m_output, *this,	"Output"));
	
	Reset();
}

Morph::~Morph(void )
{
	delete(m_input);
	delete(m_output);
}

void Morph::Reset()
{
	Module::Reset();
	m_element = getStructuringElement(m_param.element, Size(m_param.kernelSize, m_param.kernelSize), Point(m_param.kernelSize / 2 + 1, m_param.kernelSize / 2 + 1));
}

void Morph::ProcessFrame()
{
	morphologyEx(*m_input, *m_output, m_param.oper, m_element);
};