//	CMultiverseCatalogEntry.cpp
//
//	CMultiverseCatalogEntry Class
//	Copyright (c) 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define FIELD_DEPENDENCIES						CONSTLIT("dependencies")
#define FIELD_DESCRIPTION						CONSTLIT("description")
#define FIELD_FILE_TDB							CONSTLIT("fileTDB")
#define FIELD_LICENSE_TYPE						CONSTLIT("licenseType")
#define FIELD_NAME								CONSTLIT("name")
#define FIELD_RELEASE							CONSTLIT("release")
#define FIELD_RESOURCES							CONSTLIT("resources")
#define FIELD_TYPE								CONSTLIT("type")
#define FIELD_UNID								CONSTLIT("unid")
#define FIELD_VERSION							CONSTLIT("$version")

#define LICENSE_AUTO							CONSTLIT("auto")
#define LICENSE_CORE							CONSTLIT("core")
#define LICENSE_FREE							CONSTLIT("free")
#define LICENSE_PAID							CONSTLIT("paid")

#define STR_UNID_PREFIX							CONSTLIT("Transcendence:")

#define TYPE_ADVENTURE							CONSTLIT("transcendenceAdventure")
#define TYPE_LIBRARY							CONSTLIT("transcendenceLibrary")

#define ERR_INVALID_UNID						CONSTLIT("Invalid UNID for catalog entry.")

ALERROR CMultiverseCatalogEntry::CreateFromJSON (const CJSONValue &Entry, CMultiverseCatalogEntry **retpEntry, CString *retsResult)

//	CreateFromJSON
//
//	Creates a new entry from a JSON value.

	{
	int i;

	CMultiverseCatalogEntry *pNewEntry = new CMultiverseCatalogEntry;

	pNewEntry->m_sUNID = Entry.GetElement(FIELD_UNID).AsString();
	if (pNewEntry->m_sUNID.IsBlank())
		{
		delete pNewEntry;
		*retsResult = ERR_INVALID_UNID;
		return ERR_FAIL;
		}

	pNewEntry->m_dwRelease = (DWORD)Entry.GetElement(FIELD_RELEASE).AsInt32();
	pNewEntry->m_dwVersion = (DWORD)Entry.GetElement(FIELD_VERSION).AsInt32();

	pNewEntry->m_sName = Entry.GetElement(FIELD_NAME).AsString();
	pNewEntry->m_sDesc = Entry.GetElement(FIELD_DESCRIPTION).AsString();

	//	Parse the fully qualified UNID and get just the hex UNID.

	pNewEntry->m_dwUNID = ParseFullUNID(pNewEntry->m_sUNID);
	if (pNewEntry->m_dwUNID == 0)
		{
		delete pNewEntry;
		*retsResult = ERR_INVALID_UNID;
		return ERR_FAIL;
		}

	//	Get the type

	CString sType = Entry.GetElement(FIELD_TYPE).AsString();
	if (strEquals(sType, TYPE_ADVENTURE))
		pNewEntry->m_iType = extAdventure;
	else if (strEquals(sType, TYPE_LIBRARY))
		pNewEntry->m_iType = extLibrary;
	else
		pNewEntry->m_iType = extExtension;

	//	Get the license type

	CString sLicense = Entry.GetElement(FIELD_LICENSE_TYPE).AsString();
	if (strEquals(sLicense, LICENSE_AUTO))
		pNewEntry->m_iLicenseType = licenseAuto;
	else if (strEquals(sLicense, LICENSE_CORE))
		pNewEntry->m_iLicenseType = licenseCore;
	else if (strEquals(sLicense, LICENSE_FREE))
		pNewEntry->m_iLicenseType = licenseFree;
	else if (strEquals(sLicense, LICENSE_PAID))
		pNewEntry->m_iLicenseType = licensePaid;
	else
		pNewEntry->m_iLicenseType = licenseUnknown;

	//	Get the TDB file (only if not core)

	if (pNewEntry->m_iLicenseType != licenseCore)
		{
		if (pNewEntry->m_TDBFile.InitFromJSON(Entry.GetElement(FIELD_FILE_TDB), retsResult) != NOERROR)
			{
			delete pNewEntry;
			return ERR_FAIL;
			}
		}

	//	Library dependencies

	const CJSONValue &Dependencies = Entry.GetElement(FIELD_DEPENDENCIES);
	if (Dependencies.GetType() == CJSONValue::typeArray)
		{
		pNewEntry->m_Dependencies.InsertEmpty(Dependencies.GetCount());

		for (i = 0; i < Dependencies.GetCount(); i++)
			{
			pNewEntry->m_Dependencies[i] = ParseFullUNID(Dependencies.GetElement(i));
			}
		}

	//	Get the resources

	const CJSONValue &Resources = Entry.GetElement(FIELD_RESOURCES);
	if (Resources.GetType() == CJSONValue::typeArray)
		{
		pNewEntry->m_Resources.InsertEmpty(Resources.GetCount());

		for (i = 0; i < Resources.GetCount(); i++)
			{
			if (pNewEntry->m_Resources[i].InitFromJSON(Resources.GetElement(i), retsResult) != NOERROR)
				{
				delete pNewEntry;
				return ERR_FAIL;
				}
			}
		}

	//	Initialize status

	pNewEntry->m_iStatus = statusUnknown;

	//	Done

	*retpEntry = pNewEntry;
	return NOERROR;
	}

