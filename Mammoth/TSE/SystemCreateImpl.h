//	SystemCreateImpl.h
//
//	Create system classes and functions
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CAngleGenerator
	{
	public:
		Metric GetValue (int iIndex) const { return (m_Values.GetCount() > 0 ? m_Values[iIndex % m_Values.GetCount()] : m_rDefault);}
		ALERROR Init (SSystemCreateCtx &Ctx, int iCount, const CString &sValue, Metric rDefault = 0.0, int iJitter = 0);
		ALERROR InitFromInclination (SSystemCreateCtx &Ctx, const CXMLElement &XMLDesc, int iCount, Metric rDefault = 0.0);
		ALERROR InitFromRotation (SSystemCreateCtx &Ctx, const CXMLElement &XMLDesc, int iCount, Metric rDefault = 0.0);

		static ALERROR Generate (SSystemCreateCtx &Ctx, const CString &sAngle, int iCount, Metric *pAngles, int iJitter = 0);

	private:
		static constexpr int RANDOM_ANGLE_MAX_TRIES = 20;

		TArray<Metric> m_Values;
		Metric m_rDefault = 0.0;
	};

class COrbitFocusGenerator
	{
	public:
		CVector3D GetValue (int iIndex) const;
		ALERROR InitFromXML (SSystemCreateCtx &Ctx, const CXMLElement &XMLDesc, const CVector3D &vCenter, int iCount);

	private:
		CVector3D m_vCenter;
		DiceRange m_ZOffset;
		Metric m_rScale;
	};

class CSmartZAdjust
	{
	public:
		CSmartZAdjust (SSystemCreateCtx &Ctx) :
				m_Ctx(Ctx)
			{
			m_SavedZAdjust = Ctx.ZAdjust;
			m_ZOffset = Ctx.ZAdjust.ZOffset;
			Ctx.ZAdjust.ZOffset = DiceRange();
			}

		CSmartZAdjust (SSystemCreateCtx &Ctx, const COrbit &OrbitDesc) :
				m_Ctx(Ctx)
			{
			m_SavedZAdjust = Ctx.ZAdjust;

			m_ZOffset = Ctx.ZAdjust.ZOffset;
			m_NewOrbit = GetOrbit(OrbitDesc);
			Ctx.ZAdjust.ZOffset = DiceRange();
			}

		~CSmartZAdjust (void)
			{
			m_Ctx.ZAdjust = m_SavedZAdjust;
			}

		CVector3D GetFocus (const CVector3D vFocus) const
			{
			if (!m_ZOffset.IsEmpty())
				{
				Metric rZOffset = m_ZOffset.Roll() * m_Ctx.ZAdjust.rScale;
				CVector3D vOffset(0.0, 0.0, rZOffset);
				return vFocus + vOffset;
				}
			else
				return vFocus;
			}

		const COrbit &GetOrbit (void) const { return m_NewOrbit; }

		COrbit GetOrbit (const COrbit &OrbitDesc) const
			{
			if (!m_ZOffset.IsEmpty())
				{
				COrbit NewOrbit = OrbitDesc;

				Metric rZOffset = m_ZOffset.Roll() * m_Ctx.ZAdjust.rScale;
				CVector3D vOffset(0.0, 0.0, rZOffset);
				NewOrbit.SetFocus(OrbitDesc.GetFocus3D() + vOffset);

				return NewOrbit;
				}
			else
				return OrbitDesc;
			}

	private:
		SSystemCreateCtx &m_Ctx;
		SZAdjust m_SavedZAdjust;
		COrbit m_NewOrbit;
		DiceRange m_ZOffset;
	};

class CSmartSystemCreateOptions
	{
	public:
		CSmartSystemCreateOptions (SSystemCreateCtx &Ctx) :
				m_Ctx(Ctx)
			{
			m_bSavedIs3DExtra = m_Ctx.bIs3DExtra;
			}

		~CSmartSystemCreateOptions (void)
			{
			m_Ctx.bIs3DExtra = m_bSavedIs3DExtra;
			}

		ALERROR Init (const CXMLElement &XMLDesc);

	private:
		SSystemCreateCtx &m_Ctx;
		bool m_bSavedIs3DExtra;
	};
