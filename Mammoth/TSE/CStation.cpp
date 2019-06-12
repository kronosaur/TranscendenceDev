//	CStation.cpp
//
//	CStation class

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

#define LANGID_DOCKING_REQUEST_DENIED			CONSTLIT("core.dockingRequestDenied")

#define PAINT_LAYER_OVERHANG					CONSTLIT("overhang")

#define PROPERTY_ABANDONED						CONSTLIT("abandoned")
#define PROPERTY_ACTIVE							CONSTLIT("active")
#define PROPERTY_ANGRY							CONSTLIT("angry")
#define PROPERTY_BARRIER						CONSTLIT("barrier")
#define PROPERTY_DEST_NODE_ID					CONSTLIT("destNodeID")
#define PROPERTY_DEST_STARGATE_ID				CONSTLIT("destStargateID")
#define PROPERTY_DOCKING_PORT_COUNT				CONSTLIT("dockingPortCount")
#define PROPERTY_EXPLORED						CONSTLIT("explored")
#define PROPERTY_HP								CONSTLIT("hp")
#define PROPERTY_IGNORE_FRIENDLY_FIRE			CONSTLIT("ignoreFriendlyFire")
#define PROPERTY_IMAGE_SELECTOR					CONSTLIT("imageSelector")
#define PROPERTY_MAX_HP							CONSTLIT("maxHP")
#define PROPERTY_MAX_STRUCTURAL_HP				CONSTLIT("maxStructuralHP")
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
#define PROPERTY_SHOW_MAP_LABEL					CONSTLIT("showMapLabel")
#define PROPERTY_SHOW_MAP_ORBIT					CONSTLIT("showMapOrbit")
#define PROPERTY_STARGATE_ID					CONSTLIT("stargateID")
#define PROPERTY_STRUCTURAL_HP					CONSTLIT("structuralHP")
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
		m_fArmed(false),
		m_dwSpare(0),
		m_pType(NULL),
		m_pRotation(NULL),
		m_pMapOrbit(NULL),
		m_pBase(NULL),
		m_pDevices(NULL),
		m_iAngryCounter(0),
		m_iReinforceRequestCount(0),
		m_pMoney(NULL),
		m_pTrade(NULL)

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

	if (m_pDevices)
		delete [] m_pDevices;

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

	SDestroyCtx DestroyCtx;
	DestroyCtx.pObj = this;
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
			CInstalledDevice *pDevice = &m_pDevices[iDevSlot];
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

void CStation::AddSubordinate (CSpaceObject *pSubordinate)

