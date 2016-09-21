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
	Parameter(x_name, rx_description),
	mr_module(rx_module), // TODO: Do we need this
	m_requirement(rx_requirement),
	m_timeStamp(TIME_STAMP_MIN)
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
void Stream::Export(ostream& rx_os, int x_id, int x_indentation, bool x_isInput) const
{
	string tabs(x_indentation , '\t');
	string inout = "output";
	if(x_isInput) inout = "input";
	rx_os<<tabs<<"<"<<inout<<" id=\""<<x_id<<"\">"<<endl;
	tabs = string(x_indentation + 1, '\t');
	rx_os<<tabs<<"<type>"<<GetType()<<"</type>"<<endl;
	rx_os<<tabs<<"<name>"<<GetName()<<"</name>"<<endl;
	rx_os<<tabs<<"<description>"<<GetDescription()<<"</description>"<<endl;
	tabs = string(x_indentation, '\t');
	rx_os<<tabs<<"</"<<inout<<">"<<endl;
}


/**
* @brief Connect an input with an output
*
* @param x_stream Output stream to connect to
*/
void Stream::Connect(Stream* x_stream)
{
	assert(x_stream != nullptr);
	m_connected = x_stream;
	m_connected->SetAsConnected(true);
	SetAsConnected(true);
}

/**
* @brief Disconnect an input with an output
*
* @param x_stream Output stream to connect to
*/
void Stream::Disconnect()
{
	m_connected = nullptr;
	m_connected->SetAsConnected(false);
	SetAsConnected(false);
}

void Stream::Serialize(ostream& x_out, MkDirectory* xp_dir) const
{
	Json::Value root;
	root["name"]        = GetName();
	// root["id"]          = m_id;
	root["type"]        = GetType();
	root["description"] = GetDescription();
	root["timeStamp"]   = Json::UInt64(m_timeStamp.load());
	root["connected"]   = IsConnected();
	x_out << root;
}


void Stream::Deserialize(istream& x_in, MkDirectory* xp_dir)
{
	Json::Value root;
	x_in >> root;

	if(GetName() != root["name"].asString())
		throw MkException("Stream must have the same name before serializing: " + GetName(), LOC);
	// if(m_id   != root["id"].asInt())
	// throw MkException("Stream must have the right id before serializing", LOC);
	// cout<<root["type"].asString()<<" != "<<GetType()<<endl;
	if(root["type"].asString() != GetType())
		throw MkException("Stream must have the right type before serializing: " + GetType(), LOC);
	if(GetDescription() != root["description"].asString())
		LOG_WARN(m_logger, "Stream does not have the same description");
	m_timeStamp   = root["timeStamp"].asInt64();
	if(root["connected"] != IsConnected())
		throw MkException("Stream must have the same connection state before deserializing", LOC);
}
