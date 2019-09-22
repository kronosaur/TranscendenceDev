//	CSystem.cpp
//
//	CSystem class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "math.h"
#include "SystemPaintImpl.h"

#define ENHANCED_SRS_BLOCK_SIZE			6

#define LEVEL_ENCOUNTER_CHANCE			10

const Metric MAX_AUTO_TARGET_DISTANCE =			30.0 * LIGHT_SECOND;
const Metric MAX_ENCOUNTER_DIST	=				30.0 * LIGHT_MINUTE;
const Metric MAX_MIRV_TARGET_RANGE =			50.0 * LIGHT_SECOND;

const Metric GRAVITY_WARNING_THRESHOLD =		40.0;	//	Acceleration value at which we start warning
const Metric TIDAL_KILL_THRESHOLD =				7250.0;	//	Acceleration at which we get ripped apart

const BYTE MAX_SPACE_OPACITY =					128;

const int MAX_THREAD_COUNT =					16;

#define ON_CREATE_EVENT					CONSTLIT("OnCreate")
#define ON_OBJ_JUMP_POS_ADJ				CONSTLIT("OnObjJumpPosAdj")

#define SPECIAL_ATTRIB_INNER_SYSTEM		CONSTLIT("innerSystem")
#define SPECIAL_ATTRIB_LIFE_ZONE		CONSTLIT("lifeZone")
#define SPECIAL_ATTRIB_NEAR_STATIONS	CONSTLIT("nearStations")
#define SPECIAL_ATTRIB_OUTER_SYSTEM		CONSTLIT("outerSystem")

#define SEPARATION_CRITERIA						CONSTLIT("sTA")

int g_iGateTimer = 0;
int g_iGateTimerTick = -1;
int g_cxStarField = -1;
int g_cyStarField = -1;

const CG32bitPixel g_rgbSpaceColor = CG32bitPixel(0,0,8);
const Metric g_MetersPerKlick = 1000.0;
const Metric MAP_VERTICAL_ADJUST =						1.4;

const CG32bitPixel RGB_GRID_LINE =						CG32bitPixel(65, 68, 77);

const Metric BACKGROUND_OBJECT_FACTOR =					4.0;

const int GRID_SIZE =									256;
const Metric CELL_SIZE =								(512.0 * g_KlicksPerPixel);
const Metric CELL_BORDER =								(128.0 * g_KlicksPerPixel);

const Metric SAME_POS_THRESHOLD2 =						(g_KlicksPerPixel * g_KlicksPerPixel);

const Metric MAP_GRID_SIZE =							3000.0 * LIGHT_SECOND;

CSystem::CSystem (CUniverse &Universe, CTopologyNode *pTopology) : 
		m_Universe(Universe),
		m_dwID(OBJID_NULL),
		m_pTopology(pTopology),
		m_pEnvironment(NULL),
		m_iTick(0),
        m_iNextEncounter(0),
		m_iTimeStopped(0),
		m_rKlicksPerPixel(KLICKS_PER_PIXEL),
		m_rTimeScale(TIME_SCALE),
		m_iLastUpdated(-1),
		m_fNoRandomEncounters(false),
		m_fInCreate(false),
		m_fEncounterTableValid(false),
		m_fUseDefaultTerritories(true),
		m_fEnemiesInLRS(false),
		m_fEnemiesInSRS(false),
		m_fPlayerUnderAttack(false),
		m_fLocationsBlocked(false),
		m_fFlushEventHandlers(false),
		m_pThreadPool(NULL),
		m_ObjGrid(GRID_SIZE, CELL_SIZE, CELL_BORDER)

//	CSystem constructor

	{
	//	Make sure our vectors are initialized
	EuclidInit();
	}

CSystem::~CSystem (void)

//	CSystem destructor

	{
	int i;

	//	Set our topology node to NULL so that a new system is
	//	created next time we access this node.

	if (m_pTopology)
		m_pTopology->SetSystem(NULL);

	if (m_pEnvironment)
		delete m_pEnvironment;

	if (m_pThreadPool)
		delete m_pThreadPool;

	//	Clear out any attached object because those are freed by their owners.

	for (i = 0; i < m_AllObjects.GetCount(); i++)
		if (m_AllObjects[i] && m_AllObjects[i]->IsAttached())
			m_AllObjects[i] = NULL;

	//	Free objects

	for (i = 0; i < m_AllObjects.GetCount(); i++)
		if (m_AllObjects[i])
			delete m_AllObjects[i];

	//	Deleted objects

	FlushDeletedObjects();
	}

bool CSystem::AddJoint (CObjectJoint::ETypes iType, CSpaceObject *pFrom, CSpaceObject *pTo, CObjectJoint **retpJoint)

//	AddJoint
//
//	Creates a new joint and returns it.

	{
	m_Joints.CreateJoint(iType, pFrom, pTo, retpJoint);
	return true;
	}

bool CSystem::AddJoint (CObjectJoint::ETypes iType, CSpaceObject *pFrom, CSpaceObject *pTo, ICCItem *pOptions, DWORD *retdwID)

//	AddJoint
//
//	Adds a new joint between two objects.

	{
	CObjectJoint *pJoint;
	m_Joints.CreateJoint(iType, pFrom, pTo, &pJoint);

	//	Apply the options

	pJoint->ApplyOptions(pOptions);

	//	Done

	if (retdwID)
		*retdwID = pJoint->GetID();

	return true;
	}

ALERROR CSystem::AddTerritory (CTerritoryDef *pTerritory)

//	AddTerritory
//
//	Adds a territory

	{
	m_Territories.Insert(pTerritory);

	//	If we are defining one of the special attributes, then remember
	//	that so we don't implicitly define them.

	if (m_fUseDefaultTerritories
		&& (pTerritory->HasAttribute(SPECIAL_ATTRIB_INNER_SYSTEM)
			|| pTerritory->HasAttribute(SPECIAL_ATTRIB_LIFE_ZONE)
			|| pTerritory->HasAttribute(SPECIAL_ATTRIB_OUTER_SYSTEM)))
		m_fUseDefaultTerritories = false;

	return NOERROR;
	}

ALERROR CSystem::AddTimedEvent (CSystemEvent *pEvent)

//	AddTimedEvent
//
//	Adds a timed event

	{
	m_TimedEvents.AddEvent(pEvent);
	return NOERROR;
	}

void CSystem::AddToDeleteList (CSpaceObject *pObj)

//	AddToDeleteList
//
//	Adds the object to a list to be deleted later.

	{
	ASSERT(pObj->IsDestroyed());
	ASSERT(pObj->GetID() != 0xdddddddd);

	m_DeletedObjects.FastAdd(pObj);
	}

ALERROR CSystem::AddToSystem (CSpaceObject *pObj, int *retiIndex)

//	AddToSystem
//
//	Adds an object to the system

	{
	int i;

	//	If this object affects the enemy object cache, then
	//	flush the cache

	if (pObj->ClassCanAttack())
		FlushEnemyObjectCache();

	//	If this is a star, add it to our list of stars

	if (pObj->GetScale() == scaleStar)
		{
		SStarDesc *pDesc = m_Stars.Insert();
		pDesc->pStarObj = pObj;
		}

	//	Reuse a slot first

	for (i = 0; i < m_AllObjects.GetCount(); i++)
		{
		if (m_AllObjects[i] == NULL)
			{
			m_AllObjects[i] = pObj;
			if (retiIndex)
				*retiIndex = i;
			return NOERROR;
			}
		}

	//	If we could not find a free place, add a new object

	if (retiIndex)
		*retiIndex = m_AllObjects.GetCount();

	m_AllObjects.Insert(pObj);
	return NOERROR;
	}

bool CSystem::AscendObject (CSpaceObject *pObj, CString *retsError)

//	AscendObject
//
//	Removes the object from the system and adds it to the global list of 
//	ascended objects. Return FALSE if there was an error.

	{
	if (pObj->IsAscended())
		return true;

	if (pObj->IsDestroyed())
		{
		if (retsError)
			*retsError = CONSTLIT("Cannot ascend destroyed objects.");
		return false;
		}

	if (pObj->IsPlayer())
		{
		if (retsError)
			*retsError = CONSTLIT("Cannot ascend the player ship.");
		return false;
		}

	if (pObj->GetSystem() != this)
		{
		if (retsError)
			*retsError = CONSTLIT("Cannot ascend object in another system.");
		return false;
		}

	//	Ascend the object

	pObj->Ascend();

	//	Add to the list of ascended objects

	m_Universe.AddAscendedObj(pObj);

	//	Done

	return true;
	}

void CSystem::BlockOverlappingLocations (void)

//	BlockOverlappingLocations
//
//	Remove any locations that overlap something or are too close to another
//	location.
	
	{
	int i;

	//	If we've already done this, no need to do it again.

	if (m_fLocationsBlocked)
		return;

	//	Remove any locations that are directly on top of a planet. We only check 
	//	for objects above a certain size because we can move/remove asteroids 
	//	instead of locations.

	for (i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);
		if (pObj == NULL
				|| pObj->IsDestroyed()
				|| (pObj->GetScale() != scaleStar && pObj->GetPlanetarySize() < MIN_PLANET_SIZE))
			continue;

		//	Fill any locations that overlap with this object.

		m_Locations.FillOverlappingWith(pObj);
		}

	//	Block any location that is too close to another.

	m_Locations.FillCloseLocations();
	}

Metric CSystem::CalcApparentSpeedAdj (Metric rSpeed)

//	CalcApparentSpeedAdj
//
//	Calculate adjustment to speed to simulate light-lag.

	{
	//	We max out at 10x light-speed, apparent speed.

	const Metric MAX_ADJ = 10.0;

	//	Compute speed as a fraction of light-speed. But we get infinities if we
	//	actually get to light-speed, so we stop near light-speed.

	const Metric NEAR_LIGHT_SPEED = 0.99;
	Metric rSpeedFrac = Min(NEAR_LIGHT_SPEED, rSpeed / LIGHT_SPEED);

	//	Adjust to simulate speed as seen by observer if object were heading
	//	straight for observer.

	Metric rAdj = (1.0 / ((1.0 / rSpeedFrac) - 1.0)) + 1.0;
	return Min(MAX_ADJ, rAdj);
	}

void CSystem::CalcAutoTarget (SUpdateCtx &Ctx)

//	CalcAutoTarget
//
//	Initializes various fields in the context to figure out what the player's
//	auto-target is.

	{
	if (Ctx.pPlayer == NULL)
		return;

	Ctx.pPlayerTarget = Ctx.pPlayer->GetTarget(CItemCtx(), IShipController::FLAG_ACTUAL_TARGET);

	//	Check to see if the primary weapon requires autotargetting

	CInstalledDevice *pWeapon = Ctx.pPlayer->GetNamedDevice(devPrimaryWeapon);
	if (pWeapon && pWeapon->IsEnabled())
		{
		CItemCtx ItemCtx(Ctx.pPlayer, pWeapon);
		Ctx.bNeedsAutoTarget = pWeapon->GetClass()->NeedsAutoTarget(ItemCtx, &Ctx.iMinFireArc, &Ctx.iMaxFireArc);
		}

	//	If the primary does not need it, check the missile launcher

	CInstalledDevice *pLauncher;
	if ((pLauncher = Ctx.pPlayer->GetNamedDevice(devMissileWeapon))
		&& pLauncher->IsEnabled())
		{
		CItemCtx ItemCtx(Ctx.pPlayer, pLauncher);
		int iLauncherMinFireArc, iLauncherMaxFireArc;
		if (pLauncher->GetClass()->NeedsAutoTarget(ItemCtx, &iLauncherMinFireArc, &iLauncherMaxFireArc))
			{
			if (Ctx.bNeedsAutoTarget)
				CGeometry::CombineArcs(Ctx.iMinFireArc, Ctx.iMaxFireArc, iLauncherMinFireArc, iLauncherMaxFireArc, &Ctx.iMinFireArc, &Ctx.iMaxFireArc);
			else
				{
				Ctx.bNeedsAutoTarget = true;
				Ctx.iMinFireArc = iLauncherMinFireArc;
				Ctx.iMaxFireArc = iLauncherMaxFireArc;
				}
			}
		}

	//	Set up perception and max target dist

	Ctx.iPlayerPerception = Ctx.pPlayer->GetPerception();
	Ctx.rTargetDist2 = MAX_AUTO_TARGET_DISTANCE * MAX_AUTO_TARGET_DISTANCE;
	}

int CSystem::CalculateLightIntensity (const CVector &vPos, CSpaceObject **retpStar, const CG8bitSparseImage **retpVolumetricMask)

//	CalculateLightIntensity
//
//	Returns 0-100% the intensity of the light at this point
//	in space.

	{
	DEBUG_TRY

	int i;

	//	Find the nearest star to the position. We optimize the case where
	//	there is only a single star in the system.

	int iBestDist;
	SStarDesc *pBestStar = NULL;

	if (m_Stars.GetCount() == 1)
		{
		pBestStar = &m_Stars[0];
		iBestDist = (int)(vPos.Longest() / LIGHT_SECOND);
		}
	else
		{
		pBestStar = NULL;
		iBestDist = 100000000;

		for (i = 0; i < m_Stars.GetCount(); i++)
			{
			CSpaceObject *pStar = m_Stars[i].pStarObj;
			CVector vDist = vPos - pStar->GetPos();

			int iDistFromCenter = (int)(vDist.Longest() / LIGHT_SECOND);
			if (iDistFromCenter < iBestDist)
				{
				iBestDist = iDistFromCenter;
				pBestStar = &m_Stars[i];
				}
			}

		if (pBestStar == NULL)
			{
			if (retpStar)
				*retpStar = NULL;

			if (retpVolumetricMask)
				*retpVolumetricMask = NULL;

			return 0;
			}
		}

	//	Compute the percentage

	int iMaxDist = pBestStar->pStarObj->GetMaxLightDistance();
	int iDistFromCenter = (iBestDist < 15 ? 0 : iBestDist - 15);
	int iPercent = 100 - (iDistFromCenter * 100 / iMaxDist);

	if (retpStar)
		*retpStar = pBestStar->pStarObj;

	if (retpVolumetricMask)
		*retpVolumetricMask = &pBestStar->VolumetricMask;

	return Max(0, iPercent);

	DEBUG_CATCH
	}

int CSystem::CalcLocationWeight (CLocationDef *pLoc, const CAttributeCriteria &Criteria)

//	CalcLocationWeight
//
//	Calculates the weight of the given location relative to the given
//	criteria.
//
//	See: CAttributeCriteria::CalcWeightAdj
//
//	EXAMPLES:
//
//	Criteria = "*"			LocationAttribs = "{anything}"		Result = 1000
//	Criteria = "+asteroids"	LocationAttribs = "asteroids"		Result = 2000
//	Criteria = "+asteroids"	LocationAttribs = "foo"				Result = 1000
//	Criteria = "-asteroids"	LocationAttribs = "asteroids"		Result = 500
//	Criteria = "-asteroids"	LocationAttribs = "foo"				Result = 1000

	{
	int i;
	int iWeight = 1000;

	//	Handle edge cases

	if (Criteria.MatchesAll())
		return iWeight;

	//	Adjust weight based on criteria

	const CString &sAttributes = pLoc->GetAttributes();
	CVector vPos = pLoc->GetOrbit().GetObjectPos();

	for (i = 0; i < Criteria.GetCount(); i++)
		{
		DWORD dwMatchStrength;
		const CString &sAttrib = Criteria.GetAttribAndWeight(i, &dwMatchStrength);

		//	Do we have the attribute? Check the location and any attributes
		//	inherited from territories and the system.

		bool bHasAttrib = (::HasModifier(sAttributes, sAttrib)
				|| HasAttribute(vPos, sAttrib));

		//	Compute the frequency of the given attribute

		int iAttribFreq = m_Universe.GetAttributeDesc().GetLocationAttribFrequency(sAttrib);

		//	Adjust probability based on the match strength

		int iAdj = CAttributeCriteria::CalcWeightAdj(bHasAttrib, dwMatchStrength, iAttribFreq);
		iWeight = iWeight * iAdj / 1000;

		//	If weight is 0, then no need to continue

		if (iWeight == 0)
			return 0;
		}

	//	Done

	return iWeight;
	}

CVector CSystem::CalcRandomEncounterPos (const CSpaceObject &TargetObj, Metric rDistance, const CSpaceObject *pEncounterBase) const

//	CalcRandomEncounterPos
//
//	Generates a random position for an encounter attacking the given target.
//
//	LATER: Use pEncounterBase to come up with a random position near the line
//	between the target and the base.

	{
	static constexpr Metric MIN_SEPARATION =			20.0 * LIGHT_SECOND;
	static constexpr Metric MIN_PLAYER_SEPARATION =		100.0 * LIGHT_SECOND;
	static constexpr Metric MIN_PLAYER_SEPARATION2 =	MIN_PLAYER_SEPARATION * MIN_PLAYER_SEPARATION;

	CVector vPos;

	CSpaceObject *pPlayer = GetPlayerShip();
	Metric rPlayerDist = (pPlayer && pPlayer != &TargetObj ? pPlayer->GetDistance(&TargetObj) : 0.0);
	Metric rSeparation = Min(0.5 * rDistance, MIN_SEPARATION);

	//	If the target is the player, then we just pick a position in a circle
	//	around the player. Or, if the player is so far away that there's no 
	//	chance of spawning near her, then pick a random position.

	if (TargetObj.IsPlayer()
			|| pPlayer == NULL
			|| rPlayerDist > rDistance + MIN_PLAYER_SEPARATION
			|| rPlayerDist < MIN_PLAYER_SEPARATION - rDistance)
		{
		int iTries = 100;
		while (iTries > 0)
			{
			vPos = TargetObj.GetPos() + ::PolarToVector(mathRandom(0, 359), rDistance);

			if (FindObjectInRange(vPos, rSeparation, CSpaceObjectCriteria(SEPARATION_CRITERIA))
					&& --iTries > 0)
				continue;

			//	Found a good position

			return vPos;
			}

		//	If not found, then we just return an arbitrary position

		return vPos;
		}

	//	Otherwise, we pick a location at the given distance from the target, but
	//	away from the player

	else
		{
		int iTries = 100;
		while (iTries > 0)
			{
			vPos = TargetObj.GetPos() + ::PolarToVector(mathRandom(0, 359), rDistance);

			//	If this position is too close to the player, or if it is too 
			//	close to some other object, then skip.

			if (((pPlayer->GetPos() - vPos).Length2() < MIN_PLAYER_SEPARATION2)
					|| FindObjectInRange(vPos, rSeparation, CSpaceObjectCriteria(SEPARATION_CRITERIA)))
				{
				if (--iTries > 0)
					continue;
				}

			//	Found a good position

			return vPos;
			}

		//	If not found, then we just return an arbitrary position

		return vPos;
		}
	}

