

#include "Feature2D.h"

using namespace std;
using namespace cv;

using vector_float = vector<float>;
using vector_int = vector<int>;
using SimpleBlobDetector_Params = SimpleBlobDetector::Params;

namespace mk {
cv::Ptr<cv::Feature2D> Feature2D::create(const mkjson& x_config) {
	const mkjson& params(x_config.at("parameters"));
	const string name = x_config.at("name");
	const int num = x_config.at("number");


	if(name == "BRISK" && num == 0)
		return cv::BRISK::create(params.value<int>("thresh", 30), params.value<int>("octaves", 3), params.value<float>("patternScale", 1.0f));
		
	if(name == "BRISK" && num == 1)
		return cv::BRISK::create(params.at("radiusList").get<vector_float>(), params.at("numberList").get<vector_int>(), params.value<float>("dMax", 5.85f), params.value<float>("dMin", 8.2f), params.value<vector_int>("indexChange", std::vector<int>()));
		
	if(name == "BRISK" && num == 2)
		return cv::BRISK::create(params.at("thresh").get<int>(), params.at("octaves").get<int>(), params.at("radiusList").get<vector_float>(), params.at("numberList").get<vector_int>(), params.value<float>("dMax", 5.85f), params.value<float>("dMin", 8.2f), params.value<vector_int>("indexChange", std::vector<int>()));
		
	if(name == "ORB" && num == 0)
		return cv::ORB::create(params.value<int>("nfeatures", 500), params.value<float>("scaleFactor", 1.2f), params.value<int>("nlevels", 8), params.value<int>("edgeThreshold", 31), params.value<int>("firstLevel", 0), params.value<int>("WTA_K", 2), params.value<int>("scoreType", ORB::HARRIS_SCORE), params.value<int>("patchSize", 31), params.value<int>("fastThreshold", 20));
		
	if(name == "MSER" && num == 0)
		return cv::MSER::create(params.value<int>("_delta", 5), params.value<int>("_min_area", 60), params.value<int>("_max_area", 14400), params.value<double>("_max_variation", 0.25), params.value<double>("_min_diversity", .2), params.value<int>("_max_evolution", 200), params.value<double>("_area_threshold", 1.01), params.value<double>("_min_margin", 0.003), params.value<int>("_edge_blur_size", 5));
		
	if(name == "FastFeatureDetector" && num == 0)
		return cv::FastFeatureDetector::create(params.value<int>("threshold", 10), params.value<bool>("nonmaxSuppression", true), params.value<int>("type", FastFeatureDetector::TYPE_9_16));
		
	if(name == "AgastFeatureDetector" && num == 0)
		return cv::AgastFeatureDetector::create(params.value<int>("threshold", 10), params.value<bool>("nonmaxSuppression", true), params.value<int>("type", AgastFeatureDetector::OAST_9_16));
		
	if(name == "GFTTDetector" && num == 0)
		return cv::GFTTDetector::create(params.value<int>("maxCorners", 1000), params.value<double>("qualityLevel", 0.01), params.value<double>("minDistance", 1), params.value<int>("blockSize", 3), params.value<bool>("useHarrisDetector", false), params.value<double>("k", 0.04));
		
	if(name == "GFTTDetector" && num == 1)
		return cv::GFTTDetector::create(params.at("maxCorners").get<int>(), params.at("qualityLevel").get<double>(), params.at("minDistance").get<double>(), params.at("blockSize").get<int>(), params.at("gradiantSize").get<int>(), params.value<bool>("useHarrisDetector", false), params.value<double>("k", 0.04));
		
	if(name == "SimpleBlobDetector" && num == 0)
		return cv::SimpleBlobDetector::create(params.value<SimpleBlobDetector_Params>("parameters", SimpleBlobDetector::Params()));
		
	if(name == "KAZE" && num == 0)
		return cv::KAZE::create(params.value<bool>("extended", false), params.value<bool>("upright", false), params.value<float>("threshold", 0.001f), params.value<int>("nOctaves", 4), params.value<int>("nOctaveLayers", 4), params.value<int>("diffusivity", KAZE::DIFF_PM_G2));
		
	if(name == "AKAZE" && num == 0)
		return cv::AKAZE::create(params.value<int>("descriptor_type", AKAZE::DESCRIPTOR_MLDB), params.value<int>("descriptor_size", 0), params.value<int>("descriptor_channels", 3), params.value<float>("threshold", 0.001f), params.value<int>("nOctaves", 4), params.value<int>("nOctaveLayers", 4), params.value<int>("diffusivity", KAZE::DIFF_PM_G2));
		
	throw MkException("Unknown name and function number for creation of descriptor: " + name + string("/") + to_string(num), LOC);
}
} // namespace mk

