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
		virtual void ConvertToPaintCoords (int &x, int &y) const = 0;
		virtual void GetMousePos (POINT *retpt) = 0;
		virtual RECT GetPaintRect (const RECT &rcArea) const = 0;
		virtual void OnAreaSetRect (void) { }
	};

class AGArea
	{
	public:
		AGArea (void);
		virtual ~AGArea (void);

		void AddEffect (const SEffectDesc &Effect);
		void AddShadowEffect (void);
		RECT GetPaintRect (void) const { return m_pParent->GetPaintRect(GetRect()); }
		IAreaContainer *GetParent (void) const { return m_pParent; }
		RECT &GetRect (void) { return m_rcRect; }
		const RECT &GetRect (void) const { return m_rcRect; }
		AGScreen *GetScreen (void) { return m_pScreen; }
		DWORD GetTag (void) { return m_dwTag; }
		void Hide (void) { ShowHide(false); }
		ALERROR Init (AGScreen *pScreen, IAreaContainer *pParent, const RECT &rcRect, DWORD dwTag);
		void Invalidate (void) { Invalidate(m_rcRect); }
		void Invalidate (const RECT &rcInvalid);
		bool IsVisible (void) { return m_bVisible; }
		void SetController (IScreenController *pController) { m_pController = pController; }
		void SetRect (const RECT &rcRect);
		void SignalAction (DWORD dwData = 0);
		void Show (void) { ShowHide(true); }
		void ShowHide (bool bShow);

		//	AGArea virtuals
		virtual int Justify (const RECT &rcRect) { return 0; }
		virtual bool LButtonDoubleClick (int x, int y) { return false; }
		virtual bool LButtonDown (int x, int y) { return false; }
		virtual void LButtonUp (int x, int y) { }
		virtual void MouseEnter (void) { }
		virtual void MouseLeave (void) { }
		virtual void MouseMove (int x, int y) { }
		virtual void MouseWheel (int iDelta, int x, int y, DWORD dwFlags) { }
		virtual void Paint (CG32bitImage &Dest, const RECT &rcRect) { }
		virtual void Update (void) { }
		virtual bool WantsMouseOver (void) { return true; }

	protected:
		const SEffectDesc &GetEffect (int iIndex) { return m_pEffects[iIndex]; }
		int GetEffectCount (void) { return m_iEffectCount; }
		const SEffectDesc *GetEffects (void) { return m_pEffects; }
		bool HasEffects (void) { return (m_pEffects != NULL); }
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

class AGScreen : public IAreaContainer
	{
	public:
		AGScreen (HWND hWnd, const RECT &rcRect);
		virtual ~AGScreen (void);

		ALERROR AddArea (AGArea *pArea, const RECT &rcRect, DWORD dwTag, bool bSendToBack = false);
		void DestroyArea (AGArea *pArea);
		void DestroyArea (DWORD dwTag);
		AGArea *FindArea (DWORD dwTag);
		AGArea *GetArea (int iIndex) { return m_Areas[iIndex]; }
		int GetAreaCount (void) { return m_Areas.GetCount(); }
		const RECT &GetRect (void) { return m_rcRect; }
		DWORD GetTimeSinceMouseMove (void) const { return (::GetTickCount() - m_dwLastMouseTime); }
		const CG16bitFont &GetWingdingsFont (void) const;
		IScreenController *GetController (void) { return m_pController; }
		void Invalidate (void) { m_rcInvalid.left = 0; m_rcInvalid.top = 0; m_rcInvalid.right = RectWidth(m_rcRect); m_rcInvalid.bottom = RectHeight(m_rcRect); }
		void Invalidate (const RECT &rcInvalid) { UnionRect(&m_rcInvalid, &rcInvalid, &m_rcInvalid); }
		void OnAreaAdded (AGArea *pArea);
		void SetBackgroundColor (CG32bitPixel rgbColor) { m_rgbBackgroundColor = rgbColor; }
		void SetController (IScreenController *pController) { m_pController = pController; }

		//	IAreaContainer virtuals
		virtual void ConvertToPaintCoords (int &x, int &y) const override { x += m_rcRect.left; y += m_rcRect.top; }
		virtual void GetMousePos (POINT *retpt) override;
		virtual RECT GetPaintRect (const RECT &rcArea) const override { RECT rcResult = rcArea; ::OffsetRect(&rcResult, m_rcRect.left, m_rcRect.top); return rcResult; }
		virtual void OnAreaSetRect (void) override;

		//	These methods are called by the HWND
		void LButtonDoubleClick (int x, int y);
		void LButtonDown (int x, int y);
		void LButtonUp (int x, int y);
		void MouseMove (int x, int y);
		void MouseWheel (int iDelta, int x, int y, DWORD dwFlags);
		void Paint (CG32bitImage &Dest);
		void Update (void);

	private:
		void FireMouseMove (const POINT &pt);
		int GetAreaIndex (AGArea *pArea) { int iIndex; if (m_Areas.Find(pArea, &iIndex)) return iIndex; else return -1; }
		AGArea *HitTest (const POINT &pt);
		void RefreshMouseOver (void);
		void SetMouseOver (AGArea *pArea);

		HWND m_hWnd;
		RECT m_rcRect;								//	Rect of screen relative to Window
		RECT m_rcInvalid;							//	Invalid rect relative to m_rcRect
		IScreenController *m_pController = NULL;	//	Screen controller

		TArray<AGArea *> m_Areas;

		int m_xLastMousePos = 0;					//	Last position of mouse
		int m_yLastMousePos = 0;
		DWORD m_dwLastMouseTime = 0;				//	Tick on which we captured last mouse pos
		AGArea *m_pMouseCapture = NULL;				//	Area that has captured the mouse
		AGArea *m_pMouseOver = NULL;				//	Area that the mouse is currently over
		bool m_bRefreshMouseOver = false;			//	Need to check if the mouse is over an area

		CG32bitPixel m_rgbBackgroundColor = 0;

		mutable CG16bitFont m_Wingdings;

	friend CObjectClass<AGScreen>;
	};

class CGFrameArea : public AGArea, public IAreaContainer
	{
	public:
		CGFrameArea (void);
		~CGFrameArea (void);

		ALERROR AddArea (AGArea *pArea, const RECT &rcRect, DWORD dwTag);
		AGArea *FindArea (DWORD dwTag);
		AGArea *GetArea (int iIndex) { return m_Areas[iIndex]; }
		int GetAreaCount (void) { return m_Areas.GetCount(); }

		//	AGArea virtuals
		virtual bool LButtonDoubleClick (int x, int y) override;
		virtual bool LButtonDown (int x, int y) override;
		virtual void LButtonUp (int x, int y) override;
		virtual void MouseLeave (void) override;
		virtual void MouseMove (int x, int y) override;
		virtual void MouseWheel (int iDelta, int x, int y, DWORD dwFlags) override;
		virtual void Paint (CG32bitImage &Dest, const RECT &rcRect) override;
		virtual void Update (void) override;

		//	IAreaContainer virtuals
		virtual void ConvertToPaintCoords (int &x, int &y) const override { GetParent()->ConvertToPaintCoords(x, y); }
		virtual void GetMousePos (POINT *retpt) override { GetParent()->GetMousePos(retpt); }
		virtual RECT GetPaintRect (const RECT &rcArea) const override;
		virtual void OnAreaSetRect (void) override;

	private:
		RECT m_rcInvalid;						//	Invalid rect relative to m_rcRect

		TArray<AGArea *> m_Areas;				//	Array of areas

		AGArea *m_pMouseCapture;				//	Area that has captured the mouse
		AGArea *m_pMouseOver;					//	Area that the mouse is currently over
	};

class CGButtonArea : public AGArea
	{
	public:
		CGButtonArea (void);

		CString GetLabelAccelerator (void) { return (m_iAccelerator != -1 ? CString(m_sLabel.GetASCIIZPointer() + m_iAccelerator, 1) : NULL_STR); }
		bool IsDisabled (void) { return m_bDisabled; }
		bool IsPrefixAccelerator () const { return !m_sAccelerator.IsBlank(); }
		void SetAcceleratorColor (CG32bitPixel rgbColor) { m_rgbAccelColor = rgbColor; }
		void SetBackColor (CG32bitPixel rgbColor) { m_rgbBackColor = rgbColor; }
		void SetBackColorHover (CG32bitPixel rgbColor) { m_rgbBackColorHover = rgbColor; }
		void SetBorderColor (CG32bitPixel rgbColor) { m_rgbBorderColor = rgbColor; }
		void SetBorderRadius (int iRadius) { m_iBorderRadius = iRadius; }
		void SetDesc (const CString &sDesc) { m_sDesc = sDesc; }
		void SetDescColor (CG32bitPixel rgbColor) { m_rgbDescColor = rgbColor; }
		void SetDescFont (const CG16bitFont *pFont) { m_pDescFont = pFont; }
		void SetDisabled (bool bDisabled = true) { m_bDisabled = bDisabled; }
		void SetLabel (const CString &sText) { m_sLabel = sText; m_iAccelerator = -1; }
		void SetLabelAccelerator (const CString &sKey, int iKeyOffset = -1);
		void SetLabelColor (CG32bitPixel rgbColor) { m_rgbLabelColor = rgbColor; }
		void SetLabelFont (const CG16bitFont *pFont) { m_pLabelFont = pFont; }
		void SetPadding (int iPadding) { m_rcPadding.left = iPadding; m_rcPadding.top = iPadding; m_rcPadding.right = iPadding; m_rcPadding.bottom = iPadding; }

		//	AGArea virtuals
		virtual int Justify (const RECT &rcRect) override;
		virtual void LButtonUp (int x, int y) override;
		virtual void MouseEnter (void) override;
		virtual void MouseLeave (void) override;
		virtual void Paint (CG32bitImage &Dest, const RECT &rcRect) override;

	protected:
		//	AGArea virtuals
		virtual void OnSetRect (void) override;

	private:
		CG32bitPixel CalcBackColor (void) const;
		RECT CalcTextRect (const RECT &rcRect);

		CString m_sLabel;
		CG32bitPixel m_rgbLabelColor;
		const CG16bitFont *m_pLabelFont;

		int m_iAccelerator;						//	Index into label of accelerator (or -1)
		CG32bitPixel m_rgbAccelColor;			//	Accelerator color
		CString m_sAccelerator;					//	Text to draw in front of label

		CString m_sDesc;
		CG32bitPixel m_rgbDescColor;
		const CG16bitFont *m_pDescFont;

		RECT m_rcPadding;						//	Padding around the text
		int m_iBorderRadius;
		CG32bitPixel m_rgbBorderColor;
		CG32bitPixel m_rgbBackColor;
		CG32bitPixel m_rgbBackColorHover;

		TArray<CString> m_Lines;				//	Justified lines of text
		int m_cxJustifyWidth;					//	Width (in pixels) for which m_Lines
												//		was justified.

		bool m_bMouseOver;
		bool m_bDisabled;
	};

class CGImageArea : public AGArea
	{
	public:
		CGImageArea (void);

		void SetBackColor (CG32bitPixel rgbColor) { m_rgbBackColor = rgbColor; }
		void SetImage (CG32bitImage *pImage, const RECT &rcImage) { m_pImage = pImage; m_rcImage = rcImage; Invalidate(); }
		void SetStyles (DWORD dwStyles) { m_dwStyles = dwStyles; Invalidate(); }
		void SetTransBackground (bool bTrans = true) { m_bTransBackground = bTrans; }

		//	AGArea virtuals
		virtual void Paint (CG32bitImage &Dest, const RECT &rcRect) override;
		virtual bool WantsMouseOver (void) override { return false; }

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

		const CString &GetText (void) const { return m_sText; }
		void SetBackColor (CG32bitPixel rgbColor) { m_rgbBackColor = rgbColor; }
		void SetBorderRadius (int iRadius) { m_iBorderRadius = iRadius; }
		void SetColor (CG32bitPixel rgbColor) { m_rgbColor = rgbColor; }
		void SetCursor (int iLine, int iCol = 0) { m_iCursorLine = iLine; m_iCursorPos = iCol; }
		void SetEditable (bool bEditable = true) { m_bEditable = bEditable; }
		void SetFont (const CG16bitFont *pFont) { m_pFont = pFont; m_cxJustifyWidth = 0; }
		void SetFontTable (const IFontTable *pFontTable) { m_pFontTable = pFontTable; }
		void SetLineSpacing (int cySpacing) { m_cyLineSpacing = cySpacing; m_cxJustifyWidth = 0; }
		void SetPadding (int iPadding) { m_rcPadding.left = iPadding; m_rcPadding.top = iPadding; m_rcPadding.right = iPadding; m_rcPadding.bottom = iPadding; }
		void SetRichText (const CString &sRTF) { m_sRTF = sRTF; m_sText = NULL_STR; m_bRTFInvalid = true; Invalidate(); }
		void SetStyles (DWORD dwStyles) { m_dwStyles = dwStyles; m_cxJustifyWidth = 0; }
		void SetText (const CString &sText) { m_sText = sText; m_sRTF = NULL_STR; m_cxJustifyWidth = 0; Invalidate(); }

		//	AGArea virtuals
		virtual int Justify (const RECT &rcRect) override;
		virtual void Paint (CG32bitImage &Dest, const RECT &rcRect) override;
		virtual void Update (void) override { m_iTick++; }

	private:
		RECT CalcTextRect (const RECT &rcRect);
		void FormatRTF (const RECT &rcRect);
		void PaintRTF (CG32bitImage &Dest, const RECT &rcRect);
		void PaintText (CG32bitImage &Dest, const RECT &rcRect);

		CString m_sText;						//	Text text to draw
		CString m_sRTF;							//	Rich text to draw (only if m_sText is blank)

		bool m_bEditable;						//	TRUE if editable
		DWORD m_dwStyles;						//	AlignmentStyles
		RECT m_rcPadding;						//	Padding around the text
		int m_cyLineSpacing;					//	Extra spacing between lines
		int m_iBorderRadius;
		const CG16bitFont *m_pFont;
		CG32bitPixel m_rgbColor;
		CG32bitPixel m_rgbBackColor;

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
