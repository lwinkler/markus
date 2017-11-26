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
#include "feature_util.h"
#include <vector>

/**
* @brief Class representing a feature in the form of a vector of primitive type
*/
template<class T> class FeatureVectorT : public Feature
{
public:
	inline friend void to_json(mkjson& _json, const FeatureVectorT<T>& _ser){_ser.Serialize(_json);}
	inline friend void from_json(const mkjson& _json, FeatureVectorT<T>& _ser) {_ser.Deserialize(_json);}

	explicit FeatureVectorT(const std::vector<T>& x_values = std::vector<T>(0)) : values(x_values) {}
	Feature* CreateCopy() const {return new FeatureVectorT<T>(*this);}
	inline virtual double CompareSquared(const Feature& x_feature) const
	{
		const FeatureVectorT<T>& feat(dynamic_cast<const FeatureVectorT<T>&>(x_feature));
		return compareSquared(values, feat.values);
	}
	inline virtual void Randomize(unsigned int& xr_seed, const mkjson& x_param)
	{
		if(x_param.empty())
		{
			randomize(values, xr_seed);
		}
		else
		{
			int size = x_param["size"].get<int>();
			randomize(values, xr_seed, size);
		}
	}
	inline virtual void Serialize(mkjson& _json) const {nlohmann::to_json(_json, values);}
	inline virtual void Deserialize(const mkjson& _json) {nlohmann::from_json(_json, values);}

	// The value of the feature
	std::vector<T> values;
};

template<>void FeatureVectorT<int>::Serialize(mkjson& rx_json) const;
template<>void FeatureVectorT<int>::Deserialize(const mkjson& x_json);


// Definitions
typedef FeatureVectorT<float>   FeatureVectorFloat;
typedef FeatureVectorT<int>   FeatureVectorInt;


#endif
