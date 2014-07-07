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
#include "FeatureVectorFloat.h"

using namespace std;

FeatureVectorFloat::FeatureVectorFloat(vector<float> value_init)
	: Feature()
{
	value      = value_init;
	/*
	sqVariance = 0.01;
	mean       = x_value;
	initial    = x_value;
	min        = x_value;
	max        = x_value;
	nbSamples  = 1;
	*/
}

void FeatureVectorFloat::Serialize(ostream& x_out, const string& x_dir) const
{
	//TODO add some serialize code here
}

void FeatureVectorFloat::Deserialize(istream& x_in, const string& x_dir)
{
	//TODO add some deserialize code here
}
