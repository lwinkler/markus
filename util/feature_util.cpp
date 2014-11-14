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

#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include "feature_util.h"
#include "MkException.h"

using namespace std;
using namespace cv;

template <char Expect>
std::istream& get_char(std::istream& in)
{
	char c;
	if (in >> c && c != Expect) {
		throw MkException("Error in format", LOC);
	}
	return in;
}

/* -------------------------------------------------------------------------------- */
ostream& operator<< (ostream& x_out, const cv::KeyPoint& x_val)
{
	Json::Value root;
	root["angle"] = x_val.angle;
	root["classId"] = x_val.class_id;
	root["octave"] = x_val.octave;
	root["x"] = x_val.pt.x;
	root["y"] = x_val.pt.y;
	root["response"] = x_val.response;
	root["size"] = x_val.size;

	Json::FastWriter writer;
	string tmp = writer.write(root);
	tmp.erase(remove(tmp.begin(), tmp.end(), '\n'), tmp.end());
	x_out<<tmp;
}

istream& operator>> (istream& x_in,  cv::KeyPoint& xr_val)
{
	Json::Value root;
	x_in >> root;
	xr_val.angle = root["angle"].asFloat();
	xr_val.class_id = root["classId"].asInt();
	xr_val.octave = root["octave"].asInt();
	xr_val.pt.x = root["x"].asFloat();
	xr_val.pt.y = root["y"].asFloat();
	xr_val.response = root["response"].asFloat();
	xr_val.size = root["size"].asFloat();
}

void randomize(KeyPoint& xr_val, unsigned int& xr_seed)
{
	randomize(xr_val.angle, xr_seed);
	randomize(xr_val.class_id, xr_seed);
	randomize(xr_val.octave, xr_seed);
	randomize(xr_val.pt.x, xr_seed);
	randomize(xr_val.pt.y, xr_seed);
	randomize(xr_val.response, xr_seed);
	randomize(xr_val.size, xr_seed);
}
/* -------------------------------------------------------------------------------- */
istream& operator>> (istream& x_in,  cv::Point3f& xr_val)
{
	if(x_in >> get_char<'['> >> xr_val.x
	         >> get_char<','> >> xr_val.y
	         >> get_char<','> >> xr_val.z >> get_char<']'>)
	return x_in; // TODO: Check that all streams are returned
	else throw MkException("Error in format", LOC);

}

void randomize(cv::Point3f& xr_val, unsigned int& xr_seed)
{
	randomize(xr_val.x, xr_seed);
	randomize(xr_val.y, xr_seed);
	randomize(xr_val.z, xr_seed);
}
/* -------------------------------------------------------------------------------- */
