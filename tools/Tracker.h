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

#include "Object.h"

using namespace std;

class Feature;
class Template;
class Object;
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

		void MatchTemplates();
		void CleanTemplates();
		void DetectNewTemplates();
		void UpdateTemplates();
		void PrintObjects() const;
		int MatchObject(Object& x_obj);
		int MatchTemplate(Template& x_temp);
		
		std::list <Template> m_templates;
		std::vector <Object> m_objects;
		
	private:
		const TrackerParameter& m_param;
};

#endif
