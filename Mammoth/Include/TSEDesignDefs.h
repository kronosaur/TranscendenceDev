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
	propItemData,				//	Same as propData, but used by <ItemType> to 
	propObjData,				//		distinguish between item-based data and
								//		shot-based data.

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

		static CString GetPropertyTypeID (EPropertyType iType);

	private:
		struct SDef
			{
			EPropertyType iType = EPropertyType::propNone;
			ICCItemPtr pCode;
			};

		TSortMap<CString, SDef> m_Defs;
		TArray<int> m_InitOrder;
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
		Metric GetScoreAdj (void) const;
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

