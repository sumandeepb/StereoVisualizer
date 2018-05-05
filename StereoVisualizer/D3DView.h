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

#include "D3Dutility.h"
#include "D3DCamera.h"
#include "D3DModel.h"

// CD3DView
class CD3DView : public CWnd
{
// Construction
public:
	CD3DView();
	virtual ~CD3DView();

	// Attributes
public:

	// Operations
public:

	// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// Implementation
protected:
	IDirect3DDevice9*	m_pD3DDevice;
	BOOL				m_fWindowed;
	int					m_nWidth;
	int					m_nHeight;

	CD3DCamera			m_3dCamera;
	CD3DModel			m_3dModel;
public:
	// exposed routines
	BOOL InitD3D(int nWidth = 0, int nHeight = 0, BOOL fWindowed = TRUE);
	BOOL SetupD3D (const char szModelPath[], const char szModelFile []);
	BOOL DisplayD3D();
	void CleanupD3D ();
	void DeInitD3D ();

	// Generated message map functions
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint ();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};
