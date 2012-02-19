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
#include <map>
#include <assert.h>
#include <iostream>
#include <cstdlib>

enum ParameterType
{
	PARAM_INT,
	PARAM_FLOAT,
	PARAM_DOUBLE,
	PARAM_BOOL,
	PARAM_STR,
	PARAM_IMAGE_TYPE
};

enum ParameterConfigType
{
	PARAMCONF_UNSET = 0,
	PARAMCONF_DEF   = 1,
	PARAMCONF_XML,
	PARAMCONF_GUI,
	PARAMCONF_UNKNOWN
	//PARAMCONF_SIZE = 5
};
static const char configType[][16] = {"unset", "def", "xml", "gui", "unk"};


class ConfigReader;

class Parameter
{
public:
	Parameter(int x_id, const std::string& x_name, const std::string& x_description):
		m_id(x_id),
		m_name(x_name),
		m_confSource(PARAMCONF_UNSET),
		m_description(x_description){};
		
	virtual void SetValue(const std::string& x_value, ParameterConfigType x_confType = PARAMCONF_UNKNOWN) = 0;
	//virtual void SetValue(const void* x_value, ParameterConfigType x_confType = PARAMCONF_UNKNOWN) = 0;
	virtual void SetDefault(const std::string& x_value) = 0;
	//virtual const void* GetValue() const = 0;
	inline const std::string& GetName() const {return m_name;}
	virtual const ParameterType GetType() const = 0;
	virtual const std::string GetTypeString() const = 0;
	inline const std::string& GetDescription() const {return m_description;}
	inline const ParameterConfigType& GetConfigurationSource() const {return m_confSource;}
	virtual void SetValueToDefault() = 0;
	virtual void Print() const = 0;
	virtual bool CheckRange() const = 0;
	virtual void Export(std::ostream& rx_os, int x_indentation) = 0;

protected:	
	const int m_id;
	const std::string m_name;
	ParameterConfigType m_confSource;
	const std::string m_description;
};

template<class T> class ParameterT : public Parameter
{
public:
	ParameterT(int x_id, const std::string& x_name, T x_default, ParameterType x_type, T x_min, T x_max, T * xp_value, const std::string& x_description) : 
		Parameter(x_id, x_name, x_description),
		m_default(x_default),
		m_min(x_min),
		m_max(x_max),
		mp_value(xp_value){}
	inline const ParameterType GetType() const {return m_type;}
	inline const std::string GetTypeString() const{return m_typeStr;}
	inline const T GetDefault() const {return m_default;}
	inline const T GetMin() const {return m_min;}
	inline const T GetMax() const{return m_max;}
	
	virtual void SetValue(const std::string& rx_value, ParameterConfigType x_confType = PARAMCONF_UNKNOWN)
	{
		std::istringstream istr(rx_value);
		istr >> *mp_value; // atof is sensible to locale format and may use , as a separator
		m_confSource = x_confType;
	}
	inline void SetValue(T x_value, ParameterConfigType x_confType = PARAMCONF_UNKNOWN)
	{
		*mp_value = x_value;
		m_confSource = x_confType;
	}
	/*virtual void SetValue(const void * px_value, ParameterConfigType x_confType = PARAMCONF_UNKNOWN)
	{	
		*mp_value = *static_cast<const T*>(px_value);
		m_confSource = x_confType;
	};*/
	virtual void SetDefault(const std::string& rx_value)
	{	
		std::istringstream istr(rx_value);
		istr >> m_default; // atof is sensible to locale format and may use , as a separator
	}
	virtual T GetValue() const
	{
		return *mp_value;
	}
	virtual bool CheckRange() const
	{
		T value = GetValue();
		return (value <= m_max && value >= m_min);
	}
	virtual void Print() const 
	{
		std::cout<<m_name<<" = "<<GetValue()<<" ["<<m_min<<":"<<m_max<<"] ("<<configType[m_confSource]<<"); ";	
		
	}
	virtual void SetValueToDefault()
	{
		*mp_value = m_default;
		m_confSource = PARAMCONF_DEF;
	}
	virtual void Export(std::ostream& rx_os, int x_indentation)
	{
		std::string tabs(x_indentation, '\t');
		rx_os<<tabs<<"<param name=\""<<m_name<<"\">"<<std::endl;
		tabs = std::string(x_indentation + 1, '\t');
		rx_os<<tabs<<"<type>"<<GetTypeString()<<"</type>"<<std::endl;
		rx_os<<tabs<<"<value min=\""<<m_min<<"\" max=\""<<m_max<<"\" default=\""<<m_default<<"\">"<<GetValue()<<"</value>"<<std::endl;
		rx_os<<tabs<<"<description>"<<m_description<<"</description>"<<std::endl;
		tabs = std::string(x_indentation, '\t');
		rx_os<<tabs<<"</param>"<<std::endl;
	}
	
	T m_default;
	const T m_min;
	const T m_max;
private:
	T* mp_value;
	static const ParameterType m_type;
	static const std::string m_typeStr;
};


