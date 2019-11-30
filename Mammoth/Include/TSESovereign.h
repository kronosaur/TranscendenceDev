//	TSESovereign.h
//
//	Classes and functions for sovereigns.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

enum DispositionClasses
	{
	alignNone =					-1,

	alignConstructiveChaos =	0,
	alignConstructiveOrder =	1,
	alignNeutral =				2,
	alignDestructiveOrder =		3,
	alignDestructiveChaos =		4,
	};

class CSovereign : public CDesignType
	{
	public:
		enum Alignments
			{
			alignUnknown =				-1,

			alignRepublic =				0,	//	constructive	community		knowledge		evolution		(constructive chaos)
			alignFederation =			1,	//	constructive	community		knowledge		tradition		(constructive order)
			alignUplifter =				2,	//	constructive	community		spirituality	evolution		(constructive chaos)
			alignFoundation =			3,	//	constructive	community		spirituality	tradition		(neutral)
			alignCompetitor =			4,	//	constructive	independence	knowledge		evolution		(neutral)
			alignArchivist =			5,	//	constructive	independence	knowledge		tradition		(constructive order)
			alignSeeker =				6,	//	constructive	independence	spirituality	evolution		(constructive chaos)
			alignHermit =				7,	//	constructive	independence	spirituality	tradition		(constructive order)

			alignCollective =			8,	//	destructive		community		knowledge		evolution		(destructive chaos)
			alignEmpire =				9,	//	destructive		community		knowledge		tradition		(destructive order)
			alignSterelizer =			10,	//	destructive		community		spirituality	evolution		(destructive chaos)
			alignCorrector =			11,	//	destructive		community		spirituality	tradition		(destructive order)
			alignMegalomaniac =			12,	//	destructive		independence	knowledge		evolution		(destructive chaos)
			alignCryptologue =			13,	//	destructive		independence	knowledge		tradition		(destructive order)
			alignPerversion =			14,	//	destructive		independence	spirituality	evolution		(destructive chaos)
			alignSolipsist =			15,	//	destructive		independence	spirituality	tradition		(destructive order)

			alignUnorganized =			16,	//	unorganized group of beings										(neutral)
			alignSubsapient =			17,	//	animals, zoanthropes, cyberorgs, and other creatures			(neutral)
			alignPredator =				18,	//	aggressive subsapient											(destructive chaos)

			alignCount =				19,
			};

		enum Disposition
			{
			dispEnemy = 0,
			dispNeutral = 1,
			dispFriend = 2,
			};

		enum EThreatLevels
			{
			threatNone =				0,	//	Player is not a threat to us
			threatMinorPiracy =			1,	//	Player is a minor threat to ships
			threatMinorRaiding =		2,	//	Player is a minor threat to stations
			threatMajor =				3,	//	Player is a major threat
			threatExistential =			4,	//	Player threatens our existence
			};

		CSovereign (void);
		~CSovereign (void);

#ifdef DEBUG_ENEMY_CACHE_BUG
		void DebugObjDeleted (CSpaceObject *pObj) const;
#endif
		void DeleteRelationships (void);
		inline void FlushEnemyObjectCache (void) { m_EnemyObjects.DeleteAll(); m_pEnemyObjectsSystem = NULL; }
		IPlayerController *GetController (void);
		Disposition GetDispositionTowards (const CSovereign *pSovereign, bool bCheckParent = true) const;
		inline const CSpaceObjectList &GetEnemyObjectList (const CSystem *pSystem) { InitEnemyObjectList(pSystem); return m_EnemyObjects; }
		EThreatLevels GetPlayerThreatLevel (void) const;
		bool GetPropertyInteger (const CString &sProperty, int *retiValue);
		bool GetPropertyItemList (const CString &sProperty, CItemList *retItemList);
		bool GetPropertyString (const CString &sProperty, CString *retsValue);
		CString GetText (MessageTypes iMsg);
		inline bool IsEnemy (CSovereign *pSovereign) const { return (m_bSelfRel || (pSovereign != this)) && (GetDispositionTowards(pSovereign) == dispEnemy); }
		inline bool IsFriend (CSovereign *pSovereign) const { return (!m_bSelfRel && (pSovereign == this)) || (GetDispositionTowards(pSovereign) == dispFriend); }
		inline bool IsPlayer (void) const { return (GetUNID() == g_PlayerSovereignUNID); }
		void MessageFromObj (CSpaceObject *pSender, const CString &sText);
		void OnObjDestroyedByPlayer (CSpaceObject *pObj);
		static Alignments ParseAlignment (const CString &sAlign);
		void SetDispositionTowards (CSovereign *pSovereign, Disposition iDisp, bool bMutual = false);
		void SetDispositionTowards (Alignments iAlignment, Disposition iDisp, bool bMutual = false);
		void SetDispositionTowardsFlag (DWORD dwAlignmentFlag, Disposition iDisp, bool bMutual = false);
		bool SetPropertyInteger (const CString &sProperty, int iValue);
		bool SetPropertyItemList (const CString &sProperty, const CItemList &ItemList);
		bool SetPropertyString (const CString &sProperty, const CString &sValue);
		void Update (int iTick, CSystem *pSystem);

		//	CDesignType overrides
		static const CSovereign *AsType (const CDesignType *pType) { return ((pType && pType->GetType() == designSovereign) ? (CSovereign *)pType : NULL); }
		static CSovereign *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designSovereign) ? (CSovereign *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual CString GetNamePattern (DWORD dwNounFormFlags = 0, DWORD *retdwFlags = NULL) const;
		virtual DesignTypes GetType (void) const override { return designSovereign; }
		virtual bool IsVirtual (void) const override { return m_bVirtual; }

		static DWORD ParseAlignmentFlag (const CString &sValue);
		static Disposition ParseDisposition (const CString &sValue);

	protected:
		//	CDesignType overrides
		virtual void OnAccumulateXMLMergeFlags (TSortMap<DWORD, DWORD> &MergeFlags) const override;
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual ICCItemPtr OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const override;
		virtual ALERROR OnPrepareBindDesign (SDesignLoadCtx &Ctx) override;
		virtual void OnPrepareReinit (void) override;
		virtual void OnReadFromStream (SUniverseLoadCtx &Ctx) override;
		virtual void OnReinit (void) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		struct SRelationship
			{
			CSovereign *pSovereign;
			Disposition iDisp;

			SRelationship *pNext;
			};

		bool CalcSelfRel (void);
		const SRelationship *FindRelationship (const CSovereign *pSovereign, bool bCheckParent = false) const;
		SRelationship *FindRelationship (const CSovereign *pSovereign, bool bCheckParent = false);
		inline Alignments GetAlignment (void) const { return m_iAlignment; }
		void InitEnemyObjectList (const CSystem *pSystem) const;
		void InitRelationships (void);

		CString m_sName;						//	":the United States of America"
		CString m_sShortName;					//	":the USA"
		CString m_sAdjective;					//	"American"
		CString m_sDemonym;						//	":an American(s)"
		bool m_bPluralForm;						//	"The United States ARE..."

		Alignments m_iAlignment = alignUnknown;
		CXMLElement *m_pInitialRelationships = NULL;

		SRelationship *m_pFirstRelationship = NULL;		//	List of individual relationships

		int m_iStationsDestroyedByPlayer = 0;			//	Number of our stations destroyed by the player
		int m_iShipsDestroyedByPlayer = 0;				//	Number of our ships destroyed by the player

		bool m_bSelfRel = false;						//	TRUE if relationship with itself is not friendly
		bool m_bVirtual = false;						//	TRUE if this is a virtual sovereign (usually a base class)
		mutable const CSystem *m_pEnemyObjectsSystem = NULL;	//	System that we've cached enemy objects
		mutable CSpaceObjectList m_EnemyObjects;		//	List of enemy objects that can attack
	};

