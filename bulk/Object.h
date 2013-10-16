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

#ifndef MKOBJECT_H
#define MKOBJECT_H

#include <list>
#include <vector>

#include "util.h"
#include "Feature.h"

/*! \class Object
 *  \brief Class representing a region (or blob)
 *
 *  These regions are the area that are obtained after background subtraction and segmentation
 */
class Object
{
	public:
		Object(const std::string & x_name);
		Object(const std::string & x_name, const cv::Rect & x_rect);
		~Object();

		inline const std::string& GetName() const {return m_name;}
		inline int GetId() const {return m_id;}
		inline void SetId(int x_id){m_id = x_id;}

		inline void AddFeature(std::string x_name, double x_value) {m_feats.insert(std::make_pair(x_name, Feature(x_value)));}
		inline const std::map <std::string, Feature>& GetFeatures() const {return m_feats;}
		inline const Feature& GetFeature(const std::string& x_name) const {
			return m_feats.find(x_name)->second;} /// TODO: manage case where the feature is not found
		// inline void SetFeatureByName(const std::string& x_name, double x_value) {m_feats.find(x_name)->second = Feature();}
		void SetFeatures(const std::map<std::string, Feature>& x_feats){m_feats = x_feats;}
		inline cv::Rect Rect() const {return cv::Rect(m_posX - m_width / 2, m_posY - m_height / 2, m_width, m_height);}

		int m_isMatched;

	private:
		std::string m_name;
		int m_id;
		std::map <std::string, Feature> m_feats;
		//CvScalar m_color;
		//Object(const Object&);
		//Object& operator = (const Object&);
	public:
		double m_posX;
		double m_posY;
		double m_width;
		double m_height;
};
#endif
