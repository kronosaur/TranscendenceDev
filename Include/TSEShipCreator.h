//	TSEShipCreator.h
//
//	Defines classes and interfaces for creating ships
//	Copyright (c) 2016 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

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
		};
	};

class IShipGenerator
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, IShipGenerator **retpGenerator);
		static ALERROR CreateFromXMLAsGroup (SDesignLoadCtx &Ctx, CXMLElement *pDesc, IShipGenerator **retpGenerator);

		virtual ~IShipGenerator (void) { }
		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) { }
		virtual void CreateShips (SShipCreateCtx &Ctx) { }
		virtual Metric GetAverageLevelStrength (int iLevel) { return 0.0; }
		virtual ALERROR LoadFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) { return NOERROR; }
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
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override { if (m_pGenerator) m_pGenerator->AddTypesUsed(retTypesUsed); }
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;

	private:
		IShipGenerator *m_pGenerator;
	};

