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
#ifndef MK_FEATURE_HOF_H
#define MK_FEATURE_HOF_H

#include "Feature.h"
#include "MkException.h"
#include "util.h"
#include "feature_util.h"
#include <vector>

#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

/**
* @brief Class representing a feature in the form of a vector of float
*/

/**
* @brief Class representing a feature template. This can be used to create new templates
*/
template<class T> class FeatureVectorT : public Feature
{
public:
	FeatureVectorT(const std::vector<T>& x_values = std::vector<T>(0)) : values(x_values) {}
	Feature* CreateCopy() const {return new FeatureVectorT<T>(*this);}
	inline virtual double CompareSquared(const Feature& x_feature) const
	{
		const FeatureVectorT<T>& feat(dynamic_cast<const FeatureVectorT<T>&>(x_feature));
		return compareSquared(values, feat.values);
	}
	inline virtual void Randomize(unsigned int& xr_seed, const std::string& x_param)
	{
		if(x_param.empty())
		{
			randomize(values, xr_seed);
		}
		else
		{
			Json::Reader reader;
			Json::Value root;
			bool parsingSuccessful = reader.parse(x_param.c_str(), root);
			assert(parsingSuccessful);
			int size = root["size"].asInt();
			randomize(values, xr_seed, size);
		}
	}
	inline virtual void Serialize(std::ostream& x_out, const std::string& x_dir) const
	{
		serialize(x_out, values);
	}
	inline virtual void Deserialize(std::istream& x_in, const std::string& x_dir)
	{
		deserialize(x_in, values);
	}

	// The value of the feature
	std::vector<T> values;
};


// Definitions
typedef FeatureVectorT<float>   FeatureVectorFloat;
// note: FeatureVectorInt cannot be used: so far the signature of this feature is the same as FeatureVectorFloat // TODO fix or suppress these
typedef FeatureVectorT<int>   FeatureVectorInt;


#endif
