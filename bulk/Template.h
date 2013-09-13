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

#include "Object.h"

/*! \class Template
 *  \brief Class representing an object template
 *
 *  A template is what allows to track an Object, through different frames. A set of Templates is typically
 * used inside a Tracker.
 */
class Template
{
	public:
		Template(int x_maxNbFramesDisappearance);
		Template(const Object&, int x_maxNbFramesDisappearance);
		Template(const Template&, int x_maxNbFramesDisappearance);
		Template& operator = (const Template&);
		~Template();
		
		double CompareWithObject(const Object& x_reg, const std::vector<std::string>& x_features) const;
		void UpdateFeatures(double x_alpha);
		
		inline void AddFeature(std::string x_name, double x_value) {m_feats.insert(std::make_pair(x_name, Feature(x_value)));}
		inline const Feature& GetFeature(const std::string& x_name) const {return m_feats.find(x_name)->second;}
		inline const std::map <std::string, Feature>& GetFeatures() const{ return m_feats;}
		// inline const std::list <Object>& GetMatchingObjects() const{ return m_matchingObjects;}
		inline int GetNum() const {return m_num;}
		
		//int m_isMatched;
		int m_bestMatchingObject;
		int m_counterClean;
		// std::list <Object> m_matchingObjects;
		Object * m_lastMatchingObject;
		double m_posX;
		double m_posY;

	private:
		int m_num;
		static int m_counter;
		std::map <std::string, Feature> m_feats;
};
