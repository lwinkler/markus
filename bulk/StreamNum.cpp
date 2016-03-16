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

#include "StreamNum.h"

using namespace std;

// Static variables
template<> log4cxx::LoggerPtr StreamNum<bool>::m_logger(log4cxx::Logger::getLogger("StreamNum<bool>"));
template<> log4cxx::LoggerPtr StreamNum<int>::m_logger(log4cxx::Logger::getLogger("StreamNum<int>"));
template<> log4cxx::LoggerPtr StreamNum<unsigned int>::m_logger(log4cxx::Logger::getLogger("StreamNum<unsigned int>"));
template<> log4cxx::LoggerPtr StreamNum<float>::m_logger(log4cxx::Logger::getLogger("StreamNum<float>"));
template<> log4cxx::LoggerPtr StreamNum<double>::m_logger(log4cxx::Logger::getLogger("StreamNum<double>"));

template<> const string StreamNum<bool>::m_class   = "StreamNum<bool>";
template<> const string StreamNum<int>::m_class    = "StreamNum<int>";
template<> const string StreamNum<uint>::m_class   = "StreamNum<uint>";
template<> const string StreamNum<float>::m_class  = "StreamNum<float>";
template<> const string StreamNum<double>::m_class = "StreamNum<double>";

template<> const string StreamNum<bool>::m_type   = "NumBool";
template<> const string StreamNum<int>::m_type    = "NumInt";
template<> const string StreamNum<uint>::m_type   = "NumUInt";
template<> const string StreamNum<float>::m_type  = "NumFloat";
template<> const string StreamNum<double>::m_type = "NumDouble";

template<> const ParameterType StreamNum<bool>::m_parameterType   = PARAM_STREAM;
template<> const ParameterType StreamNum<int>::m_parameterType    = PARAM_STREAM;
template<> const ParameterType StreamNum<uint>::m_parameterType   = PARAM_STREAM;
template<> const ParameterType StreamNum<float>::m_parameterType  = PARAM_STREAM;
template<> const ParameterType StreamNum<double>::m_parameterType = PARAM_STREAM;
