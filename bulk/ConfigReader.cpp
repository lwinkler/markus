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
#include "Module.h"
#include "Manager.h"
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
*/
ConfigReader::ConfigReader(const string& x_fileName, bool x_allowCreation)
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
		else
			throw MkException("Could not load file as XML '" + x_fileName + "'. Error='" + mp_doc->ErrorDesc() + "'. Exiting.", LOC);
	}
	mp_node = mp_doc;
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

ConfigReader::~ConfigReader()
{
	if(m_isOriginal)
		CLEAN_DELETE(mp_doc);
	mp_node = NULL;
}

/**
* @brief Return a config objects that points to the sub element of configuration
*
* @param x_objectType The type of the sub element (= XML balise)
* @param x_objectName The name of the sub element (= attribute "name")
*
* @return config object
*/
ConfigReader ConfigReader::GetSubConfig(const string& x_objectType, string x_objectName) const
{
	if(IsEmpty())
		throw MkException("Impossible to find node " + x_objectType + " in ConfigReader with name \"" + x_objectName + "\"" , LOC);
	TiXmlNode* newNode = mp_node->FirstChild(x_objectType);
	
	if(x_objectName.compare(""))
	{
		while(newNode != NULL && x_objectName.compare(newNode->ToElement()->Attribute("name")))
		{
			newNode = newNode->NextSibling(x_objectType);
		}
	}

	return ConfigReader(newNode);
}

/**
* @brief Return a config objects that points to the sub element of configuration (non-constant)
*
* @param x_objectType The type of the sub element (= XML balise)
* @param x_objectName The name of the sub element (= attribute "name")
* @param x_allowCreation Allow to create the node in XML if inexistant
*
* @return config object
*/
ConfigReader ConfigReader::RefSubConfig(const string& x_objectType, string x_objectName, bool x_allowCreation)
{
	if(IsEmpty())
		throw MkException("Impossible to find node " + x_objectType + " in ConfigReader with name \"" + x_objectName + "\"" , LOC);

	TiXmlNode* newNode = mp_node->FirstChild(x_objectType);
	
	if(x_objectName.compare(""))
	{
		const char* name = NULL;
		while(newNode != NULL && ((name = newNode->ToElement()->Attribute("name")) == NULL || x_objectName.compare(name)))
		{
			newNode = newNode->NextSibling(x_objectType); // TODO: add breaks
		}
	}
	if(newNode == NULL && x_allowCreation)
	{ 
		// Add a sub config element if not found
		TiXmlElement* element = new TiXmlElement(x_objectType);
		if(x_objectName.size())
			element->SetAttribute("name", x_objectName);
		mp_node->LinkEndChild(element);
		return ConfigReader(element);
	}
	return ConfigReader(newNode);
}

/**
* @brief Return a sub element that points to the next sub element of the configuration
*
* @param x_objectType The type of the sub element (= XML balise)
* @param x_objectName The name of the sub element (= attribute "name")
*
* @return config object
*/
ConfigReader ConfigReader::NextSubConfig(const string& x_objectType, string x_objectName) const
{
	if(IsEmpty())
		throw MkException("Impossible to find node " + x_objectType + " in ConfigReader with name \"" + x_objectName + "\"" , LOC);
	TiXmlNode* newNode = mp_node->NextSibling(x_objectType);
	
	if(x_objectName.compare(""))
	{
		while(newNode != NULL && x_objectName.compare(newNode->ToElement()->Attribute("name")))
		{
			newNode = newNode->NextSibling(x_objectType);
		}
	}
	
	return ConfigReader(newNode);
}

/**
* @brief Return the attribute (as string) for one element
*
* @param x_attributeName Name of the attribute
*
* @return Config object
*/
const string ConfigReader::GetAttribute(const string& x_attributeName) const
{
	if(IsEmpty())
		throw MkException("Impossible to find attribute " + x_attributeName + " in ConfigReader" , LOC);
	TiXmlElement* element = mp_node->ToElement();
	//string s(*element->Attribute(x_attributeName));
	if(element == NULL)
		throw MkException("Impossible to find node in ConfigReader", LOC);

	const string * str = element->Attribute(x_attributeName);
	if(str == NULL)
		return "";
	else
		return *str;
}

/**
* @brief Set the attribute for one element
*
* @param x_attributeName Name of the attribute
* @param x_value         Value to set
*/
void ConfigReader::SetAttribute(const string& x_attributeName, string x_value)
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
const string ConfigReader::GetValue() const
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
	mp_doc->SaveFile(x_file);
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
				RefSubConfig("module", moduleConfig.GetAttribute("name"))
					.RefSubConfig("parameters").RefSubConfig("param", paramConfig.GetAttribute("name"), true)
					.SetValue(paramConfig.GetValue());
				paramConfig = paramConfig.NextSubConfig("param");
			}
		}
		moduleConfig = moduleConfig.NextSubConfig("module");
	}
}
