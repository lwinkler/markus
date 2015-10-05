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
		throw MkException("A feature already exists for this time stamp.", LOC);

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

void FeatureHistory::Randomize(unsigned int& xr_seed, const string& x_param)
{
	features.clear();
	for(int i = 0 ; i  < 100 ; i++)
	{
		FeatureFloat* feat = new FeatureFloat();
		feat->Randomize(xr_seed, "[0:1]");
		features.insert(std::make_pair(i * 1000, feat));
	}
}

void FeatureHistory::Serialize(MkJson& xr_out, const string& x_dir) const
{
	if(features.empty())
		xr_out["history"] = MkJson::emptyArray();

	for(const auto& elem : features)
	{
		MkJson root;
		root["time"] = elem.first;
		root["feature"] = elem.second.Serialize(root.create("feature"));
		xr_out["history"].Append(root);
	}
}

void FeatureHistory::Deserialize(MkJson& xr_in, const string& x_dir)
{
	features.clear();
	FactoryFeatures& factory(Factories::featuresFactoryBySignature());
	for(unsigned int i = 0 ; i < xr_in.Size() ; i++)
	{
		ss << xr_in[i]["feature"];
		string signature = Serializable::signature(ss);
		Feature* feat = factory.Create(signature);
		feat->Deserialize(root[i]["feature"]);
		features.insert(std::make_pair(xr_in[i]["time"].AsInt(), feat));
	}
}
