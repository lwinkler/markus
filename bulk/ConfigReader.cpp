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


/**
* @brief Save the parameters values to the config object, ready to be written to disk
*/
void Configurable::UpdateConfig()
{
	GetParameters().UpdateConfig();
}

/**
* @brief Constructor
*
* @param x_fileName      Name of the XML file with relative path
* @param x_allowCreation Allow the creation of a new file if unexistant
* @param x_fatal         Errors cause the program to exit (to avoid throwing exceptions in constructors)
*/
ConfigReader::ConfigReader(const string& x_fileName, bool x_allowCreation)
{
	try
	{
		m_isOriginal = true;
		mp_doc = NULL; // Initialize to null as there can be an error in construction
		mp_doc = new TiXmlDocument(x_fileName);
		if (! mp_doc->LoadFile())
		{
			delete mp_doc;
			if(x_allowCreation)
			{
				createEmptyConfigFile(x_fileName);
				mp_doc = new TiXmlDocument(x_fileName);
				assert(mp_doc->LoadFile());
			}
			else throw MkException("Could not load file as XML '" + x_fileName + "'. Error='" + mp_doc->ErrorDesc() + "'. Exiting.", LOC);
		}
		mp_node = mp_doc;
	}
	catch(...)
	{
		fatal("Fatal exception in constructor of ConfigReader", LOC);
	}
}


/**
* @brief Constructor for a class based on a sub-node (used internally)
*
* @param xp_node
*/
ConfigReader::ConfigReader(TiXmlNode * xp_node)
{
	m_isOriginal = false;
	mp_doc = NULL;
	mp_node = xp_node;
}

ConfigReader::ConfigReader(const ConfigReader& x_conf)
{
	m_isOriginal = false;
	mp_doc       = x_conf.mp_doc;
	mp_node      = x_conf.mp_node;
}

const ConfigReader& ConfigReader::operator = (const ConfigReader& x_conf)
{
	m_isOriginal = false;
	mp_doc       = x_conf.mp_doc;
	mp_node      = x_conf.mp_node;
	return *this;
}

