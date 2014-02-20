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

using namespace std;

/// Reads the configuration file with tinyxml


/// Configurable: parent class for all configurable classes

/// Save the parameters values to the config object, ready to be written to disk
void Configurable::SaveConfig()
{
	GetParameters().SaveConfig();
}

/// Constructor : config based on a configuration file

ConfigReader::ConfigReader(const std::string& x_fileName)
{
	mp_doc = NULL; // Initialize to null as there can be an error in construction
	mp_doc = new TiXmlDocument(x_fileName);
	if (! mp_doc->LoadFile())
		throw MkException("Could not load test file '" + x_fileName + "'. Error='" + mp_doc->ErrorDesc() + "'. Exiting.", LOC);
	mp_node = mp_doc;
}

/// Constructor : config based on another config objects

ConfigReader::ConfigReader(TiXmlNode * xp_node)
{
	mp_doc = NULL;
	mp_node = xp_node;
}

ConfigReader::~ConfigReader()
{
	CLEAN_DELETE(mp_doc);
	mp_node = NULL;
}

/// Return a config objects that points to the sub element of configuration

ConfigReader ConfigReader::GetSubConfig(const std::string& x_objectType, string x_objectName) const
{
	if(IsEmpty())
		throw MkException("Impossible to find node in ConfigReader", LOC);
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

/// Return a config objects that points to the sub element of configuration (non-constant)

ConfigReader ConfigReader::RefSubConfig(const std::string& x_objectType, string x_objectName, bool x_allowCreation)
{
	if(IsEmpty())
		throw MkException("Impossible to find node in ConfigReader", LOC);

	TiXmlNode* newNode = mp_node->FirstChild(x_objectType);
	
	if(x_objectName.compare(""))
	{
		while(newNode != NULL && x_objectName.compare(newNode->ToElement()->Attribute("name")))
		{
			newNode = newNode->NextSibling(x_objectType);
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

/// Return a sub element that points to the next sub element of the configuration

ConfigReader ConfigReader::NextSubConfig(const std::string& x_objectType, string x_objectName) const
{
	if(IsEmpty())
		throw MkException("Impossible to find node in ConfigReader", LOC);
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

/// Return the attribute (as string) for one element

const string ConfigReader::GetAttribute(const std::string& x_attributeName) const
{
	if(IsEmpty())
		throw MkException("Impossible to find node in ConfigReader", LOC);
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

/// Set the attribute for one element

void ConfigReader::SetAttribute(const std::string& x_attributeName, string x_value)
{
	if(IsEmpty())
		throw MkException("Impossible to find node in ConfigReader", LOC);
	TiXmlElement* element = mp_node->ToElement();
	if(element == NULL)
		throw MkException("Impossible to find node in ConfigReader", LOC);

	element->SetAttribute("name", x_value);
}

/// Return the value (as string) for one element

const string ConfigReader::GetValue() const
{
	if(IsEmpty())
		throw MkException("Impossible to find node in ConfigReader", LOC);
	TiXmlElement* element = mp_node->ToElement();
	const char * str = element->GetText();
	if(str == NULL)
		return "";
	else
		return str;
}

/// Set the value as string
void ConfigReader::SetValue(const std::string& x_value)
{
	if(IsEmpty())
		throw MkException("Impossible to find node in ConfigReader", LOC);
	mp_node->Clear();
	mp_node->LinkEndChild(new TiXmlText(x_value)); //ToText();
}

/// Save the config as an xml file
void ConfigReader::SaveToFile(const std::string& x_file) const
{
	if(!mp_doc)
		throw MkException("Can only save global config to file", LOC);
	mp_doc->SaveFile(x_file);
}

/// Validate that the configuration is valid
void ConfigReader::Validate() const
{
	ConfigReader appConf = GetSubConfig("application");
	if(appConf.IsEmpty())
		throw MkException("Configuration must contain <application> subconfiguration", LOC);
	std::map<string,bool> moduleIds;
	std::map<string,bool> moduleNames;
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

/// Check that an id is unique: for validation purpose
void ConfigReader::CheckUniquenessOfId(const string& x_group, const string& x_type, const string& x_idLabel, const string& x_moduleName) const
{
	// Check that input streams are unique
	std::map<string,bool> ids;
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

