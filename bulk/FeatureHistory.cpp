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

void FeatureHistory::Serialize(ostream& x_out, MkDirectory* xp_dir) const
{
	if(features.empty())
	{
		x_out<<"{\"history\":[]}";
		return;
	}

	x_out << "{\"history\":[";
	size_t i = 0;
	for(const auto& elem : features)
	{
		x_out << "{\"time\":"<<elem.first<<",\"feature\":";
		elem.second->Serialize(x_out);
		if(i == features.size() - 1)
			x_out << "}";
		else
			x_out << "},";
		i++;
	}
	x_out << "]}";
}

void FeatureHistory::Deserialize(istream& x_in, MkDirectory* xp_dir)
{
	Json::Value root0;
	x_in >> root0;  // note: copy first for local use
	Json::Value root = root0["history"];
	assert(root.isArray());

	features.clear();
	FactoryFeatures& factory(Factories::featuresFactoryBySignature());
	for(unsigned int i = 0 ; i < root.size() ; i++)
	{
		stringstream ss;
		ss << root[i]["feature"];
		string signature = Serializable::signature(ss);
		Feature* feat = factory.Create(signature);
		stringstream ss2;
		ss2 << root[i]["feature"];
		feat->Deserialize(ss2, xp_dir);
		features.insert(std::make_pair(root[i]["time"].asInt(), feat));
	}
}