CG32bitPixel CSystem::CalculateSpaceColor (CSpaceObject *pPOV, CSpaceObject **retpStar, const CG8bitSparseImage **retpVolumetricMask)

//	CalculateSpaceColor
//
//	Calculates the color of space from the given object

	{
	CSpaceObject *pStar;
	int iPercent = CalculateLightIntensity(pPOV->GetPos(), &pStar, retpVolumetricMask);
	if (pStar == NULL || iPercent == 0)
		return CG32bitPixel::Null();

	CG32bitPixel rgbStarColor = pStar->GetSpaceColor();
	if (rgbStarColor.IsNull())
		return rgbStarColor;

	if (retpStar)
		*retpStar = pStar;

	return CG32bitPixel(rgbStarColor, (BYTE)(MAX_SPACE_OPACITY * iPercent / 100));
	}

void CSystem::CalcViewportCtx (SViewportPaintCtx &Ctx, const RECT &rcView, CSpaceObject *pCenter, DWORD dwFlags)

//	CalcViewportCtx
//
//	Initializes the viewport context

	{
	DEBUG_TRY
	ASSERT(pCenter);

	Ctx.pCenter = pCenter;
	Ctx.vCenterPos = pCenter->GetPos();
	Ctx.rcView = rcView;
	Ctx.vDiagonal = CVector(g_KlicksPerPixel * (Metric)(RectWidth(rcView)) / 2,
				g_KlicksPerPixel * (Metric)(RectHeight(rcView)) / 2);
	Ctx.vUR = Ctx.vCenterPos + Ctx.vDiagonal;
	Ctx.vLL = Ctx.vCenterPos - Ctx.vDiagonal;

	//	Perception

	Ctx.iPerception = pCenter->GetPerception();

	//	Compute the transformation to map world coordinates to the viewport

	Ctx.xCenter = rcView.left + RectWidth(rcView) / 2;
	Ctx.yCenter = rcView.top + RectHeight(rcView) / 2;
	Ctx.XForm = ViewportTransform(Ctx.vCenterPos, g_KlicksPerPixel, Ctx.xCenter, Ctx.yCenter);
	Ctx.XFormRel = Ctx.XForm;

	//	Figure out the extended boundaries. This is used for enhanced display.

	Ctx.vEnhancedDiagonal = CVector(g_LRSRange, g_LRSRange);
	Ctx.vEnhancedUR = Ctx.vCenterPos + Ctx.vEnhancedDiagonal;
	Ctx.vEnhancedLL = Ctx.vCenterPos - Ctx.vEnhancedDiagonal;

	//	Initialize some flags

	Ctx.bEnhancedDisplay = ((dwFlags & VWP_ENHANCED_DISPLAY) ? true : false);
	Ctx.bShowUnexploredAnnotation = ((dwFlags & VWP_MINING_DISPLAY) ? true : false);
	Ctx.fNoStarfield = ((dwFlags & VWP_NO_STAR_FIELD) ? true : false);
	Ctx.fShowManeuverEffects = m_Universe.GetSFXOptions().IsManeuveringEffectEnabled();
	Ctx.fNoStarshine = !m_Universe.GetSFXOptions().IsStarshineEnabled();
	Ctx.fNoSpaceBackground = !m_Universe.GetSFXOptions().IsSpaceBackgroundEnabled();

	//	Debug options

	Ctx.bShowBounds = m_Universe.GetDebugOptions().IsShowBoundsEnabled();
	Ctx.bShowFacingsAngle = m_Universe.GetDebugOptions().IsShowFacingsAngleEnabled();

	//	Figure out what color space should be. Space gets lighter as we get
	//	near the central star

	Ctx.rgbSpaceColor = CalculateSpaceColor(pCenter, &Ctx.pStar, &Ctx.pVolumetricMask);

	//	Compute the radius of the circle on which we'll show target indicators
	//	(in pixels)

	Ctx.rIndicatorRadius = Min(RectWidth(rcView), RectHeight(rcView)) / 2.0;

	//	If we don't have a thread pool yet, create it

	if (m_pThreadPool == NULL)
		{
		m_pThreadPool = new CThreadPool;
		m_pThreadPool->Boot(Min(MAX_THREAD_COUNT, sysGetProcessorCount()));
		}

	Ctx.pThreadPool = m_pThreadPool;

	DEBUG_CATCH
	}

void CSystem::CalcVolumetricMask (CSpaceObject *pStar, CG8bitSparseImage &VolumetricMask)

//	CalcVolumetricMask
//
//	Initializes the volumetric mask for the given star

	{
	int i;

	Metric rMaxDist = (pStar->GetMaxLightDistance() + 100) * LIGHT_SECOND;
	int iSize = (int)(2.0 * rMaxDist / g_KlicksPerPixel);
	VolumetricMask.Create(iSize, iSize, 0xff);

	//	Star offset

	int xStar = (int)(pStar->GetPos().GetX() / g_KlicksPerPixel);
	int yStar = (int)(pStar->GetPos().GetY() / g_KlicksPerPixel);

	//	Loop over all planets/asteroids and generate a shadow

	for (i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);
		if (pObj == NULL
				|| pObj->IsDestroyed()
				|| !pObj->HasVolumetricShadow())
			continue;

		//	Compute the angle of the object with respect to the star
		//	And skip any objects that are outside the star's light radius.

		Metric rStarDist;
		int iStarAngle = ::VectorToPolar(pObj->GetPos() - pStar->GetPos(), &rStarDist);
		if (rStarDist > rMaxDist)
			continue;

		//	Generate an image lit from the proper angle

		pObj->CreateStarlightImage(iStarAngle, rStarDist);

		//	Add the shadow

		CVolumetricShadowPainter Painter(pStar, xStar, yStar, iStarAngle, rStarDist, pObj, VolumetricMask);
		Painter.PaintShadow();
		}
	}

void CSystem::CancelTimedEvent (CSpaceObject *pSource, bool bInDoEvent)

//	CancelTimedEvent
//
//	Cancel all events for this object.

	{
	m_TimedEvents.CancelEvent(pSource, bInDoEvent);
	}

void CSystem::CancelTimedEvent (CSpaceObject *pSource, const CString &sEvent, bool bInDoEvent)

//	CancelTimedEvent
//
//	Cancel event by name

	{
	m_TimedEvents.CancelEvent(pSource, sEvent, bInDoEvent);
	}

void CSystem::CancelTimedEvent (CDesignType *pSource, const CString &sEvent, bool bInDoEvent)

//	CancelTimedEvent
//
//	Cancel event by name

	{
	int i;

	for (i = 0; i < GetTimedEventCount(); i++)
		{
		CSystemEvent *pEvent = GetTimedEvent(i);
		if (pEvent->GetEventHandlerType() == pSource 
				&& strEquals(pEvent->GetEventHandlerName(), sEvent))
			{
			if (bInDoEvent)
				pEvent->SetDestroyed();
			else
				{
				m_TimedEvents.RemoveEvent(i);
				i--;
				}
			}
		}
	}

void CSystem::ComputeRandomEncounters (void)

//	ComputeRandomEncounters
//
//	Creates the table that lists all objects in the system that
//	can generate random encounters

	{
	int i;

	if (m_fEncounterTableValid)
		return;

	m_EncounterObjs.DeleteAll();
	if (!m_fNoRandomEncounters)
		{
		for (i = 0; i < GetObjectCount(); i++)
			{
			CSpaceObject *pObj = GetObject(i);

			if (pObj 
					&& pObj->HasRandomEncounters())
				m_EncounterObjs.Add(pObj);
			}
		}

	m_fEncounterTableValid = true;
	}

void CSystem::ComputeStars (void)

//	ComputeStars
//
//	Keep a list of the stars in the system

	{
	int i;

	m_Stars.DeleteAll();

	for (i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);

		if (pObj 
				&& pObj->GetScale() == scaleStar
				&& !pObj->IsDestroyed())
			{
			SStarDesc *pNewStar = m_Stars.Insert();
			pNewStar->pStarObj = pObj;
			}
		}
	}

ALERROR CSystem::CreateFlotsam (const CItem &Item, 
								const CVector &vPos, 
								const CVector &vVel, 
								CSovereign *pSovereign, 
								CStation **retpFlotsam)

//	CreateFlotsam
//
//	Creates a floating item

	{
	CItemType *pItemType = Item.GetType();
	if (pItemType == NULL)
		return ERR_FAIL;

	//	Create the station

	CStation *pFlotsam;
	pItemType->CreateEmptyFlotsam(*this, vPos, vVel, pSovereign, &pFlotsam);

	//	Add the items to the station

	CItemListManipulator ItemList(pFlotsam->GetItemList());
	ItemList.AddItem(Item);

	//	Done

	if (retpFlotsam)
		*retpFlotsam = pFlotsam;

	return NOERROR;
	}

ALERROR CSystem::CreateFromStream (CUniverse &Universe, 
								   IReadStream *pStream, 
								   CSystem **retpSystem,
								   CString *retsError,
								   DWORD dwObjID,
								   CSpaceObject **retpObj,
								   CSpaceObject *pPlayerShip)

//	CreateFromStream
//
//	Creates the star system from the stream
//
//	DWORD		m_dwID
//	DWORD		m_iTick
//	DWORD		m_iTimeStopped
//	CString		m_sName
//	CString		Topology node ID
//	DWORD		(unused)
//	DWORD		m_iNextEncounter
//	DWORD		flags
//	DWORD		SAVE VERSION (only if [flags & 0x02])
//	Metric		m_rKlicksPerPixel
//	Metric		m_rTimeScale
//	DWORD		m_iLastUpdated
//
//	DWORD		Number of CNavigationPath
//	CNavigationPath
//
//	CEventHandlers
//
//	DWORD		Number of mission objects
//	CSpaceObject
//
//	DWORD		Number of objects
//	CSpaceObject
//
//	DWORD		Number of named objects
//	CString		entrypoint: name
//	DWORD		entrypoint: CSpaceObject ref
//
//	DWORD		Number of timed events
//	CSystemEvent
//
//	DWORD		Number of environment maps
//	CTileMap
//
//	CLocationList		m_Locations
//	CTerritoryList		m_Territories
//	CObjectJointList	m_Joints

	{
	int i;
	DWORD dwLoad;
	DWORD dwCount;

	//	Create a context block for loading

	SLoadCtx Ctx(Universe);
	Ctx.dwVersion = 0;	//	Default to 0
	Ctx.pStream = pStream;

	//	Add all missions to the map so that they can be resolved

	for (i = 0; i < Universe.GetMissionCount(); i++)
		{
		CMission *pMission = Universe.GetMission(i);
		Ctx.ObjMap.Insert(pMission->GetID(), pMission);
		}

	//	Create the new star system

	Ctx.pSystem = new CSystem(Universe, NULL);
	if (Ctx.pSystem == NULL)
		{
		*retsError = CONSTLIT("Out of memory.");
		return ERR_MEMORY;
		}

	//	Load some misc info

	Ctx.pStream->Read((char *)&Ctx.pSystem->m_dwID, sizeof(DWORD));
	Ctx.pStream->Read((char *)&Ctx.pSystem->m_iTick, sizeof(DWORD));
	Ctx.pStream->Read((char *)&Ctx.pSystem->m_iTimeStopped, sizeof(DWORD));
	Ctx.pSystem->m_sName.ReadFromStream(Ctx.pStream);

	//	Load the topology node

	CString sNodeID;
	sNodeID.ReadFromStream(Ctx.pStream);
	Ctx.pSystem->m_pTopology = Universe.FindTopologyNode(sNodeID);
	Ctx.pSystem->m_pType = Universe.FindSystemType(Ctx.pSystem->m_pTopology->GetSystemTypeUNID());

	//	More misc info

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	Ctx.pStream->Read((char *)&Ctx.pSystem->m_iNextEncounter, sizeof(DWORD));

	//	Flags

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	Ctx.pSystem->m_fNoRandomEncounters =	((dwLoad & 0x00000001) ? true : false);
	if (dwLoad & 0x00000002)
		Ctx.pStream->Read((char *)&Ctx.dwVersion, sizeof(DWORD));
	Ctx.pSystem->m_fUseDefaultTerritories =	((dwLoad & 0x00000004) ? false : true);
	Ctx.pSystem->m_fEncounterTableValid = false;
	Ctx.pSystem->m_fEnemiesInLRS =			((dwLoad & 0x00000008) ? false : true);
	Ctx.pSystem->m_fEnemiesInSRS =			((dwLoad & 0x00000010) ? false : true);
	Ctx.pSystem->m_fPlayerUnderAttack =		((dwLoad & 0x00000020) ? false : true);

	if (Ctx.dwVersion >= 141)
		Ctx.pSystem->m_fLocationsBlocked =	((dwLoad & 0x00000040) ? false : true);
	else
		{
		//	For previous versions we always assume we've already processed 
		//	locations.

		Ctx.pSystem->m_fLocationsBlocked = true;
		}

	//	Scales

	if (Ctx.dwVersion >= 9)
		{
		Ctx.pStream->Read((char *)&Ctx.pSystem->m_rKlicksPerPixel, sizeof(Metric));
		Ctx.pStream->Read((char *)&Ctx.pSystem->m_rTimeScale, sizeof(Metric));
		}
	else
		{
		Ctx.pSystem->m_rKlicksPerPixel = KLICKS_PER_PIXEL;
		Ctx.pSystem->m_rTimeScale = TIME_SCALE;
		}

	if (Ctx.dwVersion >= 57)
		Ctx.pStream->Read((char *)&Ctx.pSystem->m_iLastUpdated, sizeof(DWORD));
	else
		Ctx.pSystem->m_iLastUpdated = -1;

	//	Load the navigation paths (we load these before objects
	//	because objects might have references to paths)

	if (Ctx.dwVersion >= 10)
		Ctx.pSystem->m_NavPaths.ReadFromStream(Ctx);

	//	Load the system event handlers

	if (Ctx.dwVersion >= 35)
		Ctx.pSystem->m_EventHandlers.ReadFromStream(Ctx);

	//	Load all objects

	Ctx.pStream->Read((char *)&dwCount, sizeof(DWORD));
	for (i = 0; i < (int)dwCount; i++)
		{
		//	Load the object

		CSpaceObject *pObj;
		try
			{
			CSpaceObject::CreateFromStream(Ctx, &pObj);
			}
		catch (...)
			{
			*retsError = CSpaceObject::DebugLoadError(Ctx);
			return ERR_FAIL;
			}

		//	Add this object to the map

		DWORD dwID = (Ctx.dwVersion >= 41 ? pObj->GetID() : pObj->GetIndex());
		Ctx.ObjMap.Insert(dwID, pObj);

		//	Update any previous objects that are waiting for this reference

		Ctx.ForwardReferences.ResolveRefs(dwID, pObj);

		//	Set the system (note: this will change the index to the new
		//	system)

		pObj->AddToSystem(*Ctx.pSystem, true);
		}

	//	If we have old style registrations then we need to convert to subscriptions

	if (Ctx.dwVersion < 77)
		{
		for (i = 0; i < Ctx.pSystem->GetObjectCount(); i++)
			{
			CSpaceObject *pObj = Ctx.pSystem->GetObject(i);
			if (pObj)
				{
				TArray<CSpaceObject *> *pList = Ctx.Subscribed.GetAt(pObj->GetID());
				if (pList)
					{
					for (int j = 0; j < pList->GetCount(); j++)
						pObj->AddEventSubscriber(pList->GetAt(j));
					}
				}
			}
		}

	//	If there are still references to the player, resolve them now.
	//	[This happens because of a bug in 1.0 RC1]

	if (pPlayerShip)
		Ctx.ForwardReferences.ResolveRefs(pPlayerShip->GetID(), pPlayerShip);

	//	If we've got left over references, then dump debug output

	if (Ctx.ForwardReferences.HasUnresolved())
		{
		*retsError = strPatternSubst(CONSTLIT("Undefined object references"));
		return ERR_FAIL;
		}

	//	Load named objects table

	Ctx.pStream->Read((char *)&dwCount, sizeof(DWORD));
	for (i = 0; i < (int)dwCount; i++)
		{
		CString sName;
		sName.ReadFromStream(Ctx.pStream);

		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
		CSpaceObject *pObj;
		if (!Ctx.ObjMap.Find(dwLoad, &pObj))
			{
			*retsError = strPatternSubst(CONSTLIT("Save file error: Unable to find named object: %s [%x]"), sName, dwLoad);
			return ERR_FAIL;
			}

		Ctx.pSystem->NameObject(sName, *pObj);
		}

	//	Load all timed events

	Ctx.pSystem->m_TimedEvents.ReadFromStream(Ctx);

	//	Load environment map

	Ctx.pStream->Read((char *)&dwCount, sizeof(DWORD));
	if (dwCount)
		{
		Ctx.pSystem->m_pEnvironment = new CEnvironmentGrid(CSystemType::sizeUnknown);
		Ctx.pSystem->m_pEnvironment->ReadFromStream(Ctx);
		}
	else
		Ctx.pSystem->m_pEnvironment = NULL;

	//	Load locations

	if (Ctx.dwVersion >= 72)
		Ctx.pSystem->m_Locations.ReadFromStream(Ctx);

	//	Load territories

	if (Ctx.dwVersion >= 60)
		Ctx.pSystem->m_Territories.ReadFromStream(Ctx);

	//	Load joints

	if (Ctx.dwVersion >= 143)
		Ctx.pSystem->m_Joints.ReadFromStream(Ctx);

	//	Create the background star field

	Ctx.pSystem->m_SpacePainter.CleanUp();

	//	Compute some tables

	Ctx.pSystem->ComputeStars();
	
	//	Map the POV object

	if (retpObj)
		{
		if (!Ctx.ObjMap.Find(dwObjID, retpObj))
			{
			*retsError = strPatternSubst(CONSTLIT("Unable to find POV object: %x"), dwObjID);

			//	Look for the player object

			bool bFound = false;
			for (i = 0; i < Ctx.pSystem->GetObjectCount(); i++)
				{
				CSpaceObject *pObj = Ctx.pSystem->GetObject(i);

				if (pObj && pObj->IsPlayer())
					{
					*retpObj = pObj;
					bFound = true;
					break;
					}
				}

			if (!bFound)
				retsError->Append(CONSTLIT("\r\nUnable to find player ship."));

			return ERR_FAIL;
			}
		}

	//	Tell all objects that the system has been loaded

	for (i = 0; i < Ctx.pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = Ctx.pSystem->GetObject(i);

		if (pObj)
			{
			pObj->LoadObjReferences(Ctx.pSystem);
			pObj->OnSystemLoaded();

			pObj->FireOnLoad(Ctx);
			}
		}

	//	Done

	*retpSystem = Ctx.pSystem;

	return NOERROR;
	}

