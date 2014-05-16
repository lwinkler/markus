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

#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <iostream>

/// Class for all serializable objects

class Serializable
{
	public:
		Serializable(){}
		virtual ~Serializable(){}

		/**
		* @brief Serialize the stream content to JSON
		*
		* @param x_out Output stream
		* @param x_dir Output directory (for images)
		*/
		virtual void Serialize(std::ostream& x_out, const std::string& x_dir) const = 0;
		/**
		* @brief  Deserialize the stream from JSON
		*
		* @param x_in   Input stream
		* @param x_dir  Input directory (for images)
		*/
		virtual void Deserialize(std::istream& x_in, const std::string& x_dir) = 0;

		std::string SerializeToString(const std::string& x_dir = "") const;

	/**
	* @brief Redefinition of the output stream operator <<
	*
	* @param x_out Output stream
	* @param x_obj Object to serialize
	*
	* @return output stream
	*/
		inline friend std::ostream& operator<< (std::ostream& x_out, const Serializable& x_obj){x_obj.Serialize(x_out, ""); return x_out;};
	/**
	* @brief Redefinition of the input stream operator <<
	*
	* @param x_in  Input stream
	* @param x_obj Object to deserialize
	*
	* @return input stream
	*/
	inline friend std::istream& operator>> (std::istream& x_in, Serializable& x_obj){x_obj.Deserialize(x_in, ""); return x_in;};
};

#endif
