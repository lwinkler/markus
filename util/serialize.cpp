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
#include <sstream>
// #include <opencv2/opencv.hpp>
#include "serialize.h"
#include "MkException.h"


namespace mk {
using namespace std;

string signatureJsonValue(const mkjson &x_val)
{
	if( x_val.is_null() )
		return "null";
	else if( x_val.is_string() )
		return "%s";
	else if( x_val.is_boolean() )
		return "%b";
	/*
	else if( x_val.isInt() )
	return "%d";
	else if( x_val.isUInt() )
	return "%u";
	*/
	// Note: the differienciation between different types of num values is ambiguous:
	// e.g. a float can be a round number so its signature might be %d when deserialized
	// so we keep %f as the only possibility
	else if( x_val.is_number() )
		return "%f";
	else if( x_val.is_array() )
		assert(false);
	else
	{
		cout << x_val << endl;
		// LOG_ERROR(m_logger, "Unknown type " <<  x_val);
		throw MkException("Unknown type in JSON", LOC);
	}
	return "";
}

string signatureJsonTree(const mkjson &x_json, int x_depth)
{
	x_depth += 1;
	// cout<<" {type=["<<x_json.type()<<"], size="<<x_json.size()<<"}"<<endl;

	string result;

	if(x_json.is_array())
	{
		return "[]";
	}
	else if(x_json.is_object())
	{
		// printf("\n");
		for(auto it = x_json.cbegin(); it != x_json.cend(); ++it)
		{
			result += "\"";
			result += it.key();
			result += "\":";
			if(it.value().is_object())
			{

				result += "{";
				result += signatureJsonTree(it.value(), x_depth);
				result += "}";
			} else {
				if(it.key() == "type")
					result += "\"" + it.value().get<string>() + "\"";
				else
					result += signatureJsonTree(it.value(), x_depth);
			}
			result += ",";
		}
		return result;
	}
	else
	{
		result += signatureJsonValue(x_json);
	}
	return result;
}

} // namespace mk
