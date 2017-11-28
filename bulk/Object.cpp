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
#include "Serializable.h"
#include "util.h"
#include "define.h"
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

void to_json(mkjson& rx_json, const Object& x_obj) {
	rx_json = mkjson{
		{"id", x_obj.m_id},
		{"name", x_obj.m_name},
		{"x", x_obj.posX},
		{"y", x_obj.posY},
		{"width", x_obj.width},
		{"height", x_obj.height},
		{"features", x_obj.m_feats}
	};
}

void from_json(const mkjson& x_json, Object& rx_obj) {
	rx_obj.m_id = x_json.at("id").get<int>();
	rx_obj.m_name = x_json.at("name").get<string>();
	rx_obj.posY = x_json.at("x").get<double>();
	rx_obj.posY = x_json.at("y").get<double>();
	rx_obj.width = x_json.at("width").get<double>();
	rx_obj.height = x_json.at("height").get<double>();

	// Get an instance of the feature factory
	const FactoryFeatures& factory(Factories::featuresFactoryBySignature());

	for(auto it = x_json.at("features").cbegin() ; it != x_json.at("features").cend() ; ++it)
	{
		// Extract the signature of the feature:
		//     this allows us to recognize the type of feature
		mkjson json(it.value());
		string sign = signature(json);
		Feature* feat = factory.Create(sign);
		feat->Deserialize(json);
		rx_obj.AddFeature(it.key(), feat);
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
				text << elem.first << "=" << mkjson(*elem.second).dump();
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
void Object::Randomize(unsigned int& xr_seed, const mkjson& x_requirement, const Size& x_size)
{
	SetRect(cv::Rect_<double>(
		Point2d(rand_r(&xr_seed) % x_size.width, rand_r(&xr_seed) % x_size.height),
		Point2d(rand_r(&xr_seed) % x_size.width, rand_r(&xr_seed) % x_size.height))
	);
	if(rand_r(&xr_seed) % 20 == 0)
	{
		width  = 0;
		height = 0;
	}
	if(!x_requirement.is_null())
	{
		mkjson widthReq  = x_requirement.value("width", nlohmann::json::object());
		mkjson heightReq = x_requirement.value("height", nlohmann::json::object());
		int minWidth = widthReq.value<int>("min", 0);
		int maxWidth = widthReq.value<int>("max", x_size.width);
		int minHeight = heightReq.value<int>("min", 0);
		int maxHeight = heightReq.value<int>("max", x_size.height);
		width  = RANGE(width,  minWidth,  maxWidth);
		height = RANGE(height, minHeight, maxHeight);
		Mat bounds(x_size, CV_8UC1);
		Intersect(bounds);
	}
	
	// Add features
	m_feats.clear();
	if(!x_requirement.empty())
	{
		const mkjson& req = x_requirement.at("features");
		if(!req.is_null())
		{
			// Get an instance of the feature factory
			const FactoryFeatures& factory(Factories::featuresFactory());
			for(auto it = req.begin() ; it != req.end() ; ++it)
			{
				// create features according to the requirement
				Feature* feat = factory.Create(it.value()["type"].get<string>());
				feat->Randomize(xr_seed, it.value());
				AddFeature(it.key(), feat);
			}
		}
	}

	int nb = rand_r(&xr_seed) % 100;
	// note: notify event does not accept empty features, so we add 1
	for(int i = 0 ; i < nb + 1 ; i++)
	{
		stringstream name;
		name<<"rand"<<i;
		AddFeature(name.str(), new FeatureFloat(static_cast<float>(rand_r(&xr_seed)) / RAND_MAX));
	}
	// LOG_DEBUG(m_logger, "Generate random object with requirements:\""<<x_requirement<<"\" --> "<<this->SerializeToString());

	m_id = rand_r(&xr_seed);
}
