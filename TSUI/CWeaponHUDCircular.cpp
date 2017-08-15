//	CWeaponHUDCircular.cpp
//
//	CWeaponHUDCircular class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define BACKGROUND_COLOR_ATTRIB				CONSTLIT("backgroundColor")
#define TARGET_COLOR_ATTRIB					CONSTLIT("targetColor")
#define WEAPON_COLOR_ATTRIB					CONSTLIT("weaponColor")

#define STR_UNKNOWN_HOSTILE					CONSTLIT("Unknown Hostile")
#define STR_UNKNOWN_FRIENDLY				CONSTLIT("Unknown Friendly")

const int RING_SPACING = 2;
const int NAME_SPACING_X = 20;
const int NAME_WIDTH = 130;

const int RANGE_OFFSET_X =					-24;
const int DAMAGE_OFFSET_X =					16;

const int CURVED_OFFSET[3] =				{ 24, 9, -8 };

const int SHIELD_OFFSET_X =					24;
const int ARMOR_OFFSET_X =					9;
const int HULL_OFFSET_X =					-8;

const CG32bitPixel DISABLED_LABEL_COLOR =	CG32bitPixel(128, 0, 0);

CWeaponHUDCircular::CWeaponHUDCircular (void) :
		m_bInvalid(true)

//	CWeaponHUDCircular constructor

	{
	}

CWeaponHUDCircular::~CWeaponHUDCircular (void)

//	CWeaponHUDCircular destructor

	{
	}

ALERROR CWeaponHUDCircular::Bind (SDesignLoadCtx &Ctx)

//	Bind
//
//	Bind design

	{
	return NOERROR;
	}

void CWeaponHUDCircular::GetBounds (int *retWidth, int *retHeight) const

//	GetBounds
//
//	Returns bounds

	{
	*retWidth = m_cxDisplay;
	*retHeight = m_cyDisplay;
	}

ALERROR CWeaponHUDCircular::InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc)

//	InitFromXML
//
//	Initialize from XML

	{
	const CVisualPalette &VI = g_pHI->GetVisuals();

	//	LATER: Load this from definition

	m_iTargetRadius = 100;

	//	Colors

	m_rgbTargetBack = ::LoadRGBColor(pDesc->GetAttribute(BACKGROUND_COLOR_ATTRIB), CG32bitPixel(55, 58, 64));
	m_rgbTargetText = ::LoadRGBColor(pDesc->GetAttribute(TARGET_COLOR_ATTRIB), CG32bitPixel(5, 211, 5));
	m_rgbWeaponText = ::LoadRGBColor(pDesc->GetAttribute(WEAPON_COLOR_ATTRIB), VI.GetColor(colorTextHighlight));
	m_rgbWeaponBack = m_rgbTargetBack;

	//	Calculate metrics

	int iTotalRadius = m_iTargetRadius + RING_SPACING;
	m_cxDisplay = (2 * iTotalRadius) + (2 * (NAME_SPACING_X + NAME_WIDTH));
	m_cyDisplay = (2 * iTotalRadius) + 2;

	m_xCenter = (iTotalRadius + 1);
	m_yCenter = (iTotalRadius + 1);

	//	Done

	return NOERROR;
	}

void CWeaponHUDCircular::OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx)

//	OnPaint
//
//	Paint

	{
	if (m_bInvalid)
		{
		Realize(Ctx);
		m_bInvalid = false;
		}

	Dest.Blt(0,
			0,
			m_Buffer.GetWidth(),
			m_Buffer.GetHeight(),
			Ctx.byOpacity,
			m_Buffer,
			x,
			y);
	}

void CWeaponHUDCircular::PaintTarget (SHUDPaintCtx &Ctx, CShip *pShip, CSpaceObject *pTarget)

