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

class ParameterValue
{
public:
	ParameterValue(int x_id, const std::string& x_name, const std::string& x_value) :
		m_id(x_id), m_name(x_name), m_value(x_value) {};
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
		m_min(x_min),
		m_max(x_max),
		mp_value(xp_value){};
	Parameter(int x_id, const std::string& x_name, const std::string& x_defaultStr, ParameterType x_type, void * xp_value) :
		m_id(x_id),
		m_name(x_name),
		m_default(0),
		m_defaultStr(x_defaultStr),
		m_type(x_type),
		m_min(0),
		m_max(0),
		mp_value(xp_value){assert(x_type == PARAM_STR);};
	void SetValue(double x_value);
	void SetValue(const std::string& x_value);
	double GetValue() const;
	std::string GetValueStr() const;
	const int m_id;
	const std::string m_name;
	const double m_default;
	const std::string m_defaultStr;
	ParameterType m_type;
	const double m_min;
	const double m_max;
private:
	const void* mp_value;
};

class ParameterStructure
{
public:
	ParameterStructure()
	{
		m_list.clear();
	};
	virtual void Init() = 0;
	virtual void SetFromConfig(const std::list<ParameterValue>& x_params);
	void SetDefault();
	void CheckRange() const;
	void PrintParameters() const;
	void SetValueByName(const char * x_name, const std::string& x_value);
	
protected:
	std::list<Parameter> m_list;
};






#endif