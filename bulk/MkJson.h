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

class MkJson_ // TODO rename
{
	public:
		inline MkJson_() {};
		MkJson_(Json::Value xr_root) : m_root(xr_root) {}; // should be used by static members only
		MkJson_(const MkJson_& x) : m_root(x.m_root){};
		~MkJson_(){};
		MkJson operator [] (const std::string& x_str);
		MkJson operator [] (int x_index);
		// inline MkJson_& operator = (const MkJson_& x){ m_root(&x.m_root) {}
		inline MkJson_& operator = (double x_value) {m_root = x_value; return *this;}
		inline MkJson_& operator = (const std::string& x_value) {m_root = x_value; return *this;}
		inline MkJson_& operator = (const MkJson_& x) {} // TODO

		inline void Append(const MkJson_& x_obj); // TODO {(*this)[m_root->size()] = *(x_obj.m_root);}
		inline void Clear(){m_root.clear();}

		inline friend std::ostream& operator<< (std::ostream& xr_out, const MkJson_& x_obj) {xr_out << (x_obj.m_root); return xr_out;}
		inline friend std::istream& operator>> (std::istream& xr_in, MkJson_& x_obj) {xr_in >> (x_obj.m_root); return xr_in;}

		inline float AsFloat() const {return m_root.asFloat();} // TODO: Use asDouble when possible
		inline double AsDouble() const {return m_root.asDouble();} // TODO: Use asDouble when possible
		inline std::string AsString() const {return m_root.asString();} 
		inline int AsInt() const {return m_root.asInt();} 
		inline bool AsBool() const {return m_root.asBool();} 
		inline int64_t AsInt64() const {return m_root.asInt64();} 

		inline std::vector<std::string> GetMemberNames() const {return m_root.getMemberNames();}
		inline size_t Size() const {return m_root.size();}  // TODO: Check if array
		inline size_t IsNull() const {return m_root.isNull();}

		MkJson Create(const std::string& x_str);
		MkJson Create(int x_index);

		static MkJson_ emptyArray();
		static MkJson_ nullValue();


	protected:

		Json::Value m_root;
};

class MkJson
{
	public:
		MkJson(MkJson_* x) : m_ptr(x) {}
		inline MkJson operator [] (const std::string& x_str) {return (*m_ptr)[x_str];}
		inline MkJson operator [] (int x_index) {return (*m_ptr)[x_index];}
		inline MkJson_ operator * () {return (*m_ptr);}
		inline MkJson_ operator -> () {return (*m_ptr);}

		inline MkJson operator = (double x_value) {*m_ptr = x_value; return *this;}
		inline MkJson operator = (const std::string& x_value) {*m_ptr = x_value; return *this;}
		inline MkJson operator = (const MkJson_& x) {} // TODO

		inline MkJson Create(const std::string& x_str){return (*m_ptr)[x_str];}
		inline MkJson Create(int x_index){return (*m_ptr)[x_index];}

		inline float AsFloat() const {return m_ptr->AsFloat();}
		inline double AsDouble() const {return m_ptr->AsDouble();}
		inline std::string AsString() const {return m_ptr->AsString();} 
		inline int AsInt() const {return m_ptr->AsInt();} 
		inline bool AsBool() const {return m_ptr->AsBool();} 
		inline int64_t AsInt64() const {return m_ptr->AsInt64();} 

		inline std::vector<std::string> GetMemberNames() const {return m_ptr->GetMemberNames();}
		inline size_t Size() const {return m_ptr->Size();}
		inline size_t IsNull() const {return m_ptr->IsNull();}
	

	protected:
		std::shared_ptr<MkJson_> m_ptr;
};


#endif

