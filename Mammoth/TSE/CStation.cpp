//	CStation.cpp
//
//	CStation class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#ifdef DEBUG
//#define DEBUG_DOCKING
//#define DEBUG_ALERTS
#endif

#define ITEM_TAG								CONSTLIT("Item")
#define DEVICES_TAG								CONSTLIT("Devices")
#define ITEMS_TAG								CONSTLIT("Items")

#define COUNT_ATTRIB							CONSTLIT("count")
#define ITEM_ATTRIB								CONSTLIT("item")
#define DEVICE_ID_ATTRIB						CONSTLIT("deviceID")
#define X_ATTRIB								CONSTLIT("x")
#define Y_ATTRIB								CONSTLIT("y")
#define SHIPWRECK_UNID_ATTRIB					CONSTLIT("shipwreckID")
#define NAME_ATTRIB								CONSTLIT("name")

#define LANGID_ABANDONED_SCREEN_DATA			CONSTLIT("core.abandonedScreenData")
#define LANGID_DOCK_SCREEN_DATA					CONSTLIT("core.dockScreenData")
#define LANGID_DOCKING_REQUEST_DENIED			CONSTLIT("core.dockingRequestDenied")

#define PAINT_LAYER_OVERHANG					CONSTLIT("overhang")

#define PROPERTY_ABANDONED						CONSTLIT("abandoned")
#define PROPERTY_ACTIVE							CONSTLIT("active")
#define PROPERTY_ALLOW_ENEMY_DOCKING			CONSTLIT("allowEnemyDocking")
#define PROPERTY_ANGRY							CONSTLIT("angry")
#define PROPERTY_BARRIER						CONSTLIT("barrier")
#define PROPERTY_CAN_BE_MINED					CONSTLIT("canBeMined")
#define PROPERTY_DEST_NODE_ID					CONSTLIT("destNodeID")
#define PROPERTY_DEST_STARGATE_ID				CONSTLIT("destStargateID")
#define PROPERTY_DESTROY_WHEN_EMPTY				CONSTLIT("destroyWhenEmpty")
#define PROPERTY_DOCKING_PORT_COUNT				CONSTLIT("dockingPortCount")
#define PROPERTY_EXPLORED						CONSTLIT("explored")
#define PROPERTY_IGNORE_FRIENDLY_FIRE			CONSTLIT("ignoreFriendlyFire")
#define PROPERTY_IMAGE_SELECTOR					CONSTLIT("imageSelector")
#define PROPERTY_IMAGE_VARIANT					CONSTLIT("imageVariant")
#define PROPERTY_ITEM_TABLE						CONSTLIT("itemTable")
#define PROPERTY_MAX_HP							CONSTLIT("maxHP")
#define PROPERTY_MAX_STRUCTURAL_HP				CONSTLIT("maxStructuralHP")
#define PROPERTY_NO_FRIENDLY_FIRE				CONSTLIT("noFriendlyFire")
#define PROPERTY_OPEN_DOCKING_PORT_COUNT		CONSTLIT("openDockingPortCount")
#define PROPERTY_ORBIT							CONSTLIT("orbit")
#define PROPERTY_PAINT_LAYER					CONSTLIT("paintLayer")
#define PROPERTY_PARALLAX						CONSTLIT("parallax")
#define PROPERTY_PLAYER_BACKLISTED				CONSTLIT("playerBlacklisted")
#define PROPERTY_RADIOACTIVE					CONSTLIT("radioactive")
#define PROPERTY_ROTATION						CONSTLIT("rotation")
#define PROPERTY_ROTATION_SPEED					CONSTLIT("rotationSpeed")
#define PROPERTY_SHIP_CONSTRUCTION_ENABLED		CONSTLIT("shipConstructionEnabled")
#define PROPERTY_SHIP_REINFORCEMENT_ENABLED		CONSTLIT("shipReinforcementEnabled")
#define PROPERTY_SHIP_REINFORCEMENT_REQUESTED	CONSTLIT("shipReinforcementRequested")
#define PROPERTY_SHOW_MAP_LABEL					CONSTLIT("showMapLabel")
#define PROPERTY_SHOW_MAP_ORBIT					CONSTLIT("showMapOrbit")
#define PROPERTY_STARGATE_ID					CONSTLIT("stargateID")
#define PROPERTY_STRUCTURAL_HP					CONSTLIT("structuralHP")
#define PROPERTY_SUBORDINATE_ID					CONSTLIT("subordinateID")
#define PROPERTY_SUBORDINATES					CONSTLIT("subordinates")
#define PROPERTY_SUPERIOR						CONSTLIT("superior")
#define PROPERTY_WRECK_TYPE						CONSTLIT("wreckType")

#define STR_TRUE								CONSTLIT("true")

const int TRADE_UPDATE_FREQUENCY =		1801;			//	Interval for checking trade
const int STATION_SCAN_TARGET_FREQUENCY	= 29;
const int STATION_ATTACK_FREQUENCY =	67;
const int STATION_REINFORCEMENT_FREQUENCY =	607;
const int STATION_TARGET_FREQUENCY =	503;
const int DAYS_TO_REFRESH_INVENTORY =	5;
const int INVENTORY_REFRESHED_PER_UPDATE = 20;			//	% of inventory refreshed on each update frequency

const Metric MAX_ATTACK_DISTANCE =				LIGHT_SECOND * 25;
const Metric MAX_ATTACK_DISTANCE2 =				MAX_ATTACK_DISTANCE * MAX_ATTACK_DISTANCE;

const Metric BEACON_RANGE = 			(LIGHT_SECOND * 20);
const Metric BEACON_RANGE2 =            (BEACON_RANGE * BEACON_RANGE);

const int INITIAL_INVENTORY_REFRESH =	300;

#define MIN_ANGER						300
#define MAX_ANGER						1800
#define ANGER_INC						30

const CG32bitPixel RGB_SIGN_COLOR =		CG32bitPixel(196, 223, 155);
const CG32bitPixel RGB_ORBIT_LINE =		CG32bitPixel(115, 149, 229);
const CG32bitPixel RGB_MAP_LABEL =		CG32bitPixel(255, 217, 128);
const CG32bitPixel RGB_LRS_LABEL =		CG32bitPixel(165, 140, 83);

static char g_ImageTag[] = "Image";
static char g_ShipsTag[] = "Ships";
static char g_DockScreensTag[] = "DockScreens";
static char g_ShipTag[] = "Ship";

static char g_DockScreenAttrib[] = "dockScreen";
static char g_AbandonedScreenAttrib[] = "abandonedScreen";
static char g_HitPointsAttrib[] = "hitPoints";
static char g_ArmorIDAttrib[] = "armorID";
static char g_ProbabilityAttrib[] = "probability";
static char g_TableAttrib[] = "table";

#define MIN_DOCK_APPROACH_SPEED			(g_KlicksPerPixel * 25.0 / g_TimeScale);
#define MAX_DOCK_APPROACH_SPEED			(g_KlicksPerPixel * 50.0 / g_TimeScale);
#define MAX_DOCK_TANGENT_SPEED			(g_KlicksPerPixel / g_TimeScale);
const Metric g_DockBeamStrength =		1000.0;
const Metric g_DockBeamTangentStrength = 250.0;

const int g_iMapScale = 5;

const int DEFAULT_TIME_STOP_TIME =				150;

CStation::CStation (CUniverse &Universe) : TSpaceObjectImpl(Universe),
		m_Devices(CDeviceSystem::FLAG_NO_NAMED_DEVICES)

//	CStation constructor

	{
	}

CStation::~CStation (void)

//	CStation destructor

	{
	if (m_pRotation)
		delete m_pRotation;

	if (m_pMapOrbit)
		delete m_pMapOrbit;

	if (m_pMoney)
		delete m_pMoney;

	if (m_pTrade)
		delete m_pTrade;
	}

void CStation::Abandon (DestructionTypes iCause, const CDamageSource &Attacker, CWeaponFireDesc *pWeaponDesc)

//	Abandon
//
//	Abandon the station.

	{
	//	Only works for stations that can be abandoned.

	if (IsDestroyed() || IsImmutable() || IsAbandoned())
		return;

	m_Hull.SetHitPoints(0);

	SDestroyCtx DestroyCtx(*this);
	DestroyCtx.pDesc = pWeaponDesc;
	DestroyCtx.Attacker = Attacker;
	DestroyCtx.pWreck = this;
	DestroyCtx.iCause = iCause;

	//	Run OnDestroy script

	m_Overlays.FireOnObjDestroyed(this, DestroyCtx);
	FireItemOnObjDestroyed(DestroyCtx);
	FireOnDestroy(DestroyCtx);

	//	Station is destroyed. Take all the installed devices and turn
	//	them into normal damaged items

	CItemListManipulator Items(GetItemList());
	while (Items.MoveCursorForward())
		{
		CItem Item = Items.GetItemAtCursor();

		if (Item.IsInstalled())
			{
			//	Uninstall the device

			int iDevSlot = Item.GetInstalled();
			CInstalledDevice *pDevice = GetDevice(iDevSlot);
			pDevice->Uninstall(this, Items);

			//	Chance that the item is destroyed

			if (Item.GetType()->IsVirtual() || mathRandom(1, 100) <= 50)
				Items.DeleteAtCursor(1);
			else
				Items.SetDamagedAtCursor(true);

			//	Reset cursor because we may have changed position

			Items.ResetCursor();
			}
		}

	InvalidateItemListAddRemove();

	//	Alert others and retaliate, if necessary.
	//
	//	NOTE: We need to do this before <OnObjDestroyed> because we want 
	//	that event to be able to override this behavior.
	//	because we want those events to be able to override this behavior.

	CSpaceObject *pOrderGiver = Attacker.GetOrderGiver();
	if (pOrderGiver && pOrderGiver->CanAttack())
		{
		//	If we're a subordinate of some base, then let it handle the
		//	retaliation.

		if (m_pBase && !m_pBase->IsAbandoned() && !m_pBase->IsDestroyed())
			m_pBase->OnSubordinateDestroyed(DestroyCtx);

		//	We also retailiate

		if (!IsAngryAt(pOrderGiver))
			OnDestroyedByFriendlyFire(Attacker.GetObj(), pOrderGiver);
		else
			OnDestroyedByHostileFire(Attacker.GetObj(), pOrderGiver);
		}

	//	Tell all objects that we've been destroyed

	for (int i = 0; i < GetSystem()->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetSystem()->GetObject(i);

		if (pObj && pObj != this)
			pObj->OnStationDestroyed(DestroyCtx);
		}

	//	Tell the system to notify events that we've been destroyed

	GetSystem()->OnStationDestroyed(DestroyCtx);

	//	NOTE: We fire <OnObjDestroyed> AFTER OnStationDestroyed
	//	so that script inside <OnObjDestroyed> can add orders
	//	about the station (without getting clobbered in 
	//	OnStationDestroyed).

	NotifyOnObjDestroyed(DestroyCtx);

	//	Tell the system and universe that a station has been destroyed so
	//	that they handle timed events, etc.

	GetSystem()->FireOnSystemObjDestroyed(DestroyCtx);
	GetUniverse().NotifyOnObjDestroyed(DestroyCtx);

	//	Clear destination

	if (IsAutoClearDestinationOnDestroy())
		ClearPlayerDestination();

	//	Clear explored flag because we want the player to dock with the wreckage
	//	even if they docked before.

	m_fExplored = false;
	}

void CStation::AddOverlay (COverlayType *pType, int iPosAngle, int iPosRadius, int iRotation, int iPosZ, int iLifeLeft, DWORD *retdwID)

//	AddOverlay
//
//	Adds an overlay to the ship

	{
	ASSERT(pType);
	if (pType == NULL)
		return;

	m_Overlays.AddField(*this, *pType, iPosAngle, iPosRadius, iRotation, iPosZ, iLifeLeft, retdwID);

	//	Recalc bonuses, etc.

	CalcBounds();
	CalcDeviceBonus();
	}

void CStation::AddSubordinate (CSpaceObject &SubordinateObj, const CString &sSubordinateID)

//	AddSubordinate
//
//	Add this object to our list of subordinates

	{
	m_Subordinates.Add(&SubordinateObj);

	//	HACK: If we're adding a station, set it to point back to us

	CStation *pSatellite = SubordinateObj.AsStation();
	if (pSatellite)
		pSatellite->SetBase(*this, sSubordinateID);

	//	Recalc bounds, if necessary

	CPaintOrder::Types iPaintOrder;
	if (SubordinateObj.IsSatelliteSegmentOf(*this, &iPaintOrder)
			&& iPaintOrder != CPaintOrder::none)
		CalcBounds();
	}

CTradingDesc *CStation::AllocTradeDescOverride (void)

//	AllocTradeDescOverride
//
//	Makes sure that we have the m_pTrade structure allocated.
//	This is an override of the trade desc in the type

	{
	if (m_pTrade == NULL)
		{
		m_pTrade = new CTradingDesc;

		//	Set the same economy type

		CTradingDesc *pBaseTrade = m_pType->GetTradingDesc();
		if (pBaseTrade)
			{
			m_pTrade->Init(*pBaseTrade);
			}
		}

	return m_pTrade;
	}

bool CStation::Blacklist (CSpaceObject *pObj)

//	Blacklist
//
//	pObj is blacklisted (this only works for the player). Returns TRUE if the 
//	object is blacklisted.

	{
	int i;

	if (pObj == NULL || !pObj->IsPlayer())
		return false;

	//	No need if we don't support blacklist

	if (!CanBlacklist())
		return false;

	//	Remember if we need to send out an event

	bool bFireEvent = !m_Blacklist.IsBlacklisted();

	//	Remember that player is blacklisted
	//	(We do this early in case we recurse)

	m_Blacklist.Blacklist();

	//	Fire event

	if (bFireEvent)
		FireOnPlayerBlacklisted();

	//	If we're still blacklisted (if the event did not reverse this) then we
	//	deter the attacker.

	if (m_Blacklist.IsBlacklisted())
		{
		//	Tell our subordinates to blacklist the player also.

		for (i = 0; i < m_Subordinates.GetCount(); i++)
			{
			CStation *pSubordinate = m_Subordinates.GetObj(i)->AsStation();
			if (pSubordinate)
				pSubordinate->m_Blacklist.Blacklist();
			}

		return true;
		}
	else
		return false;
	}

int CStation::CalcAdjustedDamage (SDamageCtx &Ctx) const

//	CalcAdjustedDamage
//
//	Adjusts damage because some hulls require WMD.

	{
	EDamageHint iHint = EDamageHint::none;

	//	Short-circuit

	if (Ctx.iDamage == 0)
		return 0;

	//	Depending on hull type we need special damage to penetrate.

	int iSpecialDamage;
	switch (m_Hull.GetHullType())
		{
		//	Multi-hull stations require WMD.

		case CStationHullDesc::hullMultiple:
			iSpecialDamage = Ctx.Damage.GetMassDestructionDamage();
			iHint = EDamageHint::useWMD;
			break;

		//	Stations built on asteroids must be attacked with either WMD or
		//	mining damage.

		case CStationHullDesc::hullAsteroid:
			iSpecialDamage = Max(Ctx.Damage.GetMassDestructionDamage(), Ctx.Damage.GetMiningDamage());
			iHint = EDamageHint::useMiningOrWMD;
			break;

		//	Underground stations must be attacked with  mining damage.

		case CStationHullDesc::hullUnderground:
			iSpecialDamage = Ctx.Damage.GetMiningDamage();
			iHint = EDamageHint::useMining;
			break;

		//	For single-hull stations we don't need special damage.

		case CStationHullDesc::hullSingle:
		default:
			iSpecialDamage = -1;
			break;
		}

	//	If we don't need special damage, then we do full damage.

	if (iSpecialDamage == -1)
		return Ctx.iDamage;

	//	Otherwise, we adjust the damage.

	else
		{
		int iDamageAdj = DamageDesc::GetMassDestructionAdjFromValue(iSpecialDamage);
		int iDamage = mathAdjust(Ctx.iDamage, iDamageAdj);

		//	If we're not making progress, then return a hint about what to do.

		if (iHint != EDamageHint::none 
				&& iDamageAdj <= SDamageCtx::DAMAGE_ADJ_HINT_THRESHOLD
				&& Ctx.Attacker.IsPlayer()
				&& Ctx.Attacker.IsAngryAt(*this))
			{
			//	Figure out the average damage for this weapon.

			Metric rAveDamage = Ctx.Damage.GetDamageValue(DamageDesc::flagAverageDamage | DamageDesc::flagIncludeBonus);

			//	Adjust damage for difficulty level.

			rAveDamage = GetUniverse().AdjustDamage(Ctx, rAveDamage);

			//	Adjust for special damage resistance.

			int iAveDamage = mathAdjust(mathRound(rAveDamage), iDamageAdj);

			//	If we're not doing much harm, then warn the player.

			if (iAveDamage == 0 || (m_Hull.GetHitPoints() / iAveDamage) > SDamageCtx::WMD_HINT_THRESHOLD)
				Ctx.SetHint(iHint);
			}

		//	Return adjusted damage

		return iDamage;
		}
	}

int CStation::CalcAdjustedDamageAbandoned (SDamageCtx &Ctx) const

//	CalcAdjustedDamageAbandoned
//
//	Adjusts damage because some hulls require WMD.

	{
	EDamageHint iHint = EDamageHint::none;

	//	Short-circuit

	if (Ctx.iDamage == 0)
		return 0;

	//	Asteroid-class objects are affected either by WMD or by mining.

	int iSpecialDamage;
	if (CanBeMined() 
			|| m_Hull.GetHullType() == CStationHullDesc::hullAsteroid
			|| m_Hull.GetHullType() == CStationHullDesc::hullUnderground)

		iSpecialDamage = Max(Ctx.Damage.GetMassDestructionDamage(), Ctx.Damage.GetMiningDamage());

	//	Other stations require WMD.

	else
		iSpecialDamage = Ctx.Damage.GetMassDestructionDamage();

	//	No damage unless we have the required special damage.

	if (iSpecialDamage <= 0)
		return 0;

	//	Otherwise, we adjust the damage.

	else
		{
		int iDamageAdj = DamageDesc::GetMassDestructionAdjFromValue(iSpecialDamage);
		return mathAdjust(Ctx.iDamage, iDamageAdj);
		}
	}

void CStation::CalcBounds (void)

//	CalcBounds
//
//	Calculates and sets station bounds

	{
	//	Start with image bounds

	const CObjectImageArray &Image = GetImage(false, NULL, NULL);
	const RECT &rcImageRect = Image.GetImageRect();

	int cxWidth = RectWidth(rcImageRect);
	int cyHeight = RectHeight(rcImageRect);
	
	RECT rcBounds;
	rcBounds.left = -cxWidth / 2;
	rcBounds.right = rcBounds.left + cxWidth;
	rcBounds.top = -cyHeight / 2;
	rcBounds.bottom = rcBounds.top + cyHeight;

	//	Adjust if offset

	int xOffset;
	int yOffset;
	if (Image.GetImageOffset(0, 0, &xOffset, &yOffset))
		{
		rcBounds.right += 2 * Absolute(xOffset);
		rcBounds.bottom += 2 * Absolute(yOffset);
		}

	//	Add overlays

	m_Overlays.AccumulateBounds(this, Image.GetImageViewportSize(), GetRotation(), &rcBounds);

	//	Add subordinates

	for (int i = 0; i < m_Subordinates.GetCount(); i++)
		{
		const CSpaceObject *pSatellite = m_Subordinates.GetObj(i);
		CPaintOrder::Types iPaintOrder;
		if (pSatellite->IsSatelliteSegmentOf(*this, &iPaintOrder)
				&& iPaintOrder != CPaintOrder::none)
			{
			CVector vPos = pSatellite->GetPos() - GetPos();
			CVector vDiag = pSatellite->GetBoundsDiag();
			RECT rcRect;
			rcRect.left = mathRound((vPos.GetX() - vDiag.GetX()) / g_KlicksPerPixel);
			rcRect.top = mathRound((-vPos.GetY() - vDiag.GetY()) / g_KlicksPerPixel);
			rcRect.right = mathRound((vPos.GetX() + vDiag.GetX())  / g_KlicksPerPixel);
			rcRect.bottom = mathRound((-vPos.GetY() + vDiag.GetY()) / g_KlicksPerPixel);

			rcBounds.left = Min(rcBounds.left, rcRect.left);
			rcBounds.top = Min(rcBounds.top, rcRect.top);
			rcBounds.right = Max(rcBounds.right, rcRect.right);
			rcBounds.bottom = Max(rcBounds.bottom, rcRect.bottom);
			}
		}

	//	Set it

	SetBounds(rcBounds, GetParallaxDist());
	}

void CStation::CalcDeviceBonus (void)

