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

// #include <iostream>
#include "Module.h"


/// This is the parent class for all streams (input and output of data)

class Stream
{
public:
	Stream(int x_id, const std::string& x_name, Module& rx_module, const std::string& rx_description);
	virtual ~Stream();
	inline const std::string& GetName() const {return m_name;}
	inline int GetId() const {return m_id;}
	inline int GetInputWidth() const {return m_width;}
	inline int GetInputHeight() const {return m_height;}
	inline const std::string& GetDescription() const {return m_description;}
	virtual const std::string GetTypeString() const = 0;
	virtual void RenderTo(cv::Mat * x_output) const  = 0;
	virtual void Connect(Stream *x_stream);
	virtual void ConvertInput() = 0;
	void Export(std::ostream& rx_os, int x_indentation, bool x_isInput);
	inline void LockModuleForRead(){mr_module.LockForRead();}
	inline void UnLockModule(){mr_module.Unlock();}
	inline bool IsConnected() const {return m_connected != NULL;}
	inline Module& RefModule(){return mr_module;}
	inline Stream& RefConnected()
	{
		if(m_connected == NULL)
			throw MkException("Stream " + GetName() + " is not connected", LOC);
		return *m_connected;
	}

	inline TIME_STAMP GetTimeStamp() const             {return m_timeStamp;}
	inline void   SetTimeStamp(TIME_STAMP x_timeStamp) {m_timeStamp = x_timeStamp;}
	inline double GetTimeStampConnected() const
	{
		if(m_connected == NULL)
			throw MkException("Stream " + GetName() + " is not connected", LOC);
		return m_connected->GetTimeStamp();
	}
	inline bool IsReady(){return m_isReady;}
	inline void SetAsReady(){m_isReady = true;}

protected:
	const std::string m_name;
	const int m_id;
	const int m_width;
	const int m_height;
	Module& mr_module;
	const std::string m_description;
	TIME_STAMP m_timeStamp;
	
	Stream * m_connected;
	bool m_isReady;
	
private:
	// These are defined as a trick to prevent unwanted copies !
	Stream& operator=(const Stream&);
	Stream(const Stream&);
};

#endif
