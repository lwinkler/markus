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

#include "Module.h"
#include "ConfigReader.h"
#include "util.h"

#include "Stream.h"
#include "Timer.h"
#include "ModuleTimer.h"


#include "ControllerModule.h"
#include "Factories.h"
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include <fstream>

using namespace std;

log4cxx::LoggerPtr Module::m_logger(log4cxx::Logger::getLogger("Module"));

Module::Module(const ConfigReader& x_configReader) :
	Processable(x_configReader),
	m_name(x_configReader.GetAttribute("name"))
{
	m_id	= atoi(x_configReader.GetAttribute("id").c_str());
	LOG_INFO(m_logger, "Create module " << m_name);

	m_timerConvertion      = 0;
	m_timerProcessing      = 0;
	m_timerWaiting         = 0;
	m_countProcessedFrames = 0;
	m_lastTimeStamp        = TIME_STAMP_MIN;
	m_currentTimeStamp     = TIME_STAMP_INITIAL;
	m_isReady              = false;
	m_unsyncWarning        = true;
	m_isUnitTestingEnabled = true;
}

Module::~Module()
{
	// Delete all streams
	for(map<int, Stream* >::iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; ++it)
		delete(it->second);
	for(map<int, Stream* >::iterator it = m_outputStreams.begin() ; it != m_outputStreams.end() ; ++it)
		delete(it->second);
#ifdef MARKUS_DEBUG_STREAMS
	for(map<int, Stream* >::iterator it = m_debugStreams.begin() ; it != m_debugStreams.end() ; ++it)
		delete(it->second);
#endif
}

/**
* @brief Reset the module. Parameters value are set to default
*/
void Module::Reset()
{
	Processable::Reset();
	LOG_INFO(m_logger, "Reseting module "<<GetName());

	// Lock the parameters that cannot be changed
	RefParameters().LockParameterByName("class");
	RefParameters().LockParameterByName("width");
	RefParameters().LockParameterByName("height");
	RefParameters().LockParameterByName("type");
	RefParameters().LockParameterByName("auto_process");
	RefParameters().LockParameterByName("allow_unsync_input");

	const Parameters& param(GetParameters());
	param.PrintParameters();
	param.CheckRange(true);

	// This must be done only once to avoid troubles in the GUI
	// Add module controller
	if(FindController("module") == NULL)
		AddController(new ControllerModule(*this));

	for(const auto& elem : param.GetList())
	{
		// Do not add param if locked or already present
		// TODO: Suppress GetType() and use a CreateController method
		if(elem->IsLocked() || FindController(elem->GetName()) != NULL)
			continue;
		Controller* ctr = Factories::parameterControllerFactory().Create(elem->GetType(), *elem);
		if(ctr == NULL)
			throw MkException("Controller creation failed", LOC);
		else AddController(ctr);
	}

	if(GetParameters().cached == CachedState::WRITE_CACHE)
		 SYSTEM("mkdir -p " + m_context.GetOutputDir() + "/cache/");
}

/**
* @brief Return the fps that can be used for recording. This value is special as it depends from preceeding modules.
*
* @return fps
*/
double Module::GetRecordingFps() const
{
	double fps = GetParameters().fps;
	bool autop = GetParameters().autoProcess;

	if(autop)
	{
		// If the module is autoprocessed then the FPS is determining
		if(fps == 0)
			throw MkException("FPS cannot be equal to zero in module " + GetName(), LOC);
		return fps;
	}
	else
	{
		// If the module is not autoprocessed then the FPS is given by the previous module
		// Note: we assume that the fps is given by the first stream in the module
		if(m_inputStreams.empty())
			throw MkException("This module must have at least one input stream", LOC);
		if(fps == 0)
		{
			Stream * stream = m_inputStreams.at(0);
			if(stream == NULL)
				throw MkException("First stream is null", LOC);
			stream = &(stream->GetConnected());
			if(stream == NULL)
				throw MkException("Connected stream is null in Module::GetRecordingFps", LOC);
			return stream->GetModule().GetRecordingFps();
		}
		else
		{
			// estimate the fps to the min of input and current
			return MIN(fps, m_inputStreams.at(0)->GetConnected().GetModule().GetRecordingFps());
		}
	}

}

