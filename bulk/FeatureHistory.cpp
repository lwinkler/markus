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
#include "FeatureHistory.h"
#include "FeatureStd.h"
#include "Factories.h"
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

#define INI_VARIANCE 0.01
#define MIN_VARIANCE 0.0001

using namespace std;

log4cxx::LoggerPtr FeatureHistory::m_logger(log4cxx::Logger::getLogger("FeatureHistory"));


/**
* @brief Keep a feature up to date in a dynamic way (similar to a running average)
*
* @param x_currentValue Value to use for updating
* @param x_alpha        Alpha coefficient for running average
*/
void FeatureHistory::Update(const TIME_STAMP& x_timeStamp, const Feature& x_feat)
{
	auto it = features.find(x_timeStamp);
	if(it != features.end())
		LOG_WARN(m_logger, "A feature already exists for this time stamp.");

	features.insert(std::make_pair(x_timeStamp, x_feat.CreateCopy()));
}

double FeatureHistory::CompareSquared(const Feature& x_feature) const
{
	const FeatureHistory& feat(dynamic_cast<const FeatureHistory&>(x_feature));
	if(features.size() != feat.features.size())
		return 1;
	if(features.empty())
		return 0;

	auto it = feat.features.begin();
	double sum = 0;
	for(const auto& elem : features)
	{
		sum += elem.second->CompareSquared(*it->second);
		it++;
	}
	return sum / POW2(features.size());
}

void FeatureHistory::Randomize(unsigned int& xr_seed, const Json::Value& x_param)
{
	features.clear();
	for(int i = 0 ; i  < 100 ; i++)
	{
		FeatureFloat* feat = new FeatureFloat();
		feat->Randomize(xr_seed, "[0:1]");
		features.insert(std::make_pair(i * 1000, feat));
	}
}

void FeatureHistory::Serialize(mkjson& rx_json) const
{
	rx_json["history"] = nlohmann::json::array();
	mkjson hist = rx_json["history"];
	for(const auto& elem : features)
	{
		mkjson feat_json{{"time", elem.first}};
		elem.second->Serialize(feat_json["feature"]);
		hist.push_back(feat_json);
	}
}

void FeatureHistory::Deserialize(const mkjson& x_json)
{
	mkjson hist = x_json.at("history");
	assert(hist.is_array());

	features.clear();
	FactoryFeatures& factory(Factories::featuresFactoryBySignature());
	for(const auto& elem : hist)
	{
		string sign= signature(elem.at("feature")); // TODO rename function signature
		Feature* feat = factory.Create(sign);
		feat->Deserialize(elem.at("feature"));
		features.insert(std::make_pair(elem.at("time").get<TIME_STAMP>(), feat));
	}
}
