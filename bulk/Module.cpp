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

Module::Module(ParameterStructure& xr_params) :
	Processable(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params))
{
	LOG_INFO(m_logger, "Create module " << GetName());
}

Module::~Module()
{
	// Delete all streams
	// note: input streams will be destroyed as parameters
	for(auto & elem : m_inputStreams)
	{
		if(elem.second->GetParameterType() == PARAM_UNKNOWN)
			delete(elem.second);
	}
	m_inputStreams.clear();
	for(auto & elem : m_outputStreams)
		delete(elem.second);
	m_outputStreams.clear();
}

/**
* @brief Reset the module. Parameters value are set to default
*/
void Module::Reset()
{
	if(GetName().empty())
		throw MkException("Module of class " + GetClass() + " has an empty name", LOC);
	LOG_INFO(m_logger, "Reseting module "<<GetName());
	Processable::Reset();
	m_nbReset++;

	// note: The input streams must never be reset since this would be incoherent with the 
	//       behavior of parameters. Reseting them would erase the current value and set them to default.
	// for(auto& stream : m_inputStreams)
		// stream.second->Reset();
	for(auto& stream : m_outputStreams)
		stream.second->Reset();

	m_timerProcessFrame.Reset();
	m_timerWaiting.Reset();
	m_timerConversion.Reset();

	// This must be done only once to avoid troubles in the GUI
	// Add module controller
	if(!HasController("module"))
		AddController(new ControllerModule(*this));

	for(const auto& elem : m_param.GetList())
	{
		// Do not add param if locked or already present
		if(elem->IsLocked() || HasController(elem->GetName()))
			continue;
		Controller* ctr = Factories::parameterControllerFactory().Create(elem->GetParameterType(), *elem, *this);
		if(ctr == nullptr)
			throw MkException("Controller creation failed", LOC);
		else AddController(ctr);
	}
}

/**
* @brief Return the fps that can be used for recording. This value is special as it depends from preceeding modules.
*
* @return fps
*/
double Module::GetRecordingFps() const
{
	if(m_param.autoProcess)
	{
		// If the module is autoprocessed then the FPS is determining
		if(m_param.fps == 0)
			throw MkException("FPS cannot be equal to zero in module " + GetName(), LOC);
		return m_param.fps;
	}
	else
	{
		// If the module is not autoprocessed then the FPS is given by the previous module
		// Note: we assume that the fps is given by the first stream in the module
		if(m_inputStreams.empty())
			throw MkException("This module must have at least one input stream", LOC);
		if(m_param.fps == 0)
		{
			Stream * stream = m_inputStreams.at(0);
			if(stream == nullptr)
				throw MkException("First stream is null", LOC);
			stream = &(stream->GetConnected());
			if(stream == nullptr)
				throw MkException("Connected stream is null in Module::GetRecordingFps", LOC);
			return stream->GetModule().GetRecordingFps();
		}
		else
		{
			// estimate the fps to the min of input and current
			return MIN(m_param.fps, m_inputStreams.at(0)->GetConnected().GetModule().GetRecordingFps());
		}
	}

}

/**
* @brief Return true if all conditions for processing are met (all blocking inputs have received a frame, all are syncronized)
*
* note: We need this kind of condition since all preceeding modules will call Process
*/
bool Module::ProcessingCondition() const
{
	TIME_STAMP syncTs = TIME_STAMP_MIN;
	for(const auto& elem : m_inputStreams)
	{
		if(!elem.second->IsConnected())
			continue;
		TIME_STAMP ts = elem.second->GetTimeStampConnected();
		LOG_DEBUG(m_logger, GetName() << ": Timestamp of input stream " << elem.second->GetName() << ":" << ts << ", last:" << m_lastTimeStamp << " " << elem.second->IsBlocking() << "," << elem.second->IsSynchronized());
		if(elem.second->IsBlocking())
		{
			if(ts == TIME_STAMP_MIN) // note: handle the case where the previous module has not processed
				return false;
			if(m_lastTimeStamp != TIME_STAMP_MIN)
			{
				// Handle the case where a rewind occured
				if(ts < m_lastTimeStamp)
				{
					LOG_DEBUG(m_logger, "Rewind detected");
					return true;
				}
				// note : condition ts == m_lastTimeStamp is here to handle special cases such as ReadObject
				if(ts == m_lastTimeStamp || (m_param.fps != 0 && (ts - m_lastTimeStamp) * m_param.fps < 1000))
					return false;
			}
		}
		if(elem.second->IsSynchronized())
		{
			if(syncTs == TIME_STAMP_MIN)
				syncTs = ts;
			else if(ts != syncTs)
				return false;
		}
	}
	return true;
}

