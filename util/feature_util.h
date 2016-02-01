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
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include "define.h"
#include "MkException.h"
#include <boost/circular_buffer.hpp>


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
inline std::istream& get_char(std::istream& x_in)
{
	char c;
	if (x_in >> c && c != Expect)
	{
		throw MkException("Error in format", LOC);
	}
	return x_in;
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
// Template specialization for features of type Point2f

std::ostream& serialize(std::ostream& x_out, const cv::Point2f& x_value);
std::istream& deserialize(std::istream& x_in,  cv::Point2f& xr_value);

inline double compareSquared(const cv::Point2f& x_1, const cv::Point2f& x_2) {return x_1 != x_2;}
void randomize(cv::Point2f& xr_val, unsigned int& xr_seed);

/* -------------------------------------------------------------------------------- */
// Template specialization for features of type Point2d

std::ostream& serialize(std::ostream& x_out, const cv::Point2d& x_value);
std::istream& deserialize(std::istream& x_in,  cv::Point2d& xr_value);

inline double compareSquared(const cv::Point2d& x_1, const cv::Point2d& x_2) {return x_1 != x_2;}
void randomize(cv::Point2d& xr_val, unsigned int& xr_seed);

/* -------------------------------------------------------------------------------- */
// Template specialization for features of type Point3f

std::ostream& serialize(std::ostream& x_out, const cv::Point3f& x_value);
std::istream& deserialize(std::istream& x_in,  cv::Point3f& xr_value);

inline double compareSquared(const cv::Point3f& x_1, const cv::Point3f& x_2) {return x_1 != x_2;}
void randomize(cv::Point3f& xr_val, unsigned int& xr_seed);

/* -------------------------------------------------------------------------------- */
// Template specialization for features of type Mat
// 	note: maybe matrices should be treated in image streams but how ?

// std::ostream& serialize(std::ostream& x_out, const cv::Mat& x_value);
// std::istream& deserialize(std::istream& x_in,  cv::Mat& xr_value);
// 
// double compareSquared(const cv::Mat& x_1, const cv::Mat& x_2);
// void randomize(cv::Mat& xr_val, unsigned int& xr_seed);

/* -------------------------------------------------------------------------------- */
// Template specialization for features of type float

inline std::ostream& serialize(std::ostream& x_out, float x_value)   { x_out << x_value;  return x_out; }
inline std::istream& deserialize(std::istream& x_in, float& xr_value) { x_in  >> xr_value; return x_in;  }

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

inline std::ostream& serialize(std::ostream& x_out, double x_value)   { x_out << "{\"valueDouble\":" << x_value << "}";  return x_out; }
inline std::istream& deserialize(std::istream& x_in, double& xr_value)
{
	// Note: we must serialize integer features differently than float to avoid having the same signature
	// this is due to the fact that the serialization does not store the type of the feature
	Json::Value root;
	x_in >> root;
	xr_value = root["valueDouble"].asDouble();
	return x_in;
}

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

inline std::ostream& serialize(std::ostream& x_out, int x_value)   { x_out << "{\"valueInt\":" << x_value << "}";  return x_out; }
inline std::istream& deserialize(std::istream& x_in, int& xr_value)
{
	// Note: we must serialize integer features differently than float to avoid having the same signature
	// this is due to the fact that the serialization does not store the type of the feature
	Json::Value root;
	x_in >> root;
	xr_value = root["valueInt"].asInt();
	return x_in;
}

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

inline std::ostream& serialize(std::ostream& x_out, uint x_value)   { x_out << "{\"valueUInt\":" << x_value << "}";  return x_out; }
inline std::istream& deserialize(std::istream& x_in, uint& xr_value)
{
	// Note: we must serialize integer features differently than float to avoid having the same signature
	// this is due to the fact that the serialization does not store the type of the feature
	Json::Value root;
	x_in >> root;
	xr_value = root["valueUInt"].asInt();
	return x_in;
}

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

inline std::ostream& serialize(std::ostream& x_out, bool x_value)   { x_out << "{\"valueBool\":" << x_value << "}";  return x_out; }
inline std::istream& deserialize(std::istream& x_in, bool& xr_value)
{
	Json::Value root;
	x_in >> root;
	xr_value = root["valueBool"].asBool();
	return x_in;
}

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

inline std::ostream& serialize(std::ostream& x_out, const Serializable& x_value) { x_value.Serialize(x_out, ""); return x_out;}
inline std::istream& deserialize(std::istream& x_in, Serializable& xr_value) {xr_value.Deserialize(x_in, ""); return x_in;}

/* -------------------------------------------------------------------------------- */
// Template specialization for features in vectors (must be at the end)

template<class T>std::ostream& serialize(std::ostream& x_out, const std::vector<T>& x_val)
{
	if(x_val.size() == 0)
	{
		x_out<<"[]";
		return x_out;
	}

	x_out << "[";
	auto it = x_val.begin();
	while(it != x_val.end() - 1)
	{
		serialize(x_out,*it);
		x_out << ",";
		++it;
	}
	serialize(x_out,*it);
	x_out << "]";
	return x_out;
}

template<class T>std::istream& deserialize(std::istream& x_in,  std::vector<T>& xr_val)
{
	Json::Value root;
	x_in >> root;  // note: copy first for local use
	assert(root.isArray());

	xr_val.clear();
	xr_val.resize(root.size());
	for(unsigned int i = 0 ; i < root.size() ; i++)
	{
		std::stringstream ss;
		ss << root[i];
		deserialize(ss, xr_val[i]);
	}

	return x_in;
}

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

template<class T>std::ostream& serialize(std::ostream& x_out, const boost::circular_buffer<T>& x_val)
{
	if(x_val.size() == 0)
	{
		x_out<<"[]";
		return x_out;
	}

	x_out << "[";
	auto it = x_val.begin();
	while(it != x_val.end() - 1)
	{
		serialize(x_out,*it);
		x_out << ",";
		++it;
	}
	serialize(x_out,*it);
	x_out << "]";
	return x_out;
}

template<class T>std::istream& deserialize(std::istream& x_in,  boost::circular_buffer<T>& xr_val)
{
	Json::Value root;
	x_in >> root;  // note: copy first for local use
	assert(root.isArray());

	xr_val.clear();
	xr_val.resize(root.size());
	for(unsigned int i = 0 ; i < root.size() ; i++)
	{
		std::stringstream ss;
		ss << root[i];
		deserialize(ss, xr_val[i]);
	}

	return x_in;
}

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
