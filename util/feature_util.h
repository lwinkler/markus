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
#include "json.hpp"
#include "define.h"
#include "MkException.h"
#include <boost/circular_buffer.hpp>


/** @brief This file contains some usefull functions for features and serialization. These are used by the FeatureT template.
 comparedSquared is used in feature comparison
 randomize is used mostly for testing

 If a function is commented this probably means that the default operator for the class is used
*/

/* -------------------------------------------------------------------------------- */
// Template specialization for features of type Point2f

namespace cv {
	template<class T> inline void to_json(mk::mkjson& _json, const Point_<T>& _pt) {
		_json = mk::mkjson{{"x", _pt.x}, {"y", _pt.y}};
	}
	template<class T> inline void from_json(const mk::mkjson& _json, Point_<T>& _pt) {
		_pt.x = _json.at("x").get<T>();
		_pt.y = _json.at("y").get<T>();
	}
} // namespace cv

inline double compareSquared(const cv::Point2f& x_1, const cv::Point2f& x_2) {return x_1 != x_2;}
void randomize(cv::Point2f& xr_val, unsigned int& xr_seed);

/* -------------------------------------------------------------------------------- */
// Template specialization for features of type Point2d

inline double compareSquared(const cv::Point2d& x_1, const cv::Point2d& x_2) {return x_1 != x_2;}
void randomize(cv::Point2d& xr_val, unsigned int& xr_seed);

/* -------------------------------------------------------------------------------- */
// Template specialization for features of type Point3f

namespace cv {
	template<class T> inline void to_json(mk::mkjson& _json, const Point3_<T>& _pt) {
		_json = mk::mkjson{
			{"x", _pt.x},
			{"y", _pt.y},
			{"z", _pt.z}
		};
	}
	template<class T> inline void from_json(const mk::mkjson& _json, Point3_<T>& _pt) {
		_pt.x = _json.at("x").get<T>();
		_pt.y = _json.at("y").get<T>();
		_pt.z = _json.at("z").get<T>();
	}
} // namespace cv

inline double compareSquared(const cv::Point3f& x_1, const cv::Point3f& x_2) {return x_1 != x_2;}
void randomize(cv::Point3f& xr_val, unsigned int& xr_seed);

/* -------------------------------------------------------------------------------- */
// Template specialization for features of type KeyPoint

namespace cv {
	inline void to_json(mk::mkjson& _json, const KeyPoint& _kp) {
		_json = mk::mkjson{
			{"angle", _kp.angle}, 
			{"classId", _kp.class_id}, 
			{"octave", _kp.octave}, 
			{"pt", _kp.pt}, 
			{"response", _kp.response},
			{"size", _kp.size}
		};
	}
	inline void from_json(const mk::mkjson& _json, KeyPoint& _kp) {
		_kp.angle = _json.at("angle").get<float>();
		_kp.class_id = _json.at("classId").get<int>();
		_kp.octave = _json.at("octave").get<int>();
		_kp.pt = _json.at("pt").get<Point2f>();
		_kp.response = _json.at("response").get<float>();
		_kp.size = _json.at("size").get<float>();
	}
} // namespace cv

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
// Template specialization for features of type float

inline double compareSquared(float x_1, float x_2)
{
	return POW2(x_1 - x_2);
}

inline void randomize(float& xr_val, unsigned int& xr_seed)
{
	xr_val = static_cast<float>(rand_r(&xr_seed)) / RAND_MAX;
}

/* -------------------------------------------------------------------------------- */
// Template specialization for features of type double

inline double compareSquared(double x_1, double x_2)
{
	return POW2(x_1 - x_2);
}

inline void randomize(double& xr_val, unsigned int& xr_seed)
{
	xr_val = static_cast<double>(rand_r(&xr_seed)) / RAND_MAX;
}


/* -------------------------------------------------------------------------------- */
// Template specialization for features of type int

inline double compareSquared(int x_1, int x_2)
{
	// Cast to double to avoid overflows
	return POW2(x_1 - x_2);
}

inline void randomize(int& xr_val, unsigned int& xr_seed)
{
	xr_val = rand_r(&xr_seed) % 1000;
}

/* -------------------------------------------------------------------------------- */
// Template specialization for features of type uint

inline double compareSquared(uint x_1, uint x_2)
{
	// Cast to double to avoid overflows
	return POW2(static_cast<int>(x_1 - x_2));
}

inline void randomize(uint& xr_val, unsigned int& xr_seed)
{
	xr_val = rand_r(&xr_seed) % 1000;
}

/* -------------------------------------------------------------------------------- */
// Template specialization for features of type bool

inline double compareSquared(bool x_1, bool x_2)
{
	// Cast to double to avoid overflows
	return POW2(static_cast<double>(x_1) - x_2);
}

inline void randomize(bool& xr_val, unsigned int& xr_seed)
{
	xr_val = rand_r(&xr_seed) % 2;
}


/* -------------------------------------------------------------------------------- */
// Template specialization for features of type string

inline double compareSquared(const std::string& x_1, const std::string& x_2)
{
	return x_1 != x_2;
}

inline void randomize(std::string& xr_val, unsigned int& xr_seed)
{
	static const std::string alphanum =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	int len = rand_r(&xr_seed) % 100;

	xr_val = "";
	for (int i = 0; i < len; ++i)
		xr_val.push_back(alphanum[rand_r(&xr_seed) % (alphanum.size() - 1)]);
}

/* -------------------------------------------------------------------------------- */
// Serialization function for class serializable


/* -------------------------------------------------------------------------------- */
// Template specialization for features in vectors (must be at the end)


template<class T> double compareSquared(const std::vector<T>& x_1, const std::vector<T>& x_2)
{
	if(x_1.size() != x_2.size())
		return 1;
	if(x_1.empty())
		return 0;
	// throw MkException("Size error while comparing FeatureVectorFloats", LOC);

	double sum = 0;
	auto it2 = x_2.begin();
	for(const auto& elem1 : x_1)
	{
		sum += compareSquared(elem1, *it2);
		++it2;
	}
	return sum / POW2(x_1.size());
}

template<class T>void randomize(std::vector<T>& xr_val, unsigned int& xr_seed, size_t x_size = 10)
{
	xr_val.resize(x_size);
	for(auto& elem : xr_val)
		randomize(elem, xr_seed);
}

/* -------------------------------------------------------------------------------- */
// Template specialization for features in circular buffers

template<class T> double compareSquared(const boost::circular_buffer<T>& x_1, const boost::circular_buffer<T>& x_2)
{
	if(x_1.size() != x_2.size())
		return 1;
	if(x_1.empty())
		return 0;
	// throw MkException("Size error while comparing FeatureVectorFloats", LOC);

	double sum = 0;
	auto it2 = x_2.begin();
	for(const auto& elem1 : x_1)
	{
		sum += compareSquared(elem1, *it2);
		++it2;
	}
	return sum / POW2(x_1.size());
}

template<class T>void randomize(boost::circular_buffer<T>& xr_val, unsigned int& xr_seed)
{
	xr_val.resize(10);
	for(auto& elem : xr_val)
		randomize(elem, xr_seed);
}


#endif
