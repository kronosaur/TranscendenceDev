//	TSUIMapPainters.h
//
//	Transcendence UI Engine
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CGalacticMapSystemDetails
    {
    public:
        CGalacticMapSystemDetails (const CVisualPalette &VI, CReanimator &Reanimator, const RECT &rcPane);

        bool CreateDetailsPane (const CTopologyNode &Node, IAnimatron **retpAni);

    private:
        struct SObjDesc
            {
            int iCount = 0;
            CObjectTracker::SObjEntry ObjData;
			TArray<CCartoucheBlock::SCartoucheDesc> Attribs;
            };

		struct SSystemHeader
			{
			CString sTitle;
			CString sDetails;
			TArray<CCartoucheBlock::SCartoucheDesc> Attribs;
			};

		void CreateObjEntry (const SObjDesc &Obj, int yPos, int cxWidth, IAnimatron **retpAni, int *retcyHeight);
		bool CreateObjIcon (const CObjectTracker::SObjEntry &Obj, CG32bitImage **retpIcon);
        void CreateSystemHeader (CAniSequencer *pContainer, const CTopologyNode &Node, int *retcyHeight) const;
		void GetObjAttribs (const CObjectTracker::SObjEntry &Obj, TArray<CCartoucheBlock::SCartoucheDesc> &retAttribs) const;
        bool GetObjList (const CTopologyNode &Node, TSortMap<CString, SObjDesc> &Results) const;
		void GetSystemHeaderData (const CTopologyNode &Node, SSystemHeader &Header) const;

        const CVisualPalette &m_VI;
        CReanimator &m_Reanimator;
        RECT m_rcPane;                      //  RECT of pane (relative to screen)
    };

class CMapLegendPainter
    {
    public:
        struct SScaleEntry
            {
            Metric rUnits;                  //  Length of scale in units
            char *pszUnitLabel;             //  Name of the unit
            Metric rUnitLength;             //  Length of a unit in logical units
            };

        CMapLegendPainter (const CVisualPalette &VI, SScaleEntry *pScaleDesc = NULL, int iCount = 0);

        int GetHeight (void) const { Realize(); return RectHeight(m_rcRect); }
        int GetWidth (void) const { return m_cxWidth; }
        void Paint (CG32bitImage &Dest, int x, int y) const;
        void SetDesc (const CString &sDesc) { m_sDesc = sDesc; m_bRealized = false; }
        void SetScale (int cxScale, const CString &sLabel) { m_cxScale = cxScale; m_sScaleLabel = sLabel; }
        void SetScale (Metric rLogicalUnitsPerPixel);
        void SetTitle (const CString &sTitle) { m_sTitle = sTitle; m_bRealized = false; }
        void SetWidth (int cxWidth) { m_cxWidth = cxWidth; m_bRealized = false; }

    private:
        void Realize (void) const;

        const CVisualPalette &m_VI;
        SScaleEntry *m_pScaleDesc;
        int m_iScaleCount;
        int m_cxWidth;

        CString m_sTitle;
        CString m_sDesc;
        int m_cxScale;
        CString m_sScaleLabel;

        mutable bool m_bRealized;
        mutable TArray<CString> m_Title;
        mutable TArray<CString> m_Desc;
        mutable RECT m_rcRect;
    };

class CMapScaleCounter
    {
    public:
        CMapScaleCounter (void);

        bool CanZoomIn (void) const { return (m_iTargetScale < m_iMaxScale); }
        bool CanZoomOut (void) const { return (m_iTargetScale > m_iMinScale); }
        int GetScale (void) const { return m_iScale; }
        int GetTargetScale (void) const { return m_iTargetScale; }
        void Init (int iScale, int iMinScale, int iMaxScale);
        void SetTargetScale (int iScale);
        bool Update (void);
        void ZoomIn (void);
        void ZoomOut (void);
        void ZoomWheel (int iDelta);

    private:
        int GetScaleIndex (int iScale) const;

        int m_iScale;
        int m_iMinScale;
        int m_iMaxScale;

        int m_iTargetScale;

        int m_iMinScaleIndex;
        int m_iMaxScaleIndex;
    };