//	CalcDeviceBonus
//
//	Computes device bonuses.

	{
	DEBUG_TRY

	//	Short-circuit if no devices

	if (m_Devices.IsEmpty())
		return;

	//	Enhancements from system

	const CEnhancementDesc *pSystemEnhancements = GetSystemEnhancements();

	//	Keep track of duplicate installed devices

	TSortMap<DWORD, int> DeviceTypes;

	//	Loop over all devices

	m_fArmed = false;
	m_fHasMissileDefense = false;

	for (CDeviceItem DeviceItem : GetDeviceSystem())
		{
		CInstalledDevice &Device = *DeviceItem.GetInstalledDevice();

		CItemCtx ItemCtx(this, &Device);

		//	Keep track of device types to see if we have duplicates

		bool bNewDevice;
		int *pCount = DeviceTypes.SetAt(Device.GetClass()->GetUNID(), &bNewDevice);
		if (bNewDevice)
			*pCount = 1;
		else
			*pCount += 1;

		//	Keep an enhancement stack for this device

		TSharedPtr<CItemEnhancementStack> pEnhancements(new CItemEnhancementStack);
		TArray<CString> EnhancementIDs;

		//	Add any enhancements on the item itself

		const CItemEnhancement &Mods = ItemCtx.GetMods();
		if (!Mods.IsEmpty())
			pEnhancements->Insert(Mods);

		//	Add enhancements from the slot

		Device.AccumulateSlotEnhancements(this, EnhancementIDs, pEnhancements);

		//	Add enhancements from other devices

		for (CDeviceItem OtherDevItem : GetDeviceSystem())
			{
			CInstalledDevice &OtherDev = *OtherDevItem.GetInstalledDevice();
			if (OtherDev.GetDeviceSlot() != Device.GetDeviceSlot())
				{
				OtherDev.AccumulateEnhancements(this, &Device, EnhancementIDs, pEnhancements);
				}
			}

		//	Add enhancements from system

		if (pSystemEnhancements)
			pSystemEnhancements->Accumulate(GetSystem()->GetLevel(), ItemCtx.GetItem(), *pEnhancements, &EnhancementIDs);

		//	Deal with class specific stuff

		m_Overlays.AccumulateEnhancements(*this, DeviceItem, EnhancementIDs, *pEnhancements);

		//	Cache some properties

		switch (Device.GetCategory())
			{
			case itemcatLauncher:
			case itemcatWeapon:
				m_fArmed = true;
				break;
			}

		//	Set the bonuses
		//	Note that these include any bonuses conferred by item enhancements

		Device.SetActivateDelay(pEnhancements->CalcActivateDelay(ItemCtx));

		//	Take ownership of the stack.

		Device.SetEnhancements(pEnhancements);

		//	If this is a missile defense weapon, cache that info. NOTE: We need
		//	to do this AFTER we set up the enhancements stack, since this 
		//	usually comes from the device slot.

		if (DeviceItem.GetTargetTypes() & CTargetList::typeMissile)
			m_fHasMissileDefense = true;
		}

	//	Mark devices as duplicate (or not)

	for (int i = 0; i < GetDeviceCount(); i++)
		{
		CInstalledDevice &Device = *GetDevice(i);
		if (!Device.IsEmpty())
			{
			int *pCount = DeviceTypes.GetAt(Device.GetClass()->GetUNID());
			Device.SetDuplicate(*pCount > 1);
			}
		}

	DEBUG_CATCH
	}

void CStation::CalcImageModifiers (CCompositeImageModifiers *retModifiers, int *retiTick) const

//	CalcImageModifier
//
//	Compute the modifiers for the station

	{
	constexpr BYTE FADE_OPACITY = 0x80;

	//	Modifiers (such as station damage)

	if (retModifiers)
		{
		//	Starlight rotation

		if (HasStarlightImage())
			{
			retModifiers->SetRotateImage(m_iStarlightImageRotation);
			}

		//	Out of plane worlds are faded

		if (m_fFadeImage)
			{
			CG32bitPixel rgbColor;
			if (CSystem *pSystem = GetSystem())
				rgbColor = pSystem->GetSpaceColor();
			else
				rgbColor = CSystemType::DEFAULT_SPACE_COLOR;

			retModifiers->SetFadeColor(rgbColor, FADE_OPACITY);
			}

		//	System filters

		retModifiers->SetFilters(GetSystemFilters());

		//	Damage

		if (ShowStationDamage())
			retModifiers->SetStationDamage(true);

		//	Rotation

		if (m_pRotation)
			retModifiers->SetRotation(m_pRotation->GetRotationAngle(m_pType->GetRotationDesc()));
		}

	//	Tick

	if (retiTick)
		{
		if (m_fActive && !IsTimeStopped())
			*retiTick = GetSystem()->GetTick() + GetDestiny();
		else
			*retiTick = 0;
		}
	}

int CStation::CalcNumberOfShips (void)

//	CalcNumberOfShips
//
//	Returns the number of ships associated with this station

	{
	DEBUG_TRY

	int i;
	int iCount = 0;

	for (i = 0; i < GetSystem()->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetSystem()->GetObject(i);

		if (pObj
				&& pObj->GetBase() == this
				&& pObj->GetCategory() == catShip
				&& pObj->CanAttack()	//	Exclude ship sections
				&& pObj != this)
			iCount++;
		}

	return iCount;

	DEBUG_CATCH
	}

bool CStation::CalcVolumetricShadowLine (SLightingCtx &Ctx, int *retxCenter, int *retyCenter, int *retiWidth, int *retiLength)

//	CalcVolumetricShadowLine
//
//	Computes the line shadow line for the object.

	{
	//	Get the image

	int iTick, iVariant;
	const CObjectImageArray &Image = GetImage(false, &iTick, &iVariant);

	//	Get the shadow line from the image

	return Image.CalcVolumetricShadowLine(Ctx, iTick, iVariant, retxCenter, retyCenter, retiWidth, retiLength);
	}

bool CStation::CanAttack (void) const

//	CanAttack
//
//	TRUE if the object can attack

	{
	return (!IsAbandoned() 
			&& !IsVirtual()
			&& (m_fArmed 
				|| (m_Subordinates.GetCount() > 0)
				|| m_pType->CanAttack()));
	}

bool CStation::CanBeDestroyedBy (CSpaceObject &Attacker) const

//	CanBeDestroyedBy
//
//	Returns TRUE if the given attacker has weapons that can destroy this 
//	station.

	{
	//	Loop over all attacker weapons.

	for (const CDeviceItem DeviceItem : Attacker.GetDeviceSystem())
		{
		if (!DeviceItem.IsWeapon())
			continue;

		//	See if any of the weapon variants can destroy us.

		for (int iVariant = 0; iVariant < DeviceItem.GetWeaponVariantCount(); iVariant++)
			{
			if (!DeviceItem.IsWeaponVariantValid(iVariant))
				continue;

			const CWeaponFireDesc &ShotDesc = DeviceItem.GetWeaponFireDescForVariant(iVariant);

			switch (m_Hull.GetHullType())
				{
				//	Multi-hull stations require WMD.

				case CStationHullDesc::hullMultiple:
					if (ShotDesc.GetDamage().GetMassDestructionDamage() > 0)
						return true;
					break;

				//	Stations built on asteroids must be attacked with either WMD or
				//	mining damage.

				case CStationHullDesc::hullAsteroid:
					if (ShotDesc.GetDamage().GetMassDestructionDamage() > 0
							|| ShotDesc.GetDamage().GetMiningDamage() > 0)
						return true;
					break;

				//	Underground stations must be attacked with  mining damage.

				case CStationHullDesc::hullUnderground:
					if (ShotDesc.GetDamage().GetMiningDamage() > 0)
						return true;
					break;

				//	For single-hull stations we don't need special damage.

				case CStationHullDesc::hullSingle:
				default:
					return true;
				}
			}
		}

	//	If we get this far, then none of the attacker's weapons can hurt us.

	return false;
	}

bool CStation::CanBlock (CSpaceObject *pObj)

//	CanBlock
//
//	Returns TRUE if this object can block the given object

	{
	return (m_fBlocksShips 
			|| (pObj->GetCategory() == catStation && !pObj->IsAnchored()));
	}

CSpaceObject::RequestDockResults CStation::CanObjRequestDock (CSpaceObject *pObj) const

//	CanObjRequestDock
//
//	Returns TRUE if pObj can request to dock with us. We return FALSE if we
//	don't want to give the player the flashing docking port UI. But we return
//	TRUE if we allow requests, but the request could be denied (e.g., because

	{
	//	There are various reasons why docking might be impossible with this 
	//	ship, including no docking ports.

	if (m_DockingPorts.GetPortCount() == 0
			|| IsDestroyed())
		return dockingNotSupported;

	//	If the object requesting docking services is an enemy,
	//	then deny docking services.

	if (pObj
			&& !IsAbandoned() 
			&& !m_fAllowEnemyDocking
			&& (IsEnemy(pObj) || IsBlacklisted(pObj)))
		return dockingDenied;

	//	If the player wants to dock with us and we don't have any docking 
	//	screens, then we do not support docking.
	//
	//	NOTE: We check this AFTER the dockingDenied check above because after
	//	a station is destroyed, the player will be able to dock (docking IS
	//	supported) but they're just not allowed to right now.

	if (pObj && pObj->IsPlayer() && !HasDockScreen())
		return dockingNotSupported;

	//	In some cases, the docking system is temporarily disabled. For example, 
	//	if we're docked with another object, no one can dock with us.

	if (IsTimeStopped())
		return dockingDisabled;

	//	Otherwise, docking is allowed

	return dockingOK;
	}

bool CStation::ClassCanAttack (void)

//	ClassCanAttack
//
//	Only returns FALSE if this object can never attack

	{
	return (m_pType->CanAttack());
	}

void CStation::ClearBlacklist (CSpaceObject *pObj)

//	ClearBlacklist
//
//	Removes blacklist

	{
	int i;

	if (pObj && !pObj->IsPlayer())
		return;

	//	No need if we don't support blacklist

	if (!m_pType->IsBlacklistEnabled())
		return;

	//	Remember that player is not blacklisted
	//	(We do this early in case we recurse)

	m_Blacklist.ClearBlacklist();

	//	Tell our subordinates to clear the blacklist also.

	for (i = 0; i < m_Subordinates.GetCount(); i++)
		{
		CStation *pSubordinate = m_Subordinates.GetObj(i)->AsStation();
		if (pSubordinate)
			pSubordinate->m_Blacklist.ClearBlacklist();
		}

	//	Cancel attack

	if (pObj)
		{
		//	Remove object from target

		m_Targets.Delete(pObj);

		//	Send all our subordinates to cancel attack

		for (int i = 0; i < m_Subordinates.GetCount(); i++)
			Communicate(m_Subordinates.GetObj(i), msgAbort, pObj);
		}
	}

void CStation::CreateDestructionEffect (void)

//	CreateDestructionEffect
//
//	Create the effect when the station is destroyed

	{
	if (GetSystem() == NULL)
		return;

	//	Start destruction animation

	m_iDestroyedAnimation = 60;

	//	Explosion effect and damage

	SExplosionType Explosion;
	FireGetExplosionType(&Explosion);
	if (Explosion.pDesc == NULL)
		Explosion.pDesc = m_pType->GetExplosionType();

	if (Explosion.pDesc)
		{
		SShotCreateCtx Ctx;
		Ctx.pDesc = Explosion.pDesc;
		if (Explosion.iBonus != 0)
			{
			Ctx.pEnhancements.TakeHandoff(new CItemEnhancementStack);
			Ctx.pEnhancements->InsertHPBonus(NULL, Explosion.iBonus);
			}

		Ctx.Source = CDamageSource(this, Explosion.iCause);
		Ctx.vPos = GetPos();
		Ctx.vVel = GetVel();
		Ctx.dwFlags = SShotCreateCtx::CWF_EXPLOSION;

		GetSystem()->CreateWeaponFire(Ctx);
		}

	//	Some air leaks

	CParticleEffect *pEffect;
	CParticleEffect::CreateEmpty(*GetSystem(),
			this,
			GetPos(),
			GetVel(),
			&pEffect);

	int iAirLeaks = mathRandom(0, 5);
	for (int j = 0; j < iAirLeaks; j++)
		{
		CParticleEffect::SParticleType *pType = new CParticleEffect::SParticleType;
		pType->m_fWake = true;

		pType->m_fLifespan = true;
		pType->iLifespan = 30;

		pType->m_fRegenerate = true;
		pType->iRegenerationTimer = 300 + mathRandom(0, 200);

		pType->iDirection = mathRandom(0, 359);
		pType->iDirRange = 3;
		pType->rAveSpeed = 0.1 * LIGHT_SPEED;

		pType->m_fMaxRadius = false;
		pType->rRadius = pType->iLifespan * pType->rAveSpeed;
		pType->rDampening = 0.75;

		pType->iPaintStyle = CParticleEffect::paintSmoke;

		pEffect->AddGroup(pType, mathRandom(50, 150));
		}

	//	Sound effects

	GetUniverse().PlaySound(this, GetUniverse().FindSound(g_StationExplosionSoundUNID));
	}

void CStation::CreateEjectaFromDamage (int iDamage, const CVector &vHitPos, int iDirection, const DamageDesc &Damage)

//	CreateEjectaFromDamage
//
//	Create ejecta when hit by damage

	{
	if (GetSystem() == NULL)
		return;

	int iEjectaAdj;
	if (iEjectaAdj = m_pType->GetEjectaAdj())
		{
		//	Ignore if damage came from ejecta (so that we avoid chain reactions)

		if (Damage.GetCause() == killedByEjecta && mathRandom(1, 100) <= 90)
			return;

		//	Adjust for the station propensity to create ejecta.

		iDamage = iEjectaAdj * iDamage / 100;
		if (iDamage == 0)
			return;

		//	Mining damage never causes ejecta (but we still get a geyser effect)

		if (Damage.GetMiningDamage() == 0 || !CanBeMined())
			{
			//	Compute the number of pieces of ejecta

			int iCount;
			if (iDamage <= 5)
				iCount = ((mathRandom(1, 100) <= (iDamage * 20)) ? 1 : 0);
			else if (iDamage <= 12)
				iCount = mathRandom(1, 3);
			else if (iDamage <= 24)
				iCount = mathRandom(2, 6);
			else
				iCount = mathRandom(4, 12);

			//	Generate ejecta

			CWeaponFireDesc *pEjectaType = m_pType->GetEjectaType();
			for (int i = 0; i < iCount; i++)
				{
				SShotCreateCtx Ctx;

				Ctx.pDesc = pEjectaType;
				Ctx.Source = CDamageSource(this, killedByEjecta);
				Ctx.iDirection = AngleMod(iDirection 
						+ (mathRandom(0, 12) + mathRandom(0, 12) + mathRandom(0, 12) + mathRandom(0, 12) + mathRandom(0, 12))
						+ (360 - 30));
				Ctx.vPos = vHitPos;
				Ctx.vVel = GetVel() + PolarToVector(Ctx.iDirection, pEjectaType->GetInitialSpeed());
				Ctx.dwFlags = SShotCreateCtx::CWF_EJECTA;

				GetSystem()->CreateWeaponFire(Ctx);
				}
			}

		//	Create geyser effect

		CParticleEffect::CreateGeyser(*GetSystem(),
				this,
				vHitPos,
				NullVector,
				mathRandom(5, 15),
				mathRandom(50, 150),
				CParticleEffect::paintFlame,
				iDamage + 2,
				0.15 * LIGHT_SPEED,
				iDirection,
				20,
				NULL);
		}
	}

ALERROR CStation::CreateFromType (CSystem &System,
								  CStationType *pType,
								  SObjCreateCtx &CreateCtx,
								  CStation **retpStation,
								  CString *retsError)

//	CreateFromType
//
//	Creates a new station based on the type

	{
	DEBUG_TRY

	ALERROR error;
	CStation *pStation;
	CXMLElement *pDesc = pType->GetDesc();

	if (!CreateCtx.bIgnoreLimits && !pType->CanBeEncountered(System, pType->GetEncounterDescConst()))
		{
		if (retsError)
			*retsError = CONSTLIT("Cannot be encountered");
		return ERR_FAIL;
		}

	//	Create the new station

	pStation = new CStation(System.GetUniverse());
	if (pStation == NULL)
		{
		if (retsError)
			*retsError = CONSTLIT("Out of memory");
		return ERR_MEMORY;
		}

	//	Initialize

	pStation->m_pType = pType;
	pStation->Place(CreateCtx.vPos, CreateCtx.vVel);
	pStation->m_pTrade = NULL;
	pStation->m_iDestroyedAnimation = 0;
	pStation->m_fKnown = false;
	pStation->m_fReconned = false;
	pStation->m_fExplored = false;
	pStation->m_fFireReconEvent = false;
	pStation->m_fActive = pType->IsActive();
	pStation->m_fNoReinforcements = false;
	pStation->m_fNoConstruction = false;
	pStation->m_fRadioactive = false;
	pStation->m_MapLabel.SetPos(CMapLabelPainter::posRight);
	pStation->m_rMass = pType->GetMass();
	pStation->m_dwWreckUNID = 0;
	pStation->m_fNoBlacklist = false;
	pStation->SetHasGravity(pType->HasGravity());
	pStation->m_iPaintOrder = pType->GetPaintOrder();
	pStation->m_fDestroyIfEmpty = false;
	pStation->m_fIsSegment = CreateCtx.bIsSegment;
	pStation->m_fAnonymous = pType->IsAnonymous();
	pStation->m_fAllowEnemyDocking = pType->IsEnemyDockingAllowed();
	pStation->Set3DExtra(CreateCtx.bIs3DExtra);

	//	3D Extra objects are always faded

	pStation->m_fFadeImage = CreateCtx.bIs3DExtra;

	//	Set up rotation, if necessary

	if (CreateCtx.iRotation != -1)
		{
		pStation->m_pRotation = new CIntegralRotation;
		pStation->m_pRotation->SetRotationAngle(pType->GetRotationDesc(), CreateCtx.iRotation);
		}
	else if (pType->GetImage().IsRotatable())
		{
		pStation->m_pRotation = new CIntegralRotation;
		pStation->m_pRotation->SetRotationAngle(pType->GetRotationDesc(), mathRandom(0, 359));
		}
	else
		pStation->m_pRotation = NULL;

	//	Handle moving stations

	if (pType->IsMobile())
		{
		//	Mobile objects cannot have a mass of 0 (otherwise the bouncing routines
		//	might fail).

		pStation->m_rMass = Max(1.0, pStation->m_rMass);
		pStation->SetCanBounce();
		}

	//	Background objects cannot be hit

	if (CreateCtx.rParallax != 1.0)
		pStation->m_rParallaxDist = CreateCtx.rParallax;
	else
		pStation->m_rParallaxDist = pType->GetParallaxDist();
	if (pStation->m_rParallaxDist != 1.0)
		pStation->SetOutOfPlaneObj(true);

	if (pType->IsVirtual())
		pStation->SetCannotBeHit();

	//	Friendly fire?

	if (!pType->CanHitFriends())
		pStation->SetNoFriendlyFire();

	if (!pType->CanBeHitByFriends())
		pStation->SetNoFriendlyTarget();

	//	Name

	const CNameDesc &Name = pType->GetNameDesc();
	if (!Name.IsConstant())
		{
		pStation->m_sName = Name.GenerateName(&CreateCtx.SystemCtx.NameParams, &pStation->m_dwNameFlags);

		//	NOTE: We leave anonymous alone because maybe we want asteroids to
		//	have a name for targeting purposes but still not show the name on 
		//	the map.
		}
	else
		pStation->m_dwNameFlags = 0;

	//	Stargates

	pStation->m_sStargateDestNode = pType->GetDestNodeID();
	pStation->m_sStargateDestEntryPoint = pType->GetDestEntryPoint();

	//	Get the scale

	pStation->m_Scale = pType->GetScale();

	//	Map

	if (CreateCtx.pOrbit)
		pStation->m_pMapOrbit = new COrbit(*CreateCtx.pOrbit);
	else
		pStation->m_pMapOrbit = NULL;

	pStation->m_fShowMapOrbit = false;
	pStation->m_fSuppressMapLabel = false;
	pStation->m_fForceMapLabel = false;

	//	We block others (CanBlock returns TRUE only for other stations)

	if (pStation->m_Scale != scaleStar && pStation->m_Scale != scaleWorld)
		pStation->SetIsBarrier();

	pStation->m_fBlocksShips = pType->IsWall();

	//	Load hit points and armor information

	pStation->m_Hull.Init(pType->GetHullDesc());
	if (!pStation->m_Hull.CanBeHit())
		pStation->SetCannotBeHit();

	//	Pick an appropriate image. This call will set the shipwreck image, if
	//	necessary or the variant (if appropriate).

	SSelectorInitCtx InitCtx;
	InitCtx.pSystem = &System;
	InitCtx.vObjPos = CreateCtx.vPos;
	InitCtx.sLocAttribs = (CreateCtx.pLoc ? CreateCtx.pLoc->GetAttributes() : NULL_STR);

	pType->SetImageSelector(InitCtx, &pStation->m_ImageSelector);

	//	If the image is a shipwreck class, then we take the parameters from the
	//	appropriate class.

	CShipClass *pWreckClass;
	if (pType->GetImage().HasShipwreckClass(pStation->m_ImageSelector, &pWreckClass))
		pStation->SetWreckParams(pWreckClass);

	//	Otherwise, if we've got a shipwreck class, take parameters from that.

	else if (CreateCtx.pWreckClass)
		{
		//	If necessary, set the image

		if (pType->GetImage().NeedsShipwreckClass())
			{
			pStation->m_ImageSelector.DeleteAll();
			pStation->m_ImageSelector.AddShipwreck(DEFAULT_SELECTOR_ID, CreateCtx.pWreckClass);
			}

		//	Set the parameters

		pStation->SetWreckParams(CreateCtx.pWreckClass, CreateCtx.pWreckShip);
		}

	//	Now that we have an image, set the bound

	pStation->CalcBounds();

	//	Create any items on the station

	if (error = pStation->CreateRandomItems(pType->GetRandomItemTable(), &System))
		{
		if (retsError)
			*retsError = CONSTLIT("Unable to create random items");
		delete pStation;
		return error;
		}

	//	Initialize devices

	CDeviceDescList Devices;
	pType->GenerateDevices(System.GetLevel(), Devices);

	//	Always set station weapons as secondaries so that they search for their 
	//	own targets.

	Devices.SetSecondary(true);

	//	Install devices

	pStation->m_Devices.Init(pStation, Devices);
	pStation->CalcDeviceBonus();

	//	Get notifications when other objects are destroyed

	pStation->SetObjectDestructionHook();

	//	Figure out the sovereign

	pStation->m_pSovereign = (CreateCtx.pSovereign ? CreateCtx.pSovereign : pType->GetSovereign());

	//	Initialize docking ports structure

	pStation->m_DockingPorts.InitPortsFromXML(pStation, pDesc);

	//	Make radioactive, if necessary

	if (pType->IsRadioactive())
		pStation->SetCondition(ECondition::radioactive);

	//	Add to system (note that we must add the station to the system
	//	before creating any ships).

	if (error = pStation->AddToSystem(System, true))
		{
		if (retsError)
			*retsError = CONSTLIT("Unable to add to system.");
		delete pStation;
		return error;
		}

	//	Initialize any CodeChain data

	pType->InitObjectData(*pStation, pStation->GetData());

	if (CreateCtx.pExtraData)
		pStation->SetDataFromXML(CreateCtx.pExtraData);

	//	Create any ships registered to this station

	IShipGenerator *pShipGenerator = pType->GetInitialShips();
	if (pShipGenerator)
		pStation->CreateRandomDockedShips(pShipGenerator, pType->GetDefenderCount());

	//	If we have a trade descriptor, create appropriate items

	CTradingDesc *pTrade = pType->GetTradingDesc();
	if (pTrade)
		pTrade->RefreshInventory(pStation, INITIAL_INVENTORY_REFRESH);

	//	This type has now been encountered

	pType->SetEncountered(System);

	//	Fire events on devices

	pStation->m_Devices.FinishInstall();

	//	Set override, just before creation.
	//	
	//	NOTE: We need to call SetOverride even if we have NULL for a handler 
	//	because it also sets event flags (SetEventFlags).

	pStation->SetOverride(CreateCtx.pEventHandler);

	//	If we're not in the middle of creating the system, call OnCreate
	//	(otherwise we will call OnCreate in OnSystemCreated)

	if (!System.IsCreationInProgress())
		{
		pStation->FinishCreation();
		pStation->CalcInsideBarrier();
		}
	else
		{
		//	Make sure we get an OnSystemCreated (and that we get it after
		//	our subordinates).

		System.RegisterForOnSystemCreated(pStation);
		}

	//	If this is a stargate, tell the system that we've got a stargate

	if (!pStation->m_sStargateDestNode.IsBlank())
		System.StargateCreated(pStation, NULL_STR, pStation->m_sStargateDestNode, pStation->m_sStargateDestEntryPoint);

	//	Return station

	if (retpStation)
		*retpStation = pStation;

	return NOERROR;

	DEBUG_CATCH
	}

