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

#include "Mask.h"
#include "StreamImage.h"
#include <opencv2/opencv.hpp>

// for debug
// #include "util.h"

using namespace cv;
using namespace std;

log4cxx::LoggerPtr Mask::m_logger(log4cxx::Logger::getLogger("Mask"));

Mask::Mask(ParameterStructure& xr_params) :
	Module(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params)),
	m_input(Size(m_param.width, m_param.height), m_param.type),
	m_mask(Size(m_param.width, m_param.height), CV_8UC1),
	m_output(Size(m_param.width, m_param.height), m_param.type)
{
	AddInputStream(0, new StreamImage("image",   m_output, *this, "Video input"));
	AddInputStream(1, new StreamImage("mask" ,   m_mask,   *this, "Binary mask"));

	AddOutputStream(0, new StreamImage("masked", m_output, *this, "Binary mask applied to input"));
};


Mask::~Mask()
{
}

void Mask::Reset()
{
	Module::Reset();
}

void Mask::ProcessFrame()
{
	threshold(m_mask, m_mask, 128, 255, THRESH_BINARY_INV);
	m_input.copyTo(m_output, m_mask);
	// cvAnd(m_input, m_mask, m_output);
};