ALERROR CMultiverseCatalogEntry::CreateBasicEntry (const SEntryCreate &Create, CMultiverseCatalogEntry **retpEntry)

//	CreateBasicEntry
//
//	Creates a hard-coded catalog entry for Steam.

	{
	CMultiverseCatalogEntry *pNewEntry = new CMultiverseCatalogEntry;

	pNewEntry->m_dwUNID = Create.dwUNID;
	pNewEntry->m_sUNID = strPatternSubst(CONSTLIT("Transcendence:%08x"), Create.dwUNID);
	pNewEntry->m_dwRelease = 1;
	pNewEntry->m_dwVersion = 1;

	pNewEntry->m_sName = Create.sName;
	pNewEntry->m_sDesc = Create.sDesc;
	pNewEntry->m_iType = Create.iType;
	pNewEntry->m_iLicenseType = Create.iLicense;

	//	Initialize status

	pNewEntry->m_iStatus = statusUnknown;

	//	Done

	*retpEntry = pNewEntry;
	return NOERROR;
	}

bool CMultiverseCatalogEntry::GetLibrariesUsed (TSortMap<DWORD, bool> &retLibrariesUsed) const

//	GetLibrariesUsed
//
//	Adds the libraries used by this entry to the map. Returns FALSE if we don't
//	use any libraries.

	{
	if (m_Dependencies.GetCount() == 0)
		return false;

	for (int i = 0; i < m_Dependencies.GetCount(); i++)
		retLibrariesUsed.SetAt(m_Dependencies[i], true);

	return true;
	}

bool CMultiverseCatalogEntry::IsValid (void)

//	IsValid
//
//	Returns TRUE if the entry is a valid entry (has all necessary fields).

	{
	//	We must have a TDB file (unless this is a core entry)

	if (m_TDBFile.IsEmpty()
			&& m_iLicenseType != licenseCore)
		return false;

	return true;
	}

DWORD CMultiverseCatalogEntry::ParseFullUNID (const CJSONValue &Value)

//	ParseFullUNID
//
//	Parses a Multiverse UNID of the form:
//
//	Transcendence:0x00000000
//
//	If parsing fails, we return 0.

	{
	CString sUNID = Value.AsString();
	if (sUNID.IsBlank())
		return 0;

	if (!strStartsWith(sUNID, STR_UNID_PREFIX))
		return 0;

	return strParseIntOfBase(sUNID.GetASCIIZPointer() + STR_UNID_PREFIX.GetLength(), 16, 0);
	}

void CMultiverseCatalogEntry::SetIcon (CG32bitImage *&pImage)

//	SetIcon
//
//	Sets the icon. We take ownership of the image

	{
	m_pIcon.TakeHandoff(pImage);
	pImage = NULL;
	}

CString CMultiverseCatalogEntry::StatusAsString (ELocalStatus iStatus)

//	StatusAsString
//
//	Returns status as a string.

	{
	switch (iStatus)
		{
		case statusNotAvailable:
			return CONSTLIT("notAvailable");

		case statusDownloadInProgress:
			return CONSTLIT("downloading");
			
		case statusLoaded:
			return CONSTLIT("loaded");
			
		case statusCorrupt:
			return CONSTLIT("corrupt");

		case statusPlayerDisabled:
			return CONSTLIT("disabled");

		default:
			return CONSTLIT("Unknown");
		}
	}
