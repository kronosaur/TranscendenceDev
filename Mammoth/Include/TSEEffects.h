//	TSEEffects.h
//
//	Classes and functions for effects
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CUniverse;

struct SEffectHitDesc
	{
	CSpaceObject *pObjHit = NULL;				//	Object that was hit by the effect
	CVector vHitPos;							//	Position hit
	int iHitStrength = 0;						//	Number of particles hitting (or 0-100 hit strength)
	};

typedef TArray<SEffectHitDesc> CEffectHitResults;

struct SEffectMoveCtx
	{
	CSpaceObject *pObj = NULL;					//	The object that owns the effect
	CVector vOldPos;							//	Old position of object

	bool bUseOrigin = false;					//	If TRUE, vOrigin is valid.
	CVector vOrigin;							//	Effect origin
	};

struct SEffectUpdateCtx
	{
	SEffectUpdateCtx (CUniverse &UniverseArg) :
			Universe(UniverseArg)
		{ }

	CUniverse &Universe;

	//	Object context
	CSystem *pSystem = NULL;					//	Current system
	CSpaceObject *pObj = NULL;					//	The object that the effect is part of
	int iTick = 0;								//	Effect tick
	int iRotation = 0;							//	Rotation
	CVector vEmitPos;							//	Emittion pos (if not center of effect)
												//		Relative to center of effect.
	bool bFade = false;							//	Effect fading

	bool bUseOrigin = false;					//	If TRUE, vOrigin is valid.
	CVector vOrigin;							//	Effect origin

	//	Damage context
	CWeaponFireDesc *pDamageDesc = NULL;		//	Damage done by particles (may be NULL)
	CItemEnhancementStack *pEnhancements = NULL;	//	Damage enhancements (may be NULL)
	DestructionTypes iCause = killedByDamage;	//	Cause of damage
	CDamageSource Attacker;						//	Attacker
	CSpaceObject *pTarget = NULL;				//	Target

	//	Particle context
	int iTotalParticleCount = 1;				//	Total particles

	//	Outputs
	CEffectHitResults Hits;						//	Filled in with the objects that hit
	bool bDestroy = false;						//	Destroy the effect
	};

