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

class Stream : public Serializable, public Parameter, boost::noncopyable
{
public:
	Stream(const std::string& x_name, Module& rx_module, const std::string& rx_description, const Json::Value& rx_requirement = Json::nullValue);
	virtual ~Stream();

	inline void Reset() {m_timeStamp = TIME_STAMP_MIN;SetValueToDefault();}

	virtual const std::string& GetClass() const = 0;
	const std::string& GetType() const override = 0;
	const ParameterType& GetParameterType() const override = 0;
	
	inline const cv::Size GetSize() const {return mr_module.GetSize();}
	virtual void RenderTo(cv::Mat& x_output) const = 0;
	virtual void Query(std::ostream& xr_out, const cv::Point& x_pt) const = 0;
	virtual void Connect(Stream& xr_stream);
	virtual void Disconnect();
	virtual void ConvertInput() = 0;
	virtual void Randomize(unsigned int& xr_seed) = 0;
	void Serialize(std::ostream& stream, MkDirectory* xp_dir = nullptr) const override;
	void Deserialize(std::istream& stream, MkDirectory* xp_dir = nullptr) override;
	virtual void Export(std::ostream& rx_os) const;
	inline bool IsConnected() const {return m_cptConnected > 0;}
	inline void SetAsConnected(bool x_val)
	{
		if(x_val)
			m_cptConnected++;
		else
		{
			m_cptConnected--;
			if(m_cptConnected <= 0)
			{
				m_connected = nullptr;
				m_cptConnected = 0;
			}
		}
	}
	inline const Module& GetModule() const {return mr_module;}
	inline Stream& GetConnected() const
	{
		if(m_connected == nullptr)
			throw MkException("Stream " + GetName() + " is not connected or is an output", LOC);
		return *m_connected;
	}

	inline TIME_STAMP GetTimeStamp() const             {return m_timeStamp;}
	inline void   SetTimeStamp(TIME_STAMP x_timeStamp) {m_timeStamp = x_timeStamp;}
	inline TIME_STAMP GetTimeStampConnected() const
	{
		if(m_connected == nullptr)
			throw MkException("Stream " + GetName() + " is not connected or is an output", LOC);
		return m_connected->GetTimeStamp();
	}
	inline bool IsBlocking() const {return m_blocking;}
	inline bool IsSynchronized() const {return m_synchronized;}
	inline void SetBlocking(bool x_block) {m_blocking = x_block;}
	inline void SetSynchronized(bool x_sync) {m_synchronized = x_sync;}

	// Methods inherited from Parameter class
	void SetValue(const ConfigReader& x_value, ParameterConfigType x_confType) override = 0;
	void SetDefault(const ConfigReader& x_value) override = 0;
	void SetValueToDefault() override = 0;
	bool CheckRange() const override {return true;}
	Json::Value GenerateValues(int x_nbSamples, const Json::Value& x_range) const override {return Json::nullValue;}
	ConfigReader GetValue() const override = 0;
	ConfigReader GetDefault() const override = 0;

	int id     = -1;
	bool debug = false;

protected:
	Module& mr_module;
	std::atomic<TIME_STAMP> m_timeStamp;

	Stream * m_connected = nullptr;
	int m_cptConnected   = 0;
	bool m_blocking      = true;
	bool m_synchronized  = true;

private:
	static log4cxx::LoggerPtr m_logger;
};

#endif
