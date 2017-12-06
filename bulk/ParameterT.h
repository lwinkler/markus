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
#include <limits>
#include <boost/lexical_cast.hpp>

#define EPSILON 1e-5

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

/// Template for all parameters with numerical values
template<class T> class ParameterT : public Parameter
{
public:
	ParameterT(const std::string& x_name, T x_default, T x_min, T x_max, T * xp_value, const std::string& x_description) :
		Parameter(x_name, x_description),
		m_default(x_default),
		mr_value(*xp_value)
	{
		m_range["min"] = x_min;
		m_range["max"] = x_max;
	}
	ParameterT(const std::string& x_name, T x_default, const mkjson& x_range, T * xp_value, const std::string& x_description) :
		Parameter(x_name, x_description),
		m_default(x_default),
		mr_value(*xp_value)
	{
		SetRange(x_range);
	}
	ParameterT(const std::string& x_name, T x_default, T * xp_value, const std::string& x_description) :
		Parameter(x_name, x_description),
		m_default(x_default),
		mr_value(*xp_value)
	{
	}
	inline mkjson GetValue() const override {return mr_value;}
	inline mkjson GetDefault() const override {return m_default;}
	inline const std::string& GetClass() const override {return m_type;}

	void SetValue(const mkjson& rx_value, ParameterConfigType x_confType/* = PARAMCONF_UNKNOWN*/) override
	{
		if(IsLocked())
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		mr_value = rx_value.get<T>();
		m_confSource = x_confType;
	}
	void SetDefault(const mkjson& rx_value) override
	{
		m_default = rx_value.get<T>();
		m_confSource = PARAMCONF_DEF;
	}
	bool CheckRange() const override
	{
		// T value = GetValue().template get<T>();
		// // std::cout << "min" << m_min << " " << m_max << std::endl;
		// if(m_range.find("max") != m_range.end() && value > m_range.at("max").template get<T>())
			// return false;
		// if(m_range.find("min") != m_range.end() && value < m_range.at("min").template get<T>())
			// return false;
		return true;
	}

	// TODO improve: template
	mkjson GenerateValues(int x_nbSamples, const mkjson& x_range) const override
	{
		return mkjson::array();
	}
	void Print(std::ostream& os) const override
	{
		os<<GetName()<<"="<< oneLine(GetValue()) << " " << m_range << " (" << configType[m_confSource] << "); ";
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
	static const std::string m_type;
};

template<> mkjson ParameterT<int>::GenerateValues(int x_nbSamples, const mkjson& x_range) const;
template<> mkjson ParameterT<unsigned int>::GenerateValues(int x_nbSamples, const mkjson& x_range) const;
template<> mkjson ParameterT<double>::GenerateValues(int x_nbSamples, const mkjson& x_range) const;
template<> mkjson ParameterT<float>::GenerateValues(int x_nbSamples, const mkjson& x_range) const;
template<> mkjson ParameterT<bool>::GenerateValues(int x_nbSamples, const mkjson& x_range) const;

template<> bool ParameterT<int>::CheckRange() const;
template<> bool ParameterT<unsigned int>::CheckRange() const;
template<> bool ParameterT<double>::CheckRange() const;
template<> bool ParameterT<float>::CheckRange() const;
template<> bool ParameterT<bool>::CheckRange() const;


/// Define types
typedef ParameterT<int> 	ParameterInt;
typedef ParameterT<unsigned int> ParameterUInt;
typedef ParameterT<double> 	ParameterDouble;
typedef ParameterT<float> 	ParameterFloat;
typedef ParameterT<bool> 	ParameterBool;
typedef ParameterT<std::string> ParameterString;

#endif
