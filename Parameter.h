#ifndef PARAMETER_H
#define PARAMETER_H

#include <list>
#include <string>
#include <assert.h>
#include <iostream>
#include <cstdlib>

enum ParameterType
{
	PARAM_INT,
	PARAM_FLOAT,
	PARAM_DOUBLE,
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
	ParameterValue(int x_id, const std::string& x_name, const std::string& x_value);
	const int m_id;
	const std::string m_name;
	const std::string m_value;
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
		*mp_value = static_cast<T>(atof(rx_value.c_str()));
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
		mp_value(xp_value){};
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
	ParameterStructure(ConfigReader& x_configReader, const std::string& x_moduleName) :
		m_moduleName(x_moduleName),
		m_configReader(x_configReader)
	{
		m_list.clear();
		//Init();
	};
	void Init();
	void ReadParametersFromConfig();
	void SetFromConfig(const std::list<ParameterValue>& x_params);
	void SetDefault();
	void CheckRange() const;
	void PrintParameters() const;
	void SetValueByName(const std::string& x_name, const std::string& x_value, ParameterConfigType x_configType = PARAMCONF_UNKNOWN);
	
//protected:
	std::list<Parameter*> m_list;
	ConfigReader& m_configReader;
	const std::string m_moduleName;
};






#endif