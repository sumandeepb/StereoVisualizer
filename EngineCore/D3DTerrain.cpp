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
#include "D3DTerrain.h"

using namespace std;

const DWORD CD3DTerrainLayer::STerrainVertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

CD3DBitmap::CD3DBitmap ()
{
	memset (&m_sFileHeader, 0, sizeof (BITMAPFILEHEADER));
	memset (&m_sInfoHeader, 0, sizeof (BITMAPINFOHEADER));
	m_psData = NULL;
}

CD3DBitmap::~CD3DBitmap ()
{
	unloadBitmap ();
}

BOOL CD3DBitmap::loadBitmap (const std::string &strFileName)
{
	// File input
	FILE*	pFile;

	pFile = fopen (strFileName.c_str(), "rb");
	if (NULL == pFile)
	{
		return FALSE;
	}

	fread (&m_sFileHeader, sizeof (BITMAPFILEHEADER), 1, pFile);
	if ('M' != m_sFileHeader.bfType >> 8 || 'B' != (unsigned char)((m_sFileHeader.bfType << 8) >> 8))
	{
		return FALSE;
	}

	fread (&m_sInfoHeader, sizeof (BITMAPINFOHEADER), 1, pFile);
	if (24 != m_sInfoHeader.biBitCount)
		return FALSE;

	m_psData = new CD3DBitmapPixel [m_sInfoHeader.biWidth * m_sInfoHeader.biHeight];
	fread (m_psData, sizeof (CD3DBitmapPixel), m_sInfoHeader.biWidth * m_sInfoHeader.biHeight, pFile);

	fclose (pFile);

	return TRUE;
}

int CD3DBitmap::getWidth ()
{
	return m_sInfoHeader.biWidth;
}

int CD3DBitmap::getHeight ()
{
	return m_sInfoHeader.biHeight;
}

CD3DBitmapPixel CD3DBitmap::getPixel (int row, int col)
{
	return m_psData [row * getWidth() + col];
}

BOOL CD3DBitmap::unloadBitmap ()
{
	memset (&m_sFileHeader, 0, sizeof (BITMAPFILEHEADER));
	memset (&m_sInfoHeader, 0, sizeof (BITMAPINFOHEADER));

	if (NULL != m_psData)
	{
		delete [] m_psData;
		m_psData = NULL;
	}

	return TRUE;
}

int matchTileCode (const STileCode &sTileCode, const std::vector<STileCode> &asTileCodes)
{
	for (unsigned int iSearch = 0; iSearch < asTileCodes.size(); iSearch++)
	{
		if (sTileCode.blue == asTileCodes[iSearch].blue &&
			sTileCode.green == asTileCodes[iSearch].green &&
			sTileCode.red == asTileCodes[iSearch].red)
		{
			return iSearch;
		}
	}

	return -1;
}

float linearInterpolate (float a, float b, float t)
{
	return a - (a * t) + (b * t);
}

CD3DTerrainLayer::CD3DTerrainLayer ()
{
	m_pTerrainMesh = NULL;
	m_terrainMtrl = initMtrl(0.2f * WHITE, 0.8f * WHITE, 0.0f * WHITE, BLACK, 2.0f);
	unloadTerrainLayer ();
}

CD3DTerrainLayer::~CD3DTerrainLayer()
{
	unloadTerrainLayer ();
}

