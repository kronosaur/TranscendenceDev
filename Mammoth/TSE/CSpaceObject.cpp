//	CSpaceObject.cpp
//
//	CSpaceObject class

#include "PreComp.h"

#ifdef DEBUG_ON_CREATE_TIME
#include <stdio.h>
static DWORD g_dwTotalTime = 0;
#endif

#define MAX_DELTA								(2.0 * g_KlicksPerPixel)
#define MAX_DELTA2								(MAX_DELTA * MAX_DELTA)
#define MAX_DELTA_VEL							(g_KlicksPerPixel / 2.0)
#define MAX_DELTA_VEL2							(MAX_DELTA_VEL * MAX_DELTA_VEL)

const int ITEM_UPDATE_CYCLE =					30;
const int HIGHLIGHT_TIMER =						200;
const int HIGHLIGHT_BLINK =						110;
const int HIGHLIGHT_FADE =						30;

const int ANNOTATION_INNER_SPACING_Y =			2;

const int DAMAGE_BAR_WIDTH =					100;
const int DAMAGE_BAR_HEIGHT =					12;

const Metric g_rMaxCommsRange =					(LIGHT_MINUTE * 60.0);
const Metric g_rMaxCommsRange2 =				(g_rMaxCommsRange * g_rMaxCommsRange);

#define BOUNDS_CHECK_DIST 						(256.0 * g_KlicksPerPixel)
#define BOUNDS_CHECK_DIST2						(BOUNDS_CHECK_DIST * BOUNDS_CHECK_DIST)

#define HIGHLIGHT_CORNER_WIDTH					8
#define HIGHLIGHT_CORNER_HEIGHT					8

#define STR_UNCHARTED							CONSTLIT("uncharted")

#define CAN_DOCK_AS_PLAYER_EVENT				CONSTLIT("CanDockAsPlayer")
#define CAN_INSTALL_ITEM_EVENT					CONSTLIT("CanInstallItem")
#define GET_DOCK_SCREEN_EVENT					CONSTLIT("GetDockScreen")
#define GET_EXPLOSION_TYPE_EVENT				CONSTLIT("GetExplosionType")
#define GET_PLAYER_PRICE_ADJ_EVENT				CONSTLIT("GetPlayerPriceAdj")
#define ON_ATTACKED_EVENT						CONSTLIT("OnAttacked")
#define ON_ATTACKED_BY_PLAYER_EVENT				CONSTLIT("OnAttackedByPlayer")
#define ON_CREATE_EVENT							CONSTLIT("OnCreate")
#define ON_CREATE_ORDERS_EVENT					CONSTLIT("OnCreateOrders")
#define ON_DAMAGE_EVENT							CONSTLIT("OnDamage")
#define ON_DATA_TRANSFER_EVENT                  CONSTLIT("OnDataTransfer")
#define ON_DESELECTED_EVENT						CONSTLIT("OnDeselected")
#define ON_DESTROY_EVENT						CONSTLIT("OnDestroy")
#define ON_DESTROY_CHECK_EVENT					CONSTLIT("OnDestroyCheck")
#define ON_DESTROY_OBJ_EVENT					CONSTLIT("OnDestroyObj")
#define ON_DOCK_OBJ_ADJ_EVENT					CONSTLIT("OnDockObjAdj")
#define ON_DOCK_OBJ_DESTROYED_EVENT				CONSTLIT("OnDockObjDestroyed")
#define ON_ENTERED_GATE_EVENT					CONSTLIT("OnEnteredGate")
#define ON_ENTERED_SYSTEM_EVENT					CONSTLIT("OnEnteredSystem")
#define ON_LOAD_EVENT							CONSTLIT("OnLoad")
#define ON_MINING_EVENT							CONSTLIT("OnMining")
#define ON_OBJ_BLACKLISTED_PLAYER_EVENT			CONSTLIT("OnObjBlacklistedPlayer")
#define ON_OBJ_DESTROYED_EVENT					CONSTLIT("OnObjDestroyed")
#define ON_OBJ_DOCKED_EVENT						CONSTLIT("OnObjDocked")
#define ON_OBJ_ENTERED_GATE_EVENT				CONSTLIT("OnObjEnteredGate")
#define ON_OBJ_GATE_EVENT						CONSTLIT("OnObjGate")
#define ON_OBJ_GATE_CHECK_EVENT					CONSTLIT("OnObjGateCheck")
#define ON_OBJ_JUMPED_EVENT						CONSTLIT("OnObjJumped")
#define ON_OBJ_JUMP_POS_ADJ_EVENT				CONSTLIT("OnObjJumpPosAdj")
#define ON_OBJ_RECONNED_EVENT					CONSTLIT("OnObjReconned")
#define ON_ORDER_CHANGED_EVENT					CONSTLIT("OnOrderChanged")
#define ON_ORDERS_COMPLETED_EVENT				CONSTLIT("OnOrdersCompleted")
#define ON_OVERRIDE_INIT_EVENT					CONSTLIT("OnEventHandlerInit")
#define ON_OVERRIDE_TERM_EVENT					CONSTLIT("OnEventHandlerTerm")
#define ON_MISSION_ACCEPTED_EVENT				CONSTLIT("OnMissionAccepted")
#define ON_MISSION_COMPLETED_EVENT				CONSTLIT("OnMissionCompleted")
#define ON_PLAYER_BLACKLISTED_EVENT				CONSTLIT("OnPlayerBlacklisted")
#define ON_PLAYER_ENTERED_SHIP_EVENT			CONSTLIT("OnPlayerEnteredShip")
#define ON_PLAYER_ENTERED_SYSTEM_EVENT			CONSTLIT("OnPlayerEnteredSystem")
#define ON_PLAYER_LEFT_SHIP_EVENT				CONSTLIT("OnPlayerLeftShip")
#define ON_PLAYER_LEFT_SYSTEM_EVENT				CONSTLIT("OnPlayerLeftSystem")
#define ON_RANDOM_ENCOUNTER_EVENT				CONSTLIT("OnRandomEncounter")
#define ON_SELECTED_EVENT						CONSTLIT("OnSelected")
#define ON_SUBORDINATE_ATTACKED_EVENT			CONSTLIT("OnSubordinateAttacked")
#define ON_SYSTEM_EXPLOSION_EVENT				CONSTLIT("OnSystemExplosion")
#define ON_SYSTEM_OBJ_DESTROYED_EVENT			CONSTLIT("OnSystemObjDestroyed")
#define ON_SYSTEM_STARTED_EVENT					CONSTLIT("OnSystemStarted")
#define ON_SYSTEM_STOPPED_EVENT					CONSTLIT("OnSystemStopped")
#define ON_SYSTEM_WEAPON_FIRE_EVENT				CONSTLIT("OnSystemWeaponFire")
#define ON_TRANSLATE_MESSAGE_EVENT				CONSTLIT("OnTranslateMessage")
#define ON_UPDATE_EVENT							CONSTLIT("OnUpdate")

#define FIELD_ARMOR_INTEGRITY					CONSTLIT("armorIntegrity")
#define FIELD_HULL_INTEGRITY					CONSTLIT("hullIntegrity")
#define FIELD_OBJ_ID							CONSTLIT("objID")
#define FIELD_POS								CONSTLIT("pos")
#define FIELD_SHIELD_LEVEL						CONSTLIT("shieldLevel")
#define FIELD_STATUS							CONSTLIT("status")

#define LANGID_DOCKING_REQUEST_DENIED			CONSTLIT("core.dockingRequestDenied")

#define ORDER_DOCKED							CONSTLIT("docked")

#define PROPERTY_ASCENDED						CONSTLIT("ascended")
#define PROPERTY_CATEGORY						CONSTLIT("category")
#define PROPERTY_COMMS_KEY						CONSTLIT("commsKey")
#define PROPERTY_CURRENCY						CONSTLIT("currency")
#define PROPERTY_CURRENCY_NAME					CONSTLIT("currencyName")
#define PROPERTY_CYBER_DEFENSE_LEVEL			CONSTLIT("cyberDefenseLevel")
#define PROPERTY_DAMAGE_DESC					CONSTLIT("damageDesc")
#define PROPERTY_DESTINY						CONSTLIT("destiny")
#define PROPERTY_DOCKING_PORTS					CONSTLIT("dockingPorts")
#define PROPERTY_EVENT_SUBSCRIBERS				CONSTLIT("eventSubscribers")
#define PROPERTY_HAS_DOCKING_PORTS				CONSTLIT("hasDockingPorts")
#define PROPERTY_ID								CONSTLIT("id")
#define PROPERTY_IDENTIFIED						CONSTLIT("identified")
#define PROPERTY_INSTALL_ARMOR_MAX_LEVEL		CONSTLIT("installArmorMaxLevel")
#define PROPERTY_INSTALL_DEVICE_MAX_LEVEL		CONSTLIT("installDeviceMaxLevel")
#define PROPERTY_INSTALL_DEVICE_UPGRADE_ONLY	CONSTLIT("installDeviceUpgradeOnly")
#define PROPERTY_KNOWN							CONSTLIT("known")
#define PROPERTY_LEVEL							CONSTLIT("level")
#define PROPERTY_MASS							CONSTLIT("mass")
#define PROPERTY_NAME_PATTERN					CONSTLIT("namePattern")
#define PROPERTY_PAINT_LAYER					CONSTLIT("paintLayer")
#define PROPERTY_PLAYER_MISSIONS_GIVEN			CONSTLIT("playerMissionsGiven")
#define PROPERTY_RADIOACTIVE					CONSTLIT("radioactive")
#define PROPERTY_REFUEL_MAX_LEVEL				CONSTLIT("refuelMaxLevel")
#define PROPERTY_REMOVE_DEVICE_MAX_LEVEL		CONSTLIT("removeDeviceMaxLevel")
#define PROPERTY_REPAIR_ARMOR_MAX_LEVEL			CONSTLIT("repairArmorMaxLevel")
#define PROPERTY_SCALE							CONSTLIT("scale")
#define PROPERTY_SHOW_AS_DESTINATION			CONSTLIT("showAsDestination")
#define PROPERTY_SIZE_PIXELS					CONSTLIT("sizePixels")
#define PROPERTY_SOVEREIGN						CONSTLIT("sovereign")
#define PROPERTY_STEALTH						CONSTLIT("stealth")
#define PROPERTY_SUSPENDED						CONSTLIT("suspended")
#define PROPERTY_TYPE							CONSTLIT("type")
#define PROPERTY_UNDER_ATTACK					CONSTLIT("underAttack")

#define SPECIAL_CHARACTER						CONSTLIT("character:")
#define SPECIAL_DATA							CONSTLIT("data:")
#define SPECIAL_IS_PLANET						CONSTLIT("isPlanet:")
#define SPECIAL_LOCATION						CONSTLIT("location:")
#define SPECIAL_PROPERTY						CONSTLIT("property:")
#define SPECIAL_UNID							CONSTLIT("unid:")

#define SPECIAL_VALUE_TRUE						CONSTLIT("true")

#define CATEGORY_BEAM							CONSTLIT("beam")
#define CATEGORY_EFFECT							CONSTLIT("effect")
#define CATEGORY_MARKER							CONSTLIT("marker")
#define CATEGORY_MISSILE						CONSTLIT("missile")
#define CATEGORY_MISSION						CONSTLIT("mission")
#define CATEGORY_SHIP							CONSTLIT("ship")
#define CATEGORY_STATION						CONSTLIT("station")

#define SCALE_STAR								CONSTLIT("star")
#define SCALE_WORLD								CONSTLIT("world")
#define SCALE_STATION							CONSTLIT("station")
#define SCALE_SHIP								CONSTLIT("ship")
#define SCALE_FLOTSAM							CONSTLIT("flotsam")

static Metric g_rMaxPerceptionRange[CSpaceObject::perceptMax+1] =
	{
	0.0,
	0.0,
	0.0,
	0.0,
	0.0,
	0.0,
	0.0,
	0.0,
	0.0,
	0.0,
	0.0,
	0.0,
	0.0,
	0.0,
	0.0,
	0.0,
	};

struct SInstallItemResultsData
	{
	char *pszID;
	int iArmorCompatibleID;
	int iDeviceCompatibleID;
	};

SInstallItemResultsData INSTALL_ITEM_RESULTS_TABLE[] =
	{
		{	"ok",						0,		0,	},

		{	"armorTooHeavy",			1,		-1,	},
		{	"cannotInstall",			-1,		-1,	},
		{	"noDeviceSlotsLeft",		-1,		2	},
		{	"noNonWeaponSlotsLeft",		-1,		13	},
		{	"noWeaponSlotsLeft",		-1,		12	},
		{	"notInstallable",			-1,		1	},
		{	"reactorIncompatible",		-1,		11	},
		{	"reactorOverload",			-1,		-1	},
		{	"reactorTooWeak",			-1,		7	},
		{	"notCompatible",			-1,		-1	},
		{	"tooMuchCargo",				-1,		-1	},

		{	"replacementRequired",		-1,		8	},
		{	"replacementRequired",		-1,		5	},
		{	"replacementRequired",		-1,		6	},
		{	"replacementRequired",		-1,		14	},
		{	"replacementRequired",		-1,		9	},
		{	"replacementRequired",		-1,		4	},
	};

CSpaceObject *CSpaceObject::m_pObjInUpdate = NULL;
bool CSpaceObject::m_bObjDestroyed = false;

CString ParseParam (char **ioPos);

CSpaceObject::CSpaceObject (CUniverse &Universe) : 
		m_Universe(Universe),
		m_pSystem(NULL),
		m_iIndex(-1),
		m_rBoundsX(0.0),
		m_rBoundsY(0.0),

		m_iHighlightCountdown(0),
		m_iHighlightChar(0),
		m_iDesiredHighlightChar(0),

		m_pFirstEffect(NULL),
		m_pOverride(NULL),
		m_pFirstJoint(NULL),

		m_iControlsFrozen(0),
		m_iSpare(0),

		m_fHookObjectDestruction(false),
		m_fNoObjectDestructionNotify(false),
		m_fCannotBeHit(false),
		m_fSelected(false),
		m_fInPOVLRS(false),
		m_fCanBounce(false),
		m_fIsBarrier(false),

		m_fNoFriendlyFire(false),
		m_fTimeStop(false),
		m_fPlayerTarget(false),
		m_fHasOnObjDockedEvent(false),
		m_fOnCreateCalled(false),
		m_fNoFriendlyTarget(false),
		m_fItemEventsValid(false),

		m_fHasOnDamageEvent(false),
		m_fHasOnAttackedEvent(false),
		m_fInDamage(false),
		m_fDestroyed(false),
		m_fPlayerDestination(false),
		m_fShowDistanceAndBearing(false),
		m_fHasInterSystemEvent(false),
		m_fAutoClearDestination(false),
		m_fHasOnOrdersCompletedEvent(false),
		m_fPlayerDocked(false),
		m_fPaintNeeded(false),
		m_fNonLinearMove(false),
		m_fHasName(false),
		m_fAscended(false),
		m_fOutOfPlaneObj(false),
		m_fPainted(false),
		m_fAutoClearDestinationOnDock(false),
		m_fAutoClearDestinationOnDestroy(false),
		m_fShowHighlight(false),
		m_fShowDamageBar(false),
		m_fHasGravity(false),
		m_fInsideBarrier(false),
		m_fHasOnSubordinateAttackedEvent(false),
		m_fHasGetDockScreenEvent(false),
		m_fHasOnAttackedByPlayerEvent(false),
		m_fHasOnOrderChangedEvent(false),
		m_fManualAnchor(false),
		m_fCollisionTestNeeded(false),
		m_fHasDockScreenMaybe(false),
		m_fAutoClearDestinationOnGate(false)

//	CSpaceObject constructor

	{
	m_iDestiny = mathRandom(0, g_DestinyRange - 1);
	m_dwID = Universe.CreateGlobalID();
	}

CSpaceObject::~CSpaceObject (void)

//	CSpaceObject destructor

	{
	//	Can't turn this on until system destroys spaces objects
	//	explicitly.
#if 0
	ASSERT(m_pSystem == NULL);
#endif

	//	Delete the list of effects

	SEffectNode *pNext = m_pFirstEffect;
	while (pNext)
		{
		SEffectNode *pDelete = pNext;
		pNext = pNext->pNext;

		pDelete->pPainter->Delete();
		delete pDelete;
		}

#ifdef DEBUG_ENEMY_CACHE_BUG
	for (int i = 0; i < GetUniverse().GetSovereignCount(); i++)
		GetUniverse().GetSovereign(i)->DebugObjDeleted(this);

	CSystem *pSystem = GetUniverse().GetCurrentSystem();
	if (pSystem)
		pSystem->GetObjectGrid().DebugObjDeleted(this);
#endif

#ifdef DEBUG_OBJ_REFERENCES
	//	Make sure the object is not being held by anyone else

	if (GetUniverse().GetPOV())
		{
		CSystem *pSystem = GetUniverse().GetPOV()->GetSystem();
		for (int i = 0; i < pSystem->GetObjectCount(); i++)
			{
			CSpaceObject *pObj = pSystem->GetObject(i);
			if (pObj)
				{
				if (pObj->m_Data.FindObjRefData(this))
					ASSERT(false);
				}
			}
		}
#endif
	}

void CSpaceObject::Accelerate (const CVector &vPush, Metric rSeconds)

//	Accelerate
//
//	Accelerates the given object along the given vector. The magnitude of
//	the vector is the force used in gigaNewtons(!). The acceleration is
//	maintained for rSeconds

	{
	Metric rMass = GetMass();

	if (rMass)
		{
		//	rAccel needs to be in klicks per second (we assume here
		//	that 1 klick = 1,000 meters).
		CVector rAccel = (vPush * 1000.0) / rMass;
		m_vVel = m_vVel + (rAccel * rSeconds);
		}
	}

void CSpaceObject::AccelerateStop (Metric rPush, Metric rSeconds)

//	AccelerateStop
//
//	Slows down the object with the given thrust

	{
	Metric rMass = GetMass();

	if (rMass)
		{
		Metric rAccel = rPush * 1000.0 / rMass;

		Metric rLength;
		CVector vDir = m_vVel.Normal(&rLength);

		if (rAccel > rLength)
			m_vVel = NullVector;
		else
			m_vVel = m_vVel - (vDir * rAccel);
		}
	}

void CSpaceObject::AddEffect (IEffectPainter *pPainter, const CVector &vPos, int iTick, int iRotation)

//	AddEffect
//
//	Adds an effect to the object

	{
	int xOffset = mathRound((vPos.GetX() - m_vPos.GetX()) / g_KlicksPerPixel);
	int yOffset = mathRound((m_vPos.GetY() - vPos.GetY()) / g_KlicksPerPixel);

	AddEffect(pPainter, xOffset, yOffset, iTick, iRotation);
	}

void CSpaceObject::AddEffect (IEffectPainter *pPainter, int xOffset, int yOffset, int iTick, int iRotation)

//	AddEffect
//
//	Adds an effect to the object

	{
	ASSERT(pPainter->GetCreator()->IsValidUNID());

	SEffectNode *pNewNode = new SEffectNode;
	pNewNode->pPainter = pPainter;
	pNewNode->xOffset = xOffset;
	pNewNode->yOffset = yOffset;
	pNewNode->iTick = iTick;
	pNewNode->iRotation = iRotation;
	pNewNode->pNext = m_pFirstEffect;

	m_pFirstEffect = pNewNode;
	}

void CSpaceObject::AddEventSubscriber (CSpaceObject *pObj)

//	AddEventSubscriber
//
//	Adds an object that wants to subscribe to our events
	
	{
	if (pObj 
			&& !pObj->IsDestroyed()
			&& pObj->NotifyOthersWhenDestroyed())
		m_SubscribedObjs.Add(pObj); 
	}

EnhanceItemStatus CSpaceObject::AddItemEnhancement (const CItem &itemToEnhance, 
													CItemType *pEnhancement, 
													int iLifetime, 
													DWORD *retdwID)

//	AddItemEnhancement
//
//	Adds an enhancement to the given item

	{
	//	Select the item

	CItemListManipulator ItemList(GetItemList());
	if (!ItemList.SetCursorAtItem(itemToEnhance))
		{
		if (retdwID)
			*retdwID = OBJID_NULL;

		return eisNoEffect;
		}

	//	Add the enhancement

	return AddItemEnhancement(ItemList, pEnhancement, iLifetime, retdwID);
	}

EnhanceItemStatus CSpaceObject::AddItemEnhancement (CItemListManipulator &ItemList, 
													CItemType *pEnhancement, 
													int iLifetime, 
													DWORD *retdwID)

//	AddItemEnhancement
//
//	Adds an enhancement to the given item
//
//	LATER: This should be replaced with EnhanceItem(), which is a superset.

	{
	//	Pre-init in case we exit early

	if (retdwID)
		*retdwID = OBJID_NULL;

	//	Determine the mod code of the new enhancement

	DWORD dwNewModCode = pEnhancement->GetModCode();

	//	For now we always replace any existing enhancements
	//	But we compute whether we repair or replace any existing enhancements

	EnhanceItemStatus iResult;
	const CItemEnhancement &OldEnhancement = ItemList.GetItemAtCursor().GetMods();
	if (OldEnhancement.IsEmpty())
		iResult = eisOK;
	else if (OldEnhancement.IsDisadvantage())
		iResult = eisRepaired;
	else if (OldEnhancement.GetModCode() == dwNewModCode)
		iResult = eisOK;
	else
		iResult = eisEnhancementReplaced;

	//	Compute expire time

	int iExpireTime = (iLifetime != -1 ? GetUniverse().GetTicks() + iLifetime : -1);

	//	Add the item enhancement

	CItemEnhancement NewEnhancement;
	NewEnhancement.SetEnhancementType(pEnhancement);
	NewEnhancement.SetModCode(dwNewModCode);
	NewEnhancement.SetExpireTime(iExpireTime);
	DWORD dwID = ItemList.AddItemEnhancementAtCursor(NewEnhancement);

	//	Give the object a chance to respond to the enhancement

	ItemEnhancementModified(ItemList);

	//	Fire On event to the enhancement

	if (ItemList.IsCursorValid())
		{
		CItem theEnhancement(pEnhancement, 1);
		theEnhancement.FireOnAddedAsEnhancement(this, ItemList.GetItemAtCursor(), iResult);
		}

	//	Done

	if (retdwID)
		*retdwID = dwID;

	return iResult;
	}

void CSpaceObject::AddOverlay (COverlayType *pType, const CVector &vPos, int iRotation, int iPosZ, int iLifetime, DWORD *retdwID)

//	AddOverlay
//
//	Adds an overlay at the given position.

	{
	//	Convert from a hit position to an overlay pos

	int iPosAngle;
	int iPosRadius;
	CalcOverlayPos(pType, vPos, &iPosAngle, &iPosRadius);
			
	//	Add the overlay

	AddOverlay(pType, iPosAngle, iPosRadius, iRotation, iPosZ, iLifetime, retdwID);
	}

void CSpaceObject::AddOverlay (DWORD dwUNID, int iPosAngle, int iPosRadius, int iRotation, int iPosZ, int iLifetime, DWORD *retdwID)

//	AddOverlay
//
//	Adds an overly by UNID

	{
	COverlayType *pType = GetUniverse().FindOverlayType(dwUNID);
	if (pType == NULL)
		{
		if (retdwID) *retdwID = 0;
		return;
		}

	AddOverlay(pType, iPosAngle, iPosRadius, iRotation, iPosZ, iLifetime, retdwID);
	}

ALERROR CSpaceObject::AddToSystem (CSystem &System, bool bNoGlobalInsert)

//	AddToSystem
//
//	Adds the object to the system

	{
	ALERROR error;

	//	We can get here with m_pSystem already set during load

	ASSERT(m_pSystem == NULL || m_pSystem == &System);

	//	Clear the destroyed bit

	m_fDestroyed = false;

	//	Add to system

	if (error = System.AddToSystem(this, &m_iIndex))
		return error;

	m_pSystem = &System;

	//	If this is a ship or station then add to the global list

	if (!bNoGlobalInsert)
		System.GetUniverse().GetGlobalObjects().InsertIfTracked(this);

	return NOERROR;
	}

void CSpaceObject::Ascend (void)

//	Ascend
//
//	Ascend out of system so that it can move to a different system.

	{
	//	Set ascended flag so that when we get called at OnObjEnteredGate
	//	we can test for it.

	SetAscended(true);

	//	To everyone else in the system, it looks like the object entered a gate

	EnterGate(NULL, NULL_STR, NULL, true);

	//	Let subclasses handle this

	OnAscended();

	//	Remove the object from the old system

	Remove(ascended, CDamageSource());
	}

void CSpaceObject::CalcInsideBarrier (void)

//	CalcInsideBarrier
//
//	Figures out if we are currently inside a barrier. If so, we set the 
//	m_fInsideBarrier flag.

	{
	int i;

	if (!m_fCanBounce)
		return;

	//	Compute the bounding rect for this object

	CVector vUR, vLL;
	GetBoundingRect(&vUR, &vLL);

	//	Loop over all other objects and see if we are inside a barrier

	CSystem *pSystem = GetSystem();
	for (i = 0; i < pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = pSystem->GetObject(i);
		if (pObj == NULL
				|| pObj == this
				|| pObj->IsDestroyed()
				|| !pObj->CanBlock(this))
			continue;

		//	Compute the bounding rect for the barrier.

		CVector vBarrierUR, vBarrierLL;
		pObj->GetBoundingRect(&vBarrierUR, &vBarrierLL);

		//	If we intersect then we're inside at least one object

		if (IntersectRect(vUR, vLL, vBarrierUR, vBarrierLL)
				&& pObj->ObjectInObject(pObj->GetPos(), this, GetPos()))
			{
			m_fInsideBarrier = true;
			break;
			}
		}
	}

void CSpaceObject::CalcOverlayPos (COverlayType *pOverlayType, const CVector &vPos, int *retiPosAngle, int *retiPosRadius)

//	CalcOverlayPos
//
//	Calculates an overlay position from the given absolute position.

	{
	Metric rRadius;
	int iDirection = VectorToPolar(vPos - GetPos(), &rRadius);
	int iRotationOrigin = ((pOverlayType && pOverlayType->RotatesWithShip()) ? GetRotation() : 0);

	if (retiPosAngle)
		*retiPosAngle = AngleMod(iDirection - iRotationOrigin);

	if (retiPosRadius)
		*retiPosRadius = (int)(rRadius / g_KlicksPerPixel);
	}

int CSpaceObject::CalcFireSolution (CSpaceObject *pTarget, Metric rMissileSpeed) const

//	CalcFireSolution
//
//	Returns the angle to aim at to hit the given target. Or -1 if the target cannot be
//	intercepted.

	{
	CVector vPos = pTarget->GetPos() - GetPos();
	CVector vVel = pTarget->GetVel() - GetVel();

	Metric rTimeToIntercept = CalcInterceptTime(vPos, vVel, rMissileSpeed);
	if (rTimeToIntercept < 0.0)
		return -1;

	//	Compute the intercept point and then the direction

	CVector vInterceptPoint = vPos + vVel * rTimeToIntercept;
	return VectorToPolar(vInterceptPoint);
	}

DWORD CSpaceObject::CalcSRSVisibility (SViewportPaintCtx &Ctx) const

//	CalcSRSVisibility
//
//	Calculates the SRS opacity to paint with.
//
//	0 = Fully visible.
//	1-255 = varying degrees of visibility (1 = lowest, 255 = highest).

	{
	int iRangeIndex = GetDetectionRangeIndex(Ctx.iPerception);
	if (iRangeIndex < 6 || Ctx.pCenter == NULL || Ctx.pCenter == this)
		return 0;

	Metric rRange = CPerceptionCalc::GetRange(iRangeIndex);
	Metric rDist = Ctx.pCenter->GetDistance(const_cast<CSpaceObject *>(this));
	if (rDist <= rRange)
		return 0;

	return 255 - Min(254, (int)((rDist - rRange) / g_KlicksPerPixel) * 2);
	}

CSpaceObject *CSpaceObject::CalcTargetToAttack (CSpaceObject *pAttacker, CSpaceObject *pOrderGiver)