ALERROR CStation::CreateMapImage (void) const

//	CreateMapImage
//
//	Creates a small version of the station image

	{
	//	Scale is 0.5 light-second per pixel (we compute the fraction of a full-sized image,
	//	while is at 12500 klicks per pixel.)

	Metric rScale = g_KlicksPerPixel / (0.3 * LIGHT_SECOND);

	//	Get it from the image

	int iTick, iVariant;
	const CObjectImageArray &Image = GetImage(false, &iTick, &iVariant);
	if (!Image.IsLoaded())
		return NOERROR;

	CG32bitImage *pBmpImage = &Image.GetImage(strFromInt(m_pType->GetUNID()));
	RECT rcBmpImage = Image.GetImageRect(iTick, iVariant);

	//	Create the image

	if (pBmpImage)
		{
		if (!m_MapImage.CreateFromImageTransformed(*pBmpImage,
				rcBmpImage.left,
				rcBmpImage.top,
				RectWidth(rcBmpImage),
				RectHeight(rcBmpImage),
				rScale,
				rScale,
				0.0))
			return ERR_FAIL;
		}

	return NOERROR;
	}

void CStation::CreateRandomDockedShips (IShipGenerator *pShipGenerator, const CShipChallengeDesc &Needed)

//	CreateRandomDockedShips
//
//	Creates all the ships that are registered at this station

	{
	SShipCreateCtx Ctx;

	Ctx.pSystem = GetSystem();
	Ctx.pBase = this;
	Ctx.vPos = GetPos();
	//	1d8+1 light-second spread
	Ctx.PosSpread = DiceRange(8, 1, 1);
	Ctx.dwFlags = SShipCreateCtx::SHIPS_FOR_STATION | SShipCreateCtx::RETURN_RESULT;

	//	Otherwise, we continue creating until we've got enough

	CShipChallengeCtx CreatedSoFar;

	int iMaxLoops = 20;
	while (iMaxLoops-- > 0 && Needed.NeedsMoreInitialShips(this, CreatedSoFar))
		{
		//	These accumulate, so we need to clear it each time.

		Ctx.Result.DeleteAll();

		//	Create the ships.

		pShipGenerator->CreateShips(Ctx);

		//	Keep track of the ships we created.

		CreatedSoFar.AddShips(Ctx.Result);
		}
	}

void CStation::CreateStructuralDestructionEffect (SDestroyCtx &Ctx)

//	CreateStructuralDestructionEffect
//
//	Create effect when station structure is destroyed

	{
	if (GetSystem() == NULL)
		return;

	//	Create fracture effect

	int iTick, iVariant;
	const CObjectImageArray &Image = GetImage(false, &iTick, &iVariant);

	if (Image.IsLoaded())
		{
		CFractureEffect::CreateExplosion(*GetSystem(),
				GetPos(),
				GetVel(),
				Image,
				iTick,
				iVariant,
				NULL);
		}

	//	Create explosion

	SExplosionType Explosion;
	FireGetExplosionType(&Explosion);
	if (Explosion.pDesc == NULL)
		Explosion.pDesc = m_pType->GetExplosionType();

	if (Explosion.pDesc)
		{
		SShotCreateCtx Ctx;
		Ctx.pDesc = Explosion.pDesc;
		if (Explosion.iBonus != 0)
			{
			Ctx.pEnhancements.TakeHandoff(new CItemEnhancementStack);
			Ctx.pEnhancements->InsertHPBonus(NULL, Explosion.iBonus);
			}

		Ctx.Source = CDamageSource(this, Explosion.iCause);
		Ctx.vPos = GetPos();
		Ctx.vVel = GetVel();
		Ctx.dwFlags = SShotCreateCtx::CWF_EXPLOSION;

		GetSystem()->CreateWeaponFire(Ctx);
		}
	else
		{
		//	Create Particles

		CObjectImageArray PartImage;
		RECT rcRect;
		rcRect.left = 0;
		rcRect.top = 0;
		rcRect.right = 4;
		rcRect.bottom = 4;
		PartImage.Init(GetUniverse(),
				g_ShipExplosionParticlesUNID,
				rcRect,
				8,
				3);

		CParticleEffect::CreateExplosion(*GetSystem(),
				NULL,
				GetPos(),
				GetVel(),
				mathRandom(1, 50),
				LIGHT_SPEED * 0.25,
				0,
				300,
				PartImage,
				NULL);

		CEffectCreator *pEffect = GetUniverse().FindEffectType(g_ExplosionUNID);
		if (pEffect)
			pEffect->CreateEffect(GetSystem(),
				NULL,
				GetPos(),
				GetVel(),
				0);
		}

	GetUniverse().PlaySound(this, GetUniverse().FindSound(g_ShipExplosionSoundUNID));
	}

CString CStation::DebugCrashInfo (void)

//	DebugCrashInfo
//
//	Dump crash information

	{
	int i;
	CString sResult;

	if (IsAbandoned())
		sResult.Append(CONSTLIT("abandoned\r\n"));

	sResult.Append(strPatternSubst(CONSTLIT("m_pBase: %s\r\n"), CSpaceObject::DebugDescribe(m_pBase)));

	for (i = 0; i < m_Targets.GetCount(); i++)
		sResult.Append(strPatternSubst(CONSTLIT("m_Targets[%d]: %s\r\n"), i, CSpaceObject::DebugDescribe(m_Targets.GetObj(i))));

	for (i = 0; i < m_Subordinates.GetCount(); i++)
		sResult.Append(strPatternSubst(CONSTLIT("m_Subordinates[%d]: %s\r\n"), i, CSpaceObject::DebugDescribe(m_Subordinates.GetObj(i))));

	for (i = 0; i < m_DockingPorts.GetPortCount(this); i++)
		{
		CSpaceObject *pDockedObj = m_DockingPorts.GetPortObj(this, i);
		if (pDockedObj)
			sResult.Append(strPatternSubst(CONSTLIT("m_DockingPorts[%d]: %s\r\n"), i, CSpaceObject::DebugDescribe(pDockedObj)));
		else if (!m_DockingPorts.IsPortEmpty(this, i))
			sResult.Append(strPatternSubst(CONSTLIT("m_DockingPorts[%d]: Not empty, but NULL object!\r\n"), i));
		}

	sResult.Append(m_Overlays.DebugCrashInfo());

	return sResult;
	}

void CStation::DeterAttack (CSpaceObject *pTarget)

//	DeterAttack
//
//	We send out patrols to attack the given target until it is out of range.

	{
	int i;

	//	Safety

	if (pTarget == NULL
			|| pTarget->IsDestroyed()
			|| !pTarget->CanAttack())
		return;

	//	Tell our guards that we were attacked

	for (i = 0; i < m_Subordinates.GetCount(); i++)
		Communicate(m_Subordinates.GetObj(i), msgAttackDeter, pTarget);

	//	Alert other stations

	if (m_pBase == NULL && m_pType->AlertWhenAttacked())
		RaiseAlert(pTarget);

	//	We get angry

	SetAngry();
	}

void CStation::FinishCreation (SSystemCreateCtx *pSysCreateCtx)

//	FinishCreation
//
//	If we're created with a system then this is called after the system has
//	completed creation.

	{
	//	Initialize trading

	CTradingDesc *pTrade = m_pType->GetTradingDesc();
	if (pTrade)
		pTrade->OnCreate(this);

	//	Fire OnCreate

	CSpaceObject::SOnCreate OnCreate;
	OnCreate.pCreateCtx = pSysCreateCtx;
	OnCreate.pOrbit = m_pMapOrbit;
	FireOnCreate(OnCreate);

	//	Add the object to the universe. We wait until the end in case
	//	OnCreate ends up setting the name (or something).

	GetUniverse().GetGlobalObjects().InsertIfTracked(this);
	}

Metric CStation::CalcMaxAttackDist (void) const

//	CalcMaxAttackDist
//
//	Returns the maximum attack distance based on weapons.

	{
	if (!m_fMaxAttackDistValid)
		{
		Metric rBestRange = 0.0;

		for (const CDeviceItem &DeviceItem : GetDeviceSystem())
			{
			const CInstalledDevice &Device = *DeviceItem.GetInstalledDevice();

			if (Device.GetCategory() == itemcatWeapon
					|| Device.GetCategory() == itemcatLauncher)
				{
				CItemCtx ItemCtx(this, &Device);
				Metric rRange = Device.GetMaxRange(ItemCtx);
				if (rRange > rBestRange)
					rBestRange = rRange;
				}
			}

		m_rMaxAttackDist = rBestRange;
		m_fMaxAttackDistValid = true;
		}

	return m_rMaxAttackDist;
	}

Metric CStation::GetAttackDistance (void) const

//	GetAttackDistance
//
//	Returns the distance at which we attack enemies.

	{
	if (m_iAngryCounter > 0)
		return Max(MAX_ATTACK_DISTANCE, CalcMaxAttackDist());
	else
		return MAX_ATTACK_DISTANCE;
	}

const CCurrencyBlock *CStation::GetCurrencyBlock (void) const

//	GetCurrencyBlock
//
//	Returns our currency store, creating one if required.

	{
	return m_pMoney;
	}

CCurrencyBlock *CStation::GetCurrencyBlock (bool bCreate)

//	GetCurrencyBlock
//
//	Returns our currency store, creating one if required.

	{
	if (m_pMoney == NULL && bCreate)
		m_pMoney = new CCurrencyBlock;

	return m_pMoney;
	}

int CStation::GetDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon)

//	GetDamageEffectiveness
//
//	Returns the effectiveness of the given weapon against this station.
//
//	< 0		The weapon is ineffective against us.
//	0-99	The weapon is less effective than average.
//	100		The weapon has average effectiveness
//	> 100	The weapon is more effective than average.

	{
	//	First ask the shields, if we have them, and if they are up.

	if (m_Devices.HasShieldsUp())
		{
		CInstalledDevice *pShields = GetNamedDevice(devShields);
		return pShields->GetDamageEffectiveness(pAttacker, pWeapon);
		}
	else
		return m_pType->GetHullDesc().CalcDamageEffectiveness(pAttacker, pWeapon);
	}

CDesignType *CStation::GetDefaultDockScreen (CString *retsName, ICCItemPtr *retpData) const

//	GetDockScreen
//
//	Returns the screen on dock (NULL_STR if none)

	{
	if (IsAbandoned() && m_pType->GetAbandonedScreen(retsName))
		{
		if (retpData)
			{
			if (!Translate(LANGID_ABANDONED_SCREEN_DATA, NULL, *retpData))
				*retpData = NULL;
			}

		return m_pType->GetAbandonedScreen(retsName);
		}
	else
		{
		if (retpData)
			{
			if (!Translate(LANGID_DOCK_SCREEN_DATA, NULL, *retpData))
				*retpData = NULL;
			}

		return m_pType->GetFirstDockScreen(retsName);
		}
	}

Metric CStation::GetGravity (Metric *retrRadius) const

//	GetGravity
//
//	Returns the force of gravity.

	{
	const Metric EARTH_RADIUS = 6371.0;	//	Kilometers

	//	Compute the acceleration due to gravity at 1 Earth radius
	//	(in kilometers per second-squared).
	//
	//	Accel = 0.0098 * 330,000 * StellarMass
	//
	//	1 StellarMass = 330,000 Earth masses
	//	Gravity at Earth's radius and Mass = 0.0098 Km/sec^2
	//
	//	Accel constant = 0.0098 * 330,000 = 3234

	Metric r1EAccel = 3234.0 * GetStellarMass();
	if (r1EAccel <= 0.0)
		return 0.0;

	//	Normally this is the radius at which the force of gravity is equal to
	//	1EAccel. Normally, this would always be 1 Earth radius, but that's way
	//	too small for the game scale, so we allow the object to set it.
	//
	//	10 light-seconds is usually a good value.

	if (retrRadius)
		*retrRadius = m_pType->GetGravityRadius();

	return r1EAccel;
	}

const CObjectImageArray &CStation::GetImage (bool bFade, int *retiTick, int *retiVariant) const

//	GetImage
//
//	Returns the image of this station

	{
	CCompositeImageModifiers Modifiers;
	CalcImageModifiers(&Modifiers, retiTick);

	//	Image

	return m_pType->GetImage(m_ImageSelector, Modifiers, retiVariant);
	}

Metric CStation::GetInvMass (void) const

//	GetInvMass
//
//	Return inverse mass.

	{
	if (IsAnchored() || m_rMass <= 0.0)
		return 0.0;

	return (1.0 / m_rMass);
	}

Metric CStation::GetMaxWeaponRange (void) const

//	GetMaxWeaponRange
//
//	Returns the maximum range of weapons.

	{
	return CalcMaxAttackDist();
	}

CString CStation::GetNamePattern (DWORD dwNounPhraseFlags, DWORD *retdwFlags) const

//	GetName
//
//	Returns the name of the station

	{
	if (m_sName.IsBlank() || (dwNounPhraseFlags & nounGeneric))
		return m_pType->GetNamePattern(dwNounPhraseFlags, retdwFlags);

	if (retdwFlags)
		*retdwFlags = m_dwNameFlags;

	return m_sName;
	}

CSystem::LayerEnum CStation::GetPaintLayer (void) const

//	GetPaintLayer
//
//	Returns the layer on which we should paint
	
	{
	//	Overrides

	if (m_iPaintOrder == CPaintOrder::paintOverhang)
		return CSystem::layerOverhang;

	switch (m_Scale)
		{
		case scaleStar:
		case scaleWorld:
			return CSystem::layerSpace;

		case scaleStructure:
		case scaleShip:
			return CSystem::layerStations;

		case scaleFlotsam:
			return CSystem::layerShips;

		default:
			return CSystem::layerStations;
		}
	}

ICCItem *CStation::GetPropertyCompatible (CCodeChainCtx &Ctx, const CString &sName) const

//	GetProperty
//
//	Returns a property

	{
	int i;
	CCodeChain &CC = GetUniverse().GetCC();
	ICCItem *pResult;

	if (strEquals(sName, PROPERTY_ABANDONED))
		return CC.CreateBool(IsAbandoned());

	else if (strEquals(sName, PROPERTY_ACTIVE))
		return CC.CreateBool(m_fActive);

	else if (strEquals(sName, PROPERTY_ALLOW_ENEMY_DOCKING))
		return CC.CreateBool(m_fAllowEnemyDocking);

	else if (strEquals(sName, PROPERTY_ANGRY))
		return (m_iAngryCounter > 0 ? CC.CreateInteger(m_iAngryCounter) : CC.CreateNil());

	else if (strEquals(sName, PROPERTY_BARRIER))
		return CC.CreateBool(m_fBlocksShips);

	else if (strEquals(sName, PROPERTY_CAN_BE_MINED))
		return CC.CreateBool(!IsOutOfPlaneObj() && m_pType->ShowsUnexploredAnnotation());

	else if (strEquals(sName, PROPERTY_DEST_NODE_ID))
		return (IsStargate() ? CC.CreateString(m_sStargateDestNode) : CC.CreateNil());

	else if (strEquals(sName, PROPERTY_DEST_STARGATE_ID))
		return (IsStargate() ? CC.CreateString(m_sStargateDestEntryPoint) : CC.CreateNil());

	else if (strEquals(sName, PROPERTY_DESTROY_WHEN_EMPTY))
		return CC.CreateBool(m_fDestroyIfEmpty);

	else if (strEquals(sName, PROPERTY_DOCKING_PORT_COUNT))
		return CC.CreateInteger(m_DockingPorts.GetPortCount(this));

	else if (strEquals(sName, PROPERTY_EXPLORED))
		return CC.CreateBool(m_fExplored);

	else if (strEquals(sName, PROPERTY_IGNORE_FRIENDLY_FIRE))
		return CC.CreateBool(!CanBlacklist());

	else if (strEquals(sName, PROPERTY_IMAGE_SELECTOR))
		return m_ImageSelector.WriteToItem()->Reference();

	else if (strEquals(sName, PROPERTY_IMAGE_VARIANT))
		{
		IImageEntry *pRoot = m_pType->GetImage().GetRoot();
		DWORD dwID = (pRoot ? pRoot->GetID() : DEFAULT_SELECTOR_ID);
		int iVariantID = m_ImageSelector.GetVariant(dwID);

		CString sVariantID;
		if (pRoot && !(sVariantID = pRoot->GetVariantID(iVariantID)).IsBlank())
			return CC.CreateString(sVariantID);
		else
			return CC.CreateInteger(iVariantID);
		}
	else if (strEquals(sName, PROPERTY_ITEM_TABLE))
		{
		IItemGenerator *pTable = m_pType->GetRandomItemTable();
		if (pTable == NULL)
			return CC.CreateNil();

		SItemAddCtx Ctx(GetUniverse());
		Ctx.pSystem = GetSystem();
		Ctx.vPos = GetPos();
		Ctx.iLevel = (Ctx.pSystem ? Ctx.pSystem->GetLevel() : 1);

		CItemTypeProbabilityTable Table = pTable->GetProbabilityTable(Ctx);
		ICCItemPtr pResult(ICCItem::List);
		for (int i = 0; i < Table.GetCount(); i++)
			{
			ICCItemPtr pEntry(ICCItem::SymbolTable);
			pEntry->SetIntegerAt(CONSTLIT("itemType"), Table.GetType(i)->GetUNID());
			pEntry->SetStringAt(CONSTLIT("itemName"), Table.GetType(i)->GetNounPhrase());
			pEntry->SetIntegerAt(CONSTLIT("chance"), mathRound(100.0 * Table.GetProbability(i)));

			pResult->Append(pEntry);
			}

		return pResult->Reference();
		}

	else if (strEquals(sName, PROPERTY_NO_FRIENDLY_FIRE))
		return CC.CreateBool(!CanHitFriends());

	else if (strEquals(sName, PROPERTY_OPEN_DOCKING_PORT_COUNT))
		return CC.CreateInteger(GetOpenDockingPortCount());

	else if (strEquals(sName, PROPERTY_ORBIT))
		return (m_pMapOrbit ? CreateListFromOrbit(CC, *m_pMapOrbit) : CC.CreateNil());

	else if (strEquals(sName, PROPERTY_PARALLAX))
		return (m_rParallaxDist != 1.0 ? CC.CreateInteger((int)(m_rParallaxDist * 100.0)) : CC.CreateNil());

	else if (strEquals(sName, PROPERTY_PAINT_LAYER))
		{
		if (m_iPaintOrder == CPaintOrder::none)
			return CC.CreateNil();
		else
			return CC.CreateString(CPaintOrder::GetID(m_iPaintOrder));
		}

	else if (strEquals(sName, PROPERTY_PLAYER_BACKLISTED))
		return CC.CreateBool(IsBlacklisted(NULL));

	else if (strEquals(sName, PROPERTY_ROTATION))
		return CC.CreateInteger(GetRotation());

	else if (strEquals(sName, PROPERTY_ROTATION_SPEED))
		return CC.CreateDouble(m_pRotation ? m_pRotation->GetRotationSpeedDegrees(m_pType->GetRotationDesc()) : 0.0);

	else if (strEquals(sName, PROPERTY_SHIP_CONSTRUCTION_ENABLED))
		return CC.CreateBool(!m_fNoConstruction);

	else if (strEquals(sName, PROPERTY_SHIP_REINFORCEMENT_ENABLED))
		return CC.CreateBool(!m_fNoReinforcements);

	else if (strEquals(sName, PROPERTY_SHIP_REINFORCEMENT_REQUESTED))
		return (m_iReinforceRequestCount ? CC.CreateInteger(m_iReinforceRequestCount) : CC.CreateNil());

	else if (strEquals(sName, PROPERTY_SHOW_MAP_LABEL))
		return CC.CreateBool(ShowMapLabel());

	else if (strEquals(sName, PROPERTY_SHOW_MAP_ORBIT))
		return CC.CreateBool(m_pMapOrbit && m_fShowMapOrbit);

	else if (strEquals(sName, PROPERTY_STARGATE_ID))
		{
		CSystem *pSystem;
		CString sGateID;
		if (!IsStargate() 
				|| (pSystem = GetSystem()) == NULL
				|| !pSystem->FindObjectName(this, &sGateID))
			return CC.CreateNil();

		return CC.CreateString(sGateID);
		}

	else if (strEquals(sName, PROPERTY_SUBORDINATE_ID))
		{
		if (!m_sSubordinateID.IsBlank())
			return CC.CreateString(m_sSubordinateID);
		else
			return CC.CreateNil();
		}

	else if (strEquals(sName, PROPERTY_SUBORDINATES))
		{
		if (GetSubordinateCount() == 0)
			return CC.CreateNil();

		ICCItem *pResult = CC.CreateLinkedList();
		for (i = 0; i < GetSubordinateCount(); i++)
			{
			ICCItem *pItem = CreateObjPointer(CC, GetSubordinate(i));
			pResult->Append(pItem);
			pItem->Discard();
			}

		return pResult;
		}

	else if (strEquals(sName, PROPERTY_SUPERIOR))
		return CreateObjPointer(CC, GetBase());

	else if (strEquals(sName, PROPERTY_WRECK_TYPE))
		{
		if (m_dwWreckUNID == 0)
			return CC.CreateNil();
		else
			return CC.CreateInteger(m_dwWreckUNID);
		}

	else if (pResult = m_Hull.FindProperty(sName))
		return pResult;

	else
		return CSpaceObject::GetPropertyCompatible(Ctx, sName);
	}

