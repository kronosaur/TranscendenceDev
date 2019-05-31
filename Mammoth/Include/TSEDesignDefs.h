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
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc);
		void InitItemData (CUniverse &Universe, CItem &Item) const;
		void InitObjectData (CUniverse &Universe, CSpaceObject &Obj, CAttributeDataBlock &Dest) const;
		void InitTypeData (CUniverse &Universe, CAttributeDataBlock &Dest) const;

	private:
		struct SDef
			{
			EPropertyType iType = EPropertyType::propNone;
			ICCItemPtr pCode;
			};

		TSortMap<CString, SDef> m_Defs;
	};
