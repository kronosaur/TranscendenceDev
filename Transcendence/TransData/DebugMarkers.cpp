//	DebugMarkers.cpp
//
//	Output debug of markers.
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

struct SMarkerDesc
	{
	MarkerTypes iType;
	const char *pszLabel;
	const char *pszColor;
	};

static constexpr SMarkerDesc MARKERS[] = 
		{
			{	markerPixel,					"Pixel",						"#ff5050"	},
			{	markerRoundDot,					"RoundDot",						"#ff5050"	},
			{	markerSquareDot,				"SquareDot",					"#ff5050"	},
			{	markerDiamondDot,				"DiamondDot",					"#ff5050"	},
			{	markerTriangleUpDot,			"TriangleUpDot",				"#ff5050"	},
			{	markerTriangleDownDot,			"TriangleDownDot",				"#ff5050"	},

			{	markerSmallCross,				"SmallCross",					"#ff5050"	},
			{	markerMediumCross,				"MediumCross",					"#ff5050"	},

			{	markerSmallSquare,				"SmallSquare",					"#ff5050"	},
			{	markerSmallFilledSquare,		"SmallFilledSquare",			"#ff5050"	},

			{	markerTinyCircle,				"TinyCircle",					"#ff5050"	},
			{	markerSmallCircle,				"SmallCircle",					"#ff5050"	},
			{	markerSmallFilledCircle,		"SmallFilledCircle",			"#ff5050"	},

			{	markerSmallTriangleUp,			"SmallTriangleUp",				"#ff5050"	},
			{	markerSmallFilledTriangleUp,	"SmallFilledTriangleUp",		"#ff5050"	},

			{	markerSmallTriangleDown,		"SmallTriangleDown",			"#ff5050"	},
			{	markerSmallFilledTriangleDown,	"SmallFilledTriangleDown",		"#ff5050"	},

			{	markerSmallFilledDiamond,		"SmallFilledDiamond",			"#ff5050"	},
			{	markerSmallDiamond,				"SmallDiamond",					"#ff5050"	},
		};

static constexpr int MARKER_COUNT = sizeof(MARKERS) / sizeof(MARKERS[0]);

static constexpr int CELL_WIDTH = 200;
static constexpr int CELL_HEIGHT = 100;
static constexpr int COLUMNS = 5;
static constexpr int LABEL_OFFSET_Y = 20;

void DebugMarkers (CXMLElement *pCmdLine)
	{
	CHost Host;
	const CG16bitFont *pFont;
	if (!Host.FindFont(CONSTLIT("Medium"), &pFont))
		{
		printf("ERROR: Unable to load font.");
		return;
		}

	//	Metric

	constexpr int ROWS = (MARKER_COUNT + COLUMNS - 1) / COLUMNS;
	constexpr int OUTPUT_WIDTH = CELL_WIDTH * COLUMNS;
	constexpr int OUTPUT_HEIGHT = CELL_HEIGHT * ROWS;

	constexpr CG32bitPixel RGB_BACKGROUND(0, 0, 0);
	constexpr CG32bitPixel RGB_TEXT(255, 255, 255);

	//	Output file

	CString sFilespec = pCmdLine->GetAttribute(CONSTLIT("output"));
	if (!sFilespec.IsBlank())
		sFilespec = pathAddExtensionIfNecessary(sFilespec, CONSTLIT(".bmp"));

	//	Generate an image to hold all markers

	CG32bitImage Output;
	Output.Create(OUTPUT_WIDTH, OUTPUT_HEIGHT);
	printf("Creating %dx%d image.\n", OUTPUT_WIDTH, OUTPUT_HEIGHT);

	//	Fill with background color

	Output.Fill(RGB_BACKGROUND);

	//	Paint each marker

	int iRow = 0;
	int iCol = 0;
	for (int i = 0; i < MARKER_COUNT; i++)
		{
		const int x = (iCol * CELL_WIDTH) + (CELL_WIDTH / 2);
		const int y = (iRow * CELL_HEIGHT) + (CELL_HEIGHT / 2);
		const CG32bitPixel rgbColor = ::LoadRGBColor(MARKERS[i].pszColor);

		Output.DrawDot(x, y, rgbColor, MARKERS[i].iType);

		pFont->DrawText(Output, x, y + LABEL_OFFSET_Y, RGB_TEXT, MARKERS[i].pszLabel, CG16bitFont::AlignCenter);

		//	Next

		if (++iCol >= COLUMNS)
			{
			iCol = 0;
			iRow++;
			}
		}

	//	Write to file or clipboard

	OutputImage(Output, sFilespec);
	}
