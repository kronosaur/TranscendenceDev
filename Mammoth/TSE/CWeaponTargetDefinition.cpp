#include "PreComp.h"

bool CWeaponTargetDefinition::MatchesTarget(CSpaceObject* pSource, CSpaceObject* pTarget) const

//	MatchesTarget
//
//	Returns True if the target matches the criteria
	{
	CSpaceObjectCriteria::SCtx Ctx(pSource, m_TargetCriteria);
	return pTarget ? pTarget->MatchesCriteria(Ctx, m_TargetCriteria) : false;
	}

CSpaceObject* CWeaponTargetDefinition::FindTarget(CWeaponClass* pWeapon, CInstalledDevice* pDevice, CSpaceObject* pSource, CItemCtx& ItemCtx) const

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

	CSpaceObject* pBestTarget = NULL;
	CSystem* pSystem = pSource->GetSystem();
	CVector vSourcePos = pDevice->GetPos(pSource);

	//  Find out whether our autoDefenseDevice is omnidirectional,
	//  directional or has a fixed angle

	if (pDevice->IsOmniDirectional() || rotationType == CDeviceRotationDesc::rotOmnidirectional)
		isOmniDirectional = true;

	//  If not omnidirectional, check directional. If not, then
	//  our device points in one direction

	else if (!((pDevice->IsDirectional() || rotationType == CDeviceRotationDesc::rotSwivel)))
		{
		iMinFireArc = AngleMod((pDevice ? pDevice->GetRotation() : 0)
			+ AngleMiddle(iMinFireArc, iMaxFireArc));
		iMaxFireArc = iMinFireArc;
		}

	//  Calculate min/max fire arcs given the object's rotation

	iMinFireArc = (pSource->GetRotation() + iMinFireArc) % 360;
	iMaxFireArc = (pSource->GetRotation() + iMaxFireArc) % 360;

		//	Compute the range

		Metric rBestDist2;
		if (m_TargetCriteria.MatchesMaxRadius() < g_InfiniteDistance)
			rBestDist2 = (m_TargetCriteria.MatchesMaxRadius() * m_TargetCriteria.MatchesMaxRadius());
		else
			rBestDist2 = pDevice->GetMaxRange(ItemCtx) * pDevice->GetMaxRange(ItemCtx);

		//	Now look for the nearest object

		CSpaceObjectCriteria::SCtx Ctx(pSource, m_TargetCriteria);
	for (int i = 0; i < pSystem->GetObjectCount(); i++)
		{
		CSpaceObject* pObj = pSystem->GetObject(i);
		Metric rDistance2;
		if (pObj
			&& pObj->MatchesCriteriaCategory(Ctx, m_TargetCriteria)
			&& ((rDistance2 = (pObj->GetPos() - vSourcePos).Length2()) < rBestDist2)
			&& pObj->MatchesCriteria(Ctx, m_TargetCriteria)
			&& !pObj->IsIntangible()
			&& pObj != pSource
			&& (isOmniDirectional
				|| AngleInArc(VectorToPolar((pObj->GetPos() - vSourcePos)), iMinFireArc, iMaxFireArc)))
			{
			pBestTarget = pObj;
			rBestDist2 = rDistance2;
			}
		}

	return pBestTarget;
	}

bool CWeaponTargetDefinition::AimAndFire(CWeaponClass* pWeapon, CInstalledDevice* pDevice, CSpaceObject* pSource, CDeviceClass::SDeviceUpdateCtx& Ctx) const
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

	//	Fire

	CDeviceClass::SActivateCtx ActivateCtx(Ctx, pTarget, iFireAngle);
	CWeaponFireDesc* pShot = pWeapon->GetWeaponFireDesc(ItemCtx);

	bool bActivateResult = pWeapon->Activate(*pDevice, ActivateCtx);

	Ctx.bConsumedItems = ActivateCtx.bConsumedItems;
	return bActivateResult;
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
