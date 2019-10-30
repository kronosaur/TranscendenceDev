//	TSEOverlays.h
//
//	Overlay instance classes.
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class COverlay
	{
	public:
		struct SImpactDesc
			{
			CConditionSet Conditions;			//	Set of conditions imparted
			Metric rDrag = 1.0;					//	Drag coefficient (1.0 = no drag)
			};

		COverlay (void);
		~COverlay (void);
		static void CreateFromType (COverlayType &Type, 
									CSpaceObject &Source,
									int iPosAngle,
									int iPosRadius,
									int iRotation,
									int iPosZ,
									int iLifeLeft, 
									COverlay **retpField);

		bool AbsorbDamage (CSpaceObject *pSource, SDamageCtx &Ctx);
		void AccumulateBounds (CSpaceObject *pSource, int iScale, int iRotation, RECT *ioBounds);
		void Destroy (CSpaceObject *pSource);
		bool Disarms (CSpaceObject *pSource) const { return m_pType->Disarms(); }
		void FireCustomEvent (CSpaceObject *pSource, const CString &sEvent, ICCItem *pData, ICCItem **retpResult);
		bool FireGetDockScreen (const CSpaceObject *pSource, CDockScreenSys::SSelector &Selector) const;
		void FireOnCreate (CSpaceObject *pSource);
		bool FireOnDamage (CSpaceObject *pSource, SDamageCtx &Ctx);
		void FireOnDestroy (CSpaceObject *pSource);
		void FireOnObjDestroyed (CSpaceObject *pSource, const SDestroyCtx &Ctx) const;
		void FireOnObjDocked (CSpaceObject *pSource, CSpaceObject *pShip) const;
		CConditionSet GetConditions (CSpaceObject *pSource) const;
		int GetCounter (void) const { return m_iCounter; }
		ICCItemPtr GetData (const CString &sAttrib) const { return m_Data.GetDataAsItem(sAttrib); }
		int GetDevice (void) const { return m_iDevice; }
		Metric GetDrag (CSpaceObject *pSource) const { return m_pType->GetDrag(); }
		DWORD GetID (void) const { return m_dwID; }
		bool GetImpact (CSpaceObject *pSource, SImpactDesc &Impact) const;
		const CString &GetMessage (void) const { return m_sMessage; }
		COverlay *GetNext (void) const { return m_pNext; }
		CVector GetPos (CSpaceObject *pSource) const;
		ICCItemPtr GetProperty (CCodeChainCtx &CCCtx, CSpaceObject &SourceObj, const CString &sProperty) const;
		int GetRotation (void) const { return m_iRotation; }
		COverlayType *GetType(void) const { return m_pType; }
        ICCItemPtr IncData (const CString &sAttrib, ICCItem *pValue = NULL) { return m_Data.IncData(sAttrib, pValue); }
		bool IncProperty (CSpaceObject &SourceObj, const CString &sProperty, ICCItem *pInc, ICCItemPtr &pResult);
		bool IsDestroyed (void) const { return (m_fDestroyed ? true : false); }
		bool IsFading (void) const { return (m_fFading ? true : false); }
		bool IsShieldOverlay (void) const { return m_pType->IsShieldOverlay(); }
		bool IsShipScreenDisabled (void) const { return m_pType->IsShipScreenDisabled(); }
		void Paint (CG32bitImage &Dest, int iScale, int x, int y, SViewportPaintCtx &Ctx);
		void PaintAnnotations (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void PaintBackground (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void PaintLRSAnnotations (const ViewportTransform &Trans, CG32bitImage &Dest, int x, int y);
		void PaintMapAnnotations (CMapViewportCtx &Ctx, CG32bitImage &Dest, int x, int y);
		bool Paralyzes (CSpaceObject *pSource) const { return m_pType->Paralyzes(); }
		void ReadFromStream (SLoadCtx &Ctx);
		void SetData (const CString &sAttrib, ICCItem *pData) { m_Data.SetData(sAttrib, pData); }
		void SetDevice (int iDev) { m_iDevice = iDev; }
		bool SetEffectProperty (const CString &sProperty, ICCItem *pValue);
		void SetNext (COverlay *pNext) { m_pNext = pNext; }
		void SetPos (CSpaceObject *pSource, const CVector &vPos);
		bool SetProperty (CSpaceObject *pSource, const CString &sName, ICCItem *pValue);
		void SetRotation (int iRotation) { m_iRotation = iRotation; }
		bool Spins (CSpaceObject *pSource) const { return m_pType->Spins(); }
		bool StopsTime (const CSpaceObject *pSource) const { return m_pType->StopsTime(); }
		void Update (CSpaceObject *pSource, int iScale, int iRotation, bool *retbModified = NULL);
		void WriteToStream (IWriteStream *pStream);

		static void PaintCounterFlag (CG32bitImage &Dest, int x, int y, const CString &sCounter, const CString &sLabel, CG32bitPixel rgbColor, SViewportPaintCtx &Ctx);

	private:
		void CalcOffset (int iScale, int iRotation, int *retxOffset, int *retyOffset, int *retiRotationOrigin = NULL) const;
		void FireOnUpdate (CSpaceObject *pSource);
		void CreateHitEffect (CSpaceObject *pSource, SDamageCtx &Ctx);
		bool FindCustomProperty (CCodeChainCtx &CCCtx, CSpaceObject &SourceObj, const CString &sProperty, ICCItemPtr &pValue) const;
		CUniverse &GetUniverse (void) const { return (m_pType ? m_pType->GetUniverse() : *g_pUniverse); }
		bool IncCustomProperty (CSpaceObject &SourceObj, const CString &sProperty, ICCItem *pInc, ICCItemPtr &pResult);
		bool SetCustomProperty (CSpaceObject &SourceObj, const CString &sProperty, ICCItem *pValue);

		COverlayType *m_pType;					//	Type of field
		DWORD m_dwID;							//	Universal ID
		int m_iTick;							//	Overlay tick
		int m_iLifeLeft;						//	Ticks left of energy field life (-1 = permanent)
		int m_iDevice;							//	Index of device that we're associated with (-1 if not a device)

		int m_iPosAngle;						//	Position relative to source (degrees)
		int m_iPosRadius;						//	Position relative to source (pixels)
		int m_iRotation;						//	Overlay orientation (degrees)
		int m_iPosZ;							//  Overlay height (pixels)

		CAttributeDataBlock m_Data;				//	data
		int m_iCounter;							//	Arbitrary counter
		CString m_sMessage;						//	Message text

		IEffectPainter *m_pPainter;				//	Painter

		int m_iPaintHit;						//	If >0 then we paint a hit
		int m_iPaintHitTick;					//	Tick for hit painter
		IEffectPainter *m_pHitPainter;			//	Hit painter

		DWORD m_fDestroyed:1;					//	TRUE if field should be destroyed
		DWORD m_fFading:1;						//	TRUE if we're destroyed, but fading the effect

		COverlay *m_pNext;					//	Next energy field associated with this object
	};

class COverlayList
	{
	public:
		COverlayList (void);
		~COverlayList (void);

		void AddField (CSpaceObject &Source, 
					   COverlayType &Type,
					   int iPosAngle,
					   int iPosRadius,
					   int iRotation,
					   int iPosZ,
					   int iLifeLeft, 
					   DWORD *retdwID = NULL);
		bool AbsorbDamage (CSpaceObject *pSource, SDamageCtx &Ctx);
		bool AbsorbsWeaponFire (CInstalledDevice *pDevice);
		void AccumulateBounds (CSpaceObject *pSource, int iScale, int iRotation, RECT *ioBounds);
		bool Damage (CSpaceObject *pSource, SDamageCtx &Ctx);
		CString DebugCrashInfo (void) const;
		bool FireGetDockScreen (const CSpaceObject *pSource, CDockScreenSys::SSelector *retSelector = NULL) const;
		void FireOnObjDestroyed (CSpaceObject *pSource, const SDestroyCtx &Ctx) const;
		void FireOnObjDocked (CSpaceObject *pSource, CSpaceObject *pShip) const;
		const CConditionSet &GetConditions (void) const { return m_Conditions; }
		int GetCountOfType (COverlayType *pType);
		ICCItemPtr GetData (DWORD dwID, const CString &sAttrib) const;
		bool GetImpact (CSpaceObject *pSource, COverlay::SImpactDesc &Impact) const;
		void GetList (TArray<COverlay *> *retList);
		void GetListOfCommandPaneCounters (TArray<COverlay *> *retList);
		COverlay *GetOverlay (DWORD dwID) const;
		CVector GetPos (CSpaceObject *pSource, DWORD dwID);
		ICCItem *GetProperty (CCodeChainCtx *pCCCtx, CSpaceObject *pSource, DWORD dwID, const CString &sName) const;
		int GetRotation (DWORD dwID);
		COverlayType *GetType(DWORD dwID);
		int GetWeaponBonus (CInstalledDevice *pDevice, CSpaceObject *pSource);
        ICCItemPtr IncData (DWORD dwID, const CString &sAttrib, ICCItem *pValue = NULL);
		bool IncProperty (CSpaceObject &SourceObj, DWORD dwID, const CString &sProperty, ICCItem *pInc, ICCItemPtr &pResult);
		bool IsEmpty (void) { return (m_pFirst == NULL); }
		void OnNewSystem (CSpaceObject *pSource, CSystem *pSystem) { m_Conditions = CalcConditions(pSource); }
		void Paint (CG32bitImage &Dest, int iScale, int x, int y, SViewportPaintCtx &Ctx);
		void PaintAnnotations (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void PaintBackground (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void PaintLRSAnnotations (const ViewportTransform &Trans, CG32bitImage &Dest, int x, int y);
		void PaintMapAnnotations (CMapViewportCtx &Ctx, CG32bitImage &Dest, int x, int y);
		void ReadFromStream (SLoadCtx &Ctx, CSpaceObject *pSource);
		void RemoveField (CSpaceObject *pSource, DWORD dwID);
		void ScrapeHarmfulOverlays (CSpaceObject *pSource, int iMaxRemoved = 1);
		void SetData (DWORD dwID, const CString &sAttrib, ICCItem *pData);
		bool SetEffectProperty (DWORD dwID, const CString &sProperty, ICCItem *pValue);
		void SetPos (CSpaceObject *pSource, DWORD dwID, const CVector &vPos);
		bool SetProperty (CSpaceObject *pSource, DWORD dwID, const CString &sName, ICCItem *pValue);
		void SetRotation (DWORD dwID, int iRotation);
		void Update (CSpaceObject *pSource, int iScale, int iRotation, bool *retbModified = NULL);
		void UpdateTimeStopped (CSpaceObject *pSource, int iScale, int iRotation, bool *retbModified = NULL);
		void WriteToStream (IWriteStream *pStream);

	private:
		CConditionSet CalcConditions (CSpaceObject *pSource) const;
		bool DestroyDeleted (void);
		COverlay *FindField (DWORD dwID);
		void OnConditionsChanged (CSpaceObject *pSource);

		COverlay *m_pFirst;
		CConditionSet m_Conditions;			//	Imparted conditions (cached from actual overlays)
	};

