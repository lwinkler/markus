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
#ifndef ALL_FEATURES_H
#define ALL_FEATURES_H

#include "Factories.h"
#include "FeatureStd.h"
#include "FeatureFloatInTime.h"
#include "FeatureVector.h"
#include "FeatureOpenCv.h"


#define REGISTER_FEATURE(child, type){\
{\
fact1.Register<child>(type);\
Feature* pfeat = new child();\
fact2.Register<child>(pfeat->Signature());\
delete pfeat;\
}}

void registerAllFeatures()
{
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
	// REGISTER_FEATURE(FeatureMat, "FeatureMat"); // Experimental
}
#endif
