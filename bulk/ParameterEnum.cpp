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

#include "ParameterEnum.h"
#include "util.h"

using namespace std;


/**
* @brief Export the parameter for module description
*
* @param rx_os         Output stream
* @param x_indentation Number of tabs for indentation
*/
void ParameterEnum::Export(ostream& rx_os, int x_indentation)
{
	string tabs(x_indentation, '\t');
	rx_os<<tabs<<"<param name=\""<<m_name<<"\">"<<endl;
	tabs = string(x_indentation + 1, '\t');
	rx_os<<tabs<<"<type>"<<GetTypeString()<<"</type>"<<endl;
	rx_os<<tabs<<"<value default=\""<<GetReverseEnum().at(m_default)<<"\">"<<GetReverseEnum().at(GetValue())<<"</value>"<<endl;
	rx_os<<tabs<<"<description>"<<m_description<<"</description>"<<endl;
	tabs = string(x_indentation, '\t');
	rx_os<<tabs<<"</param>"<<endl;
}



/**
* @brief Set the value of the parameter
*
* @param rx_value   String containing the value
* @param x_confType Source of the configuration (default, xml, GUI, ...)
*/
void ParameterEnum::SetValue(const string& rx_value, ParameterConfigType x_confType)
{
	if(m_isLocked) 
		throw ParameterException("You tried to set the value of a locked parameter.", LOC);
	mr_value = GetEnum().at(rx_value);
	m_confSource = x_confType;
}

/**
* @brief Set the value of the parameter
*
* @param rx_value   Integer containing the value
* @param x_confType Source of the configuration (default, xml, GUI, ...)
*/
void ParameterEnum::SetValue(int rx_value, ParameterConfigType x_confType)
{
	if(m_isLocked) 
		throw ParameterException("You tried to set the value of a locked parameter.", LOC);
	mr_value = rx_value;
	m_confSource = x_confType;
}


/**
* @brief Set the default value 
*
* @param rx_value Value to set
*/
void ParameterEnum::SetDefault(const string& rx_value)
{
	m_default = GetEnum().at(rx_value);
}

/**
* @brief Check that the parameter value is in range
*
* @return true if valid
*/
bool ParameterEnum::CheckRange() const
{
	map<int,bool>::const_iterator it = m_allowedValues.find(mr_value);
	if(it != m_allowedValues.end())
		return it->second;
	else
		return m_allowAllValues;
}

/**
* @brief Print the range of possible values
*
* @return Range as a string
*/
string ParameterEnum::GetRange() const
{
	stringstream ss; 
	ss<<"[";
	for(map<string,int>::const_iterator it1 = GetEnum().begin() ; it1 != GetEnum().end() ; it1++)
	{
		// If a value is specified in allowed values we respect this
		// otherwise look at m_allowAllValues
		map<int,bool>::const_iterator it2 = m_allowedValues.find(it1->second);
		if(it2 != m_allowedValues.end())
		{
			if(it2->second)
				ss<<it1->first<<",";
		}
		else
		{
			if(m_allowAllValues)
				ss<<it1->first<<",";
		}
	}
	ss<<"]";
	return ss.str();
}

/**
* @brief Print the parameter with default value and range
*
* @param os Output stream
*/
void ParameterEnum::Print(ostream& os) const
{
	os<<m_name<<"="<<GetReverseEnum().at(GetValue())<<" ("<<GetValue()<<") ("<<configType[m_confSource]<<"); ";
}

/**
* @brief Set the range of acceptable values
*
* @param x_range Range in the form "[val1,val2,val3]"
*/
void ParameterEnum::SetRange(const string& x_range)
{
	vector<string> values;
	if(x_range.substr(0, 1) != "[" || x_range.substr(x_range.size() - 1, 1) != "]")
		throw MkException("Error in range " + x_range, LOC);
	split(x_range.substr(1, x_range.size() - 2), ',', values);
	// Remove last element if empty, due to an extra comma
	assert(values.size() > 0);
	if(values.back() == "")
		values.pop_back();
	assert(values.size() > 0);

	AllowAllValues(false);
	m_allowedValues.clear();
	for(vector<string>::const_iterator it = values.begin() ; it != values.end() ; it++)
	{
		AllowValue(*it, true);
	}
}
