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

#ifndef MODULE_FEATURE2D_H
#define MODULE_FEATURE2D_H

#include "Module.h"
#include "StreamObject.h"
#include "CreationFunction.h"

namespace cv {
	inline void to_json(mk::mkjson& _json, const SimpleBlobDetector::Params& _obj) {
		_json = mk::mkjson{
			{"blobColor", _obj.blobColor}, 
			{"filterByArea", _obj.filterByArea}, 
			{"filterByCircularity", _obj.filterByCircularity}, 
			{"filterByColor", _obj.filterByColor}, 
			{"filterByConvexity", _obj.filterByConvexity}, 
			{"filterByInertia", _obj.filterByInertia}, 
			{"maxArea", _obj.maxArea}, 
			{"maxCircularity", _obj.maxCircularity}, 
			{"maxConvexity", _obj.maxConvexity}, 
			{"maxInertiaRatio", _obj.maxInertiaRatio}, 
			{"maxThreshold", _obj.maxThreshold}, 
			{"minArea", _obj.minArea}, 
			{"minCircularity", _obj.minCircularity}, 
			{"minConvexity", _obj.minConvexity}, 
			{"minDistBetweenBlobs", _obj.minDistBetweenBlobs}, 
			{"minInertiaRatio", _obj.minInertiaRatio}, 
			{"minRepeatability", _obj.minRepeatability}, 
			{"minThreshold", _obj.minThreshold}, 
			{"thresholdStep", _obj.thresholdStep}
		};
	}
	inline void from_json(const mk::mkjson& _json, SimpleBlobDetector::Params& _obj) {
		_obj.blobColor = _json.get<uchar>();
		_obj.filterByArea = _json.get<bool>();
		_obj.filterByCircularity = _json.get<bool>();
		_obj.filterByColor = _json.get<bool>();
		_obj.filterByConvexity = _json.get<bool>();
		_obj.filterByInertia = _json.get<bool>();
		_obj.maxArea = _json.get<float>();
		_obj.maxCircularity = _json.get<float>();
		_obj.maxConvexity = _json.get<float>();
		_obj.maxInertiaRatio = _json.get<float>();
		_obj.maxThreshold = _json.get<float>();
		_obj.minArea = _json.get<float>();
		_obj.minCircularity = _json.get<float>();
		_obj.minConvexity = _json.get<float>();
		_obj.minDistBetweenBlobs = _json.get<float>();
		_obj.minInertiaRatio = _json.get<float>();
		_obj.minRepeatability = _json.get<size_t>();
		_obj.minThreshold = _json.get<float>();
		_obj.thresholdStep = _json.get<float>();
	}
} // namespace cv

namespace mk {

/**
* @brief This class is a parent class for all module of keypoint extraction
* http://docs.opencv.org/modules/features2d/doc/common_interfaces_of_feature_detectors.html
*/
class Feature2D : public Module
{
public:
	Feature2D() = default;
	static cv::Ptr<cv::Feature2D> create(const CreationFunction& x_funct);
	class Parameters : public Module::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : Module::Parameters(x_name)
		{
			AddParameter(new ParameterT<CreationFunction>("create", R"({"name": "ORB", "number": 0, "parameters":{}})"_json, &create, "The parameters to pass to the create method method of ORB, BRIEF, ..."));
			AddParameter(new ParameterT<bool>("computeFeatures", false, &computeFeatures, "Compute the features associated with the keypoints."));

			RefParameterByName("type").SetRange(R"({"allowed":["CV_8UC1"]})"_json);
			RefParameterByName("width").SetRange(R"({"min":64, "max":6400})"_json);
			RefParameterByName("height").SetRange(R"({"min":48, "max":4800})"_json);
		};
		CreationFunction create;
		bool computeFeatures;
	};
	MKCLASS("Feature2D")
	MKCATEG("KeyPoints")
	MKDESCR("Extract key points of all types")

	explicit Feature2D(ParameterStructure& xr_params);
	~Feature2D() override;
	// MKCLASS("Feature2D")
	// MKDESCR("Extract different types of keyPoints from image")

private:
	Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	void ProcessFrame() override;
	void Reset() override;

	// input
	cv::Mat m_input;
	std::vector <Object> m_objectsIn;

	// output
	std::vector <Object> m_objectsOut;

	// state variables
	cv::Ptr<cv::Feature2D>     mp_detector;
	// cv::Ptr<cv::DescriptorExtractor> mp_descriptor;

#ifdef MARKUS_DEBUG_STREAMS
	cv::Mat m_debug;
#endif
};


} // namespace mk
#endif
