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
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  <See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with Markus.  If not, see <http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------------------*/

#ifndef CONTEXT_H
#define CONTEXT_H

/**
* @brief All informations that must be known by modules concerning application run-time
*/
/// All informations that must be known by modules concerning application run-time
class Context
{
	public:
		Context() : m_isset(false){}
		Context(const std::string& x_applicationName, const std::string& x_outputDir) : 
			m_isset(true),
			m_applicationName (x_applicationName),
			m_outputDir       (x_outputDir) {}
		Context &operator = (const Context &x_context)
		{
			if(m_isset)
				throw MkException("Context must be set only once", LOC);
			m_applicationName = x_context.GetApplicationName();
			m_outputDir       = x_context.GetOutputDir();
			m_isset           = true;
			return *this;
		}
		inline const std::string& GetOutputDir() const {Check(); return m_outputDir;}
		inline const std::string& GetApplicationName() const {Check(); return m_applicationName;}

	protected:
		inline void Check() const {if(!m_isset)throw MkException("Module and Manager context must be set after initialization (with SetContext)", LOC);}
		std::string m_applicationName;
		std::string m_outputDir;
		bool m_isset;

	private:
		Context(const Context &);
};
#endif
