//	CWeaponHUDDefault.cpp
//
//	CWeaponHUDDefault class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define IMAGE_TAG							CONSTLIT("Image")

#define STR_UNKNOWN_HOSTILE					CONSTLIT("Unknown Hostile")
#define STR_UNKNOWN_FRIENDLY				CONSTLIT("Unknown Friendly")

#define ERR_WEAPON_DISPLAY_NEEDED			CONSTLIT("invalid <WeaponDisplay> element")

constexpr int DISPLAY_WIDTH =					420;
constexpr int DISPLAY_HEIGHT =					120;

constexpr int PRIMARY_WEAPON_X =				360;
constexpr int PRIMARY_WEAPON_Y =				80;
constexpr int MISSILE_WEAPON_X =				360;
constexpr int MISSILE_WEAPON_Y =				100;

constexpr int DEVICE_STATUS_HEIGHT =			20;

constexpr int TARGET_IMAGE_X =					60;
constexpr int TARGET_IMAGE_Y =					60;
constexpr int TARGET_NAME_X =					122;
constexpr int TARGET_NAME_Y =					27;
constexpr int TARGET_STAT_X =					130;

constexpr int TARGET_MASK_RADIUS =				56;
constexpr BYTE TARGET_MASK_TRANSPARENCY =		0x60;

constexpr int STAT_WIDTH =						52;

constexpr CG32bitPixel DISABLED_LABEL_COLOR =	CG32bitPixel(128, 0, 0);
constexpr CG32bitPixel TARGET_NAME_COLOR =		CG32bitPixel(80, 255, 80);
constexpr CG32bitPixel DAMAGED_COLOR =			CG32bitPixel(255, 80, 80);

CWeaponHUDDefault::CWeaponHUDDefault (void) :
		m_bInvalid(true),
		m_pDefaultBack(NULL)

//	CWeaponHUDDefault constructor

	{
	}

CWeaponHUDDefault::~CWeaponHUDDefault (void)

//	CWeaponHUDDefault destructor

	{
	}

ALERROR CWeaponHUDDefault::Bind (SDesignLoadCtx &Ctx)

//	Bind
//
//	Bind design

	{
	ALERROR error;

	if (error = m_BackImage.OnDesignLoadComplete(Ctx))
		return error;

	m_pDefaultBack = NULL;

	return NOERROR;
	}

void CWeaponHUDDefault::GetBounds (int *retWidth, int *retHeight) const

//	GetBounds
//
//	Returns the bounds of the HUD

	{
	*retWidth = DISPLAY_WIDTH;
	*retHeight = DISPLAY_HEIGHT;
	}

bool CWeaponHUDDefault::OnCreate (SHUDCreateCtx &CreateCtx, CString *retsError)

//	InitFromXML
//
//	Initializes from XML descriptor

	{
	ALERROR error;
	SDesignLoadCtx Ctx;

	//	Load the image

	if (CXMLElement *pImage = CreateCtx.Desc.GetContentElementByTag(IMAGE_TAG))
		{
		if (error = m_BackImage.InitFromXML(Ctx, *pImage))
			return ComposeLoadError(ERR_WEAPON_DISPLAY_NEEDED, retsError);
		}

	//	Bind

	if (Bind(Ctx) != NOERROR)
		return ComposeLoadError(Ctx.sError, retsError);

	//	Done

	return true;
	}

void CWeaponHUDDefault::OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx)

//	OnPaint
//
//	Paint

	{
	DEBUG_TRY

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

	DEBUG_CATCH
	}

void CWeaponHUDDefault::PaintDeviceStatus (CShip *pShip, DeviceNames iDev, int x, int y)

