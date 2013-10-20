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

#include "TempDiff.h"
#include "StreamImage.h"

// for debug
#include "util.h"
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

const char * TempDiff::m_type = "TempDiff";


TempDiff::TempDiff(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader)
{
	m_description = "Perform temporal differencing: compare frame with previous frame by subtraction";
	m_output   = new Mat(Size(m_param.width, m_param.height), m_param.type);
	m_input    = new Mat(Size(m_param.width, m_param.height), m_param.type);

	m_lastImg 		= new Mat(Size(m_param.width, m_param.height), m_param.type);
	m_temporalDiff 		= new Mat(Size(m_param.width, m_param.height), CV_8UC1);
	
	m_inputStreams.push_back(new StreamImage(0, "input", m_input, *this,             "Video input"));
	m_outputStreams.push_back(new StreamImage(0, "temp_diff", m_temporalDiff, *this, "Temporal difference"));
		
	Reset();
}

TempDiff::~TempDiff(void )
{
	delete(m_input);
	delete(m_output);
	delete(m_lastImg);
	delete(m_temporalDiff);
}

void TempDiff::Reset()
{
	Module::Reset();
	m_emptyTemporalDiff = true;
}

void TempDiff::ProcessFrame()
{
	// Main part of the program
	if(m_emptyTemporalDiff) 
	{
		m_input->copyTo(*m_lastImg);
		m_emptyTemporalDiff = false;
	}
	else 
	{
		static Mat* tmp = new Mat(Size(m_input->cols, m_input->rows), m_input->depth(), m_input->channels());
	
		subtract(*m_input, *m_lastImg, *tmp);
		absdiff(*m_input, *m_lastImg, *tmp);
		adjustChannels(tmp, m_temporalDiff);
		
		m_input->copyTo(*m_lastImg);
	}
};

