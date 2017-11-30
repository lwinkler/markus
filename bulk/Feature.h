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
#ifndef MK_FEATURE_H
#define MK_FEATURE_H

#include <map>
#include <string>
#include "define.h"
#include "feature_util.h"
#include "serialize.h"

/**
* @brief Class representing a feature of a template/object. (e.g. area, perimeter, length, ...)
*/
class Feature
{
public:
	friend inline void to_json(mkjson& _json, const Feature& _ser) {_ser.Serialize(_json);}
	friend inline void from_json(const mkjson& _json, Feature& _ser) {_ser.Deserialize(_json);}

	Feature() {}
	virtual ~Feature() {}
	virtual Feature* CreateCopy() const = 0;
	virtual double CompareSquared(const Feature& x_feature) const = 0;
	virtual void Randomize(unsigned int& xr_seed, const mkjson& x_param) = 0;
	virtual void Serialize(mkjson& _json) const = 0;
	virtual void Deserialize(const mkjson& _json) = 0;
};

/**
* @brief Class representing a feature pointer: used in vectors and maps
*/
class FeaturePtr final
{
public:
	friend inline void to_json(mkjson& _json, const FeaturePtr& _ser) {
		_ser->Serialize(_json);
	}
	friend inline void from_json(const mkjson& _json, FeaturePtr& _ser) {
		_ser->Deserialize(_json);
	}

	explicit FeaturePtr(Feature* x_feat) : mp_feat(x_feat) {}
	FeaturePtr(const FeaturePtr& x_feat) : mp_feat(x_feat->CreateCopy()) {}
	~FeaturePtr() {delete mp_feat;}
	FeaturePtr& operator = (const FeaturePtr& x_feat)
	{
		if(this != &x_feat)
		{
			delete(mp_feat);
			mp_feat = x_feat->CreateCopy();
		}
		return *this;
	}
	inline Feature& operator*  () const {return *mp_feat;}
	inline Feature* operator-> () const {return mp_feat;}

protected:
	Feature* mp_feat;
};

#endif