BOOL CD3DTerrainLayer::loadTerrainLayer (IDirect3DDevice9* pDevice, const std::string &strHeightmap,
	float rCellSpacing, float rBaseHeight, float rHeightScale,
	const std::string &strTerrainMap, const std::vector<STileCode> &asTileCodes,
	const std::vector<IDirect3DTexture9*> &apTexture, const std::string &strMovementMap)
{
	m_pD3DDevice  = pDevice;
	m_rCellSpacing = rCellSpacing;
	m_rBaseHeight = rBaseHeight;
	m_rHeightScale = rHeightScale;

	// load heightmap
	if (FALSE == readHeightMap (strHeightmap))
	{
		return FALSE;
	}

#ifdef _DEBUG
	cout << "Loaded terrain height map: " << strHeightmap << endl;
#endif	

	// scale heights
	for (unsigned int i = 0; i < m_aHeightMap.size(); i++)
	{
		m_aHeightMap[i] = m_rBaseHeight + m_aHeightMap[i] * m_rHeightScale;
	}

	// load terrain map
	if (FALSE == readTerrainMap (strTerrainMap))
	{
		return FALSE;
	}
#ifdef _DEBUG
	cout << "Loaded terrain texture map: " << strHeightmap << endl;
#endif

	// make copy of texture pointers
	m_apTexture.assign(apTexture.begin(), apTexture.end());

	// compute mesh vertices, indices and texture mappings
	if (FALSE == computeMesh ())
	{
		return FALSE;
	}

	// load attribute buffer
	int nNumTriangles = 2 * m_numCellsPerRow * m_numCellsPerCol;
	DWORD* aAttribBuff = 0;
	m_pTerrainMesh->LockAttributeBuffer(0, &aAttribBuff);
	for (int i = 0; i < nNumTriangles; i++)
	{
		// generate location of terrain map from triangle index
		int row = (i / 2) / m_numCellsPerCol;
		int col = (i / 2) % m_numCellsPerCol;
		// match terrain map info at location with tile list
		int iTileList = matchTileCode (getTerrainMapEntry (row, col), asTileCodes);
		if (-1 == iTileList)
		{
			// could not find matching texture in tile list
			aAttribBuff [i] = 0;
		}
		aAttribBuff [i] = iTileList + 1;
	}
	m_pTerrainMesh->UnlockAttributeBuffer();

	// load movement map
	if (FALSE == readMovementMap (strMovementMap))
	{
		return FALSE;
	}
#ifdef _DEBUG
	cout << "Loaded movement map: " << strMovementMap << endl;
#endif

	return TRUE;
}

BOOL CD3DTerrainLayer::renderTerrainLayer()
{
	if (m_pD3DDevice)
	{
		// Set the world matrix that positions the object.
		m_pD3DDevice->SetTransform (D3DTS_WORLD, &m_matWorld);

		for (unsigned int index = 0; index < m_apTexture.size(); index++)
		{
			// Set object material
			m_pD3DDevice->SetMaterial (&m_terrainMtrl);

			// Set object texture
			m_pD3DDevice->SetTexture (0, m_apTexture[index]);

			// Draw the object using the previously set world matrix.
			if (m_pTerrainMesh)
			{
				m_pTerrainMesh->DrawSubset (index + 1);
			}
		}
	}
	return TRUE;
}

BOOL CD3DTerrainLayer::unloadTerrainLayer ()
{
	m_pD3DDevice = NULL;
	_SAFE_RELEASE (m_pTerrainMesh);

	// initialize world matrix - position the terrain in world space
	D3DXMatrixTranslation(&m_matWorld,  0.0f, 0.0f,  0.0f);

	m_aHeightMap.clear ();
	m_aTerrainMap.clear ();
	m_aMovementMap.clear ();

	m_apTexture.clear ();

	return TRUE;
}

void CD3DTerrainLayer::setWorldTransform (const D3DXMATRIX &matWorld)
{
	m_matWorld = matWorld;
}

BOOL CD3DTerrainLayer::readHeightMap (const std::string &strHeightmapFile)
{
	CD3DBitmap cTempBitmap;

	if (FALSE == cTempBitmap.loadBitmap (strHeightmapFile))
	{
		return FALSE;
	}

	m_numRow = cTempBitmap.getHeight ();
	m_numCol = cTempBitmap.getWidth ();
	m_numCellsPerRow = m_numRow - 1;
	m_numCellsPerCol = m_numCol - 1;
	m_rWidth = m_numCellsPerCol * m_rCellSpacing;
	m_rDepth = m_numCellsPerRow * m_rCellSpacing;

	// A height for each vertex
	m_aHeightMap.resize (m_numRow * m_numCol);

	// load height map 
	for (int row = 0; row < m_numRow; row++)
	{
		for (int col = 0; col < m_numCol; col++)
		{
			setHeightmapEntry (row, col, (float) cTempBitmap.getPixel (row, col).blue);
		}
	}

	return TRUE;
}

