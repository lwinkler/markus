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
#include "Object.h"
#include "util.h"
#include "define.h"
#include <jsoncpp/json/writer.h>
#include <jsoncpp/json/reader.h>
#include <boost/lexical_cast.hpp>
#include "Factories.h"

using namespace std;
using namespace cv;

log4cxx::LoggerPtr Object::m_logger(log4cxx::Logger::getLogger("Object"));

Object::Object(const string& x_name) :
	m_name(x_name),
	m_id(-1),
	posX(0),
	posY(0),
	width(0),
	height(0)
{
};

// note: we must use cv::Rect in this file since Rect() is a method
Object::Object(const string& x_name, const cv::Rect& x_rect) :
	m_name(x_name)
{
	m_id     = -1;
	SetRect(x_rect);
}

Object::Object(const Object & x_obj)
	: m_name(x_obj.GetName())
{
	m_id = x_obj.GetId();
	posX = x_obj.posX;
	posY = x_obj.posY;
	width = x_obj.width;
	height = x_obj.height;
	m_feats = x_obj.m_feats;
}

Object& Object::operator=(const Object & x_obj)
{
	m_name = x_obj.GetName();
	m_id = x_obj.GetId();
	posX = x_obj.posX;
	posY = x_obj.posY;
	width = x_obj.width;
	height = x_obj.height;

	m_feats = x_obj.m_feats;
	return *this;
}

Object::~Object()
{
	m_feats.clear();
}

void Object::Serialize(ostream& x_out, const string& x_dir) const
{
	Json::Value root;
	root["id"]     = m_id;
	root["name"]   = m_name;
	root["x"]      = posX;
	root["y"]      = posY;
	root["width"]  = width;
	root["height"] = height;

	for(const auto & elem : m_feats)
	{
		stringstream ss;
		elem.second->Serialize(ss, x_dir);
		ss >> root["features"][elem.first];
	}
	if(m_feats.empty())
		root["features"] = Json::Value::null;

	x_out << root;
}

void Object::Deserialize(istream& x_in, const string& x_dir)
{
	Json::Value root;
	x_in >> root;

	m_id   = root["id"].asInt();
	m_name = root["name"].asString();
	posX   = root["x"].asDouble();
	posY   = root["y"].asDouble();
	width  = root["width"].asDouble();
	height = root["height"].asDouble();

	m_feats.clear();
	// TODO: JsonCpp has a bug for serializing floats !

	// Get an instance of the feature factory
	const FactoryFeatures& factory(Factories::featuresFactoryBySignature());

	for(const auto& elem : root["features"].getMemberNames())
	{
		// Extract the signature of the feature:
		//     this allows us to recognize the type of feature
		stringstream ss;
		ss << root["features"][elem];
		string signature = Serializable::signature(ss);
		Feature* feat = factory.Create(signature);
		stringstream ss2;
		ss2 << root["features"][elem];
		feat->Deserialize(ss2, x_dir);
		AddFeature(elem, feat);
	}
}


/**
* @brief Draw an object on an image (for visualization)
*
* @param x_output Image to draw the object on
* @param x_color  Color to use
*/
void Object::RenderTo(Mat& x_output, const Scalar& x_color) const
{
	//Rect rect = it1->GetRect();
	Point2d p1(posX - width / 2, posY - height / 2);
	Point2d p2(posX + width / 2, posY + height / 2);

	// Draw the rectangle in the input image
	// if id is present, draw to the equivalent color
#ifndef MARKUS_DEBUG_STREAMS
	rectangle(x_output, p1, p2, Scalar(20,0,230), 3, 8, 0 );
#else
	Scalar color = x_color;
	Point pText = p1;

	if(GetId() >= 0)
	{
		color = colorFromId(GetId());
		ostringstream text;
		text<<GetName()<<" "<<GetId();
		try
		{
			text<<" "<<dynamic_cast<const FeatureString&>(GetFeature("class")).value;
		}
		catch(FeatureNotFoundException &e) {}
		pText.y -= 3;
		putText(x_output, text.str(), pText,  FONT_HERSHEY_COMPLEX_SMALL, 0.4, color);
	}
	else
	{
		// color from stream
		pText.y -= 3;
		putText(x_output, GetName(), pText, FONT_HERSHEY_COMPLEX_SMALL, 0.4, color);
	}
	rectangle(x_output, p1, p2, color, 1, 8, 0 );

	// Print features and values: Only if debug enabled in log
	if(m_logger->isDebugEnabled())
	{
		pText.x += 2;
		int i = 0;
		for(const auto & elem : m_feats)
		{
			//try
			{
				ostringstream text;
				text << elem.first << "=" << *elem.second;
				pText.y += 7;
				putText(x_output, text.str(), pText,  FONT_HERSHEY_COMPLEX_SMALL, 0.4, color);
				i++;
			}
			//catch(...){}
		}
	}
#endif
}

