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

#ifndef PARAMETER_NUM_H
#define PARAMETER_NUM_H

#include "define.h"
#include "MkException.h"
#include "Parameter.h"
#include <log4cxx/logger.h>
#include <limits>
#include <boost/lexical_cast.hpp>

#define EPSILON 1e-5

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

/// Template for all parameters with numerical values
template<class T> class ParameterNum : public Parameter
{
public:
	ParameterNum(const std::string& x_name, T x_default, T x_min, T x_max, T * xp_value, const std::string& x_description) :
		Parameter(x_name, x_description),
		m_default(x_default),
		mr_value(*xp_value)
	{
		if(GetType() == "bool")
		{
			if(x_min == 1)
				m_range["min"] = true;
			if(x_max == 0)
				m_range["max"] = false;
		}
		else
		{
			m_range["min"] = x_min;
			m_range["max"] = x_max;
		}
	}
	inline ConfigReader GetValue() const override {return mr_value;}
	inline ConfigReader GetDefault() const override {return m_default;}
	inline const ParameterType& GetParameterType() const override {return m_type;}
	inline const std::string& GetType() const override {return m_typeStr;}

	void SetValue(const ConfigReader& rx_value, ParameterConfigType x_confType/* = PARAMCONF_UNKNOWN*/) override
	{
		if(IsLocked())
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		mr_value = rx_value.get<T>();
		m_confSource = x_confType;
	}
	// inline void SetValue(T x_value, ParameterConfigType x_confType/* = PARAMCONF_UNKNOWN*/)
	// {
		// if(IsLocked())
			// throw MkException("You tried to set the value of a locked parameter.", LOC);
		// mr_value = x_value;
		// m_confSource = x_confType;
	// }
	void SetDefault(const ConfigReader& rx_value) override
	{
		m_default = rx_value.get<T>();
		m_confSource = PARAMCONF_DEF;
	}
	bool CheckRange() const override
	{
		T value = GetValue().template get<T>();
		// std::cout << "min" << m_min << " " << m_max << std::endl;
		if(m_range.find("max") != m_range.end() && value > m_range.at("max").template get<T>())
			return false;
		if(m_range.find("min") != m_range.end() && value < m_range.at("min").template get<T>())
			return false;
		return true;
	}

	// TODO improve: template
	mkjson GenerateValues(int x_nbSamples, const mkjson& x_range) const override
	{
		const mkjson range = x_range.is_null() ? GetRange() : x_range;
		const ParameterType& type = GetParameterType();
		if(range.find("allowed") != range.end())
			return range.at("allowed");
		T min = range.find("min") != range.end() ? range.at("min").get<T>() : std::numeric_limits<T>::min();
		T max = range.find("max") != range.end() ? range.at("max").get<T>() : std::numeric_limits<T>::max();
		if(min == max && x_nbSamples > 1)
			x_nbSamples = 1;
		mkjson values = mkjson::array();

		if((type == PARAM_UINT || type == PARAM_INT || type == PARAM_BOOL) && static_cast<int>(max - min + 1) <= x_nbSamples)
		{
			for(int i = static_cast<int>(min) ; i <= static_cast<int>(max) ; i++)
			{
				values.push_back(i);
				// values.append(static_cast<int>(min + static_cast<int>(i/x_nbSamples) % static_cast<int>(max - min + 1)));
			}
		}
		else if(type == PARAM_UINT || type == PARAM_INT || type == PARAM_BOOL)
		{
			double incr = x_nbSamples <= 1 ? 0 : (max - min) / (x_nbSamples - 1);
			for(int i = 0 ; i < x_nbSamples ; i++)
			{
				values.push_back(static_cast<int>(min + i * incr));
			}
		}
		else
		{
			// x_nbSamples values in range
			double incr = static_cast<double>(max - min) / (x_nbSamples - 1);
			// std::cout << x_nbSamples << std::endl;
			for(int i = 0 ; i < x_nbSamples ; i++)
			{
				double val = min + i * incr;
				if(val <= max) // handle infinite case
					values.push_back(val);
			}
		}
		if(values.empty())
			throw MkException("Value array is empty, range= " + jsonToString(range), LOC);
		return values;
	}
	void Print(std::ostream& os) const override
	{
		os<<GetName()<<"="<< GetValue().template get<T>() << " [";
		if(m_range.find("min") != m_range.end())
			os << m_range["min"].template get<T>();
		os << ":";
		if(m_range.find("max") != m_range.end())
			os << m_range["max"].template get<T>();
		os << "] (" << configType[m_confSource] << "); ";
	}
	void SetValueToDefault() override
	{
		if(IsLocked())
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		mr_value = m_default;
		m_confSource = PARAMCONF_DEF;
	}

	T m_default;
private:
	T& mr_value;
	static const ParameterType m_type;
	static const std::string m_typeStr;
	static log4cxx::LoggerPtr m_logger;
};



/// Define types
typedef ParameterNum<int> 	ParameterInt;
typedef ParameterNum<unsigned int> ParameterUInt;
typedef ParameterNum<double> 	ParameterDouble;
typedef ParameterNum<float> 	ParameterFloat;
typedef ParameterNum<bool> 	ParameterBool;

#endif
