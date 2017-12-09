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

#ifndef WRITE_OBJECT_MONGO_H
#define WRITE_OBJECT_MONGO_H

#include "Module.h"
#include "Object.h"
#include <mongoc.h>

namespace mk {
/**
* @brief Write a set of objects to Mongo DB
*/
class WriteObjectMongo : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) :
			Module::Parameters(x_name)
		{
			AddParameter(new ParameterString("folder"   ,  "events_img"   , &folder    ,  "Name of the folder to create for images"));
			AddParameter(new ParameterString("host"     ,  "mongodb://localhost:27017/", &host    ,  "Host for MongoDB"));
			AddParameter(new ParameterString("database" ,  "test"         , &database,  "MongoDB database"));
			AddParameter(new ParameterString("collection", "objects"       , &collection, "MongoDB collection"));
			AddParameter(new ParameterBool("cleanFiles", true            , &cleanFiles, "Erase image and other files after insertion in database"));
			AddParameter(new ParameterString("extension"  , "jpg"        , &extension , "Extension of the thumbnails. Determines the output format."));

			RefParameterByName("type").SetDefaultAndValue("CV_8UC3");
			RefParameterByName("type").SetValueToDefault();
			RefParameterByName("type").SetRange(R"({"allowed":["CV_8UC1","CV_8UC3"]})"_json);
		}
		bool cleanFiles;
		std::string file;
		std::string extension;
		double duration;
		std::string folder;
		std::string host;
		std::string database;
		std::string collection;
	};

	void ProcessFrame() override;
	void Reset() override;
	static const char* contentType(const std::string& x_fileName);

	explicit WriteObjectMongo(ParameterStructure& xr_params);
	virtual ~WriteObjectMongo();
	MKCLASS("WriteObjectMongo")
	MKCATEG("Output")
	MKDESCR("Outputs objects to MongoDB")

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	bool IsInputProcessed() const override;

	// input
	std::vector<Object> m_objects;
	cv::Mat m_image;

	// temporary
	mongoc_client_t*     mp_client     = nullptr;
	mongoc_collection_t* mp_collection = nullptr;
	mongoc_gridfs_t*     mp_gridfs     = nullptr;

	std::string m_dbCollection;
	std::string m_jobId;
	std::unique_ptr<MkDirectory> mp_outputDir;
};

} // namespace mk
#endif