ConfigReader::~ConfigReader()
{
	if(m_isOriginal)
		CLEAN_DELETE(mp_doc);
	mp_node = NULL;
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
	
	if(newNode == NULL && x_allowCreation)
	{ 
		// Add a sub config element if not found
		TiXmlElement* element = new TiXmlElement(x_tagName);
		mp_node->LinkEndChild(element);
		return ConfigReader(element);
	}
	return ConfigReader(newNode);
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
	
	while(newNode != NULL && newNode->ToElement()->Attribute(x_attrName.c_str()) != x_attrValue)
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
	
	const char* name = NULL;
	while(newNode != NULL && ((name = newNode->ToElement()->Attribute("name")) == NULL || x_attrValue != name))
	{
		newNode = newNode->NextSibling(x_tagName);
	}
	if(newNode == NULL && x_allowCreation)
	{ 
		// Add a sub config element if not found
		TiXmlElement* element = new TiXmlElement(x_tagName);
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
		while(newNode != NULL && x_attrValue != newNode->ToElement()->Attribute(x_attrName.c_str()))
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
	if(element == NULL)
		throw MkException("Impossible to find node in ConfigReader", LOC);

	const string * str = element->Attribute(x_attributeName);
	if(str == NULL)
		return ""; // TODO: throw exception and fix the rest of the code
		// throw MkException("Attribute is unexistant", LOC);
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
	if(element == NULL)
		throw MkException("Impossible to find node in ConfigReader", LOC);

	const string * str = element->Attribute(x_attributeName);
	if(str == NULL)
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
	if(element == NULL)
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
	if(str == NULL)
		return "";
	else
		return str;
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
void ConfigReader::SaveToFile(const string& x_file) const
{
	if(!mp_doc)
		throw MkException("Can only save global config to file", LOC);
	if(!mp_doc->SaveFile(x_file))
		throw MkException("Error saving to file ", LOC);
}

/**
* @brief Validate that the configuration is valid
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
		string id = conf.GetAttribute("id");
		string name = conf.GetAttribute("name");
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
		if(id == "")
			throw MkException(x_type + " of module name=" + x_moduleName + " has no " + x_idLabel, LOC);
		if(ids[id])
			throw MkException(x_type + " with " + x_idLabel + "=" + id + " must be unique for module name=" + x_moduleName, LOC);
		ids[id] = true;
		conf = conf.NextSubConfig(x_type);
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
	// TODO unit test
	// TODO this function should be more generic
	ConfigReader moduleConfig = x_extraConfig.GetSubConfig("application").GetSubConfig("module");
	while(!moduleConfig.IsEmpty())
	{
		if(!moduleConfig.GetSubConfig("parameters").IsEmpty())
		{
			ConfigReader paramConfig = moduleConfig.GetSubConfig("parameters").GetSubConfig("param");
			while(!paramConfig.IsEmpty())
			{
				// Override parameter
				RefSubConfig("module", "name", moduleConfig.GetAttribute("name"))
					.RefSubConfig("parameters").RefSubConfig("param", "name", paramConfig.GetAttribute("name"), true)
					.SetValue(paramConfig.GetValue());
				paramConfig = paramConfig.NextSubConfig("param");
			}
		}
		moduleConfig = moduleConfig.NextSubConfig("module");
	}
}

/**
* @brief Find a sub config (with a similar syntax as JQuery)
*
* @param  x_searchString The search path with jquery-like syntax
* @return value
*/
const ConfigReader ConfigReader::Find(const string& x_searchString, bool x_fatal) const
{
	try
	{
		// If empty return node: for recurrent function
		if(x_searchString.empty())
			return *this;

		size_t pos1 = x_searchString.find('>');
		ConfigReader conf1(*this);

		// split search string according to >
		string searchString1 = x_searchString;
		string searchString2;
		if(pos1 != string::npos)
		{
			searchString1 = x_searchString.substr(0, pos1);
			searchString2 = x_searchString.substr(pos1 + 1);
		}

		size_t pos2 = searchString1.find('[');
		if(pos2 == string::npos)
			return GetSubConfig(searchString1).Find(searchString2);

		// If we have a [...] part in search string
		size_t pos3 = searchString1.find("=\"", pos2);
		if(pos3 == string::npos)
			throw MkException("Expecting a '=\"'");

		size_t pos4 = searchString1.find("\"]", pos3);
		if(pos4 == string::npos)
			throw MkException("Expecting a '\"]'");

		// Search subconfigs for the right one
		string nodeName      = searchString1.substr(0, pos2);
		string attrName      = searchString1.substr(pos2 + 1, pos3 - pos2 - 1);
		string attrValue     = searchString1.substr(pos3 + 2, pos4 - pos3 - 2);

		return GetSubConfig(nodeName, attrName, attrValue).Find(searchString2);
	}
	catch(...)
	{
		if(x_fatal)
			fatal("Fatal exception while finding target " + x_searchString, LOC);
		else throw;
	}
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

		size_t pos1 = x_searchString.find('>');
		ConfigReader conf1(*this);

		// split search string according to >
		string searchString1 = x_searchString;
		string searchString2;
		if(pos1 != string::npos)
		{
			searchString1 = x_searchString.substr(0, pos1);
			searchString2 = x_searchString.substr(pos1 + 1);
		}

		size_t pos2 = searchString1.find('[');
		if(pos2 == string::npos)
			return RefSubConfig(searchString1, x_allowCreation).FindRef(searchString2, x_allowCreation);

		// If we have a [...] part in search string
		size_t pos3 = searchString1.find("=\"", pos2);
		if(pos3 == string::npos)
			throw MkException("Expecting a '=\"'");

		size_t pos4 = searchString1.find("\"]", pos3);
		if(pos4 == string::npos)
			throw MkException("Expecting a '\"]'");

		// Search subconfigs for the right one
		string nodeName      = searchString1.substr(0, pos2);
		string attrName      = searchString1.substr(pos2 + 1, pos3 - pos2 - 1);
		string attrValue     = searchString1.substr(pos3 + 2, pos4 - pos3 - 2);

		return RefSubConfig(nodeName, attrName, attrValue, x_allowCreation).FindRef(searchString2, x_allowCreation);
	}
	catch(...)
	{
		if(x_fatal)
			fatal("Fatal exception while finding target " + x_searchString, LOC);
		else throw;
	}
}