/**
* @brief Process one frame
*/
bool Module::Process()
{
	LockForWrite();
	if(m_pause)
	{
		Unlock();
		return true;
	}
	try
	{
		if(!m_isReady)
			throw MkException("Module must be ready before processing", LOC);

		const Parameters& param = GetParameters();

		// Timestamp of the module is given by the input stream
		m_currentTimeStamp = 0;
		if(!m_inputStreams.empty())
		{
			// m_inputStreams[0]->LockModuleForRead();
			m_currentTimeStamp = m_inputStreams[0]->GetTimeStampConnected(); // TODO: should we lock module here ?
			// m_inputStreams[0]->UnLockModule();
		}
		else if(! param.autoProcess)
			throw MkException("Module must have at least one input or have parameter auto_process=true", LOC);

		// TODO if(m_currentTimeStamp == m_lastTimeStamp)
		// TODO LOG_WARN(m_logger, "Timestamp are not increasing correctly");
		if(param.autoProcess || (param.fps == 0 && m_currentTimeStamp != m_lastTimeStamp) || (m_currentTimeStamp - m_lastTimeStamp) * param.fps > 1000)
		{
			// Process this frame

			// Timer for benchmark
			Timer ti;

			// cout<<GetName()<<" "<<m_currentTimeStamp<<" "<<m_lastTimeStamp<<endl;

			// Check that all inputs are in sync
			if(! param.allowUnsyncInput && m_unsyncWarning)
				for(unsigned int i = 1 ; i < m_inputStreams.size() ; i++)
				{
					Stream& stream(*m_inputStreams.at(i));
					if(stream.IsConnected() && stream.GetTimeStampConnected() != m_currentTimeStamp)
					{
						LOG_WARN(m_logger, "Input stream id="<<i<<" is not in sync with input stream id=0 for module "<<GetName()<<". If this is acceptable set parameter allow_unsync_input=1 for this module. To fix this problem cleanly you should probably set parameters auto_process=0 and fps=0 for the modules located between the input and module "<<GetName()<<".");
						m_unsyncWarning = false;
					}
				}

			// note: Inputs must call ProcessFrame to set the time stamp
			// TODO: There is no reason to cache input modules !
			if(param.cached < CachedState::READ_CACHE || IsInput())
			{
				// Read and convert inputs
				if(IsInputProcessed())
				{
					for(map<int, Stream*>::iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; ++it)
					{
						Timer ti2;
						//(*it)->LockModuleForRead();
						m_timerWaiting += ti2.GetMSecLong();
						it->second->ConvertInput();
						//(*it)->UnLockModule();
					}
					//assert(m_currentTimeStamp == m_inputStreams[0]->GetTimeStamp());
				}
				m_timerConvertion 	+= ti.GetMSecLong();
				ti.Restart();

				ProcessFrame();

				m_timerProcessing 	 += ti.GetMSecLong();
			}
			if(param.cached == CachedState::READ_CACHE)
			{
				// TODO: check that module is not an input
				ti.Restart();
				ReadFromCache();
				m_timerProcessing 	 += ti.GetMSecLong();
			}

			// Propagate time stamps to outputs
			for(map<int, Stream*>::iterator it = m_outputStreams.begin() ; it != m_outputStreams.end() ; ++it)
				it->second->SetTimeStamp(m_currentTimeStamp);

			// Write outputs to cache
			if(param.cached == CachedState::WRITE_CACHE)
			{
				WriteToCache();
			}

			// Call depending modules (modules with fps = 0)
			for(vector<Module*>::iterator it = m_modulesDepending.begin() ; it != m_modulesDepending.end() ; ++it)
				(*it)->Process();

			m_countProcessedFrames++;
			m_lastTimeStamp = m_currentTimeStamp;
		}
	}
	catch(...)
	{
		LOG_WARN(m_logger, "Exception in module " << GetName());
		Unlock();
		throw;
	}
	Unlock();
	return true;
}

