#include <iostream>
#include <map>
#include <boost/lexical_cast.hpp>
#include "ConfigReader.h"
#include "ConfigXml.h"
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

using namespace std;
using namespace boost;


map<int, string> modules;
map<string, string> moduleTypes;

string camelCase(const string& str)
{
	string s = str;
	string res;
	for(uint i = 0 ; i < s.size() ; i++)
	{
		if(s[i] == '_')
		{
			assert(i < s.size() - 1);
			s[i+1]-= ('a' - 'A');
		}
		else
			res.push_back(s[i]);
	}
	return res;
}

string getInputNameFromExport(const string& module, int id, bool isInput)
{
	ConfigReader json;
	readFromFile(json, "editor/modules/" + module + ".json");
	auto inp = json[isInput ? "inputs" : "outputs"];
	if(!inp.isObject())
	{
		cout << module << ":" << inp.type() << endl;
		stringstream ss;
		ss << inp;
		cout << ss.str() << endl;
	}
	for(const auto& name : inp.getMemberNames())
	{
		if(inp[name]["id"] == id)
			return name;
	}
	cerr << "Found no " << (isInput ? "input" : "output") << " id=" << id << " for " << module << endl;
	exit(-1);
}

string getModuleType(const ConfigXml& conf, const string& moduleName)
{
	for(const auto& conf2 : conf.FindAll("parameters>param"))
	{
		if(conf2.GetAttribute("name") == "class")
			return conf2.GetValue();
	}
	cerr << "Found no class for module " << moduleName << endl;
	exit(-1);
}

// json serialize float as double and test

void translateElement(const ConfigXml x_xml, ConfigReader& xr_json)
{
	string field = x_xml.GetAttribute("id", "");
	// if(!field.empty())
		// xr_json["id"] = lexical_cast<int>(field);

	field = x_xml.GetAttribute("moduleid", "");
	if(!field.empty())
	{
		string moduleName = modules[lexical_cast<int>(field)];
		string moduleType = moduleTypes[moduleName];
		xr_json["connected"]["module"] = moduleName;
		xr_json["connected"]["output"] = getInputNameFromExport(moduleType, lexical_cast<int>(x_xml.GetAttribute("outputid")), false);
	}

	field = x_xml.GetValue();
	if(!field.empty())
	{
		try
		{
			xr_json = lexical_cast<double>(field);
		}
		catch(...)
		{
			xr_json = field;
		}
	}
}

void translateModule(const ConfigXml x_xml, ConfigReader& xr_json)
{
	string modName  = x_xml.GetAttribute("name");
	string modClass = moduleTypes[modName];
	// xr_json["name"] = modName;
	xr_json["inputs"] = Json::objectValue;
	xr_json["outputs"] = Json::objectValue;
	for(const auto& xml : x_xml.FindAll("parameters>param"))
	{
		string name = camelCase(xml.GetAttribute("name"));
		if(name == "class")
		{
			xr_json["class"] = xml.GetValue();
			continue;
		}
		// note: parameters are now inputs
		translateElement(xml, xr_json["inputs"][name]);
		if(xr_json["inputs"][name].isNull())
			xr_json["inputs"].removeMember(name);
	}
	for(const auto& xml : x_xml.FindAll("inputs>input"))
	{
		string name = camelCase(getInputNameFromExport(modClass, lexical_cast<int>(xml.GetAttribute("id")), true));
		if(name == "class")
			continue;
		if(xr_json["inputs"].isMember(name))
		{
			cerr << "There cannot be 2 inputs or parameters with the same name: " << name << endl;
			exit(-1);
		}
		translateElement(xml, xr_json["inputs"][name]);
		if(xr_json["inputs"][name].isNull())
			xr_json["inputs"].removeMember(name);
	}
	for(const auto& xml : x_xml.FindAll("outputs>output"))
	{
		string name = camelCase(getInputNameFromExport(modClass, lexical_cast<int>(xml.GetAttribute("id")), false));
		if(xr_json["outputs"].isMember(name))
		{
			cerr << "There cannot be 2 outputs with the same name: " << name << endl;
			exit(-1);
		}
		translateElement(xml, xr_json["outputs"][name]);
		if(xr_json["outputs"][name].isNull())
			xr_json["outputs"].removeMember(name);
	}
}


int main(int argc, char** argv)
{
	if(argc != 3)
	{
		cerr << "usage: " << argv[0] << " old_config.xml new_config.json" << endl;
		return -1;
	}

	// Open xml config
	const string filename_old(argv[1]);
	const string filename_new(argv[2]);
	ConfigFileXml file_old(filename_old);
	ConfigXml     conf(file_old.GetSubConfig("application"));
	ConfigReader json;

	json["name"]        = conf.GetAttribute("name", "(unk)");
	json["description"] = conf.GetAttribute("description", "(unk)");

	cout << "Translate application " << json["name"].asString() << endl;

	for(const auto& xml : conf.FindAll("module"))
	{
		int id = lexical_cast<int>(xml.GetAttribute("id"));
		string name = xml.GetAttribute("name");
		modules[id] = name;
		moduleTypes[name] = getModuleType(xml, name);
	}

	for(const auto& xml : conf.FindAll("module"))
	{
		string name = xml.GetAttribute("name");
		cout << "Translate module " << name << endl;
		translateModule(xml, json["modules"][name]);
	}

	ConfigXml confui = conf.GetSubConfig("uiobject");
	if(!confui.IsEmpty())
	{
		json["uiobject"]["x"] = lexical_cast<int>(confui.GetAttribute("x"));
		json["uiobject"]["y"] = lexical_cast<int>(confui.GetAttribute("y"));
		json["uiobject"]["width"] = lexical_cast<int>(confui.GetAttribute("width"));
		json["uiobject"]["height"] = lexical_cast<int>(confui.GetAttribute("height"));
	}


	// Open json file
	writeToFile(json, filename_new);
}

