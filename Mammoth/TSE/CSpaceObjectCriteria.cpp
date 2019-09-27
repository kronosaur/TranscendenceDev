//	CSpaceObjectCriteria.cpp
//
//	CSpaceObjectCriteria class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ORDER_DOCKED							CONSTLIT("docked")

CSpaceObjectCriteria::CSpaceObjectCriteria (const CString &sCriteria)

//	CSpaceObjectCriteria constructor

	{
	Parse(NULL, sCriteria);
	}

CSpaceObjectCriteria::CSpaceObjectCriteria (CSpaceObject *pSourceArg, const CString &sCriteria)

//	CSpaceObjectCriteria constructor

	{
	Parse(pSourceArg, sCriteria);
	}

bool CSpaceObjectCriteria::MatchesAttributes (const CSpaceObject &Obj) const

//	MatchesAttributes
//
//	Returns TRUE if the given object matches the attributes that we specify.

	{
	int i;

	for (i = 0; i < m_AttribsRequired.GetCount(); i++)
		if (!Obj.HasAttribute(m_AttribsRequired[i]))
			return false;

	//	Check attributes not allowed

	for (i = 0; i < m_AttribsNotAllowed.GetCount(); i++)
		if (Obj.HasAttribute(m_AttribsNotAllowed[i]))
			return false;

	//	Check special attribs required

	for (i = 0; i < m_SpecialRequired.GetCount(); i++)
		if (!Obj.HasSpecialAttribute(m_SpecialRequired[i]))
			return false;

	//	Check special attribs not allowed

	for (i = 0; i < m_SpecialNotAllowed.GetCount(); i++)
		if (Obj.HasSpecialAttribute(m_SpecialNotAllowed[i]))
			return false;

	return true;
	}

bool CSpaceObjectCriteria::MatchesLevel (int iLevel) const

//	MatchesLevel
//
//	Returns TRUE if we match the level.

	{
	if (m_iEqualToLevel != -1 && iLevel != m_iEqualToLevel)
		return false;

	if (m_iGreaterThanLevel != -1 && iLevel <= m_iGreaterThanLevel)
		return false;

	if (m_iLessThanLevel != -1 && iLevel >= m_iLessThanLevel)
		return false;

	return true;
	}

bool CSpaceObjectCriteria::MatchesOrder (const CSpaceObject *pSource, const CSpaceObject &Obj) const

//	MatchesOrder
//
//	Returns TRUE if the given object matches the order.

	{
	switch (m_iOrder)
		{
		case IShipController::orderNone:
			return true;

		case IShipController::orderEscort:
			return (Obj.GetEscortPrincipal() == pSource);

		default:
			{
			IShipController::OrderTypes iOrder = IShipController::orderNone;
			CSpaceObject *pTarget = NULL;

			CShip *pShip = const_cast<CSpaceObject *>(&Obj)->AsShip();
			if (pShip && pShip->GetController())
				iOrder = pShip->GetController()->GetCurrentOrderEx(&pTarget);

			return (m_iOrder == iOrder && pSource == pTarget);
			}
		}
	}

bool CSpaceObjectCriteria::MatchesPosition (const CSpaceObject &Obj) const