/**
* @brief Describe the module with name, parameters, inputs, outputs to an output stream (in xml)
*
* @param rx_os         Output stream
* @param x_indentation Number of tabs in indentation
*/
void Module::Export(ostream& rx_os, int x_indentation)
{
	string tabs(x_indentation, '\t');
	tabs = string(x_indentation, '\t');
	rx_os<<tabs<<"<module name=\""<<m_name<<"\" description=\""<<GetDescription()<<"\">"<<endl;
	tabs = string(x_indentation + 1, '\t');
	rx_os<<tabs<<"<parameters>"<<endl;
	for(vector<Parameter*>::const_iterator it = GetParameters().GetList().begin() ; it != GetParameters().GetList().end() ; ++it)
		(*it)->Export(rx_os, x_indentation + 2);
	rx_os<<tabs<<"</parameters>"<<endl;

	rx_os<<tabs<<"<inputs>"<<endl;
	for(map<int, Stream*>::const_iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; ++it)
		it->second->Export(rx_os, it->first, x_indentation + 2, true);
	rx_os<<tabs<<"</inputs>"<<endl;

	rx_os<<tabs<<"<outputs>"<<endl;
	for(map<int, Stream*>::const_iterator it = m_outputStreams.begin() ; it != m_outputStreams.end() ; ++it)
		it->second->Export(rx_os, it->first, x_indentation + 2, false);
	rx_os<<tabs<<"</outputs>"<<endl;
	tabs = string(x_indentation, '\t');
	rx_os<<tabs<<"</module>"<<endl;
}

Stream& Module::RefInputStreamById(int x_id)
{
	//for(vector<Stream *>::const_iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; ++it)
	//if((*it)->GetId() == x_id) return **it;
	map<int, Stream*>::iterator it = m_inputStreams.find(x_id);

	if(it == m_inputStreams.end())
	{
		stringstream ss;
		ss<<"GetInputStreamById : no stream with id="<<x_id<<" for module "<<GetName();
		throw MkException(ss.str(), LOC);
	}
	return *(it->second);
}

Stream& Module::RefOutputStreamById(int x_id)
{
	map<int, Stream*>::iterator it = m_outputStreams.find(x_id);

	if(it == m_outputStreams.end())
	{
		stringstream ss;
		ss<<"GetInputStreamById : no stream with id="<<x_id<<" for module "<<GetName();
		throw MkException(ss.str(), LOC);
	}
	return *(it->second);
}

/**
* @brief Print all statistics related to the module
*/
void Module::PrintStatistics(ConfigReader& xr_xmlResult) const
{
	double fps = (m_countProcessedFrames * 1000.0 / (m_timerProcessing + m_timerConvertion + m_timerWaiting));
	LOG_INFO(m_logger, "Module "<<GetName()<<" : "<<m_countProcessedFrames<<" frames processed (tproc="<<
			 m_timerProcessing<<"ms, tconv="<<m_timerConvertion<<"ms, twait="<<
			 m_timerWaiting<<"ms), "<< fps <<" fps");

	// Write perf to output XML
	ConfigReader perfModule = xr_xmlResult.FindRef("module[name=\"" + GetName() + "\"]", true);
	perfModule.FindRef("nb_frames", true).SetValue(m_countProcessedFrames);
	perfModule.FindRef("timer[name=\"processing\"]", true).SetValue(m_timerProcessing);
	perfModule.FindRef("timer[name=\"conversion\"]", true).SetValue(m_timerConvertion);
	perfModule.FindRef("timer[name=\"waiting\"]", true).SetValue(m_timerWaiting);
	perfModule.FindRef("fps", true).SetValue(fps);
}

