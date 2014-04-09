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

#include "StreamState.h"
#include "util.h"

using namespace std;
using namespace cv;


StreamState::StreamState(const string& x_name, bool& x_state, Module& rx_module, const string& rx_description) : 
	Stream(x_name, rx_module, rx_description),
	m_state(x_state)
{
}

StreamState::~StreamState()
{

}

// Transmit the state to the connected module

void StreamState::ConvertInput()
{
	if(m_connected == NULL) return;
	
	// Copy time stamp to output
	m_timeStamp = RefConnected().GetTimeStamp();

	const StreamState * pstream = dynamic_cast<const StreamState*>(m_connected);
	if(pstream == NULL) return;
	m_state = pstream->GetState();
}

/// Render : to display the state we simply color the image in black/white

void StreamState::RenderTo(Mat& x_output) const
{
	x_output.setTo(Scalar(255 * m_state, 255 * m_state, 255 * m_state));
}

/// Serialize the stream content to a directory
/// @param x_in Input stream
/// @param x_dir Input directory (for images)
void StreamState::Serialize(std::ostream& x_out, const string& x_dir) const
{
	ofstream of;
	string fileName = x_dir+ "/" + GetModule().GetName() + "." + GetName() + ".state.txt";
	of.open(fileName.c_str());
	of << (m_state ? "1" : "0") << endl;
	of.close();
}
/// Deserialize the stream from JSON
/// @param x_in Input stream
/// @param x_dir Input directory (for images)
void StreamState::Deserialize(std::istream& x_in, const string& x_dir)
{
	// TODO
}
