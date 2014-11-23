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

#include <QApplication>

#include "Manager.h"
#include "MkException.h"
#include "Event.h"
#include "util.h"

#ifndef MARKUS_NO_GUI
#include "Editor.h"
#include "Markus.h"
#include "MarkusApplication.h"
#endif

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <log4cxx/xml/domconfigurator.h>
#include <getopt.h>    /* for getopt_long; standard getopt is in unistd.h */

using namespace std;




void usage()
{
	printf("Usage : markus <options> <configuration.xml> <video_file> \n");
	printf("\n");
	printf(" configuration.xml     The configuration of the application describing the different modules\n");
	printf(" video_file (opt.)     A video file to use as input. (assuming the input module is named \"Input\")\n");
	printf("\n");
	printf("options:\n");
	printf(" -h  --help            Print this help        \n");
	printf(" -v  --version         Print version information\n");
	printf(" -d  --describe        Create a description of all modules in XML format inside module/ directory. For development purpose.\n");
	printf(" -e  --editor          Launch the module editor. \n");
	printf(" -S  --simulation      Prepare a full simultion based on a *.sim.xml file\n");
	printf("                       Override some parameters in an extra XML file\n");
	printf(" -c  --centralized     Module processing function is called from the manager (instead of decentralized timers)\n");
	printf(" -i  --stdin           Read commands from stdin\n");
	printf(" -n  --no-gui          Run process without gui\n");
	printf(" -f  --fast            Fast, no real-time input\n");
	printf(" -l  --log-conf <log4cxx_config>.xml\n");
	printf("                       Set logging mode\n");
	printf(" -o  --output-dir directory\n");
	printf("                       Set the name of the output directory\n");
	printf(" -p  --parameter \"moduleName.paramName=value\"\n");
	printf("                       Override the value of one parameter\n");
	printf(" -x  --xml extra_config.xml\n");
	printf("                       Override some parameters in an extra XML file\n");
}


/// Specific thread dedicated to the reading of commands via stdin
void *send_commands(void *x_void_ptr)
{
	Manager *pManager = reinterpret_cast<Manager*>(x_void_ptr);
	static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("Module"));
	assert(pManager != NULL);
	string input;
	vector<string> elems;
	string value;
	while(true)
	{
		try
		{
			if(getline(cin, input))
			{
				split(input, ' ', elems);
				if(elems.size() == 1)
					value = "";
				else if(elems.size() == 2)
					value = elems.at(1);
				else throw MkException("Command must have one or two elements", LOC);
				LOG_INFO(logger, "Send command: "<<elems.at(0)<<" \""<<value<<"\"");
				pManager->SendCommand(elems.at(0), value);
			}
			// else cout<<"Getline failed"<<endl;
		}
		catch(std::exception& e)
		{
			LOG_ERROR(logger, "Cannot execute command: "<<e.what());
		}
		catch(...)
		{
			LOG_ERROR(logger, "Cannot execute command");
		}
		// usleep(1000000);
		cin.clear();
	}

	return NULL;
}

struct arguments
{
	bool describe    = false;
	bool nogui       = false;
	bool centralized = false;
	bool fast        = false;
	bool useStdin    = false;
	bool editor      = false;
	bool simulation  = false;

	string configFile    = "config.xml";
	string logConfigFile = "log4cxx.xml";
	string outputDir     = "";
	vector<string> parameters;
	vector<string> extraConfig;
};

