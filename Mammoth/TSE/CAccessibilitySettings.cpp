#include "PreComp.h"
#include "TSEAccessibilitySettings.h"
#include <sstream>

CAccessibilitySettings::CAccessibilitySettings(void) {
	m_rgbColorPlayer = CG32bitPixel(255, 255, 255);
	m_rgbColorEscort = CG32bitPixel(80, 200, 200);
	m_rgbColorFriendly = CG32bitPixel(80, 255, 80);
	m_rgbColorNeutral = CG32bitPixel(80, 80, 255);
	m_rgbColorEnemy = CG32bitPixel(255, 80, 80);
	m_rgbColorAngry = CG32bitPixel(230, 150, 80);
	m_rgbColorProjectile = CG32bitPixel(255, 255, 0);
};
CAccessibilitySettings::~CAccessibilitySettings(void) {};

void CAccessibilitySettings::SetIFFColor (CString sSettingsValue, int iIFFType) {
	std::stringstream hex_ss;
	hex_ss << std::hex << sSettingsValue;
	unsigned int iRawColor;
	hex_ss >> iRawColor;
	BYTE bBlue = (BYTE)iRawColor;
	BYTE bGreen = (BYTE)(iRawColor >> 8);
	BYTE bRed = (BYTE)(iRawColor >> 16);
	CG32bitPixel rgbColor = CG32bitPixel(bRed, bGreen, bBlue);
	switch (iIFFType) {
	case IFFType::player: {
		m_rgbColorPlayer = rgbColor;
		break;
	}
	case IFFType::escort: {
		m_rgbColorEscort = rgbColor;
		break;
	}
	case IFFType::friendly: {
		m_rgbColorFriendly = rgbColor;
		break;
	}
	case IFFType::neutral: {
		m_rgbColorNeutral = rgbColor;
		break;
	}
	case IFFType::enemy: {
		m_rgbColorEnemy = rgbColor;
		break;
	}
	case IFFType::angry: {
		m_rgbColorAngry = rgbColor;
		break;
	}
	case IFFType::projectile: {
		m_rgbColorProjectile = rgbColor;
		break;
	}
	}
};

CG32bitPixel CAccessibilitySettings::GetIFFColor (int iIFFType) {
	switch (iIFFType) {
	case IFFType::player:
		return m_rgbColorPlayer;
	case IFFType::escort:
		return m_rgbColorEscort;
	case IFFType::friendly:
		return m_rgbColorFriendly;
	case IFFType::neutral:
		return m_rgbColorNeutral;
	case IFFType::enemy:
		return m_rgbColorEnemy;
	case IFFType::angry:
		return m_rgbColorAngry;
	case IFFType::projectile:
		return m_rgbColorProjectile;
	default:
		return NULL;
	}
};