IShipGenerator *CStation::GetRandomEncounterTable (int *retiFrequency) const

//	GetRandomEncounterTable
//
//	Returns a random encounter table for this station
//
//	Note that we can return a NULL table, which means that we
//	we want to be called at <OnRandomEncounter>

	{
	if (retiFrequency)
		*retiFrequency = m_pType->GetEncounterFrequency();
	return m_pType->GetEncountersTable();
	}

int CStation::GetRotation (void) const

//	GetRotation
//
//	Returns the current rotation angle.

	{
	//	For rotated worlds, use the starlight rotation.

	if (HasStarlightImage())
		return m_iStarlightImageRotation;

	//	Otherwise, see if we have a rotation object

	else if (m_pRotation)
		return m_pRotation->GetRotationAngle(m_pType->GetRotationDesc());

	//	Otherwise, no rotation

	else
		return 0;
	}

CString CStation::GetStargateID (void) const

//	GetStargateID
//
//	Returns the stargate ID

	{
	if (m_sStargateDestNode.IsBlank())
		return NULL_STR;

	CSystem *pSystem = GetSystem();
	if (pSystem == NULL)
		return NULL_STR;

	CTopologyNode *pNode = pSystem->GetTopology();
	if (pNode == NULL)
		return NULL_STR;

	return pNode->FindStargateName(m_sStargateDestNode, m_sStargateDestEntryPoint);
	}

int CStation::GetStealth (void) const 

//	GetStealth
//
//	Returns current stealth

	{
	//	If we're "ghostly" then we always honor stealth.

	if (!m_Hull.CanBeHit())
		return m_pType->GetStealth();

	//	Otherwise, if we're known and do not move then we're always visible.

	else if (m_fKnown && IsAnchored())
		return stealthMin;

	//	Otherwise, honor stealth.

	else
		return m_pType->GetStealth();
	}

CSpaceObject *CStation::GetTarget (DWORD dwFlags) const

//	GetTarget
//
//	Returns the station's current target

	{
	//	If we're not armed, then we never have a target

	if (IsAbandoned() || !m_fArmed || m_pType->IsVirtual())
		return NULL;

	//	Otherwise, see if the player is in range, if so, then it is our target.

	CSpaceObject *pPlayer = GetUniverse().GetPlayerShip();
	if (pPlayer == NULL)
		return NULL;

	Metric rAttackDist = GetAttackDistance();
	Metric rAttackDist2 = rAttackDist * rAttackDist;
	if (GetDistance2(pPlayer) < rAttackDist2)
		return pPlayer;

	return NULL;
	}

CDesignType *CStation::GetWreckType (void) const

//	GetWreckType
//
//	Returns the type of the ship class that this is a wreck of (or NULL)

	{
	if (m_dwWreckUNID == 0)
		return NULL;

	return GetUniverse().FindDesignType(m_dwWreckUNID);
	}

bool CStation::HasAttribute (const CString &sAttribute) const

//	HasAttribute
//
//	Returns TRUE if it has the given attribute

	{
	return m_pType->HasLiteralAttribute(sAttribute);
	}

bool CStation::HasVolumetricShadow (int *retiStarAngle, Metric *retrStarDist) const

//	HasVolumetricShadow
//
//	Returns TRUE if we should create a volumetric shadow for this object.

	{
	constexpr int DEFAULT_LIGHTING_ANGLE = 315;

	if (m_rStarlightDist > 0.0 && !IsOutOfPlaneObj())
		{
		if (retiStarAngle) *retiStarAngle = m_iStarlightImageRotation + DEFAULT_LIGHTING_ANGLE;
		if (retrStarDist) *retrStarDist = m_rStarlightDist;
		return true;
		}
	else
		return false;
	}

bool CStation::ImageInObject (const CVector &vObjPos, const CObjectImageArray &Image, int iTick, int iRotation, const CVector &vImagePos)

//	ImageInObject
//
//	Returns TRUE if the given image at the given position intersects this
//	station

	{
	int iDestTick, iDestVariant;
	const CObjectImageArray &DestImage = GetImage(false, &iDestTick, &iDestVariant);

	return ImagesIntersect(Image, iTick, iRotation, vImagePos,
			DestImage, iDestTick, iDestVariant, vObjPos);
	}

bool CStation::IsBlacklisted (const CSpaceObject *pObj) const

//	IsBlacklisted
//
//	Returns TRUE if we are blacklisted
	
	{
	if (pObj)
		return (m_Blacklist.IsBlacklisted() && pObj->GetSovereign() && pObj->GetSovereign()->IsPlayer());
	else
		return m_Blacklist.IsBlacklisted();
	}

bool CStation::IsNameSet (void) const

//	IsNameSet
//
//	Returns TRUE if the station has a valid name already.
	
	{
	//	If we have a unique name, then we're done.

	if (!m_sName.IsBlank())
		return true;

	//	If the type has a valid name, then we're done.

	CString sName = m_pType->GetNamePattern();
	if (!sName.IsBlank() && *sName.GetASCIIZPointer() != '(')
		return true;

	//	Otherwise, we don't have a valid name

	return false;
	}

bool CStation::IsShownInGalacticMap (void) const

//  IsShownInGalacticMap
//
//  Returns TRUE if this object should be shown on the details pane of the 
//  galactic map.

	{
	//  Skip if we purposefully disable this

	if (!m_pType->ShowsMapDetails())
		return false;

	//  If we're virtual and we've got a trading descriptor, then we should
	//  be included. This handles the case of stations like New Victoria
	//  Arcology, which are composed of multiple parts with a virtual center.

	if (IsVirtual() && m_pType->GetTradingDesc())
		return true;

	//  We only show stations/wrecks (not worlds)

	if (GetScale() != scaleStructure && GetScale() != scaleShip)
		return false;

	//  Only if we would show it on the system map
	//
	//	NOTE: We only care about what the type specifies, not what the object
	//	overrides, because sometimes the object just hides the map label to keep
	//	the map clean (not because it is not an interesting station).

	if (!m_pType->ShowsMapIcon() || m_pType->SuppressMapLabel())
		return false;

	//  Show it

	return true;
	}

void CStation::OnClearCondition (ECondition iCondition, DWORD dwFlags)

//	OnClearCondition
//
//	Clears a condition

	{
	switch (iCondition)
		{
		case ECondition::radioactive:
			m_fRadioactive = false;
			break;
		}
	}

EDamageResults CStation::OnDamage (SDamageCtx &Ctx)

//	Damage
//
//	Station takes damage

	{
	DEBUG_TRY

	GetUniverse().AdjustDamage(Ctx);

	//	Stations don't have armor segments

	Ctx.iSectHit = -1;

	//	Short-circuit

	if (Ctx.iDamage == 0)
		{
		if (IsImmutable())
			return damageNoDamageNoPassthrough;
		else
			return damageNoDamage;
		}

	//	OnAttacked event

	if (HasOnAttackedEvent())
		FireOnAttacked(Ctx);

	GetSystem()->FireOnSystemObjAttacked(Ctx);

	//	See if the damage is blocked by some external defense

	Ctx.iOverlayHitDamage = Ctx.iDamage;
	if (!Ctx.bIgnoreOverlays 
			&& !Ctx.bIgnoreShields 
			&& m_Overlays.AbsorbDamage(this, Ctx))
		{
		if (IsDestroyed())
			return damageDestroyed;
		else if (Ctx.iDamage == 0)
			{
			if (IsImmutable())
				return damageNoDamageNoPassthrough;
			else
				return damageNoDamage;
			}
		}

	//	If this is a momentum attack then we are pushed

	Metric rImpulse;
	if (!IsAnchored() && Ctx.Damage.HasImpulseDamage(&rImpulse))
		{
		CVector vAccel = PolarToVector(Ctx.iDirection, -0.5 * rImpulse);
		AddForce(vAccel);
		}

	//	Let our shield generators take a crack at it

	Ctx.iShieldHitDamage = Ctx.iDamage;
	if (!Ctx.bIgnoreShields)
		{
		for (CDeviceItem DeviceItem : GetDeviceSystem())
			{
			CInstalledDevice &Device = *DeviceItem.GetInstalledDevice();
			bool bAbsorbed = Device.AbsorbDamage(this, Ctx);
			if (IsDestroyed())
				return damageDestroyed;
			else if (bAbsorbed)
				return damageAbsorbedByShields;
			}
		}

	//	If we're immutable, then nothing else happens.

	Ctx.iArmorHitDamage = Ctx.iDamage;
	if (IsImmutable())
		return OnDamageImmutable(Ctx);

	//	We go through a different path if we're already abandoned
	//	(i.e., there is no life on the station)

	else if (IsAbandoned())
		return OnDamageAbandoned(Ctx);

	//	If we're not abandoned, we go through a completely different code-path

	else
		return OnDamageNormal(Ctx);

	DEBUG_CATCH_OBJ(this)
	}

EDamageResults CStation::OnDamageAbandoned (SDamageCtx &Ctx)

//	OnDamageAbandoned
//
//	An abandoned station is damaged

	{
	EDamageResults iResult = damageNoDamageNoPassthrough;

	//	Let custom weapons get a chance

	Ctx.GetDesc().FireOnDamageAbandoned(Ctx);
	if (IsDestroyed())
		return damageDestroyed;

	//	If this is a paralysis attack then no damage

	int iEMP = Ctx.Damage.GetEMPDamage();
	if (iEMP)
		Ctx.iDamage = 0;

	//	If this is blinding attack then no damage

	int iBlinding = Ctx.Damage.GetBlindingDamage();
	if (iBlinding)
		Ctx.iDamage = 0;

	//	If this is a radioactive attack then there is a chance that we will
	//	be contaminated. (Note: We can only be contaminated by an attack if
	//	we're abandoned)

	int iRadioactive = Ctx.Damage.GetRadiationDamage();
	if (iRadioactive 
			&& GetScale() != scaleStar
			&& GetScale() != scaleWorld)
		{
		int iChance = 4 * iRadioactive * iRadioactive;
		if (mathRandom(1, 100) <= iChance)
			SetCondition(ECondition::radioactive);
		}

	//	If we have mining damage then call OnMining

	if (Ctx.Damage.HasMiningDamage())
		{
		if (!OnMiningDamage(Ctx))
			return damageDestroyed;
		}

	//	Adjust the damage based on WMD requirements. Most hull types require
	//	WMD damage to be hurt.

	Ctx.iDamage = CalcAdjustedDamageAbandoned(Ctx);

	//	Give events a chance to change the damage

	if (HasOnDamageEvent())
		Ctx.iDamage = FireOnDamage(Ctx, Ctx.iDamage);

	//	Hit effect

	if (!Ctx.bNoHitEffect)
		Ctx.GetDesc().CreateHitEffect(GetSystem(), Ctx);

	//	Take damage

	if (Ctx.iDamage > 0)
		{
		//	See if this hit destroyed us

		if (m_Hull.GetStructuralHP() > 0 && m_Hull.GetStructuralHP() <= Ctx.iDamage)
			{
			//	Destroy

			Destroy(Ctx);
			iResult = damageDestroyed;
			}

		//	Otherwise, take damage

		else
			{
			//	See if we should generate ejecta

			CreateEjectaFromDamage(Ctx.iDamage, Ctx.vHitPos, Ctx.iDirection, Ctx.Damage);

			//	If we can be destroyed, subtract from hp

			if (m_Hull.GetStructuralHP() > 0)
				{
				m_Hull.IncStructuralHP(-Ctx.iDamage);
				iResult = damageStructuralHit;
				}
			}
		}

	//	Done

	return iResult;
	}

EDamageResults CStation::OnDamageImmutable (SDamageCtx &Ctx)

//	OnDamageImmutable
//
//	An immutable station is damaged

	{
	//	If we don't have ejecta, then decrease damage to 0.
	//
	//  NOTE: We check MassDestructionLevel (instead of MassDestructionAdj) 
	//  because even level 0 has some WMD. But for this case we only case
	//  about "real" WMD.

	if (m_pType->GetEjectaAdj() == 0
			|| Ctx.Damage.GetMassDestructionLevel() == 0)
		Ctx.iDamage = 0;

	//	Otherwise, adjust for WMD

	else
		Ctx.iDamage = mathAdjust(Ctx.iDamage, Ctx.Damage.GetMassDestructionAdj());

	//	Hit effect

	if (!Ctx.bNoHitEffect)
		Ctx.GetDesc().CreateHitEffect(GetSystem(), Ctx);

	//	Ejecta

	if (Ctx.iDamage > 0)
		CreateEjectaFromDamage(Ctx.iDamage, Ctx.vHitPos, Ctx.iDirection, Ctx.Damage);

	return damageNoDamageNoPassthrough;
	}

EDamageResults CStation::OnDamageNormal (SDamageCtx &Ctx)

//	OnDamageNormal
//
//	A normal station, neither abandoned nor immutable, is damaged.

	{
	CSpaceObject *pOrderGiver = Ctx.GetOrderGiver();

	if (pOrderGiver 
			&& pOrderGiver->CanAttack()
			&& !Ctx.Attacker.IsAutomatedWeapon())
		{
		//	Tell our base that we were attacked.

		if (m_pBase && !m_pBase->IsAbandoned() && !m_pBase->IsDestroyed())
			m_pBase->OnSubordinateHit(Ctx);

		//	If the attacker is a friend then we should keep track of
		//	friendly fire hits

		if (!IsAngryAt(pOrderGiver))
			OnHitByFriendlyFire(Ctx.Attacker.GetObj(), pOrderGiver);

		//	Otherwise, consider this a hostile act.

		else
			OnHitByHostileFire(Ctx.Attacker.GetObj(), pOrderGiver);
		}

	//	Armor effects

	bool bCustomDamage = false;
	CArmorClass *pArmorClass = m_pType->GetHullDesc().GetArmorClass();
	if (pArmorClass)
		{
		//	Create an item context

		CItem Item = m_pType->GetHullDesc().GetArmorItem();
		CArmorItem ArmorItem = Item.AsArmorItem();
		CItemCtx ItemCtx(&Item, this);

		//	Compute the effects based on damage and our armor

		pArmorClass->CalcDamageEffects(ItemCtx, Ctx);

		//	Give custom weapons a chance

		bCustomDamage = Ctx.GetDesc().FireOnDamageArmor(Ctx);
		if (IsDestroyed())
			return damageDestroyed;

		//	Compute the damage for the armor

		pArmorClass->CalcAdjustedDamage(ItemCtx, Ctx);

		//	If this armor section reflects this kind of damage then
		//	send the damage on

		if (Ctx.IsShotReflected() && Ctx.pCause)
			{
			Ctx.pCause->CreateReflection(Ctx.vHitPos, (Ctx.iDirection + 120 + mathRandom(0, 120)) % 360);
			return damageNoDamage;
			}

		//	For stations, armor damage is passed through.

		Ctx.iDamage = Ctx.iArmorDamage;
		}

	//	Adjust the damage based on WMD requirements. Most hull types require
	//	WMD damage to be hurt.

	Ctx.iDamage = CalcAdjustedDamage(Ctx);

	//	Give events a chance to change the damage

	if (HasOnDamageEvent() && (Ctx.iDamage > 0 || bCustomDamage))
		Ctx.iDamage = FireOnDamage(Ctx, Ctx.iDamage);

	//	Hit effect

	if (!Ctx.bNoHitEffect)
		Ctx.GetDesc().CreateHitEffect(GetSystem(), Ctx);

	//	Tell our attacker that we got hit

	if (pOrderGiver && pOrderGiver->CanAttack())
		pOrderGiver->OnObjHit(Ctx);

	//	If no damage, we're done

	if (Ctx.iDamage == 0 && !bCustomDamage)
		return damageNoDamage;

	//	Handle special attacks

	if (Ctx.IsTimeStopped() 
			&& !IsImmuneTo(specialTimeStop)
			&& !IsTimeStopped())
		{
		AddOverlay(UNID_TIME_STOP_OVERLAY, 0, 0, 0, 0, DEFAULT_TIME_STOP_TIME + mathRandom(0, 29));

		//	No damage

		Ctx.iDamage = 0;
		}

	//	If we've still got armor left, then we take damage but otherwise
	//	we're OK.

	if (Ctx.iDamage < m_Hull.GetHitPoints())
		{
		m_Hull.IncHitPoints(-Ctx.iDamage);
		return damageArmorHit;
		}

	//	Some stations are destroyed when abandoned.

	else if (m_pType->IsDestroyWhenAbandoned())
		{
		Destroy(Ctx);
		return damageDestroyed;
		}

	//	Otherwise we're abandoned

	else
		{
		Abandon(Ctx.Damage.GetCause(), Ctx.Attacker, &Ctx.GetDesc());
		CreateDestructionEffect();
		return damageDestroyedAbandoned;
		}
	}

void CStation::OnDestroyed (SDestroyCtx &Ctx)

//	OnDestroyed
//
//	Station has been destroyed

	{
	DEBUG_TRY

	//	Run OnDestroy script

	m_Overlays.FireOnObjDestroyed(this, Ctx);
	FireItemOnObjDestroyed(Ctx);
	FireOnDestroy(Ctx);

	//	Release docking port objects

	m_DockingPorts.OnDestroyed();

	//	Create effect

	switch (Ctx.iCause)
		{
		case removedFromSystem:
		case enteredStargate:
		case ascended:
			//	No effect
			break;

		case killedByPowerFailure:
		case killedByRadiationPoisoning:
		case killedByRunningOutOfFuel:
		case killedByOther:
			//	No effect
			break;

		default:
			CreateStructuralDestructionEffect(Ctx);
		}

	DEBUG_CATCH
	}

void CStation::OnDestroyedByFriendlyFire (CSpaceObject *pAttacker, CSpaceObject *pOrderGiver)

//	OnDestroyedByFriendlyFire
//
//	Station destroyed by friendly fire

	{
	if (!pOrderGiver || !pOrderGiver->CanAttack())
		throw CException(ERR_FAIL);

	//	If the player attacked us, we need to blacklist her

	CSpaceObject *pTarget;
	if (pOrderGiver->IsPlayer()
			&& CanBlacklist()
			&& (pTarget = CalcTargetToAttack(pAttacker, pOrderGiver)))
		{
		if (Blacklist(pOrderGiver))
			AvengeAttack(pTarget);
		}
	}

void CStation::OnDestroyedByHostileFire (CSpaceObject *pAttacker, CSpaceObject *pOrderGiver)

//	OnDestroyedByHostileFire
//
//	Station destroyed by hostile fire

	{
	CSpaceObject *pTarget;

	ASSERT(pOrderGiver && pOrderGiver->CanAttack());

	//	Figure out which target we should attack (based on visibility,
	//	proximity, etc.).

	if ((pTarget = CalcTargetToAttack(pAttacker, pOrderGiver)))
		{
		AvengeAttack(pTarget);
		}
	}

bool CStation::OnGetCondition (ECondition iCondition) const

//	OnGetCondition
//
//	Returns station condition.

	{
	switch (iCondition)
		{
		case ECondition::radioactive:
			return (m_fRadioactive ? true : false);

		default:
			return false;
		}
	}

bool CStation::OnIsImmuneTo (SpecialDamageTypes iSpecialDamage) const

//	OnIsImmuneTo
//
//	Returns TRUE if we are immune to the given condition.

	{
	switch (iSpecialDamage)
		{
		case specialTimeStop:
			return m_pType->IsTimeStopImmune();

		default:
			return false;
		}
	}

bool CStation::OnMiningDamage (SDamageCtx &Ctx)

//	OnMiningDamage
//
//	We've been hit by mining damage. Returns TRUE if we should continue; FALSE 
//	if the station was destroyed.

	{
	//	See if any overlays will handle mining. E.g., if there is an underground 
	//	vault, then mining is handled by them.

	if (!Ctx.bIgnoreOverlays 
			&& m_Overlays.OnMiningDamage(*this, m_pType->GetAsteroidDesc().GetType(), Ctx))
		{ }

	//	Otherwise, fire <OnMining>

	else
		FireOnMining(Ctx, m_pType->GetAsteroidDesc().GetType());

	return !IsDestroyed();
	}

void CStation::OnMove (const CVector &vOldPos, Metric rSeconds)

//	OnMove
//
//	Do stuff when station moves

	{
	//	If the station is moving then make sure all docked ships
	//	move along with it.

	m_DockingPorts.MoveAll(this);
	}

void CStation::OnSetCondition (ECondition iCondition, int iTimer)

//	OnSetCondition
//
//	Sets a condition

	{
	switch (iCondition)
		{
		case ECondition::radioactive:
			m_fRadioactive = true;
			break;
		}
	}

void CStation::AvengeAttack (CSpaceObject *pTarget)

//	AvengeAttack
//
//	We've been killed, so we need to avenge the attack.

	{
	int i;

	//	Safety checks

	if (pTarget == NULL || pTarget->IsDestroyed() || !pTarget->CanAttack())
		return;

	//	Tell our subordinates to attack to kill

	for (i = 0; i < m_Subordinates.GetCount(); i++)
		Communicate(m_Subordinates.GetObj(i), msgBaseDestroyedByTarget, pTarget);

	//	Alert

	if (m_pBase == NULL && m_pType->AlertWhenDestroyed())
		RaiseAlert(pTarget);

	SetAngry();
	}

void CStation::ObjectDestroyedHook (const SDestroyCtx &Ctx)