//	AddSubordinate
//
//	Add this object to our list of subordinates

	{
	m_Subordinates.Add(pSubordinate);

	//	HACK: If we're adding a station, set it to point back to us

	CStation *pSatellite = pSubordinate->AsStation();
	if (pSatellite)
		pSatellite->SetBase(this);
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

void CStation::CalcBounds (void)

//	CalcBounds
//
//	Calculates and sets station bounds

	{
	const CObjectImageArray &Image = GetImage(false, NULL, NULL);
	RECT rcBounds = Image.GetImageRect();

	int xOffset;
	int yOffset;
	if (Image.GetImageOffset(0, 0, &xOffset, &yOffset))
		{
		rcBounds.right += 2 * Absolute(xOffset);
		rcBounds.bottom += 2 * Absolute(yOffset);
		}

	//	Add overlays

	m_Overlays.AccumulateBounds(this, Image.GetImageViewportSize(), GetRotation(), &rcBounds);

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

	if (m_pDevices == NULL)
		return;

	//	Enhancements from system

	const CEnhancementDesc *pSystemEnhancements = GetSystemEnhancements();

	//	Keep track of duplicate installed devices

	TSortMap<DWORD, int> DeviceTypes;

	//	Loop over all devices

	for (int i = 0; i < GetDeviceCount(); i++)
		{
		CInstalledDevice &Device = *GetDevice(i);
		if (Device.IsEmpty())
			continue;

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

		for (int j = 0; j < GetDeviceCount(); j++)
			{
			CInstalledDevice &OtherDev = *GetDevice(j);
			if (i != j && !OtherDev.IsEmpty())
				{
				OtherDev.AccumulateEnhancements(this, &Device, EnhancementIDs, pEnhancements);
				}
			}

		//	Add enhancements from system

		if (pSystemEnhancements)
			pSystemEnhancements->Accumulate(GetSystem()->GetLevel(), ItemCtx.GetItem(), EnhancementIDs, pEnhancements);

		//	Deal with class specific stuff

		switch (Device.GetCategory())
			{
			case itemcatLauncher:
			case itemcatWeapon:
				{
				//	Overlays add a bonus

				int iBonus = m_Overlays.GetWeaponBonus(&Device, this);
				if (iBonus != 0)
					pEnhancements->InsertHPBonus(iBonus);
				break;
				}
			}

		//	Set the bonuses
		//	Note that these include any bonuses confered by item enhancements

		Device.SetActivateDelay(pEnhancements->CalcActivateDelay(ItemCtx));

		//	Take ownership of the stack.

		Device.SetEnhancements(pEnhancements);
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
	//	Modifiers (such as station damage)

	if (retModifiers)
		{
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
			&& !m_pType->IsEnemyDockingAllowed()
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
			Ctx.pEnhancements->InsertHPBonus(Explosion.iBonus);
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
	int i;

	if (!CreateCtx.bIgnoreLimits && !pType->CanBeEncountered())
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
	pStation->m_iMapLabelPos = CMapLabelArranger::posRight;
	pStation->m_rMass = pType->GetMass();
	pStation->m_dwWreckUNID = 0;
	pStation->m_fNoBlacklist = false;
	pStation->SetHasGravity(pType->HasGravity());
	pStation->m_fPaintOverhang = pType->IsPaintLayerOverhang();
	pStation->m_fDestroyIfEmpty = false;
    pStation->m_fIsSegment = CreateCtx.bIsSegment;

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
		pStation->m_sName = Name.GenerateName(&CreateCtx.SystemCtx.NameParams, &pStation->m_dwNameFlags);
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
	pStation->m_fNoMapLabel = !pType->ShowsMapLabel();

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

	CXMLElement *pDevices = pDesc->GetContentElementByTag(DEVICES_TAG);
	if (pDevices)
		{
		CItemListManipulator Items(pStation->GetItemList());

		for (i = 0; 
				(i < pDevices->GetContentElementCount() && i < maxDevices);
				i++)
			{
			CXMLElement *pDeviceDesc = pDevices->GetContentElement(i);
			DWORD dwDeviceID = pDeviceDesc->GetAttributeInteger(DEVICE_ID_ATTRIB);
			if (dwDeviceID == 0)
				dwDeviceID = pDeviceDesc->GetAttributeInteger(ITEM_ATTRIB);
			CDeviceClass *pClass = System.GetUniverse().FindDeviceClass(dwDeviceID);
			if (pClass == NULL)
				{
				if (retsError)
					*retsError = strPatternSubst(CONSTLIT("Cannot find deviceID: %08x"), dwDeviceID);
				return ERR_FAIL;
				}

			//	Allocate the devices structure

			if (pStation->m_pDevices == NULL)
				pStation->m_pDevices = new CInstalledDevice [maxDevices];

			//	Add as an item

			CItem DeviceItem(pClass->GetItemType(), 1);
			Items.AddItem(DeviceItem);

			//	Initialize properties of the device slot

			SDesignLoadCtx Ctx;
			pStation->m_pDevices[i].InitFromXML(Ctx, pDeviceDesc);
			pStation->m_pDevices[i].OnDesignLoadComplete(Ctx);

			//	Install the device

			pStation->m_pDevices[i].Install(*pStation, Items, i, true);

			//	Is this a weapon? If so, set a flag so that we know that
			//	this station is armed. This is an optimization so that we don't
			//	do a lot of work for stations that have no weapons (e.g., asteroids)

			if (pStation->m_pDevices[i].GetCategory() == itemcatWeapon
					|| pStation->m_pDevices[i].GetCategory() == itemcatLauncher)
				{
				pStation->m_fArmed = true;
				pStation->m_pDevices[i].SelectFirstVariant(pStation);
				}
			}

		//	Compute bonuses

		pStation->CalcDeviceBonus();
		}

	//	Get notifications when other objects are destroyed

	pStation->SetObjectDestructionHook();

	//	Figure out the sovereign

	pStation->m_pSovereign = (CreateCtx.pSovereign ? CreateCtx.pSovereign : pType->GetSovereign());

	//	Initialize docking ports structure

	pStation->m_DockingPorts.InitPortsFromXML(pStation, pDesc);

	//	Make radioactive, if necessary

	if (pType->IsRadioactive())
		pStation->SetCondition(CConditionSet::cndRadioactive);

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

	pType->SetEncountered(&System);

	//	Fire events on devices

	if (pStation->m_pDevices)
		{
		for (i = 0; i < maxDevices; i++)
			if (!pStation->m_pDevices[i].IsEmpty())
				pStation->m_pDevices[i].FinishInstall();
		}

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

ALERROR CStation::CreateMapImage (void)

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

void CStation::CreateStarlightImage (int iStarAngle, Metric rStarDist)

//	CreateStarlightImage
//
//	Creates an image of the object rotated so it's shadow faces towards 
//	iStarAngle.

	{
	//	If we already have a starlight image, then we skip. This can happen if
	//	we have a double star and overlapping light zones.

	if (!m_StarlightImage.IsEmpty())
		return;

	//	Figure out the rotation

	int iRotation = iStarAngle - 315;

	//	Get the source image

	int iTick, iVariant;
	const CObjectImageArray &Image = GetImage(false, &iTick, &iVariant);

	//	Create a rotated image

	m_StarlightImage.InitFromRotated(Image, iTick, iVariant, iRotation);

	//	Recalculate bounds

	CalcBounds();

	//	While we're here, create the map image

	CreateMapImage();
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
			Ctx.pEnhancements->InsertHPBonus(Explosion.iBonus);
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
		PartImage.Init(g_ShipExplosionParticlesUNID,
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

Metric CStation::GetAttackDistance (void) const

//	GetAttackDistance
//
//	Returns the distance at which we attack enemies.

	{
	if (m_iAngryCounter > 0)
		return Max(MAX_ATTACK_DISTANCE, m_pType->GetMaxEffectiveRange());
	else
		return MAX_ATTACK_DISTANCE;
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

	CInstalledDevice *pShields = GetNamedDevice(devShields);
	if (pShields && GetShieldLevel() > 0)
		return pShields->GetDamageEffectiveness(pAttacker, pWeapon);
	else
		return m_pType->GetHullDesc().CalcDamageEffectiveness(pAttacker, pWeapon);
	}

CDesignType *CStation::GetDefaultDockScreen (CString *retsName) const

//	GetDockScreen
//
//	Returns the screen on dock (NULL_STR if none)

	{
	if (IsAbandoned() && m_pType->GetAbandonedScreen(retsName))
		return m_pType->GetAbandonedScreen(retsName);
	else
		return m_pType->GetFirstDockScreen(retsName);
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
	//	If we have a rotated image, use that

	if (!m_StarlightImage.IsEmpty())
		{
		if (retiTick)
			*retiTick = 0;

		if (retiVariant)
			*retiVariant = 0;

		return m_StarlightImage;
		}

	//	Otherwise get the image from the type

	else
		{
		CCompositeImageModifiers Modifiers;
		CalcImageModifiers(&Modifiers, retiTick);

		//	Image

		return m_pType->GetImage(m_ImageSelector, Modifiers, retiVariant);
		}
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
	int i;

	Metric rRange = 0.0;
	if (m_pDevices)
		{
		for (i = 0; i < maxDevices; i++)
			if (!m_pDevices[i].IsEmpty())
				rRange = Max(rRange, m_pDevices[i].GetMaxRange(CItemCtx(const_cast<CStation *>(this), &m_pDevices[i])));
		}

	return rRange;
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

	if (m_fPaintOverhang)
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

ICCItem *CStation::GetProperty (CCodeChainCtx &Ctx, const CString &sName)

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

	else if (strEquals(sName, PROPERTY_ANGRY))
		return (m_iAngryCounter > 0 ? CC.CreateInteger(m_iAngryCounter) : CC.CreateNil());

	else if (strEquals(sName, PROPERTY_BARRIER))
		return CC.CreateBool(m_fBlocksShips);

	else if (strEquals(sName, PROPERTY_DEST_NODE_ID))
		return (IsStargate() ? CC.CreateString(m_sStargateDestNode) : CC.CreateNil());

	else if (strEquals(sName, PROPERTY_DEST_STARGATE_ID))
		return (IsStargate() ? CC.CreateString(m_sStargateDestEntryPoint) : CC.CreateNil());

	else if (strEquals(sName, PROPERTY_DOCKING_PORT_COUNT))
		return CC.CreateInteger(m_DockingPorts.GetPortCount(this));

	else if (strEquals(sName, PROPERTY_EXPLORED))
		return CC.CreateBool(m_fExplored);

	else if (strEquals(sName, PROPERTY_IGNORE_FRIENDLY_FIRE))
		return CC.CreateBool(!CanBlacklist());

	else if (strEquals(sName, PROPERTY_IMAGE_SELECTOR))
		return m_ImageSelector.WriteToItem()->Reference();

	else if (strEquals(sName, PROPERTY_OPEN_DOCKING_PORT_COUNT))
		return CC.CreateInteger(GetOpenDockingPortCount());

	else if (strEquals(sName, PROPERTY_ORBIT))
		return (m_pMapOrbit ? CreateListFromOrbit(CC, *m_pMapOrbit) : CC.CreateNil());

	else if (strEquals(sName, PROPERTY_PARALLAX))
		return (m_rParallaxDist != 1.0 ? CC.CreateInteger((int)(m_rParallaxDist * 100.0)) : CC.CreateNil());

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
		return CSpaceObject::GetProperty(Ctx, sName);
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

CSpaceObject *CStation::GetTarget (CItemCtx &ItemCtx, DWORD dwFlags) const

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

bool CStation::HasMapLabel (void)

//	HasMapLabel
//
//	Returns TRUE if the object has a map label

	{
	return m_Scale != scaleWorld
			&& m_Scale != scaleStar
			&& m_pType->ShowsMapIcon() 
			&& !m_fNoMapLabel;
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

void CStation::InitMapLabel (void)

//	InitMapLabel
//
//	Make sure the map label is initialized.

	{
	if (m_sMapLabel.IsBlank())
		{
		m_sMapLabel = GetNounPhrase(nounTitleCapitalize);
		CMapLabelArranger::CalcLabelPos(m_sMapLabel, m_iMapLabelPos, m_xMapLabel, m_yMapLabel);
		}
	}

bool CStation::IsBlacklisted (CSpaceObject *pObj) const

//	IsBlacklisted
//
//	Returns TRUE if we are blacklisted
	
	{
	if (pObj)
		return (pObj->IsPlayer() && m_Blacklist.IsBlacklisted());
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

    if (!m_pType->ShowsMapIcon() || !m_pType->ShowsMapLabel())
        return false;

    //  Skip stargates, which we don't need to show in the details pane

    if (IsStargate())
        return false;

    //  Show it

    return true;
    }

void CStation::OnClearCondition (CConditionSet::ETypes iCondition, DWORD dwFlags)

//	OnClearCondition
//
//	Clears a condition

	{
	switch (iCondition)
		{
		case CConditionSet::cndRadioactive:
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

	int i;

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

	int iMomentum;
	if (!IsAnchored() && (iMomentum = Ctx.Damage.GetMomentumDamage()))
		{
		CVector vAccel = PolarToVector(Ctx.iDirection, -10 * iMomentum * iMomentum);
		Accelerate(vAccel, g_MomentumConstant);
		ClipSpeed(0.25 * LIGHT_SPEED);
		}

	//	Let our shield generators take a crack at it

	Ctx.iShieldHitDamage = Ctx.iDamage;
	if (m_pDevices && !Ctx.bIgnoreShields)
		{
		for (i = 0; i < maxDevices; i++)
			if (!m_pDevices[i].IsEmpty())
				{
				bool bAbsorbed = m_pDevices[i].AbsorbDamage(this, Ctx);
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

	Ctx.pDesc->FireOnDamageAbandoned(Ctx);
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
			SetCondition(CConditionSet::cndRadioactive);
		}

	//	If we have mining damage then call OnMining

	if (Ctx.Damage.GetMiningAdj())
		FireOnMining(Ctx);

	//	Once the station is abandoned, only WMD damage can destroy it.
    //  NOTE: We check level (which is 0 for no WMD) rather than 
    //  MassDestructionAdj, because we always have a little WMD. But for 
    //  this case, we want "real" WMD.

    if (Ctx.Damage.GetMassDestructionLevel() > 0)
        Ctx.iDamage = mathAdjust(Ctx.iDamage, Ctx.Damage.GetMassDestructionAdj());
	else
		Ctx.iDamage = 0;

	//	Give events a chance to change the damage

	if (HasOnDamageEvent())
		Ctx.iDamage = FireOnDamage(Ctx, Ctx.iDamage);

	//	Hit effect

	if (!Ctx.bNoHitEffect)
		Ctx.pDesc->CreateHitEffect(GetSystem(), Ctx);

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
		Ctx.pDesc->CreateHitEffect(GetSystem(), Ctx);

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

		bCustomDamage = Ctx.pDesc->FireOnDamageArmor(Ctx);
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

	//	If we're a multi-hull object then we adjust for mass destruction
	//	effects (non-mass destruction weapons don't hurt us very much)

    if (Ctx.iDamage > 0 && m_Hull.IsMultiHull())
        Ctx.iDamage = mathAdjust(Ctx.iDamage, Ctx.Damage.GetMassDestructionAdj());

	//	Give events a chance to change the damage

	if (HasOnDamageEvent() && (Ctx.iDamage > 0 || bCustomDamage))
		Ctx.iDamage = FireOnDamage(Ctx, Ctx.iDamage);

	//	Hit effect

	if (!Ctx.bNoHitEffect)
		Ctx.pDesc->CreateHitEffect(GetSystem(), Ctx);

	//	If no damage, we're done

	if (Ctx.iDamage == 0 && !bCustomDamage)
		return damageNoDamage;

	//	Tell our attacker that we got hit

	if (pOrderGiver && pOrderGiver->CanAttack())
		pOrderGiver->OnObjDamaged(Ctx);

	//	Handle special attacks

	if (Ctx.IsTimeStopped() 
			&& !IsImmuneTo(CConditionSet::cndTimeStopped)
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

	//	Otherwise we're in big trouble

	else
		{
		Abandon(Ctx.Damage.GetCause(), Ctx.Attacker, Ctx.pDesc);
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
	CSpaceObject *pTarget;

	ASSERT(pOrderGiver && pOrderGiver->CanAttack());

	//	If the player attacked us, we need to blacklist her

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

bool CStation::OnGetCondition (CConditionSet::ETypes iCondition) const

//	OnGetCondition
//
//	Returns station condition.

	{
	switch (iCondition)
		{
		case CConditionSet::cndRadioactive:
			return (m_fRadioactive ? true : false);

		default:
			return false;
		}
	}

bool CStation::OnIsImmuneTo (CConditionSet::ETypes iCondition) const

//	OnIsImmuneTo
//
//	Returns TRUE if we are immune to the given condition.

	{
	switch (iCondition)
		{
		case CConditionSet::cndTimeStopped:
			return m_pType->IsTimeStopImmune();

		default:
			return false;
		}
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

void CStation::OnSetCondition (CConditionSet::ETypes iCondition, int iTimer)

//	OnSetCondition
//
//	Sets a condition

	{
	switch (iCondition)
		{
		case CConditionSet::cndRadioactive:
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
	int i;

	//	If this object is docked with us, remove it from the
	//	docking table.

	m_DockingPorts.OnObjDestroyed(this, Ctx.pObj);

	//	Remove the object from any lists that it may be on

	m_Targets.Delete(Ctx.pObj);

	for (i = 0; i < m_WeaponTargets.GetCount(); i++)
		if (m_WeaponTargets[i] == Ctx.pObj)
			m_WeaponTargets[i] = NULL;

	//	If this was our base, remove it.

	if (Ctx.pObj == m_pBase)
		m_pBase = NULL;

	//	Remove from the subordinate list. No need to take action because the 
	//	ship/turret will communicate if we need to avenge.

	m_Subordinates.Delete(Ctx.pObj);
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

DWORD CStation::OnCommunicate (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2)

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
	int i;

	switch (iComponent)
		{
		case comCargo:
			{
			if (m_pDevices)
				{
				for (i = 0; i < maxDevices; i++)
					{
					CInstalledDevice *pDevice = &m_pDevices[i];
					if (pDevice->IsEmpty())
						continue;

					//	If one of our weapons doesn't have a variant selected, then
					//	try to select it now (if we just got some new ammo, this will
					//	select the ammo)

					if (!pDevice->IsVariantSelected(this))
						pDevice->SelectFirstVariant(this);
					}
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
	ASSERT(pOrderGiver && pOrderGiver->CanAttack());

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
	RECT rcOldObjBounds;
	int yOldAnnotions;

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
		//	If we need to paint docked ships, then preserve the object-specific
		//	context.

		rcOldObjBounds = Ctx.rcObjBounds;
		yOldAnnotions = Ctx.yAnnotations;

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

	//	Paint

	if (byShimmer)
		Image.PaintImageShimmering(Dest, x, y, iTick, iVariant, byShimmer);

	else if (m_fRadioactive)
		Image.PaintImageWithGlow(Dest, x, y, iTick, iVariant, CG32bitPixel(0, 255, 0));

	else
		Image.PaintImage(Dest, x, y, iTick, iVariant);

    //  If necessary, paint attached satellite stations

    if (Ctx.fShowSatellites)
        {
		CSpaceObject *pOldObj = Ctx.pObj;

        //  Loop over all our subordinates and paint any segments.

        for (i = 0; i < m_Subordinates.GetCount(); i++)
            {
            CSpaceObject *pObj = m_Subordinates.GetObj(i);
            if (pObj->IsSatelliteSegmentOf(this))
                {
				int xObj, yObj;
				Ctx.XForm.Transform(pObj->GetPos(), &xObj, &yObj);

				Ctx.pObj = pObj;
				pObj->Paint(Dest, xObj, yObj, Ctx);
                }
            }

		Ctx.pObj = pOldObj;
        }

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

		//	Restore context

		Ctx.rcObjBounds = rcOldObjBounds;
		Ctx.yAnnotations = yOldAnnotions;
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
			&& m_pType->ShowsUnexploredAnnotation()
			&& Ctx.bShowUnexploredAnnotation)
		{
		COverlay::PaintCounterFlag(Dest, x, y, NULL_STR, CONSTLIT("Unexplored"), CG32bitPixel(128, 128, 128), Ctx);
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
		//	Figure out the color

		CG32bitPixel rgbColor = GetSymbolColor();

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
				if (IsActiveStargate())
					{
					Dest.DrawDot(x, y, rgbColor, markerSmallSquare);
					Dest.DrawDot(x, y, rgbColor, markerMediumCross);
					}
				else if (!IsAbandoned() || IsImmutable())
					{
					Dest.DrawDot(x+1, y+1, 0, markerSmallSquare);
					Dest.DrawDot(x, y, rgbColor, markerSmallFilledSquare);
					}
				else
					{
					Dest.DrawDot(x+1, y+1, 0, markerSmallSquare);
					Dest.DrawDot(x, y, rgbColor, markerSmallSquare);
					}
				}
			}
		else
			Dest.DrawDot(x, y, 
					rgbColor, 
					markerSmallRound);

		//	Paint the label

		if (!m_fNoMapLabel)
			{
			//	We cache the label and the position here.

			InitMapLabel();

			GetUniverse().GetNamedFont(CUniverse::fontMapLabel).DrawText(Dest, 
					x + m_xMapLabel + 1, 
					y + m_yMapLabel + 1, 
					0,
					m_sMapLabel);
			GetUniverse().GetNamedFont(CUniverse::fontMapLabel).DrawText(Dest, 
					x + m_xMapLabel, 
					y + m_yMapLabel, 
					RGB_MAP_LABEL,
					m_sMapLabel);
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
//
//	CIntegralRotation m_pRotation
//
//	DWORD		No of weapon targets (only if armed)
//	DWORD		target (CSpaceObject ref)

	{
#ifdef DEBUG_LOAD
	::OutputDebugString("CStation::OnReadFromStream\n");
#endif
	int i;
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
		//	LATER: COrbit should load itself
		Ctx.pStream->Read((char *)m_pMapOrbit, sizeof(COrbit));
		}

	//	Map label

	if (Ctx.dwVersion >= 162)
		{
		Ctx.pStream->Read(dwLoad);
		m_iMapLabelPos = CMapLabelArranger::LoadPosition(dwLoad);
		}
	else
		{
		int xMapLabel, yMapLabel;
		Ctx.pStream->Read(xMapLabel);
		Ctx.pStream->Read(yMapLabel);

		//	For backwards compatibility we reverse engineer label coordinates
		//	to a label position.

		if (xMapLabel < 0 && yMapLabel > 0)
			m_iMapLabelPos = CMapLabelArranger::posBottom;
		else if (xMapLabel < 0)
			m_iMapLabelPos = CMapLabelArranger::posLeft;
		else
			m_iMapLabelPos = CMapLabelArranger::posRight;
		}

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

	Ctx.pStream->Read(dwLoad);
	if (dwLoad)
		{
		m_pDevices = new CInstalledDevice [dwLoad];

		for (i = 0; i < (int)dwLoad; i++)
			m_pDevices[i].ReadFromStream(*this, Ctx);

		//	In debug mode, recalc weapon bonus in case anything has changed.

		if (Ctx.GetUniverse().InDebugMode())
			CalcDeviceBonus();
		}

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
			for (i = 0; i < (int)dwCount; i++)
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
	m_fNoMapLabel =			((dwLoad & 0x00000004) ? true : false);
	m_fRadioactive =		((dwLoad & 0x00000008) ? true : false);
	bool bHasRotation =		(Ctx.dwVersion >= 146 ? ((dwLoad & 0x00000010) ? true : false) : false);
	m_fActive =				((dwLoad & 0x00000020) ? true : false);
	m_fNoReinforcements =	((dwLoad & 0x00000040) ? true : false);
	m_fReconned =			((dwLoad & 0x00000080) ? true : false);
	m_fFireReconEvent =		((dwLoad & 0x00000100) ? true : false);
	bool fNoArticle =		((dwLoad & 0x00000200) ? true : false);

	bool bImmutable;
	if (Ctx.dwVersion < 151)
		bImmutable =		((dwLoad & 0x00000400) ? true : false);
	else
		bImmutable = false;

	m_fExplored =			((dwLoad & 0x00000800) ? true : false);
	//	0x00001000 Unused as of version 160
	//	0x00002000 Unused as of version 160
	m_fNoBlacklist =		((dwLoad & 0x00004000) ? true : false);
	m_fNoConstruction =		((dwLoad & 0x00008000) ? true : false);
	m_fBlocksShips =		((dwLoad & 0x00010000) ? true : false);
	m_fPaintOverhang =		((dwLoad & 0x00020000) ? true : false);
	m_fShowMapOrbit =		((dwLoad & 0x00040000) ? true : false);
	m_fDestroyIfEmpty =		((dwLoad & 0x00080000) ? true : false);
	m_fIsSegment =		    ((dwLoad & 0x00100000) ? true : false);

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
		m_Hull.SetMultiHull(m_pType->GetHullDesc().IsMultiHull());
		}

	//	Rotation

	if (bHasRotation)
		{
		m_pRotation = new CIntegralRotation;
		m_pRotation->ReadFromStream(Ctx, m_pType->GetRotationDesc());
		}
	else
		m_pRotation = NULL;

	//	Weapon targets

	if (m_fArmed && Ctx.dwVersion >= 153)
		{
		int iCount;
		Ctx.pStream->Read(iCount);
		m_WeaponTargets.InsertEmpty(iCount);

		for (i = 0; i < iCount; i++)
			{
			CSystem::ReadObjRefFromStream(Ctx, &m_WeaponTargets[i]);
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
	int i;

	//	Remove the object from any lists that it may be on

	m_Targets.Delete(Ctx.pObj);

	for (i = 0; i < m_WeaponTargets.GetCount(); i++)
		if (m_WeaponTargets[i] == Ctx.pObj)
			m_WeaponTargets[i] = NULL;
	}

void CStation::OnSubordinateDestroyed (SDestroyCtx &Ctx)

//	OnSubordinateDestroyed
//
//	One of our guards has been destroyed and we might need to retaliate.
//
//	NOTE: Do not call this if the subordinate is an enemy of this station
//	(that can happen in occupation situations).

	{
	CSpaceObject *pSubordinate = Ctx.pObj;
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
	CSpaceObject *pSubordinate = Ctx.pObj;
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

	int i;
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

	//	Basic update

	UpdateAttacking(Ctx, iTick);
	m_DockingPorts.UpdateAll(Ctx, this);
	UpdateReinforcements(iTick);

	//	Trade

	if ((iTick % TRADE_UPDATE_FREQUENCY) == 0
			 && !IsAbandoned())
		UpdateTrade(Ctx, INVENTORY_REFRESHED_PER_UPDATE);

	//	Update each device

	if (m_pDevices)
		{
		CDeviceClass::SDeviceUpdateCtx DeviceCtx(iTick);
		for (i = 0; i < maxDevices; i++)
			{
			DeviceCtx.ResetOutputs();

			m_pDevices[i].Update(this, DeviceCtx);
			if (DeviceCtx.bSourceDestroyed)
				return;

			//	If the device was repaired or disabled, we need to update

			if (DeviceCtx.bRepaired)
				{
				bCalcDeviceBonus = true;
				}

			if (DeviceCtx.bSetDisabled && m_pDevices[i].SetEnabled(this, false))
				{
				bCalcDeviceBonus = true;
				}
			}
		}

	//	Update destroy animation

	if (m_iDestroyedAnimation)
		{
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

	//	If we're moving, slow down

	if (!IsAnchored() && !GetVel().IsNull())
		{
		//	If we're moving really slowly, force to 0. We do this so that we can optimize calculations
		//	and not have to compute wreck movement down to infinitesimal distances.

		if (GetVel().Length2() < g_MinSpeed2)
			SetVel(NullVector);
		else
			SetVel(CVector(GetVel().GetX() * g_SpaceDragFactor, GetVel().GetY() * g_SpaceDragFactor));
		}

	//	Overlays

	if (!m_Overlays.IsEmpty())
		{
		bool bModified;

		const CObjectImageArray &Image = GetImage(true);

		m_Overlays.Update(this, Image.GetImageViewportSize(), GetRotation(), &bModified);
		if (CSpaceObject::IsDestroyedInUpdate())
			return;
		else if (bModified)
			{
			bCalcBounds = true;
			bCalcDeviceBonus = true;
			}
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
//
//	CIntegralRotation	m_pRotation
//
//	DWORD		No of weapon targets (only if armed)
//	DWORD		target (CSpaceObject ref)

	{
	int i;
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
		//	LATER: COrbit should have a save method
		pStream->Write((char *)m_pMapOrbit, sizeof(COrbit));
		}
	else
		{
		dwSave = 0xffffffff;
		pStream->Write(dwSave);
		}

	pStream->Write(CMapLabelArranger::SavePosition(m_iMapLabelPos));
	pStream->Write(m_rParallaxDist);
	m_sStargateDestNode.WriteToStream(pStream);
	m_sStargateDestEntryPoint.WriteToStream(pStream);

	m_Hull.WriteToStream(*pStream, this);

	dwSave = (m_pDevices ? maxDevices : 0);
	pStream->Write(dwSave);

	for (i = 0; i < (int)dwSave; i++)
		m_pDevices[i].WriteToStream(pStream);

	m_Overlays.WriteToStream(pStream);

	m_DockingPorts.WriteToStream(this, pStream);

	GetSystem()->WriteObjRefToStream(m_pBase, pStream, this);
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
	dwSave |= (m_fNoMapLabel ?			0x00000004 : 0);
	dwSave |= (m_fRadioactive ?			0x00000008 : 0);
	dwSave |= (m_pRotation ?			0x00000010 : 0);
	dwSave |= (m_fActive ?				0x00000020 : 0);
	dwSave |= (m_fNoReinforcements ?	0x00000040 : 0);
	dwSave |= (m_fReconned ?			0x00000080 : 0);
	dwSave |= (m_fFireReconEvent ?		0x00000100 : 0);
	//	0x00000200 retired
	//	0x00000400 retired at 151
	dwSave |= (m_fExplored ?			0x00000800 : 0);
	//	0x00001000
	//	0x00002000
	dwSave |= (m_fNoBlacklist ?			0x00004000 : 0);
	dwSave |= (m_fNoConstruction ?		0x00008000 : 0);
	dwSave |= (m_fBlocksShips ?			0x00010000 : 0);
	dwSave |= (m_fPaintOverhang ?		0x00020000 : 0);
	dwSave |= (m_fShowMapOrbit ?		0x00040000 : 0);
	dwSave |= (m_fDestroyIfEmpty ?		0x00080000 : 0);
	dwSave |= (m_fIsSegment ?		    0x00100000 : 0);
	pStream->Write(dwSave);

	//	Rotation

	if (m_pRotation)
		m_pRotation->WriteToStream(pStream);

	//	Weapon targets

	if (m_fArmed)
		{
		dwSave = m_WeaponTargets.GetCount();
		pStream->Write(dwSave);

		for (i = 0; i < m_WeaponTargets.GetCount(); i++)
			GetSystem()->WriteObjRefToStream(m_WeaponTargets[i], pStream, this);
		}
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

		if (m_fKnown 
				&& m_pType->ShowsMapIcon() 
				&& !m_fNoMapLabel)
			{
			InitMapLabel();

			GetUniverse().GetNamedFont(CUniverse::fontMapLabel).DrawText(Dest, 
					x + m_xMapLabel, 
					y + m_yMapLabel, 
					RGB_LRS_LABEL,
					m_sMapLabel);
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
		//	Paint red if enemy, green otherwise

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
				Dest.DrawDot(x+1, y+1, 0, markerSmallSquare);
				Dest.DrawDot(x, y, rgbColor, markerSmallFilledSquare);
				}
			else
				{
				Dest.DrawDot(x+1, y+1, 0, markerSmallSquare);
				Dest.DrawDot(x, y, rgbColor, markerSmallSquare);
				}
			}
		else if (IsWreck())
			{
			Dest.DrawDot(x, y, 
					(m_fExplored ? CG32bitPixel(128, 128, 128) : rgbColor),
					markerTinyCircle);
			}
		else
			{
			Dest.DrawDot(x, y, 
					rgbColor,
					markerSmallRound);
			}
		}

	DEBUG_CATCH_MSG1("Crash in CStation::PaintLRSForeground: type: %08x", m_pType->GetUNID());
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

	//	Clear cache so we recompute label metrics

	m_sMapLabel = NULL_STR;
	}

void CStation::SetStargate (const CString &sDestNode, const CString &sDestEntryPoint)

//	SetStargate
//
//	Sets the stargate label

	{
	m_sStargateDestNode = sDestNode;
	m_sStargateDestEntryPoint = sDestEntryPoint;
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
	else if (strEquals(sName, PROPERTY_BARRIER))
		{
		m_fBlocksShips = !pValue->IsNil();
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
	else if (strEquals(sName, PROPERTY_PAINT_LAYER))
		{
		if (pValue->IsNil())
			m_fPaintOverhang = false;
		else if (strEquals(pValue->GetStringValue(), PAINT_LAYER_OVERHANG))
			m_fPaintOverhang = true;
		else
			{
			*retsError = strPatternSubst(CONSTLIT("Unable to set paint layer: %s"), pValue->GetStringValue());
			return false;
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
			ClearCondition(CConditionSet::cndRadioactive);
		else
			SetCondition(CConditionSet::cndRadioactive);
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
		m_fNoMapLabel = pValue->IsNil();
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

void CStation::UpdateAttacking (SUpdateCtx &Ctx, int iTick)

//	UpdateAttacking
//
//	Station attacks any enemies in range

	{
	DEBUG_TRY

	int i, j;
	
	//	Update blacklist counter
	//	NOTE: Once the player is blacklisted by this station, there is
	//	no way to get off the blacklist. (At least no automatic way).

	m_Blacklist.Update(iTick);

	//	If we're abandoned or if we have no weapons then
	//	there's nothing we can do

	if (IsAbandoned() || !m_fArmed || m_pType->IsVirtual())
		return;

	//	Compute the range at which we attack enemies

	Metric rAttackRange;
	Metric rAttackRange2;
	if (m_iAngryCounter > 0)
		{
		rAttackRange = Max(MAX_ATTACK_DISTANCE, m_pType->GetMaxEffectiveRange());
		rAttackRange2 = rAttackRange * rAttackRange;

		//	If the player is in range, then she is under attack

		if (Ctx.pPlayer 
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

	if (m_WeaponTargets.GetCount() > 0 
			&& m_pDevices
			&& !IsParalyzed()
			&& !IsDisarmed())
		{
		bool bSourceDestroyed = false;

		for (i = 0; i < maxDevices; i++)
			{
			CInstalledDevice *pWeapon = &m_pDevices[i];

			//	Skip non-weapons and weapons that aren't ready.

			if (pWeapon->IsEmpty() 
					|| (pWeapon->GetCategory() != itemcatWeapon && pWeapon->GetCategory() != itemcatLauncher)
					|| !pWeapon->IsReady())
				continue;

			//	Find the best target for this weapon and fire.

			for (j = 0; j < m_WeaponTargets.GetCount(); j++)
				{
				CSpaceObject *pTarget = m_WeaponTargets[j];
				if (pTarget == NULL)
					continue;

				//	See if we can hit this target. If so, then we fire.

				int iFireAngle;
				if (pWeapon->IsWeaponAligned(this, pTarget, NULL, &iFireAngle)
						&& IsLineOfFireClear(pWeapon, pTarget, iFireAngle, rAttackRange))
					{
					pWeapon->SetFireAngle(iFireAngle);
					pWeapon->SetTarget(pTarget);
					pWeapon->Activate(this, pTarget, &bSourceDestroyed);
					if (bSourceDestroyed)
						return;

					pWeapon->SetTimeUntilReady(m_pType->GetFireRateAdj() * pWeapon->GetActivateDelay(this) / 10);

					//	Found a target, so stop looking

					break;
					}
				}
			}
		}

	DEBUG_CATCH
	}

void CStation::UpdateReinforcements (int iTick)

//	UpdateReinforcements
//
//	Check to see if it is time to send reinforcements to the station

	{
	DEBUG_TRY

	//	Nothing to do if we're abandoned

	if (IsAbandoned())
		return;

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

	if ((iTick % STATION_REINFORCEMENT_FREQUENCY) == 0
			&& !m_fNoReinforcements
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
	CPerceptionCalc Perception(GetPerception());
	Metric rAttackRange2 = rAttackRange * rAttackRange;

	//	We start by creating a list of all targets within the attack range.

	m_WeaponTargets.DeleteAll();

	//	If the player is blacklisted, then we add her first.

	if (m_Blacklist.IsBlacklisted())
		{
		CSpaceObject *pPlayerShip = GetUniverse().GetPlayerShip();
		Metric rDist2;
		if (pPlayerShip
				&& (rDist2 = GetDistance2(pPlayerShip)) <= rAttackRange2
				&& Perception.CanBeTargeted(pPlayerShip, rDist2))
			m_WeaponTargets.Insert(pPlayerShip);
		}

	//	Add the list of enemy objects in range. This list will come back in
	//	sorted order, with the nearest enemies first.

	int MAX_ENEMIES = 10;
	GetNearestVisibleEnemies(MAX_ENEMIES, rAttackRange, &m_WeaponTargets, NULL, FLAG_INCLUDE_NON_AGGRESSORS);
	}
