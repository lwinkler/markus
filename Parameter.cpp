#include "Parameter.h"
#include <cstring>
#include <iostream>
#include <cstdlib>

using namespace std;

void ParameterStructure::SetFromConfig(const std::list<ParameterValue>& x_list)
{
	for(list<ParameterValue>::const_iterator it = x_list.begin(); it != x_list.end(); it++)
	{
		SetValueByName(it->m_name.c_str(), it->m_value);
	}
}

void ParameterStructure::SetValueByName(const char * x_name, const string& x_value)
{
	for(list<Parameter>::iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		if(it->m_name.compare(x_name) == 0)//(!strcmp(it->m_name, x_name))
		{
			//if(x_value < it->m_min || x_value > it->m_max)
			//	throw("Parameter " + std::string(x_name) + " out of range");
			it->SetValue(x_value);
			return;
		}
	}
	
	cout<<("Parameter not found in list (by name) : " + std::string(x_name))<<endl;
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

void Parameter::SetValue(const string& x_value)
{
	switch(m_type)
	{
		case PARAM_INT:
		{
			int * p_value = (int*) mp_value;
			*p_value = atoi(x_value.c_str());
		}
			break;
		case PARAM_FLOAT:
		{
			float * p_value = (float*) mp_value;
			*p_value = (float)atof(x_value.c_str());
		}
			break;
		case PARAM_DOUBLE:
		{
			double * p_value = (double*) mp_value;
			*p_value = atof(x_value.c_str());
		}
			break;
		case PARAM_STR:
		{
			string * p_value = (string*) mp_value;
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
		if(it->m_type == PARAM_STR)
			it->SetValue(it->m_defaultStr);
		else
			it->SetValue(it->m_default);
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
			throw(std::string("Error in Parameter::GetValue"));
	}
	return value;
}

std::string Parameter::GetValueStr() const
{
	string * pstr = (string*)mp_value;
	return *pstr;
}

void ParameterStructure::CheckRange() const
{
	for(list<Parameter>::const_iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		if(it->m_type != PARAM_STR)
		{
			double value = it->GetValue();
			if(value < it->m_min || value > it->m_max)
				throw("Parameter " + std::string(it->m_name) + " out of range");
		}
	}
}

void ParameterStructure::PrintParameters() const
{
	for(list<Parameter>::const_iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		if(it->m_type == PARAM_STR)
			cout<<it->m_name<<" = "<<it->GetValueStr()<<"; ";
		else
			cout<<it->m_name<<" = "<<it->GetValue()<<" ["<<it->m_min<<":"<<it->m_max<<"]; ";
	}
	cout<<endl;
}
