//	TSESources.h
//
//	Defines classes and interfaces for sounds and music
//	Copyright (c) 2016 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	Sounds

//	Sound FX Options -----------------------------------------------------------

struct SSoundOptions
	{
	Metric rVolumeMultiplier	= 1.0;	//	Multiplier to sound effect
	Metric rFalloffFactor		= 1.0;	//	Multiplier to sound effect falloff
	Metric rFalloffStart		= 0.0;	//	Distance in ls that falloff starts
	};

class CSoundResource : public CDesignType
	{
	public:
		CSoundResource (void);

		int GetSound (void) const;
        bool IsMarked (void) const { return m_bMarked; }
		void Mark (void) { GetSound(); m_bMarked = true; }

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
		ALERROR Bind (SDesignLoadCtx &Ctx);
		DWORD GetUNID (void) const { return m_dwUNID; }
		int GetSound (void) const { return (m_pSound ? m_pSound->GetSound() : -1); }
		const CSoundResource *GetSoundResource () const { return m_pSound; }
		bool IsNull (void) const { return (m_dwUNID == 0); }
		ALERROR LoadUNID (SDesignLoadCtx &Ctx, const CString &sAttrib);
		void Mark (void) const { if (m_pSound) m_pSound->Mark(); }
		void PlaySound (CSpaceObject *pSource) const;
		void PlaySound (CSpaceObject *pSource, SSoundOptions *pOptions) const;

	private:
		DWORD m_dwUNID = 0;
		CSoundResource *m_pSound = NULL;
	};

class CMusicResource : public CDesignType
	{
	public:
		CMusicResource (void) : m_iNextSegment(0)
			{ }

		~CMusicResource (void) { }

		int FindSegment (int iPos);
		const CString &GetAlbum (void) const;
		const CString &GetComposedBy (void) const { return m_sComposedBy; }
		CString GetFilename (void) const { return m_sFilename; }
		CString GetFilespec (void) const;
		const CAffinityCriteria &GetLocationCriteria (void) const { return m_LocationCriteria; }
		int GetNextFadePos (int iPos);
		int GetNextPlayPos (void);
		const CString &GetPerformedBy (void) const { return m_sPerformedBy; }
		int GetPriority (void) const { return m_iPriority; }
		int GetSegmentCount (void) const { return (m_Segments.GetCount() == 0 ? 1 : m_Segments.GetCount()); }
		const CString &GetTitle (void) const { return m_sTitle; }
		void Init (DWORD dwUNID, const CString &sFilespec, int iPriority = 0) { SetUNID(dwUNID); m_sFilespec = sFilespec; m_iPriority = iPriority; }
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
