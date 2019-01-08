//	CMapScaleCounter.cpp
//
//	CMapScaleCounter class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

const Metric MOUSE_WHEEL_ZOOM_IN = 1.001;
const Metric MOUSE_WHEEL_ZOOM_OUT = 1.0 / MOUSE_WHEEL_ZOOM_IN;

static int SCALE_LEVELS[] =
	{	1, 2, 3, 6, 12, 25, 50, 100, 200, 400, 800, 1600 };

const int SCALE_LEVELS_COUNT = (sizeof(SCALE_LEVELS) / sizeof(SCALE_LEVELS[0]));

CMapScaleCounter::CMapScaleCounter (void)

//  CMapScaleCounter constructor

    {
    Init(100, 50, 200);
    }

int CMapScaleCounter::GetScaleIndex (int iScale) const

//  GetScaleIndex
//
//	Returns the scale index for the given scale

    {
	int i;

	for (i = 0; i < SCALE_LEVELS_COUNT; i++)
		if (iScale <= SCALE_LEVELS[i])
			return i;

    return SCALE_LEVELS_COUNT - 1;
    }

void CMapScaleCounter::Init (int iScale, int iMinScale, int iMaxScale)

//  Init
//
//  Initialize scale counter.
//
//  iScale is the amount of zoom applied to the map.
//
//  100 = normal
//  >100 = zoom in
//  <100 = zoom out

    {
    m_iScale = iScale;
    m_iMinScale = Max(1, iMinScale);
    m_iMaxScale = Max(m_iMinScale, iMaxScale);

    m_iTargetScale = iScale;

	m_iMinScaleIndex = GetScaleIndex(m_iMinScale);
	m_iMaxScaleIndex = GetScaleIndex(m_iMaxScale);
    }

void CMapScaleCounter::SetTargetScale (int iScale)

//  SetTargetScale
//
//  Zoom in to the given scale

    {
    m_iTargetScale = Min(Max(m_iMinScale, iScale), m_iMaxScale);
    }

bool CMapScaleCounter::Update (void)

//  Update
//
//  Updates the scale until it reaches the target scale. Should be called once
//  per tick.
//
//  We return TRUE if the scale changed.

    {
    if (m_iTargetScale == m_iScale)
        return false;

    int iDiff = m_iTargetScale - m_iScale;
    m_iScale = (Absolute(iDiff) > 1 ? m_iScale + (iDiff / 2) : m_iTargetScale);
    return true;
    }

void CMapScaleCounter::ZoomIn (void)

//  ZoomIn
//
//  Zoom in one stop

    {
    int iTargetScaleIndex = GetScaleIndex(m_iTargetScale);
    int iCurScale = SCALE_LEVELS[iTargetScaleIndex];
    if (iCurScale > m_iTargetScale)
        SetTargetScale(iCurScale);
    else
        SetTargetScale(SCALE_LEVELS[Min(iTargetScaleIndex + 1, m_iMaxScaleIndex)]);
    }

void CMapScaleCounter::ZoomOut (void)

//  ZoomOut
//
//  Zoom out one stop

    {
    int iTargetScaleIndex = GetScaleIndex(m_iTargetScale);
    int iCurScale = SCALE_LEVELS[iTargetScaleIndex];
    if (iCurScale < m_iTargetScale)
        SetTargetScale(iCurScale);
    else
        SetTargetScale(SCALE_LEVELS[Max(m_iMinScaleIndex, iTargetScaleIndex - 1)]);
    }

void CMapScaleCounter::ZoomWheel (int iDelta)

//  ZoomWheel
//
//  Set the target scale based on the mouse wheel delta.

    {
    if (iDelta > 0)
        SetTargetScale((int)(m_iTargetScale * Min(100.0, pow(MOUSE_WHEEL_ZOOM_IN, (Metric)iDelta))));
    else if (iDelta < 0)
        SetTargetScale((int)(m_iTargetScale * pow(MOUSE_WHEEL_ZOOM_OUT, (Metric)-iDelta)));
    }
