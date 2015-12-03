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

template<typename T>class MultipleStreamT : public StreamT<T>
{
public:
	MultipleStreamT(const std::string& rx_name, std::vector<T>& rx_objects, Module& rx_module, const std::string& rx_description, const std::string& rx_requirement = "") :
		StreamT<T>(rx_name, rx_objects.at(0), rx_module, rx_description, rx_requirement),
		m_objects(rx_objects)
	{
		if(rx_objects.empty())
			throw MkException("Multiple inputs must contain a reference to a vector of size 1 or more. Please resize vector before initializing the stream.", LOC);
	}
	~MultipleStreamT(){}
	virtual void Connect(Stream *x_stream, bool x_bothWays = true)
	{
		if(m_objects.size() == 0)
		{
			throw MkException("Multiple inputs must contain a reference to a vector of size 1 or more. Please resize vector before initializing the stream.", LOC);
		}
		else if(m_objects.size() == 1)
		{
			Stream::Connect(x_stream, x_bothWays);
		}
		else
		{
			assert(!StreamT<T>::mr_module.GetInputStreamList().empty());
			auto it = StreamT<T>::mr_module.GetInputStreamList().end();
			it--;
			int lastId = it->first;
			Stream::mr_module.AddInputStream(lastId + 1, new StreamT<T>(StreamT<T>::GetName(), m_objects.back(), Stream::mr_module, Stream::GetDescription()));
		}
	}

	virtual void Export(std::ostream& rx_os, int x_id, int x_indentation, bool x_isInput) const override
	{
		std::string tabs(x_indentation , '\t');
		std::string inout = "output";
		if(x_isInput) inout = "input";
		rx_os<<tabs<<"<"<<inout<<" id=\""<<x_id<<"\" multi=\"1\">"<<std::endl;
		tabs = std::string(x_indentation + 1, '\t');
		rx_os<<tabs<<"<type>"<<StreamT<T>::GetType()<<"</type>"<<std::endl;
		rx_os<<tabs<<"<name>"<<Stream::m_name<<"</name>"<<std::endl;
		rx_os<<tabs<<"<description>"<<Stream::GetDescription()<<"</description>"<<std::endl;
		tabs = std::string(x_indentation, '\t');
		rx_os<<tabs<<"</"<<inout<<">"<<std::endl;
	}

protected:
	std::vector<T>& m_objects;

private:
	DISABLE_COPY(MultipleStreamT)
	static log4cxx::LoggerPtr m_logger;
	static const std::string m_type;
	static const std::string m_class;
};

#endif
