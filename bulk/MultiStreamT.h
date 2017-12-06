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

/// Stream in the form of located objects

template<typename T>class MultiStreamT : public StreamT<T>
{
public:
	inline void to_json(mkjson& rx_json, const MultiStreamT<T>& x_ser) {x_ser.Serialize(rx_json);}
	inline void from_json(const mkjson& x_json, MultiStreamT<T>& rx_ser) {rx_ser.Deserialize(x_json);}
	
	MultiStreamT(const std::string& rx_name, std::vector<T>& rx_objects, Module& rx_module, const std::string& rx_description, const mkjson& rx_requirement = nullptr) :
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

	void Serialize(mkjson& rx_json, MkDirectory* xp_dir = nullptr) const
	{
		StreamT<T>::Serialize(rx_json, xp_dir);

		rx_json["maxSize"] = m_maxSize;
		rx_json["nextObj"] = m_nextObj;
		rx_json["content"] = m_objects;
	}

	void Deserialize(const mkjson& x_json, MkDirectory* xp_dir = nullptr)
	{
		StreamT<T>::Deserialize(x_json, xp_dir);

		if(m_maxSize != x_json.at("maxSize").get<uint>())
			throw MkException("Deserializing to a MultiStream of wrong size", LOC);
		from_mkjson(x_json.at("nextObj"), m_nextObj);
		from_mkjson(x_json.at("content"), m_objects);
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

	mkjson Export() const override
	{
		mkjson json = StreamT<T>::Export();
		json["multi"] = m_maxSize;
		return json;
	}

protected:
	std::vector<T>& m_objects;
	const size_t    m_maxSize;
	size_t          m_nextObj = 0;
};

#endif
