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
	CG32bitPixel rgbColor = 0;
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
		inline void DeleteAll (void) { m_Formatted.DeleteAll(); m_Text.DeleteAll(); }
		void Format (const SBlockFormatDesc &BlockFormat);
		void GetBounds (RECT *retrcRect);
		inline const SFormattedTextSpan &GetFormattedSpan (int iIndex) const { return m_Formatted[iIndex]; }
		inline int GetFormattedSpanCount (void) const { return m_Formatted.GetCount(); }
		bool InitFromRTF (const CString &RTF, const IFontTable &FontTable, const SBlockFormatDesc &BlockFormat, CString *retsError = NULL);
		inline bool IsEmpty (void) const { return (m_Formatted.GetCount() == 0); }
		void Paint (CG32bitImage &Dest, int x, int y) const;

		static CString Escape (const CString &sText);
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
