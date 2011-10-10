#include "Parameter.h"
#include "ConfigReader.h"

#include <cstring>
#include <iostream>
#include <cstdlib>


using namespace std;

ParameterValue::ParameterValue(int x_id, const std::string& x_name, const std::string& x_value) :
	m_id(x_id), m_name(x_name), m_value(x_value) 
{
};

void ParameterStructure::Init()
{
	// Read config file
	SetDefault();
	ReadParametersFromConfig();
	cout<<"Module "<<m_moduleName<<" initialized."<<endl;
	PrintParameters();
}

void ParameterStructure::ReadParametersFromConfig()
{
	m_configReader.ReadConfigModule(m_moduleName);
	SetFromConfig(m_configReader.m_parameterList);
}

void ParameterStructure::SetFromConfig(const std::list<ParameterValue>& x_list)
{
	for(list<ParameterValue>::const_iterator it = x_list.begin(); it != x_list.end(); it++)
	{
		SetValueByName(it->m_name.c_str(), it->m_value, PARAMCONF_XML);
	}
}

void ParameterStructure::SetValueByName(const string& x_name, const string& x_value, ParameterConfigType x_configType)
{
	for(list<Parameter*>::iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		if((*it)->m_name.compare(x_name) == 0)//(!strcmp(it->m_name, x_name))
		{
			//if(x_value < it->m_min || x_value > it->m_max)
			//	throw("Parameter " + std::string(x_name) + " out of range");
			(*it)->SetValue(x_value, x_configType);
			return;
		}
	}
	
	cout<<("Parameter not found in list (by name) : " + std::string(x_name))<<endl;
}


void ParameterStructure::SetDefault()
{
	for(list<Parameter*>::iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		(*it)->SetDefault();
	}
}

void ParameterStructure::CheckRange() const
{
	for(list<Parameter*>::const_iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		if((*it)->m_type != PARAM_STR)
		{
			/*
			double value = it->GetValue();
			if(value < it->m_min || value > it->m_max)
				throw("Parameter " + std::string(it->m_name) + " out of range");*/
		}
	}
}

void ParameterStructure::PrintParameters() const
{
	string confType = "";
	for(list<Parameter*>::const_iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		switch((*it)->m_confType)
		{
			case PARAMCONF_DEF: confType = "def"; break;
			case PARAMCONF_GUI: confType = "gui"; break;
			case PARAMCONF_UNKNOWN: confType = "unk"; break;
			case PARAMCONF_UNSET: confType = "unset"; break;
			case PARAMCONF_XML: confType = "xml"; break;
		}
		(*it)->Print();
	}
	cout<<endl;
}