/**
* @brief Return the timestamp to use
*
*/
void Module::ComputeCurrentTimeStamp()
{
	// Return the timestamp of the first blocking input
	for(const auto& elem : m_inputStreams)
	{
		if(elem.second->IsConnected() && elem.second->IsBlocking())
		{
			m_currentTimeStamp = elem.second->GetTimeStampConnected();
			if(m_currentTimeStamp != TIME_STAMP_MIN)
				return;
		}
	}
	if(!IsAutoProcessed())
		throw MkException("Only an autoprocessed (or input) module can have no connected input. Module " + GetName(), LOC);
	m_currentTimeStamp = TIME_STAMP_MIN;
}

/**
* @brief Process one frame
* @return True if the frame was processed
*/
void Module::Process()
{
	m_timerWaiting.Start();
	WriteLock lock(RefLock());
	try
	{
		if(!m_param.autoProcess && !ProcessingCondition())
			return;
		ComputeCurrentTimeStamp();

		// Process this frame

		// Timer for benchmark
		m_timerWaiting.Stop();

		// note: Inputs must call ProcessFrame to set the time stamp
		if(m_param.cached < CachedState::READ_CACHE || IsInput())
		{
			m_timerConversion.Start();
			// Read and convert inputs
			if(IsInputProcessed())
			{
				for(auto & elem : m_inputStreams)
				{
					Timer ti2;
					elem.second->ConvertInput();
				}
			}
			m_timerConversion.Stop();
			m_timerProcessFrame.Start();

			ProcessFrame();

			m_timerProcessFrame.Stop();
		}
		else if(m_param.cached == CachedState::READ_CACHE)
		{
			m_timerProcessFrame.Start();
			ReadFromCache();
			m_timerProcessFrame.Stop();
		}

		// Propagate time stamps to outputs
		for(auto & elem : m_outputStreams)
			elem.second->SetTimeStamp(m_currentTimeStamp);

		// Write outputs to cache
		if(m_param.cached == CachedState::WRITE_CACHE)
		{
			WriteToCache();
		}

		// Call depending modules (modules with fps = 0)
		if(PropagateCondition())
		{
			/*
			vector<thread> threads;
			for(auto & elem : m_modulesDepending)
			{
				threads.emplace_back([&elem]{
					elem->Process();
				});
			}
			for(auto& thread : threads)
				thread.join();
			*/
			for(auto & elem : m_modulesDepending)
			{
				elem->Process();
			}
		}
		else LOG_DEBUG(m_logger, "No propagation of processing to depending modules");

#ifdef MARKUS_DEBUG_STREAMS
		if(m_nbReset == UINT16_MAX)
		{
			LOG_ERROR(m_logger, "Module " << GetName() << " was never reseted. This should never happen");
		}
		if(m_currentTimeStamp == TIME_STAMP_MIN)
			LOG_WARN(m_logger, "Module " << GetName() << " must set a timestamp!");
		if(m_currentTimeStamp == m_lastTimeStamp)
			LOG_WARN(m_logger, "Timestamp are not increasing correctly in " << GetName());
#endif

		m_countProcessedFrames++;
		m_lastTimeStamp = m_currentTimeStamp;
	}
	catch(...)
	{
		m_timerWaiting.Stop();
		m_timerConversion.Stop();
		m_timerProcessFrame.Stop();
		m_lastTimeStamp = m_currentTimeStamp;
		LOG_INFO(m_logger, "Exception in module " << GetName());
		throw;
	}
}

/**
* @brief Describe the module with name, parameters, inputs, outputs to an output stream (in xml)
*
* @param rx_os         Output stream
*/
void Module::Export(ostream& rx_os) const
{
	Json::Value root;
	root["class"]       = GetClass();
	root["name"]        = GetName();
	root["description"] = GetDescription();

	for(const auto & elem : m_param.GetList())
	{
		if(elem->GetName().find('_') != string::npos)
			throw MkException("Forbidden character _ in " + elem->GetName(), LOC);
		stringstream ss;
		elem->Export(ss);
		ss >> root["inputs"][elem->GetName()];
	}
	for(const auto& elem : m_inputStreams)
	{
		if(elem.second->GetName().find('_') != string::npos)
			throw MkException("Forbidden character _ in " + elem.second->GetName(), LOC);
		// TODO: parameters are now inputs
		stringstream ss;
		elem.second->Export(ss);
		ss >> root["inputs"][elem.second->GetName()];
	}
	for(const auto& elem : m_outputStreams)
	{
		if(elem.second->GetName().find('_') != string::npos)
			throw MkException("Forbidden character _ in " + elem.second->GetName(), LOC);
		if(!elem.second->debug)
		{
			stringstream ss;
			elem.second->Export(ss);
			ss >> root["outputs"][elem.second->GetName()];
		}
	}
	rx_os << root;
}

