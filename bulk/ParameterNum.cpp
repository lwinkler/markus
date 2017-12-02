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

#include "ParameterNum.h"
#include "Polygon.h"

using namespace std;

// Static variables
template<> log4cxx::LoggerPtr ParameterNum<bool>::m_logger(log4cxx::Logger::getLogger("ParameterNum<bool>"));
template<> log4cxx::LoggerPtr ParameterNum<int>::m_logger(log4cxx::Logger::getLogger("ParameterNum<int>"));
template<> log4cxx::LoggerPtr ParameterNum<unsigned int>::m_logger(log4cxx::Logger::getLogger("ParameterNum<unsigned int>"));
template<> log4cxx::LoggerPtr ParameterNum<float>::m_logger(log4cxx::Logger::getLogger("ParameterNum<float>"));
template<> log4cxx::LoggerPtr ParameterNum<double>::m_logger(log4cxx::Logger::getLogger("ParameterNum<double>"));
template<> log4cxx::LoggerPtr ParameterNum<string>::m_logger(log4cxx::Logger::getLogger("ParameterNum<string>"));
template<> log4cxx::LoggerPtr ParameterNum<Polygon>::m_logger(log4cxx::Logger::getLogger("ParameterNum<Polygon>"));

// Static variables
template<> const ParameterType ParameterNum<bool>::m_type         = PARAM_BOOL;
template<> const ParameterType ParameterNum<int>::m_type          = PARAM_INT;
template<> const ParameterType ParameterNum<unsigned int>::m_type = PARAM_UINT;
template<> const ParameterType ParameterNum<float>::m_type        = PARAM_FLOAT;
template<> const ParameterType ParameterNum<double>::m_type       = PARAM_DOUBLE;
template<> const ParameterType ParameterNum<string>::m_type       = PARAM_STR;
template<> const ParameterType ParameterNum<Polygon>::m_type      = PARAM_POLYGON;

template<> const string ParameterNum<bool>::m_typeStr         = "bool";
template<> const string ParameterNum<unsigned int>::m_typeStr = "unsigned int";
template<> const string ParameterNum<int>::m_typeStr          = "int";
template<> const string ParameterNum<float>::m_typeStr        = "float";
template<> const string ParameterNum<double>::m_typeStr       = "double";
template<> const string ParameterNum<string>::m_typeStr       = "string";
template<> const string ParameterNum<Polygon>::m_typeStr      = "Polygon";

template<class T> inline bool checkRangeNum(const T& x_value, const mkjson& x_range)
{
	// std::cout << "min" << m_min << " " << m_max << std::endl;
	if(x_range.find("max") != x_range.end() && x_value > x_range.at("max").template get<T>())
		return false;
	if(x_range.find("min") != x_range.end() && x_value < x_range.at("min").template get<T>())
		return false;
	return true;
}

template<> bool ParameterNum<int>::CheckRange() const
{
	return checkRangeNum(GetValue().template get<int>(), m_range);
}

template<> bool ParameterNum<unsigned int>::CheckRange() const
{
	return checkRangeNum(GetValue().template get<unsigned int>(), m_range);
}

template<> bool ParameterNum<bool>::CheckRange() const
{
	return checkRangeNum(GetValue().template get<bool>(), m_range);
}

template<> bool ParameterNum<double>::CheckRange() const
{
	return checkRangeNum(GetValue().template get<double>(), m_range);
}

template<> bool ParameterNum<float>::CheckRange() const
{
	return checkRangeNum(GetValue().template get<float>(), m_range);
}

template<class T> mkjson generateValuesFloat(int x_nbSamples, const mkjson& x_range)
{
	// const mkjson range = x_range.is_null() ? GetRange() : x_range;
	if(x_range.find("allowed") != x_range.end())
		return x_range.at("allowed");
	T min = x_range.find("min") != x_range.end() ? x_range.at("min").get<T>() : std::numeric_limits<T>::min();
	T max = x_range.find("max") != x_range.end() ? x_range.at("max").get<T>() : std::numeric_limits<T>::max();
	if(min == max && x_nbSamples > 1)
		x_nbSamples = 1;
	mkjson values = mkjson::array();

	// x_nbSamples values in range
	double incr = static_cast<double>(max - min) / (x_nbSamples - 1);
	// std::cout << x_nbSamples << std::endl;
	for(int i = 0 ; i < x_nbSamples ; i++)
	{
		double val = min + i * incr;
		if(val <= max) // handle infinite case
			values.push_back(val);
	}
	if(values.empty())
		throw MkException("Value array is empty, range= " + oneLine(x_range), LOC);
	return values;
}

template<class T> mkjson generateValuesInt(int x_nbSamples, const mkjson& x_range)
{
	if(x_range.find("allowed") != x_range.end())
		return x_range.at("allowed");
	T min = x_range.find("min") != x_range.end() ? x_range.at("min").get<T>() : std::numeric_limits<T>::min();
	T max = x_range.find("max") != x_range.end() ? x_range.at("max").get<T>() : std::numeric_limits<T>::max();
	if(min == max && x_nbSamples > 1)
		x_nbSamples = 1;
	mkjson values = mkjson::array();
	if(static_cast<int>(max - min + 1) <= x_nbSamples)
	{
		for(int i = static_cast<int>(min) ; i <= static_cast<int>(max) ; i++)
		{
			values.push_back(i);
			// values.append(static_cast<int>(min + static_cast<int>(i/x_nbSamples) % static_cast<int>(max - min + 1)));
		}
	}
	else
	{
		double incr = x_nbSamples <= 1 ? 0 : (max - min) / (x_nbSamples - 1);
		for(int i = 0 ; i < x_nbSamples ; i++)
		{
			values.push_back(static_cast<int>(min + i * incr));
		}
	}

	if(values.empty())
		throw MkException("Value array is empty, range= " + oneLine(x_range), LOC);
	return values;
}

template<> mkjson ParameterNum<int>::GenerateValues(int x_nbSamples, const mkjson& x_range) const
{
	return generateValuesInt<int>(x_nbSamples, x_range.is_null() ? GetRange() : x_range);
}

template<> mkjson ParameterNum<unsigned int>::GenerateValues(int x_nbSamples, const mkjson& x_range) const
{
	return generateValuesInt<unsigned int>(x_nbSamples, x_range.is_null() ? GetRange() : x_range);
}

template<> mkjson ParameterNum<bool>::GenerateValues(int x_nbSamples, const mkjson& x_range) const
{
	return generateValuesInt<bool>(x_nbSamples, x_range.is_null() ? GetRange() : x_range);
}

template<> mkjson ParameterNum<double>::GenerateValues(int x_nbSamples, const mkjson& x_range) const
{
	return generateValuesFloat<double>(x_nbSamples, x_range.is_null() ? GetRange() : x_range);
}

template<> mkjson ParameterNum<float>::GenerateValues(int x_nbSamples, const mkjson& x_range) const
{
	return generateValuesFloat<float>(x_nbSamples, x_range.is_null() ? GetRange() : x_range);
}
