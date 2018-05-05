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
