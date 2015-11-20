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

#ifndef CASCADE_DETECTOR_H
#define CASCADE_DETECTOR_H

#include "Module.h"
#include "Parameter.h"
#include "StreamObject.h"

/*! \class CascadeDetector
 *  \brief Module class for detection based on cascade filters (Haar, ...)
 *
 */



/**
* @brief Detect objects from a video stream using a cascade filter (c.f. Haar patterns)
*/
class CascadeDetector : public Module
{
public:
	class Parameters : public Module::Parameters
	{

	public:
		Parameters(const ConfigReader& x_confReader) :
			Module::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterInt("min_neighbors", 2, 1, 100, 	&minNeighbors,	"Minimum numbers of neighbors (higher: less sensitive)")); // Note : Seems to be a bug with minNeighbors = 1 with most filters
			m_list.push_back(new ParameterInt("min_side", 0, 0, 200, 		&minSide,	"Minimum size of the detected object"));
			m_list.push_back(new ParameterFloat("scale_factor", 1.2, 1, 2, 	&scaleFactor,	"Scale factor for scanning (higher: less sensitive)"));
			m_list.push_back(new ParameterString("filter_file", "modules/CascadeDetector/lbpcascade_frontalface.xml",  &filterFile, "File with filter data of the detected object"));
			// m_list.push_back(new ParameterString("color", "(255,255,255)",		&color,	"Color to draw the output"));
			m_list.push_back(new ParameterString("object_label", "casc", 			&objectLabel,	"Label to be applied to the objects detected by the cascade filter (e.g. face)"));

			RefParameterByName("type").SetRange("[CV_8UC1]");
			RefParameterByName("type").SetDefault("CV_8UC1");
			// Limit size to accelerate unit tests
			RefParameterByName("width").SetRange("[1:1280]");
			RefParameterByName("height").SetRange("[1:960]");
			RefParameterByName("filter_file").SetRange("["
				"modules/CascadeDetector/haarcascade_eye_tree_eyeglasses.xml,"
				"modules/CascadeDetector/haarcascade_eye.xml,"
				"modules/CascadeDetector/haarcascade_frontalface_alt2.xml,"
				"modules/CascadeDetector/haarcascade_frontalface_alt_tree.xml,"
				"modules/CascadeDetector/haarcascade_frontalface_alt.xml,"
				"modules/CascadeDetector/haarcascade_frontalface_default.xml,"
				"modules/CascadeDetector/haarcascade_fullbody.xml,"
				"modules/CascadeDetector/haarcascade_lefteye_2splits.xml,"
				"modules/CascadeDetector/haarcascade_lowerbody.xml,"
				"modules/CascadeDetector/haarcascade_mcs_eyepair_big.xml,"
				"modules/CascadeDetector/haarcascade_mcs_eyepair_small.xml,"
				"modules/CascadeDetector/haarcascade_mcs_lefteye.xml,"
				"modules/CascadeDetector/haarcascade_mcs_mouth.xml,"
				"modules/CascadeDetector/haarcascade_mcs_nose.xml,"
				"modules/CascadeDetector/haarcascade_mcs_righteye.xml,"
				"modules/CascadeDetector/haarcascade_mcs_upperbody.xml,"
				"modules/CascadeDetector/haarcascade_profileface.xml,"
				"modules/CascadeDetector/haarcascade_righteye_2splits.xml,"
				"modules/CascadeDetector/haarcascade_upperbody.xml,"
				"modules/CascadeDetector/lbpcascade_frontalface.xml"
			"]");

			Init();

			LockParameterByName("filter_file");
		}

		int minNeighbors;
		int minSide;
		float scaleFactor;
		std::string filterFile;
		std::string objectLabel;
	};

	CascadeDetector(ParameterStructure& xr_params);
	~CascadeDetector(void);
	MKCLASS("CascadeDetector")
	MKDESCR("Detect objects from a video stream using a cascade filter (c.f. Haar patterns)")

	void Reset() override;
	void ProcessFrame() override;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	// state
	cv::CascadeClassifier m_cascade;

	// input
	cv::Mat m_input;

	// output
	std::vector<Object> m_detectedObjects;

	// debug
#ifdef MARKUS_DEBUG_STREAMS
	cv::Mat m_debug;
#endif
};

#endif

