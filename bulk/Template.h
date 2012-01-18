
#include "Object.h"

/*! \class Template
 *  \brief Class representing an object template
 *
 *  A template is what allows to track an object, e.g. a red car, through different frames.
 */
class Template
{
	public:
		Template(int x_maxNbFramesDisappearance);
		Template(const Object&, int x_maxNbFramesDisappearance);
		Template(const Template&, int x_maxNbFramesDisappearance);
		Template& operator = (const Template&);
		~Template();
		
		double CompareWithObject(const Object& x_reg) const;
		void UpdateFeatures();
		
		
		inline void AddFeature(const char* name, double value)
		{
			Feature f(/*name,*/value);
			m_feats.push_back(f);
		};
		inline const std::vector <Feature>& GetFeatures() const{ return m_feats;};
		inline const std::list <Object>& GetMatchingObjects() const{ return m_matchingObjects;};
		inline int GetNum() const {return m_num;};
		
		//int m_isMatched;
		int m_bestMatchingObject;
		int m_counterClean;
		std::list <Object> m_matchingObjects;
		double m_posX;
		double m_posY;

	private:
		int m_num;
		static int m_counter;
		std::vector <Feature> m_feats;
		
};