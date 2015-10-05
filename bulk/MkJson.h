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

#ifndef MK_JSON_H
#define MK_JSON_H

#include "jsoncpp/json/reader.h"
#include "jsoncpp/json/writer.h"


class MkJson;

class MkJson
{
	public:
		inline MkJson(){};
		MkJson operator [] (const std::string& x_str);
		MkJson operator [] (int x_index);
		inline MkJson operator = (double x_value) {m_root = x_value; return *this;}
		inline MkJson operator = (const std::string& x_value) {m_root = x_value; return *this;}

		inline void Append(const MkJson& x_obj){} // TODO
		inline void Clear(){m_root.clear();}

		inline friend std::ostream& operator<< (std::ostream& xr_out, const MkJson& x_obj) {xr_out << x_obj.m_root; return xr_out;}
		inline friend std::istream& operator>> (std::istream& xr_in, MkJson& x_obj) {xr_in >> x_obj.m_root; return xr_in;}

		inline float AsFloat() const {return m_root.asFloat();} // TODO: Use asDouble when possible
		inline double AsDouble() const {return m_root.asDouble();} // TODO: Use asDouble when possible
		inline std::string AsString() const {return m_root.asString();} 
		inline int AsInt() const {return m_root.asInt();} 
		inline bool AsBool() const {return m_root.asBool();} 
		inline int64_t AsInt64() const {return m_root.asInt64();} 

		inline size_t Size() const {return m_root.size();}  // TODO: Check if array
		inline size_t IsNull() const {return m_root.isNull();}

		MkJson& Create(const std::string& x_str);
		MkJson& Create(int x_index);

		static MkJson emptyArray();
		static MkJson nullValue();
	private:
		MkJson(Json::Value& xr_root) : m_root(xr_root) {};


	protected:
		Json::Value m_root;
};


#endif

