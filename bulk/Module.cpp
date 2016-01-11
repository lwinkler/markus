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

Module::Module(ParameterStructure& xr_params) :
	Processable(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params)),
	m_name(xr_params.GetConfig().GetAttribute("name"))
{
	m_id	= atoi(xr_params.GetConfig().GetAttribute("id").c_str());
	LOG_INFO(m_logger, "Create module " << m_name);
}

Module::~Module()
{
	// Delete all streams
	for(auto & elem : m_inputStreams)
		delete(elem.second);
	for(auto & elem : m_outputStreams)
		delete(elem.second);
#ifdef MARKUS_DEBUG_STREAMS
	for(auto & elem : m_debugStreams)
		delete(elem.second);
#endif
}

/**
* @brief Reset the module. Parameters value are set to default
*/
void Module::Reset()
{
	LOG_INFO(m_logger, "Reseting module "<<GetName());
	Processable::Reset();

	for(auto& stream : m_outputStreams)
		stream.second->Reset();

	m_param.PrintParameters();
	m_param.CheckRange(true);

	m_timerProcessFrame.Reset();
	m_timerWaiting.Reset();
	m_timerConversion.Reset();

	// Lock all parameters if needed
	m_param.LockIfRequired();

	// This must be done only once to avoid troubles in the GUI
	// Add module controller
	if(!HasController("module"))
		AddController(new ControllerModule(*this));

	for(const auto& elem : m_param.GetList())
	{
		// Do not add param if locked or already present
		if(elem->IsLocked() || HasController(elem->GetName()))
			continue;
		Controller* ctr = Factories::parameterControllerFactory().Create(elem->GetType(), *elem, *this);
		if(ctr == nullptr)
			throw MkException("Controller creation failed", LOC);
		else AddController(ctr);
	}

	if(m_param.cached == CachedState::WRITE_CACHE)
		 SYSTEM("mkdir -p " + GetContext().GetOutputDir() + "/cache/");
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
				// TODO: Handle the case where we rewind
				if(ts <= m_lastTimeStamp || (m_param.fps != 0 && (ts - m_lastTimeStamp) * m_param.fps < 1000))
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
	assert(IsAutoProcessed());
	m_currentTimeStamp = 0;
}

