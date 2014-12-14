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

#include <cppunit/TestFixture.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>

#include "util.h"
#include "MkException.h"
#include "FeatureStd.h"
#include "FeatureFloatInTime.h"
#include "FeatureVector.h"
#include "FeatureOpenCv.h"

/// Test class for serialization

class TestFeatures : public CppUnit::TestFixture
{
public:
	TestFeatures() :
		m_factoryFeatures(Factories::featuresFactory())
	{}

private:
		static log4cxx::LoggerPtr m_logger;
		FactoryFeatures& m_factoryFeatures;

public:
	void setUp()
	{
	}
	void tearDown()
	{
	}

	/// Test the serialization of one serializable class
	void testFeature(Feature& feat, const std::string& name, unsigned int& xr_seed)
	{
		// Initialize the feature with random value since int/floats are not initialized by default // TODO: maybe fix this
		feat.Randomize(xr_seed, ""); 
		LOG_TEST(m_logger, "Test feature of type "<<name<<" = "<<feat.SerializeToString() << " with signature = " << feat.Signature());
		Feature* copy = feat.CreateCopy();
		LOG_DEBUG(m_logger, "feat.CompareSquared(feat) = " << feat.CompareSquared(feat));
		CPPUNIT_ASSERT(feat.CompareSquared(feat) == 0);
		CPPUNIT_ASSERT(feat.CompareSquared(*copy) == 0);
		copy->Randomize(xr_seed, "");
		LOG_TEST(m_logger, " compare with "<<copy->SerializeToString() << " with signature = " << feat.Signature());
		LOG_DEBUG(m_logger, "feat.CompareSquared(*copy) = " << feat.CompareSquared(*copy));
		CPPUNIT_ASSERT(feat.CompareSquared(*copy) > 0);
		delete copy;
	}


	void testSerialization1()
	{
		std::vector<std::string> listFeatures;
		m_factoryFeatures.ListFeatures(listFeatures);
		unsigned int seed = 23456625;
		for(std::vector<std::string>::const_iterator it = listFeatures.begin() ; it != listFeatures.end() ; ++it)
		{
			if(*it == "FeatureString") continue; // TODO: randomize strings
			Feature* feat = m_factoryFeatures.CreateFeature(*it);
			testFeature(*feat, *it, seed);
			delete(feat);
		}
	}

	static CppUnit::Test *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite("TestFeatures");
		suiteOfTests->addTest(new CppUnit::TestCaller<TestFeatures>("testSerialization1", &TestFeatures::testSerialization1));
		return suiteOfTests;
	}
};
log4cxx::LoggerPtr TestFeatures::m_logger(log4cxx::Logger::getLogger("TestFeatures"));
#endif
