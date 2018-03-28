//	CShipwreckDesc.cpp
//
//	CShipwreckDesc class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define WRECK_TAG								CONSTLIT("Wreck")

#define EXPLOSION_TYPE_ATTRIB					CONSTLIT("explosionType")
#define LEAVES_WRECK_ATTRIB						CONSTLIT("leavesWreck")
#define MAX_STRUCTURAL_HIT_POINTS_ATTRIB		CONSTLIT("maxStructuralHitPoints")
#define RADIOACTIVE_WRECK_ATTRIB				CONSTLIT("radioactiveWreck")
#define STRUCTURAL_HIT_POINTS_ATTRIB			CONSTLIT("structuralHitPoints")
#define WRECK_TYPE_ATTRIB						CONSTLIT("wreckType")

CG32bitImage *CShipwreckDesc::m_pDamageBitmap = NULL;
CStationType *CShipwreckDesc::m_pWreckDesc = NULL;

void CShipwreckDesc::AddItemsToWreck (CShip *pShip, CSpaceObject *pWreck) const

//	AddItemsToWreck
//
//	Adds items from the ship to the wreck, damaging or destroying as 
//	appropriate.

	{
	//	The chance that an installed item survives is related to
	//	the wreck chance.

	int iArmorCount = pShip->GetArmorSectionCount();
	int iDestroyArmorChance = 100 - (GetWreckChance() / 2);

	//	Decrease the chance of armor surviving if this ship class
	//	has lots of armor segments

	iDestroyArmorChance = Min(Max(iDestroyArmorChance, 100 - (100 / (1 + iArmorCount))), 95);

	CItemListManipulator Source(pShip->GetItemList());
	CItemListManipulator Dest(pWreck->GetItemList());

	while (Source.MoveCursorForward())
		{
		CItem WreckItem = Source.GetItemAtCursor();

		//	Installed items may or may not be damaged.

		if (WreckItem.IsInstalled())
			{
			//	Make sure that the armor item reflects the current
			//	state of the ship's armor.

			if (WreckItem.IsArmor())
				{
				//	Most armor is destroyed

				if (mathRandom(1, 100) <= iDestroyArmorChance)
					continue;

				//	Compute the % damage of the armor.

				CInstalledArmor *pArmor = pShip->GetArmorSection(WreckItem.GetInstalled());
				int iArmorHP = pArmor->GetHitPoints();
				int iArmorMaxHP = pArmor->GetMaxHP(pShip);
				int iArmorIntegrity = (iArmorMaxHP > 0 ? 100 * iArmorHP / iArmorMaxHP : 0);

				//	Add this item to the wreck

				Dest.AddDamagedComponents(WreckItem, 100 - iArmorIntegrity);
				}

			//	Other installed devices have a chance of being
			//	damaged or destroyed.

			else
				{
				//	Roll to see if device is damaged

				bool bDropDamaged = (mathRandom(1, 100) <= DEVICE_DAMAGED_CHANCE);

				//	Compute the installed device's fate

				ItemFates iFate = CalcDeviceFate(pShip, WreckItem, pWreck, bDropDamaged);

				//	Implement the appropriate fate

				switch (iFate)
					{
					case fateComponetized:
						Dest.AddItems(WreckItem.GetComponents(), CalcDeviceComponentChance(WreckItem, bDropDamaged));
						break;

					case fateDestroyed:
						break;

					case fateDamaged:
						{
						CItem ItemToDrop(WreckItem);
						ItemToDrop.SetInstalled(-1);

						if (WreckItem.GetMods().IsEnhancement())
							{
							CItemEnhancement Mods(WreckItem.GetMods());
							Mods.Combine(WreckItem, etLoseEnhancement);
							ItemToDrop.AddEnhancement(Mods);
							}
						else if (ItemToDrop.IsEnhanced())
							ItemToDrop.ClearEnhanced();
						else
							ItemToDrop.SetDamaged();

						Dest.AddItem(ItemToDrop);
						break;
						}

					case fateSurvives:
						{
						CItem ItemToDrop(WreckItem);
						ItemToDrop.SetInstalled(-1);
						Dest.AddItem(ItemToDrop);
						break;
						}
					}
				}
			}

		//	Non-installed virtual items are always lost

		else if (WreckItem.IsVirtual())
			continue;

		//	Otherwise, if this is just cargo, add to wreck

		else
			Dest.AddItem(WreckItem);
		}
	}