/**
* @brief Process one frame
* @return True if the frame was processed
*/
void Module::Process()
{
	m_timerWaiting.Start();
	WriteLock lock(m_lock);
	try
	{
		/*
		// Timestamp of the module is given by the input stream
		m_currentTimeStamp = 0;
		if(!m_inputStreams.empty())
		{
			m_currentTimeStamp = m_inputStreams[0]->GetTimeStampConnected();
		}
		else if(! m_param.autoProcess)
			throw MkException("Module must have at least one input or have parameter auto_process=true", LOC); // TODO: Check at the beginnning
			*/

#ifdef MARKUS_DEBUG_STREAMS
		// if(m_currentTimeStamp == m_lastTimeStamp)
			// LOG_WARN(m_logger, "Timestamp are not increasing correctly");
#endif
		if(!m_param.autoProcess && !ProcessingCondition())
			return;
		ComputeCurrentTimeStamp();

		// Process this frame

		// Timer for benchmark
		m_timerWaiting.Stop();

		// note: Inputs must call ProcessFrame to set the time stamp
		// TODO: There is no reason to cache input modules !
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
		if(m_param.cached == CachedState::READ_CACHE)
		{
			assert(!IsInput());
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
				cout << threads.size() << endl;
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


		m_countProcessedFrames++;
		m_lastTimeStamp = m_currentTimeStamp;
	}
	catch(...)
	{
		m_timerWaiting.Stop();
		m_timerConversion.Stop();
		m_timerProcessFrame.Stop();
		LOG_WARN(m_logger, "Exception in module " << GetName());
		throw;
	}
	return;
}

/**
* @brief Describe the module with name, parameters, inputs, outputs to an output stream (in xml)
*
* @param rx_os         Output stream
* @param x_indentation Number of tabs in indentation
*/
void Module::Export(ostream& rx_os, int x_indentation) const
{
	string tabs(x_indentation, '\t');
	tabs = string(x_indentation, '\t');
	rx_os<<tabs<<"<module name=\""<<m_name<<"\" description=\""<<GetDescription()<<"\">"<<endl;
	tabs = string(x_indentation + 1, '\t');
	rx_os<<tabs<<"<parameters>"<<endl;
	for(const auto & elem : m_param.GetList())
		elem->Export(rx_os, x_indentation + 2);
	rx_os<<tabs<<"</parameters>"<<endl;

	rx_os<<tabs<<"<inputs>"<<endl;
	for(const auto& elem : m_inputStreams)
		elem.second->Export(rx_os, elem.first, x_indentation + 2, true);
	rx_os<<tabs<<"</inputs>"<<endl;

	rx_os<<tabs<<"<outputs>"<<endl;
	for(const auto& elem : m_outputStreams)
		elem.second->Export(rx_os, elem.first, x_indentation + 2, false);
	rx_os<<tabs<<"</outputs>"<<endl;
	tabs = string(x_indentation, '\t');
	rx_os<<tabs<<"</module>"<<endl;
}

Stream& Module::RefInputStreamById(int x_id)
{
	auto it = m_inputStreams.find(x_id);

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
	auto it = m_outputStreams.find(x_id);

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
	double fps = (m_countProcessedFrames / (m_timerProcessFrame.GetSecDouble() + m_timerConversion.GetSecDouble() + m_timerWaiting.GetSecDouble()));
	LOG_INFO(m_logger, "Module "<<GetName()<<": "<<m_countProcessedFrames<<" frames processed (tproc="<<
			 m_timerProcessFrame.GetMsLong()<<"ms, tconv="<<m_timerConversion.GetMsLong()<<"ms, twait="<<
			 m_timerWaiting.GetMsLong()<<"ms), "<< fps <<" fps");
	if(m_timerProcessable.GetMsLong() > 0)
		LOG_INFO(m_logger, "Module "<<GetName()<<": Time spent in parent: " << m_timerProcessable.GetMsLong() << " ms");

	// Write perf to output XML
	ConfigReader perfModule(xr_xmlResult.FindRef("module[name=\"" + GetName() + "\"]", true));
	perfModule.FindRef("nb_frames", true).SetValue(m_countProcessedFrames);
	perfModule.FindRef("timer[name=\"processable\"]", true).SetValue(m_timerProcessable.GetMsLong());
	perfModule.FindRef("timer[name=\"processing\"]", true).SetValue(m_timerProcessFrame.GetMsLong());
	perfModule.FindRef("timer[name=\"conversion\"]", true).SetValue(m_timerConversion.GetMsLong());
	perfModule.FindRef("timer[name=\"waiting\"]", true).SetValue(m_timerWaiting.GetMsLong());
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
	// root["timer_conversion"]      = m_timerConversion.GetMsLong();
	// root["timer_processing"]      = m_timerProcessFrame.GetMsLong();
	// root["timer_waiting"]         = m_timerWaiting.GetMsLong();
	root["countProcessedFrames"] = Json::UInt64(m_countProcessedFrames);

	// Dump inputs
	for(const auto & elem : m_inputStreams)
	{
		stringstream ss;
		elem.second->Serialize(ss, x_dir);
		ss >> root["inputs"][elem.first];
	}
	// Dump outputs
	for(const auto & elem : m_outputStreams)
	{
		stringstream ss;
		elem.second->Serialize(ss, x_dir);
		ss >> root["outputs"][elem.first];
	}
#ifdef MARKUS_DEBUG_STREAMS
	// Dump debugs
	for(const auto & elem : m_debugStreams)
	{
		stringstream ss;
		elem.second->Serialize(ss, x_dir);
		ss >> root["debugs"][elem.first];
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
	// m_timerConversion.Reset();
	// m_timerProcessFrame.Reset();
	// m_timerWaiting.Reset();
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
		string directory = GetContext().GetOutputDir() + "/cache/";
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
	for(auto& elem : m_outputStreams)
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

/// Static members of ParameterCachedState
const map<string, int> Module::ParameterCachedState::Enum = {
	{"NO_CACHE", CachedState::NO_CACHE},
	{"WRITE_CACHE", CachedState::WRITE_CACHE},
	{"READ_CACHE", CachedState::READ_CACHE},
	{"DISABLED", CachedState::DISABLED}
};
const map<int, string> Module::ParameterCachedState::ReverseEnum = ParameterEnum::CreateReverseMap(ParameterCachedState::Enum);
