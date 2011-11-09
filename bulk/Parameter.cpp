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


using namespace std;

ParameterValue::ParameterValue(int x_id, const string& x_name, const string& x_value) :
	m_id(x_id), m_name(x_name),m_value(x_value) 
{
};

void ParameterStructure::Init()
{
	// Read config file
	SetDefault();
	
	// Read parameters from config
	SetFromConfig(m_configReader.ReadConfigObject(m_objectType, m_objectName));
	
	cout<<"Parameters for "<<m_objectType<<"::"<<m_objectName<<" initialized."<<endl;
	PrintParameters();
}

void ParameterStructure::SetFromConfig(const std::vector<ParameterValue>& x_list)
{
	for(vector<ParameterValue>::const_iterator it = x_list.begin(); it != x_list.end(); it++)
	{
		SetValueByName(it->m_name.c_str(), it->m_value, PARAMCONF_XML);
	}
}

/// Browse the parameter list to find the parameter with this name

void ParameterStructure::SetValueByName(const string& x_name, const string& x_value, ParameterConfigType x_configType)
{
	for(vector<Parameter*>::iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		if((*it)->m_name.compare(x_name) == 0)//(!strcmp(it->m_name, x_name))
		{
			//if(x_value < it->m_min || x_value > it->m_max)
			//	throw("Parameter " + std::string(x_name) + " out of range");
			(*it)->SetValue(x_value, x_configType);
			return;
		}
	}
	
	cout<<("Warning : Parameter not found in list (by name) : " + x_name)<<endl;
}


void ParameterStructure::SetDefault()
{
	for(vector<Parameter*>::iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		(*it)->SetDefault();
	}
}

void ParameterStructure::CheckRange() const
{
	for(vector<Parameter*>::const_iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		if((*it)->m_type != PARAM_STR)
		{
			/* TODO
			double value = (*it)->GetValue();
			if(value < (*it)->m_min || value > (*it)->m_max)
				throw("Parameter " + std::string(it->m_name) + " out of range");
			*/
		}
	}
}

void ParameterStructure::PrintParameters() const
{
	string confType = "";
	for(vector<Parameter*>::const_iterator it = m_list.begin(); it != m_list.end(); it++)
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
