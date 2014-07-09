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
		it++;
	}
	x_out << *it << "]";
}

void FeatureVectorFloat::Deserialize(istream& x_in, const string& x_dir)
{
	string tmp;
	vector<string> valuesStr;
	x_in >> tmp;

	if(tmp.substr(0, 1) != "[" || tmp.substr(tmp.size() - 1, 1) != "]")
		throw MkException("Error in deserialization for value: " + tmp, LOC);
	split(tmp.substr(1, tmp.size() - 2), ',', valuesStr);

	// Remove last element if empty, due to an extra comma
	assert(values.size() > 0);
	if(valuesStr.back() == "")
		values.pop_back();
	assert(values.size() > 0);

	values.clear();
	for(vector<string>::const_iterator it = valuesStr.begin() ; it != valuesStr.end() ; it++)
		values.push_back(atof(it->c_str()));
}
