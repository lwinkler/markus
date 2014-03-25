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

#ifndef MKEVENT_H
#define MKEVENT_H

#include "util.h"
#include "Object.h"


/*! \class Event
 *  \brief Class representing a event (e.g. Ponctual moment in time where an intrusion occurs)
 *
 */
class Event
{
	public:
		Event();
		~Event();
		void Empty();
		void Raise(const std::string x_label, TIME_STAMP x_absTimeEvent = 0);
		void Raise(const std::string x_label, const Object& m_object, TIME_STAMP x_absTimeEvent = 0);
		inline bool IsRaised() {return m_label != "";}
		inline const std::string& GetLabel(){return m_label;}
		inline const Object& GetObject() const {return m_object;}
		inline Object& RefObject() {return m_object;}
		inline void SetObject(const Object& x_object){m_object = x_object;}
		// inline const std::string& GetObjectLabel(){return m_objectLabel;};

		inline void AddFeature(std::string x_name, double x_value) {m_object.AddFeature(x_name, x_value);}
		inline const std::map <std::string, Feature>& GetFeatures() const {return m_object.GetFeatures();}
		inline const Feature& GetFeature(const std::string& x_name) const
		{
			return m_object.GetFeature(x_name);
		}
		void Notify(const std::string& x_extraInfo = "", bool x_isProcessEvent=false);

	protected:
		std::string m_label;
		Object m_object;
		TIME_STAMP m_absTimeEvent; // Abs time of the event (given by current date)

	private:
		static log4cxx::LoggerPtr m_logger;
};
#endif
