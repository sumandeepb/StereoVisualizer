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

// D3DRenderer.h - direct 3D renderer class declaration
#pragma once

#include <vector>
#include "D3Dutility.h"
#include "D3DSettings.h"
#include "D3DCamera.h"
#include "D3DTerrain.h"
#include "D3DModel.h"
#include "D3DGraphDraw.h"

// CD3DRenderer
class CD3DRenderer
{
public:
	// construction
	CD3DRenderer(CD3DSettings& cSettings);
	virtual ~CD3DRenderer();

protected:
	// implementation
	CD3DSettings&			m_cSettings;

	HWND					m_hRenderWnd;
	BOOL					m_fWindowed;
	UINT					m_nWidth;
	UINT					m_nHeight;

	IDirect3D9*				m_pD3D;
	IDirect3DDevice9*		m_pD3DDevice;
	ID3DXFont*				m_pMsgStringFont;
	bool					m_fSaveSnapShot;
	D3DVIEWPORT9			m_asScreenShotViewPort;
	D3DXMATRIX				m_sFrontalPersProjection;
	//D3DXMATRIX			m_sPeripheralPersProjection;
	D3DVIEWPORT9			m_asFrontalViewPort[2];		// used in pass 1 (wide fov scene rendering)
	//D3DVIEWPORT9			m_asPeripheralViewPort[2];	// used in pass 1 (wide fov scene rendering)
	D3DVIEWPORT9			m_asWarpViewPort[2];		// used in pass 2 (distortion warping)
	CD3DCamera				m_cCamera;


	int						m_nSelectedViewport;		// see function ResetParameters() for details
	bool					m_fStereo;
	bool					m_fCrossEyed;
	bool					m_fDistortion;
	float					m_rFOV;
	float					m_rFrustumOffset;
	float					m_rEyeOffset;
	float					m_rHorizOverlap[2];
	float					m_rVertiAlign[2];
	float					m_rBarrelFactor;

	ID3DXEffect*			m_pDistEffect;
	IDirect3DSurface9*		m_pBackBuffer;
	IDirect3DTexture9*		m_pPreWarpBuffer;
	IDirect3DVertexDeclaration9* m_pVertDeclaration;

	CD3DTerrain				m_cTerrain;
	std::vector<CD3DModel*>	m_apModel;
	CD3DGraph*				m_pGraph;

	void drawWarpedBuffer (int iShader, int iViewPort);

public:
	// exposed routines
	void ResetParameters ();
	BOOL initD3D (HWND hWnd, UINT nWidth = 0, UINT nHeight = 0, BOOL fWindowed = FALSE);
	BOOL setupD3D (const std::string &strAssetFile);
	BOOL displayD3D (float fTimeDelta);
	void cleanupD3D ();
	void deInitD3D ();

public:
	// message handler
	void OnKeyDown (WPARAM wParam, LPARAM lParam);
	void AsyncKeyDown (float fTimeDelta);
	void MouseMove (float rMoveX, float rMoveY);
	void MouseSpin (float rSpinX, float rSpinY);
};
