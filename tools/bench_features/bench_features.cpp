#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/reader.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"

#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>




#define SAMPLES 10000
#define TIMES   100

/* srand example */
#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <sys/time.h>
#include <vector>

using namespace std;
using namespace rapidjson;
using boost::property_tree::ptree;


// Global vars
vector<float> data;

ptree pt;

Json::Value root;

Document document;
// Value value(kObjectType);


void benchmark(void (*x_myFunc)(), const string& x_label)
{
	struct timeval timeStart,
			timeEnd;
	gettimeofday(&timeStart, NULL);

	(*x_myFunc)();

	gettimeofday(&timeEnd, NULL);

	std::cout
			<< ((timeEnd.tv_sec - timeStart.tv_sec) * 1000000 + timeEnd.tv_usec - timeStart.tv_usec)
			<< " us to execute "<<x_label
			<< std::endl;
}

// Generate data
void generateData()
{
	unsigned int seed = 223423444;
	double sum = 0;
	// Generation of data
	for(int i = 0 ; i < SAMPLES ; i++)
	{
		data.push_back(static_cast<float>(rand_r(&seed)) / RAND_MAX);
		sum += data.back();
	}
	cout<<"Sum = "<<sum<<endl;
}

// Property tree
void loadPropertyTree()
{
	// Generation of data for property tree
	for(int i = 0 ; i < SAMPLES ; i++)
	{
		char str[32];
		sprintf(str, "feat%d",i);
		pt.put<float>(str, data[i]);
	}
}

void sumPropertyTree()
{
	double sum = 0;
	for(int i = 0 ; i < SAMPLES ; i++)
	{
		char str[32];
		sprintf(str, "feat%d",i);
		sum += pt.get<float>(str);
	}
	cout<<"Sum = "<<sum<<endl;
}

void writePropertyTree()
{
	// Prepare writer and output stream.
	ofstream of("proptree.json");
	write_json(of, pt);
}

// RapidJSON
void loadRapidJSON()
{
	char json[32] = "{  }";
	assert(!document.Parse<0>(json).HasParseError());
	// rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

	// Generation of data for property tree
	for(int i = 0 ; i < SAMPLES ; i++)
	{
		char str[64];
		sprintf(str, "feat%d",i);
		Value val;
		val.SetDouble(data[i]);
		Value name;
		name.SetString(str);
		document.AddMember(name, val, document.GetAllocator());
	}
}

void sumRapidJSON()
{
	double sum = 0;
	for(int i = 0 ; i < SAMPLES ; i++)
	{
		char str[32];
		sprintf(str, "feat%d",i);
		sum += document[str].GetDouble();
	}
	cout<<"Sum = "<<sum<<endl;
}

void writeRapidJSON()
{
	// Prepare writer and output stream.
	char writeBuffer[65536];
	FILE* pFile = fopen("rapid.json", "w");
	FileWriteStream os(pFile, writeBuffer, sizeof(writeBuffer));
	PrettyWriter<FileWriteStream> writer(os);

	document.Accept(writer);
	fclose(pFile);
}


void loadJsonCpp()
{
	for(int i = 0 ; i < SAMPLES ; i++)
	{
		char str[32];
		sprintf(str, "feat%d",i);
		root[str] = data[i];
	}
}

void sumJsonCpp()
{
	double sum = 0;
	for(int i = 0 ; i < SAMPLES ; i++)
	{
		char str[32];
		sprintf(str, "feat%d",i);
		sum += root[str].asFloat();
	}
	cout<<"Sum = "<<sum<<endl;
}


void writeJsonCpp()
{
	Json::FastWriter writer;
	ofstream of("jsoncpp.json");
	string tmp = writer.write(root);
	of << tmp;
}


int main()
{
	benchmark(generateData, "Generation of data");

	benchmark(loadPropertyTree, "Property tree load");
	benchmark(sumPropertyTree, "Property tree sum");
	benchmark(writePropertyTree, "Property tree write");


	benchmark(loadRapidJSON, "RapidJSON load");
	// benchmark(sumRapidJSON, "RapidJSON sum");
	benchmark(writeRapidJSON, "RapidJSON write");

	benchmark(loadJsonCpp, "JsonCpp load");
	benchmark(sumJsonCpp, "JsonCpp sum");
	benchmark(writeJsonCpp, "JsonCpp write");



}
