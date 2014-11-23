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

using namespace std;

/// @brief: This file contains all the functions needed to prepare an optimization and variate parameters of the config

/// Add targets lines for inclusion in Makefile
void addSimulationEntry(const std::string& x_variationName, const std::string& x_outputDir, const ConfigReader& x_mainConfig, ostream& xr_allTargets, ostream& xr_targets, int& xr_cpt);

/// Add variation to simulation
void addVariations(std::string x_variationName, Manager& xr_manager, const ConfigReader& x_varConf, const std::string& x_outputDir, ConfigReader& xr_mainConfig, ostream& xr_allTargets, ostream& xr_targets, int& xr_cpt);


/// Generate a simulation ready to be launched
bool generateSimulation(ConfigReader& mainConfig, const Context& context, log4cxx::LoggerPtr& logger);
#endif
