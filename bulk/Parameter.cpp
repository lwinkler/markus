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
std::map<std::string,int>  ParameterImageType::m_map_types;

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
	SetDefault();
	
	// Read parameters from config
	SetFromConfig(m_configReader.SubConfig("parameters"));
	
	cout<<"Parameters for "<<m_objectName<<" initialized."<<endl;
	PrintParameters();
	CheckRange();
}

/// Set the value from xml configuration 

void ParameterStructure::SetFromConfig(const ConfigReader& x_conf)
{
	ConfigReader conf = x_conf.SubConfig("param");
	while(!conf.IsEmpty())
	{
		string name = conf.GetAttribute("name");
		string value = conf.GetValue();
		SetValueByName(name, value, PARAMCONF_XML);
		conf = conf.NextSubConfig("param");
	}
}

/// Browse the parameter list to find the parameter with this name

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
}

/// Set the hard default value

void ParameterStructure::SetDefault()
{
	for(vector<Parameter*>::iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		(*it)->SetDefault();
	}
}

/// Check that the parameter values are in range [min;max]

void ParameterStructure::CheckRange() const
{
	for(vector<Parameter*>::const_iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		if((*it)->GetType() != PARAM_STR)
		{
			if(!(*it)->CheckRange())
			{
				throw("Parameter " + (*it)->GetName() + " out of range");
			}
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
		}
		(*it)->Print();
	}
	cout<<endl;
}

/// Return the type as a string (e.g. for xml)

const std::string Parameter::GetTypeString() const
{	
	switch(m_type)
	{
		case PARAM_INT: 	return "int";
		case PARAM_FLOAT:	return "float";
		case PARAM_DOUBLE: 	return "double";
		case PARAM_BOOL:	return "bool";
		case PARAM_STR: 	return "string";
		case PARAM_IMAGE_TYPE:	return "image type";
		default :		return "unknown";
	}
}

ParameterImageType::ParameterImageType(int x_id, const std::string& x_name, int x_default, int * xp_value, const std::string x_description) : 
		Parameter(x_id, x_name, PARAM_IMAGE_TYPE, x_description),
		m_default(x_default),
		mp_value(xp_value)
{
	// Init static type vector once
	if(ParameterImageType::m_map_types.size() == 0)
	{
		m_map_types["CV_8UC1"] = CV_8UC1;
		m_map_types["CV_8UC2"] = CV_8UC2;
		m_map_types["CV_8UC3"] = CV_8UC3;
		//m_map_types["CV_8UC(n)"] = CV_8UC(n);
		m_map_types["CV_8SC1"] = CV_8SC1;
		m_map_types["CV_8SC2"] = CV_8SC2;
		m_map_types["CV_8SC3"] = CV_8SC3;
		m_map_types["CV_8SC4"] = CV_8SC4;
		//m_map_types["CV_8SC(n)"] = CV_8SC(n);
		m_map_types["CV_16UC1"] = CV_16UC1;
		m_map_types["CV_16UC2"] = CV_16UC2;
		m_map_types["CV_16UC3"] = CV_16UC3;
		m_map_types["CV_16UC4"] = CV_16UC4;
		//m_map_types["CV_16UC(n)"] = CV_16UC(n);
		m_map_types["CV_16SC1"] = CV_16SC1;
		m_map_types["CV_16SC2"] = CV_16SC2;
		m_map_types["CV_16SC3"] = CV_16SC3;
		m_map_types["CV_16SC4"] = CV_16SC4;
		//m_map_types["CV_16SC(n)"] = ;
		m_map_types["CV_32SC1"] = CV_32SC1;
		m_map_types["CV_32SC2"] = CV_32SC2;
		m_map_types["CV_32SC3"] = CV_32SC3;
		m_map_types["CV_32SC4"] = CV_32SC4;
		//m_map_types["CV_32SC1"] = CV_32SC1;
		m_map_types["CV_32FC1"] = CV_32FC1;
		m_map_types["CV_32FC2"] = CV_32FC2;
		m_map_types["CV_32FC3"] = CV_32FC3;
		m_map_types["CV_32FC4"] = CV_32FC4;
		//m_map_types["CV_32FC1"] = CV_32FC1;
		m_map_types["CV_64FC1"] = CV_64FC1;
		m_map_types["CV_64FC2"] = CV_64FC2;
		m_map_types["CV_64FC3"] = CV_64FC3;
		m_map_types["CV_64FC4"] = CV_64FC4;
		//m_map_types["CV_64FC1"] = CV_64FC1;
	}
};
