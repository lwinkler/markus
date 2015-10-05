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
#include <memory>


class MkJson;

class MkJson
{
	public:
		inline MkJson() {};
		MkJson(Json::Value xr_root) : mp_root(&xr_root) {}; // should be used by static members only
		MkJson(const MkJson& x) : mp_root(x.mp_root.get()){};
		~MkJson(){};
		MkJson operator [] (const std::string& x_str);
		MkJson operator [] (int x_index);
		// inline MkJson& operator = (const MkJson& x){ mp_root(&x.mp_root) {}
		inline MkJson& operator = (double x_value) {*mp_root = x_value; return *this;}
		inline MkJson& operator = (const std::string& x_value) {*mp_root = x_value; return *this;}
		inline MkJson& operator=(const MkJson& x) {} // TODO

		inline void Append(const MkJson& x_obj); // TODO {(*this)[mp_root->size()] = *(x_obj.mp_root);}
		inline void Clear(){mp_root->clear();}

		inline friend std::ostream& operator<< (std::ostream& xr_out, const MkJson& x_obj) {xr_out << *(x_obj.mp_root); return xr_out;}
		inline friend std::istream& operator>> (std::istream& xr_in, MkJson& x_obj) {xr_in >> *(x_obj.mp_root); return xr_in;}

		inline float AsFloat() const {return mp_root->asFloat();} // TODO: Use asDouble when possible
		inline double AsDouble() const {return mp_root->asDouble();} // TODO: Use asDouble when possible
		inline std::string AsString() const {return mp_root->asString();} 
		inline int AsInt() const {return mp_root->asInt();} 
		inline bool AsBool() const {return mp_root->asBool();} 
		inline int64_t AsInt64() const {return mp_root->asInt64();} 

		inline std::vector<std::string> GetMemberNames() const {return mp_root->getMemberNames();}
		inline size_t Size() const {return mp_root->size();}  // TODO: Check if array
		inline size_t IsNull() const {return mp_root->isNull();}

		MkJson Create(const std::string& x_str);
		MkJson Create(int x_index);

		static MkJson emptyArray();
		static MkJson nullValue();


	protected:

		std::unique_ptr<Json::Value> mp_root;
};


#endif

