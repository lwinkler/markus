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

#include "Input.h"

using namespace std;


Input::Input(const std::string& x_name, ConfigReader& x_configReader): 
	//m_param(x_configReader, x_name), 
	//Configurable(x_configReader),
	m_name(x_name), Module(x_name, x_configReader)
{
	cout<<endl<<"*** Create object Input : "<<x_name<<" ***"<<endl;

	m_render = NULL;  // cvCreateImage( cvSize(12,12)/*GetWidth(), GetHeight())*/, IPL_DEPTH_8U, 3);
	m_inputWidth = 0;
	m_inputHeight = 0;
// 	m_outputStreams.push_back(new StreamImage("detected", m_input));

}

Input::~Input()
{
	//cvReleaseImage(&m_input);
}

void Input::ProcessFrame(const IplImage* x_img, const double x_timeSinceLastProcessing)
{
	cvCopy(m_input, m_render);
	for(vector<Stream *>::const_iterator it = m_relatedStreams.begin() ; it != m_relatedStreams.end() ; it++)
	{
		(*it)->Render(m_render);
	}
}