ALERROR CSystem::CreateLocation (const CString &sID, const COrbit &Orbit, const CString &sAttributes, CLocationDef **retpLocation)

//	CreateLocation
//
//	Creates a location

	{
	CLocationDef *pLocation = m_Locations.Insert(sID);
	pLocation->SetOrbit(Orbit);
	pLocation->SetAttributes(sAttributes);
	if (retpLocation)
		*retpLocation = pLocation;

	return NOERROR;
	}

ALERROR CSystem::CreateRandomEncounter (IShipGenerator *pTable, 
										CSpaceObject *pBase,
										CSovereign *pBaseSovereign,
										CSpaceObject *pTarget,
										CSpaceObject *pGate)

//	CreateRandomEncounter
//
//	Creates a random ship encounter

	{
	ASSERT(pTable);

	SShipCreateCtx Ctx;
	Ctx.pSystem = this;
	Ctx.pBase = pBase;
	Ctx.pBaseSovereign = pBaseSovereign;
	Ctx.pTarget = pTarget;

	//	Figure out where the encounter will come from

	if (pGate && pGate->IsActiveStargate())
		Ctx.pGate = pGate;
	else if (pGate)
		{
		Ctx.vPos = pGate->GetPos();
		Ctx.PosSpread = DiceRange(2, 1, 2);
		}
	else if (pTarget)
		//	Exclude uncharted stargates
		Ctx.pGate = pTarget->GetNearestStargate(true);

	//	Generate ship

	pTable->CreateShips(Ctx);

	return NOERROR;
	}

ALERROR CSystem::CreateShip (DWORD dwClassID,
							 IShipController *pController,
							 CDesignType *pOverride,
							 CSovereign *pSovereign,
							 const CVector &vPos,
							 const CVector &vVel,
							 int iRotation,
							 CSpaceObject *pExitGate,
							 SShipGeneratorCtx *pCtx,
							 CShip **retpShip,
							 CSpaceObjectList *retpShipList)

//	CreateShip
//
//	Creates a ship based on the class.
//
//	pController is owned by the ship if this call is successful.

	{
	DEBUG_TRY

	ALERROR error;
	CDesignType *pType = m_Universe.FindDesignType(dwClassID);
	if (pType == NULL)
		return ERR_FAIL;

	//	If we have a ship table, then we go through a totally different path

	if (pType->GetType() == designShipTable)
		{
		//	Get the table

		CShipTable *pTable = CShipTable::AsType(pType);
		if (pTable == NULL)
			return ERR_FAIL;

		SShipCreateCtx CreateCtx;
		CreateCtx.pSystem = this;
		CreateCtx.pGate = pExitGate;
		CreateCtx.vPos = vPos;
		CreateCtx.pBaseSovereign = pSovereign;
		CreateCtx.pEncounterInfo = NULL;
		CreateCtx.pOverride = pOverride;
		CreateCtx.dwFlags = SShipCreateCtx::RETURN_RESULT;

		//	Create

		pTable->CreateShips(CreateCtx);

		//	Return at least one of the ships created

		if (CreateCtx.Result.GetCount() == 0)
			return ERR_NOTFOUND;

		if (retpShip)
			*retpShip = CreateCtx.Result.GetObj(0)->AsShip();

		if (retpShipList)
			*retpShipList = CreateCtx.Result;

		//	On this path, we don't need the controller.

		if (pController)
			delete pController;

		return NOERROR;
		}

	//	Find the class

	CShipClass *pClass = CShipClass::AsType(pType);
	if (pClass == NULL)
		return ERR_FAIL;

	//	Create the controller

	if (pController == NULL)
		{
		pController = new CStandardShipAI;
		if (pController == NULL)
			return ERR_MEMORY;
		}

	//	Create a new ship based on the class

	CShip *pShip;
	if (error = CShip::CreateFromClass(*this, 
			pClass, 
			pController, 
			pOverride,
			pSovereign, 
			vPos, 
			vVel, 
			iRotation,
			pCtx,
			&pShip))
		return error;

	//	If the ship destroyed itself, then we return NOT_FOUND

	if (pShip->IsDestroyed())
		return ERR_NOTFOUND;

	//	If we're coming out of a gate, set the timer

	if (pExitGate)
		PlaceInGate(pShip, pExitGate);

	//	Load images, if necessary

	if (!IsCreationInProgress())
		{
		m_Universe.SetLogImageLoad(false);
		pShip->MarkImages();
		m_Universe.SetLogImageLoad(true);
		}

	//	Create escorts, if necessary

	IShipGenerator *pEscorts = pClass->GetEscorts();
	if (pEscorts)
		{
		//	If the ship has changed its position, then the escorts should not
		//	appear at a gate. [This happens when an override handler moves the
		//	ship at create time to make the ship appear near the player.]

		CSpaceObject *pEscortGate = pExitGate;
		if (pExitGate == NULL || (pExitGate->GetPos() - pShip->GetPos()).Length2() > (LIGHT_SECOND * LIGHT_SECOND))
			pEscortGate = pShip;
		else
			pEscortGate = pExitGate;

		//	Create escorts

		SShipCreateCtx ECtx;
		ECtx.pSystem = this;
		ECtx.vPos = pEscortGate->GetPos();
		ECtx.pBase = pShip;
		ECtx.pTarget = NULL;
		ECtx.pGate = pEscortGate;

		pEscorts->CreateShips(ECtx);
		}

	if (retpShip)
		*retpShip = pShip;

	if (retpShipList)
		{
		retpShipList->DeleteAll();
		retpShipList->Add(pShip);
		}

	return NOERROR;

	DEBUG_CATCH
	}

ALERROR CSystem::CreateShipwreck (CShipClass *pClass,
								  const CVector &vPos,
								  const CVector &vVel,
								  CSovereign *pSovereign,
								  CStation **retpWreck)

//	CreateShipWreck
//
//	Creates an empty ship wreck of the given class

	{
	if (!pClass->CreateEmptyWreck(*this,
			NULL,
			vPos,
			vVel,
			pSovereign,
			retpWreck))
		return ERR_FAIL;

	return NOERROR;
	}

void GenerateSquareDist (int iTotalCount, int iMinValue, int iMaxValue, int *Dist)

//	GenerateSquareDist
//
//	Generates buckets such that:
//
//	1. The sum of the buckets = iTotalCount
//	2. Each bucket has units proportional to the square of its index value
//
//	Dist must be allocated to at least iMaxValue + 1

	{
	int i;

	//	First generate a square distribution

	int iTotalProb = 0;
	for (i = 0; i < iMaxValue + 1; i++)
		{
		if (i >= iMinValue)
			Dist[i] = i * i;
		else
			Dist[i] = 0;

		iTotalProb += Dist[i];
		}

	ASSERT(iTotalProb > 0);
	if (iTotalProb == 0)
		return;

	//	Scale the distribution to the total count

	int iLeft = iTotalCount;
	for (i = 0; i < iMaxValue + 1; i++)
		{
		int iNumerator = Dist[i] * iTotalCount;
		int iBucketCount = iNumerator / iTotalProb;
		int iBucketCountRemainder = iNumerator % iTotalProb;
		if (mathRandom(0, iTotalProb - 1) < iBucketCountRemainder)
			iBucketCount++;

		iBucketCount = Min(iBucketCount, iLeft);
		Dist[i] = iBucketCount;
		iLeft -= iBucketCount;
		}
	}

ALERROR CSystem::CreateStargate (CStationType *pType,
								 CVector &vPos,
								 const CString &sStargateID,
								 const CString &sDestNodeID,
								 const CString &sDestStargateID,
								 CSpaceObject **retpStation)

//	CreateStargate
//
//	Creates a stargate in the system

	{
	ALERROR error;
	CStation *pStation;

	CTopologyNode *pDestNode = m_Universe.FindTopologyNode(sDestNodeID);
	if (pDestNode == NULL)
		return ERR_FAIL;

	//	Create the station

	CSpaceObject *pObj;
	if (error = CreateStation(pType, NULL, vPos, &pObj))
		return error;

	pStation = pObj->AsStation();
	if (pStation == NULL)
		return ERR_FAIL;

	//	Create stargate stuff

	if (error = StargateCreated(pStation, sStargateID, sDestNodeID, sDestStargateID))
		return error;

	//	Set stargate properties (note: CreateStation also looks at objName and adds the name
	//	to the named-objects system table.)

	pStation->SetStargate(sDestNodeID, sDestStargateID);

	//	If we haven't already set the name, set the name of the stargate
	//	to include the name of the destination system

	if (!pStation->IsNameSet())
		pStation->SetName(strPatternSubst(CONSTLIT("%s Stargate"), pDestNode->GetSystemName()), nounDefiniteArticle);

	//	Done

	if (retpStation)
		*retpStation = pStation;

	return NOERROR;
	}

ALERROR CSystem::CreateStation (CStationType *pType,
							    CDesignType *pEventHandler,
								CVector &vPos,
								CSpaceObject **retpStation)

//	CreateStation
//
//	Creates a station outside of a system definition

	{
	ALERROR error;

	//	Generate context block

	SSystemCreateCtx Ctx(*this);

	//	Generate an orbit. First we look for the nearest object with
	//	an orbit.

	CSpaceObject *pBestObj = NULL;
	Metric rBestDist2 = g_InfiniteDistance * g_InfiniteDistance;
	for (int i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);
		if (pObj && pObj->GetMapOrbit())
			{
			Metric rDist2 = (pObj->GetPos() - vPos).Length2();
			if (rDist2 < rBestDist2)
				{
				pBestObj = pObj;
				rBestDist2 = rDist2;
				}
			}
		}

	//	If we found an object, create an orbit around it. Otherwise, orbit around origin

	CVector vCenter = (pBestObj ? pBestObj->GetPos() : CVector());
	Metric rRadius;
	Metric rAngle = VectorToPolarRadians(vPos - vCenter, &rRadius);
	COrbit NewOrbit(vCenter, rRadius, rAngle);

	//	Create the station

	SObjCreateCtx CreateCtx(Ctx);
	CreateCtx.vPos = vPos;
	CreateCtx.pOrbit = &NewOrbit;
	CreateCtx.bCreateSatellites = true;
	CreateCtx.pEventHandler = pEventHandler;

	CSpaceObject *pStation;
	if (error = CreateStation(&Ctx,
			pType,
			CreateCtx,
			&pStation))
		return error;

	//	Fire deferred OnCreate

	if (error = Ctx.Events.FireDeferredEvent(ON_CREATE_EVENT, &Ctx.sError))
		kernelDebugLogPattern("Deferred OnCreate: %s", Ctx.sError);

	//	Recompute encounter table

	m_fEncounterTableValid = false;

	//	Done

	if (retpStation)
		*retpStation = pStation;

	return NOERROR;
	}

ALERROR CSystem::CreateWeaponFire (SShotCreateCtx &Ctx, CSpaceObject **retpShot)

//	CreateWeaponFire
//
//	Creates a shot

	{
	CSpaceObject *pShot;

	switch (Ctx.pDesc->GetType())
		{
		case CWeaponFireDesc::ftBeam:
		case CWeaponFireDesc::ftMissile:
			{
			CMissile *pMissile;

			CMissile::Create(*this, Ctx, &pMissile);

			pShot = pMissile;
			break;
			}

		case CWeaponFireDesc::ftContinuousBeam:
			{
			CContinuousBeam *pBeam;

			CContinuousBeam::Create(*this, Ctx, &pBeam);

			pShot = pBeam;
			break;
			}

		case CWeaponFireDesc::ftArea:
			{
			CAreaDamage *pArea;

			CAreaDamage::Create(*this, Ctx, &pArea);

			pShot = pArea;
			break;
			}

		case CWeaponFireDesc::ftParticles:
			{
			CParticleDamage *pParticles;

			CParticleDamage::Create(*this, Ctx, &pParticles);

			pShot = pParticles;
			break;
			}

		case CWeaponFireDesc::ftRadius:
			{
			CRadiusDamage *pRadius;

			CRadiusDamage::Create(*this, Ctx, &pRadius);

			pShot = pRadius;
			break;
			}

		default:
			ASSERT(FALSE);
			pShot = NULL;
		}

	//	If no shot, then we're done

	if (pShot == NULL)
		{
		if (retpShot) *retpShot = NULL;
		return NOERROR;
		}

	//	Fire OnCreateShot event

	Ctx.pDesc->FireOnCreateShot(Ctx.Source, pShot, Ctx.pTarget);

	//	Fire a system events, if we have any handlers

	FireSystemWeaponEvents(pShot, Ctx.pDesc, Ctx.Source, Ctx.iRepeatingCount, Ctx.dwFlags);

	//	Done

	if (retpShot)
		*retpShot = pShot;

	return NOERROR;
	}

ALERROR CSystem::CreateWeaponFragments (SShotCreateCtx &Ctx, CSpaceObject *pMissileSource, int iFraction)

//	CreateWeaponFragments
//
//	Creates the fragments from a fragmentation weapon

	{
	DEBUG_TRY

	ALERROR error;
	int i;

	CWeaponFireDesc::SFragmentDesc *pFragDesc = Ctx.pDesc->GetFirstFragment();
	while (pFragDesc)
		{
		int iFragmentCount = pFragDesc->Count.Roll();
		if (iFragmentCount > 0)
			{
			TArray<int> Angles;
			Angles.InsertEmpty(iFragmentCount);
			TArray<CSpaceObject *> Targets;
			Targets.InsertEmpty(iFragmentCount);

			//	If we have lots of fragments then we just pick random angles

			if (iFragmentCount > 90)
				{
				for (i = 0; i < iFragmentCount; i++)
					{
					Angles[i] = mathRandom(0, 359);
					Targets[i] = Ctx.pTarget;
					}
				}

			//	Otherwise, we try to distribute evenly

			else
				{
				int iAngleOffset = mathRandom(0, 359);
				int iAngleVar = 90 / iFragmentCount;

				//	Compute angles for each fragment

				int iAngleInc = 360 / iFragmentCount;
				for (i = 0; i < iFragmentCount; i++)
					{
					Angles[i] = AngleMod(iAngleOffset + (iAngleInc * i) + mathRandom(-iAngleVar, iAngleVar));
					Targets[i] = Ctx.pTarget;
					}
				}

			//	For multitargets, we need to find a target 
			//	for each fragment

			if (pFragDesc->bMIRV)
				{
				TArray<CSpaceObject *> TargetList;

				if (pMissileSource)
					{
					int iFound = pMissileSource->GetNearestVisibleEnemies(iFragmentCount, 
							MAX_MIRV_TARGET_RANGE, 
							&TargetList, 
							NULL, 
							FLAG_INCLUDE_NON_AGGRESSORS | FLAG_INCLUDE_STATIONS);

					Metric rSpeed = pFragDesc->pDesc->GetInitialSpeed();

					if (iFound > 0)
						{
						for (i = 0; i < iFragmentCount; i++)
							{
							CSpaceObject *pTarget = TargetList[i % iFound];
							Targets[i] = pTarget;

							//	Calculate direction to fire in

							CVector vTarget = pTarget->GetPos() - Ctx.vPos;
							Metric rTimeToIntercept = CalcInterceptTime(vTarget, pTarget->GetVel(), rSpeed);
							CVector vInterceptPoint = vTarget + pTarget->GetVel() * rTimeToIntercept;

							//	If fragments can maneuver, then fire angle jitters a bit.

							if (pFragDesc->pDesc->IsTracking())
								Angles[i] = AngleMod(VectorToPolar(vInterceptPoint, NULL) + mathRandom(-45, 45));

							//	If we've got multiple fragments to the same target, then
							//	jitter a bit.

							else if (i >= iFound)
								Angles[i] = AngleMod(VectorToPolar(vInterceptPoint, NULL) + mathRandom(-6, 6));

							//	Otherwise, head straight for the target

							else
								Angles[i] = VectorToPolar(vInterceptPoint, NULL);
							}
						}

					//	If no targets found, an we require a target, then we skip

					else if (Ctx.pDesc->IsTargetRequired() || pFragDesc->pDesc->IsTargetRequired())
						{
						pFragDesc = pFragDesc->pNext;
						continue;
						}
					}
				}

			//	The initial velocity is the velocity of the missile
			//	(unless we are MIRVed)

			CVector vInitVel;
			if (!pFragDesc->bMIRV)
				vInitVel = Ctx.vVel;

			//	If we don't want to create all fragments, we randomly delete 
			//	some fragments (by setting angle to -1).

			if (iFraction < 100)
				{
				int iNewFragmentCount = Max(1, iFraction * iFragmentCount / 100);
				if (iNewFragmentCount < iFragmentCount)
					{
					TArray<int> ToDelete;
					ToDelete.InsertEmpty(iFragmentCount);
					for (i = 0; i < iFragmentCount; i++)
						ToDelete[i] = i;

					ToDelete.Shuffle();

					//	Randomly pick some fragments to delete.

					int iDeleteCount = iFragmentCount - iNewFragmentCount;
					for (i = 0; i < iDeleteCount; i++)
						{
						ASSERT(Angles[ToDelete[i]] >= 0);
						Angles[ToDelete[i]] = -1;
						}
					}
				}

			//	Create the fragments

			for (i = 0; i < iFragmentCount; i++)
				{
                //  If we're only creating a fraction of fragments, then skip some.

                if (Angles[i] < 0)
                    continue;

                //  Generate initial speed (this might be random for each fragment)

				Metric rSpeed = pFragDesc->pDesc->GetInitialSpeed();

                //  Create the fragment

				SShotCreateCtx FragCtx;
				FragCtx.pDesc = pFragDesc->pDesc;
				FragCtx.pEnhancements = Ctx.pEnhancements;
				FragCtx.Source = Ctx.Source;
				FragCtx.vPos = Ctx.vPos + CVector(mathRandom(-10, 10) * g_KlicksPerPixel / 10.0, mathRandom(-10, 10) * g_KlicksPerPixel / 10.0);
				FragCtx.vVel = vInitVel + PolarToVector(Angles[i], rSpeed);
				FragCtx.iDirection = Angles[i];
				FragCtx.pTarget = Targets[i];
				FragCtx.dwFlags = SShotCreateCtx::CWF_FRAGMENT;

				CSpaceObject *pNewObj;
				if (error = CreateWeaponFire(FragCtx, &pNewObj))
					return error;
				}
			}

		pFragDesc = pFragDesc->pNext;
		}

	return NOERROR;

	DEBUG_CATCH
	}

