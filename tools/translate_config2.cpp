#include <iostream>
#include <map>
#include <boost/lexical_cast.hpp>
#include "ConfigReader.h"
#include "ConfigXml.h"
#include "util.h"
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

using namespace std;
using namespace boost;


map<int, string> modules;
map<string, string> moduleTypes;

// json serialize float as double and test

Json::Value translateElement(const Json::Value& x_in)
{
	Json::Value out = Json::arrayValue;
	for(const auto& elem : x_in.getMemberNames()) {
		Json::Value tmp;
		if(x_in[elem].isObject() && x_in[elem].isMember("connected"))
		{
			tmp = x_in[elem];
		}
		else
		{
			tmp["value"] = x_in[elem];
		}
		tmp["name"] = elem;

		out.append(tmp);
	}
	return out;
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
	cout << "Translate " << filename_old << " to " << filename_new << endl;
	Json::Value json1;
	Json::Value json2;
	readFromFile(json1, filename_old);

	if(json1.isMember("name"))
		json2["name"] = json1["name"];
	if(json1.isMember("description"))
		json2["description"] = json1["description"];

	cout << "Translate application " << json2["name"].asString() << endl;
	json2["modules"] = Json::arrayValue;

	for(const auto& name : json1["modules"].getMemberNames())
	{
		Json::Value tmp;
		tmp = json1["modules"][name];
		tmp["inputs"]  = translateElement(json1["modules"][name]["inputs"]);
		// tmp["outputs"] = translateElement(json1["modules"][name]["outputs"]);
		tmp["name"] = name;
		json2["modules"].append(tmp);
	}

	// Open json file
	writeToFile(json2, filename_new);
}