class CEffectParamDesc
	{
	public:
		enum EDataTypes
			{
			typeNull =						0,

			typeColorConstant =				1,
			typeIntegerConstant =			2,
			typeIntegerDiceRange =			3,
			typeIntegerExpression =			4,
			typeStringConstant =			5,
			typeBoolConstant =				6,
			typeVectorConstant =			7,
			typeImage =						8,
			};

		CEffectParamDesc (void) : m_iType(typeNull)
			{ }

		CEffectParamDesc (const CEffectParamDesc &Src)
			{ Copy(Src); }

		CEffectParamDesc (int iValue) : m_iType(typeIntegerConstant), m_dwData(iValue) { }
		CEffectParamDesc (bool bValue) : m_iType(typeBoolConstant), m_dwData(bValue ? 1 : 0) { }
		CEffectParamDesc (EDataTypes iType, int iValue);
		CEffectParamDesc (ICCItem *pValue);
		explicit CEffectParamDesc (const CString &sValue) { m_sData = sValue; m_iType = typeStringConstant; }

		~CEffectParamDesc (void);

		CEffectParamDesc &operator= (const CEffectParamDesc &Src)
			{ CleanUp(); Copy(Src); return *this; }

		ICCItemPtr AsItem (void) const;
		ALERROR Bind (SDesignLoadCtx &Ctx) { if (m_iType == typeImage) return m_pImage->OnDesignLoadComplete(Ctx); return NOERROR; }
		CGDraw::EBlendModes EvalBlendMode (CGDraw::EBlendModes iDefault = CGDraw::blendNormal) const;
		bool EvalBool (void) const;
		CG32bitPixel EvalColor (CG32bitPixel rgbDefault = CG32bitPixel::Null()) const;
		DiceRange EvalDiceRange (int iDefaultCount, int iDefaultSides, int iDefaultBonus) const;
		DiceRange EvalDiceRange (int iDefault = -1) const { return EvalDiceRange(0, 0, iDefault); }
		int EvalIdentifier (LPCSTR *pIDMap, int iMax, int iDefault = 0) const;
		const CObjectImageArray &EvalImage (void) const;
		int EvalInteger (void) const;
		int EvalIntegerBounded (int iMin, int iMax = -1, int iDefault = -1) const;
		BYTE EvalOpacity (BYTE byDefault = 255) const;
		CString EvalString (void) const;
		CVector EvalVector (void) const;
		EDataTypes GetType (void) const { return m_iType; }
		void InitBool (bool bValue) { CleanUp(); m_dwData = (bValue ? 1 : 0); m_iType = typeBoolConstant; }
		void InitColor (CG32bitPixel rgbValue) { CleanUp(); m_dwData = rgbValue.AsDWORD(); m_iType = typeColorConstant; }
		void InitDiceRange (const DiceRange &Value) { CleanUp(); m_DiceRange = Value; m_iType = typeIntegerDiceRange; }
		void InitImage (const CObjectImageArray &Value) { CleanUp(); m_pImage = new CObjectImageArray(Value); m_iType = typeImage; }
		void InitInteger (int iValue) { CleanUp(); m_dwData = iValue; m_iType = typeIntegerConstant; }
		void InitNull (void) { CleanUp(); }
		void InitString (const CString &sValue) { CleanUp(); m_sData = sValue; m_iType = typeStringConstant; }
		void InitVector (const CVector &vValue) { CleanUp(); m_pVector = new CVector(vValue); m_iType = typeVectorConstant; }
		ALERROR InitBlendModeFromXML (SDesignLoadCtx &Ctx, const CString &sValue);
		ALERROR InitColorFromXML (SDesignLoadCtx &Ctx, const CString &sValue);
		ALERROR InitIdentifierFromXML (SDesignLoadCtx &Ctx, const CString &sValue, LPCSTR *pIDMap);
		ALERROR InitImageFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		ALERROR InitIntegerFromXML (SDesignLoadCtx &Ctx, const CString &sValue);
		ALERROR InitStringFromXML (SDesignLoadCtx &Ctx, const CString &sValue);
		bool IsConstant (void);
		bool IsNull (void) const { return (m_iType == typeNull); }
		void MarkImage (void) { if (m_iType == typeImage) m_pImage->MarkImage(); }
		void ReadFromStream (SLoadCtx &Ctx);
		void WriteToStream (IWriteStream *pStream);

		static bool FindIdentifier (const CString &sValue, LPCSTR *pIDMap, DWORD *retdwID = NULL);

	private:
		void CleanUp (void);
		void Copy (const CEffectParamDesc &Src);

		EDataTypes m_iType;

		CString m_sData;
		DiceRange m_DiceRange;

		union
			{
			DWORD m_dwData;
			CObjectImageArray *m_pImage;
			ICCItem *m_pItem;
			CVector *m_pVector;
			};
	};

class CEffectParamSet
	{
	public:
		void AddParam (const CString &sParam, const CEffectParamDesc &Value) { m_Params.SetAt(sParam, Value); }
		bool FindParam (const CString &sParam, CEffectParamDesc *retValue = NULL) const
			{
			const CEffectParamDesc *pValue = m_Params.GetAt(sParam);
			if (pValue == NULL)
				return false;

			if (retValue)
				*retValue = *pValue;

			return true;
			}

		const CEffectParamDesc *GetParam (const CString &sParam) const {	return m_Params.GetAt(sParam); }

	private:
		TSortMap<CString, CEffectParamDesc> m_Params;
	};

