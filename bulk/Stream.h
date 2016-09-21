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
	Stream(const std::string& x_name, Module& rx_module, const std::string& rx_description, const std::string& rx_requirement = "");
	virtual ~Stream();

	inline void Reset() {m_timeStamp = TIME_STAMP_MIN;SetValueToDefault();}

	virtual const std::string& GetClass() const = 0;
	virtual const std::string& GetType() const override = 0;
	virtual const ParameterType& GetParameterType() const override = 0;
	
	inline const cv::Size GetSize() const {return mr_module.GetSize();}
	virtual void RenderTo(cv::Mat& x_output) const = 0;
	virtual void Query(int x_posX, int x_posY) const = 0;
	virtual void Connect(Stream *x_stream);
	virtual void ConvertInput() = 0;
	virtual void Randomize(unsigned int& xr_seed) = 0;
	virtual void Serialize(std::ostream& stream, MkDirectory* xp_dir = nullptr) const override;
	virtual void Deserialize(std::istream& stream, MkDirectory* xp_dir = nullptr) override;
	virtual void Export(std::ostream& rx_os, int x_id, int x_indentation, bool x_isInput) const;
	inline bool IsConnected() const {return m_isConnected;}
	inline void SetAsConnected() {m_isConnected = true;}
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
	inline const std::string& GetRequirement() {return m_requirement;}
	inline void SetRequirement(const std::string& x_requirement) {m_requirement = x_requirement;}
	inline bool IsBlocking() const {return m_blocking;}
	inline bool IsSynchronized() const {return m_synchronized;}
	inline void SetBlocking(bool x_block) {m_blocking = x_block;}
	inline void SetSynchronized(bool x_sync) {m_synchronized = x_sync;}

	// Methods inherited from Parameter class
	virtual void SetValue(const std::string& x_value, ParameterConfigType x_confType) override = 0;
	virtual void SetDefault(const std::string& x_value) override = 0;
	virtual void SetValueToDefault() override = 0;
	virtual bool CheckRange() const override {return true;}
	virtual void GenerateValues(int x_nbSamples, std::vector<std::string>& rx_values, const std::string& x_range = "") const override {rx_values.clear();}
	virtual std::string GetValueString() const override = 0;
	virtual std::string GetDefaultString() const override = 0;
	inline std::string GetRange() const override {return "";}
	inline virtual void SetRange(const std::string& x_range) override {}

protected:
	Module& mr_module;
	std::atomic<TIME_STAMP> m_timeStamp;

	Stream * m_connected = nullptr;
	bool m_isConnected   = false;
	bool m_blocking      = true;
	bool m_synchronized  = true;
	std::string m_requirement;

private:
	static log4cxx::LoggerPtr m_logger;
};

#endif