/**
* @brief Serialize the stream content to a directory
*
* @param x_out Output stream
* @param x_dir Directory (for images)
*/
void Module::Serialize(ostream& x_out, const string& x_dir) const
{
	Json::Value root;

	root["id"]                   = m_id;
	root["name"]                 = m_name;
	root["pause"]                = m_pause;
	root["timerConvertion"]      = m_timerConvertion;
	root["timerProcessing"]      = m_timerProcessing;
	root["timerWaiting"]         = m_timerWaiting;
	root["countProcessedFrames"] = m_countProcessedFrames;

	// Dump inputs
	for(map<int, Stream*>::const_iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; ++it)
	{
		stringstream ss;
		it->second->Serialize(ss, x_dir);
		ss >> root["inputs"][it->first];
	}
	// Dump outputs
	for(map<int, Stream*>::const_iterator it = m_outputStreams.begin() ; it != m_outputStreams.end() ; ++it)
	{
		stringstream ss;
		it->second->Serialize(ss, x_dir);
		ss >> root["outputs"][it->first];
	}
#ifdef MARKUS_DEBUG_STREAMS
	// Dump debugs
	for(map<int, Stream*>::const_iterator it = m_debugStreams.begin() ; it != m_debugStreams.end() ; ++it)
	{
		stringstream ss;
		it->second->Serialize(ss, x_dir);
		ss >> root["debugs"][it->first];
	}
#endif
	x_out << root;
}

/**
* @brief Deserialize the module from JSON
*
* @param x_in  Input stream
* @param x_dir Input dir (for images)
*/
void Module::Deserialize(istream& x_in, const string& x_dir)
{
	Json::Value root;
	x_in >> root;

	m_id                   = root["id"].asInt();
	m_name                 = root["name"].asString();
	m_pause                = root["pause"].asBool();
	m_timerConvertion      = root["timerConvertion"].asInt64();
	m_timerProcessing      = root["timerProcessing"].asInt64();
	m_timerWaiting         = root["timerWaiting"].asInt64();
	m_countProcessedFrames = root["countProcessedFrames"].asInt64();

	stringstream ss;

	// read inputs
	unsigned int size1 = root["inputs"].size();
	assert(size1 == m_inputStreams.size());
	for(unsigned int i = 0 ; i < size1 ; i++)
	{
		ss.clear();
		ss << root["inputs"][i];
		m_inputStreams[i]->Deserialize(ss, x_dir);
	}

	// read outputs
	size1 = root["outputs"].size();
	assert(size1 == m_outputStreams.size());
	for(unsigned int i = 0 ; i < size1 ; i++)
	{
		ss.clear();
		ss << root["outputs"][i];
		m_outputStreams[i]->Deserialize(ss, x_dir);
	}

#ifdef MARKUS_DEBUG_STREAMS
	// read debug streams
	size1 = root["debugs"].size();
	assert(size1 == m_debugStreams.size());
	for(unsigned int i = 0 ; i < size1 ; i++)
	{
		ss.clear();
		ss << root["debugs"][i];
		m_debugStreams[i]->Deserialize(ss, x_dir);
	}
#endif
}

/**
* @brief Check that all inputs are ready (they are connected to a working module)
*
* @return True if all are ready
*/
bool Module::AllInputsAreReady() const
{
	for(map<int, Stream*>::const_iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; ++it)
	{
		if(it->second->IsConnected() && !it->second->GetConnected().IsReady())
			return false;
	}
	return true;
}

/**
* @brief Return a reference to the master module. The master module is the preceeding module that is
 	 responsible for calling the process method. The first connected input determines what module is master.
*
* @return The master module
*/
const Module& Module::GetMasterModule() const
{
	for(map<int, Stream*>::const_iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; ++it)
	{
		if(it->second->IsConnected())
		{
			const Module& preceding = it->second->GetConnected().GetModule();
			if(preceding.IsAutoProcessed())
				return preceding;
			else
				return preceding.GetMasterModule();
		}
	}
	throw MkException("Module must have at least one input connected or have auto_process=1", LOC);
}

