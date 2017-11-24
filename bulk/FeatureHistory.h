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
#ifndef MK_FEATURE_HISTORY_H
#define MK_FEATURE_HISTORY_H

#include <log4cxx/logger.h>
#include "Feature.h"


class FeatureHistory : public Feature
{
public:
	friend void to_json(mkjson& _json, const FeatureHistory& _ser){_ser.Serialize(_json);}
	friend void from_json(const mkjson& _json, FeatureHistory& _ser){_ser.Deserialize(_json);}

	FeatureHistory() {}
	Feature* CreateCopy() const {return new FeatureHistory(*this);}
	void Update(const TIME_STAMP& x_timeStamp, const Feature& x_feat);
	double CompareSquared(const Feature& x_feature) const override;
	void Randomize(unsigned int& xr_seed, const Json::Value& x_param) override;
	void Serialize(mkjson& _json) const override;
	void Deserialize(const mkjson& _json) override;

	std::map <TIME_STAMP, FeaturePtr> features;
private:
	static log4cxx::LoggerPtr m_logger;
};

#endif
