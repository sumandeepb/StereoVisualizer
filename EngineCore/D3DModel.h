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

#include <string>
#include <vector>
#include "D3DUtility.h"

class CD3DModel
{
public:
	struct SVertex
	{
		SVertex () {}
		SVertex (float x, float y, float z, 
			float nx, float ny, float nz, float u, float v)
		{
			_x = x;   _y = y;   _z = z;
			_nx = nx; _ny = ny; _nz = nz;
			_u = u;   _v = v;
		}

		float _x, _y, _z, _nx, _ny, _nz, _u, _v;

		static const DWORD FVF;
	};

protected:
	IDirect3DDevice9*				m_pD3DDevice;
	D3DXMATRIX						m_matWorld;
	ID3DXMesh*						m_pObjectMesh;
	D3DFILLMODE						m_eFillMode;
	std::vector<D3DMATERIAL9>		m_asMaterial;
	std::vector<IDirect3DTexture9*>	m_apTexture;

public:
	CD3DModel(void);
	~CD3DModel(void);
	BOOL load3DModel (IDirect3DDevice9* pD3DDevice, const std::string &strModelFile, float rScale = 1.0f);
	void setFillMode (D3DFILLMODE eFillMode);
	void setWorldTransform (const D3DXMATRIX &matWorld);
	BOOL renderModel ();
	BOOL unload3DModel ();
};
