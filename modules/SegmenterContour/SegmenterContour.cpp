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

SegmenterContour::SegmenterContour(ParameterStructure& xr_params) :
	Module(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params)),
	m_input(Size(m_param.width, m_param.height), m_param.type)
{
	// Initialize inputs and outputs streams
	AddInputStream(0, new StreamImage("image", m_input, *this,	"Input binary stream"));

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
	m_computeMoment = false;
	m_computeHuMoment = false;

	for(const auto& name : m_featureNames)
	{
		if (!m_computeMoment)
			m_computeMoment = (name.find("moment") != string::npos);

		if (!m_computeHuMoment && (m_computeHuMoment  = (name.find("hu_moment") != string::npos)))
		{
			break;
		}
	}
}

/// Process the frame
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

			if (m_computeMoment)
			{
				mom = moments(contours[i]);
				m00_scaled = mom.m00;

				if (m_computeHuMoment)	// if computeHuMoment is true, computeMoment is already true
					HuMoments(mom,hu);
			}


			// Add the possible features
			for(const auto& name : m_featureNames)
			{
				if(name == "x")
				{
					obj.AddFeature("x", obj.posX / diagonal);
				}
				else if(name == "y")
				{
					obj.AddFeature("y", obj.posY / diagonal);
				}
				else if(name == "width")
				{
					obj.AddFeature("width", obj.width / diagonal);
				}
				else if(name == "height")
				{
					obj.AddFeature("height", obj.height / diagonal);
				}
				else if(name == "area")
				{
					obj.AddFeature("area", contourArea(contours[i]) / fullArea);
				}
				else if(name == "ellipse_angle")
				{
					obj.AddFeature("ellipse_angle", minEllipse.angle / 180.0); // 180 is the max possible angle
				}
				else if(name == "ellipse_cos")
				{
					obj.AddFeature("ellipse_cos", cos(minEllipse.angle * M_PI / 180.0)); // [-1..1]
				}
				else if(name == "ellipse_sin")
				{
					obj.AddFeature("ellipse_sin", sin(minEllipse.angle * M_PI / 180.0)); // [0..1]
				}
				else if(name == "ellipse_width")
				{
					obj.AddFeature("ellipse_width", static_cast<double>(minEllipse.size.width) / diagonal);
				}
				else if(name == "ellipse_height")
				{
					obj.AddFeature("ellipse_height", static_cast<double>(minEllipse.size.height)  / diagonal);
				}
				else if(name == "ellipse_ratio")
				{
					obj.AddFeature("ellipse_ratio", minEllipse.size.height == 0 ? 0 : static_cast<double>(minEllipse.size.width) / minEllipse.size.height);
				}
				else if(name == "moment_00")
				{
					obj.AddFeature("moment_00", m00_scaled);
				}
				else if(name == "moment_11")
				{
					obj.AddFeature("moment_11", mom.mu11/pow(m00_scaled, 2));
				}
				else if(name == "moment_02")
				{
					obj.AddFeature("moment_02", mom.mu02/pow(m00_scaled, 2));
				}
				else if(name == "moment_20")
				{
					obj.AddFeature("moment_20", mom.mu20/pow(m00_scaled, 2));
				}
				else if(name == "hu_moment_1")
				{
					obj.AddFeature("hu_moment_1", hu[0]);
				}
				else if(name == "hu_moment_2")
				{
					obj.AddFeature("hu_moment_2", hu[1]);
				}
				else if(name == "hu_moment_3")
				{
					obj.AddFeature("hu_moment_3", hu[2]);
				}
				else if(name == "hu_moment_4")
				{
					obj.AddFeature("hu_moment_4", hu[3]);
				}
				else if(name == "hu_moment_5")
				{
					obj.AddFeature("hu_moment_5", hu[4]);
				}
				else if(name == "hu_moment_6")
				{
					obj.AddFeature("hu_moment_6", hu[5]);
				}
				else if(name == "hu_moment_7")
				{
					obj.AddFeature("hu_moment_7", hu[6]);
				}
				else if(name == "solidity")
				{
					vector<Point> ptConvexHull;
					convexHull(contours[i],ptConvexHull);
					obj.AddFeature("solidity", contourArea(contours[i]) / contourArea(ptConvexHull));
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



