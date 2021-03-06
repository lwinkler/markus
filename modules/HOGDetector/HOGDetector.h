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

#ifndef HOG_DETECTOR_H
#define HOG_DETECTOR_H

#include "Module.h"
#include "Parameter.h"
#include "StreamObject.h"

namespace mk {
/*! \class HOGDetector
 *  \brief Module class for detection based on HOG models
 *
 */

class HOGDetector : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : Module::Parameters(x_name)
		{
			// AddParameter(new ParameterInt("minNeighbors", 2, 1, 100, 	&minNeighbors,	"Minimum numbers of neighbors (higher: less sensitive)")); // Note : Seems to be a bug with minNeighbors = 1 with most filters
			AddParameter(new ParameterInt("minSide", 0, 0, 200, 		&minSide,	"Minimum size of the detected object"));
			AddParameter(new ParameterFloat("scaleFactor", 1.2, 1, 2, 	&scaleFactor,	"Scale factor for scanning (higher: less sensitive)"));
			// "File with filter data of the detected object"));
			AddParameter(new ParameterString("objectLabel", "hog", 			&objectLabel,	"Label to be applied to the objects detected by the cascade filter (e.g. face)"));

			// Limit size to accelerate unit tests
			RefParameterByName("width").SetDefaultAndValue(320);
			RefParameterByName("height").SetDefaultAndValue(240);
			RefParameterByName("width").SetRange(R"({"min":320, "max":1280})"_json);
			RefParameterByName("height").SetRange(R"({"min":240, "max":960})"_json);
			RefParameterByName("type").SetRange(R"({"allowed":["CV_8UC1"]})"_json);
		};

		// int minNeighbors;
		int minSide;
		float scaleFactor;
		std::string objectLabel;
	};

	explicit HOGDetector(ParameterStructure& xr_params);
	~HOGDetector() override;
	MKCLASS("HOGDetector")
	MKDESCR("Detect objects from a video stream using a HOG descriptor")
	MKCATEG("Other")

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	void Reset() override;
	void ProcessFrame() override;

	// state
	cv::HOGDescriptor m_hog;

	// input
	cv::Mat m_input;

	// output
	std::vector<Object> m_detectedObjects;

	// debug
#ifdef MARKUS_DEBUG_STREAMS
	cv::Mat m_debug;
#endif
};

} // namespace mk
#endif

