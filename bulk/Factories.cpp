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

#include "Factories.h"

#include "FeatureStd.h"
#include "FeatureFloatInTime.h"
#include "FeatureVector.h"
#include "FeatureOpenCv.h"
#include "FeatureHistory.h"
#define REGISTER_FEATURE(child, type){\
{\
fact1.Register<child>(type);\
Feature* pfeat = new child();\
fact2.Register<child>(signatureOfFeature(*pfeat));\
delete pfeat;\
}}
// cout << type << ":"  << signatureOfFeature(*pfeat) << endl;\

#include "ControllerParameterT.h"
#include "Polygon.h"
// TODO: Remove classnames map

// Factories: The RegisterAll method will register all necessary creators


using namespace std;

// log4cxx::LoggerPtr Processable::m_logger(log4cxx::Logger::getLogger("Processable"));

// This method is implemented in AllModules.cpp (generated automatically)
void registerAllModules(FactoryModules& x_fact1, FactoryParameters& x_fact2);

void Factories::RegisterAll()
{
	// All modules
	registerAllModules(Factories::modulesFactory(), Factories::parametersFactory());

	// All features
	FactoryFeatures& fact1(Factories::featuresFactory());
	FactoryFeatures& fact2(Factories::featuresFactoryBySignature());

	REGISTER_FEATURE(FeatureFloat, "FeatureFloat");
	REGISTER_FEATURE(FeatureInt, "FeatureInt");
	REGISTER_FEATURE(FeatureFloatInTime, "FeatureFloatInTime");
	REGISTER_FEATURE(FeatureVectorFloat, "FeatureVectorFloat");
	REGISTER_FEATURE(FeatureVectorInt, "FeatureVectorInt");
	REGISTER_FEATURE(FeatureString, "FeatureString");
	REGISTER_FEATURE(FeatureKeyPoint, "FeatureKeyPoint");
	REGISTER_FEATURE(FeaturePoint2f, "FeaturePoint2f");
	REGISTER_FEATURE(FeaturePoint3f, "FeaturePoint3f");
	REGISTER_FEATURE(FeatureHistory, "FeatureHistory");
	// REGISTER_FEATURE(FeatureMat, "FeatureMat"); // Experimental

	// All controllers
	FactoryParameterController& factParamCtr(Factories::parameterControllerFactory());
	factParamCtr.Register<ControllerBool>("ParameterBool");
	factParamCtr.Register<ControllerDouble>("ParameterDouble");
	factParamCtr.Register<ControllerFloat>("ParameterFloat");
	// factParamCtr.Register<ControllerEnum>("ParameterEnum");

	// enums
	factParamCtr.Register<ControllerEnum>("ParameterCachedState");
	factParamCtr.Register<ControllerEnum>("ParameterImageType");
	factParamCtr.Register<ControllerEnum>("ParameterKernelTypes");
	factParamCtr.Register<ControllerEnum>("ParameterSvmTypes");

	factParamCtr.Register<ControllerInt>("ParameterInt");
	factParamCtr.Register<ControllerUInt>("ParameterUInt");
	// factParamCtr.Register<ControllerSerializable>(PARAM_SERIALIZABLE);
	// factParamCtr.Register<ControllerCalibrationByHeight>(PARAM_OBJECT_HEIGHT);
	factParamCtr.Register<ControllerString>("ParameterString");
	factParamCtr.Register<ControllerParameterT<ParameterT<Polygon>>>("ParameterPolygon");
	factParamCtr.Register<ControllerParameterT<Stream>>("StreamObjects");
	factParamCtr.Register<ControllerParameterT<Stream>>("StreamImage");
	factParamCtr.Register<ControllerParameterT<Stream>>("StreamEvent");
	factParamCtr.Register<ControllerParameterT<Stream>>("StreamState");
}

