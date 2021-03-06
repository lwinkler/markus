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

#include "ConfigXml.h"
#include <tinyxml.h>
#include "ParameterStructure.h"
#include "util.h"

namespace mk {
using namespace std;

log4cxx::LoggerPtr ConfigXml::m_logger(log4cxx::Logger::getLogger("ConfigXml"));


/// split tag[name="bla"]>bloo into tag, name, bla and bloo
void splitTagName(const string& x_searchString, string& xr_tagName, string& xr_attrName, string& xr_attrValue, string& xr_searchString2)
{
	size_t pos1 = x_searchString.find('>');

	// split search string according to >
	string searchString1 = x_searchString;
	xr_searchString2 = "";
	if(pos1 != string::npos)
	{
		searchString1 = x_searchString.substr(0, pos1);
		xr_searchString2 = x_searchString.substr(pos1 + 1);
	}

	size_t pos2 = searchString1.find('[');
	if(pos2 == string::npos)
	{
		xr_tagName   = searchString1;
		xr_attrName  = "";
		xr_attrValue = "";
		return;
	}

	// If we have a [...] part in search string
	size_t pos3 = searchString1.find("=\"", pos2);
	if(pos3 == string::npos)
		throw MkException("Expecting a '=\"'", LOC);

	size_t pos4 = searchString1.find("\"]", pos3);
	if(pos4 == string::npos)
		throw MkException("Expecting a '\"]'", LOC);

	// Search subconfigs for the right one
	xr_tagName       = searchString1.substr(0, pos2);
	xr_attrName      = searchString1.substr(pos2 + 1, pos3 - pos2 - 1);
	xr_attrValue     = searchString1.substr(pos3 + 2, pos4 - pos3 - 2);
}


TiXmlDocument* createDoc(const std::string& x_fileName, bool x_allowCreation, bool x_header)
{
	try
	{
		// TiXmlDocument* doc = nullptr; // Initialize to null as there can be an error in construction
		TiXmlDocument* doc = new TiXmlDocument(x_fileName);
		if (!doc->LoadFile())
		{
			CLEAN_DELETE(doc);
			if(x_allowCreation)
			{
				assert(false); // note: this is not handled anymore
				createEmptyConfigFile(x_fileName);
				doc = new TiXmlDocument(x_fileName);
				auto tmp = doc->LoadFile();
				if(!tmp)
					throw MkException("Cannot create temporary file " + x_fileName, LOC);
			}
			else throw MkException("Could not load file as XML '" + x_fileName + "'. Error='" + (doc ? doc->ErrorDesc() : "") + "'. Exiting.", LOC);
		}
		return doc;
	}
	catch(exception& e)
	{
		throw MkException("Fatal exception in constructor of ConfigXml: " + string(e.what()), LOC);
	}
	catch(...)
	{
		throw MkException("Fatal exception in constructor of ConfigXml", LOC);
	}
	// note: avoid a compiler warning
	return nullptr;
}

/**
* @brief Constructor
*
* @param x_fileName      Name of the XML file with relative path
* @param x_allowCreation Allow the creation of a new file if unexistant
*/
ConfigFileXml::ConfigFileXml(const string& x_fileName, bool x_allowCreation, bool x_header) :
	mp_doc(dynamic_cast<TiXmlDocument*>(mp_node)),
	ConfigXml(createDoc(x_fileName, x_allowCreation, x_header))
{
	if(IsEmpty() || mp_doc == nullptr)
		throw MkException("Initialize a ConfigXml to an empty config", LOC);
}

ConfigXml& ConfigXml::operator = (const ConfigXml& x_conf)
{
	mp_node      = x_conf.mp_node;
	return *this;
}

ConfigFileXml::~ConfigFileXml()
{
	CLEAN_DELETE(mp_doc);
}


/**
* @brief Constructor for a class based on a sub-node (used internally)
*
* @param xp_node
*/
ConfigXml::ConfigXml(TiXmlNode* xp_node) :
	mp_node(xp_node)
{
}

ConfigXml::ConfigXml(const ConfigXml& x_conf) :
	mp_node(x_conf.mp_node)
{
	// if(IsEmpty())
	// throw MkException("Initialize a ConfigXml to an empty config", LOC);
}

ConfigXml::~ConfigXml()
{
	// ConfigXml is a reference: do not delete anything
}

/**
* @brief Return a config objects that points to the sub element of configuration
*
* @param x_tagName   The node name of the sub element (= XML tag)
*
* @return config object
*/
const ConfigXml ConfigXml::GetSubConfig(const string& x_tagName) const
{
	if(IsEmpty())
		throw MkException("Impossible to find node " + x_tagName + " in ConfigXml", LOC);
	TiXmlNode* newNode = mp_node->FirstChild(x_tagName);
	return ConfigXml(newNode);
}


/**
* @brief Return a config objects that points to the sub element of configuration
*
* @param x_tagName   The node name of the sub element (= XML tag)
* @param x_allowCreation Allow to create the node in XML if inexistant
*
* @return config object
*/
ConfigXml ConfigXml::RefSubConfig(const string& x_tagName, bool x_allowCreation)
{
	if(IsEmpty())
		throw MkException("Impossible to find node " + x_tagName + " in ConfigXml" , LOC);

	TiXmlNode* newNode = mp_node->FirstChild(x_tagName);

	if(newNode == nullptr && x_allowCreation)
	{
		// Add a sub config element if not found
		auto   element = new TiXmlElement(x_tagName);
		mp_node->LinkEndChild(element);
		return ConfigXml(element);
	}
	return ConfigXml(newNode);
}

/**
* @brief Return a config objects that was appended to the current config
*
* @param x_tagName   The node name of the sub element (= XML tag)
*
* @return config object
*/
ConfigXml ConfigXml::Append(const string& x_tagName)
{
	if(IsEmpty())
		throw MkException("Impossible to find node " + x_tagName + " in ConfigXml" , LOC);

	// Add a sub config element if not found
	auto element = new TiXmlElement(x_tagName);
	mp_node->LinkEndChild(element);
	return ConfigXml(element);
}



/**
* @brief Return a config objects that points to the sub element of configuration
*
* @param x_tagName   The node name of the sub element (= XML tag)
* @param x_attrName   The name of the sub element attribute
* @param x_attrValue  The name of the sub element attribute value
*
* @return config object
*/
const ConfigXml ConfigXml::GetSubConfig(const string& x_tagName, const string& x_attrName, const string& x_attrValue) const
{
	if(IsEmpty())
		throw MkException("Impossible to find node " + x_tagName + " in ConfigXml with attribute " + x_attrName + "=\"" + x_attrValue + "\"" , LOC);
	TiXmlNode* newNode = mp_node->FirstChild(x_tagName);

	if(x_attrName == "")
		return ConfigXml(newNode);

	while(newNode != nullptr && newNode->ToElement()->Attribute(x_attrName.c_str()) != x_attrValue)
	{
		newNode = newNode->NextSibling(x_tagName);
	}

	return ConfigXml(newNode);
}

/**
* @brief Return a config objects that points to the sub element of configuration (non-constant)
*
* @param x_tagName       The type of the sub element (= XML balise)
* @param x_attrName      One attribute of the sub element to match
* @param x_attrValue     The value of the sub element to match
* @param x_allowCreation Allow to create the node in XML if inexistant
*
* @return config object
*/
ConfigXml ConfigXml::RefSubConfig(const string& x_tagName, const string& x_attrName, const string& x_attrValue, bool x_allowCreation)
{
	if(IsEmpty())
		throw MkException("Impossible to find node " + x_tagName + " in ConfigXml with attribute " + x_attrName + "=\"" + x_attrValue + "\"" , LOC);

	TiXmlNode* newNode = mp_node->FirstChild(x_tagName);

	if(x_attrName != "")
	{
		const char* name = nullptr;
		while(newNode != nullptr && ((name = newNode->ToElement()->Attribute("name")) == nullptr || x_attrValue != name))
		{
			newNode = newNode->NextSibling(x_tagName);
		}
	}
	if(newNode == nullptr && x_allowCreation)
	{
		// Add a sub config element if not found
		auto   element = new TiXmlElement(x_tagName);
		if(x_attrName != "")
			element->SetAttribute(x_attrName, x_attrValue);
		mp_node->LinkEndChild(element);
		return ConfigXml(element);
	}
	return ConfigXml(newNode);
}

/**
* @brief Return a sub element that points to the next sub element of the configuration
*
* @param x_tagName The type of the sub element (= XML balise)
* @param x_objectName The name of the sub element (= attribute "name")
*
* @return config object
*/
ConfigXml ConfigXml::NextSubConfig(const string& x_tagName, const string& x_attrName, const string& x_attrValue) const
{
	if(IsEmpty())
		throw MkException("Impossible to find node " + x_tagName + " in ConfigXml with " + x_attrName + "\"" + x_attrValue + "\"" , LOC);
	TiXmlNode* newNode = mp_node->NextSibling(x_tagName);

	if(x_attrName != "")
	{
		while(newNode != nullptr && x_attrValue != newNode->ToElement()->Attribute(x_attrName.c_str()))
		{
			newNode = newNode->NextSibling(x_tagName);
		}
	}

	return ConfigXml(newNode);
}

/**
* @brief Return the attribute (as string) for one element
*
* @param x_attributeName Name of the attribute
*
* @return attribute
*/
const string ConfigXml::GetAttribute(const string& x_attributeName) const
{
	if(IsEmpty())
		throw MkException("Impossible to find attribute " + x_attributeName + " in ConfigXml, node is empty" , LOC);
	TiXmlElement* element = mp_node->ToElement();
	//string s(*element->Attribute(x_attributeName));
	if(element == nullptr)
		throw MkException("Impossible to find node in ConfigXml", LOC);

	const string * str = element->Attribute(x_attributeName);
	if(str == nullptr)
		throw MkException("Attribute " + x_attributeName + " is unexistant", LOC);
	else
		return *str;
}


/**
* @brief Return the attribute (as string) for one element
*
* @param x_attributeName Name of the attribute
* @param x_default       Value to return if attribute is unexistant
*
* @return attribute
*/
const string ConfigXml::GetAttribute(const string& x_attributeName, const string& x_default) const
{
	if(IsEmpty())
		throw MkException("Impossible to find attribute " + x_attributeName + " in ConfigXml" , LOC);
	TiXmlElement* element = mp_node->ToElement();
	//string s(*element->Attribute(x_attributeName));
	if(element == nullptr)
		throw MkException("Impossible to find node in ConfigXml", LOC);

	const string * str = element->Attribute(x_attributeName);
	if(str == nullptr)
		return x_default;
	else
		return *str;
}

/**
* @brief Set the attribute for one element
*
* @param x_attributeName Name of the attribute
* @param x_value         Value to set
*/
void ConfigXml::SetAttribute(const string& x_attributeName, const string& x_value)
{
	if(IsEmpty())
		throw MkException("Impossible to find attribute " + x_attributeName + " in ConfigXml" , LOC);
	TiXmlElement* element = mp_node->ToElement();
	if(element == nullptr)
		throw MkException("Impossible to find attribute " + x_attributeName + " in ConfigXml" , LOC);

	element->SetAttribute(x_attributeName, x_value);
}

/**
* @brief Return the value (as string) for one element
*
* @return value
*/
string ConfigXml::GetValue() const
{
	if(IsEmpty())
		throw MkException("Impossible to find node" , LOC);
	TiXmlElement* element = mp_node->ToElement();
	const char * str = element->GetText();
	if(str == nullptr)
		return "";
	else
		return str;
}

/**
* @brief Set the value as string
*
* @param x_value Value to set
*/
void ConfigXml::SetValue(const string& x_value)
{
	if(IsEmpty())
		throw MkException("Impossible to find node" , LOC);
	mp_node->Clear();
	mp_node->LinkEndChild(new TiXmlText(x_value)); //ToText();
}

/**
* @brief Save the config as an xml file
*
* @param x_file Name of the file with relative path
*/
void ConfigFileXml::SaveToFile(const string& x_file) const
{
	if(!mp_doc->SaveFile(x_file))
		throw MkException("Error saving to file " + x_file, LOC);
}


/**
* @brief Find a sub config (with a similar syntax as JQuery)
*
* @param  x_searchString The search path with jquery-like syntax
* @return value
*/
const ConfigXml ConfigXml::Find(const string& x_searchString) const
{
	// If empty return node: for recurrent function
	if(x_searchString.empty())
		return *this;

	string tagName, attrName, attrValue, searchString2;
	splitTagName(x_searchString, tagName, attrName, attrValue, searchString2);

	// cout << "Search in config: " << tagName << "[" << attrName << "=\"" << attrValue << "\"]" << endl;

	if(attrName == "")
		return GetSubConfig(tagName).Find(searchString2);
	else
		return GetSubConfig(tagName, attrName, attrValue).Find(searchString2);
}

/**
* @brief Find a sub config (with a similar syntax as JQuery)
*
* @param  x_searchString The search path with jquery-like syntax
* @return value
*/
ConfigXml ConfigXml::FindRef(const string& x_searchString, bool x_allowCreation)
{
	if(x_searchString.empty())
		return *this;

	string tagName, attrName, attrValue, searchString2;
	splitTagName(x_searchString, tagName, attrName, attrValue, searchString2);

	if(attrName == "")
		return RefSubConfig(tagName, x_allowCreation).FindRef(searchString2, x_allowCreation);
	else
		return RefSubConfig(tagName, attrName, attrValue, x_allowCreation).FindRef(searchString2, x_allowCreation);
}


/**
* @brief Find all sub configs (with a similar syntax as JQuery)
*
* @param  x_searchString The search path with jquery-like syntax
* @return A vector of configurations
*/
vector<ConfigXml> ConfigXml::FindAll(const string& x_searchString) const
{
	vector<ConfigXml> results;

	if(IsEmpty())
	{
		return results;
	}
	// If empty return node: for recurrent function
	if(x_searchString.empty())
	{
		results.push_back(*this);
		return results;
	}

	string tagName, attrName, attrValue, searchString2;
	splitTagName(x_searchString, tagName, attrName, attrValue, searchString2);

	if(searchString2 == "")
	{
		ConfigXml conf = GetSubConfig(tagName, attrName, attrValue);
		while(!conf.IsEmpty())
		{
			results.push_back(conf);
			conf = conf.NextSubConfig(tagName, attrName, attrValue);
		}
		return results;
	}
	else if(attrName == "")
		return GetSubConfig(tagName).FindAll(searchString2);
	else
		return GetSubConfig(tagName, attrName, attrValue).FindAll(searchString2);
}
} // namespace mk
