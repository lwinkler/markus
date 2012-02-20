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

#include "Parameter.h"
#include "ConfigReader.h"

#include <cstring>
#include <iostream>
#include <cstdlib>

#include <cv.h>


using namespace std;

// Static variables
std::map<std::string,int>  ParameterImageType::m_map_enum;


template<> const ParameterType ParameterT<bool>::m_type   = PARAM_BOOL;
template<> const ParameterType ParameterT<int>::m_type    = PARAM_INT;
template<> const ParameterType ParameterT<float>::m_type  = PARAM_FLOAT;
template<> const ParameterType ParameterT<double>::m_type = PARAM_DOUBLE;

template<> const std::string ParameterT<bool>::m_typeStr   = "bool";
template<> const std::string ParameterT<int>::m_typeStr    = "int";
template<> const std::string ParameterT<float>::m_typeStr  = "float";
template<> const std::string ParameterT<double>::m_typeStr = "double";

/// Parent for all parameter structures

ParameterStructure::ParameterStructure(const ConfigReader& x_configReader):
		m_configReader(x_configReader)
{
	m_objectName = m_configReader.GetAttribute("name");
	m_list.clear();
};
	
ParameterStructure::~ParameterStructure()
{
	for(std::vector<Parameter* >::iterator it = m_list.begin() ; it != m_list.end() ; it++)
		delete(*it);
}

/// Initialize the parameter structure with the value from default or xml configuration

void ParameterStructure::Init()
{
	// Read config file
	SetValueToDefault();
	
	// Read parameters from config
	SetFromConfig();
	
	cout<<"Parameters for "<<m_objectName<<" initialized."<<endl;
	PrintParameters();
	CheckRange();
}

/// Set the value from xml configuration 

void ParameterStructure::SetFromConfig()
{
	ConfigReader conf = m_configReader.SubConfig("parameters").SubConfig("param");
	while(!conf.IsEmpty())
	{
		string name = conf.GetAttribute("name");
		string value = conf.GetValue();
		//SetValueByName(name, value, PARAMCONF_XML);
		
		RefParameterByName(name).SetValue(value, PARAMCONF_XML);
		conf = conf.NextSubConfig("param");
	}
}

/// Browse the parameter list to find the parameter with this name
/*
void ParameterStructure::SetValueByName(const string& x_name, const string& x_value, ParameterConfigType x_configType)
{
	for(vector<Parameter*>::iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		if((*it)->GetName().compare(x_name) == 0)//(!strcmp(it->m_name, x_name))
		{
			(*it)->SetValue(x_value, x_configType);
			return;
		}
	}
	
	cout<<("Warning : Parameter not found in list (by name) : " + x_name)<<endl;
}*/

/// Get the reference to a parameter by name
Parameter& ParameterStructure::RefParameterByName(const std::string& x_name)
{
	for(vector<Parameter*>::iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		if((*it)->GetName().compare(x_name) == 0)//(!strcmp(it->m_name, x_name))
		{
			return **it;
		}
	}
	
	throw("Parameter not found in list (by name) : " + x_name);
}


/// Set the hard default value

void ParameterStructure::SetValueToDefault()
{
	for(vector<Parameter*>::iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		(*it)->SetValueToDefault();
	}
}

/// Check that the parameter values are in range [min;max]

void ParameterStructure::CheckRange() const
{
	for(vector<Parameter*>::const_iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		if(!(*it)->CheckRange())
		{
			throw("Parameter " + (*it)->GetName() + " out of range");
		}
	}
}

/// Print parameters to stdout with details

void ParameterStructure::PrintParameters() const
{
	string confType = "";
	for(vector<Parameter*>::const_iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		switch((*it)->GetConfigurationSource())
		{
			case PARAMCONF_DEF: confType = "def"; break;
			case PARAMCONF_GUI: confType = "gui"; break;
			case PARAMCONF_UNKNOWN: confType = "unk"; break;
			case PARAMCONF_UNSET: confType = "unset"; break;
			case PARAMCONF_XML: confType = "xml"; break;
			default: assert(false); break;
		}
		(*it)->Print();
	}
	cout<<endl;
}


