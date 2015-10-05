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

#ifndef STREAM_OBJECT_H
#define STREAM_OBJECT_H

#include "Stream.h"
#include "Object.h"

#define DEFAULT_STREAM_COLOR cv::Scalar(255, 255, 255)


/// Stream in the form of located objects

class StreamObject : public Stream
{
public:
	StreamObject(const std::string& rx_name, std::vector<Object>& r_rects, Module& rx_module, const std::string& rx_description, const std::string& rx_requirement = "");
	~StreamObject();
	MKCLASS("StreamObjects")
	MKTYPE("Objects")

	inline void Clear() {m_objects.clear();}
	inline void AddObject(const Object& x_obj) {m_objects.push_back(x_obj);}
	inline const std::vector<Object>& GetObjects() const {return m_objects;}

	virtual void ConvertInput();
	virtual void RenderTo(cv::Mat& x_output) const;
	virtual void Query(int x_posX, int x_posY) const;
	virtual void Randomize(unsigned int& xr_seed);
	virtual void Serialize(MkJson stream, const std::string& x_dir) const;
	virtual void Deserialize(MkJson stream, const std::string& x_dir);
	// double GetFeatureValue(const std::vector<Feature>& x_vect, const char* x_name);

protected:
	std::vector<Object> & m_objects;

private:
	DISABLE_COPY(StreamObject)
	static log4cxx::LoggerPtr m_logger;
};

#endif
