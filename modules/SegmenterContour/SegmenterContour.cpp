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

#include "SegmenterContour.h"
#include "StreamDebug.h"
#include "StreamObject.h"
#include "StreamImage.h"

// for debug
#include "util.h"

using namespace std;
using namespace cv;

const char * SegmenterContour::m_type = "SegmenterContour";


SegmenterContour::SegmenterContour(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader),
	m_rng(12345)
{
	m_description = "Segments a binary image and outputs a stream of objects (with OpenCV contour) and extracts their features (position, width and height)";
	m_input = new Mat(cvSize(m_param.width, m_param.height), m_param.type);
	m_debug = new Mat(cvSize(m_param.width, m_param.height), CV_8UC3);

	// Initialize inputs and outputs streams
	m_inputStreams.push_back(new StreamImage(0, "input", m_input, *this,	"Input binary stream"));

	m_outputStreams.push_back(new StreamObject(0, "segmented", m_param.width, m_param.height, m_regions, cvScalar(255, 255, 255), *this,	"Segmented objects"));

	m_debugStreams.push_back(new StreamDebug(0, "blobs", m_debug, *this,	"Blobs"));


	// Add features to extract for each object
	m_outputObjectStream = dynamic_cast<StreamObject*>(m_outputStreams[0]);


	// tmp->AddFeatureName("area");
	// tmp->AddFeatureName("perimeter");

	vector<string> elems;
	split(m_param.features, ',', elems);
	for(vector<std::string>::const_iterator it = elems.begin() ; it != elems.end() ; it++)
	{
		cout<<"add feature "<<*it<<endl;
		m_outputObjectStream->AddFeatureName(*it);
	}
	
}

SegmenterContour::~SegmenterContour()
{
	delete(m_input);
	delete(m_debug);
}

void SegmenterContour::Reset()
{
	Module::Reset();
}

void SegmenterContour::ProcessFrame()
{
	// Mat threshold_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/// Detect edges using Threshold
	// threshold(m_input, threshold_output, thresh, 255, THRESH_BINARY);
	/// Find contours
	findContours(*m_input, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// Find the rotated rectangles and ellipses for each contour
	vector<RotatedRect> minRect(contours.size());
	vector<RotatedRect> minEllipse(contours.size());

	for(unsigned int i = 0; i < contours.size(); i++)
	{
		minRect[i] = minAreaRect(Mat(contours[i]));
		if(contours[i].size() > 5)
		{
			minEllipse[i] = fitEllipse(Mat(contours[i]));
		}
	}

	m_regions.clear();
	m_debug->setTo(0); // TODO debug only
	/// Draw contours + rotated rects + ellipses
	for(unsigned int i = 0; i< contours.size(); i++)
	{
		Rect rect = boundingRect(contours[i]);
		if(rect.width >= m_param.minWidth && rect.height >= m_param.minHeight)
		{
			// contour
			Scalar color = Scalar(m_rng.uniform(0, 255), m_rng.uniform(0,255), m_rng.uniform(0,255));
			drawContours(*m_debug, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point()); // TODO debug only


			Object obj(m_param.objectLabel);
			obj.m_posX 	= rect.x + rect.width / 2;
			obj.m_posY 	= rect.y + rect.height / 2;
			obj.m_width	= rect.width;
			obj.m_height 	= rect.height;

			// Add the possible features
			for(vector<string>::const_iterator it = m_outputObjectStream->GetFeatureNames().begin() ; it != m_outputObjectStream->GetFeatureNames().end() ; it++)
			{
				if(it->compare("x") == 0)
					obj.AddFeature(rect.x, 0.1);
				if(it->compare("y") == 0)
					obj.AddFeature(rect.y, 0.1);
				if(it->compare("width") == 0)
					obj.AddFeature(rect.width, 0.1);
				if(it->compare("height") == 0)
					obj.AddFeature(rect.height, 0.1);
				m_regions.push_back(obj);
			}

			// ellipse
			ellipse(*m_debug, minEllipse[i], color, 2, 8);
			// rotated rectangle
			Point2f rect_points[4]; minRect[i].points(rect_points);
			for(int j = 0; j < 4; j++)
				line(*m_debug, rect_points[j], rect_points[(j+1)%4], color, 1, 8);
		}
	}
}



