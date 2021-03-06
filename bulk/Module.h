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

#include <log4cxx/logger.h>
#include <opencv2/core/core.hpp>
#include "ParameterT.h"
#include "Controller.h"
#include "Processable.h"
#include "Timer.h"
#include "enums.h"
#include "ParameterEnumT.h"

#define MAX_WIDTH  6400
#define MAX_HEIGHT 4800

namespace mk {
class Stream;

/**
* @brief Class representing a module. A module is a node of the application, it processes streams
*/
class Module : public Processable, public Controllable
{
public:
	// utilities for serialization
	inline friend void to_json(mkjson& _json, const Module& _ser) {_ser.Serialize(_json);}
	inline friend void from_json(const mkjson& _json, Module& _ser) {_ser.Deserialize(_json);}

	class Parameters : public Processable::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : Processable::Parameters(x_name)
		{
			// list here the different parameters of the module (the list gets inherited)
			AddParameter(new ParameterString("master"      , ""      , &master   , "Master module on which this module's processing is dependent. If empty, use all preceeding modules"));
			AddParameter(new ParameterInt("width"          , 640     , 1         , MAX_WIDTH , &width  , "Width of the input"));
			AddParameter(new ParameterInt("height"         , 480     , 1         , MAX_HEIGHT, &height , "Height of the input"));
			AddParameter(new ParameterEnumT<mk::ImageType>("type"     , CV_8UC1 , &type     , "Format of the input image"));
			AddParameter(new ParameterEnumT<mk::CachedState>("cached" , mk::CachedState::NO_CACHE, &cached   , "Format of the input image"));

			// Lock the parameters that cannot be changed
			LockParameterByName("width");
			LockParameterByName("height");
			LockParameterByName("type");
			LockParameterByName("autoProcess");
		}

		int width;
		int height;
		int type;
		std::string objClass;
		std::string master;
		int cached;
	};

	class DependingModules : boost::noncopyable
	{
		/// A class to protect the list of depending modules
		public:
			/// Add a module to the list: depending modules are called when processing is complete
			inline void AddDependingModule(Module & rx_module)
			{
				WriteLock lock(m_lock);
				m_modulesDepending.push_back(&rx_module);
			}
			void RemoveDependingModule(const Module & x_module);
			void Process();

		protected:
			Lock m_lock;
			std::vector<Module *> m_modulesDepending;
	};

	explicit Module(ParameterStructure& x_param);
	~Module() override;

	bool ProcessingCondition() const;                                      /// Return true if the current frame must be processed
	inline virtual bool PropagateCondition() const {return true;}          /// Return true if the depending modules must be called. To be overridden
	inline bool AbortCondition() const override {return false;}             /// Return true if the processing should be aborted

	const std::string& GetName() const override {return m_param.GetName();}
	virtual const std::string& GetClass() const = 0;
	virtual const std::string& GetCategory() const{static const std::string cat = "Other"; return cat;}
	virtual const std::string& GetDescription() const = 0;

	const std::map<std::string, Stream*>& GetInputStreamList() const {return m_inputStreams;}
	const std::map<std::string, Stream*>& GetOutputStreamList() const {return m_outputStreams;}
	void ProcessRandomInput(unsigned int& xr_seed);  /// For unit testing
	void AddInputStream(Stream* xp_stream);
	void AddOutputStream(Stream* xp_stream);
	void AddDebugStream(Stream* xp_stream);

	// For backward compatibility: TODO remove since ids are not used anymore
	void AddInputStream(int x_id, Stream* xp_stream);
	void AddOutputStream(int x_id, Stream* xp_stream);
	void AddDebugStream(int x_id, Stream* xp_stream);


	inline int GetWidth() const          {return m_param.width;}
	inline int GetHeight() const         {return m_param.height;}
	inline cv::Size GetSize() const      {return cv::Size(m_param.width, m_param.height);}
	inline int GetImageType() const      {return m_param.type;}
	inline double GetFps() const         {return m_param.fps;}
	inline bool IsAutoProcessed() const  {return m_param.autoProcess;}
	double GetRecordingFps() const override;

	/// Add a module to the list: depending modules are called when processing is complete
	inline void AddDependingModule(Module & rx_module) {m_dependingModules.AddDependingModule(rx_module);}
	inline void RemoveDependingModule(const Module & x_module) {m_dependingModules.RemoveDependingModule(x_module);}
	virtual void PrintStatistics(mkconf& xr_result) const;
	void Serialize(mkjson& rx_json, MkDirectory* xp_dir = nullptr) const;
	void Deserialize(const mkjson& x_json, MkDirectory* xp_dir = nullptr);

	virtual inline bool IsInput() const {return false;}
	mkjson Export() const;
	const Stream& GetInputStreamByName(const std::string& x_name) const;
	const Stream& GetOutputStreamByName(const std::string& x_name) const;
	Stream& RefInputStreamByName(const std::string& x_name);
	Stream& RefOutputStreamByName(const std::string& x_name);
	inline void CheckRangeAndThrow() {m_param.CheckRangeAndThrow();}
	inline bool IsUnitTestingEnabled() const {return m_isUnitTestingEnabled;}
	inline TIME_STAMP GetCurrentTimeStamp() const {return m_currentTimeStamp;}
	inline TIME_STAMP GetLastTimeStamp()    const {return m_lastTimeStamp;}
	void ComputeCurrentTimeStamp();

	void WriteToCache() const;
	void ReadFromCache();

protected:
	void Reset() override;
	void Process() override;

	// for benchmarking
	Timer m_timerConversion;
	Timer m_timerProcessFrame;
	Timer m_timerWaiting;
	uint64_t m_countProcessedFrames = 0;
	uint16_t m_nbReset = UINT16_MAX;

	// for testing
	bool m_isUnitTestingEnabled = true;

	TIME_STAMP m_lastTimeStamp    = TIME_STAMP_MIN;  // time stamp of the lastly processed input
	TIME_STAMP m_currentTimeStamp = TIME_STAMP_MIN;  // time stamp of the current input
	bool m_unsyncWarning = true;

	/// Process on frame: This method should be redefined in each module and defines what gets processed
	virtual void ProcessFrame() = 0;
	inline virtual bool IsInputProcessed() const {return true;}

	// Streams
	std::map<std::string, Stream *> m_inputStreams;
	std::map<std::string, Stream *> m_outputStreams;
	std::map<std::string, Stream *> m_debugStreams;

	DependingModules m_dependingModules;

private:
	Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;
};

} // namespace mk
#endif
