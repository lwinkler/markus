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

#ifndef MORPH_H
#define MORPH_H

#include "Module.h"

class ConfigReader;


class MorphParameterStructure : public ModuleParameterStructure
{
public:
	MorphParameterStructure(const ConfigReader& x_confReader) : ModuleParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterInt("operator",    0,   PARAM_INT, 0, 6,    &oper,       "Morphological operator: ERODE=0,DILATE=1,OPEN=2,CLOSE=3,GRADIENT=4,TOPHAT=5,BLACKHAT=6"));
		m_list.push_back(new ParameterInt("element",     2,   PARAM_INT, 0, 2,    &element,    "Morphological element: RECT=0, CROSS=1, ELLIPSE=2"));
		m_list.push_back(new ParameterInt("kernel_size", 5,   PARAM_INT, 1, 100,  &kernelSize, "Size of the element"));

		ParameterStructure::Init();
	};
	int oper;
	int element;
	int kernelSize;
};

class Morph : public Module
{
public:
	Morph(const ConfigReader& x_configReader);
	~Morph();
	
	virtual void ProcessFrame();
	void Reset();
		
private:
	MorphParameterStructure m_param;
	inline virtual ModuleParameterStructure& RefParameter() { return m_param;};

	// Background subtraction	
	cv::Mat* m_foreground;

protected:
	cv::Mat * m_input;
	cv::Mat * m_output;
	cv::Mat m_element;
	
	static const char * m_type;
};


#endif
