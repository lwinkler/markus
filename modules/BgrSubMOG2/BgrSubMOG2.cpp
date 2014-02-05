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

#include "BgrSubMOG2.h"
#include "StreamImage.h"
#include "StreamDebug.h"

using namespace cv;
using namespace std;

BgrSubMOG2::BgrSubMOG2(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader),
	m_input(Size(m_param.width, m_param.height), m_param.type),
	m_foreground(Size(m_param.width, m_param.height), CV_8UC1),
	m_background(Size(m_param.width, m_param.height), m_param.type),
	m_foregroundWithShadows(Size(m_param.width, m_param.height), m_param.type)
{
	mp_mog2 = NULL;

	m_description = "Perform background subtraction via Mixtures Of Gaussians";
	
	AddInputStream(0, new StreamImage("input",       m_input, *this,   "Video input"));

	AddOutputStream(0, new StreamImage("foreground", m_foreground,*this,      "Foreground"));
	AddOutputStream(1, new StreamImage("background", m_background, *this,		"Background"));

#ifdef MARKUS_DEBUG_STREAMS
	AddDebugStream(0, new StreamDebug("foreground_with_shadows", m_foregroundWithShadows, *this,	"Foreground with shadows"));
#endif
};
		

BgrSubMOG2::~BgrSubMOG2()
{
}

void BgrSubMOG2::Reset()
{
	Module::Reset();
	if(mp_mog2 != NULL)
		delete(mp_mog2);
	mp_mog2 = new BackgroundSubtractorMOG2(m_param.history, m_param.varThres, m_param.bShadowDetection);
	mp_mog2->initialize(m_input.size(), m_input.type());
}

void BgrSubMOG2::ProcessFrame()
{
	mp_mog2->operator ()(m_input, m_foregroundWithShadows, m_param.learningRate);
	mp_mog2->getBackgroundImage(m_background);

	// Threshold shadows (value=128) to 0
	threshold(m_foregroundWithShadows, m_foreground, 254, 255, cv::THRESH_BINARY);
}

