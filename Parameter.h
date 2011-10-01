#ifdef PARAMETER_H
#define PARAMETER_H

#include <list>

class ParameterStructure
{
	ParameterStructure(){};
	virtual void SetFromList();
};

class Parameter
{
public:
	int m_id;
	const char * m_name;
	double m_default;
	double m_min;
	double m_max;
};

class ParameterList
{
public:
	ParameterList(){};
	~ParameterList{};
private:
	std::list<Parameter> list;
};

#endif