//	ObjectDestroyedHook
//
//	Notification of another object being destroyed

	{
	//	If this object is docked with us, remove it from the
	//	docking table.

	m_DockingPorts.OnObjDestroyed(this, &Ctx.Obj);

	//	Remove the object from any lists that it may be on

	m_Targets.Delete(&Ctx.Obj);
	m_WeaponTargets.Delete(Ctx.Obj);

	//	If this was our base, remove it.

	if (Ctx.Obj == m_pBase)
		m_pBase = NULL;

	//	Remove from the subordinate list. No need to take action because the 
	//	ship/turret will communicate if we need to avenge.

	if (m_Subordinates.Delete(&Ctx.Obj))
		{
		//	See if we have any weapons associated with this subordinate

		const CString &sSubordinateID = Ctx.Obj.GetSubordinateID();
		if (!sSubordinateID.IsBlank())
			m_Devices.OnSubordinateDestroyed(Ctx.Obj, sSubordinateID);
		}
	}

bool CStation::ObjectInObject (const CVector &vObj1Pos, CSpaceObject *pObj2, const CVector &vObj2Pos)

//	ObjectInObject
//
//	Returns TRUE if the given object intersects this object

	{
	DEBUG_TRY

	int iTick, iVariant;
	const CObjectImageArray &Image = GetImage(false, &iTick, &iVariant);

	return pObj2->ImageInObject(vObj2Pos, Image, iTick, iVariant, vObj1Pos);

	DEBUG_CATCH
	}

DWORD CStation::OnCommunicate (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2, ICCItem *pData)

//	OnCommunicate
//
//	Handle communications

	{
	switch (iMessage)
		{
		case msgBaseDestroyedByTarget:
			if (!IsAbandoned())
				AvengeAttack(pParam1);
			return resAck;

		case msgAttackDeter:
			if (!IsAbandoned())
				DeterAttack(pParam1);
			return resAck;

		case msgDestroyBroadcast:
			{
			CSpaceObject *pTarget = pParam1;

			if (!IsEnemy(pTarget) && !IsBlacklisted(pTarget))
				{
				if (Blacklist(pTarget))
					DeterAttack(pTarget);
				}

			//	Attack the target, unless we've already got it on our
			//	target list.

			else if (!m_Targets.FindObj(pTarget))
				{
				m_Targets.Add(pTarget);

#ifdef DEBUG_ALERTS
				GetUniverse().DebugOutput("%d: Received msgDestroyBroadcast", this);
#endif

				//	Order out some number of subordinates to attack
				//	the target.

				int iLeft = mathRandom(3, 5);
				int iCount = m_Subordinates.GetCount();
				for (int i = 0; i < iCount && iLeft > 0; i++)
					{
					if (Communicate(m_Subordinates.GetObj(i), 
							msgDestroyBroadcast, 
							pTarget) == resAck)
						{
						iLeft--;
#ifdef DEBUG_ALERTS
						GetUniverse().DebugOutput("   %d acknowledges attack order", m_Subordinates.GetObj(i));
#endif
						}
					}
				}

			return resAck;
			}

		//	NOTE: We no longer use these messages. They are here only for backwards 
		//	compatibility.

		case msgHitByHostileFire:
			if (!IsAbandoned())
				DeterAttack(pParam1);
			return resAck;

		case msgDestroyedByFriendlyFire:
			{
			//	If we're abandoned, then we don't care.

			if (IsAbandoned())
				return resNoAnswer;

			//	The sender has already figured out which target to retaliate against.
			//	But we need to recompute the order giver.

			CSpaceObject *pTarget = pParam1;
			CSpaceObject *pOrderGiver = pTarget->GetOrderGiver();

			//	If this is not the player, then we don't care.
			//	If we don't blacklist, then we don't care.

			if (!pOrderGiver->IsPlayer()
					|| !CanBlacklist())
				return resAck;

			//	If our sovereign considers the player to be a major threat, then
			//	we immediately blacklist.

			if (GetSovereign()
					&& GetSovereign()->GetPlayerThreatLevel() >= CSovereign::threatMajor)
				{
				//	Fall through and blacklist.
				}

			//	If one of our subordinates was destroyed, then we take it seriously.

			else
				{
				//	If the player had a good reason for using deadly force, then
				//	we treat this as an accident.
				//
				//	Also, neutral sovereigns don't give us the benefit of the doubt.

				if (IsFriend(pOrderGiver) && IsPlayerAttackJustified())
					return resAck;

				//	Fall through and blaclist.
				}

			//	Otherwise, blacklist the player (but go after the target, which 
			//	might be an auton because the player is hidden).

			if (Blacklist(pOrderGiver))
				DeterAttack(pTarget);

			return resAck;
			}

		case msgDestroyedByHostileFire:
			if (!IsAbandoned())
				DeterAttack(pParam1);
			return resAck;

		case msgHitByFriendlyFire:
			{
			//	If we're abandoned, then we don't care.

			if (IsAbandoned())
				return resNoAnswer;

			//	The sender has already figured out which target to retaliate against.
			//	But we need to recompute the order giver.

			CSpaceObject *pTarget = pParam1;
			CSpaceObject *pOrderGiver = pTarget->GetOrderGiver();

			//	If this is not the player, then we don't care.
			//	If we don't blacklist, then we don't care.

			if (!pOrderGiver->IsPlayer()
					|| !CanBlacklist())
				return resAck;

			//	If our sovereign considers the player to be a major threat, then
			//	we immediately blacklist.

			if (GetSovereign()
					&& GetSovereign()->GetPlayerThreatLevel() >= CSovereign::threatMajor)
				{
				//	Fall through and blacklist.
				}

			//	Otherwise, we treat it like a hit on us.

			else
				{
				//	If we think this might have been accidental, then ignore it.

				if (!m_Blacklist.Hit(GetSystem()->GetTick()))
					return resAck;
				}

			//	Otherwise, blacklist the player (but go after the target, which 
			//	might be an auton because the player is hidden).

			if (Blacklist(pOrderGiver))
				DeterAttack(pTarget);

			return resAck;
			}

		default:
			return resNoAnswer;
		}
	}

void CStation::OnComponentChanged (ObjectComponentTypes iComponent)

//	OnComponentChanged
//
//	Some part of the object has changed

	{
	switch (iComponent)
		{
		case comCargo:
			{
			for (CDeviceItem DeviceItem : GetDeviceSystem())
				{
				CInstalledDevice &Device = *DeviceItem.GetInstalledDevice();
				if (!Device.IsVariantSelected(this))
					Device.SelectFirstVariant(this);
				}

			break;
			}
		}
	}

void CStation::OnHitByFriendlyFire (CSpaceObject *pAttacker, CSpaceObject *pOrderGiver)

//	OnHitByFriendlyFire
//
//	Station is hit by friendly fire. See if we need to blacklist
//	the attacker.

	{
	if (!pOrderGiver || !pOrderGiver->CanAttack())
		throw CException(ERR_FAIL);

	//	Warn the attacker

	Communicate(pOrderGiver, msgWatchTargets);

	//	See if we should blacklist and retaliate. We only blacklist the player
	//	and only if we can tell who attacked us.

	CSpaceObject *pTarget;
	if (pOrderGiver->IsPlayer()
			&& CanBlacklist()
			&& (pTarget = CalcTargetToAttack(pAttacker, pOrderGiver)))
		{
		if (
			//	If our sovereign considers the player to be a major threat, then
			//	we immediately blacklist.

				(GetSovereign()
					&& GetSovereign()->GetPlayerThreatLevel() >= CSovereign::threatMajor)

			//	If this doesn't seem like an accident, then blacklist

				|| m_Blacklist.Hit(GetSystem()->GetTick()))
			{
			if (Blacklist(pOrderGiver))
				DeterAttack(pTarget);
			}
		}
	}

void CStation::OnHitByHostileFire (CSpaceObject *pAttacker, CSpaceObject *pOrderGiver)

//	OnHitByHostileFire
//
//	Station is hit by enemy fire.

	{
	ASSERT(pOrderGiver && pOrderGiver->CanAttack());

	//	Figure out which target we should attack (based on visibility,
	//	proximity, etc.).

	CSpaceObject *pTarget;
	if ((pTarget = CalcTargetToAttack(pAttacker, pOrderGiver)))
		{
		DeterAttack(pTarget);
		}
	}

void CStation::OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	OnPaint
//
//	Paint the station

	{
	int i;

	//	Known

	SetKnown();
	if (!m_fReconned && !Ctx.fNoRecon)
		{
		if (m_fFireReconEvent)
			{
			Reconned();
			m_fFireReconEvent = false;
			}

		m_fReconned = true;
		}

	//	Get the image

	int iTick, iVariant;
	const CObjectImageArray &Image = GetImage(true, &iTick, &iVariant);

	//	Set some context

	CViewportPaintCtxSmartSave Save(Ctx);
	Ctx.iTick = iTick;
	Ctx.iVariant = 0;
	Ctx.iRotation = GetRotation();
	Ctx.iDestiny = GetDestiny();

	//	Calculate visibility

	DWORD byShimmer = CalcSRSVisibility(Ctx);

	//	Known, immobile objects always have a minimum visibility in SRS.

	if (byShimmer && m_fKnown && IsAnchored())
		byShimmer = Max(byShimmer, (DWORD)60);

	//	Paints overlay background

	m_Overlays.PaintBackground(Dest, x, y, Ctx);

	//	First paint any object that are docked behind us

	if (!Ctx.fNoDockedShips)
		{
		//	Paint docked ships

		for (i = 0; i < m_DockingPorts.GetPortCount(this); i++)
			{
			CSpaceObject *pObj = m_DockingPorts.GetPortObj(this, i);
			if (pObj 
					&& pObj->IsPaintNeeded() 
					&& !pObj->IsPlayer()
					&& !m_DockingPorts.DoesPortPaintInFront(this, i))
				{
				int xObj, yObj;
				Ctx.XForm.Transform(pObj->GetPos(), &xObj, &yObj);

				CSpaceObject *pOldObj = Ctx.pObj;
				Ctx.pObj = pObj;

				pObj->Paint(Dest, xObj, yObj, Ctx);

				Ctx.pObj = pOldObj;
				}
			}
		}

	//	Paint satellites behind the station.

	PaintSatellites(Dest, x, y, CPaintOrder::sendToBack, Ctx);

	//	Paint

	if (byShimmer)
		Image.PaintImageShimmering(Dest, x, y, iTick, iVariant, byShimmer);

	else if (m_fRadioactive)
		Image.PaintImageWithGlow(Dest, x, y, iTick, iVariant, CG32bitPixel(0, 255, 0));

	else
		Image.PaintImage(Dest, x, y, iTick, iVariant);

	//  Paint satellites in front of the station.

	PaintSatellites(Dest, x, y, (Ctx.fShowSatellites ? CPaintOrder::none : CPaintOrder::bringToFront), Ctx);

	//	Paint animations

	if (!IsAbandoned() && m_pType->HasAnimations())
		m_pType->PaintAnimations(Dest, x, y, iTick);

	//	If this is a sign, then paint the name of the station

	if (m_pType->IsSign() && !IsAbandoned())
		{
		RECT rcRect;

		rcRect.left = x - 26;
		rcRect.top = y - 20;
		rcRect.right = x + 40;
		rcRect.bottom = y + 20;

		GetUniverse().GetNamedFont(CUniverse::fontSign).DrawText(Dest, rcRect, RGB_SIGN_COLOR, GetNounPhrase(), -2);
		}

	//	Paint overlays

	m_Overlays.Paint(Dest, Image.GetImageViewportSize(), x, y, Ctx);

	//	Now paint any object that are docked in front of us

	if (!Ctx.fNoDockedShips)
		{
		for (i = 0; i < m_DockingPorts.GetPortCount(this); i++)
			{
			CSpaceObject *pObj = m_DockingPorts.GetPortObj(this, i);
			if (pObj 
					&& !pObj->IsPlayer()
					&& m_DockingPorts.DoesPortPaintInFront(this, i))
				{
				int xObj, yObj;
				Ctx.XForm.Transform(pObj->GetPos(), &xObj, &yObj);

				CSpaceObject *pOldObj = Ctx.pObj;
				Ctx.pObj = pObj;

				pObj->Paint(Dest, xObj, yObj, Ctx);

				Ctx.pObj = pOldObj;
				}
			}
		}

#ifdef DEBUG_BOUNDING_RECT
	{
	CVector vUR, vLL;
	GetBoundingRect(&vUR, &vLL);

	int xLeft, xRight, yTop, yBottom;
	Ctx.XForm.Transform(vUR, &xRight, &yTop);
	Ctx.XForm.Transform(vLL, &xLeft, &yBottom);

	DrawRectDotted(Dest, 
			xLeft, 
			yTop, 
			xRight - xLeft, 
			yBottom - yTop, 
			CG32bitPixel(220,220,220));
	}
#endif

#ifdef DEBUG_DOCK_PORT_POS
	for (int i = 0; i < m_DockingPorts.GetPortCount(this); i++)
		{
		int x, y;
		Ctx.XForm.Transform(m_DockingPorts.GetPortPos(this, i, NULL), &x, &y);
		Dest.Fill(x - 2, y - 2, 4, 4, CG32bitPixel(0, 255, 0));
		}
#endif
	}

