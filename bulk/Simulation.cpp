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

#include "Simulation.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <boost/filesystem.hpp>
#include "MkException.h"
#include "util.h"

namespace mk {
using namespace std;
// using namespace boost::filesystem;
using boost::filesystem::create_directory;
using boost::filesystem::copy;
using boost::filesystem::create_symlink;

log4cxx::LoggerPtr Simulation::m_logger(log4cxx::Logger::getLogger("Simulation"));

/// Function to return either a module or the manager from a config
inline mkconf& manOrMod(mkconf& xr_mainConfig, const string& x_name)
{
	return x_name == "manager" ? xr_mainConfig : findFirstInArray(xr_mainConfig.at("modules"), "name", x_name);
}


Simulation::Simulation(Parameters& xr_params, Context& x_context) :
	Configurable(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params)),
	m_managerParams(m_param.config),
	m_manager(m_managerParams, x_context),
	m_outputDir("simulation_" + timeStamp())
{
	m_cpt = 0;
}


/// Add targets lines for inclusion in Makefile
void Simulation::AddSimulationEntry(const vector<string>& x_variationNames, const mkconf& x_mainConfig)
{
	// Generate entries for makefile
	stringstream sd;
	// sd << "simul" << setfill('0') << setw(6) << xr_cpt;
	string name = join(x_variationNames, '_');
	LOG_DEBUG(m_logger, "Add entry for variation " << name);
	sd << name;
	string arguments;
	if(exist(x_mainConfig, "inputs") && exist(x_mainConfig.at("inputs"), "arguments"))
		arguments = x_mainConfig.at("inputs").at("arguments").get<string>();

	m_allTargets << "$(OUTDIR)/results/" <<  sd.str() << " ";
	m_targets << "$(OUTDIR)/results/" << sd.str() << ":" << endl;
	// xr_targets << "\t" << "mkdir -p $(OUTDIR)/results/"  << sd.str() << endl;
	m_targets << "\t" << "rm -rf $(OUTDIR)/running/"  << sd.str() << endl;
	m_targets << "\t" << "$(EXE) $(PARAMS) $(OUTDIR)/ready/" << sd.str() << "/" << name << ".json -o $(OUTDIR)/running/" << sd.str() <<
			  " " << arguments << endl;
	m_targets << "\t" << "mv $(OUTDIR)/running/" << sd.str() << " $(OUTDIR)/results/" << endl;
	m_targets << endl;

	// Create ready/... directory that describes the simulation
	stringstream subdir;
	subdir << m_outputDir << "/ready/" << sd.str();
	stringstream jsonProjName;
	jsonProjName << subdir.str() << "/" << name << ".json";
	create_directory(subdir.str());
	writeToFile(x_mainConfig, jsonProjName.str());

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

const mkjson createArray(const string& x_namePlural, const string& x_nameSingular, const mkjson& varConf)
{
	if(exist(varConf, x_namePlural))
		return varConf.at(x_namePlural);
	mkjson val(mkjson::array());
	if(exist(varConf, x_nameSingular))
		val.push_back(varConf.at(x_nameSingular));
	return val;
	// throw MkException("Expected to find object with name " + x_namePlural + " or " + x_nameSingular, LOC);
}

/// Add variation to simulation
void Simulation::AddVariations(vector<string>& xr_variationNames, const mkconf& x_varConf, mkconf& xr_mainConfig)
{
	if(x_varConf.is_null())
		throw MkException("Variation config is null. Please check that a 'variations' property is present in simulation file", LOC);
	for(const auto& varConf : x_varConf)
	{
		// Read module and parameter attribute
		// assert(varConf.at("param").is_null());  // note: no longer supported
		const auto paramNames(createArray("parameters", "parameter", varConf));
		const auto moduleNames(createArray("modules", "module", varConf));
		if(moduleNames.size() != 1 && moduleNames.size() != paramNames.size())
			throw MkException("Modules and parameters must have the same size (in variation) or modules must only contain one module", LOC);

		// Get all targets to be varied in config
		vector<mkjson*> targets;
		mkjson originalValues;
		auto itmod = moduleNames.begin();
		for(const auto& itpar : paramNames)
		{
			try
			{
				LOG_DEBUG(m_logger, "Param:" << itmod->get<string>() << ":" << itpar.get<string>());
				mkconf mod = manOrMod(xr_mainConfig, itmod->get<string>())["inputs"];
				if(existInArray(mod, "name", itpar.get<string>()))
				{
					mkconf& target = findFirstInArray(mod, "name", itpar.get<string>());
					targets.push_back(&target);
					originalValues.push_back(target.get<string>());
				}
				else originalValues.push_back(""); //TODO: Use default value instead
				if(moduleNames.size() > 1)
					itmod++;
			}
			catch(exception &e)
			{
				throw MkException("Cannot variate parameter " + itpar.get<string>() + " of module " + itmod->get<string>() + ": " + e.what(), LOC);
			}
		}



		if(exist(varConf, "file"))
		{
			string file = varConf.at("file").get<string>();
			// Reference to the value to use (in json file)
			const auto& refNames = varConf.at("references");
			if(refNames.size() != paramNames.size())
				throw MkException("References must have the same sizes as parameters", LOC);

			// set values of parameters by using a JSON file
			copy(file, m_outputDir + "/" + basename(file));
			ifstream ifs;
			ifs.open(file);
			mkjson root;
			ifs >> root;

			// For each set of parameters
			for(auto it = root.cbegin() ; it != root.cend() ; ++it)
			{
				int i = 0;
				for(const auto& ref : refNames)
				{
					if(it.value().at(ref.get<string>()).is_null())
						throw MkException("No tag " + ref.get<string>() + " for " + it.key(), LOC);

					*(targets.at(i)) = it.value().at(ref.get<string>());
					i++;
				}

				// add a name
				// note: in the future maybe use a hash
				xr_variationNames.push_back(it.key());

				// Change value of param
				if(varConf.find("variations") != varConf.end())
					AddVariations(xr_variationNames, varConf.at("variations"), xr_mainConfig);
				else
					AddSimulationEntry(xr_variationNames, xr_mainConfig);
				xr_variationNames.pop_back();
			}
			ifs.close();
		}
		else
		{
			// Set values by using range information provided in JSON
			if(paramNames.size() > 1)
				throw MkException("To set more than one parameter variation, use an external file with option file=...", LOC);
			// default values. Empty range means that the prog uses the default range of the param
			// const mkjson& range = varConf.at("range");
			int nb = varConf.value<int>("nb", 10);

			LOG_DEBUG(m_logger, "Variations for module " << moduleNames.at(0).get<string>());
			const Parameter& param = m_manager.GetModuleByName(moduleNames.at(0).get<string>()).GetParameters().GetParameterByName(paramNames.at(0).get<string>());
			mkjson values = param.GenerateValues(nb);

			// Generate a config for each variation
			for(auto& elem : values)
			{
				LOG_DEBUG(m_logger, "Value = " << elem);
				xr_variationNames.push_back(paramNames.at(0).get<string>() + "-" + oneLine(elem));

				// Change value of param
				*(targets.at(0)) = elem;
				if(varConf.find("variations") != varConf.end())
					AddVariations(xr_variationNames, varConf.at("variations"), xr_mainConfig);
				else
					AddSimulationEntry(xr_variationNames, xr_mainConfig);
				xr_variationNames.pop_back();
			}
		}

		// Set the target back to its original value and delete config obj
		auto itval = originalValues.begin();
		for(auto& target : targets)
		{
			*target = *itval;
			itval++;
		}
	}
}


/// Generate a simulation ready to be launched
void Simulation::Generate()
{
	create_directory(m_outputDir);

	remove("simulation_latest");
	create_symlink(m_outputDir, "simulation_latest");
	writeToFile(m_param.config, "simulation_latest/Simulation.json");
	create_directory(m_outputDir + "/ready");
	create_directory(m_outputDir + "/running");
	create_directory(m_outputDir + "/results");

	// Initialize members
	m_allTargets.str("");
	m_targets.str("");
	m_cpt = 0;

	vector<string> variationNames;
	mkconf copyConfig = m_param.config;
	AddVariations(variationNames, m_param.config.at("variations"), copyConfig);

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
} // namespace mk