class CCreatePainterCtx
	{
	public:
		void AddDataInteger (const CString &sField, int iValue);
		bool CanCreateSingleton (void) const { return !m_bNoSingleton; }
		bool FindDefaultParam (const CString &sParam, CEffectParamDesc *retValue) const { return (m_pDefaultParams ? m_pDefaultParams->FindParam(sParam, retValue) : false); }
		CSpaceObject *GetAnchor (void) const { return m_pAnchor; }
		DWORD GetAPIVersion (void) const { return m_dwAPIVersion; }
		SDamageCtx *GetDamageCtx (void) const { return m_pDamageCtx; }
		ICCItem *GetData (void);
		const CEffectParamDesc *GetDefaultParam (const CString &sParam) const { return (m_pDefaultParams ? m_pDefaultParams->GetParam(sParam) : NULL); }
		int GetLifetime (void) const { return m_iLifetime; }
		DWORD GetLoadVersion (void) const { return m_dwLoadVersion; }
		ICCItemPtr GetParams (void) const { return m_pParams; }
		const CVector &GetPos (void) const { return m_vPos; }
		bool IsRawPainter (void) const { return m_bRaw; }
		bool IsTracking (void) const { return m_bTracking; }
		void SetAnchor (CSpaceObject *pAnchor) { m_pAnchor = pAnchor; }
		void SetAPIVersion (DWORD dwVersion) { m_dwAPIVersion = dwVersion; }
		void SetDamageCtx (SDamageCtx &Ctx) { m_pDamageCtx = &Ctx; }
		void SetDefaultParam (const CString &sParam, const CEffectParamDesc &Value);
		void SetLifetime (int iLifetime) { m_iLifetime = iLifetime; }
		void SetLoadVersion (DWORD dwVersion) { m_dwLoadVersion = dwVersion; }
		void SetNoSingleton (bool bValue = true) { m_bNoSingleton = bValue; }
		void SetOverlay (const COverlay &Overlay) { m_pOverlay = &Overlay; }
		void SetParams (ICCItem *pParams) { m_pParams = pParams; }
		void SetPos (const CVector &vPos) { m_vPos = vPos; }
		void SetRawPainter (bool bValue = true) { m_bRaw = bValue; }
		void SetTrackingObject (bool bValue = true) { m_bTracking = bValue; }
		void SetUseObjectCenter (bool bValue = true) { m_bUseObjectCenter = bValue; }
		void SetWeaponFireDesc (CWeaponFireDesc *pDesc);
		bool UseObjectCenter (void) const { return m_bUseObjectCenter; }

	private:
		struct SDataEntry
			{
			CString sField;
			int iValue;
			};

		void SetDamageCtxData (ICCItem *pTable, SDamageCtx &DamageCtx) const;
		void SetOverlayData (ICCItem *pTable, const COverlay &Overlay) const;
		void SetWeaponFireDescData (ICCItem *pTable, CWeaponFireDesc *pDesc) const;

		CSpaceObject *m_pAnchor = NULL;					//	Optional anchor (e.g., for effects that need an object to operate)
		CVector m_vPos;									//	Optional position of effect
		int m_iLifetime = 0;							//	Optional lifetime 0 = use creator defaults; -1 = infinite;
		const CSpaceObject *m_pSource = NULL;			//	Optional object associated with effect
		const COverlay *m_pOverlay = NULL;				//	Optional overlay that created the effect
		SDamageCtx *m_pDamageCtx = NULL;				//	Optional damage context
		CWeaponFireDesc *m_pWeaponFireDesc = NULL;		//	Optional weapon fire desc
		TArray<SDataEntry> m_Data;						//	Data to add
		TUniquePtr<CEffectParamSet> m_pDefaultParams;	//	Default parameters (owned by us)
		DWORD m_dwLoadVersion = SYSTEM_SAVE_VERSION;	//	Optional system version at load time
		DWORD m_dwAPIVersion = API_VERSION;				//	API version of creator

		bool m_bUseObjectCenter = false;				//	If TRUE, particle clouds always use the object as center
		bool m_bTracking = false;						//	If TRUE, object sets velocity
		bool m_bRaw = false;							//	We want a raw painter (default parameters).
		bool m_bNoSingleton = false;					//	Always create a new object

		ICCItemPtr m_pData;								//	Generated data (for <GetParameters> event)
		ICCItemPtr m_pParams;							//	Parameters (if set by sysCreateEffect).
	};

