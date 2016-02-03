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


#include "Manager.h"
#include "MkException.h"
#include "Event.h"
#include "util.h"
#include "Simulation.h"

#ifndef MARKUS_NO_GUI
#include <QApplication>
#include "Editor.h"
#include "Markus.h"
#include "MarkusApplication.h"
#endif

#include <signal.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <log4cxx/xml/domconfigurator.h>
#include <getopt.h>    /* for getopt_long; standard getopt is in unistd.h */
#include <thread>

using namespace std;




void usage()
{
	printf(
		"Usage : markus <options> <configuration.xml> <video_file> \n"
		"\n"
		" configuration.xml     The configuration of the application describing the different modules\n"
		" video_file (opt.)     A video file to use as input. (assuming the input module is named \"Input\")\n"
		"\n"
		"options:\n"
		" -h  --help            Print this help        \n"
		" -v  --version         Print version information\n"
		" -d  --describe        Create a description of all modules in XML format inside module/ directory. For development purpose.\n"
		" -e  --editor          Launch the module editor. \n"
		" -S  --simulation      Prepare a full simultion based on a *.sim.xml file\n"
		"                       Override some parameters in an extra XML file\n"
		" -c  --centralized     Module processing function is called from the manager (instead of decentralized timers)\n"
		" -i  --stdin           Read commands from stdin\n"
		" -n  --no-gui          Run process without gui\n"
		" -f  --fast            Fast, no real-time input\n"
		" -R  --robust          Robust, continue if an exception occured during processing\n"
		" -l  --log-conf <log4cxx_config>.xml\n"
		"                       Set logging mode\n"
		" -o  --output-dir directory\n"
		"                       Set the name of the output directory\n"
		" -p  --parameter \"moduleName.paramName=value\"\n"
		"                       Override the value of one parameter\n"
		" -x  --xml extra_config.xml\n"
		"                       Override some parameters in an extra XML file\n"
		" -C  --cache           Cache directory (from another run) if using cache\n"
	);
}

/// Specific thread dedicated to the reading of commands via stdin
void *send_commands(void *x_void_ptr)
{
	Manager *pManager = reinterpret_cast<Manager*>(x_void_ptr);
	static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("send_commands"));
	assert(pManager != nullptr);
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

	return nullptr;
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
	bool robust      = false;

	string configFile    = "config.xml";
	string logConfigFile = "log4cxx.xml";
	string outputDir     = "";
	string cacheDirectory = "";
	vector<string> parameters;
	vector<string> extraConfig;
};

