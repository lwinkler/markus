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

#include "BgrSubMOG.h"
#include "StreamImage.h"
#include "StreamDebug.h"

// for debug
#include "util.h"

using namespace cv;
using namespace std;

BgrSubMOG::BgrSubMOG(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader),
	m_input(Size(m_param.width, m_param.height), m_param.type),
	m_foreground(Size(m_param.width, m_param.height), CV_8UC1),
	m_background(Size(m_param.width, m_param.height), m_param.type)
{
	mp_mog = NULL;
	m_description = "Perform background subtraction via Mixtures Of Gaussians";
	
	m_inputStreams.push_back(new StreamImage(0, "input",       m_input,      *this,   "Video input"));

	m_outputStreams.push_back(new StreamImage(0, "foreground", m_foreground, *this,   "Foreground"));
	m_outputStreams.push_back(new StreamImage(1, "background", m_background, *this,   "Background"));

#ifdef MARKUS_DEBUG_STREAMS
	m_debugStreams.push_back(new StreamDebug(0, "input",       m_input,      *this,   "Input"));
#endif
};

BgrSubMOG::~BgrSubMOG()
{
}

void BgrSubMOG::Reset()
{
	Module::Reset();
	if(mp_mog == NULL)
		delete mp_mog;
	mp_mog = new BackgroundSubtractorMOG(m_param.history, m_param.nmixtures, m_param.backgroundRatio, m_param.noiseSigma);
	mp_mog->initialize(m_input.size(), m_input.type());
}

void BgrSubMOG::ProcessFrame()
{
	mp_mog->operator ()(m_input, m_foreground , m_param.learningRate);
	mp_mog->getBackgroundImage(m_background);
};

