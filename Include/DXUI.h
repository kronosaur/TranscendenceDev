//	DXUI.h
//
//	Implements simple user interface objects
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class AGArea;
class AGScreen;

class IScreenController
	{
	public:
		virtual void Action (DWORD dwTag, DWORD dwData = 0) = 0;
	};

class IAreaContainer
	{
	public:
		virtual void GetMousePos (POINT *retpt) { }
		virtual void OnAreaSetRect (void) { }
	};

class AGArea : public CObject
	{
	public:
		AGArea (void);
		virtual ~AGArea (void);

		void AddEffect (const SEffectDesc &Effect);
		void AddShadowEffect (void);
		inline IAreaContainer *GetParent (void) { return m_pParent; }
		inline RECT &GetRect (void) { return m_rcRect; }
		inline const RECT &GetRect (void) const { return m_rcRect; }
		inline AGScreen *GetScreen (void) { return m_pScreen; }
		inline DWORD GetTag (void) { return m_dwTag; }
		inline void Hide (void) { ShowHide(false); }
		ALERROR Init (AGScreen *pScreen, IAreaContainer *pParent, const RECT &rcRect, DWORD dwTag);
		inline void Invalidate (void) { Invalidate(m_rcRect); }
		inline void Invalidate (const RECT &rcInvalid);
		inline bool IsVisible (void) { return m_bVisible; }
		inline void SetController (IScreenController *pController) { m_pController = pController; }
		void SetRect (const RECT &rcRect);
		void SignalAction (DWORD dwData = 0);
		inline void Show (void) { ShowHide(true); }
		void ShowHide (bool bShow);

		//	AGArea virtuals
		virtual int Justify (const RECT &rcRect) { return 0; }
		virtual bool LButtonDoubleClick (int x, int y) { return false; }
		virtual bool LButtonDown (int x, int y) { return false; }
		virtual void LButtonUp (int x, int y) { }
		virtual void MouseEnter (void) { }
		virtual void MouseLeave (void) { }
		virtual void MouseMove (int x, int y) { }
		virtual void Paint (CG32bitImage &Dest, const RECT &rcRect) { }
		virtual void Update (void) { }
		virtual bool WantsMouseOver (void) { return true; }

	protected:
		inline const SEffectDesc &GetEffect (int iIndex) { return m_pEffects[iIndex]; }
		inline int GetEffectCount (void) { return m_iEffectCount; }
		inline const SEffectDesc *GetEffects (void) { return m_pEffects; }
		inline bool HasEffects (void) { return (m_pEffects != NULL); }
		virtual void OnSetRect (void) { }
		virtual void OnShowHide (bool bShow) { }

	private:
		IAreaContainer *m_pParent;
		AGScreen *m_pScreen;
		RECT m_rcRect;
		DWORD m_dwTag;
		IScreenController *m_pController;

		int m_iEffectCount;
		SEffectDesc *m_pEffects;

		bool m_bVisible;						//	TRUE if visible

	friend CObjectClass<AGArea>;
	};

class AGScreen : public CObject, public IAreaContainer
	{
	public:
		AGScreen (HWND hWnd, const RECT &rcRect);
		virtual ~AGScreen (void);

		ALERROR AddArea (AGArea *pArea, const RECT &rcRect, DWORD dwTag, bool bSendToBack = false);
		void DestroyArea (AGArea *pArea);
		AGArea *FindArea (DWORD dwTag);
		inline AGArea *GetArea (int iIndex) { return m_Areas[iIndex]; }
		inline int GetAreaCount (void) { return m_Areas.GetCount(); }
		inline const RECT &GetRect (void) { return m_rcRect; }
		inline IScreenController *GetController (void) { return m_pController; }
		inline void Invalidate (const RECT &rcInvalid) { UnionRect(&m_rcInvalid, &rcInvalid, &m_rcInvalid); }
		inline void SetBackgroundColor (CG32bitPixel rgbColor) { m_rgbBackgroundColor = rgbColor; }
		inline void SetController (IScreenController *pController) { m_pController = pController; }

		//	IAreaContainer virtuals
		virtual void GetMousePos (POINT *retpt);
		virtual void OnAreaSetRect (void);

		//	These methods are called by the HWND
		void LButtonDoubleClick (int x, int y);
		void LButtonDown (int x, int y);
		void LButtonUp (int x, int y);
		void MouseMove (int x, int y);
		void Paint (CG32bitImage &Dest);
		void Update (void);

	private:
		AGScreen (void);
		void FireMouseMove (const POINT &pt);
		inline int GetAreaIndex (AGArea *pArea) { int iIndex; if (m_Areas.Find(pArea, &iIndex)) return iIndex; else return -1; }
		AGArea *HitTest (const POINT &pt);
		void SetMouseOver (AGArea *pArea);

		HWND m_hWnd;
		RECT m_rcRect;							//	Rect of screen relative to Window
		RECT m_rcInvalid;						//	Invalid rect relative to m_rcRect
		IScreenController *m_pController;		//	Screen controller

		TArray<AGArea *> m_Areas;

		AGArea *m_pMouseCapture;				//	Area that has captured the mouse
		AGArea *m_pMouseOver;					//	Area that the mouse is currently over

		CG32bitPixel m_rgbBackgroundColor;

	friend CObjectClass<AGScreen>;
	};

