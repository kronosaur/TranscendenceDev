//	TSESources.h
//
//	Defines classes and interfaces for sounds and music
//	Copyright (c) 2016 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	Sounds

class CSoundResource : public CDesignType
	{
	public:
		CSoundResource (void);

		int GetSound (void) const;
        inline bool IsMarked (void) const { return m_bMarked; }
		inline void Mark (void) { GetSound(); m_bMarked = true; }

		//	CDesignType overrides

		static CSoundResource *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designSound) ? (CSoundResource *)pType : NULL); }
		virtual DesignTypes GetType (void) const override { return designSound; }

	protected:

		//	CDesignType overrides

		virtual void OnClearMark (void) override { m_bMarked = false; }
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual void OnSweep (void) override;
		virtual void OnUnbindDesign (void) override;

	private:
		void LoadResource (void) const;
		void UnloadResource (void);

		CString m_sResourceDb;			//	Resource db
		CString m_sFilename;			//	Resource name

		mutable int m_iChannel;			//	Loaded in the given channel (-1 if not loaded)
		bool m_bMarked;					//	Marked
	};

class CSoundRef
	{
	public:
		CSoundRef (void) : m_dwUNID(0), m_pSound(NULL)
			{ }

		ALERROR Bind (SDesignLoadCtx &Ctx);
		DWORD GetUNID (void) const { return m_dwUNID; }
		int GetSound (void) const { return (m_pSound ? m_pSound->GetSound() : -1); }
		inline bool IsNull (void) const { return (m_dwUNID == 0); }
		ALERROR LoadUNID (SDesignLoadCtx &Ctx, const CString &sAttrib);
		inline void Mark (void) { if (m_pSound) m_pSound->Mark(); }
		void PlaySound (CSpaceObject *pSource);

	private:
		DWORD m_dwUNID;
		CSoundResource *m_pSound;
	};

class CMusicResource : public CDesignType
	{
	public:
		CMusicResource (void) : m_iNextSegment(0)
			{ }

		~CMusicResource (void) { }

		int FindSegment (int iPos);
		const CString &GetAlbum (void) const;
		inline const CString &GetComposedBy (void) const { return m_sComposedBy; }
		CString GetFilename (void) const { return m_sFilename; }
		CString GetFilespec (void) const;
		inline const CAffinityCriteria &GetLocationCriteria (void) const { return m_LocationCriteria; }
		int GetNextFadePos (int iPos);
		int GetNextPlayPos (void);
		inline const CString &GetPerformedBy (void) const { return m_sPerformedBy; }
		inline int GetPriority (void) const { return m_iPriority; }
		inline int GetSegmentCount (void) const { return (m_Segments.GetCount() == 0 ? 1 : m_Segments.GetCount()); }
		inline const CString &GetTitle (void) const { return m_sTitle; }
		inline void Init (DWORD dwUNID, const CString &sFilespec, int iPriority = 0) { SetUNID(dwUNID); m_sFilespec = sFilespec; m_iPriority = iPriority; }
		void SetLastPlayPos (int iPos);

		//	CDesignType overrides
		static CMusicResource *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designMusic) ? (CMusicResource *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual DesignTypes GetType (void) const override { return designMusic; }

	protected:
		//	CDesignType overrides
		virtual void OnAddExternals (TArray<CString> *retExternals) override { if (!m_sFilespec.IsBlank()) retExternals->Insert(m_sFilespec); }
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;

	private:
		struct SSegmentDesc
			{
			int iStartPos;
			int iEndPos;				//	-1 = end of track
			};

		CString m_sResourceDb;			//	Resource db
		CString m_sFilename;			//	Filename
		CString m_sFilespec;			//	Sound resource within db

		CString m_sTitle;
		CString m_sComposedBy;
		CString m_sPerformedBy;
		int m_iPriority;				//	Track priority
		CAffinityCriteria m_LocationCriteria;	//	Play in specific systems

		TArray<SSegmentDesc> m_Segments;
		int m_iNextSegment;				//	Index of last segment played
	};
