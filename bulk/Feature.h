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
#ifndef MK_FEATURE_H
#define MK_FEATURE_H

#include <string>
#include <vector>

/*! \class Feature
 *  \brief Class representing a feature of a template/region
 *
 * e.g. area, perimeter, length, ...
 */
class Feature
{
	public:
		Feature(double value=0);
		Feature(const Feature&);
		Feature& operator = (const Feature&);
		~Feature();
		
		//inline const char* GetName() const {return m_name;};
		inline double GetValue() const {return m_value;};
		inline void SetValue( double x) {m_value = x;};
		inline double GetVariance() const {return m_variance;};
		inline void SetVariance( double x) {m_variance = x;};
		
		static double GetFeatureValue(const std::vector<Feature>& vect, const char* name);
		
		
	public:
		static std::vector<std::string> m_names; // TODO : Names should be in a static array to save memory space
		double m_value;
		double m_variance;
};


#endif