//	CalcTargetToAttack
//
//	Figure out whether to attack the order giver or not. NOTE: We return NULL if
//	neither can be attacked.

	{
	CPerceptionCalc Perception(GetPerception());

	//	Figure out if we can see the attacker

	Metric rAttackerDist2 = g_InfiniteDistance;
	bool bCanSeeAttacker = false;
	if (pAttacker)
		{
		rAttackerDist2 = GetDistance2(pAttacker);
		if (Perception.CanBeTargeted(pAttacker, rAttackerDist2))
			bCanSeeAttacker = true;
		}

	//	Figure out if we can see the order giver

	Metric rOrderGiverDist2 = g_InfiniteDistance;
	bool bCanSeeOrderGiver = false;
	if (pOrderGiver == pAttacker)
		{
		bCanSeeOrderGiver = bCanSeeAttacker;
		rOrderGiverDist2 = rAttackerDist2;
		}
	else if (pOrderGiver)
		{
		rOrderGiverDist2 = GetDistance2(pOrderGiver);
		if (Perception.CanBeTargeted(pOrderGiver, rOrderGiverDist2))
			bCanSeeOrderGiver = true;
		}

	//	Choose whichever is valid and visible.

	if (pAttacker == pOrderGiver)
		return (bCanSeeAttacker ? pAttacker : NULL);

	else if (pAttacker == NULL || !bCanSeeAttacker)
		return (bCanSeeOrderGiver ? pOrderGiver : NULL);

	else if (pOrderGiver == NULL || !bCanSeeOrderGiver)
		return (bCanSeeAttacker ? pAttacker : NULL);

	//	Both attacker and order giver are valid and visible to us. If one is
	//	twice as close to us, we pick that one.

	else if (rAttackerDist2 < rOrderGiverDist2 * 4.0)
		return pAttacker;
	else if (rOrderGiverDist2 < rAttackerDist2 * 4.0)
		return pOrderGiver;

	//	Otherwise, we pick randomly, weighing the order giver a little more.

	else if (mathRandom(1, 100) <= 60)
		return pOrderGiver;
	else
		return pAttacker;
	}

Metric CSpaceObject::CalculateItemMass (Metric *retrCargoMass) const

//	CalculateCargoMass
//
//	Returns the total mass of the items

	{
	int i;
	Metric rTotal = 0.0;
	Metric rTotalCargo = 0.0;

	for (i = 0; i < m_ItemList.GetCount(); i++)
		{
		const CItem &Item = m_ItemList.GetItem(i);

		Metric rMass = Item.GetMass() * Item.GetCount();

		//	All items count towards item mass

		rTotal += rMass;

		//	Only uninstalled items count in cargo space

		if (!Item.IsInstalled())
			rTotalCargo += rMass;
		}

	if (retrCargoMass)
		*retrCargoMass = rTotalCargo;

	return rTotal;
	}

bool CSpaceObject::CanCommunicateWith (CSpaceObject *pSender)

//	CanCommunicateWith
//
//	Returns TRUE if this object can receive communications from pSender

	{
	int i;

	//	We can't communicate if we don't have a handler

	CCommunicationsHandler *pHandler = GetCommsHandler();
	if (pHandler == NULL)
		return false;

	//	We can't communicate if we don't know about the object

	if (!IsKnown())
		return false;

	//	We can't communicate if we're suspended, etc. (But it's OK if we're
	//	virtual.)

	if (IsUnreal())
		return false;

	//	Can't communicate if we're part of a different object

	if (IsAttached())
		return false;

	//	We can't communicate if we are out of range

	if ((pSender->GetPos() - m_vPos).Length2() > g_rMaxCommsRange2)
		return false;

	//	See if any of the messages are valid. If at least
	//	one is, then we can communicate.

	for (i = 0; i < pHandler->GetCount(); i++)
		{
		if (pHandler->GetMessage(i).OnShowEvent.pCode == NULL)
			return true;
		else
			{
			CCodeChainCtx Ctx(GetUniverse());

			//	Define parameters

			Ctx.DefineContainingType(this);
			Ctx.SaveAndDefineSourceVar(this);
			Ctx.DefineSpaceObject(CONSTLIT("gSender"), pSender);

			//	Execute

			bool bShow;

			ICCItem *pResult = Ctx.Run(pHandler->GetMessage(i).OnShowEvent);

			if (pResult->IsNil())
				bShow = false;
			else if (pResult->IsError())
				{
				pSender->SendMessage(this, pResult->GetStringValue());
				bShow = false;
				}
			else
				bShow = true;

			Ctx.Discard(pResult);
			if (bShow)
				return bShow;
			}
		}

	return false;
	}

bool CSpaceObject::CanDetect (int iPerception, CSpaceObject *pObj)

//	CanDetect
//
//	Returns TRUE if this object (with given perception) can detect the target

	{
	CVector vDist = pObj->GetPos() - GetPos();
	return (vDist.Length2() < pObj->GetDetectionRange2(iPerception));
	}

bool CSpaceObject::CanFireOnObjHelper (CSpaceObject *pObj)

//	CanFireOnObjHelper
//
//	Return TRUE if a missile fired by this object can hit the given object

	{
	return (
		//	We cannot hit our friends (if our source can't)
		(CanHitFriends() && pObj->CanBeHitByFriends()) || GetSovereign() == NULL || !GetSovereign()->IsFriend(pObj->GetSovereign())
		);
	}

bool CSpaceObject::CanInstallItem (const CItem &Item, int iSlot, InstallItemResults *retiResult, CString *retsResult, CItem *retItemToReplace)

//	CanInstallItem
//
//	Must be overridden by subclasses.

	{
	if (retiResult)
		*retiResult = insCannotInstall;

	if (retsResult)
		*retsResult = CONSTLIT("Item installation not supported.");

	return false;
	}

void CSpaceObject::ClearCondition (CConditionSet::ETypes iCondition, DWORD dwFlags)

//	ClearCondition
//
//	Clears the given condition (generically).

	{
	switch (iCondition)
		{
		case CConditionSet::cndTimeStopped:
			m_fTimeStop = false;
			break;

		default:
			OnClearCondition(iCondition, dwFlags);
			break;
		}
	}

void CSpaceObject::CommsMessageFrom (CSpaceObject *pSender, int iIndex)

//	CommsMessageFrom
//
//	Handle comms message from the sender

	{
	CCommunicationsHandler *pHandler = GetCommsHandler();
	ASSERT(pHandler && iIndex < pHandler->GetCount());
	const CCommunicationsHandler::SMessage &Msg = pHandler->GetMessage(iIndex);

	if (Msg.InvokeEvent.pCode)
		{
		CCodeChainCtx Ctx(GetUniverse());

		//	Define parameters

		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
		Ctx.DefineSpaceObject(CONSTLIT("gSender"), pSender);

		//	Execute

		ICCItem *pResult = Ctx.Run(Msg.InvokeEvent);
		if (pResult->IsError())
			pSender->SendMessage(this, pResult->GetStringValue());

		Ctx.Discard(pResult);
		}
	}

int CSpaceObject::ConvertToCompatibleIndex (const CItem &Item, InstallItemResults iResult)

//	ConvertToCompatibleIndex
//
//	Converts a result to the compatible ID number

	{
	if (iResult < 0 || iResult >= insInstallItemResultsCount)
		{
		ASSERT(false);
		return -1;
		}

	if (Item.IsArmor())
		return INSTALL_ITEM_RESULTS_TABLE[iResult].iArmorCompatibleID;
	else
		return INSTALL_ITEM_RESULTS_TABLE[iResult].iDeviceCompatibleID;
	}

CString CSpaceObject::ConvertToID (InstallItemResults iResult)

//	ConvertToID
//
//	Converts a result to its ID

	{
	if (iResult < 0 || iResult >= insInstallItemResultsCount)
		{
		ASSERT(false);
		return NULL_STR;
		}

	return CString(INSTALL_ITEM_RESULTS_TABLE[iResult].pszID);
	}

void CSpaceObject::CopyDataFromObj (CSpaceObject *pSource)

//	CopyDataFromObj
//
//	Copies data from the source object (this is used when we
//	change ships)

	{
    int i;

    //  Ask our object for a list of data fields that we DO NOT want to copy.
    //  We only ask our class because it should be the only thing that stores
    //  per-ship data. All other uses should store data in other places, such
    //  as overlays, items, etc.
    //
    //  [The other exception is non-player ships, which often have event
    //  handlers that store state, but those are not player ships.]

	SEventHandlerDesc Event;
    TSortMap<CString, CAttributeDataBlock::STransferDesc> Options;
	if (FindEventHandler(ON_DATA_TRANSFER_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());

		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_DATA_TRANSFER_EVENT, pResult);
        else if (pResult->IsSymbolTable())
            {
            for (i = 0; i < pResult->GetCount(); i++)
                {
                CAttributeDataBlock::STransferDesc *pDesc = Options.SetAt(pResult->GetKey(i));
                CString sOption = pResult->GetElement(i)->GetStringValue();
                if (strEquals(sOption, CONSTLIT("ignore")))
                    pDesc->iOption = CAttributeDataBlock::transIgnore;
                else if (strEquals(sOption, CONSTLIT("copy")))
                    pDesc->iOption = CAttributeDataBlock::transCopy;
                else
                	kernelDebugLogPattern("Unknown transfer option: %s", sOption);
                }
            }

		Ctx.Discard(pResult);
		}

    //  Copy

    m_Data.Copy(pSource->m_Data, Options);
	}

CSpaceObject *CSpaceObject::CreateFromClassID (CUniverse &Universe, DWORD dwClass)

//	CreateFromClassID
//
//	Creates a new object of the specified class.

	{
	switch (dwClass)
		{
		case CAreaDamage::ClassID():
			return new CAreaDamage(Universe);

		case CBeam::ClassID():
			return new CBeam(Universe);

		case CBoundaryMarker::ClassID():
			return new CBoundaryMarker(Universe);

		case CContinuousBeam::ClassID():
			return new CContinuousBeam(Universe);

		case CDisintegrationEffect::ClassID():
			return new CDisintegrationEffect(Universe);

		case CEffect::ClassID():
			return new CEffect(Universe);

		case CFractureEffect::ClassID():
			return new CFractureEffect(Universe);

		case CMarker::ClassID():
			return new CMarker(Universe);

		case CMissile::ClassID():
			return new CMissile(Universe);

		case CMission::ClassID():
			return new CMission(Universe);

		case CParticleDamage::ClassID():
			return new CParticleDamage(Universe);

		case CParticleEffect::ClassID():
			return new CParticleEffect(Universe);

		case CPOVMarker::ClassID():
			return new CPOVMarker(Universe);

		case CRadiusDamage::ClassID():
			return new CRadiusDamage(Universe);

		case CSequencerEffect::ClassID():
			return new CSequencerEffect(Universe);

		case CShip::ClassID():
			return new CShip(Universe);

		case CStaticEffect::ClassID():
			return new CStaticEffect(Universe);

		case CStation::ClassID():
			return new CStation(Universe);
			
		default:
			throw CException(ERR_FAIL, strPatternSubst(CONSTLIT("Invalid CSpaceObject class: %08x"), dwClass));
		}
	}

void CSpaceObject::CreateFromStream (SLoadCtx &Ctx, CSpaceObject **retpObj)

//	CreateFromStream
//
//	Creates an object from the stream
//
//	DWORD		ObjID
//	DWORD		m_iIndex
//	DWORD		m_dwID
//	DWORD		m_iDestiny
//	Vector		m_vPos
//	Vector		m_vVel
//	Metric		m_rBoundsX
//	Metric		m_rBoundsY
//	DWORD		low = m_iDesiredHighlightChar; hi = m_iHighlightCountdown
//	DWORD		m_pOverride
//	CItemList	m_ItemList
//	DWORD		m_iControlsFrozen
//	DWORD		flags
//	CAttributeDataBlock	m_Data
//	CVector		m_vOldPos (only if !IsAnchored())
//
//	For each effect:
//	IEffectPainter (0 == no more)
//	DWORD		x
//	DWORD		y
//	DWORD		iTick
//	DWORD		iRotation

	{
	ELoadStates iOldLoadState = Ctx.iLoadState;

	//	Create the object

	DWORD dwLoad;
	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	CSpaceObject *pObj = CreateFromClassID(Ctx.GetUniverse(), dwLoad);

	//	Remember the type of object that we're loading (in case of crash)

	Ctx.iLoadState = loadStateObject;
	Ctx.dwObjClassID = dwLoad;

	//	Load the index. This will not be the final index (because the
	//	index will change relative to the new system). But this is the
	//	index that other objects will refer to during load.

	Ctx.pStream->Read((char *)&pObj->m_iIndex, sizeof(DWORD));

	//	Load the global ID

	if (Ctx.dwVersion >= 13)
		Ctx.pStream->Read((char *)&pObj->m_dwID, sizeof(DWORD));
	else
		pObj->m_dwID = Ctx.GetUniverse().CreateGlobalID();

	//	Set the system as soon as possible because we rely on it during loading

	pObj->m_pSystem = Ctx.pSystem;

	//	Load other stuff

	Ctx.pStream->Read((char *)&pObj->m_iDestiny, sizeof(DWORD));
	Ctx.pStream->Read((char *)&pObj->m_vPos, sizeof(CVector));
	Ctx.pStream->Read((char *)&pObj->m_vVel, sizeof(CVector));
	Ctx.pStream->Read((char *)&pObj->m_rBoundsX, sizeof(Metric));
	Ctx.pStream->Read((char *)&pObj->m_rBoundsY, sizeof(Metric));
	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	if (Ctx.dwVersion >= 99)
		pObj->m_iDesiredHighlightChar = LOWORD(dwLoad);
	else
		pObj->m_iDesiredHighlightChar = 0;
	pObj->m_iHighlightCountdown = HIWORD(dwLoad);

	//	Override

	if (Ctx.dwVersion >= 48
			&& Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD)) == NOERROR
			&& dwLoad != 0)
		pObj->m_pOverride = Ctx.GetUniverse().FindDesignType(dwLoad);
	else
		pObj->m_pOverride = NULL;

	//	Item List

	pObj->m_ItemList.ReadFromStream(Ctx);

	//	Load other stuff

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	pObj->m_iControlsFrozen = dwLoad;

	//	Load flags

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	pObj->m_fHookObjectDestruction =	((dwLoad & 0x00000001) ? true : false);
	pObj->m_fNoObjectDestructionNotify = ((dwLoad & 0x00000002) ? true : false);
	pObj->m_fCannotBeHit =				((dwLoad & 0x00000004) ? true : false);
	pObj->m_fSelected =					((dwLoad & 0x00000008) ? true : false);
	pObj->m_fInPOVLRS =					((dwLoad & 0x00000010) ? true : false);
	pObj->m_fCanBounce =				((dwLoad & 0x00000020) ? true : false);
	pObj->m_fIsBarrier =				((dwLoad & 0x00000040) ? true : false);
	bool bSavedOldPos =					((dwLoad & 0x00000080) ? false : true);
	pObj->m_fNoFriendlyFire =			((dwLoad & 0x00000100) ? true : false);
	pObj->m_fTimeStop =					((dwLoad & 0x00000200) ? true : false);
	pObj->m_fPlayerTarget =				((dwLoad & 0x00000400) ? true : false);
	pObj->m_fAutoClearDestinationOnGate = (((dwLoad & 0x00000800) ? true : false) && Ctx.dwVersion >= 158);
	pObj->m_fNoFriendlyTarget =			((dwLoad & 0x00001000) ? true : false);
	pObj->m_fPlayerDestination =		((dwLoad & 0x00002000) ? true : false);
	pObj->m_fShowDistanceAndBearing =	((dwLoad & 0x00004000) ? true : false);
	pObj->m_fHasOnObjDockedEvent =		((dwLoad & 0x00008000) ? true : false);
	pObj->m_fHasInterSystemEvent =		((dwLoad & 0x00010000) ? true : false);
	pObj->m_fHasOnDamageEvent =			((dwLoad & 0x00020000) ? true : false);
	pObj->m_fHasOnAttackedEvent =		((dwLoad & 0x00040000) ? true : false);
	pObj->m_fAutoClearDestination =		((dwLoad & 0x00080000) ? true : false);
	pObj->m_fHasOnOrdersCompletedEvent =((dwLoad & 0x00100000) ? true : false);
	pObj->m_fPlayerDocked =				((dwLoad & 0x00200000) ? true : false);
	pObj->m_fNonLinearMove =			((dwLoad & 0x00400000) ? true : false);
	pObj->m_fAscended =					((dwLoad & 0x00800000) ? true : false);
	pObj->m_fOutOfPlaneObj =			((dwLoad & 0x01000000) ? true : false);
	pObj->m_fAutoClearDestinationOnDock = ((dwLoad & 0x02000000) ? true : false);
	pObj->m_fShowHighlight =			((dwLoad & 0x04000000) ? true : false);
	pObj->m_fAutoClearDestinationOnDestroy = ((dwLoad & 0x08000000) ? true : false);
	pObj->m_fShowDamageBar =			((dwLoad & 0x10000000) ? true : false);
	pObj->m_fHasGravity =				((dwLoad & 0x20000000) ? true : false);
	pObj->m_fInsideBarrier =			((dwLoad & 0x40000000) ? true : false);
	pObj->m_fHasOnSubordinateAttackedEvent = ((dwLoad & 0x80000000) ? true : false);

	//	More flags

	if (Ctx.dwVersion >= 136)
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	else
		dwLoad = 0;

	//	0x00000001 unused since version 148
	pObj->m_fHasGetDockScreenEvent =	((dwLoad & 0x00000002) ? true : false);
	pObj->m_fHasOnAttackedByPlayerEvent =	((dwLoad & 0x00000004) ? true : false);
	pObj->m_fHasOnOrderChangedEvent =	((dwLoad & 0x00000008) ? true : false);
	pObj->m_fManualAnchor =				((dwLoad & 0x00000010) ? true : false);

	//	No need to save the following

	pObj->m_fOnCreateCalled = true;
	pObj->m_fItemEventsValid = false;
	pObj->m_fInDamage = false;
	pObj->m_fDestroyed = false;
	pObj->m_fPaintNeeded = false;
	pObj->m_fPainted = false;

	//	Load opaque data

	Ctx.iLoadState = loadStateObjData;
	pObj->m_Data.ReadFromStream(Ctx);
	Ctx.iLoadState = loadStateObject;

	//	Load additional data

	if (bSavedOldPos && Ctx.dwVersion >= 65)
		Ctx.pStream->Read((char *)&pObj->m_vOldPos, sizeof(CVector));
	else
		pObj->m_vOldPos = pObj->m_vPos - (pObj->m_vVel * g_SecondsPerUpdate);

	//	Subscriptions
	//
	//	NOTE: We ignore any missing objects. This can happen if (e.g.) a mission
	//	gets destroyed while we're out of the system.

	if (Ctx.dwVersion >= 77)
		pObj->m_SubscribedObjs.ReadFromStream(Ctx, true);

	//	Load the effect list

	Ctx.iLoadState = loadStateObjEffects;
	IEffectPainter *pEffect = CEffectCreator::CreatePainterFromStream(Ctx);
	while (pEffect)
		{
		int x, y, iTick, iRotation;
		Ctx.pStream->Read((char *)&x, sizeof(DWORD));
		Ctx.pStream->Read((char *)&y, sizeof(DWORD));
		Ctx.pStream->Read((char *)&iTick, sizeof(DWORD));
		if (Ctx.dwVersion >= 51)
			Ctx.pStream->Read((char *)&iRotation, sizeof(DWORD));
		else
			iRotation = 0;

		pObj->AddEffect(pEffect, x, y, iTick);

		pEffect = CEffectCreator::CreatePainterFromStream(Ctx);
		}

	//	Let the subclass read its part

	Ctx.iLoadState = loadStateObjSubClass;
	pObj->OnReadFromStream(Ctx);

	//	We must handle this after we load the subclass

	if (Ctx.dwVersion < 137)
		{
		pObj->m_fHasOnAttackedByPlayerEvent = pObj->FindEventHandler(CONSTLIT("OnAttackedByPlayer"));
		pObj->m_fHasOnOrderChangedEvent = pObj->FindEventHandler(CONSTLIT("OnOrderChanged"));
		}

	//	Done

	*retpObj = pObj;
	Ctx.iLoadState = iOldLoadState;
	}

EDamageResults CSpaceObject::Damage (SDamageCtx &Ctx)

//	Damage
//
//	Cause damage to the object

	{
	DEBUG_TRY

	ASSERT(!IsInDamageCode());
	SetInDamageCode();

	//	Let our subclasses handle it

	EDamageResults iResult = OnDamage(Ctx);

	//	Done

	ClearInDamageCode();
	return iResult;

	DEBUG_CATCH
	}

void CSpaceObject::DamageItem (CInstalledDevice *pDevice)

//	DamageItem
//
//	Damages an item

	{
	CItemListManipulator ItemList(GetItemList());
	SetCursorAtDevice(ItemList, pDevice);
	DamageItem(ItemList);
	}

void CSpaceObject::DamageItem (CItemListManipulator &ItemList)

//	DamageItem
//
//	Damages an item

	{
	const CItem &Item = ItemList.GetItemAtCursor();
	if (Item.IsDamaged())
		return;

	//	Figure out the current mods on this item

	CItemEnhancement Mods(Item.GetMods());

	//	If the item has an enhancement mod, then we remove it

	if (Mods.IsEnhancement())
		EnhanceItem(ItemList, etLoseEnhancement);

	//	If the item is enhanced, then damaging it removes the enhancements

	else if (Item.IsEnhanced())
		ItemList.SetEnhancedAtCursor(false);

	//	Otherwise, damage the item

	else if (!Item.IsDamaged())
		ItemList.SetDamagedAtCursor(true);

	//	Done

	ItemEnhancementModified(ItemList);
	}

CString CSpaceObject::DebugDescribe (CSpaceObject *pObj)

//	DebugDescribe
//
//	Describe object

	{
	try
		{
		if (pObj == NULL)
			return CONSTLIT("none");
		else if (pObj->IsDestroyed())
			return strPatternSubst(CONSTLIT("%x %s (%s) [destroyed]"), (DWORD)pObj, pObj->GetNounPhrase(), pObj->GetObjClassName());
		else
			return strPatternSubst(CONSTLIT("%x %s (%s)"), (DWORD)pObj, pObj->GetNounPhrase(), pObj->GetObjClassName());
		}
	catch (...)
		{
		}

	return strPatternSubst(CONSTLIT("%x [invalid]"), (DWORD)pObj);
	}

CString CSpaceObject::DebugLoadError (SLoadCtx &Ctx)

//	DebugLoadError
//
//	Compose error message from loading.

	{
	CString sLine = strPatternSubst(CONSTLIT(
			"Unable to load object.\r\n"
			"State: %s\r\n"
			"ObjectClassID: %x\r\n"),

			GetLoadStateString(Ctx.iLoadState),
			Ctx.dwObjClassID);

	//	If we're loading an effect, output that

	if (Ctx.iLoadState == loadStateEffect)
		sLine.Append(strPatternSubst(CONSTLIT(
			"EffectUNID: %s\r\n"),

			Ctx.sEffectUNID));

	return sLine;
	}

void CSpaceObject::Destroy (DestructionTypes iCause, const CDamageSource &Attacker, CWeaponFireDesc *pWeaponDesc, CSpaceObject **retpWreck)

//	Destroy
//
//	Destroy this object

	{
	DEBUG_TRY

	//	Do not recurse

	if (IsDestroyed())
		return;

	//	Prepare struct

	SDestroyCtx Ctx;
	Ctx.pObj = this;
	Ctx.pDesc = pWeaponDesc;
	Ctx.iCause = iCause;
	Ctx.Attacker = Attacker;
	Ctx.pWreck = NULL;
	Ctx.bResurrectPending = false;
	Ctx.pResurrectedObj = NULL;
	if (retpWreck)
		*retpWreck = NULL;

	//	Give our descendants a chance to do something
	//	If necessary, our descendants will set bResurrectPending.

	OnDestroyed(Ctx);

	//	Even if resurrecting, mark object as destroyed. We need to
	//	do this because cached lists will still have the object for a bit
	//	(We will clear this in AddToSystem).
	//
	//	Also, we need to mark this before we start calling OnObjDestroyed
	//	(so that we can detect any attempts at recursion).

	m_fDestroyed = true;

	//	If we were destroyed by a weapon, let the weapon know that it succeeded.

	if (pWeaponDesc)
		pWeaponDesc->FireOnDestroyObj(Ctx);

	if (Attacker.GetObj())
		Attacker.GetObj()->FireOnDestroyObj(Ctx);

	//	Remove from the object from the universal list (NOTE: We must do this
	//	before we clear out m_pSystem.)

	CSpaceObject::Categories iCategory = GetCategory();
	if (iCategory == CSpaceObject::catStation || iCategory == CSpaceObject::catShip)
		GetUniverse().GetGlobalObjects().Delete(this);

	//	Remove from system. This will call OnObjDestroyed to all other
	//	interested objects

	CSystem *pSystem = m_pSystem;
	m_pSystem = NULL;
	if (m_iIndex != -1)
		{
		//	Give our descendants a chance to remove. For example, ships will
		//	deal with attached sections.

		OnRemoved(Ctx);

		//	Remove from system

		pSystem->RemoveObject(Ctx);
		m_iIndex = -1;

		//	Delete

		if (!Ctx.bResurrectPending)
			{
			//	If this was the player, remove ship variables

			if (IsPlayer())
				{
				//	Clean up these variables since the player is out
				//	of the system. We need to do this because otherwise
				//	an event might set a target for the player and if the
				//	target is destroyed, we would never get an OnObjDestroyed message

				GetUniverse().SetPlayerShip(NULL);

				//	The player will be deleted at higher layers, but
				//	it is out of the system now.
				}

			//	The objects get deleted at the end of the update

			else
				pSystem->AddToDeleteList(this);
			}
		}

	//	Return wreck

	if (retpWreck)
		*retpWreck = Ctx.pWreck;

	//	See if we are in the middle of update

	if (m_pObjInUpdate == this)
		m_bObjDestroyed = true;

	//	If resurrection is pending, then clear the destroyed flag. Otherwise, 
	//	wingmen might leave the player.
	//
	//	NOTE: This is somewhat a hack, and it works only because we remove the
	//	player ship from lists that might contain it (including the system grid).
	//	In the future it might be cleaner to leave the destroyed flag set but
	//	set an additional resurrection flag and have wingmen check that.

	if (Ctx.bResurrectPending && IsPlayer())
		m_fDestroyed = false;

	DEBUG_CATCH
	}

void CSpaceObject::DisruptItem (CItemListManipulator &ItemList, DWORD dwDuration)

//	DisruptItem
//
//	Disrupts an item

	{
	ItemList.SetDisruptedAtCursor(dwDuration);
	ItemEnhancementModified(ItemList);
	}

EnhanceItemStatus CSpaceObject::EnhanceItem (CItemListManipulator &ItemList, const CItemEnhancement &Mods, DWORD *retdwID)

//	EnhanceItem
//
//	Enhances the item at cursor (either installed or in cargo hold)

	{
	DEBUG_TRY

	//	Pre-init in case we exit early

	if (retdwID)
		*retdwID = OBJID_NULL;

	//	Get the item to enhance

	const CItem &Item = ItemList.GetItemAtCursor();
	CItemType *pType = Item.GetType();

	//	If this is an old-style enhancement, then handle it the old way

	if (Mods.GetModCode() == etBinaryEnhancement)
		{
		//	If the item is damaged, then enhancing it repairs it

		if (Item.IsDamaged())
			ItemList.SetDamagedAtCursor(false);

		//	Otherwise, enhance it if it is not already enhanced

		else if (!Item.IsEnhanced())
			ItemList.SetEnhancedAtCursor(true);

		//	Otherwise, we are already enhanced

		else
			return eisAlreadyEnhanced;

		//	Raise event

		ItemEnhancementModified(ItemList);

		return eisOK;
		}

	//	Figure out the effect of the enhancement on the item

	CItemEnhancement Enhancement = Item.GetMods();
	EnhanceItemStatus iResult = Enhancement.Combine(Item, Mods.GetModCode());

	//	Handle some special cases

	if (pType->IsArmor())
		{
		CInstalledArmor *pArmor = FindArmor(ItemList.GetItemAtCursor());
		CArmorClass *pArmorClass = pType->GetArmorClass();

		//	If we're trying to make armor immune to radiation and it is already immune
		//	then we return already enhanced

		if (iResult == eisOK 
				&& Enhancement.IsRadiationImmune()
				&& pArmorClass->IsImmune(CItemCtx(this, pArmor), specialRadiation))
			return eisAlreadyEnhanced;
		}

	//	Enhance

	DWORD dwID = ItemList.AddItemEnhancementAtCursor(Enhancement);

	//	Deal with installed items

	ItemEnhancementModified(ItemList);

	//	Fire On event to the enhancement

	if (Mods.GetEnhancementType() && ItemList.IsCursorValid())
		{
		CItem theEnhancement(Mods.GetEnhancementType(), 1);
		theEnhancement.FireOnAddedAsEnhancement(this, ItemList.GetItemAtCursor(), iResult);
		}

	//	Done

	if (retdwID)
		*retdwID = dwID;

	//	Done

	return iResult;

	DEBUG_CATCH
	}

void CSpaceObject::EnterGate (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate, bool bAscend)

//	EnterGate
//
//	We enter a stargate.
//
//	NOTE: pDestNode and pStargate may be NULL.

	{
	int i;

	//	If we're going to the same system, then do nothing

	if (pDestNode && pDestNode->GetSystem() == m_pSystem)
		return;

	//	Notify subscribers

	m_SubscribedObjs.NotifyOnObjEnteredGate(this, pDestNode, sDestEntryPoint, pStargate);

	//	Handle the case where the player entered the gate

	if (IsPlayer())
		{
		//	Clear destination, if necessary

		if (pStargate && pStargate->IsAutoClearDestinationOnGate())
			pStargate->ClearPlayerDestination();

		//	If some object destroyed the player while gating

		if (IsDestroyed())
			return;
		}

	//	Tell all listeners that this object entered a stargate

	for (i = 0; i < m_pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = m_pSystem->GetObject(i);

		if (pObj && pObj != this)
			pObj->OnObjEnteredGate(this, pDestNode, sDestEntryPoint, pStargate);
		}

	//	Let the object do the appropriate thing when entering a gate
	//	Note: Objects rely on this happening after other objects
	//	are notified.

	GateHook(pDestNode, sDestEntryPoint, pStargate, bAscend);
	}