void CShipwreckDesc::AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) const

//	AddTypesUsed
//
//	Adds to the list of types used.

	{
	retTypesUsed->SetAt(m_pWreckType.GetUNID(), true);
	retTypesUsed->SetAt(m_WreckImage.GetBitmapUNID(), true);
	retTypesUsed->SetAt(m_pExplosionType.GetUNID(), true);
	}

ALERROR CShipwreckDesc::Bind (SDesignLoadCtx &Ctx)

//	Bind
//
//	Bind design elements.

	{
	ALERROR error;

	if (error = m_pExplosionType.Bind(Ctx))
		return error;

	if (error = m_pWreckType.Bind(Ctx))
		return error;

	return NOERROR;
	}

int CShipwreckDesc::CalcDeviceComponentChance (const CItem &Item, bool bDropDamaged) const

//	CalcDeviceComponentChance
//
//	Returns the chance that a given component of an installed device will drop.

	{
	int iChance = (bDropDamaged ? 50 : 80);
	if (Item.IsDamaged())
		iChance /= 2;

	return iChance;
	}

ItemFates CShipwreckDesc::CalcDeviceFate (CShip *pSource, const CItem &Item, CSpaceObject *pWreck, bool bDropDamaged) const

//	CalcDeviceFate
//
//	Calculates the fate of the given intalled device

	{
	CShipClass *pClass = pSource->GetClass();
	CInstalledDevice *pDevice = pSource->FindDevice(Item);

	//	Check to see if the device slot has a specified a fate

	ItemFates iFate;
	if (pDevice && (iFate = pDevice->GetFate()) != fateNone)
		return iFate;

	//	There's a chance that we'll be destroyed outright.

	else if (mathRandom(1, 100) <= CalcDeviceDestroyChance())
		return fateDestroyed;

	//	If this is a reactor, then we always destroy it (unless it is part of
	//	a capital ship).

	else if (Item.GetType()->GetCategory() == itemcatReactor
			&& !(pClass->GetCategoryFlags() & CShipClass::catCapitalShip))
		return fateDestroyed;

	//	If we have components, we usually drop the components.

	else if (Item.HasComponents() && (Item.IsVirtual() || mathRandom(1, 100) <= DROP_COMPONENTS_CHANCE))
		{
		//	If we're not virtual and not damaged, then we drop intact.

		if (!Item.IsVirtual() && !Item.IsDamaged() && !bDropDamaged)
			return fateSurvives;

		//	Otherwise, we drop components

		else
			return fateComponetized;
		}

	//	Virtual devices never drop

	else if (Item.IsVirtual())
		return fateDestroyed;

	//	If we're not damaged more, then we drop intact

	else if (!bDropDamaged)
		return fateSurvives;

	//	If we're already damaged, then we never drop

	else if (Item.IsDamaged())
		return fateDestroyed;

	//	We drop a damaged device

	else
		return fateDamaged;
	}

void CShipwreckDesc::CleanUp (void)

//	CleanUp
//
//	Clean up images to free up space.

	{
	m_WreckBitmap.CleanUp();
	m_WreckImage.CleanUp();
	}

void CShipwreckDesc::ClearMarks (void)

//	ClearMarks
//
//	Clear image marks

	{
	m_WreckImage.ClearMark();
	}

bool CShipwreckDesc::CreateEmptyWreck (CSystem *pSystem, CShipClass *pClass, CShip *pShip, const CVector &vPos, const CVector &vVel, CSovereign *pSovereign, CStation **retpWreck) const

