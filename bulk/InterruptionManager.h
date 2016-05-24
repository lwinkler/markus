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

#ifndef MK_INTERRUPTION_MANAGER_H
#define MK_INTERRUPTION_MANAGER_H

#include "ConfigReader.h"
#include "ParameterStructure.h"
#include <log4cxx/logger.h>

/// Class to represent a command
struct Command
{
	Command(const std::string& x_name, const std::string& x_value):
		name(x_name),
		value(x_value) {}
	std::string name;
	std::string value;
};

struct Interruption
{
	Interruption(const Command& x_command, int x_count) :
		command(x_command),
		remaining(x_count) {}
	Command command;
	int remaining;
};

/**
* @brief Class to manage all global interruptions: an interruption is a link between an event and a command. The command is called as a result of the event.
*/
class InterruptionManager // : public Configurable
{
public:
	InterruptionManager();

	// Singleton: InterruptionManager is instanciated here !
	static InterruptionManager& GetInst() {static InterruptionManager m; return m;}

	inline void AddEvent(const std::string& x_name) {m_events.push_back(x_name);}
	inline void AddCommand(const Command& x_command) {m_commands.push_back(x_command);}
	inline void Reset()                      {m_events.clear(); m_commands.clear();}

	void Configure(const ConfigReader& x_config);
	std::vector<Command> ReturnCommandsToSend();



protected:

	// state
	std::vector<std::string> m_events;
	std::vector<Command>     m_commands;
	std::map<std::string, std::vector<Interruption> > m_interruptions;


private:
	static log4cxx::LoggerPtr m_logger;
};
#endif
