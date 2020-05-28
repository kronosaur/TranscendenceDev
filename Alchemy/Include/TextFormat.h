//	TextFormat.h
//
//	Text format representation

#pragma once

//	Text Format Structures -----------------------------------------------------
//
//	These structures are used to represent a text format.

struct STextFormatDesc
	{
	CString sTypeface = CONSTLIT("Default");	//	Typeface (use IFontTable to look up)
	const CG16bitFont *pFont = NULL;			//	pFont (Use instead of sTypeface, if not NULL)
	int iFontSize = 10;							//	Font size in points
	bool bBold = false;							//	Bold
	bool bItalic = false;						//	Italic

	CG32bitPixel rgbColor = 0;					//	32-bit color
	DWORD dwOpacity = 255;						//	8-bit alpha value

	int iExtraLineSpacing = -1;					//	-1 = use defaults
	};

class IFontTable
	{
	public:
		virtual ~IFontTable (void) { }

		virtual const CG16bitFont *GetFont (const STextFormatDesc &Desc) const { return NULL; }
	};

//	Text Object Structures -----------------------------------------------------
//
//	These structures are used to represent formatted text objects (they are
//	designed for painting, not for representing abstract text formatting).

struct STextFormat
	{
	const CG16bitFont *pFont = NULL;
	CG32bitPixel rgbColor = CG32bitPixel(0, 0, 0);
	DWORD dwOpacity = 255;

	int iExtraLineSpacing = -1;				//	-1 = use SBlockFormatDesc
	};

struct SBlockFormatDesc
	{
	AlignmentStyles iHorzAlign = alignLeft;
	AlignmentStyles iVertAlign = alignTop;
	int iExtraLineSpacing = 0;				//	Pixels between lines

	STextFormat DefaultFormat;				//	Default format

	int cxWidth = -1;						//	Width of block (in pixels). -1 = no fixed width.
	int cyHeight = -1;						//	Height of block (in pixels). -1 = no fixed height.
	};

struct SFormattedTextSpan
	{
	CString sText;
	STextFormat Format;

	int x;
	int y;
	int cx;
	int cy;
	};

class CTextBlock
	{
	public:
		CTextBlock (void) { }

		void AddTextSpan (const CString &sText, const STextFormat &Format, bool bEoP = false);
		void DeleteAll (void) { m_Formatted.DeleteAll(); m_Text.DeleteAll(); }
		void Format (const SBlockFormatDesc &BlockFormat);
		void GetBounds (RECT *retrcRect);
		const SFormattedTextSpan &GetFormattedSpan (int iIndex) const { return m_Formatted[iIndex]; }
		int GetFormattedSpanCount (void) const { return m_Formatted.GetCount(); }
		bool InitFromRTF (const CString &RTF, const IFontTable &FontTable, const SBlockFormatDesc &BlockFormat, CString *retsError = NULL);
		bool IsEmpty (void) const { return (m_Formatted.GetCount() == 0); }
		void Paint (CG32bitImage &Dest, int x, int y) const;

		static CString Escape (const CString &sText);
		static bool IsRTFText (const CString &sText);
		static CString LoadAsRichText (const CString &sText);

	private:
		struct STextSpan
			{
			CString sText;
			STextFormat Format;
			bool bEoP;

			int cx;						//	Temp variable used while formatting
			};

		TArray<STextSpan> m_Text;
		TArray<SFormattedTextSpan> m_Formatted;
	};