//	PaintTarget
//
//	Paints the target on the back buffer.

	{
	ASSERT(pTarget);

	//	Metrics and resources

	const CVisualPalette &VI = g_pHI->GetVisuals();
	const CG16bitFont &SmallFont = VI.GetFont(fontSmall);
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);
	const CG16bitFont &MediumHeavyBoldFont = VI.GetFont(fontMediumHeavyBold);
	const CG16bitFont &RangeFont = VI.GetFont(fontLarge);

	CG32bitPixel rgbInfoBack = CG32bitPixel(CG32bitPixel::Darken(m_rgbTargetBack, 128), 180);
	CG32bitPixel rgbTitle = CG32bitPixel::Darken(m_rgbTargetText, 180);

	int cyInfoArea = MediumHeavyBoldFont.GetHeight() + 4 * MediumFont.GetHeight();

	//	Paint an image of the target

	if (pTarget->IsIdentified())
		{
		int yCenter = m_yCenter - (cyInfoArea / 3);

		SViewportPaintCtx PaintCtx;
		PaintCtx.pObj = pTarget;
		PaintCtx.XForm = ViewportTransform(pTarget->GetPos(),
				g_KlicksPerPixel,
				m_xCenter,
				yCenter);
		PaintCtx.XFormRel = PaintCtx.XForm;
		PaintCtx.fNoRecon = true;
		PaintCtx.fNoDockedShips = true;
		PaintCtx.fNoSelection = true;

		pTarget->Paint(m_Buffer, m_xCenter, yCenter, PaintCtx);
		}

	//	Paint information about the target below it.

	if (m_InfoArea.IsEmpty())
		CGShape::ArcSegment(CVector(), m_iTargetRadius, 1.5 * PI, cyInfoArea, &m_InfoArea);

	CGDraw::Region(m_Buffer, m_xCenter, m_yCenter, m_InfoArea, rgbInfoBack);

	//	Paint various information

	int xText = m_xCenter;
	int yText = m_yCenter + m_iTargetRadius - cyInfoArea;

	//	Paint the target name

	CString sName;
	if (pTarget->IsIdentified())
		sName = pTarget->GetNounPhrase(nounCapitalize);
	else if (pShip->IsEnemy(pTarget))
		sName = STR_UNKNOWN_HOSTILE;
	else
		sName = STR_UNKNOWN_FRIENDLY;

	MediumHeavyBoldFont.DrawText(m_Buffer,
			xText,
			yText,
			rgbTitle,
			sName,
			CG16bitFont::AlignCenter);
	yText += MediumHeavyBoldFont.GetHeight();

	//	Paint some stats

	CVector vDist = pTarget->GetPos() - pShip->GetPos();
	int iDist = (int)((vDist.Length() / LIGHT_SECOND) + 0.5);
	RangeFont.DrawText(m_Buffer,
			xText + RANGE_OFFSET_X,
			yText,
			m_rgbTargetText,
			strPatternSubst(CONSTLIT("%,d ls"), iDist),
			CG16bitFont::AlignRight);

	//	Paint the damage

	if (pTarget->IsIdentified())
		{
		SVisibleDamage Damage;
		pTarget->GetVisibleDamageDesc(Damage);

		int iOffset = 0;

		if (Damage.iShieldLevel != -1)
			{
			PaintTargetStat(m_Buffer, xText + DAMAGE_OFFSET_X + CURVED_OFFSET[iOffset++], yText, CONSTLIT("Shields:"), strPatternSubst(CONSTLIT("%d%%"), Damage.iShieldLevel));
			yText += MediumFont.GetHeight();
			}

		if (Damage.iArmorLevel != -1)
			{
			PaintTargetStat(m_Buffer, xText + DAMAGE_OFFSET_X + CURVED_OFFSET[iOffset++], yText, CONSTLIT("Armor:"), strPatternSubst(CONSTLIT("%d%%"), Damage.iArmorLevel));
			yText += MediumFont.GetHeight();
			}

		if (Damage.iHullLevel != -1)
			{
			PaintTargetStat(m_Buffer, xText + DAMAGE_OFFSET_X + CURVED_OFFSET[iOffset++], yText, CONSTLIT("Hull:"), strPatternSubst(CONSTLIT("%d%%"), Damage.iHullLevel));
			yText += MediumFont.GetHeight();
			}
		}
	}

void CWeaponHUDCircular::PaintTargetStat (CG32bitImage &Dest, int x, int y, const CString &sLabel, const CString &sStat)

//	PaintTargetStat
//
//	Paints a stat line

	{
	const CVisualPalette &VI = g_pHI->GetVisuals();
	const CG16bitFont &SmallFont = VI.GetFont(fontSmall);
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);

	CG32bitPixel rgbLabel = CG32bitPixel(128, 128, 128);

	int cyOffset = MediumFont.GetAscent() - SmallFont.GetAscent();

	SmallFont.DrawText(Dest,
			x - 2,
			y + cyOffset,
			rgbLabel,
			sLabel,
			CG16bitFont::AlignRight);

	MediumFont.DrawText(Dest,
			x + 2,
			y,
			m_rgbTargetText,
			sStat);
	}

void CWeaponHUDCircular::PaintWeaponStatus (CShip *pShip, CInstalledDevice *pDevice, Metric rAngle)

