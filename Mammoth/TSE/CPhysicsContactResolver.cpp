//	CPhysicsContactResolver.cpp
//
//	CPhysicsContactResolver class
//	Copyright (c) 2017 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const int MAX_CONTACTS = 100;

void CPhysicsContactResolver::AddCollision (CSpaceObject *pObj, CSpaceObject *pContactObj)

//	AddCollision
//
//	Adds a collision between the two objects.
//
//	NOTE: Callers must guarantee that we only have one collision for each pair
//	of objects. I.e., if we call AddCollision on objects A and B, we should NOT
//	later call it on objects B and A.

	{
	CPhysicsContact *pContact = m_Contacts.Insert();
	pContact->InitCollision(pObj, pContactObj);
	}

void CPhysicsContactResolver::AddRod (CPhysicsContact::ETypes iType, CSpaceObject *pObj1, CSpaceObject *pObj2, Metric rLength)

//	AddRod
//
//	Adds a rod between the two objects.

	{
	//	Compute the position of obj2 relative to obj1

	CVector vDist = pObj2->GetPos() - pObj1->GetPos();
	Metric rDist;
	CVector vNormal = vDist.Normal(&rDist);

	//	First add a contact that will resolve when the rod is too short.

	CPhysicsContact *pContact = m_Contacts.Insert();
	pContact->Init(iType, pObj1, pObj2, -vNormal, rLength - rDist, 0.0);

	//	Next add a contact that will resolve when the rod is too long.

	pContact = m_Contacts.Insert();
	pContact->Init(iType, pObj1, pObj2, vNormal, rDist - rLength, 0.0);
	}

void CPhysicsContactResolver::BeginUpdate (void)

//	BeginUpdate
//
//	Starts a new frame

	{
	m_Contacts.DeleteAll();
	m_Contacts.GrowToFit(MAX_CONTACTS);
	}

CPhysicsContact *CPhysicsContactResolver::FindContactToResolve (void)

//	FindContactToResolve
//
//	Returns the best contact to resolve. If none found, we return NULL.

	{
	//	This is an array, so we traverse by pointer for efficiency.

	CPhysicsContact *pContact = &m_Contacts[0];
	CPhysicsContact *pContactEnd = pContact + m_Contacts.GetCount();

	//	Find the contact with the largest negative separation velocity
	//	(i.e., the one where we're colliding the fastest).

	CPhysicsContact *pBestContact = NULL;
	Metric rBestV = DBL_MAX;

	while (pContact < pContactEnd)
		{
		Metric rSeparatingV = pContact->GetSeparatingVel();
		if ((rSeparatingV < 0.0 || pContact->GetPenetration() > 0.0)
				&& rSeparatingV < rBestV)
			{
			pBestContact = pContact;
			rBestV = rSeparatingV;
			}

		//	Next

		pContact++;
		}

	return pBestContact;
	}

void CPhysicsContactResolver::RecalcContacts (CPhysicsContact *pChanged)

//	RecalcContacts
//
//	Invalidates other contacts involved with pChanged, which just got resolved.

	{
	CPhysicsContact *pContact = &m_Contacts[0];
	CPhysicsContact *pContactEnd = pContact + m_Contacts.GetCount();
	while (pContact < pContactEnd)
		{
		//	Skip if this is the contact that changed because we update it in
		//	the resolve step.

		if (pContact == pChanged)
			{ }

		//	Otherwise, recalc

		else
			{
			if (pChanged->GetObj1() == pContact->GetObj1())
				pContact->Recalc(-pChanged->GetMove1());
			else if (pChanged->GetObj2() == pContact->GetObj1())
				pContact->Recalc(-pChanged->GetMove2());
		
			if (pChanged->GetObj1() == pContact->GetObj2())
				pContact->Recalc(pChanged->GetMove1());
			else if (pChanged->GetObj2() == pContact->GetObj2())
				pContact->Recalc(pChanged->GetMove2());
			}

		pContact++;
		}
	}

void CPhysicsContactResolver::Update (void)

//	Update
//
//	Resolves all contacts.

	{
	//	Keep looping util we've resolved everything or exhausted our iteration
	//	limit.

	int iIterationsLeft = 4 * m_Contacts.GetCount();
	while (iIterationsLeft > 0)
		{
		//	Find the next contact to resolve. If we have no more, then we're
		//	done with the loop.

		CPhysicsContact *pContact = FindContactToResolve();
		if (pContact == NULL)
			break;

		//	Resolve the contact

		pContact->Resolve();

		//	Invalidate the separating velocity for all contacts that involve 
		//	these two objects.

		RecalcContacts(pContact);

		//	Next

		iIterationsLeft--;
		}

	//	Close out the contacts

	if (m_Contacts.GetCount() > 0)
		{
		CPhysicsContact *pContact = &m_Contacts[0];
		CPhysicsContact *pContactEnd = pContact + m_Contacts.GetCount();
		while (pContact < pContactEnd)
			{
			pContact->OnUpdateDone();
			pContact++;
			}
		}
	}
