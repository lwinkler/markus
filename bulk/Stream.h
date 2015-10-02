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

#ifndef STREAM_H
#define STREAM_H

#include <log4cxx/logger.h>
#include <atomic>
#include "Module.h"


/// This is the parent class for all streams (input and output of data)

class Stream : public Serializable
{
public:
	Stream(const std::string& x_name, Module& rx_module, const std::string& rx_description, const std::string& rx_requirement = "");
	virtual ~Stream();

	virtual const std::string& GetClass() const = 0;
	virtual const std::string& GetType() const = 0;
	inline const std::string& GetName() const {return m_name;}
	// inline int GetId() const {return m_id;}
	// inline void SetId(int x_id) {m_id = x_id;} // id should disappear at term
	inline int GetWidth() const {return m_width;}
	inline int GetHeight() const {return m_height;}
	inline cv::Size GetSize() const {return cv::Size(m_width, m_height);}
	inline const std::string& GetDescription() const {return m_description;}
	virtual void RenderTo(cv::Mat& x_output) const = 0;
	virtual void Query(int x_posX, int x_posY) const = 0;
	virtual void Connect(Stream *x_stream, bool x_bothWays = true);
	virtual void ConvertInput() = 0;
	virtual void Randomize(unsigned int& xr_seed) = 0;
	virtual void Serialize(MkJson& stream, const std::string& x_dir) const;
	virtual void Deserialize(MkJson& stream, const std::string& x_dir);
	void Export(std::ostream& rx_os, int x_id, int x_indentation, bool x_isInput);
	inline void LockModuleForRead() {mr_module.LockForRead();}
	inline void UnLockModule() {mr_module.Unlock();}
	inline bool IsConnected() const {return m_connected != nullptr;}
	inline const Module& GetModule() const {return mr_module;}
	inline Stream& GetConnected() const
	{
		if(m_connected == nullptr)
			throw MkException("Stream " + GetName() + " is not connected", LOC);
		return *m_connected;
	}

	inline TIME_STAMP GetTimeStamp() const             {return m_timeStamp;}
	inline void   SetTimeStamp(TIME_STAMP x_timeStamp) {m_timeStamp = x_timeStamp;}
	inline TIME_STAMP GetTimeStampConnected() const
	{
		if(m_connected == nullptr)
			throw MkException("Stream " + GetName() + " is not connected", LOC);
		return m_connected->GetTimeStamp();
	}
	inline bool IsReady() const {return m_isReady;}
	inline void SetAsReady() {m_isReady = true;}
	inline const std::string& GetRequirement() {return m_requirement;}
	inline void SetRequirement(const std::string& x_requirement) {m_requirement = x_requirement;}

protected:
	std::string m_name;
	// const int m_id;
	int m_width;
	int m_height;
	Module& mr_module;
	std::string m_description;
	std::atomic<TIME_STAMP> m_timeStamp;

	Stream * m_connected;
	bool m_isReady;
	std::string m_requirement;

private:
	DISABLE_COPY(Stream)
	static log4cxx::LoggerPtr m_logger;
};

#endif
