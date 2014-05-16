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
#include <sstream>

#include <opencv2/opencv.hpp>
#include "Serializable.h"


using namespace std;

string Serializable::SerializeToString(const string& x_dir) const
{
	stringstream ss;
	Serialize(ss, x_dir);
	Json::Value root;
	ss >> root;
	Json::FastWriter writer;
	string tmp = writer.write(root);
	tmp.erase(std::remove(tmp.begin(), tmp.end(), '\n'), tmp.end());
	return tmp;
}
