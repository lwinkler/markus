#ifndef MK_FEATURE_H
#define MK_FEATURE_H

#include <string>
#include <vector>

/*! \class Feature
 *  \brief Class representing a feature of a template/region
 *
 * e.g. area, perimeter, length, ...
 */
class Feature
{
	public:
		Feature(double value=0);
		Feature(const Feature&);
		Feature& operator = (const Feature&);
		~Feature();
		
		//inline const char* GetName() const {return m_name;};
		inline double GetValue() const {return m_value;};
		inline void SetValue( double x) {m_value = x;};
		inline double GetVariance() const {return m_variance;};
		inline void SetVariance( double x) {m_variance = x;};
		
		static double GetFeatureValue(const std::vector<Feature>& vect, const char* name);
		
		
	public:
		static std::vector<std::string> m_names; // TODO : Names should be in a static array to save memory space
		double m_value;
		double m_variance;
};


#endif