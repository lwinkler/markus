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
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with Markus.  If not, see <http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------------------*/

#ifndef MODULE_H
#define MODULE_H

#include "ParameterImageType.h"
#include "ParameterNum.h"
#include "ParameterString.h"
#include "Controller.h"
#include "Processable.h"
#include "Timer.h"
#include <log4cxx/logger.h>
#include <opencv2/core/core.hpp>

#define MAX_WIDTH  6400
#define MAX_HEIGHT 4800



class Stream;
class QModuleTimer;

/**
* @brief Class representing a module. A module is a node of the application, it processes streams
*/
class Module : public Processable, public Controllable, public Serializable
{
public:
	/// Parameter enum class. Determines if the module is cached (its output is stored to avoid computation costs)
	enum CachedState : char
	{
		NO_CACHE    = 0,
		WRITE_CACHE = 1,
		READ_CACHE  = 2,
		DISABLED    = 3
	};

	class ParameterCachedState : public ParameterEnum
	{
	public:
		ParameterCachedState(const std::string& x_name, int x_default, int * xp_value, const std::string& x_description)
			: ParameterEnum(x_name, x_default, xp_value, x_description){}
		~ParameterCachedState() {}

		// Conversion methods
		inline const std::string& GetTypeString() const {const static std::string s = "cachedState"; return s;}
		const std::map<std::string, int> & GetEnum() const {return Enum;}
		const std::map<int, std::string> & GetReverseEnum() const {return ReverseEnum;}

	private:
		// static attributes
		const static std::map<std::string, int> Enum;
		const static std::map<int, std::string> ReverseEnum;
	};

	class Parameters : public Processable::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : Processable::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterString("class"       , ""      , &objClass , "Class of the module (define the module's function)"));
			m_list.push_back(new ParameterInt("width"          , 640     , 1         , MAX_WIDTH , &width  , "Width of the input"));
			m_list.push_back(new ParameterInt("height"         , 480     , 1         , MAX_HEIGHT, &height , "Height of the input"));
			m_list.push_back(new ParameterImageType("type"     , CV_8UC1 , &type     , "Format of the input image"));
			m_list.push_back(new ParameterCachedState("cached" , CV_8UC1 , &cached   , "Format of the input image"));

			Init();
		}

		int width;
		int height;
		int type;
		std::string objClass;
		int cached;
	};

	Module(ParameterStructure& x_param);
	virtual ~Module();

	virtual void Reset();
	virtual bool Process();

	virtual const std::string& GetName() const {return m_name;}
	virtual const std::string& GetClass() const = 0;
	virtual const std::string& GetDescription() const = 0;
	int GetId() const {return m_id;}

	const std::map<int, Stream*>& GetInputStreamList() const {return m_inputStreams;}
	const std::map<int, Stream*>& GetOutputStreamList() const {return m_outputStreams;}
	const std::map<int, Stream*>& GetDebugStreamList() const {return m_debugStreams;}
	void ProcessRandomInput(unsigned int& xr_seed);  /// For unit testing
	void AddInputStream(int x_id, Stream* xp_stream);
	void AddOutputStream(int x_id, Stream* xp_stream);
	void AddDebugStream(int x_id, Stream* xp_stream);


	inline int GetWidth() const          {return m_param.width;}
	inline int GetHeight() const         {return m_param.height;}
	inline int GetImageType() const      {return m_param.type;}
	inline double GetFps() const         {return m_param.fps;}
	inline bool IsAutoProcessed() const  {return m_param.autoProcess;}
	virtual double GetRecordingFps() const;

	inline void AddDependingModule (Module & x_module) {m_modulesDepending.push_back(&x_module);} /// Add a module to the list: depending modules are called when processing is complete
	virtual void PrintStatistics(ConfigReader& xr_result) const;
	virtual void Serialize(std::ostream& stream, const std::string& x_dir) const;
	virtual void Deserialize(std::istream& stream, const std::string& x_dir);

	virtual inline bool IsInput() {return false;}
	void Export(std::ostream& rx_os, int x_indentation);
	Stream& RefInputStreamById(int x_id);
	Stream& RefOutputStreamById(int x_id);
	inline bool IsReady() {return IsAutoProcessed() || m_isReady;}
	void SetAsReady();
	bool AllInputsAreReady() const;
	const Module& GetMasterModule() const;
	inline void CheckParameterRange() {m_param.CheckRange(false);}
	inline bool IsUnitTestingEnabled() const {return m_isUnitTestingEnabled;}

	void WriteToCache() const;
	void ReadFromCache();

protected:
	// for benchmarking
	Timer m_timerConversion;
	Timer m_timerProcessing;
	Timer m_timerWaiting;
	long long m_countProcessedFrames;

	// for testing
	bool m_isUnitTestingEnabled;

	TIME_STAMP m_lastTimeStamp;     // time stamp of the lastly processed input
	TIME_STAMP m_currentTimeStamp;  // time stamp of the current input
	bool m_isReady;
	bool m_unsyncWarning;

	virtual void ProcessFrame() = 0;
	inline virtual bool IsInputProcessed() const {return true;}

	// Streams
	std::map<int, Stream *> m_inputStreams;
	std::map<int, Stream *> m_outputStreams;
	std::map<int, Stream *> m_debugStreams;

	std::string m_name;
	int m_id;
	std::vector<Module *> m_modulesDepending;

private:
	Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;
};

#endif
