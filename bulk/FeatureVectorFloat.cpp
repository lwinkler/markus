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
#include "FeatureVectorFloat.h"
#include "MkException.h"
#include "util.h"

using namespace std;

FeatureVectorFloat::FeatureVectorFloat(vector<float> x_values)
	: Feature()
{
	values = x_values;
}

// TODO: Test feature comparison
double FeatureVectorFloat::Compare2(const Feature& x_feature) const
{
	const FeatureVectorFloat& feat(dynamic_cast<const FeatureVectorFloat&>(x_feature));
	double sum = 0;
	if(values.size() != feat.values.size())
		return 1;
		// throw MkException("Size error while comparing FeatureVectorFloats", LOC);

	vector<float>::const_iterator it2 = feat.values.begin();
	for(vector<float>::const_iterator it1 = values.begin() ; it1 != values.end() ; ++it1, ++it2)
	{
		sum += POW2(*it1 - *it2);
	}
	return sum / POW2(values.size());
}

void FeatureVectorFloat::Randomize(unsigned int& xr_seed, const std::string& x_param)
{
	values.clear();
	int limit = 10; //  rand_r(xp_seed) % 100;
	for(int i = 0 ; i < limit ; i++)
		values.push_back(static_cast<float>(rand_r(&xr_seed)) / RAND_MAX);
}


void FeatureVectorFloat::Serialize(ostream& x_out, const string& x_dir) const
{
	if(values.size() == 0)
	{
		x_out<<"[]";
		return;
	}

	x_out << "[";
	vector<float>::const_iterator it = values.begin();
	while(it != values.end() - 1)
	{
		x_out << *it << ",";
		++it;
	}
	x_out << *it << "]";
}

void FeatureVectorFloat::Deserialize(istream& x_in, const string& x_dir)
{
	string tmp;
	vector<string> valuesStr;
	getline(x_in, tmp);

	if(tmp.substr(0, 1) != "[" || tmp.substr(tmp.size() - 1, 1) != "]")
		throw MkException("Error in deserialization for value: " + tmp, LOC);
	split(tmp.substr(1, tmp.size() - 2), ',', valuesStr);

	// Remove last element if empty, due to an extra comma
	if(valuesStr.back() == "")
		values.pop_back();

	values.clear();
	for(vector<string>::const_iterator it = valuesStr.begin() ; it != valuesStr.end() ; ++it)
		values.push_back(atof(it->c_str()));
}
