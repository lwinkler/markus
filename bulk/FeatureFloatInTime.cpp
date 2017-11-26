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
#include "FeatureFloatInTime.h"
#include "FeatureStd.h"

#define INI_VARIANCE 0.01
#define MIN_VARIANCE 0.0001

using namespace std;

FeatureFloatInTime::FeatureFloatInTime(const FeatureFloat& x_feat)
	: FeatureFloat(x_feat)
{
	sqVariance = INI_VARIANCE;
	mean       = value;
	initial    = value;
	min        = value;
	max        = value;
	nbSamples  = 1;
}

/**
* @brief Keep a feature up to date in a dynamic way (similar to a running average)
*
* @param x_currentValue Value to use for updating
* @param x_alpha        Alpha coefficient for running average
*/
void FeatureFloatInTime::Update(const Feature& x_feat, double x_alpha)
{
	const FeatureFloat& ff(dynamic_cast<const FeatureFloat&>(x_feat));
	value      = ff.value;
	mean       = mean * (1.0 - x_alpha) + value * x_alpha;
	sqVariance = sqVariance * (1.0 - x_alpha) + POW2(ff.value - mean) * x_alpha;

	// setting a min to the variance as a security
	if(sqVariance < MIN_VARIANCE)
		sqVariance = MIN_VARIANCE;
	if(ff.value < min)
		min        = ff.value;
	if(ff.value > max)
		max        = ff.value;
	nbSamples++;
}

double FeatureFloatInTime::CompareSquared(const Feature& x_feature) const
{
	const FeatureFloat& feat(dynamic_cast<const FeatureFloat&>(x_feature));
	// cout<<value<<" "<<feat.value<<" "<<" var:"<<sqVariance<<" --> "<<POW2(value - feat.value) / POW2(sqVariance)<<endl;
	return POW2(value - feat.value) / POW2(sqVariance);
}

void FeatureFloatInTime::Randomize(unsigned int& xr_seed, const mkjson& x_param)
{
	FeatureFloat::Randomize(xr_seed, nullptr);

	// Create a float feature and update
	double alpha = static_cast<float>(rand_r(&xr_seed)) / RAND_MAX;
	FeatureFloat ff;
	for(int i = rand_r(&xr_seed) % 20 + 2 ; i != 0 ; i--)
	{
		ff.Randomize(xr_seed, nullptr);
		Update(ff, alpha);
	}
}

void FeatureFloatInTime::Serialize(mkjson& rx_json) const
{
	rx_json = mkjson{
		{"value", value},
		{"mean", mean},
		{"sqVariance", sqVariance},
		{"initial", initial},
		{"min", min},
		{"max", max},
		{"nbSamples", nbSamples}
	};
}

void FeatureFloatInTime::Deserialize(const mkjson& x_json)
{
	value = x_json.at("value").get<float>();
	mean = x_json.at("mean").get<float>();
	sqVariance = x_json.at("sqVariance").get<float>();
	initial = x_json.at("initial").get<float>();
	min = x_json.at("min").get<float>();
	max = x_json.at("max").get<float>();
	nbSamples = x_json.at("nbSamples").get<int>();
}