bool CSystem::DescendObject (DWORD dwObjID, const CVector &vPos, CSpaceObject **retpObj, CString *retsError)

//	DescendObject
//
//	Descends the object back to the system.

	{
	CSpaceObject *pObj = m_Universe.RemoveAscendedObj(dwObjID);
	if (pObj == NULL)
		{
		//	See if this object is already descended. Then we succeed.

		pObj = m_Universe.FindObject(dwObjID);
		if (pObj && pObj->GetSystem() == this && !pObj->IsAscended())
			{
			if (retpObj)
				*retpObj = pObj;

			return true;
			}

		//	Otherwise, failure

		else
			{
			if (retsError)
				*retsError = CONSTLIT("Object not ascended.");
			return false;
			}
		}

	pObj->SetAscended(false);

	//	Clear the time-stop flag if necessary

	if (pObj->IsTimeStopped())
		pObj->RestartTime();

	//	Place the ship at the gate in the new system

	pObj->Place(vPos);
	pObj->AddToSystem(*this);
	pObj->NotifyOnNewSystem(this);
	pObj->Resume();

	//	Done

	if (retpObj)
		*retpObj = pObj;

	return true;
	}

CSpaceObject *CSystem::FindObject (DWORD dwID) const

//	FindObject
//
//	Finds the object with the given ID (or NULL)

	{
	int i;

	for (i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);
		if (pObj && pObj->GetID() == dwID && !pObj->IsDestroyed())
			return pObj;
		}

	return NULL;
	}

CSpaceObject *CSystem::FindObjectInRange (const CVector &vCenter, Metric rRange, const CSpaceObjectCriteria &Criteria) const

//	FindObjectInRange
//
//	Returns an object in range of the given position.
//	NOTE: We do not return the nearest object, just an arbitrary object inside 
//	the range.

	{
	//	If we have a criteria, we need to check.

	if (!Criteria.IsEmpty())
		{
		CCriteriaObjSelector Selector(Criteria);
		CNearestInRadiusRange Range(vCenter, rRange);

		return CSpaceObjectEnum::FindObjInRange(*this, Range, Selector);
		}

	//	If we don't have a criteria, then we can do this faster.

	else 
		{
		CAnyObjSelector Selector;
		CNearestInRadiusRange Range(vCenter, rRange);

		return CSpaceObjectEnum::FindObjInRange(*this, Range, Selector);
		}
	}

CSpaceObject *CSystem::FindObjectWithOrbit (const COrbit &Orbit) const

//  FindObjectWithOrbit
//
//  Returns an object that shows the given orbit

    {
	int i;

	for (i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);
		const COrbit *pOrbit;
		if (pObj 
                && !pObj->IsDestroyed()
                && pObj->ShowMapOrbit()
                && (pOrbit = pObj->GetMapOrbit())
				&& (*pOrbit == Orbit))
			return pObj;
		}

	return NULL;
    }

bool CSystem::FindObjectName (CSpaceObject *pObj, CString *retsName)

//	FindObjectName
//
//	Finds the name of the given object (if it has one)

	{
	int i;

	for (i = 0; i < m_NamedObjects.GetCount(); i++)
		if (m_NamedObjects[i] == pObj)
			{
			if (retsName)
				*retsName = m_NamedObjects.GetKey(i);
			return true;
			}

	return false;
	}

bool CSystem::FindRandomLocation (const SLocationCriteria &Criteria, DWORD dwFlags, const COrbit &CenterOrbitDesc, CStationType *pStationToPlace, int *retiLocID)

//	FindRandomLocation
//
//	Finds a random location weighted towards those that match the
//	given criteria.

	{
	//	Generate a table of LocationIDs that match the given criteria.

	TProbabilityTable<int> Table;
	if (!GetEmptyLocations(Criteria, CenterOrbitDesc, pStationToPlace, &Table))
		return false;

	//	Done

	if (retiLocID)
		*retiLocID = Table[Table.RollPos()];

	return true;
	}

void CSystem::FireOnSystemExplosion (CSpaceObject *pExplosion, CWeaponFireDesc *pDesc, const CDamageSource &Source)

//	FireOnSystemExplosion
//
//	Fires OnSystemExplosion event to all handlers

	{
	CSpaceObject *pSource = Source.GetObj();
	CItemType *pWeapon = pDesc->GetWeaponType();
	DWORD dwWeaponUNID = (pWeapon ? pWeapon->GetUNID() : 0);
	CVector vPos = pExplosion->GetPos();

	CSystemEventHandler *pHandler = m_EventHandlers.GetNext();
	while (pHandler)
		{
		if (pHandler->InRange(vPos))
			pHandler->GetObj()->FireOnSystemExplosion(pExplosion, pSource, dwWeaponUNID);

		pHandler = pHandler->GetNext();
		}
	}

void CSystem::FireOnSystemObjAttacked (SDamageCtx &Ctx)

//	FireOnSystemObjAttacked
//
//	Fires OnSystemObjAttacked event to all handlers

	{
	DEBUG_TRY

	CSystemEventHandler *pHandler = m_EventHandlers.GetNext();
	while (pHandler)
		{
		if (pHandler->InRange(Ctx.pObj->GetPos()))
			pHandler->GetObj()->FireOnSystemObjAttacked(Ctx);

		pHandler = pHandler->GetNext();
		}

	DEBUG_CATCH
	}

void CSystem::FireOnSystemObjDestroyed (SDestroyCtx &Ctx)

//	FireOnSystemObjDestroyed
//
//	Fires OnSystemObjDestroyed event to all handlers

	{
	DEBUG_TRY

	CSystemEventHandler *pHandler = m_EventHandlers.GetNext();
	while (pHandler)
		{
		if (pHandler->InRange(Ctx.pObj->GetPos()))
			pHandler->GetObj()->FireOnSystemObjDestroyed(Ctx);

		pHandler = pHandler->GetNext();
		}

	DEBUG_CATCH
	}

void CSystem::FireOnSystemWeaponFire (CSpaceObject *pShot, CWeaponFireDesc *pDesc, const CDamageSource &Source, int iRepeatingCount)

//	FireOnSystemWeaponFire
//
//	Fires OnSystemWeaponFire event to all handlers

	{
	CSpaceObject *pSource = Source.GetObj();
	CItemType *pWeapon = pDesc->GetWeaponType();
	DWORD dwWeaponUNID = (pWeapon ? pWeapon->GetUNID() : 0);
	CVector vPos = pShot->GetPos();

	CSystemEventHandler *pHandler = m_EventHandlers.GetNext();
	while (pHandler)
		{
		if (pHandler->InRange(vPos))
			pHandler->GetObj()->FireOnSystemWeaponFire(pShot, pSource, dwWeaponUNID, iRepeatingCount);

		pHandler = pHandler->GetNext();
		}
	}

void CSystem::FireSystemWeaponEvents (CSpaceObject *pShot, CWeaponFireDesc *pDesc, const CDamageSource &Source, int iRepeatingCount, DWORD dwFlags)

//	FireSystemWeaponEvents
//
//	Fires events when a weapon is fired.

	{
	if (!m_EventHandlers.IsEmpty())
		{
		//	Skip any anything except the first shot in a multi-shot weapon.

		if (!(dwFlags & SShotCreateCtx::CWF_PRIMARY))
			NULL;

		//	Skip any fragments

		else if (dwFlags & SShotCreateCtx::CWF_FRAGMENT)
			NULL;

		//	If this is an explosion, then fire explosion event

		else if (dwFlags & SShotCreateCtx::CWF_EXPLOSION)
			{
			FireOnSystemExplosion(pShot, pDesc, Source);
			}

		//	If this is weapons fire, we fire that event

		else if (dwFlags & SShotCreateCtx::CWF_WEAPON_FIRE)
			{
			if (Source.GetObj() && Source.GetObj()->CanAttack())
				FireOnSystemWeaponFire(pShot, pDesc, Source, iRepeatingCount);
			}
		}
	}

void CSystem::FlushAllCaches (void)

//	FlushAllCaches
//
//	Flushes all caches to save memory.

	{
	FlushEnemyObjectCache();
	FlushDeletedObjects();
	}

void CSystem::FlushDeletedObjects (void)

//	FlushDeletedObjects
//
//	Flush deleted objects from the deleted list.

	{
	//	Clear out the grid, so that it's not holding on to stale objects.

	m_ObjGrid.DeleteAll();

	//	Flush objects deleted last tick

	for (int i = 0; i < m_DeletedObjects.GetCount(); i++)
		{
		CSpaceObject *pObj = m_DeletedObjects.GetObj(i);
		if (pObj->IsNamed())
			{
			}

		delete pObj;
		}

	m_DeletedObjects.DeleteAll();

	//	Event handlers

	if (m_fFlushEventHandlers)
		{
		m_EventHandlers.FlushDeletedObjs();
		m_fFlushEventHandlers = false;
		}
	}

void CSystem::FlushEnemyObjectCache (void)

//	FlushEnemyObjectCache
//
//	Flush the enemy object cache

	{
	int i;

	for (i = 0; i < m_Universe.GetSovereignCount(); i++)
		m_Universe.GetSovereign(i)->FlushEnemyObjectCache();
	}

CString CSystem::GetAttribsAtPos (const CVector &vPos)

//	GetAttribsAtPos
//
//	Returns the attributes at the given position

	{
	CString sAttribs = (m_pTopology ? m_pTopology->GetAttributes() : NULL_STR);
	return ::AppendModifiers(sAttribs, m_Territories.GetAttribsAtPos(vPos));
	}

void CSystem::GetDebugInfo (SDebugInfo &Info) const

//	GetDebugInfo
//
//	Returns debug info when we crash. 

	{
	TArray<CSpaceObject *> Stars;

	//	Loop over all objects

	for (int i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);
		if (pObj == NULL)
			continue;

		try
			{
			if (pObj->GetSystem() != this)
				Info.iBadObjs++;
			else if (pObj->IsDestroyed())
				Info.iDestroyedObjs++;
			else
				{
				Info.iTotalObjs++;
				}

			//	If this is a star, keep track.

			if (pObj->GetScale() == scaleStar)
				{
				Info.iStarObjs++;
				Stars.Insert(pObj);
				}
			}
		catch (...)
			{
			//	Crashed accessing an object

			Info.iBadObjs++;
			}
		}

	//	Make sure our cached list of stars is OK.

	for (int i = 0; i < m_Stars.GetCount(); i++)
		{
		try
			{
			CSpaceObject *pStar = m_Stars[i].pStarObj;
			if (pStar == NULL)
				Info.bBadStarCache = true;
			else if (pStar->IsDestroyed())
				Info.bBadStarCache = true;
			else if (pStar->GetSystem() != this)
				Info.bBadStarCache = true;
			else if (!Stars.Find(pStar))
				Info.bBadStarCache = true;
			}
		catch (...)
			{
			Info.bBadStarCache = true;
			}
		}

	//	Deleted object list

	Info.iDeletedObj += m_DeletedObjects.GetCount();
	}

int CSystem::GetEmptyLocationCount (void)

//	GetEmptyLocationCount
//
//	Returns the number of empty locations

	{
	TArray<int> EmptyLocations;
	m_Locations.GetEmptyLocations(&EmptyLocations);
	return EmptyLocations.GetCount();
	}

bool CSystem::GetEmptyLocations (const SLocationCriteria &Criteria, const COrbit &CenterOrbitDesc, CStationType *pStationToPlace, TProbabilityTable<int> *retTable)

//	GetEmptyLocations
//
//	Returns a list of empty locations that match the given criteria.

	{
	int i;

	retTable->DeleteAll();

	//	Check labels for overlap

	BlockOverlappingLocations();

	//	See if we need to check for distance from center

	CVector vCenter;
	bool bCheckMin = (Criteria.rMinDist != 0.0);
	bool bCheckMax = (Criteria.rMaxDist != 0.0);
	Metric rMinDist2;
	Metric rMaxDist2;
	if (bCheckMin || bCheckMax)
		{
		vCenter = CenterOrbitDesc.GetObjectPos();
		rMinDist2 = Criteria.rMinDist * Criteria.rMinDist;
		rMaxDist2 = Criteria.rMaxDist * Criteria.rMaxDist;
		}
	else
		{
		rMinDist2 = 0.0;
		rMaxDist2 = 0.0;
		}

	//	Loop over all locations and add as appropriate

	for (i = 0; i < m_Locations.GetCount(); i++)
		{
		CLocationDef &Loc = m_Locations.GetLocation(i);

		//	Skip locations that are not empty.

		if (!Loc.IsEmpty())
			continue;

		//	Compute the probability based on attributes

		int iChance = CalcLocationWeight(&Loc, Criteria.AttribCriteria);
		if (iChance == 0)
			continue;

		//	If we need to check distance, do it now

		if (bCheckMin || bCheckMax)
			{
			CVector vDist = Loc.GetOrbit().GetObjectPos() - vCenter;
			Metric rDist2 = vDist.Length2();

			if (bCheckMin && rDist2 < rMinDist2)
				continue;
			else if (bCheckMax && rDist2 > rMaxDist2)
				continue;
			}

		//	Make sure the area is clear

		if (pStationToPlace)
			{
			if (!IsExclusionZoneClear(Loc.GetOrbit().GetObjectPos(), pStationToPlace))
				continue;
			}

		//	Add to the table

		retTable->Insert(i, iChance);
		}

	//	Done

	return (retTable->GetCount() > 0);
	}

int CSystem::GetLevel (void)

//	GetLevel
//
//	Returns the level of the system

	{
	if (m_pTopology)
		return m_pTopology->GetLevel();
	else
		return 1;
	}

CSpaceObject *CSystem::GetNamedObject (const CString &sName)

//	GetNamedObject
//
//	Returns the object by name

	{
	CSpaceObject **pPoint;

	pPoint = m_NamedObjects.GetAt(sName);
	if (pPoint == NULL)
		return NULL;

	return *pPoint;
	}

CNavigationPath *CSystem::GetNavPath (CSovereign *pSovereign, CSpaceObject *pStart, CSpaceObject *pEnd)

//	GetNavPath
//
//	Returns the navigation path for the given parameters

	{
	CNavigationPath *pNext = m_NavPaths.GetNext();
	while (pNext)
		{
		if (pNext->Matches(pSovereign, pStart, pEnd))
			return pNext;

		pNext = pNext->GetNext();
		}

	//	If we cannot find an appropriate path, we create a new one

	CNavigationPath *pPath;
	CNavigationPath::Create(this, pSovereign, pStart, pEnd, &pPath);

	m_NavPaths.Insert(pPath);

	return pPath;
	}

CNavigationPath *CSystem::GetNavPathByID (DWORD dwID)

//	GetNavPathByID
//
//	Returns the nav path with the given ID (or NULL if not found)

	{
	CNavigationPath *pNext = m_NavPaths.GetNext();
	while (pNext)
		{
		if (pNext->GetID() == dwID)
			return pNext;

		pNext = pNext->GetNext();
		}

	return NULL;
	}

CSpaceObject *CSystem::GetPlayerShip (void) const

//	GetPlayerShip
//
//	Returns the player ship, if she is in the system (NULL otherwise)

	{
	CSpaceObject *pPlayer = m_Universe.GetPlayerShip();
	if (pPlayer && pPlayer->GetSystem() == this)
		return pPlayer;
	else
		return NULL;
	}

DWORD CSystem::GetSaveVersion (void)

//	GetSaveVersion
//
//	Returns the save version

	{
	return SYSTEM_SAVE_VERSION;
	}

CSpaceEnvironmentType *CSystem::GetSpaceEnvironment (int xTile, int yTile)

//	GetSpaceEnvironment
//
//	Returns the given tile

	{
	if (m_pEnvironment)
		return m_pEnvironment->GetTileType(xTile, yTile);
	else
		return NULL;
	}

CSpaceEnvironmentType *CSystem::GetSpaceEnvironment (const CVector &vPos, int *retxTile, int *retyTile)

//	GetSpaceEnvironment
//
//	Returns the tile at the given position

	{
	if (m_pEnvironment)
		{
		int x, y;

		VectorToTile(vPos, &x, &y);

		if (retxTile)
			*retxTile = x;

		if (retyTile)
			*retyTile = y;

		return m_pEnvironment->GetTileType(x, y);
		}
	else
		{
		if (retxTile)
			*retxTile = -1;

		if (retyTile)
			*retyTile = -1;

		return NULL;
		}
	}

CTopologyNode *CSystem::GetStargateDestination (const CString &sStargate, CString *retsEntryPoint)

//	GetStargateDestination
//
//	Get the destination topology node and entry point

	{
	return m_pTopology->GetGateDest(sStargate, retsEntryPoint);
	}

int CSystem::GetTileSize (void) const

//	GetTileSize
//
//	Returns the tile size in pixels

	{
	InitSpaceEnvironment();
	return m_pEnvironment->GetTileSize();
	}

bool CSystem::HasAttribute (const CVector &vPos, const CString &sAttrib)

