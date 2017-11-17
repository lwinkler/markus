/* This file was generated automatically the generate_param_enum.py parser */

#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include "ParameterEnumT.h"

using namespace std;



#include "opencv/modules/ml/include/opencv2/ml.hpp"

// Static variables
template<> const map<string, int>  ParameterEnumT<cv::ml::SVM::KernelTypes>::Enum =
{
	{"CUSTOM", -1},
	{"LINEAR", 0},
	{"POLY", 1},
	{"RBF", 2},
	{"SIGMOID", 3},
	{"CHI2", 4},
	{"INTER", 5}
};
template<> const map<int, string> ParameterEnumT<cv::ml::SVM::KernelTypes>::ReverseEnum = ParameterEnumT::CreateReverseMap(ParameterEnumT<cv::ml::SVM::KernelTypes>::Enum);
template<> const string ParameterEnumT<cv::ml::SVM::KernelTypes>::m_type = "KernelTypes";