/**
* @brief Intersect the object with an image. Used to check that objects are valid
*
* @param x_image Image
*
* @return Intersected object
*/

void Object::Intersect(const Mat& x_image)
{
	// cout<<"in "<<posX<<" "<<posY<<" "<<width<<" "<<height<<endl;
	const cv::Rect& rect(GetRect());
	Point2d tl = rect.tl();
	Point2d br = rect.br();

	if(tl.x < 0 || tl.y < 0
			|| br.x > x_image.cols - 1 || br.y > x_image.rows - 1)
	{
		LOG_DEBUG(m_logger, "Correcting object " + GetName());

		tl.x = RANGE(tl.x, 0, x_image.cols - 1);
		tl.y = RANGE(tl.y, 0, x_image.rows - 1);

		br.x = RANGE(br.x, 0, x_image.cols - 1);
		br.y = RANGE(br.y, 0, x_image.rows - 1);

		// recompute object boundaries
		SetRect(cv::Rect(tl, br));
		// cout<<"out "<<posX<<" "<<posY<<" "<<width<<" "<<height<<endl;
	}
}

/// Randomize the content of the object
void Object::Randomize(unsigned int& xr_seed, const string& x_requirement, const Size& x_size)
{
	SetRect(cv::Rect_<double>(
				Point2d(rand_r(&xr_seed) % x_size.width, rand_r(&xr_seed) % x_size.height),
				Point2d(rand_r(&xr_seed) % x_size.width, rand_r(&xr_seed) % x_size.height))
		   );
	if(x_requirement != "")
	{
		Json::Value root;
		Json::Reader reader;
		if(!reader.parse(x_requirement, root, false))
			throw MkException("Error parsing requirement: " + x_requirement, LOC);
		int minWidth = root["width"].get("min", 0).asInt();
		int maxWidth = root["width"].get("max", x_size.width).asInt();
		int minHeight = root["height"].get("min", 0).asInt();
		int maxHeight = root["height"].get("max", x_size.height).asInt();
		width  = RANGE(width,  minWidth,  maxWidth);
		height = RANGE(height, minHeight, maxHeight);
		Mat bounds(x_size, CV_8UC1);
		Intersect(bounds);
	}

	// Add features
	m_feats.clear();
	if(x_requirement != "")
	{
		Json::Value root;
		Json::Reader reader;
		// cout<<x_requirement<<endl;
		if(!reader.parse(x_requirement, root, false))
			throw MkException("Error parsing requirement: " + x_requirement, LOC);
		Json::Value req = root["features"];
		if(!req.isNull())
		{
			// Get an instance of the feature factory
			const FactoryFeatures& factory(Factories::featuresFactory());
			for(const auto& elem : req.getMemberNames())
			{
				// create features according to the requirement
				Feature* feat = factory.Create(req[elem]["type"].asString());
				stringstream ss;
				ss << req[elem];
				feat->Randomize(xr_seed, ss.str());
				AddFeature(elem, feat);
			}
		}
	}

	int nb = rand_r(&xr_seed) % 100;
	// note: notify event does not accept empty features, so we add 1
	for(int i = 0 ; i < nb + 1 ; i++)
	{
		stringstream name;
		name<<"rand"<<i;
		AddFeature(name.str(), static_cast<float>(rand_r(&xr_seed)) / RAND_MAX);
	}
	// LOG_DEBUG(m_logger, "Generate random object with requirements:\""<<x_requirement<<"\" --> "<<this->SerializeToString());

	m_id = rand_r(&xr_seed);
}
