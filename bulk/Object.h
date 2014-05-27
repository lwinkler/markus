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

#include <opencv2/opencv.hpp>
#include "define.h"
#include "Feature.h"
#include "MkException.h"

/*! \class Object
 *  \brief Class representing an extracted object (or blob)
 *
 *  Examples of objects 
 *  - blob obtained after background subtraction and segmentation
 *  - face detected with a sliding window algorithm
 */
class Object : public Serializable
{
	public:
		Object(const std::string & x_name);
		Object(const std::string & x_name, const cv::Rect & x_rect);
		~Object();
		Object(const Object &);
		Object& operator=(const Object &);

		inline const std::string& GetName() const {return m_name;}
		inline int GetId() const {return m_id;}
		inline void SetId(int x_id){m_id = x_id;}

		inline void AddFeature(std::string x_name, const Feature& x_feat) {m_feats.insert(std::make_pair(x_name, new Feature(x_feat)));}
		inline void AddFeature(std::string x_name, float x_value) {m_feats.insert(std::make_pair(x_name, new Feature(x_value)));}
		inline const std::map <std::string, Feature*>& GetFeatures() const {return m_feats;}
		inline const Feature& GetFeature(const std::string& x_name) const
		{
			std::map <std::string, Feature*>::const_iterator it = m_feats.find(x_name);
			if(it == m_feats.end())
				throw FeatureNotFoundException("Feature " + x_name + " does not exist", LOC);
			return *it->second;
		}
		// inline void SetFeatureByName(const std::string& x_name, float x_value) {m_feats.find(x_name)->second = Feature();}
		void SetFeatures(const std::map<std::string, Feature*>& x_feats){m_feats = x_feats;}
		inline cv::Rect Rect() const {return cv::Rect(posX - width / 2, posY - height / 2, width, height);}
		void RenderTo(cv::Mat& x_output, const cv::Scalar& x_color) const;
		virtual void Serialize(std::ostream& stream, const std::string& x_dir) const;
		virtual void Deserialize(std::istream& stream, const std::string& x_dir);

	private:
		std::string m_name;
		int m_id;
		std::map <std::string, Feature*> m_feats;

	public:
		double posX;
		double posY;
		double width;
		double height;
};
#endif