class IEffectPainter
	{
	public:
		IEffectPainter (bool bSingleton = false) : m_bSingleton(bSingleton),
				m_bNoSound(false)
			{ }

		void GetBounds (RECT *retRect);
		void GetBounds (const CVector &vPos, CVector *retvUR, CVector *retvLL);
		bool IsSingleton (void) const { return m_bSingleton; }
		inline void PlaySound (CSpaceObject *pSource);
		void ReadFromStream (SLoadCtx &Ctx) { OnReadFromStream(Ctx); }
		static CString ReadUNID (SLoadCtx &Ctx);
		void SetNoSound (bool bNoSound = true) { m_bNoSound = bNoSound; }
		bool SetParam (CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value)
			{
			//	If we don't have a value, see if there is a default.

			const CEffectParamDesc *pDefaultValue;
			if (Value.IsNull() && (pDefaultValue = Ctx.GetDefaultParam(sParam)))
				return OnSetParam(Ctx, sParam, *pDefaultValue);

			//	Otherwise, just set it (null or not)

			else
				return OnSetParam(Ctx, sParam, Value);
			}

		bool SetParamFromItem (CCreatePainterCtx &Ctx, const CString &sParam, ICCItem *pValue);
		void SetParamInteger (const CString &sParam, int iValue);
		void SetSingleton (bool bSingleton = true) { m_bSingleton = bSingleton; }
		static ALERROR ValidateClass (SLoadCtx &Ctx, const CString &sOriginalClass);
		void WriteToStream (IWriteStream *pStream);

#ifdef DEBUG_SINGLETON_EFFECTS
		virtual ~IEffectPainter (void)
			{
			if (m_bSingleton)
				{
				::kernelDebugLogPattern("Delete singleton painter: %08x", (DWORD)this);
				}
			}
#else
		virtual ~IEffectPainter (void) { }
#endif
		virtual bool CanPaintComposite (void) { return false; }
		virtual void Delete (void) { if (!m_bSingleton) delete this; }
		virtual CEffectCreator *GetCreator (void) = 0;
		virtual int GetFadeLifetime (bool bHit) const { return 0; }
		virtual int GetLifetime (void) { return GetInitialLifetime(); }
		virtual bool GetParam (const CString &sParam, CEffectParamDesc *retValue) const { return false; }
		virtual bool GetParamList (TArray<CString> *retList) const { return false; }
		virtual int GetParticleCount (void) { return 1; }
		virtual bool GetParticlePaintDesc (SParticlePaintDesc *retDesc) { return false; }
		virtual Metric GetRadius (int iTick) const;
		virtual void GetRect (RECT *retRect) const;
		virtual int GetVariants (void) const { return 1; }
		virtual void OnBeginFade (void) { }
		virtual void OnMove (SEffectMoveCtx &Ctx, bool *retbBoundsChanged = NULL) { if (retbBoundsChanged) *retbBoundsChanged = false; }
		virtual void OnUpdate (SEffectUpdateCtx &Ctx) { }
		virtual void Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) = 0;
		virtual void PaintComposite (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) { Paint(Dest, x, y, Ctx); }
		virtual void PaintFade (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) { }
		virtual void PaintHit (CG32bitImage &Dest, int x, int y, const CVector &vHitPos, SViewportPaintCtx &Ctx) { }
		virtual void PaintLine (CG32bitImage &Dest, const CVector &vHead, const CVector &vTail, SViewportPaintCtx &Ctx);
		virtual bool PointInImage (int x, int y, int iTick, int iVariant = 0, int iRotation = 0) const { return false; }
		virtual bool SetParamString (const CString &sParam, const CString &sValue) { return false; }
		virtual bool SetParamStruct (CCreatePainterCtx &Ctx, const CString &sParam, ICCItem *pValue) { return false; }
		virtual bool SetProperty (const CString &sProperty, ICCItem *pValue) { return false; }
		virtual void SetPos (const CVector &vPos) { }
		virtual void SetVariants (int iVariants) { }
		virtual bool UsesOrigin (void) const { return false; }

	protected:
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual bool OnSetParam (CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value) { return false; }
		virtual void OnWriteToStream (IWriteStream *pStream);

		CUniverse &PainterGetUniverse (void) const { return *g_pUniverse; }

	private:
		int GetInitialLifetime (void);

		bool m_bSingleton;
		bool m_bNoSound;
	};

