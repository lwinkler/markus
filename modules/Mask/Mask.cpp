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

// for debug
// #include "util.h"

using namespace cv;
using namespace std;


Mask::Mask(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader)
{
	m_description = "Apply a binary mask to an image input";
	m_input       = new Mat(Size(m_param.width, m_param.height), m_param.type);
	m_mask        = new Mat(Size(m_param.width, m_param.height), CV_8UC1);
	m_output      = new Mat(Size(m_param.width, m_param.height), m_param.type);

	m_inputStreams.push_back(new StreamImage(0, "input",   m_output, *this, "Video input"));
	m_inputStreams.push_back(new StreamImage(1, "mask" ,   m_mask,   *this, "Binary mask"));

	m_outputStreams.push_back(new StreamImage(0, "masked", m_output, *this, "Binary mask applied to input"));
};
		

Mask::~Mask()
{
	delete(m_input);
	delete(m_output);
}

void Mask::Reset()
{
	Module::Reset();
}

void Mask::ProcessFrame()
{
	// m_input->mul(*m_mask);
	threshold(*m_mask, *m_mask, 128, 255, THRESH_BINARY_INV);
	m_input->copyTo(*m_output, *m_mask);
	// cvAnd(m_input, m_mask, m_output);
};

