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


namespace mk {
/**
* @brief Apply a morphological operator to an image
*/
class Morph : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : Module::Parameters(x_name)
		{
			AddParameter(new ParameterInt("operator",    0,   0, 6,    &oper,       "Morphological operator: ERODE=0,DILATE=1,OPEN=2,CLOSE=3,GRADIENT=4,TOPHAT=5,BLACKHAT=6"));
			AddParameter(new ParameterInt("element",     2,   0, 2,    &element,    "Morphological element: RECT=0, CROSS=1, ELLIPSE=2"));
			AddParameter(new ParameterInt("kernelSize", 5,   3, 30,   &kernelSize, "Size of the element"));
			AddParameter(new ParameterInt("iterations",  1,   1, 10,   &iterations, "Number of times the operation is applied to the image"));

			RefParameterByName("type").SetRange(R"({"allowed":["CV_8UC1","CV_8UC3"]})"_json);
		};
		int oper;
		int element;
		int kernelSize;
		int iterations;
	};

	explicit Morph(ParameterStructure& xr_params);
	virtual ~Morph();
	MKCLASS("Morph")
	MKCATEG("Image")
	MKDESCR("Apply a morphological operator to an image")

	void ProcessFrame() override;
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


} // namespace mk
#endif
