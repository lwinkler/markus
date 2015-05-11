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

#include "FastNlMeansDenoising.h"
#include "StreamImage.h"
// #include "StreamDebug.h"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

log4cxx::LoggerPtr FastNlMeansDenoising::m_logger(log4cxx::Logger::getLogger("FastNlMeansDenoising"));

FastNlMeansDenoising::FastNlMeansDenoising(ParameterStructure& xr_params) :
	Module(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params)),
	m_input(Size(m_param.width, m_param.height), m_param.type),
	m_output(Size(m_param.width, m_param.height), m_param.type)
{
	AddInputStream(0  , new StreamImage("input"  , m_input  , *this , "Video input"));

	AddOutputStream(0 , new StreamImage("output" , m_output , *this , "Foreground"));
};


FastNlMeansDenoising::~FastNlMeansDenoising()
{
}

void FastNlMeansDenoising::Reset()
{
	Module::Reset();
}

void FastNlMeansDenoising::ProcessFrame()
{
	fastNlMeansDenoising(m_input, m_output, m_param.h, m_param.templateWindowSize, m_param.searchWindowSize);
	// fastNlMeansDenoisingColored(InputArray src, OutputArray dst, float h=3, float hColor=3, int templateWindowSize=7, int searchWindowSize=21 )
}

