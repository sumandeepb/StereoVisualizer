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

#pragma once

#include <vector>
#include <d3dx9.h>

class CD3DSettings
{
	// construction
public:
	CD3DSettings ();
	~CD3DSettings ();

	// implementation
protected:
	int			m_nWidth;
	int			m_nHeight;
	float		m_rWalkingSensitivity;
	float		m_rLookingSensitivity;
	float		m_rMouseSensitivity;

	// exposed routines
public:
	BOOL		loadD3DSettings (const std::string &strConfigFile);
	BOOL		saveD3DSettings (const std::string &strConfigFile);

	int			getWidth ();
	int			getHeight ();
	float		getWalkingSensitivity ();
	float		getLookingSensitivity ();
	float		getMouseSensitivity ();
};