int CSpaceObject::FindCommsMessage (const CString &sID)

//	FindCommsMessage
//
//	Returns the index of the given comms message (or -1 if not found)

	{
	CCommunicationsHandler *pHandler = GetCommsHandler();
	if (pHandler == NULL)
		return -1;

	return pHandler->FindMessageByID(sID);
	}

int CSpaceObject::FindCommsMessageByName (const CString &sName)

//	FindCommsMessageByName
//
//	Returns the index of the given comms message (or -1 if not found)

	{
	CCommunicationsHandler *pHandler = GetCommsHandler();
	if (pHandler == NULL)
		return -1;

	return pHandler->FindMessageByName(sName);
	}

bool CSpaceObject::FindDevice (const CItem &Item, CInstalledDevice **retpDevice, CString *retsError)

//	FindDevice
//
//	Looks for the device of the item; returns an error if not found.

	{
	CInstalledDevice *pDevice = FindDevice(Item);
	if (pDevice == NULL)
		{
		*retsError = CONSTLIT("Item is not an installed device on object.");
		return false;
		}

	if (retpDevice)
		*retpDevice = pDevice;

	return true;
	}

bool CSpaceObject::FindEventHandler (const CString &sEntryPoint, SEventHandlerDesc *retEvent) const

//	FindEventHandler
//
//	Finds the event handler for the given event

	{
	//	Check our override

	if (m_pOverride && m_pOverride->FindEventHandler(sEntryPoint, retEvent))
		return true;

	//	Check our type

	CDesignType *pType = GetType();
	if (pType)
		return pType->FindEventHandler(sEntryPoint, retEvent);
	
	//	Not found

	return false;
	}

bool CSpaceObject::FindEventHandler (CDesignType::ECachedHandlers iEvent, SEventHandlerDesc *retEvent) const

//	FindEventHandler
//
//	Finds the event handler for the given event

	{
	//	Check our override

	if (m_pOverride && m_pOverride->FindEventHandler(iEvent, retEvent))
		return true;

	//	Check our type

	CDesignType *pType = GetType();
	if (pType)
		return pType->FindEventHandler(iEvent, retEvent);
	
	//	Not found

	return false;
	}

bool CSpaceObject::FireCanDockAsPlayer (CSpaceObject *pDockTarget, CString *retsError)

//	FireCanDockAsPlayer
//
//	Fires an event to ask the object if it should be allowed to dock with the
//	given dock target.
//
//	Returns TRUE if docking is allowed. If FALSE, retsError is initialized with 
//	the message to return to the player.

	{
	SEventHandlerDesc Event;
	if (FindEventHandler(CAN_DOCK_AS_PLAYER_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
		Ctx.DefineSpaceObject(CONSTLIT("aDockTarget"), pDockTarget);

		ICCItem *pResult = Ctx.Run(Event);

		bool bAllowDock;
		if (pResult->IsError())
			{
			ReportEventError(CAN_DOCK_AS_PLAYER_EVENT, pResult);
			*retsError = NULL_STR;
			bAllowDock = false;
			}
		else if (!pResult->IsTrue())
			{
			*retsError = pResult->GetStringValue();
			bAllowDock = false;
			}
		else
			bAllowDock = true;

		Ctx.Discard(pResult);

		return bAllowDock;
		}
	else
		return true;
	}

bool CSpaceObject::FireCanInstallItem (const CItem &Item, int iSlot, CString *retsResult)

//	FireCanInstallItem
//
//	Asks the object whether we can install the given item.

	{
	SEventHandlerDesc Event;
	if (FindEventHandler(CDesignType::evtCanInstallItem, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
		Ctx.SaveAndDefineItemVar(Item);

		if (iSlot == -1)
			{
			Ctx.DefineNil(CONSTLIT("aArmorSeg"));
			Ctx.DefineNil(CONSTLIT("aItemToReplace"));
			}
		else if (Item.IsArmor())
			{
			Ctx.DefineInteger(CONSTLIT("aArmorSeg"), iSlot);
			Ctx.DefineNil(CONSTLIT("aItemToReplace"));
			}
		else
			{
			CItemListManipulator ItemList(GetItemList());
			if (SetCursorAtDevice(ItemList, iSlot))
				{
				const CItem &ItemToReplace = ItemList.GetItemAtCursor();
				Ctx.DefineItem(CONSTLIT("aItemToReplace"), ItemToReplace);
				}
			else
				Ctx.DefineNil(CONSTLIT("aItemToReplace"));

			Ctx.DefineNil(CONSTLIT("aArmorSeg"));
			}

		ICCItem *pResult = Ctx.Run(Event);

		bool bCanBeInstalled;
		if (pResult->IsError())
			{
			*retsResult = pResult->GetStringValue();
			ReportEventError(strPatternSubst(CONSTLIT("Ship %x CanInstallItem"), GetType()->GetUNID()), pResult);
			bCanBeInstalled = false;
			}
		else if (!pResult->IsTrue())
			{
			*retsResult = pResult->GetStringValue();
			bCanBeInstalled = false;
			}
		else
			bCanBeInstalled = true;

		Ctx.Discard(pResult);

		return bCanBeInstalled;
		}
	else
		return true;
	}

bool CSpaceObject::FireCanRemoveItem (const CItem &Item, int iSlot, CString *retsResult)

//	FireCanRemoveItem
//
//	Asks the object whether we can remove the given item.

	{
	SEventHandlerDesc Event;
	if (FindEventHandler(CDesignType::evtCanRemoveItem, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
		Ctx.SaveAndDefineItemVar(Item);
		if (iSlot != -1)
			Ctx.DefineInteger(CONSTLIT("aArmorSeg"), iSlot);
		else
			Ctx.DefineNil(CONSTLIT("aArmorSeg"));

		ICCItem *pResult = Ctx.Run(Event);

		bool bCanBeRemoved;
		if (pResult->IsError())
			{
			*retsResult = pResult->GetStringValue();
			ReportEventError(strPatternSubst(CONSTLIT("Ship %x CanRemoveItem"), GetType()->GetUNID()), pResult);
			bCanBeRemoved = false;
			}
		else if (!pResult->IsTrue())
			{
			*retsResult = pResult->GetStringValue();
			bCanBeRemoved = false;
			}
		else
			bCanBeRemoved = true;

		Ctx.Discard(pResult);

		return bCanBeRemoved;
		}
	else
		return true;
	}

void CSpaceObject::FireCustomEvent (const CString &sEvent, ECodeChainEvents iEvent, ICCItem *pData, ICCItem **retpResult)

//	FireCustomEvent
//
//	Fires a named event and optionally returns result

	{
	CCodeChainCtx Ctx(GetUniverse());

	SEventHandlerDesc Event;
	if (FindEventHandler(sEvent, &Event))
		{
		Ctx.SetEvent(iEvent);
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
		Ctx.SaveAndDefineDataVar(pData);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(sEvent, pResult);

		//	Either return the event result or discard it

		if (retpResult)
			*retpResult = pResult;
		else
			Ctx.Discard(pResult);
		}
	else
		{
		if (retpResult)
			*retpResult = Ctx.CreateNil();
		}
	}

void CSpaceObject::FireCustomItemEvent (const CString &sEvent, const CItem &Item, ICCItem *pData, ICCItem **retpResult)

//	FireCustomItemEvent
//
//	Fires a named event to an item and optionally returns result

	{
	Item.FireCustomEvent(CItemCtx(&Item, this), sEvent, pData, retpResult);
	}

void CSpaceObject::FireCustomOverlayEvent (const CString &sEvent, DWORD dwOverlayID, ICCItem *pData, ICCItem **retpResult)

//	FireCustomOverlayEvent
//
//	Fires a custom event on an overlay

	{
	CCodeChain &CC = GetUniverse().GetCC();

	//	Find the overlay

	COverlay *pOverlay = GetOverlay(dwOverlayID);
	if (pOverlay == NULL)
		{
		if (retpResult)
			*retpResult = CC.CreateNil();
		return;
		}

	//	Fire event

	pOverlay->FireCustomEvent(this, sEvent, pData, retpResult);
	}

void CSpaceObject::FireCustomShipOrderEvent (const CString &sEvent, CSpaceObject *pShip, ICCItem **retpResult)

//	FireCustomShipOrderEvent
//
//	Fires an event in response to a fireEvent order.

	{
	CCodeChainCtx Ctx(GetUniverse());

	SEventHandlerDesc Event;
	if (FindEventHandler(sEvent, &Event))
		{
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
        Ctx.SaveAndDefineDataVar(NULL);
		Ctx.DefineSpaceObject(CONSTLIT("aShipObj"), pShip);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(sEvent, pResult);

		//	Either return the event result or discard it

		if (retpResult)
			*retpResult = pResult;
		else
			Ctx.Discard(pResult);
		}
	else
		{
		if (retpResult)
			*retpResult = Ctx.CreateNil();
		}
	}

bool CSpaceObject::FireGetDockScreen (CDockScreenSys::SSelector *retSelector) const

//	FireGetDockScreen
//
//	Allows the object to override the first dock screen

	{
	SEventHandlerDesc Event;
	if (!HasGetDockScreenEvent() 
			|| !FindEventHandler(GET_DOCK_SCREEN_EVENT, &Event))
		return false;

	CCodeChainCtx Ctx(GetUniverse());
	Ctx.DefineContainingType(this);
	Ctx.SaveAndDefineSourceVar(const_cast<CSpaceObject *>(this));

	ICCItemPtr pResult = Ctx.RunCode(Event);
	if (pResult->IsError())
		{
		ReportEventError(GET_DOCK_SCREEN_EVENT, pResult);
		return false;
		}

	return CTLispConvert::AsScreenSelector(pResult, retSelector);
	}

bool CSpaceObject::FireGetExplosionType (SExplosionType *retExplosion) const

//	FireGetExplosionType
//
//	Allows the object to compute the kind of explosion

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(GET_EXPLOSION_TYPE_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(const_cast<CSpaceObject *>(this));

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(GET_EXPLOSION_TYPE_EVENT, pResult);

		//	Return UNID, bonus, and cause

		DWORD dwUNID;
		int iBonus;
		DestructionTypes iCause;

		//	If the result is a list, then we expect a list with the following values:
		//
		//	The UNID of the explosion
		//	The bonus
		//	The cause (e.g., "explosion" or "playerCreatedExplosion")

		if (pResult->IsNil())
			{
			dwUNID = 0;
			iBonus = 0;
			iCause = killedByExplosion;
			}
		else if (pResult->IsList())
			{
			dwUNID = (DWORD)pResult->GetElement(0)->GetIntegerValue();
			iBonus = pResult->GetElement(1)->GetIntegerValue();
			if (pResult->GetElement(2)->IsNil())
				iCause = killedByExplosion;
			else
				{
				iCause = ::GetDestructionCause(pResult->GetElement(2)->GetStringValue());
				if (iCause == killedNone)
					iCause = killedByExplosion;
				}
			}

		//	Otherwise, expect just an UNID
		else
			{
			dwUNID = (DWORD)pResult->GetIntegerValue();
			iBonus = 0;
			iCause = killedByExplosion;
			}

		//	Done

		Ctx.Discard(pResult);

		//	Return

		retExplosion->pDesc = (dwUNID ? GetUniverse().FindWeaponFireDesc(strPatternSubst(CONSTLIT("%d/0"), dwUNID)) : NULL);
		retExplosion->iBonus = iBonus;
		retExplosion->iCause = iCause;

		return (retExplosion->pDesc != NULL);
		}
	else
		{
		retExplosion->pDesc = NULL;
		retExplosion->iBonus = 0;
		retExplosion->iCause = killedByExplosion;

		return false;
		}
	}

bool CSpaceObject::FireGetPlayerPriceAdj (STradeServiceCtx &ServiceCtx, ICCItem *pData, int *retiPriceAdj)

//	FireGetPlayerPriceAdj
//
//	Fire GetPlayerPriceAdj

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(GET_PLAYER_PRICE_ADJ_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());

		//	Set up

		Ctx.SetEvent(eventGetTradePrice);
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
		if (ServiceCtx.pItem)
			{
			Ctx.SaveAndDefineItemVar(*ServiceCtx.pItem);
			Ctx.SetItemType(ServiceCtx.pItem->GetType());
			}
		else if (ServiceCtx.pObj)
			{
			Ctx.DefineSpaceObject(CONSTLIT("aObj"), ServiceCtx.pObj);
			}

		Ctx.DefineString(CONSTLIT("aService"), CTradingDesc::ServiceToString(ServiceCtx.iService));
		Ctx.DefineSpaceObject(CONSTLIT("aProviderObj"), ServiceCtx.pProvider);
		if (pData)
			Ctx.SaveAndDefineDataVar(pData);

		//	Run

		ICCItem *pResult = Ctx.Run(Event);

		int iPriceAdj = 100;
		if (pResult->IsError())
			ReportEventError(GET_PLAYER_PRICE_ADJ_EVENT, pResult);
		else if (pResult->IsNil())
			;
		else
			iPriceAdj = pResult->GetIntegerValue();

		//	Done

		Ctx.Discard(pResult);

		if (retiPriceAdj)
			*retiPriceAdj = iPriceAdj;

		return (iPriceAdj != 100);
		}
	else
		return false;
	}

void CSpaceObject::FireItemOnAIUpdate (void)

//	FireItemOnAIUpdate
//
//	Fires OnAIUpdate event for all items

	{
	if (!m_fItemEventsValid)
		InitItemEvents();

	m_ItemEvents.FireEvent(this, eventOnAIUpdate);
	}

void CSpaceObject::FireItemOnDocked (CSpaceObject *pDockedAt)

//	FireItemOnDocked
//
//	Fires OnDocked event for all items

	{
	//	We need to initialize, even if we don't use the event list because we
	//	rely on the flag to tell us if items changed out from under us.

	if (!m_fItemEventsValid)
		InitItemEvents();

	m_ItemEvents.FireOnDocked(this, pDockedAt);
	}

void CSpaceObject::FireItemOnObjDestroyed (const SDestroyCtx &Ctx)

//	FireItemOnObjDestroyed
//
//	Fires OnObjDestroyed event for all items

	{
	//	We need to initialize, even if we don't use the event list because we
	//	rely on the flag to tell us if items changed out from under us.

	if (!m_fItemEventsValid)
		InitItemEvents();

	m_ItemEvents.FireOnObjDestroyed(this, Ctx);
	}

void CSpaceObject::FireItemOnUpdate (void)

//	FireItemOnUpdate
//
//	Fires OnUpdate event for all items

	{
	if (!m_fItemEventsValid)
		InitItemEvents();

	m_ItemEvents.FireUpdateEvents(this);
	}

void CSpaceObject::FireOnAttacked (const SDamageCtx &Ctx)

//	FireOnAttacked
//
//	Fire OnAttacked event

	{
	DEBUG_TRY

	SEventHandlerDesc Event;

	if (FindEventHandler(ON_ATTACKED_EVENT, &Event))
		{
		CCodeChainCtx CCCtx(GetUniverse());
		CCCtx.DefineContainingType(this);
		CCCtx.SaveAndDefineSourceVar(this);
		CCCtx.DefineDamageCtx(Ctx);

		ICCItem *pResult = CCCtx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_ATTACKED_EVENT, pResult);

		//	Done

		CCCtx.Discard(pResult);
		}

	DEBUG_CATCH
	}

void CSpaceObject::FireOnAttackedByPlayer (void)

//	FireOnAttackedByPlayer
//
//	Fire OnAttackedByPlayer event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_ATTACKED_BY_PLAYER_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_ATTACKED_BY_PLAYER_EVENT, pResult);
		Ctx.Discard(pResult);
		}
	}

void CSpaceObject::FireOnCreate (void)

//	FireOnCreate
//
//	Fire OnCreate event

	{
	FireOnCreate(SOnCreate());
	}

void CSpaceObject::FireOnCreate (const SOnCreate &OnCreate)

//	FireOnCreate
//
//	Fire OnCreate event

	{
	SEventHandlerDesc Event;

	if (!m_fOnCreateCalled 
			&& FindEventHandler(ON_CREATE_EVENT, &Event))
		{
#ifdef DEBUG_ON_CREATE_TIME
		DWORD dwStart = ::GetTickCount();
#endif

		CCodeChainCtx Ctx(GetUniverse());
		Ctx.SetSystemCreateCtx(OnCreate.pCreateCtx);
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
		Ctx.SaveAndDefineDataVar(OnCreate.pData);
		Ctx.DefineSpaceObject(CONSTLIT("aBaseObj"), OnCreate.pBaseObj);
		Ctx.DefineSpaceObject(CONSTLIT("aOwnerObj"), OnCreate.pOwnerObj);
		Ctx.DefineSpaceObject(CONSTLIT("aTargetObj"), OnCreate.pTargetObj);
		if (OnCreate.pOrbit)
			Ctx.DefineOrbit(CONSTLIT("aOrbit"), *OnCreate.pOrbit);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_CREATE_EVENT, pResult);
		Ctx.Discard(pResult);

#ifdef DEBUG_ON_CREATE_TIME
		g_dwTotalTime += ::sysGetTicksElapsed(dwStart);
		printf("[%x] OnCreate: %s (%d)\n", GetID(), (LPSTR)GetNounPhrase(), g_dwTotalTime);
#endif
		}

	//	Remember that we already called OnCreate. This is helpful in case we
	//	create an object inside another object's OnCreate
	//
	//	[We set this even if there is no OnCreate event because we use this
	//	to test whether we're fully initialized.]

	m_fOnCreateCalled = true;
	}

void CSpaceObject::FireOnCreateOrders (CSpaceObject *pBase, CSpaceObject *pTarget)

//	FireOnCreateOrders
//
//	Fire OnCreateOrders event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_CREATE_ORDERS_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
		Ctx.DefineSpaceObject(CONSTLIT("aBaseObj"), pBase);
		Ctx.DefineSpaceObject(CONSTLIT("aTargetObj"), pTarget);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_CREATE_ORDERS_EVENT, pResult);
		Ctx.Discard(pResult);
		}
	}

int CSpaceObject::FireOnDamage (SDamageCtx &Ctx, int iDamage)

//	FireOnDamage
//
//	Fire OnDamage event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_DAMAGE_EVENT, &Event))
		{
		CCodeChainCtx CCCtx(GetUniverse());
		CCCtx.DefineContainingType(this);
		CCCtx.SaveAndDefineSourceVar(this);
		CCCtx.DefineDamageCtx(Ctx, iDamage);

		ICCItem *pResult = CCCtx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_DAMAGE_EVENT, pResult);

		//	Result is the amount of damage

		iDamage = pResult->GetIntegerValue();
		CCCtx.Discard(pResult);
		}

	return iDamage;
	}

void CSpaceObject::FireOnDeselected (void)

//	FireOnDeselected
//
//	Fire OnDeselected event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_DESELECTED_EVENT, &Event))
		{
		CCodeChainCtx CCCtx(GetUniverse());
		CCCtx.DefineContainingType(this);
		CCCtx.SaveAndDefineSourceVar(this);
		CCCtx.DefineInteger(CONSTLIT("aPlayer"), g_PlayerSovereignUNID);

		//	Run code

		ICCItem *pResult = CCCtx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_DESELECTED_EVENT, pResult);
		CCCtx.Discard(pResult);
		}
	}

void CSpaceObject::FireOnDestroy (const SDestroyCtx &Ctx)

//	FireOnDestroy
//
//	Fire OnDestroy event

	{
	DEBUG_TRY

	SEventHandlerDesc Event;

	if (FindEventHandler(ON_DESTROY_EVENT, &Event))
		{
		CCodeChainCtx CCCtx(GetUniverse());
		CCCtx.DefineContainingType(this);
		CCCtx.SaveAndDefineSourceVar(this);
		CCCtx.DefineSpaceObject(CONSTLIT("aDestroyer"), Ctx.Attacker.GetObj());
		CCCtx.DefineSpaceObject(CONSTLIT("aOrderGiver"), Ctx.GetOrderGiver());
		CCCtx.DefineSpaceObject(CONSTLIT("aWreckObj"), Ctx.pWreck);
		CCCtx.DefineBool(CONSTLIT("aDestroy"), Ctx.WasDestroyed());
		CCCtx.DefineString(CONSTLIT("aDestroyReason"), GetDestructionName(Ctx.iCause));

		//	Run code

		ICCItem *pResult = CCCtx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_DESTROY_EVENT, pResult);
		CCCtx.Discard(pResult);
		}

	DEBUG_CATCH
	}

bool CSpaceObject::FireOnDestroyCheck (DestructionTypes iCause, const CDamageSource &Attacker)

//	FireOnDestroyCheck
//
//	Fire OnDestroyCheck event. Returns FALSE if we avoid destruction.

	{
	CCodeChainCtx Ctx(GetUniverse());

	//	If we have code, call it.

	SEventHandlerDesc Event;
	if (FindEventHandler(CDesignType::evtOnDestroyCheck, &Event)
			&& !Ctx.InEvent(eventOnDestroyCheck))
		{
		Ctx.SetEvent(eventOnDestroyCheck);
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
		Ctx.DefineSpaceObject(CONSTLIT("aDestroyer"), Attacker.GetObj());
		Ctx.DefineSpaceObject(CONSTLIT("aOrderGiver"), Attacker.GetOrderGiver());
		Ctx.DefineBool(CONSTLIT("aDestroy"), (iCause != enteredStargate && iCause != ascended));
		Ctx.DefineString(CONSTLIT("aDestroyReason"), GetDestructionName(iCause));

		ICCItemPtr pResult = Ctx.RunCode(Event);
		return !pResult->IsNil();
		}

	//	Otherwise, destruction succeeds

	return true;
	}

void CSpaceObject::FireOnDestroyObj (const SDestroyCtx &Ctx)

//	FireOnDestroyObj
//
//	Fire OnDestroyObj event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_DESTROY_OBJ_EVENT, &Event))
		{
		CCodeChainCtx CCCtx(GetUniverse());
		CCCtx.DefineContainingType(this);
		CCCtx.SaveAndDefineSourceVar(this);
		CCCtx.DefineSpaceObject(CONSTLIT("aObjDestroyed"), Ctx.pObj);
		CCCtx.DefineSpaceObject(CONSTLIT("aDestroyer"), Ctx.Attacker.GetObj());
		CCCtx.DefineSpaceObject(CONSTLIT("aOrderGiver"), Ctx.GetOrderGiver());
		CCCtx.DefineSpaceObject(CONSTLIT("aWreckObj"), Ctx.pWreck);
		CCCtx.DefineBool(CONSTLIT("aDestroy"), Ctx.WasDestroyed());
		CCCtx.DefineString(CONSTLIT("aDestroyReason"), GetDestructionName(Ctx.iCause));

		ICCItem *pResult = CCCtx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_DESTROY_OBJ_EVENT, pResult);
		CCCtx.Discard(pResult);
		}
	}

bool CSpaceObject::FireOnDockObjAdj (CSpaceObject **retpObj)

//	FireOnDockObjAdj
//
//	Fires an event to adjust the object that a player will dock with

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_DOCK_OBJ_ADJ_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);

		ICCItem *pResult = Ctx.Run(Event);

		if (pResult->IsError())
			{
			ReportEventError(ON_DOCK_OBJ_ADJ_EVENT, pResult);
			Ctx.Discard(pResult);
			return false;
			}
		else if (pResult->IsNil())
			{
			Ctx.Discard(pResult);
			return false;
			}
		else
			{
			CSpaceObject *pNewObj = Ctx.AsSpaceObject(pResult);
			Ctx.Discard(pResult);

			if (pNewObj == NULL || pNewObj == this)
				return false;

			*retpObj = pNewObj;
			return true;
			}
		}

	return false;
	}

void CSpaceObject::FireOnDockObjDestroyed (CSpaceObject *pDockTarget, const SDestroyCtx &Ctx)

//	FireOnDockObjDestroyed
//
//	Fire event when a object that we are docked with (or in the process of 
//	docking with) got destroyed.

	{
	SEventHandlerDesc Event;
	if (!FindEventHandler(ON_DOCK_OBJ_DESTROYED_EVENT, &Event))
		return;

	CCodeChainCtx CCCtx(GetUniverse());
	CCCtx.DefineContainingType(this);
	CCCtx.SaveAndDefineSourceVar(this);
	CCCtx.DefineSpaceObject(CONSTLIT("aObjDestroyed"), Ctx.pObj);
	CCCtx.DefineSpaceObject(CONSTLIT("aDestroyer"), Ctx.Attacker.GetObj());
	CCCtx.DefineSpaceObject(CONSTLIT("aOrderGiver"), Ctx.GetOrderGiver());
	CCCtx.DefineSpaceObject(CONSTLIT("aWreckObj"), Ctx.pWreck);
	CCCtx.DefineBool(CONSTLIT("aDestroy"), Ctx.WasDestroyed());
	CCCtx.DefineString(CONSTLIT("aDestroyReason"), GetDestructionName(Ctx.iCause));

	ICCItemPtr pResult = CCCtx.RunCode(Event);
	if (pResult->IsError())
		ReportEventError(ON_DOCK_OBJ_DESTROYED_EVENT, pResult);
	}

void CSpaceObject::FireOnEnteredGate (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pGate)

//	FireOnEnteredGate
//
//	Fire event when this object has entered a gate

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_ENTERED_GATE_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
		Ctx.DefineSpaceObject(CONSTLIT("aGateObj"), pGate);
		Ctx.DefineString(CONSTLIT("aDestNodeID"), (pDestNode ? pDestNode->GetID() : NULL_STR));
		Ctx.DefineString(CONSTLIT("aDestEntryPoint"), sDestEntryPoint);

		//	Run code

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_ENTERED_GATE_EVENT, pResult);
		Ctx.Discard(pResult);
		}
	}

void CSpaceObject::FireOnEnteredSystem (CSpaceObject *pGate)

//	FireOnEnteredSystem
//
//	Fire event when this object has comes out of a gate into a new system

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_ENTERED_SYSTEM_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
		Ctx.DefineSpaceObject(CONSTLIT("aGateObj"), pGate);

		//	Run code

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_ENTERED_SYSTEM_EVENT, pResult);
		Ctx.Discard(pResult);
		}
	}

void CSpaceObject::FireOnLoad (SLoadCtx &Ctx)

//	FireOnLoad
//
//	Fire OnLoad event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_LOAD_EVENT, &Event))
		{
		CCodeChainCtx CCCtx(GetUniverse());
		CCCtx.DefineContainingType(this);
		CCCtx.SaveAndDefineSourceVar(this);
		CCCtx.DefineInteger(CONSTLIT("aVersion"), Ctx.dwVersion);

		ICCItem *pResult = CCCtx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_LOAD_EVENT, pResult);
		CCCtx.Discard(pResult);
		}
	}

void CSpaceObject::FireOnMining (const SDamageCtx &Ctx)

//	FireOnMining
//
//	Fire OnMining event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_MINING_EVENT, &Event))
		{
		CCodeChainCtx CCCtx(GetUniverse());
		CCCtx.DefineContainingType(this);
		CCCtx.SaveAndDefineSourceVar(this);
		CCCtx.DefineSpaceObject(CONSTLIT("aMiner"), Ctx.Attacker.GetObj());
		CCCtx.DefineVector(CONSTLIT("aMinePos"), Ctx.vHitPos);
		CCCtx.DefineInteger(CONSTLIT("aMineDir"), Ctx.iDirection);
		CCCtx.DefineInteger(CONSTLIT("aMineProbability"), Ctx.Damage.GetMiningAdj());
		CCCtx.DefineInteger(CONSTLIT("aHP"), Ctx.iDamage);
		CCCtx.DefineString(CONSTLIT("aDamageType"), GetDamageShortName(Ctx.Damage.GetDamageType()));
		CCCtx.DefineItemType(CONSTLIT("aWeaponType"), Ctx.pDesc->GetWeaponType());

		ICCItem *pResult = CCCtx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_MINING_EVENT, pResult);
		CCCtx.Discard(pResult);
		}
	}

void CSpaceObject::FireOnMissionAccepted (CMission *pMission)

//	FireOnMissionAccepted
//
//	Fire <OnMissionAccepted> event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_MISSION_ACCEPTED_EVENT, &Event))
		{
		CCodeChainCtx CCCtx(GetUniverse());
		CCCtx.DefineContainingType(this);
		CCCtx.SaveAndDefineSourceVar(this);
		CCCtx.DefineSpaceObject(CONSTLIT("aMissionObj"), pMission);

		ICCItem *pResult = CCCtx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_MISSION_ACCEPTED_EVENT, pResult);
		CCCtx.Discard(pResult);
		}
	}

