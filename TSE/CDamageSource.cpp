//	CDamageSource.cpp
//
//	CDamageSource class

#include "PreComp.h"

CDamageSource CDamageSource::m_Null;

CDamageSource::CDamageSource (CSpaceObject *pSource, DestructionTypes iCause, CSpaceObject *pSecondarySource, const CString &sSourceName, DWORD dwSourceFlags) : 
		m_iCause(iCause),
		m_dwFlags(0),
		m_sSourceName(sSourceName),
		m_dwSourceNameFlags(dwSourceFlags)

//	CDamageSource constructor

	{
	SetObj(pSource);

	m_pSecondarySource = (pSecondarySource && !pSecondarySource->IsDestroyed() ? pSecondarySource : NULL);
	}

bool CDamageSource::CanHitFriends (void) const

//	CanHitFriends
//
//	Returns TRUE if we know that the source can hit friend.

	{
	CSpaceObject *pObj = GetObj();
	return (pObj ? pObj->CanHitFriends() : false);
	}

CString CDamageSource::GetDamageCauseNounPhrase (DWORD dwFlags)

//	GetDamageCauseNounPhrase
//
//	Returns the name of the damage source

	{
	if (IsObjPointer())
		return m_pSource->GetDamageCauseNounPhrase(dwFlags);
	else if (!m_sSourceName.IsBlank())
		return ::ComposeNounPhrase(m_sSourceName, 1, NULL_STR, m_dwSourceNameFlags, dwFlags);
	else
		return CONSTLIT("damage");
	}

CSpaceObject *CDamageSource::GetObj (void) const

//	GetObj
//
//	Returns the source object

	{
	//	If the source is the player then always return the player
	//	object (regardless of m_pSource). We do this in case
	//	the player changes ships.

	if (m_dwFlags & FLAG_IS_PLAYER)
		{
		CSystem *pSystem = g_pUniverse->GetCurrentSystem();
		return (pSystem ? pSystem->GetPlayerShip() : NULL);
		}

	//	If we're a player subordinate then we do custom handling because we 
	//	could return the player if necessary.

	else if (m_dwFlags & FLAG_IS_PLAYER_SUBORDINATE)
		{
		//	If we have a valid object pointer, just return that.

		if (IsObjPointer())
			return m_pSource;

		//	Otherwise, we return the player.

		else
			{
			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			return (pSystem ? pSystem->GetPlayerShip() : NULL);
			}
		}

	//	If all we have is an object ID, then we can't return the
	//	object pointer.

	else if (m_dwFlags & FLAG_OBJ_ID)
		return NULL;

	//	Otherwise, return the source (even if NULL)

	else
		return m_pSource;
	}

DWORD CDamageSource::GetObjID (void) const

//	GetObjID
//
//	Returns the object ID of the source, which we are more likely to have than 
//	the object pointer.
//
//	We return OBJID_NULL if not found.
//
//	NOTE: We do not guarantee that the ID of the object returned by GetObj() 
//	will be the same as this ID. Sometimes, we might return the player object
//	in GetObj but return a valid (but different) ID here.

	{
	//	If the source is the player then always return the player
	//	object (regardless of m_pSource). We do this in case
	//	the player changes ships.

	if (m_dwFlags & FLAG_IS_PLAYER)
		{
		CSystem *pSystem = g_pUniverse->GetCurrentSystem();
		if (pSystem == NULL)
			return OBJID_NULL;

		CSpaceObject *pPlayerShip = pSystem->GetPlayerShip();
		if (pPlayerShip == NULL)
			return OBJID_NULL;

		return pPlayerShip->GetID();
		}

	//	If we have an object ID, then that's enough

	else if (IsObjID())
		return (DWORD)m_pSource;

	//	If we have an actual object pointer, then return it.

	else if (m_pSource)
		return m_pSource->GetID();

	//	Otherwise, nothing

	else
		return OBJID_NULL;
	}

CSpaceObject *CDamageSource::GetOrderGiver (void) const

//	GetOrderGiver
//
//	Returns the object which actually gave the order (or NULL)
	
	{
	CSpaceObject *pObj = GetObj();
	return (pObj ? pObj->GetOrderGiver(GetCause()) : NULL);
	}

CSovereign *CDamageSource::GetSovereign (void) const

//	GetSovereign
//
//	Returns the sovereign of the source

	{
	if (IsPlayer())
		return g_pUniverse->GetPlayerSovereign();
	else if (IsObjPointer())
		return m_pSource->GetSovereign();
	else
		return NULL;
	}

DWORD CDamageSource::GetSovereignUNID (void) const

