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

#ifndef FAST_DENOISING_H
#define FAST_DENOISING_H

#include "Module.h"


// based on: http://docs.opencv.org/modules/photo/doc/denoising.html


namespace mk {
class FastNlMeansDenoising : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : Module::Parameters(x_name)
		{
			AddParameter(new ParameterFloat("h"                 , 3 , 1 , 100 , &h                  , "Parameter regulating filter strength. Big h value perfectly removes noise but also removes image details   , smaller h value preserves details but also preserves some noise"));
			AddParameter(new ParameterInt  ("templateWinSize" , 7 , 1 ,  33 , &templateWindowSize , "Size in pixels of the template patch that is used to compute weights. Should be odd"));
			AddParameter(new ParameterInt  ("searchWinSize"   , 7 , 1 ,  33 , &searchWindowSize   , "Size in pixels of the window that is used to compute weighted average for given pixel. Should be odd. "));
			AddParameter(new ParameterBool ("cielab"   , 0 , 0, 1 , &cielab, "Convert image to CIELAB colorspace and then separately denoise L and AB"));

			RefParameterByName("type").SetRange(R"({"allowed":["CV_8UC1","CV_8UC2","CV_8UC3"]})"_json);
			RefParameterByName("width").SetDefaultAndValue(160);
			RefParameterByName("height").SetDefaultAndValue(120);
		};
		float h;
		int templateWindowSize;
		int searchWindowSize;
		bool cielab;

		// Redefine CheckRangeAndThrow: we want to add new conditions
		virtual void CheckRangeAndThrow() const
		{
			Module::Parameters::CheckRangeAndThrow();
			cv::Mat m(1,1, this->type);
			if(cielab && m.channels() != 3)
				throw ParameterException("Cielab color space is only for images with 3 channels", LOC);
		}
	};

	explicit FastNlMeansDenoising(ParameterStructure& xr_params);
	virtual ~FastNlMeansDenoising();
	MKCLASS("FastNlMeansDenoising")
	MKCATEG("Image")
	MKDESCR("Perform image denoising using Non-local Means Denoising algorithm, for gaussian noise")

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
};

} // namespace mk

#endif
