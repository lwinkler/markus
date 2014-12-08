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
#include "Simulation.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

using namespace std;

log4cxx::LoggerPtr Simulation::m_logger(log4cxx::Logger::getLogger("Simulation"));


/// Function to return either a module or the manager from a config
inline ConfigReader manOrMod(ConfigReader xr_mainConfig, const string& x_name)
{
	return x_name == "manager" ? xr_mainConfig.RefSubConfig("application") : xr_mainConfig.RefSubConfig("application").RefSubConfig("module", "name", x_name);
}


Simulation::Simulation(const ConfigReader& x_configReader, const Context& x_context) :
	Configurable(x_configReader),
	m_param(x_configReader),
	m_manager(x_configReader),
	m_outputDir("simulation_" + timeStamp())
{
	m_manager.SetContext(x_context);
}


/// Add targets lines for inclusion in Makefile
void Simulation::AddSimulationEntry(const vector<string>& x_variationNames, const ConfigReader& x_mainConfig)
{
	// Generate entries for makefile
	stringstream sd;
	// sd << "simul" << setfill('0') << setw(6) << xr_cpt;
	string name = join(x_variationNames, '_');
	LOG_DEBUG(m_logger, "Add entry for variation " << name);
	sd << name;
	string arguments;
	try
	{
		arguments = x_mainConfig.GetSubConfig("application").GetSubConfig("parameters").GetSubConfig("param", "name", "arguments").GetValue();
	}
	catch(MkException &e){}
	m_allTargets << "$(OUTDIR)/results/" <<  sd.str() << " ";
	m_targets << "$(OUTDIR)/results/" << sd.str() << ":" << endl;
	// xr_targets << "\t" << "mkdir -p $(OUTDIR)/results/"  << sd.str() << endl;
	m_targets << "\t" << "rm -rf $(OUTDIR)/running/"  << sd.str() << 
		" && cp -r $(OUTDIR)/ready/" << sd.str() << " $(OUTDIR)/running/" << sd.str() << endl;
	m_targets << "\t" << "$(EXE) $(PARAMS) $(OUTDIR)/running/" << sd.str() << "/" << name << ".xml -o $(OUTDIR)/running/" << sd.str() <<
		" " << arguments << endl;
	m_targets << "\t" << "mv $(OUTDIR)/running/" << sd.str() << " $(OUTDIR)/results/" << endl;
	m_targets << endl;

	// Create ready/... directory that describes the simulation
	stringstream subdir;
	subdir << m_outputDir << "/ready/" << sd.str();
	stringstream xmlProjName;
	xmlProjName << subdir.str() << "/" << name << ".xml";
	SYSTEM("mkdir -p " + subdir.str());
	x_mainConfig.SaveToFile(xmlProjName.str());

	// Last but not least:
	// Register the different variations for summaries in .txt files
	// This will allow to aggregate the results
	for(auto it : x_variationNames)
	{
		string fileName = m_outputDir + "/" + it + ".txt";
		ofstream ofs(fileName.c_str(), ios_base::app);
		ofs << name << endl;
		ofs.close();
	}

	m_cpt++;
}

/// Add variation to simulation
void Simulation::AddVariations(vector<string>& xr_variationNames, const ConfigReader& x_varConf, ConfigReader& xr_mainConfig)
{
	ConfigReader varConf = x_varConf;
	while(! varConf.IsEmpty())
	{
		// Read module and parameter attribute
		vector<string> moduleNames;
		split(varConf.GetAttribute("modules", ""), ',', moduleNames);
		if(moduleNames.empty())
			moduleNames.push_back(varConf.GetAttribute("module"));
		vector<string> paramNames;
		split(varConf.GetAttribute("parameters", ""), ',', paramNames);
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
			LOG_DEBUG(m_logger, "Param:"<< *itmod << ":" << itpar);
			*ittar = new ConfigReader(manOrMod(xr_mainConfig, *itmod).RefSubConfig("parameters", true).RefSubConfig("param", "name", itpar, true));
			*itval = (*ittar)->GetValue();
			ittar++;
			itval++;
			if(moduleNames.size() > 1)
				itmod++;
		}


		string file;
		file = varConf.GetAttribute("file", "");

		if(file != "")
		{
			// set values of parameters by using a JSON file
			SYSTEM("cp " + file + " " + m_outputDir);
			ifstream ifs;
			ifs.open(file);
			Json::Value root;
			ifs >> root;
			Json::Value::Members members1 = root.getMemberNames();
			for(Json::Value::Members::const_iterator it1 = members1.begin() ; it1 != members1.end() ; ++it1)
			{
				if(!root[*it1].isArray())
					throw MkException("Range " + *it1 + " in JSON must contain an array", LOC);
				if(root[*it1].size() != targets.size())
					throw MkException("Range " + *it1 + " in JSON must have the same size as parameters to variate", LOC);

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
					AddSimulationEntry(xr_variationNames, xr_mainConfig);
				else
					AddVariations(xr_variationNames, subConf, xr_mainConfig);
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
			string range = varConf.GetAttribute("range", "");
			int nb = atof(varConf.GetAttribute("nb", "10").c_str());

			LOG_DEBUG(m_logger, "Variations for module " << moduleNames.at(0));
			const Parameter& param = m_manager.GetModuleByName(moduleNames.at(0)).GetParameters().GetParameterByName(paramNames.at(0));
			vector<string> values;
			param.GenerateValues(nb, values, range);

			// Generate a config for each variation
			for(vector<string>::const_iterator it = values.begin() ; it != values.end() ; it++)
			{
				LOG_DEBUG(m_logger, "Value = " << *it);
				xr_variationNames.push_back(paramNames.at(0) + "-" + *it);

				// Change value of param
				targets.at(0)->SetValue(*it);
				ConfigReader subConf = varConf.GetSubConfig("var");
				if(subConf.IsEmpty())
					AddSimulationEntry(xr_variationNames, xr_mainConfig);
				else
					AddVariations(xr_variationNames, subConf, xr_mainConfig);
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
void Simulation::Generate(ConfigReader& mainConfig)
{
	SYSTEM("mkdir -p " + m_outputDir);
	SYSTEM("ln -sfn " + m_outputDir + " simulation_latest");
	mainConfig.SaveToFile("simulation_latest/Simulation.xml");
	SYSTEM("mkdir -p " + m_outputDir + "/ready");
	SYSTEM("mkdir -p " + m_outputDir + "/running");
	SYSTEM("mkdir -p " + m_outputDir + "/results");

	// Initialize members
	m_allTargets.str("");
	m_targets.str("");
	m_cpt = 0;

	ConfigReader varConf = m_configReader.GetSubConfig("variations").GetSubConfig("var");
	vector<string> variationNames;
	AddVariations(variationNames, varConf, mainConfig);

	// Generate a MakeFile for the simulation
	string makefile = m_outputDir + "/simulation.make";
	ofstream of(makefile.c_str());

	// generate all: ...
	of << "# Makefile for Markus simulation" << endl << endl;
	of << "OUTDIR :=" << m_outputDir << endl;
	of << "EXE :=./markus" << endl;
	of << "PARAMS :=-ncf" << endl;
	of << endl;
	of << "all: ";
	of << m_allTargets.rdbuf();
	of << endl << endl;
	// generate each target
	of << m_targets.rdbuf();
	of << endl;
	of.close();
	
	LOG_INFO(m_logger, m_cpt << " simulations generated in directory " << m_outputDir);
	LOG_INFO(m_logger, "Launch with: make -f " << makefile << " -j4");
}
