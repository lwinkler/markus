

#include "BackgroundSubtractor.h"

using namespace std;
using namespace cv;

// using vector_float = vector<float>;
// using vector_int = vector<int>;
// using SimpleBlobDetector_Params = SimpleBlobDetector::Params;

namespace mk {
cv::Ptr<cv::BackgroundSubtractor> BackgroundSubtractor::create(const CreationFunction& x_funct) {
	const mkjson& params(x_funct.parameters);
	const string name = x_funct.name;
	const int num = x_funct.number;


	if(name == "BackgroundSubtractorMOG2" && num == 0)
		return cv::createBackgroundSubtractorMOG2(params.value<int>("history", 500), params.value<double>("varThreshold", 16), params.value<bool>("detectShadows", true));
		
	if(name == "BackgroundSubtractorKNN" && num == 1)
		return cv::createBackgroundSubtractorKNN(params.value<int>("history", 500), params.value<double>("dist2Threshold", 400.0), params.value<bool>("detectShadows", true));
		
	throw MkException("Unknown name and function number for creation of descriptor: " + name + string("/") + to_string(num), LOC);
}
} // namespace mk

