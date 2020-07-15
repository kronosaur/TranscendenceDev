#include "PreComp.h"
#include "TSEAccessibilitySettings.h"
#include <sstream>

CAccessibilitySettings::CAccessibilitySettings(void) {};
CAccessibilitySettings::~CAccessibilitySettings(void) {};

void CAccessibilitySettings::SetIFFColor (CString sSettingsValue, CAccessibilitySettings::IFFType iIFFType)
	{
	std::stringstream hex_ss;
	hex_ss << std::hex << sSettingsValue;
	unsigned int iRawColor;
	hex_ss >> iRawColor;
	BYTE bBlue = (BYTE)iRawColor;
	BYTE bGreen = (BYTE)(iRawColor >> 8);
	BYTE bRed = (BYTE)(iRawColor >> 16);
	CG32bitPixel rgbColor = CG32bitPixel(bRed, bGreen, bBlue);
	m_aIFFColors[(int)iIFFType] = rgbColor;
	};

CG32bitPixel CAccessibilitySettings::GetIFFColor (CAccessibilitySettings::IFFType iIFFType)
{
	return m_aIFFColors[(int)iIFFType];
};
