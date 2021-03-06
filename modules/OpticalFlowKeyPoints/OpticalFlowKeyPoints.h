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

#ifndef OPTICAL_FLOW_KEYPOINTS_H
#define OPTICAL_FLOW_KEYPOINTS_H

#include "StreamObject.h"
#include "StreamImage.h"



namespace mk {
/**
* @brief Extract different types of keypoints
*/
class OpticalFlowKeyPoints : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : Module::Parameters(x_name)
		{
			AddParameter(new ParameterInt("winSide",  21, 3, 255, &winSide, "Side of the square window used as search window at each pyramid level"));
			AddParameter(new ParameterInt("maxLevel",  3, 0, 12, &maxLevel, "0-based maximal pyramid level number; if set to 0, pyramids are not used (single level), if set to 1, two levels are used, and so on; if pyramids are passed to input then algorithm will use as many levels as pyramids have but no more than maxLevel"));

			RefParameterByName("type").SetRange(R"({"allowed":["CV_8UC1","CV_8UC3"]})"_json);
		};
		int winSide;
		int maxLevel;
		// TermCriteria criteria=TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 30, 0.01)
		// int flags=0
		// double minEigThreshold=1e-4
	};

	explicit OpticalFlowKeyPoints(ParameterStructure& x_configReader);
	~OpticalFlowKeyPoints() override;
	MKCLASS("OpticalFlowKeyPoints")
	MKCATEG("KeyPoints")
	MKDESCR("Calculate optical flow for all key points")

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	void Reset() override;
	void ProcessFrame() override;

	// input
	cv::Mat m_input;
	std::vector <Object> m_keyPointsIn;

	// output
	// std::vector <Object> m_keyPointsOut;

	//last img used to compute OF
	cv::Mat                  m_lastImg;
	std::vector<cv::Point2f> m_lastPoints;

#ifdef MARKUS_DEBUG_STREAMS
	cv::Mat m_debug;
#endif
};


} // namespace mk
#endif
