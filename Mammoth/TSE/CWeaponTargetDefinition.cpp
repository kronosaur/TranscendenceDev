#include "PreComp.h"

bool CWeaponTargetDefinition::MatchesTarget(CSpaceObject* pSource, CSpaceObject* pTarget) const

//	MatchesTarget
//
//	Returns True if the target matches the criteria
	{
	CSpaceObjectCriteria::SCtx Ctx(pSource, m_TargetCriteria);
	return pTarget ? pTarget->MatchesCriteria(Ctx, m_TargetCriteria) : false;
	}

CSpaceObject* CWeaponTargetDefinition::FindTarget (CWeaponClass* pWeapon, CInstalledDevice* pDevice, CSpaceObject* pSource, CItemCtx& ItemCtx) const

//	FindTarget
//
//	Returns an appropriate target (or NULL).

	{
	if (!pDevice)
		throw CException(ERR_FAIL);

	//	Look for a target

	bool isOmniDirectional = false;
	int iMinFireArc, iMaxFireArc;
	auto rotationType = pWeapon->GetRotationType(ItemCtx.GetDeviceItem(), &iMinFireArc, &iMaxFireArc);

	CSystem* pSystem = pSource->GetSystem();
	CVector vSourcePos = pDevice->GetPos(pSource);

	//  Find out whether our autoDefenseDevice is omnidirectional,
	//  directional or has a fixed angle

	if (pDevice->IsOmniDirectional() || rotationType == CDeviceRotationDesc::rotOmnidirectional)
		isOmniDirectional = true;

	//  If not omnidirectional, check directional. If not, then
	//  our device points in one direction

	else if (!((pDevice->IsDirectional() 
			|| rotationType == CDeviceRotationDesc::rotSwivelAlways
			|| rotationType == CDeviceRotationDesc::rotSwivelIfTargetInArc)))
		{
		iMinFireArc = AngleMod((pDevice ? pDevice->GetRotation() : 0)
			+ AngleMiddle(iMinFireArc, iMaxFireArc));
		iMaxFireArc = iMinFireArc;
		}

	//  Calculate min/max fire arcs given the object's rotation

	iMinFireArc = isOmniDirectional ? -1 : (pSource->GetRotation() + iMinFireArc) % 360;
	iMaxFireArc = isOmniDirectional ? -1 : (pSource->GetRotation() + iMaxFireArc) % 360;

	//	Compute the range

	Metric rBestDist;
	if (m_TargetCriteria.MatchesMaxRadius() < g_InfiniteDistance)
		rBestDist = m_TargetCriteria.MatchesMaxRadius();
	else
		rBestDist = pDevice->GetMaxRange(ItemCtx);

	//	Now look for the nearest object

	return pSystem->FindNearestTangibleObjectInArc(pSource, vSourcePos, rBestDist, m_TargetCriteria, iMinFireArc, iMaxFireArc);
	}

int CWeaponTargetDefinition::AimAndFire(CWeaponClass* pWeapon, CInstalledDevice* pDevice, CSpaceObject* pSource, CDeviceClass::SDeviceUpdateCtx& Ctx) const
	{
	//	If we're docked with a station, then we do not fire.
	CItemCtx ItemCtx(pSource, pDevice);

	if (m_bCheckLineOfFire && pSource->GetDockedObj())
		return false;

	//	Look for a target; if none, then skip.

	CSpaceObject* pTarget = FindTarget(pWeapon, pDevice, pSource, ItemCtx);
	if (pTarget == NULL)
		return false;

	//	Shoot at target

	int iFireAngle;
	if (!pWeapon->CalcFireSolution(*pDevice, *pTarget, &iFireAngle))
		return false;

	//	If friendlies are in the way, don't shoot

	if (m_bCheckLineOfFire
		&& !pSource->IsLineOfFireClear(pDevice, pTarget, iFireAngle, pSource->GetDistance(pTarget)))
		return false;

	//	Since we're using this as a target, set the destroy notify flag
	//	(Normally beams don't notify, so we need to override this).

	pTarget->SetDestructionNotify();

	//	Fire (Activate will fire as many shots as possible this tick)

	CDeviceClass::SActivateCtx ActivateCtx(Ctx, pTarget, iFireAngle);
	CWeaponFireDesc* pShot = pWeapon->GetWeaponFireDesc(ItemCtx);

	int iNumActivations = pWeapon->Activate(*pDevice, ActivateCtx);

	Ctx.bConsumedItems = ActivateCtx.bConsumedItems;
	return iNumActivations;
	}

std::unique_ptr<CWeaponTargetDefinition> CWeaponTargetDefinition::ReadFromStream(SLoadCtx& Ctx)

//	ReadFromStream
//
//	Reads from stream

	{
	std::unique_ptr<CWeaponTargetDefinition> weaponTargetDefinition = std::make_unique<CWeaponTargetDefinition>();

	Kernel::CString sCriteriaString;
	sCriteriaString.ReadFromStream(Ctx.pStream);
	weaponTargetDefinition->SetTargetCriteria(sCriteriaString);

	DWORD dwLoad;
	Ctx.pStream->Read(dwLoad);
	weaponTargetDefinition->SetCheckLineOfFire((dwLoad & 0x00000001) ? true : false);

	return std::move(weaponTargetDefinition);
	}

void CWeaponTargetDefinition::WriteToStream(IWriteStream *pStream) const

//	WriteToStream
//
//	CString				CriteriaString
//	DWORD				flags

	{
	m_CriteriaString.WriteToStream(pStream);
	
	DWORD dwSave = 0;
	dwSave |= (m_bCheckLineOfFire ? 0x00000001 : 0);
	pStream->Write(dwSave);
	}
