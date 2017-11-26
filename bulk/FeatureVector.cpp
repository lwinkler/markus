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
#include "FeatureVector.h"
#include "MkException.h"
#include "util.h"

using namespace std;

template<class T>std::ostream& serializeSimple(std::ostream& x_out, const std::vector<T>& x_val)
{
	if(x_val.size() == 0)
	{
		x_out<<"[]";
		return x_out;
	}

	x_out << "[";
	auto it = x_val.begin();
	while(it != x_val.end() - 1)
	{
		x_out << *it << ",";
		++it;
	}
	x_out << *it;
	x_out << "]";
	return x_out;
}

template<class T>std::istream& deserializeSimple(std::istream& x_in,  std::vector<T>& xr_val)
{
	mkjson root;
	root << x_in;  // note: copy first for local use
	assert(root.is_array());

	xr_val.clear();
	xr_val.resize(root.size());
	for(unsigned int i = 0 ; i < root.size() ; i++)
	{
		xr_val[i] = root[i].get<float>();
	}

	return x_in;
}

template<>void FeatureVectorT<int>::Serialize(mkjson& rx_json) const
{
	rx_json = mkjson{
		{"type", "int"},
		{"values", values}
	};
}

template<>void FeatureVectorT<int>::Deserialize(const mkjson& x_json)
{
	if(x_json.at("type").get<string>() != "int")
		throw MkException("Wrong feature vector type " + x_json.at("type").get<string>(), LOC);
	values = x_json.at("values").get<vector<int>>();
}