void CSpaceObject::FireOnMissionCompleted (CMission *pMission, const CString &sReason)

//	FireOnMissionCompleted
//
//	Fire <OnMissionCompleted> event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_MISSION_COMPLETED_EVENT, &Event))
		{
		CCodeChainCtx CCCtx(GetUniverse());
		CCCtx.DefineContainingType(this);
		CCCtx.SaveAndDefineSourceVar(this);
		CCCtx.DefineSpaceObject(CONSTLIT("aMissionObj"), pMission);
		CCCtx.DefineString(CONSTLIT("aReason"), sReason);

		ICCItem *pResult = CCCtx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_MISSION_COMPLETED_EVENT, pResult);
		CCCtx.Discard(pResult);
		}
	}

void CSpaceObject::FireOnObjBlacklistedPlayer (CSpaceObject *pObj)

//	FireOnObjBlacklistedPlayer
//
//	Fire OnObjBlacklistedPlayer event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_OBJ_BLACKLISTED_PLAYER_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
		Ctx.DefineSpaceObject(CONSTLIT("aObj"), pObj);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_OBJ_BLACKLISTED_PLAYER_EVENT, pResult);
		Ctx.Discard(pResult);
		}
	}

void CSpaceObject::FireOnObjDestroyed (const SDestroyCtx &Ctx)

//	FireOnObjDestroyed
//
//	Fire OnObjDestroyed event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_OBJ_DESTROYED_EVENT, &Event))
		{
		CCodeChainCtx CCCtx(GetUniverse());
		CCCtx.DefineContainingType(this);
		CCCtx.SaveAndDefineSourceVar(this);
		CCCtx.DefineSpaceObject(CONSTLIT("aObjDestroyed"), Ctx.pObj);
		CCCtx.DefineSpaceObject(CONSTLIT("aDestroyer"), Ctx.Attacker.GetObj());
		CCCtx.DefineSpaceObject(CONSTLIT("aOrderGiver"), Ctx.GetOrderGiver());
		CCCtx.DefineSpaceObject(CONSTLIT("aWreckObj"), Ctx.pWreck);
		CCCtx.DefineBool(CONSTLIT("aDestroy"), Ctx.WasDestroyed());
		CCCtx.DefineString(CONSTLIT("aDestroyReason"), GetDestructionName(Ctx.iCause));

		ICCItem *pResult = CCCtx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_OBJ_DESTROYED_EVENT, pResult);
		CCCtx.Discard(pResult);
		}
	}

void CSpaceObject::FireOnObjDocked (CSpaceObject *pObj, CSpaceObject *pDockTarget)

//	FireOnObjDocked
//
//	Fire OnObjDocked event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_OBJ_DOCKED_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
		Ctx.DefineSpaceObject(CONSTLIT("aObjDocked"), pObj);
		Ctx.DefineSpaceObject(CONSTLIT("aDockTarget"), pDockTarget);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_OBJ_DOCKED_EVENT, pResult);
		Ctx.Discard(pResult);
		}
	}

void CSpaceObject::FireOnObjEnteredGate (CSpaceObject *pObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate)

//	FireOnObjEnteredGate
//
//	Fire OnObjEnteredGate event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_OBJ_ENTERED_GATE_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
		Ctx.DefineSpaceObject(CONSTLIT("aObj"), pObj);
		Ctx.DefineSpaceObject(CONSTLIT("aGateObj"), pStargate);
		Ctx.DefineString(CONSTLIT("aDestNodeID"), (pDestNode ? pDestNode->GetID() : NULL_STR));
		Ctx.DefineString(CONSTLIT("aDestEntryPoint"), sDestEntryPoint);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_OBJ_ENTERED_GATE_EVENT, pResult);
		Ctx.Discard(pResult);
		}
	}

bool CSpaceObject::FireOnObjGate (CSpaceObject *pObj)

//	FireOnObjGate
//
//	Fire OnObjGate event. Allows us to manipulate the object that gated. We
//	return TRUE if we handled gating. Otherwise, we return FALSE and the default
//	behavior is to destroy the object inside the gate.

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_OBJ_GATE_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
		Ctx.DefineSpaceObject(CONSTLIT("aObj"), pObj);

		bool bResult;
		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			{
			ReportEventError(ON_OBJ_GATE_EVENT, pResult);
			bResult = false;
			}
		else if (pResult->IsNil())
			bResult = false;
		else
			bResult = true;

		Ctx.Discard(pResult);

		return bResult;
		}

	return false;
	}

bool CSpaceObject::FireOnObjGateCheck (CSpaceObject *pObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate)

//	FireOnObjGateCheck
//
//	Fire OnObjGateCheck event. Returns TRUE if we allow pObj to gate through pStargate.

	{
	SEventHandlerDesc Event;
	if (!FindEventHandler(ON_OBJ_GATE_CHECK_EVENT, &Event))
		return true;

	//	Run

	CCodeChainCtx Ctx(GetUniverse());
	Ctx.DefineContainingType(this);
	Ctx.SaveAndDefineSourceVar(this);
	Ctx.DefineSpaceObject(CONSTLIT("aObj"), pObj);
	Ctx.DefineSpaceObject(CONSTLIT("aGateObj"), pStargate);
	Ctx.DefineString(CONSTLIT("aDestNodeID"), (pDestNode ? pDestNode->GetID() : NULL_STR));
	Ctx.DefineString(CONSTLIT("aDestEntryPoint"), sDestEntryPoint);

	//	Handle result

	ICCItemPtr pResult = Ctx.RunCode(Event);
	if (pResult->IsError())
		{
		ReportEventError(ON_OBJ_ENTERED_GATE_EVENT, pResult);
		return false;
		}
	else if (pResult->IsNil())
		return false;
	else
		return true;
	}

void CSpaceObject::FireOnObjJumped (CSpaceObject *pObj)

//	FireOnObjJumped
//
//	Fire OnObjJumped event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_OBJ_JUMPED_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
		Ctx.DefineSpaceObject(CONSTLIT("aObj"), pObj);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_OBJ_JUMPED_EVENT, pResult);
		Ctx.Discard(pResult);
		}
	}

bool CSpaceObject::FireOnObjJumpPosAdj (CSpaceObject *pObj, CVector *iovPos)

//	FireOnObjJumpPosAdj
//
//	Fires an event to adjust the position of an object that jumped
//	Returns TRUE if the event adjusted the position

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_OBJ_JUMP_POS_ADJ_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
		Ctx.DefineSpaceObject(CONSTLIT("aObj"), pObj);
		Ctx.DefineVector(CONSTLIT("aJumpPos"), *iovPos);

		ICCItem *pResult = Ctx.Run(Event);

		if (pResult->IsError())
			{
			ReportEventError(ON_OBJ_JUMP_POS_ADJ_EVENT, pResult);
			Ctx.Discard(pResult);
			return false;
			}
		else if (pResult->IsNil())
			{
			Ctx.Discard(pResult);
			return false;
			}
		else
			{
			CVector vNewPos = Ctx.AsVector(pResult);
			Ctx.Discard(pResult);

			if (vNewPos == *iovPos)
				return false;

			*iovPos = vNewPos;
			return true;
			}
		}

	return false;
	}

void CSpaceObject::FireOnObjReconned (CSpaceObject *pObj)

//	FireOnObjReconned
//
//	Fire OnObjReconned event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_OBJ_RECONNED_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
		Ctx.DefineSpaceObject(CONSTLIT("aObj"), pObj);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_OBJ_RECONNED_EVENT, pResult);
		Ctx.Discard(pResult);
		}
	}

void CSpaceObject::FireOnOrderChanged (void)

//	FireOnOrderChanged
//
//	Fire OnOrderChanged event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_ORDER_CHANGED_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_ORDER_CHANGED_EVENT, pResult);
		Ctx.Discard(pResult);
		}
	}

void CSpaceObject::FireOnOrdersCompleted (void)

//	FireOnOrdersCompleted
//
//	Fire OnOrdersCompleted event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_ORDERS_COMPLETED_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_ORDERS_COMPLETED_EVENT, pResult);
		Ctx.Discard(pResult);
		}
	}

void CSpaceObject::FireOnPlayerBlacklisted (void)

//	FireOnPlayerBlacklisted
//
//	Fire OnPlayerBlacklisted event

	{
	SEventHandlerDesc Event;

	//	Fire an event for ourselves

	if (FindEventHandler(ON_PLAYER_BLACKLISTED_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_PLAYER_BLACKLISTED_EVENT, pResult);

		Ctx.Discard(pResult);
		}

	//	Now fire an event for all subscribers

	m_SubscribedObjs.NotifyOnPlayerBlacklisted(this);
	}

void CSpaceObject::FireOnPlayerEnteredShip (CSpaceObject *pOldShip)

//	FireOnPlayerEnteredShip
//
//	Fire OnPlayerEnteredShip event

	{
	SEventHandlerDesc Event;

	//	Fire an event for ourselves

	if (FindEventHandler(ON_PLAYER_ENTERED_SHIP_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
        Ctx.DefineSpaceObject(CONSTLIT("aOldShip"), pOldShip);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_PLAYER_ENTERED_SHIP_EVENT, pResult);

		Ctx.Discard(pResult);
		}
	}

CSpaceObject::InterSystemResults CSpaceObject::FireOnPlayerEnteredSystem (CSpaceObject *pPlayer)

//	FireOnPlayerEnteredSystem
//
//	Fire OnPlayerEnteredSystem event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_PLAYER_ENTERED_SYSTEM_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_PLAYER_ENTERED_SYSTEM_EVENT, pResult);

		InterSystemResults iResult = GetInterSystemResult(pResult->GetStringValue());
		Ctx.Discard(pResult);

		return iResult;
		}

	return interNoAction;
	}

void CSpaceObject::FireOnPlayerLeftShip (CSpaceObject *pNewShip)

//  FireOnPlayerLeftShip
//
//	Fire OnPlayerLeftShip event

	{
	SEventHandlerDesc Event;

	//	Fire an event for ourselves

	if (FindEventHandler(ON_PLAYER_LEFT_SHIP_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
        Ctx.DefineSpaceObject(CONSTLIT("aNewShip"), pNewShip);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_PLAYER_LEFT_SHIP_EVENT, pResult);

		Ctx.Discard(pResult);
		}
	}

CSpaceObject::InterSystemResults CSpaceObject::FireOnPlayerLeftSystem (CSpaceObject *pPlayer, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate)

//	FireOnPlayerLeftSystem
//
//	Fire OnPlayerLeftSystem event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_PLAYER_LEFT_SYSTEM_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
		Ctx.DefineSpaceObject(CONSTLIT("aGateObj"), pStargate);
		Ctx.DefineString(CONSTLIT("aDestNodeID"), (pDestNode ? pDestNode->GetID() : NULL_STR));
		Ctx.DefineString(CONSTLIT("aDestEntryPoint"), sDestEntryPoint);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_PLAYER_LEFT_SYSTEM_EVENT, pResult);

		InterSystemResults iResult = GetInterSystemResult(pResult->GetStringValue());
		Ctx.Discard(pResult);

		return iResult;
		}

	return interNoAction;
	}

void CSpaceObject::FireOnSelected (void)

//	FireOnSelected
//
//	Fire OnSelected event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_SELECTED_EVENT, &Event))
		{
		CCodeChainCtx CCCtx(GetUniverse());
		CCCtx.DefineContainingType(this);
		CCCtx.SaveAndDefineSourceVar(this);
		CCCtx.DefineInteger(CONSTLIT("aPlayer"), g_PlayerSovereignUNID);

		//	Run code

		ICCItem *pResult = CCCtx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_SELECTED_EVENT, pResult);
		CCCtx.Discard(pResult);
		}
	}

bool CSpaceObject::FireOnSubordinateAttacked (const SDamageCtx &Ctx)

//	FireOnSubordinateAttacked
//
//	Fire OnSubordinateAttacked event. Return TRUE if we handle it (and we should 
//	skip the default action).

	{
	SEventHandlerDesc Event;

	bool bHandled;
	if (bHandled = (HasOnSubordinateAttackedEvent() && FindEventHandler(ON_SUBORDINATE_ATTACKED_EVENT, &Event)))
		{
		CCodeChainCtx CCCtx(GetUniverse());
		CCCtx.DefineContainingType(this);
		CCCtx.SaveAndDefineSourceVar(this);
		CCCtx.DefineDamageCtx(Ctx);
		CCCtx.DefineSpaceObject(CONSTLIT("aObjAttacked"), Ctx.pObj);

		ICCItem *pResult = CCCtx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_SUBORDINATE_ATTACKED_EVENT, pResult);
		else if (pResult->IsNil())
			bHandled = false;

		//	Done

		CCCtx.Discard(pResult);
		}

	return bHandled;
	}

void CSpaceObject::FireOnSystemExplosion (CSpaceObject *pExplosion, CSpaceObject *pSource, DWORD dwItemUNID)

//	FireOnSystemExplosion
//
//	Fire OnSystemExplosion event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_SYSTEM_EXPLOSION_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
		Ctx.DefineSpaceObject(CONSTLIT("aExplosionObj"), pSource);
		Ctx.DefineInteger(CONSTLIT("aExplosionUNID"), dwItemUNID);
		Ctx.DefineVector(CONSTLIT("aExplosionPos"), pExplosion->GetPos());

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_SYSTEM_EXPLOSION_EVENT, pResult);
		Ctx.Discard(pResult);
		}
	}

void CSpaceObject::FireOnSystemObjAttacked (SDamageCtx &Ctx)

//	FireOnSystemObjAttacked
//
//	Fire OnSystemObjAttacked event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(CDesignType::evtOnSystemObjAttacked, &Event))
		{
		CCodeChainCtx CCCtx(GetUniverse());
		CCCtx.DefineContainingType(this);
		CCCtx.SaveAndDefineSourceVar(this);
		CCCtx.DefineDamageCtx(Ctx);
		CCCtx.DefineSpaceObject(CONSTLIT("aObjAttacked"), Ctx.pObj);

		ICCItem *pResult = CCCtx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_ATTACKED_EVENT, pResult);

		//	Done

		CCCtx.Discard(pResult);
		}
	}

void CSpaceObject::FireOnSystemObjDestroyed (SDestroyCtx &Ctx)

//	FireOnSystemObjDestroyed
//
//	Fire OnSystemObjDestroyed event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_SYSTEM_OBJ_DESTROYED_EVENT, &Event))
		{
		CCodeChainCtx CCCtx(GetUniverse());
		CCCtx.DefineContainingType(this);
		CCCtx.SaveAndDefineSourceVar(this);
		CCCtx.DefineSpaceObject(CONSTLIT("aObjDestroyed"), Ctx.pObj);
		CCCtx.DefineSpaceObject(CONSTLIT("aDestroyer"), Ctx.Attacker.GetObj());
		CCCtx.DefineSpaceObject(CONSTLIT("aOrderGiver"), Ctx.GetOrderGiver());
		CCCtx.DefineSpaceObject(CONSTLIT("aWreckObj"), Ctx.pWreck);
		CCCtx.DefineBool(CONSTLIT("aDestroy"), Ctx.WasDestroyed());
		CCCtx.DefineString(CONSTLIT("aDestroyReason"), GetDestructionName(Ctx.iCause));

		ICCItem *pResult = CCCtx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_OBJ_DESTROYED_EVENT, pResult);
		CCCtx.Discard(pResult);
		}
	}

void CSpaceObject::FireOnSystemStarted (DWORD dwElapsedTime)

//	FireOnSystemStarted
//
//	Fire OnSystemStarted event

	{
	SEventHandlerDesc Event;
	if (!FindEventHandler(CDesignType::evtOnSystemStarted, &Event))
		return;

	CCodeChainCtx Ctx(GetUniverse());
	Ctx.DefineContainingType(this);
	Ctx.SaveAndDefineSourceVar(this);
	Ctx.DefineInteger(CONSTLIT("aElapsedTime"), dwElapsedTime);

	ICCItemPtr pResult = Ctx.RunCode(Event);
	if (pResult->IsError())
		ReportEventError(ON_SYSTEM_STARTED_EVENT, pResult);
	}

void CSpaceObject::FireOnSystemStopped (void)

//	FireOnSystemStopped
//
//	Fire OnSystemStopped event

	{
	SEventHandlerDesc Event;
	if (!FindEventHandler(CDesignType::evtOnSystemStopped, &Event))
		return;

	CCodeChainCtx Ctx(GetUniverse());
	Ctx.DefineContainingType(this);
	Ctx.SaveAndDefineSourceVar(this);

	ICCItemPtr pResult = Ctx.RunCode(Event);
	if (pResult->IsError())
		ReportEventError(ON_SYSTEM_STOPPED_EVENT, pResult);
	}

void CSpaceObject::FireOnSystemWeaponFire (CSpaceObject *pShot, CSpaceObject *pSource, DWORD dwItemUNID, int iRepeatingCount)

//	FireOnSystemWeaponFire
//
//	Fire OnSystemWeaponFire event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(CDesignType::evtOnSystemWeaponFire, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
		Ctx.DefineInteger(CONSTLIT("aFireRepeat"), iRepeatingCount);
		Ctx.DefineSpaceObject(CONSTLIT("aShotObj"), pShot);
		Ctx.DefineSpaceObject(CONSTLIT("aWeaponObj"), pSource);
		Ctx.DefineInteger(CONSTLIT("aWeaponUNID"), dwItemUNID);
		Ctx.DefineVector(CONSTLIT("aWeaponPos"), pShot->GetPos());
		Ctx.DefineItemType(CONSTLIT("aWeaponType"), (pShot->GetWeaponFireDesc() ? pShot->GetWeaponFireDesc()->GetWeaponType() : GetUniverse().GetItemType(dwItemUNID)));

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_SYSTEM_WEAPON_FIRE_EVENT, pResult);
		Ctx.Discard(pResult);
		}
	}

bool CSpaceObject::FireOnTranslateMessage (const CString &sMessage, CString *retsMessage)

//	FireOnTranslateMessage
//
//	Fire OnTranslateMessage event

	{
	bool bHandled = false;
	CString sResult;
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_TRANSLATE_MESSAGE_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
		Ctx.DefineString(CONSTLIT("aMessage"), sMessage);

		ICCItem *pResult = Ctx.Run(Event);
		if (!pResult->IsNil())
			{
			bHandled = true;
			sResult = pResult->GetStringValue();
			}
		Ctx.Discard(pResult);
		}

	if (retsMessage)
		*retsMessage = sResult;

	return bHandled;
	}

void CSpaceObject::FireOnUpdate (void)

//	FireOnUpdate
//
//	Fire OnUpdate event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(CDesignType::evtOnUpdate, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_UPDATE_EVENT, pResult);
		Ctx.Discard(pResult);
		}
	}

void CSpaceObject::GetBoundingRect (CVector *retvUR, CVector *retvLL)

//	GetBoundingRect
//
//	Returns the bounding RECT of the object image centered on the object position

	{
	CVector vDiag(m_rBoundsX, m_rBoundsY);
	*retvUR = m_vPos + vDiag;
	*retvLL = m_vPos - vDiag;
	}

CCommunicationsHandler *CSpaceObject::GetCommsHandler (void)

//	GetCommsHandler
//
//	Returns the comms handler for the object

	{
	CCommunicationsHandler *pHandler;

	if (m_pOverride && (pHandler = m_pOverride->GetCommsHandler()))
		return pHandler;

	CDesignType *pType = GetType();
	if (pType && (pHandler = pType->GetCommsHandler()))
		return pHandler;

	return NULL;
	}

int CSpaceObject::GetCommsMessageCount (void)

//	GetCommsMessageCount
//
//	Returns the number of messages that his object understands

	{
	CCommunicationsHandler *pHandler = GetCommsHandler();
	if (pHandler == NULL)
		return 0;
	else
		return pHandler->GetCount();
	}

bool CSpaceObject::GetCondition (CConditionSet::ETypes iCondition) const

//	GetCondition
//
//	Returns TRUE if the object is in the given condition.

	{
	//	We handle certain conditions.

	switch (iCondition)
		{
		case CConditionSet::cndTimeStopped:
			if (m_fTimeStop)
				return true;
			break;
		}

	//	See if an overlay imparts this condition

	const COverlayList *pList = GetOverlays();
	if (pList && pList->GetConditions().IsSet(iCondition))
		return true;

	//	Finally, let the sub-class handle it.

	return OnGetCondition(iCondition);
	}

CConditionSet CSpaceObject::GetConditions (void) const

//	GetConditions
//
//	Returns the set of all conditions.

	{
	int i;

	CConditionSet Conditions;
	DWORD dwFlag = 1;
	for (i = 0; i < CConditionSet::cndCount; i++)
		{
		if (GetCondition((CConditionSet::ETypes)dwFlag))
			Conditions.Set((CConditionSet::ETypes)dwFlag);

		dwFlag = dwFlag << 1;
		}

	return Conditions;
	}

int CSpaceObject::GetDataInteger (const CString &sAttrib) const

//	GetDataInteger
//
//	Get integer value

	{
	ICCItemPtr pValue = GetData(sAttrib);
	return pValue->GetIntegerValue();
	}

CDesignCollection &CSpaceObject::GetDesign (void) const

//	GetDesign
//
//	Returns the design collection.

	{
	return GetUniverse().GetDesignCollection();
	}

CString CSpaceObject::GetDesiredCommsKey (void) const

//	GetDesiredCommsKey
//
//	Returns the key that we want to use for comms. If NULL_STR then we will use
//	the default.

	{
	if (m_iDesiredHighlightChar == 0)
		return NULL_STR;

	char chChar = (char)m_iDesiredHighlightChar;
	return CString(&chChar, 1);
	}

Metric CSpaceObject::GetDetectionRange2 (int iPerception) const

//	GetDetectionRange2
//
//	Returns the square of the range at which this object can be detected by
//	the given perception.

	{
	Metric rRange = CPerceptionCalc::GetRange(GetDetectionRangeIndex(iPerception));
	return rRange * rRange;
	}

int CSpaceObject::GetDetectionRangeIndex (int iPerception) const

//	GetDetectionRangeIndex
//
//	Returns the range index at which this object can be detected by
//	the given perception.

	{
	return CPerceptionCalc::GetRangeIndex(GetStealth(), iPerception);
	}

CSovereign::Disposition CSpaceObject::GetDispositionTowards (CSpaceObject *pObj)

//	GetDispositionTowards
//
//	Returns the disposition of this objects towards the given object

	{
	CSovereign *pOurSovereign = GetSovereign();
	if (pOurSovereign)
		return pOurSovereign->GetDispositionTowards(pObj->GetSovereign());
	else
		return CSovereign::dispFriend;
	}

CDesignType *CSpaceObject::GetFirstDockScreen (CString *retsScreen, ICCItemPtr *retpData)

//	GetFirstDockScreen
//
//	Returns the dock screen to show when the player docks with
//	this object.
//
//	NOTE: Caller must discard *retpData.

	{
	//	First see if any global types override this

	CDockScreenSys::SSelector Screen;
	if (!GetDesign().FireGetGlobalDockScreen(this, 0, &Screen))
		Screen.iPriority = -1;

	//	See if any overlays have dock screens

	COverlayList *pOverlays;
	if (pOverlays = GetOverlays())
		{
		CDockScreenSys::SSelector OverlayScreen;
		if (pOverlays->FireGetDockScreen(this, &OverlayScreen)
				&& OverlayScreen.iPriority > Screen.iPriority)
			Screen = OverlayScreen;
		}

	//	Next see if we have an event that handles this

	CDockScreenSys::SSelector CustomScreen;
	if (FireGetDockScreen(&CustomScreen)
			&& CustomScreen.iPriority > Screen.iPriority)
		Screen = CustomScreen;

	//	If an event has overridden the dock screen, then resolve
	//	the screen now.

	if (Screen.iPriority != -1)
		{
		CDesignType *pScreen = GetDesign().ResolveDockScreen(GetType(), Screen.sScreen, retsScreen);
		if (pScreen)
			{
			if (retpData)
				*retpData = Screen.pData;

			return pScreen;
			}
		else
			{
			::kernelDebugLogPattern("Unable to resolve screen: %s", Screen.sScreen);
			}
		}

	//	Otherwise, we return the default screen associated with the object

	if (retpData)
		*retpData = NULL;

	return GetDefaultDockScreen(retsScreen);
	}

ICCItemPtr CSpaceObject::GetGlobalData (const CString &sAttribute) const

//	GetGlobalData
//
//	Returns data from the type

	{
	CDesignType *pType = GetType();
	if (pType == NULL)
		return ICCItemPtr(ICCItem::Nil);

	return pType->GetGlobalData(sAttribute);
	}

void CSpaceObject::GetHitRect (CVector *retvUR, CVector *retvLL) const

//	GetHitRect
//
//	Returns the RECT that bounds the object's hit size.

	{
	Metric rHalfSize = 0.5 * GetHitSize();
	CVector vDiag(rHalfSize, rHalfSize);
	*retvUR = m_vPos + vDiag;
	*retvLL = m_vPos - vDiag;
	}

int CSpaceObject::GetHitSizePixels (void) const

//	GetHitSizePixels
//
//	Returns the size of the object (in pixels) for purposes of determining the
//	size that can be hit. This is much larger than the real size of the	object
//	(since object images are greatly magnified) but it is less than the	object 
//	bounds, which includes engine effects.

	{
	const CObjectImageArray &Image = GetImage();
	if (Image.IsEmpty())
		return 32;

	const RECT &rcRect = Image.GetImageRect();
	return Max(RectWidth(rcRect), RectHeight(rcRect));
	}

Metric CSpaceObject::GetMaxGateDist2 (void) const

//	GetMaxGateDist2
//
//	Returns the maximum distance at which we can gate through this object.

	{
	Metric rHitSize = GetHitSize();
	return (0.25 * rHitSize * rHitSize);
	}

const CObjectImageArray &CSpaceObject::GetImage (int *retiRotationFrameIndex) const

//	GetImage
//
//	Returns the image for the object

	{
	static CObjectImageArray NullImage;

	if (retiRotationFrameIndex)
		*retiRotationFrameIndex = 0;

	return NullImage;
	}

int CSpaceObject::GetImageScale (void) const

//	GetImageScale
//
//	Returns the scale.

	{
	const CObjectImageArray &Image = GetImage();
	if (Image.IsEmpty())
		return 512;	//	Default

	return Image.GetImageViewportSize();
	}

int CSpaceObject::GetNearestDockPort (CSpaceObject *pRequestingObj, CVector *retvPort)

//	GetNearestDockPort
//
//	Returns the index of the nearest dock point (or -1)
	
	{
	CDockingPorts *pPorts = GetDockingPorts();
	if (pPorts == NULL)
		return -1;

	int iPort = pPorts->FindNearestEmptyPort(this, pRequestingObj);

	if (retvPort)
		*retvPort = pPorts->GetPortPos(this, iPort, pRequestingObj);

	return iPort;
	}

CSpaceObject *CSpaceObject::GetNearestEnemyStation (Metric rMaxRange)

//	GetNearestEnemyStation
//
//	Returns the nearest enemy station that could threaten the object

	{
	DEBUG_TRY

	int i;

	//	Get the sovereign

	CSovereign *pSovereign = GetSovereign();
	if (pSovereign == NULL || GetSystem() == NULL)
		return NULL;

	//	Get the list of enemy objects

	const CSpaceObjectList &ObjList = pSovereign->GetEnemyObjectList(GetSystem());

	//	Start a max range

	Metric rBestDist = rMaxRange * rMaxRange;
	CSpaceObject *pBestObj = NULL;

	//	Loop for all enemy objects

	int iCount = ObjList.GetCount();
	for (i = 0; i < iCount; i++)
		{
		CSpaceObject *pObj = ObjList.GetObj(i);

		if ((pObj->GetCategory() == catStation)
				&& pObj->CanAttack()
				&& pObj != this)
			{
			CVector vDist = GetPos() - pObj->GetPos();
			Metric rDist = vDist.Length2();

			if (rDist < rBestDist)
				{
				rBestDist = rDist;
				pBestObj = pObj;
				}
			}
		}

	return pBestObj;

	DEBUG_CATCH
	}

CSpaceObject *CSpaceObject::GetNearestStargate (bool bExcludeUncharted)

//	GetNearestStargate
//
//	Returns the nearest stargate

	{
	int i;
	Metric rNearestGateDist2 = (g_InfiniteDistance * g_InfiniteDistance);
	CSpaceObject *pNearestGate = NULL;
	for (i = 0; i < GetSystem()->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetSystem()->GetObject(i);

		if (pObj 
				&& pObj->IsActiveStargate()
				//	Do not include uncharted stargates (such as Huaramarca gate)
				&& (!bExcludeUncharted || !pObj->HasAttribute(STR_UNCHARTED)))
			{
			Metric rDist2 = (GetPos() - pObj->GetPos()).Length2();
			if (rDist2 < rNearestGateDist2)
				{
				rNearestGateDist2 = rDist2;
				pNearestGate = pObj;
				}
			}
		}

	return pNearestGate;
	}

