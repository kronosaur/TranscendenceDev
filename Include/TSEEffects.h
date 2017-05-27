//	TSEEffects.h
//
//	Classes and functions for effects
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

struct SEffectHitDesc
	{
	CSpaceObject *pObjHit;						//	Object that was hit by the effect
	CVector vHitPos;							//	Position hit
	int iHitStrength;							//	Number of particles hitting (or 0-100 hit strength)
	};

typedef TArray<SEffectHitDesc> CEffectHitResults;

struct SEffectMoveCtx
	{
	SEffectMoveCtx (void) :
			pObj(NULL),
			bUseOrigin(false)
		{ }

	CSpaceObject *pObj;							//	The object that owns the effect
	CVector vOldPos;							//	Old position of object

	bool bUseOrigin;							//	If TRUE, vOrigin is valid.
	CVector vOrigin;							//	Effect origin
	};

struct SEffectUpdateCtx
	{
	SEffectUpdateCtx (void) : 
			pSystem(NULL),
			pObj(NULL),
			iTick(0),
			iRotation(0),
			bFade(false),

			pDamageDesc(NULL),
			pEnhancements(NULL),
			iCause(killedByDamage),
			bAutomatedWeapon(false),
			pTarget(NULL),

			iTotalParticleCount(1),

			bDestroy(false)
		{ }

	//	Object context
	CSystem *pSystem;							//	Current system
	CSpaceObject *pObj;							//	The object that the effect is part of
	int iTick;									//	Effect tick
	int iRotation;								//	Rotation
	CVector vEmitPos;							//	Emittion pos (if not center of effect)
												//		Relative to center of effect.
	bool bFade;									//	Effect fading

	//	Damage context
	CWeaponFireDesc *pDamageDesc;				//	Damage done by particles (may be NULL)
	CItemEnhancementStack *pEnhancements;		//	Damage enhancements (may be NULL)
	DestructionTypes iCause;					//	Cause of damage
	bool bAutomatedWeapon;						//	TRUE if this is an automated attack
	CDamageSource Attacker;						//	Attacker
	CSpaceObject *pTarget;						//	Target

	//	Particle context
	int iTotalParticleCount;					//	Total particles

	//	Outputs
	CEffectHitResults Hits;						//	Filled in with the objects that hit
	bool bDestroy;								//	Destroy the effect
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
		~CEffectParamDesc (void);

		CEffectParamDesc &operator= (const CEffectParamDesc &Src)
			{ CleanUp(); Copy(Src); return *this; }

		inline ALERROR Bind (SDesignLoadCtx &Ctx) { if (m_iType == typeImage) return m_pImage->OnDesignLoadComplete(Ctx); return NOERROR; }
		CGDraw::EBlendModes EvalBlendMode (CGDraw::EBlendModes iDefault = CGDraw::blendNormal) const;
		bool EvalBool (void) const;
		CG32bitPixel EvalColor (CG32bitPixel rgbDefault = CG32bitPixel::Null()) const;
		DiceRange EvalDiceRange (int iDefault = -1) const;
		int EvalIdentifier (LPSTR *pIDMap, int iMax, int iDefault = 0) const;
		const CObjectImageArray &EvalImage (void) const;
		int EvalInteger (void) const;
		int EvalIntegerBounded (int iMin, int iMax = -1, int iDefault = -1) const;
		CString EvalString (void) const;
		CVector EvalVector (void) const;
		inline EDataTypes GetType (void) const { return m_iType; }
		inline void InitBool (bool bValue) { CleanUp(); m_dwData = (bValue ? 1 : 0); m_iType = typeBoolConstant; }
		inline void InitColor (CG32bitPixel rgbValue) { CleanUp(); m_dwData = rgbValue.AsDWORD(); m_iType = typeColorConstant; }
		inline void InitDiceRange (const DiceRange &Value) { CleanUp(); m_DiceRange = Value; m_iType = typeIntegerDiceRange; }
		inline void InitImage (const CObjectImageArray &Value) { CleanUp(); m_pImage = new CObjectImageArray(Value); m_iType = typeImage; }
		inline void InitInteger (int iValue) { CleanUp(); m_dwData = iValue; m_iType = typeIntegerConstant; }
		inline void InitNull (void) { CleanUp(); }
		inline void InitString (const CString &sValue) { CleanUp(); m_sData = sValue; m_iType = typeStringConstant; }
		inline void InitVector (const CVector &vValue) { CleanUp(); m_pVector = new CVector(vValue); m_iType = typeVectorConstant; }
		ALERROR InitBlendModeFromXML (SDesignLoadCtx &Ctx, const CString &sValue);
		ALERROR InitColorFromXML (SDesignLoadCtx &Ctx, const CString &sValue);
		ALERROR InitIdentifierFromXML (SDesignLoadCtx &Ctx, const CString &sValue, LPSTR *pIDMap);
		ALERROR InitImageFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		ALERROR InitIntegerFromXML (SDesignLoadCtx &Ctx, const CString &sValue);
		ALERROR InitStringFromXML (SDesignLoadCtx &Ctx, const CString &sValue);
		bool IsConstant (void);
		inline bool IsNull (void) const { return (m_iType == typeNull); }
		inline void MarkImage (void) { if (m_iType == typeImage) m_pImage->MarkImage(); }
		void ReadFromStream (SLoadCtx &Ctx);
		void WriteToStream (IWriteStream *pStream);

		static bool FindIdentifier (const CString &sValue, LPSTR *pIDMap, DWORD *retdwID = NULL);

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
		inline void AddParam (const CString &sParam, const CEffectParamDesc &Value) { m_Params.SetAt(sParam, Value); }
		inline bool FindParam (const CString &sParam, CEffectParamDesc *retValue = NULL) const
			{
			const CEffectParamDesc *pValue = m_Params.GetAt(sParam);
			if (pValue == NULL)
				return false;

			if (retValue)
				*retValue = *pValue;

			return true;
			}

		inline const CEffectParamDesc *GetParam (const CString &sParam) const {	return m_Params.GetAt(sParam); }

	private:
		TSortMap<CString, CEffectParamDesc> m_Params;
	};