class CGFrameArea : public AGArea, public IAreaContainer
	{
	public:
		CGFrameArea (void);

		ALERROR AddArea (AGArea *pArea, const RECT &rcRect, DWORD dwTag);
		AGArea *FindArea (DWORD dwTag);
		inline AGArea *GetArea (int iIndex) { return (AGArea *)m_Areas.GetObject(iIndex); }
		inline int GetAreaCount (void) { return m_Areas.GetCount(); }

		//	AGArea virtuals
		virtual bool LButtonDoubleClick (int x, int y);
		virtual bool LButtonDown (int x, int y);
		virtual void LButtonUp (int x, int y);
		virtual void MouseMove (int x, int y);
		virtual void Paint (CG32bitImage &Dest, const RECT &rcRect);
		virtual void Update (void);

		//	IAreaContainer virtuals
		virtual void OnAreaSetRect (void);

	private:
		RECT m_rcInvalid;						//	Invalid rect relative to m_rcRect

		CObjectArray m_Areas;					//	Array of areas

		AGArea *m_pMouseCapture;				//	Area that has captured the mouse
		AGArea *m_pMouseOver;					//	Area that the mouse is currently over
	};

class CGButtonArea : public AGArea
	{
	public:
		CGButtonArea (void);

		inline CString GetLabelAccelerator (void) { return (m_iAccelerator != -1 ? CString(m_sLabel.GetASCIIZPointer() + m_iAccelerator, 1) : NULL_STR); }
		inline bool IsDisabled (void) { return m_bDisabled; }
		inline void SetDisabled (bool bDisabled = true) { m_bDisabled = bDisabled; }
		inline void SetLabel (const CString &sText) { m_sLabel = sText; m_iAccelerator = -1; }
		void SetLabelAccelerator (const CString &sKey);
		inline void SetLabelColor (CG32bitPixel rgbColor) { m_rgbLabelColor = rgbColor; }
		inline void SetLabelFont (const CG16bitFont *pFont) { m_pLabelFont = pFont; }

		//	AGArea virtuals
		virtual void LButtonUp (int x, int y);
		virtual void MouseEnter (void);
		virtual void MouseLeave (void);
		virtual void Paint (CG32bitImage &Dest, const RECT &rcRect);

	protected:
		//	AGArea virtuals
		virtual void OnSetRect (void);

	private:
		CString m_sLabel;
		CG32bitPixel m_rgbLabelColor;
		const CG16bitFont *m_pLabelFont;
		int m_iAccelerator;

		bool m_bMouseOver;
		bool m_bDisabled;
	};

class CGImageArea : public AGArea
	{
	public:
		CGImageArea (void);

		inline void SetBackColor (CG32bitPixel rgbColor) { m_rgbBackColor = rgbColor; }
		inline void SetImage (CG32bitImage *pImage, const RECT &rcImage) { m_pImage = pImage; m_rcImage = rcImage; Invalidate(); }
		inline void SetStyles (DWORD dwStyles) { m_dwStyles = dwStyles; Invalidate(); }
		inline void SetTransBackground (bool bTrans = true) { m_bTransBackground = bTrans; }

		//	AGArea virtuals
		virtual void Paint (CG32bitImage &Dest, const RECT &rcRect);
		virtual bool WantsMouseOver (void) { return false; }

	private:
		CG32bitImage *m_pImage;
		RECT m_rcImage;

		CG32bitPixel m_rgbBackColor;
		bool m_bTransBackground;
		DWORD m_dwStyles;
	};

class CGTextArea : public AGArea
	{
	public:
		CGTextArea (void);

		inline const CString &GetText (void) { return m_sText; }
		inline void SetColor (CG32bitPixel rgbColor) { m_rgbColor = rgbColor; }
		inline void SetCursor (int iLine, int iCol = 0) { m_iCursorLine = iLine; m_iCursorPos = iCol; }
		inline void SetEditable (bool bEditable = true) { m_bEditable = bEditable; }
		inline void SetFont (const CG16bitFont *pFont) { m_pFont = pFont; m_cxJustifyWidth = 0; }
		inline void SetFontTable (const IFontTable *pFontTable) { m_pFontTable = pFontTable; }
		inline void SetLineSpacing (int cySpacing) { m_cyLineSpacing = cySpacing; m_cxJustifyWidth = 0; }
		inline void SetRichText (const CString &sRTF) { m_sRTF = sRTF; m_sText = NULL_STR; m_bRTFInvalid = true; Invalidate(); }
		inline void SetStyles (DWORD dwStyles) { m_dwStyles = dwStyles; m_cxJustifyWidth = 0; }
		inline void SetText (const CString &sText) { m_sText = sText; m_sRTF = NULL_STR; m_cxJustifyWidth = 0; Invalidate(); }

		//	AGArea virtuals
		virtual int Justify (const RECT &rcRect);
		virtual void Paint (CG32bitImage &Dest, const RECT &rcRect);
		virtual void Update (void) { m_iTick++; }

	private:
		void FormatRTF (const RECT &rcRect);
		void PaintRTF (CG32bitImage &Dest, const RECT &rcRect);
		void PaintText (CG32bitImage &Dest, const RECT &rcRect);

		CString m_sText;						//	Text text to draw
		CString m_sRTF;							//	Rich text to draw (only if m_sText is blank)

		bool m_bEditable;						//	TRUE if editable
		DWORD m_dwStyles;						//	AlignmentStyles
		int m_cyLineSpacing;					//	Extra spacing between lines
		const CG16bitFont *m_pFont;
		CG32bitPixel m_rgbColor;

		bool m_bRTFInvalid;						//	TRUE if we need to format rich text
		CTextBlock m_RichText;					//	Rich text to draw (only if m_sText is blank)
		const IFontTable *m_pFontTable;			//	For rich text

		TArray<CString> m_Lines;				//	Justified lines of text
		int m_cxJustifyWidth;					//	Width (in pixels) for which m_Lines
												//		was justified.

		int m_iTick;							//	Cursor tick
		int m_iCursorLine;						//	Cursor position (-1 = no cursor)
		int m_iCursorPos;						//	Position in line
	};
