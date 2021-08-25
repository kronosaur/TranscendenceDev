//	TSEAchievements.h
//
//	Transcendence design definitions (used by TSEDesign.h classes).
//	Copyright 2021 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CAchievementDef
	{
	public:
		CAchievementDef (CDesignType &Type) :
				m_Type(Type)
			{ }

		ALERROR BindDesign (SDesignLoadCtx &Ctx);
		bool CanPost () const;
		bool CanPostToSteam () const;
		const CString &GetID () const { return m_sID; }
		const CObjectImageArray &GetImage () const { return m_Image; }
		CString GetName () const;
		int GetSortOrder () const { return m_iSortOrder; }
		const CString &GetSteamID () const { return m_sSteamID; }
		int GetSteamIDCode () const { return m_iSteamID; }
		const CDesignType &GetType () const { return m_Type; }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &Entry);

	private:
		CDesignType &m_Type;					//	Type defining achievement (for translation)
		CString m_sID;
		int m_iSortOrder = 0;
		CObjectImageArray m_Image;

		int m_iSteamID = 0;
		CString m_sSteamID;
	};

class CAchievementDefinitions
	{
	public:
		ALERROR AddDefinitions (SDesignLoadCtx &Ctx, const CAchievementDataBlock &Def);
		const CAchievementDef *FindDefinition (const CString &sID) const;
		int GetCount () const { return m_List.GetCount(); }
		const CAchievementDef &GetDefinition (int iIndex) const { if (iIndex < 0 || iIndex >= m_List.GetCount()) throw CException(ERR_FAIL); return *m_List[iIndex]; }

	private:
		TSortMap<CString, const CAchievementDef *> m_List;
	};

class CAchievementRecord
	{
	public:
		struct SAchievement
			{
			const CAchievementDef *pDef = NULL;
			CString sName;
			CString sDesc;

			bool bAchievedThisGame = false;
			bool bAchievedPreviously = false;
			};

		const SAchievement &GetAchievement (int iIndex) const { if (iIndex < 0 || iIndex >= m_Achievements.GetCount()) throw CException(ERR_FAIL); return m_Achievements[iIndex]; }
		int GetCount () const { return m_Achievements.GetCount(); }
		void Init (const CAchievementDefinitions &Defs);
		void RegisterAchievement (const CString &sID);

	private:
		TSortMap<CString, SAchievement> m_Achievements;
		TSortMap<CString, CString> m_Sorted;
	};