//	GetSovereignUNID
//
//	Returns the sovereign

	{
	CSovereign *pSovereign = GetSovereign();
	if (pSovereign == NULL)
		return 0;

	return pSovereign->GetUNID();
	}

bool CDamageSource::HasSource (void) const

//	HasSource
//
//	Returns TRUE if we have a source, either a direct pointer or an object ID.

	{
	//	If our ultimate source is the player, or if we have an object ID, then
	//	we have a source.
	//
	//	NOTE: If the player is dead, then we would return TRUE here but return
	//	OBJID_NULL when trying to get the actual ID. That's OK for now since 
	//	this is only used for hit testing (and it's more efficient this way).

	if (m_dwFlags & (FLAG_IS_PLAYER | FLAG_IS_PLAYER_SUBORDINATE | FLAG_OBJ_ID))
		return true;

	//	If we have an actual object pointer, then we have a source.

	else
		return (m_pSource != NULL);
	}

bool CDamageSource::IsCausedByEnemyOf (CSpaceObject *pObj) const

//	IsCausedByEnemyOf
//
//	Returns TRUE if the damage is caused by an object that is
//	an enemy of pObj.

	{
	if (IsCausedByPlayer())
		{
		CSovereign *pSovereign = pObj->GetSovereign();
		return (pSovereign ? pSovereign->IsEnemy(g_pUniverse->GetPlayerSovereign()) : false);
		}
	else
		return (IsObjPointer() && m_pSource->CanAttack() && pObj->IsEnemy(m_pSource));
	}

bool CDamageSource::IsCausedByFriendOf (CSpaceObject *pObj) const

//	IsCausedByFriendOf
//
//	Returns TRUE if the damage is caused by an object that is
//	friends of pObj.

	{
	if (IsCausedByPlayer())
		{
		CSovereign *pSovereign = pObj->GetSovereign();
		return (pSovereign ? pSovereign->IsFriend(g_pUniverse->GetPlayerSovereign()) : false);
		}
	else
		return (IsObjPointer() && m_pSource->CanAttack() && pObj->IsFriend(m_pSource));
	}

bool CDamageSource::IsCausedByNonFriendOf (CSpaceObject *pObj) const

//	IsCausedByNonFriendOf
//
//	Returns TRUE if the damage is caused by an object that is
//	not a friend of pObj and that can attack.

	{
	if (IsCausedByPlayer())
		{
		CSovereign *pSovereign = pObj->GetSovereign();
		return (pSovereign ? !pSovereign->IsFriend(g_pUniverse->GetPlayerSovereign()) : false);
		}
	else
		return (IsObjPointer() && m_pSource->CanAttack() && !pObj->IsFriend(m_pSource));
	}

bool CDamageSource::IsEnemy (CDamageSource &Src) const

//	IsEnemy
//
//	Returns TRUE if we are an enemy of the given source.

	{
	CSpaceObject *pObj = GetObj();
	CSpaceObject *pSrc = Src.GetObj();

	return ((pObj && pSrc) ? pObj->IsEnemy(pSrc) : false);
	}

bool CDamageSource::IsEqual (const CDamageSource &Src) const

//	IsEqual
//
//	Returns if this is the same damage source

	{
	DWORD dwID = GetObjID();
	DWORD dwOtherID = Src.GetObjID();

	//	If we have a secondary object, see if we match that.

	if (m_pSecondarySource && m_pSecondarySource->GetID() == dwOtherID)
		return true;
	else if (Src.m_pSecondarySource && Src.m_pSecondarySource->GetID() == dwID)
		return true;

	//	If we have a valid object ID, then we are equal if the other object has
	//	the exact same ID.

	if (dwID != OBJID_NULL)
		return (dwID == dwOtherID);

	//	If the other source has an ID, then we are NOT equal

	if (dwOtherID != OBJID_NULL)
		return false;

	//	If neither has a known object, then we check to see if we're both part 
	//	of an explosion. If so, we're equal.

	return (GetCause() == Src.GetCause()
			&& (GetCause() == killedByExplosion || GetCause() == killedByPlayerCreatedExplosion));
	}

bool CDamageSource::IsEqual (CSpaceObject *pSrc) const

//	IsEqual
//
//	Returns TRUE if this is the same as the given object.

	{
	//	If pSrc is equal to our secondary source, then we match

	if (pSrc == m_pSecondarySource)
		return true;

	//	Otherwise, see if we match the primary source

	else
		{
		DWORD dwID = GetObjID();

		//	If we have an ID, and it's the same as the given one, then we're equal.

		return (dwID != OBJID_NULL && pSrc && dwID == pSrc->GetID());
		}
	}

