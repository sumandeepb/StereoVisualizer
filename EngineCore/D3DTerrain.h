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

class CD3DBitmapPixel
{
public:
	unsigned char blue;
	unsigned char green;
	unsigned char red;

	CD3DBitmapPixel ():blue(0), green(0), red(0)
	{
	}

	CD3DBitmapPixel (unsigned char r, unsigned char g, unsigned char b)
	{
		blue = b; green = g; red = r;
	}

	bool operator == ( CONST CD3DBitmapPixel& cPixel) const
	{
		if (blue == cPixel.blue && green == cPixel.green && red == cPixel.red)
		{
			return true;
		}
		{
			return false;
		}
	}
};

class CD3DBitmap
{
protected:
	BITMAPFILEHEADER	m_sFileHeader;
	BITMAPINFOHEADER	m_sInfoHeader;
	CD3DBitmapPixel*	m_psData;

public:
	CD3DBitmap ();
	~CD3DBitmap ();

	BOOL loadBitmap (const std::string &strFileName);
	int getWidth ();
	int getHeight ();
	CD3DBitmapPixel getPixel (int row, int col);

	BOOL unloadBitmap ();
};

enum EMovementType { MOVEMENT_BLOCK, MOVEMENT_ALLOW, MOVEMENT_LEVELUP, MOVEMENT_LEVELDOWN };

typedef struct _tag_STileCode
{
	int		red;
	int		green;
	int		blue;
}STileCode;

class CD3DTerrainLayer
{
protected:
	struct STerrainVertex
	{
		STerrainVertex () {}
		STerrainVertex (float x, float y, float z, 
			float nx, float ny, float nz, float u, float v)
		{
			_x = x; _y = y; _z = z; 
			_nx = nx; _ny = ny; _nz = nz;
			_u = u; _v = v;
		}

		float _x, _y, _z, _nx, _ny, _nz, _u, _v;

		static const DWORD FVF;
	};

	IDirect3DDevice9*	m_pD3DDevice;
	ID3DXMesh*			m_pTerrainMesh;
	D3DXMATRIX			m_matWorld;

	int m_numRow;
	int m_numCol;
	int m_numCellsPerRow;
	int m_numCellsPerCol;

	float m_rCellSpacing;
	float m_rBaseHeight;
	float m_rHeightScale;

	float m_rWidth;
	float m_rDepth;

	std::vector<float>			m_aHeightMap;
	std::vector<STileCode>		m_aTerrainMap;
	std::vector<EMovementType>	m_aMovementMap;

	D3DMATERIAL9				m_terrainMtrl;				
	std::vector<IDirect3DTexture9*> m_apTexture;

	BOOL readHeightMap (const std::string &strHeightmap);
	BOOL readTerrainMap (const std::string &strTerrainMap);
	BOOL readMovementMap (const std::string &strMovementMap);
	float getHeightmapEntry (int row, int col);
	void setHeightmapEntry (int row, int col, float value);
	STileCode getTerrainMapEntry (int row, int col);
	void setTerrainMapEntry (int row, int col, const STileCode &sTileCode);
	EMovementType getMovementMapEntry (int row, int col);
	void setMovementMapEntry (int row, int col, EMovementType &eMovementType);

	BOOL computeMesh ();

public:
	CD3DTerrainLayer ();
	~CD3DTerrainLayer ();

	BOOL loadTerrainLayer (IDirect3DDevice9* pDevice, const std::string &strHeightmap,
		float rCellSpacing, float rBaseHeight, float rHeightScale,
		const std::string &strTerrainMap, const std::vector<STileCode> &asTileCodes,
		const std::vector<IDirect3DTexture9*> &apTexture, const std::string &strMovementMap);
	BOOL renderTerrainLayer ();
	BOOL unloadTerrainLayer ();

	void setWorldTransform (const D3DXMATRIX &matWorld);

	float getHeight (float x, float z);
	bool isValidPosition (float x, float z);
	EMovementType getMovementMap (float x, float z);
};

class CD3DTerrain
{
protected:
	IDirect3DDevice9* m_pD3DDevice;

	int m_nCurrentLayer;
	std::vector<STileCode> m_asTileCodes;
	std::vector<IDirect3DTexture9*> m_apTexture;
	std::vector<CD3DTerrainLayer*> m_apTerrainLayer;

public:
	CD3DTerrain ();
	~CD3DTerrain ();

	BOOL loadTerrain (IDirect3DDevice9* pDevice, const std::string &strTerrainFile);
	BOOL renderTerrain ();
	BOOL unloadTerrain ();
	bool isValidTerrain ();

	void setCurrentLayerID (int index);
	int getLayerCount ();
	int getCurrentLayerID ();
	//	CD3DTerrainLayer& getCurrentLayer ();
	//	CD3DTerrainLayer& getLayer (int index);
	bool isValidPosition (float x, float z, int iLayer = -1);
	float getHeight (float x, float z, int iLayer = -1);
	void updateTerrainLevel (float x, float z);
};
