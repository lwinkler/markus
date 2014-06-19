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
#include "Feature.h"

#define POW2(x) (x) * (x)


FeatureFloat::FeatureFloat(float x_value)
	: Feature()
{
	value      = x_value;
	sqVariance = 0.01;
	mean       = x_value;
	initial    = x_value;
	min        = x_value;
	max        = x_value;
	nbSamples  = 1;
}

FeatureFloat::FeatureFloat(const FeatureFloat& f)
	: Feature()
{
	value      = f.value;
	sqVariance = f.sqVariance;
	mean       = f.mean;
	initial    = f.initial;
	min        = f.min;
	max        = f.max;
	nbSamples  = f.nbSamples;
}

FeatureFloat&  FeatureFloat::operator = (const FeatureFloat& f)
{
	value      = f.value;
	sqVariance = f.sqVariance;
	mean       = f.mean;
	initial    = f.initial;
	min        = f.min;
	max        = f.max;
	nbSamples  = f.nbSamples;

	return *this;
}

/**
* @brief Keep a feature up to date in a dynamic way (similar to a running average)
*
* @param x_currentValue Value to use for updating
* @param x_alpha        Alpha coefficient for running average
*/
void FeatureFloat::Update(float x_currentValue, double x_alpha)
{
	value      = x_currentValue;
	mean       = mean * (1.0 - x_alpha) + x_currentValue * x_alpha;
	sqVariance = sqVariance * (1.0 - x_alpha) + POW2(x_currentValue - mean) * x_alpha;
	if(x_currentValue < min)
		min        = x_currentValue;
	if(x_currentValue > max)
		max        = x_currentValue;
	nbSamples++;
}
