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

#include "AllModules.h"

#include "FeatureStd.h"
#include "FeatureFloatInTime.h"
#include "FeatureVector.h"
#include "FeatureOpenCv.h"
#include "FeatureHistory.h"
#define REGISTER_FEATURE(child, type){\
{\
fact1.Register<child>(type);\
Feature* pfeat = new child();\
fact2.Register<child>(pfeat->Signature());\
delete pfeat;\
}}

#include "ControllerParameters.h"


// Factories: The RegisterAll method will register all necessary creators


using namespace std;

// log4cxx::LoggerPtr Processable::m_logger(log4cxx::Logger::getLogger("Processable"));

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
	REGISTER_FEATURE(FeatureString, "FeatureString");
	REGISTER_FEATURE(FeatureKeyPoint, "FeatureKeyPoint");
	REGISTER_FEATURE(FeaturePoint2f, "FeaturePoint2f");
	REGISTER_FEATURE(FeaturePoint3f, "FeaturePoint3f");
	REGISTER_FEATURE(FeatureHistory, "FeatureHistory");
	// REGISTER_FEATURE(FeatureMat, "FeatureMat"); // Experimental

	// All controllers
	FactoryParameterController& factParamCtr(Factories::parameterControllerFactory());
	factParamCtr.Register<ControllerBool>(PARAM_BOOL);
	factParamCtr.Register<ControllerDouble>(PARAM_DOUBLE);
	factParamCtr.Register<ControllerFloat>(PARAM_FLOAT);
	factParamCtr.Register<ControllerEnum>(PARAM_ENUM);
	factParamCtr.Register<ControllerInt>(PARAM_INT);
	factParamCtr.Register<ControllerUInt>(PARAM_UINT);
	factParamCtr.Register<ControllerSerializable>(PARAM_SERIALIZABLE);
	factParamCtr.Register<ControllerCalibrationByHeight>(PARAM_OBJECT_HEIGHT);
	factParamCtr.Register<ControllerString>(PARAM_STR);
}

