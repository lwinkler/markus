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

#include "ParameterImageType.h"
#include <opencv2/core/core.hpp>

using namespace std;

// Static variables
const map<string, int>  ParameterImageType::Enum        = ParameterImageType::CreateMap();
const map<int, string>  ParameterImageType::ReverseEnum = ParameterImageType::CreateReverseMap(ParameterImageType::Enum);


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


ParameterImageType::ParameterImageType(const string& x_name, int x_default, int * xp_value, const string& x_description) :
	ParameterEnum(x_name, x_default, xp_value, x_description)
{
}

