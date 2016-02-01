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
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

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
	Json::Value root;
	x_in >> root;  // note: copy first for local use
	assert(root.isArray());

	xr_val.clear();
	xr_val.resize(root.size());
	for(unsigned int i = 0 ; i < root.size() ; i++)
	{
		xr_val[i] = root[i].asFloat();
	}

	return x_in;
}

template<>void FeatureVectorT<float>::Serialize(std::ostream& x_out, const std::string& x_dir) const
{
	serialize(x_out, values);
}
template<>void FeatureVectorT<float>::Deserialize(std::istream& x_in, const std::string& x_dir)
{
	deserialize(x_in, values);
}

template<>void FeatureVectorT<int>::Serialize(std::ostream& xr_out, const std::string& x_dir) const
{
	Json::Value root;
	root["type"] = "int";
	stringstream ss;
	serializeSimple(ss, values);
	ss >> root["values"];
	xr_out << root;
}
template<>void FeatureVectorT<int>::Deserialize(std::istream& xr_in, const std::string& x_dir)
{
	Json::Value root;
	xr_in >> root;
	stringstream ss;
	ss << root["values"];
	deserializeSimple(ss, values);
	if(root["type"].asString() != "int")
		throw MkException("Wrong feature vector type " + root["type"].asString(), LOC);
}
