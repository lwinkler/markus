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

#ifndef OUTPUT_STREAM_H
#define OUTPUT_STREAM_H

#include <cv.h>
#include <iostream>

#include "Module.h"

enum StreamType
{
	STREAM_DEBUG,
	STREAM_IMAGE,
	STREAM_RECTS,
	STREAM_POINTS
};

class Stream
{
public:
	//Stream(const std::string& x_name, StreamType x_type, Module& rx_module);
	Stream(int x_id, const std::string& x_name, StreamType x_type, int x_width, int x_height, Module& rx_module);
	~Stream();
	inline const std::string& GetName() const {return m_name;};
	inline int GetId() const {return m_id;};
	inline int GetInputWidth() const {return m_width;};
	inline int GetInputHeight() const {return m_height;};
	inline StreamType GetType() const {return m_type;};
	virtual void Render(cv::Mat * x_output) const  = 0;
	void Connect(const Stream * x_stream);
	virtual void ConvertInput() = 0;
	void Export(std::ostream& rx_os, int x_tabs, bool x_isInput);
	inline void LockForRead(){mr_module.m_lock.lockForRead();};
	inline void UnLock(){mr_module.m_lock.unlock();};
protected:
	const std::string m_name;
	const int m_id;
	const StreamType m_type;
	const int m_width;
	const int m_height;
	
	const Stream * m_connected;
	
	cv::Mat * m_img_tmp1; // To convert the input
	cv::Mat * m_img_tmp2;
private:
	Stream& operator=(const Stream&);
	Stream(const Stream&);
	Module& mr_module;
};

#endif