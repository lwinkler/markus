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


// note: see test_mltests2.cpp in opencv sources

#include "Svm.h"
#include "StreamObject.h"
// #include "StreamDebug.h"
#include "util.h"

#include <opencv2/ml.hpp>

using namespace std;
using namespace cv;
using namespace cv::ml;

log4cxx::LoggerPtr Svm::m_logger(log4cxx::Logger::getLogger("Svm"));

Svm::Svm(ParameterStructure& xr_params)
	: Module(xr_params),
	  m_param(dynamic_cast<Parameters&>(xr_params))
{
	// Init output images
	AddInputStream(0, new StreamObject("input", m_objects, *this,	"Objects input"));

	AddOutputStream(0, new StreamObject("output", m_objects, *this, "Classified objects output"));
}

Svm::~Svm()
{
	if(m_param.train)
		TrainModel();
	else
		TestModel();
}

void Svm::Reset()
{
	if(m_param.train)
		TrainModel();
	else
		TestModel();

	Module::Reset();
	if(m_param.train) {
		Ptr<SVM> model = SVM::create();
		// model->setType(m_param.type);
		/*
		model->setType(SVM::EPS_SVR);
		model->setKernel(m_param.kernelType);
		model->setDegree(m_param.degree);
		model->setGamma(m_param.gamma);
		model->setCoef0(m_param.coef0);
		model->setC(m_param.c);
		model->setNu(m_param.nu);
		model->setP(m_param.p);
		model->setTermCriteria(TermCriteria( CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 1000, 1e-3 ));
		*/
		/*
		model->setCoef0( 0.0 );
		model->setDegree( 3 );
		model->setTermCriteria( TermCriteria( CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 1000, 1e-3 ) );
		model->setGamma( 0 );
		model->setKernel( SVM::LINEAR );
		model->setNu( 0.5 );
		model->setP( 0.1 ); // for EPSILON_SVR, epsilon in loss function?
		model->setC( 0.01 ); // From paper, soft classifier
		model->setType( SVM::EPS_SVR ); // C_SVC; // EPSILON_SVR; // may be also NU_SVR; // do regression task
		*/
		                                     
		mp_statModel = model;
	} else {
		mp_statModel = SVM::load(m_param.modelFile);
	}
	split(m_param.features, ',', m_featureNames);

	m_dataLength = 0;
	m_samples    = Mat();
	m_responses  = Mat();
	m_row        = Mat();

}

void Svm::ProcessFrame()
{
	for(auto& obj: m_objects)
	{
		if(m_dataLength == 0)
		{
			// prepare sample matrix
			m_dataLength = m_featureNames.size();
			if(m_dataLength == 0)
				throw MkException("Data has size zero", LOC);
			m_samples   = Mat(0, m_dataLength, CV_32FC1);
			m_responses = Mat(0, 1, CV_32SC1);
			m_row = Mat(1, m_dataLength, CV_32FC1);
			m_row.setTo(0);
		}

		// append data
		int i = 0;
		assert(static_cast<int>(m_featureNames.size()) == m_row.cols);
		for(const auto& name : m_featureNames)
		{
			float dat = dynamic_cast<const FeatureFloat&>(obj.GetFeature(name)).value;
			// cout << "res feat " << res << endl;
			m_row.at<float>(0, i) = dat;
			i++;
		}
		// if(m_param.train) {
			m_samples.push_back(m_row);
			// int response = dynamic_cast<const FeatureFloat&>(obj.GetFeature("x")).value + dynamic_cast<const FeatureFloat&>(obj.GetFeature("y")).value > 0.5;
			// int response = dynamic_cast<const FeatureFloat&>(obj.GetFeature("x")).value + dynamic_cast<const FeatureFloat&>(obj.GetFeature("x")).value > 0.5;
		float res = dynamic_cast<const FeatureFloat&>(obj.GetFeature(m_param.response)).value;
        m_responses.push_back(static_cast<int32_t>(res > 0.4));
// cout << "row " << m_row<< " res " << m_responses<< endl;
			                        
			// m_responses.push_back(response);
			// m_responses.push_back(dynamic_cast<const FeatureFloat&>(obj.GetFeature("x")).value > 0.1 ? 1 : 0);
		// } else {
			// float res = 2; // TODO dynamic_cast<const FeatureFloat&>(obj.GetFeature(m_param.response)).value;
			// obj.AddFeature("label", res);
		// }
		if(!m_param.train) {
			Mat est;
			cout << "predict " << mp_statModel->predict(m_row, est/*, StatModel::RAW_OUTPUT*/) << " ";
			cout << est  << endl;
		}
	}
}


void Svm::TrainModel()
{
	if(m_samples.empty())
		return;

	cout << "Samples" << m_samples << endl;
	cout << "Responses" << m_responses << endl;
	Ptr<TrainData> data = TrainData::create(m_samples, ml::ROW_SAMPLE, m_responses); //, InputArray varIdx=noArray(), InputArray sampleIdx=noArray(), InputArray sampleWeights=noArray(), InputArray varType=noArray());
	bool is_trained = mp_statModel.dynamicCast<SVM>()->train(data, 10);

	cout << "trained" << is_trained << endl;

	if(!is_trained)
	{
		throw MkException("Unable to train model", LOC);
	}

	float err = mp_statModel->calcError(data, true, m_responses);
	LOG_INFO(m_logger, "Error of model on train data: " << err << "%");

	string modelFile = RefContext().RefOutputDir().ReserveFile("model.data");
	mp_statModel->save(modelFile);
}

void Svm::TestModel()
{
	if(m_samples.empty())
		return;

	cout << "Samples" << m_samples << endl;
	cout << "Responses" << m_responses << endl;
	Ptr<TrainData> data = TrainData::create(m_samples, ROW_SAMPLE, m_responses); //, InputArray varIdx=noArray(), InputArray sampleIdx=noArray(), InputArray sampleWeights=noArray(), InputArray varType=noArray());
	
	float err = mp_statModel->calcError(data, true, m_responses);
	LOG_INFO(m_logger, "Error of model on test data: " << err << "%");
}