BOOL CD3DTerrainLayer::readTerrainMap (const std::string &strTerrainMap)
{
	CD3DBitmap cTempBitmap;

	if (FALSE == cTempBitmap.loadBitmap (strTerrainMap))
	{
		return FALSE;
	}

	// A terrain specifier for each vertex
	m_aTerrainMap.resize (m_numRow * m_numCol);

	// load terrain map 
	for (int row = 0; row < m_numRow; row++)
	{
		for (int col = 0; col < m_numCol; col++)
		{
			STileCode sTempCode;
			sTempCode.blue = (int) cTempBitmap.getPixel (row, col).blue;
			sTempCode.green = (int) cTempBitmap.getPixel (row, col).green;
			sTempCode.red = (int) cTempBitmap.getPixel (row, col).red;
			setTerrainMapEntry (row, col, sTempCode);
		}
	}

	return TRUE;
}

BOOL CD3DTerrainLayer::readMovementMap (const std::string &strMovementMap)
{
	CD3DBitmap cTempBitmap;

	if (FALSE == cTempBitmap.loadBitmap (strMovementMap))
	{
		return FALSE;
	}

	// A movement specifier for each vertex
	m_aMovementMap.resize (m_numRow * m_numCol);

	// load movement map 
	for (int row = 0; row < m_numRow; row++)
	{
		for (int col = 0; col < m_numCol; col++)
		{
			EMovementType sTempMove;
			if (CD3DBitmapPixel (255, 0, 0) ==  cTempBitmap.getPixel (row, col))
			{
				sTempMove = MOVEMENT_BLOCK;
			}
			else if (CD3DBitmapPixel (0, 255, 0) ==  cTempBitmap.getPixel (row, col))
			{
				sTempMove = MOVEMENT_ALLOW;
			}
			else if (CD3DBitmapPixel (255, 255, 255) ==  cTempBitmap.getPixel (row, col))
			{
				sTempMove = MOVEMENT_LEVELUP;
			}
			else if (CD3DBitmapPixel (0, 0, 0) ==  cTempBitmap.getPixel (row, col))
			{
				sTempMove = MOVEMENT_LEVELDOWN;
			}
			else
			{
				sTempMove = MOVEMENT_BLOCK;
			}

			setMovementMapEntry (row, col, sTempMove);
		}
	}

	return TRUE;
}

BOOL CD3DTerrainLayer::computeMesh ()
{
	int nNumVertices = 4 * m_numCellsPerRow * m_numCellsPerCol;
	int nNumIndices = 6 * m_numCellsPerRow * m_numCellsPerCol;

	// Create the encapsulated mesh
	HRESULT hr = D3DXCreateMeshFVF (nNumIndices / 3, nNumVertices,
		D3DXMESH_32BIT | D3DXMESH_MANAGED, STerrainVertex::FVF, m_pD3DDevice, &m_pTerrainMesh);
	if(FAILED(hr))
	{
		return FALSE;
	}

	STerrainVertex* aVertexBuff = 0;
	m_pTerrainMesh->LockVertexBuffer(0, (void**)&aVertexBuff);

	for (int j = 0; j < m_numCellsPerRow; j++)
	{
		for (int i = 0; i < m_numCellsPerCol; i++)
		{
			int index = j * m_numCellsPerCol + i;
			aVertexBuff[4 * index] = STerrainVertex (((float)i - 0.5f * (float)m_numCellsPerCol) * m_rCellSpacing,
				getHeightmapEntry (j, i),
				((float)j - 0.5f * (float)m_numCellsPerRow) * m_rCellSpacing,
				0.0f, 1.0f, 0.0f,
				0.0, 0.0);
			aVertexBuff[4 * index + 1] = STerrainVertex (((float)i + 1.0f - 0.5f * (float)m_numCellsPerCol) * m_rCellSpacing,
				getHeightmapEntry (j, i + 1),
				((float)j - 0.5f * (float)m_numCellsPerRow) * m_rCellSpacing,
				0.0f, 1.0f, 0.0f,
				1.0, 0.0);
			aVertexBuff[4 * index + 2] = STerrainVertex (((float)i - 0.5f * (float)m_numCellsPerCol) * m_rCellSpacing,
				getHeightmapEntry (j + 1, i),
				((float)j + 1.0f - 0.5f * (float)m_numCellsPerRow) * m_rCellSpacing,
				0.0f, 1.0f, 0.0f,
				0.0, 1.0);
			aVertexBuff[4 * index + 3] = STerrainVertex (((float)i + 1.0f - 0.5f * (float)m_numCellsPerCol) * m_rCellSpacing,
				getHeightmapEntry (j + 1, i + 1),
				((float)j + 1.0f - 0.5f * (float)m_numCellsPerRow) * m_rCellSpacing,
				0.0f, 1.0f, 0.0f,
				1.0, 1.0);
		}
	}

	m_pTerrainMesh->UnlockVertexBuffer();

	DWORD* aIndexBuff = 0;
	m_pTerrainMesh->LockIndexBuffer(0, (void**)&aIndexBuff);

	// index to start of a group of 6 indices that describe the
	// two triangles that make up a quad
	int baseIndex = 0, baseVertex = 0;

	// loop through and compute the triangles of each quad
	for(int j = 0; j < m_numCellsPerRow; j++)
	{
		for(int i = 0; i < m_numCellsPerCol; i++)
		{
			aIndexBuff[baseIndex++] = baseVertex;
			aIndexBuff[baseIndex++] = baseVertex + 2;
			aIndexBuff[baseIndex++] = baseVertex + 1;

			aIndexBuff[baseIndex++] = baseVertex + 1;
			aIndexBuff[baseIndex++] = baseVertex + 2;
			aIndexBuff[baseIndex++] = baseVertex + 3;
			baseVertex += 4;
		}
	}
	m_pTerrainMesh->UnlockIndexBuffer();

	return true;
}