class CEffectPainterRef
	{
	public:
		~CEffectPainterRef (void) { Delete(); }

		operator IEffectPainter *() const { return m_pPainter; }
		IEffectPainter * operator->() const { return m_pPainter; }

		void Delete (void)
			{
			if (m_pPainter && m_bDelete)
				m_pPainter->Delete();

			m_pPainter = NULL;
			}

		bool IsEmpty (void) const { return (m_pPainter == NULL); }

		void Set (IEffectPainter *pPainter)
			{
			Delete();
			if (pPainter)
				{
				m_pPainter = pPainter;
				m_bDelete = !pPainter->IsSingleton();
				}
			}

	private:
		IEffectPainter *m_pPainter = NULL;
		bool m_bDelete = false;
	};

class CObjectEffectDesc
	{
	public:
		enum ETypes
			{
			effectNone =				0x00000000,	//	No effect

			effectThrustLeft =			0x00000001,	//	Left (counter-clockwise) thrusters
			effectThrustRight =			0x00000002,	//	Right (clockwise) thrusters
			effectThrustMain =			0x00000004,	//	Forward thrust
			effectThrustStop =			0x00000008,	//	Stop thrusters
			effectWeaponFire =			0x00000010,	//	Weapon flash
			};

		struct SEffectDesc
			{
			ETypes iType;					//	Type of effect
			CEffectCreatorRef pEffect;		//	Effect type
			C3DConversion PosCalc;			//	Position of effect
			int iRotation;					//	Direction of effect
			};

		ALERROR Bind (SDesignLoadCtx &Ctx, const CObjectImageArray &Image);
		IEffectPainter *CreatePainter (CCreatePainterCtx &Ctx, int iIndex) const { return m_Effects[iIndex].pEffect.CreatePainter(Ctx); }
		CEffectCreator *FindEffectCreator (const CString &sUNID) const;
		int GetEffectCount (void) const { return m_Effects.GetCount(); }
		int GetEffectCount (DWORD dwEffects) const;
		const SEffectDesc &GetEffectDesc (int iIndex) const { return m_Effects[iIndex]; }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, const CString &sUNID, CXMLElement *pDesc);
		void MarkImages (void);

		static bool IsManeuverEffect (const SEffectDesc &Desc) { return (((DWORD)Desc.iType & (effectThrustLeft | effectThrustRight | effectThrustStop)) ? true : false); }

	private:
		TArray<SEffectDesc> m_Effects;
	};

class CObjectEffectList
	{
	public:
		~CObjectEffectList (void);

		void AccumulateBounds (CSpaceObject *pObj, const CObjectEffectDesc &Desc, int iRotation, RECT *ioBounds);
		void Init (const CObjectEffectDesc &Desc, const TArray<IEffectPainter *> &Painters);
		void Move (CSpaceObject *pObj, const CVector &vOldPos, bool *retbBoundsChanged = NULL);
		void Paint (SViewportPaintCtx &Ctx, const CObjectEffectDesc &Desc, DWORD dwEffects, CG32bitImage &Dest, int x, int y);
		void PaintAll (SViewportPaintCtx &Ctx, const CObjectEffectDesc &Desc, CG32bitImage &Dest, int x, int y);
		void Update (CSpaceObject *pObj, const CObjectEffectDesc &Desc, int iRotation, DWORD dwEffects);

	private:
		struct SFixedEffect
			{
			SFixedEffect (void) :
					pPainter(NULL)
				{ }

			IEffectPainter *pPainter;
			};

		void CleanUp (void);

		TArray<SFixedEffect> m_FixedEffects;
	};

