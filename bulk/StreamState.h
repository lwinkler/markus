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

#ifndef STREAM_STATE_H
#define STREAM_STATE_H

#include "cv.h"

#include "Stream.h"

/// Stream in the form of located objects

class StreamState : public Stream
{
public:
	StreamState(int x_id, const std::string& rx_name, bool& x_state, const CvScalar& x_color, Module& rx_module, const std::string& rx_description);
	StreamState(int x_id, const std::string& rx_name, bool& x_state, Module& rx_module, const std::string& rx_description);
	~StreamState();
	// inline void SetState(bool x_state){m_state = x_state;};
	
	virtual void ConvertInput() {};
	virtual void RenderTo(cv::Mat * xp_output) const;
	inline virtual const std::string GetTypeString()const {return "State";};

protected:
	bool& m_state;
	CvScalar m_color;
	bool m_isColorSet;

private:
	StreamState& operator=(const StreamState&);
	StreamState(const StreamState&);
};

#endif