int CSpaceObject::GetNearestVisibleEnemies (int iMaxEnemies, 
											Metric rMaxDist, 
											TArray<CSpaceObject *> *pretList, 
											CSpaceObject *pExcludeObj,
											DWORD dwFlags)

//	GetNearestVisibleEnemies
//
//	Returns a list of the nearest n enemies visible to this object. The targets
//	are added in ascending order of distance.
//
//	NOTE: We append to the list because callers may have added their own.

	{
	CSystem *pSystem = GetSystem();
	if (pSystem == NULL)
		return NULL;

	CNearestInRadiusRange Range(GetPos(), rMaxDist);

	if (dwFlags & FLAG_INCLUDE_NON_AGGRESSORS)
		{
		CVisibleObjSelector Selector(*this);
		Selector.SetExcludeObj(pExcludeObj);
		if (dwFlags & FLAG_INCLUDE_MISSILES)
			Selector.SetIncludeMissiles();

		return CSpaceObjectEnum::FindNearestEnemyObjs(*pSystem, *this, Range, Selector, *pretList, iMaxEnemies);
		}
	else
		{
		CVisibleAggressorObjSelector Selector(*this);
		Selector.SetExcludeObj(pExcludeObj);
		if (dwFlags & FLAG_INCLUDE_MISSILES)
			Selector.SetIncludeMissiles();

		return CSpaceObjectEnum::FindNearestEnemyObjs(*pSystem, *this, Range, Selector, *pretList, iMaxEnemies);
		}
	}

CSpaceObject *CSpaceObject::GetNearestVisibleEnemy (Metric rMaxRange, bool bIncludeStations, CSpaceObject *pExcludeObj)

//	GetNearestVisibleEnemy
//
//	Returns the nearest enemy that is visible to the given center point.

	{
	CSystem *pSystem = GetSystem();
	if (pSystem == NULL)
		return NULL;

	CVisibleObjSelector Selector(*this);
	Selector.SetIncludeStations(bIncludeStations);
	Selector.SetExcludeObj(pExcludeObj);

	CNearestInRadiusRange Range(GetPos(), rMaxRange);

	return CSpaceObjectEnum::FindNearestEnemyObj(*pSystem, *this, Range, Selector);
	}

CSpaceObject *CSpaceObject::GetNearestVisibleEnemyInArc (int iMinFireArc, int iMaxFireArc, Metric rMaxRange, bool bIncludeStations, CSpaceObject *pExcludeObj)

//	GetNearestVisibleEnemy
//
//	Returns the nearest enemy that is visible to us and within the given fire
//	arc.

	{
	CSystem *pSystem = GetSystem();
	if (pSystem == NULL)
		return NULL;

	CVisibleObjSelector Selector(*this);
	Selector.SetIncludeStations(bIncludeStations);
	Selector.SetExcludeObj(pExcludeObj);

	CNearestInArcAndRadiusRange Range(GetPos(), rMaxRange, iMinFireArc, iMaxFireArc);

	return CSpaceObjectEnum::FindNearestEnemyObj(*pSystem, *this, Range, Selector);
	}

CString CSpaceObject::GetNounPhrase (DWORD dwFlags) const

//	GetNounPhrase
//
//	Get the name of the object as a noun phrase modified by the
//	given flags

	{
	//	Get the name and modifiers from the actual object

	DWORD dwNounFlags;
	CString sName = GetNamePattern(dwFlags, &dwNounFlags);

	return CLanguage::ComposeNounPhrase(sName, 1, CString(), dwNounFlags, dwFlags);
	}

CSpaceObject *CSpaceObject::GetOrderGiver (DestructionTypes iCause)

//	GetOrderGiver
//
//	Returns the object that is responsible for this object's attack

	{
	if (iCause == killedByPlayerCreatedExplosion)
		{
		CSpaceObject *pPlayerShip = GetPlayerShip();
		if (pPlayerShip)
			return pPlayerShip;
		else
			return this;
		}
	else
		return OnGetOrderGiver();
	}

ICCItemPtr CSpaceObject::GetOverlayData (DWORD dwID, const CString &sAttrib) const

//	GetOverlayData
//
//	Returns data for overlay

	{
	const COverlayList *pOverlays = GetOverlays();
	if (pOverlays == NULL)
		return ICCItemPtr(GetUniverse().GetCC().CreateNil());

	return pOverlays->GetData(dwID, sAttrib);
	}

ICCItem *CSpaceObject::GetOverlayProperty (CCodeChainCtx *pCCCtx, DWORD dwID, const CString &sName)

//	GetOverlayProperty
//
//	Returns a property

	{
	COverlayList *pOverlays = GetOverlays();
	if (pOverlays)
		return pOverlays->GetProperty(pCCCtx, this, dwID, sName);
	else
		{
		CCodeChain &CC = GetUniverse().GetCC();
		return CC.CreateNil();
		}
	}

bool CSpaceObject::GetPlayerDestinationOptions (SPlayerDestinationOptions &retOptions) const

//	GetPlayerDestinationOptions
//
//	If this option is a player destination, we return TRUE and fill in 
//	retOptions appropriately. Otherwise, we return FALSE.

	{
	if (!m_fPlayerDestination)
		return false;

	retOptions.bShowDistanceAndBearing = (m_fShowDistanceAndBearing ? true : false);
	retOptions.bAutoClearDestination = (m_fAutoClearDestination ? true : false);
	retOptions.bAutoClearOnDestroy = (m_fAutoClearDestinationOnDestroy ? true : false);
	retOptions.bAutoClearOnDock = (m_fAutoClearDestinationOnDock ? true : false);
	retOptions.bAutoClearOnGate = (m_fAutoClearDestinationOnGate ? true : false);
	retOptions.bShowHighlight = (m_fShowHighlight ? true : false);

	return true;
	}

ICCItem *CSpaceObject::GetProperty (CCodeChainCtx &Ctx, const CString &sName)

//	GetProperty
//
//	Returns the property

	{
	int i;
	CCodeChain &CC = GetUniverse().GetCC();
	CDesignType *pType;

	if (strEquals(sName, PROPERTY_ASCENDED))
		return CC.CreateBool(IsAscended());

	else if (strEquals(sName, PROPERTY_CATEGORY))
		{
		switch (GetCategory())
			{
			case catShip:
				return CC.CreateString(CATEGORY_SHIP);

			case catStation:
				return CC.CreateString(CATEGORY_STATION);

			case catBeam:
				return CC.CreateString(CATEGORY_BEAM);

			case catMissile:
				return CC.CreateString(CATEGORY_MISSILE);

			case catMission:
				return CC.CreateString(CATEGORY_MISSION);

			case catMarker:
				return CC.CreateString(CATEGORY_MARKER);

			default:
				return CC.CreateString(CATEGORY_EFFECT);
			}
		}
	else if (strEquals(sName, PROPERTY_COMMS_KEY))
		{
		if (m_iHighlightChar)
			{
			char chChar = m_iHighlightChar;
			return CC.CreateString(CString(&chChar, 1));
			}
		else if (m_iDesiredHighlightChar)
			{
			char chChar = m_iHighlightChar;
			return CC.CreateString(CString(&chChar, 1));
			}
		else
			return CC.CreateNil();
		}
	else if (strEquals(sName, PROPERTY_CURRENCY))
		return CC.CreateInteger(GetDefaultEconomy()->GetUNID());

	else if (strEquals(sName, PROPERTY_CURRENCY_NAME))
		return CC.CreateString(GetDefaultEconomy()->GetSID());

	else if (strEquals(sName, PROPERTY_CYBER_DEFENSE_LEVEL))
		return CC.CreateInteger(GetCyberDefenseLevel());

	else if (strEquals(sName, PROPERTY_DAMAGE_DESC))
		{
		ICCItem *pResult = CC.CreateSymbolTable();
		SVisibleDamage Damage;
		GetVisibleDamageDesc(Damage);

		if (Damage.iShieldLevel != -1)
			pResult->SetIntegerAt(FIELD_SHIELD_LEVEL, Damage.iShieldLevel);

		if (Damage.iArmorLevel != -1)
			pResult->SetIntegerAt(FIELD_ARMOR_INTEGRITY, Damage.iArmorLevel);

		if (Damage.iHullLevel != -1)
			pResult->SetIntegerAt(FIELD_HULL_INTEGRITY, Damage.iHullLevel);

		return pResult;
		}

	else if (strEquals(sName, PROPERTY_DESTINY))
		return CC.CreateInteger(GetDestiny());

	else if (strEquals(sName, PROPERTY_DOCKING_PORTS))
		{
		CDockingPorts *pPorts = GetDockingPorts();
		if (pPorts == NULL || pPorts->GetPortCount(this) == 0)
			return CC.CreateNil();

		ICCItem *pList = CC.CreateLinkedList();
		for (i = 0; i < pPorts->GetPortCount(this); i++)
			{
			ICCItem *pPortDesc = CC.CreateSymbolTable();

			//	Status

			if (pPorts->IsPortEmpty(this, i))
				pPortDesc->SetStringAt(FIELD_STATUS, CONSTLIT("empty"));
			else
				pPortDesc->SetStringAt(FIELD_STATUS, CONSTLIT("inUse"));

			//	Position

			ICCItem *pValue = ::CreateListFromVector(pPorts->GetPortPos(this, i, NULL));
			pPortDesc->SetAt(FIELD_POS, pValue);
			pValue->Discard();

			//	ObjectID

			CSpaceObject *pObj = pPorts->GetPortObj(this, i);
			if (pObj)
				pPortDesc->SetIntegerAt(FIELD_OBJ_ID, pObj->GetID());

			//	Add to list

			pList->Append(pPortDesc);
			pPortDesc->Discard();
			}

		return pList;
		}

	else if (strEquals(sName, PROPERTY_EVENT_SUBSCRIBERS))
		{
		ICCItem *pResult = CC.CreateLinkedList();
		for (int i = 0; i < m_SubscribedObjs.GetCount(); i++)
			{
			CSpaceObject *pObj = m_SubscribedObjs.GetObj(i);
			if (!pObj->IsDestroyed())
				pResult->AppendInteger((int)pObj);
			}

		if (pResult->GetCount() == 0)
			{
			pResult->Discard();
			return CC.CreateNil();
			}
		else
			return pResult;
		}

	else if (strEquals(sName, PROPERTY_HAS_DOCKING_PORTS))
		return CC.CreateBool(GetDockingPortCount() > 0);

	else if (strEquals(sName, PROPERTY_ID))
		return CC.CreateInteger(GetID());

	else if (strEquals(sName, PROPERTY_IDENTIFIED))
		return CC.CreateBool(IsIdentified());

	else if (strEquals(sName, PROPERTY_INSTALL_ARMOR_MAX_LEVEL))
		{
		int iMaxLevel = GetTradeMaxLevel(serviceReplaceArmor);
		return (iMaxLevel != -1 ? CC.CreateInteger(iMaxLevel) : CC.CreateNil());
		}

	else if (strEquals(sName, PROPERTY_INSTALL_DEVICE_MAX_LEVEL))
		{
		int iMaxLevel = GetTradeMaxLevel(serviceInstallDevice);
		return (iMaxLevel != -1 ? CC.CreateInteger(iMaxLevel) : CC.CreateNil());
		}

	else if (strEquals(sName, PROPERTY_INSTALL_DEVICE_UPGRADE_ONLY))
		return CC.CreateBool(HasTradeUpgradeOnly(serviceInstallDevice));

	else if (strEquals(sName, PROPERTY_KNOWN))
		return CC.CreateBool(IsKnown());

	else if (strEquals(sName, PROPERTY_LEVEL))
		return CC.CreateInteger(GetLevel());

	else if (strEquals(sName, PROPERTY_MASS))
		return CC.CreateInteger((int)GetMass());

    else if (strEquals(sName, PROPERTY_NAME_PATTERN))
		{
		ICCItem *pResult = CC.CreateSymbolTable();
		DWORD dwFlags;
		pResult->SetStringAt(CONSTLIT("pattern"), GetNamePattern(0, &dwFlags));
		pResult->SetIntegerAt(CONSTLIT("flags"), dwFlags);
		return pResult;
		}

	else if (strEquals(sName, PROPERTY_PAINT_LAYER))
		return CC.CreateString(GetPaintLayerID(GetPaintLayer()));

	else if (strEquals(sName, PROPERTY_PLAYER_MISSIONS_GIVEN))
		{
		int iCount = GetUniverse().GetObjStats(GetID()).iPlayerMissionsGiven;
		if (iCount > 0)
			return CC.CreateInteger(iCount);
		else
			return CC.CreateNil();
		}

	else if (strEquals(sName, PROPERTY_RADIOACTIVE))
		return CC.CreateBool(IsRadioactive());

	else if (strEquals(sName, PROPERTY_REFUEL_MAX_LEVEL))
		{
		int iMaxLevel = GetTradeMaxLevel(serviceRefuel);
		return (iMaxLevel != -1 ? CC.CreateInteger(iMaxLevel) : CC.CreateNil());
		}

	else if (strEquals(sName, PROPERTY_REMOVE_DEVICE_MAX_LEVEL))
		{
		int iMaxLevel = GetTradeMaxLevel(serviceRemoveDevice);
		return (iMaxLevel != -1 ? CC.CreateInteger(iMaxLevel) : CC.CreateNil());
		}

	else if (strEquals(sName, PROPERTY_REPAIR_ARMOR_MAX_LEVEL))
		{
		int iMaxLevel = GetTradeMaxLevel(serviceRepairArmor);
		return (iMaxLevel != -1 ? CC.CreateInteger(iMaxLevel) : CC.CreateNil());
		}

	else if (strEquals(sName, PROPERTY_SCALE))
		{
		switch (GetScale())
			{
			case scaleStar:
				return CC.CreateString(SCALE_STAR);

			case scaleWorld:
				return CC.CreateString(SCALE_WORLD);

			case scaleStructure:
				return CC.CreateString(SCALE_STATION);

			case scaleShip:
				return CC.CreateString(SCALE_SHIP);

			case scaleFlotsam:
				return CC.CreateString(SCALE_FLOTSAM);

			default:
				return CC.CreateNil();
			}
		}

	else if (strEquals(sName, PROPERTY_SHOW_AS_DESTINATION))
		{
		if (!IsPlayerDestination())
			return CC.CreateNil();

		ICCItemPtr pResult(ICCItem::SymbolTable);
		pResult->SetBooleanAt(CONSTLIT("showDestination"), true);

		if (m_fAutoClearDestination)			pResult->SetBooleanAt(CONSTLIT("autoClear"), true);
		if (m_fAutoClearDestinationOnDestroy)	pResult->SetBooleanAt(CONSTLIT("autoClearOnDestroy"), true);
		if (m_fAutoClearDestinationOnDock)		pResult->SetBooleanAt(CONSTLIT("autoClearOnDock"), true);
		if (m_fAutoClearDestinationOnGate)		pResult->SetBooleanAt(CONSTLIT("autoClearOnGate"), true);
		if (m_fShowDistanceAndBearing)			pResult->SetBooleanAt(CONSTLIT("showDistance"), true);
		if (m_fShowHighlight)					pResult->SetBooleanAt(CONSTLIT("showHighlight"), true);

		return pResult->Reference();
		}

	else if (strEquals(sName, PROPERTY_SIZE_PIXELS))
		return CC.CreateInteger(GetHitSizePixels());

	else if (strEquals(sName, PROPERTY_SOVEREIGN))
		{
		CSovereign *pSovereign = GetSovereign();
		if (pSovereign)
			return CC.CreateInteger(pSovereign->GetUNID());
		else
			return CC.CreateNil();
		}

	else if (strEquals(sName, PROPERTY_STEALTH))
		return CC.CreateInteger(GetStealth());

	else if (strEquals(sName, PROPERTY_SUSPENDED))
		return CC.CreateBool(IsSuspended());

	else if (strEquals(sName, PROPERTY_TYPE))
		{
		if (CDesignType *pType = GetType())
			return CC.CreateInteger(pType->GetUNID());
		else
			return CC.CreateNil();
		}

	else if (strEquals(sName, PROPERTY_UNDER_ATTACK))
		return CC.CreateBool(IsUnderAttack());

	else if (pType = GetType())
		{
		EPropertyType iType;
		ICCItemPtr pResult = pType->GetProperty(Ctx, sName, &iType);

		//	If the property is an object property, then we need to look in 
		//	object data.

		if (iType == EPropertyType::propVariant || iType == EPropertyType::propData)
			return GetData(sName)->Reference();

		//	If this is a dynamic property, we need to evaluate.

		else if (iType == EPropertyType::propDynamicData)
			{
			CCodeChainCtx Ctx(GetUniverse());
			Ctx.SaveAndDefineSourceVar(this);

			ICCItemPtr pValue = Ctx.RunCode(pResult);
			return pValue->Reference();
			}

		//	Otherwise we have a valid property.

		else
			return pResult->Reference();
		}

	else
		return CC.CreateNil();
	}

CXMLElement *CSpaceObject::GetScreen (const CString &sName)

//	GetScreen
//
//	Returns a screen object

	{
	CDockScreenTypeRef Screen;
	Screen.LoadUNID(sName);
	Screen.Bind(NULL);
	return Screen.GetDesc();
	}

CSovereign *CSpaceObject::GetSovereignToDefend (void) const

//	GetSovereignToDefend
//
//	Returns either our sovereign or the sovereign of the ship that we're
//	escorting (if we're escorting)

	{
	CSpaceObject *pPrincipal = GetEscortPrincipal();
	if (pPrincipal)
		return pPrincipal->GetSovereign();
	else
		return GetSovereign();
	}

ICCItemPtr CSpaceObject::GetStaticData (const CString &sAttrib)

//	GetStaticData
//
//	Returns static data

	{
	ICCItemPtr pData;

	//	Check our override

	if (m_pOverride && m_pOverride->FindStaticData(sAttrib, pData))
		return pData;

	//	Check our type

	CDesignType *pType = GetType();
	if (pType)
		return pType->GetStaticData(sAttrib);
	
	//	Not found

	return ICCItemPtr(GetUniverse().GetCC().CreateNil());
	}

CG32bitPixel CSpaceObject::GetSymbolColor (void)

//	GetSymbolColor
//
//	Returns the color to paint this object in the player's scanner

	{
	CSovereign *pPlayer = GetUniverse().GetPlayerSovereign();
	CSpaceObject *pPlayerShip;

	if (GetSovereign() == pPlayer)
		return CG32bitPixel(255, 255, 255);
	else if (IsWreck())
		return CG32bitPixel(0, 192, 0);
	else if ((pPlayerShip = GetUniverse().GetPlayerShip()) 
			&& IsAngryAt(pPlayerShip))
		return CG32bitPixel(255, 80, 80);
	else if (GetCategory() == CSpaceObject::catShip)
		return CG32bitPixel(80, 255, 80);
	else
		return CG32bitPixel(0, 192, 0);
	}

const CEnhancementDesc *CSpaceObject::GetSystemEnhancements (void) const

//	GetSystemEnhancements
//
//	Returns the list of system enhancements (or NULL if none).

	{
	CSystem *pSystem = GetSystem();
	if (pSystem == NULL)
		return NULL;

	CSystemType *pType = pSystem->GetType();
	if (pType == NULL)
		return NULL;

	const CEnhancementDesc &Enhancements = pType->GetItemEnhancements();
	if (Enhancements.IsEmpty())
		return NULL;

	return &Enhancements;
	}

const CImageFilterStack *CSpaceObject::GetSystemFilters (void) const

//	GetSystemFilters
//
//	Returns the paint filters to apply to this object.

	{
	CSystem *pSystem = GetSystem();
	if (pSystem == NULL)
		return NULL;

	CSystemType *pType = pSystem->GetType();
	if (pType == NULL)
		return NULL;

	if (!pType->GetImageFiltersCriteria().IsEmpty()
			&& !MatchesCriteria(pType->GetImageFiltersCriteria()))
		return NULL;

	const CImageFilterStack *pFilters = &pType->GetImageFilters();
	if (pFilters->IsEmpty())
		return NULL;

	return pFilters;
	}

void CSpaceObject::GetVisibleEnemies (DWORD dwFlags, TArray<CSpaceObject *> *retList, CSpaceObject *pExcludeObj)

//	GetVisibleEnemies
//
//	Returns a list of visible enemies.

	{
	int i;

	CVector vCenter = GetPos();

	//	Get the sovereign

	CSovereign *pSovereign = GetSovereignToDefend();
	if (pSovereign == NULL || GetSystem() == NULL)
		return;

	//	Include stations

	bool bIncludeStations = ((dwFlags & FLAG_INCLUDE_STATIONS) ? true : false);

	//	If a ship has fired its weapon after this time, then it counts
	//	as an aggressor

	int iAggressorThreshold;
	if (dwFlags & FLAG_INCLUDE_NON_AGGRESSORS)
		iAggressorThreshold = -1;
	else
		iAggressorThreshold = GetUniverse().GetTicks() - AGGRESSOR_THRESHOLD;

	//	Get the list of enemy objects

	const CSpaceObjectList &ObjList = pSovereign->GetEnemyObjectList(GetSystem());

	//	Compute this object's perception and perception range

	CPerceptionCalc Perception(GetPerception());

	//	Loop over all objects

	int iObjCount = ObjList.GetCount();
	for (i = 0; i < iObjCount; i++)
		{
		CSpaceObject *pObj = ObjList.GetObj(i);

		if ((pObj->GetCategory() == catShip
					|| (bIncludeStations && pObj->GetCategory() == catStation))
				&& pObj->CanAttack()
				&& !pObj->IsDestroyed()
				&& pObj != this)
			{
			CVector vDist = vCenter - pObj->GetPos();
			Metric rDist2 = vDist.Length2();

			if (Perception.CanBeTargeted(pObj, rDist2)
					&& pObj != pExcludeObj
					&& pObj->GetLastFireTime() > iAggressorThreshold
					&& !pObj->IsEscortingFriendOf(this))
				{
				retList->Insert(pObj);
				}
			}
		}
	}

CSpaceObject *CSpaceObject::GetVisibleEnemyInRange (CSpaceObject *pCenter, Metric rMaxRange, bool bIncludeStations, CSpaceObject *pExcludeObj)

//	GetVisibleEnemyInRange
//
//	Returns the first enemy that we find in range.

	{
	DEBUG_TRY_OBJ_LOOP

	int i;
	Metric rMaxRange2 = rMaxRange * rMaxRange;

	//	Compute this object's perception and perception range

	CPerceptionCalc Perception(GetPerception());

	//	The player is a special case (because sometimes a station is angry at the 
	//	player even though she is not an enemy)

	CSpaceObject *pPlayer = GetPlayerShip();
	if (pPlayer 
			&& pCenter->IsAngryAt(pPlayer)
			&& pPlayer != pExcludeObj
			&& !pPlayer->IsEscortingFriendOf(this))
		{
		CVector vRange = pPlayer->GetPos() - pCenter->GetPos();
		Metric rDistance2 = vRange.Dot(vRange);

		if (rDistance2 < rMaxRange2
				&& Perception.CanBeTargeted(pPlayer, rDistance2))
			return pPlayer;
		}

	//	Get the sovereign

	CSovereign *pSovereign = GetSovereignToDefend();
	if (pSovereign == NULL || GetSystem() == NULL)
		return NULL;

	//	Loop

	const CSpaceObjectList &ObjList = pSovereign->GetEnemyObjectList(GetSystem());
	int iCount = ObjList.GetCount();
	for (i = 0; i < iCount; i++)
		{
		pObj = ObjList.GetObj(i);

		if ((pObj->GetCategory() == catShip
					|| (bIncludeStations && pObj->GetCategory() == catStation))
				&& pObj->CanAttack()
				&& pObj != this)
			{
			CVector vRange = pObj->GetPos() - pCenter->GetPos();
			Metric rDistance2 = vRange.Dot(vRange);

			if (rDistance2 < rMaxRange2
					&& Perception.CanBeTargeted(pObj, rDistance2)
					&& pObj != pExcludeObj
					&& !pObj->IsEscortingFriendOf(this))
				return pObj;
			}
		}

	return NULL;

	DEBUG_CATCH_OBJ_LOOP
	}

bool CSpaceObject::HasBeenHitLately (int iTicks)

//	HasBeenHitLately
//
//	Returns TRUE if we've been hit in the last iTicks.

	{
	int iLastHit = GetLastHitTime();
	if (iLastHit == 0)
		return false;

	int iNow = GetUniverse().GetTicks();
	if ((iNow - iLastHit) <= iTicks)
		return true;

	return false;
	}

bool CSpaceObject::HasDockScreen (void) const

//	HasDockScreen
//
//	Returns TRUE if we have at least one dock screen. This should match ::GetFirstDockScreen()

	{
	if (GetDefaultDockScreen())
		return true;

	if (FireGetDockScreen())
		return true;

	const COverlayList *pOverlays;
	if ((pOverlays = GetOverlays()) 
			&& pOverlays->FireGetDockScreen(const_cast<CSpaceObject *>(this)))
		return true;

	//	If we still have no screens, we call <GetGlobalDockScreen>, but we're
	//	only interested in non-override screens. Override screens are screens
	//	like decontamination screens, which should only show up if the station
	//	or ship has other screens.
	//
	//	If we don't do this, then some ships would automatically get screens
	//	because we auto-create docking ports for ships if they have screens.

	if (GetDesign().FireGetGlobalDockScreen(this, CDesignCollection::FLAG_NO_OVERRIDE))
		return true;

	return false;
	}

bool CSpaceObject::HasFiredLately (int iTicks)

//	HasFiredHitLately
//
//	Returns TRUE if we've fired our weapons lately.

	{
	int iLastFire = GetLastFireTime();
	if (iLastFire == 0)
		return false;

	int iNow = GetUniverse().GetTicks();
	if ((iNow - iLastFire) <= iTicks)
		return true;

	return false;
	}

bool CSpaceObject::HasFuelItem (void)

//	HasFuelItem
//
//	Returns TRUE if the object has fuel on board

	{
	CItemListManipulator Search(GetItemList());
	while (Search.MoveCursorForward())
		{
		const CItem &Item = Search.GetItemAtCursor();
		if (Item.GetType()->IsFuel())
			return true;
		}

	return false;
	}

bool CSpaceObject::HasSpecialAttribute (const CString &sAttrib) const

//	HasSpecialAttribute
//
//	Returns TRUE if object has the special attribute
//
//	NOTE: Subclasses may override this, but they must call the
//	base class if they do not handle the attribute.

	{
	if (strStartsWith(sAttrib, SPECIAL_CHARACTER))
		{
		CString sCharacter = strSubString(sAttrib, SPECIAL_CHARACTER.GetLength());
		DWORD dwUNID = (DWORD)strToInt(sCharacter, 0);
		if (dwUNID == 0)
			return false;

		CDesignType *pCharacter = GetCharacter();
		if (pCharacter == NULL)
			return false;

		return (dwUNID == pCharacter->GetUNID());
		}
	else if (strStartsWith(sAttrib, SPECIAL_DATA))
		{
		CString sDataField = strSubString(sAttrib, SPECIAL_DATA.GetLength());
		return !m_Data.IsDataNil(sDataField);
		}
	else if (strStartsWith(sAttrib, SPECIAL_IS_PLANET))
		{
		CString sValue = strSubString(sAttrib, SPECIAL_IS_PLANET.GetLength());

		//	Figure out if we are a planet or not.

		const COrbit *pOrbit;
		bool bIsPlanet = (HasAttribute(CONSTLIT("planet"))
				||  (GetScale() == scaleWorld
					&& (GetPlanetarySize() >= MIN_PLANET_SIZE)
					&& (pOrbit = GetMapOrbit())
					&& (pOrbit->GetFocus().IsNull() || GetSystem()->IsStarAtPos(pOrbit->GetFocus()))));

		//	Check value

		return (bIsPlanet == strEquals(sValue, SPECIAL_VALUE_TRUE));
		}
	else if (strStartsWith(sAttrib, SPECIAL_LOCATION))
		{
		CString sLocationAttrib = strSubString(sAttrib, SPECIAL_LOCATION.GetLength());
		CSystem *pSystem = GetSystem();
		if (pSystem == NULL)
			return false;

		//	See if we have this as a system attribute or a territory attribute.

		if (::HasModifier(pSystem->GetAttribsAtPos(GetPos()), sLocationAttrib))
			return true;

		//	See if we have this as a location

		const CLocationDef *pLoc = pSystem->GetLocations().GetLocationByObjID(GetID());
		if (pLoc && ::HasModifier(pLoc->GetAttributes(), sLocationAttrib))
			return true;

		//	Otherwise, we don't have this location attribute.

		return false;
		}
	else if (strStartsWith(sAttrib, SPECIAL_PROPERTY))
		{
		CString sProperty = strSubString(sAttrib, SPECIAL_PROPERTY.GetLength());

		CString sError;
		CPropertyCompare Compare;
		if (!Compare.Parse(sProperty, &sError))
			{
			::kernelDebugLogPattern("ERROR: Unable to parse property expression: %s", sError);
			return false;
			}

		ICCItemPtr pValue = ICCItemPtr(((CSpaceObject *)this)->GetProperty(CCodeChainCtx(GetUniverse()), Compare.GetProperty()));
		return Compare.Eval(pValue);
		}
	else
		{
		CDesignType *pType = GetType();
		if (pType == NULL)
			return false;

		return pType->HasSpecialAttribute(sAttrib);
		}
	}