//	PaintDeviceStatus
//
//	Paints the status and ammo for a device

	{
	const CVisualPalette &VI = g_pHI->GetVisuals();
	const CG16bitFont &SmallFont = VI.GetFont(fontSmall);
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);
	const CG16bitFont &LargeBoldFont = VI.GetFont(fontLargeBold);

	CInstalledDevice *pDevice = pShip->GetNamedDevice(iDev);
	if (pDevice && pDevice->GetClass())
		{
		CItemCtx ItemCtx(pShip, pDevice);
		CDeviceClass *pClass = pDevice->GetClass();

		CString sVariant;
		int iAmmoLeft;
		pClass->GetSelectedVariantInfo(pShip, pDevice, &sVariant, &iAmmoLeft, NULL, true);
		int iSelectedFireAmmoLeft = pShip->GetAmmoForSelectedLinkedFireWeapons(pDevice);
		if (iSelectedFireAmmoLeft >= 0)
			iAmmoLeft = iSelectedFireAmmoLeft;
		CString sDevName = pDevice->GetItem()->GetNounPhrase(nounDuplicateModifier | nounNoModifiers | nounHUDName);

		//	Paint the bonus

		int cxBonus = 0;

		CString sBonus = pDevice->GetEnhancedDesc();
		if (!sBonus.IsBlank())
			{
			int cyHeight;
			cxBonus = SmallFont.MeasureText(sBonus, &cyHeight);

			//	Background

			RECT rcRect;
			rcRect.left = x - cxBonus - 8;
			rcRect.right = rcRect.left + cxBonus;
			rcRect.top = y + (DEVICE_STATUS_HEIGHT - cyHeight) / 2;
			rcRect.bottom = rcRect.top + cyHeight;

			bool bDisadvantage = (*(sBonus.GetASCIIZPointer()) == '-');
			m_Buffer.Fill(rcRect.left - 2, 
					rcRect.top + 1, 
					cxBonus + 4, 
					cyHeight - 2, 
					(bDisadvantage ? VI.GetColor(colorAreaDisadvantage) : VI.GetColor(colorAreaAdvantage)));

			//	Bonus text

			SmallFont.DrawText(m_Buffer,
					rcRect.left,
					rcRect.top,
					(bDisadvantage ? VI.GetColor(colorTextDisadvantage) : VI.GetColor(colorTextAdvantage)),
					sBonus);

			cxBonus += 4;
			}

		//	Figure out what color to use

		CG32bitPixel rgbColor;
		if (pDevice->IsWorking())
			rgbColor = VI.GetColor(colorTextHighlight);
		else
			rgbColor = DISABLED_LABEL_COLOR;

		//	Paint the name

		if (sVariant.IsBlank())
			sVariant = sDevName;

		int cyHeight;
		int cxWidth = MediumFont.MeasureText(sVariant, &cyHeight);

		MediumFont.DrawText(m_Buffer,
				x - cxWidth - 8 - cxBonus,
				y + (DEVICE_STATUS_HEIGHT - cyHeight) / 2,
				rgbColor,
				sVariant);

		//	Paint the ammo counter

		if (iAmmoLeft != -1)
			{
			CString sAmmo = strFromInt(iAmmoLeft);
			LargeBoldFont.DrawText(m_Buffer,
					x,
					y,
					VI.GetColor(colorTextHighlight),
					sAmmo);
			}
		}
	}

void CWeaponHUDDefault::PaintStat (int x, int y, const CString &sHeader, const CString &sStat, CG32bitPixel rgbColor)

//	PaintStat
//
//	Paints a stat

	{
	const CVisualPalette &VI = g_pHI->GetVisuals();
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);

	MediumFont.DrawText(m_Buffer,
			x,
			y,
			rgbColor,
			sHeader);
	y += MediumFont.GetHeight();

	MediumFont.DrawText(m_Buffer,
			x,
			y,
			rgbColor,
			sStat);
	}

void CWeaponHUDDefault::Realize (SHUDPaintCtx &Ctx)

