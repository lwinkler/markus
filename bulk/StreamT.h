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

#ifndef STREAM_T_H
#define STREAM_T_H

#include "Stream.h"
#include "feature_util.h"

namespace mk {
/// Stream in the form of located objects

template<typename T>class StreamT : public Stream
{
public:
	friend inline void to_json(mkjson& _json, const StreamT<T>& _ser){_ser.Serialize(_json);}
	friend inline void from_json(const mkjson& _json, StreamT<T>& _ser){_ser.Deserialize(_json);}

	explicit StreamT(const std::string& rx_name, T& rx_object, Module& rx_module, const std::string& rx_description, const mkjson& rx_requirement = nullptr) :
		Stream(rx_name, rx_module, rx_description, rx_requirement),
		m_content(rx_object),
		m_default(T{})
	{}
	~StreamT() override {}
	const std::string& GetClass() const override {return className;}

	inline const T& GetContent() const {return m_content;}
	inline       T& RefContent() const {return m_content;}

	void ConvertInput() override;
	void RenderTo(cv::Mat& x_output) const override;
	void Query(std::ostream& xr_out, const cv::Point& x_pt) const override;
	void Randomize(unsigned int& rx_seed) override;
	virtual void Serialize(mkjson& rx_json, MkDirectory* xp_dir = nullptr) const;
	virtual void Deserialize(const mkjson& x_json, MkDirectory* xp_dir = nullptr);

	void SetValue(const mkconf& x_value, ParameterConfigType x_confType) override
	{
		from_mkjson(x_value, m_content);
		m_confSource = x_confType;
	}
	void SetDefault(const mkconf& x_value) override{
		from_mkjson(x_value, m_default);
	}
	void SetValueToDefault() override {m_content = m_default; m_confSource = PARAMCONF_DEF;}
	mkconf GetValue() const override
	{
		return m_content;
	}
	mkconf GetDefault() const override
	{
		return m_default;
	}

	static const std::string className;

protected:
	T& m_content;
	T  m_default;
};

} // namespace mk
#endif