void CStation::OnPaintAnnotations (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	OnPaintAnnotations
//
//	Paint additional annotations

	{
	m_Overlays.PaintAnnotations(Dest, x, y, Ctx);

	//	If this is an unexplored asteroid then annotate it so that we know which
	//	asteroids we've scanned or not.

	if (!m_fExplored 
			&& !IsOutOfPlaneObj()
			&& m_pType->ShowsUnexploredAnnotation()
			&& Ctx.bShowUnexploredAnnotation)
		{
		CString sType = strCapitalize(m_pType->GetAsteroidDesc().GetTypeLabel(GetUniverse()));
		COverlay::PaintCounterFlag(Dest, x, y, sType, CONSTLIT("Unexplored"), RGB_MINING_MARKER_UNEXPORED, Ctx);
		}
	}

void CStation::OnObjBounce (CSpaceObject *pObj, const CVector &vPos)

//	OnObjBounce
//
//	An object has just bounced off the station

	{
	CEffectCreator *pEffect;
	if (pEffect = m_pType->GetBarrierEffect())
		pEffect->CreateEffect(GetSystem(),
				this,
				vPos,
				GetVel(),
				0);
	}

void CStation::OnObjEnteredGate (CSpaceObject *pObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate)

//	OnObjEnteredGate
//
//	The given object has just entered some random gate (no necessarily us).

	{
	//	If the player entered a gate, fire a special event

	if (pObj->IsPlayer())
		FireOnPlayerLeftSystem(pObj, pDestNode, sDestEntryPoint, pStargate);
	}

void CStation::OnObjLeaveGate (CSpaceObject *pObj)

//	OnObjLeaveGate
//
//	An object has just passed through gate

	{
	//	Create gating effect

	if (!pObj->IsVirtual())
		{
		CEffectCreator *pEffect = m_pType->GetGateEffect();
		if (pEffect)
			pEffect->CreateEffect(GetSystem(),
					NULL,
					GetPos(),
					GetVel(),
					0);
		}
	}

void CStation::OnPaintMap (CMapViewportCtx &Ctx, CG32bitImage &Dest, int x, int y)

//	OnPaintMap
//
//	Paint the station

	{
	if (m_pType->IsVirtual())
		return;

	//	Draw an orbit

	if (m_pMapOrbit && m_fShowMapOrbit)
		m_pMapOrbit->Paint(Ctx, Dest, RGB_ORBIT_LINE);

	//	Draw the station

	if (m_Scale == scaleWorld)
		{
		if (m_MapImage.IsEmpty())
			CreateMapImage();

		Dest.Blt(0, 0, m_MapImage.GetWidth(), m_MapImage.GetHeight(), 255,
				m_MapImage,
				x - (m_MapImage.GetWidth() / 2),
				y - (m_MapImage.GetHeight() / 2));

		m_Overlays.PaintMapAnnotations(Ctx, Dest, x, y);

		//	Paint name

		if (ShowMapLabel())
			{
			Ctx.GetLabelPainter().AddLabel(m_MapLabel.GetLabel(),
					m_MapLabel.GetFont(),
					CG32bitPixel(255, 255, 255, 0x80),
					x + m_MapLabel.GetPosX(),
					y + m_MapLabel.GetPosY());
			}
		}
	else if (m_Scale == scaleStar)
		{
		if (m_MapImage.IsEmpty())
			CreateMapImage();

		CGDraw::BltLighten(Dest,
				x - (m_MapImage.GetWidth() / 2),
				y - (m_MapImage.GetHeight() / 2),
				m_MapImage,
				0, 0, m_MapImage.GetWidth(), m_MapImage.GetHeight());
		}
	else if (m_pType->ShowsMapIcon() && m_fKnown)
		{
		//	Paint the marker

		if (m_Scale == scaleStructure && m_rMass > 100000.0)
			{
			if (Ctx.IsPaintStationImagesEnabled())
				{
				if (m_MapImage.IsEmpty())
					CreateMapImage();

				if (!IsAbandoned() || IsImmutable())
					{
					Dest.Blt(0, 0, m_MapImage.GetWidth(), m_MapImage.GetHeight(), 255,
							m_MapImage,
							x - (m_MapImage.GetWidth() / 2),
							y - (m_MapImage.GetHeight() / 2));
					}
				else
					{
					CGDraw::BltGray(Dest,
							x - (m_MapImage.GetWidth() / 2),
							y - (m_MapImage.GetHeight() / 2),
							m_MapImage,
							0, 0, m_MapImage.GetWidth(), m_MapImage.GetHeight(), 0x80);
					}
				}
			else
				{
				// Paint the Icon
				PaintMarkerIcon(Dest, x, y);
				}
			}
		else
			// Paint the Icon
			PaintMarkerIcon(Dest, x, y);

		//	Paint the label

		if (ShowMapLabel())
			{
			GetUniverse().GetNamedFont(CUniverse::fontMapLabel).DrawText(Dest, 
					x + m_MapLabel.GetPosX() + 1, 
					y + m_MapLabel.GetPosY() + 1, 
					0,
					m_MapLabel.GetLabel());
			GetUniverse().GetNamedFont(CUniverse::fontMapLabel).DrawText(Dest, 
					x + m_MapLabel.GetPosX(), 
					y + m_MapLabel.GetPosY(), 
					RGB_MAP_LABEL,
					m_MapLabel.GetLabel());
			}
		}
	}

void CStation::OnPlayerObj (CSpaceObject *pPlayer)

//	OnPlayObj
//
//	Player has entered the system

	{
	int i;

	//	If this is a beacon, scan all stations in range. We scan here because
	//  we don't want to scan distant systems at game creation.

	if (m_pType->IsBeacon())
		{
		for (i = 0; i < GetSystem()->GetObjectCount(); i++)
			{
			CSpaceObject *pObj = GetSystem()->GetObject(i);

			if (pObj 
					&& pObj->GetScale() == scaleStructure
					&& pObj != this)
				{
				CVector vDist = pObj->GetPos() - GetPos();
				Metric rDist2 = vDist.Length2();
				if (rDist2 < BEACON_RANGE2)
					pObj->SetKnown();
				}
			}
		}

	//  Fire event

	FireOnPlayerEnteredSystem(pPlayer);
	}

void CStation::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	Read object data from a stream
//
//	DWORD		station type UNID
//	CString		m_sName
//	DWORD		sovereign UNID
//	DWORD		m_Scale
//	Metric		m_rMass
//	CCompositeImageSelector m_ImageSelector
//	DWORD		m_iDestroyedAnimation
//	DWORD		1 if orbit, 0xffffffff if no orbit
//	Orbit		System orbit
//	DWORD		m_iMapLabelPos
//	Metric		m_rParallaxDist
//	CString		m_sStargateDestNode
//	CString		m_sStargateDestEntryPoint
//	DWORD		armor class UNID, 0xffffffff if no armor
//	DWORD		m_iHitPoints
//	DWORD		m_iMaxHitPoints
//	DWORD		m_iStructuralHP
//	DWORD		m_iMaxStructuralHP
//
//	DWORD		No of devices
//	DWORD		device: class UNID
//	DWORD		device: m_dwData
//	DWORD		device: low = m_iPosAngle; hi = m_iPosRadius
//	DWORD		device: low = m_iMinFireArc; hi = m_iMaxFireArc
//	DWORD		device: low = m_iActivationDelay
//	DWORD		device: flags
//
//	Overlays	m_Overlays
//
//	DWORD		No of registered objects
//	DWORD		registered object (CSpaceObject ref)
//
//	DWORD		No of docking ports
//	DWORD		port: iStatus
//	DWORD		port: pObj (CSpaceObject ref)
//	Vector		port: vPos
//
//	DWORD		m_pBase
//	CString		m_sSubordinateID
//
//	DWORD		No of subordinates
//	DWORD		subordinate (CSpaceObject ref)
//
//	DWORD		No of targets
//	DWORD		target (CSpaceObject ref)
//
//	CAttackDetector m_Blacklist
//	DWORD		m_iAngryCounter
//	DWORD		m_iReinforceRequestCount
//	CCurrencyBlock	m_pMoney
//	CTradeDesc	m_pTrade
//
//	DWORD		m_dwWreckUNID
//	DWORD		flags
//	DWORD		m_iPaintOrder
//
//	CIntegralRotation m_pRotation
//	DWORD		m_iStarlightImageRotation
//	CTargetList	m_WeaponTargets

	{
#ifdef DEBUG_LOAD
	::OutputDebugString("CStation::OnReadFromStream\n");
#endif
	DWORD dwLoad;

	//	Station type

	Ctx.pStream->Read(dwLoad);
	m_pType = Ctx.GetUniverse().FindStationType(dwLoad);

	//	Read name

	m_sName.ReadFromStream(Ctx.pStream);
	if (Ctx.dwVersion >= 36)
		Ctx.pStream->Read(m_dwNameFlags);
	else
		m_dwNameFlags = 0;

	//	Stuff

	CSystem::ReadSovereignRefFromStream(Ctx, &m_pSovereign);

	Ctx.pStream->Read(dwLoad);
	m_Scale = CStationType::LoadScaleType(dwLoad);

	Ctx.pStream->Read(m_rMass);

	//	Read composite image selector

	if (Ctx.dwVersion >= 61)
		m_ImageSelector.ReadFromStream(Ctx);
	else
		{
		Ctx.pStream->Read(dwLoad);
		IImageEntry *pRoot = m_pType->GetImage().GetRoot();
		DWORD dwID = (pRoot ? pRoot->GetID() : DEFAULT_SELECTOR_ID);
		m_ImageSelector.AddVariant(dwID, dwLoad);
		}

	//	Animation data

	Ctx.pStream->Read(m_iDestroyedAnimation);

	//	Load orbit

	Ctx.pStream->Read(dwLoad);
	if (dwLoad != 0xffffffff)
		{
		m_pMapOrbit = new COrbit;
		m_pMapOrbit->ReadFromStream(Ctx);
		}

	//	Map label

	m_MapLabel.ReadFromStream(Ctx);

	//	Parallax

	if (Ctx.dwVersion >= 94)
		Ctx.pStream->Read(m_rParallaxDist);
	else
		{
		m_rParallaxDist = m_pType->GetParallaxDist();

		//	For previous versions, we have to set the OutOfPlaneObj cached bit
		//	manually.

		if (m_rParallaxDist != 1.0)
			SetOutOfPlaneObj(true);
		}

	//	Load the stargate info

	if (Ctx.dwVersion >= 16)
		{
		m_sStargateDestNode.ReadFromStream(Ctx.pStream);
		m_sStargateDestEntryPoint.ReadFromStream(Ctx.pStream);
		}
	else
		{
		CString sStargate;
		sStargate.ReadFromStream(Ctx.pStream);

		if (!sStargate.IsBlank())
			{
			CTopologyNode *pNode = Ctx.pSystem->GetStargateDestination(sStargate, &m_sStargateDestEntryPoint);
			if (pNode)
				m_sStargateDestNode = pNode->GetID();
			}
		}

	//	Armor class

	if (Ctx.dwVersion >= 151)
		m_Hull.ReadFromStream(Ctx);
	else
		{
		//	Armor class no longer loaded

		Ctx.pStream->Read(dwLoad);

		//	Hit points

		int iValue;
		Ctx.pStream->Read(iValue);
		m_Hull.SetHitPoints(iValue);

		//	Max hit points

		if (Ctx.dwVersion >= 77)
			{
			Ctx.pStream->Read(iValue);
			m_Hull.SetMaxHitPoints(iValue);
			}
		else
			m_Hull.SetMaxHitPoints(m_pType->GetHullDesc().GetMaxHitPoints());

		//	Structural HP

		Ctx.pStream->Read(iValue);
		m_Hull.SetStructuralHP(iValue);

		//	Max structural

		if (Ctx.dwVersion >= 31)
			{
			Ctx.pStream->Read(iValue);
			m_Hull.SetMaxStructuralHP(iValue);
			}
		else
			m_Hull.SetMaxStructuralHP(m_pType->GetHullDesc().GetMaxStructuralHP());

		//	Armor level

		m_Hull.SetArmorLevel(0);
		}

	//	Devices

	m_Devices.ReadFromStream(Ctx, this);

	//	Prior to version 180 we didn't automatically set devices to be
	//	secondary.

	if (Ctx.dwVersion < 180)
		m_Devices.SetSecondary(true);

	//	In debug mode, recalc weapon bonus in case anything has changed.

	if (Ctx.GetUniverse().InDebugMode())
		CalcDeviceBonus();

	//	Overlays

	if (Ctx.dwVersion >= 56)
		m_Overlays.ReadFromStream(Ctx, this);

	//	Registered objects / subscriptions

	if (Ctx.dwVersion < 77)
		{
		DWORD dwCount;
		Ctx.pStream->Read(dwCount);
		if (dwCount)
			{
			for (int i = 0; i < (int)dwCount; i++)
				{
				DWORD dwObjID;
				Ctx.pStream->Read(dwObjID);

				TArray<CSpaceObject *> *pList = Ctx.Subscribed.SetAt(dwObjID);
				pList->Insert(this);
				}
			}
		}

	//	Docking ports

	m_DockingPorts.ReadFromStream(this, Ctx);

	//	Subordinates

	if (Ctx.dwVersion >= 153)
		{
		CSystem::ReadObjRefFromStream(Ctx, &m_pBase);
		}
	else if (Ctx.dwVersion >= 47)
		{
		DWORD dwDummy;
		Ctx.pStream->Read(dwDummy);
		CSystem::ReadObjRefFromStream(Ctx, &m_pBase);
		}
	else
		{
		m_pBase = NULL;
		}

	if (Ctx.dwVersion >= 189)
		m_sSubordinateID.ReadFromStream(Ctx.pStream);

	m_Subordinates.ReadFromStream(Ctx);
	m_Targets.ReadFromStream(Ctx);

	//	More

	if (Ctx.dwVersion >= 9)
		m_Blacklist.ReadFromStream(Ctx);
	else
		{
		CSovereign *pBlacklist;
		int iCounter;

		CSystem::ReadSovereignRefFromStream(Ctx, &pBlacklist);
		Ctx.pStream->Read(iCounter);

		if (pBlacklist != NULL)
			m_Blacklist.Blacklist();
		}

	if (Ctx.dwVersion >= 3)
		Ctx.pStream->Read(m_iAngryCounter);
	else
		m_iAngryCounter = 0;

	if (Ctx.dwVersion >= 9)
		Ctx.pStream->Read(m_iReinforceRequestCount);
	else
		m_iReinforceRequestCount = 0;

	//	Money

	if (Ctx.dwVersion >= 62)
		{
		Ctx.pStream->Read(dwLoad);
		if (dwLoad != 0xffffffff)
			{
			m_pMoney = new CCurrencyBlock;
			m_pMoney->ReadFromStream(Ctx);
			}
		else
			m_pMoney = NULL;
		}
	else if (Ctx.dwVersion >= 12)
		{
		Ctx.pStream->Read(dwLoad);
		if (dwLoad)
			{
			m_pMoney = new CCurrencyBlock;
			m_pMoney->SetCredits(CONSTLIT("credit"), dwLoad);
			}
		else
			m_pMoney = NULL;
		}
	else
		m_pMoney = NULL;

	//	Trade desc

	if (Ctx.dwVersion >= 37)
		{
		Ctx.pStream->Read(dwLoad);
		if (dwLoad != 0xffffffff)
			{
			m_pTrade = new CTradingDesc;
			m_pTrade->ReadFromStream(Ctx);
			}
		else
			m_pTrade = NULL;
		}
	else
		m_pTrade = NULL;

	//	Wreck UNID

	Ctx.pStream->Read(m_dwWreckUNID);

	//	Previous versions didn't have m_ImageSelector

	if (m_dwWreckUNID && Ctx.dwVersion < 61)
		{
		CShipClass *pClass = Ctx.GetUniverse().FindShipClass(m_dwWreckUNID);
		if (pClass)
			{
			m_ImageSelector.DeleteAll();
			m_ImageSelector.AddShipwreck(DEFAULT_SELECTOR_ID, pClass);
			}
		}

	//	Flags

	Ctx.pStream->Read(dwLoad);
	m_fArmed =				((dwLoad & 0x00000001) ? true : false);
	m_fKnown =				((dwLoad & 0x00000002) ? true : false);
	m_fSuppressMapLabel =	((dwLoad & 0x00000004) ? true : false);
	m_fRadioactive =		((dwLoad & 0x00000008) ? true : false);
	bool bHasRotation =		(Ctx.dwVersion >= 146 ? ((dwLoad & 0x00000010) ? true : false) : false);
	m_fActive =				((dwLoad & 0x00000020) ? true : false);
	m_fNoReinforcements =	((dwLoad & 0x00000040) ? true : false);
	m_fReconned =			((dwLoad & 0x00000080) ? true : false);
	m_fFireReconEvent =		((dwLoad & 0x00000100) ? true : false);
	bool fNoArticle =		((dwLoad & 0x00000200) ? true : false);
	bool bImmutable =		(Ctx.dwVersion < 151  ? ((dwLoad & 0x00000400) ? true : false) : false);
	m_fExplored =			((dwLoad & 0x00000800) ? true : false);
	m_fAnonymous =			(Ctx.dwVersion >= 182 ? ((dwLoad & 0x00001000) ? true : false) : (m_pType->IsAnonymous() && m_sName.IsBlank()));
	m_fFadeImage =			(Ctx.dwVersion >= 187 ? ((dwLoad & 0x00002000) ? true : false) : false);
	m_fNoBlacklist =		((dwLoad & 0x00004000) ? true : false);
	m_fNoConstruction =		((dwLoad & 0x00008000) ? true : false);
	m_fBlocksShips =		((dwLoad & 0x00010000) ? true : false);
	bool bPaintOverhang =	(Ctx.dwVersion < 177  ? ((dwLoad & 0x00020000) ? true : false) : false);
	m_fShowMapOrbit =		((dwLoad & 0x00040000) ? true : false);
	m_fDestroyIfEmpty =		((dwLoad & 0x00080000) ? true : false);
	m_fIsSegment =		    ((dwLoad & 0x00100000) ? true : false);
	m_fForceMapLabel =		((dwLoad & 0x00200000) ? true : false);
	m_fHasMissileDefense =	((dwLoad & 0x00400000) ? true : false);
	m_fAllowEnemyDocking =	(Ctx.dwVersion >= 194 ? ((dwLoad & 0x00800000) ? true : false) : m_pType->IsEnemyDockingAllowed());

	//	Paint order

	if (Ctx.dwVersion >= 177)
		{
		Ctx.pStream->Read(dwLoad);
		m_iPaintOrder = (CPaintOrder::Types)dwLoad;
		}
	else
		{
		if (bPaintOverhang)
			m_iPaintOrder = CPaintOrder::paintOverhang;
		else
			m_iPaintOrder = CPaintOrder::none;
		}

	//	Init name flags

	if (Ctx.dwVersion < 36 && m_dwNameFlags == 0)
		{
		if (fNoArticle)
			m_dwNameFlags = nounNoArticle;
		}

	//	Previous versions did not store m_fImmutable

	if (Ctx.dwVersion < 77)
		bImmutable = m_pType->GetHullDesc().IsImmutable();

	//	Previous versions did not store m_fBlocksShips

	if (Ctx.dwVersion < 111)
		m_fBlocksShips = m_pType->IsWall();

	//	Fix a bug in version 94 in which asteroids were inadvertently marked
	//	as immutable.

	else if (Ctx.dwVersion == 94)
		{
		if (bImmutable
				&& m_pType->GetEjectaAdj() != 0)
			bImmutable = m_pType->GetHullDesc().IsImmutable();
		}

	//	Previous versions stored m_fImmutable here (instead of in m_Hull)

	if (Ctx.dwVersion < 151)
		{
		m_Hull.SetImmutable(bImmutable);
		m_Hull.SetHullType(m_pType->GetHullDesc().GetHullType());
		}

	//	Rotation

	if (bHasRotation)
		{
		m_pRotation = new CIntegralRotation;
		m_pRotation->ReadFromStream(Ctx, m_pType->GetRotationDesc());
		}
	else
		m_pRotation = NULL;

	if (Ctx.dwVersion >= 183)
		Ctx.pStream->Read(m_iStarlightImageRotation);

	//	Weapon targets

	if (m_fArmed)
		{
		if (Ctx.dwVersion >= 180)
			m_WeaponTargets.ReadFromStream(Ctx);
		else if (Ctx.dwVersion >= 153)
			{
			DWORD dwCount;
			Ctx.pStream->Read(dwCount);
			if (dwCount != 0xffffffff)
				{
				for (int i = 0; i < (int)dwCount; i++)
					{
					DWORD dwDummy;
					Ctx.pStream->Read(dwDummy);
					}
				}
			}
		}
	}

void CStation::OnSetEventFlags (void)

//	OnSetEventFlags
//
//	Sets the flags the cache whether the object has certain events

	{
	}

void CStation::OnStationDestroyed (const SDestroyCtx &Ctx)

//	OnStationDestroyed
//
//	Station in the system has been destroyed

	{
	//	Remove the object from any lists that it may be on

	m_Targets.Delete(&Ctx.Obj);
	m_WeaponTargets.Delete(Ctx.Obj);
	}

void CStation::OnSubordinateDestroyed (SDestroyCtx &Ctx)

//	OnSubordinateDestroyed
//
//	One of our guards has been destroyed and we might need to retaliate.
//
//	NOTE: Do not call this if the subordinate is an enemy of this station
//	(that can happen in occupation situations).

	{
	CSpaceObject *pAttacker = (Ctx.Attacker.GetObj());
	CSpaceObject *pOrderGiver = Ctx.GetOrderGiver();
	CSpaceObject *pTarget;

	if (pAttacker 
			&& pAttacker->CanAttack()
			&& pOrderGiver
			&& !IsAbandoned()
			&& (pTarget = CalcTargetToAttack(pAttacker, pOrderGiver)))
		{
		//	If we were attacked by a friend, then we tell our station
		//	so they can be blacklisted.

		if (!IsAngryAt(pOrderGiver))
			{
			//	See if we should blacklist and retaliate. We only blacklist the player
			//	and only if we can tell who attacked us.

			if (pOrderGiver->IsPlayer()
					&& CanBlacklist())
				{
				if (
					//	If our sovereign considers the player to be a major threat, then
					//	we immediately blacklist.

						(GetSovereign()
							&& GetSovereign()->GetPlayerThreatLevel() >= CSovereign::threatMajor)

					//	If we're neutral, then this attack is enough to blacklist

						|| !IsFriend(pOrderGiver)

					//	If the attack is unjustified, then we blacklist

						|| !IsPlayerAttackJustified())
					{
					if (Blacklist(pOrderGiver))
						DeterAttack(pTarget);
					}
				}
			}

		//	Otherwise, we deter the target

		else
			{
			DeterAttack(pTarget);
			}
		}
	}

void CStation::OnSubordinateHit (SDamageCtx &Ctx)

//	OnSubordinateHit
//
//	One of our subordinates was hit.

	{
	CSpaceObject *pAttacker = (Ctx.Attacker.GetObj());
	CSpaceObject *pOrderGiver = Ctx.GetOrderGiver();
	CSpaceObject *pTarget;

	if (pAttacker 
			&& pAttacker->CanAttack()
			&& pOrderGiver
			&& !IsAbandoned()
			&& (pTarget = CalcTargetToAttack(pAttacker, pOrderGiver)))
		{
		//	If we were attacked by a friend, then we tell our station
		//	so they can be blacklisted.

		if (!IsAngryAt(pOrderGiver))
			{
			if (pOrderGiver->IsPlayer()
					&& CanBlacklist())
				{
				if (
					//	If our sovereign considers the player to be a major threat,
					//	then we immediately blacklist.

						(GetSovereign()
								&& GetSovereign()->GetPlayerThreatLevel() >= CSovereign::threatMajor)

					//	Or if we've been hit enough times that we're sure it's not
					//	an accident.

						|| m_Blacklist.Hit(GetSystem()->GetTick()))
					{
					if (Blacklist(pOrderGiver))
						DeterAttack(pTarget);
					}
				}
			}

		//	Otherwise, we deter the target

		else
			{
			DeterAttack(pTarget);
			}
		}
	}

void CStation::OnSystemCreated (SSystemCreateCtx &CreateCtx)

//	OnSystemCreated
//
//	Called when the system is created

	{
	FinishCreation(&CreateCtx);
	}

void CStation::OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick)

//	OnUpdate
//
//	Update

	{
	DEBUG_TRY

	bool bCalcBounds = false;
	bool bCalcDeviceBonus = false;
	int iTick = GetSystem()->GetTick() + GetDestiny();

	//	If we need to destroy this station, do it now

	if (m_fDestroyIfEmpty 
			&& GetItemList().GetCount() == 0
			&& m_DockingPorts.GetPortsInUseCount(this) == 0)
		{
		Destroy(removedFromSystem, CDamageSource());
		return;
		}

	//	Active station attack, etc.

	if (!IsAbandoned())
		{
		//	Update blacklist counter
		//	NOTE: Once the player is blacklisted by this station, there is
		//	no way to get off the blacklist. (At least no automatic way).

		m_Blacklist.Update(iTick);

		//	Update attacks

		if (m_fArmed && !m_pType->IsVirtual()
				&& (m_pType->CanAttackIndependently() || (m_pBase && !m_pBase->IsAbandoned())))
			{
			if (!UpdateAttacking(Ctx, iTick))
				return;
			}

		//	Update reinforcements

		UpdateReinforcements(iTick);

		//	Update trade

		if ((iTick % TRADE_UPDATE_FREQUENCY) == 0)
			UpdateTrade(Ctx, INVENTORY_REFRESHED_PER_UPDATE);
		}

	//	Update docking ports

	m_DockingPorts.UpdateAll(Ctx, this);

	//	Update each device. If updating destroys the station, then we're done.

	if (!m_Devices.IsEmpty())
		{
		if (!UpdateDevices(Ctx, iTick, m_WeaponTargets, bCalcDeviceBonus))
			return;
		}

	//	Update destroy animation

	if (m_iDestroyedAnimation)
		UpdateDestroyedAnimation();

	//	If we're moving, slow down

	if (!IsAnchored() && !GetVel().IsNull())
		AddDrag(g_SpaceDragFactor);

	//	Overlays

	if (!m_Overlays.IsEmpty())
		{
		if (!UpdateOverlays(Ctx, bCalcBounds, bCalcDeviceBonus))
			return;
		}

	//	Update as necessary

	if (bCalcDeviceBonus)
		CalcDeviceBonus();

	if (bCalcBounds)
		CalcBounds();

	DEBUG_CATCH
	}

void CStation::OnUpdateExtended (const CTimeSpan &ExtraTime)

//	OnUpdateExtended
//
//	Update after an extended period of time

	{
	m_Hull.UpdateExtended(m_pType->GetHullDesc(), ExtraTime);
	}

void CStation::OnWriteToStream (IWriteStream *pStream)

//	OnWriteToStream
//
//	Write the object's data to stream
//
//	DWORD		station type UNID
//	CString		m_sName
//	DWORD		m_dwNameFlags
//	DWORD		sovereign UNID
//	DWORD		m_Scale
//	Metric		m_rMass
//	CCompositeImageSelector m_ImageSelector
//	DWORD		m_iDestroyedAnimation
//	DWORD		1 if orbit, 0xffffffff if no orbit
//	Orbit		System orbit
//	DWORD		m_iMapLabelPos
//	Metric		m_rParallaxDist
//	CString		m_sStargateDestNode
//	CString		m_sStargateDestEntryPoint
//	DWORD		armor class UNID, 0xffffffff if no armor
//	DWORD		m_iHitPoints
//	DWORD		m_iMaxHitPoints
//	DWORD		m_iStructuralHP
//	DWORD		m_iMaxStructuralHP
//
//	DWORD		device: class UNID
//	DWORD		device: m_dwData
//	DWORD		device: low = m_iPosAngle; hi = m_iPosRadius
//	DWORD		device: low = m_iMinFireArc; hi = m_iMaxFireArc
//	DWORD		device: low = m_iActivationDelay
//	DWORD		device: flags
//
//	Overlays	m_Overlays
//
//	DWORD		No of registered objects
//	DWORD		registered object (CSpaceObject ref)
//
//	DWORD		No of docking ports
//	DWORD		port: iStatus
//	DWORD		port: pObj (CSpaceObject ref)
//	Vector		port: vPos
//
//	DWORD		m_pBase
//	CString		m_sSubordinateID
//	
//	DWORD		No of subordinates
//	DWORD		subordinate (CSpaceObject ref)
//
//	DWORD		No of targets
//	DWORD		target (CSpaceObject ref)
//
//	CAttackDetector m_Blacklist
//	DWORD		m_iAngryCounter
//	DWORD		m_iReinforceRequestCount
//	CCurrencyBlock	m_pMoney
//	CTradingDesc	m_pTrade
//
//	DWORD		m_dwWreckUNID
//	DWORD		flags
//	DWORD		m_iPaintOrder
//
//	CIntegralRotation	m_pRotation
//	DWORD		m_iStarlightImageRotation
//	CTargetList	m_WeaponTargets

	{
	DWORD dwSave;

	dwSave = m_pType->GetUNID();
	pStream->Write(dwSave);
	m_sName.WriteToStream(pStream);
	pStream->Write(m_dwNameFlags);
	GetSystem()->WriteSovereignRefToStream(m_pSovereign, pStream);
	pStream->Write((DWORD)m_Scale);
	pStream->Write(m_rMass);
	m_ImageSelector.WriteToStream(pStream);
	pStream->Write(m_iDestroyedAnimation);

	if (m_pMapOrbit)
		{
		dwSave = 1;
		pStream->Write(dwSave);
		m_pMapOrbit->WriteToStream(*pStream);
		}
	else
		{
		dwSave = 0xffffffff;
		pStream->Write(dwSave);
		}

	m_MapLabel.WriteToStream(*pStream);
	pStream->Write(m_rParallaxDist);
	m_sStargateDestNode.WriteToStream(pStream);
	m_sStargateDestEntryPoint.WriteToStream(pStream);

	m_Hull.WriteToStream(*pStream, this);
	m_Devices.WriteToStream(pStream);
	m_Overlays.WriteToStream(pStream);
	m_DockingPorts.WriteToStream(this, pStream);

	GetSystem()->WriteObjRefToStream(m_pBase, pStream, this);
	m_sSubordinateID.WriteToStream(pStream);
	m_Subordinates.WriteToStream(GetSystem(), pStream);
	m_Targets.WriteToStream(GetSystem(), pStream);

	m_Blacklist.WriteToStream(pStream);
	pStream->Write(m_iAngryCounter);
	pStream->Write(m_iReinforceRequestCount);

	//	Money

	if (m_pMoney)
		{
		dwSave = 1;
		pStream->Write(dwSave);

		m_pMoney->WriteToStream(pStream);
		}
	else
		{
		dwSave = 0xffffffff;
		pStream->Write(dwSave);
		}

	//	Trade desc

	if (m_pTrade)
		{
		dwSave = 1;
		pStream->Write(dwSave);

		m_pTrade->WriteToStream(pStream);
		}
	else
		{
		dwSave = 0xffffffff;
		pStream->Write(dwSave);
		}

	pStream->Write(m_dwWreckUNID);

	dwSave = 0;
	dwSave |= (m_fArmed ?				0x00000001 : 0);
	dwSave |= (m_fKnown ?				0x00000002 : 0);
	dwSave |= (m_fSuppressMapLabel ?	0x00000004 : 0);
	dwSave |= (m_fRadioactive ?			0x00000008 : 0);
	dwSave |= (m_pRotation ?			0x00000010 : 0);
	dwSave |= (m_fActive ?				0x00000020 : 0);
	dwSave |= (m_fNoReinforcements ?	0x00000040 : 0);
	dwSave |= (m_fReconned ?			0x00000080 : 0);
	dwSave |= (m_fFireReconEvent ?		0x00000100 : 0);
	//	0x00000200 retired
	//	0x00000400 retired at 151
	dwSave |= (m_fExplored ?			0x00000800 : 0);
	dwSave |= (m_fAnonymous ?			0x00001000 : 0);
	dwSave |= (m_fFadeImage ?			0x00002000 : 0);
	dwSave |= (m_fNoBlacklist ?			0x00004000 : 0);
	dwSave |= (m_fNoConstruction ?		0x00008000 : 0);
	dwSave |= (m_fBlocksShips ?			0x00010000 : 0);
	//	0x00020000 retired at 177
	dwSave |= (m_fShowMapOrbit ?		0x00040000 : 0);
	dwSave |= (m_fDestroyIfEmpty ?		0x00080000 : 0);
	dwSave |= (m_fIsSegment ?		    0x00100000 : 0);
	dwSave |= (m_fForceMapLabel ?		0x00200000 : 0);
	dwSave |= (m_fHasMissileDefense ?	0x00400000 : 0);
	dwSave |= (m_fAllowEnemyDocking ?	0x00800000 : 0);
	pStream->Write(dwSave);

	//	Paint order

	dwSave = (DWORD)m_iPaintOrder;
	pStream->Write(dwSave);

	//	Rotation

	if (m_pRotation)
		m_pRotation->WriteToStream(pStream);

	pStream->Write(m_iStarlightImageRotation);

	//	Weapon targets

	if (m_fArmed)
		m_WeaponTargets.WriteToStream(*GetSystem(), *pStream);
	}

