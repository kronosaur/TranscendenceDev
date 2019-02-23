//	CNamedEffects.cpp
//
//	CNamedEffects class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

DWORD CNamedEffects::m_NamedPainterUNID[painterCount] = 
	{
	MEDIUM_STATION_DAMAGE_UNID,
	LARGE_STATION_DAMAGE_UNID,
	};

CNamedEffects::CNamedEffects (CNamedEffects &&Src)

//	CNamedEffects move constructor

	{
	Move(Src);
	}

CNamedEffects &CNamedEffects::operator= (CNamedEffects &&Src)

//	CNamedEffects move operator

	{
	Move(Src);
	return *this;
	}

void CNamedEffects::CleanUp (void)

//	CleanUp
//
//	Restore to initial state

	{
	for (int i = 0; i < painterCount; i++)
		{
		if (m_pNamedPainter[i])
			{
			m_pNamedPainter[i]->Delete();
			m_pNamedPainter[i] = NULL;
			}
		}

	for (int i = 0; i < damageCount; i++)
		{
		m_pDefaultFireEffect[i] = NULL;
		m_pDefaultHitEffect[i] = NULL;
		}
	}

CEffectCreator &CNamedEffects::GetFireEffect (CDesignCollection &Design, DamageTypes iDamage) const

//	GetFireEffect
//
//	Returns a default fire effect creator

	{
	if (iDamage == damageGeneric)
		return GetNullEffect(Design);

	else if (m_pDefaultFireEffect[iDamage] == NULL)
		{
		DWORD dwCreatorUNID = UNID_FIRST_DEFAULT_FIRE_EFFECT + iDamage;
		m_pDefaultFireEffect[iDamage] = CEffectCreator::AsType(Design.FindEntry(dwCreatorUNID));
		if (m_pDefaultFireEffect[iDamage] == NULL)
			throw CException(ERR_FAIL, strPatternSubst(CONSTLIT("Default fire effect for %s not defined"), GetDamageName(iDamage)));
		}

	return *m_pDefaultFireEffect[iDamage];
	}

CEffectCreator &CNamedEffects::GetHitEffect (CDesignCollection &Design, DamageTypes iDamage) const

//	GetHitEffect
//
//	Returns a default hit effect creator

	{
	if (iDamage == damageGeneric)
		return GetNullEffect(Design);

	else if (m_pDefaultHitEffect[iDamage] == NULL)
		{
		DWORD dwCreatorUNID = UNID_FIRST_DEFAULT_EFFECT + iDamage;
		m_pDefaultHitEffect[iDamage] = CEffectCreator::AsType(Design.FindEntry(dwCreatorUNID));
		if (m_pDefaultHitEffect[iDamage] == NULL)
			{
			m_pDefaultHitEffect[iDamage] = CEffectCreator::AsType(Design.FindEntry(UNID_BASIC_HIT_EFFECT));
			if (m_pDefaultHitEffect[iDamage] == NULL)
				throw CException(ERR_FAIL, strPatternSubst(CONSTLIT("Default hit effect for %s not defined"), GetDamageName(iDamage)));
			}
		}

	return *m_pDefaultHitEffect[iDamage];
	}

CEffectCreator &CNamedEffects::GetNullEffect (CDesignCollection &Design) const

//	GetNullEffect
//
//	Returns a null effect.

	{
	if (m_pNullEffect == NULL)
		{
		m_pNullEffect = CEffectCreator::AsType(Design.FindEntry(UNID_NULL_EFFECT));
		if (m_pNullEffect == NULL)
			throw CException(ERR_FAIL, strPatternSubst(CONSTLIT("Null effect %08x not defined"), UNID_NULL_EFFECT));
		}

	return *m_pNullEffect;
	}

IEffectPainter &CNamedEffects::GetPainter (CDesignCollection &Design, ETypes iType) const

//	GetPainter
//
//	Returns a painter.

	{
	if (m_pNamedPainter[iType] == NULL)
		{
		DWORD dwCreatorUNID = m_NamedPainterUNID[iType];
		CEffectCreator *pEffect = CEffectCreator::AsType(Design.FindEntry(dwCreatorUNID));
		if (pEffect == NULL)
			throw CException(ERR_FAIL, strPatternSubst(CONSTLIT("Named painter effect %08x not defined"), dwCreatorUNID));

		m_pNamedPainter[iType] = pEffect->CreatePainter(CCreatePainterCtx());
		if (m_pNamedPainter[iType] == NULL)
			throw CException(ERR_FAIL, strPatternSubst(CONSTLIT("Unable to create named painter %08x"), dwCreatorUNID));
		}

	return *m_pNamedPainter[iType];
	}

void CNamedEffects::Move (CNamedEffects &Src)

//	Move
//
//	Move from Src to this.

	{
	for (int i = 0; i < painterCount; i++)
		{
		m_pNamedPainter[i] = Src.m_pNamedPainter[i];
		Src.m_pNamedPainter[i] = NULL;
		}

	for (int i = 0; i < damageCount; i++)
		{
		m_pDefaultFireEffect[i] = Src.m_pDefaultFireEffect[i];
		Src.m_pDefaultFireEffect[i] = NULL;

		m_pDefaultHitEffect[i] = Src.m_pDefaultHitEffect[i];
		Src.m_pDefaultHitEffect[i] = NULL;
		}
	}
