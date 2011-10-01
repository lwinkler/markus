// +--------------------------------------------------------------------------+
// | File      : Tracker.h                                                    |
// | Utility   : Experimental object tracker                                  |
// | Author    : Laurent Winkler                                              |
// | Date      : July 2010                                                    |
// | Remarque  : none                                                         |
// +--------------------------------------------------------------------------+
#ifndef TRACKER_H
#define TRACKER_H


#include <cv.h>
#include <vector>
#include <list>
#include <cstring>

#include "cvblobs/BlobResult.h"
#include "cvblobs/BlobExtraction.h"
#include "cvblobs/Blob.h"
#include "cvblobs/BlobLibraryConfiguration.h"
#include "cvblobs/BlobResult.h"

using namespace std;

class Feature;
class Template;
class Region;
class Tracker;

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
		inline double SetValue( double x) {m_value = x;};
		inline double GetVariance() const {return m_variance;};
		inline double SetVariance( double x) {m_variance = x;};
		
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
class Region
{
	public:
		Region(int x_num);
		Region(const Region&);
		Region& operator = (const Region&);
		~Region();
		int Match(const std::list<Template>&);
		
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
		//static int m_counter;
		
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
		Template();
		Template(const Region&);
		Template(const Template&);
		Template& operator = (const Template&);
		~Template();
		
		double CompareWithRegion(const Region& x_reg) const;
		int Match(const std::list<Template>& x_temp, std::vector<Region>& x_reg, IplImage* x_blobsImg);
		void UpdateFeatures();
		
		
		inline void AddFeature(const char* name, double value)
		{
			Feature f(/*name,*/value);
			m_feats.push_back(f);
		};
		inline const std::vector <Feature>& GetFeatures() const{ return m_feats;};
		inline const std::list <Region>& GetMatchingRegions() const{ return m_matchingRegions;};
		inline int GetNum() const {return m_num;};
		
		//int m_isMatched;
		int m_bestMatchingRegion;
		int m_counterClean;
		std::list <Region> m_matchingRegions;
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
		Tracker(int width, int height, int depth, int channels);
		~Tracker(void);
		
		void CreateParamWindow();
		
		
		void ExtractBlobs(IplImage* x_img);
		void MatchTemplates();
		void CleanTemplates();
		void DetectNewTemplates();
		void UpdateTemplates();
		void PrintRegions() const;
		
		inline IplImage* GetBlobsImg(){ return m_blobsImg;};
		
		
		static CvScalar colorArray[];
		static int colorArraySize;
		static int m_maxMatchingDistance;
		static int m_maxNbFramesDisappearance;
		
		static int GetMaxNbFramesDisappearance(){return m_maxNbFramesDisappearance;};
		static void SetMaxNbFramesDisappearance(int a){ m_maxNbFramesDisappearance = a;};

		static int GetMaxMatchingDistance(){return m_maxMatchingDistance;};
		static void SetMaxMatchingDistance(int a){ m_maxMatchingDistance = a;};
		
	private:
		double GetSTLResult( CBlob* blob, funcio_calculBlob *evaluador ) const;
		// Background subtraction
		IplImage* m_blobsImg;
		std::list <Template> m_templates;
		std::vector <Region> m_regions;		
		

};

#endif