/// Process arguments from command line
int processArguments(int argc, char** argv, struct arguments& args, log4cxx::LoggerPtr& logger)
{
	static struct option long_options[] = {
		{"help",        0, 0, 'h'},
		{"version",     0, 0, 'v'},
		{"describe",    0, 0, 'd'},
		{"editor",      0, 0, 'e'},
		{"simulation",  0, 0, 'S'},
		{"centralized", 0, 0, 'c'},
		{"fast",        0, 0, 'f'},
		{"stdin",       0, 0, 'i'},
		{"no-gui",      0, 0, 'n'},
		{"log-conf",    1, 0, 'l'},
		{"output_dir",  1, 0, 'o'},
		{"parameter",   1, 0, 'p'},
		{"xml",         1, 0, 'x'},
		{NULL, 0, NULL, 0}
	};
	char c;
	int option_index = 0;
	while ((c = getopt_long(argc, argv, "hvdeScfinl:o:p:x:", long_options, &option_index)) != -1)
	{
		switch (c) {
			case 'h':
				usage();
				return 0;
				break;
			case 'v':
				LOG_INFO(logger, Context::Version(true));
				return 0;
			case 'd':
				args.describe = true;
				break;
			case 'e':
				args.editor = true;
				break;
			case 'S':
				args.simulation = true;
				break;
			case 'c':
				args.centralized = true;
				break;
			case 'f':
				args.fast = true;
				break;
			case 'i':
				args.useStdin = true;
				break;
			case 'n':
				args.nogui = true;
				break;
			case 'l':
				args.logConfigFile = optarg;
				break;
			case 'o':
				args.outputDir = optarg;
				break;
			case 'p':
				args.parameters.push_back(optarg);
				break;
			case 'x':
				args.extraConfig.push_back(optarg);
				break;
			case ':': // missing argument
				LOG_ERROR(logger, "--"<<long_options[::optopt].name<<": an argument is required");
				return -1;

			case '?': // unknown option
				LOG_ERROR(logger, argv[optind - 1]<<": unknown option");
				return -1;
			default:
				LOG_ERROR(logger, "Unknown parameter "<<c);
				return -1;
		}
	}

	// Handle other arguments
	if (optind == argc - 2)
	{
		args.configFile = argv[argc - 2];
		stringstream ss;
		ss<<"Input.file="<<argv[argc - 1];
		args.parameters.push_back(ss.str());
		args.parameters.push_back("Input.class=VideoFileReader");
	}
	else if (optind == argc - 1)
	{
		args.configFile = argv[argc - 1];
	}
	else if(optind == argc)
	{
		LOG_INFO(logger, "Using default configuration file "<<args.configFile);
	}
	else 
	{
		LOG_ERROR(logger, "Invalid number of arguments "<<(argc - optind));
		usage();
		return -1;
	}
	return 1;
}

/// Override the initial config with extra config files and argument set parameters
void overrideConfig(ConfigReader& appConfig, const vector<string>& extraConfig, const vector<string>& parameters, log4cxx::LoggerPtr& logger)
{
	// Override values of parameters if an extra config is used
	for(vector<string>::const_iterator it1 = extraConfig.begin() ; it1 != extraConfig.end() ; ++it1)
	{
		try
		{
			// open the config and override the initial config
			ConfigReader extra(*it1);
			appConfig.OverrideWith(extra);
		}
		catch(MkException& e)
		{
			LOG_WARN(logger, "Cannot read parameters from extra config \""<<*it1<<"\": "<<e.what());
		}
	}

	// Set values of parameters if set from command line
	for(vector<string>::const_iterator it = parameters.begin() ; it != parameters.end() ; ++it)
	{
		try
		{
			string param = it->substr(0, it->find("="));
			// remove quote if necessary
			if (*(param.begin()) == '\'')
				param = param.substr(1,param.length());

			string value = it->substr(it->find("=") + 1);
			// remove quote if necessary
			if (*(value.rbegin()) == '\'')
				value = value.substr(0,value.length()-1);

			vector<string> path;
			split(param, '.', path);
			if(path.size() != 2)
				throw MkException("Parameter set in command line must be in format 'module.parameter'", LOC);
			if(path[0] == "manager")
				appConfig.RefSubConfig("parameters", "", true).RefSubConfig("param", path[1], true).SetValue(value);
			else
				appConfig.RefSubConfig("module", path[0]).RefSubConfig("parameters").RefSubConfig("param", path[1], true).SetValue(value);
			// manager.GetModuleByName(path[0])->GetParameters().RefParameterByName(path[1]).SetValue(value, PARAMCONF_CMD);
		}
		catch(std::exception& e)
		{
			LOG_ERROR(logger, "Cannot parse command line parameter "<<*it<<": "<<e.what());
			throw MkException("Cannot parse command line parameter", LOC);
		}
	}
}

