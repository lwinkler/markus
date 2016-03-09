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



/**
* @brief Apply a morphological operator to an image
*/
class Morph : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : Module::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterInt("operator",    0,   0, 6,    &oper,       "Morphological operator: ERODE=0,DILATE=1,OPEN=2,CLOSE=3,GRADIENT=4,TOPHAT=5,BLACKHAT=6"));
			m_list.push_back(new ParameterInt("element",     2,   0, 2,    &element,    "Morphological element: RECT=0, CROSS=1, ELLIPSE=2"));
			m_list.push_back(new ParameterInt("kernel_size", 5,   3, 30,   &kernelSize, "Size of the element"));
			m_list.push_back(new ParameterInt("iterations",  1,   1, 10,   &iterations, "Number of times the operation is applied to the image"));

			RefParameterByName("type").SetRange("[CV_8UC1,CV_8UC3]");
			Init();
		};
		int oper;
		int element;
		int kernelSize;
		int iterations;
	};

	Morph(ParameterStructure& xr_params);
	~Morph();
	MKCLASS("Morph")
	MKCATEG("Image")
	MKDESCR("Apply a morphological operator to an image")

	virtual void ProcessFrame() override;
	void Reset() override;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;
protected:

	// input
	cv::Mat m_input;

	// output
	cv::Mat m_output;

	// temporary
	cv::Mat m_element;
};


#endif
