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

#include "MkException.h"
#include "util.h"
#include "simulation.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

using namespace std;

/// @brief: This file contains all the functions needed to prepare an optimization and variate parameters of the config

/// Add targets lines for inclusion in Makefile
void addSimulationEntry(const vector<string>& x_variationNames, const string& x_outputDir, const ConfigReader& x_mainConfig, ostream& xr_allTargets, ostream& xr_targets, int& xr_cpt)
{
	// Generate entries for makefile
	stringstream sd;
	// sd << "simul" << setfill('0') << setw(6) << xr_cpt;
	string name = join(x_variationNames, '_');
	cout << "Add entry for variation " << name << endl;
	sd << name;
	string arguments;
	try
	{
		arguments = x_mainConfig.GetSubConfig("application").GetSubConfig("parameters").GetSubConfig("param", "arguments").GetValue();
	}
	catch(MkException &e){}
	xr_allTargets << "$(OUTDIR)/results/" <<  sd.str() << " ";
	xr_targets << "$(OUTDIR)/results/" << sd.str() << ":" << endl;
	// xr_targets << "\t" << "mkdir -p $(OUTDIR)/results/"  << sd.str() << endl;
	xr_targets << "\t" << "rm -rf $(OUTDIR)/running/"  << sd.str() << 
		" && cp -r $(OUTDIR)/ready/" << sd.str() << " $(OUTDIR)/running/" << sd.str() << endl;
	xr_targets << "\t" << "$(EXE) $(PARAMS) $(OUTDIR)/running/" << sd.str() << "/" << name << ".xml -o $(OUTDIR)/running/" << sd.str() <<
		" " << arguments << endl;
	xr_targets << "\t" << "mv $(OUTDIR)/running/" << sd.str() << " $(OUTDIR)/results/" << endl;
	xr_targets << endl;

	// Create ready/... directory that describes the simulation
	stringstream subdir;
	subdir << x_outputDir << "/ready/" << sd.str();
	stringstream xmlProjName;
	xmlProjName << subdir.str() << "/" << name << ".xml";
	SYSTEM("mkdir -p " + subdir.str());
	x_mainConfig.SaveToFile(xmlProjName.str());

	// Last but not least:
	// Register the different variations for summaries
	// This will allow to aggregate the results
	for(vector<string>::const_iterator it = x_variationNames.begin() ; it != x_variationNames.end() ; it++)
	{
		string fileName = x_outputDir + "/" + *it + ".txt";
		ofstream ofs(fileName.c_str(), ios_base::app);
		ofs << name << endl;
		ofs.close();
	}

	xr_cpt++;
}