void CSpaceObject::Highlight (const CString &sText)

//	Highlight
//
//	Highlight object.
	
	{
	m_sHighlightText = sText;
	m_iHighlightCountdown = HIGHLIGHT_TIMER;
	}

void CSpaceObject::HighlightAppend (const CString &sText)

//	HighlightAppend
//
//	Appends highlight.

	{
	if (m_sHighlightText.IsBlank())
		m_sHighlightText = sText;
	else
		m_sHighlightText = strPatternSubst(CONSTLIT("%s\n%s"), m_sHighlightText, sText);

	m_iHighlightCountdown = HIGHLIGHT_TIMER;
	}

CSpaceObject *CSpaceObject::HitTest (const CVector &vStart, 
									 const DamageDesc &Damage, 
									 CVector *retvHitPos, 
									 int *retiHitDir)

//	HitTest
//
//	Returns the object that the beam hit or NULL if no object was hit.
//	If rThreshold > 0 and the object passes within the threshold distance
//	to some target, then retiHitDir = -1 and retvHitPos is the nearest point.

	{
	DEBUG_TRY_OBJ_LOOP

	//	Get the interaction of this object

	int iInteraction = GetInteraction();

	//	Get the list of objects that intersect the object

	SSpaceObjectGridEnumerator i;
	GetSystem()->EnumObjectsInBoxStart(i, GetPos(), g_SecondsPerUpdate * LIGHT_SECOND);

	//	We need some variables for stepping

	int iSteps = 0;
	CVector vStep;

	//	See if the beam hit anything. We start with a crude first pass.
	//	Any objects near the beam are then analyzed further to see if
	//	the beam hit them.

	int k;
	while (GetSystem()->EnumObjectsInBoxHasMore(i))
		{
		pObj = GetSystem()->EnumObjectsInBoxGetNext(i);

		//	Skip objects that we cannot hit

		if (!CanHit(pObj)
				|| !pObj->CanBeHitBy(Damage)
				|| pObj == this)
			continue;

		//	Skip if we do not interact with this object.

		int iInteractChance;
		if (iInteraction < 100
				&& (iInteractChance = Max(iInteraction, pObj->GetInteraction())) < 100
				&& mathRandom(1, 100) > iInteractChance)
			continue;

		//	Step towards this object and see if we hit it. Start by computing 
		//	the step vector, which should be 2 pixels long.

		if (iSteps == 0)
			{
			CVector vMissileTravel = (GetPos() - vStart);
			Metric rMissileTravel = vMissileTravel.Length();
			iSteps = (int)(rMissileTravel / (2.0 * g_KlicksPerPixel)) + 1;
			vStep = vMissileTravel / iSteps;
			}

		//	Prepare for point in object calculations

		SPointInObjectCtx PiOCtx;
		pObj->PointInObjectInit(PiOCtx);

		//	Step

		CVector vTest = vStart;
		for (k = 0; k < iSteps; k++)
			{
			//	If we hit this object then we're done.

			if (pObj->PointInObject(PiOCtx, pObj->GetPos(), vTest))
				{
				if (retvHitPos)
					*retvHitPos = vTest;

				//	Figure out the direction that the hit came from

				if (retiHitDir)
					*retiHitDir = VectorToPolar(-vStep, NULL);

				return pObj;
				}

			//	Next

			vTest = vTest + vStep;
			}
		}

	pObj = NULL;

	//	We didn't hit anything.

	return NULL;

	DEBUG_CATCH_OBJ_LOOP
	}

CSpaceObject *CSpaceObject::HitTestProximity (const CVector &vStart, 
											  Metric rMinThreshold, 
											  Metric rMaxThreshold, 
											  const DamageDesc &Damage, 
											  CVector *retvHitPos, 
											  int *retiHitDir)

//	HitTest
//
//	Returns the object that the beam hit or NULL if no object was hit.
//	If rThreshold > 0 and the object passes within the threshold distance
//	to some target, then retiHitDir = -1 and retvHitPos is the nearest point.

	{
	DEBUG_TRY_OBJ_LOOP

	const Metric OBJ_RADIUS_ADJ = 0.25;

	//	Get the interaction of this object

	int iInteraction = GetInteraction();

	//	Get the list of objects that intersect the object

	SSpaceObjectGridEnumerator i;
	GetSystem()->EnumObjectsInBoxStart(i, GetPos(), Max(rMaxThreshold, g_SecondsPerUpdate * LIGHT_SECOND));

	//	We need some variables for stepping

	int iSteps = 0;
	CVector vStep;

	//	We need some variables to track the closest object

	Metric rClosestApproach = rMaxThreshold;
	CVector vClosestPos;
	CSpaceObject *pClosestHit = NULL;

	//	See if the beam hit anything. We start with a crude first pass.
	//	Any objects near the beam are then analyzed further to see if
	//	the beam hit them.

	int k;
	while (GetSystem()->EnumObjectsInBoxHasMore(i))
		{
		pObj = GetSystem()->EnumObjectsInBoxGetNext(i);

		//	Skip objects that we cannot hit

		if (!CanHit(pObj)
				|| !pObj->CanBeHitBy(Damage)
				|| pObj == this)
			continue;

		//	Skip if we do not interact with this object.

		int iInteractChance;
		if (iInteraction < 100
				&& (iInteractChance = Max(iInteraction, pObj->GetInteraction())) < 100
				&& mathRandom(1, 100) > iInteractChance)
			continue;

		//	Step towards this object and see if we hit it. Start by computing 
		//	the step vector, which should be 2 pixels long.

		if (iSteps == 0)
			{
			CVector vMissileTravel = (GetPos() - vStart);
			Metric rMissileTravel = vMissileTravel.Length();
			iSteps = (int)(rMissileTravel / (2.0 * g_KlicksPerPixel)) + 1;
			vStep = vMissileTravel / iSteps;
			}

		//	Prepare for point in object calculations

		SPointInObjectCtx PiOCtx;
		pObj->PointInObjectInit(PiOCtx);

		//	Do we need to calculate proximity detonation for this object?

		bool bCanTriggerDetonation = (pObj->GetScale() == scaleShip
					|| pObj->GetScale() == scaleStructure)
				&& IsAngryAt(pObj)
				&& pObj->CanAttack();

		//	Compute the size of the object, if we're doing proximity computations

		Metric rObjRadius;
		if (bCanTriggerDetonation)
			rObjRadius = OBJ_RADIUS_ADJ * pObj->GetHitSize();

		//	Step

		CVector vTest = vStart;
		for (k = 0; k < iSteps; k++)
			{
			//	If we hit this object then we're done.

			if (pObj->PointInObject(PiOCtx, pObj->GetPos(), vTest))
				{
				if (retvHitPos)
					*retvHitPos = vTest;

				//	Figure out the direction that the hit came from

				if (retiHitDir)
					*retiHitDir = VectorToPolar(-vStep, NULL);

				return pObj;
				}

			//	Otherwise, if we're calculating proximity, calculate

			if (bCanTriggerDetonation)
				{
				CVector vDist = vTest - pObj->GetPos();
				Metric rDist = vDist.Length() - rObjRadius;

				if (rDist < rClosestApproach)
					{
					rClosestApproach = rDist;
					vClosestPos = vTest;
					pClosestHit = pObj;
					}
				}

			//	Next

			vTest = vTest + vStep;
			}

		//	If we're calculating proximity and if we're going to hit this object next
		//	tick anyways, then explode now.
		//
		//	NOTE that in this case we do this for all objects (including asteroids, etc.).

		CVector vNextPos = GetPos() + (GetVel() * g_SecondsPerUpdate);
		if (pObj->PointInObject(PiOCtx, pObj->GetPos(), vNextPos))
			{
			if (retvHitPos)
				*retvHitPos = GetPos();

			if (retiHitDir)
				*retiHitDir = -1;

			return pObj;
			}
		}

	pObj = NULL;

	//	See if we need to detonate

	if (pClosestHit)
		{
		//	If we're inside our minimum fragmentation radius, then we always
		//	detonate, regardless of whether or not we get closer later.

		if (rClosestApproach <= rMinThreshold)
			{
			if (retvHitPos)
				*retvHitPos = vClosestPos;

			if (retiHitDir)
				*retiHitDir = -1;

			return pClosestHit;
			}

		//	If we got inside the threshold radius for some object check to see if 
		//	we are now farther away. If so, then we	reached the closest point.
		//	(If not, then it means that next tick we will get closer still.)

		else
			{
			CVector vDist = GetPos() - pClosestHit->GetPos();
			Metric rDist = vDist.Length() - (OBJ_RADIUS_ADJ * pClosestHit->GetHitSize());

			if (rDist > rClosestApproach)
				{
				if (retvHitPos)
					*retvHitPos = vClosestPos;

				if (retiHitDir)
					*retiHitDir = -1;

				return pClosestHit;
				}
			}
		}

	//	We didn't hit anything.

	return NULL;

	DEBUG_CATCH_OBJ_LOOP
	}

bool CSpaceObject::ImagesIntersect (const CObjectImageArray &Image1, int iTick1, int iRotation1, const CVector &vPos1,
									const CObjectImageArray &Image2, int iTick2, int iRotation2, const CVector &vPos2)

//	ImagesIntersect
//
//	Returns TRUE if the two images intersect

	{
	//	Compute the offset of Image2 relative to Image1 in pixels

	CVector vOffset = vPos2 - vPos1;
	int x = mathRound(vOffset.GetX() / g_KlicksPerPixel);
	int y = -mathRound(vOffset.GetY() / g_KlicksPerPixel);
	
	//	Images intersect

	return Image1.ImagesIntersect(iTick1, iRotation1, x, y, Image2, iTick2, iRotation2);
	}

bool CSpaceObject::InBarrier (const CVector &vPos)

//	InBarrier
//
//	Returns TRUE if the given position is in a barrier

	{
	int i;

	//	Compute the bounding rect for this object

	CVector vUR, vLL;
	GetBoundingRect(&vUR, &vLL);

	//	Loop over all other objects and see if we bounce off

	for (i = 0; i < m_pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pBarrier = m_pSystem->GetObject(i);
		if (pBarrier 
				&& pBarrier->m_fIsBarrier 
				&& pBarrier != this 
				&& pBarrier->CanBlock(this))
			{
			//	Compute the bounding rect for the barrier.

			CVector vBarrierUR, vBarrierLL;
			pBarrier->GetBoundingRect(&vBarrierUR, &vBarrierLL);

			//	If we intersect then block

			if (IntersectRect(vUR, vLL, vBarrierUR, vBarrierLL)
					&& pBarrier->ObjectInObject(pBarrier->GetPos(), this, GetPos()))
				return true;
			}
		}

	return false;
	}

bool CSpaceObject::IncProperty (const CString &sProperty, ICCItem *pInc, ICCItemPtr &pResult)

//	IncProperty
//
//	Increment the given property.

	{
	//	First see if our sub-classes handle this property

	if (OnIncProperty(sProperty, pInc, pResult))
		return true;

	//	See if this is a custom property, we set data

	else if (CDesignType *pType = GetType())
		{
		ICCItemPtr pDummy;
		EPropertyType iType;
		if (!pType->FindCustomProperty(sProperty, pDummy, &iType))
			return false;

		switch (iType)
			{
			case EPropertyType::propGlobal:
				pResult = pType->IncGlobalData(sProperty, pInc);
				return true;

			case EPropertyType::propData:
				pResult = IncData(sProperty, pInc);
				return true;

			default:
				return false;
			}
		}

	//	Not handled

	else
		return false;
	}

bool CSpaceObject::InteractsWith (int iInteraction) const

//	InteractsWith
//
//	Returns TRUE if this object interacts with an object of the given interaciton.
//	NOTE: This is random, so the same call twice might return different results.

	{
	if (iInteraction >= 100)
		return true;

	iInteraction = Max(iInteraction, GetInteraction());
	if (iInteraction >= 100)
		return true;

	return (iInteraction > 0 && mathRandom(1, 100) <= iInteraction);
	}

bool CSpaceObject::IsAngryAt (const CDamageSource &Obj) const

//	IsAngryAt
//
//	Returns TRUE if we're angry at the given objet

	{
	//	If we have a real object, then we can ask about it.

	CSpaceObject *pObj = Obj.GetObj();
	if (pObj)
		return IsAngryAt(pObj);

	//	Otherwise we just go by the sovereign relationship.

	return IsEnemy(Obj);
	}

bool CSpaceObject::IsEnemyInRange (Metric rMaxRange, bool bIncludeStations)

//	IsEnemyInRange
//
//	Returns TRUE if there is an enemy in the range

	{
	int i;

	//	Get the sovereign

	CSovereign *pSovereign = GetSovereignToDefend();
	if (pSovereign == NULL || GetSystem() == NULL)
		return false;

	//	Get the list of enemy objects

	const CSpaceObjectList &ObjList = pSovereign->GetEnemyObjectList(GetSystem());

	//	Start a max range

	Metric rBestDist = rMaxRange * rMaxRange;
	CSpaceObject *pBestObj = NULL;

	//	Loop for all enemy objects

	int iCount = ObjList.GetCount();
	for (i = 0; i < iCount; i++)
		{
		CSpaceObject *pObj = ObjList.GetObj(i);

		if ((pObj->GetCategory() == catShip
					|| (bIncludeStations && pObj->GetCategory() == catStation))
				&& pObj->CanAttack()
				&& pObj != this)
			{
			CVector vDist = GetPos() - pObj->GetPos();
			Metric rDist = vDist.Length2();

			if (rDist < rBestDist
					&& !pObj->IsEscortingFriendOf(this))
				return true;
			}
		}

	return false;
	}

bool CSpaceObject::IsEscortingFriendOf (const CSpaceObject *pObj) const

//	IsEscortingFriendOf
//
//	Returns TRUE if we're escorting a friend of pObj

	{
	CSpaceObject *pPrincipal = GetEscortPrincipal();
	if (pPrincipal)
		return pObj->IsFriend(pPrincipal);
	else
		return false;
	}

bool CSpaceObject::IsPlayerAttackJustified (void) const

//	IsPlayerAttackJustified
//
//	Returns TRUE if we think the player attacked us by accident (e.g., while
//	defending us or themselves from attack).
//
//	NOTE: This assumes the player is not an enemy or already blacklisted.

	{
	CSpaceObject *pPlayer = GetPlayerShip();
	if (pPlayer == NULL)
		return false;

	//	Iterate over all objects to see who has this object as a target

	int i;
	for (i = 0; i < GetSystem()->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetSystem()->GetObject(i);
		CSpaceObject *pTarget;

		if (pObj
				&& IsEnemy(pObj)
				&& ((pTarget = pObj->GetTarget(CItemCtx())) == this
						|| pTarget == pPlayer)
				&& pObj != pPlayer
				&& pObj != this)
			return true;
		}

	return false;
	}

bool CSpaceObject::IsPlayerEscortTarget (CSpaceObject *pPlayer)

//	IsPlayerEscortTarget
//
//	Returns TRUE if we are being escorted by the player.

	{
	//	If we're not a player destination, then we're not being escorted.

	if (!IsPlayerDestination())
		return false;

	//	Get the player as a ship object

	if (pPlayer == NULL)
		{
		pPlayer = GetUniverse().GetPlayerShip();
		if (pPlayer == NULL)
			return false;
		}

	CShip *pPlayerShip = pPlayer->AsShip();
	if (pPlayerShip == NULL)
		return false;

	//	Check the player's target

	CSpaceObject *pTarget;
	IShipController::OrderTypes iOrder = pPlayerShip->GetController()->GetCurrentOrderEx(&pTarget);

	return (pTarget == this
			&& (iOrder == IShipController::orderGuard || iOrder == IShipController::orderEscort));
	}

bool CSpaceObject::IsStargateInRange (Metric rMaxRange)

//	IsStargateInRange
//
//	Returns TRUE if stargate is in range

	{
	int i;
	Metric rNearestGateDist2 = (rMaxRange * rMaxRange);
	for (i = 0; i < GetSystem()->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetSystem()->GetObject(i);

		if (pObj && pObj->IsActiveStargate())
			{
			Metric rDist2 = (GetPos() - pObj->GetPos()).Length2();
			if (rDist2 < rNearestGateDist2)
				return true;
			}
		}

	return false;
	}

bool CSpaceObject::IsUnderAttack (void)

//	IsUnderAttack
//
//	Returns TRUE if the space object is being attacked
//	by another object.

	{
	//	Iterate over all objects to see who has this object as a target

	int i;
	for (i = 0; i < GetSystem()->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetSystem()->GetObject(i);

		if (pObj
				&& pObj->GetTarget(CItemCtx()) == this
				&& IsEnemy(pObj)
				&& pObj != this)
			return true;
		}

	return false;
	}

bool CSpaceObject::IsCommsMessageValidFrom (CSpaceObject *pSender, int iIndex, CString *retsMsg, CString *retsKey)

//	IsCommsMessageValidFrom
//
//	Returns TRUE if the given object can send the given comms message to
//	this object

	{
	CCommunicationsHandler *pHandler = GetCommsHandler();
	ASSERT(pHandler && iIndex < pHandler->GetCount());
	const CCommunicationsHandler::SMessage &Msg = pHandler->GetMessage(iIndex);

	//	If we have an OnShow code block then see if it evaluates to TRUE. If not,
	//	then we bail.

	if (Msg.OnShowEvent.pCode)
		{
		CCodeChainCtx Ctx(GetUniverse());

		//	Define parameters

		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(this);
		Ctx.DefineSpaceObject(CONSTLIT("gSender"), pSender);

		//	Execute

		ICCItemPtr pResult = Ctx.RunCode(Msg.OnShowEvent);

		if (pResult->IsNil())
			return false;
		else if (pResult->IsError())
			{
			pSender->SendMessage(this, pResult->GetStringValue());
			return false;
			}

		//	<OnShow> returns non-Nil, so we continue.
		}

	//	If we don't have a message name then use the ID to lookup the name and
	//	key (shortcut).

	if (Msg.sMessage.IsBlank() && !Msg.sID.IsBlank())
		{
		CString sLabelDesc;
		if (TranslateText(Msg.sID, NULL, &sLabelDesc))
			{
			CLanguage::ParseLabelDesc(sLabelDesc, retsMsg, retsKey);

			//	If no short cut is defined, then use the one provided. [This 
			//	should not be a common case.]

			if (retsKey && retsKey->IsBlank())
				*retsKey = Msg.sShortcut;
			}
		else
			{
			if (retsMsg)
				*retsMsg = Msg.sID;

			if (retsKey)
				*retsKey = Msg.sShortcut;
			}
		}

	//	Otherwise, we expect a valid name and shortcut

	else
		{
		if (retsMsg)
			*retsMsg = Msg.sMessage;

		if (retsKey)
			*retsKey = Msg.sShortcut;
		}

	return true;
	}

bool CSpaceObject::IsCovering (CSpaceObject *pObj)

//	IsCovering
//
//	Returns TRUE if this object is covering the given object. That is,
//	we return TRUE if pObj is on top of (or below) this object and
//	this object is bigger than pObj

	{
	if (this != pObj)
		{
		CVector vThisUR;
		CVector vThisLL;

		GetBoundingRect(&vThisUR, &vThisLL);
		return pObj->PosInBox(vThisUR, vThisLL);
		}
	else
		return false;
	}

bool CSpaceObject::IsDestinyTime (int iCycle, int iOffset)

//	IsDestinyTime
//
//	Returns TRUE if the space object's destiny aligns with
//	the given cycle. A cycle of n aligns with a space object
//	once every n ticks. Each object aligns at different times
//	depending on its destiny.
 
	{
	return (((GetUniverse().GetTicks() + GetDestiny()) % iCycle) == iOffset);
	}

bool CSpaceObject::IsEnemy (const CSpaceObject *pObj) const

//	IsEnemy
//
//	Returns TRUE if the given object is our enemy

	{
	CSovereign *pOurSovereign = GetSovereign();
	CSovereign *pEnemySovereign = pObj->GetSovereign();

	if (pOurSovereign == NULL || pEnemySovereign == NULL)
		return false;
	else
		return pOurSovereign->IsEnemy(pEnemySovereign);
	}

bool CSpaceObject::IsEnemy (const CDamageSource &Obj) const

//	IsEnemy
//
//	Returns TRUE if the given object is our enemy

	{
	CSovereign *pOurSovereign = GetSovereign();
	CSovereign *pEnemySovereign = Obj.GetSovereign();

	if (pOurSovereign == NULL || pEnemySovereign == NULL)
		return false;
	else
		return pOurSovereign->IsEnemy(pEnemySovereign);
	}

bool CSpaceObject::IsFriend (const CSpaceObject *pObj) const

//	IsFriend
//
//	Returns TRUE if the given object is our friend. Note that this
//	is not equal to !IsEnemy. It is also possible for an object to
//	be "neutral"

	{
	CSovereign *pOurSovereign = GetSovereign();
	CSovereign *pEnemySovereign = pObj->GetSovereign();

	if (pOurSovereign == NULL || pEnemySovereign == NULL)
		return false;
	else
		return pOurSovereign->IsFriend(pEnemySovereign);
	}

bool CSpaceObject::IsLineOfFireClear (CInstalledDevice *pWeapon,
									  CSpaceObject *pTarget, 
									  int iAngle, 
									  Metric rDistance,
									  CSpaceObject **retpFriend)

//	IsLineOfFireClear
//
//	Returns TRUE if there are no friendly objects along the given
//	direction for the given distance

	{
	static const Metric TOO_CLOSE = 20.0 * g_KlicksPerPixel;

	//	If we can't hit friends, then this whole function is moot

	if (!CanHitFriends() || !pWeapon->CanHitFriends())
		return true;

	//	Compute some values

	CVector vSource = pWeapon->GetPos(this);
	bool bAreaWeapon = pWeapon->IsAreaWeapon(this);

	//	We need to adjust the angle to compensate for the fact that the shot
	//	will take on the velocity of the ship.

	Metric rShotSpeed = pWeapon->GetShotSpeed(CItemCtx(this, pWeapon));
	if (rShotSpeed > 0.0)
		{
		CVector vShotVel = GetVel() + PolarToVector(iAngle, rShotSpeed);
		iAngle = VectorToPolar(vShotVel);
		}

	//	We look for objects in the range of the weapon

	CSystem *pSystem = GetSystem();
	SSpaceObjectGridEnumerator i;

	//	Create a box around the path of the shot

	CVector vEndPoint = vSource + PolarToVector(iAngle, rDistance);
	CVector vLL = CVector(Min(vSource.GetX(), vEndPoint.GetX()), Min(vSource.GetY(), vEndPoint.GetY()));
	CVector vUR = CVector(Max(vSource.GetX(), vEndPoint.GetX()), Max(vSource.GetY(), vEndPoint.GetY()));
	pSystem->EnumObjectsInBoxStart(i, vUR, vLL, gridNoBoxCheck);

	//	Compute position of target

	CVector vTarget = (pTarget ? pTarget->GetPos() : vSource);
	Metric rMaxDist2 = rDistance * rDistance;
	bool bCheckTargetOverStation = (pTarget && pTarget->CanThrust());
	bool bTargetIsStation = (pTarget && pTarget->GetCategory() == catStation);

	//	See if any friendly object is in the line of fire

	bool bResult = true;
	while (pSystem->EnumObjectsInBoxHasMore(i))
		{
		CSpaceObject *pObj = pSystem->EnumObjectsInBoxGetNextFast(i);

		if (!pObj->IsDestroyed()
				&& pObj->CanAttack()
				//	Only check for ships, structures
				&& (pObj->GetScale() == scaleStructure 
					|| pObj->GetScale() == scaleShip)
				&& CanFireOn(pObj)
				&& !pObj->IsImmutable()
				&& pObj != this
				&& pObj != pTarget)
			{
			CSpaceObject::Categories iCategory = pObj->GetCategory();

			//	If this is an enemy then see if it is OK to hit them.

			if (IsEnemy(pObj))
				{
				//	If this is another ship, then it is OK to hit.

				if (iCategory == catShip)
					continue;

				//	If the target is a station, then it is OK to hit other 
				//	stations of the same sovereign.

				if (bTargetIsStation && pTarget->GetSovereign() == pObj->GetSovereign())
					continue;
				}

			//	If the target is right on top of a station, then we
			//	cannot fire.

			if (iCategory == catStation
					&& bCheckTargetOverStation)
				{
				//	Compute the distance of the object from us and from
				//	the target.

				CVector vDistFromTarget = pObj->GetPos() - vTarget;
				Metric rDistFromTarget2 = vDistFromTarget.Length2();

				if (rDistFromTarget2 < BOUNDS_CHECK_DIST2)
					{
					CVector vUR, vLL;
					pObj->GetBoundingRect(&vUR, &vLL);

					if (rDistFromTarget2 < 2.0 * vUR.Length2()
							&& pObj->PointInObject(pObj->GetPos(), vTarget))
						{
						if (retpFriend) *retpFriend = pObj;
						bResult = false;
						break;
						}
					}
				}

			//	Skip if we're too far

			CVector vCurObjPos = pObj->GetPos();
			CVector vCurDist = vCurObjPos - vSource;
			Metric rCurDist2 = vCurDist.Length2();
			if (rCurDist2 > rMaxDist2)
				continue;

			//	Get the current distance. Because this is all a heuristic, we
			//	assume that this is not too different from the object distance
			//	when the shot gets there.

			Metric rCurDist = sqrt(rCurDist2);

			//	If we're inside the object radius, then we would hit it no matter what
			//	the angle.

			Metric rHalfSize = 0.5 * pObj->GetHitSize();
			if (rCurDist < rHalfSize)
				{
				if (retpFriend) *retpFriend = pObj;
				bResult = false;
				break;
				}

			//	Figure out where the object will be by the time the shot gets
			//	to it.

			CVector vFutureDist;
			if (rShotSpeed > 0.0 && pObj->CanThrust())
				{
				Metric rTimeToReachObj = rCurDist / rShotSpeed;
				CVector vFutureObjPos = vCurObjPos + (rTimeToReachObj * pObj->GetVel());
				vFutureDist = vFutureObjPos - vSource;
				}
			else
				vFutureDist = vCurDist;

			//	Figure out the object's bearing relative to us

			int iFutureAngle = VectorToPolar(vFutureDist);

			//	Figure out how big the object is from that distance.
			//
			//	NOTE: For now we use the current (not future) distance because
			//	we assume it is close enough (as opposed to the angle).

			int iHalfAngularSize;
			if (rCurDist < rHalfSize + TOO_CLOSE)
				iHalfAngularSize = 90;
			else if (bAreaWeapon)
				iHalfAngularSize = 45;
			else
				iHalfAngularSize = pObj->GetHitSizeHalfAngle(rCurDist);

			//	See if it is in our line of fire

			if (AreAnglesAligned(iAngle, iFutureAngle, iHalfAngularSize))
				{
				if (retpFriend) *retpFriend = pObj;
				bResult = false;
				break;
				}
			}
		}

	return bResult;
	}

void CSpaceObject::Jump (const CVector &vPos)

//	Jump
//
//	Object jumps to a different position in the system

	{
	//	Create a gate effect at the old position

	CEffectCreator *pEffect = GetUniverse().FindEffectType(g_StargateInUNID);
	if (pEffect)
		pEffect->CreateEffect(m_pSystem,
				NULL,
				GetPos(),
				NullVector,
				0);

	//	Ask to see if any object wants to alter the jump position

	CVector vNewPos = GetSystem()->OnJumpPosAdj(this, vPos);

	//	Move the ship to the new position

	Place(vNewPos);

	//	Create a gate effect at the destination

	pEffect = GetUniverse().FindEffectType(g_StargateOutUNID);
	if (pEffect)
		pEffect->CreateEffect(m_pSystem,
				NULL,
				vNewPos,
				NullVector,
				0);

	//	Set the ship to hide while coming out of gate

	CShip *pShip = AsShip();
	if (pShip)
		pShip->SetInGate(NULL, 0);

	//	Notify subscribers

	m_SubscribedObjs.NotifyOnObjJumped(this);
	}

bool CSpaceObject::MatchesCriteria (CSpaceObjectCriteria::SCtx &Ctx, const CSpaceObjectCriteria &Crit) const

