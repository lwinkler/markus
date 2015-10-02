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

#include "Stream.h"
#include "Module.h"
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

using namespace std;

log4cxx::LoggerPtr Stream::m_logger(log4cxx::Logger::getLogger("Stream"));

Stream::Stream(const string& x_name, Module& rx_module, const string& rx_description, const string& rx_requirement) :
	m_name(x_name),
	m_width(rx_module.GetWidth()),
	m_height(rx_module.GetHeight()),
	mr_module(rx_module),
	m_description(rx_description),
	m_requirement(rx_requirement),
	m_timeStamp(TIME_STAMP_INITIAL),
	m_connected(nullptr),
	m_isReady(false)
{
}

Stream::~Stream()
{
}

/**
* @brief Write description as XML of stream for module description
*
* @param rx_os         Output stream
* @param x_id          Id of stream
* @param x_indentation Number of tabs for indentation
* @param x_isInput     Is an input
*/
void Stream::Export(ostream& rx_os, int x_id, int x_indentation, bool x_isInput)
{
	string tabs(x_indentation , '\t');
	string inout = "output";
	if(x_isInput) inout = "input";
	rx_os<<tabs<<"<"<<inout<<" id=\""<<x_id<<"\">"<<endl;
	tabs = string(x_indentation + 1, '\t');
	rx_os<<tabs<<"<type>"<<GetType()<<"</type>"<<endl;
	rx_os<<tabs<<"<name>"<<m_name<<"</name>"<<endl;
	rx_os<<tabs<<"<description>"<<GetDescription()<<"</description>"<<endl;
	tabs = string(x_indentation, '\t');
	rx_os<<tabs<<"</"<<inout<<">"<<endl;
}


/**
* @brief Connect an input with an output
*
* @param x_stream Output stream to connect to
*/
void Stream::Connect(Stream* x_stream, bool x_bothWays)
{
	assert(x_stream != nullptr);
	m_connected = x_stream;
	if(x_bothWays)
		x_stream->Connect(this, false);
}

void Stream::Serialize(MkJson& xr_out, const string& x_dir) const
{
	xr_out["name"]        = m_name;
	// xr_out["id"]          = m_id;
	xr_out["type"]        = GetType();
	xr_out["width"]       = m_width;
	xr_out["height"]      = m_height;
	xr_out["description"] = m_description;
	xr_out["timeStamp"]   = m_timeStamp.load();
	xr_out["connected"]   = IsConnected();
	xr_out["ready"]       = m_isReady;
}


void Stream::Deserialize(MkJson& xr_in, const string& x_dir)
{
	m_name = xr_in["name"].AsString();
	// if(m_id   != xr_in["id"].AsInt())
	// throw MkException("Stream must have the right id before serializing", LOC);
	// cout<<xr_in["type"].AsString()<<" != "<<GetType()<<endl;
	if(xr_in["type"].AsString() != GetType())
		throw MkException("Stream must have the right type before serializing", LOC);
	m_width       = xr_in["width"].AsDouble();
	m_height      = xr_in["height"].AsDouble();
	m_description = xr_in["description"].AsString();
	m_timeStamp   = xr_in["timeStamp"].AsInt64();
	if(xr_in["connected"] != IsConnected())
		throw MkException("Stream must have the same connection state before deserializing", LOC);
	m_isReady = xr_in["ready"].AsBool();
}