//	MatchesPosition
//
//	Returns TRUE if the given object matches the position we require.

	{
	switch (m_iPosCheck)
		{
		case checkNone:
			return true;

		case checkPosIntersect:
			return Obj.PointInObject(Obj.GetPos(), m_vPos1);

		case checkLineIntersect:
			{
			//	Get the size of the object

			CVector vObjUR;
			CVector vObjLL;
			Obj.GetHitRect(&vObjUR, &vObjLL);

			//	Get the distance along the ray at which we intersect the given
			//	object rectangle

			Metric rObjHitFrac;
			if (!IntersectRectAndRay(vObjUR, vObjLL, m_vPos1, m_vPos2, NULL, &rObjHitFrac))
				return false;

			//	See if we actually hit the object

			CVector vLine = m_vPos2 - m_vPos1;
			Metric rLineLen = vLine.Length();
			if (rLineLen == 0.0)
				return false;

			CVector vRayN = vLine / rLineLen;
			CVector vRayPixel = vRayN * g_KlicksPerPixel;
			Metric rObjHitLen = rObjHitFrac * rLineLen;

			SPointInObjectCtx Ctx;
			Obj.PointInObjectInit(Ctx);

			Metric rTestHitLen = rObjHitLen;
			CVector vTestPos = m_vPos1 + (rTestHitLen * vRayN);
			bool bHit = false;
			while (rTestHitLen < rLineLen)
				{
				if (Obj.PointInObject(Ctx, Obj.GetPos(), vTestPos))
					{
					bHit = true;
					break;
					}

				rTestHitLen += g_KlicksPerPixel;
				vTestPos = vTestPos + vRayPixel;
				}

			return bHit;
			}

		default:
			return true;
		}
	}

bool CSpaceObjectCriteria::MatchesSovereign (CSovereign *pSovereign) const

//	MatchesSovereign
//
//	Returns TRUE if we match the sovereign

	{
	return ((m_dwSovereignUNID == 0) || (pSovereign && pSovereign->GetUNID() == m_dwSovereignUNID));
	}

void CSpaceObjectCriteria::Parse (CSpaceObject *pSource, const CString &sCriteria)

