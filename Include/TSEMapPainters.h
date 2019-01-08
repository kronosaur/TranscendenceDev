//	TSEMapPainters.h
//
//	Classes and functions for painting system and galactic maps.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//#define FULL_SYSTEM_THUMBNAILS

class CSystemMapThumbnails;

const int SYSTEM_MAP_THUMB_WIDTH =              320;
const int SYSTEM_MAP_THUMB_HEIGHT =             320;

class CGalacticMapPainter
	{
	public:
        struct SSelectResult
            {
            CTopologyNode *pNode;
            };

		CGalacticMapPainter (const CUniverse &Universe, CSystemMap *pMap, CSystemMapThumbnails &SystemMapThumbnails);
		~CGalacticMapPainter (void);

		void AdjustCenter (int xCenter, int yCenter, int iScale, int *retxCenter, int *retyCenter) const;
        void GalacticToView (int x, int y, int xCenter, int yCenter, int iScale, int *retx, int *rety) const;
		inline int GetHeight (void) const { return m_cyMap; }
        inline void GetPos (int *retx, int *rety) const { *retx = m_xCenter; *rety = m_yCenter; }
        inline int GetScale (void) const { return m_iScale; }
        inline CTopologyNode *GetSelection (void) const { return m_pSelected; }
        inline const RECT &GetViewport (void) const { return m_rcView; }
		inline int GetWidth (void) const { return m_cxMap; }
        bool HitTest (int x, int y, SSelectResult &Result) const;
		void Paint (CG32bitImage &Dest) const;
        inline void SetPos (int x, int y) { m_xCenter = x; m_yCenter = y; }
        inline void SetScale (int iScale) { m_iScale = iScale; }
        inline void SetSelection (CTopologyNode *pNode) { m_pSelected = pNode; }
        inline void SetViewport (const RECT &rcRect) { m_rcView = rcRect; RectCenter(m_rcView, &m_xViewCenter, &m_yViewCenter); }
        void ViewToGalactic (int x, int y, int xCenter, int yCenter, int iScale, int *retx, int *rety) const;

	private:
		void DrawNode (CG32bitImage &Dest, CTopologyNode *pNode, int x, int y, Metric rScale, CG32bitPixel rgbColor) const;
		void DrawNodeConnections (CG32bitImage &Dest, CTopologyNode *pNode, int x, int y) const;
        void DrawSelection (CG32bitImage &Dest, int x, int y, CG32bitPixel rgbColor) const;
		void DrawUnknownNode (CG32bitImage &Dest, CTopologyNode *pNode, int x, int y, Metric rScale, CG32bitPixel rgbColor) const;
		inline int GetImageGalacticHeight (void) const { return (m_pImage ? (int)(m_pImage->GetHeight() / m_rImageScale) : 0); }
		inline int GetImageGalacticWidth (void) const { return (m_pImage ? (int)(m_pImage->GetWidth() / m_rImageScale) : 0); }
		void Init (void);
		inline SPoint Xform (const SPoint &pt) const { return SPoint(m_xViewCenter + m_iScale * (pt.x - m_xCenter) / 100, m_yViewCenter + m_iScale * (m_yCenter - pt.y) / 100); }

		const CUniverse &m_Universe;
		const CG16bitFont &m_MediumFont;
		CG32bitPixel m_rgbSelection;

		CSystemMap *m_pMap;
        CSystemMapThumbnails &m_SystemMapThumbnails;
		int m_cxMap;
		int m_cyMap;

		CG32bitImage *m_pImage;				//	Background image (with annotations)
		bool m_bFreeImage;
		Metric m_rImageScale;				//	Image pixels per galactic coordinate

		//	Painting options

        CTopologyNode *m_pSelected;         //  Selected node (may be NULL)
        mutable int m_iSelectAngle;         //  Animate selection

		int m_iScale;                       //  Scale to paint at 100 = normal; 200 = 2x size
		int m_xCenter;                      //  Center of viewport in galactic coordinates
		int m_yCenter;

        RECT m_rcView;                      //  Viewport in output coordinates
		int m_xViewCenter;                  //  Center of viewport in output coordinates
		int m_yViewCenter;
	};

class CSystemMapThumbnails
    {
    public:
        CSystemMapThumbnails (void);
        ~CSystemMapThumbnails (void);

        void CleanUp (void);
        void DrawThumbnail (CTopologyNode *pNode, CG32bitImage &Dest, int x, int y, bool bFullSystem = true, Metric rScale = 1.0) const;
        void Init (CObjectTracker &SystemData, int cxThumb = SYSTEM_MAP_THUMB_WIDTH, int cyThumb = SYSTEM_MAP_THUMB_HEIGHT);

    private:
        struct SStarDesc
            {
            CG32bitImage *pStar;
            CVector vPos;                   //  Position relative to center of node (in pixels at scale = 1.0).
            };

        struct SThumbnailCacheEntry
            {
            SThumbnailCacheEntry (void) :
                    pFullSystem(NULL),
                    pStarsOnly(NULL)
                { }

            ~SThumbnailCacheEntry (void)
                {
                if (pFullSystem)
                    delete pFullSystem;

                if (pStarsOnly)
                    delete pStarsOnly;
                }

            CG32bitImage *pFullSystem;
            CG32bitImage *pStarsOnly;

            TArray<SStarDesc> Stars;
            };

        bool CreateThumbnail (CTopologyNode *pNode, bool bFullSystem, CG32bitImage &Result) const;
        bool CreateThumbnailStars (CTopologyNode *pNode, TArray<SStarDesc> &Result) const;
        CG32bitImage *GetObjImage (const CObjectTracker::SBackgroundObjEntry &ObjEntry) const;
        CG32bitImage *GetThumbnail (CTopologyNode *pNode, bool bFullSystem = true) const;
        const TArray<SStarDesc> &GetThumbnailStars (CTopologyNode *pNode) const;

        CObjectTracker *m_pSystemData;
        int m_cxThumb;                      //  Size of thumbnail
        int m_cyThumb;
        ViewportTransform m_Trans;          //  Transform from system coords to thumb
        Metric m_rMapScale;                 //  Map scale in klicks per pixel
        Metric m_rImageScale;               //  Scale factor for objects in system
        int m_iMaxImage;
        int m_iMinImage;

        mutable TSortMap<CTopologyNode *, SThumbnailCacheEntry> m_Cache;
        mutable TSortMap<DWORDLONG, CG32bitImage *> m_ObjImageCache;
    };
