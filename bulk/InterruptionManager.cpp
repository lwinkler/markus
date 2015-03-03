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
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

#include "MkException.h"
#include "util.h"
#include "InterruptionManager.h"
#include "Manager.h"

using namespace std;

log4cxx::LoggerPtr InterruptionManager::m_logger(log4cxx::Logger::getLogger("InterruptionManager"));


InterruptionManager::InterruptionManager()
{
}

/// Configure interruptions from config
void InterruptionManager::Configure(const ConfigReader& x_config)
{
	m_interruptions.clear();
	for(auto config : x_config.FindAll("interruptions>interruption"))
	{
		Interruption inter(Command(config.GetAttribute("command"), config.GetAttribute("value")), -1);
		try
		{
			inter.remaining = boost::lexical_cast<int>(config.GetAttribute("count"));
		}
		catch(...){}
		m_interruptions[config.GetAttribute("event")].push_back(inter);
		cout<<"Add interruption for "<<config.GetAttribute("event")<<" --> "<<config.GetAttribute("value")<<endl;
	}
}


/// Send all commands generated from interruptions
vector<Command> InterruptionManager::ReturnCommandsToSend()
{
	vector<Command> commands;
	for(const auto& event : m_events)
	{
		auto it = m_interruptions.find(event);
		if(it != m_interruptions.end()) // && it->second.remaining != 0)
		{
			// Add all command in this slot to Return vector and decrement
			for(auto& interruption : it->second)
			{
				commands.push_back(interruption.command);
				if(interruption.remaining > 0)
					interruption.remaining--;
			}
		}
	}
	m_events.clear();
	return commands;
}
