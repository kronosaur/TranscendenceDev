//	CUserSettings.cpp
//
//	CUserSettings class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define ATTRIB_NO_ARGS							CONSTLIT("noArgs")

#define EXTENSION_FOLDER_TAG					CONSTLIT("ExtensionFolder")
#define EXTENSIONS_TAG							CONSTLIT("Extensions")
#define OPTION_TAG								CONSTLIT("Option")

#define NAME_ATTRIB								CONSTLIT("name")
#define PATH_ATTRIB								CONSTLIT("path")
#define VALUE_ATTRIB							CONSTLIT("value")

bool CUserSettings::GetValueBoolean (const CString &sID) const

//	GetValueBoolean
//
//	Returns a boolean option

	{
	const SOption *pOption = m_Options.GetAt(sID);
	if (pOption == NULL)
		return false;

	switch (pOption->iType)
		{
		case optionBoolean:
			return pOption->bValue;

		case optionInteger:
			return (pOption->iValue > 0);

		case optionString:
			return (!pOption->sValue.IsBlank());

		default:
			ASSERT(false);
			return false;
		}
	}

ALERROR CUserSettings::Load (CHumanInterface &HI, const CString &sFilespec, SOptionDef *pDefinitions, CString *retsError)

//	Load
//
//	Load settings from the given file location.

	{
	ALERROR error;
	int i;

	//	Initialize from defaults

	m_Options.DeleteAll();

	SOptionDef *pDef = pDefinitions;
	while (pDef->pszID)
		{
		CString sID = CString(pDef->pszID, -1, true);
		SOption *pNewOption = m_Options.SetAt(sID);

		pNewOption->sID = sID;
		pNewOption->iType = pDef->iType;
		pNewOption->sDefault = CString(pDef->pszDefault, -1, true);
		pNewOption->dwFlags = pDef->dwFlags;

		SetValue(pNewOption, pNewOption->sDefault, true);

		pDef++;
		}

	//	Look for a file in the current directory and see if it is writable. If
	//	not, then look in AppData. We remember the place where we found a valid
	//	file as our AppData root (and we base other directories off that).

	if (pathIsWritable(sFilespec))
		{
		//	AppData is current directory
		m_sUserRoot = NULL_STR;
		}
	else
		{
		m_sUserRoot = pathAddComponent(pathGetSpecialFolder(folderAppData), HI.GetOptions().sAppData);
		if (!pathCreate(m_sUserRoot)
				|| !pathIsWritable(m_sUserRoot))
			{
			*retsError = strPatternSubst(CONSTLIT("Unable to write to AppData folder: %s"), m_sUserRoot);
			return ERR_FAIL;
			}
		}

	//	Settings file

	CString sSettingsFilespec = pathAddComponent(m_sUserRoot, sFilespec);

	//	Load XML

	CFileReadBlock DataFile(sSettingsFilespec);
	CXMLElement *pData;
	CString sError;

	if (error = CXMLElement::ParseXML(DataFile, CXMLElement::SParseOptions(), &pData, retsError))
		{
		//	ERR_NOTFOUND means that we couldn't find the Settings.xml
		//	file. In that case, initialize from defaults

		if (error == ERR_NOTFOUND)
			{
			m_bModified = true;
			return NOERROR;
			}

		//	Otherwise, it means that we got an error parsing the file.
		//	Return the error, but leave the settings initialized to defaults
		//	(We should be OK to continue, even with an error).

		else
			{
			m_bModified = false;
			return error;
			}
		}

	//	Initialize to unmodified (as we load settings we might change this)

	m_bModified = false;

	//	Loop over all elements

	for (i = 0; i < pData->GetContentElementCount(); i++)
		{
		CXMLElement *pItem = pData->GetContentElement(i);

		if (strEquals(pItem->GetTag(), OPTION_TAG))
			{
			SOption *pOption = m_Options.GetAt(pItem->GetAttribute(NAME_ATTRIB));
			if (pOption == NULL)
				{
				::kernelDebugLogPattern("Unknown option: %s", pItem->GetAttribute(NAME_ATTRIB));
				continue;
				}

			SetValue(pOption, pItem->GetAttribute(VALUE_ATTRIB), true);
			}
		else if (strEquals(pItem->GetTag(), EXTENSION_FOLDER_TAG))
			{
			CString sFolder;
			if (pItem->FindAttribute(PATH_ATTRIB, &sFolder))
				m_ExtensionFolders.Insert(sFolder);
			}
		else if (strEquals(pItem->GetTag(), EXTENSIONS_TAG))
			{
			if (error = m_Extensions.ReadFromXML(pItem))
				return error;
			}
		}

	//	Done

	delete pData;

	return NOERROR;
	}

ALERROR CUserSettings::LoadCommandLine (char *pszCmdLine, CString *retsError)

//	LoadCommandLine
//
//	Applies any command line arguments

	{
	ALERROR error;
	int i;

	char *argv[2];
	argv[0] = "Application";
	argv[1] = pszCmdLine;
	CXMLElement *pCmdLine;
	if (error = CreateXMLElementFromCommandLine(2, argv, &pCmdLine))
		return error;

	//	Loop over all command line arguments

	for (i = 0; i < pCmdLine->GetAttributeCount(); i++)
		{
		CString sID = pCmdLine->GetAttributeName(i);

		//	Skip special attribute

		if (strEquals(sID, ATTRIB_NO_ARGS))
			continue;

		//	Load the options

		SOption *pOption = m_Options.GetAt(sID);
		if (pOption == NULL)
			{
			if (retsError) *retsError = strPatternSubst(CONSTLIT("Unknown command line switch: %s."), sID);
			delete pCmdLine;
			return ERR_FAIL;
			}

		//	Do not set value because this is a temporary override--we do not save it.

		SetValue(pOption, pCmdLine->GetAttribute(i));
		}

	//	Done

	delete pCmdLine;

	return NOERROR;
	}

void CUserSettings::SetValue (const CString &sID, const CString &sValue, bool bSetSettings)

//	SetValue
//
//	Sets the boolean, integer, or string value of the option

	{
	SOption *pOption = m_Options.GetAt(sID);
	if (pOption == NULL)
		return;

	SetValue(pOption, sValue, bSetSettings);
	}

void CUserSettings::SetValue (SOption *pOption, const CString &sValue, bool bSetSettings)

//	SetValue
//
//	Sets the boolean, integer, or string value of the option

	{
	switch (pOption->iType)
		{
		case optionBoolean:
			pOption->bValue = (strEquals(sValue, CONSTLIT("true")) || strEquals(sValue, CONSTLIT("on")) || strEquals(sValue, CONSTLIT("yes")));
			break;

		case optionInteger:
			pOption->iValue = strToInt(sValue, 0);
			break;

		case optionString:
			pOption->sValue = sValue;
			break;

		default:
			ASSERT(false);
		}

	//	Set the settings value, if appropriate

	if (bSetSettings)
		pOption->sSettingsValue = sValue;
	}

