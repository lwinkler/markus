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
		m_range["min"] = x_min;
		m_range["max"] = x_max;
	}
	inline ConfigReader GetValue() const {return mr_value;}
	inline ConfigReader GetDefault() const {return m_default;}
	static T castJson(const ConfigReader& x_json);
	inline const ParameterType& GetParameterType() const {return m_type;}
	inline const std::string& GetType() const {return m_typeStr;}

	virtual void SetValue(const ConfigReader& rx_value, ParameterConfigType x_confType/* = PARAMCONF_UNKNOWN*/)
	{
		if(IsLocked())
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		mr_value = castJson(rx_value);
		m_confSource = x_confType;
	}
	// inline void SetValue(T x_value, ParameterConfigType x_confType/* = PARAMCONF_UNKNOWN*/)
	// {
		// if(IsLocked())
			// throw MkException("You tried to set the value of a locked parameter.", LOC);
		// mr_value = x_value;
		// m_confSource = x_confType;
	// }
	virtual void SetDefault(const ConfigReader& rx_value) override
	{
		m_default = castJson(rx_value);
		m_confSource = PARAMCONF_DEF;
	}
	virtual bool CheckRange() const
	{
		T value = castJson(GetValue());
		// std::cout << "min" << m_min << " " << m_max << std::endl;
		return (value <= m_range["max"].asDouble() + EPSILON && value >= m_range["min"].asDouble() - EPSILON);
	}
	virtual Json::Value GenerateValues(int x_nbSamples, const Json::Value& x_range) const
	{
		const Json::Value range = x_range.isNull() ? GetRange() : x_range;
		const ParameterType& type = GetParameterType();
		if(range.isMember("allowed"))
			return range["allowed"];
		double min = range["min"].asDouble();
		double max = range["max"].asDouble();
		Json::Value values = Json::arrayValue;

		if((type == PARAM_UINT || type == PARAM_INT || type == PARAM_BOOL) && max - min + 1 <= x_nbSamples)
		{
			for(int i = min ; i <= max ; i++)
			{
				values.append(i);
				// values.append(static_cast<int>(min + static_cast<int>(i/x_nbSamples) % static_cast<int>(max - min + 1)));
			}
		}
		else if(type == PARAM_UINT || type == PARAM_INT || type == PARAM_BOOL)
		{
			double incr = x_nbSamples <= 1 ? 0 : (max - min) / (x_nbSamples - 1);
			for(int i = 0 ; i < x_nbSamples ; i++)
			{
				values.append(static_cast<int>(min + i * incr));
			}
		}
		else
		{
			// x_nbSamples values in range
			double incr = static_cast<double>(max - min) / (x_nbSamples - 1);
			// std::cout << x_nbSamples << std::endl;
			for(int i = 0 ; i < x_nbSamples ; i++)
			{
				values.append(min + i * incr);
			}
		}
		if(values.empty())
			throw MkException("Value array is empty, range= " + jsonToString(range), LOC);
		return values;
	}
	virtual void Print(std::ostream& os) const
	{
		os<<GetName()<<"="<<castJson(GetValue())<<" ["<<m_range["min"].asDouble()<<":"<<m_range["max"].asDouble()<<"] ("<<configType[m_confSource]<<"); ";
	}
	virtual void SetValueToDefault()
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



/// Parameter of type integer
typedef ParameterNum<int> 	ParameterInt;
/// Parameter of type unsigned integer
typedef ParameterNum<unsigned int> ParameterUInt;
/// Parameter of type double
typedef ParameterNum<double> 	ParameterDouble;
/// Parameter of type float
typedef ParameterNum<float> 	ParameterFloat;
/// Parameter of type boolean
typedef ParameterNum<bool> 	ParameterBool;

#endif
