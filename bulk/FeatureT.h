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
#ifndef MK_FEATURET_H
#define MK_FEATURET_H

#include "Feature.h"
#include "feature_util.h"

/**
* @brief Class representing a feature template. This can be used to create new templates
*/
template<class T> class FeatureT : public Feature
{
public:
	inline friend void to_json(mkjson& _json, const FeatureT<T>& _ser){_ser.Serialize(_json);}
	inline friend void from_json(const mkjson& _json, FeatureT<T>& _ser) {_ser.Deserialize(_json);}
	
	FeatureT() : value(T {} /*initialize to 0 or other*/) {}
	explicit FeatureT(T x_value) : value(x_value) {}
	Feature* CreateCopy() const {return new FeatureT(*this);}
	inline virtual double CompareSquared(const Feature& x_feature) const
	{
		const FeatureT<T>& feat(dynamic_cast<const FeatureT<T>&>(x_feature));
		return compareSquared(value, feat.value);
	}
	inline void Randomize(unsigned int& xr_seed, const Json::Value& x_param)
	{
		randomize(value, xr_seed);
	}
	inline virtual void Serialize(mkjson& rx_json) const {
		nlohmann::to_json(rx_json, value);
	}
	inline virtual void Deserialize(const mkjson& x_json) {
		nlohmann::from_json(x_json, value);
	}

	// The value of the feature
	T value;
};

// Special case for int to change the signature
template<> inline void FeatureT<int>::Serialize(mkjson& rx_json) const {rx_json["valueInt"] = value;}
template<> inline void FeatureT<int>::Deserialize(const mkjson& rx_json) {value = rx_json.at("valueInt").get<int>();}
#endif