/// Process arguments from command line
int processArguments(int argc, char** argv, struct arguments& args, log4cxx::LoggerPtr& logger)
{
	static struct option long_options[] =
	{
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
		{"output-dir",  1, 0, 'o'},
		{"parameter",   1, 0, 'p'},
		{"xml",         1, 0, 'x'},
		{"cache",       1, 0, 'C'},
		{"robust",      0, 0, 'R'},
		{nullptr, 0, nullptr, 0}
	};
	char c;
	int option_index = 0;
	while ((c = getopt_long(argc, argv, "hvdeScfinRl:o:p:x:C:", long_options, &option_index)) != -1)
	{
		switch (c)
		{
		case 'h':
			usage();
			return 0;
			break;
		case 'v':
			LOG_INFO(logger, Context::Version(true));
			exit(0);
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
		case 'R':
			args.robust = true;
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
		case 'C':
			args.cacheDirectory = optarg;
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
void overrideConfig(ConfigReader& mainConfig, const vector<string>& extraConfig, const vector<string>& parameters, log4cxx::LoggerPtr& logger)
{
	// Override values of parameters if an extra config is used
	for(const auto& elem1 : extraConfig)
	{
		try
		{
			// open the config and override the initial config
			ConfigFile extra(elem1);
			mainConfig.OverrideWith(extra);
		}
		catch(MkException& e)
		{
			LOG_WARN(logger, "Cannot read parameters from extra config \""<<elem1<<"\": "<<e.what());
		}
	}

	// Set values of parameters if set from command line
	for(const auto& elem : parameters)
	{
		try
		{
			string param = elem.substr(0, elem.find("="));
			// remove quote if necessary
			if (*(param.begin()) == '\'')
				param = param.substr(1,param.length());

			string value = elem.substr(elem.find("=") + 1);
			// remove quote if necessary
			if (*(value.rbegin()) == '\'')
				value = value.substr(0,value.length()-1);

			vector<string> path;
			split(param, '.', path);
			if(path.size() != 2)
				throw MkException("Parameter set in command line must be in format 'module.parameter'", LOC);
			if(path[0] == "manager")
				mainConfig.RefSubConfig("application").FindRef("parameters>param[name=\"" + path[1] + "\"]", true).SetValue(value);
			else
				mainConfig.RefSubConfig("application").FindRef("module[name=\"" + path[0] + "\"]>parameters>param[name=\"" + path[1] + "\"]", true).SetValue(value);
			// manager.GetModuleByName(path[0])->GetParameters().RefParameterByName(path[1]).SetValue(value, PARAMCONF_CMD);
		}
		catch(std::exception& e)
		{
			LOG_ERROR(logger, "Cannot parse command line parameter "<<elem<<": "<<e.what());
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
	exit(0);
#else
	cerr<<"To launch the editor Markus must be compiled with GUI"<<endl;
	exit(-1);
#endif
}


int main(int argc, char** argv)
{
	// Install error handler
	// Enabling this will handle segfault but will not create a nice core dump
	// signal(SIGSEGV, printStack);

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
		// Register all modules, features, ... in factories
		Factories::RegisterAll();

		LOG_INFO(logger, Context::Version(true));
		ConfigFile mainConfig(args.configFile);
		mainConfig.Validate();
		ConfigReader appConfig = mainConfig.Find("application");
		assert(!appConfig.IsEmpty());

		// Init global variables and objects
		// Context manages all call to system, files, ...
		Context::Parameters contextParameters(mainConfig.Find("application"), args.configFile, appConfig.GetAttribute("name"), args.outputDir);
		contextParameters.centralized    = args.centralized;
		contextParameters.robust         = args.robust;
		contextParameters.realTime       = !args.fast;
		contextParameters.cacheDirectory = args.cacheDirectory;
		Context context(contextParameters);
		if(args.outputDir != "")
		{
			string dir = args.outputDir + "/";
			setenv("LOG_DIR", dir.c_str(), 1);
		}

		log4cxx::xml::DOMConfigurator::configure(args.logConfigFile);

#ifndef MARKUS_NO_GUI
		MarkusApplication app(argc, argv);
#endif

		overrideConfig(mainConfig, args.extraConfig, args.parameters, logger);

		if(args.simulation)
		{
			Simulation::Parameters parameters(mainConfig.Find("application"));
			Simulation sim(parameters, context);
			sim.Generate(mainConfig);
			return 0;
		}

		// Set manager and context
		Manager::Parameters managerParameters(appConfig);
		// Override parameter auto_process with centralized
		managerParameters.autoProcess = !args.nogui;
		Manager manager(managerParameters);
		manager.SetContext(context);

		if(args.describe)
		{
			manager.CreateEditorFiles("editor/js/all_modules.js");
			return 0;
		}

		manager.Connect();
		manager.Reset();


		/// Create a separate thread to read the commands from stdin
		pthread_t command_thread;
		if(args.useStdin && pthread_create(&command_thread, nullptr, send_commands, &manager))
		{
			LOG_ERROR(logger, "Error creating thread");
			return -1;
		}

		// Notify the parent process (for monitoring purposes)
		Event ev1;
		ev1.AddExternalInfo("pid", getpid());
		ev1.Raise("started", 0, 0);
		ev1.Notify(context, true);

		if(args.nogui)
		{
			// No gui. launch the process directly
			// note: so far we cannot launch the process in a decentralized manner (with a timer on each module)
			if(args.centralized)
			{
				assert(!managerParameters.autoProcess);
				while(manager.ProcessAndCatch())
				{
					// nothing
				}
			}
			else
			{
				LOG_ERROR(logger, "Markus cannot run in decentralized mode without GUI yet.");
			}
		}
		else
		{
#ifndef MARKUS_NO_GUI
			assert(managerParameters.autoProcess);
			manager.Start();
			ConfigFile mainGuiConfig("gui.xml", true);
			ConfigReader guiConfig = mainGuiConfig.FindRef("gui[name=\"" + args.configFile + "\"]", true);
			guiConfig.FindRef("parameters", true);

			MarkusWindow::Parameters windowParameters(guiConfig);
			MarkusWindow gui(windowParameters, manager);
			gui.setWindowTitle("Markus");
			if(!args.nogui)
				gui.show();
			if(app.exec() != 0)
				LOG_ERROR(logger, "Qt Application returned error code");

			// write the modified params in config and save
			gui.UpdateConfig();
			mainGuiConfig.SaveToFile("gui.xml");
#else
			LOG_ERROR(logger, "Markus was compiled without GUI. It can only be launched with option -nc");
#endif
		}
		manager.Stop();


		returnValue = MK_EXCEPTION_NORMAL - MK_EXCEPTION_FIRST;

		// Write the modified params in config and save
		manager.UpdateConfig();
#ifndef MARKUS_NO_GUI
		// Save the last config with modifs. This would cause a problem if we had no write access to the current dir
		mainConfig.SaveToFile("last_config.xml");
#endif
	}
	catch(MkException& e)
	{
		// improvement: maybe try to print the full stack trace of exceptions
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


	// For benchmark only: this line should have no effect if no QuickTimer is used
	QuickTimer::PrintTimers();


	return returnValue;
}