void CStation::MarkImages (void)

//	MarkImages
//
//	Mark images in use.

	{
	CCompositeImageModifiers Modifiers;
	CalcImageModifiers(&Modifiers);

	m_pType->MarkImages(m_ImageSelector, Modifiers);
	}

void CStation::PaintLRSBackground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans)

//	PaintLRSBackground
//
//	Paints the object on an LRS

	{
	DEBUG_TRY

	if (m_pType->IsVirtual())
		return;

	//	Paint worlds and stars fully (and always in the background)

	if (m_Scale == scaleWorld || m_Scale == scaleStar)
		{
		if (m_MapImage.IsEmpty())
			CreateMapImage();

		Dest.Blt(0, 0, m_MapImage.GetWidth(), m_MapImage.GetHeight(), 255,
				m_MapImage,
				x - (m_MapImage.GetWidth() / 2),
				y - (m_MapImage.GetHeight() / 2));
		}

	//	Other kinds of stations are just dots

	else
		{
		//	Paint the label in the background

		if (m_fKnown && ShowMapLabel())
			{
			GetUniverse().GetNamedFont(CUniverse::fontMapLabel).DrawText(Dest, 
					x + m_MapLabel.GetPosX(), 
					y + m_MapLabel.GetPosY(), 
					RGB_LRS_LABEL,
					m_MapLabel.GetLabel());
			}
		}

	DEBUG_CATCH_MSG1("Crash in CStation::PaintLRSBackground: type: %08x", m_pType->GetUNID());
	}

void CStation::PaintLRSForeground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans)

//	PaintLRSForeground
//
//	Paints the object on an LRS

	{
	DEBUG_TRY

	if (m_pType->IsVirtual())
		return;

	//	Object is known if we can scan it.

	SetKnown();

	//	We've already painted these, but paint annotations on top

	if (m_Scale == scaleWorld || m_Scale == scaleStar)
		{
		m_Overlays.PaintLRSAnnotations(Trans, Dest, x, y);
		}

	//	Other kinds of stations are just dots

	else
		{
		PaintMarkerIcon(Dest, x, y);
		}

	DEBUG_CATCH_MSG1("Crash in CStation::PaintLRSForeground: type: %08x", m_pType->GetUNID());
	}


void CStation::PaintMarkerIcon (CG32bitImage& Dest, int x, int y)

//	PaintMarkerIcon
//
//	Paints the appropriate map icon for this station type at the given location 
//	on the given image.

	{
	DEBUG_TRY

	if (m_pType->IsVirtual())
		return;

	if (m_Scale == scaleWorld || m_Scale == scaleStar)
		{
		//these do not have an icon
		return;
		}

	else
		{
		CG32bitPixel rgbColor = GetSymbolColor();

		if (m_Scale == scaleStructure && m_rMass > 100000.0)
			{
			if (IsActiveStargate())
				{
				Dest.DrawDot(x, y, rgbColor, markerSmallSquare);
				Dest.DrawDot(x, y, rgbColor, markerMediumCross);
				}
			else if (!IsAbandoned() || IsImmutable())
				{
				CSovereign* pPlayer = GetUniverse().GetPlayerSovereign();
				CSpaceObject* pPlayerShip;
				if (IsPlayer() || GetSovereign()->IsPlayerOwned())
					{
					Dest.DrawDot(x + 1, y + 1, 0, markerSmallCircle);
					Dest.DrawDot(x, y, rgbColor, markerSmallFilledCircle);
					}
				else if ((pPlayerShip = GetUniverse().GetPlayerShip())
						&& IsAngryAt(pPlayerShip) && (IsFriend(*pPlayer) || IsNeutral(*pPlayer)))
					{
					Dest.DrawDot(x + 1, y + 1, 0, markerSmallTriangleUp);
					Dest.DrawDot(x, y, rgbColor, markerSmallFilledTriangleUp);
					}
				else if (pPlayer && IsFriend(*pPlayer))
					{
					Dest.DrawDot(x + 1, y + 1, 0, markerSmallSquare);
					Dest.DrawDot(x, y, rgbColor, markerSmallFilledSquare);
					}
				else if (pPlayer && IsNeutral(*pPlayer))
					{
					Dest.DrawDot(x + 1, y + 1, 0, markerSmallDiamond);
					Dest.DrawDot(x, y, rgbColor, markerSmallFilledDiamond);
					}
				else if (pPlayer && IsEnemy(*pPlayer))
					{
					Dest.DrawDot(x + 1, y + 1, 0, markerSmallTriangleDown);
					Dest.DrawDot(x, y, rgbColor, markerSmallFilledTriangleDown);
					}
				else
					{
					Dest.DrawDot(x + 1, y + 1, 0, markerSmallSquare);
					Dest.DrawDot(x, y, rgbColor, markerSmallFilledSquare);
					}
				}
			else if (IsAbandoned())
				{
				if (m_fExplored)
					rgbColor = CG32bitPixel(128, 128, 128);

				CSovereign* pPlayer = GetUniverse().GetPlayerSovereign();
				CSpaceObject* pPlayerShip;
				if (IsPlayer() || GetSovereign()->IsPlayerOwned())
					{
					Dest.DrawDot(x + 1, y + 1, 0, markerSmallCircle);
					Dest.DrawDot(x, y, rgbColor, markerSmallCircle);
					}
				else if ((pPlayerShip = GetUniverse().GetPlayerShip())
						&& IsAngryAt(pPlayerShip) && (IsFriend(*pPlayer) || IsNeutral(*pPlayer)))
					{
					Dest.DrawDot(x + 1, y + 1, 0, markerSmallTriangleUp);
					Dest.DrawDot(x, y, rgbColor, markerSmallTriangleUp);
					}
				else if (pPlayer && IsFriend(*pPlayer))
					{
					Dest.DrawDot(x + 1, y + 1, 0, markerSmallSquare);
					Dest.DrawDot(x, y, rgbColor, markerSmallSquare);
					}
				else if (pPlayer && IsNeutral(*pPlayer))
					{
					Dest.DrawDot(x + 1, y + 1, 0, markerSmallDiamond);
					Dest.DrawDot(x, y, rgbColor, markerSmallDiamond);
					}
				else if (pPlayer && IsEnemy(*pPlayer))
					{
					Dest.DrawDot(x + 1, y + 1, 0, markerSmallTriangleDown);
					Dest.DrawDot(x, y, rgbColor, markerSmallTriangleDown);
					}
				else
					{
					Dest.DrawDot(x + 1, y + 1, 0, markerSmallSquare);
					Dest.DrawDot(x, y, rgbColor, markerSmallSquare);
					}
				}
			else
				{
				CSovereign* pPlayer = GetUniverse().GetPlayerSovereign();
				CSpaceObject* pPlayerShip;
				if (IsPlayer() || GetSovereign()->IsPlayerOwned())
					{
					Dest.DrawDot(x + 1, y + 1, 0, markerSmallCircle);
					Dest.DrawDot(x, y, rgbColor, markerSmallFilledCircle);
					}
				else if ((pPlayerShip = GetUniverse().GetPlayerShip())
						&& IsAngryAt(pPlayerShip) && (IsFriend(*pPlayer) || IsNeutral(*pPlayer)))
					{
					Dest.DrawDot(x + 1, y + 1, 0, markerSmallTriangleUp);
					Dest.DrawDot(x, y, rgbColor, markerSmallFilledTriangleUp);
					}
				else if (pPlayer && IsFriend(*pPlayer))
					{
					Dest.DrawDot(x + 1, y + 1, 0, markerSmallSquare);
					Dest.DrawDot(x, y, rgbColor, markerSmallFilledSquare);
					}
				else if (pPlayer && IsNeutral(*pPlayer))
					{
					Dest.DrawDot(x + 1, y + 1, 0, markerSmallDiamond);
					Dest.DrawDot(x, y, rgbColor, markerSmallFilledDiamond);
					}
				else if (pPlayer && IsEnemy(*pPlayer))
					{
					Dest.DrawDot(x + 1, y + 1, 0, markerSmallTriangleDown);
					Dest.DrawDot(x, y, rgbColor, markerSmallFilledTriangleDown);
					}
				else
					{
					Dest.DrawDot(x + 1, y + 1, 0, markerSmallSquare);
					Dest.DrawDot(x, y, rgbColor, markerSmallFilledSquare);
					}
				}
			}
		else if (IsWreck())
			{
			//	Handle explored

			if (m_fExplored)
				rgbColor = CG32bitPixel(128, 128, 128);

			//	Draw icon

			Dest.DrawDot(x, y, rgbColor, markerSmallCircle);
			}
		else
			{
			CSovereign* pPlayer = GetUniverse().GetPlayerSovereign();
			CSpaceObject* pPlayerShip;

			//	Draw icon

			if (IsPlayer() || GetSovereign()->IsPlayerOwned())
				Dest.DrawDot(x, y, rgbColor, markerTinyCircle);
			else if ((pPlayerShip = GetUniverse().GetPlayerShip())
					&& IsAngryAt(pPlayerShip) && (IsFriend(*pPlayer) || IsNeutral(*pPlayer)))
				Dest.DrawDot(x, y, rgbColor, markerTriangleUpDot);
			else if (pPlayer && IsFriend(*pPlayer))
				Dest.DrawDot(x, y, rgbColor, markerSquareDot);
			else if (pPlayer && IsNeutral(*pPlayer))
				Dest.DrawDot(x, y, rgbColor, markerDiamondDot);
			else if (pPlayer && IsEnemy(*pPlayer))
				Dest.DrawDot(x, y, rgbColor, markerTriangleDownDot);
			else
				Dest.DrawDot(x, y, rgbColor, markerTinyCircle);
			}
		}

	DEBUG_CATCH_MSG1("Crash in CStation::PaintMarkerIcon: type: %08x", m_pType->GetUNID());
	}


void CStation::PaintSatellites (CG32bitImage &Dest, int x, int y, DWORD dwPaintOptions, SViewportPaintCtx &Ctx) const

//	PaintSatellites
//
//	Paint satellites. dwPaintOptions is one of the following:
//
//	CPaintOrder::none: Paint all satellites, regardless of paint order
//	CPaintOrder::bringToFront: Paint all satellites with bringToFront paint 
//		order.
//	CPaintOrder::sendToBack: Paint all satellites with sendToBack paint order.

	{
	CSpaceObject *pOldObj = Ctx.pObj;
	bool bOldInPaintSubordinate = Ctx.bInPaintSubordinate;
	Ctx.bInPaintSubordinate = true;

	//  Loop over all our subordinates and paint any segments.

	for (int i = 0; i < m_Subordinates.GetCount(); i++)
		{
		CSpaceObject *pObj = m_Subordinates.GetObj(i);

		//	If not a satellite, then ignore.

		CPaintOrder::Types iPaintOrder;
		if (!pObj->IsSatelliteSegmentOf(*this, &iPaintOrder))
			continue;

		//	If not the right paint order, then skip.

		if (dwPaintOptions != CPaintOrder::none
				&& !(iPaintOrder & dwPaintOptions))
			continue;

		//	Paint

		if (dwPaintOptions == CPaintOrder::none || pObj->IsPaintNeeded())
			{
			int xObj, yObj;
			Ctx.XForm.Transform(pObj->GetPos(), &xObj, &yObj);

			Ctx.pObj = pObj;
			pObj->Paint(Dest, xObj, yObj, Ctx);
			}
		}

	Ctx.pObj = pOldObj;
	Ctx.bInPaintSubordinate = bOldInPaintSubordinate;
	}

bool CStation::PointInObject (const CVector &vObjPos, const CVector &vPointPos) const

//	PointInObject
//
//	Returns TRUE if the given point is inside the object

	{
	DEBUG_TRY

	//	Figure out the coordinates of vPos relative to the center of the
	//	ship, in pixels.

	CVector vOffset = vPointPos - vObjPos;
	int x = mathRound(vOffset.GetX() / g_KlicksPerPixel);
	int y = -mathRound(vOffset.GetY() / g_KlicksPerPixel);

	//	Ask the image if the point is inside or not

	int iTick, iVariant;
	const CObjectImageArray &Image = GetImage(false, &iTick, &iVariant);

	return Image.PointInImage(x, y, iTick, iVariant);

	DEBUG_CATCH
	}

bool CStation::PointInObject (SPointInObjectCtx &Ctx, const CVector &vObjPos, const CVector &vPointPos) const

//	PointInObject
//
//	Returns TRUE if the given point is inside the object

	{
	DEBUG_TRY

	//	Figure out the coordinates of vPos relative to the center of the
	//	ship, in pixels.

	CVector vOffset = vPointPos - vObjPos;
	int x = mathRound(vOffset.GetX() / g_KlicksPerPixel);
	int y = -mathRound(vOffset.GetY() / g_KlicksPerPixel);

	return Ctx.pObjImage->PointInImage(Ctx, x, y);

	DEBUG_CATCH
	}

void CStation::PointInObjectInit (SPointInObjectCtx &Ctx) const

//	PointInObjectInit
//
//	Initializes context for PointInObject (for improved performance in loops)

	{
	int iTick, iVariant;
	Ctx.pObjImage = &GetImage(false, &iTick, &iVariant);

	Ctx.pObjImage->PointInImageInit(Ctx, iTick, iVariant);
	}

void CStation::RaiseAlert (CSpaceObject *pTarget)

//	RaiseAlert
//
//	Raise an alert (if we're that kind of station)

	{
	if (pTarget == NULL)
		return;
	
	if (m_Targets.FindObj(pTarget))
		return;

	//	Add this to our target list

	m_Targets.Add(pTarget);

#ifdef DEBUG_ALERTS
	GetUniverse().DebugOutput("%d: Raising alert...", this);
#endif

	//	Tell all other friendly stations in the system that they
	//	should attack the target.

	CSovereign *pSovereign = GetSovereign();
	for (int i = 0; i < GetSystem()->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetSystem()->GetObject(i);

		if (pObj 
				&& pObj->GetCategory() == catStation
				&& pObj->GetSovereign() == pSovereign
				&& pObj->CanAttack()
				&& pObj != this)
			Communicate(pObj, msgAttackDeter, pTarget);
		}
	}

void CStation::RemoveOverlay (DWORD dwID)

//	RemoveOverlay
//
//	Removes the given overlay
	
	{
	m_Overlays.RemoveField(this, dwID); 

	//	Recalc bonuses, etc.

	CalcBounds();
	CalcDeviceBonus();
	}

bool CStation::RemoveSubordinate (CSpaceObject *pSubordinate)

//	RemoveSubordinate
//
//	If the object is a subordinate, it removes it (and returns TRUE)

	{
	return m_Subordinates.Delete(pSubordinate);
	}

bool CStation::RequestGate (CSpaceObject *pObj)

//	RequestGate
//
//	Requests that the given object be transported through the gate

	{
	//	Get the destination node for this gate
	//	(If pNode == NULL then it means that we are gating to nowhere;
	//	This is used by ships that "gate" back into their carrier or their
	//	station.)

	CTopologyNode *pNode = GetUniverse().FindTopologyNode(m_sStargateDestNode);

	//	For the player, ask all objects if they want to allow the player to 
	//	enter a gate.

	if (pObj->IsPlayer() && !pObj->OnGateCheck(pNode, m_sStargateDestEntryPoint, this))
		return false;

	//	Create gating effect

	if (!pObj->IsVirtual())
		{
		CEffectCreator *pEffect = m_pType->GetGateEffect();
		if (pEffect)
			pEffect->CreateEffect(GetSystem(),
					NULL,
					GetPos(),
					GetVel(),
					0);
		}

	//	Let the object gate itself

	pObj->EnterGate(pNode, m_sStargateDestEntryPoint, this);

	return true;
	}

void CStation::SetAngry (void)

//	SetAngry
//
//	Station is angry

	{
	if (m_iAngryCounter < MAX_ANGER)
		m_iAngryCounter = Max(MIN_ANGER, m_iAngryCounter + ANGER_INC);
	}

void CStation::SetFlotsamImage (CItemType *pItemType)

//	SetFlotsamImage
//
//	Sets the image for the station

	{
	m_ImageSelector.DeleteAll();
	m_ImageSelector.AddFlotsam(DEFAULT_SELECTOR_ID, pItemType);

	//	Set bounds

	CalcBounds();
	}

void CStation::SetKnown (bool bKnown)

//	SetKnown
//
//	Makes station known to the player.
	
	{
	if (m_fKnown != bKnown)
		{
		//	If this is a stargate, we reveal the destination node.

		CTopologyNode *pDestNode;
		if (bKnown
				&& IsStargate()
				&& (pDestNode = GetUniverse().FindTopologyNode(m_sStargateDestNode)))
			pDestNode->SetKnown();

		//  If we have a virtual base, then set it to be known. This handles the
		//  case of New Victoria Arcology, which has a bunch of segments and a
		//  virtual center.

		if (bKnown
				&& m_pBase 
				&& m_pBase->IsVirtual())
			m_pBase->SetKnown();

		//	Done

		m_fKnown = bKnown;
		}
	}

int CStation::GetImageVariant (void)

//	GetImageVariant
//
//	Returns the current image variant

	{
	IImageEntry *pRoot = m_pType->GetImage().GetRoot();
	DWORD dwID = (pRoot ? pRoot->GetID() : DEFAULT_SELECTOR_ID);
	return m_ImageSelector.GetVariant(dwID);
	}

void CStation::SetImageVariant (int iVariant)

//	SetImageVariant
//
//	Sets the given variant
	
	{
	IImageEntry *pRoot = m_pType->GetImage().GetRoot();
	DWORD dwID = (pRoot ? pRoot->GetID() : DEFAULT_SELECTOR_ID);

	m_ImageSelector.DeleteAll();
	m_ImageSelector.AddVariant(dwID, iVariant);

	//	Set bounds

	CalcBounds();
	}

void CStation::SetImageVariant (const CString &sVariantID)

//	SetImageVariant
//
//	Sets image variant from a string, which can be either a number or an ID.

	{
	int iVariant = m_pType->GetImage().GetVariantByID(sVariantID);
	if (iVariant == -1)
		SetImageVariant(0);
	else
		SetImageVariant(iVariant);
	}

void CStation::SetMapOrbit (const COrbit &oOrbit)

//	SetMapOrbit
//
//	Sets the orbit description

	{
	if (m_pMapOrbit)
		delete m_pMapOrbit;

	m_pMapOrbit = new COrbit(oOrbit);
	m_fShowMapOrbit = true;
	}

void CStation::SetName (const CString &sName, DWORD dwFlags)

//	SetName
//
//	Sets the name of the station

	{
	m_sName = sName;
	m_dwNameFlags = dwFlags;
	m_fAnonymous = false;

	//	Clear cache so we recompute label metrics

	m_MapLabel.CleanUp();
	m_fMapLabelInitialized = false;
	}

void CStation::SetStargate (const CString &sDestNode, const CString &sDestEntryPoint)

//	SetStargate
//
//	Sets the stargate label

	{
	m_sStargateDestNode = sDestNode;
	m_sStargateDestEntryPoint = sDestEntryPoint;
	}

void CStation::SetStarlightParams (const CSpaceObject &StarObj, Metric rLightRadius)

//	SetStarlightParams
//
//	Sets the parameters for starlight.

	{
	constexpr int DEFAULT_LIGHTING_ANGLE = 315;

	//	Only world objects respond to starlight.

	if (GetScale() != scaleWorld)
		return;

	//	Compute the direction and distance of the star

	Metric rStarDist;
	int iStarAngle = ::VectorToPolar(GetPos() - StarObj.GetPos(), &rStarDist);

	//	Skip if we're outside the light radius.

	if (rStarDist > rLightRadius)
		return;

	//	Skip if some other star is closer than this one.

	if (m_rStarlightDist != 0.0 && m_rStarlightDist < rStarDist)
		return;

	//	Set the parameters

	m_iStarlightImageRotation = iStarAngle - DEFAULT_LIGHTING_ANGLE;
	m_rStarlightDist = rStarDist;
	}

void CStation::SetWreckParams (CShipClass *pWreckClass, CShip *pShip)

