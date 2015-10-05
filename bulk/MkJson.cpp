#include "MkJson.h"


std::unique_ptr<MkJson> MkJson::operator [] (const std::string& x_str)
{
	return *mp_root;// MkJson(m_root[x_str]);
}

std::unique_ptr<MkJson> MkJson::operator [] (int x_index)
{
	return *mp_root;// MkJson(m_root[x_index]);
}

std::unique_ptr<MkJson> MkJson::Create(const std::string& x_str)
{
	return MkJson(m_root[x_str]);
}

std::unique_ptr<MkJson> MkJson::Create(int x_index)
{
	return MkJson(m_root[x_index]);
}

MkJson MkJson::emptyArray()
{
	return MkJson(Json::Value(Json::arrayValue));
}

MkJson MkJson::nullValue()
{
	return MkJson(Json::Value(Json::nullValue));
}