void CDamageSource::OnObjDestroyed (CSpaceObject *pObjDestroyed)

//	OnObjDestroyed
//
//	An object was destroyed

	{
	if (pObjDestroyed == m_pSource && !IsObjID())
		{
		m_sSourceName = m_pSource->GetName(&m_dwSourceNameFlags);
		m_pSource = (CSpaceObject *)m_pSource->GetID();
		m_dwFlags |= FLAG_OBJ_ID;
		}

	if (pObjDestroyed == m_pSecondarySource)
		m_pSecondarySource = NULL;
	}

void CDamageSource::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Read data from a stream
//
//	DWORD		m_pSource
//	CString		m_sSourceName
//	DWORD		m_dwSourceNameFlags
//	DWORD		m_pSecondarySource
//	DWORD		m_iCause
//	DWORD		m_dwFlags

	{
	DWORD dwLoad;

	//	Read the source object ID. We're not sure yet (until we read the flags
	//	whether this is a real object or just an ID).

	DWORD dwObjID;
	Ctx.pStream->Read((char *)&dwObjID, sizeof(DWORD));

	//	Read source name

	if (Ctx.dwVersion >= 30)
		{
		m_sSourceName.ReadFromStream(Ctx.pStream);
		Ctx.pStream->Read((char *)&m_dwSourceNameFlags, sizeof(DWORD));
		}

	if (Ctx.dwVersion >= 32)
		CSystem::ReadObjRefFromStream(Ctx, &m_pSecondarySource);

	if (Ctx.dwVersion >= 45)
		{
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
		m_iCause = (DestructionTypes)dwLoad;
		}
	else
		m_iCause = killedByDamage;

	if (Ctx.dwVersion >= 55)
		Ctx.pStream->Read((char *)&m_dwFlags, sizeof(DWORD));
	else
		m_dwFlags = 0;

	//	If this is an object ID, then we just store it in m_pSource.

	if (IsObjID())
		m_pSource = (CSpaceObject *)dwObjID;

	//	Otherwise, we need to resolve the pointer

	else
		CSystem::GetObjRefFromID(Ctx, dwObjID, &m_pSource);
	}

void CDamageSource::SetObj (CSpaceObject *pSource)

//	SetObj
//
//	Sets the damage source
	
	{
	//	If this is the player, remember it in case we lose the
	//	source later.

	if (pSource)
		{
		CSpaceObject *pOrderGiver;

		if (pSource->IsPlayer())
			{
			m_dwFlags |= FLAG_IS_PLAYER;
			m_dwFlags |= FLAG_IS_PLAYER_CAUSED;
			}
		else if ((pOrderGiver = pSource->GetOrderGiver()) && pOrderGiver->IsPlayer())
			{
			m_dwFlags |= FLAG_IS_PLAYER_SUBORDINATE;
			m_dwFlags |= FLAG_IS_PLAYER_CAUSED;
			}
		}
	else
		m_dwFlags &= ~(FLAG_IS_PLAYER | FLAG_IS_PLAYER_SUBORDINATE | FLAG_IS_PLAYER_CAUSED);

	//	If the source is already destroyed, then don't store it--just get the
	//	name of the source.

	if (pSource && pSource->IsDestroyed())
		{
		m_sSourceName = pSource->GetName(&m_dwSourceNameFlags);
		m_pSource = (CSpaceObject *)pSource->GetID();
		m_dwFlags |= FLAG_OBJ_ID;
		}

	//	Otherwise, remember the source

	else
		{
		m_pSource = pSource;
		m_dwFlags &= ~FLAG_OBJ_ID;
		}
	}

void CDamageSource::WriteToStream (CSystem *pSystem, IWriteStream *pStream)

//	WriteToStream
//
//	Write data to a stream
//
//	DWORD		m_pSource
//	CString		m_sSourceName
//	DWORD		m_dwSourceNameFlags
//	DWORD		m_pSecondarySource
//	DWORD		m_iCause
//	DWORD		m_dwFlags

	{
	DWORD dwSave;

	//	If this is an object ID, we just save it

	if (IsObjID())
		{
		dwSave = GetRawObjID();
		pStream->Write((char *)&dwSave, sizeof(DWORD));
		}

	//	Otherwise, save a reference

	else
		pSystem->WriteObjRefToStream(m_pSource, pStream);

	m_sSourceName.WriteToStream(pStream);
	pStream->Write((char *)&m_dwSourceNameFlags, sizeof(DWORD));
	pSystem->WriteObjRefToStream(m_pSecondarySource, pStream);

	dwSave = m_iCause;
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	pStream->Write((char *)&m_dwFlags, sizeof(DWORD));
	}
