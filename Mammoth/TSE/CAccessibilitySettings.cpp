#include "PreComp.h"
#include "TSEAccessibilitySettings.h"

void CAccessibilitySettings::SetIFFColor (CString sSettingsValue, CAccessibilitySettings::IFFType iIFFType)
	{
	m_aIFFColors[(int)iIFFType] = LoadRGBColor(sSettingsValue);
	};

CG32bitPixel CAccessibilitySettings::GetIFFColor (CAccessibilitySettings::IFFType iIFFType) const
	{
	return m_aIFFColors[(int)iIFFType];
	};
