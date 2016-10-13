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

log4cxx::LoggerPtr BgrSubMOG::m_logger(log4cxx::Logger::getLogger("BgrSubMOG"));

BgrSubMOG::BgrSubMOG(ParameterStructure& xr_params) :
	Module(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params)),
	m_input(Size(m_param.width, m_param.height), m_param.type),
	m_foreground(Size(m_param.width, m_param.height), CV_8UC1),
	m_background(Size(m_param.width, m_param.height), m_param.type)
{
	mp_mog = nullptr;

	AddInputStream(0, new StreamImage("image",       m_input,      *this,   "Video input"));

	AddOutputStream(0, new StreamImage("foreground", m_foreground, *this,   "Foreground"));
	AddOutputStream(1, new StreamImage("background", m_background, *this,   "Background"));

#ifdef MARKUS_DEBUG_STREAMS
	AddDebugStream(0, new StreamDebug("debug",       m_input,      *this,   "Input"));
#endif
};

BgrSubMOG::~BgrSubMOG()
{
	CLEAN_DELETE(mp_mog);
}

void BgrSubMOG::Reset()
{
	Module::Reset();
	CLEAN_DELETE(mp_mog);
	mp_mog = new BackgroundSubtractorMOG(m_param.history, m_param.nmixtures, m_param.backgroundRatio, m_param.noiseSigma);
	mp_mog->initialize(m_input.size(), m_input.type());
}

void BgrSubMOG::ProcessFrame()
{
	mp_mog->operator ()(m_input, m_foreground , m_param.learningRate);
	mp_mog->getBackgroundImage(m_background);
};

