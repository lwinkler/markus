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

#include "ParameterT.h"
#include "Polygon.h"

using namespace std;


// Static variables
template<> const string ParameterT<bool>::m_type         = "ParameterBool";
template<> const string ParameterT<unsigned int>::m_type = "ParameterUInt";
template<> const string ParameterT<int>::m_type          = "ParameterInt";
template<> const string ParameterT<float>::m_type        = "ParameterFloat";
template<> const string ParameterT<double>::m_type       = "ParameterDouble";
template<> const string ParameterT<string>::m_type       = "ParameterString";
template<> const string ParameterT<Polygon>::m_type      = "ParameterPolygon";

template<class T> inline bool checkRangeNum(const T& x_value, const mkjson& x_range)
{
	if(x_range.find("max") != x_range.end() && x_value > x_range.at("max").template get<T>())
		return false;
	if(x_range.find("min") != x_range.end() && x_value < x_range.at("min").template get<T>())
		return false;
	return true;
}

template<> bool ParameterT<int>::CheckRange() const
{
	return checkRangeNum(GetValue().template get<int>(), m_range);
}

template<> bool ParameterT<unsigned int>::CheckRange() const
{
	return checkRangeNum(GetValue().template get<unsigned int>(), m_range);
}

template<> bool ParameterT<bool>::CheckRange() const
{
	return checkRangeNum(GetValue().template get<bool>(), m_range);
}

template<> bool ParameterT<double>::CheckRange() const
{
	// note: json.hpp seems to convert double to float TODO check this
	return checkRangeNum(GetValue().template get<double>(), m_range);
}

template<> bool ParameterT<float>::CheckRange() const
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
	if(static_cast<int>(max - min + 1) <= x_nbSamples && max != numeric_limits<T>::max())
	{
		for(int i = static_cast<int>(min) ; i <= static_cast<int>(max) ; i++)
		{
			values.push_back(static_cast<T>(i));
			// values.append(static_cast<int>(min + static_cast<int>(i/x_nbSamples) % static_cast<int>(max - min + 1)));
		}
	}
	else
	{
		double incr = x_nbSamples <= 1 ? 0 : (max - min) / (x_nbSamples - 1);
		for(int i = 0 ; i < x_nbSamples ; i++)
		{
			values.push_back(static_cast<T>(min + i * incr));
		}
	}

	if(values.empty())
		throw MkException("Value array is empty, range= " + oneLine(x_range), LOC);
	return values;
}

template<> mkjson ParameterT<int>::GenerateValues(int x_nbSamples, const mkjson& x_range) const
{
	return generateValuesInt<int>(x_nbSamples, x_range.is_null() ? GetRange() : x_range);
}

template<> mkjson ParameterT<unsigned int>::GenerateValues(int x_nbSamples, const mkjson& x_range) const
{
	return generateValuesInt<unsigned int>(x_nbSamples, x_range.is_null() ? GetRange() : x_range);
}

template<> mkjson ParameterT<bool>::GenerateValues(int x_nbSamples, const mkjson& x_range) const
{
	return generateValuesInt<bool>(x_nbSamples, x_range.is_null() ? GetRange() : x_range);
}

template<> mkjson ParameterT<double>::GenerateValues(int x_nbSamples, const mkjson& x_range) const
{
	return generateValuesFloat<double>(x_nbSamples, x_range.is_null() ? GetRange() : x_range);
}

template<> mkjson ParameterT<float>::GenerateValues(int x_nbSamples, const mkjson& x_range) const
{
	return generateValuesFloat<float>(x_nbSamples, x_range.is_null() ? GetRange() : x_range);
}