void launchEditor(int argc, char** argv)
{
#ifndef MARKUS_NO_GUI
	QApplication app(argc, argv);
	string projectFile = "";
	if(argc > 2)
		projectFile = argv[2];
	Editor editor(projectFile);
	app.exec();
	exit(1); // TODO: See what to do here
#else
	LOG_ERROR(logger, "To launch the editor Markus must be compiled with GUI");
	exit(-1);
#endif
}

/// Add targets lines for inclusion in Makefile
void addSimulationEntry(const string& x_variationName, const string& x_outputDir, const ConfigReader& x_mainConfig, ostream& xr_allTargets, ostream& xr_targets, int& xr_cpt)
{
	// Generate entries for makefile
	stringstream sd;
	sd << "simul" << setfill('0') << setw(6) << xr_cpt;
	xr_allTargets << "$(OUTDIR)/results/" <<  sd.str() << " ";
	xr_targets << "$(OUTDIR)/results/" << sd.str() << ":" << endl;
	// xr_targets << "\t" << "mkdir -p $(OUTDIR)/results/"  << sd.str() << endl;
	xr_targets << "\t" << "rm -rf $(OUTDIR)/running/"  << sd.str() << 
		" && cp -r $(OUTDIR)/ready/" << sd.str() << " $(OUTDIR)/running/" << sd.str() << endl;
	xr_targets << "\t" << "$(EXE) $(PARAMS) $(OUTDIR)/running/" << sd.str() << "/" << x_variationName << ".xml -o $(OUTDIR)/running/"  << sd.str() << endl;
	xr_targets << "\t" << "mv $(OUTDIR)/running/" << sd.str() << " $(OUTDIR)/results/" << endl;
	xr_targets << endl;

	// Create ready/... directory that describes the simulation
	stringstream subdir;
	subdir << x_outputDir << "/ready/" << sd.str();
	stringstream xmlProjName;
	xmlProjName << subdir.str() << "/" << x_variationName << ".xml";
	SYSTEM("mkdir -p " + subdir.str());
	x_mainConfig.SaveToFile(xmlProjName.str());
	xr_cpt++;
}

