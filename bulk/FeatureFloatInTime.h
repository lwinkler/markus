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
#ifndef MK_FEATURE_FLOAT_IN_TIME_H
#define MK_FEATURE_FLOAT_IN_TIME_H

#include <log4cxx/logger.h>
#include "Object.h"


class FeatureFloatInTime : public FeatureFloat
{
public:
	friend void to_json(mkjson& _json, const FeatureFloatInTime& _ser){_ser.Serialize(_json);}
	friend void from_json(const mkjson& _json, FeatureFloatInTime& _ser){_ser.Deserialize(_json);}

	explicit FeatureFloatInTime(const FeatureFloat& x_feat = FeatureFloat());
	Feature* CreateCopy() const {return new FeatureFloatInTime(*this);}
	void Update(const Feature& x_feat, double x_alpha);
	double CompareSquared(const Feature& x_feature) const override;
	void Randomize(unsigned int& xr_seed, const Json::Value& x_param) override;
	void Serialize(mkjson& _json) const override;
	void Deserialize(const mkjson& _json) override;

	float mean;
	float sqVariance;
	float initial;
	float min;
	float max;
	int   nbSamples;
};

#endif