float CD3DTerrainLayer::getHeightmapEntry (int row, int col)
{
	if (row >= 0 && row < m_numRow && col >= 0 && col < m_numCol)
		return m_aHeightMap[row * m_numCol + col];
	else
		return 0.0;
}

void CD3DTerrainLayer::setHeightmapEntry (int row, int col, float value)
{
	if (row >= 0 && row < m_numRow && col >= 0 && col < m_numCol)
		m_aHeightMap[row * m_numCol + col] = value;
}

STileCode CD3DTerrainLayer::getTerrainMapEntry (int row, int col)
{
	if (row >= 0 && row < m_numRow && col >= 0 && col < m_numCol)
		return m_aTerrainMap[row * m_numCol + col];
	else
	{
		STileCode sTempCode = {0,};
		return sTempCode;
	}
}

void CD3DTerrainLayer::setTerrainMapEntry (int row, int col, const STileCode &sTileCode)
{
	if (row >= 0 && row < m_numRow && col >= 0 && col < m_numCol)
		m_aTerrainMap[row * m_numCol + col] = sTileCode;
}

EMovementType CD3DTerrainLayer::getMovementMapEntry (int row, int col)
{
	if (row >= 0 && row < m_numRow && col >= 0 && col < m_numCol)
		return m_aMovementMap[row * m_numCol + col];
	else
		return MOVEMENT_BLOCK;
}

void CD3DTerrainLayer::setMovementMapEntry (int row, int col, EMovementType &eMovementType)
{
	if (row >= 0 && row < m_numRow && col >= 0 && col < m_numCol)
		m_aMovementMap[row * m_numCol + col] = eMovementType;
}

