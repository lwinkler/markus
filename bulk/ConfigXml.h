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

#ifndef CONFIG_XML_H
#define CONFIG_XML_H

// note: this class is kept to draw SVG and also for legacy reasons

#include <string>
#include <vector>
#include <sstream>
#include <boost/noncopyable.hpp>

#include "define.h"
#include <log4cxx/xml/domconfigurator.h>


class TiXmlNode;
class TiXmlDocument;


/**
* @brief       Class used to manipulate configuration files
* @brief       A ConfigXml object is a reference to a sub part of a ConfigFile.
               This utility relies on TinyXml.
*/
class ConfigXml
{
public:
	ConfigXml(const ConfigXml& x_conf);
	virtual ~ConfigXml();

	// Method to access elements of the config
	const ConfigXml GetSubConfig(const std::string& x_tagName) const;
	const ConfigXml GetSubConfig(const std::string& x_tagName, const std::string& x_attrName, const std::string& x_attrValue) const;
	ConfigXml RefSubConfig(const std::string& x_tagName, bool x_allowCreation = false);
	ConfigXml RefSubConfig(const std::string& x_tagName, const std::string& x_attrName, const std::string& x_attrValue, bool x_allowCreation = false);
	ConfigXml Append(const std::string& x_tagName);

	/// Check if the config object is empty
	inline bool IsEmpty() const {return mp_node == nullptr;}
	std::string GetValue() const;
	void SetValue(const std::string& x_value);
	template<typename T> inline void SetValue(const T& x_value)
	{
		std::stringstream ss;
		ss << x_value;
		SetValue(ss.str());
	}
	const std::string GetAttribute(const std::string& x_attributeName) const;
	const std::string GetAttribute(const std::string& x_attributeName, const std::string& x_default) const;
	void SetAttribute(const std::string& x_attributeName, const std::string& x_value);
	template<typename T> inline void SetAttribute(const std::string& x_attributeName, const T& x_value)
	{
		std::stringstream ss;
		ss << x_value;
		SetAttribute(x_attributeName, ss.str());
	}
	/// Redefinition of == operator
	inline bool operator == (const ConfigXml &a) {return a.mp_node == mp_node;}

	// New access functions with JQuery-like syntax
	const ConfigXml Find(const std::string& x_searchString) const;
	ConfigXml    FindRef(const std::string& x_searchString, bool x_allowCreation = false);
	std::vector<ConfigXml> FindAll(const std::string& x_searchString) const;

protected:
	ConfigXml(TiXmlNode* xp_node);
	ConfigXml& operator = (const ConfigXml& x_conf);
	ConfigXml NextSubConfig(const std::string& x_tagName, const std::string& x_attrName = "", const std::string& x_attrValue = "") const;

	TiXmlNode* mp_node;

private:
	static log4cxx::LoggerPtr m_logger;
};

/**
* @brief       Class used to manipulate configuration files
* @brief       A ConfigFile object is created from an XML file. It can browse the different tags of the file by creating sub config objects. Each sub config object is
               a reference to the sub configuration. Not a copy.
               This utility relies on TinyXml.
*/
class ConfigFileXml : public ConfigXml, boost::noncopyable
{
public:
	ConfigFileXml(const std::string& x_fileName, bool x_allowCreation = false, bool x_header = true);
	virtual ~ConfigFileXml();
	void SaveToFile(const std::string& x_file) const;

private:
	TiXmlDocument* mp_doc;
};


#endif
