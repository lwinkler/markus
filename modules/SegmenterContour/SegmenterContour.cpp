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

log4cxx::LoggerPtr SegmenterContour::m_logger(log4cxx::Logger::getLogger("SegmenterContour"));

SegmenterContour::SegmenterContour(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader),
	m_input(Size(m_param.width, m_param.height), m_param.type)
{
	// Initialize inputs and outputs streams
	AddInputStream(0, new StreamImage("input", m_input, *this,	"Input binary stream"));

	AddOutputStream(0, new StreamObject("segmented", m_regions, *this,	"Segmented objects"));

#ifdef MARKUS_DEBUG_STREAMS
	m_debug = Mat(Size(m_param.width, m_param.height), CV_8UC3);
	AddDebugStream(0, new StreamDebug("blobs", m_debug, *this,	"Blobs"));
#endif

	// Decide which features to compute
	m_computeFitEllipse = m_param.features.find("ellipse_");
	m_computeMinRect    = m_param.features.find("minrect_");
}

SegmenterContour::~SegmenterContour()
{
}

void SegmenterContour::Reset()
{
	Module::Reset();
	split(m_param.features, ',', m_featureNames);
	computeMoment = false;
	computeHuMoment = false;

	for(vector<string>::const_iterator it = m_featureNames.begin() ; it != m_featureNames.end() ; it++)
	{
		if (!computeMoment)
			computeMoment = (it->find("moment") != string::npos);

		if (!computeHuMoment && (computeHuMoment  = (it->find("hu_moment") != string::npos)))
		{
			break;
		}
	}
}

void SegmenterContour::ProcessFrame()
{
	// Mat threshold_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/// Detect edges using Threshold
	// threshold(m_input, threshold_output, thresh, 255, THRESH_BINARY);
	/// Find contours
	findContours(m_input, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));


	m_regions.clear();
#ifdef MARKUS_DEBUG_STREAMS
	m_debug.setTo(0);
#endif
	const double diagonal = sqrt(m_input.rows * m_input.rows + m_input.cols * m_input.cols);
	const double fullArea = m_input.rows * m_input.cols;


	/// Extract features
	for(unsigned int i = 0; i< contours.size(); i++)
	{
		Rect rect = boundingRect(contours[i]);
		if(rect.width >= m_param.minWidth && rect.height >= m_param.minHeight)
		{
			RotatedRect minEllipse;
			/// Find the rotated rectangles and ellipses for each contour
			if(contours[i].size() >= 5)
				minEllipse = fitEllipse(Mat(contours[i]));
			// RotatedRect minRect = minAreaRect(Mat(contours[i]));

			// contour
#ifdef MARKUS_DEBUG_STREAMS
			Scalar color = Scalar(m_rng.uniform(0, 255), m_rng.uniform(0,255), m_rng.uniform(0,255));
			drawContours(m_debug, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point());
#endif


			Object obj(m_param.objectLabel, rect);

			// Extract moments only if necessary
			Moments mom;
			double m00_scaled;
			double hu[7];

			if (computeMoment)
			{
				mom = moments(contours[i]);
				m00_scaled = mom.m00;

				if (computeHuMoment)	// if computeHuMoment is true, computeMoment is already true
					HuMoments(mom,hu);
			}


			// Add the possible features
			for(vector<string>::const_iterator it = m_featureNames.begin() ; it != m_featureNames.end() ; it++)
			{
				if(it->compare("x") == 0)
				{
					obj.AddFeature("x", obj.posX / diagonal);
				}
				else if(it->compare("y") == 0)
				{
					obj.AddFeature("y", obj.posY / diagonal);
				}
				else if(it->compare("width") == 0)
				{
					obj.AddFeature("width", obj.width / diagonal);
				}
				else if(it->compare("height") == 0)
				{
					obj.AddFeature("height", obj.height / diagonal);
				}
				else if(it->compare("area") == 0)
				{
					obj.AddFeature("area", contourArea(contours[i]) / fullArea);
				}
				else if(it->compare("ellipse_angle") == 0)
				{
					obj.AddFeature("ellipse_angle", minEllipse.angle / 180.0); // 180 is the max possible angle
				}
				else if(it->compare("ellipse_cos") == 0)
				{
					obj.AddFeature("ellipse_cos", cos(minEllipse.angle * M_PI / 180.0)); // [-1..1]
				}
				else if(it->compare("ellipse_sin") == 0)
				{
					obj.AddFeature("ellipse_sin", sin(minEllipse.angle * M_PI / 180.0)); // [0..1]
				}
				else if(it->compare("ellipse_width") == 0)
				{
					obj.AddFeature("ellipse_width", static_cast<double>(minEllipse.size.width) / diagonal);
				}
				else if(it->compare("ellipse_height") == 0)
				{
					obj.AddFeature("ellipse_height", static_cast<double>(minEllipse.size.height)  / diagonal);
				}
				else if(it->compare("ellipse_ratio") == 0)
				{
					obj.AddFeature("ellipse_ratio", minEllipse.size.height == 0 ? 0 : static_cast<double>(minEllipse.size.width) / minEllipse.size.height);
				}
				else if(it->compare("moment_00") == 0)
				{
					obj.AddFeature("moment_00", m00_scaled);
				}
				else if(it->compare("moment_11") == 0)
				{
					obj.AddFeature("moment_11", mom.mu11/pow(m00_scaled, 2));
				}
				else if(it->compare("moment_02") == 0)
				{
					obj.AddFeature("moment_02", mom.mu02/pow(m00_scaled, 2));
				}
				else if(it->compare("moment_20") == 0)
				{
					obj.AddFeature("moment_20", mom.mu20/pow(m00_scaled, 2));
				}
				else if(it->compare("hu_moment_1") == 0)
				{
					obj.AddFeature("hu_moment_1", hu[0]);
				}
				else if(it->compare("hu_moment_2") == 0)
				{
					obj.AddFeature("hu_moment_2", hu[1]);
				}
				else if(it->compare("hu_moment_3") == 0)
				{
					obj.AddFeature("hu_moment_3", hu[2]);
				}
				else if(it->compare("hu_moment_4") == 0)
				{
					obj.AddFeature("hu_moment_4", hu[3]);
				}
				else if(it->compare("hu_moment_5") == 0)
				{
					obj.AddFeature("hu_moment_5", hu[4]);
				}
				else if(it->compare("hu_moment_6") == 0)
				{
					obj.AddFeature("hu_moment_6", hu[5]);
				}
				else if(it->compare("hu_moment_7") == 0)
				{
					obj.AddFeature("hu_moment_7", hu[6]);
				}
			}

#ifdef MARKUS_DEBUG_STREAMS
			// ellipse
			if(m_computeFitEllipse && minEllipse.size.width != 0
					&& minEllipse.center.x > 0 && minEllipse.center.y > 0
					&& minEllipse.center.x < m_debug.cols && minEllipse.center.y < m_debug.rows) // note: extra conditions are present to avoid a segfault
				ellipse(m_debug, minEllipse, color, 2, 8);
#endif
			// rotated rectangle
			/*Point2f rect_points[4]; minRect[i].points(rect_points);
			for(int j = 0; j < 4; j++)
				line(*m_debug, rect_points[j], rect_points[(j+1)%4], color, 1, 8);
			*/
			m_regions.push_back(obj);
		}
	}
}