class CCreatePainterCtx
	{
	public:
		CCreatePainterCtx (void) :
				m_iLifetime(0),
				m_pDamageCtx(NULL),
				m_pWeaponFireDesc(NULL),
				m_bUseObjectCenter(false),
				m_bTracking(false),
				m_bRaw(false),
				m_pData(NULL),
				m_pDefaultParams(NULL),
				m_dwLoadVersion(SYSTEM_SAVE_VERSION)
			{ }

		~CCreatePainterCtx (void);

		void AddDataInteger (const CString &sField, int iValue);
		inline bool FindDefaultParam (const CString &sParam, CEffectParamDesc *retValue) const { return (m_pDefaultParams ? m_pDefaultParams->FindParam(sParam, retValue) : false); }
		inline SDamageCtx *GetDamageCtx (void) const { return m_pDamageCtx; }
		ICCItem *GetData (void);
		inline const CEffectParamDesc *GetDefaultParam (const CString &sParam) const { return (m_pDefaultParams ? m_pDefaultParams->GetParam(sParam) : NULL); }
		inline int GetLifetime (void) const { return m_iLifetime; }
		inline DWORD GetLoadVersion (void) const { return m_dwLoadVersion; }
		inline bool IsRawPainter (void) const { return m_bRaw; }
		inline bool IsTracking (void) const { return m_bTracking; }
		inline void SetDamageCtx (SDamageCtx &Ctx) { m_pDamageCtx = &Ctx; }
		void SetDefaultParam (const CString &sParam, const CEffectParamDesc &Value);
		inline void SetLifetime (int iLifetime) { m_iLifetime = iLifetime; }
		inline void SetLoadVersion (DWORD dwVersion) { m_dwLoadVersion = dwVersion; }
		inline void SetRawPainter (bool bValue = true) { m_bRaw = bValue; }
		inline void SetTrackingObject (bool bValue = true) { m_bTracking = bValue; }
		inline void SetUseObjectCenter (bool bValue = true) { m_bUseObjectCenter = bValue; }
		inline void SetWeaponFireDesc (CWeaponFireDesc *pDesc) { m_pWeaponFireDesc = pDesc; }
		inline bool UseObjectCenter (void) const { return m_bUseObjectCenter; }

	private:
		struct SDataEntry
			{
			CString sField;
			int iValue;
			};

		void SetDamageCtxData (CCodeChain &CC, CCSymbolTable *pTable, SDamageCtx &DamageCtx);
		void SetWeaponFireDescData (CCodeChain &CC, CCSymbolTable *pTable, CWeaponFireDesc *pDesc);

		int m_iLifetime;						//	Optional lifetime 0 = use creator defaults; -1 = infinite;
		SDamageCtx *m_pDamageCtx;				//	Optional damage context
		CWeaponFireDesc *m_pWeaponFireDesc;		//	Optional weapon fire desc
		TArray<SDataEntry> m_Data;				//	Data to add
		CEffectParamSet *m_pDefaultParams;		//	Default parameters (owned by us)
		DWORD m_dwLoadVersion;					//	Optional system version at load time

		bool m_bUseObjectCenter;				//	If TRUE, particle clouds always use the object as center
		bool m_bTracking;						//	If TRUE, object sets velocity
		bool m_bRaw;							//	We want a raw painter (default parameters).

		ICCItem *m_pData;						//	Generated data
	};

