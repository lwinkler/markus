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
#include <log4cxx/logger.h>
#include "define.h"
#include "FeatureStd.h"
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
	explicit Object(const std::string & x_name = "unnamed");
	Object(const std::string & x_name, const cv::Rect & x_rect);
	virtual ~Object();
	Object(const Object &);
	Object& operator=(const Object &);

	inline void SetName(const std::string& x_name) {m_name = x_name;}
	inline const std::string& GetName() const {return m_name;}
	inline int GetId() const {return m_id;}
	inline void SetId(int x_id) {m_id = x_id;}

	inline void AddFeature(std::string x_name, Feature* xp_feat)
	{
		auto it = m_feats.find(x_name);
		if(it != m_feats.end())
			m_feats.erase(it);

		m_feats.insert(std::make_pair(x_name, xp_feat));
	}
	inline void AddFeature(std::string x_name, float x_value)
	{
		auto it = m_feats.find(x_name);
		if(it != m_feats.end())
			m_feats.erase(it);

		m_feats.insert(std::make_pair(x_name, new FeatureFloat(x_value)));
	}
	inline const std::map <std::string, FeaturePtr>& GetFeatures() const {return m_feats;}
	inline bool HasFeature(const std::string& x_name) const
	{
		return m_feats.find(x_name) != m_feats.end();
	}
	inline Feature& RefFeature(const std::string& x_name)
	{
		auto it = m_feats.find(x_name);
		if(it == m_feats.end())
			throw FeatureNotFoundException("Feature " + x_name + " does not exist", LOC);
		return *it->second;
	}
	inline const Feature& GetFeature(const std::string& x_name) const
	{
		const auto it = m_feats.find(x_name);
		if(it == m_feats.end())
			throw FeatureNotFoundException("Feature " + x_name + " does not exist", LOC);
		return *it->second;
	}
	// inline void SetFeatureByName(const std::string& x_name, float x_value) {m_feats.find(x_name)->second = Feature();}
	void SetFeatures(const std::map<std::string, FeaturePtr>& x_feats) {m_feats = x_feats;}

	// Conversion functions for convenance
	inline cv::Rect GetRect()  const {return cv::Rect(posX - width / 2, posY - height / 2, width, height);}
	inline void SetRect(const cv::Rect& x_rect)
	{
		width  = x_rect.width;
		height = x_rect.height;
		posX   = x_rect.tl().x + width / 2;
		posY   = x_rect.tl().y + height / 2;
	}
	inline cv::Point2d TopLeft() const {return cv::Point2d(posX - width / 2.0, posY - height / 2.0);}
	inline cv::Point2d Center() const {return cv::Point2d(posX, posY);}

	void RenderTo(cv::Mat& x_output, const cv::Scalar& x_color) const;
	void Intersect(const cv::Mat& x_image);
	virtual void Randomize(unsigned int& xr_seed, const Json::Value& x_requirement, const cv::Size& xr_size);
	virtual void Serialize(std::ostream& stream, MkDirectory* xp_dir = nullptr) const;
	virtual void Deserialize(std::istream& stream, MkDirectory* xp_dir = nullptr);

protected:
	std::string m_name;
	int m_id;
	std::map <std::string, FeaturePtr> m_feats;
private:
	static log4cxx::LoggerPtr m_logger;

public:
	double posX;
	double posY;
	double width;
	double height;
};
#endif
