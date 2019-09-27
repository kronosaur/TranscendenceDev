//	CDesignTypeCriteria.cpp
//
//	CDesignTypeCriteria class
//	Copyright (c) 2015 by Kronosaur Productions, LLC.

#include "PreComp.h"

CDesignTypeCriteria::CDesignTypeCriteria (void) :
		m_dwTypeSet(0),
		m_iGreaterThanLevel(INVALID_COMPARE),
		m_iLessThanLevel(INVALID_COMPARE),
		m_bIncludeVirtual(false),
        m_bStructuresOnly(false)

//	CDesignTypeCriteria constructor

	{
	}

CString CDesignTypeCriteria::AsString (void) const

//	AsString
//
//	Outputs back to a string

	{
	int i;
	CMemoryWriteStream Output(64 * 1024);

	if (Output.Create() != NOERROR)
		return NULL_STR;

	//	Write out the typeset
	
	if (m_dwTypeSet == designSetAll)
		Output.WriteChar('*');
	else
		{
		for (i = 0; i < designCount; i++)
			{
			DWORD dwType = (1 << i);
			if (m_dwTypeSet & dwType)
				{
				switch (i)
					{
					case designItemType:
						Output.WriteChar(charItemType);
						break;

					case designItemTable:
						Output.WriteChar(charItemTable);
						break;

					case designShipClass:
						Output.WriteChar(charShipClass);
						break;

					case designOverlayType:
						Output.WriteChar(charOverlayType);
						break;

					case designSystemType:
						Output.WriteChar(charSystemType);
						break;

					case designStationType:
                        if (m_bStructuresOnly)
						    Output.WriteChar('T');
                        else
						    Output.WriteChar(charStationType);
						break;

					case designSovereign:
						Output.WriteChar(charSovereign);
						break;

					case designDockScreen:
						Output.WriteChar(charDockScreen);
						break;

					case designEffectType:
						Output.WriteChar(charEffectType);
						break;

					case designPower:
						Output.WriteChar(charPower);
						break;

					case designSpaceEnvironmentType:
						Output.WriteChar(charSpaceEnvironmentType);
						break;

					case designShipTable:
						Output.WriteChar(charShipTable);
						break;

					case designAdventureDesc:
						Output.WriteChar(charAdventureDesc);
						break;

					case designGlobals:
						Output.WriteChar(charGlobals);
						break;

					case designImage:
						Output.WriteChar(charImage);
						break;

					case designMusic:
						Output.WriteChar(charMusic);
						break;

					case designMissionType:
						Output.WriteChar(charMissionType);
						break;

					case designSound:
						Output.WriteChar(charSound);
						break;

					case designSystemTable:
						Output.WriteChar(charSystemTable);
						break;

					case designSystemMap:
						Output.WriteChar(charSystemMap);
						break;

					case designEconomyType:
						Output.WriteChar(charEconomyType);
						break;

					case designTemplateType:
						Output.WriteChar(charTemplateType);
						break;

					case designGenericType:
						Output.WriteChar(charGenericType);
						break;

					case designImageComposite:
						Output.WriteChar(charImageComposite);
						break;
					}
				}
			}
		}

	//	Virtual

	if (m_bIncludeVirtual)
		Output.WriteChar('V');

	//	Write out all the attributes

	for (i = 0; i < m_sRequire.GetCount(); i++)
		Output.WriteChars(strPatternSubst(CONSTLIT(" +%s;"), m_sRequire[i]));

	for (i = 0; i < m_sRequireSpecial.GetCount(); i++)
		Output.WriteChars(strPatternSubst(CONSTLIT(" +%s;"), m_sRequireSpecial[i]));

	for (i = 0; i < m_sExclude.GetCount(); i++)
		Output.WriteChars(strPatternSubst(CONSTLIT(" -%s;"), m_sExclude[i]));

	for (i = 0; i < m_sExcludeSpecial.GetCount(); i++)
		Output.WriteChars(strPatternSubst(CONSTLIT(" -%s;"), m_sExcludeSpecial[i]));

	//	Level

	if (m_iGreaterThanLevel != INVALID_COMPARE)
		Output.WriteChars(strPatternSubst(CONSTLIT(" >%d"), m_iGreaterThanLevel));

	if (m_iLessThanLevel != INVALID_COMPARE)
		Output.WriteChars(strPatternSubst(CONSTLIT(" >%d"), m_iLessThanLevel));

	//	Done

	return CString(Output.GetPointer(), Output.GetLength());
	}

bool CDesignTypeCriteria::MatchesLevel (int iMinLevel, int iMaxLevel) const

//	MatchesLevel
//
//	Returns true if we match the level

	{
	if (m_iGreaterThanLevel != INVALID_COMPARE 
			&& iMaxLevel <= m_iGreaterThanLevel)
		return false;

	if (m_iLessThanLevel != INVALID_COMPARE 
			&& iMinLevel >= m_iLessThanLevel)
		return false;

	return true;
	}

ALERROR CDesignTypeCriteria::ParseCriteria (const CString &sCriteria, CDesignTypeCriteria *retCriteria)

