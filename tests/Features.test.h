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
		m_factoryFeatures(Factories::featuresFactory()){}

protected:
	const FactoryFeatures& m_factoryFeatures;

	/// Test the serialization of one serializable class
	void testFeature(Feature& feat, const string& name, unsigned int& xr_seed)
	{
		// Initialize the feature with random value since int/floats are not initialized by default // TODO: maybe fix this
		feat.Randomize(xr_seed, ""); 
		TS_TRACE("Test feature of type " + name + " = " + feat.SerializeToString() + " with signature = " + feat.Signature());
		Feature* copy = feat.CreateCopy();
		// TS_TRACE("feat.CompareSquared(feat) = " + feat.CompareSquared(feat));
		TS_ASSERT(feat.CompareSquared(feat) == 0);
		TS_ASSERT(feat.CompareSquared(*copy) == 0);
		copy->Randomize(xr_seed, "");
		TS_TRACE("compare with "+copy->SerializeToString() + " with signature = " + feat.Signature());
		// TS_TRACE("feat.CompareSquared(*copy) = " + feat.CompareSquared(*copy));
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
		for(vector<string>::const_iterator it = listFeatures.begin() ; it != listFeatures.end() ; ++it)
		{
			TS_TRACE("Test the serialization of feature "+*it);
			if(*it == "FeatureString") continue; // TODO: randomize strings
			Feature* feat = m_factoryFeatures.Create(*it);
			testFeature(*feat, *it, seed);
			delete(feat);
		}
	}
};
#endif
