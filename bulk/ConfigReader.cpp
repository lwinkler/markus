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

#include "ConfigReader.h"
#include "ParameterStructure.h"
#include "util.h"
#include <tinyxml.h>

using namespace std;

log4cxx::LoggerPtr ConfigReader::m_logger(log4cxx::Logger::getLogger("ConfigReader"));


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
		TiXmlDocument* doc = nullptr; // Initialize to null as there can be an error in construction
		doc = new TiXmlDocument(x_fileName);
		if (!doc->LoadFile())
		{
			CLEAN_DELETE(doc);
			if(x_allowCreation)
			{
				createEmptyConfigFile(x_fileName, x_header);
				doc = new TiXmlDocument(x_fileName);
				auto tmp = doc->LoadFile();
				if(!tmp)
					throw MkException("Cannot create temporary file", LOC);
			}
			else throw MkException("Could not load file as XML '" + x_fileName + "'. Error='" + (doc ? doc->ErrorDesc() : "") + "'. Exiting.", LOC);
		}
		return doc;
	}
	catch(exception& e)
	{
		throw MkException("Fatal exception in constructor of ConfigReader: " + string(e.what()), LOC);
	}
	catch(...)
	{
		throw MkException("Fatal exception in constructor of ConfigReader", LOC);
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
ConfigFile::ConfigFile(const string& x_fileName, bool x_allowCreation, bool x_header) :
	mp_doc(dynamic_cast<TiXmlDocument*>(mp_node)),
	ConfigReader(createDoc(x_fileName, x_allowCreation, x_header))
{
	if(IsEmpty() || mp_doc == nullptr)
		throw MkException("Initialize a ConfigReader to an empty config", LOC);
}

ConfigReader& ConfigReader::operator = (const ConfigReader& x_conf)
{
	mp_node      = x_conf.mp_node;
	return *this;
}

ConfigFile::~ConfigFile()
{
	CLEAN_DELETE(mp_doc);
}


/**
* @brief Constructor for a class based on a sub-node (used internally)
*
* @param xp_node
*/
ConfigReader::ConfigReader(TiXmlNode* xp_node) :
	mp_node(xp_node)
{
}

ConfigReader::ConfigReader(const ConfigReader& x_conf) :
	mp_node(x_conf.mp_node)
{
	// if(IsEmpty())
	// throw MkException("Initialize a ConfigReader to an empty config", LOC);
}

ConfigReader::~ConfigReader()
{
	// ConfigReader is a reference: do not delete anything
}

/**
* @brief Return a config objects that points to the sub element of configuration
*
* @param x_tagName   The node name of the sub element (= XML tag)
*
* @return config object
*/
const ConfigReader ConfigReader::GetSubConfig(const string& x_tagName) const
{
	if(IsEmpty())
		throw MkException("Impossible to find node " + x_tagName + " in ConfigReader", LOC);
	TiXmlNode* newNode = mp_node->FirstChild(x_tagName);
	return ConfigReader(newNode);
}

/**
* @brief Return a config objects that points to the sub element of configuration (ignoring XML namespaces). Experimental
*
* Note: xml namespaces can be of different formats http://www.w3schools.com/xml/xml_namespaces.asp
*       We only account for the tag name. This can of course be done in a cleaner way by implementing libxml++. So far a bug is preventing this.
*
* Known bug: it seems that NextSibling only browse children nodes with one name and ignores others.
*
* @param x_tagName   The node name of the sub element (= XML tag)
*
* @return config object
*/
const ConfigReader ConfigReader::GetSubConfigIgnoreNamespace(const string& x_tagName) const
{
	if(IsEmpty())
		throw MkException("Impossible to find node " + x_tagName + " in ConfigReader", LOC);
	TiXmlNode* newNode = mp_node->FirstChild();

	while(newNode != nullptr && mp_node->Value() != x_tagName)
	{
		string tag(newNode->Value());

		// Strip everything after first space
		auto pos = tag.find(' ');
		if(pos != string::npos)
			tag = tag.substr(0, pos);

		// Strip everything before :
		pos = tag.find(':');
		if(pos != string::npos)
			tag = tag.substr(pos + 1);

		if(tag == x_tagName)
			return ConfigReader(newNode);

		newNode = mp_node->NextSibling();
	}
	return ConfigReader(newNode);
}


/**
* @brief Return a config objects that points to the sub element of configuration
*
* @param x_tagName   The node name of the sub element (= XML tag)
* @param x_allowCreation Allow to create the node in XML if inexistant
*
* @return config object
*/
ConfigReader ConfigReader::RefSubConfig(const string& x_tagName, bool x_allowCreation)
{
	if(IsEmpty())
		throw MkException("Impossible to find node " + x_tagName + " in ConfigReader" , LOC);

	TiXmlNode* newNode = mp_node->FirstChild(x_tagName);

	if(newNode == nullptr && x_allowCreation)
	{
		// Add a sub config element if not found
		auto   element = new TiXmlElement(x_tagName);
		mp_node->LinkEndChild(element);
		return ConfigReader(element);
	}
	return ConfigReader(newNode);
}

/**
* @brief Return a config objects that was appended to the current config
*
* @param x_tagName   The node name of the sub element (= XML tag)
*
* @return config object
*/
ConfigReader ConfigReader::Append(const string& x_tagName)
{
	if(IsEmpty())
		throw MkException("Impossible to find node " + x_tagName + " in ConfigReader" , LOC);

	// Add a sub config element if not found
	auto element = new TiXmlElement(x_tagName);
	mp_node->LinkEndChild(element);
	return ConfigReader(element);
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
const ConfigReader ConfigReader::GetSubConfig(const string& x_tagName, const string& x_attrName, const string& x_attrValue) const
{
	if(IsEmpty())
		throw MkException("Impossible to find node " + x_tagName + " in ConfigReader with name " + x_attrName + "=\"" + x_attrValue + "\"" , LOC);
	TiXmlNode* newNode = mp_node->FirstChild(x_tagName);

	if(x_attrName == "")
		return ConfigReader(newNode);

	while(newNode != nullptr && newNode->ToElement()->Attribute(x_attrName.c_str()) != x_attrValue)
	{
		newNode = newNode->NextSibling(x_tagName);
	}

	return ConfigReader(newNode);
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
ConfigReader ConfigReader::RefSubConfig(const string& x_tagName, const string& x_attrName, const string& x_attrValue, bool x_allowCreation)
{
	if(IsEmpty())
		throw MkException("Impossible to find node " + x_tagName + " in ConfigReader with name " + x_attrName + "=\"" + x_attrValue + "\"" , LOC);

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
		return ConfigReader(element);
	}
	return ConfigReader(newNode);
}

/**
* @brief Return a sub element that points to the next sub element of the configuration
*
* @param x_tagName The type of the sub element (= XML balise)
* @param x_objectName The name of the sub element (= attribute "name")
*
* @return config object
*/
ConfigReader ConfigReader::NextSubConfig(const string& x_tagName, const string& x_attrName, const string& x_attrValue) const
{
	if(IsEmpty())
		throw MkException("Impossible to find node " + x_tagName + " in ConfigReader with " + x_attrName + "\"" + x_attrValue + "\"" , LOC);
	TiXmlNode* newNode = mp_node->NextSibling(x_tagName);

	if(x_attrName != "")
	{
		while(newNode != nullptr && x_attrValue != newNode->ToElement()->Attribute(x_attrName.c_str()))
		{
			newNode = newNode->NextSibling(x_tagName);
		}
	}

	return ConfigReader(newNode);
}

/**
* @brief Return the attribute (as string) for one element
*
* @param x_attributeName Name of the attribute
*
* @return attribute
*/
const string ConfigReader::GetAttribute(const string& x_attributeName) const
{
	if(IsEmpty())
		throw MkException("Impossible to find attribute " + x_attributeName + " in ConfigReader, node is empty" , LOC);
	TiXmlElement* element = mp_node->ToElement();
	//string s(*element->Attribute(x_attributeName));
	if(element == nullptr)
		throw MkException("Impossible to find node in ConfigReader", LOC);

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
const string ConfigReader::GetAttribute(const string& x_attributeName, const string& x_default) const
{
	if(IsEmpty())
		throw MkException("Impossible to find attribute " + x_attributeName + " in ConfigReader" , LOC);
	TiXmlElement* element = mp_node->ToElement();
	//string s(*element->Attribute(x_attributeName));
	if(element == nullptr)
		throw MkException("Impossible to find node in ConfigReader", LOC);

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
void ConfigReader::SetAttribute(const string& x_attributeName, const string& x_value)
{
	if(IsEmpty())
		throw MkException("Impossible to find attribute " + x_attributeName + " in ConfigReader" , LOC);
	TiXmlElement* element = mp_node->ToElement();
	if(element == nullptr)
		throw MkException("Impossible to find attribute " + x_attributeName + " in ConfigReader" , LOC);

	element->SetAttribute(x_attributeName, x_value);
}

/**
* @brief Return the value (as string) for one element
*
* @return value
*/
string ConfigReader::GetValue() const
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

bool ConfigReader::IsFinal() const
{
	return mp_node->ToElement()->GetText() != nullptr;
}

/**
* @brief Set the value as string
*
* @param x_value Value to set
*/
void ConfigReader::SetValue(const string& x_value)
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
void ConfigFile::SaveToFile(const string& x_file) const
{
	if(!mp_doc->SaveFile(x_file))
		throw MkException("Error saving to file " + x_file, LOC);
}

/**
* @brief Validate the configuration
*/
void ConfigReader::Validate() const
{
	ConfigReader appConf = GetSubConfig("application");
	if(appConf.IsEmpty())
		throw MkException("Configuration must contain <application> subconfiguration", LOC);
	map<string,bool> moduleIds;
	map<string,bool> moduleNames;
	ConfigReader conf = appConf.GetSubConfig("module");

	while(!conf.IsEmpty())
	{
		string id = conf.GetAttribute("id", "");
		string name = conf.GetAttribute("name", "");
		if(id == "")
			throw MkException("Module " + name + " has no id", LOC);
		if(name == "")
			throw MkException("Module " + id + " has no name", LOC);
		if(moduleIds[id])
			throw MkException("Module with id=" + id + " must be unique", LOC);
		moduleIds[id] = true;
		if(moduleNames[name])
			throw MkException("Module with name=" + name + " must be unique", LOC);
		moduleNames[name] = true;

		conf.CheckUniquenessOfId("inputs",     "input",  "id",   name);
		conf.CheckUniquenessOfId("outputs",    "output", "id",   name);
		conf.CheckUniquenessOfId("parameters", "param",  "name", name);
		conf = conf.NextSubConfig("module");
	}
}

/**
* @brief Check that an id is unique: for validation purpose
*
* @param x_group      Group (e.g. inputs, parameters, ...)
* @param x_type       Type (e.g. input)
* @param x_idLabel    Label
* @param x_moduleName Name of the module
*/
void ConfigReader::CheckUniquenessOfId(const string& x_group, const string& x_type, const string& x_idLabel, const string& x_moduleName) const
{
	// Check that input streams are unique
	map<string,bool> ids;
	ConfigReader conf = GetSubConfig(x_group);
	if(conf.IsEmpty())
		return;
	conf = conf.GetSubConfig(x_type);
	while(!conf.IsEmpty())
	{
		string id = conf.GetAttribute(x_idLabel);
		if(ids[id])
			throw MkException(x_type + " with " + x_idLabel + "=" + id + " must be unique for module name=" + x_moduleName, LOC);
		ids[id] = true;
		conf = conf.NextSubConfig(x_type);
	}
}



void ConfigReader::overrideParameters(const ConfigReader& x_newConfig, ConfigReader x_oldConfig)
{
	// if(x_newConfig.GetSubConfig("parameters").IsEmpty())
	// return;
	for(const auto& conf2 : x_newConfig.GetSubConfig("parameters").FindAll("param"))
	{
		try
		{
			// cout << x_newConfig.GetAttribute("name") << ":" << conf2.GetAttribute("name") << endl;
			if(x_oldConfig.IsEmpty())
			{
				LOG_WARN(m_logger, "Module " << x_newConfig.GetAttribute("name") << " cannot be overriden since it does not exist in the current config");
				continue;
			}
			// Override parameter
			LOG_DEBUG(m_logger, "Override parameter " << conf2.GetAttribute("name") << " with value " << conf2.GetValue());
			x_oldConfig.RefSubConfig("parameters").RefSubConfig("param", "name", conf2.GetAttribute("name"), true)
			.SetValue(conf2.GetValue());
		}
		catch(MkException& e)
		{
			LOG_WARN(m_logger, "Cannot read parameters from extra config: "<<e.what());
		}
	}
}

/**
* @brief Apply extra XML config to modify the initial config (used with option -x)
*
* @param xr_extraConfig Extra config to use for overriding
*
* @return
*/

void ConfigReader::OverrideWith(const ConfigReader& x_extraConfig)
{
	// Note: This method is very specific to our type of configuration
	overrideParameters(x_extraConfig.GetSubConfig("application"), RefSubConfig("application"));

	for(const auto& conf1 : x_extraConfig.GetSubConfig("application").FindAll("module"))
	{
		overrideParameters(conf1, RefSubConfig("application").RefSubConfig("module", "name", conf1.GetAttribute("name")));
	}
}

/**
* @brief Find a sub config (with a similar syntax as JQuery)
*
* @param  x_searchString The search path with jquery-like syntax
* @param  x_fatal        All exceptions are fatal: close program
* @return value
*/
const ConfigReader ConfigReader::Find(const string& x_searchString, bool x_fatal) const
{
	try
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
	catch(...)
	{
		if(x_fatal)
			fatal("Fatal exception while finding target " + x_searchString, LOC);
		else throw;
	}
	// return something to avoid compilation warnings
	return *this;
}

/**
* @brief Find a sub config (with a similar syntax as JQuery)
*
* @param  x_searchString The search path with jquery-like syntax
* @return value
*/
ConfigReader ConfigReader::FindRef(const string& x_searchString, bool x_allowCreation, bool x_fatal)
{
	try
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
	catch(...)
	{
		if(x_fatal)
			fatal("Fatal exception while finding target " + x_searchString, LOC);
		else throw;
	}
	// return something to avoid compilation warnings
	return *this;
}


/**
* @brief Find all sub configs (with a similar syntax as JQuery)
*
* @param  x_searchString The search path with jquery-like syntax
* @param  x_fatal        All exceptions are fatal: close program
* @return A vector of configurations
*/
vector<ConfigReader> ConfigReader::FindAll(const string& x_searchString, bool x_fatal) const
{
	vector<ConfigReader> results;

	try
	{
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
			ConfigReader conf = GetSubConfig(tagName, attrName, attrValue);
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
	catch(...)
	{
		if(x_fatal)
			fatal("Fatal exception while finding target " + x_searchString, LOC);
		else throw;
	}
	// return something to avoid compilation warnings
	return results;
}