/// Add variation to simulation
void addVariations(string x_variationName, Manager& xr_manager, const ConfigReader& x_varConf, const string& x_outputDir, ConfigReader& xr_mainConfig, ostream& xr_allTargets, ostream& xr_targets, int& xr_cpt)
{
	ConfigReader varConf = x_varConf;
	while(! varConf.IsEmpty())
	{
		// Retrieve args from config
		string moduleName = varConf.GetAttribute("module");
		string paramName  = varConf.GetAttribute("param");
		ConfigReader target = xr_mainConfig.RefSubConfig("application").RefSubConfig("module", moduleName).RefSubConfig("parameters").RefSubConfig("param", paramName);
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

		const Parameter& param = xr_manager.GetModuleByName(moduleName).GetParameters().GetParameterByName(paramName);
		vector<string> values;
		param.GenerateValues(nb, values, range);

		// Generate a config for each variation
		string originalValue = target.GetValue();
		for(vector<string>::const_iterator it = values.begin() ; it != values.end() ; it++)
		{
			string variationName;
			if(x_variationName == "")
				variationName = paramName + "-" + *it;
			else
				variationName = x_variationName + "_" + paramName + "-" + *it;

			// Change value of param
			target.SetValue(*it);
			ConfigReader subConf = varConf.GetSubConfig("var");
			if(subConf.IsEmpty())
				addSimulationEntry(variationName, x_outputDir, xr_mainConfig, xr_allTargets, xr_targets, xr_cpt);
			else
				addVariations(variationName, xr_manager, subConf, x_outputDir, xr_mainConfig, xr_allTargets, xr_targets, xr_cpt);

		}
		target.SetValue(originalValue);
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
	addVariations("", manager, varConf, outputDir, mainConfig, allTargets, targets, cpt);

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

int main(int argc, char** argv)
{
	// Load XML configuration file using DOMConfigurator
	log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("main"));
	int returnValue  = -1;


	// Read arguments
	struct arguments args;
	if(processArguments(argc, argv, args, logger) < 1)
		return -1;

	if(args.editor)
		launchEditor(argc, argv);

	try
	{
		LOG_INFO(logger, Context::Version(true));
		ConfigReader mainConfig(args.configFile); // TODO: segfault if file non-existant. Fix
		mainConfig.Validate();
		ConfigReader appConfig = mainConfig.GetSubConfig("application");
		assert(!appConfig.IsEmpty());

		// Init global variables and objects
		Context context("", appConfig.GetAttribute("name"), args.outputDir);
		if(args.outputDir != "")
		{
			string dir = args.outputDir + "/";
			setenv("LOG_DIR", dir.c_str(), 1);
		}

		log4cxx::xml::DOMConfigurator::configure(args.logConfigFile);

#ifndef MARKUS_NO_GUI
		MarkusApplication app(argc, argv);
#endif

		overrideConfig(appConfig, args.extraConfig, args.parameters, logger);

		if(args.simulation)
			return generateSimulation(mainConfig, context, logger);
			

		// Override parameter auto_process with centralized
		appConfig.RefSubConfig("parameters", "", true).RefSubConfig("param", "auto_process", true).SetValue(args.centralized ? "1" : "0");
		appConfig.RefSubConfig("parameters", "", true).RefSubConfig("param", "fast", true).SetValue(args.fast ? "1" : "0");

		// Set manager and context
		Manager manager(appConfig);
		manager.SetContext(context);

		if(args.describe) 
		{
			manager.Export();
			return 0;
		}

		manager.Connect();
		manager.Reset();


		/// Create a separate thread to read the commands from stdin
		pthread_t command_thread;
		if(args.useStdin && pthread_create(&command_thread, NULL, send_commands, &manager))
		{
			LOG_ERROR(logger, "Error creating thread");
			return -1;
		}

		// Notify the parent process (for monitoring purposes)


		if(args.nogui)
		{
			// No gui. launch the process directly
			// note: so far we cannot launch the process in a decentralized manner (with a timer on each module)

			Event ev1;
			ev1.AddExternalInfo("pid", getpid());
			ev1.Raise("started");
			ev1.Notify(context, true);

			while(manager.Process())
			{
				// nothing 
			}

			Event ev2;
			ev2.Raise("stopped");
			ev2.Notify(context, true);
			returnValue = MK_EXCEPTION_NORMAL - MK_EXCEPTION_FIRST;
		}
		else
		{
#ifndef MARKUS_NO_GUI
			ConfigReader mainGuiConfig("gui.xml", true);
			ConfigReader guiConfig = mainGuiConfig.RefSubConfig("gui", args.configFile, true);
			guiConfig.RefSubConfig("parameters", "", true);

			MarkusWindow gui(guiConfig, manager);
			gui.setWindowTitle("Markus");
			if(!args.nogui)
				gui.show();
			returnValue = app.exec();

			// write the modified params in config and save
			gui.UpdateConfig();
			mainGuiConfig.SaveToFile("gui.xml");
#else
			LOG_ERROR(logger, "Markus was compiled without GUI. It can only be launched with option -nc");
			returnValue = -1;
#endif
		}

		// Write the modified params in config and save
		manager.UpdateConfig();
		mainConfig.SaveToFile("last_config.xml");
	}
	catch(MkException& e)
	{
		// TODO: Try to print the full stack trace of exceptions
		LOG_ERROR(logger, "(Markus exception " << e.GetCode() << "): " << e.what());
		returnValue = e.GetCode() - MK_EXCEPTION_FIRST;
	}
	catch(cv::Exception& e)
	{
		LOG_ERROR(logger, "Exception (cv::Exception): " << e.what() );
		returnValue = -1;
	}
	catch(std::exception& e)
	{
		LOG_ERROR(logger, "Exception (std::exception): " << e.what() );
		returnValue = -1;
	}
	catch(std::string& str)
	{
		LOG_ERROR(logger, "Exception (string): " << str );
		returnValue = -1;
	}
	catch(const char* str)
	{
		LOG_ERROR(logger, "Exception (const char*): " << str );
		returnValue = -1;
	}
	catch(...)
	{
		LOG_ERROR(logger, "Exception: Unknown");
		returnValue = -1;
	}


	return returnValue;
}

