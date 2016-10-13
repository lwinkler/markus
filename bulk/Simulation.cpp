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
#include <boost/filesystem.hpp>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

using namespace std;
using namespace boost::filesystem;

log4cxx::LoggerPtr Simulation::m_logger(log4cxx::Logger::getLogger("Simulation"));

/// Function to return either a module or the manager from a config
inline ConfigReader manOrMod(ConfigReader xr_mainConfig, const string& x_name)
{
	return x_name == "manager" ? xr_mainConfig : xr_mainConfig["modules"][x_name];
}


Simulation::Simulation(Parameters& xr_params, Context& x_context) :
	Configurable(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params)),
	m_managerParams(m_param.config),
	m_manager(m_managerParams),
	m_outputDir("simulation_" + timeStamp())
{
	m_manager.SetContext(x_context);
	m_cpt = 0;
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
		arguments = x_mainConfig["inputs"]["arguments"].asString();
	}
	catch(MkException &e) {}
	m_allTargets << "$(OUTDIR)/results/" <<  sd.str() << " ";
	m_targets << "$(OUTDIR)/results/" << sd.str() << ":" << endl;
	// xr_targets << "\t" << "mkdir -p $(OUTDIR)/results/"  << sd.str() << endl;
	m_targets << "\t" << "rm -rf $(OUTDIR)/running/"  << sd.str() << endl;
	m_targets << "\t" << "$(EXE) $(PARAMS) $(OUTDIR)/ready/" << sd.str() << "/" << name << ".xml -o $(OUTDIR)/running/" << sd.str() <<
			  " " << arguments << endl;
	m_targets << "\t" << "mv $(OUTDIR)/running/" << sd.str() << " $(OUTDIR)/results/" << endl;
	m_targets << endl;

	// Create ready/... directory that describes the simulation
	stringstream subdir;
	subdir << m_outputDir << "/ready/" << sd.str();
	stringstream xmlProjName;
	xmlProjName << subdir.str() << "/" << name << ".xml";
	create_directory(subdir.str());
	writeToFile(x_mainConfig, xmlProjName.str());

	// Last but not least:
	// Register the different variations for summaries in .txt files
	// This will allow to aggregate the results
	for(const auto& it : x_variationNames)
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
	for(const auto& varConf : x_varConf)
	{
		// Read module and parameter attribute
		assert(varConf["module"].isNull()); // note: no longer supported
		assert(varConf["param"].isNull());  // note: no longer supported
		const auto& paramNames(varConf["inputs"]);
		const auto& moduleNames(varConf["modules"]);
		if(moduleNames.size() != 1 && moduleNames.size() != paramNames.size())
			throw MkException("Modules and parameters must have the same size in <var> or modules must only contain one module", LOC);

		// Get all targets to be varied in config
		vector<ConfigReader> targets;
		targets.resize(paramNames.size());
		vector<ConfigReader> originalValues;
		originalValues.resize(paramNames.size());
		auto ittar = targets.begin();
		auto itmod = moduleNames.begin();
		auto itval = originalValues.begin();
		for(const auto& itpar : paramNames)
		{
			try
			{
				LOG_DEBUG(m_logger, "Param:"<< itmod->asString() << ":" << itpar.asString());
				*ittar = manOrMod(xr_mainConfig, itmod->asString())["inputs"][itpar.asString()];
				*itval = ittar->asString();
				ittar++;
				itval++;
				if(moduleNames.size() > 1)
					itmod++;
			}
			catch(exception &e)
			{
				throw MkException("Cannot variate parameter " + itpar.asString() + " of module " + itmod->asString() + ": " + e.what(), LOC);
			}
		}


		string file = varConf["file"].asString();

		if(file != "")
		{
			// Reference to the value to use (in json file)
			const auto& refNames = varConf["references"];
			if(refNames.size() != paramNames.size())
				throw MkException("References must have the same sizes as parameters", LOC);

			// set values of parameters by using a JSON file
			copy(file, m_outputDir + "/" + basename(file));
			ifstream ifs;
			ifs.open(file);
			Json::Value root;
			ifs >> root;

			// For each set of parameters
			for(const auto& elem : root.getMemberNames())
			{
				int i = 0;
				for(const auto& ref : refNames)
				{
					if(root[elem][ref.asString()].isNull())
						throw MkException("No tag " + ref.asString() + " for " + elem, LOC);

					Json::Value jvalue = root[elem][ref.asString()];
					// cout<<value.asString()<<endl;
					try
					{
						targets.at(i) = jvalue.asString();
					}
					catch(...)
					{
						stringstream ss1;
						ss1 << jvalue;
						targets.at(i) = ss1.str();
					}
					i++;
				}

				// add a name
				// note: in the future maybe use a hash
				xr_variationNames.push_back(elem);

				// Change value of param
				ConfigReader subConf = varConf["var"];
				if(subConf.isNull())
					AddSimulationEntry(xr_variationNames, xr_mainConfig);
				else
					AddVariations(xr_variationNames, varConf, xr_mainConfig);
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
			string range = varConf["range"].asString();
			int nb = varConf.get("nb", 10).asInt();

			LOG_DEBUG(m_logger, "Variations for module " << moduleNames[0].asString());
			const Parameter& param = m_manager.GetModuleByName(moduleNames[0].asString()).GetParameters().GetParameterByName(paramNames[0].asString());
			vector<string> values;
			param.GenerateValues(nb, values, range);

			// Generate a config for each variation
			for(const auto& elem : values)
			{
				LOG_DEBUG(m_logger, "Value = " << elem);
				xr_variationNames.push_back(paramNames[0].asString() + "-" + elem);

				// Change value of param
				targets.at(0) = elem;
				ConfigReader subConf = varConf["var"];
				if(subConf.isNull())
					AddSimulationEntry(xr_variationNames, xr_mainConfig);
				else
					AddVariations(xr_variationNames, varConf, xr_mainConfig);
				xr_variationNames.pop_back();
			}
		}

		// Set the target back to its original value and delete config obj
		itval = originalValues.begin();
		for(auto& target : targets)
		{
			target = *itval;
			itval++;
		}
	}
}


/// Generate a simulation ready to be launched
void Simulation::Generate(ConfigReader& mainConfig)
{
	create_directory(m_outputDir);

	remove("simulation_latest");
	create_symlink(m_outputDir, "simulation_latest");
	writeToFile(mainConfig, "simulation_latest/Simulation.xml");
	create_directory(m_outputDir + "/ready");
	create_directory(m_outputDir + "/running");
	create_directory(m_outputDir + "/results");

	// Initialize members
	m_allTargets.str("");
	m_targets.str("");
	m_cpt = 0;

	vector<string> variationNames;
	AddVariations(variationNames, m_param.config["variations"], mainConfig);

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