//	Realize
//
//	Paint the m_Buffer

	{
	//	Skip if we don't have a ship

	CShip *pShip = Ctx.Source.AsShip();
	if (pShip == NULL)
		return;

	//	Set up some metrics

	const CVisualPalette &VI = g_pHI->GetVisuals();

	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);
	const CG16bitFont &MediumHeavyBoldFont = VI.GetFont(fontMediumHeavyBold);

	//	Create the buffer, if necessary

	if (m_Buffer.IsEmpty())
		m_Buffer.Create(DISPLAY_WIDTH, DISPLAY_HEIGHT, CG32bitImage::alpha8);

	m_Buffer.Set(CG32bitPixel::Null());

	//	Paint the background image

	if (!m_BackImage.IsEmpty())
		{
		const RECT &rcImage = m_BackImage.GetImageRect();

		m_Buffer.Blt(rcImage.left, 
				rcImage.top, 
				RectWidth(rcImage), 
				RectHeight(rcImage), 
				255,
				m_BackImage.GetImage(NULL_STR),
				0,
				0);
		}
	else
		{
		if (m_pDefaultBack == NULL)
			m_pDefaultBack = g_pUniverse->GetLibraryBitmap(UNID_TARGETING_HUD_BACKGROUND, CDesignCollection::FLAG_IMAGE_LOCK);

		if (m_pDefaultBack)
			m_Buffer.Blt(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, *m_pDefaultBack, 0, 0);
		}

	//	Prepare a buffer for the target

	if (m_Target.IsEmpty())
		m_Target.Create(DISPLAY_WIDTH, DISPLAY_HEIGHT, CG32bitImage::alpha8);

	if (m_TargetMask.IsEmpty())
		{
		m_TargetMask.Create(DISPLAY_WIDTH, DISPLAY_HEIGHT, TARGET_MASK_TRANSPARENCY);

		//	Erase the area outside the target pane

		if (!m_BackImage.IsEmpty())
			m_TargetMask.IntersectChannel(channelAlpha, m_BackImage.GetImage(NULL_STR));

		else if (m_pDefaultBack)
			m_TargetMask.IntersectChannel(channelAlpha, *m_pDefaultBack);

		//	Show the target

		CGDraw::Circle(m_TargetMask, TARGET_IMAGE_X, TARGET_IMAGE_Y, TARGET_MASK_RADIUS, 0xff);
		}

	//	Draw the primary weapon status

	PaintDeviceStatus(pShip, 
			devPrimaryWeapon,
			PRIMARY_WEAPON_X,
			PRIMARY_WEAPON_Y);

	//	Draw the missile weapon status

	PaintDeviceStatus(pShip,
			devMissileWeapon,
			MISSILE_WEAPON_X,
			MISSILE_WEAPON_Y);

	//	Paint the target

	CSpaceObject *pTarget = pShip->GetTarget(IShipController::FLAG_ACTUAL_TARGET);
	if (pTarget)
		{
		//	Paint image

		if (pTarget->IsIdentified())
			{
			//	Paint the target on the target bitmap

			m_Target.Set(CG32bitPixel::Null());
			CGDraw::Circle(m_Target, TARGET_IMAGE_X, TARGET_IMAGE_Y, TARGET_MASK_RADIUS, CG32bitPixel(0, 0, 0));

			SViewportPaintCtx Ctx;
			Ctx.pObj = pTarget;
			Ctx.XForm = ViewportTransform(pTarget->GetPos(), 
					g_KlicksPerPixel, 
					TARGET_IMAGE_X, 
					TARGET_IMAGE_Y);
			Ctx.XFormRel = Ctx.XForm;
			Ctx.fNoRecon = true;
			Ctx.fNoDockedShips = true;
			Ctx.fNoSelection = true;
            Ctx.fShowSatellites = true;

			int xCenter = TARGET_IMAGE_X;
			int yCenter = TARGET_IMAGE_Y;

			//	If we've docked with a satellite of a composite object, then we 
			//	figure out the parent so that we can paint the entire composite.

			CSpaceObject *pBase = pTarget->GetBase();
			if (pBase && pTarget->IsSatelliteSegmentOf(*pBase))
				{
				Ctx.XForm.Transform(pBase->GetPos(), &xCenter, &yCenter);
				Ctx.pObj = pBase;
				}

			//	Paint

			Ctx.pObj->Paint(m_Target, xCenter, yCenter, Ctx);

			//	Blt on buffer through the target mask

			CGDraw::BltMask(m_Buffer, 0, 0, m_Target, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, m_TargetMask);
			}

		//	Paint the name of the target

		int x = TARGET_NAME_X;
		int y = TARGET_NAME_Y;

		CString sName;
		if (pTarget->IsIdentified())
			sName = pTarget->GetNounPhrase(nounCapitalize);
		else if (pShip->IsEnemy(pTarget))
			sName = STR_UNKNOWN_HOSTILE;
		else
			sName = STR_UNKNOWN_FRIENDLY;

		MediumHeavyBoldFont.DrawText(m_Buffer,
				x,
				y,
				CG32bitPixel::Darken(TARGET_NAME_COLOR, 180),
				sName);
		y += MediumHeavyBoldFont.GetHeight();

		//	We paint various stats horizontally.

		int xStat = TARGET_STAT_X;
		int yStat = y;

		//	Paint the range

		CVector vDist = pTarget->GetPos() - pShip->GetPos();
		int iDist = mathRound(vDist.Length() / LIGHT_SECOND);

		PaintStat(xStat, yStat, CONSTLIT("Range"), strFormatInteger(iDist, -1, FORMAT_THOUSAND_SEPARATOR), TARGET_NAME_COLOR);
		xStat += STAT_WIDTH;

		//	Paint the damage

		if (pTarget->IsIdentified())
			{
			SVisibleDamage Damage;
			pTarget->GetVisibleDamageDesc(Damage);

			if (Damage.iShieldLevel != -1)
				{
				PaintStat(xStat, yStat, CONSTLIT("Shields"), strPatternSubst(CONSTLIT("%d%%"), Damage.iShieldLevel), (Damage.iShieldLevel < 20 ? DAMAGED_COLOR : TARGET_NAME_COLOR));
				xStat += STAT_WIDTH;
				}

			if (Damage.iArmorLevel != -1)
				{
				PaintStat(xStat, yStat, CONSTLIT("Armor"), strPatternSubst(CONSTLIT("%d%%"), Damage.iArmorLevel), (Damage.iArmorLevel < 20 ? DAMAGED_COLOR : TARGET_NAME_COLOR));
				xStat += STAT_WIDTH;
				}

			if (Damage.iHullLevel != -1)
				{
				PaintStat(xStat, yStat, CONSTLIT("Hull"), strPatternSubst(CONSTLIT("%d%%"), Damage.iHullLevel), (Damage.iHullLevel < 20 ? DAMAGED_COLOR : TARGET_NAME_COLOR));
				xStat += STAT_WIDTH;
				}
			}
		}
	}