const Stream& Module::GetInputStreamByName(const string& x_name) const
{
	auto it = m_inputStreams.find(x_name);

	if(it == m_inputStreams.end())
	{
		stringstream ss;
		ss<<"GetInputStreamByName : no stream with name="<<x_name<<" for module "<<GetName();
		throw MkException(ss.str(), LOC);
	}
	return *(it->second);
}

const Stream& Module::GetOutputStreamByName(const string& x_name) const
{
	auto it = m_outputStreams.find(x_name);

	if(it == m_outputStreams.end())
	{
		stringstream ss;
		ss<<"GetInputStreamByName : no stream with name="<<x_name<<" for module "<<GetName();
		throw MkException(ss.str(), LOC);
	}
	return *(it->second);
}

Stream& Module::RefInputStreamByName(const string& x_name)
{
	auto it = m_inputStreams.find(x_name);

	if(it == m_inputStreams.end())
	{
		stringstream ss;
		ss<<"GetInputStreamByName : no stream with name="<<x_name<<" for module "<<GetName();
		throw MkException(ss.str(), LOC);
	}
	return *(it->second);
}

Stream& Module::RefOutputStreamByName(const string& x_name)
{
	auto it = m_outputStreams.find(x_name);

	if(it == m_outputStreams.end())
	{
		stringstream ss;
		ss<<"GetInputStreamByName : no stream with name="<<x_name<<" for module "<<GetName();
		throw MkException(ss.str(), LOC);
	}
	return *(it->second);
}

/**
* @brief Print all statistics related to the module
*/
void Module::PrintStatistics(ConfigReader& xr_xmlResult) const
{
	double fps = (m_countProcessedFrames / (m_timerProcessFrame.GetSecDouble() + m_timerConversion.GetSecDouble() + m_timerWaiting.GetSecDouble()));
	LOG_INFO(m_logger, "Module "<<GetName()<<": "<<m_countProcessedFrames<<" frames processed (tproc="<<
			 m_timerProcessFrame.GetMsLong()<<"ms, tconv="<<m_timerConversion.GetMsLong()<<"ms, twait="<<
			 m_timerWaiting.GetMsLong()<<"ms), "<< fps <<" fps");
	if(m_timerProcessable.GetMsLong() > 0)
		LOG_INFO(m_logger, "Module "<<GetName()<<": Time spent in current and depending module: " << m_timerProcessable.GetMsLong() << " ms");

	// Write perf to output XML
	ConfigReader& perfModule(xr_xmlResult["module"][GetName()]);
	perfModule["nb_frames"]             = Json::UInt64(m_countProcessedFrames);
	perfModule["timers"]["processable"] = Json::UInt64(m_timerProcessable.GetMsLong());
	perfModule["timers"]["processing"]  = Json::UInt64(m_timerProcessFrame.GetMsLong());
	perfModule["timers"]["conversion"]  = Json::UInt64(m_timerConversion.GetMsLong());
	perfModule["timers"]["waiting"]     = Json::UInt64(m_timerWaiting.GetMsLong());
	perfModule["fps"]                   = fps;
}

/**
* @brief Serialize the stream content to a directory
*
* @param x_out Output stream
* @param xp_dir MkDirectory (for images)
*/
void Module::Serialize(ostream& x_out, MkDirectory* xp_dir) const
{
	Json::Value root;

	root["name"]                 = GetName();
	// root["timer_conversion"]      = m_timerConversion.GetMsLong();
	// root["timer_processing"]      = m_timerProcessFrame.GetMsLong();
	// root["timer_waiting"]         = m_timerWaiting.GetMsLong();
	root["countProcessedFrames"] = Json::UInt64(m_countProcessedFrames);

	// Dump inputs
	for(const auto & elem : m_inputStreams)
	{
		stringstream ss;
		elem.second->Serialize(ss, xp_dir);
		ss >> root["inputs"][elem.first];
	}
	// Dump outputs
	for(const auto & elem : m_outputStreams)
	{
		stringstream ss;
		elem.second->Serialize(ss, xp_dir);
		ss >> root["outputs"][elem.first];
	}
	x_out << root;
}

/**
* @brief Deserialize the module from JSON
*
* @param x_in  Input stream
* @param xp_dir Input dir (for images)
*/
void Module::Deserialize(istream& x_in, MkDirectory* xp_dir)
{
	Json::Value root;
	x_in >> root;

	if(GetName() != root["name"].asString())
		throw MkException("Name does not match in serialization for module " + GetName(), LOC);
	m_countProcessedFrames = root["countProcessedFrames"].asInt64();

	stringstream ss;

	// read inputs
	unsigned int size1 = root["inputs"].size();
	assert(size1 == m_inputStreams.size());
	for(auto& elem : m_inputStreams)
	{
		ss.clear();
		ss << root["inputs"][elem.first];
		m_inputStreams[elem.first]->Deserialize(ss, xp_dir);
	}

	// read outputs
	size1 = root["outputs"].size();
	assert(size1 == m_outputStreams.size());
	for(auto& elem : m_outputStreams)
	{
		ss.clear();
		ss << root["outputs"][elem.first];
		m_outputStreams[elem.first]->Deserialize(ss, xp_dir);
	}
}

