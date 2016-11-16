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

#ifndef MULTIPLE_STREAM_T_H
#define MULTIPLE_STREAM_T_H

#include "StreamT.h"
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

/// Stream in the form of located objects

template<typename T>class MultiStreamT : public StreamT<T>
{
public:
	MultiStreamT(const std::string& rx_name, std::vector<T>& rx_objects, Module& rx_module, const std::string& rx_description, const Json::Value& rx_requirement = Json::nullValue) :
		StreamT<T>(rx_name, rx_objects.at(0), rx_module, rx_description, rx_requirement),
		m_objects(rx_objects),
		m_maxSize(rx_objects.size())
	{
		// note: we need to decide the size before this constructor
		//       since we use references to each element of the vector
		if(rx_objects.empty())
			throw MkException("Multiple inputs must contain a reference to a vector of size 1 or more. Please resize vector before initializing the stream. Please note that this size is the max size of the vector", LOC);
	}
	virtual ~MultiStreamT() {}

	void Serialize(std::ostream& x_out, MkDirectory* xp_dir = nullptr) const
	{
		Json::Value root;
		std::stringstream ss;
		StreamT<T>::Serialize(ss, xp_dir);
		ss >> root;

		root["maxSize"] = static_cast<int>(m_maxSize);
		root["nextObj"] = static_cast<int>(m_nextObj);
		std::stringstream ss2;
		serialize(ss2, m_objects);
		ss2 >> root["content"];

		x_out << root;
	}

	void Deserialize(std::istream& x_in, MkDirectory* xp_dir = nullptr)
	{
		Json::Value root;
		x_in >> root;  // note: copy first for local use

		std::stringstream ss;
		ss << root;
		StreamT<T>::Deserialize(ss, xp_dir);

		if(m_maxSize != root["maxSize"].asUInt())
			throw MkException("Deserializing to a MultiStream of wrong size", LOC);
		m_nextObj = root["m_nextObj"].asUInt();

		std::stringstream ss2;
		ss2 << root["content"];
		deserialize(ss2, m_objects);
	}

	void Connect(Stream& xr_stream) override
	{
		assert(m_objects.size() == m_maxSize && m_nextObj < m_maxSize);
		if(!this->IsConnected())
		{
			assert(m_nextObj == 0);
			StreamT<T>::Connect(xr_stream);
		}
		else
		{
			assert(!StreamT<T>::mr_module.GetInputStreamList().empty());
			auto it = StreamT<T>::mr_module.GetInputStreamList().end();
			it--;
			int lastId = it->second->id;
			std::stringstream ss;
			ss << StreamT<T>::GetName() << "-" << lastId;
			Stream* pstream = new StreamT<T>(ss.str(), m_objects.at(m_nextObj), Stream::mr_module, Stream::GetDescription());
			Stream::mr_module.AddInputStream(lastId + 1, pstream);
			pstream->Connect(xr_stream);
		}
		m_nextObj++;
	}

	void Disconnect() override
	{
		Stream::Disconnect();
		m_nextObj = 0;
		m_objects.clear();
	}

	void Export(std::ostream& rx_os) const override
	{
		Json::Value root;
		std::stringstream ss;
		StreamT<T>::Export(ss);
		ss >> root;
		root["multi"] = static_cast<int>(m_maxSize);
		rx_os << root;
	}

protected:
	std::vector<T>& m_objects;
	const size_t    m_maxSize;
	size_t          m_nextObj = 0;

private:
	static log4cxx::LoggerPtr m_logger;
};

#endif
