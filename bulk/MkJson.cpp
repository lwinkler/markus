#include "MkJson.h"


MkJson MkJson_::operator [] (const std::string& x_str)
{
	return new MkJson_(m_root);// MkJson_(m_root[x_str]);
}

MkJson MkJson_::operator [] (int x_index)
{
	return new MkJson_(m_root);// MkJson_(m_root[x_index]);
}

MkJson MkJson_::Create(const std::string& x_str)
{
	return new MkJson_(m_root[x_str]);
}

MkJson MkJson_::Create(int x_index)
{
	return new MkJson_(m_root[x_index]);
}

MkJson_ MkJson_::emptyArray()
{
	return MkJson_(Json::Value(Json::arrayValue));
}

MkJson_ MkJson_::nullValue()
{
	return MkJson_(Json::Value(Json::nullValue));
}
