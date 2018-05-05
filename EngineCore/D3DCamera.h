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

// D3DCamera.h - viewpoint camera class declaration
#pragma once

#include "D3Dutility.h"

enum ECameraType { CAMTYPE_LANDOBJECT, CAMTYPE_AIRCRAFT };

class CD3DCamera
{
protected:
	ECameraType		m_eCameraType;
	D3DXVECTOR3		m_pos;
	D3DXVECTOR3		m_right;
	D3DXVECTOR3		m_up;
	D3DXVECTOR3		m_look;
	float			m_height;

public:
	CD3DCamera ();
	CD3DCamera (ECameraType eCameraType);
	~CD3DCamera ();

	void setCameraType (ECameraType eCameraType);
	void SetCameraHeight (float height);
	void setPosition (const D3DXVECTOR3 &pos);
	void setLook (const D3DXVECTOR3 &look);

	void strafe (float units); // left/right
	void fly (float units);    // up/down
	void walk (float units);   // forward/backward

	void pitch (float angle); // rotate on right vector
	void yaw (float angle);   // rotate on up vector
	void roll (float angle);  // rotate on look vector

	ECameraType getCameraType () const; 
	float GetCameraHeight () const;
	void getPosition (D3DXVECTOR3 &pos) const; 
	void getRight (D3DXVECTOR3 &right) const;
	void getUp (D3DXVECTOR3 &up) const;
	void getLook (D3DXVECTOR3 &look) const;

	void getViewMatrix (D3DXMATRIX &view); 
};
