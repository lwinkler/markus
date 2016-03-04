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
	MultiStreamT(const std::string& rx_name, std::vector<T>& rx_objects, Module& rx_module, const std::string& rx_description, const std::string& rx_requirement = "") :
		StreamT<T>(rx_name, rx_objects.at(0), rx_module, rx_description, rx_requirement),
		m_objects(rx_objects),
		m_size(rx_objects.size())
	{
		// note: we need to decide the size before this constructor
		//       since we use references to each element of the vector
		if(rx_objects.empty())
			throw MkException("Multiple inputs must contain a reference to a vector of size 1 or more. Please resize vector before initializing the stream. Please note that this size is the max size of the vector", LOC);
	}
	~MultiStreamT() {}
	virtual void Connect(Stream *x_stream)
	{
		assert(m_objects.size() == m_size && m_nextObj < m_size);
		if(!this->IsConnected())
		{
			assert(m_nextObj == 0);
			StreamT<T>::Connect(x_stream);
		}
		else
		{
			assert(!StreamT<T>::mr_module.GetInputStreamList().empty());
			auto it = StreamT<T>::mr_module.GetInputStreamList().end();
			it--;
			int lastId = it->first;
			Stream* pstream = new StreamT<T>(StreamT<T>::GetName(), m_objects.at(m_nextObj), Stream::mr_module, Stream::GetDescription());
			Stream::mr_module.AddInputStream(lastId + 1, pstream);
			pstream->Connect(x_stream);
		}
		m_nextObj++;
	}

	virtual void Export(std::ostream& rx_os, int x_id, int x_indentation, bool x_isInput) const override
	{
		std::string tabs(x_indentation , '\t');
		std::string inout = "output";
		if(x_isInput) inout = "input";
		rx_os<<tabs<<"<"<<inout<<" id=\""<<x_id<<"\" multi=\"" << m_size << "\">"<<std::endl;
		tabs = std::string(x_indentation + 1, '\t');
		rx_os<<tabs<<"<type>"<<StreamT<T>::GetParameterType()<<"</type>"<<std::endl;
		rx_os<<tabs<<"<name>"<<Stream::m_name<<"</name>"<<std::endl;
		rx_os<<tabs<<"<description>"<<Stream::GetDescription()<<"</description>"<<std::endl;
		tabs = std::string(x_indentation, '\t');
		rx_os<<tabs<<"</"<<inout<<">"<<std::endl;
	}

protected:
	std::vector<T>& m_objects;
	const size_t    m_size;
	size_t          m_nextObj = 0;

private:
	DISABLE_COPY(MultiStreamT)
	static log4cxx::LoggerPtr m_logger;
	static const std::string m_type;
	static const std::string m_class;
};

#endif