//	PaintWeaponStatus
//
//	Paints the status of the given named weapon.

	{
	const CVisualPalette &VI = g_pHI->GetVisuals();
	const CG16bitFont &SmallFont = VI.GetFont(fontSmall);
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);
	const CG16bitFont &LargeBoldFont = VI.GetFont(fontLargeBold);

	CItemCtx ItemCtx(pShip, pDevice);
	CDeviceClass *pClass = pDevice->GetClass();

	//	Figure out what we're painting

	CString sVariant;
	int iAmmoLeft;
	pClass->GetSelectedVariantInfo(pShip, pDevice, &sVariant, &iAmmoLeft);
	CString sDevName = pDevice->GetItem()->GetNounPhrase(ItemCtx, nounDuplicateModifier);
	CString sName = (sVariant.IsBlank() ? sDevName : sVariant);

	//	Figure out metrics for the background

	int cxExtra = 20;
	int cxAmmo = 5 * LargeBoldFont.GetAverageWidth();
	int cxBack = MediumFont.MeasureText(sName) + cxAmmo + cxExtra;
	int cyBack = 2 + Max(MediumFont.GetHeight() + SmallFont.GetHeight(), LargeBoldFont.GetHeight());
	CVector vInnerPos = CVector::FromPolarInv(rAngle, m_iTargetRadius + RING_SPACING + 1);
	CVector vOuterPos = vInnerPos + CVector(cxBack, 0.0);

	//	Draw the background

	CGDraw::ArcQuadrilateral(m_Buffer, CVector(m_xCenter, m_yCenter), vInnerPos, vOuterPos, cyBack, m_rgbWeaponBack, CGDraw::blendCompositeNormal);

	//	Figure out what color to use

	CG32bitPixel rgbColor;
	if (pDevice->IsEnabled() && !pDevice->IsDamaged() && !pDevice->IsDisrupted())
		rgbColor = m_rgbWeaponText;
	else
		rgbColor = DISABLED_LABEL_COLOR;

	//	Paint the weapon name

	int xText = m_xCenter + (int)vOuterPos.GetX() - cxAmmo;
	int yText = m_yCenter + (int)vOuterPos.GetY() - (cyBack / 2);
	MediumFont.DrawText(m_Buffer,
			xText - 2,
			yText,
			rgbColor,
			sName,
			CG16bitFont::AlignRight);

	//	Paint enhancement

	CString sBonus = pDevice->GetEnhancedDesc(pShip);
	if (sBonus.IsBlank())
		DrawModifier(m_Buffer, xText - 2, yText + MediumFont.GetHeight(), sBonus, locAlignRight);

	//	Paint the ammo count

	if (iAmmoLeft != -1)
		LargeBoldFont.DrawText(m_Buffer,
				xText + 2,
				yText,
				m_rgbWeaponText,
				strFormatInteger(iAmmoLeft, -1, FORMAT_THOUSAND_SEPARATOR));
	}

void CWeaponHUDCircular::Realize (SHUDPaintCtx &Ctx)

//	Realize
//
//	Generate buffer

	{
	//	Skip if we don't have a ship

	CShip *pShip;
	if (Ctx.pSource == NULL
			|| (pShip = Ctx.pSource->AsShip()) == NULL)
		return;

	//	Get the current target

	CSpaceObject *pTarget = pShip->GetTarget(CItemCtx(), true);

	//	Set up some metrics

	const CVisualPalette &VI = g_pHI->GetVisuals();

	const CG16bitFont &SmallFont = VI.GetFont(fontSmall);
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);

	//	Create the buffer, if necessary

	if (m_Buffer.IsEmpty())
		{
		m_Buffer.Create(m_cxDisplay, m_cyDisplay, CG32bitImage::alpha8);

		//	Create the target mask

		m_TargetMask.Create(m_cxDisplay, m_cyDisplay, 0xff);
		CGDraw::Circle(m_TargetMask, m_xCenter, m_yCenter, m_iTargetRadius, 0);
		}

	//	Paint the target display background

	CGDraw::Circle(m_Buffer, m_xCenter, m_yCenter, m_iTargetRadius, m_rgbTargetBack);

	//	Paint an image of the target on top (make sure to mask it).

	if (pTarget)
		PaintTarget(Ctx, pShip, pTarget);

	//	Erase everything outside the mask

	m_Buffer.SetMask(0, 0, m_cxDisplay, m_cyDisplay, m_TargetMask, CG32bitPixel::Null(), 0, 0);

	//	Weapon status

	CInstalledDevice *pPrimary = pShip->GetNamedDevice(devPrimaryWeapon);
	CInstalledDevice *pLauncher = pShip->GetNamedDevice(devMissileWeapon);

	//	If we have a primary and launcher, put the launcher below the primary

	Metric rSpacingAngle = mathDegreesToRadians(9);
	Metric rTopAngle = mathDegreesToRadians(340);
	if (pPrimary && pLauncher)
		{
		PaintWeaponStatus(pShip, pPrimary, rSpacingAngle);
		PaintWeaponStatus(pShip, pLauncher, mathAngleMod(-rSpacingAngle));
		}

	//	Otherwise, we put weapon at center

	else if (pPrimary)
		PaintWeaponStatus(pShip, pPrimary, 0.0);

	else if (pLauncher)
		PaintWeaponStatus(pShip, pLauncher, 0.0);
	}
