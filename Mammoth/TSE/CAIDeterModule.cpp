//	CAIDeterModule.cpp
//
//	CAIDeterModule class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

bool CAIDeterModule::Behavior (CShip &Ship, CAIBehaviorCtx &Ctx)

//	Behavior
//
//	Update the module. We return TRUE if we control the ship. FALSE otherwise.

	{
	if (!m_pTarget)
		return false;

	//	Every once in a while, check to see if we've still got a target.

	else if (Ship.IsDestinyTime(19) 
			&& !Ctx.CalcIsDeterNeeded(Ship, *m_pTarget))
		{
		Cancel();
		return false;
		}

	//	Aim and fire at target.

	else
		{
		if (m_bNoTurn)
			{
			CVector vTarget = m_pTarget->GetPos() - Ship.GetPos();
			Metric rTargetDist2 = vTarget.Dot(vTarget);

			Ctx.ImplementFireWeaponOnTarget(&Ship, -1, -1, m_pTarget, vTarget, rTargetDist2);
			}
		else
			Ctx.ImplementAttackTarget(&Ship, m_pTarget, true);

		Ctx.ImplementFireOnTargetsOfOpportunity(&Ship, m_pTarget);

		return true;
		}
	}

void CAIDeterModule::BehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, CSpaceObject &TargetObj, bool bNoTurn)

//	BehaviorStart
//
//	Starts deterring.

	{
	//	If we're already deterring a target, see if the new one is better. If 
	//	not, then we're done.

	if (m_pTarget
			&& !Ctx.CalcIsBetterTarget(&Ship, m_pTarget, &TargetObj))
		return;

	//	Take the target.

	m_pTarget = &TargetObj;
	m_bNoTurn = bNoTurn;
	}

void CAIDeterModule::OnObjDestroyed (CShip &Ship, const SDestroyCtx &Ctx)

//	OnObjDestroy
//
//	Object has been destroyed.

	{
	if (Ctx.Obj == m_pTarget)
		{
		//	If a friend destroyed our target then thank them

		if (Ctx.Attacker.IsCausedByFriendOf(&Ship) && Ctx.Attacker.GetObj())
			Ship.Communicate(Ctx.Attacker.GetObj(), msgNiceShooting);

		//	Done

		Cancel();
		}
	}

void CAIDeterModule::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Reads the object from stream.

	{
	CSystem::ReadObjRefFromStream(Ctx, &m_pTarget);

	DWORD dwFlags = 0;
	Ctx.pStream->Read(dwFlags);
	m_bNoTurn = ((dwFlags & 0x00000001) ? true : false);
	}

void CAIDeterModule::WriteToStream (IWriteStream &Stream) const

//	WriteToStream
//
//	Writes the object to a stream.

	{
	CSystem::WriteObjRefToStream(Stream, m_pTarget);

	DWORD dwFlags = 0;
	dwFlags |= (m_bNoTurn ? 0x00000001 : 0);
	Stream.Write(dwFlags);
	}
