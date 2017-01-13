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

#include "WriteObjectMongo.h"
#include "StreamObject.h"
#include "StreamImage.h"
#include "Manager.h"
#include "util.h"

using namespace std;
using namespace cv;


log4cxx::LoggerPtr WriteObjectMongo::m_logger(log4cxx::Logger::getLogger("WriteObjectMongo"));



WriteObjectMongo::WriteObjectMongo(ParameterStructure& xr_params)
	: Module(xr_params), m_param(dynamic_cast<Parameters&>(xr_params)),
	m_image(Size(m_param.width, m_param.height), m_param.type)
{
	// Init input images
	AddInputStream(new StreamObject("objects", m_objects, *this, "Input objects to be written"));
	AddInputStream(new StreamImage("image", m_image, *this, "Image stream"));
	mongoc_init();
}

WriteObjectMongo::~WriteObjectMongo()
{
	mongoc_gridfs_destroy(mp_gridfs);
	mongoc_collection_destroy(mp_collection);
	mongoc_client_destroy(mp_client);
	mongoc_cleanup();
}

void WriteObjectMongo::Reset()
{
	Module::Reset();

	m_jobId = GetContext().GetJobId();

	mp_outputDir.reset(new MkDirectory(m_param.folder, RefContext().RefOutputDir(), false));

	// initialize connections to the DB
	if(mp_client)
		mongoc_client_destroy(mp_client);
	mp_client = mongoc_client_new(m_param.host.c_str());
	assert(mp_client);
	if(mp_collection)
		mongoc_collection_destroy(mp_collection);
	mp_collection = mongoc_client_get_collection(mp_client, m_param.database.c_str(), m_param.collection.c_str());
	assert(mp_collection);
	bson_error_t error;
	if(mp_gridfs)
		mongoc_gridfs_destroy(mp_gridfs);
	mp_gridfs = mongoc_client_get_gridfs(mp_client, "test", "fs", &error);
	if(mp_gridfs == nullptr)
		throw MkException("Cannot create GridFS connection", LOC);

	m_dbCollection = m_param.database + "." + m_param.collection;

	// Write job id to jobs collection
	bson_t *doc = bson_new();
	mongoc_collection_t* p_job_collection = mongoc_client_get_collection(mp_client, m_param.database.c_str(), "jobs");

	bson_append_date_time(doc, "startDate", -1, getAbsTimeMs());
	bson_append_utf8(doc, "_id", -1, m_jobId.c_str(), -1);
	bson_append_utf8(doc, "applicationName", -1, GetContext().GetApplicationName().c_str(), -1);
	bson_append_utf8(doc, "applicationVersion", -1, GetContext().Version(false).c_str(), -1);
	if(!mongoc_collection_insert(p_job_collection, MONGOC_INSERT_NONE, doc, NULL, &error))
	{
		if(error.code == 11000)
		{
			LOG_WARN(m_logger, "An entry is already present for job " << m_jobId);
		}
		else throw MkException("Error at mongo db insertion: " + string(error.message), LOC);
	}

	bson_destroy(doc);
	mongoc_collection_destroy(p_job_collection);
}

const char* WriteObjectMongo::contentType(const string& x_fileName)
{
	static const map<string, string> typeList = {
		{"jpg", "image/jpeg"}, 
		{"jpeg", "image/jpeg"}, 
		{"svg", "image/svg+xml"}, 
		{"image", "image/png"}
	};
	size_t r = x_fileName.find_last_of('.');
	if(r == string::npos)
		throw MkException("Impossible to guess type from name " + x_fileName, LOC);
	string ext = x_fileName.substr(r + 1);

	auto it = typeList.find(ext);
	if(it == typeList.end())
	{
		LOG_WARN(m_logger, "Cannot determine type of content from " << x_fileName);
		return "";
	}
	return it->second.c_str();
}


void WriteObjectMongo::ProcessFrame()
{
	int cpt = 0;
	for(const auto& elem : m_objects)
	{
		mongoc_cursor_t *cursor;
		bson_error_t error;

		stringstream sobj;
		elem.Serialize(sobj, mp_outputDir.get());
		bson_t* doc = bson_new_from_json(reinterpret_cast<const uint8_t*>(sobj.str().c_str()), -1, &error);
		assert(doc);

		// create a mongo document
		bson_oid_t oid;
		bson_oid_init(&oid, NULL);
		BSON_APPEND_OID(doc, "_id", &oid);
		bson_append_utf8(doc, "jobId", -1, m_jobId.c_str(), -1);

		// For each object save an image
		if(m_inputStreams.at("image")->IsConnected())
		{
			stringstream sfile;
			sfile << m_currentTimeStamp << "_" << elem.GetName() << elem.GetId() << "_" << cpt << "." << m_param.extension;
			string path = mp_outputDir.get()->ReserveFile(sfile.str());
			imwrite(path, (m_image)(elem.GetRect()));

			// Save to mongo db
			mongoc_gridfs_file_opt_t opt = {};
			mongoc_stream_t *stream = mongoc_stream_file_new_for_path(path.c_str(), O_RDONLY, 0);
			assert(stream);
			string name = m_jobId + '/' + sfile.str();
			opt.filename = name.c_str();
			mongoc_gridfs_file_t *file = mongoc_gridfs_create_file_from_stream(mp_gridfs, stream, &opt);
			assert(file);

			mongoc_gridfs_file_save(file);
			mongoc_gridfs_file_destroy(file);

			bson_append_utf8(doc, "file", -1, name.c_str(), -1);
			if(m_param.cleanFiles)
				mp_outputDir.get()->Rm(sfile.str());
		}

		if(!mongoc_collection_insert(mp_collection, MONGOC_INSERT_NONE, doc, NULL, &error))
		{
			throw MkException("Error at mongo db insertion: " + string(error.message), LOC);
		}

		bson_destroy(doc);
		cpt++;
	}
}

bool WriteObjectMongo::IsInputProcessed() const
{
	const StreamObject& stream =  dynamic_cast<const StreamObject&>(m_inputStreams.at("objects")->GetConnected());
	return !m_objects.empty() || !stream.GetContent().empty();
}

