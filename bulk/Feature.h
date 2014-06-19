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

#include <map>
#include <string>

/**
* @brief Class representing a feature of a template/object. (e.g. area, perimeter, length, ...)
*/
class Feature
{
	public:
		Feature(float value);
		Feature(const Feature&);
		Feature& operator = (const Feature&);
		~Feature();
		
		void Update(float x_currentValue, double x_alpha);
		
	public:
		// The different values of the feature
		float value;
		float mean;
		float sqVariance;
		float initial;
		float min;
		float max;
		int    nbSamples;
};

class FeaturePtr
{
	public:
		//FeaturePtr() : mp_feat (NULL){}
		FeaturePtr(Feature* x_feat) : mp_feat(x_feat){}
		FeaturePtr(const FeaturePtr& x_feat) : mp_feat(new Feature(*x_feat)) {}
		~FeaturePtr(){delete mp_feat;}
		inline FeaturePtr& operator = (const FeaturePtr& x_feat){*mp_feat = *x_feat;}
		
		inline void Update(float x_currentValue, double x_alpha){mp_feat->Update(x_currentValue, x_alpha);}
		const Feature& operator* () const {return *mp_feat;}

	protected:
		Feature* const mp_feat;
};

#endif