class IEffectPainter
	{
	public:
		IEffectPainter (bool bSingleton = false) : m_bSingleton(bSingleton),
				m_bNoSound(false)
			{ }

		void GetBounds (RECT *retRect);
		void GetBounds (const CVector &vPos, CVector *retvUR, CVector *retvLL);
		inline bool IsSingleton (void) const { return m_bSingleton; }
		inline void OnUpdate (void) { SEffectUpdateCtx Ctx; OnUpdate(Ctx); }
		inline void PlaySound (CSpaceObject *pSource);
		inline void ReadFromStream (SLoadCtx &Ctx) { OnReadFromStream(Ctx); }
		static CString ReadUNID (SLoadCtx &Ctx);
		inline void SetNoSound (bool bNoSound = true) { m_bNoSound = bNoSound; }
		inline void SetParam (CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value)
			{
			//	If we don't have a value, see if there is a default.

			const CEffectParamDesc *pDefaultValue;
			if (Value.IsNull() && (pDefaultValue = Ctx.GetDefaultParam(sParam)))
				OnSetParam(Ctx, sParam, *pDefaultValue);

			//	Otherwise, just set it (null or not)

			else
				OnSetParam(Ctx, sParam, Value);
			}

		void SetParamFromItem (CCreatePainterCtx &Ctx, const CString &sParam, ICCItem *pValue);
		inline void SetSingleton (bool bSingleton = true) { m_bSingleton = bSingleton; }
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
		virtual void GetParam (const CString &sParam, CEffectParamDesc *retValue) { retValue->InitNull(); }
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
		virtual void SetParamStruct (CCreatePainterCtx &Ctx, const CString &sParam, ICCItem *pValue) { }
		virtual bool SetProperty (const CString &sProperty, ICCItem *pValue) { return false; }
		virtual void SetPos (const CVector &vPos) { }
		virtual void SetVariants (int iVariants) { }
		virtual bool UsesOrigin (void) const { return false; }

	protected:
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual void OnSetParam (CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value) { }
		virtual void OnWriteToStream (IWriteStream *pStream);

	private:
		int GetInitialLifetime (void);

		bool m_bSingleton;
		bool m_bNoSound;
	};

class CEffectPainterRef
	{
	public:
		CEffectPainterRef (void) :
				m_pPainter(NULL)
			{ }

		~CEffectPainterRef (void) { Delete(); }

		inline operator IEffectPainter *() const { return m_pPainter; }
		inline IEffectPainter * operator->() const { return m_pPainter; }

		void Delete (void)
			{
			if (m_pPainter && m_bDelete)
				m_pPainter->Delete();

			m_pPainter = NULL;
			}

		inline bool IsEmpty (void) const { return (m_pPainter == NULL); }

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
		IEffectPainter *m_pPainter;
		bool m_bDelete;
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
		inline IEffectPainter *CreatePainter (CCreatePainterCtx &Ctx, int iIndex) { return m_Effects[iIndex].pEffect.CreatePainter(Ctx); }
		CEffectCreator *FindEffectCreator (const CString &sUNID) const;
		inline int GetEffectCount (void) const { return m_Effects.GetCount(); }
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
