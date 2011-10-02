#include "Parameter.h"
#include <cstring>

using namespace std;

void ParameterStructure::SetFromConfig(const std::list<ParameterValue>& x_list)
{
	for(list<ParameterValue>::const_iterator it = x_list.begin(); it != x_list.end(); it++)
	{
		SetValueByName(it->m_name.c_str(), it->m_value);
	}
}

void ParameterStructure::SetValueByName(const char * x_name, double x_value)
{
	for(list<Parameter>::iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		if(it->m_name.compare(x_name) == 0)//(!strcmp(it->m_name, x_name))
		{
			if(x_value < it->m_min || x_value > it->m_max)
				throw("Parameter " + std::string(x_name) + " out of range");
			it->SetValue(x_value);
			return;
		}
	}
	
	throw("Parameter not found in list (by name) : " + std::string(x_name));
}

void Parameter::SetValue(double x_value)
{
	switch(m_type)
	{
		case PARAM_INT:
		{
			int * p_value = (int*) mp_value;
			*p_value = x_value;
		}
			break;
		case PARAM_FLOAT:
		{
			float * p_value = (float*) mp_value;
			*p_value = x_value;
		}
			break;
		case PARAM_DOUBLE:
		{
			double * p_value = (double*) mp_value;
			*p_value = x_value;
		}
			break;
		default:
			throw(std::string("Error in Parameter::SetValue"));
	}
}

void ParameterStructure::SetDefault()
{
	for(list<Parameter>::iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		it->SetValue(it->m_default);
	}
}

void ParameterStructure::CheckRange() const
{
	for(list<Parameter>::const_iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		double value = it->GetValue();
		if(value < it->m_min || value > it->m_max)
			throw("Parameter " + std::string(it->m_name) + " out of range");
	}
}

double Parameter::GetValue() const
{
	double value = -1;
	switch(m_type)
	{
		case PARAM_INT:
		{
			int * p_value = (int*) mp_value;
			value = *p_value;
		}
			break;
		case PARAM_FLOAT:
		{
			float * p_value = (float*) mp_value;
			value = *p_value;
		}
			break;
		case PARAM_DOUBLE:
		{
			double * p_value = (double*) mp_value;
			value = *p_value;
		}
			break;
		default:
			throw(std::string("Error in Parameter::SetValue"));
	}
	return value;
}
