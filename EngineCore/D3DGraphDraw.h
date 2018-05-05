#pragma once

#include <string>
#include <vector>
#include "D3DUtility.h"

class CD3DGraph
{
public:
	struct SVertex
	{
		SVertex () {}
		SVertex (float x, float y, float z, 
			float nx, float ny, float nz)
		{
			_x = x;   _y = y;   _z = z;
			_nx = nx; _ny = ny; _nz = nz;
		}

		float _x, _y, _z, _nx, _ny, _nz;

		static const DWORD FVF;
	};

protected:
	IDirect3DDevice9*			m_pD3DDevice;
	D3DXMATRIX					m_matWorld;
	LPD3DXMESH					m_pNodeSphere;
	D3DMATERIAL9				m_sNodeMaterial;
	std::vector<D3DXMATRIX>		m_aNodeTransform;
	std::vector<LPD3DXMESH>		m_apEdgeCylinder;
	D3DMATERIAL9				m_sEdgeMaterial;
	std::vector<D3DXMATRIX>		m_aEdgeTransform;
	//IDirect3DVertexBuffer9*		m_pEdgeLineBuffer;
	
	std::vector<D3DXVECTOR3>	m_asNodes;
	std::vector<std::pair<D3DXVECTOR3, D3DXVECTOR3>> m_asEdges;
	//std::vector<IDirect3DTexture9*>	m_apTexture;

public:
	CD3DGraph(void);
	~CD3DGraph(void);
	BOOL load3DGraph (IDirect3DDevice9* pD3DDevice, const std::string &strGraphFile, const float rScale = 1.0f);
	void setWorldTransform (const D3DXMATRIX &matWorld);
	BOOL renderGraph ();
	BOOL unload3DGraph ();
};
