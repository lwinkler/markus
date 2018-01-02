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

#ifndef MK_STATMODEL_H
#define MK_STATMODEL_H

#include "Module.h"
#include "CreationFunction.h"
#include "Parameter.h"
#include "StreamObject.h"
#include "ParameterEnumT.h"

namespace mk {
/*! \class StatModel
 *  \brief Classification using StatModel
 *
 */

/**
* @brief Classify objects by using a StatModel on its features
*/
class StatModel : public Module
{
public:
	static cv::Ptr<cv::ml::StatModel> create(const CreationFunction& x_funct);
	class Parameters : public Module::Parameters
	{

	public:
		explicit Parameters(const std::string& x_name) :
			Module::Parameters(x_name)
		{
			AddParameter(new ParameterT<CreationFunction>("create", R"({"name": "SVM", "number": 0, "parameters":{}})"_json, &create, "The parameters to pass to the create method method of ORB, BRIEF, ..."));
			AddParameter(new ParameterBool("train", true   , &train, "True if in training mode"));
			AddParameter(new ParameterString("modelFile", "model.data", &modelFile, "Path to the model file for testing"));
			AddParameter(new ParameterString("features", "x", &features, "List of features to use from the object. Separated by a comma."));
			AddParameter(new ParameterString("response", "y", &response, "Name of the response feature to train with."));

			/*
			AddParameter(new ParameterEnumT<cv::ml::SVM::KernelTypes>("kernelType", cv::ml::SVM::KernelTypes::LINEAR, &kernelType, "SVM kernel type"));
			AddParameter(new ParameterDouble("gamma", 0, 0, 1, &gamma, "SVM gamma. For SVM::POLY, SVM::RBF, SVM::SIGMOID or SVM::CHI2."));
			AddParameter(new ParameterDouble("coef0", 0, 0, 1, &coef0, "SVM coef0. For SVM::POLY or SVM::SIGMOID.."));
			AddParameter(new ParameterDouble("degree", 3, 0, 10, &degree, "SVM degree. For SVM::POLY."));
			AddParameter(new ParameterDouble("c", 0.01, 0, 1, &c, "SVM c. For SVM::C_SVC, SVM::EPS_SVR or SVM::NU_SVR."));
			AddParameter(new ParameterDouble("nu", 0.5, 0, 1, &nu, "SVM nu. For SVM::NU_SVC, SVM::ONE_CLASS or SVM::NU_SVR."));
			AddParameter(new ParameterDouble("p", 0.1, 0, 1, &p, "SVM p. For SVM::EPS_SVR."));
			*/
		}
		bool train;
		std::string modelFile;
		std::string features;
		std::string response;
		int kernelType;

		CreationFunction create;
	};

	explicit StatModel(ParameterStructure& xr_params);
	virtual ~StatModel();
	MKCLASS("StatModel")
	MKDESCR("Classify objects by using a StatModel on its features")
	MKCATEG("Classifier")

private:
	void TrainModel();
	void TestModel();

	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	void Reset() override;
	void ProcessFrame() override;

	// state
	cv::Ptr<cv::ml::StatModel> mp_statModel;
	cv::Mat m_samples;
	cv::Mat m_responses;
	size_t m_dataLength = 0;

	// input and output
	std::vector<Object> m_objects;

	// output

	// temp
	std::vector<std::string> m_featureNames;
	cv::Mat m_row;

	// debug
#ifdef MARKUS_DEBUG_STREAMS
#endif
};

} // namespace mk
#endif

