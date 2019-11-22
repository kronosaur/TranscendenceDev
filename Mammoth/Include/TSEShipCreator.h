//	TSEShipCreator.h
//
//	Defines classes and interfaces for creating ships
//	Copyright (c) 2016 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CShipChallengeCtx
	{
	public:
		CShipChallengeCtx (void) :
				m_iTotalCount(0),
				m_iTotalScore(0),
				m_rTotalCombat(0.0)
			{ }

		void AddShip (CSpaceObject *pObj);
		void AddShips (CSpaceObjectList &List);
		inline Metric GetTotalCombat (void) const { return m_rTotalCombat; }
		inline int GetTotalCount (void) const { return m_iTotalCount; }
		inline int GetTotalScore (void) const { return m_iTotalScore; }

	private:
		int m_iTotalCount;
		int m_iTotalScore;
		Metric m_rTotalCombat;
	};

class CShipChallengeDesc
	{
	public:
		enum ECountTypes
			{
			countNone,

			countReinforcements,			//	Use m_Count as minimum number
			countScore,						//	Use m_Count as desired total score
			countStanding,					//	Use m_Count as minimum number

			countChallengeEasy,				//	1.5x ships of system level (by score)
			countChallengeStandard,			//	2.5x
			countChallengeHard,				//	4x ships
			countChallengeDeadly,			//	6x ships
			};

		CShipChallengeDesc (void) :
				m_iType(countNone)
			{ }

		inline ECountTypes GetCountType (void) const { return m_iType; }
		bool Init (ECountTypes iType, int iCount = 0);
		bool Init (ECountTypes iType, const CString &sCount);
		bool InitFromChallengeRating (const CString &sChallenge);
		inline bool IsEmpty (void) const { return m_iType == countNone; }
		bool NeedsMoreInitialShips (CSpaceObject *pBase, const CShipChallengeCtx &Ctx) const;
		bool NeedsMoreReinforcements (CSpaceObject *pBase) const;

	private:
		static Metric CalcChallengeStrength (ECountTypes iType, int iLevel);
		static ECountTypes ParseChallengeType (const CString &sValue);

		ECountTypes m_iType;				//	Method for determining ships numbers
		DiceRange m_Count;
	};

//	IShipGenerator

struct SShipCreateCtx
	{
	SShipCreateCtx (void) :
			pSystem(NULL),
			pGate(NULL),
			pBase(NULL),
			pBaseSovereign(NULL),
			pTarget(NULL),
			pEncounterInfo(NULL),
			pOverride(NULL),
			dwFlags(0)
		{ }

	inline CUniverse &GetUniverse (void) const { return *g_pUniverse; }

	CSystem *pSystem;							//	System to create ship in
	CSpaceObject *pGate;						//	Gate where ship will appear (may be NULL)
	CVector vPos;								//	Position where ship will appear (only if pGate is NULL)
	DiceRange PosSpread;						//	Distance from vPos (in light-seconds)
	CSpaceObject *pBase;						//	Base for this ship (may be NULL)
	CSovereign *pBaseSovereign;					//	Only if pBase is NULL
	CSpaceObject *pTarget;						//	Target for ship orders (may be NULL)
	CStationType *pEncounterInfo;				//	Encounter info (may be NULL)
	CDesignType *pOverride;						//	Override to apply to ships (may be NULL)

	DWORD dwFlags;								//	Flags

	CSpaceObjectList Result;					//	List of ships created

	enum Flags
		{
		SHIPS_FOR_STATION =			0x00000001,	//	Create ships at station creation time
		ATTACK_NEAREST_ENEMY =		0x00000002,	//	After all other orders, attack nearest enemy
		RETURN_RESULT =				0x00000004,	//	Add created ships to Result
		RETURN_ESCORTS =			0x00000008,	//	Include escorts in result
		};
	};

class IShipGenerator
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, IShipGenerator **retpGenerator);
		static ALERROR CreateFromXMLAsGroup (SDesignLoadCtx &Ctx, const CXMLElement *pDesc, IShipGenerator **retpGenerator);

		virtual ~IShipGenerator (void) { }
		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) { }
		virtual void CreateShips (SShipCreateCtx &Ctx) { }
		virtual Metric GetAverageLevelStrength (int iLevel) { return 0.0; }
		virtual ALERROR LoadFromXML (SDesignLoadCtx &Ctx, const CXMLElement *pDesc) { return NOERROR; }
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) { return NOERROR; }
		virtual ALERROR ValidateForRandomEncounter (void) { return NOERROR; }
	};

struct SShipGeneratorCtx
	{
	SShipGeneratorCtx (void) :
			pItems(NULL),
			pOnCreate(NULL),
			iOrder(IShipController::orderNone),
			pBase(NULL),
			pTarget(NULL),
			dwCreateFlags(0)
		{ }

	CString sName;								//	If not blank, use as name of ship
	DWORD dwNameFlags;							//	Name flags (only if sName is not blank)
	IItemGenerator *pItems;						//	Items to add to ship (may be NULL)
	CAttributeDataBlock InitialData;			//	Initial data
	ICCItem *pOnCreate;							//	Additional OnCreate code (may be NULL)
	DWORD dwCreateFlags;

	IShipController::OrderTypes iOrder;			//	Order for ship
	IShipController::SData OrderData;			//	Order data

	CSpaceObject *pBase;						//	Base for ship (may be NULL)
	CSpaceObject *pTarget;						//	Target for ship (may be NULL)
	};

//	CShipTable ----------------------------------------------------------------

class CShipTable : public CDesignType
	{
	public:
		CShipTable (void);
		virtual ~CShipTable (void);

		inline void CreateShips (SShipCreateCtx &Ctx) { if (m_pGenerator) m_pGenerator->CreateShips(Ctx); }
		inline Metric GetAverageLevelStrength (int iLevel) { return (m_pGenerator ? m_pGenerator->GetAverageLevelStrength(iLevel) : 0.0); }
		ALERROR ValidateForRandomEncounter (void) { if (m_pGenerator) return m_pGenerator->ValidateForRandomEncounter(); }

		//	CDesignType overrides
		static CShipTable *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designShipTable) ? (CShipTable *)pType : NULL); }
		virtual DesignTypes GetType (void) const override { return designShipTable; }

	protected:
		//	CDesignType overrides
		virtual void OnAccumulateXMLMergeFlags (TSortMap<DWORD, DWORD> &MergeFlags) const override;
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override { if (m_pGenerator) m_pGenerator->AddTypesUsed(retTypesUsed); }
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;

	private:
		IShipGenerator *m_pGenerator;
	};

