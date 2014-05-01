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

#ifndef STREAM_DEBUG_H
#define STREAM_DEBUG_H

#ifdef MARKUS_DEBUG_STREAMS // If the flag is not set, do not use the streams at all

#include "Stream.h"

/// This class represents a debug stream, a stream that is only used to help the user 
/// it can typically be used to show what is happening inside a detection module

class StreamDebug : public Stream
{
public:
	StreamDebug(const std::string& x_name, cv::Mat& x_image, Module& rx_module, const std::string& rx_description);
	~StreamDebug();
	const cv::Mat& GetImage() const {return m_image;}
	
	virtual void ConvertInput();
	virtual void RenderTo(cv::Mat& x_output) const;
	virtual void Serialize(std::ostream& stream, const std::string& x_dir) const;
	virtual void Deserialize(std::istream& stream, const std::string& x_dir);
	inline virtual const std::string GetTypeString()const {return "Debug";}

protected:
	const cv::Mat& m_image;
private:

	StreamDebug& operator=(const StreamDebug&);
	StreamDebug(const StreamDebug&);
};

#endif
#endif
