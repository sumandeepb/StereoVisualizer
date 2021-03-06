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

#include "D3Dutility.h"

D3DXCOLOR vectorToColor (const D3DXVECTOR3 &vec, float rAlpha)
{
	return D3DXCOLOR (vec.x, vec.y, vec.z, rAlpha);
}

ID3DXFont* initFont (IDirect3DDevice9* pD3DDevice)
{
	// Initialize a LOGFONT structure that describes the font we want to create.
	D3DXFONT_DESC lf;
	::ZeroMemory (&lf, sizeof(D3DXFONT_DESC));

	lf.Height			= 25;    // in logical units
	lf.Width			= 12;    // in logical units
	lf.Weight			= 500;   // boldness, range 0(light) - 1000(bold)
	lf.Italic			= false;   
	lf.CharSet			= DEFAULT_CHARSET;
	lf.OutputPrecision	= 0;              
	lf.Quality			= 0;           
	lf.PitchAndFamily	= 0;           
	strcpy (lf.FaceName, "Times New Roman"); // font style

	// Create an ID3DXFont based on 'lf'.
	ID3DXFont* pFont;
	if (FAILED (D3DXCreateFontIndirect (pD3DDevice, &lf, &pFont)))
	{
		return NULL;
	}

	return pFont;
}

D3DLIGHT9 initPointLight (const D3DXCOLOR& color, const D3DXVECTOR3& position)
{
	D3DLIGHT9 light;
	::ZeroMemory (&light, sizeof(light));

	light.Type      = D3DLIGHT_POINT;
	light.Ambient   = color * 0.4f;
	light.Diffuse   = color;
	light.Specular  = color * 0.6f;
	light.Position  = position;
	light.Range     = 1000.0f;
	light.Falloff      = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;

	return light;
}

D3DLIGHT9 initSpotLight (const D3DXCOLOR& color, const D3DXVECTOR3& position, const D3DXVECTOR3& direction)
{
	D3DLIGHT9 light;
	::ZeroMemory (&light, sizeof(light));
	D3DXVECTOR3	normalizedDir;
	D3DXVec3Normalize (&normalizedDir, &direction);

	light.Type      = D3DLIGHT_SPOT;
	light.Ambient   = color * 0.4f;
	light.Diffuse   = color;
	light.Specular  = color * 0.6f;
	light.Position  = position;
	light.Direction = normalizedDir;
	light.Range        = 1000.0f;
	light.Falloff      = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;
	light.Theta        = 0.5f;
	light.Phi          = 0.7f;

	return light;
}

D3DLIGHT9 initDirectionalLight (const D3DXCOLOR& color, const D3DXVECTOR3& direction)
{
	D3DLIGHT9 light;
	::ZeroMemory (&light, sizeof(light));
	D3DXVECTOR3	normalizedDir;
	D3DXVec3Normalize (&normalizedDir, &direction);

	light.Type      = D3DLIGHT_DIRECTIONAL;
	light.Ambient   = color * 0.4f;
	light.Diffuse   = color;
	light.Specular  = color * 0.6f;
	light.Direction = normalizedDir;

	return light;
}

D3DMATERIAL9 initMtrl (const D3DXCOLOR& a, const D3DXCOLOR& d, const D3DXCOLOR& s, const D3DXCOLOR& e, float p)
{
	D3DMATERIAL9 mtrl;
	mtrl.Ambient  = a;
	mtrl.Diffuse  = d;
	mtrl.Specular = s;
	mtrl.Emissive = e;
	mtrl.Power    = p;
	return mtrl;
}

D3DXVECTOR3 computeNormal (const D3DXVECTOR3& p0, const D3DXVECTOR3& p1, const D3DXVECTOR3& p2)
{
	D3DXVECTOR3 out;
	D3DXVECTOR3 u = p1 - p0;
	D3DXVECTOR3 v = p2 - p0;
	D3DXVec3Cross (&out, &u, &v);
	D3DXVec3Normalize (&out, &out);
	return out;
}