//	CEffectCreator ------------------------------------------------------------

class CEffectCreator : public CDesignType
	{
	public:
		enum ECachedHandlers
			{
			evtGetParameters			= 0,

			evtCount					= 1,
			};

		enum EInstanceTypes
			{
			instGame,
			instOwner,
			instCreator,
			};

		CEffectCreator (void);
		virtual ~CEffectCreator (void);

		static ALERROR CreateBeamEffect (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID, CEffectCreator **retpCreator);
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID, CEffectCreator **retpCreator);
		static ALERROR CreateSimpleFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID, CEffectCreator **retpCreator);
		static ALERROR CreateTypeFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CEffectCreator **retpCreator);
		static ALERROR CreateFromTag (const CString &sTag, CEffectCreator **retpCreator);
		static IEffectPainter *CreatePainterFromStream (SLoadCtx &Ctx, bool bNullCreator = false);
		static IEffectPainter *CreatePainterFromStreamAndCreator (SLoadCtx &Ctx, CEffectCreator *pCreator);
		static IEffectPainter *CreatePainterFromTag (const CString &sTag);
		static CEffectCreator *FindEffectCreator (const CString &sUNID);
		static void WritePainterToStream (IWriteStream *pStream, IEffectPainter *pPainter);

		IEffectPainter *CreatePainter (CCreatePainterCtx &Ctx);
		bool FindEventHandlerEffectType (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const 
			{
			if (!m_CachedEvents[iEvent].pCode)
				return false;

			if (retEvent) *retEvent = m_CachedEvents[iEvent];
			return true;
			}

		CWeaponFireDesc *GetDamageDesc (void) { return m_pDamage; }
		EInstanceTypes GetInstance (void) const { return m_iInstance; }
		const CString &GetUNIDString (void) { return m_sUNID; }
		bool IsLooping (void) const { return m_bLoop; }
		bool IsValidUNID (void);
		void PlaySound (CSpaceObject *pSource = NULL);
		void SetLooping (bool bLoop = true) { m_bLoop = bLoop; }

		//	Virtuals

		virtual ALERROR CreateEffect (CSystem *pSystem,
									  CSpaceObject *pAnchor,
									  const CVector &vPos,
									  const CVector &vVel,
									  int iRotation,
									  int iVariant = 0,
									  ICCItem *pData = NULL,
									  CSpaceObject **retpEffect = NULL);
		virtual int GetLifetime (void) { return 0; }
		virtual CEffectCreator *GetSubEffect (int iIndex) { return NULL; }
		virtual CString GetTag (void) = 0;
		virtual void SetLifetime (int iLifetime) { }
		virtual void SetVariants (int iVariants) { }

		//	CDesignType overrides
		static CEffectCreator *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designEffectType) ? (CEffectCreator *)pType : NULL); }
		virtual DesignTypes GetType (void) const override { return designEffectType; }

	protected:

		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) override;
		virtual bool OnFindEventHandler (const CString &sEvent, SEventHandlerDesc *retEvent = NULL) const override;
		virtual void OnMarkImages (void) override { m_Sound.Mark(); OnEffectMarkResources(); }

		//	CEffectCreator virtuals

		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) = 0;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) { return NOERROR; }
		virtual ALERROR OnEffectBindDesign (SDesignLoadCtx &Ctx) { return NOERROR; }
		virtual void OnEffectMarkResources (void) { }
		virtual void OnEffectPlaySound (CSpaceObject *pSource);

		void InitPainterParameters (CCreatePainterCtx &Ctx, IEffectPainter *pPainter);

	private:
		ALERROR InitBasicsFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		ALERROR InitInstanceFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);

		CString m_sUNID;
		CSoundRef m_Sound;
		EInstanceTypes m_iInstance = instCreator;
		bool m_bLoop = false;

		CWeaponFireDesc *m_pDamage = NULL;

		//	Events
		CEventHandler m_Events;				//	Local events
		SEventHandlerDesc m_CachedEvents[evtCount];
	};

