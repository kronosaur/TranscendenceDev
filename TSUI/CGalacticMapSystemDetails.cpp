//	CGalacticMapSystemDetails.h
//
//	CGalacticMapSystemDetails class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define PROP_COLOR								CONSTLIT("color")
#define PROP_ENABLED							CONSTLIT("enabled")
#define PROP_FADE_EDGE_HEIGHT					CONSTLIT("fadeEdgeHeight")
#define PROP_FILL_TYPE							CONSTLIT("fillType")
#define PROP_FONT								CONSTLIT("font")
#define PROP_LINE_COLOR							CONSTLIT("lineColor")
#define PROP_LINE_PADDING						CONSTLIT("linePadding")
#define PROP_LINE_TYPE							CONSTLIT("lineType")
#define PROP_LINE_WIDTH							CONSTLIT("lineWidth")
#define PROP_LL_RADIUS							CONSTLIT("llRadius")
#define PROP_LR_RADIUS							CONSTLIT("lrRadius")
#define PROP_OPACITY							CONSTLIT("opacity")
#define PROP_PADDING_BOTTOM						CONSTLIT("paddingBottom")
#define PROP_POSITION							CONSTLIT("position")
#define PROP_SCALE								CONSTLIT("scale")
#define PROP_TEXT								CONSTLIT("text")
#define PROP_TEXT_ALIGN_HORZ					CONSTLIT("textAlignHorz")
#define PROP_UL_RADIUS							CONSTLIT("ulRadius")
#define PROP_UR_RADIUS							CONSTLIT("urRadius")

CGalacticMapSystemDetails::CGalacticMapSystemDetails (const CVisualPalette &VI, CReanimator &Reanimator, const RECT &rcPane) :
        m_VI(VI),
        m_Reanimator(Reanimator),
        m_rcPane(rcPane)

//  CGalacticMapSystemDetails constructor

    {
    }

bool CGalacticMapSystemDetails::CreateDetailsPane (CTopologyNode *pNode, IAnimatron **retpAni)

//  CreateDetailsPane
//
//  Creates an animation showing the details for a given system. Returns FALSE 
//  if we fail to create the pane.

    {
    return true;
    }
