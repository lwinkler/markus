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

#ifndef TRACKER_H
#define TRACKER_H


#include <cv.h>
#include <vector>
#include <list>
#include <cstring>

using namespace std;

class Feature;
class Template;
class TrackedRegion;
class Tracker;

class TrackerParameter
{
public:
	TrackerParameter()
	{
		maxMatchingDistance = 100;
		maxNbFramesDisappearance = 10;
	};
	double maxMatchingDistance;
	int maxNbFramesDisappearance;
};

/*! \class Feature
 *  \brief Class representing a feature of a template/region
 *
 * e.g. area, perimeter, length, ...
 */
class Feature
{
	public:
		Feature(double value=0);
		Feature(const Feature&);
		Feature& operator = (const Feature&);
		~Feature();
		
		//inline const char* GetName() const {return m_name;};
		inline double GetValue() const {return m_value;};
		inline void SetValue( double x) {m_value = x;};
		inline double GetVariance() const {return m_variance;};
		inline void SetVariance( double x) {m_variance = x;};
		
		static double GetFeatureValue(const std::vector<Feature>& vect, const char* name);
		
		
	public:
		static std::vector<std::string> m_names; // TODO : Names should be in a static array to save memory space
		double m_value;
		double m_variance;
};

/*! \class Region
 *  \brief Class representing a region (or blob)
 *
 *  These regions are the area that are obtained after background subtraction and segmentation
 */
class TrackedRegion
{
	public:
		TrackedRegion(int x_num);
		TrackedRegion(const TrackedRegion&);
		TrackedRegion& operator = (const TrackedRegion&);
		~TrackedRegion();
		int Match(const std::list<Template>&, double x_maxMaxingDistance);
		
		inline void AddFeature(/*const char* name, */double value)
		{
			Feature f(value);
			m_feats.push_back(f);
		};
		inline const std::vector <Feature>& GetFeatures() const {return m_feats;};
		inline int GetNum() const {return m_num;};
		
		int m_isMatched;
		double m_posX;
		double m_posY;
	private:
		int m_num;
		std::vector <Feature> m_feats;
};

/*! \class Template
 *  \brief Class representing an object template
 *
 *  A template is what allows to track an object, e.g. a red car, through different frames.
 */
class Template
{
	public:
		Template(int x_maxNbFramesDisappearance);
		Template(const TrackedRegion&, int x_maxNbFramesDisappearance);
		Template(const Template&, int x_maxNbFramesDisappearance);
		Template& operator = (const Template&);
		~Template();
		
		double CompareWithTrackedRegion(const TrackedRegion& x_reg) const;
		int Match(const std::list<Template>& x_temp, std::vector<TrackedRegion>& x_reg, /*cv::Mat* x_blobsImg,*/ double x_maxMatchingDistance);
		void UpdateFeatures();
		
		
		inline void AddFeature(const char* name, double value)
		{
			Feature f(/*name,*/value);
			m_feats.push_back(f);
		};
		inline const std::vector <Feature>& GetFeatures() const{ return m_feats;};
		inline const std::list <TrackedRegion>& GetMatchingTrackedRegions() const{ return m_matchingTrackedRegions;};
		inline int GetNum() const {return m_num;};
		
		//int m_isMatched;
		int m_bestMatchingTrackedRegion;
		int m_counterClean;
		std::list <TrackedRegion> m_matchingTrackedRegions;
		double m_posX;
		double m_posY;

	private:
		int m_num;
		static int m_counter;
		std::vector <Feature> m_feats;
		
};

/*! \class Tracker
 *  \brief Class containing methodes/attributes for tracking
 *
 */

class Tracker
{
	public:
		Tracker(const TrackerParameter& x_param, int width, int height, int type);
		~Tracker(void);
		void Reset();
				
		//void ExtractBlobs(cv::Mat* x_img);
		void MatchTemplates();
		void CleanTemplates();
		void DetectNewTemplates();
		void UpdateTemplates();
		void PrintTrackedRegions() const;
		
		//inline cv::Mat* GetBlobsImg(){ return m_blobsImg;};
		
		static int m_colorArraySize;
		static CvScalar m_colorArray[];
		std::list <Template> m_templates;
		std::vector <TrackedRegion> m_regions;
		
	private:
		// Background subtraction
//		cv::Mat* m_blobsImg;
		
		const TrackerParameter& m_param;
};

#endif
