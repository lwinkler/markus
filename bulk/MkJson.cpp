#include "MkJson.h"


MkJson MkJson::operator [] (const std::string& x_str)
{
	return MkJson(m_root[x_str]);
}

MkJson MkJson::operator [] (int x_index)
{
	return MkJson(m_root[x_str]);
}

MkJson MkJson::Create(const std::string& x_str)
{
	return MkJson(m_root[x_str]);
}

MkJson MkJson::Create(int x_index)
{
	return MkJson(m_root[x_str]);
}

static MkJson emptyArray()
{
	return MkJson(Json::Value(Json::arrayValue));
}

static MkJson nullValue()
{
	return MkJson(Json::Value(Json::nullValue));
}
