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
#ifndef TEST_FEATURES_H
#define TEST_FEATURES_H

#include <cxxtest/TestSuite.h>
#include "Global.test.h"
#include "util.h"
#include "Factories.h"
#include "MkException.h"
#include "FeatureStd.h"
#include "FeatureFloatInTime.h"
#include "FeatureVector.h"
#include "FeatureOpenCv.h"

/// Test class for serialization

using namespace std;

class FeatureTestSuite : public CxxTest::TestSuite
{
public:
	FeatureTestSuite() :
		m_factoryFeatures(Factories::featuresFactory()),
		m_factoryFeaturesBySignature(Factories::featuresFactoryBySignature())
	{}

protected:
	const FactoryFeatures& m_factoryFeatures;
	const FactoryFeatures& m_factoryFeaturesBySignature;

	/// Test the serialization of one serializable class
	void testFeature(Feature& feat, const string& name, unsigned int& xr_seed)
	{
		// Note: features need to be initialized to 0 or similar value
		TS_TRACE("Test feature of type " + name + " = " + feat.SerializeToString() + " with signature = " + feat.Signature());
		Feature* copy = feat.CreateCopy();
		// cout << "feat.CompareSquared(feat) = " << feat.CompareSquared(feat) << endl;
		TS_ASSERT(feat.CompareSquared(feat) == 0);
		TS_ASSERT(feat.CompareSquared(*copy) == 0);
		copy->Randomize(xr_seed, Json::nullValue);
		TS_TRACE("compare with "+copy->SerializeToString() + " with signature = " + feat.Signature());
		// cout << "feat.CompareSquared(*copy) = " << feat.CompareSquared(*copy) << endl;
		TS_ASSERT(feat.CompareSquared(*copy) > 0);
		delete copy;
	}

public:
	void setUp()
	{
	}
	void tearDown()
	{
	}


	void testSerialization()
	{
		TS_TRACE("Test the serialization of features");
		vector<string> listFeatures;
		m_factoryFeatures.List(listFeatures);
		unsigned int seed = 23456625;
		for(const auto& elem : listFeatures)
		{
			TS_TRACE("Test the serialization of feature " + elem);
			Feature* feat = m_factoryFeatures.Create(elem);
			testFeature(*feat, elem, seed);
			delete(feat);
		}
	}

	void testSerializationCreateBySignature()
	{
		TS_TRACE("Test the serialization of features (creation by signature)");
		vector<string> listFeatures;
		m_factoryFeatures.List(listFeatures);
		unsigned int seed = 23456644;
		for(const auto& elem : listFeatures)
		{
			TS_TRACE("Test the serialization of feature " + elem);
			Feature* feat = m_factoryFeatures.Create(elem);

			string signature = feat->Signature();
			delete(feat);

			// Generate feature by signature
			feat = m_factoryFeaturesBySignature.Create(signature);
			testFeature(*feat, elem, seed);
			delete(feat);
		}
	}
};
#endif
