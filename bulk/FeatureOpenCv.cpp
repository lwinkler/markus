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
/*
#include "FeatureOpenCv.h"
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

using namespace std;
using namespace cv;

FeatureT<KeyPoint>::FeatureT(const KeyPoint&  x_keypoint)
	: Feature()
{
	keypoint = x_keypoint;
}

double FeatureT<KeyPoint>::Compare2(const Feature& x_feature) const
{
	const KeyPoint& kp(dynamic_cast<const FeatureT<KeyPoint>&>(x_feature).keypoint);
	return !(keypoint.angle == kp.angle
		&& keypoint.class_id == kp.class_id
		&& keypoint.octave == kp.octave
		&& keypoint.pt == kp.pt
		&& keypoint.response == kp.response
		&& keypoint.size == kp.size);
}

void FeatureT<KeyPoint>::Randomize(unsigned int& xr_seed, const std::string& x_param)
{
	assert(false); // TODO
}

void FeatureT<KeyPoint>::Serialize(ostream& x_out, const string& x_dir) const
{
	Json::Value root;
	root["angle"] = keypoint.angle;
	root["classId"] = keypoint.class_id;
	root["octave"] = keypoint.octave;
	root["x"] = keypoint.pt.x;
	root["y"] = keypoint.pt.y;
	root["response"] = keypoint.response;
	root["size"] = keypoint.size;

	Json::FastWriter writer;
	string tmp = writer.write(root);
	tmp.erase(std::remove(tmp.begin(), tmp.end(), '\n'), tmp.end());
	x_out<<tmp;
}

template<> void FeatureT<Point3f>::Deserialize(istream& x_in, const string& x_dir){}

void FeatureT<KeyPoint>::Deserialize(istream& x_in, const string& x_dir)
{
	Json::Value root;
	x_in >> root;
	keypoint.angle = root["angle"].asFloat();
	keypoint.class_id = root["classId"].asInt();
	keypoint.octave = root["octave"].asInt();
	keypoint.pt.x = root["x"].asFloat();
	keypoint.pt.y = root["y"].asFloat();
	keypoint.response = root["response"].asFloat();
	keypoint.size = root["size"].asFloat();
}
*/