//	HasAttribute
//
//	Returns TRUE if the system has the attribute at the given position

	{
	//	If this is the special innerSystem attribute and we're using
	//	defaults, then check the default distance

	if (m_fUseDefaultTerritories && strEquals(sAttrib, SPECIAL_ATTRIB_INNER_SYSTEM))
		return (vPos.Length() < (360 * LIGHT_SECOND));

	//	If this is the special lifeZone attribute and we're using
	//	defaults, the check the default distance

	else if (m_fUseDefaultTerritories && strEquals(sAttrib, SPECIAL_ATTRIB_LIFE_ZONE))
		{
		Metric rDist = vPos.Length();
		return (rDist >= (360 * LIGHT_SECOND) && rDist <= (620 * LIGHT_SECOND));
		}

	//	If this is the special outerSystem attribute and we're using
	//	defaults, the check the default distance

	else if (m_fUseDefaultTerritories && strEquals(sAttrib, SPECIAL_ATTRIB_OUTER_SYSTEM))
		return (vPos.Length() > (620 * LIGHT_SECOND));

	//	If this is the special nearStations attribute, then check to see
	//	if we're near any station

	else if (strEquals(sAttrib, SPECIAL_ATTRIB_NEAR_STATIONS))
		{
		for (int i = 0; i < GetObjectCount(); i++)
			{
			CSpaceObject *pObj = GetObject(i);

			if (pObj && pObj->GetScale() == scaleStructure)
				{
				CVector vDist = vPos - pObj->GetPos();
				Metric rDist = vDist.Length();

				if (rDist < 30 * LIGHT_SECOND)
					return true;
				}
			}

		return false;
		}

	//	Otherwise, check the topology and territories

	else
		{
		if (m_pTopology && m_pTopology->HasAttribute(sAttrib))
			return true;

		return m_Territories.HasAttribute(vPos, sAttrib);
		}
	}

CSpaceObject *CSystem::HitScan (CSpaceObject *pExclude, const CVector &vStart, const CVector &vEnd, bool bExcludeWorlds, CVector *retvHitPos)

//	HitScan
//
//	Looks for the first object that intersects the ray from vStart to vEnd and 
//	returns the point of intersection.
//
//	If no object is found, we return NULL.

	{
	//	Convert to a ray

	CVector vLine = vEnd - vStart;
	Metric rLineLen = vLine.Length();
	if (rLineLen == 0)
		return NULL;

	CVector vRayN = vLine / rLineLen;
	CVector vRayPixel = vRayN * g_KlicksPerPixel;
	
	//	Track the best object we've found

	CSpaceObject *pHitObj = NULL;
	CVector vHitPos;
	Metric rHitLen = rLineLen;

	//	Loop over all objects that are inside the box defined by the two line
	//	endpoints.

	SSpaceObjectGridEnumerator i;
	CVector vLL = CVector(Min(vStart.GetX(), vEnd.GetX()), Min(vStart.GetY(), vEnd.GetY()));
	CVector vUR = CVector(Max(vStart.GetX(), vEnd.GetX()), Max(vStart.GetY(), vEnd.GetY()));
	EnumObjectsInBoxStart(i, vUR, vLL, gridNoBoxCheck);

	while (EnumObjectsInBoxHasMore(i))
		{
		CSpaceObject *pObj = EnumObjectsInBoxGetNextFast(i);

		//	Skip objects we don't care about

		if (pObj->IsDestroyed()
				|| pObj == pExclude
				|| pObj->IsIntangible()
				|| (bExcludeWorlds
					&& pObj->GetScale() != scaleStructure 
					&& pObj->GetScale() != scaleShip))
			continue;

		//	Get the size of the object

		CVector vObjUR;
		CVector vObjLL;
		pObj->GetHitRect(&vObjUR, &vObjLL);

		//	Get the distance along the ray at which we intersect the given
		//	object rectangle

		Metric rObjHitFrac;
		if (!IntersectRectAndRay(vObjUR, vObjLL, vStart, vEnd, NULL, &rObjHitFrac))
			continue;

		//	If the intersection point is after our current best hit pos, then
		//	skip.

		Metric rObjHitLen = rLineLen * rObjHitFrac;
		if (rObjHitLen >= rHitLen)
			continue;

		//	Advance along the ray one pixel at a time to see where exactly we
		//	touch the object.

		SPointInObjectCtx Ctx;
		pObj->PointInObjectInit(Ctx);

		Metric rTestHitLen = rObjHitLen;
		CVector vTestPos = vStart + (rTestHitLen * vRayN);
		while (rTestHitLen < rHitLen)
			{
			if (pObj->PointInObject(Ctx, pObj->GetPos(), vTestPos))
				{
				pHitObj = pObj;
				vHitPos = vTestPos;
				rHitLen = rTestHitLen;
				break;
				}

			rTestHitLen += g_KlicksPerPixel;
			vTestPos = vTestPos + vRayPixel;
			}
		}

	//	Done

	if (pHitObj == NULL)
		return NULL;

	if (retvHitPos)
		*retvHitPos = vHitPos;

	return pHitObj;
	}

CSpaceObject *CSystem::HitTest (CSpaceObject *pExclude, const CVector &vPos, bool bExcludeWorlds)

//	HitTest
//
//	Return the object at the given position (if any).

	{
	SSpaceObjectGridEnumerator i;
	EnumObjectsInBoxStart(i, vPos, LIGHT_SECOND);

	while (EnumObjectsInBoxHasMore(i))
		{
		CSpaceObject *pObj = EnumObjectsInBoxGetNext(i);

		//	Skip objects we don't care about

		if (pObj->IsDestroyed()
				|| pObj == pExclude
				|| pObj->IsIntangible()
				|| (bExcludeWorlds
					&& pObj->GetScale() != scaleStructure 
					&& pObj->GetScale() != scaleShip))
			continue;

		//	See if the point is on this object.

		if (pObj->PointInObject(pObj->GetPos(), vPos))
			return pObj;
		}

	//	If we get this far, no objects

	return NULL;
	}

void CSystem::InitSpaceEnvironment (void) const

//	InitSpaceEnvironment
//
//	Initialize if not already

	{
	if (m_pEnvironment == NULL && m_pType)
		m_pEnvironment = new CEnvironmentGrid(m_pType->GetSpaceEnvironmentTileSize());
	}

void CSystem::InitVolumetricMask (void)

//	InitVolumetricMask
//
//	Initializes the volumetric mask for all stars

	{
	int i;

	for (i = 0; i < m_Stars.GetCount(); i++)
		{
		if (m_Stars[i].VolumetricMask.IsEmpty())
			CalcVolumetricMask(m_Stars[i].pStarObj, m_Stars[i].VolumetricMask);
		}
	}

bool CSystem::IsExclusionZoneClear (const CVector &vPos, CStationType *pType)

//	IsExclusionZoneClear
//
//	Returns TRUE if the region around vPos is clear enough to place the given 
//	station type. We consider exclusion zones as defined in the station's
//	encounter descriptor.

	{
	int i;

	//	Compute some stuff about the type

	CSovereign *pSourceSovereign = pType->GetControllingSovereign();
	CStationEncounterDesc::SExclusionDesc SourceExclusion;
	pType->GetExclusionDesc(SourceExclusion);

	//	Check against all objects in the system

	for (i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);

		//	Skip any objects that cannot attack

		if (pObj == NULL)
			continue;

		if (pObj->GetScale() != scaleStructure 
				&& (pObj->GetScale() != scaleShip || pObj->GetEncounterInfo() == NULL))
			continue;

		//	Get the exclusion zone for this object (because it may exclude more
		//	than we do). But it is OK if it doesn't have one.

		CStationEncounterDesc::SExclusionDesc Exclusion;
		CStationType *pObjType = pObj->GetEncounterInfo();
		if (pObjType)
			{
			pObjType->GetExclusionDesc(Exclusion);

			Exclusion.rAllExclusionRadius2 = Max(Exclusion.rAllExclusionRadius2, SourceExclusion.rAllExclusionRadius2);
			Exclusion.bHasAllExclusion = (Exclusion.rAllExclusionRadius2 > 0.0);

			Exclusion.rEnemyExclusionRadius2 = Max(Exclusion.rEnemyExclusionRadius2, SourceExclusion.rEnemyExclusionRadius2);
			Exclusion.bHasEnemyExclusion = (Exclusion.rEnemyExclusionRadius2 > Exclusion.rAllExclusionRadius2);
			}
		else
			Exclusion = SourceExclusion;

		//	If we have an enemy exclusion zone, and we're enemies, then check 
		//	distance. NOTE: We count stargates as enemies because enemies often
		//	come out of them.

		Metric rDist2 = 0.0;
		if (Exclusion.bHasEnemyExclusion
				&& (pObj->IsStargate()
					|| (pSourceSovereign && pObj->GetSovereign() && pObj->GetSovereign()->IsEnemy(pSourceSovereign))))
			{
			rDist2 = (vPos - pObj->GetPos()).Length2();
			if (rDist2 < Exclusion.rEnemyExclusionRadius2)
				{
#ifdef DEBUG_EXCLUSION_RADIUS
				if (m_Universe.InDebugMode())
					m_Universe.LogOutput(strPatternSubst(CONSTLIT("%s: %s too close to %s"), GetName(), pType->GetNounPhrase(), pObj->GetNounPhrase()));
#endif
				return false;
				}
			}

		//	Otherwise, if we have an exclusion to all objects, check that 
		//	distance too.

		if (Exclusion.bHasAllExclusion)
			{
			if (rDist2 == 0.0)
				rDist2 = (vPos - pObj->GetPos()).Length2();

			if (rDist2 < Exclusion.rAllExclusionRadius2)
				return false;
			}
		}

	//	If we get this far, then zone is clear

	return true;
	}

bool CSystem::IsStarAtPos (const CVector &vPos)

//	IsStarAtPos
//
//	Returns TRUE if there is a star at the given position.

	{
	int i;

	for (i = 0; i < m_Stars.GetCount(); i++)
		{
		CSpaceObject *pStar = m_Stars[i].pStarObj;
		CVector vDist = vPos - pStar->GetPos();
		if (vDist.Length2() < SAME_POS_THRESHOLD2)
			return true;
		}

	return false;
	}

bool CSystem::IsStationInSystem (CStationType *pType)

//	IsStationInSystem
//
//	Returns TRUE if the given station type has already been created in the system

	{
	int i;

	for (i = 0; i < GetObjectCount(); i++)
		{
		for (i = 0; i < GetObjectCount(); i++)
			{
			CSpaceObject *pObj = GetObject(i);
			if (pObj && pObj->GetEncounterInfo() == pType)
				return true;
			}
		}

	return false;
	}

void CSystem::MarkImages (void)

//	MarkImages
//
//	Mark images in use

	{
	DEBUG_TRY

	int i;

	m_Universe.SetLogImageLoad(false);

	//	Mark images for all objects that currently exist in the system.

	for (i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);

		if (pObj && !pObj->IsDestroyed())
			pObj->MarkImages();
		}

	//	Give all types a chance to mark images

	if (m_pEnvironment)
		m_pEnvironment->MarkImages();

	//	Mark system type images
	//	[The intro screen does not have a type.]

	if (m_pType)
		m_pType->MarkImages();
	else
		{
		//	If we have no type, then mark the default space background because
		//	we use it by default for the intro.

		TSharedPtr<CObjectImage> pDefault = m_Universe.FindLibraryImage(UNID_DEFAULT_SYSTEM_BACKGROUND);
		if (pDefault)
			pDefault->Mark();
		}

	//	Initialize the volumetric mask

	if (m_Universe.GetSFXOptions().IsStarshineEnabled())
		InitVolumetricMask();

	//	We mark some default effects, which are very commonly used (e.g., for
	//	ship explosions).

	CEffectCreator *pEffect = m_Universe.FindEffectType(g_LargeExplosionUNID);
	if (pEffect)
		pEffect->MarkImages();

	pEffect = m_Universe.FindEffectType(g_ExplosionUNID);
	if (pEffect)
		pEffect->MarkImages();

	TSharedPtr<CObjectImage> pImage = m_Universe.FindLibraryImage(g_ShipExplosionParticlesUNID);
	if (pImage)
		pImage->Mark();

	for (i = 0; i < damageCount; i++)
		{
		CEffectCreator &Effect = m_Universe.GetDefaultHitEffect((DamageTypes)i);
		Effect.MarkImages();
		}

	//	Done

	m_Universe.SetLogImageLoad(true);

	DEBUG_CATCH
	}

void CSystem::NameObject (const CString &sName, CSpaceObject &Obj)

//	NameObject
//
//	Name an object

	{
	m_NamedObjects.SetAt(sName, &Obj);
	Obj.SetNamed();
	}

CVector CSystem::OnJumpPosAdj (CSpaceObject *pObj, const CVector &vPos)

//	OnJumpPosAdj
//
//	The object wants to jump to the given coordinates. This function will call
//	other objects in the system and see if the coords need to be adjusted

	{
	if (m_pType == NULL)
		return vPos;

	//	See if the system wants to change jump coordinates

	CVector vNewPos = vPos;
	if (m_pType->FireOnObjJumpPosAdj(pObj, &vNewPos))
		return vNewPos;

	return vPos;
	}

void CSystem::OnStationDestroyed (SDestroyCtx &Ctx)

//	OnStationDestroyed
//
//	A station has been abandoned.

	{
	m_TimedEvents.OnStationDestroyed(Ctx.pObj);
	}

void CSystem::PaintDestinationMarker (SViewportPaintCtx &Ctx, CG32bitImage &Dest, int x, int y, CSpaceObject *pObj)

//	PaintDestinationMarker
//
//	Paints a directional indicator

	{
	DEBUG_TRY

	CVector vPos;

	//	Figure out the bearing for the destination object.

	int iBearing = VectorToPolar(pObj->GetPos() - (Ctx.pCenter ? Ctx.pCenter->GetPos() : NullVector));
	CG32bitPixel rgbColor = pObj->GetSymbolColor();

	//	Paint the arrow

	CPaintHelper::PaintArrow(Dest, x, y, iBearing, rgbColor);

	//	Paint the text

	const CG16bitFont &Font = m_Universe.GetNamedFont(CUniverse::fontSRSObjName);
	vPos = PolarToVector(iBearing, 5 * ENHANCED_SRS_BLOCK_SIZE);
	int xText = x - (int)vPos.GetX();
	int yText = y + (int)vPos.GetY();

	DWORD iAlign = alignCenter;
	if (iBearing <= 180)
		yText += 2 * ENHANCED_SRS_BLOCK_SIZE;
	else
		{
		yText -= (2 * ENHANCED_SRS_BLOCK_SIZE);
		iAlign |= alignBottom;
		}

	pObj->PaintHighlightText(Dest, xText, yText, Ctx, (AlignmentStyles)iAlign, rgbColor);

	DEBUG_CATCH
	}

void CSystem::PaintViewport (CG32bitImage &Dest, 
							 const RECT &rcView, 
							 CSpaceObject *pCenter, 
							 DWORD dwFlags, 
							 SViewportAnnotations *pAnnotations)

//	PaintViewport
//
//	Paints the system in the viewport

	{
	DEBUG_TRY

	ASSERT(pCenter);

	//	Initialize the viewport context

	SViewportPaintCtx Ctx;
	CalcViewportCtx(Ctx, rcView, pCenter, dwFlags);
	Dest.SetClipRect(rcView);

	//	Keep track of the player object because sometimes we do special processing

	CSpaceObject *pPlayerCenter = (pCenter->IsPlayer() ? pCenter : NULL);

	//	Compute the bounds relative to the center

	RECT rcBounds;
	rcBounds.left = rcView.left - Ctx.xCenter + (ENHANCED_SRS_BLOCK_SIZE / 2);
	rcBounds.top = rcView.top - Ctx.yCenter + (ENHANCED_SRS_BLOCK_SIZE / 2);
	rcBounds.right = rcView.right - Ctx.xCenter - (ENHANCED_SRS_BLOCK_SIZE / 2);
	rcBounds.bottom = rcView.bottom - Ctx.yCenter - (ENHANCED_SRS_BLOCK_SIZE / 2);

	//	Calculate whether we can see the objects or not

	CPerceptionCalc Perception(Ctx.iPerception);

	//	Generate lists of all objects to paint by layer

	const Metric MAX_OBJ_SIZE_PIXELS = g_KlicksPerPixel * 2048.0;
	Metric rTopEdge = Ctx.vUR.Y() + MAX_OBJ_SIZE_PIXELS;
	Metric rBottomEdge = Ctx.vLL.Y() + MAX_OBJ_SIZE_PIXELS;

	//	We create the following paint layer and paint them in this order:

	CParallaxPaintList ParallaxBackground;
	CDepthPaintList MainBackground(rTopEdge);
	CDepthPaintList MainSpace(rTopEdge);
	CDepthPaintList MainStations(rTopEdge);
	CUnorderedPaintList MainShips;
	CUnorderedPaintList MainEffects;
	CUnorderedPaintList MainOverhang;
	CParallaxPaintList ParallaxForeground;
	CMarkerPaintList EnhancedDisplay(rcBounds);

	IPaintList *MainLayer[layerCount];
	MainLayer[layerBackground] = &MainBackground;
	MainLayer[layerSpace] = &MainSpace;
	MainLayer[layerStations] = &MainStations;
	MainLayer[layerShips] = &MainShips;
	MainLayer[layerEffects] = &MainEffects;
	MainLayer[layerOverhang] = &MainOverhang;

	//	Add all objects to one of the above layers, as appropriate.

	for (int i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);
		if (pObj 
				&& !pObj->IsVirtual() 
				&& pObj != pPlayerCenter)
			{
			Metric rParallaxDist;

			if (pObj->IsOutOfPlaneObj()
					&& ((rParallaxDist = pObj->GetParallaxDist()) != 1.0))
				{
				//	Compute the size of the viewport at the given object's distance

				CVector vParallaxDiag = Ctx.vDiagonal * rParallaxDist;
				CVector vParallaxUR = Ctx.vCenterPos + vParallaxDiag;
				CVector vParallaxLL = Ctx.vCenterPos - vParallaxDiag;

				//	If we're in the viewport, then we add it

				if (pObj->InBox(vParallaxUR, vParallaxLL))
					{
					if (rParallaxDist > 1.0)
						ParallaxBackground.Insert(*pObj);
					else
						ParallaxForeground.Insert(*pObj);
					}
				}
			else
				{
				bool bInViewport = pObj->InBox(Ctx.vUR, Ctx.vLL);

				//	If we're in the viewport, then we need to paint on the main screen

				if (bInViewport)
					MainLayer[pObj->GetPaintLayer()]->Insert(*pObj);

				//	See if we need to paint a marker. Note that sometimes we end up 
				//	painting both because we might be in-bounds (because of effects)
				//	but still off-screen.

				bool bMarker = pObj->IsPlayerTarget()
						|| pObj->IsPlayerDestination()
						|| pObj->IsHighlighted()
						|| (Ctx.bEnhancedDisplay
							&& (pObj->GetScale() == scaleShip || pObj->GetScale() == scaleStructure)
							&& pObj->PosInBox(Ctx.vEnhancedUR, Ctx.vEnhancedLL)
							&& Perception.IsVisibleInLRS(Ctx.pCenter, pObj));

				if (bMarker
						&& !pObj->IsHidden()
						&& (!bInViewport || !pObj->IsPartlyVisibleInBox(Ctx.vUR, Ctx.vLL)))
					EnhancedDisplay.Insert(*pObj);
				}
			}
		}

	//	Always add the player at the end (so we paint on top of our layer)

	if (pPlayerCenter)
		MainLayer[pPlayerCenter->GetPaintLayer()]->Insert(*pPlayerCenter);

	//	Paint the background

	m_SpacePainter.PaintViewport(Dest, GetType(), Ctx);

	//	Paint background objects

	ParallaxBackground.Paint(Dest, Ctx);

	//	Paint any space environment (e.g., nebulae)

	if (m_pEnvironment)
		m_pEnvironment->Paint(Ctx, Dest);

	//	Paint all the objects by layer

	for (int i = 0; i < layerCount; i++)
		MainLayer[i]->Paint(Dest, Ctx);

	//	Paint all joints

	m_Joints.Paint(Dest, Ctx);

	//	Paint foreground objects

	ParallaxForeground.Paint(Dest, Ctx);

	//	Paint all the enhanced display markers

	EnhancedDisplay.Paint(Dest, Ctx);

	//	Let the POV paint any other enhanced displays

	pCenter->PaintSRSEnhancements(Dest, Ctx);
	if (pAnnotations)
		PaintViewportAnnotations(Dest, *pAnnotations, Ctx);

	//	Done

	Dest.ResetClipRect();

	DEBUG_CATCH
	}

