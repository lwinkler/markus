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
#include <opencv2/core/core.hpp>

using namespace std;

// Static variables
const map<string, int>  ParameterImageType::Enum        = ParameterImageType::CreateMap();
const map<int, string>  ParameterImageType::ReverseEnum = ParameterImageType::CreateReverseMap();

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
* @brief Static function used for map initialization
*
* @return map
*/
map<string,int> ParameterImageType::CreateMap()
{
	map<string, int> map1;
	map1["CV_8UC1"] = CV_8UC1;
	map1["CV_8UC2"] = CV_8UC2;
	map1["CV_8UC3"] = CV_8UC3;
	//map1["CV_8UC(n)"] = CV_8UC(n);
	map1["CV_8SC1"] = CV_8SC1;
	map1["CV_8SC2"] = CV_8SC2;
	map1["CV_8SC3"] = CV_8SC3;
	map1["CV_8SC4"] = CV_8SC4;
	//map1["CV_8SC(n)"] = CV_8SC(n);
	map1["CV_16UC1"] = CV_16UC1;
	map1["CV_16UC2"] = CV_16UC2;
	map1["CV_16UC3"] = CV_16UC3;
	map1["CV_16UC4"] = CV_16UC4;
	//map1["CV_16UC(n)"] = CV_16UC(n);
	map1["CV_16SC1"] = CV_16SC1;
	map1["CV_16SC2"] = CV_16SC2;
	map1["CV_16SC3"] = CV_16SC3;
	map1["CV_16SC4"] = CV_16SC4;
	//map1["CV_16SC(n)"] = ;
	map1["CV_32SC1"] = CV_32SC1;
	map1["CV_32SC2"] = CV_32SC2;
	map1["CV_32SC3"] = CV_32SC3;
	map1["CV_32SC4"] = CV_32SC4;
	//map1["CV_32SC1"] = CV_32SC1;
	map1["CV_32FC1"] = CV_32FC1;
	map1["CV_32FC2"] = CV_32FC2;
	map1["CV_32FC3"] = CV_32FC3;
	map1["CV_32FC4"] = CV_32FC4;
	//map1["CV_32FC1"] = CV_32FC1;
	map1["CV_64FC1"] = CV_64FC1;
	map1["CV_64FC2"] = CV_64FC2;
	map1["CV_64FC3"] = CV_64FC3;
	map1["CV_64FC4"] = CV_64FC4;
	//map1["CV_64FC1"] = CV_64FC1;

	return map1;
}

/**
* @brief Create a reverse map. Used internally
*
* @return map
*/
map<int,string> ParameterImageType::CreateReverseMap()
{
	const map<string, int> map1 = CreateMap();
	map<int, string> out;
	for(map<string, int>::const_iterator it = map1.begin() ; it != map1.end() ; it++)
		out[it->second] = it->first;

	return out;
}

ParameterImageType::ParameterImageType(const string& x_name, int x_default, int * xp_value, const string x_description) :
	ParameterEnum(x_name, x_default, xp_value, x_description)
{
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
	return true;
}

/**
* @brief Print the parameter with default value and range
*
* @param os Output stream
*/
void ParameterEnum::Print(ostream& os) const
{
	os<<m_name<<" = "<<GetReverseEnum().at(GetValue())<<" ["<<GetValue()<<"] ("<<configType[m_confSource]<<"); ";
}

