//	TSEDesignDefs.h
//
//	Transcendence design definitions (used by TSEDesign.h classes).
//	Copyright 2018 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	Dock Screen System ---------------------------------------------------------

class CDockScreenSys
	{
	public:
		struct SSelector
			{
			CString sScreen;						//	Screen to show
			ICCItemPtr pData;						//	Data for screen

			int iPriority = 0;						//	Priority
			bool bOverrideOnly = false;				//	Valid only if we have a screen already
			};
	};

//	Custom Property Definitions ------------------------------------------------

enum class EPropertyType
	{
	propNone,

	propDefinition,				//	Type-based, eval at load-time, constant
	propConstant,				//	Type-based, eval at game-create, constant
	propGlobal,					//	Type-based, eval at game-create, mutable
	propDynamicGlobal,			//	Type-based, eval at get-time
	propVariant,				//	Object-based, eval at object-create, constant
	propData,					//	Object-based, eval at object-create, mutable
	propDynamicData,			//	Object-based, eval at get-time

	propEngine,					//	Property handled by the engine

	propDefinitionEvalNeeded,	//	Same as propDefinition, but needs eval.
	};

class CDesignPropertyDefinitions
	{
	public:
		ALERROR BindDesign (SDesignLoadCtx &Ctx);
		bool Find (const CString &sProperty, ICCItemPtr &pData, EPropertyType *retiType = NULL) const;
		ICCItemPtr GetCode (int iIndex) const { return m_Defs[iIndex].pCode; }
		int GetCount (void) const { return m_Defs.GetCount(); }
		const CString &GetName (int iIndex) const { return m_Defs.GetKey(iIndex); }
		EPropertyType GetType (int iIndex) const { return m_Defs[iIndex].iType; }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc);
		void InitItemData (CUniverse &Universe, CItem &Item) const;
		void InitObjectData (CUniverse &Universe, CSpaceObject &Obj, CAttributeDataBlock &Dest) const;
		void InitTypeData (CUniverse &Universe, CDesignType &Type) const;

	private:
		struct SDef
			{
			EPropertyType iType = EPropertyType::propNone;
			ICCItemPtr pCode;
			};

		TSortMap<CString, SDef> m_Defs;
	};

//	Difficulty Levels ----------------------------------------------------------

class CDifficultyOptions
	{
	public:
		enum ELevels
			{
			lvlUnknown			= -1,

			lvlStory			= 0,
			lvlNormal			= 1,
			lvlChallenge		= 2,
			lvlPermadeath		= 3,

			lvlCount			= 4,
			};

		Metric GetEnemyDamageAdj (void) const { ASSERT(m_iLevel != lvlUnknown); return m_Table[m_iLevel].rEnemyDamageAdj; }
		ELevels GetLevel (void) const { return m_iLevel; }
		Metric GetPlayerDamageAdj (void) const { ASSERT(m_iLevel != lvlUnknown); return m_Table[m_iLevel].rPlayerDamageAdj; }
		void ReadFromStream (IReadStream &Stream);
		bool SaveOnUndock (void) const;
		void SetLevel (ELevels iLevel) { m_iLevel = iLevel; }
		void WriteToStream (IWriteStream &Stream) const;

		static CString GetID (ELevels iLevel);
		static CString GetLabel (ELevels iLevel);
		static ELevels NextLevel (ELevels iLevel) { iLevel = (ELevels)(iLevel + 1); if (iLevel == lvlCount) iLevel = lvlStory; return iLevel; }
		static ELevels ParseID (const CString &sValue);

	private:
		struct SDesc
			{
			LPCSTR pID;
			LPCSTR pName;
			Metric rPlayerDamageAdj;
			Metric rEnemyDamageAdj;
			};

		ELevels m_iLevel = lvlUnknown;

		static const SDesc m_Table[lvlCount];
	};

//	Engine Options -------------------------------------------------------------

class CEngineOptions
	{
	public:
		CEngineOptions (void);

		const CDamageAdjDesc *GetArmorDamageAdj (int iLevel) const { if (iLevel < 1 || iLevel > MAX_ITEM_LEVEL) throw CException(ERR_FAIL); return &m_ArmorDamageAdj[iLevel - 1]; }
		int GetDefaultInteraction (void) const { return m_iDefaultInteraction; }
		int GetDefaultShotHP (void) const { return m_iDefaultShotHP; }
		const CDamageAdjDesc *GetShieldDamageAdj (int iLevel) const { if (iLevel < 1 || iLevel > MAX_ITEM_LEVEL) throw CException(ERR_FAIL); return &m_ShieldDamageAdj[iLevel - 1]; }
		bool InitArmorDamageAdjFromXML (SDesignLoadCtx &Ctx, const CXMLElement &XMLDesc) { m_bCustomArmorDamageAdj = true; return InitDamageAdjFromXML(Ctx, XMLDesc, m_ArmorDamageAdj); }
		bool InitFromProperties (SDesignLoadCtx &Ctx, const CDesignType &Type);
		bool InitShieldDamageAdjFromXML (SDesignLoadCtx &Ctx, const CXMLElement &XMLDesc) { m_bCustomShieldDamageAdj = true; return InitDamageAdjFromXML(Ctx, XMLDesc, m_ShieldDamageAdj); }
		void Merge (const CEngineOptions &Src);

	private:
		bool InitDamageAdjFromXML (SDesignLoadCtx &Ctx, const CXMLElement &XMLDesc, CDamageAdjDesc *DestTable);

		static void InitDefaultDamageAdj (void);

		CDamageAdjDesc m_ArmorDamageAdj[MAX_ITEM_LEVEL];
		CDamageAdjDesc m_ShieldDamageAdj[MAX_ITEM_LEVEL];
		int m_iDefaultInteraction = -1;
		int m_iDefaultShotHP = -1;

		bool m_bCustomArmorDamageAdj = false;
		bool m_bCustomShieldDamageAdj = false;
	};

