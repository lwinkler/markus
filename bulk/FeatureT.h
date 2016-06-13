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
	FeatureT() : value(T {} /*initialize to 0 or other*/) {}
	FeatureT(T x_value) : value(x_value) {}
	Feature* CreateCopy() const {return new FeatureT(*this);}
	inline virtual double CompareSquared(const Feature& x_feature) const
	{
		const FeatureT<T>& feat(dynamic_cast<const FeatureT<T>&>(x_feature));
		return compareSquared(value, feat.value);
	}
	inline void Randomize(unsigned int& xr_seed, const std::string& x_param)
	{
		randomize(value, xr_seed);
	}
	inline virtual void Serialize(std::ostream& x_out, MkDirectory* xp_dir = nullptr) const { serialize(x_out, value); }
	inline virtual void Deserialize(std::istream& x_in, MkDirectory* xp_dir = nullptr) { deserialize(x_in, value);}

	// The value of the feature
	T value;
};
#endif