//	CreateEmptyWreck
//
//	Create an empty wreck of the given ship class.
//
//	NOTE: pShip may be NULL.

	{
	DEBUG_TRY

	SSystemCreateCtx Ctx(pSystem);

	SObjCreateCtx CreateCtx(Ctx);
	CreateCtx.vPos = vPos;
	CreateCtx.vVel = vVel;

	//	Create the wreck

	CStation *pWreck;
	if (CStation::CreateFromType(pSystem,
			GetWreckType(),
			CreateCtx,
			&pWreck) != NOERROR)
		return false;

	//	Set properties of the wreck

	pWreck->SetSovereign(pSovereign);
	pWreck->SetWreckImage(pClass);
	pWreck->SetWreckParams(pClass, pShip);

	//	The wreck is radioactive if the ship is radioactive (or if this
	//	ship class always has radioactive wrecks)

	if ((pShip && pShip->IsRadioactive()) || IsRadioactive())
		pWreck->MakeRadioactive();

	//	Done

	if (retpWreck)
		*retpWreck = pWreck;

	return true;

	DEBUG_CATCH
	}

bool CShipwreckDesc::CreateWreck (CShip *pShip, CSpaceObject **retpWreck) const

//	CreateWreck
//
//	Creates a shipwreck

	{
	DEBUG_TRY

	//	Create the wreck

	CStation *pWreck;
	if (!CreateEmptyWreck(pShip->GetSystem(),
			pShip->GetClass(),
			pShip,
			pShip->GetPos(),
			pShip->GetVel(),
			pShip->GetSovereign(),
			&pWreck))
		return false;

	//	Add items to the wreck

	AddItemsToWreck(pShip, pWreck);

	//	Done

	if (retpWreck)
		*retpWreck = pWreck;

	return true;

	DEBUG_CATCH
	}

void CShipwreckDesc::CreateWreckImage (DWORD dwShipClass, const CObjectImageArray &ShipImage)

//	CreateWreckImage
//
//	Creates a wreck image, if we don't already have one.

	{
	DEBUG_TRY

	int i;

	//	If we've already got a wreck image, then we just mark it here.

	if (m_WreckImage.IsLoaded())
		{
		m_WreckImage.MarkImage();
		return;
		}

	//	If no ship image, then we can't do anything

	if (!ShipImage.IsLoaded())
		return;

	int cxWidth = RectWidth(ShipImage.GetImageRect());
	int cyHeight = RectHeight(ShipImage.GetImageRect());

	//	Get the image for damage

	if (m_pDamageBitmap == NULL)
		{
		CObjectImage *pDamageImage = g_pUniverse->FindLibraryImage(g_DamageImageUNID);
		if (pDamageImage == NULL)
			return;

		//	Lock the image because we keep it around in a global

		SDesignLoadCtx Ctx;
		if (pDamageImage->Lock(Ctx) != NOERROR)
			return;

		//	Get the image

		m_pDamageBitmap = pDamageImage->GetRawImage(strFromInt(ShipImage.GetBitmapUNID()));
		if (m_pDamageBitmap == NULL)
			return;
		}

	//	Create the bitmap

	CG32bitImage &SourceImage = ShipImage.GetImage(NULL_STR);
	m_WreckBitmap.Create(cxWidth, cyHeight * WRECK_IMAGE_VARIANTS, SourceImage.GetAlphaType());

	//	Initialize the random seed so that we can have a consistent pattern for
	//	each ship class.

	DWORD dwOldSeed = mathGetSeed();
	mathSetSeed(mathMakeSeed(dwShipClass));

	//	Blt the images

	TArray<int> Rotations;
	Rotations.InsertEmpty(WRECK_IMAGE_VARIANTS);

	for (i = 0; i < WRECK_IMAGE_VARIANTS; i++)
		{
		//	Pick a random rotation

		Rotations[i] = mathRandom(0, ShipImage.GetRotationCount() - 1);

		//	Copy the frame

		ShipImage.CopyImage(m_WreckBitmap,
				0,
				i * cyHeight,
				0,
				Rotations[i]);

		//	Add some destruction

		int iCount = cxWidth * 2;
		for (int j = 0; j < iCount; j++)
			{
			m_WreckBitmap.Blt(DAMAGE_IMAGE_WIDTH * mathRandom(0, DAMAGE_IMAGE_COUNT-1),
					0,
					DAMAGE_IMAGE_WIDTH,
					DAMAGE_IMAGE_COUNT,
					255,
					*m_pDamageBitmap,
					mathRandom(0, cxWidth-1) - (DAMAGE_IMAGE_WIDTH / 2),
					(i * cyHeight) + mathRandom(0, cyHeight-1) - (DAMAGE_IMAGE_HEIGHT / 2));
			}

		}

	mathSetSeed(dwOldSeed);

	//	Copy the mask back to the image because we blew it away painting
	//	the damage.

	for (i = 0; i < WRECK_IMAGE_VARIANTS; i++)
		{
		RECT rcSrc = ShipImage.GetImageRect(0, Rotations[i]);
		m_WreckBitmap.CopyChannel(channelAlpha, rcSrc.left, rcSrc.top, cxWidth, cyHeight, SourceImage, 0, i * cyHeight);
		}

	//	Initialize an image

	RECT rcRect;
	rcRect.left = 0;
	rcRect.top = 0;
	rcRect.right = cxWidth;
	rcRect.bottom = cyHeight;
	m_WreckImage.Init(&m_WreckBitmap, rcRect, 0, 0, false);

	//	Mark to indicate in use (we call this function from CShipClass::MarkImages)

	m_WreckImage.MarkImage();

	DEBUG_CATCH
	}

