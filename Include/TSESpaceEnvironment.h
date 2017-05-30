//	TSESpaceEnvironment.h
//
//	Classes and functions for space environments.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CSpaceEnvironmentType : public CDesignType
	{
	public:
		CSpaceEnvironmentType (void) : m_iTileSize(0), m_bMarked(false) { }

		ALERROR FireOnUpdate (CSpaceObject *pObj, CString *retsError = NULL);
		inline Metric GetDragFactor (void) { return m_rDragFactor; }
		inline bool HasOnUpdateEvent (void) { return m_bHasOnUpdateEvent; }
		inline bool IsLRSJammer (void) { return m_bLRSJammer; }
		inline bool IsShieldJammer (void) { return m_bShieldJammer; }
		inline bool IsSRSJammer (void) { return m_bSRSJammer; }
		void Paint (CG32bitImage &Dest, int x, int y, int xTile, int yTile, DWORD dwEdgeMask);
		void PaintLRS (CG32bitImage &Dest, int x, int y);
		void PaintMap (CG32bitImage &Dest, int x, int y, int cxWidth, int cyHeight, DWORD dwFade, int xTile, int yTile, DWORD dwEdgeMask);

		//	CDesignType overrides
		static CSpaceEnvironmentType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designSpaceEnvironmentType) ? (CSpaceEnvironmentType *)pType : NULL); }
		virtual DesignTypes GetType (void) const override { return designSpaceEnvironmentType; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual void OnClearMark (void) override { m_bMarked = false; }
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual void OnMarkImages (void) override;
		virtual void OnSweep (void) override;

	private:
		enum EConstants
			{
			TILES_IN_TILE_SET =				15,
			};

		struct STileDesc
			{
			CG8bitImage Mask;			//	Mask for edges
			CG16bitRegion Region;		//	Region created from mask
			};

		enum EEdgeTypes
			{
			edgeNone,

			edgeCloud,					//	Isolated (no edges)
			edgeStraight,				//	Open on one side only
			edgeCorner,					//	Open on two sides next to each other
			edgePeninsula,				//	Open on three sides
			edgeNarrow,					//	Open on two sides opposite each other
			};

		struct SEdgeDesc
			{
			EEdgeTypes iType;			//	Type of edge
			int iRotation;				//	Angle pointing from nebula to space

			//	Transform
			Metric rXx;
			Metric rXy;
			Metric rXc;					//	(as fraction of tile size)
			Metric rYx;
			Metric rYy;
			Metric rYc;					//	(as fraction of tile size)

			//	Wave parameters
			Metric rHalfWaveAngle;		//	Angle in half wave (radians)
			Metric rPixelsPerHalfWave;	//	Size of half wave as fraction of tile size
			Metric rWaveMin;			//	Min value of wave (to scale wave from 0.0-1.0)
			Metric rWaveScale;			//	To scale to 0.0-1.0
			Metric rMinAmplitude;		//	Minimum amplitude
			Metric rMaxAmplitude;		//	Max amplitude
			};

		void CreateAutoTileSet (int iVariants);
		void CreateEdgeTile (const SEdgeDesc &EdgeDesc, STileDesc *retTile);
		void CreateTileSet (const CObjectImageArray &Edges);
		int GetVariantOffset (int xTile, int yTile);

		CObjectImageArray m_Image;
		CObjectImageArray m_EdgeMask;
		int m_iImageTileCount;			//	Tiles in m_Image
		CG32bitPixel m_rgbMapColor;		//	Color of tile on map
		DWORD m_dwOpacity;				//	Opacity (0-255)

		bool m_bLRSJammer;				//	If TRUE, LRS is disabled
		bool m_bShieldJammer;			//	If TRUE, shields are disabled
		bool m_bSRSJammer;				//	If TRUE, SRS is disabled
		bool m_bHasOnUpdateEvent;		//	If TRUE, we have an OnUpdate event
		bool m_bAutoEdges;				//	If TRUE, we automatically created edges
		Metric m_rDragFactor;			//	Coefficient of drag (1.0 = no drag)

		//	We check for damage every 15 ticks
		int m_iDamageChance;			//	Chance of damage
		DamageDesc m_Damage;			//	Damage caused to objects in environment

		//	Generated tiles
		int m_iTileSize;				//	Size of tiles (in pixels)
		int m_iVariantCount;			//	Number of variants
		TArray<STileDesc> m_TileSet;
		bool m_bMarked;					//	Currently in use

		static SEdgeDesc EDGE_DATA[TILES_IN_TILE_SET];
	};

