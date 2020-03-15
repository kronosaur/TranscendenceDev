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

		int GetCount (void) const { return m_Runs.GetCount(); }
		int GetMinX (void) const { return (m_Runs.GetCount() > 0 ? m_Runs[0].xStart : 0); }
		void GetRun (int iIndex, int *retxStart, int *retcxLength, int *retiSamples) const;
		void InsertSampled (int xStartSampled, int cxLengthSampled, int iSamples);
		void SetY (int y) { m_yLine = y; }
		int Y (void) const { return m_yLine; }

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
		int GetRowCount (void) const { return m_Rows.GetCount(); }
		DWORD *GetRow (int iIndex, int *retyRow) const { *retyRow = m_Rows.GetKey(iIndex); return m_Rows[iIndex]; }
		int GetXOffset (void) const { return m_xOffset; }
		void InitFromRect (int x, int y, int cxWidth, int cyHeight, DWORD dwAlpha = 0xff);
		void InitFromRunList (const TArray<CGRunList> &Runs, int iSamples, bool bSorted = false);
		bool IsEmpty (void) const { return (m_Rows.GetCount() == 0); }

		static DWORD DecodeRun (DWORD dwRun, int *retcxLength) { *retcxLength = (dwRun >> 16); return GetOpacity8(dwRun); }

	private:
		static DWORD GetOpacity8 (DWORD dwRun) { return ((dwRun >> 8) & 0xff); }
		DWORD MakeOpacity (DWORD dwSamples, DWORD dwMaxSamples) { return (0xffff * dwSamples / dwMaxSamples); }
		DWORD MakeOpacityFromAlpha (DWORD dwAlpha) { return ((dwAlpha & 0xff) << 8); }
		DWORD MakeRun (DWORD cxLength, DWORD dwOpacity) { return ((cxLength & 0xffff) << 16) | dwOpacity; }

		TSortMap<int, DWORD *> m_Rows;		//	Rows sorted by y coord
		int m_xOffset;
	};

