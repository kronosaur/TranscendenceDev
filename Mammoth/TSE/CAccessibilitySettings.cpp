#include "PreComp.h"
#include "TSEAccessibilitySettings.h"

CAccessibilitySettings::CAccessibilitySettings(void) {};
CAccessibilitySettings::~CAccessibilitySettings(void) {};

void CAccessibilitySettings::SetIFFColor (CString sSettingsValue, CAccessibilitySettings::IFFType iIFFType)
	{
	m_aIFFColors[(int)iIFFType] = LoadRGBColor(sSettingsValue);
	};

CG32bitPixel CAccessibilitySettings::GetIFFColor (CAccessibilitySettings::IFFType iIFFType)
{
	return m_aIFFColors[(int)iIFFType];
};
