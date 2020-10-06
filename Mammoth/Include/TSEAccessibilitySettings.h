//	TSE.h
//
//	Transcendence Space Engine
//	Copyright 2020 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

#include "DirectXUtil.h"
#include "Kernel.h"

class CAccessibilitySettings
	{
	public:
		enum class IFFType
			{
			player			= 0,
			escort			= 1,
			friendly		= 2,
			neutral			= 3,
			enemy			= 4,
			angry			= 5,
			projectile		= 6,
			};

		static constexpr int IFFTYPE_COUNT = 7;

		void SetIFFColor (CString sSettingsValue, CAccessibilitySettings::IFFType iIFFType);
		CG32bitPixel GetIFFColor (CAccessibilitySettings::IFFType iIFFType) const;

	private:
		CG32bitPixel m_aIFFColors[IFFTYPE_COUNT]
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
