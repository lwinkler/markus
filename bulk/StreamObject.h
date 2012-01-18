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

#ifndef STREAM_RECT_H
#define STREAM_RECT_H

#include "cv.h"

#include "Stream.h"
#include "Object.h"

/// Stream in the form of located objects

class StreamObject : public Stream
{
public:
	StreamObject(int x_id, const std::string& x_name, int x_width, int x_height, 
		   std::vector<Object>& r_rects, const CvScalar& x_color, Module& rx_module, const std::string& rx_description);
	~StreamObject();
	void Clear() {m_objects.clear();};
	void AddObject(const Object x_obj) {m_objects.push_back(x_obj);};
	
	virtual void ConvertInput();
	virtual void RenderTo(cv::Mat * xp_output) const;
	inline virtual const std::string GetTypeString()const {return "Objects";};
	inline void AddPropertyName(const std::string & x_name){m_propertyNames.push_back(x_name);};

protected:
	std::vector<Object> & m_objects;
	std::vector<std::string> m_propertyNames;

private:
	StreamObject& operator=(const StreamObject&);
	StreamObject(const StreamObject&);
};

#endif