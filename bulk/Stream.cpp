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
#include "json.hpp"

using namespace std;

Stream::Stream(const string& x_name, Module& rx_module, const string& rx_description, const mkjson& rx_requirement) :
	Parameter(x_name, rx_description),
	mr_module(rx_module),
	m_timeStamp(TIME_STAMP_MIN)
{
	if(!rx_requirement.is_null())
		SetRange(rx_requirement);
}

Stream::~Stream()
{
}

/**
* @brief Write description as JSON of stream for module description
*
* @param rx_os         Output stream
* @param x_id          Id of stream
*/
void Stream::Export(ostream& rx_os) const
{
	mkjson root;
	// root["id"] = id;
	root["type"] = GetType();
	root["name"] = GetName();
	root["default"] = GetDefault();
	root["description"] = GetDescription();
	root["stream"] = true;
	root["range"] = GetRange();
	rx_os << multiLine(root);
}


/**
* @brief Connect an input with an output
*
* @param xr_stream Output stream to connect to
*/
void Stream::Connect(Stream& xr_stream)
{
	m_connected = &xr_stream;
	m_connected->SetAsConnected(true);
	SetAsConnected(true);
}

/**
* @brief Disconnect an input with an output
*
*/
void Stream::Disconnect()
{
	SetAsConnected(false);
	m_connected = nullptr;
}

void Stream::Serialize(mkjson& rx_json, MkDirectory* xp_dir) const
{
	rx_json = mkjson{
		{"name", GetName()}, 
		{"type", GetType()}, 
		{"description", GetDescription()}, 
		{"timeStamp", m_timeStamp.load()}, 
		{"connected", IsConnected()}
	};
}


void Stream::Deserialize(const mkjson& x_json, MkDirectory* xp_dir)
{
	// if(GetName() != root["name"].asString())
		// throw MkException("Stream must have the same name before serializing: " + GetName(), LOC);
	if(x_json.at("type").get<string>() != GetType())
		throw MkException("Stream must have the right type before serializing: " + GetType(), LOC);
	if(GetDescription() != x_json.at("description").get<string>())
		LOG_WARN(m_logger, "Stream does not have the same description");
	m_timeStamp = x_json.at("timeStamp").get<uint64_t>();
	if(x_json.at("connected") != IsConnected())
		throw MkException("Stream must have the same connection state before deserializing", LOC);
}

