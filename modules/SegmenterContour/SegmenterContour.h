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

#ifndef SEGMENTER_CONTOUR_H
#define SEGMENTER_CONTOUR_H

#include "Module.h"
#include "StreamObject.h"

namespace mk {
/**
* @brief Segments a binary image and outputs a stream of objects (with OpenCV contour) and extracts their features (position, width and height)
*/
class SegmenterContour : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : Module::Parameters(x_name)
		{
			AddParameter(new ParameterInt(   "minWidth",  0, 	 0, MAX_WIDTH,  &minWidth,	"Minimal width of an object to segment."));
			AddParameter(new ParameterInt(   "minHeight", 0, 	 0, MAX_HEIGHT, &minHeight,	"Minimal height of an object to segment."));
			AddParameter(new ParameterString("objectLabel",         "object",             &objectLabel,"Label to be applied to the objects detected by the cascade filter (e.g. face)"));
			AddParameter(new ParameterString("features",     "x,y,width,height",           &features,   "List of features to extract, separated with ',' possible: x,y,width,height,area,ellipse_{angle,cos,sin,ratio,width,height}, moment_{00,11,20,02}, hu_moment_{1-7}"));

			RefParameterByName("type").SetRange(R"({"allowed":["CV_8UC1"]})"_json); //,CV_32SC1]");
			RefParameterByName("features").SetRange(R"({"allowed":[
									"x,y,width,height",
									"area",
									"ellipse_angle,ellipse_cos,ellipse_sin,ellipse_width,ellipse_height,ellipse_ratio",
									"moment_00,moment_11,moment_02,moment_20",
									"hu_moment_1,hu_moment_2,hu_moment_3,hu_moment_4,hu_moment_5,hu_moment_6,hu_moment_7",
									"solidity"
								]})"_json);
		};
		std::string objectLabel;
		int minWidth;
		int minHeight;
		std::string features;
	};

	explicit SegmenterContour(ParameterStructure& xr_params);
	virtual ~SegmenterContour();
	MKCLASS("SegmenterContour")
	MKDESCR("Segments a binary image and outputs a stream of objects (with OpenCV contour) and extracts their features (position, width and height)")

	void ProcessFrame() override;
	void Reset() override;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:

	// input
	cv::Mat m_input;

	// output
	std::vector<Object> m_regions;

	// temporary
	std::vector<std::string> m_featureNames;
	bool m_computeFitEllipse;
	bool m_computeMinRect;
	bool m_computeMoment;
	bool m_computeHuMoment;

	// debug
#ifdef MARKUS_DEBUG_STREAMS
	cv::RNG m_rng;
	cv::Mat m_debug;
#endif
};


} // namespace mk
#endif
