#pragma once
#include "DirectXUtil.h"
#include "Kernel.h"

using namespace Kernel;

class CAccessibilitySettings
{
public:
	enum class IFFType
		{
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
	void SetIFFColor (CString sSettingsValue, CAccessibilitySettings::IFFType iIFFType);
	CG32bitPixel GetIFFColor (CAccessibilitySettings::IFFType iIFFType);
protected:

private:
	CG32bitPixel m_aColors[7]
		{
		CG32bitPixel(255, 255, 255),	//player
		CG32bitPixel(80, 200, 200),		//escort
		CG32bitPixel(80, 255, 80),		//friendly
		CG32bitPixel(80, 80, 255),		//neutral
		CG32bitPixel(255, 80, 80),		//enemy
		CG32bitPixel(230, 150, 80),		//angry
		CG32bitPixel(255, 255, 0),		//projectile
		};
};