//	MatchesCriteria
//
//	Returns TRUE if this object matches the criteria

	{
	CSpaceObject *pSource = Crit.GetSource();
	if (pSource == this)
		return false;

	if (!(Crit.MatchesCategory(GetCategory()))
			&& (!Crit.MatchesPlayer() || !IsPlayer()))
		return false;

	//	NOTE: Virtual objects always count as active. E.g., we want the virtual
	//	St. Victoria station to count as active, but not as CanAttack
	//	[An alternative is to add !IsVirtual() to all places that look for
	//	enemies to attack.]

	if (Crit.MatchesActiveOnly() && IsInactive() && !IsVirtual())
		return false;

	if (Crit.MatchesStargatesOnly() && !IsActiveStargate())
		return false;

	if (Crit.MatchesStructureScaleOnly() 
			&& GetCategory() == CSpaceObject::catStation
			&& (GetScale() == scaleStar || GetScale() == scaleWorld))
		return false;

	if (Crit.MatchesManufacturedOnly()
			&& (GetScale() == scaleWorld || GetScale() == scaleStar))
		return false;

	if (Crit.MatchesKilledOnly() && (CanAttack() || IsVirtual()))
		return false;

	if (!Crit.MatchesSovereign(GetSovereign()))
		return false;

	if (Crit.MatchesDockedWithSource())
		if (pSource == NULL || !pSource->IsObjDocked(const_cast<CSpaceObject *>(this)))
			return false;

	if (Crit.MatchesFriendlyOnly() 
			&& pSource
			&& pSource->IsEnemy(this) 
			&& !IsEscortingFriendOf(pSource) 
			&& !pSource->IsEscortingFriendOf(this))
		return false;

	if (Crit.MatchesEnemyOnly() 
			&& pSource
			&& (!pSource->IsEnemy(this) || IsEscortingFriendOf(pSource) || pSource->IsEscortingFriendOf(this)))
		return false;

	if (Crit.MatchesAngryOnly() 
			&& pSource
			&& (!IsAngryAt(pSource) || IsEscortingFriendOf(pSource) || pSource->IsEscortingFriendOf(this)))
		return false;

	if (!Crit.MatchesStargate(GetStargateID()))
		return false;

	if (Crit.MatchesHomeBaseIsSource() && GetBase() != pSource)
		return false;

	if (Crit.MatchesTargetIsSource() && GetTarget(CItemCtx()) != pSource)
		return false;

	//	Check level

	if (!Crit.MatchesLevel(GetLevel()))
		return false;

	//	Check attributes

	if (!Crit.MatchesAttributes(*this))
		return false;

	//	Other checks

	if (Crit.ExcludesPlayer() && IsPlayer())
		return false;

	if (!Crit.MatchesData().IsBlank() && m_Data.IsDataNil(Crit.MatchesData()))
		return false;

	if (Crit.ExcludesVirtual() && IsVirtual())
		return false;

	//	Virtual objects are also intangible, so if we want to find virtual 
	//	objects (but not all intangible objects) then we need the extra check
	//	about !IsVirtual.

	if (Crit.ExcludesIntangible() && IsIntangible() && !IsVirtual())
		return false;

	//	With a particular order

	if (!Crit.MatchesOrder(pSource, *this))
		return false;

	//	If necessary, compute the distance and angle from the source to the obj.

	int iObjAngle;
	Metric rObjDist;
	Metric rObjDist2;
	if (Ctx.bCalcPolar)
		{
		CVector vCenter = (pSource ? pSource->GetPos() : CVector());
		CVector vDist = GetPos() - vCenter;

		iObjAngle = VectorToPolar(vDist, &rObjDist);
		rObjDist2 = rObjDist * rObjDist;
		}
	else if (Ctx.bCalcDist2)
		{
		CVector vCenter = (pSource ? pSource->GetPos() : CVector());
		CVector vDist = GetPos() - vCenter;

		iObjAngle = 0;
		rObjDist = 0.0;
		rObjDist2 = vDist.Length2();
		}

	//	Ranges

	if (Crit.MatchesNearerThan() && rObjDist2 > Ctx.rMaxRadius2)
		return false;

	if (Crit.MatchesFartherThan() && rObjDist2 < Ctx.rMinRadius2)
		return false;

	//	Visible only

	if (Crit.MatchesPerceivableOnly() && rObjDist2 > GetDetectionRange2(Crit.MatchesPerception()))
		return false;

	//	Angle
	//
	//	Only bother checking if rDist > 0 (we always intersect with an
	//	object that is 0 distance from us :).

	if (Crit.MatchesIntersectAngle() != -1 && rObjDist > 0.0)
		{
		//	Figure out how large the object is at this distance

		int iHalfAngularSize = GetHitSizeHalfAngle(rObjDist);
		
		//	If we do not intersect the line then we're done

		if (!AreAnglesAligned(Crit.MatchesIntersectAngle(), iObjAngle, iHalfAngularSize))
			return false;
		}

	//	Position checks

	if (!Crit.MatchesPosition(*this))
		return false;

	//	If we're looking for the nearest or farthest, do that computation now

	if (Crit.MatchesNearestOnly())
		{
		if (rObjDist2 < Ctx.rBestDist2
				&& (!IsIntangible() || IsVirtual()))
			{
			Ctx.pBestObj = const_cast<CSpaceObject *>(this);
			Ctx.rBestDist2 = rObjDist2;
			}
		}
	else if (Crit.MatchesFarthestOnly())
		{
		if (rObjDist2 > Ctx.rBestDist2
				&& (!IsIntangible() || IsVirtual()))
			{
			Ctx.pBestObj = const_cast<CSpaceObject *>(this);
			Ctx.rBestDist2 = rObjDist2;
			}
		}

	//	If we're sorting by distance, then add the object to the list

	if (Crit.GetSort() == CSpaceObjectCriteria::sortByDistance)
		Ctx.DistSort.Insert(rObjDist2, const_cast<CSpaceObject *>(this));

	return true;
	}

bool CSpaceObject::MissileCanHitObj (CSpaceObject *pObj, CDamageSource &Source, CWeaponFireDesc *pDesc)

//	MissileCanHitObj
//
//	Return TRUE if this object (a missile) can hit the given object (and assuming
//	that this object was fired by Source).

	{
	DEBUG_TRY

	//	If we have a source...

	if (Source.HasSource())
		{
		//	If we can damage our source, then we don't need to check further

		if (pDesc->CanDamageSource())
			return (
				//	We cannot hit another beam/missile from the same source
				//	(otherwise we get fratricide on fragmentation weapons).
				!Source.IsEqual(pObj->GetDamageSource())

				//	See if the missile has rules about what it cannot hit
				&& pDesc->CanHit(pObj));

		//	Otherwise, we can only hit if we're not hitting our source, etc.

		else
			return (
				//	We cannot hit the source of the beam...
				!Source.IsEqual(pObj)

				//	We cannot hit another beam/missile from the same source...
				&& !Source.IsEqual(pObj->GetDamageSource())

				//	See if the missile has rules about what it cannot hit
				&& pDesc->CanHit(pObj)

				//	We cannot hit our friends (if our source can't)
				&& ((CanHitFriends() && Source.CanHitFriends() && pObj->CanBeHitByFriends()) || !Source.IsFriend(pObj->GetSovereign()))

				//	If our source is the player, then we cannot hit player wingmen

				&& Source.CanHit(pObj));
		}

	//	If we don't have a source...

	else
		{
		//	If we don't have a primary source, then don't hit our secondary source either
		//	(For ship explosions, the secondary source is the wreck; the wreck cannot be the
		//	primary source or else the tombstone message will be wrong)

		if (pObj == GetSecondarySource())
			return false;

		//	Make sure we can hit

		else if (!pDesc->CanHit(pObj))
			return false;

		//	If we are part of an explosion, then we cannot hit other parts of an explosion
		//	that also have no source. This is so that fragments from an explosion where the source
		//	got destroyed (i.e., pSource == NULL) do not hit each other.

		else
			return !Source.IsEqual(pObj->GetDamageSource());
		}

	DEBUG_CATCH
	}

void CSpaceObject::Move (SUpdateCtx &Ctx, Metric rSeconds)

//	Move
//
//	Moves the object in a straight line based on its current
//	velocity

	{
	DEBUG_TRY

	//	Remember the old position

	m_vOldPos = m_vPos;

	//	Move the object on a straight line along the velocity vector

	if (!m_vVel.IsNull() && !m_fNonLinearMove && !IsAnchored())
		m_vPos = m_vPos + (m_vVel * g_SecondsPerUpdate);

	//	Let descendents process the move (if necessary)

	OnMove(m_vOldPos, rSeconds);

	//	Clear painted (until the next tick)

	ClearPainted();

	//	Set a flag so we check collisions

	if (IsAnchored())
		SetCollisionTestNeeded(false);
	else if (Ctx.bHasShipBarriers)
		SetCollisionTestNeeded(GetCategory() == CSpaceObject::catShip || GetCategory() == CSpaceObject::catStation);
	else
		SetCollisionTestNeeded(GetCategory() == CSpaceObject::catStation);

	DEBUG_CATCH;
	}

void CSpaceObject::NotifyOnNewSystem (CSystem *pNewSystem)

//	NotifyOnNewSystem
//
//	The object has been moved to a new system (this is commonly done for wingmen
//	and other followers of the player). We guarantee that the old system is 
//	still loaded at this point.

	{
	int i;

	//	If any objects in the old system subscribe to us, then we need to
	//	cancel the subscription (missions are OK because they cross systems).

	for (i = 0; i < m_SubscribedObjs.GetCount(); i++)
		{
		CSpaceObject *pSubscriber = m_SubscribedObjs.GetObj(i);
		if (pSubscriber->IsDestroyed()
				|| (!pSubscriber->IsMission() 
					&& pSubscriber->GetSystem() != pNewSystem))
			{
			m_SubscribedObjs.Delete(i);
			i--;
			}
		}

	//	Let our subclasses handle it

	OnNewSystem(pNewSystem);
	}

void CSpaceObject::NotifyOnObjDestroyed (SDestroyCtx &Ctx)

//	NotifyOnObjDestroyed
//
//	Notify subscribers OnObjDestroyed

	{
	DEBUG_TRY

	m_SubscribedObjs.NotifyOnObjDestroyed(Ctx);

	//	Notify any docked objects that their dock target got destroyed.

	CDockingPorts *pPorts = GetDockingPorts();
	if (pPorts)
		pPorts->OnDockObjDestroyed(this, Ctx);

	DEBUG_CATCH
	}

void CSpaceObject::NotifyOnObjDocked (CSpaceObject *pDockTarget)

//	NotifyOnObjDocked
//
//	Notify subscribers OnObjDocked

	{
	m_SubscribedObjs.NotifyOnObjDocked(this, pDockTarget);
	}

bool CSpaceObject::NotifyOnObjGateCheck (CSpaceObject *pGatingObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pGateObj)

//	NotifyOnObjGateCheck
//
//	Notify subscribers OnObjGateCheck

	{
	return m_SubscribedObjs.NotifyOnObjGateCheck(pGatingObj, pDestNode, sDestEntryPoint, pGateObj);
	}

bool CSpaceObject::ObjRequestDock (CSpaceObject *pObj, int iPort)

//	ObjRequestDock
//
//	pObj requests docking services with this object. Returns TRUE if docking 
//	is engaged.

	{
	switch (CanObjRequestDock(pObj))
		{
		case dockingOK:
			{
			CDockingPorts *pPorts = GetDockingPorts();
			if (pPorts == NULL)
				{
				//	Should never happen; we would not return dockingOK in this case.

				ASSERT(false);
				pObj->SendMessage(this, CONSTLIT("No docking services available"));
				return false;
				}

			//	Request dock

			return pPorts->RequestDock(this, pObj, iPort);
			}

		case dockingNotSupported:
			pObj->SendMessage(this, CONSTLIT("No docking services available"));
			return false;

		case dockingDisabled:
			pObj->SendMessage(this, CONSTLIT("Unable to dock"));
			return false;

		case dockingDenied:
			{
			CString sText;
			if (!pObj->TranslateText(LANGID_DOCKING_REQUEST_DENIED, NULL, &sText))
				sText = CONSTLIT("Docking request denied");

			pObj->SendMessage(this, sText);
			return false;
			}
			
		default:
			//	Should never happen. This means that we missed a result type.
			ASSERT(false);
			return false;
		}
	}

void CSpaceObject::OnModifyItemBegin (IDockScreenUI::SModifyItemCtx &ModifyCtx, const CItem &Item)

//	OnModifyItemBegin
//
//	The given Item (which must be part of the object) is about to be modified.

	{
	GetUniverse().GetDockSession().OnModifyItemBegin(ModifyCtx, this, Item);
	}

void CSpaceObject::OnModifyItemComplete (IDockScreenUI::SModifyItemCtx &ModifyCtx, const CItem &Result)

//	OnModifyItemComplete
//
//	An item was modified. Result is the newly modified item.

	{
	InvalidateItemListState();
	GetUniverse().GetDockSession().OnModifyItemComplete(ModifyCtx, this, Result);
	}

void CSpaceObject::OnObjDestroyed (const SDestroyCtx &Ctx)

//	OnObjDestroyed
//
//	Called whenever another object in the system is destroyed

	{
	DEBUG_TRY

	//	Give our subclasses a chance to do something (note that we need
	//	to do this before we NULL-out the references because some objects
	//	need to check their references.

	if (IsObjectDestructionHooked())
		ObjectDestroyedHook(Ctx);

	//	NULL-out any references to the object

	m_Data.OnObjDestroyed(Ctx.pObj);

	//	Remove the object if it had a subscription to us

	m_SubscribedObjs.Delete(Ctx.pObj);

	DEBUG_CATCH
	}

void CSpaceObject::SetCondition (CConditionSet::ETypes iCondition, int iTimer)

//	SetCondition
//
//	Sets the given condition (generically).

	{
	switch (iCondition)
		{
		case CConditionSet::cndTimeStopped:
			m_fTimeStop = true;
			break;

		default:
			OnSetCondition(iCondition, iTimer);
			break;
		}
	}

void CSpaceObject::SetConditionDueToDamage (SDamageCtx &DamageCtx, CConditionSet::ETypes iCondition)

//	SetConditionDueToDamage
//
//	The given damage has imparted the given condition. NOTE: The caller is 
//	responsible for checking to see if the object is immune or not.

	{
	OnSetConditionDueToDamage(DamageCtx, iCondition);
	}

void CSpaceObject::Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	Paint
//
//	Paint the object

	{
	if (IsHidden())
		{
		SetPainted();
		ClearPaintNeeded();
		return;
		}

	//	Initialize the object bounds

	Ctx.rcObjBounds = GetImage().GetImageRectAtPoint(x, y);
	Ctx.yAnnotations = Ctx.rcObjBounds.bottom + ANNOTATION_INNER_SPACING_Y + 1;

	//	Paint annotations under the object

	PaintDebugVector(Dest, x, y, Ctx);

	if (m_fShowHighlight && !Ctx.fNoSelection && !m_fShowDamageBar)
		PaintTargetHighlight(Dest, x, y, Ctx);

	//	Paint the object

	OnPaint(Dest, x, y, Ctx);

	//	Paint effects (e.g., muzzle flash)

	if (m_pFirstEffect)
		PaintEffects(Dest, x, y, Ctx);

	//	Mark this object's joints as needed to be painted

	if (m_pFirstJoint)
		m_pFirstJoint->SetObjListPaintNeeded(this);

	//	Paint annotations about the object (damage bar, etc.)

	if (!Ctx.fNoSelection)
		{
		if (IsHighlighted())
			PaintHighlight(Dest, x, y, Ctx);

		//	Paint damage bar

		if (m_fShowDamageBar)
			{
			int cyHeight;

			CPaintHelper::PaintStatusBar(Dest,
					x,
					Ctx.yAnnotations,
					GetUniverse().GetPaintTick(),
					GetSymbolColor(),
					NULL_STR,
					100 - GetVisibleDamage(),
					100,
					&cyHeight);

			Ctx.yAnnotations += cyHeight + ANNOTATION_INNER_SPACING_Y;
			}

		//	Show bounds

		if (Ctx.bShowBounds)
			{
			CG32bitPixel rgbColor = GetSymbolColor();
			int xHalf = mathRound(m_rBoundsX / g_KlicksPerPixel);
			int yHalf = mathRound(m_rBoundsY / g_KlicksPerPixel);
			CGDraw::RectOutline(Dest, x - xHalf, y - yHalf, 2 * xHalf, 2 * yHalf,rgbColor);
			}

		//	Let the object paint additional annotations

		OnPaintAnnotations(Dest, x, y, Ctx);
		}

	//	Done

	SetPainted();
	ClearPaintNeeded();
	}

