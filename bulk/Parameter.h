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

#ifndef PARAMETER_H
#define PARAMETER_H

#include <vector>
#include <string>
#include <assert.h>
#include <iostream>
#include <cstdlib>

enum ParameterType
{
	PARAM_INT,
	PARAM_FLOAT,
	PARAM_DOUBLE,
	PARAM_BOOL,
	PARAM_STR
};

enum ParameterConfigType
{
	PARAMCONF_UNSET = 0,
	PARAMCONF_DEF   = 1,
	PARAMCONF_XML,
	PARAMCONF_GUI,
	PARAMCONF_UNKNOWN, 
	PARAMCONF_SIZE = 5
};
static const char configType[][16] = {"unset", "def", "xml", "gui", "unk"};


class ConfigReader;

class ParameterValue
{
public:
	ParameterValue(int x_id, const std::string& x_name, const std::string& x_class, const std::string& x_value);
	int m_id;
	std::string m_name;
	std::string m_class;
	std::string m_value;
};

class Parameter
{
public:
	Parameter(int x_id, const std::string& x_name, ParameterType x_type):
		m_id(x_id),
		m_name(x_name),
		m_type(x_type),
		m_confType(PARAMCONF_UNSET){};
		
	virtual void SetValue(const std::string& x_value, ParameterConfigType x_confType = PARAMCONF_UNKNOWN) = 0;
	virtual void SetValue(const void* x_value, ParameterConfigType x_confType = PARAMCONF_UNKNOWN) = 0;
	virtual const void* GetValue() const = 0;
	virtual void SetDefault() = 0;
	virtual void Print() const = 0;
	const int m_id;
	const std::string m_name;
	const ParameterType m_type;
	ParameterConfigType m_confType;
private:
	//const void* mp_value;
};

template<class T> class ParameterT : public Parameter
{
public:
	ParameterT(int x_id, const std::string& x_name, T x_default, ParameterType x_type, T x_min, T x_max, T * xp_value) : 
		Parameter(x_id, x_name, x_type),
		m_default(x_default),
		m_min(x_min),
		m_max(x_max),
		mp_value(xp_value){};
	virtual void SetValue(const std::string& rx_value, ParameterConfigType x_confType = PARAMCONF_UNKNOWN)
	{
		std::istringstream istr(rx_value);
		istr >> *mp_value; // atof is sensible to locale format and may use , as a separator
		m_confType = x_confType;
	};
	virtual void SetValue(const void * px_value, ParameterConfigType x_confType = PARAMCONF_UNKNOWN)
	{	
		*mp_value = *static_cast<const T*>(px_value);
		m_confType = x_confType;
	};
	virtual const void * GetValue() const
	{
		return static_cast<const void*>(mp_value);
	}
	virtual void Print() const 
	{
		std::cout<<m_name<<" = "<<*static_cast<const T*>(GetValue())<<" ["<<m_min<<":"<<m_max<<"] ("<<configType[m_confType]<<"); ";	
		
	};
	virtual void SetDefault()
	{
		*mp_value = m_default;
	}
    std::basic_ostream< char >::__ostream_type of(const char* arg1);
	const T m_default;
	const T m_min;
	const T m_max;
private:
	T* mp_value;
};

// Specialization for string type param
template <> class ParameterT<std::string> : public Parameter
{
public:
	ParameterT(int x_id, const std::string& x_name, std::string x_default, ParameterType x_type, std::string * xp_value) : 
		Parameter(x_id, x_name, x_type),
		m_default(x_default),
		mp_value(xp_value){ assert(x_type == PARAM_STR);};
	virtual void SetValue(const std::string& rx_value, ParameterConfigType x_confType = PARAMCONF_UNKNOWN)
	{
		*mp_value = rx_value;
		m_confType = x_confType;
	};
	virtual void SetValue(const void * px_value, ParameterConfigType x_confType = PARAMCONF_UNKNOWN)
	{	
		*mp_value = *static_cast<const std::string*>(px_value);
		m_confType = x_confType;
	};
	virtual const void * GetValue() const
	{
		return static_cast<const void*>(mp_value);
	}
	virtual void Print() const 
	{
		std::cout<<m_name<<" = "<<*static_cast<const std::string*>(GetValue())<<" ("<<configType[m_confType]<<"); ";	
		
	};
	virtual void SetDefault()
	{
		*mp_value = m_default;
	}
	const std::string m_default;
private:
	std::string* mp_value;
};



class ParameterStructure
{
public:
	ParameterStructure(ConfigReader& x_configReader, const std::string& x_objectType, const std::string& x_objectName, int x_nb = 0) :
		m_objectType(x_objectType),
		m_objectName(x_objectName),
		m_objectNumber(x_nb),
		m_configReader(x_configReader)
	{
		m_list.clear();
		//Init();
	};
	void Init();
	//void ReadParametersFromConfig();
	void SetFromConfig(const std::vector<ParameterValue>& x_params);
	void SetDefault();
	void CheckRange() const;
	void PrintParameters() const;
	void SetValueByName(const std::string& x_name, const std::string& x_value, ParameterConfigType x_configType = PARAMCONF_UNKNOWN);
	
protected:
	std::vector<Parameter*> m_list;
	ConfigReader& m_configReader;
	const std::string m_objectName;
	const std::string m_objectType;
	const int m_objectNumber;
};






#endif