/**
* @brief Set as ready (and all inputs too)
*/
void Module::SetAsReady()
{
	m_isReady = true;
	for(map<int, Stream*>::iterator it = m_outputStreams.begin() ; it != m_outputStreams.end() ; ++it)
	{
		it->second->SetAsReady();
	}
}


/**
* @brief Add an input stream
*
* @param x_id      id
* @param xp_stream stream
*/
void Module::AddInputStream(int x_id, Stream* xp_stream)
{
	// xp_stream->SetId(x_id);
	if(m_inputStreams.find(x_id) != m_inputStreams.end())
		throw MkException("Two streams with same id", LOC);
	m_inputStreams.insert(make_pair(x_id, xp_stream));
}

/// Add an output stream
/**
* @brief Add a debug stream
*
* @param x_id      id
* @param xp_stream stream
*/
void Module::AddOutputStream(int x_id, Stream* xp_stream)
{
	// xp_stream->SetId(x_id);
	if(m_outputStreams.find(x_id) != m_outputStreams.end())
		throw MkException("Two streams with same id", LOC);
	m_outputStreams.insert(make_pair(x_id, xp_stream));
}

/**
* @brief Add a debug stream
*
* @param x_id      id
* @param xp_stream stream
*/
void Module::AddDebugStream(int x_id, Stream* xp_stream)
{
	// xp_stream->SetId(x_id);
	if(m_debugStreams.find(x_id) != m_debugStreams.end())
		throw MkException("Two streams with same id", LOC);
	m_debugStreams.insert(make_pair(x_id, xp_stream));
}


/**
* @brief Write output stream to cache directory
*
*/
void Module::WriteToCache() const
{
	// Write output stream to cache
	for(const auto &elem : m_outputStreams)
	{
		if(!elem.second->IsConnected()) continue;
		string directory = m_context.GetOutputDir() + "/cache/";
		stringstream fileName;
		fileName << directory << GetName() << "." << elem.second->GetName() << "." << m_currentTimeStamp << ".json";
		ofstream of;
		of.open(fileName.str().c_str());
		elem.second->Serialize(of, directory);
		of.close();
	}
}

/**
* @brief Read output stream from cache directory
*
*/
void Module::ReadFromCache()
{
	// Read output stream from cache
	for(auto elem : m_outputStreams)
	{
		if(!elem.second->IsConnected()) continue;
		string directory = "cache/"; // TODO fix this m_context.GetOutputDir() + "/cache/";
		stringstream fileName;
		fileName << directory << GetName() << "." << elem.second->GetName() << "." << m_currentTimeStamp << ".json";
		ifstream ifs;
		ifs.open(fileName.str().c_str());
		if(!ifs.good()) // TODO: Check that all files are open correctly !!
			throw MkException("Error while reading from stream cache: " + fileName.str(), LOC);
		elem.second->Deserialize(ifs, directory);
		ifs.close();
	}
}

/**
* @brief Randomize all inputs and process (unit testing only)
*
* @param xr_seed      Seed for randomization
*/
// TODO: Add an ifdef for unit tests
void Module::ProcessRandomInput(unsigned int& xr_seed)
{
	unsigned int lastseed = xr_seed;
	for(map<int, Stream* >::iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; ++it)
	{
		// note: we use this trick to generate similar inputs
		// this avoids many errors while unit testing comparison modules
		xr_seed = lastseed;
		it->second->Randomize(xr_seed);
	}
	ProcessFrame();
};

const map<string, int> Module::ParameterCachedState::Enum = {
	{"NO_CACHE", CachedState::NO_CACHE},
	{"WRITE_CACHE", CachedState::WRITE_CACHE},
	{"READ_CACHE", CachedState::READ_CACHE},
	{"DISABLED", CachedState::DISABLED}
};
const map<int, string> Module::ParameterCachedState::ReverseEnum = Module::ParameterCachedState::CreateReverseMap(ParameterCachedState::Enum);


Module::ParameterCachedState::ParameterCachedState(const string& x_name, int x_default, int * xp_value, const string& x_description) :
	ParameterEnum(x_name, x_default, xp_value, x_description)
{}