/// Add variation to simulation
void addVariations(vector<string>& xr_variationNames, Manager& xr_manager, const ConfigReader& x_varConf, const string& x_outputDir, ConfigReader& xr_mainConfig, ostream& xr_allTargets, ostream& xr_targets, int& xr_cpt)
{
	ConfigReader varConf = x_varConf;
	while(! varConf.IsEmpty())
	{
		// Retrieve args from config
		vector<string> moduleNames;
		vector<string> paramNames;
		try
		{
			split(varConf.GetAttribute("modules"), ',', moduleNames);
		}
		catch(MkException& e)
		{}
		if(moduleNames.empty())
			moduleNames.push_back(varConf.GetAttribute("module"));

		try
		{
			split(varConf.GetAttribute("parameters"), ',', paramNames);
		}
		catch(MkException& e)
		{}
		if(paramNames.empty())
			paramNames.push_back(varConf.GetAttribute("param"));
		if(moduleNames.size() != 1 && moduleNames.size() != paramNames.size())
			throw MkException("Modules and parameters must have the same size in <var> or modules must only contain one module", LOC);

		// Get all targets to be varied in config
		vector<ConfigReader*> targets;
		targets.resize(paramNames.size());
		vector<string> originalValues;
		originalValues.resize(paramNames.size());
		auto ittar = targets.begin();
		auto itmod = moduleNames.begin();
		auto itval = originalValues.begin();
		for(string itpar : paramNames)
		{
			cout << "param:"<< *itmod << ":" << itpar << endl;
			if(*itmod == "manager")
				*ittar = new ConfigReader(xr_mainConfig.RefSubConfig("application").RefSubConfig("parameters", "", true).RefSubConfig("param", itpar, true));
			else
				*ittar = new ConfigReader(xr_mainConfig.RefSubConfig("application").RefSubConfig("module", *itmod).RefSubConfig("parameters", "", true).RefSubConfig("param", itpar, true));
			*itval = (*ittar)->GetValue();
			ittar++;
			itval++;
			if(moduleNames.size() > 1)
				itmod++;
		}


		string file;
		try
		{
			file = varConf.GetAttribute("file");
		}
		catch(MkException& e)
		{}

		if(file != "")
		{
			// set values of parameters by using a JSON file
			SYSTEM("cp " + file + " " + x_outputDir);
			ifstream ifs;
			ifs.open(file);
			Json::Value root;
			ifs >> root;
			Json::Value::Members members1 = root.getMemberNames();
			for(Json::Value::Members::const_iterator it1 = members1.begin() ; it1 != members1.end() ; ++it1)
			{
				if(!root[*it1].isArray())
					throw MkException("Range " + *it1 +  " in JSON must contain an array", LOC);
				if(root[*it1].size() != targets.size())
					throw MkException("Range " + *it1 +  " in JSON must have the same size as parameters to variate", LOC);

				for(unsigned int i = 0 ; i < root[*it1].size() ; i++)
				{
					Json::Value jvalue = root[*it1][i];
					// cout<<value.asString()<<endl;
					try
					{
						targets.at(i)->SetValue(jvalue.asString());
					}
					catch(...)
					{
						stringstream ss1;
						ss1 << jvalue;
						targets.at(i)->SetValue(ss1.str());
					}
				}

				// add a name
				// TODO: in the future maybe use a hash
				xr_variationNames.push_back(*it1);

				// Change value of param
				ConfigReader subConf = varConf.GetSubConfig("var");
				if(subConf.IsEmpty())
					addSimulationEntry(xr_variationNames, x_outputDir, xr_mainConfig, xr_allTargets, xr_targets, xr_cpt);
				else
					addVariations(xr_variationNames, xr_manager, subConf, x_outputDir, xr_mainConfig, xr_allTargets, xr_targets, xr_cpt);
				xr_variationNames.pop_back();
			}
			ifs.close();
		}
		else
		{
			// Set values by using range information provided in XML
			if(paramNames.size() > 1)
				throw MkException("To set more than one parameter variation, use an external file with option file=...", LOC);
			// default values. Empty range means that the prog uses the default range of the param
			string range = "";
			int nb       = 100;
			try
			{
				range  = varConf.GetAttribute("range");
			}
			catch(MkException& e){}
			try
			{
				nb = atof(varConf.GetAttribute("nb").c_str());
			}
			catch(MkException& e){}

cout<<moduleNames.at(0)<<endl;
			const Parameter& param = xr_manager.GetModuleByName(moduleNames.at(0)).GetParameters().GetParameterByName(paramNames.at(0));
			cout<<"asdfasf"<<endl;
			vector<string> values;
			param.GenerateValues(nb, values, range);

			// Generate a config for each variation
			for(vector<string>::const_iterator it = values.begin() ; it != values.end() ; it++)
			{
				xr_variationNames.push_back(paramNames.at(0) + "-" + *it);

				// Change value of param
				targets.at(0)->SetValue(*it);
				ConfigReader subConf = varConf.GetSubConfig("var");
				if(subConf.IsEmpty())
					addSimulationEntry(xr_variationNames, x_outputDir, xr_mainConfig, xr_allTargets, xr_targets, xr_cpt);
				else
					addVariations(xr_variationNames, xr_manager, subConf, x_outputDir, xr_mainConfig, xr_allTargets, xr_targets, xr_cpt);
				xr_variationNames.pop_back();
			}
		}

		// Set the target back to its original value and delete config obj
		itval = originalValues.begin();
		for(ConfigReader* target : targets)
		{
			target->SetValue(*itval);
			delete target;
			itval++;
		}
		varConf = varConf.NextSubConfig("var");
	}
}


/// Generate a simulation ready to be launched
bool generateSimulation(ConfigReader& mainConfig, const Context& context, log4cxx::LoggerPtr& logger)
{
	string outputDir = "simulation_" + timeStamp();
	SYSTEM("mkdir -p " + outputDir);
	SYSTEM("ln -sfn " + outputDir + " simulation_latest");
	mainConfig.SaveToFile("simulation_latest/Simulation.xml");
	SYSTEM("mkdir -p " + outputDir + "/ready");
	SYSTEM("mkdir -p " + outputDir + "/running");
	SYSTEM("mkdir -p " + outputDir + "/results");
	stringstream  allTargets;
	stringstream targets;

	Manager manager(mainConfig.GetSubConfig("application"));
	manager.SetContext(context);

	ConfigReader varConf = mainConfig.RefSubConfig("application")
	      .RefSubConfig("variations", "", true).GetSubConfig("var");

	int cpt = 0;
	vector<string> variationNames;
	addVariations(variationNames, manager, varConf, outputDir, mainConfig, allTargets, targets, cpt);

	// Generate a MakeFile for the simulation
	string makefile = outputDir + "/simulation.make";
	ofstream of(makefile.c_str());

	// generate all: ...
	of << "# Makefile for Markus simulation" << endl << endl;
	of << "OUTDIR :=" << outputDir << endl;
	of << "EXE :=./markus" << endl;
	of << "PARAMS :=-ncf" << endl;
	of << endl;
	of << "all: ";
	of << allTargets.rdbuf();
	of << endl << endl;
	// generate each target
	of << targets.rdbuf();
	of << endl;
	of.close();
	
	LOG_INFO(logger, "Simulation generated in directory " << outputDir);
	LOG_INFO(logger, "Launch with: make -f " << makefile << " -j4");
	return 1;
}