void CSystem::PaintViewportAnnotations (CG32bitImage &Dest, SViewportAnnotations &Annotations, SViewportPaintCtx &Ctx)

//	PaintViewportAnnotations
//
//	Paint viewport annotations.

	{
#ifdef DEBUG_FORMATION
	if (Annotations.bDebugFormation)
		{
		int x, y;
		Ctx.XForm.Transform(Annotations.vFormationPos, &x, &y);

		CPaintHelper::PaintArrow(Dest, x, y, Annotations.iFormationAngle, CG32bitPixel(255, 255, 0));
		}
#endif
	}

void CSystem::PaintViewportGrid (CMapViewportCtx &Ctx, CG32bitImage &Dest, Metric rGridSize)

//	PaintViewportGrid
//
//	Paints a grid

	{
	const RECT &rcView = Ctx.GetViewportRect();

	int cxWidth = RectWidth(rcView);
	int cyHeight = RectHeight(rcView);

	//	Figure out where the center is

	int xCenter, yCenter;
	Ctx.Transform(Ctx.GetCenterPos(), &xCenter, &yCenter);

	//	Figure out the grid spacing

	int xSpacing, ySpacing;
	Ctx.Transform(Ctx.GetCenterPos() + CVector(rGridSize, -rGridSize), &xSpacing, &ySpacing);
	xSpacing -= xCenter;
	ySpacing -= yCenter;
	ySpacing = xSpacing;

	if (xSpacing <= 0 || ySpacing <= 0)
		return;

	//	Find the x coordinate of the left-most grid line

	int xStart;
	if (xCenter < rcView.left)
		xStart = xCenter + ((rcView.left - xCenter) / xSpacing) * xSpacing;
	else
		xStart = xCenter - ((xCenter - rcView.left) / xSpacing) * xSpacing;

	//	Paint vertical grid lines

	int x = xStart;
	while (x < rcView.right)
		{
		Dest.FillColumn(x, rcView.top, cyHeight, RGB_GRID_LINE);
		x += xSpacing;
		}

	//	Find the y coordinate of the top-most grid line

	int yStart;
	if (yCenter < rcView.top)
		yStart = yCenter + ((rcView.top - yCenter) / ySpacing) * ySpacing;
	else
		yStart = yCenter - ((yCenter - rcView.top) / ySpacing) * ySpacing;

	//	Paint horizontal grid lines

	int y = yStart;
	while (y < rcView.bottom)
		{
		Dest.FillLine(rcView.left, y, cxWidth, RGB_GRID_LINE);
		y += ySpacing;
		}
	}

void CSystem::PaintViewportObject (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pCenter, CSpaceObject *pObj)

//	PaintViewportObject
//
//	Paints a single object

	{
	//	Figure out the boundary of the viewport in system coordinates

	int xCenter = rcView.left + RectWidth(rcView) / 2;
	int yCenter = rcView.top + RectHeight(rcView) / 2;

	CVector vDiagonal(g_KlicksPerPixel * (Metric)(RectWidth(rcView) + 256) / 2,
				g_KlicksPerPixel * (Metric)(RectHeight(rcView) + 256) / 2);
	CVector vUR = pCenter->GetPos() + vDiagonal;
	CVector vLL = pCenter->GetPos() - vDiagonal;

	//	Compute the transformation to map world coordinates to the viewport

	SViewportPaintCtx Ctx;
	Ctx.pCenter = pCenter;
	Ctx.rgbSpaceColor = CalculateSpaceColor(pCenter);
	Ctx.XForm = ViewportTransform(pCenter->GetPos(), g_KlicksPerPixel, xCenter, yCenter);
	Ctx.XFormRel = Ctx.XForm;

	//	Paint object

	if (pObj && pObj->InBox(vUR, vLL))
		{
		//	Figure out the position of the object in pixels

		int x, y;
		Ctx.XForm.Transform(pObj->GetPos(), &x, &y);

		//	Paint the object in the viewport

		Ctx.pObj = pObj;
		pObj->Paint(Dest, 
				x,
				y,
				Ctx);
		}
	}

void CSystem::PaintViewportLRS (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pCenter, Metric rScale, DWORD dwFlags, bool *retbNewEnemies)

//	PaintViewportLRS
//
//	Paints an LRS from the point of view of the given object. We assume that
//	the destination bitmap is already clear; we just draw on top of it.

	{
	DEBUG_TRY

	struct SPaintEntry
		{
		CSpaceObject *pObj;
		int x;
		int y;
		};

	int i;
	Metric rKlicksPerPixel = rScale;

	//	Figure out the boundary of the viewport in system coordinates. We generate
	//	a viewport for each detection range 1-5.

	CVector vUR[CPerceptionCalc::RANGE_ARRAY_SIZE];
	CVector vLL[CPerceptionCalc::RANGE_ARRAY_SIZE];
	Metric rMaxDist2[CPerceptionCalc::RANGE_ARRAY_SIZE];

	for (i = 0; i < CPerceptionCalc::RANGE_ARRAY_SIZE; i++)
		{
		Metric rRange = CPerceptionCalc::GetRange(i);

		//	Player ship can't see beyond LRS range

		if (rRange > g_LRSRange)
			rRange = g_LRSRange;

		CVector vRange(rRange, rRange);
		vUR[i] = pCenter->GetPos() + vRange;
		vLL[i] = pCenter->GetPos() - vRange;
		rMaxDist2[i] = rRange * rRange;
		}

	int iPerception = pCenter->GetPerception();

	//	For planetary and stellar objects we use a larger box to make sure that 
	//	we include it even if it is slightly off screen.

	CVector vLargeDiagonal(rKlicksPerPixel * ((RectWidth(rcView) / 2) + 128),
				rKlicksPerPixel * ((RectHeight(rcView) / 2) + 128));
	CVector vLargeUR = pCenter->GetPos() + vLargeDiagonal;
	CVector vLargeLL = pCenter->GetPos() - vLargeDiagonal;

	//	Compute the transformation to map world coordinates to the viewport

	int xCenter = rcView.left + RectWidth(rcView) / 2;
	int yCenter = rcView.top + RectHeight(rcView) / 2;
	ViewportTransform Trans(pCenter->GetPos(), rKlicksPerPixel, xCenter, yCenter);

	//	Paint any space environment

	if (m_pEnvironment)
		{
		int x, y, x1, y1, x2, y2;

		VectorToTile(vUR[0], &x2, &y1);
		VectorToTile(vLL[0], &x1, &y2);
		
		//	Increase bounds (so we can paint the edges)

		x1--; y1--;
		x2++; y2++;

		for (x = x1; x <= x2; x++)
			for (y = y1; y <= y2; y++)
				{
				CSpaceEnvironmentType *pEnv = m_pEnvironment->GetTileType(x, y);
				if (pEnv)
					{
					int xCenter, yCenter;
					CVector vCenter = TileToVector(x, y);
					Trans.Transform(vCenter, &xCenter, &yCenter);

					pEnv->PaintLRS(Dest, xCenter, yCenter);
					}
				}
		}

	//	Loop over all objects and add them to a list for painting.
	//	(We do two passes for painting, so we need to keep the object in a 
	//	smaller list.)

	TArray<SPaintEntry> PaintList(100);

	m_fEnemiesInLRS = false;
	bool bNewEnemies = false;
	for (i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);
		if (pObj == NULL)
			continue;

		int iRange;
		if (!pObj->IsHidden()
				&& !pObj->IsVirtual()
				&& pObj->InBox(vLargeUR, vLargeLL))
			{
			if ((pObj->GetScale() == scaleStar 
					|| pObj->GetScale() == scaleWorld 
					|| ((iRange = pObj->GetDetectionRangeIndex(iPerception)) < CPerceptionCalc::RANGE_ARRAY_SIZE
						&& pCenter->GetDistance2(pObj) <= rMaxDist2[iRange])))
				{
				//	Add to the list

				SPaintEntry *pEntry = PaintList.Insert();
				pEntry->pObj = pObj;

				//	Figure out the position of the object in pixels

				Trans.Transform(pObj->GetPos(), &pEntry->x, &pEntry->y);

				//	This object is now in the LRS

				bool bNewInLRS = pObj->SetPOVLRS();

				//	If an enemy, keep track

				if (pCenter->IsEnemy(pObj)
						&& pObj->CanAttack())
					{
					if (bNewInLRS 
							&& pObj->GetCategory() == CSpaceObject::catShip)
						bNewEnemies = true;

					m_fEnemiesInLRS = true;
					}
				}

			//	NOTE: We don't clear the POVLRS flag until it leaves the large
			//	bounding box. We do this so that we don't get repeated new enemy
			//	notifications when ships are circling just outside the edge of the
			//	LRS.
			}
		else
			{
			//	This object is not in the LRS

			pObj->ClearPOVLRS();
			}
		}

	//	First we paint the background part of all objects

	for (i = 0; i < PaintList.GetCount(); i++)
		{
		PaintList[i].pObj->PaintLRSBackground(Dest, PaintList[i].x, PaintList[i].y, Trans);
		}

	//	Then we paint the foreground part

	for (i = 0; i < PaintList.GetCount(); i++)
		{
		PaintList[i].pObj->PaintLRSForeground(Dest, PaintList[i].x, PaintList[i].y, Trans);
		}

	//	If new enemies have appeared in LRS, tell the POV

	if (retbNewEnemies)
		*retbNewEnemies = bNewEnemies;

	DEBUG_CATCH
	}

void CSystem::PaintViewportMap (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pCenter, Metric rMapScale)

//	PaintViewportMap
//
//	Paints a system map

	{
	int i;
	int x, y;

	//	Initialize context

	CMapViewportCtx Ctx(pCenter, rcView, rMapScale);
	Ctx.Set3DMapEnabled(m_Universe.GetSFXOptions().Is3DSystemMapEnabled());
	Ctx.SetSpaceBackgroundEnabled(m_Universe.GetSFXOptions().IsSpaceBackgroundEnabled());

	//	In the future we should paint station images if the zoom level is greater
	//	than a certain value. NOTE: We would need to improve the current scaling
	//	algorithm, which is terrible for shrinking.

#ifdef LATER
	Ctx.SetPaintStationImagesEnabled(true);
#endif

	//	Make sure we've initialized the grid

	if (m_GridPainter.IsEmpty())
		{
		for (i = 0; i < m_Stars.GetCount(); i++)
			{
			CSpaceObject *pStar = m_Stars[i].pStarObj;
			m_GridPainter.AddRegion(pStar->GetPos(), MAP_GRID_SIZE, MAP_GRID_SIZE);
			}
		}

	//	Clear the rect

	m_SpacePainter.PaintViewportMap(Dest, rcView, GetType(), Ctx);

	//	Paint space environment

	if (m_pEnvironment)
		m_pEnvironment->PaintMap(Ctx, Dest);

	//	Paint grid

	m_GridPainter.Paint(Dest, Ctx);

	//	Paint the glow from all stars

	if (m_Universe.GetSFXOptions().IsStarGlowEnabled())
		{
		for (i = 0; i < m_Stars.GetCount(); i++)
			{
			CSpaceObject *pStar = m_Stars[i].pStarObj;

			//	Paint glow

			Ctx.Transform(pStar->GetPos(), &x, &y);
			int iGlowRadius = (int)((pStar->GetMaxLightDistance() * LIGHT_SECOND) / rMapScale);

			CGDraw::CircleGradient(Dest, x, y, iGlowRadius, pStar->GetSpaceColor());
			}
		}

	//	Paint all planets and stars first

	for (i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);

		if (pObj 
				&& (pObj->GetScale() == scaleStar || pObj->GetScale() == scaleWorld || pObj->GetCategory() == CSpaceObject::catMarker)
				&& (pObj->GetMapOrbit() || Ctx.IsInViewport(pObj)))
			{
			//	Figure out the position of the object in pixels

			Ctx.Transform(pObj->GetPos(), &x, &y);

			//	Paint the object in the viewport

			pObj->PaintMap(Ctx,
					Dest, 
					x,
					y);
			}
		}

	//	Paint all structures next

	for (i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);

		if (pObj 
				&& (pObj->GetScale() == scaleStructure
					|| pObj->GetScale() == scaleShip)
				&& Ctx.IsInViewport(pObj))
			{
			//	Figure out the position of the object in pixels

			Ctx.Transform(pObj->GetPos(), &x, &y);

			//	Paint the object in the viewport

			pObj->PaintMap(Ctx,
					Dest, 
					x,
					y);
			}
		}

	//	Paint NavPaths

	if (m_Universe.GetDebugOptions().IsShowNavPathsEnabled())
		{
		CNavigationPath *pNext = m_NavPaths.GetNext();
		while (pNext)
			{
			pNext->DebugPaintInfo(Dest, 0, 0, Ctx);

			pNext = pNext->GetNext();
			}
		}

	//	Paint the POV

	Ctx.Transform(pCenter->GetPos(), &x, &y);
	pCenter->PaintMap(Ctx, Dest, x, y);
	}

void CSystem::PaintViewportMapObject (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pCenter, CSpaceObject *pObj)

//	PaintViewportMapObject
//
//	Paints a system map object

	{
	int x, y;

	//	Initialize context

	CMapViewportCtx Ctx(pCenter, rcView, g_KlicksPerPixel);

	//	Paint the obj

	Ctx.Transform(pObj->GetPos(), &x, &y);
	pObj->PaintMap(Ctx, Dest, x, y);
	}

void CSystem::PlaceInGate (CSpaceObject *pObj, CSpaceObject *pGate)

//	PlaceInGate
//
//	Place the object at the gate

	{
	DEBUG_TRY

	ASSERT(pGate);

	CShip *pShip = pObj->AsShip();
	if (pShip == NULL)
		return;

	//	Set at gate position

	pShip->Place(pGate->GetPos(), pGate->GetVel());

	//	We keep on incrementing the timer as long as we are creating ships
	//	in the same tick. [But only if we're not creating the system.]

	if (!m_fInCreate)
		{
		if (m_iTick != g_iGateTimerTick)
			{
			g_iGateTimer = 0;
			g_iGateTimerTick = m_iTick;
			}

		pShip->SetInGate(pGate, g_iGateTimer);
		g_iGateTimer += mathRandom(11, 22);
		}
	else
		pShip->SetInGate(pGate, 0);

	DEBUG_CATCH
	}

void CSystem::PlayerEntered (CSpaceObject *pPlayer)

//	PlayerEntered
//
//	Player has entered the system

	{
	int i;

	//	Tell other objects that the player has entered

	for (i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);

		if (pObj && pObj != pPlayer)
			pObj->OnPlayerObj(pPlayer);
		}

	//	Mark the node as known

	if (m_pTopology)
		m_pTopology->SetKnown();
	}

void CSystem::GetObjRefFromID (SLoadCtx &Ctx, DWORD dwID, CSpaceObject **retpObj)

//	GetObjRefFromID
//
//	Resolves and object reference or adds a forward pointer.

	{
	//	Initialize

	*retpObj = NULL;
	if (dwID == OBJID_NULL)
		return;

	//	Lookup the ID in the map

	if (Ctx.ObjMap.Find(dwID, retpObj))
		return;

	//	If we could not find it, add the return pointer as a reference

	Ctx.ForwardReferences.InsertRef(dwID, retpObj);
	}

void CSystem::ReadObjRefFromStream (SLoadCtx &Ctx, CSpaceObject **retpObj)

//	ReadObjRefFromStream
//
//	Reads the object reference from the stream

	{
	//	Initialize

	*retpObj = NULL;

	//	Load the ID

	DWORD dwID;
	Ctx.pStream->Read((char *)&dwID, sizeof(DWORD));
	if (dwID == OBJID_NULL)
		return;

	//	Lookup the ID in the map

	if (Ctx.ObjMap.Find(dwID, retpObj))
		return;

	//	If we could not find it, add the return pointer as a reference

	Ctx.ForwardReferences.InsertRef(dwID, retpObj);
	}

