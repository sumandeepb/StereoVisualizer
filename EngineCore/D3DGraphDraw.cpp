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

// D3DGraphDraw.cpp - 
#include <iostream>
#include <fstream>

#include "D3DGraphDraw.h"

using namespace std;

const DWORD CD3DGraph::SVertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL; //  | D3DFVF_TEX1;

CD3DGraph::CD3DGraph(void)
{
	// initialize device
	m_pD3DDevice = NULL;
	// initialize world matrix
	D3DXMatrixTranslation(&m_matWorld,  0.0f, 0.0f,  0.0f);
	// initialize sphere properties
	m_pNodeSphere = NULL;
	m_sNodeMaterial = initMtrl(0.6f * RED, RED, WHITE, BLACK, 100.0f);
	// initialise cylinder properties
	//m_pEdgeCylinder = NULL;
	m_sEdgeMaterial = initMtrl(0.6f * BLUE, BLUE, WHITE, BLACK, 100.0f);
}

CD3DGraph::~CD3DGraph(void)
{
	unload3DGraph();
}

BOOL CD3DGraph::load3DGraph (IDirect3DDevice9* pD3DDevice, const string &strGraphFile, const float rScale)
{
	// set device and model parameters
	m_pD3DDevice = pD3DDevice;

	// File input
	ifstream InFile (strGraphFile);
	if (!InFile)
		return S_FALSE;

#ifdef _DEBUG
	cout << "Loading graph file... " << strGraphFile << endl;
#endif

	// create sphere mesh
	const float rRadius = 0.5f;
	D3DXCreateSphere (m_pD3DDevice, rRadius, 32, 32, &m_pNodeSphere, NULL);

	char szLineBuff [_MAX_PATH];
	char szCommand [_MAX_PATH];
	while (1)
	{
		InFile.getline (szLineBuff, _MAX_PATH);
		if( !InFile )
			break;

		szCommand [0] = 0;
		sscanf (szLineBuff, "%s", szCommand);

		if( 0 == strcmp( szCommand, "n" ) )
		{
			D3DXVECTOR3 nodeCenter;
			sscanf (szLineBuff, "%s%f%f%f", szCommand, &nodeCenter.x, &nodeCenter.y, &nodeCenter.z);
			nodeCenter.x = -nodeCenter.x; // DirectX uses LH Coordinate System
			m_asNodes.push_back (nodeCenter);

			D3DXMATRIX	matNodeTransform;
			D3DXMatrixTranslation(&matNodeTransform, rScale * nodeCenter.x, rScale * nodeCenter.y, rScale * nodeCenter.z);
			m_aNodeTransform.push_back (matNodeTransform);
		}
		else if( 0 == strcmp( szCommand, "e" ) )
		{
			D3DXVECTOR3 edgeStart, edgeEnd;
			sscanf (szLineBuff, "%s%f%f%f%f%f%f", szCommand, &edgeStart.x, &edgeStart.y, &edgeStart.z, &edgeEnd.x, &edgeEnd.y, &edgeEnd.z);
			edgeStart.x = -edgeStart.x; // DirectX uses LH Coordinate System
			edgeEnd.x = -edgeEnd.x;
			m_asEdges.push_back (pair<D3DXVECTOR3, D3DXVECTOR3> (edgeStart, edgeEnd));

			D3DXVECTOR3 K(0.0f, 0.0f, 1.0f);
			D3DXVECTOR3 A = edgeEnd - edgeStart;
			D3DXVECTOR3 C = edgeStart + 0.5f * A;
			D3DXVECTOR3 N;
			D3DXVec3Cross (&N, &A, &K);

			LPD3DXMESH pCylinderMesh = NULL;
			D3DXCreateCylinder (m_pD3DDevice, 0.25f * rRadius, 0.25f * rRadius, rScale * D3DXVec3Length (&A), 32, 32, &pCylinderMesh, NULL);
			m_apEdgeCylinder.push_back (pCylinderMesh);

			D3DXMATRIX	matRotation, matTranslation, matCombo;
			D3DXMatrixRotationAxis (&matRotation, &N, -acos (D3DXVec3Dot (&A, &K) / D3DXVec3Length(&A)));
			D3DXMatrixTranslation(&matTranslation, rScale * C.x, rScale * C.y, rScale * C.z);
			D3DXMatrixMultiply(&matCombo, &matRotation, &matTranslation);
			m_aEdgeTransform.push_back (matCombo);
		}
		else if( 0 == strcmp( szCommand, "#" ) )
		{
			// Comment
		}
		else
		{
			// unimplemented or unrecognized command
		}
	}

	InFile.close();

#ifdef _DEBUG
	cout << "Loaded graph file: " << strGraphFile << endl;
#endif

	//m_pD3DDevice->CreateVertexBuffer(
	//	2 * m_asEdges.size() * sizeof(SEdgeVertex), // size in bytes
	//	D3DUSAGE_WRITEONLY, // flags
	//	CD3DGraph::SEdgeVertex::FVF,        // vertex format
	//	D3DPOOL_MANAGED,    // managed memory pool
	//	&m_pEdgeLineBuffer,          // return create vertex buffer
	//	0);                 // not used - set to 0

	//for (unsigned int i = 0; i < m_asEdges.size(); i++)
	//{
	//}

	return TRUE;
}

void CD3DGraph::setWorldTransform (const D3DXMATRIX &matWorld)
{
	// set world matrix - position in world space
	m_matWorld = matWorld;
}

BOOL CD3DGraph::renderGraph ()
{
	if (m_pD3DDevice)
	{
		// render solid
		m_pD3DDevice->SetRenderState (D3DRS_FILLMODE, D3DFILL_SOLID);

		for (unsigned int index = 0; index < m_aNodeTransform.size(); index++)
		{
			// Set the world matrix that positions the node sphere.
			m_pD3DDevice->SetTransform (D3DTS_WORLD, &m_aNodeTransform[index]);

			// Set object material
			m_pD3DDevice->SetMaterial (&m_sNodeMaterial);

			// Set object texture
			//m_pD3DDevice->SetTexture (0, m_apTexture[index]);

			// Draw the object using the previously set world matrix.
			if (m_pNodeSphere)
			{
				m_pNodeSphere->DrawSubset (0);
			}
		}

		for (unsigned int index = 0; index < m_aEdgeTransform.size(); index++)
		{
			// Set the world matrix that positions the node sphere.
			m_pD3DDevice->SetTransform (D3DTS_WORLD, &m_aEdgeTransform[index]);

			// Set object material
			m_pD3DDevice->SetMaterial (&m_sEdgeMaterial);

			// Set object texture
			//m_pD3DDevice->SetTexture (0, m_apTexture[index]);

			// Draw the object using the previously set world matrix.
			if (m_apEdgeCylinder[index])
			{
				m_apEdgeCylinder[index]->DrawSubset (0);
			}
		}
	}

	return  TRUE;
}

BOOL CD3DGraph::unload3DGraph ()
{
	_SAFE_RELEASE (m_pNodeSphere);
	m_aNodeTransform.clear ();

	for (unsigned int i = 0; i < m_apEdgeCylinder.size(); i++)
	{
		_SAFE_RELEASE (m_apEdgeCylinder[i]);
	}
	m_apEdgeCylinder.clear();
	m_aEdgeTransform.clear();

	m_asNodes.clear();
	m_asEdges.clear();

	return TRUE;
}
