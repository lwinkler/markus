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

using namespace std;

// Static variables
template<> const ParameterType ParameterNum<bool>::m_type         = PARAM_BOOL;
template<> const ParameterType ParameterNum<int>::m_type          = PARAM_INT;
template<> const ParameterType ParameterNum<unsigned int>::m_type = PARAM_UINT;
template<> const ParameterType ParameterNum<float>::m_type        = PARAM_FLOAT;
template<> const ParameterType ParameterNum<double>::m_type       = PARAM_DOUBLE;

template<> const string ParameterNum<bool>::m_typeStr         = "bool";
template<> const string ParameterNum<unsigned int>::m_typeStr = "unsigned int";
template<> const string ParameterNum<int>::m_typeStr          = "int";
template<> const string ParameterNum<float>::m_typeStr        = "float";
template<> const string ParameterNum<double>::m_typeStr       = "double";

/**
 * @brief Generate valuew in range
 *
 * @param x_nbSamples Number of valuew to generate
 * @param rx_values Output
 * @param x_range      Range (if empty take parameter range)
 *
 */
