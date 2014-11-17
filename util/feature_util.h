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

#ifndef FEATURE_UTIL_H
#define FEATURE_UTIL_H

#include <opencv2/features2d/features2d.hpp>
#include "define.h"
#include "MkException.h"


/** @brief This file contains some usefull functions for features and serialization. These are used by the FeatureT template.
 operator << is the stream operator for serialization
 operator >> is the stream operator for deserialization
 comparedSquared is used in feature comparison
 randomize is used mostly for testing
 
 If a function is commented this probably means that the default operator for the class is used
*/

/* -------------------------------------------------------------------------------- */
// Acquire a char from stream and verify
template <char Expect>
inline std::istream& get_char(std::istream& in)
{
	char c;
	if (in >> c && c != Expect) {
		throw MkException("Error in format", LOC);
	}
	return in;
}

/* -------------------------------------------------------------------------------- */
// Template specialization for features of type KeyPoint

std::ostream& serialize(std::ostream& x_out, const cv::KeyPoint& x_val);
std::istream& deserialize(std::istream& x_in,  cv::KeyPoint& xr_val);
inline double compareSquared(const cv::KeyPoint& x_1, const cv::KeyPoint& x_2)
{
	return !(x_1.angle      == x_2.angle
		&& x_1.class_id == x_2.class_id
		&& x_1.octave   == x_2.octave
		&& x_1.pt       == x_2.pt
		&& x_1.response == x_2.response
		&& x_1.size     == x_2.size);
}
void randomize(cv::KeyPoint& xr_val, unsigned int& xr_seed);

/* -------------------------------------------------------------------------------- */
// Template specialization for features of type Point3f

inline std::ostream& serialize(std::ostream& x_out, const cv::Point3f& x_value) { x_out << x_value;  return x_out; }
std::istream& deserialize(std::istream& x_in,  cv::Point3f& xr_value);

inline double compareSquared(const cv::Point3f& x_1, const cv::Point3f& x_2){return x_1 != x_2;}
void randomize(cv::Point3f& xr_val, unsigned int& xr_seed);

/* -------------------------------------------------------------------------------- */
// Template specialization for features of type float

inline std::ostream& serialize(std::ostream& x_out, float x_value)   { x_out << x_value;  return x_out; }
inline std::istream& deserialize(std::istream& x_in, float& xr_value){ x_in  >> xr_value; return x_in;  }

inline double compareSquared(float x_1, float x_2)
{
	return POW2(x_1 - x_2);
}

inline void randomize(float& xr_val, unsigned int& xr_seed)
{
	xr_val = static_cast<float>(rand_r(&xr_seed)) / RAND_MAX;
}

/* -------------------------------------------------------------------------------- */
// Template specialization for features of type int

inline std::ostream& serialize(std::ostream& x_out, int x_value)   { x_out << x_value;  return x_out; }
inline std::istream& deserialize(std::istream& x_in, int& xr_value){ x_in  >> xr_value; return x_in; }

inline double compareSquared(int x_1, int x_2)
{
	return POW2(x_1 - x_2);
}

inline void randomize(int& xr_val, unsigned int& xr_seed)
{
	xr_val = rand_r(&xr_seed) % 1000;
}

/* -------------------------------------------------------------------------------- */
// Template specialization for features of type string

inline std::ostream& serialize(std::ostream& x_out, const std::string& x_value) { x_out << '"' << x_value << '"'; return x_out;}
inline std::istream& deserialize(std::istream& x_in, std::string& xr_value)
{
	x_in >> get_char<'"'> >> xr_value;
	xr_value.erase( xr_value.size() - 1 ); // erase the last character. Should be "
	return x_in;
}

inline double compareSquared(const std::string& x_1, const std::string& x_2)
{
	return x_1 != x_2;
}

inline void randomize(std::string& xr_val, unsigned int& xr_seed)
{
	xr_val = "random_str"; // TODO
}


#endif