//	Parse
//
//	Parses the given criteria string and initializes our member variables. We
//	assume that we are initialized.
//
//	Parses a string and returns criteria structure
//
//		b			Include beams
//		k			Include markers
//		m			Include missiles
//		s			Include ships
//		t			Include stations (including planets)
//		v			Include intangible objects
//		z			Include the player
//
//		A			Active objects only (i.e., objects that can attack)
//		B:xyz;		Only objects with attribute 'xyz'
//		C			(unused)
//		D:xyz;		Only objects with data 'xyz'
//		E			Enemy objects only
//		F			Friendly objects only
//		G			Stargates only
//		G:xyz;		Stargate with ID 'xyz'
//		H			Only objects whose base = source
//		I:angle		Only objects that intersect line from source
//		J			Same sovereign as source
//		J:unid;		Sovereign unid = unid
//		K			Killed objects only (i.e., objects that cannot attack)
//		L:x-y;		Objects of level x to y.
//		M			Manufactured objects only (i.e., no planets or asteroids)
//		N			Return only the nearest object to the source
//		N:nn;		Return only objects within nn light-seconds
//		O:docked;	Ships that are currently docked at source
//		O:escort;	Ships ordered to escort source
//		O:guard;	Ships ordered to guard source
//		P			Only objects that can be detected (perceived) by source
//		Q			(unused)
//		R			Return only the farthest object to the source
//		R:nn;		Return only objects greater than nn light-seconds away
//		S:sort		Sort order ('d' = distance ascending; 'D' = distance descending
//		T			Include structure-scale stations
//		T:xyz;		Include stations with attribute 'xyz'
//		U			(unused)
//		V			Include virtual objects
//		W			(unused)
//		X			Only objects whose target is the source
//		Y			Enemy/angry objects only
//		Z			Exclude the player
//
//		+xyz;		Include objects with the given attribute
//		-xyz;		Exclude objects with the given attribute
//
//		=n			Level comparisons

	{
	CString sParam;

	//	Initialize

	m_pSource = pSource;

	//	Parse

	const char *pPos = sCriteria.GetPointer();
	while (*pPos != '\0')
		{
		switch (*pPos)
			{
			case '*':
				m_dwCategories = 0xffffffff;
				break;

			case 'A':
				m_bActiveObjectsOnly = true;
				break;

			case 'B':
				{
				CString sAttrib = ParseCriteriaParam(&pPos);
				if (!sAttrib.IsBlank())
					m_AttribsRequired.Insert(sAttrib);
				break;
				}

			case 'b':
				m_dwCategories |= CSpaceObject::catBeam;
				break;

			case 'D':
				m_sData = ParseCriteriaParam(&pPos);
				break;

			case 'E':
				m_bEnemyObjectsOnly = true;
				break;

			case 'F':
				m_bFriendlyObjectsOnly = true;
				break;

			case 'G':
				m_dwCategories |= CSpaceObject::catStation;
				m_sStargateID = ParseCriteriaParam(&pPos);
				m_bStargatesOnly = true;
				break;

			case 'H':
				m_bHomeBaseIsSource = true;
				break;

			case 'I':
				m_iIntersectAngle = strToInt(ParseCriteriaParam(&pPos), -1);
				break;

			case 'J':
				sParam = ParseCriteriaParam(&pPos);
				if (!sParam.IsBlank())
					m_dwSovereignUNID = strToInt(sParam, 0);
				else
					{
					m_bSourceSovereignOnly = true;
					m_dwSovereignUNID = (pSource && pSource->GetSovereign() ? pSource->GetSovereign()->GetUNID() : 0);
					}
				break;

			case 'K':
				m_bKilledObjectsOnly = true;
				break;

			case 'k':
				m_dwCategories |= CSpaceObject::catMarker;
				break;

			case 'L':
				{
				CString sParam = ParseCriteriaParam(&pPos);
				const char *pParamPos = sParam.GetASCIIZPointer();

				//	Parse the first number

				int iLow = strParseInt(pParamPos, -1, &pParamPos);

				//	If we don't have a second number, then we just want items
				//	of the given level.

				if (*pParamPos != '-')
					{
					if (iLow != -1)
						m_iEqualToLevel = iLow;
					}

				//	Otherwise, we parse a second number

				else
					{
					pParamPos++;
					int iHi = strParseInt(pParamPos, -1, &pParamPos);

					if (iLow == -1)
						iLow = 1;
					if (iHi == -1)
						iHi = MAX_OBJECT_LEVEL;

					m_iGreaterThanLevel = iLow - 1;
					m_iLessThanLevel = iHi + 1;
					}

				break;
				}

			case 'M':
				m_bManufacturedObjectsOnly = true;
				break;

			case 'm':
				m_dwCategories |= CSpaceObject::catMissile;
				break;

			case 'N':
				sParam = ParseCriteriaParam(&pPos);
				if (sParam.IsBlank())
					m_bNearestOnly = true;
				else
					{
					m_bNearerThan = true;
					m_rMaxRadius = LIGHT_SECOND * strToInt(sParam, 0, NULL);
					}
				break;

			case 'O':
				{
				CString sAttrib = ParseCriteriaParam(&pPos);
				if (strEquals(sAttrib, ORDER_DOCKED))
					m_bDockedWithSource = true;
				else
					{
					m_iOrder = IShipController::GetOrderType(sAttrib);
					if (m_iOrder == IShipController::orderNone)
						::kernelDebugLogPattern("Invalid sysFindObject order: %s", sAttrib);
					}
				break;
				}

			case 'P':
				m_bPerceivableOnly = true;
				m_iPerception = (pSource ? pSource->GetPerception() : 0);
				break;

			case 'R':
				sParam = ParseCriteriaParam(&pPos);
				if (sParam.IsBlank())
					m_bFarthestOnly = true;
				else
					{
					m_bFartherThan = true;
					m_rMinRadius = LIGHT_SECOND * strToInt(sParam, 0, NULL);
					}
				break;

			case 's':
				m_dwCategories |= CSpaceObject::catShip;
				break;

			case 'S':
				{
				sParam = ParseCriteriaParam(&pPos);
				char *pSort = sParam.GetASCIIZPointer();
				if (*pSort == 'd')
					{
					m_iSort = sortByDistance;
					m_iSortOrder = AscendingSort;
					}
				else if (*pSort == 'D')
					{
					m_iSort = sortByDistance;
					m_iSortOrder = DescendingSort;
					}
				break;
				}

			case 't':
				{
				m_dwCategories |= CSpaceObject::catStation;

				CString sAttrib = ParseCriteriaParam(&pPos);
				if (!sAttrib.IsBlank())
					m_AttribsRequired.Insert(sAttrib);
				break;
				}

			case 'T':
				{
				m_dwCategories |= CSpaceObject::catStation;

				CString sAttrib = ParseCriteriaParam(&pPos);
				if (!sAttrib.IsBlank())
					m_AttribsRequired.Insert(sAttrib);

				m_bStructureScaleOnly = true;
				break;
				}

			case 'v':
				m_bIncludeIntangible = true;
				m_bIncludeVirtual = true;
				break;

			case 'V':
				m_bIncludeVirtual = true;
				break;

			case 'X':
				m_bTargetIsSource = true;
				break;

			case 'Y':
				m_bAngryObjectsOnly = true;
				break;

			case 'z':
				m_bSelectPlayer = true;
				break;

			case 'Z':
				m_bExcludePlayer = true;
				break;

			case '+':
			case '-':
				{
				bool bRequired = (*pPos == '+');
				bool bBinaryParam;
				CString sParam = ParseCriteriaParam(&pPos, false, &bBinaryParam);

				if (!sParam.IsBlank())
					{
					if (bRequired)
						{
						if (bBinaryParam)
							m_SpecialRequired.Insert(sParam);
						else
							m_AttribsRequired.Insert(sParam);
						}
					else
						{
						if (bBinaryParam)
							m_SpecialNotAllowed.Insert(sParam);
						else
							m_AttribsNotAllowed.Insert(sParam);
						}
					}
				break;
				}

			case '>':
			case '<':
			case '=':
				{
				char chChar = *pPos;
				pPos++;

				//	<= or >=

				int iEqualAdj;
				if (*pPos == '=')
					{
					pPos++;
					iEqualAdj = 1;
					}
				else
					iEqualAdj = 0;

				//	Get the number

				const char *pNewPos;
				int iValue = strParseInt(pPos, 0, &pNewPos);

				//	Back up one because we will increment at the bottom
				//	of the loop.

				if (pPos != pNewPos)
					pPos = pNewPos - 1;

				//	Levels

				if (chChar == '=')
					m_iEqualToLevel = iValue;
				else if (chChar == '>')
					m_iGreaterThanLevel = iValue - iEqualAdj;
				else if (chChar == '<')
					m_iLessThanLevel = iValue + iEqualAdj;

				break;
				}
			}

		pPos++;
		}
	}

