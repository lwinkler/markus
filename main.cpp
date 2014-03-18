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

#ifndef MARKUS_NO_GUI
#include "Editor.h"
#include "Markus.h"
#include "MarkusApplication.h"
#endif

#include <iostream>
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
	printf(" -c  --centralized     Module processing function is called from the manager (instead of decentralized timers)\n");
	printf(" -i  --stdin           Read commands from stdin\n");
	printf(" -n  --no-gui          Run process without gui\n");
	printf(" -l  --log-conf <log4cxx_config>.xml\n");
	printf("                       Set logging mode\n");
	printf(" -o  --output-dir directory\n");
	printf("                       Set the name of the output directory\n");
	printf(" -p  --parameter \"moduleName.paramName=value\"\n");
	printf("                       Override the value of one parameter\n");
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
				cin.clear();
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
	}

	return NULL;
}

int main(int argc, char** argv)
{
	// Load XML configuration file using DOMConfigurator
	log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("main"));

	bool describe    = false;
	bool nogui       = false;
	bool centralized = false;
	bool useStdin    = false;
	int returnValue  = -1;

	string configFile    = "config.xml";
	string logConfigFile = "log4cxx.xml";
	string outputDir     = "";
	vector<string> parameters;

	// Read arguments

	//int digit_optind = 0;
	static struct option long_options[] = {
		{"help",        0, 0, 'h'},
		{"version",     0, 0, 'v'},
		{"describe",    0, 0, 'd'},
		{"editor",      0, 0, 'e'},
		{"centralized", 0, 0, 'c'},
		{"stdin",       0, 0, 'i'},
		{"no-gui",      0, 0, 'n'},
		{"log-conf",    1, 0, 'l'},
		{"output_dir",  1, 0, 'o'},
		{"parameter",   1, 0, 'p'},
		{NULL, 0, NULL, 0}
	};
	int c;
	int option_index = 0;
	while ((c = getopt_long(argc, argv, "hvdecinl:o:p:", long_options, &option_index)) != -1)
	{
		switch (c) {
			case 'h':
				usage();
				return 0;
				break;
			case 'v':
				LOG_INFO(logger, Manager::Version());
				return 0;
			case 'd':
				describe = true;
				break;
			case 'e':
				{
#ifndef MARKUS_NO_GUI
					QApplication app(argc, argv);
					Editor editor;
					return app.exec();
#else
					LOG_ERROR(logger, "To launch the editor Markus must be compiled with GUI");
					return -1;
#endif
				}
				break;
			case 'c':
				centralized = true;
				break;
			case 'i':
				useStdin = true;
				break;
			case 'n':
				nogui = true;
				break;
			case 'l':
				logConfigFile = optarg;
				break;
			case 'o':
				outputDir = optarg;
				break;
			case 'p':
				parameters.push_back(optarg);
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

	if (optind == argc - 2) {
		configFile = argv[argc - 2];
		stringstream ss;
		ss<<"Input.file="<<argv[argc - 1];
		parameters.push_back(ss.str());
		parameters.push_back("Input.class=VideoFileReader");
	}
	else if (optind == argc - 1) {
		configFile = argv[argc - 1];
	}
	else if(optind == argc)
	{
		LOG_INFO(logger, "Using default configuration file "<<configFile);
	}
	else 
	{
		LOG_ERROR(logger, "Invalid number of arguments "<<(argc - optind));
		usage();
		return -1;
	}

	// Init global variables and objects
	if(outputDir != "")
	{
		outputDir = Manager::OutputDir(outputDir, configFile);
		string dir = outputDir + "/";
		setenv("LOG_DIR", dir.c_str(), 1);
	}

	log4cxx::xml::DOMConfigurator::configure(logConfigFile);

	// Raise a start event. The utility of this is simply to notify a parent process
	Event ev;
	ev.Raise("start");
	ev.Notify(jsonify("pid", getpid()));

	try
	{
#ifndef MARKUS_NO_GUI
		MarkusApplication app(argc, argv);
#endif
		LOG_INFO(logger, Manager::Version());
		ConfigReader mainConfig(configFile);
		mainConfig.Validate();
		ConfigReader appConfig = mainConfig.GetSubConfig("application");
		assert(!appConfig.IsEmpty());

		// Set values of parameters if set from command line
		for(vector<string>::const_iterator it = parameters.begin() ; it != parameters.end() ; it++)
		{
			try
			{
				string param = it->substr(0, it->find("="));
				string value = it->substr(it->find("=") + 1);
				vector<string> path;
				split(param, '.', path);
				if(path.size() != 2)
					throw MkException("Parameter set in command line must be in format 'module.parameter'", LOC);
				appConfig.RefSubConfig("module", path[0]).RefSubConfig("parameters").RefSubConfig("param", path[1], true).SetValue(value);
				// manager.GetModuleByName(path[0])->GetParameters().RefParameterByName(path[1]).SetValue(value, PARAMCONF_CMD);
			}
			catch(std::exception& e)
			{
				LOG_ERROR(logger, "Cannot parse command line parameter "<<*it<<": "<<e.what());
				throw MkException("Cannot parse command line parameter", LOC);
			}
		}
		Manager manager(appConfig, centralized);

		if(describe) 
		{
			manager.Export();
			return 0;
		}

		manager.Connect();
		manager.Reset();


		/// Create a separate thread to read the commands from stdin
		pthread_t command_thread;
		if(useStdin && pthread_create(&command_thread, NULL, send_commands, &manager))
		{
			LOG_ERROR(logger, "Error creating thread");
			return -1;
		}

		if(centralized)
		{
			if(!nogui)
				LOG_WARN(logger, "GUI is not shown if you use --centralized option. To avoid this message use --no-gui option.");
			// No gui. launch the process directly
			// so far we cannot launch the process in a decentralized manner (with a timer on each module)
			while(manager.Process())
			{
				// nothing 
			}

			returnValue = MK_EXCEPTION_NORMAL - MK_EXCEPTION_FIRST;
		}
		else
		{
#ifndef MARKUS_NO_GUI
			ConfigReader mainGuiConfig("gui.xml", true);
			ConfigReader guiConfig = mainGuiConfig.RefSubConfig("gui", configFile, true);
			guiConfig.RefSubConfig("parameters", "", true);

			Markus gui(guiConfig, manager);
			gui.setWindowTitle("Markus");
			if(!nogui)
				gui.show();
			returnValue = app.exec(); // TODO: Manage case where centralized with GUI

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
	catch(std::string str)
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