void CSpaceObject::PaintEffects (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	PaintEffects
//
//	Paints the list of effects for this object

	{
	SEffectNode *pEffect = m_pFirstEffect;
	if (pEffect)
		{
		CViewportPaintCtxSmartSave Save(Ctx);
		Ctx.iVariant = 0;
		Ctx.iDestiny = GetDestiny();

		while (pEffect)
			{
			Ctx.iTick = pEffect->iTick;
			Ctx.iRotation = pEffect->iRotation;

			pEffect->pPainter->Paint(Dest, 
					x + pEffect->xOffset,
					y + pEffect->yOffset,
					Ctx);

			pEffect = pEffect->pNext;
			}
		}
	}

void CSpaceObject::PaintHighlight (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	PaintHighlight
//
//	Paints a highlight around the object

	{
	//	Figure out the color of the highlight

	CG32bitPixel rgbColor = GetSymbolColor();

	//	Paint the corners

	Dest.DrawLine(Ctx.rcObjBounds.left, Ctx.rcObjBounds.top,
			Ctx.rcObjBounds.left + HIGHLIGHT_CORNER_WIDTH, Ctx.rcObjBounds.top,
			1, rgbColor);

	Dest.DrawLine(Ctx.rcObjBounds.left, Ctx.rcObjBounds.top,
			Ctx.rcObjBounds.left, Ctx.rcObjBounds.top + HIGHLIGHT_CORNER_HEIGHT,
			1, rgbColor);

	Dest.DrawLine(Ctx.rcObjBounds.right, Ctx.rcObjBounds.top,
			Ctx.rcObjBounds.right - HIGHLIGHT_CORNER_WIDTH, Ctx.rcObjBounds.top,
			1, rgbColor);

	Dest.DrawLine(Ctx.rcObjBounds.right, Ctx.rcObjBounds.top,
			Ctx.rcObjBounds.right, Ctx.rcObjBounds.top + HIGHLIGHT_CORNER_HEIGHT,
			1, rgbColor);

	Dest.DrawLine(Ctx.rcObjBounds.left, Ctx.rcObjBounds.bottom,
			Ctx.rcObjBounds.left, Ctx.rcObjBounds.bottom - HIGHLIGHT_CORNER_HEIGHT,
			1, rgbColor);

	Dest.DrawLine(Ctx.rcObjBounds.left, Ctx.rcObjBounds.bottom,
			Ctx.rcObjBounds.left + HIGHLIGHT_CORNER_WIDTH, Ctx.rcObjBounds.bottom,
			1, rgbColor);

	Dest.DrawLine(Ctx.rcObjBounds.right, Ctx.rcObjBounds.bottom,
			Ctx.rcObjBounds.right - HIGHLIGHT_CORNER_WIDTH, Ctx.rcObjBounds.bottom,
			1, rgbColor);

	Dest.DrawLine(Ctx.rcObjBounds.right, Ctx.rcObjBounds.bottom,
			Ctx.rcObjBounds.right, Ctx.rcObjBounds.bottom - HIGHLIGHT_CORNER_HEIGHT,
			1, rgbColor);

	//	Paint message, if we have one

	if (!m_sHighlightText.IsBlank() || m_iHighlightChar)
		{
		int cyHeight;
		PaintHighlightText(Dest, x, Ctx.yAnnotations, Ctx, alignCenter, rgbColor, &cyHeight);
		Ctx.yAnnotations += cyHeight + ANNOTATION_INNER_SPACING_Y;
		}
	}

void CSpaceObject::PaintHighlightText (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx, AlignmentStyles iAlign, CG32bitPixel rgbColor, int *retcyHeight)

//	PaintHighlightText
//
//	Paint highlight text

	{
	int yOriginal = y;

	const int KEY_BOX_SIZE = 18;
	const CG16bitFont &NameFont = GetUniverse().GetNamedFont(CUniverse::fontSRSObjName);
	const CG16bitFont &MessageFont = GetUniverse().GetNamedFont(CUniverse::fontSRSMessage);
	const RECT &rcClip = Dest.GetClipRect();

	if (iAlign & alignBottom)
		{
		y -= NameFont.GetHeight()
				+ (IsShowingDistanceAndBearing() ? NameFont.GetHeight() : 0)
				+ (!m_sHighlightText.IsBlank() ? MessageFont.GetHeight() : 0)
				+ (m_iHighlightChar ? KEY_BOX_SIZE + 2 : 0);
		}

	DWORD dwFontFlags = CG16bitFont::AdjustToFit;
	if (iAlign & alignCenter)
		dwFontFlags |= CG16bitFont::AlignCenter;
	else if (iAlign & alignRight)
		dwFontFlags |= CG16bitFont::AlignRight;

	//	Figure out what name to paint

	CString sName;
	if (IsIdentified())
		sName = GetNounPhrase(0);
	else if (Ctx.pCenter == NULL)
		sName = CONSTLIT("Unknown Object");
	else if (Ctx.pCenter->IsEnemy(this))
		sName = CONSTLIT("Unknown Hostile");
	else
		sName = CONSTLIT("Unknown Friendly");

	//	Paint it

	NameFont.DrawText(Dest, 
			x,
			y,
			rgbColor, 
			sName,
			dwFontFlags);
	y += NameFont.GetHeight();

	//	Paint distance and bearing, if required

	if (IsShowingDistanceAndBearing() && Ctx.pCenter)
		{
		Metric rDist = (GetPos() - Ctx.pCenter->GetPos()).Length();
		CString sText = strPatternSubst(CONSTLIT("Distance: %d"), (int)(rDist / LIGHT_SECOND));
		NameFont.DrawText(Dest,
				x,
				y,
				rgbColor,
				sText,
				dwFontFlags);

		y += NameFont.GetHeight();
		}

	//	Paint the message, if we have one

	if (!m_sHighlightText.IsBlank())
		{
		RECT rcRect;
		rcRect.left = x - 1000;
		rcRect.right = x + 1000;
		rcRect.top = y;
		rcRect.bottom = y + 1000;

		//	Paint message

		CG32bitPixel rgbMessageColor;
		if (m_iHighlightCountdown > HIGHLIGHT_BLINK)
			rgbMessageColor = CG32bitPixel::Blend(rgbColor, 0xffff, (BYTE)(255 * (m_iHighlightCountdown - HIGHLIGHT_BLINK) / (HIGHLIGHT_TIMER - HIGHLIGHT_BLINK)));
		else
			rgbMessageColor = rgbColor;

		DWORD dwOpacity;
		if (m_iHighlightCountdown < HIGHLIGHT_FADE)
			dwOpacity = 255 * m_iHighlightCountdown / HIGHLIGHT_FADE;
		else
			dwOpacity = 255;

		int cyHeight;
		MessageFont.DrawText(Dest,
				rcRect,
				CG32bitPixel(rgbMessageColor, (BYTE)dwOpacity),
				m_sHighlightText,
				0,
				dwFontFlags,
				&cyHeight);

		y += cyHeight;
		}

	//	Paint the highlight key

	if (m_iHighlightChar)
		{
		const CG16bitFont &KeyFont = GetUniverse().GetNamedFont(CUniverse::fontSRSObjName);
		char chChar = (char)m_iHighlightChar;
		CString sKey = CString(&chChar, 1);

		y += ANNOTATION_INNER_SPACING_Y;

		Dest.Fill(x - KEY_BOX_SIZE / 2, y, KEY_BOX_SIZE, KEY_BOX_SIZE, rgbColor);

		int xText = x - (KeyFont.MeasureText(sKey) / 2);
		int yText = y + (KEY_BOX_SIZE / 2) - (KeyFont.GetHeight() / 2);

		KeyFont.DrawText(Dest,
				xText,
				yText,
				0,
				sKey);

		y += KEY_BOX_SIZE;
		}

	//	Done

	if (retcyHeight)
		*retcyHeight = y - yOriginal;
	}

void CSpaceObject::PaintLRSForeground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans)

//	PaintLRSForeground
//
//	Paints the object on an LRS

	{
	Dest.DrawDot(x, y, 
			CG32bitPixel(255, 255, 0), 
			markerSmallRound);
	}

void CSpaceObject::PaintMap (CMapViewportCtx &Ctx, CG32bitImage &Dest, int x, int y)

//	PaintMap
//
//	Paints the object on a system map

	{
	OnPaintMap(Ctx, Dest, x, y);

	if (IsPlayerDestination() || (IsHighlighted() && !m_sHighlightText.IsBlank()))
		{
		int iTick = GetUniverse().GetPaintTick();
		int iRadius = 10;
		int iRingSpacing = 4;
		CG32bitPixel rgbColor = GetSymbolColor();

		CPaintHelper::PaintTargetHighlight(Dest, x, y, iTick, iRadius, iRingSpacing, 6, rgbColor);

		if (IsHighlighted() && !m_sHighlightText.IsBlank())
			{
			SViewportPaintCtx ViewportCtx;
			ViewportCtx.pCenter = Ctx.GetCenterObj();

			PaintHighlightText(Dest, x, y, ViewportCtx, alignCenter, rgbColor);
			}
		}
	}

void CSpaceObject::PaintTargetHighlight (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	PaintTargetHighlight
//
//	Paints an animated highlight

	{
	int iTick = GetUniverse().GetPaintTick();
	int iRadius = (int)(0.5 * GetHitSize() / g_KlicksPerPixel);
	int iRingSpacing = 10;
	CG32bitPixel rgbColor = GetSymbolColor();

	CPaintHelper::PaintTargetHighlight(Dest, x, y, iTick, iRadius, iRingSpacing, 3, rgbColor);
	}

void CSpaceObject::Reconned (void)

//	Reconned
//
//	Tell all other objects that this object has been reconned
//	(used for missions)

	{
	m_SubscribedObjs.NotifyOnObjReconned(this);
	}

void CSpaceObject::RecordBuyItem (CSpaceObject *pSellerObj, const CItem &Item, const CCurrencyAndValue &Price)

//	RecordBuyItem
//
//	Transfers funds from the buyer to the seller and calls appropriate events.
//	NOTE: This does not transfer the item.

	{
	ASSERT(pSellerObj);
	ASSERT(Price.GetCurrencyType());
	//	NOTE: It is OK if the Item is null.

	//	Charge the buyer (us)

	ChargeMoney(Price.GetCurrencyType()->GetUNID(), Price.GetValue());

	//	Give money to the seller

	pSellerObj->CreditMoney(Price.GetCurrencyType()->GetUNID(), Price.GetValue());

	//	If the player is buying, then record it as the player buying.

	if (IsPlayer())
		{
		CShip *pPlayerShip = AsShip();
		if (pPlayerShip)
			pPlayerShip->GetController()->OnItemBought(Item, Price.GetCreditValue());

		//	If the player is buying, then allow types to keep track
		//	(e.g., Black Market gives out experience points).

		GetUniverse().FireOnGlobalPlayerBoughtItem(pSellerObj, Item, Price);
		}

	//	Otherwise, if the seller is the player, record it as the player
	//	selling.

	else if (pSellerObj->IsPlayer())
		{
		CShip *pPlayerShip = pSellerObj->AsShip();
		if (pPlayerShip)
			pPlayerShip->GetController()->OnItemSold(Item, Price.GetCreditValue());

		//	If the player is selling, then allow types to keep track.

		GetUniverse().FireOnGlobalPlayerSoldItem(this, Item, Price);
		}
	}

void CSpaceObject::Remove (DestructionTypes iCause, const CDamageSource &Attacker, bool bRemovedByOwner)

//	Remove
//
//	Removes this object from the system
//
//	pCause is the object that caused us to be removed (may be NULL)

	{
	if (m_iIndex != -1 && !m_fDestroyed)
		{
		//	Set the destroyed flag so that this object 
		//	doesn't show up in cached lists like the system object grid
		//
		//	Also, we use this to prevent recursion

		m_fDestroyed = true;

		//	Remove from the object from the universal list (NOTE: We must do this
		//	before we clear out m_pSystem.)

		CSpaceObject::Categories iCategory = GetCategory();
		if (iCategory == CSpaceObject::catStation || iCategory == CSpaceObject::catShip)
			GetUniverse().GetGlobalObjects().Delete(this);

		//	Remove

		SDestroyCtx Ctx;
		Ctx.pObj  = this;
		Ctx.iCause = iCause;
		Ctx.Attacker = Attacker;
		Ctx.pWreck = NULL;
		Ctx.bResurrectPending = false;
		Ctx.bRemovedByOwner = bRemovedByOwner;
		Ctx.pResurrectedObj = NULL;

		CSystem *pSystem = m_pSystem;
		m_pSystem = NULL;

		//	Give our descendants a chance to remove. For example, ships will
		//	deal with attached sections.

		OnRemoved(Ctx);

		//	This will call OnObjDestroyed for all interested objects

		pSystem->RemoveObject(Ctx);

		//	Delete all subscriptions. We are leaving the system, so we can't
		//	hold on to pointers to the old system.
		//
		//	NOTE: We leave mission objects intact

		m_SubscribedObjs.DeleteSystemObjs();

		//	Done

		m_iIndex = -1;
		}
	}

void CSpaceObject::RemoveAllEventSubscriptions (CSystem *pSystem, TArray<DWORD> *retRemoved)

//	RemoveAllEventSubscriptions
//
//	Removes all of this object's subscriptions to other objects. Optionally
//	returns an array of object IDs that we subscribed to.

	{
	int i;

	if (retRemoved)
		retRemoved->DeleteAll();

	for (i = 0; i < pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = pSystem->GetObject(i);
		if (pObj && pObj != this)
			{
			if (pObj->m_SubscribedObjs.Delete(this))
				{
				if (retRemoved)
					retRemoved->Insert(pObj->GetID());
				}
			}
		}
	}

void CSpaceObject::RemoveItemEnhancement (const CItem &itemToEnhance, DWORD dwID, bool bExpiredOnly)

//	RemoveItemEnhancement
//
//	Removes the given item enhancement

	{
	DEBUG_TRY

	//	Find the item

	CItemListManipulator ItemList(GetItemList());
	if (!ItemList.SetCursorAtItem(itemToEnhance))
		return;

	//	Make sure it is the right mod

	const CItemEnhancement &Mod = ItemList.GetItemAtCursor().GetMods();
	if (Mod.GetID() != dwID)
		return;

	//	If we're removed expired only, check for expiration

	if (bExpiredOnly)
		{
		if (Mod.GetExpireTime() > GetUniverse().GetTicks())
			return;
		}

	//	Get the enhancement type now because Mod will be invalid after
	//	we remove it.

	CItemType *pEnhancementType = Mod.GetEnhancementType();

	//	Remove the enhancement

	ItemList.RemoveItemEnhancementAtCursor(dwID);

	//	Notify source

	ItemEnhancementModified(ItemList);

	//	Fire an event to the enhancement to tell it

	if (ItemList.IsCursorValid())
		{
		CItem theEnhancement(pEnhancementType, 1);
		theEnhancement.FireOnRemovedAsEnhancement(this, ItemList.GetItemAtCursor());
		}

	DEBUG_CATCH
	}

void CSpaceObject::RepairItem (CItemListManipulator &ItemList)

//	RepairItem
//
//	Repairs the selected item

	{
	const CItem &Item = ItemList.GetItemAtCursor();

	if (Item.IsDamaged())
		{
		ItemList.SetDamagedAtCursor(false);

		//	Raise event

		ItemEnhancementModified(ItemList);
		}
	}

void CSpaceObject::ReportEventError (const CString &sEvent, ICCItem *pError) const

//	ReportEventError
//
//	Report an error during an event

	{
	CString sError = strPatternSubst(CONSTLIT("%s [%s]: %s"), sEvent, GetNounPhrase(), pError->GetStringValue());
	CSpaceObject *pPlayer = GetUniverse().GetPlayerShip();
	if (pPlayer)
		pPlayer->SendMessage(const_cast<CSpaceObject *>(this), sError);

	kernelDebugLogString(sError);
	}

bool CSpaceObject::RequestGate (CSpaceObject *pObj)

//	RequestGate
//
//	Requests that the given object be transported through the gate

	{
	pObj->EnterGate(NULL, NULL_STR, this);
	return true;
	}

void CSpaceObject::ScrapeOverlays (void)

//	ScrapeOverlays
//
//	Remove attached, foreign overlays

	{
	COverlayList *pOverlays = GetOverlays();
	if (pOverlays == NULL)
		return;

	pOverlays->ScrapeHarmfulOverlays(this);
	}

void CSpaceObject::SetCursorAtArmor (CItemListManipulator &ItemList, CInstalledArmor *pArmor)

//	SetCursorAtArmor
//
//	Sets the given cursor at the item for the armor.

	{
	ItemList.ResetCursor();

	while (ItemList.MoveCursorForward())
		{
		const CItem &Item = ItemList.GetItemAtCursor();
		if (Item.IsInstalled()
				&& Item.GetType() == pArmor->GetClass()->GetItemType()
				&& Item.GetInstalled() == pArmor->GetSect())
			{
			ASSERT(Item.GetCount() == 1);
			break;
			}
		}
	}

bool CSpaceObject::SetCursorAtDevice (CItemListManipulator &ItemList, int iDevSlot)

//	SetCursorAtDevice
//
//	Sets the given cursor at the item for the device. Returns TRUE if successful.

	{
	ItemList.ResetCursor();

	while (ItemList.MoveCursorForward())
		{
		const CItem &Item = ItemList.GetItemAtCursor();
		if (Item.IsInstalled()
				&& Item.IsDevice()
				&& Item.GetInstalled() == iDevSlot)
			{
			ASSERT(Item.GetCount() == 1);
			return true;
			}
		}

	return false;
	}

bool CSpaceObject::SetCursorAtDevice (CItemListManipulator &ItemList, CInstalledDevice *pDevice)

//	SetCursorAtDevice
//
//	Sets the given cursor at the item for the device.

	{
	return SetCursorAtDevice(ItemList, pDevice->GetDeviceSlot());
	}

void CSpaceObject::SetCursorAtRandomItem (CItemListManipulator &ItemList, const CItemCriteria &Crit)

//	SetCursorAtRandomItem
//
//	Sets the given cursor at a random item that matches the criteria
//	(We may leave the cursor invalid if there is no item that matches
//	the criteria.)

	{
	TArray<int> ItemPos;

	//	Make a list of the position of all the items that match
	//	the criteria.

	int iCount = 0;
	ItemList.ResetCursor();
	while (ItemList.MoveCursorForward())
		{
		if (ItemList.GetItemAtCursor().MatchesCriteria(Crit))
			ItemPos.Insert(iCount);

		iCount++;
		}

	//	Now choose a random device

	ItemList.ResetCursor();
	if (ItemPos.GetCount() > 0)
		{
		int iRoll = ItemPos[mathRandom(0, ItemPos.GetCount() - 1)];

		while (ItemList.MoveCursorForward() && iRoll > 0)
			iRoll--;
		}

	//	Done
	}

void CSpaceObject::SetDataInteger (const CString &sAttrib, int iValue)

//	SetDataInteger
//
//	Set integer value

	{
	CCodeChain &CC = GetUniverse().GetCC();
	ICCItemPtr pValue(iValue);
	SetData(sAttrib, pValue);
	}

void CSpaceObject::SetEventFlags (void)

//	SetEventFlags
//
//	Sets cached flags for events

	{
	SetHasGetDockScreenEvent(FindEventHandler(CONSTLIT("GetDockScreen")));
	SetHasOnAttackedEvent(FindEventHandler(CONSTLIT("OnAttacked")));
	SetHasOnAttackedByPlayerEvent(FindEventHandler(CONSTLIT("OnAttackedByPlayer")));
	SetHasOnDamageEvent(FindEventHandler(CONSTLIT("OnDamage")));
	SetHasOnObjDockedEvent(FindEventHandler(CONSTLIT("OnObjDocked")));

	//	Let subclasses do their bit

	OnSetEventFlags();
	}

void CSpaceObject::SetGlobalData (const CString &sAttribute, ICCItem *pData)

//	SetGlobalData
//
//	Sets data for the type

	{
	CDesignType *pType = GetType();
	if (pType == NULL)
		return;

	pType->SetGlobalData(sAttribute, pData);
	}

void CSpaceObject::SetOverride (CDesignType *pOverride)

//	SetOverride
//
//	Sets the override.
	
	{
	//	NULL means go back to the default event handler (which could be NULL).

	if (pOverride == NULL)
		pOverride = GetDefaultOverride();

	//	Short-circuit if no change

	if (pOverride == m_pOverride)
		{
		//	Always set the event flags because we call this when we create the
		//	universe and we want to make sure we set the flags in that case.

		SetEventFlags();
		return;
		}

	//	Let the previous event handler terminate

	if (m_pOverride)
		{
		SEventHandlerDesc Event;
		if (FindEventHandler(ON_OVERRIDE_TERM_EVENT, &Event))
			{
			CCodeChainCtx Ctx(GetUniverse());

			Ctx.DefineContainingType(this);
			Ctx.SaveAndDefineSourceVar(this);

			ICCItem *pResult = Ctx.Run(Event);
			if (pResult->IsError())
				ReportEventError(ON_OVERRIDE_TERM_EVENT, pResult);
			Ctx.Discard(pResult);
			}
		}

	//	Set it.

	m_pOverride = pOverride;
	SetEventFlags();

	//	Fire OnOverrideInit

	if (m_pOverride)
		{
		SEventHandlerDesc Event;
		if (FindEventHandler(ON_OVERRIDE_INIT_EVENT, &Event))
			{
			CCodeChainCtx Ctx(GetUniverse());

			Ctx.DefineContainingType(this);
			Ctx.SaveAndDefineSourceVar(this);

			ICCItem *pResult = Ctx.Run(Event);
			if (pResult->IsError())
				ReportEventError(ON_OVERRIDE_INIT_EVENT, pResult);
			Ctx.Discard(pResult);
			}
		}
	}

void CSpaceObject::SetPlayerDestination (const SPlayerDestinationOptions &Options)

//	SetPlayerDestination
//
//	Sets the object as a player destination.

	{
	m_fPlayerDestination = true;

	if (Options.bShowDistanceAndBearing)
		m_fShowDistanceAndBearing = true;

	if (Options.bAutoClearDestination)
		m_fAutoClearDestination = true;

	if (Options.bAutoClearOnDestroy)
		m_fAutoClearDestinationOnDestroy = true;

	if (Options.bAutoClearOnDock)
		m_fAutoClearDestinationOnDock = true;

	if (Options.bAutoClearOnGate)
		m_fAutoClearDestinationOnGate = true;

	if (Options.bShowHighlight)
		m_fShowHighlight = true;
	}

bool CSpaceObject::SetProperty (const CString &sName, ICCItem *pValue, CString *retsError)

//	SetProperty
//
//	Sets an object property

	{
	if (strEquals(sName, PROPERTY_IDENTIFIED))
		{
		SetIdentified(!pValue->IsNil());
		return true;
		}
	else if (strEquals(sName, PROPERTY_COMMS_KEY))
		{
		CString sKey = pValue->GetStringValue();
		m_iDesiredHighlightChar = *sKey.GetASCIIZPointer();
		return true;
		}
	else if (strEquals(sName, PROPERTY_KNOWN))
		{
		SetKnown(!pValue->IsNil());
		return true;
		}
	else if (strEquals(sName, PROPERTY_SOVEREIGN))
		{
		CSovereign *pSovereign = GetUniverse().FindSovereign(pValue->GetIntegerValue());
		if (pSovereign == NULL)
			{
			if (retsError) *retsError = strPatternSubst(CONSTLIT("Unknown sovereign: %s."), pValue->GetStringValue());
			return false;
			}

		SetSovereign(pSovereign);
		return true;
		}

	//	See if this is a custom property, we set data

	else if (CDesignType *pType = GetType())
		{
		ICCItemPtr pDummy;
		EPropertyType iType;
		if (!pType->FindCustomProperty(sName, pDummy, &iType))
			return false;

		switch (iType)
			{
			case EPropertyType::propGlobal:
				pType->SetGlobalData(sName, pValue);
				return true;

			case EPropertyType::propData:
				SetData(sName, pValue);
				return true;

			default:
				return false;
			}
		}

	else
		return false;
	}

void CSpaceObject::SetSovereign (CSovereign *pSovereign)

//	SetSovereign
//
//	Sets the object sovereign

	{
	OnSetSovereign(pSovereign);

	//	If we're part of a system, we need to flush the enemy object cache when
	//	we change sovereigns.

	CSystem *pSystem;
	if (ClassCanAttack() && (pSystem = GetSystem()))
		pSystem->FlushEnemyObjectCache();
	}

bool CSpaceObject::Translate (const CString &sID, ICCItem *pData, ICCItemPtr &retResult)

//	Translate
//
//	Translate a message by ID. The caller is responsible for discarding the 
//	result.

	{
	//	First we ask the override

	if (m_pOverride && m_pOverride->Translate(this, sID, pData, retResult))
		return true;

	//	Ask the type

	CDesignType *pType = GetType();
	if (pType && pType->Translate(this, sID, pData, retResult))
		return true;

	//	Otherwise, see if the sovereign has it

	CSovereign *pSovereign = GetSovereign();
	if (pSovereign && pSovereign->Translate(this, sID, pData, retResult))
		return true;

	//	Otherwise, we can't find it.

	return false;
	}

bool CSpaceObject::TranslateText (const CString &sID, ICCItem *pData, CString *retsText)

//	Translate
//
//	Translate a message by ID.

	{
	//	Ask the override

	if (m_pOverride && m_pOverride->TranslateText(this, sID, pData, retsText))
		return true;

	//	Then the type

	CDesignType *pType = GetType();
	if (pType && pType->TranslateText(this, sID, pData, retsText))
		return true;

	//	Otherwise, see if the sovereign has it

	CSovereign *pSovereign = GetSovereign();
	if (pSovereign && pSovereign->TranslateText(this, sID, pData, retsText))
		return true;

	//	Otherwise, we can't find it.

	return false;
	}

void CSpaceObject::Update (SUpdateCtx &Ctx)

//	Update
//
//	Update the object

	{
	SetInUpdateCode();

	//	Update as long as we are not time-stopped.

	if (!Ctx.IsTimeStopped())
		{
		//	Update effects

		if (m_pFirstEffect)
			UpdateEffects();

		//	Update items

		if (IsDestinyTime(ITEM_ON_UPDATE_CYCLE, ITEM_ON_UPDATE_OFFSET))
			{
			FireItemOnUpdate();

			//	We could have gotten destroyed here.

			if (IsDestroyed())
				{
				ClearInUpdateCode();
				return;
				}
			}

		//	Update object

		CDesignType *pType;
		if (FindEventHandler(CDesignType::evtOnUpdate)
				&& IsDestinyTime(OBJECT_ON_UPDATE_CYCLE, OBJECT_ON_UPDATE_OFFSET)
				&& (pType = GetType())
				//	Skip missiles, because we can't tell the difference between OnUpdate
				//	for the item and OnUpdate for the missile object.
				&& pType->GetType() != designItemType
				&& pType->GetAPIVersion() >= 31)
			{
			FireOnUpdate();

			//	We could have gotten destroyed here, so we check and leave if 
			//	necessary.

			if (IsDestroyed())
				{
				ClearInUpdateCode();
				return;
				}
			}

		//	Update the specific object subclass.

		OnUpdate(Ctx, g_SecondsPerUpdate);
		if (IsDestroyed())
			{
			ClearInUpdateCode();
			return;
			}
		}

	//	Otherwise, if we're time-stopped we need to update any overlays that
	//	cause time stop (so that they can expire properly).

	else
		{
		COverlayList *pOverlays = GetOverlays();
		if (pOverlays)
			pOverlays->UpdateTimeStopped(this, GetImageScale(), GetRotation());
		}

	//	Now update some variables that are only used by the player but relate
	//	to this object.
	//
	//	NOTE: These should update even if we're time-stopped.

	if (m_iHighlightCountdown > 0)
		{
		if (--m_iHighlightCountdown == 0)
			m_sHighlightText = NULL_STR;
		}

	//	See if this is the nearest player target

	if (Ctx.pPlayer
			&& !Ctx.pPlayer->IsDestroyed()
			&& this != Ctx.pPlayer)
		{
		UpdatePlayerTarget(Ctx);
		}

	//	See if we have a dock screen. We only check every 20 ticks or so, so this
	//	information might be stale. Use this for the docking ports animation, but
	//	not for actually determining if we can dock.
	//
	//	NOTE: <GetDockScreen> and <GetGlobalDockScreen> must not have side-
	//	effects, so we assume we cannot be destroyed after the call.

	if (IsDestinyTime(21, 8))
		{
		m_fHasDockScreenMaybe = (CanObjRequestDock(Ctx.pPlayer) == dockingOK);
		}

	//	Done

	ClearInUpdateCode();
	}

void CSpaceObject::UpdateEffects (void)

//	UpdateEffects
//
//	Updates old-style effects

	{
	SEffectMoveCtx MoveCtx;
	MoveCtx.pObj = this;

	SEffectUpdateCtx UpdateCtx;
	UpdateCtx.pSystem = GetSystem();
	UpdateCtx.pObj = this;

	//	Update the effects

	SEffectNode *pEffect = m_pFirstEffect;
	SEffectNode *pPrev = NULL;
	while (pEffect)
		{
		SEffectNode *pNext = pEffect->pNext;

		if (++pEffect->iTick >= pEffect->pPainter->GetLifetime())
			{
			if (pPrev)
				pPrev->pNext = pNext;
			else
				m_pFirstEffect = pNext;

			pEffect->pPainter->Delete();
			delete pEffect;
			}
		else
			{
			UpdateCtx.iTick = pEffect->iTick;
			UpdateCtx.iRotation = pEffect->iRotation;

			pEffect->pPainter->OnUpdate(UpdateCtx);
			pEffect->pPainter->OnMove(MoveCtx);

			pPrev = pEffect;
			}

		pEffect = pNext;
		}
	}

void CSpaceObject::UpdateExtended (const CTimeSpan &ExtraTime)

//	UpdateExtended
//
//	Update the object after a long time.
	
	{
	UpdateTradeExtended(ExtraTime);

	//	Let subclasses update

	OnUpdateExtended(ExtraTime);
	}

void CSpaceObject::UpdatePlayerTarget (SUpdateCtx &Ctx)

//	UpdatePlayerTarget
//
//	Called during update to see if this could be the player's best target.

	{
	bool bIsAngryAtPlayer = IsAngryAt(Ctx.pPlayer);
	CVector vDist = GetPos() - Ctx.pPlayer->GetPos();

	//	If this is the player's current target, then see if we can actually
	//	hit it with any of our weapons.

	if (Ctx.pPlayerTarget == this)
		{
		Metric rDist;
		int iAngle = VectorToPolar(vDist, &rDist);

		//	If we're out of range of both the primary and the launcher, then 
		//	we remember that fact.

		if (rDist > Ctx.pPlayer->GetMaxWeaponRange())
			Ctx.bPlayerTargetOutOfRange = true;

		//	If we have a fire arc and we're outside the arc, then we're not in
		//	range either.

		else if (Ctx.iMinFireArc != Ctx.iMaxFireArc
				&& !AngleInArc(iAngle, Ctx.iMinFireArc, Ctx.iMaxFireArc))
			Ctx.bPlayerTargetOutOfRange = true;

		//	If we're outside detection range, then we can't keep the target

		else if (rDist > GetDetectionRange(Ctx.iPlayerPerception))
			Ctx.bPlayerTargetOutOfRange = true;

		//	Otherwise, if this object is angry at the player, then it is a
		//	valid auto-target

		else if (bIsAngryAtPlayer && CanAttack())
			{
			Metric rDist2 = rDist * rDist;
			if (rDist2 < Ctx.rTargetDist2)
				{
				Ctx.pTargetObj = this;
				Ctx.rTargetDist2 = rDist2;
				}
			}
		}

	//	If the object cannot attack, then it is never an auto-target.

	else if (!CanAttack())
		{ }

	//	If this object is not angry at us, then it can never be an auto-
	//	target.

	else if (!bIsAngryAtPlayer)
		{ }

	//	If the player's weapons has an arc of fire, then limit ourselves to
	//	targets in the arc.

	else if (Ctx.iMinFireArc != Ctx.iMaxFireArc)
		{
		Metric rDist;
		int iAngle = VectorToPolar(vDist, &rDist);
		Metric rDist2 = rDist * rDist;

		if (rDist2 < Ctx.rTargetDist2
				&& AngleInArc(iAngle, Ctx.iMinFireArc, Ctx.iMaxFireArc)
				&& rDist <= GetDetectionRange(Ctx.iPlayerPerception))
			{
			Ctx.pTargetObj = this;
			Ctx.rTargetDist2 = rDist2;
			}
		}

	//	Otherwise, just find the nearest target

	else
		{
		Metric rDist2 = vDist.Length2();
		if (rDist2 < Ctx.rTargetDist2
				&& rDist2 <= GetDetectionRange2(Ctx.iPlayerPerception))
			{
			Ctx.pTargetObj = this;
			Ctx.rTargetDist2 = rDist2;
			}
		}
	}

bool CSpaceObject::UseItem (const CItem &Item, CString *retsError)

//	UseItem
//
//	Uses the given item

	{
	CCodeChainCtx Ctx(GetUniverse());

	//	If this item is a device and it is installed, get the
	//	installed device structure.

	if (Item.GetType()->IsDevice() && Item.IsInstalled())
		{
		CInstalledDevice *pDevice = FindDevice(Item);

		ASSERT(pDevice);
		if (pDevice)
			{
			//	If the device is disabled, then we can't use it

			if (!pDevice->IsEnabled())
				{
				if (retsError) *retsError = strPatternSubst(CONSTLIT("%s not enabled"), Item.GetNounPhrase(CItemCtx(Item), nounCapitalize));
				return false;
				}

			//	If the device is not ready, then we can't use it

			if (!pDevice->IsReady())
				{
				if (retsError) *retsError = strPatternSubst(CONSTLIT("%s not yet recharged"), Item.GetNounPhrase(CItemCtx(Item), nounCapitalize));
				return false;
				}

			//	Reset the activation delay, if necessary

			int iActivationDelay = pDevice->GetActivateDelay(this);
			if (iActivationDelay)
				{
				pDevice->SetTimeUntilReady(iActivationDelay);

				if (pDevice->ShowActivationDelayCounter(this))
					OnComponentChanged(comDeviceCounter);
				}
			}
		}

	//	Get the code from the item

	CItemType::SUseDesc UseDesc;
	if (Item.GetType()->GetUseDesc(&UseDesc) && UseDesc.pCode)
		{
		//	Define parameters

		Ctx.DefineContainingType(Item);
		Ctx.SaveAndDefineSourceVar(this);
		Ctx.SaveAndDefineItemVar(Item);
		Ctx.SetScreensRoot(Item.GetType());
		Ctx.SetExtension(UseDesc.pExtension);

		//	Invoke

		ICCItemPtr pResult = Ctx.RunCode(UseDesc.pCode);
		if (pResult->IsError())
			{
			if (retsError) *retsError = pResult->GetStringValue();
			return false;
			}
		}

	return true;
	}

void CSpaceObject::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Write the object to a stream
//
//	DWORD		ObjID
//	DWORD		m_iIndex
//	DWORD		m_dwID
//	DWORD		m_iDestiny
//	Vector		m_vPos
//	Vector		m_vVel
//	Metric		m_rBoundsX
//	Metric		m_rBoundsY
//	DWORD		low = unused; hi = m_iHighlightCountdown
//	DWORD		m_pOverride
//	CItemList	m_ItemList
//	DWORD		m_iControlsFrozen
//	DWORD		flags
//	CAttributeDataBlock	m_Data
//	CVector		m_vOldPos (only if !IsAnchored())
//	CSpaceObjectList m_SubscribedObjs
//
//	For each effect:
//	IEffectPainter (0 == no more)
//	DWORD		x
//	DWORD		y
//	DWORD		iTick
//	DWORD		iRotation

	{
	//	Write out the Kernel object ID

	DWORD dwSave = GetClassID();

	//	Save out stuff

	pStream->Write((char *)&dwSave, sizeof(DWORD));
	pStream->Write((char *)&m_iIndex, sizeof(DWORD));
	pStream->Write((char *)&m_dwID, sizeof(DWORD));
	pStream->Write((char *)&m_iDestiny, sizeof(DWORD));
	pStream->Write((char *)&m_vPos, sizeof(m_vPos));
	pStream->Write((char *)&m_vVel, sizeof(m_vVel));
	pStream->Write((char *)&m_rBoundsX, sizeof(m_rBoundsX));
	pStream->Write((char *)&m_rBoundsY, sizeof(m_rBoundsY));
	dwSave = MAKELONG((WORD)(BYTE)m_iDesiredHighlightChar, m_iHighlightCountdown);
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	//	Override

	dwSave = (m_pOverride ? m_pOverride->GetUNID() : 0);
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	//	Write out the list of items

	m_ItemList.WriteToStream(pStream);

	//	More Data

	dwSave = m_iControlsFrozen;
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	//	Write out flags

	dwSave = 0;
	dwSave |= (m_fHookObjectDestruction		? 0x00000001 : 0);
	dwSave |= (m_fNoObjectDestructionNotify ? 0x00000002 : 0);
	dwSave |= (m_fCannotBeHit				? 0x00000004 : 0);
	dwSave |= (m_fSelected					? 0x00000008 : 0);
	dwSave |= (m_fInPOVLRS					? 0x00000010 : 0);
	dwSave |= (m_fCanBounce					? 0x00000020 : 0);
	dwSave |= (m_fIsBarrier					? 0x00000040 : 0);
	dwSave |= (IsAnchored()					? 0x00000080 : 0);
	dwSave |= (m_fNoFriendlyFire			? 0x00000100 : 0);
	dwSave |= (m_fTimeStop					? 0x00000200 : 0);
	dwSave |= (m_fPlayerTarget				? 0x00000400 : 0);
	dwSave |= (m_fAutoClearDestinationOnGate ? 0x00000800 : 0);
	dwSave |= (m_fNoFriendlyTarget			? 0x00001000 : 0);
	dwSave |= (m_fPlayerDestination			? 0x00002000 : 0);
	dwSave |= (m_fShowDistanceAndBearing	? 0x00004000 : 0);
	dwSave |= (m_fHasOnObjDockedEvent		? 0x00008000 : 0);
	dwSave |= (m_fHasInterSystemEvent		? 0x00010000 : 0);
	dwSave |= (m_fHasOnDamageEvent			? 0x00020000 : 0);
	dwSave |= (m_fHasOnAttackedEvent		? 0x00040000 : 0);
	dwSave |= (m_fAutoClearDestination		? 0x00080000 : 0);
	dwSave |= (m_fHasOnOrdersCompletedEvent	? 0x00100000 : 0);
	dwSave |= (m_fPlayerDocked				? 0x00200000 : 0);
	dwSave |= (m_fNonLinearMove				? 0x00400000 : 0);
	dwSave |= (m_fAscended					? 0x00800000 : 0);
	dwSave |= (m_fOutOfPlaneObj				? 0x01000000 : 0);
	dwSave |= (m_fAutoClearDestinationOnDock ? 0x02000000 : 0);
	dwSave |= (m_fShowHighlight				? 0x04000000 : 0);
	dwSave |= (m_fAutoClearDestinationOnDestroy ? 0x08000000 : 0);
	dwSave |= (m_fShowDamageBar				? 0x10000000 : 0);
	dwSave |= (m_fHasGravity				? 0x20000000 : 0);
	dwSave |= (m_fInsideBarrier				? 0x40000000 : 0);
	dwSave |= (m_fHasOnSubordinateAttackedEvent	? 0x80000000 : 0);
	//	No need to save m_fHasName because it is set by CSystem on load.
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	//	More flags

	dwSave = 0;
	//	0x00000001 unused
	dwSave |= (m_fHasGetDockScreenEvent		? 0x00000002 : 0);
	dwSave |= (m_fHasOnAttackedByPlayerEvent	? 0x00000004 : 0);
	dwSave |= (m_fHasOnOrderChangedEvent	? 0x00000008 : 0);
	dwSave |= (m_fManualAnchor				? 0x00000010 : 0);
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	//	Write out the opaque data

	m_Data.WriteToStream(pStream, m_pSystem);

	//	Write out other stuff

	if (!IsAnchored())
		pStream->Write((char *)&m_vOldPos, sizeof(CVector));

	//	Subscriptions

	m_SubscribedObjs.WriteToStream(m_pSystem, pStream);

	//	Write out the effect list

	SEffectNode *pNext = m_pFirstEffect;
	while (pNext)
		{
		CEffectCreator::WritePainterToStream(pStream, pNext->pPainter);

		pStream->Write((char *)&pNext->xOffset, sizeof(DWORD));
		pStream->Write((char *)&pNext->yOffset, sizeof(DWORD));
		pStream->Write((char *)&pNext->iTick, sizeof(DWORD));
		pStream->Write((char *)&pNext->iRotation, sizeof(DWORD));

		pNext = pNext->pNext;
		}

	//	Effects list ends in a NULL

	CEffectCreator::WritePainterToStream(pStream, NULL);

	//	Let the subclass write out its part

	OnWriteToStream(pStream);
	}

CString ParseParam (char **ioPos)
	{
	char *pPos = *ioPos;
	if (pPos[1] == ':')
		{
		pPos++;
		pPos++;

		char *pStart = pPos;
		while (*pPos != ';' && *pPos != '\0')
			pPos++;

		//	If we hit the end, we backup one character because our
		//	caller will advance the position by one.

		*ioPos = (*pPos == '\0' ? (pPos - 1) : pPos);

		//	Return the string

		return CString(pStart, pPos - pStart);
		}
	else
		return NULL_STR;
	}

#ifdef DEBUG_VECTOR

void CSpaceObject::PaintDebugVector (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)
	{
	if (!m_vDebugVector.IsNull())
		{
		int xDest, yDest;

		Ctx.XForm.Transform(GetPos() + m_vDebugVector, &xDest, &yDest);

		Dest.DrawLine(x, y,
				xDest, yDest,
				3,
				CG32bitPixel(0,255,0));
		}
	}

#endif

