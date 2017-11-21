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

#include <log4cxx/logger.h>
#include "Object.h"
#include "FeatureFloatInTime.h"


/*! \class Template
 *  \brief Class representing an object template
 *
 *  A template is what allows to track an Object, through different frames. A set of Templates is typically
 * used inside a Tracker.
 */
class Template final
{
public:
	Template();
	Template(const Object& x_obj, TIME_STAMP x_currentTimeStamp);
	Template(const Template&);
	Template& operator = (const Template&);
	~Template();

	double CompareWithObject(const Object& x_reg, const std::vector<std::string>& x_features) const;
	void UpdateFeatures(double x_alpha, TIME_STAMP m_currentTimeStamp);
	bool NeedCleaning(TIME_STAMP x_cleaningTimeStamp);

	inline void AddFeature(std::string x_name, double x_value) {m_feats.insert(std::make_pair(x_name, FeatureFloatInTime(FeatureFloat(x_value))));}
	inline const FeatureFloatInTime& GetFeature(const std::string& x_name) const
	{
		auto it = m_feats.find(x_name);
		if(it == m_feats.end())
			throw MkException("Feature is non-existant", LOC);
		return it->second;
	}
	inline void SetFeatures(const std::map <std::string, FeatureFloatInTime>& x_feats) {m_feats = x_feats;}
	inline const std::map <std::string, FeatureFloatInTime>& GetFeatures() const { return m_feats;}
	// inline const std::list <Object>& GetMatchingObjects() const{ return m_matchingObjects;}
	inline int GetNum() const {return m_num;}

	Object * m_lastMatchingObject;
	TIME_STAMP m_lastSeen;

private:
	static log4cxx::LoggerPtr m_logger;
	int m_num;
	static int m_counter; // Counter to attribute ids
	std::map <std::string, FeatureFloatInTime> m_feats;
};