void CSystem::ReadObjRefFromStream (SLoadCtx &Ctx, void *pCtx, PRESOLVEOBJIDPROC pfnResolveProc)

//	ReadObjRefFromStream
//
//	Reads the object reference from the stream

	{
	//	Load the ID

	DWORD dwID;
	Ctx.pStream->Read((char *)&dwID, sizeof(DWORD));
	if (dwID == OBJID_NULL)
		return;

	//	Lookup the ID in the map. If we find it, then resolve it now.

	CSpaceObject *pObj;
	if (Ctx.ObjMap.Find(dwID, &pObj))
		(pfnResolveProc)(pCtx, dwID, pObj);

	//	If we could not find it, add the return pointer as a reference

	else
		Ctx.ForwardReferences.InsertRef(dwID, pCtx, pfnResolveProc);
	}

void CSystem::ReadSovereignRefFromStream (SLoadCtx &Ctx, CSovereign **retpSovereign)

//	ReadSovereignRefFromStream
//
//	Reads the sovereign reference

	{
	DWORD dwUNID;
	Ctx.pStream->Read((char *)&dwUNID, sizeof(DWORD));
	if (dwUNID == 0xffffffff)
		{
		*retpSovereign = NULL;
		return;
		}

	*retpSovereign = Ctx.GetUniverse().FindSovereign(dwUNID);
	}

void CSystem::RegisterEventHandler (CSpaceObject *pObj, Metric rRange)

//	RegisterEventHandler
//
//	Register the object as a handler of system events

	{
	if (pObj == NULL || pObj->IsDestroyed())
		return;

	//	See if the object is already registered. If so, then we just
	//	take the new range and return.

	CSystemEventHandler *pNext = m_EventHandlers.GetNext();
	while (pNext)
		{
		if (pNext->GetObj() == pObj)
			{
			pNext->SetRange(rRange);
			return;
			}

		pNext = pNext->GetNext();
		}

	//	Otherwise, we add the event

	CSystemEventHandler *pNew;
	CSystemEventHandler::Create(pObj, rRange, &pNew);
	m_EventHandlers.Insert(pNew);
	}

void CSystem::RegisterForOnSystemCreated (CSpaceObject *pObj, CStationType *pEncounter, const COrbit &Orbit)

//	RegisterForOnSystemCreated
//
//	Add a new entry for a deferred call.

	{
	SDeferredOnCreateCtx *pDeferred = m_DeferredOnCreate.Insert();
	pDeferred->pObj = pObj;
	pDeferred->pEncounter = pEncounter;
	pDeferred->Orbit = Orbit;
	}

void CSystem::RemoveObject (SDestroyCtx &Ctx)

//	RemoveObject
//
//	Removes the object from the system without destroying it

	{
	DEBUG_TRY

	int i;

	//	Tell all other objects that the given object was destroyed
	//	NOTE: The destroyed flag is already set on the object

	ASSERT(Ctx.pObj->IsDestroyed());
	if (Ctx.pObj->NotifyOthersWhenDestroyed())
		{
		DEBUG_SAVE_PROGRAMSTATE;

		//	Notify subscribers

		Ctx.pObj->NotifyOnObjDestroyed(Ctx);

		//	Notify other objects in the system

		for (i = 0; i < GetObjectCount(); i++)
			{
			CSpaceObject *pObj = GetObject(i);

			if (pObj && pObj != Ctx.pObj)
				{
				SetProgramState(psOnObjDestroyed, pObj);

				pObj->OnObjDestroyed(Ctx);
				}
			}

		//	System-level notifications

		CSpaceObject::Categories iObjCat = Ctx.pObj->GetCategory();
		if (iObjCat == CSpaceObject::catShip || iObjCat == CSpaceObject::catStation)
			{
			FireOnSystemObjDestroyed(Ctx);
			m_Universe.NotifyOnObjDestroyed(Ctx);
			}

		DEBUG_RESTORE_PROGRAMSTATE;
		}

	//	Deal with event handlers

	m_TimedEvents.OnObjDestroyed(Ctx.pObj);
	if (m_EventHandlers.ObjDestroyed(Ctx.pObj))
		m_fFlushEventHandlers = true;

	//	If this is the player and we're resurrecting, then remove the player from
	//	the grid. We need to do this because resurrecting player ships don't have
	//	the destroyed flag set, but we don't want them to show up in future 
	//	searches.

	if (Ctx.pObj->IsPlayer() && Ctx.bResurrectPending)
		{
		m_ObjGrid.Delete(Ctx.pObj);
		}

	//	Deal with joints

	m_Joints.ObjDestroyed(Ctx.pObj);

	//	Check to see if this was the POV

	if (Ctx.pObj == m_Universe.GetPOV())
		{
		//	If this was not the player, then set back to the player

		CSpaceObject *pPlayer = GetPlayerShip();
		if (Ctx.pObj != pPlayer && pPlayer && pPlayer->CanBePOV())
			m_Universe.SetPOV(pPlayer);

		//	Otherwise, set to a marker

		else
			{
			CPOVMarker *pMarker;
			CPOVMarker::Create(*this, Ctx.pObj->GetPos(), NullVector, &pMarker);
			m_Universe.SetPOV(pMarker);
			}
		}

	m_AllObjects[Ctx.pObj->GetIndex()] = NULL;

	//	Invalidate cache of enemy objects

	FlushEnemyObjectCache();

	//	Remove from named object list

	if (Ctx.pObj->IsNamed())
		UnnameObject(*Ctx.pObj);

	//	Invalidate encounter table cache

	if (Ctx.pObj->HasRandomEncounters())
		m_fEncounterTableValid = false;

	//	If this was a star then recalc the list of stars

	if (Ctx.pObj->GetScale() == scaleStar)
		{
		ComputeStars();
		if (m_Universe.GetSFXOptions().IsStarshineEnabled())
			InitVolumetricMask();
		}

	//	If this object has a volumetric shadow, then we need to remove it.

	if (Ctx.pObj->HasVolumetricShadow()
			&& m_Universe.GetSFXOptions().IsStarshineEnabled())
		RemoveVolumetricShadow(Ctx.pObj);

	//	Debug code to see if we ever delete a barrier in the middle of move

#ifdef DEBUG_PROGRAMSTATE
	if (g_iProgramState == psUpdatingMove)
		{
		if (Ctx.pObj->IsBarrier())
			{
			CString sObj = CONSTLIT("ERROR: Destroying barrier during move.\r\n");

			ReportCrashObj(&sObj, Ctx.pObj);
			kernelDebugLogString(sObj);

#ifdef DEBUG
			DebugBreak();
#endif
			}
		}
#endif

	DEBUG_CATCH
	}

void CSystem::RemoveVolumetricShadow (CSpaceObject *pObj)

//	RemoveVolumetricShadow
//
//	Removes the object's volumetric shadow from the mask. We call this when we
//	delete an object at runtime.

	{
	DEBUG_TRY

	int i, j;

	//	Loop over all stars

	for (i = 0; i < m_Stars.GetCount(); i++)
		{
		CSpaceObject *pStar = m_Stars[i].pStarObj;
		int xStar = (int)(pStar->GetPos().GetX() / g_KlicksPerPixel);
		int yStar = (int)(pStar->GetPos().GetY() / g_KlicksPerPixel);
		Metric rMaxDist = (pStar->GetMaxLightDistance() + 100) * LIGHT_SECOND;

		//	Compute the angle of the object with respect to the star
		//	And skip any objects that are outside the star's light radius.

		Metric rStarDist;
		int iStarAngle = ::VectorToPolar(pObj->GetPos() - pStar->GetPos(), &rStarDist);
		if (rStarDist > rMaxDist)
			continue;

		//	Figure out the rect that encloses the shadow for this object (relative to
		//	the mask for the star).

		CVolumetricShadowPainter ObjShadowPainter(pStar, xStar, yStar, iStarAngle, rStarDist, pObj, m_Stars[i].VolumetricMask);
		RECT rcRect;
		ObjShadowPainter.GetShadowRect(&rcRect);

		//	Erase the rect

		m_Stars[i].VolumetricMask.Fill(rcRect.left, rcRect.top, RectWidth(rcRect), RectHeight(rcRect), 0xff);

		//	Mask out everything outside the rect so we can redraw the shadows just
		//	inside the rect.

		m_Stars[i].VolumetricMask.SetClipRect(rcRect);

		//	Draw all remaining objects that intersect with the rect.

		for (j = 0; j < GetObjectCount(); j++)
			{
			CSpaceObject *pPrevObj = GetObject(j);
			if (pPrevObj == NULL
					|| pPrevObj->IsDestroyed()
					|| !pPrevObj->HasVolumetricShadow())
				continue;

			//	Compute the largest possible shadow we could have (this is a rough
			//	heuristic so we don't have to compute the full shadow shape).

			Metric rMaxShadow = g_KlicksPerPixel * ((10 * RectWidth(pPrevObj->GetImage().GetImageRect())) + RectWidth(rcRect));
			Metric rMaxShadow2 = rMaxShadow * rMaxShadow;

			//	If any object is further than this from us, then we can safely ignore it.

			if ((pPrevObj->GetPos() - pObj->GetPos()).Length2() > rMaxShadow2)
				continue;

			//	Compute the angle of the object with respect to the star
			//	And skip any objects that are outside the star's light radius.

			Metric rStarDist;
			int iStarAngle = ::VectorToPolar(pPrevObj->GetPos() - pStar->GetPos(), &rStarDist);
			if (rStarDist > rMaxDist)
				continue;

			//	Compute the shadow rect

			CVolumetricShadowPainter Painter(pStar, xStar, yStar, iStarAngle, rStarDist, pPrevObj, m_Stars[i].VolumetricMask);
			RECT rcPrevShadow;
			Painter.GetShadowRect(&rcPrevShadow);

			//	If this rect overlaps with the rect that we erased, then we need 
			//	to redraw the shadow

			if (::RectsIntersect(rcPrevShadow, rcRect))
				Painter.PaintShadow();
			}

		//	Done

		m_Stars[i].VolumetricMask.ResetClipRect();
		}

	DEBUG_CATCH
	}

void CSystem::RestartTime (void)

//	RestartTime
//
//	Restart time for all

	{
	DEBUG_TRY

	for (int i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);

		if (pObj)
			pObj->RestartTime();
		}

	m_iTimeStopped = 0;

	DEBUG_CATCH
	}

ALERROR CSystem::SaveToStream (IWriteStream *pStream)

//	SaveToStream
//
//	Save the system to a stream
//
//	DWORD		m_dwID
//	DWORD		m_iTick
//	DWORD		m_iTimeStopped
//	CString		m_sName
//	CString		Topology node ID
//	DWORD		(unused)
//	DWORD		m_iNextEncounter
//	DWORD		flags
//	DWORD		SAVE VERSION (only if [flags & 0x02])
//	Metric		m_rKlicksPerPixel
//	Metric		m_rTimeScale
//	DWORD		m_iLastUpdated
//
//	DWORD		Number of CNavigationPath
//	CNavigationPath
//
//	CEventHandlers
//
//	DWORD		Number of mission objects
//	CSpaceObject
//
//	DWORD		Number of objects
//	CSpaceObject
//
//	DWORD		Number of named objects
//	CString		entrypoint: name
//	DWORD		entrypoint: CSpaceObject ref
//
//	DWORD		Number of timed events
//	CSystemEvent
//
//	DWORD		Number of environment maps
//	CTileMap
//
//	CLocationList	m_Locations
//	CTerritoryList	m_Territories
//	CObjectJointList	m_Joints

	{
	int i;
	DWORD dwSave;

	//	Write basic data

	pStream->Write((char *)&m_dwID, sizeof(DWORD));
	pStream->Write((char *)&m_iTick, sizeof(DWORD));
	pStream->Write((char *)&m_iTimeStopped, sizeof(DWORD));
	m_sName.WriteToStream(pStream);
	m_pTopology->GetID().WriteToStream(pStream);
	dwSave = 0;
	pStream->Write((char *)&dwSave, sizeof(DWORD));
	pStream->Write((char *)&m_iNextEncounter, sizeof(DWORD));

	//	Write flags

	dwSave = 0;
	dwSave |= (m_fNoRandomEncounters ?		0x00000001 : 0);
	dwSave |= 0x00000002;	//	Include version (this is a hack for backwards compatibility)
	dwSave |= (!m_fUseDefaultTerritories ?	0x00000004 : 0);
	dwSave |= (m_fEnemiesInLRS ?			0x00000008 : 0);
	dwSave |= (m_fEnemiesInSRS ?			0x00000010 : 0);
	dwSave |= (m_fPlayerUnderAttack ?		0x00000020 : 0);
	dwSave |= (m_fLocationsBlocked ?		0x00000040 : 0);
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	//	Save version

	dwSave = SYSTEM_SAVE_VERSION;
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	//	Scale

	pStream->Write((char *)&m_rKlicksPerPixel, sizeof(Metric));
	pStream->Write((char *)&m_rTimeScale, sizeof(Metric));
	pStream->Write((char *)&m_iLastUpdated, sizeof(DWORD));

	//	Save navigation paths

	m_NavPaths.WriteToStream(this, pStream);

	//	Save event handlers

	m_EventHandlers.FlushDeletedObjs();
	m_EventHandlers.WriteToStream(this, pStream);

	//	Save all objects in the system

	DWORD dwCount = 0;
	for (i = 0; i < GetObjectCount(); i++)
		if (GetObject(i))
			dwCount++;

	pStream->Write((char *)&dwCount, sizeof(DWORD));
	for (i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);

		if (pObj)
			{
			try
				{
				pObj->WriteToStream(pStream);
				}
			catch (...)
				{
				CString sError = CONSTLIT("Unable to save object:\r\n");
				ReportCrashObj(&sError, pObj);
				kernelDebugLogString(sError);
				return ERR_FAIL;
				}
			}
		}

	//	Save all named objects

	dwCount = m_NamedObjects.GetCount();
	pStream->Write((char *)&dwCount, sizeof(DWORD));

	for (i = 0; i < (int)dwCount; i++)
		{
		CSpaceObject *pObj = m_NamedObjects[i];
		m_NamedObjects.GetKey(i).WriteToStream(pStream);
		WriteObjRefToStream(pObj, pStream);
		}

	//	Save timed events

	m_TimedEvents.WriteToStream(this, pStream);

	//	Save environment maps

	dwCount = (m_pEnvironment ? 1 : 0);
	pStream->Write((char *)&dwCount, sizeof(DWORD));
	if (m_pEnvironment)
		m_pEnvironment->WriteToStream(pStream);

	//	Locations & Territories

	m_Locations.WriteToStream(pStream);
	m_Territories.WriteToStream(pStream);

	//	Joints

	m_Joints.WriteToStream(this, *pStream);

	return NOERROR;
	}

void CSystem::SetLastUpdated (void)

//	SetLastUpdated
//
//	Marks this time as the last time the system was updated.
//	We use this to figure out how much time passed since we last updated

	{
	m_iLastUpdated = m_Universe.GetTicks();
	}

void CSystem::SetPainted (void)

//	SetPainted
//
//	Set all objects as painted.

	{
	int i;

	for (i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);
		if (pObj)
			pObj->SetPainted();
		}
	}

void CSystem::SetPOVLRS (CSpaceObject *pCenter)

//	SetPOVLRS
//
//	Sets the POVLRS flag for all objects in the viewport

	{
	//	Figure out the boundary of the viewport in system coordinates

	CVector vDiagonal(g_LRSRange, g_LRSRange);
	CVector vUR = pCenter->GetPos() + vDiagonal;
	CVector vLL = pCenter->GetPos() - vDiagonal;

	//	Loop over all objects

	for (int i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);

		if (pObj)
			{
			if (pObj->InBox(vUR, vLL))
				pObj->SetPOVLRS();
			else
				pObj->ClearPOVLRS();
			}
		}
	}

void CSystem::SetSpaceEnvironment (int xTile, int yTile, CSpaceEnvironmentType *pEnvironment)

//	SetSpaceEnvironment
//
//	Sets the space environment

	{
	InitSpaceEnvironment();
	m_pEnvironment->SetTileType(xTile, yTile, pEnvironment);
	}

ALERROR CSystem::StargateCreated (CSpaceObject *pGate, const CString &sStargateID, const CString &sDestNodeID, const CString &sDestEntryPoint)

//	StargateCreated
//
//	Called whenever a stargate object is created in the system. This code will
//	fix up the topology nodes as appropriate.

	{
	ASSERT(pGate);
	if (pGate == NULL)
		return ERR_FAIL;

	//	Get the ID of the stargate

	CString sGateID;
	if (sStargateID.IsBlank())
		sGateID = strPatternSubst(CONSTLIT("ID%x"), pGate->GetID());
	else
		sGateID = sStargateID;

	//	Look for the stargate in the topology; if we don't find it, then we need to add it

	if (!m_pTopology->FindStargate(sGateID))
		{
		CTopologyNode::SStargateDesc GateDesc;
		GateDesc.sName = sGateID;
		GateDesc.sDestNode = sDestNodeID;
		GateDesc.sDestName = sDestEntryPoint;

		m_pTopology->AddStargateAndReturn(GateDesc);
		}

	//	Add this as a named object (so we can come back here)

	if (GetNamedObject(sGateID) == NULL)
		NameObject(sGateID, *pGate);

	return NOERROR;
	}

void CSystem::StopTime (const CSpaceObjectList &Targets, int iDuration)

//	StopTime
//
//	Stops time for all targets

	{
	if (IsTimeStopped() || iDuration == 0)
		return;

	for (int i = 0; i < Targets.GetCount(); i++)
		{
		CSpaceObject *pObj = Targets.GetObj(i);

		if (pObj && !pObj->IsImmuneTo(CConditionSet::cndTimeStopped))
			pObj->StopTime();
		}

	m_iTimeStopped = iDuration;
	}

void CSystem::StopTimeForAll (int iDuration, CSpaceObject *pExcept)

//	StopTimeForAll
//
//	Stop time for all objects in the system (except exception)

	{
	if (IsTimeStopped() || iDuration == 0)
		return;

	for (int i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);

		if (pObj && pObj != pExcept && !pObj->IsImmuneTo(CConditionSet::cndTimeStopped))
			pObj->StopTime();
		}

	m_iTimeStopped = iDuration;
	}

