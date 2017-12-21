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

#include <boost/lexical_cast.hpp>

#include "MkException.h"
#include "util.h"
#include "InterruptionManager.h"
#include "Manager.h"

namespace mk {
using namespace std;

log4cxx::LoggerPtr InterruptionManager::m_logger(log4cxx::Logger::getLogger("InterruptionManager"));


InterruptionManager::InterruptionManager()
{
}

/// Configure interruptions from config
void InterruptionManager::Configure(const mkconf& x_config)
{
	m_interruptions.clear();
	if(x_config.find("interruptions") != x_config.end()) {
		for(const auto& config : x_config.at("interruptions"))
		{
			Interruption inter(Command(
				config.at("command").get<string>(),
				config.value<string>("value", "")),
				boost::lexical_cast<int>(config.value<string>("nb", "-1"))
			);
			string event = config.at("event").get<string>();
			m_interruptions[event].push_back(inter);
			LOG_INFO(m_logger, "Add interruption for "<<event<<" --> "<<inter.command.name<<"="<<inter.command.value<<" "<<inter.remaining<<" time(s)");
		}
	}
}


/// Send all commands generated from interruptions
vector<Command> InterruptionManager::ReturnCommandsToSend()
{
	vector<Command> commands = m_commands;
	for(const auto& event : m_events)
	{
		auto it = m_interruptions.find(event);
		if(it != m_interruptions.end()) // && it->second.remaining != 0)
		{
			// Add all command in this slot to Return vector and decrement
			for(auto& inter : it->second)
			{
				if(inter.remaining == 0)
					continue;
				commands.push_back(inter.command);
				if(inter.remaining > 0)
					inter.remaining--;
				LOG_DEBUG(m_logger, "Return interruption for "<<event<<" --> "<<inter.command.name<<"="<<inter.command.value);
			}
		}
	}
	if(!commands.empty())
		LOG_INFO(m_logger, "Returning " << commands.size() << " interruptions");
	m_events.clear();
	m_commands.clear();
	return commands;
}
} // namespace mk
