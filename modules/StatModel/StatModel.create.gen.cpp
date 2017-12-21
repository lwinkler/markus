

#include "StatModel.h"

using namespace std;
using namespace cv;

// using vector_float = vector<float>;
// using vector_int = vector<int>;
// using SimpleBlobDetector_Params = SimpleBlobDetector::Params;

namespace mk {
cv::Ptr<cv::ml::StatModel> StatModel::create(const CreationFunction& x_funct) {
	const mkjson& params(x_funct.parameters);
	const string name = x_funct.name;
	const int num = x_funct.number;


	if(name == "NormalBayesClassifier" && num == 0)
		return cv::ml::NormalBayesClassifier::create();
		
	if(name == "KNearest" && num == 0)
		return cv::ml::KNearest::create();
		
	if(name == "SVM" && num == 0)
		return cv::ml::SVM::create();
		
	if(name == "EM" && num == 0)
		return cv::ml::EM::create();
		
	if(name == "DTrees" && num == 0)
		return cv::ml::DTrees::create();
		
	if(name == "RTrees" && num == 0)
		return cv::ml::RTrees::create();
		
	if(name == "Boost" && num == 0)
		return cv::ml::Boost::create();
		
	if(name == "ANN_MLP" && num == 0)
		return cv::ml::ANN_MLP::create();
		
	if(name == "LogisticRegression" && num == 0)
		return cv::ml::LogisticRegression::create();
		
	if(name == "SVMSGD" && num == 0)
		return cv::ml::SVMSGD::create();
		
	throw MkException("Unknown name and function number for creation of descriptor: " + name + string("/") + to_string(num), LOC);
}
} // namespace mk