class CCartoucheBlock
	{
	public:
		struct SCartoucheDesc
			{
			CString sText;
			CG32bitPixel rgbColor = CG32bitPixel(255, 255, 255);
			CG32bitPixel rgbBack = CG32bitPixel(0, 0, 0);
			};

		struct SPaintOptions
			{
			bool bDisabled = false;
			};

		void Add (const TArray<SCartoucheDesc> &List);
		void AddCartouche (const CString &sText, CG32bitPixel rgbColor, CG32bitPixel rgbBack);
		void Format (int cxWidth);
		RECT GetBounds (void) const;
		int GetHeight (void) const { return m_cyHeight; }
		bool IsEmpty (void) const { return m_Data.GetCount() == 0; }
		void Paint (CG32bitImage &Dest, int x, int y, const SPaintOptions &Options = SPaintOptions()) const;
		void SetFont (const CG16bitFont *pFont) { if (m_pFont != pFont) { m_pFont = pFont; Invalidate(); } }

		static void PaintCartouche (CG32bitImage &Dest, int x, int y, const SCartoucheDesc &Desc, const CG16bitFont &Font, DWORD dwAlignment = 0);

	private:
		static constexpr int ATTRIB_PADDING_X =	4;
		static constexpr int ATTRIB_PADDING_Y =	1;
		static constexpr int ATTRIB_SPACING_X =	2;
		static constexpr int ATTRIB_SPACING_Y =	2;
		static constexpr int RADIUS = 4;

		struct SCartouche
			{
			CString sText;
			CG32bitPixel rgbColor = CG32bitPixel(255, 255, 255);
			CG32bitPixel rgbBack = CG32bitPixel(0, 0, 0);
			DWORD dwOpacity = 255;
			
			int x = 0;
			int y = 0;
			int cx = 0;
			int cy = 0;
			};

		void Invalidate (void) { m_bFormatted = false; m_cxWidth = 0; m_cyHeight = 0; }

		TArray<SCartouche> m_Data;
		const CG16bitFont *m_pFont = NULL;

		bool m_bFormatted = false;
		int m_cxWidth = 0;
		int m_cyHeight = 0;
	};

class CIconLabelBlock
	{
	public:
		struct SLabelDesc
			{
			TSharedPtr<CG32bitImage> pIcon;
			RECT rcIconSrc = { 0, 0, 0, 0 };
			CString sText;
			const CG16bitFont *pFont = NULL;
			bool bNewLine = false;
			};

		struct SPaintOptions
			{
			bool bDisabled = false;
			};

		void Add (const SLabelDesc &Label);
		void DeleteAll (void) { m_Data.DeleteAll(); Invalidate(); }
		void Format (int cxWidth);
		int GetHeight (void) const { return m_cyHeight; }
		bool IsEmpty (void) const { return m_Data.GetCount() == 0; }
		void Paint (CG32bitImage &Dest, int x, int y, CG32bitPixel rgbText, const SPaintOptions &Options = SPaintOptions()) const;
		void SetFont (const CG16bitFont &Font) { if (m_pDefaultFont != &Font) { m_pDefaultFont = &Font; Invalidate(); } }

	private:
		static constexpr int ENTRY_SPACING_X = 2;
		static constexpr int ICON_MARGIN_X = 2;
		static constexpr int LINE_SPACING_Y = 2;

		struct SLabel
			{
			TSharedPtr<CG32bitImage> pIcon;
			RECT rcIconSrc = { 0 };
			CString sText;
			const CG16bitFont *pFont = NULL;
			bool bNewLine = false;

			int x = 0;
			int y = 0;
			int cx = 0;
			int cy = 0;
			};

		int CalcLineHeight (void) const;
		const CG16bitFont *GetFont (const SLabel &Entry) const { return (Entry.pFont ? Entry.pFont : m_pDefaultFont); }
		void Invalidate (void) { m_bFormatted = false; m_cxWidth = 0; m_cyHeight = 0; }

		TArray<SLabel> m_Data;
		const CG16bitFont *m_pDefaultFont = NULL;

		bool m_bFormatted = false;
		int m_cxWidth = 0;
		int m_cyHeight = 0;
		int m_cyLine = 0;
	};

class CDrawText
	{
	public:
		static void WithAccelerator (CG32bitImage &Dest, int x, int y, const CString &sText, int iAcceleratorPos, const CG16bitFont &Font, CG32bitPixel rgbTextColor, CG32bitPixel rgbAcceleratorColor);
	};