//	ParseCriteria
//
//	Parses the criteria and initializes retCriteria

	{
	//	Initialize

	retCriteria->m_dwTypeSet = 0;
	retCriteria->m_iGreaterThanLevel = INVALID_COMPARE;
	retCriteria->m_iLessThanLevel = INVALID_COMPARE;
	retCriteria->m_bIncludeVirtual = false;
    retCriteria->m_bStructuresOnly = false;

	//	Parse

	const char *pPos = sCriteria.GetPointer();
	while (*pPos != '\0')
		{
		switch (*pPos)
			{
			case '*':
				retCriteria->m_dwTypeSet = designSetAll;
				break;

			case charAdventureDesc:
				retCriteria->m_dwTypeSet |= (1 << designAdventureDesc);
				break;

			case charItemTable:
				retCriteria->m_dwTypeSet |= (1 << designItemTable);
				break;

			case charEffectType:
				retCriteria->m_dwTypeSet |= (1 << designEffectType);
				break;

			case charDockScreen:
				retCriteria->m_dwTypeSet |= (1 << designDockScreen);
				break;

			case charSpaceEnvironmentType:
				retCriteria->m_dwTypeSet |= (1 << designSpaceEnvironmentType);
				break;

			case charEconomyType:
				retCriteria->m_dwTypeSet |= (1 << designEconomyType);
				break;

			case charOverlayType:
				retCriteria->m_dwTypeSet |= (1 << designOverlayType);
				break;

			case charGenericType:
				retCriteria->m_dwTypeSet |= (1 << designGenericType);
				break;

			case charGlobals:
				retCriteria->m_dwTypeSet |= (1 << designGlobals);
				break;

			case charShipTable:
				retCriteria->m_dwTypeSet |= (1 << designShipTable);
				break;

			case charItemType:
				retCriteria->m_dwTypeSet |= (1 << designItemType);
				break;

			case charImage:
				retCriteria->m_dwTypeSet |= (1 << designImage);
				break;

			case charImageComposite:
				retCriteria->m_dwTypeSet |= (1 << designImageComposite);
				break;

			case charMissionType:
				retCriteria->m_dwTypeSet |= (1 << designMissionType);
				break;

			case charPower:
				retCriteria->m_dwTypeSet |= (1 << designPower);
				break;

			case charShipClass:
				retCriteria->m_dwTypeSet |= (1 << designShipClass);
				break;

			case charStationType:
				retCriteria->m_dwTypeSet |= (1 << designStationType);
				break;

			case charMusic:
				retCriteria->m_dwTypeSet |= (1 << designMusic);
				break;

			case charSound:
				retCriteria->m_dwTypeSet |= (1 << designSound);
				break;

			case charSovereign:
				retCriteria->m_dwTypeSet |= (1 << designSovereign);
				break;

			case charSystemTable:
				retCriteria->m_dwTypeSet |= (1 << designSystemTable);
				break;

			case charSystemType:
				retCriteria->m_dwTypeSet |= (1 << designSystemType);
				break;

			case charSystemMap:
				retCriteria->m_dwTypeSet |= (1 << designSystemMap);
				break;

			case charTemplateType:
				//	We don't support enumerating template types
				break;

			case 'L':
				{
				int iHigh;
				int iLow;

				if (ParseCriteriaParamLevelRange(&pPos, &iLow, &iHigh))
					{
					if (iHigh == -1)
						{
						retCriteria->m_iGreaterThanLevel = iLow - 1;
						retCriteria->m_iLessThanLevel = iLow + 1;
						}
					else
						{
						retCriteria->m_iGreaterThanLevel = iLow - 1;
						retCriteria->m_iLessThanLevel = iHigh + 1;
						}
					}

				break;
				}

            case 'T':
                retCriteria->m_dwTypeSet |= (1 << designStationType);
                retCriteria->m_bStructuresOnly = true;
                break;

			case 'V':
				retCriteria->m_bIncludeVirtual = true;
				break;

			case '+':
			case '-':
				{
				bool bRequired = (*pPos == '+');
				bool bBinaryParam;
				CString sParam = ParseCriteriaParam(&pPos, false, &bBinaryParam);

				if (bRequired)
					{
					if (bBinaryParam)
						retCriteria->m_sRequireSpecial.Insert(sParam);
					else
						retCriteria->m_sRequire.Insert(sParam);
					}
				else
					{
					if (bBinaryParam)
						retCriteria->m_sExcludeSpecial.Insert(sParam);
					else
						retCriteria->m_sExclude.Insert(sParam);
					}
				break;
				}

			case '=':
			case '>':
			case '<':
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

				//	Is this price?

				char comparison;
				if (*pPos == '$' || *pPos == '#')
					comparison = *pPos++;
				else
					comparison = '\0';

				//	Get the number

				const char *pNewPos;
				int iValue = strParseInt(pPos, 0, &pNewPos);

				//	Back up one because we will increment at the bottom
				//	of the loop.

				if (pPos != pNewPos)
					pPos = pNewPos - 1;

				//	Level limits

				if (chChar == '=')
					{
					retCriteria->m_iGreaterThanLevel = iValue - 1;
					retCriteria->m_iLessThanLevel = iValue + 1;
					}
				else if (chChar == '>')
					retCriteria->m_iGreaterThanLevel = iValue - iEqualAdj;
				else if (chChar == '<')
					retCriteria->m_iLessThanLevel = iValue + iEqualAdj;

				break;
				}
			}

		pPos++;
		}

	return NOERROR;
	}

void CDesignTypeCriteria::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Reads from a stream

	{
	CString sCriteria;
	sCriteria.ReadFromStream(Ctx.pStream);
	ParseCriteria(sCriteria, this);
	}

void CDesignTypeCriteria::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Writes to a stream

	{
	//	Convert to a string and save

	CString sCriteria = AsString();
	sCriteria.WriteToStream(pStream);
	}
