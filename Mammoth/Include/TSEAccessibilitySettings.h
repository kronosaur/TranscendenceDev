#pragma once
#include "DirectXUtil.h"
#include "Kernel.h"

using namespace Kernel;

class CAccessibilitySettings {
public:
	enum IFFType {
		player,
		escort,
		friendly,
		neutral,
		enemy,
		angry,
		projectile,
	};
	CAccessibilitySettings (void);
	~CAccessibilitySettings (void);
	void SetIFFColor (CString sSettingsValue, int iIFFType);
	CG32bitPixel GetIFFColor (int iIFFType);
protected:

private:
	CG32bitPixel m_rgbColorPlayer;
	CG32bitPixel m_rgbColorEscort;
	CG32bitPixel m_rgbColorFriendly;
	CG32bitPixel m_rgbColorNeutral;
	CG32bitPixel m_rgbColorEnemy;
	CG32bitPixel m_rgbColorAngry;
	CG32bitPixel m_rgbColorProjectile;
};
