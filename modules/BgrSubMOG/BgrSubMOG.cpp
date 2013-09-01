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

// for debug
#include "util.h"
/*#include "cv.h"
#include "highgui.h"
*/
using namespace cv;
using namespace std;

const char * BgrSubMOG::m_type = "BackgroundSubtractorSimpleMOG";


BgrSubMOG::BgrSubMOG(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_mog(3, 4, false),
	m_param(x_configReader)
{
	//m_mog.nmixtures = 3;


	m_description = "Perform background subtraction via Mixtures Of Gaussians";
	m_input    = new Mat(cvSize(m_param.width, m_param.height), m_param.type);
	m_background 		= new Mat(cvSize(m_param.width, m_param.height), m_param.type);
	m_foreground 		= new Mat(cvSize(m_param.width, m_param.height), m_param.type);
	
	m_inputStreams.push_back(new StreamImage(0, "input",             m_input, *this,   "Video input"));

	m_outputStreams.push_back(new StreamImage(0, "foreground", m_foreground,*this,      "Foreground"));
	m_outputStreams.push_back(new StreamImage(1, "background", m_background, *this,		"Background"));
};
		

BgrSubMOG::~BgrSubMOG()
{
	delete(m_input);
	delete(m_background);
	delete(m_foreground);
}

void BgrSubMOG::Reset()
{
	Module::Reset();
	// m_emptyBackgroundSubtractor = true;
}

void BgrSubMOG::ProcessFrame()
{
	m_mog.operator ()(*m_input, *m_foreground, 0.00001);
	m_mog.getBackgroundImage(*m_background);
};

