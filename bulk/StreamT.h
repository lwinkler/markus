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

/// Stream in the form of located objects

template<typename T>class StreamT : public Stream
{
public:
	StreamT(const std::string& rx_name, T& rx_object, Module& rx_module, const std::string& rx_description, const std::string& rx_requirement = "") :
		Stream(rx_name, rx_module, rx_description, rx_requirement),
		m_content(rx_object),
		m_default(T{})
	{}
	~StreamT() {}
	virtual const std::string& GetClass() const {return m_class;}
	virtual const std::string& GetType() const {return m_type;}
	virtual const ParameterType& GetParameterType() const {return m_parameterType;}

	inline const T& GetContent() const {return m_content;}
	inline       T& RefContent() const {return m_content;}

	virtual void ConvertInput();
	virtual void RenderTo(cv::Mat& x_output) const;
	virtual void Query(int x_posX, int x_posY) const;
	virtual void Randomize(unsigned int& rx_seed);
	virtual void Serialize(std::ostream& stream, const std::string& x_dir) const;
	virtual void Deserialize(std::istream& stream, const std::string& x_dir);
	// double GetFeatureValue(const std::vector<Feature>& x_vect, const char* x_name);

	virtual void SetValue(const std::string& x_value, ParameterConfigType x_confType)
	{
		std::stringstream ss(x_value);
		deserialize(ss, m_content);
		m_confSource = x_confType;
	}
	virtual void SetDefault(const std::string& x_value){std::stringstream ss(x_value); deserialize(ss, m_default);}
	virtual void SetValueToDefault(){m_content = m_default; m_confSource = PARAMCONF_DEF;}
	virtual std::string GetValueString() const{std::stringstream ss; serialize(ss, m_content); return ss.str();}
	virtual std::string GetDefaultString() const {std::stringstream ss; serialize(ss, m_default); return ss.str();}

protected:
	T& m_content;
	T  m_default;

private:
	DISABLE_COPY(StreamT)
	static log4cxx::LoggerPtr m_logger;
	static const std::string m_type;
	static const std::string m_class;
	static const ParameterType m_parameterType;
};

#endif