float CD3DTerrainLayer::getHeight (float x, float z)
{
	if (NULL == m_pD3DDevice)
		return 0.0;

	// Translate on xz-plane by the transformation that takes
	// the terrain START point to the origin.
	x = (m_rWidth / 2.0f) + x;
	z = (m_rDepth / 2.0f) + z;

	// Scale down by the transformation that makes the 
	// cellspacing equal to one.  This is given by 
	// 1 / cellspacing since; cellspacing * 1 / cellspacing = 1.
	x /= m_rCellSpacing;
	z /= m_rCellSpacing;

	// From now on, we will interpret our positive z-axis as
	// going in the 'down' direction, rather than the 'up' direction.
	// This allows to extract the row and column simply by 'flooring'
	// x and z:
	int col = (int)floorf(x);
	int row = (int)floorf(z);

	// get the heights of the quad we're in:
	// 
	//  A   B
	//  *---*
	//  | / |
	//  *---*  
	//  C   D
	float A = getHeightmapEntry(row,   col);
	float B = getHeightmapEntry(row,   col + 1);
	float C = getHeightmapEntry(row + 1, col);
	float D = getHeightmapEntry(row + 1, col + 1);

	//
	// Find the triangle we are in:
	//

	// Translate by the transformation that takes the upper-left
	// corner of the cell we are in to the origin.  Recall that our 
	// cellspacing was nomalized to 1.  Thus we have a unit square
	// at the origin of our +x -> 'right' and +z -> 'down' system.
	float dx = x - col;
	float dz = z - row;

	// Note the below compuations of u and v are unneccessary, we really
	// only need the height, but we compute the entire vector to emphasis
	// the books discussion.
	float height = 0.0f;
	if(dz < 1.0f - dx)  // upper triangle ABC
	{
		float uy = B - A; // A->B
		float vy = C - A; // A->C

		// Linearly interpolate on each vector.  The height is the vertex
		// height the vectors u and v originate from {A}, plus the heights
		// found by interpolating on each vector u and v.
		height = A + linearInterpolate (0.0f, uy, dx) + linearInterpolate (0.0f, vy, dz);
	}
	else // lower triangle DCB
	{
		float uy = C - D; // D->C
		float vy = B - D; // D->B

		// Linearly interpolate on each vector.  The height is the vertex
		// height the vectors u and v originate from {D}, plus the heights
		// found by interpolating on each vector u and v.
		height = D + linearInterpolate (0.0f, uy, 1.0f - dx) + linearInterpolate (0.0f, vy, 1.0f - dz);
	}

	return height;
}

bool CD3DTerrainLayer::isValidPosition (float x, float z)
{
	if (MOVEMENT_ALLOW == getMovementMap (x, z))
	{
		return true;
	}
	else
	{
		return false;
	}
}

EMovementType CD3DTerrainLayer::getMovementMap (float x, float z)
{
	if (NULL == m_pD3DDevice)
		return MOVEMENT_BLOCK;

	// Translate on xz-plane by the transformation that takes
	// the terrain START point to the origin.
	x = (m_rWidth / 2.0f) + x;
	z = (m_rDepth / 2.0f) + z;

	// Scale down by the transformation that makes the 
	// cellspacing equal to one.  This is given by 
	// 1 / cellspacing since; cellspacing * 1 / cellspacing = 1.
	x /= m_rCellSpacing;
	z /= m_rCellSpacing;

	// From now on, we will interpret our positive z-axis as
	// going in the 'down' direction, rather than the 'up' direction.
	// This allows to extract the row and column simply by 'flooring'
	// x and z:
	int col = (int)floorf(x);
	int row = (int)floorf(z);

	return getMovementMapEntry (row, col);
}

CD3DTerrain::CD3DTerrain ()
{
	unloadTerrain ();
}

CD3DTerrain::~CD3DTerrain ()
{
	unloadTerrain ();
}

BOOL CD3DTerrain::loadTerrain (IDirect3DDevice9* pDevice, const std::string &strTerrainFile)
{
	m_pD3DDevice = pDevice;
	// File input
	std::ifstream InFile (strTerrainFile.c_str());
	if (!InFile)
		return FALSE;
#ifdef _DEBUG
	cout << "Loading terrain file... " << strTerrainFile.c_str() << endl;
#endif
	int n_nTextureCount = 0;
	int	m_nLayerCount = 0;
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
		else if ( 0 == strcmp( szCommand, "tile" ) )
		{
			int r = 0, g = 0, b = 0;
			char szTileTexFile [_MAX_PATH];
			szTileTexFile [0] = 0;
			sscanf (szLineBuff, "%s%d%d%d%s", szCommand, &r, &g, &b, szTileTexFile);

			IDirect3DTexture9* pTexture;
			if (FAILED (D3DXCreateTextureFromFile (m_pD3DDevice, szTileTexFile, &pTexture)))
				return FALSE;
			m_apTexture.push_back (pTexture);

			STileCode sTempCode;
			sTempCode.red = r;
			sTempCode.green = g;
			sTempCode.blue = b;
			m_asTileCodes.push_back (sTempCode);
		}
		else if ( 0 == strcmp( szCommand, "layer" ) )
		{
			float bh = 0.0f, cs = 1.0f, hs = 1.0f;
			char szHeightMap[_MAX_PATH], szTerrainMap[_MAX_PATH], szMovementMap[_MAX_PATH];
			szHeightMap [0] = 0; szTerrainMap [0] = 0; szMovementMap [0] = 0;
			sscanf (szLineBuff, "%s%s%f%f%f%s%s", szCommand, szHeightMap, &cs, &bh, &hs, szTerrainMap, szMovementMap);

			CD3DTerrainLayer *pTerrainLayer = new CD3DTerrainLayer;
			pTerrainLayer->loadTerrainLayer (m_pD3DDevice, szHeightMap, cs, bh, hs, szTerrainMap, m_asTileCodes, m_apTexture, szMovementMap);
			m_apTerrainLayer.push_back (pTerrainLayer);
			m_nCurrentLayer = 0;
		}
		else
		{
			// unimplemented or unrecognized command
		}
	}

	return TRUE;
}

