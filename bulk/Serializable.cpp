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
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include <sstream>

#include <opencv2/opencv.hpp>
#include "Serializable.h"
#include "MkException.h"


using namespace std;

string signatureJSONValue(const Json::Value &x_val)
{
	if( x_val.isString() )
		return "%s";
	/*
	else if( x_val.isBool() )
	return "%b";
	else if( x_val.isInt() )
	return "%d";
	else if( x_val.isUInt() )
	return "%u";
	*/
	// Note: the differienciation between different types of num values is ambiguous:
	// e.g. a float can be a round number so its signature might be %d when deserialized
	// so we keep %f as the only possibility
	else if( x_val.isNumeric() )
		return "%f";
	else if( x_val.isArray() )
		assert(false);
	else
	{
		cout << x_val;
		throw MkException("Unknown type in JSON ", LOC);
	}
	return "";
}

string signatureJSONTree(const Json::Value &x_root, int x_depth)
{
	x_depth += 1;
	// cout<<" {type=["<<x_root.type()<<"], size="<<x_root.size()<<"}"<<endl;

	string result;

	if(x_root.isArray())
	{
		return "[]";
	}
	else if(x_root.isObject())
	{
		// printf("\n");
		for(Json::ValueIterator itr = x_root.begin() ; itr != x_root.end() ; itr++)
		{
			result += "\"";
			result += itr.memberName();
			result += "\":";
			if((*itr).isObject())
			{

				result += "{";
				result += signatureJSONTree(*itr, x_depth);
				result += "}";
			}
			else
				result += signatureJSONTree(*itr, x_depth);
			result += ",";
		}
		return result;
	}
	else
	{
		result += signatureJSONValue(x_root);
	}
	return result;
}

string Serializable::SerializeToString(const string& x_dir) const
{
	stringstream ss;
	Serialize1(ss, x_dir);
	Json::Value root;
	ss >> root;
	Json::FastWriter writer;
	string tmp = writer.write(root);
	tmp.erase(remove(tmp.begin(), tmp.end(), '\n'), tmp.end());
	return tmp;
}

string Serializable::Signature() const
{
	stringstream ss;
	ss<<SerializeToString();
	return Serializable::signature(ss);
}

string Serializable::signature(istream& x_in)
{
	Json::Value root;
	x_in >> root;
	return signatureJSONTree(root, 0);
}
