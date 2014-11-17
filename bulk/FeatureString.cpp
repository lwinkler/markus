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
#include "FeatureString.h"

using namespace std;

FeatureString::FeatureString(const string& x_value)
	: Feature(),
	value(x_value)
{
}

double FeatureString::CompareSquared(const Feature& x_feature) const
{
	const FeatureString& feat(dynamic_cast<const FeatureString&>(x_feature));
	return value != feat.value;
}

void FeatureString::Randomize(unsigned int& xr_seed, const std::string& x_param)
{
	value = "random_string";
}

void FeatureString::Serialize(ostream& x_out, const string& x_dir) const
{
	x_out << "\"" << value << "\"";
}

void FeatureString::Deserialize(istream& x_in, const string& x_dir)
{
	x_in >> value;
	//assert(value.front() == '"');
	//assert(value.back() == '"');
	value.erase( 0, 1 ); // erase the first character
	value.erase( value.size() - 1 ); // erase the last character
}
