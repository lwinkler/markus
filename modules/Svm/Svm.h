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

#ifndef SVM_H
#define SVM_H

#include "Module.h"
#include "Parameter.h"
#include "StreamObject.h"

/*! \class Svm
 *  \brief Classification using Svm
 *
 */



/**
* @brief Classify objects by using a SVM on its features
*/
class Svm : public Module
{
public:
	class Parameters : public Module::Parameters
	{

	public:
		Parameters(const std::string& x_name) :
			Module::Parameters(x_name)
		{
			AddParameter(new ParameterBool("train", 1, 0, 1, &train, "True if in training mode"));
			AddParameter(new ParameterString("modelFile", "model.data", &modelFile, "Path to the model file for testing"));
			AddParameter(new ParameterString("features", "x", &features, "List of features to use from the object. Separated by a comma."));
			AddParameter(new ParameterString("response", "y", &response, "Name of the response feature to train with."));

			AddParameter(new ParameterDouble("gamma", 1, 0, 1, &gamma, "SVM gamma. For SVM::POLY, SVM::RBF, SVM::SIGMOID or SVM::CHI2."));
			AddParameter(new ParameterDouble("coef0", 0, 0, 1, &coef0, "SVM coef0. For SVM::POLY or SVM::SIGMOID.."));
			AddParameter(new ParameterDouble("degree", 0, 0, 1, &degree, "SVM degree. For SVM::POLY."));
			AddParameter(new ParameterDouble("c", 0, 0, 1, &c, "SVM c. For SVM::C_SVC, SVM::EPS_SVR or SVM::NU_SVR."));
			AddParameter(new ParameterDouble("nu", 0, 0, 1, &nu, "SVM nu. For SVM::NU_SVC, SVM::ONE_CLASS or SVM::NU_SVR."));
			AddParameter(new ParameterDouble("p", 0, 0, 1, &p, "SVM p. For SVM::EPS_SVR."));
		}
		bool train;
		std::string modelFile;
		std::string features;
		std::string response;

		// svm
		double gamma;
		double coef0;
		double degree;
		double c;
		double nu;
		double p;
	};

	Svm(ParameterStructure& xr_params);
	virtual ~Svm();
	MKCLASS("Svm")
	MKDESCR("Classify objects by using a SVM on its features")
	MKCATEG("Classifier")

	void Reset() override;
	void ProcessFrame() override;

private:
	void TrainModel();
	void TestModel();

	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	// state
	cv::Ptr<cv::ml::StatModel> mp_statModel;
	cv::Mat m_samples;
	cv::Mat m_responses;
	short m_dataLength = 0;

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

#endif