// for string type param
class ParameterString : public Parameter
{
public:
	ParameterString(int x_id, const std::string& x_name, std::string x_default, std::string * xp_value, const std::string& x_description) : 
		Parameter(x_id, x_name, x_description),
		m_default(x_default),
		mp_value(xp_value){}
	virtual void SetValue(const std::string& rx_value, ParameterConfigType x_confType = PARAMCONF_UNKNOWN)
	{
		*mp_value = rx_value;
		m_confSource = x_confType;
	}
	virtual void SetDefault(const std::string& x_value)
	{	
		m_default = x_value;
	}
	virtual std::string GetValue() const
	{
		return *mp_value;
	}
	inline virtual bool CheckRange() const
	{
		return true;
	}
	virtual void Print() const 
	{
		std::cout<<m_name<<" = "<<GetValue()<<" ("<<configType[m_confSource]<<"); ";	
		
	}
	virtual void SetValueToDefault()
	{
		*mp_value = m_default;
		m_confSource = PARAMCONF_DEF;
	}
	virtual void Export(std::ostream& rx_os, int x_tabs)
	{
		std::string tabs(x_tabs, '\t');
		rx_os<<tabs<<"<param name=\""<<m_name<<"\">"<<std::endl;
		tabs = std::string(x_tabs + 1, '\t');
		rx_os<<tabs<<"<type>"<<GetTypeString()<<"</type>"<<std::endl;
		rx_os<<tabs<<"<value default=\""<<m_default<<"\">"<<GetValue()<<"</value>"<<std::endl;
		rx_os<<tabs<<"<description>"<<m_description<<"</description>"<<std::endl;
		tabs = std::string(x_tabs, '\t');
		rx_os<<tabs<<"</param>"<<std::endl;
	}
	inline const ParameterType GetType() const {return PARAM_STR;}
	inline const std::string GetTypeString() const {return "string";}

	inline const std::string& GetDefault(){return m_default;}
private:
	std::string m_default;
	std::string* mp_value;
};


class ParameterEnum : public Parameter
{
public:
	ParameterEnum(int x_id, const std::string& x_name, int x_default, int * xp_value, const std::string x_description):
		Parameter(x_id, x_name, x_description),
		m_default(x_default),
		mp_value(xp_value){}
	void SetValue(const std::string& rx_value, ParameterConfigType x_confType = PARAMCONF_UNKNOWN);
	void SetDefault(const std::string& rx_value);
	inline int GetValue() const
	{
		return *mp_value;
	}
	virtual bool CheckRange() const;
	virtual void Print() const;
	virtual void SetValueToDefault()
	{
		*mp_value = m_default;
		m_confSource = PARAMCONF_DEF;
	}
	inline const ParameterType GetType() const {return PARAM_IMAGE_TYPE;}
	virtual const std::string GetTypeString() const = 0;
	
	virtual void Export(std::ostream& rx_os, int x_indentation) = 0;

	virtual int Str2Int(const std::string& x) const = 0;
	virtual const std::string Int2Str(int x) const = 0;

protected:

	int m_default;
	int* mp_value;
};

class ParameterImageType : public ParameterEnum
{
public:
	ParameterImageType(int x_id, const std::string& x_name, int x_default, int * xp_value, const std::string x_description);
	~ParameterImageType(){}
	void Export(std::ostream& rx_os, int x_indentation);

	// Conversion methods
	inline const std::string GetTypeString() const {return "image type";}
	//virtual void SetValue(const std::string& rx_value, ParameterConfigType x_confType = PARAMCONF_UNKNOWN) = 0;
	//virtual void SetDefault(const std::string& rx_value);
	virtual int Str2Int(const std::string& x) const
	{
		std::map<std::string, int>::iterator found = m_map_enum.find(x);
		if(found != m_map_enum.end())
			return found->second;
		else return -1;
	}
	virtual const std::string Int2Str(int x) const
	{
		static const std::string unknown = "(unknown image type)";
		for(std::map<std::string, int>::const_iterator it = m_map_enum.begin() ; it != m_map_enum.end() ; it++)
			if(it->second == x)
				return(it->first);
		return unknown;
	}

private:
	static std::map<std::string,int>  m_map_enum;
};

/// Represents a set of parameters for a configurable objects
class ParameterStructure
{
public:
	ParameterStructure(const ConfigReader& x_configReader);
	~ParameterStructure();
	void Init();
	void SetFromConfig();
	void SetValueToDefault();
	void CheckRange() const;
	void PrintParameters() const;
	//void SetValueByName(const std::string& x_name, const std::string& x_value, ParameterConfigType x_configType = PARAMCONF_UNKNOWN);
	Parameter & RefParameterByName(const std::string& x_name);
	const std::vector<Parameter*>& GetList() const {return m_list;}
	
protected:
	std::vector<Parameter*> m_list;
	const ConfigReader& m_configReader;
	std::string m_objectName;
};

typedef ParameterT<int> 	ParameterInt;
typedef ParameterT<double> 	ParameterDouble;
typedef ParameterT<float> 	ParameterFloat;
typedef ParameterT<bool> 	ParameterBool;




#endif
