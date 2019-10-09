//	TSESystemType.h
//
//	Classes and functions for system types.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CSystemType : public CDesignType
	{
	public:
		enum ETileSize
			{
			sizeUnknown =					-1,

			sizeSmall =						0,		//	128 pixels per tile
			sizeMedium =					1,		//	256 pixels per tile
			sizeLarge =						2,		//	512 pixels per tile
			sizeHuge =						3,		//	1024 pixels per tile

			sizeCount =						4,
			};

		enum ECachedHandlers
			{
			evtOnObjJumpPosAdj			= 0,

			evtCount					= 1,
			};

		CSystemType (void);
		virtual ~CSystemType (void);

		inline bool FindEventHandlerSystemType (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const 
			{
			if (!m_CachedEvents[iEvent].pCode)
				return false;

			if (retEvent) *retEvent = m_CachedEvents[iEvent];
			return true;
			}

		ALERROR FireOnCreate (SSystemCreateCtx &SysCreateCtx, CString *retsError = NULL);
		bool FireOnObjJumpPosAdj (CSpaceObject *pPos, CVector *iovPos);
		ALERROR FireSystemCreateCode (SSystemCreateCtx &SysCreateCtx, ICCItem *pCode, const COrbit &OrbitDesc, CString *retsError);
		inline DWORD GetBackgroundUNID (void) { return m_dwBackgroundUNID; }
		inline CXMLElement *GetDesc (void) { return m_pDesc; }
		inline const CImageFilterStack &GetImageFilters (void) const { return m_ImageFilters; }
		inline const CSpaceObjectCriteria &GetImageFiltersCriteria (void) const { return m_ImageFilterCriteria; }
		inline const CEnhancementDesc &GetItemEnhancements (void) const { return m_Enhancements; }
		inline CXMLElement *GetLocalSystemTables (void) { return m_pLocalTables; }
		inline ETileSize GetSpaceEnvironmentTileSize (void) const { return m_iTileSize; }
		inline Metric GetSpaceScale (void) const { return m_rSpaceScale; }
		inline Metric GetTimeScale (void) const { return m_rTimeScale; }
		inline bool HasExtraEncounters (void) const { return !m_bNoExtraEncounters; }
		inline bool HasRandomEncounters (void) const { return !m_bNoRandomEncounters; }

		//	CDesignType overrides
		static CSystemType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designSystemType) ? (CSystemType *)pType : NULL); }
		virtual DesignTypes GetType (void) const override { return designSystemType; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual void OnMarkImages (void) override;

	private:
		DWORD m_dwBackgroundUNID;
		Metric m_rSpaceScale;				//	Klicks per pixel
		Metric m_rTimeScale;				//	Seconds of game time per real time
		ETileSize m_iTileSize;				//	Tile size for environment
		CImageFilterStack m_ImageFilters;	//	Filters on object images
		CSpaceObjectCriteria m_ImageFilterCriteria;	//	Only if object matches criteria
		CEnhancementDesc m_Enhancements;	//	Enhancements conferred on items

		CXMLElement *m_pDesc;				//	System definition
		CXMLElement *m_pLocalTables;		//	Local system tables

		bool m_bNoRandomEncounters;			//	TRUE if we don't have random encounters
		bool m_bNoExtraEncounters;			//	TRUE if we don't add new encounters to
											//		satisfy minimums.

		SEventHandlerDesc m_CachedEvents[evtCount];
	};

