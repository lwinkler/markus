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

Stream::Stream(const std::string& x_name, Module& rx_module, const string& rx_description) :
	m_name(x_name),
	m_id(0),
	m_width(rx_module.GetWidth()),
	m_height(rx_module.GetHeight()),
	mr_module(rx_module),
	m_description(rx_description)
{
	m_connected = NULL;
	m_isReady = false;
	m_timeStamp = TIME_STAMP_INITIAL;
}

Stream::~Stream()
{
}

/**
* @brief Write description as XML of stream for module description
*
* @param rx_os         Output stream
* @param x_indentation Number of tabs for indentation
* @param x_isInput     Is an input
*/
void Stream::Export(ostream& rx_os, int x_indentation, bool x_isInput)
{
	string tabs(x_indentation , '\t');
	string inout = "output";
	if(x_isInput) inout = "input";
	rx_os<<tabs<<"<"<<inout<<" id=\""<<m_id<<"\">"<<endl;
	tabs = std::string(x_indentation + 1, '\t');
	rx_os<<tabs<<"<type>"<<GetTypeString()<<"</type>"<<endl;
	rx_os<<tabs<<"<name>"<<m_name<<"</name>"<<endl;
	rx_os<<tabs<<"<description>"<<GetDescription()<<"</description>"<<endl;
	tabs = std::string(x_indentation, '\t');
	rx_os<<tabs<<"</"<<inout<<">"<<endl;
}


/**
* @brief Connect an input with an output
*
* @param x_stream Output stream to connect to
*/
void Stream::Connect(Stream* x_stream)
{
	m_connected = x_stream;
}

void Stream::Serialize(std::ostream& x_out, const string& x_dir) const
{
	Json::Value root;
	root["name"]        = m_name;
	root["id"]          = m_id;
	root["type"]        = GetTypeString();
	root["width"]       = m_width;
	root["height"]      = m_height;
	root["description"] = m_description;
	root["timeStamp"]   = m_timeStamp;
	root["connected"]   = IsReady();
	root["ready"]       = m_isReady;
	x_out << root;
}

void Stream::Deserialize(std::istream& x_in, const string& x_dir)
{
	Json::Value root;
	x_in >> root;

	m_id   = root["id"].asInt();
	m_name = root["name"].asString();
	m_id   = root["id"].asInt();
	if(root["type"].asString() != GetTypeString())
		throw MkException("Stream must have the right type before serializing", LOC);
	m_width       = root["width"].asDouble();
	m_height      = root["height"].asDouble();
	m_description = root["description"].asString();
	m_timeStamp   = root["timeStamp"].asInt64();
	if(root["connected"] != IsConnected())
		throw MkException("Stream must have the same connection state before serializing", LOC);
	m_isReady = root["ready"].asBool();
}
