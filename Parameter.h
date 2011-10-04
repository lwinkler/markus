#ifndef PARAMETER_H
#define PARAMETER_H

#include <list>
#include <string>
#include <assert.h>

enum ParameterType
{
	PARAM_INT,
	PARAM_FLOAT,
	PARAM_DOUBLE,
	PARAM_STR
};

enum ParameterConfigType
{
	PARAMCONF_UNSET,
	PARAMCONF_DEF,
	PARAMCONF_XML,
	PARAMCONF_GUI,
	PARAMCONF_UNKNOWN
};

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
	Parameter(int x_id, const std::string& x_name, double x_default, ParameterType x_type, double x_min, double x_max, void * xp_value) :
		m_id(x_id),
		m_name(x_name),
		m_default(x_default),
		m_defaultStr(""),
		m_type(x_type),
		m_confType(PARAMCONF_UNSET),
		m_min(x_min),
		m_max(x_max),
		mp_value(xp_value){};
	Parameter(int x_id, const std::string& x_name, const std::string& x_defaultStr, ParameterType x_type, void * xp_value) :
		m_id(x_id),
		m_name(x_name),
		m_default(0),
		m_defaultStr(x_defaultStr),
		m_type(x_type),
		m_confType(PARAMCONF_UNSET),
		m_min(0),
		m_max(0),
		mp_value(xp_value){assert(x_type == PARAM_STR);};
	void SetValue(double x_value, ParameterConfigType x_confType = PARAMCONF_UNKNOWN);
	void SetValue(const std::string& x_value, ParameterConfigType x_confType = PARAMCONF_UNKNOWN);
	double GetValue() const;
	std::string GetValueStr() const;
	const int m_id;
	const std::string m_name;
	const double m_default;
	const std::string m_defaultStr;
	const ParameterType m_type;
	ParameterConfigType m_confType;
	const double m_min;
	const double m_max;
private:
	const void* mp_value;
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
	virtual void SetFromConfig(const std::list<ParameterValue>& x_params);
	void SetDefault();
	void CheckRange() const;
	void PrintParameters() const;
	void SetValueByName(const char * x_name, const std::string& x_value, ParameterConfigType x_configType = PARAMCONF_UNKNOWN);
	
//protected:
	std::list<Parameter> m_list;
	ConfigReader& m_configReader;
	const std::string m_moduleName;
};






#endif