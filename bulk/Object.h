
#ifndef MKOBJECT_H
#define MKOBJECT_H

#include <list>
#include <vector>

#include <cv.h>

#include "Feature.h"

/*! \class Object
 *  \brief Class representing a region (or blob)
 *
 *  These regions are the area that are obtained after background subtraction and segmentation
 */
class Object
{
	public:
		Object();
		~Object();
		
		inline void AddFeature(/*const char* name, */double value)
		{
			Feature f(value);
			m_feats.push_back(f);
		};
		inline const std::vector <Feature>& GetFeatures() const {return m_feats;};
		//inline int GetNum() const {return m_num;};
		//inline const cv::Rect& GetRect() const {return cv::Rect();};
		//inline void SetRect(const cv::Rect x_rect){m_rect = x_rect;};
		inline const CvScalar& GetColor() const {return m_color;};
		inline void SetColor(const CvScalar& x_color) {m_color = x_color;};
		
		int m_isMatched;
		double m_posX;
		double m_posY;
		double m_width;
		double m_height;
	private:
		//int m_num;
		std::vector <Feature> m_feats;
		CvScalar m_color;
		//Object(const Object&);
		//Object& operator = (const Object&);

};
#endif