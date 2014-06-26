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
#ifndef MK_FEATURE_FLOAT_H
#define MK_FEATURE_FLOAT_H

#include "Feature.h"

/**
* @brief Class representing a feature in the form of a float value
*/
class FeatureFloat : public Feature
{
	public:
		FeatureFloat(float x_value);
		Feature* CreateCopy() const{return new FeatureFloat(*this);}
		virtual void Serialize(std::ostream& stream, const std::string& x_dir) const;
		virtual void Deserialize(std::istream& stream, const std::string& x_dir);
		
		// The value of the feature
		float value;
};
#endif