BOOL CD3DTerrain::renderTerrain ()
{
	for (unsigned int iLayer = 0; iLayer < m_apTerrainLayer.size(); iLayer++)
	{
		m_apTerrainLayer [iLayer]->renderTerrainLayer ();
	}

	return TRUE;
}

BOOL CD3DTerrain::unloadTerrain ()
{
	m_pD3DDevice = NULL;

	m_nCurrentLayer = -1;

	m_asTileCodes.clear ();

	for (unsigned int i = 0; i < m_apTexture.size(); i++)
	{
		_SAFE_RELEASE (m_apTexture[i]);
	}
	m_apTexture.clear ();

	for (unsigned int i = 0; i < m_apTerrainLayer.size(); i++)
	{
		delete m_apTerrainLayer[i];
	}

	m_apTerrainLayer.clear ();

	return TRUE;
}

bool CD3DTerrain::isValidTerrain ()
{
	return (getLayerCount() > 0);
}

void CD3DTerrain::setCurrentLayerID (int index)
{
	if (index >= 0 && index < getLayerCount ())
	{
		m_nCurrentLayer = index;
	}
}

int CD3DTerrain::getLayerCount ()
{
	return m_apTerrainLayer.size();
}

int CD3DTerrain::getCurrentLayerID ()
{
	return m_nCurrentLayer;
}

//CD3DTerrainLayer& CD3DTerrain::getCurrentLayer ()
//{
//	return *(m_apTerrainLayer[m_nCurrentLayer]);
//}
//
//CD3DTerrainLayer& CD3DTerrain::getLayer (int index)
//{
//	return *(m_apTerrainLayer[index]);
//}

bool CD3DTerrain::isValidPosition (float x, float z, int iLayer)
{
	if (getLayerCount() <= 0)
	{
		return false;
	}
	else if (iLayer < 0 || iLayer >= getLayerCount())
	{
		iLayer = m_nCurrentLayer;
		if (-1 == iLayer)
		{
			return false;
		}
	}

	return m_apTerrainLayer[iLayer]->isValidPosition(x, z);
}

float CD3DTerrain::getHeight (float x, float z, int iLayer)
{
	if (getLayerCount() <= 0)
	{
		return 0.0f;
	}
	else if (iLayer < 0 || iLayer >= getLayerCount ())
	{
		iLayer = m_nCurrentLayer;
		if (-1 == iLayer)
		{
			return 0.0f;
		}
	}

	return m_apTerrainLayer[iLayer]->getHeight(x, z);
}

void CD3DTerrain::updateTerrainLevel (float x, float z)
{
	if (-1 == m_nCurrentLayer || getLayerCount() <= 0)
	{
		return;
	}

	EMovementType eMoveType = m_apTerrainLayer[m_nCurrentLayer]->getMovementMap (x, z);

	if (MOVEMENT_LEVELUP == eMoveType)
	{
		if (m_nCurrentLayer < getLayerCount() - 1)
		{
			m_nCurrentLayer++;
		}
	}
	else if (MOVEMENT_LEVELDOWN == eMoveType)
	{
		if (m_nCurrentLayer > 0)
		{
			m_nCurrentLayer--;
		}
	}
}
