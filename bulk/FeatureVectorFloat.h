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
#include "MkException.h"
#include "feature_util.h"
#include "util.h"
#include <vector>
/**
* @brief Class representing a feature in the form of a vector of float
*/

/**
* @brief Class representing a feature template. This can be used to create new templates
*/
template<class T> class FeatureVectorT : public Feature
{
	public:
		FeatureVectorT(const std::vector<T>& x_values = std::vector<T>(0)){ values = x_values;}
		Feature* CreateCopy() const{return new FeatureVectorT<T>(*this);}
		inline virtual double CompareSquared(const Feature& x_feature) const
		{
			const FeatureVectorT<T>& feat(dynamic_cast<const FeatureVectorT<T>&>(x_feature));
			double sum = 0;
			if(values.size() != feat.values.size())
				return 1;
			// throw MkException("Size error while comparing FeatureVectorFloats", LOC);

			typename std::vector<T>::const_iterator it2 = feat.values.begin();
			for(typename std::vector<T>::const_iterator it1 = values.begin() ; it1 != values.end() ; ++it1, ++it2)
			{
				sum += compareSquared(*it1, *it2);
			}
			return sum / POW2(values.size());
		}
		inline void Randomize(unsigned int& xr_seed, const std::string& x_param)
		{
			values.resize(10);
			for(typename std::vector<T>::iterator it1 = values.begin() ; it1 != values.end() ; ++it1)
				randomize(*it1, xr_seed);
		}
		virtual void Serialize(std::ostream& x_out, const std::string& x_dir) const
		{
			if(values.size() == 0)
			{
				x_out<<"[]";
				return;
			}

			x_out << "[";
			typename std::vector<T>::const_iterator it = values.begin();
			while(it != values.end() - 1)
			{
				x_out << *it << ",";
				++it;
			}
			x_out << *it << "]";
		}
		virtual void Deserialize(std::istream& x_in, const std::string& x_dir)
		{
			// TODO
			std::string tmp;
			std::vector<std::string> valuesStr;
			getline(x_in, tmp);

			if(tmp.substr(0, 1) != "[" || tmp.substr(tmp.size() - 1, 1) != "]")
				throw MkException("Error in deserialization for value: " + tmp, LOC);
			split(tmp.substr(1, tmp.size() - 2), ',', valuesStr);

			// Remove last element if empty, due to an extra comma
			if(valuesStr.back() == "")
				values.pop_back();

			values.clear();
			for(std::vector<std::string>::const_iterator it = valuesStr.begin() ; it != valuesStr.end() ; ++it)
				values.push_back(atof(it->c_str()));
		}
		
		// The value of the feature
		std::vector<T> values;
};


// Definitions
typedef FeatureVectorT<float>   FeatureVectorFloat;


#endif
