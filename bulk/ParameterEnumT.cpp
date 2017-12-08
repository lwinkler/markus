/* This file was generated automatically the generate_param_enum.py parser */

#include "ParameterEnumT.h"

using namespace std;



#include "util/enums.h"

// Static variables
template<> const map<string, int>  ParameterEnumT<mk::CachedState>::Enum =
{
	{"NO_CACHE", 0},
	{"WRITE_CACHE", 1},
	{"READ_CACHE", 2},
	{"DISABLED", 3}
};
template<> const map<int, string> ParameterEnumT<mk::CachedState>::ReverseEnum = ParameterEnum::CreateReverseMap(ParameterEnumT<mk::CachedState>::Enum);
template<> const string ParameterEnumT<mk::CachedState>::className = "ParameterCachedState";



#include "util/enums.h"

// Static variables
template<> const map<string, int>  ParameterEnumT<mk::ImageType>::Enum =
{
	{"CV_8UC1", 0},
	{"CV_8UC2", 8},
	{"CV_8UC3", 16},
	{"CV_8SC1", 1},
	{"CV_8SC2", 9},
	{"CV_8SC3", 17},
	{"CV_8SC4", 25},
	{"CV_16UC1", 2},
	{"CV_16UC2", 10},
	{"CV_16UC3", 18},
	{"CV_16UC4", 26},
	{"CV_16SC1", 3},
	{"CV_16SC2", 11},
	{"CV_16SC3", 19},
	{"CV_16SC4", 27},
	{"CV_32SC1", 4},
	{"CV_32SC2", 12},
	{"CV_32SC3", 20},
	{"CV_32SC4", 28},
	{"CV_32FC1", 5},
	{"CV_32FC2", 13},
	{"CV_32FC3", 21},
	{"CV_32FC4", 29},
	{"CV_64FC1", 6},
	{"CV_64FC2", 14},
	{"CV_64FC3", 22},
	{"CV_64FC4", 30}
};
template<> const map<int, string> ParameterEnumT<mk::ImageType>::ReverseEnum = ParameterEnum::CreateReverseMap(ParameterEnumT<mk::ImageType>::Enum);
template<> const string ParameterEnumT<mk::ImageType>::className = "ParameterImageType";



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
template<> const map<int, string> ParameterEnumT<cv::ml::SVM::KernelTypes>::ReverseEnum = ParameterEnum::CreateReverseMap(ParameterEnumT<cv::ml::SVM::KernelTypes>::Enum);
template<> const string ParameterEnumT<cv::ml::SVM::KernelTypes>::className = "ParameterKernelTypes";



#include "opencv/modules/ml/include/opencv2/ml.hpp"

// Static variables
template<> const map<string, int>  ParameterEnumT<cv::ml::SVM::Types>::Enum =
{
	{"C_SVC", 100},
	{"NU_SVC", 101},
	{"ONE_CLASS", 102},
	{"EPS_SVR", 103},
	{"NU_SVR", 104}
};
template<> const map<int, string> ParameterEnumT<cv::ml::SVM::Types>::ReverseEnum = ParameterEnum::CreateReverseMap(ParameterEnumT<cv::ml::SVM::Types>::Enum);
template<> const string ParameterEnumT<cv::ml::SVM::Types>::className = "ParameterSvmTypes";

