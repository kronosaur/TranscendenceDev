//	DXRegion.h
//
//	Implements 2D Regions
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CGRunList
	{
	public:
		struct SRun
			{
			int xStart;
			int xEnd;
			int cxLength;

			int iSamples;
			};

		CGRunList (void) :
				m_yLine(0)
			{ }

		inline int GetCount (void) const { return m_Runs.GetCount(); }
		inline int GetMinX (void) const { return (m_Runs.GetCount() > 0 ? m_Runs[0].xStart : 0); }
		void GetRun (int iIndex, int *retxStart, int *retcxLength, int *retiSamples) const;
		void InsertSampled (int xStartSampled, int cxLengthSampled, int iSamples);
		inline void SetY (int y) { m_yLine = y; }
		inline int Y (void) const { return m_yLine; }

	private:
		void CombineRun (int xStart, int cxLength, int iSamples);
		void InsertRun (int xStart, int xEnd, int cxLength, int iSamples, int iPos = -1);

		int m_yLine;
		TArray<SRun> m_Runs;
	};

class CGRegion
	{
	public:
		~CGRegion (void);

		void CleanUp (void);
		inline int GetRowCount (void) const { return m_Rows.GetCount(); }
		inline DWORD *GetRow (int iIndex, int *retyRow) const { *retyRow = m_Rows.GetKey(iIndex); return m_Rows[iIndex]; }
		inline int GetXOffset (void) const { return m_xOffset; }
		void InitFromRect (int x, int y, int cxWidth, int cyHeight, DWORD dwAlpha = 0xff);
		void InitFromRunList (const TArray<CGRunList> &Runs, int iSamples, bool bSorted = false);
		inline bool IsEmpty (void) const { return (m_Rows.GetCount() == 0); }

		inline static DWORD DecodeRun (DWORD dwRun, int *retcxLength) { *retcxLength = (dwRun >> 16); return GetOpacity8(dwRun); }

	private:
		inline static DWORD GetOpacity8 (DWORD dwRun) { return ((dwRun >> 8) & 0xff); }
		inline DWORD MakeOpacity (DWORD dwSamples, DWORD dwMaxSamples) { return (0xffff * dwSamples / dwMaxSamples); }
		inline DWORD MakeOpacityFromAlpha (DWORD dwAlpha) { return ((dwAlpha & 0xff) << 8); }
		inline DWORD MakeRun (DWORD cxLength, DWORD dwOpacity) { return ((cxLength & 0xffff) << 16) | dwOpacity; }

		TSortMap<int, DWORD *> m_Rows;		//	Rows sorted by y coord
		int m_xOffset;
	};

