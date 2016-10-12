#include <iostream>
#include <boost/lexical_cast.hpp>
#include "ConfigReader.h"
#include "ConfigXml.h"

using namespace std;
using namespace boost;

void translateElement(const ConfigXml x_xml, ConfigReader& xr_json)
{
	string field = x_xml.GetAttribute("id", "");
	if(!field.empty())
		xr_json["id"] = lexical_cast<int>(field);

	field = x_xml.GetAttribute("moduleid", "");
	if(!field.empty())
		xr_json["moduleid"] = lexical_cast<int>(field);

	field = x_xml.GetAttribute("outputid", "");
	if(!field.empty())
		xr_json["outputid"] = lexical_cast<int>(field);

	field = x_xml.GetValue();
	if(!field.empty())
	{
		try
		{
			xr_json["value"] = lexical_cast<double>(field);
		}
		catch(...)
		{
			xr_json["value"] = field;
		}
	}
}

void translateModule(const ConfigXml x_xml, ConfigReader& xr_json)
{
	for(const auto& xml : x_xml.FindAll("parameters>param"))
	{
		string name = xml.GetAttribute("name");
		// note: parameters are now inputs
		translateElement(xml, xr_json["inputs"][name]);
	}
	for(const auto& xml : x_xml.FindAll("inputs>input"))
	{
		string name = xml.GetAttribute("name");
		if(xr_json["inputs"].isMember(name))
		{
			cerr << "There cannot be 2 inputs of parameters with the same id: " << name << endl;
			exit(-1);
		}
		translateElement(xml, xr_json["inputs"][name]);
	}
	for(const auto& xml : x_xml.FindAll("outputs>param"))
	{
		string name = xml.GetAttribute("name");
		translateElement(xml, xr_json["outputs"][name]);
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

