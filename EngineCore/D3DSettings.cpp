/*
 *     Copyright (C) 2013-2018 Sumandeep Banerjee
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU Lesser General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU Lesser General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* 
 * File:   
 * Author: sumandeep
 * Email:  sumandeep.banerjee@gmail.com
*/

#include <iostream>
#include <fstream>
#include "D3DSettings.h"

using namespace std;

CD3DSettings::CD3DSettings ()
{
	m_rWalkingSensitivity = 3.0f;
	m_rLookingSensitivity = 0.2f;
	m_rMouseSensitivity = 0.005f;
}

CD3DSettings::~CD3DSettings ()
{
}

BOOL CD3DSettings::loadD3DSettings (const std::string &strConfigFile)
{
	// File input
	std::ifstream InFile (strConfigFile.c_str());
	if (!InFile)
		return FALSE;
#ifdef _DEBUG
	cout << "Loading config file... " << strConfigFile.c_str()<< endl;
#endif

	char szLineBuff [_MAX_PATH];
	char szCommand [_MAX_PATH];
	while (1)
	{
		InFile.getline (szLineBuff, _MAX_PATH);
		if( !InFile )
			break;

		szCommand [0] = 0;
		sscanf (szLineBuff, "%s", szCommand);

		if ( 0 == strcmp( szCommand, "#" ) )
		{
			// Comment
		}
		else if ( 0 == strcmp( szCommand, "resolution" ) )
		{
			sscanf (szLineBuff, "%s%d%d", szCommand, &m_nWidth, &m_nHeight);
		}
		else if ( 0 == strcmp( szCommand, "keyboard" ) )
		{
			sscanf (szLineBuff, "%s%f%f", szCommand, &m_rWalkingSensitivity, &m_rLookingSensitivity);
		}
		else if ( 0 == strcmp( szCommand, "mouse" ) )
		{
			sscanf (szLineBuff, "%s%f", szCommand, &m_rMouseSensitivity);
		}
		else
		{
			// unimplemented or unrecognized command
		}
	}

	return TRUE;
}

int	CD3DSettings::getWidth ()
{
	return m_nWidth;
}

int	CD3DSettings::getHeight ()
{
	return m_nHeight;
}

float CD3DSettings::getWalkingSensitivity ()
{
	return m_rWalkingSensitivity;
}

float CD3DSettings::getLookingSensitivity ()
{
	return m_rLookingSensitivity;
}

float CD3DSettings::getMouseSensitivity ()
{
	return m_rMouseSensitivity;
}
