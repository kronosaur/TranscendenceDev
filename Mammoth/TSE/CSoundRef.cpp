//	CSoundRef.cpp
//
//	CSoundRef class

#include "PreComp.h"

ALERROR CSoundRef::Bind (SDesignLoadCtx &Ctx)

//	Bind
//
//	Binds the design (looking up the actual sound file)

	{
	if (Ctx.bNoResources || Ctx.GetUniverse().GetSoundMgr() == NULL)
		{
		m_pSound = NULL;
		return NOERROR;
		}

	if (m_dwUNID)
		{
		m_pSound = Ctx.GetUniverse().FindSoundResource(m_dwUNID);
		if (m_pSound)
			{
			if (!m_pSound->IsBound())
				{
				if (ALERROR error = m_pSound->BindDesign(Ctx))
					return error;
				}
			}
		else if (Ctx.GetAPIVersion() >= 12)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Unable to find sound: %x."), m_dwUNID);
			return ERR_FAIL;
			}
		}
	else
		m_pSound = NULL;

	return NOERROR;
	}

ALERROR CSoundRef::LoadUNID (SDesignLoadCtx &Ctx, const CString &sAttrib)

//	LoadUNID
//
//	Loads the sound file UNID

	{
	return ::LoadUNID(Ctx, sAttrib, &m_dwUNID);
	}

void CSoundRef::PlaySound (CSpaceObject *pSource)

//	PlaySound
//
//	Plays the sound

	{
	if (m_pSound)
		g_pUniverse->PlaySound(pSource, m_pSound->GetSound());
	}
