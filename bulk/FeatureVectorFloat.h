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
#ifndef MK_FEATURE_HOF_H
#define MK_FEATURE_HOF_H

#include "Feature.h"
#include <vector>
/**
* @brief Class representing a feature in the form of a vector of float
*/
class FeatureVectorFloat : public Feature
{
	public:
		FeatureVectorFloat(std::vector <float> x_values = std::vector<float>(0));
		Feature* CreateCopy() const{return new FeatureVectorFloat(*this);}
		virtual double Compare2(const Feature& x_feature);
		virtual void Serialize(std::ostream& stream, const std::string& x_dir) const;
		virtual void Deserialize(std::istream& stream, const std::string& x_dir);
		
		// The value of the feature
		std::vector<float> values;
};
#endif
