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
#include "FeatureFloatInTime.h"

FeatureFloatInTime::FeatureFloatInTime(const FeatureFloat& x_feat)
	: FeatureFloat(x_feat)
{
	sqVariance = 0.01;
	mean       = value;
	initial    = value;
	min        = value;
	max        = value;
	nbSamples  = 1;
}

/**
* @brief Keep a feature up to date in a dynamic way (similar to a running average)
*
* @param x_currentValue Value to use for updating
* @param x_alpha        Alpha coefficient for running average
*/
void FeatureFloatInTime::Update(const Feature& x_feat, double x_alpha)
{
	const FeatureFloat& ff(dynamic_cast<const FeatureFloat&>(x_feat));
	value      = ff.value;
	mean       = mean * (1.0 - x_alpha) + value * x_alpha;
	sqVariance = sqVariance * (1.0 - x_alpha) + POW2(ff.value - mean) * x_alpha;
	if(ff.value < min)
		min        = ff.value;
	if(ff.value > max)
		max        = ff.value;
	nbSamples++;
}