void CSpaceObjectCriteria::SetSource (CSpaceObject *pSource)

//	SetSource
//
//	Sets the source.

	{
	ASSERT(pSource);

	m_pSource = pSource;

	if (m_bSourceSovereignOnly)
		m_dwSovereignUNID = (pSource->GetSovereign() ? pSource->GetSovereign()->GetUNID() : 0);

	if (m_bPerceivableOnly)
		m_iPerception = pSource->GetPerception();
	}

//	SCtx -----------------------------------------------------------------------

CSpaceObjectCriteria::SCtx::SCtx (const CSpaceObjectCriteria &Crit) :
		DistSort(Crit.GetSortOrder())

//	SCriteriaMatchCtx constructor

	{
	pBestObj = NULL;
	rBestDist2 = (Crit.MatchesNearestOnly() ? g_InfiniteDistance * g_InfiniteDistance : 0.0);

	bCalcPolar = (Crit.MatchesIntersectAngle() != -1);
	bCalcDist2 = (!bCalcPolar 
			&& (Crit.MatchesNearestOnly() 
				|| Crit.MatchesFarthestOnly() 
				|| Crit.MatchesNearerThan() 
				|| Crit.MatchesFartherThan() 
				|| Crit.MatchesPerceivableOnly()
				|| Crit.GetSort() == CSpaceObjectCriteria::sortByDistance));

	rMinRadius2 = Crit.MatchesMinRadius() * Crit.MatchesMinRadius();
	rMaxRadius2 = Crit.MatchesMaxRadius() * Crit.MatchesMaxRadius();
	}
