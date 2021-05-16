//	SUpdateCtx.cpp
//
//	SUpdateCtx structure
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

int SUpdateCtx::GetLightIntensity (CSpaceObject *pObj) const
	{
	if (pObj == m_pCacheObj && m_iLightIntensity != -1)
		return m_iLightIntensity;

	m_pCacheObj = pObj;
	m_iLightIntensity = pSystem->CalculateLightIntensity(pObj->GetPos());
	return m_iLightIntensity;
	}

CTargetList &SUpdateCtx::GetTargetList ()

//	GetTargetList
//
//	Initializes and returns the target list.

	{
	if (!m_bTargetListValid && m_pObj)
		{
		m_TargetList = m_pObj->GetTargetList();
		m_bTargetListValid = true;
		}

	return m_TargetList;
	}

void SUpdateCtx::OnStartUpdate (CSpaceObject &Obj)
	{
	m_pObj = &Obj;
	m_bTimeStopped = Obj.IsTimeStopped();
	m_bTargetListValid = false;
	}

void SUpdateCtx::SetPlayerShip (CSpaceObject &PlayerObj)

//	SetPlayerShip
//
//	Sets the player.

	{
	if (PlayerObj.IsDestroyed())
		return;

	m_pPlayer = &PlayerObj;

	m_AutoMining.Init(*m_pPlayer);
	m_AutoTarget.Init(*m_pPlayer);
	}

void SUpdateCtx::UpdatePlayerCalc (const CSpaceObject &Obj)

//	UpdatePlayerCalc
//
//	Updates calculations required by the player UI, etc.

	{
	if (!m_pPlayer || m_pPlayer->IsDestroyed() || m_pPlayer == Obj)
		return;

	m_AutoMining.Update(*m_pPlayer, Obj);
	m_AutoTarget.Update(*m_pPlayer, Obj);

	if (m_pPlayer->IsInOurSquadron(Obj))
		m_bPlayerHasSquadron = true;

	//	NOTE: Because of the current Squadron UI, ships in our squadron don't
	//	count for purposes of communications.

	else if (Obj.CanCommunicateWith(*m_pPlayer))
		m_bPlayerHasCommsTarget = true;
	}
