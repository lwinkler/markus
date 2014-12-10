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

#ifndef MK_SIMULATION_H
#define MK_SIMULATION_H

#include "Manager.h"
#include "ConfigReader.h"
#include <sstream>

using namespace std;

/**
* @brief This file contains all the functions needed to prepare an optimization and variate parameters of the config
*/
class Simulation : public Configurable
{
public:
	class Parameters : public Processable::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : Processable::Parameters(x_confReader)
		{
			ParameterStructure::Init();
		}
	};

	Simulation(const ConfigReader& x_configReader, const Context& x_context);
	void Generate(ConfigReader& mainConfig);

protected:
	/// Add an entry in the Makefile
	void AddSimulationEntry(const vector<string>& x_variationNames, const ConfigReader& x_mainConfig);

	/// Add variation to simulation
	void AddVariations(std::vector<std::string>& x_variationNames, const ConfigReader& x_varConf, ConfigReader& xr_mainConfig);

	virtual const ParameterStructure & GetParameters() const {return m_param;}

	// Streams for generation of the simulation Makefile
	std::stringstream m_allTargets;
	std::stringstream m_targets;

	Manager m_manager;
	const std::string m_outputDir;
	int m_cpt;

private:
	static log4cxx::LoggerPtr m_logger;
	Parameters m_param; // note: to save time
	inline Parameters& RefParameters() {return m_param;}
};
#endif