//	SetWreckParams
//
//	Sets the mass and name for the station based on the wreck class

	{
	//	If the station doesn't have a name, set it now

	if (!IsNameSet())
		{
		CString sName;
		DWORD dwFlags;

		//	If the ship has a proper name, then use that

		if (pShip && pWreckClass->HasShipName())
			{
			DWORD dwNounFlags;
			sName = pShip->GetNamePattern(0, &dwNounFlags);

			if (dwNounFlags & nounPersonalName)
				sName = strPatternSubst(CONSTLIT("wreck of %s's %s"),
						CLanguage::ComposeNounPhrase(sName, 1, CString(), dwNounFlags, nounArticle),
						(!pWreckClass->GetShipTypeName().IsBlank() ? pWreckClass->GetShipTypeName() : pWreckClass->GetShortName()));
			else
				sName = strPatternSubst(CONSTLIT("wreck of %s"),
						CLanguage::ComposeNounPhrase(sName, 1, CString(), dwNounFlags, nounArticle));

			dwFlags = nounDefiniteArticle;
			}

		//	Otherwise, use the generic name for the class

		else
			{
			DWORD dwShipFlags;
			CString sShipName = pWreckClass->GetNamePattern(0, &dwShipFlags);

			if (dwShipFlags & nounDefiniteArticle)
				{
				sName = strPatternSubst(CONSTLIT("wreck of %s"),
						CLanguage::ComposeNounPhrase(sShipName, 1, CString(), dwShipFlags, nounArticle));
				dwFlags = nounDefiniteArticle;
				}
			else
				{
				sName = strPatternSubst(CONSTLIT("%s wreck"), sShipName);
				dwFlags = 0;
				}
			}

		SetName(sName, dwFlags);
		}

	//	Set the mass
	//	NOTE: Mass cannot be 0 or else the bouncing routines might fail.

	SetMass(Max(1.0, (Metric)pWreckClass->GetHullDesc().GetMass()));

	//	Set hit points for the structure

	int iHP = pWreckClass->GetMaxStructuralHitPoints();
	m_Hull.SetStructuralHP(iHP);
	m_Hull.SetMaxStructuralHP(iHP);

	//	Set the wreck UNID

	m_dwWreckUNID = pWreckClass->GetUNID();
	}

bool CStation::SetProperty (const CString &sName, ICCItem *pValue, CString *retsError)

//	SetProperty
//
//	Sets a station property

	{
	CCodeChain &CC = GetUniverse().GetCC();
	CString sError;

	if (strEquals(sName, PROPERTY_ACTIVE))
		{
		if (pValue->IsNil())
			SetInactive();
		else
			SetActive();
		return true;
		}
	else if (strEquals(sName, PROPERTY_ANGRY))
		{
		if (pValue->IsNil())
			m_iAngryCounter = 0;
		else if (pValue->IsInteger())
			m_iAngryCounter = Max(0, pValue->GetIntegerValue());
		else
			SetAngry();
		return true;
		}
	else if (strEquals(sName, PROPERTY_ALLOW_ENEMY_DOCKING))
		{
		m_fAllowEnemyDocking = !pValue->IsNil();
		return true;
		}
	else if (strEquals(sName, PROPERTY_BARRIER))
		{
		m_fBlocksShips = !pValue->IsNil();
		return true;
		}
	else if (strEquals(sName, PROPERTY_DESTROY_WHEN_EMPTY))
		{
		m_fDestroyIfEmpty = !pValue->IsNil();
		return true;
		}
	else if (strEquals(sName, PROPERTY_EXPLORED))
		{
		m_fExplored = !pValue->IsNil();
		return true;
		}
	else if (strEquals(sName, PROPERTY_IGNORE_FRIENDLY_FIRE))
		{
		m_fNoBlacklist = !pValue->IsNil();
		return true;
		}
	else if (strEquals(sName, PROPERTY_IMAGE_SELECTOR))
		{
		m_ImageSelector.ReadFromItem(GetUniverse().GetDesignCollection(), ICCItemPtr(pValue->Reference()));
		return true;
		}
	else if (strEquals(sName, PROPERTY_IMAGE_VARIANT))
		{
		IImageEntry *pRoot = m_pType->GetImage().GetRoot();
		if (pRoot == NULL)
			{
			*retsError = CONSTLIT("Station has no image.");
			return false;
			}

		if (pValue->IsNumber())
			{
			int iVariant = pValue->GetIntegerValue();
			if (iVariant < 0 || iVariant >= pRoot->GetVariantCount())
				{
				*retsError = strPatternSubst(CONSTLIT("Invalid variant: %d"), iVariant);
				return false;
				}

			SetImageVariant(iVariant);
			}
		else
			{
			CString sVariant = pValue->GetStringValue();
			int iVariant = pRoot->GetVariantByID(sVariant);
			if (iVariant < 0 || iVariant >= pRoot->GetVariantCount())
				{
				*retsError = strPatternSubst(CONSTLIT("Invalid variant: %s"), sVariant);
				return false;
				}

			SetImageVariant(iVariant);
			}

		return true;
		}

	else if (strEquals(sName, PROPERTY_NO_FRIENDLY_FIRE))
		{
		if (pValue->IsNil())
			ClearNoFriendlyFire();
		else
			SetNoFriendlyFire();

		return true;
		}

	else if (strEquals(sName, PROPERTY_PAINT_LAYER))
		{
		if (pValue->IsNil())
			m_iPaintOrder = CPaintOrder::none;
		else
			{
			CPaintOrder::Types iOrder = CPaintOrder::Parse(pValue->GetStringValue());
			if (iOrder == CPaintOrder::error)
				{
				*retsError = strPatternSubst(CONSTLIT("Unable to set paint layer: %s"), pValue->GetStringValue());
				return false;
				}

			m_iPaintOrder = iOrder;
			}

		return true;
		}
	else if (strEquals(sName, PROPERTY_PARALLAX))
		{
		if (pValue->IsNil())
			{
			m_rParallaxDist = 1.0;
			SetOutOfPlaneObj(false);
			}
		else
			{
			Metric rParallaxDist = pValue->GetIntegerValue() / 100.0;
			if (rParallaxDist <= 0.0)
				{
				*retsError = strPatternSubst(CONSTLIT("Parallax must be >= 1."));
				return false;
				}

			m_rParallaxDist = rParallaxDist;
			SetOutOfPlaneObj(m_rParallaxDist != 1.0);

			CalcBounds();
			}

		return true;
		}
	else if (strEquals(sName, PROPERTY_ORBIT))
		{
		if (pValue->IsNil())
			{
			if (m_pMapOrbit)
				{
				delete m_pMapOrbit;
				m_pMapOrbit = NULL;
				}

			m_fShowMapOrbit = false;
			return true;
			}
		else
			{
			COrbit OrbitDesc;
			if (!CreateOrbitFromList(CC, pValue, &OrbitDesc))
				{
				*retsError = CONSTLIT("Invalid orbit.");
				return false;
				}

			SetMapOrbit(OrbitDesc);
			return true;
			}
		}
	else if (strEquals(sName, PROPERTY_PLAYER_BACKLISTED))
		{
		CSpaceObject *pPlayer = GetUniverse().GetPlayerShip();

		if (pValue->IsNil())
			ClearBlacklist(pPlayer);
		else
			{
			if (Blacklist(pPlayer))
				DeterAttack(pPlayer);
			}

		return true;
		}
	else if (strEquals(sName, PROPERTY_RADIOACTIVE))
		{
		if (pValue->IsNil())
			ClearCondition(ECondition::radioactive);
		else
			SetCondition(ECondition::radioactive);
		return true;
		}
	else if (strEquals(sName, PROPERTY_ROTATION))
		{
		SetRotation(pValue->GetIntegerValue());
		return true;
		}
	else if (strEquals(sName, PROPERTY_ROTATION_SPEED))
		{
		if (m_pRotation)
			m_pRotation->SetRotationSpeedDegrees(m_pType->GetRotationDesc(), pValue->GetDoubleValue());
		return true;
		}
	else if (strEquals(sName, PROPERTY_SHIP_CONSTRUCTION_ENABLED))
		{
		m_fNoConstruction = pValue->IsNil();
		return true;
		}
	else if (strEquals(sName, PROPERTY_SHIP_REINFORCEMENT_ENABLED))
		{
		m_fNoReinforcements = pValue->IsNil();
		return true;
		}
	else if (strEquals(sName, PROPERTY_SHOW_MAP_LABEL))
		{
		m_fSuppressMapLabel = pValue->IsNil();
		m_fForceMapLabel = !pValue->IsNil();
		return true;
		}
	else if (strEquals(sName, PROPERTY_SHOW_MAP_ORBIT))
		{
		m_fShowMapOrbit = !pValue->IsNil();
		return true;
		}
	else if (m_Hull.SetPropertyIfFound(sName, pValue, &sError))
		{
		if (!sError.IsBlank())
			{
			if (retsError) *retsError = sError;
			return false;
			}

		return true;
		}
	else
		return CSpaceObject::SetProperty(sName, pValue, retsError);
	}

bool CStation::ShowMapLabel (int *retcxLabel, int *retcyLabel) const

//	ShowMapLabel
//
//	Returns TRUE if the object has a map label. This will also make sure that
//	m_MapLabel is initialized and valid.
//
//	If we have a map label, and if we support re-positioning labels to avoid
//	overlap, then we optionally return the width and height of the label in 
//	pixels.

	{
	//	If we're not initialized, then we need to figure out the deal with our
	//	map label.

	if (!m_fMapLabelInitialized)
		{
		switch (m_Scale)
			{
			case scaleStar:
				//	Stars never have a map label
				m_MapLabel.CleanUp();
				break;

			case scaleWorld:
				{
				CString sMapLabel = GetNounPhrase(nounTitleCapitalize);
				int iPlanetarySize = GetPlanetarySize();
				bool bHasMapLabel;

				//	Figure out if our creator is either forcing or suppressing
				//	a map label.

				if (m_fSuppressMapLabel || m_pType->SuppressMapLabel() || !m_pType->ShowsMapIcon())
					bHasMapLabel = false;

				else if (m_fForceMapLabel || m_pType->ForceMapLabel())
					bHasMapLabel = true;

				//	Otherwise, figure out if this planet is large enough to
				//	deserve a label.

				else
					{
					bHasMapLabel = !sMapLabel.IsBlank() 
						&& *sMapLabel.GetASCIIZPointer() != '('
						&& !m_fAnonymous
						&& iPlanetarySize >= MIN_NAMED_WORLD_SIZE;
					}

				//	If we have a map label, initialize it now.

				if (bHasMapLabel)
					{
					//	We must have a map image to compute the label position.

					if (m_MapImage.IsEmpty())
						CreateMapImage();

					if (iPlanetarySize < WORLD_SIZE)
						m_MapLabel.SetLabel(sMapLabel, GetUniverse().GetNamedFont(CUniverse::fontPlanetoidMapLabel));
					else if (iPlanetarySize < LARGE_WORLD_SIZE)
						m_MapLabel.SetLabel(sMapLabel, GetUniverse().GetNamedFont(CUniverse::fontWorldMapLabel));
					else
						m_MapLabel.SetLabel(sMapLabel, GetUniverse().GetNamedFont(CUniverse::fontLargeWorldMapLabel));

					m_MapLabel.SetPos(-m_MapLabel.GetFont().MeasureText(sMapLabel) / 2, m_MapImage.GetHeight() / 2);
					}
				else
					m_MapLabel.CleanUp();

				break;
				}

			default:
				{
				if (m_fSuppressMapLabel || m_pType->SuppressMapLabel() || !m_pType->ShowsMapIcon())
					m_MapLabel.CleanUp();
				else
					{
					m_MapLabel.SetLabel(GetNounPhrase(nounTitleCapitalize), GetUniverse().GetNamedFont(CUniverse::fontMapLabel));
					m_MapLabel.RealizePos();
					}
				}
			}

		//	Initialized

		m_fMapLabelInitialized = true;
		}

	//	Once initialized, we store the map label (if any) in m_MapLabel.

	if (m_MapLabel.IsEmpty())
		return false;
	else if (retcxLabel == NULL && retcyLabel == NULL)
		return true;
	else if (m_Scale == scaleWorld)
		{
		//	World labels are never repositioned

		if (retcxLabel) *retcxLabel = 0;
		if (retcyLabel) *retcyLabel = 0;
		return true;
		}
	else
		{
		m_MapLabel.MeasureLabel(retcxLabel, retcyLabel);
		return true;
		}
	}

void CStation::Undock (CSpaceObject *pObj)

//	Undock
//
//	Undocks from the station

	{
	bool bWasDocked;
	m_DockingPorts.Undock(this, pObj, &bWasDocked);

	//	If we're set to destroy when empty AND we're empty AND no one else is 
	//	docked, then destroy the station at the next update.

	if (m_pType->IsDestroyWhenEmpty() 
			&& bWasDocked
			&& GetItemList().GetCount() == 0
			&& m_DockingPorts.GetPortsInUseCount(this) == 0)
		m_fDestroyIfEmpty = true;
	}

bool CStation::UpdateAttacking (SUpdateCtx &Ctx, int iTick)

//	UpdateAttacking
//
//	Station attacks any enemies in range. Returns FALSE if the station was
//	destroyed.

	{
	DEBUG_TRY
	constexpr int MAX_ENEMIES = 10;

	//	Compute the range at which we attack enemies

	Metric rAttackRange;
	Metric rAttackRange2;
	if (m_iAngryCounter > 0)
		{
		//	If we're angry, we look for targets out to our perception range.
		//	We used to check out to the range of the farthest weapon, but for
		//	some cases, such as Penitents' Oracus weapon, we want to fire even
		//	if the target is well out of range (because we want to block 
		//	incoming missiles).

		rAttackRange = CPerceptionCalc::GetMaxDist(GetPerception());
		rAttackRange2 = rAttackRange * rAttackRange;

		//	If the player is in range, then she is under attack

		if (Ctx.pPlayer 
				&& IsAngryAt(Ctx.pPlayer)
				&& GetDistance2(Ctx.pPlayer) < rAttackRange2)
			Ctx.pSystem->SetPlayerUnderAttack();

		//	Countdown

		m_iAngryCounter--;
		}
	else
		{
		rAttackRange = MAX_ATTACK_DISTANCE;
		rAttackRange2 = rAttackRange * rAttackRange;
		}

	//	Update the list of weapon targets

	if ((iTick % STATION_SCAN_TARGET_FREQUENCY) == 0)
		UpdateTargets(Ctx, rAttackRange + GetHitSize());

	//	Fire with all weapons (if we've got a target)

	if (!m_WeaponTargets.IsEmpty()
			&& !IsParalyzed()
			&& !IsDisarmed())
		{
		for (CDeviceItem DeviceItem : GetDeviceSystem())
			{
			CInstalledDevice &Weapon = *DeviceItem.GetInstalledDevice();

			//	Skip non-weapons and weapons that aren't ready.

			if ((DeviceItem.GetCategory() != itemcatWeapon && DeviceItem.GetCategory() != itemcatLauncher)
					|| !Weapon.IsReady())
				continue;

			CDeviceClass::SActivateCtx ActivateCtx(NULL, m_WeaponTargets);
			Weapon.Activate(ActivateCtx);
			if (IsDestroyed())
				return false;

			Weapon.SetTimeUntilReady(m_pType->GetFireRateAdj() * Weapon.GetActivateDelay(this) / 10);
			}
		}

	return true;

	DEBUG_CATCH
	}

void CStation::UpdateDestroyedAnimation (void)

//	UpdateDestroyedAnimation
//
//	Updates animation when station is destroyed.

	{
	ASSERT(m_iDestroyedAnimation > 0);

	const CObjectImageArray &Image = GetImage(false);
	int cxWidth = RectWidth(Image.GetImageRect());

	CEffectCreator *pEffect = GetUniverse().FindEffectType(g_StationDestroyedUNID);
	if (pEffect)
		{
		for (int i = 0; i < mathRandom(1, 3); i++)
			{
			CVector vPos = GetPos() 
					+ PolarToVector(mathRandom(0, 359), g_KlicksPerPixel * mathRandom(1, cxWidth / 3));

			pEffect->CreateEffect(GetSystem(),
					this,
					vPos,
					GetVel(),
					0);
			}
		}

	m_iDestroyedAnimation--;
	}

bool CStation::UpdateDevices (SUpdateCtx &Ctx, int iTick, CTargetList &TargetList, bool &iobModified)

//	UpdateDevices
//
//	Updates all devices. We set iobModified to TRUE if a device was modified,
//	requiring a device recalc.
//
//	We return TRUE if successful. FALSE if the station was destroyed inside the
//	update.

	{
	CDeviceClass::SDeviceUpdateCtx DeviceCtx(TargetList, iTick);
	for (CDeviceItem DeviceItem : GetDeviceSystem())
		{
		CInstalledDevice &Device = *DeviceItem.GetInstalledDevice();

		DeviceCtx.ResetOutputs();

		Device.Update(this, DeviceCtx);
		if (IsDestroyed())
			return false;

		//	If the device was repaired or disabled, we need to update

		if (DeviceCtx.bRepaired)
			iobModified = true;

		else if (DeviceCtx.bSetDisabled && Device.SetEnabled(this, false))
			iobModified = true;
		}

	return true;
	}

bool CStation::UpdateOverlays (SUpdateCtx &Ctx, bool &iobCalcBounds, bool &iobCalcDevices)

//	UpdateOverlays
//
//	Update overlays. Returns TRUE if we succeed or FALSE if the update destroyed
//	the station.

	{
	bool bModified;
	const CObjectImageArray &Image = GetImage(true);

	m_Overlays.Update(this, Image.GetImageViewportSize(), GetRotation(), &bModified);
	if (IsDestroyed())
		return false;

	else if (bModified)
		{
		iobCalcBounds = true;
		iobCalcDevices = true;
		}

	return true;
	}

void CStation::UpdateReinforcements (int iTick)

//	UpdateReinforcements
//
//	Check to see if it is time to send reinforcements to the station

	{
	DEBUG_TRY

	//	Repair damage

	if ((iTick % STATION_REPAIR_FREQUENCY) == 0)
		{
		//	Repair damage to station

		if (m_Hull.UpdateRepairDamage(m_pType->GetHullDesc(), iTick))
			{
			//	Remove any harmful overlays

			if (mathRandom(1, 100) <= 20)
				ScrapeOverlays();
			}

		//	Repair damage to ships

		if (!m_pType->GetShipRegenDesc().IsEmpty())
			m_DockingPorts.RepairAll(this, m_pType->GetShipRegenDesc().GetRegen(iTick, STATION_REPAIR_FREQUENCY));
		}

	//	Construction

	if (m_pType->GetShipConstructionRate()
			&& !m_fNoConstruction
			&& (iTick % m_pType->GetShipConstructionRate()) == 0)
		{
		//	Iterate over all ships and count the number that are
		//	associated with the station.

		int iCount = CalcNumberOfShips();

		//	If we already have the maximum number, then don't bother

		if (iCount < m_pType->GetMaxShipConstruction())
			{
			SShipCreateCtx Ctx;
			Ctx.pSystem = GetSystem();
			Ctx.pBase = this;
			Ctx.pGate = this;
			m_pType->GetConstructionTable()->CreateShips(Ctx);
			}
		}

	//	Get reinforcements

	if ((iTick % STATION_REINFORCEMENT_FREQUENCY) == 0)
		{
		if (!m_fNoReinforcements
				&& m_pType->GetDefenderCount().NeedsMoreReinforcements(this))
			{
			//	If we've requested several rounds of reinforcements but have
			//	never received any, then it's likely that they are being
			//	destroyed at the gate, so we stop requesting so many

			if (m_iReinforceRequestCount > 0)
				{
				int iLongTick = (iTick / STATION_REINFORCEMENT_FREQUENCY);
				int iCycle = Min(32, m_iReinforceRequestCount * m_iReinforceRequestCount);
				if ((iLongTick % iCycle) != 0)
					return;
				}

			//	We either bring in ships from the nearest gate or we build
			//	them ourselves.

			CSpaceObject *pGate;
			if (m_pType->BuildsReinforcements()
					|| (pGate = GetNearestStargate(true)) == NULL)
				pGate = this;

			//	Generate reinforcements

			SShipCreateCtx Ctx;
			Ctx.pSystem = GetSystem();
			Ctx.pBase = this;
			Ctx.pGate = pGate;
			m_pType->GetReinforcementsTable()->CreateShips(Ctx);

			//	Increment counter

			m_iReinforceRequestCount++;
			}
		else
			m_iReinforceRequestCount = 0;
		}

	//	Attack targets on the target list

	if ((iTick % STATION_TARGET_FREQUENCY) == 0)
		{
		int i;

#ifdef DEBUG_ALERTS
		GetUniverse().DebugOutput("%d: Attack target list", this);
#endif

		for (i = 0; i < m_Targets.GetCount(); i++)
			{
			CSpaceObject *pTarget = m_Targets.GetObj(i);

			int iLeft = mathRandom(3, 5);
			int iCount = m_Subordinates.GetCount();
			for (int j = 0; j < iCount && iLeft > 0; j++)
				{
				if (Communicate(m_Subordinates.GetObj(j), 
						msgDestroyBroadcast, 
						pTarget) == resAck)
					{
					iLeft--;
#ifdef DEBUG_ALERTS
					GetUniverse().DebugOutput("   %d acknowledges attack order", m_Subordinates.GetObj(i));
#endif
					}
				}
			}
		}

	DEBUG_CATCH
	}

void CStation::UpdateTargets (SUpdateCtx &Ctx, Metric rAttackRange)

//	UpdateTargets
//
//	Update the targets for each weapon.

	{
	CTargetList::STargetOptions Options;
	Options.rMaxDist = rAttackRange;
	Options.bIncludeNonAggressors = true;

	if (m_fHasMissileDefense && m_iAngryCounter > 0)
		Options.bIncludeMissiles = true;

	if (m_Blacklist.IsBlacklisted())
		Options.bIncludePlayer = true;

	if (m_iAngryCounter > 0)
		Options.bNoRangeCheck = true;

	m_WeaponTargets.Init(*this, Options);
	}