void ParameterImageType::Export(std::ostream& rx_os, int x_indentation)
{
	std::string tabs(x_indentation, '\t');
	rx_os<<tabs<<"<param name=\""<<m_name<<"\">"<<std::endl;
	tabs = std::string(x_indentation + 1, '\t');
	rx_os<<tabs<<"<type>"<<GetTypeString()<<"</type>"<<std::endl;
	rx_os<<tabs<<"<value default=\""<<Int2Str(m_default)<<"\">"<<Int2Str(GetValue())<<"</value>"<<std::endl;
	rx_os<<tabs<<"<description>"<<m_description<<"</description>"<<std::endl;
	tabs = std::string(x_indentation, '\t');
	rx_os<<tabs<<"</param>"<<std::endl;
}

ParameterImageType::ParameterImageType(int x_id, const std::string& x_name, int x_default, int * xp_value, const std::string x_description) :
		ParameterEnum(x_id, x_name, x_default, xp_value, x_description)
{
	// Init static type vector once
	if(ParameterImageType::m_map_enum.size() == 0)
	{
		m_map_enum["CV_8UC1"] = CV_8UC1;
		m_map_enum["CV_8UC2"] = CV_8UC2;
		m_map_enum["CV_8UC3"] = CV_8UC3;
		//m_map_enum["CV_8UC(n)"] = CV_8UC(n);
		m_map_enum["CV_8SC1"] = CV_8SC1;
		m_map_enum["CV_8SC2"] = CV_8SC2;
		m_map_enum["CV_8SC3"] = CV_8SC3;
		m_map_enum["CV_8SC4"] = CV_8SC4;
		//m_map_enum["CV_8SC(n)"] = CV_8SC(n);
		m_map_enum["CV_16UC1"] = CV_16UC1;
		m_map_enum["CV_16UC2"] = CV_16UC2;
		m_map_enum["CV_16UC3"] = CV_16UC3;
		m_map_enum["CV_16UC4"] = CV_16UC4;
		//m_map_enum["CV_16UC(n)"] = CV_16UC(n);
		m_map_enum["CV_16SC1"] = CV_16SC1;
		m_map_enum["CV_16SC2"] = CV_16SC2;
		m_map_enum["CV_16SC3"] = CV_16SC3;
		m_map_enum["CV_16SC4"] = CV_16SC4;
		//m_map_enum["CV_16SC(n)"] = ;
		m_map_enum["CV_32SC1"] = CV_32SC1;
		m_map_enum["CV_32SC2"] = CV_32SC2;
		m_map_enum["CV_32SC3"] = CV_32SC3;
		m_map_enum["CV_32SC4"] = CV_32SC4;
		//m_map_enum["CV_32SC1"] = CV_32SC1;
		m_map_enum["CV_32FC1"] = CV_32FC1;
		m_map_enum["CV_32FC2"] = CV_32FC2;
		m_map_enum["CV_32FC3"] = CV_32FC3;
		m_map_enum["CV_32FC4"] = CV_32FC4;
		//m_map_enum["CV_32FC1"] = CV_32FC1;
		m_map_enum["CV_64FC1"] = CV_64FC1;
		m_map_enum["CV_64FC2"] = CV_64FC2;
		m_map_enum["CV_64FC3"] = CV_64FC3;
		m_map_enum["CV_64FC4"] = CV_64FC4;
		//m_map_enum["CV_64FC1"] = CV_64FC1;
	}
}

void ParameterEnum::SetValue(const std::string& rx_value, ParameterConfigType x_confType)
{
	*mp_value = Str2Int(rx_value);
	m_confSource = x_confType;
}

void ParameterEnum::SetValue(int rx_value, ParameterConfigType x_confType)
{
	*mp_value = rx_value;
	m_confSource = x_confType;
}


void ParameterEnum::SetDefault(const std::string& rx_value)
{
	m_default = Str2Int(rx_value);
}

bool ParameterEnum::CheckRange() const
{
	//for(std::map<std::string, int>::const_iterator it = m_map_enum.begin() ; it != m_map_enum.end() ; it++)
		return true;
	// TODO : Check range !!!
	return false;
}

void ParameterEnum::Print() const
{
	std::cout<<m_name<<" = "<<Int2Str(GetValue())<<" ["<<GetValue()<<"] ("<<configType[m_confSource]<<"); ";

}

