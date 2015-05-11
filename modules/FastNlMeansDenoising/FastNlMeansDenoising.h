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
// #include "opencv2/video/background_segm.hpp"


// based on: http://docs.opencv.org/modules/photo/doc/denoising.html


class FastNlMeansDenoising : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : Module::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterFloat("h"                 , 3 , 1 , 100 , &h                  , "Parameter regulating filter strength. Big h value perfectly removes noise but also removes image details   , smaller h value preserves details but also preserves some noise"));
			m_list.push_back(new ParameterInt  ("template_win_size" , 7 , 1 , 100 , &templateWindowSize , "Size in pixels of the template patch that is used to compute weights. Should be odd"));
			m_list.push_back(new ParameterInt  ("search_win_size"   , 7 , 1 , 100 , &searchWindowSize   , "Size in pixels of the window that is used to compute weighted average for given pixel. Should be odd. "));

			Init();
		};
		float h;
		int templateWindowSize; 
		int searchWindowSize;
	};

	FastNlMeansDenoising(ParameterStructure& xr_params);
	~FastNlMeansDenoising();
	MKCLASS("FastNlMeansDenoising")
	MKDESCR("Perform image denoising using Non-local Means Denoising algorithm, for gaussian noise")

	virtual void ProcessFrame();
	void Reset();

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	// input
	cv::Mat m_input;

	// output
	cv::Mat m_output;
};


#endif