CVector CSystem::TileToVector (int x, int y) const

//	TileToVector
//
//	Convert space environment coordinates

	{
	InitSpaceEnvironment();
	return m_pEnvironment->TileToVector(x, y);
	}

void CSystem::TransferObjEventsIn (CSpaceObject *pObj, CSystemEventList &ObjEvents)

//	TransferObjEventsIn
//
//	Moves all of the timed events in ObjEvents to the system

	{
	int i;

	for (i = 0; i < ObjEvents.GetCount(); i++)
		{
		CSystemEvent *pEvent = ObjEvents.GetEvent(i);

		//	Set tick to be relative to new system

		pEvent->SetTick(m_iTick + pEvent->GetTick());

		//	Move

		ObjEvents.MoveEvent(i, m_TimedEvents);
		i--;
		}
	}

void CSystem::TransferObjEventsOut (CSpaceObject *pObj, CSystemEventList &ObjEvents)

//	TransferObjEventsOut
//
//	Moves any timed events for the given object out of the system and into
//	ObjEvents.

	{
	int i;

	for (i = 0; i < GetTimedEventCount(); i++)
		{
		CSystemEvent *pEvent = GetTimedEvent(i);
		if (pEvent->OnObjChangedSystems(pObj))
			{
			//	Set the tick to an offset from system time

			pEvent->SetTick(pEvent->GetTick() - m_iTick);

			//	Move to list

			m_TimedEvents.MoveEvent(i, ObjEvents);
			i--;
			}
		}
	}

void CSystem::UnnameObject (CSpaceObject &Obj)

//	UnnameObject
//
//	Remove the name for the object

	{
	for (int i = 0; i < m_NamedObjects.GetCount(); i++)
		if (m_NamedObjects[i] == &Obj)
			m_NamedObjects.Delete(i);

	Obj.SetNamed(false);
	}

void CSystem::UnregisterEventHandler (CSpaceObject *pObj)

//	UnregisterEventHandler
//
//	Removes the handler

	{
	CSystemEventHandler *pNext = m_EventHandlers.GetNext();
	while (pNext)
		{
		if (pNext->OnUnregister(pObj))
			{
			m_fFlushEventHandlers = true;

			//	NOTE: We can guarantee that this object is only in the list once
			//	because we check at register time.

			return;
			}

		pNext = pNext->GetNext();
		}
	}

void CSystem::Update (SSystemUpdateCtx &SystemCtx, SViewportAnnotations *pAnnotations)

//	Update
//
//	Updates the system

	{
	DEBUG_TRY

	int i;
#ifdef DEBUG_PERFORMANCE
	int iUpdateObj = 0;
	int iMoveObj = 0;
#endif

	//	Delete all objects in the deleted list (we do this at the
	//	beginning because we want to keep the list after the update
	//	so that callers can examine it).

	FlushDeletedObjects();

	//	Set up context

	SUpdateCtx Ctx;
	Ctx.pSystem = this;
	Ctx.pPlayer = GetPlayerShip();
	Ctx.pAnnotations = pAnnotations;

	//	Initialize the player weapon context so that we can select the auto-
	//	target.

	CalcAutoTarget(Ctx);

	//	Add all objects to the grid so that we can do faster
	//	hit tests

	m_ObjGrid.Init(this, Ctx);

	//	Fire timed events
	//	NOTE: We only do this if we have a player because otherwise, some
	//	of the scripts might crash. We won't have a player when we first
	//	create the universe.

	SetProgramState(psUpdatingEvents);
	if (!IsTimeStopped() && (m_Universe.GetPlayerShip() || SystemCtx.bForceEventFiring))
		m_TimedEvents.Update(m_iTick, *this);

	//	If necessary, mark as painted so that objects update correctly.

	if (SystemCtx.bForcePainted)
		SetPainted();

	//	Give all objects a chance to react

	m_fPlayerUnderAttack = false;
	DebugStartTimer();
	for (i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);
		if (pObj == NULL)
			continue;

		//	Initialize context

		Ctx.SetTimeStopped(pObj->IsTimeStopped());

		//	Update behavior first.

		if (!Ctx.IsTimeStopped())
			{
			SetProgramState(psUpdatingBehavior, pObj);
			pObj->Behavior(Ctx);
			}

		//	Now update. We do this even if we're time-stopped because we might
		//	need to update the overlay that stops time.

		SetProgramState(psUpdatingObj, pObj);
		pObj->Update(Ctx);

		//	Debug

#ifdef DEBUG_PERFORMANCE
		iUpdateObj++;
#endif
		}
	DebugStopTimer("Updating objects");

	//	Initialize a structure that holds context for motion

	DebugStartTimer();

	//	Start physics

	m_ContactResolver.BeginUpdate();

	//	Do a pass before move to update gravity and other things.

	for (i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);
		if (pObj && !pObj->IsDestroyed())
			{
			if (pObj->HasGravity())
				UpdateGravity(Ctx, pObj);

			//	If necessary, keep track of objects that belong to the player

			CSovereign *pSovereign;
			if (SystemCtx.bTrackPlayerObjs
					&& (pSovereign = pObj->GetSovereign())
					&& pSovereign->GetUNID() == g_PlayerSovereignUNID
					&& (pObj->GetCategory() == CSpaceObject::catShip
							|| pObj->GetCategory() == CSpaceObject::catStation))
				Ctx.PlayerObjs.Insert(pObj);
			}
		}

	//	Move all objects. Note: We always move last because we want to
	//	paint right after a move. Otherwise, when a laser/missile hits
	//	an object, the laser/missile is deleted (in update) before it
	//	gets a chance to paint.

	for (i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);

		if (pObj 
                && !pObj->IsDestroyed()
                && pObj->CanMove()
				&& !pObj->IsSuspended()
                && !pObj->IsTimeStopped())
			{
			//	Move the objects

			SetProgramState(psUpdatingMove, pObj);
			pObj->Move(Ctx, SystemCtx.rSecondsPerTick);

#ifdef DEBUG_PERFORMANCE
			iMoveObj++;
#endif
			}
		}
	DebugStopTimer("Moving objects");

	//	Update collisions. This function will iterate over each object that 
	//	needs collision testing and add a CPhysicsContact for each unique pair
	//	of collisions.

	UpdateCollisionTesting(Ctx);

	//	Add contacts from joints

	m_Joints.AddContacts(m_ContactResolver);

	//	Now resolve all contacts

	m_ContactResolver.Update();
	m_Joints.Update(Ctx);

	//	Update random encounters

	SetProgramState(psUpdatingEncounters);
	if (m_iTick >= m_iNextEncounter
			&& !IsTimeStopped())
		UpdateRandomEncounters();

	//	Update time stopped

	SetProgramState(psUpdating);
	if (IsTimeStopped())
		if (m_iTimeStopped > 0 && --m_iTimeStopped == 0)
			RestartTime();

	//	Update the player controller

	IPlayerController *pPlayerController = m_Universe.GetPlayer();
	if (pPlayerController)
		pPlayerController->Update(Ctx);

	//	Give the player ship a chance to do something with data that we've
	//	accumulated during update. For example, we use this to set the nearest
	//	docking port.

	CSpaceObject *pPlayer = GetPlayerShip();
	if (pPlayer && !pPlayer->IsDestroyed())
		pPlayer->UpdatePlayer(Ctx);

	//	Perf output

#ifdef DEBUG_PERFORMANCE
	{
	char szBuffer[1024];
	wsprintf(szBuffer, "Objects: %d  Updating: %d  Moving: %d\n", 
			GetObjectCount(), 
			iUpdateObj, 
			iMoveObj);
	::OutputDebugString(szBuffer);
	}
#endif

	//	Next

	m_iTick++;

	DEBUG_CATCH
	}

void CSystem::UpdateCollisionTesting (SUpdateCtx &Ctx)

//	UpdateCollisionTesting
//
//	Loops over all objects that need collision testing and adds physics 
//	contacts for every unique pair of collisions detected.

	{
	int i;

	for (i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);
		if (pObj == NULL || !pObj->IsCollisionTestNeeded())
			continue;

		//	Reset the collision flag so we don't consider this object again.

		pObj->SetCollisionTestNeeded(false);
		bool bBlocked = false;

		//	Loop over all objects in range.

		SSpaceObjectGridEnumerator j;
		EnumObjectsInBoxStart(j, pObj->GetPos(), g_SecondsPerUpdate * LIGHT_SECOND);
		while (EnumObjectsInBoxHasMore(j))
			{
			CSpaceObject *pContactObj = EnumObjectsInBoxGetNext(j);

			//	If this contact object is a moving object and if we've already
			//	handled it, then we don't need anything further. This can happen,
			//	for example, with two wrecks running into each other.

			if (pContactObj->CanMove() 
					&& !pContactObj->IsCollisionTestNeeded())
				continue;

			//	If the contact object cannot block us, then continue.

			if (!pContactObj->Blocks(pObj))
				continue;

			//	See if we collided. For objects against immobile barriers we do a
			//	point test because we might get stuck due to rotation changes.

			if (pContactObj->IsAnchored())
				{
				if (!pContactObj->PointInObject(pContactObj->GetPos(), pObj->GetPos()))
					continue;
				}

			//	See if we intersect. If we do, then we add a collision.

			else if (!pObj->ObjectInObject(pObj->GetPos(), pContactObj, pContactObj->GetPos()))
				continue;

			//	At this point we know that we've hit pContactObj.

			bBlocked = true;

			//	If we were born inside a barrier, then we remember that we're 
			//	still blocked, but we continue without bouncing until we're out.

			if (pObj->IsInsideBarrier())
				continue;

			//	Add a collision contact

			m_ContactResolver.AddCollision(pObj, pContactObj);
			}

		//	If we were inside a barrier but we're no longer blocked, then clear
		//	the flag.

		if (pObj->IsInsideBarrier() && !bBlocked)
			pObj->SetInsideBarrier(false);
		}
	}

void CSystem::UpdateExtended (const CTimeSpan &ExtraTime)

//	UpdateExtended
//
//	Updates the system for many ticks

	{
	int i;

	SSystemUpdateCtx UpdateCtx;

	//	Update for a few seconds

	int iTime = mathRandom(250, 350);
	for (i = 0; i < iTime; i++)
		Update(UpdateCtx);

	//	Give all objects a chance to update

	for (i = 0; i < GetObjectCount(); i++)
		{
		CSpaceObject *pObj = GetObject(i);

		if (pObj)
			{
			SetProgramState(psUpdatingExtended, pObj);
			pObj->UpdateExtended(ExtraTime);
			}
		}

	SetProgramState(psUpdating);
	}

void CSystem::UpdateGravity (SUpdateCtx &Ctx, CSpaceObject *pGravityObj)

//	UpdateGravity
//
//	Accelerates objects around high-gravity fields

	{
	int i;

	//	Compute the acceleration due to gravity at the scale radius
	//	(in kilometers per second-squared).

	Metric rScaleRadius;
	Metric r1EAccel = pGravityObj->GetGravity(&rScaleRadius);
	if (r1EAccel <= 0.0)
		return;

	Metric rScaleRadius2 = rScaleRadius * rScaleRadius;

	//	Compute the radius at which we get ripped apart

	Metric rTidalKillDist2 = r1EAccel * rScaleRadius2 / TIDAL_KILL_THRESHOLD;

	//	We don't care about accelerations less than 1 km/sec^2.

	const Metric MIN_ACCEL = 1.0;

	//	Compute the radius at which the acceleration is the minimum that we 
	//	care about.
	//
	//	minA = A/r^2
	//	r = sqrt(A/minA) * Earth-radius

	Metric rMaxDist = sqrt(r1EAccel / MIN_ACCEL) * rScaleRadius;
	Metric rMaxDist2 = rMaxDist * rMaxDist;

	//	Loop over all objects inside the given distance and accelerate them.

	CSpaceObjectList Objs;
	GetObjectsInBox(pGravityObj->GetPos(), rMaxDist, Objs);

	for (i = 0; i < Objs.GetCount(); i++)
		{
		//	Skip objects not affected by gravity

		CSpaceObject *pObj = Objs.GetObj(i);
		if (pObj == pGravityObj 
				|| pObj->IsDestroyed()
				|| pObj->IsAnchored())
			continue;

		//	Skip objects outside the maximum range

		CVector vDist = (pGravityObj->GetPos() - pObj->GetPos());
		Metric rDist2 = pGravityObj->GetDistance2(pObj);
		if (rDist2 > rMaxDist2)
			continue;

		//	Inside the kill radius, we destroy the object

		if (rDist2 < rTidalKillDist2)
			{
			if (pObj->OnDestroyCheck(killedByGravity, pGravityObj))
				pObj->Destroy(killedByGravity, pGravityObj);

			continue;
			}

		//	Compute acceleration

		Metric rAccel = r1EAccel * rScaleRadius2 / rDist2;

		//	Accelerate towards the center

		pObj->DeltaV(g_SecondsPerUpdate * rAccel * vDist / sqrt(rDist2));
		pObj->ClipSpeed(LIGHT_SPEED);

		//	If this is the player, then gravity warning

		if (pObj == Ctx.pPlayer && rAccel > GRAVITY_WARNING_THRESHOLD)
			Ctx.bGravityWarning = true;
		}
	}

void CSystem::UpdateRandomEncounters (void)

//	UpdateRandomEncounters
//
//	Updates random encounters

	{
	struct SEncounter
		{
		int iWeight;
		CSpaceObject *pObj;
		IShipGenerator *pTable;
		};

	int i;

	if (m_fNoRandomEncounters)
		return;

	//	No need for random encounters if the player isn't in the system

	CSpaceObject *pPlayer = GetPlayerShip();
	if (pPlayer == NULL || pPlayer->IsDestroyed())
		return;

	IShipGenerator *pTable = NULL;
	CSpaceObject *pBase = NULL;
	CDesignType *pType = NULL;
	CSovereign *pBaseSovereign = NULL;

	//	Some percent of the time we generate a generic level encounter; the rest of the
	//	time, the encounter is based on the stations in this system.

	if (mathRandom(1, 100) <= LEVEL_ENCOUNTER_CHANCE)
		m_Universe.GetRandomLevelEncounter(GetLevel(), &pType, &pTable, &pBaseSovereign);
	else
		{
		//	Compute the list of all objects that have encounters (and cache it)

		ComputeRandomEncounters();

		//	Allocate and fill-in the table

		if (m_EncounterObjs.GetCount() > 0)
			{
			SEncounter *pMainTable = new SEncounter [m_EncounterObjs.GetCount()];
			int iCount = 0;
			int iTotal = 0;
			for (i = 0; i < m_EncounterObjs.GetCount(); i++)
				{
				CSpaceObject *pObj = m_EncounterObjs.GetObj(i);

				//	Get frequency and (optionally) table

				int iFreq;
				IShipGenerator *pTable = pObj->GetRandomEncounterTable(&iFreq);

				//	Adjust frequency to account for the player's distance from the object

				Metric rDist = Max(LIGHT_MINUTE, pPlayer->GetDistance(pObj));
				Metric rDistAdj = (rDist <= MAX_ENCOUNTER_DIST ? LIGHT_MINUTE / rDist : 0.0);
				iFreq = (int)(iFreq * 10.0 * rDistAdj);

				//	Add to table

				if (iFreq > 0)
					{
					pMainTable[iCount].iWeight = iFreq;
					pMainTable[iCount].pObj = pObj;
					pMainTable[iCount].pTable = pTable;

					iTotal += iFreq;
					iCount++;
					}
				}

			//	Pick a random entry in the table

			if (iTotal > 0)
				{
				int iRoll = mathRandom(0, iTotal - 1);
				int iPos = 0;

				//	Get the position

				while (pMainTable[iPos].iWeight <= iRoll)
					iRoll -= pMainTable[iPos++].iWeight;

				//	Done

				pTable = pMainTable[iPos].pTable;
				pBase = pMainTable[iPos].pObj;
				if (pBase)
					pType = pBase->GetType();
				}

			delete [] pMainTable;
			}
		}

	//	If we've got a table, then create the random encounter

	if (pTable)
		CreateRandomEncounter(pTable, pBase, pBaseSovereign, pPlayer);

	//	Otherwise, fire the OnRandomEncounter event

	else if (pType)
		pType->FireOnRandomEncounter(pBase);

	//	Next encounter

	m_iNextEncounter = m_iTick + mathRandom(6000, 9000);
	}

void CSystem::VectorToTile (const CVector &vPos, int *retx, int *rety) const

//	VectorToTile
//
//	Convert space environment coordinates

	{
	InitSpaceEnvironment();
	m_pEnvironment->VectorToTile(vPos, retx, rety);
	}

void CSystem::WriteObjRefToStream (CSpaceObject *pObj, IWriteStream *pStream, CSpaceObject *pReferrer)

//	WriteObjRefToStream
//
//	DWORD		0xffffffff if NULL
//				Otherwise, index of object in system

	{
	DWORD dwSave = OBJID_NULL;
	if (pObj)
		{
		dwSave = pObj->GetID();
		ASSERT(dwSave != 0xDDDDDDDD);

		//	Make sure we save references to objects in the current system.
		//	This will help to track a bug in gating objects.
		//
		//	If the object has no system then it means that it is not tied to
		//	a particular system.

		CSystem *pObjSystem = pObj->GetSystem();
		if (pObjSystem && pObjSystem != this)
			{
			kernelDebugLogPattern("Save file error: Saving reference to object in another system");

			CString sError;
			ReportCrashObj(&sError, pObj);
			kernelDebugLogPattern("Object being referenced:");
			kernelDebugLogString(sError);

			if (pReferrer)
				{
				ReportCrashObj(&sError, pReferrer);
				kernelDebugLogPattern("Referring object:");
				kernelDebugLogString(sError);
				}

			dwSave = OBJID_NULL;
			}
		}

	pStream->Write((char *)&dwSave, sizeof(DWORD));
	}

void CSystem::WriteSovereignRefToStream (CSovereign *pSovereign, IWriteStream *pStream)

//	WriteSovereignRefToStream
//
//	DWORD		0xffffffff if NULL
//				Otherwise, UNID

	{
	DWORD dwSave = 0xffffffff;
	if (pSovereign)
		dwSave = pSovereign->GetUNID();

	pStream->Write((char *)&dwSave, sizeof(DWORD));
	}