CStationType *CShipwreckDesc::GetWreckType (void) const

//	GetWreckType
//
//	Returns the wreck station type to use.

	{
	if (m_pWreckType)
		return m_pWreckType;
	else
		{
		if (m_pWreckDesc == NULL)
			m_pWreckDesc = g_pUniverse->FindStationType(g_ShipWreckUNID);

		return m_pWreckDesc;
		}
	}

ALERROR CShipwreckDesc::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, Metric rHullMass)

//	InitFromXML
//
//	Initialize from XML

	{
	ALERROR error;

	CXMLElement *pWreck = pDesc->GetContentElementByTag(WRECK_TAG);
	if (pWreck == NULL)
		pWreck = pDesc;

	//	Miscellaneous

	if (pDesc->FindAttributeInteger(LEAVES_WRECK_ATTRIB, &m_iLeavesWreck))
		m_iLeavesWreck = Max(0, m_iLeavesWreck);
	else
		{
		//	Chance of wreck is a function of mass:
		//
		//	prob = 5 * MASS^0.45

		m_iLeavesWreck = Max(0, Min((int)(5.0 * pow(rHullMass, 0.45)), 100));
		}

	if (error = m_pWreckType.LoadUNID(Ctx, pDesc->GetAttribute(WRECK_TYPE_ATTRIB)))
		return error;

	m_bRadioactiveWreck = pDesc->GetAttributeBool(RADIOACTIVE_WRECK_ATTRIB);
	m_iStructuralHP = pDesc->GetAttributeIntegerBounded(STRUCTURAL_HIT_POINTS_ATTRIB, 0, -1, -1);
	if (m_iStructuralHP == -1)
		m_iStructuralHP = pDesc->GetAttributeIntegerBounded(MAX_STRUCTURAL_HIT_POINTS_ATTRIB, 0, -1, 0);

	//	Explosion

	if (error = m_pExplosionType.LoadUNID(Ctx, pDesc->GetAttribute(EXPLOSION_TYPE_ATTRIB)))
		return error;

	//	Done

	return NOERROR;
	}

void CShipwreckDesc::MarkImages (void)

//	MarkImages
//
//	Mark images as in use

	{
	if (m_pExplosionType)
		m_pExplosionType->MarkImages();

	//	NOTE: Caller will call CreateWreckImage to mark it.
	}

void CShipwreckDesc::SweepImages (void)

//	SweepImages
//
//	Sweep unused images

	{
	if (!m_WreckImage.IsMarked())
		CleanUp();

	//	NOTE: No need to sweep m_pExplosionType because that is a normal design type.
	}

void CShipwreckDesc::UnbindGlobal (void)

//	UnbindGlobal
//
//	We're unbinding, which means we have to release any resources.

	{
	m_pDamageBitmap = NULL;
	m_pWreckDesc = NULL;
	}