void Module::AddInputStream(int x_id, Stream* xp_stream){AddInputStream(xp_stream);xp_stream->id = x_id;}
void Module::AddOutputStream(int x_id, Stream* xp_stream){AddOutputStream(xp_stream);xp_stream->id = x_id;}
void Module::AddDebugStream(int x_id, Stream* xp_stream){AddDebugStream(xp_stream);xp_stream->id = x_id;}

/**
* @brief Add an input stream
*
* @param x_id      id
* @param xp_stream stream
*/
void Module::AddInputStream(Stream* xp_stream)
{
	m_inputStreams.insert(make_pair(xp_stream->GetName(), xp_stream));
	if(xp_stream->GetParameterType() != PARAM_UNKNOWN)
	{
		m_param.AddParameter(xp_stream);
	}
}

/**
* @brief Add an output stream
*
* @param x_id      id
* @param xp_stream stream
*/
void Module::AddOutputStream(Stream* xp_stream)
{
	m_outputStreams.insert(make_pair(xp_stream->GetName(), xp_stream));
}

/**
* @brief Add a debug stream
*
* @param x_id      id
* @param xp_stream stream
*/
void Module::AddDebugStream(Stream* xp_stream)
{
#ifdef MARKUS_DEBUG_STREAMS
	// Simply add an output with a id increased by 1000
	AddOutputStream(xp_stream);
	xp_stream->debug = true;
#endif
}


/**
* @brief Write output stream to cache directory
*
*/
void Module::WriteToCache() const
{
	// Read output stream from cache
	// Write output stream to cache
	MkDirectory& dir(RefContext().RefCacheOut());
	for(const auto &elem : m_outputStreams)
	{
		if(!elem.second->IsConnected()) continue;
		stringstream fileName;
		fileName << GetName() << "." << elem.second->GetName() << "." << m_currentTimeStamp << ".json";
		ofstream of(dir.ReserveFile(fileName.str()));
		if(!of.good())
			throw MkException("Error while writing to cache: " + fileName.str(), LOC);
		elem.second->Serialize(of, &dir);
	}
}

/**
* @brief Read output stream from cache directory
*
*/
void Module::ReadFromCache()
{
	MkDirectory& dir(RefContext().RefCacheIn());
	// Read output stream from cache
	for(auto& elem : m_outputStreams)
	{
		if(!elem.second->IsConnected()) continue;
		stringstream fileName;
		fileName << GetName() << "." << elem.second->GetName() << "." << m_currentTimeStamp << ".json";
		ifstream ifs;
		ifs.open(dir.ReserveFile(fileName.str()));
		if(!ifs.good())
			throw MkException("Error while reading from cache: " + fileName.str(), LOC);
		elem.second->Deserialize(ifs, &dir);
		ifs.close();
	}
}

/**
* @brief Randomize all inputs and process (unit testing only)
*
* @param xr_seed      Seed for randomization
*/
void Module::ProcessRandomInput(unsigned int& xr_seed)
{
	unsigned int lastseed = xr_seed;
	for(auto & elem : m_inputStreams)
	{
		// note: we use this trick to generate similar inputs
		// this avoids many errors while unit testing comparison modules
		xr_seed = lastseed;
		elem.second->Randomize(xr_seed);
	}
	if(m_currentTimeStamp == TIME_STAMP_MIN)
		m_currentTimeStamp = 0;
	else m_currentTimeStamp += 1 + static_cast<float>(rand_r(&xr_seed)) / RAND_MAX;
	ProcessFrame();
};

/**
* @brief Remove a module from depending
*
* @param x_module Pointer to module
*/
void Module::RemoveDependingModule(const Module & x_module)
{
	auto it = find(m_modulesDepending.begin(), m_modulesDepending.end(), &x_module);
	if(it == m_modulesDepending.end())
	{
		throw MkException("Module " + x_module.GetName() + " not found in depending modules of " + GetName(), LOC);
	}
	else
	{
		LOG_DEBUG(m_logger, "Remove depending module " << &x_module << " from " << GetName());
		m_modulesDepending.erase(it);
	}
}

/// Static members of ParameterCachedState
const map<string, int> Module::ParameterCachedState::Enum =
{
	{"NO_CACHE", CachedState::NO_CACHE},
	{"WRITE_CACHE", CachedState::WRITE_CACHE},
	{"READ_CACHE", CachedState::READ_CACHE},
	{"DISABLED", CachedState::DISABLED}
};
const map<int, string> Module::ParameterCachedState::ReverseEnum = ParameterEnum::CreateReverseMap(ParameterCachedState::Enum);
