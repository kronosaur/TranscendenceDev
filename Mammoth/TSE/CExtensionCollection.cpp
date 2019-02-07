//	CExtensionCollection.cpp
//
//	CExtensionCollection class
//	Copyright (c) 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define LIBRARY_TAG									CONSTLIT("Library")
#define TRANSCENDENCE_ADVENTURE_TAG					CONSTLIT("TranscendenceAdventure")
#define TRANSCENDENCE_EXTENSION_TAG					CONSTLIT("TranscendenceExtension")
#define TRANSCENDENCE_LIBRARY_TAG					CONSTLIT("TranscendenceLibrary")
#define TRANSCENDENCE_MODULE_TAG					CONSTLIT("TranscendenceModule")

#define FILENAME_ATTRIB								CONSTLIT("filename")
#define OPTIONAL_ATTRIB								CONSTLIT("optional")
#define RELEASE_ATTRIB								CONSTLIT("release")
#define UNID_ATTRIB									CONSTLIT("unid")

#define EXTENSIONS_FILTER							CONSTLIT("*.*")
#define EXTENSION_TDB								CONSTLIT("tdb")
#define EXTENSION_XML								CONSTLIT("xml")

#define FILE_AMERICA								CONSTLIT("America")
#define FILE_TRANSCENDENCE							CONSTLIT("Transcendence")

#define FILESPEC_COLLECTION_FOLDER					CONSTLIT("Collection")
#define FILESPEC_EXTENSIONS_FOLDER					CONSTLIT("Extensions")

#define ERR_CANT_MOVE								CONSTLIT("%s: Unable to move to %s.")

const int DIGEST_SIZE = 20;
static BYTE g_BaseFileDigest[] =
	{
     23, 171, 205, 171,  76,  81,  63,  51, 175,  24,
     47, 134, 222,  96, 227, 195,  50,  62, 195, 222,
	};

class CLibraryResolver : public IXMLParserController
	{
	public:
		CLibraryResolver (CExtensionCollection &Extensions) : 
				m_Extensions(Extensions),
				m_bReportError(false)
			{ }

		void AddDefaults (CExtension *pExtension);
		inline void AddLibrary (CExtension *pLibrary) { m_Tables.Insert(pLibrary->GetEntities()); }
		inline void AddTable (IXMLParserController *pTable) { m_Tables.Insert(pTable); }
		inline void ReportLibraryErrors (void) { m_bReportError = true; }

		//	IXMLParserController virtuals
		virtual ALERROR OnOpenTag (CXMLElement *pElement, CString *retsError);
		virtual CString ResolveExternalEntity (const CString &sName, bool *retbFound = NULL);

	private:
		ALERROR AddLibrary (DWORD dwUNID, DWORD dwRelease, bool bOptional, CString *retsError);

		CExtensionCollection &m_Extensions;

		TArray<IXMLParserController *> m_Tables;
		bool m_bReportError;				//	If TRUE, we report errors if we fail to load a library
	};

CExtensionCollection::CExtensionCollection (void) :
		m_iGame(gameUnknown),
		m_sCollectionFolder(FILESPEC_COLLECTION_FOLDER),
		m_pBase(NULL),
		m_bReloadNeeded(true),
		m_bLoadedInDebugMode(false)

//	CExtensionCollection constructor

	{
	}

CExtensionCollection::~CExtensionCollection (void)

//	CExtensionCollection destructor

	{
	CleanUp();
	}

ALERROR CExtensionCollection::AddCompatibilityLibrary (CExtension *pAdventure, const TArray<CExtension *> &Extensions, DWORD dwFlags, const TArray<CExtension *> &Compatibility, TArray<CExtension *> *retList, CString *retsError)

//	AddCompatibilityLibrary
//
//	Examines the adventure and extensions and, if necessary, adds the compatibility
//	library to the bind list.

	{
	int i;

	//	In debug mode we output which extension required us to include this library.

	bool bDebugMode = ((dwFlags & FLAG_DEBUG_MODE) == FLAG_DEBUG_MODE);
	CString sExtensionName;

	bool bNeedLibrary = false;

	//	If we're forcing include, then do it.

	if (dwFlags & FLAG_FORCE_COMPATIBILITY_LIBRARY)
		{
		bNeedLibrary = true;
		sExtensionName = CONSTLIT("save file");
		}

	//	See if the adventure needs it

	else if (pAdventure->UsesCompatibilityLibrary())
		{
		bNeedLibrary = true;
		sExtensionName = pAdventure->GetName();
		}

	//	Otherwise, see if any extensions need it

	else
		{
		for (i = 0; i < Extensions.GetCount(); i++)
			if (Extensions[i]->UsesCompatibilityLibrary())
				{
				sExtensionName = Extensions[i]->GetName();
				bNeedLibrary = true;
				break;
				}
		}

	if (!bNeedLibrary)
		return NOERROR;

	if (bDebugMode)
		::kernelDebugLogPattern("Adding compatibility library because of %s.", sExtensionName);

	//	Add the library

	CExtension *pLibrary;
	if (!FindBestExtension(DEFAULT_COMPATIBILITY_LIBRARY_UNID, 1, dwFlags, &pLibrary))
		{
		if (retsError) *retsError = strPatternSubst(CONSTLIT("Unable to find compatibility library: %08x"), DEFAULT_COMPATIBILITY_LIBRARY_UNID);
		return ERR_FAIL;
		}

	if (AddToBindList(pLibrary, dwFlags, Compatibility, retList, retsError) != NOERROR)
		return ERR_FAIL;

	return NOERROR;
	}

void CExtensionCollection::AddOrReplace (CExtension *pExtension)

//	AddOrReplace
//
//	Adds the given extension to the main array and all indices. If this replaces
//	an existing extension (by filespec), the previous one is deleted.

	{
	CSmartLock Lock(m_cs);

	//	First see if we're replacing this extension (we only check for non-base
	//	extensions).

	CExtension *pPreviousExtension;
	if (pExtension->GetFolderType() != CExtension::folderBase
			&& m_ByFilespec.Find(pExtension->GetFilespec(), &pPreviousExtension))
		{
		//	Remove from UNID index

		int iPos;
		if (m_ByUNID.FindPos(pPreviousExtension->GetUNID(), &iPos))
			{
			TArray<CExtension *> &List = m_ByUNID.GetValue(iPos);

			int iIndex;
			if (List.Find(pPreviousExtension, &iIndex))
				List.Delete(iIndex);
			}

		//	Remove from main list

		int iIndex;
		if (m_Extensions.Find(pPreviousExtension, &iIndex))
			m_Extensions.Delete(iIndex);

		//	Add to deleted list. We can't delete outright because its 
		//	structures may have been bound by CDesignCollection.

		pPreviousExtension->SetDeleted();
		m_Deleted.Insert(pPreviousExtension);
		::kernelDebugLogPattern("Replaced extension: %s", pPreviousExtension->GetFilespec());
		}

	//	Insert

	m_Extensions.Insert(pExtension);

	//	Base index

	if (pExtension->GetUNID() == 0)
		m_pBase = pExtension;

	//	UNID index

	TArray<CExtension *> *pList = m_ByUNID.SetAt(pExtension->GetUNID());
	pList->Insert(pExtension);

	//	Filespec index
	//
	//	NOTE: We don't add base extensions to the list because otherwise they 
	//	would conflict. (E.g., Transcendence.xml could have multiple adventure 
	//	extensions).

	if (pExtension->GetFolderType() != CExtension::folderBase)
		m_ByFilespec.SetAt(pExtension->GetFilespec(), pExtension);
	}

ALERROR CExtensionCollection::AddToBindList (CExtension *pExtension, DWORD dwFlags, const TArray<CExtension *> &Compatibility, TArray<CExtension *> *retList, CString *retsError)

//	AddToBindList
//
//	Recursively adds the extension and any dependencies to the bind list.
//	(We make sure that we don't add the same extension twice.)

	{
	CSmartLock Lock(m_cs);
	ALERROR error;
	int i;

	//	If we're already marked, then we're already in the list, so we don't
	//	need to continue.

	if (pExtension->IsMarked())
		return NOERROR;

	bool bDebugMode = ((dwFlags & FLAG_DEBUG_MODE) == FLAG_DEBUG_MODE);

	//	Mark now in case there is a circular dependency (in that case, we will
	//	ignore the circular dependency.)

	pExtension->SetMarked();

	//	If the extension is already disabled (probably because it is a later version)
	//	then we exclude it.

	if (pExtension->IsDisabled() || m_DisabledExtensions.Find(pExtension->GetUNID()))
		return NOERROR;

	//	Create load options

	CLibraryResolver Resolver(*this);
	Resolver.AddDefaults(pExtension);
	Resolver.ReportLibraryErrors();

	CExtension::SLoadOptions LoadOptions;
	LoadOptions.bNoResources = ((dwFlags & FLAG_NO_RESOURCES) == FLAG_NO_RESOURCES);
	LoadOptions.bNoDigestCheck = ((dwFlags & FLAG_NO_COLLECTION_CHECK) == FLAG_NO_COLLECTION_CHECK);

	//	Make sure the extension is loaded completely.

	if (error = pExtension->Load(CExtension::loadComplete, &Resolver, LoadOptions, retsError))
		return error;

	//	Now add any libraries used by this extension to the list.

	for (i = 0; i < pExtension->GetLibraryCount(); i++)
		{
		const CExtension::SLibraryDesc &LibraryDesc = pExtension->GetLibrary(i);

		//	Find the extension

		CExtension *pLibrary;
		if (!FindBestExtension(LibraryDesc.dwUNID, LibraryDesc.dwRelease, dwFlags, &pLibrary))
			{
			//	If this is an optional library, then it is OK if we cannot find it.

			if (LibraryDesc.bOptional)
				;

			//	Otherwise, disable the extension with the appropriate message,
			//	but otherwise continue loading.

			else
				pExtension->SetDisabled(strPatternSubst(CONSTLIT("Cannot find library: %08x"), LibraryDesc.dwUNID));

			continue;
			}

		//	If the library is disabled, then the extension is also disabled.

		if (pLibrary->IsDisabled())
			{
			pExtension->SetDisabled(strPatternSubst(CONSTLIT("Required library disabled: %s (%08x)"), pLibrary->GetName(), pLibrary->GetUNID()));
			continue;
			}

		//	Make sure it is a library

		if (pLibrary->GetType() != extLibrary)
			{
			*retsError = strPatternSubst(CONSTLIT("UNID referenced by %s (%08x) is not a library: %08x"), pExtension->GetName(), pExtension->GetUNID(), LibraryDesc.dwUNID);
			return ERR_FAIL;
			}

		//	Add it to the bind list (recursively)

		if (error = AddToBindList(pLibrary, dwFlags, Compatibility, retList, retsError))
			return error;
		}

	//	Check to see if there are any compatibility libraries that we need to include

	for (i = 0; i < Compatibility.GetCount(); i++)
		if (pExtension->GetAPIVersion() <= Compatibility[i]->GetAutoIncludeAPIVersion()
				&& pExtension->GetUNID() != Compatibility[i]->GetUNID())
			{
			if (error = AddToBindList(Compatibility[i], dwFlags, Compatibility, retList, retsError))
				return error;
			}

	//	Finally add the extension itself.

	if (!pExtension->IsDisabled())
		{
		retList->Insert(pExtension);

#ifdef DEBUG_LOAD_EXTENSIONS
		if (bDebugMode)
			::kernelDebugLogPattern("Adding: %s.", pExtension->GetName());
#endif
		}

	//	Success.

	return NOERROR;
	}

void CExtensionCollection::CleanUp (void)

//	CleanUp
//
//	Clean up and free data

	{
	int i;

	for (i = 0; i < m_Extensions.GetCount(); i++)
		delete m_Extensions[i];

	m_Extensions.DeleteAll();

	FreeDeleted();
	}

void CExtensionCollection::ClearAllMarks (void)

//	ClearAllMarks
//
//	Clear marks on all extensions.

	{
	CSmartLock Lock(m_cs);
	int i;

	for (i = 0; i < m_Extensions.GetCount(); i++)
		m_Extensions[i]->SetMarked(false);
	}

int CExtensionCollection::Compare (CExtension *pExt1, CExtension *pExt2, bool bDebugMode)

//	Compare
//
//	If pExt1 overrides pExt2 then we return 1. Otherwise we return -1.
//	(If neither overrides the other we return 0).

	{
	//	If either is NULL then non-NULL wins

	if (pExt1 == NULL && pExt2 == NULL)
		return 0;
	else if (pExt1 == NULL)
		return -1;
	else if (pExt2 == NULL)
		return 1;

	//	Disabled adventures always lose

	if (!pExt1->IsDisabled() && pExt2->IsDisabled())
		return 1;

	else if (pExt1->IsDisabled() && !pExt2->IsDisabled())
		return -1;

	//	A later release always wins

	else if (pExt1->GetRelease() > pExt2->GetRelease())
		return 1;

	//	A lesser release always loses

	else if (pExt1->GetRelease() < pExt2->GetRelease())
		return -1;

	//	If we're the same release then we must be in different folders. In that
	//	case see which folder wins

	else
		{
		//	If the same folder, then we're even

		if (pExt1->GetFolderType() == pExt2->GetFolderType())
			return 0;

		//	In debug mode, the Extensions folder wins

		else if (bDebugMode)
			{
			if (pExt1->GetFolderType() == CExtension::folderExtensions)
				return 1;
			else if (pExt2->GetFolderType() == CExtension::folderExtensions)
				return -1;
			else
				return 0;
			}

		//	In non-debug, the Collection folder wins

		else
			{
			if (pExt1->GetFolderType() == CExtension::folderCollection)
				return 1;
			else if (pExt2->GetFolderType() == CExtension::folderCollection)
				return -1;
			else
				return 0;
			}
		}
	}

ALERROR CExtensionCollection::ComputeAvailableAdventures (DWORD dwFlags, TArray<CExtension *> *retList, CString *retsError)

//	ComputeAvailableAdventures
//
//	Computes a list of all available adventures.

	{
	CSmartLock Lock(m_cs);

	int i, j;

	//	Initialize

	bool bDebugMode = ((dwFlags & FLAG_DEBUG_MODE) == FLAG_DEBUG_MODE);
	bool bRegisteredOnly = ((dwFlags & FLAG_REGISTERED_ONLY) == FLAG_REGISTERED_ONLY);
	retList->DeleteAll();

	//	Loop by UNID because we allow at most one of each UNID.

	for (i = 0; i < m_ByUNID.GetCount(); i++)
		{
		TArray<CExtension *> &ExtensionList = m_ByUNID.GetValue(i);
		if (ExtensionList.GetCount() == 0)
			continue;

		//	We only consider adventures (this property does not change across
		//	releases, so we only need to check once).

		if (ExtensionList[0]->GetType() != extAdventure)
			continue;

		//	If manually disabled, then exclude

		if (m_DisabledExtensions.Find(ExtensionList[0]->GetUNID()))
			continue;

		//	Out of all the releases, select the latest version.

		CExtension *pBest = NULL;
		for (j = 0; j < ExtensionList.GetCount(); j++)
			{
			//	If this is debug only and we're not in debug mode then skip.

			if (ExtensionList[j]->IsDebugOnly() && !bDebugMode)
				continue;

			//	Exclude unregistered if we want registered only

			if (bRegisteredOnly && !ExtensionList[j]->IsRegistered())
				continue;

			//	If we're disabled and we're not in debug mode, then skip

			if (ExtensionList[j]->IsDisabled() && !bDebugMode)
				continue;

			//	Compute the best extension out of the list.

			if (Compare(ExtensionList[j], pBest, bDebugMode) == 1)
				pBest = ExtensionList[j];
			}

		//	If we found one then add it to the list.

		if (pBest)
			retList->Insert(pBest);
		}

	//	Done

	return NOERROR;
	}

ALERROR CExtensionCollection::ComputeAvailableExtensions (CExtension *pAdventure, DWORD dwFlags, const TArray<DWORD> &Extensions, TArray<CExtension *> *retList, CString *retsError)

//	ComputeAvailableExtensions
//
//	Fills retList with extension objects for the given extensions by UNID.
//	We only include extensions that are compatible with the given adventure.
//
//	NOTE: An empty Extensions input means we want all extensions.

	{
	CSmartLock Lock(m_cs);

	int i, j;

	//	Initialize

	bool bDebugMode = ((dwFlags & FLAG_DEBUG_MODE) == FLAG_DEBUG_MODE);
	bool bAllExtensions = (Extensions.GetCount() == 0);
	bool bAutoOnly = ((dwFlags & FLAG_AUTO_ONLY) == FLAG_AUTO_ONLY);
	bool bIncludeAuto = bAutoOnly || ((dwFlags & FLAG_INCLUDE_AUTO) == FLAG_INCLUDE_AUTO);
	bool bRegisteredOnly = ((dwFlags & FLAG_REGISTERED_ONLY) == FLAG_REGISTERED_ONLY);
	bool bNoDiagnostics = !((dwFlags & FLAG_DIAGNOSTICS) == FLAG_DIAGNOSTICS);

	if (!(dwFlags & FLAG_ACCUMULATE))
		retList->DeleteAll();

	//	Loop by UNID because we allow at most one of each UNID.

	for (i = 0; i < m_ByUNID.GetCount(); i++)
		{
		TArray<CExtension *> &ExtensionList = m_ByUNID.GetValue(i);
		if (ExtensionList.GetCount() == 0)
			continue;

		//	We can do a lot of matching on just the first extension in the list
		//	because some things should not change across releases.

		CExtension *pExtension = ExtensionList[0];

		//	We only consider extensions (this property does not change across
		//	releases, so we only need to check once).

		if (pExtension->GetType() != extExtension)
			continue;

		//	If this extension is disabled, then exclude it.

		if (m_DisabledExtensions.Find(pExtension->GetUNID()))
			continue;

		//	If this extension is not on our list, then skip it

		if (!bAllExtensions 
				&& !Extensions.Find(pExtension->GetUNID())
				&& (!bIncludeAuto || !pExtension->IsAutoInclude()))
			continue;

		//	If this is the diagnostics extension and we don't want diagnostics,
		//	then skip it.

		if (bNoDiagnostics
				&& pExtension->GetUNID() == UNID_DIAGNOSTICS_EXTENSION)
			continue;

		//	Out of all the releases, select the latest version.

		CExtension *pBest = NULL;
		for (j = 0; j < ExtensionList.GetCount(); j++)
			{
			//	Hidden extensions are not available

			if (ExtensionList[j]->IsHidden())
				continue;

			//	If this is debug only and we're not in debug mode then skip.

			if (ExtensionList[j]->IsDebugOnly() && !bDebugMode)
				continue;

			//	Exclude unregistered if we want registered only

			if (bRegisteredOnly && !ExtensionList[j]->IsRegistered())
				continue;

			//	If this is an auto extension, include it only if we ask for it.

			if (ExtensionList[j]->IsAutoInclude() && !bIncludeAuto)
				continue;

			//	If this is not an auto extension, then exclude it if all we want
			//	is auto extensions

			if (!ExtensionList[j]->IsAutoInclude() && bAutoOnly)
				continue;

			//	Compute the best extension out of the list.

			if (Compare(ExtensionList[j], pBest, bDebugMode) == 1)
				pBest = ExtensionList[j];
			}

		//	If best not found, skip

		if (pBest == NULL)
			continue;

		//	If this extension does not extend the adventure, then skip.
		//	NOTE: We do this outside the loop because we honor the settings of 
		//	the latest version. Otherwise, we might exclude a later version but
		//	include an earlier one.

		//	If this extension does not extend the adventure, then skip.

		if (pAdventure && !pBest->CanExtend(pAdventure))
			continue;

		//	If we get this far, then we can add this extension

		retList->Insert(pBest);
		}

	//	Done

	return NOERROR;
	}

ALERROR CExtensionCollection::ComputeBindOrder (CExtension *pAdventure, 
												const TArray<CExtension *> &DesiredExtensions, 
												DWORD dwFlags, 
												TArray<CExtension *> *retList, 
												CString *retsError)

//	ComputeBindOrder
//
//	Generates an ordered list of extensions to bind for the given adventure.
//	(In the order in which they should be bound.)
//
//	DesiredExtensions is either a list of extensions selected by the user
//	(at game create time) or the set of extensions stored in the game file.
//
//	We return an error if we cannot compute the list for any reason (e.g., if
//	a required extension is missing).

	{
	DEBUG_TRY

	CSmartLock Lock(m_cs);

	ALERROR error;
	int i;

	bool bDebugMode = ((dwFlags & FLAG_DEBUG_MODE) == FLAG_DEBUG_MODE);

	//	Initialize

	retList->DeleteAll();

	//	If no adventure, then we just bind the base. This can happen in case we're 
	//	running TransData on an old Transcendence.tdb

	if (pAdventure == NULL)
		{
		retList->Insert(m_pBase);
		return NOERROR;
		}

	//	Clear marks; we will used marks to determine when we've already added
	//	an extension to the list.

	ClearAllMarks();

	//	Make a list of core libraries and add them

	TArray<CExtension *> CoreLibraries;
	ComputeCoreLibraries(pAdventure, &CoreLibraries);

	for (i = 0; i < CoreLibraries.GetCount(); i++)
		{
		CoreLibraries[i]->SetMarked();
		retList->Insert(CoreLibraries[i]);

#ifdef DEBUG_LOAD_EXTENSIONS
		if (bDebugMode)
			::kernelDebugLogPattern("Adding core library: %s", CoreLibraries[i]->GetName());
#endif
		}

	//	Make a list of all compatibility libraries

	TArray<CExtension *> CompatibilityLibraries;
	ComputeCompatibilityLibraries(pAdventure, dwFlags, &CompatibilityLibraries);

	//	See if we need the compatibility library

	if (error = AddCompatibilityLibrary(pAdventure, DesiredExtensions, dwFlags, CompatibilityLibraries, retList, retsError))
		return error;

	//	Now add the adventure and any dependencies

	if (pAdventure)
		if (error = AddToBindList(pAdventure, dwFlags, CompatibilityLibraries, retList, retsError))
			return error;

	//	Now add all of the desired extensions.

	for (i = 0; i < DesiredExtensions.GetCount(); i++)
		{
		//	NOTE: We cannot add adventures, so check that here. (This should
		//	never happen since the list of desired extensions should come
		//	from us).

		if (DesiredExtensions[i]->GetType() == extAdventure)
			{
			*retsError = CONSTLIT("Cannot have more than one adventure.");
			return ERR_FAIL;
			}

		//	Add it and all dependencies

		if (error = AddToBindList(DesiredExtensions[i], dwFlags, CompatibilityLibraries, retList, retsError))
			return error;
		}

	return NOERROR;

	DEBUG_CATCH
	}

void CExtensionCollection::ComputeCompatibilityLibraries (CExtension *pAdventure, DWORD dwFlags, TArray<CExtension *> *retList)

//	ComputeCompatibilityLibraries
//
//	Make a list of all libraries that may be auto included for compatibility.

	{
	CSmartLock Lock(m_cs);

	int i, j;

	//	Initialize

	bool bDebugMode = ((dwFlags & FLAG_DEBUG_MODE) == FLAG_DEBUG_MODE);
	retList->DeleteAll();

	//	Loop by UNID because we allow at most one of each UNID.

	for (i = 0; i < m_ByUNID.GetCount(); i++)
		{
		TArray<CExtension *> &ExtensionList = m_ByUNID.GetValue(i);
		if (ExtensionList.GetCount() == 0)
			continue;

		//	We only consider libraries with a compatibility value

		if (ExtensionList[0]->GetType() != extLibrary
				|| ExtensionList[0]->GetAutoIncludeAPIVersion() == 0)
			continue;

		//	Out of all the releases, select the latest version.

		CExtension *pBest = NULL;
		for (j = 0; j < ExtensionList.GetCount(); j++)
			{
			//	If this is debug only and we're not in debug mode then skip.

			if (ExtensionList[j]->IsDebugOnly() && !bDebugMode)
				continue;

			//	If this extension does not extend the adventure, then skip.

			if (pAdventure && !ExtensionList[j]->CanExtend(pAdventure))
				continue;

			//	Compute the best extension out of the list.

			if (Compare(ExtensionList[j], pBest, bDebugMode) == 1)
				pBest = ExtensionList[j];
			}

		//	If we found one then add it to the list.

		if (pBest)
			retList->Insert(pBest);
		}
	}

void CExtensionCollection::ComputeCoreLibraries (CExtension *pExtension, TArray<CExtension *> *retList)

//	ComputeCoreLibraries
//
//	Compute the list of core libraries needed for the given extension.

	{
	//	We always need the base

	retList->Insert(m_pBase);

	//	Decide based on the game.

	switch (m_iGame)
		{
		case gameAmerica:
			break;

		case gameTranscendence:
			{
			CExtension *pLibrary;

			if (FindBestExtension(UNID_CORE_TYPES_LIBRARY, 1, 0, &pLibrary))
				retList->Insert(pLibrary);

			if (FindBestExtension(UNID_RPG_LIBRARY, 1, 0, &pLibrary))
				retList->Insert(pLibrary);

			if (FindBestExtension(UNID_UNIVERSE_LIBRARY, 1, 0, &pLibrary))
				retList->Insert(pLibrary);

			//	Prior to API 26 we expected these UNIDs to be defined, so we 
			//	need to add them.

			if (pExtension->GetAPIVersion() < 26)
				{
				if (FindBestExtension(UNID_HUMAN_SPACE_LIBRARY, 1, 0, &pLibrary))
					retList->Insert(pLibrary);
				}

			break;
			}

		default:
			ASSERT(false);
		}
	}

bool CExtensionCollection::ComputeDownloads (const TArray<CMultiverseCatalogEntry> &Collection, TArray<CMultiverseCatalogEntry> &retNotFound)

//	ComputeDownloads
//
//	Given the user's collection, looks for the extension on the local machine
//	and sees if we need to download a new version. If so, we add it to the list.
//	We return TRUE if we have to download at least one extension.

	{
	CSmartLock Lock(m_cs);

	retNotFound.DeleteAll();

	//	First we make a list of all libraries that are needed by extensions in 
	//	our collection.

	TSortMap<DWORD, bool> LibrariesUsed;
	for (int i = 0; i < Collection.GetCount(); i++)
		{
		const CMultiverseCatalogEntry &Entry = Collection[i];

		//	Skip libraries because we expect an extension/library to list all
		//	dependencies (including dependencies of its libraries). Moreover, 
		//	we don't disable libraries separately (there is no UI).

		if (Entry.GetType() == extLibrary)
			continue;

		//	If this extension is disabled, then we don't need its dependencies
		//	either.

		if (m_DisabledExtensions.Find(Entry.GetUNID()))
			continue;

		//	Add libraries

		Entry.GetLibrariesUsed(LibrariesUsed);
		}

	//	Now loop through the collection and figure out what to download.

	for (int i = 0; i < Collection.GetCount(); i++)
		{
		const CMultiverseCatalogEntry &Entry = Collection[i];

		//	Skip core entries

		if (Entry.GetLicenseType() == CMultiverseCatalogEntry::licenseCore)
			continue;

		//  Skip Steam UGC

		if (Entry.GetLicenseType() == CMultiverseCatalogEntry::licenseSteamUGC)
			continue;

		//	Steam versions don't need to be downloaded

		if (Entry.GetLicenseType() == CMultiverseCatalogEntry::licenseSteam)
			continue;

		//	If the user does not want to download the extension to this machine
		//	then we skip.

		if (m_DisabledExtensions.Find(Entry.GetUNID()))
			continue;

		//	If this is a library, and none of our other extensions need this library,
		//	then we skip it.

		if (Entry.GetType() == extLibrary
				&& !LibrariesUsed.Find(Entry.GetUNID()))
			continue;

		//	Look for this extension in our list. If we found it then compare
		//	the signature to make sure that we have the right version.

		CExtension *pExtension;
		if (FindExtension(Entry.GetUNID(), Entry.GetRelease(), CExtension::folderCollection, &pExtension))
			{
			//	Compare the digests. If they match, then this is a registered
			//	extension and we don't need to download.

			if (Entry.GetTDBFileRef().GetDigest() == pExtension->GetDigest())
				continue;

			//	If this is an unregistered version, then for now we skip (because we
			//	don't know how to download).

			else if (!pExtension->IsRegistered()
					&& strEquals(Entry.GetTDBFileRef().GetVersion(), pExtension->GetVersion()))
				continue;
			}

		//	Otherwise, we need to download

		retNotFound.Insert(Entry);
		}

	//	Done

	return (retNotFound.GetCount() > 0);
	}

ALERROR CExtensionCollection::ComputeFilesToLoad (const CString &sFilespec, CExtension::EFolderTypes iFolder, TSortMap<CString, int> &List, CString *retsError)

//	ComputeFilesToLoad
//
//	Initializes a list with the set of files to load in the given folder.

	{
	ALERROR error;

	//	Load all extensions in the folder

	CFileDirectory Dir(pathAddComponent(sFilespec, EXTENSIONS_FILTER));
	while (Dir.HasMore())
		{
		SFileDesc FileDesc;
		Dir.GetNextDesc(&FileDesc);

		//	Skip hidden or system files

		if (FileDesc.bHiddenFile || FileDesc.bSystemFile)
			continue;

		//	Skip any file or directory that starts with a dot

		if (*FileDesc.sFilename.GetASCIIZPointer() == '.')
			continue;

		//	Skip any file or directory that stars with '_'

		if (*FileDesc.sFilename.GetASCIIZPointer() == '_')
			continue;

		//	Get path and extension

		CString sFilepath = pathAddComponent(sFilespec, FileDesc.sFilename);
		CString sFileExtension = pathGetExtension(sFilepath);
		CString sExtensionRoot = pathStripExtension(sFilepath);

		//	If this is a folder, then recurse

		if (FileDesc.bFolder)
			{
			if (error = ComputeFilesToLoad(sFilepath, iFolder, List, retsError))
				return error;

			continue;
			}

		//	If this is an XML file then we overwrite a TDB file of the same name.
		//	Except in the Collection folder, which only allows TDBs

		if (strEquals(sFileExtension, EXTENSION_XML)
				&& iFolder != CExtension::folderCollection)
			{
			CString sTDBFilespec = strPatternSubst(CONSTLIT("%s.tdb"), sExtensionRoot);

			int iPos;
			if (List.FindPos(sTDBFilespec, &iPos))
				List.Delete(iPos);
			}

		//	If this is a TDB file, then add only if there isn't an XML file of the
		//	same name.

		else if (strEquals(sFileExtension, EXTENSION_TDB))
			{
			CString sXMLFilespec = strPatternSubst(CONSTLIT("%s.xml"), sExtensionRoot);

			if (List.FindPos(sXMLFilespec))
				continue;
			}

		//	We ignore all other extensions

		else
			continue;

		//	Open the file

		CResourceDb ExtDb(sFilepath, true);
		ExtDb.SetDebugMode(g_pUniverse->InDebugMode());
		if (error = ExtDb.Open(DFOPEN_FLAG_READ_ONLY, retsError))
			{
			//	If this is a TDB then ignore the error--we assume that we will try to 
			//	repair it when we open the collection

			if (ExtDb.IsTDB())
				{
				::kernelDebugLogPattern("Unable to load extension: %s", sFilepath);
				continue;
				}

			return error;
			}

		//	If this is a module, then skip it

		CString sDOCTYPERootTag = ExtDb.GetRootTag();
		if (strEquals(sDOCTYPERootTag, TRANSCENDENCE_MODULE_TAG))
			continue;
		else if (!strEquals(sDOCTYPERootTag, TRANSCENDENCE_EXTENSION_TAG)
				&& !strEquals(sDOCTYPERootTag, TRANSCENDENCE_ADVENTURE_TAG)
				&& !strEquals(sDOCTYPERootTag, TRANSCENDENCE_LIBRARY_TAG))
			{
			*retsError = strPatternSubst(CONSTLIT("%s: Expected <TranscendenceAdventure>, <TranscendenceExtension>, <TranscendenceLibrary> or <TranscendenceModule> instead of <%s>"), 
					sFilepath,
					sDOCTYPERootTag);
			return ERR_FAIL;
			}

		//	Add the full filepath to our list

		List.Insert(sFilepath);
		}

	return NOERROR;
	}

void CExtensionCollection::DebugDump (void)

//	DebugDump
//
//	Dump information on crash

	{
	CSmartLock Lock(m_cs);
	int i, j;

	::kernelDebugLogPattern("m_pBase:");
	CExtension::DebugDump(m_pBase);

	::kernelDebugLogPattern("m_Extensions:");
	for (i = 0; i < m_Extensions.GetCount(); i++)
		CExtension::DebugDump(m_Extensions[i], true);

	::kernelDebugLogPattern("m_ByUNID:");
	for (i = 0; i < m_ByUNID.GetCount(); i++)
		{
		const TArray<CExtension *> &List = m_ByUNID[i];
		for (j = 0; j < List.GetCount(); j++)
			CExtension::DebugDump(List[j]);
		}

	::kernelDebugLogPattern("m_ByFilespec:");
	for (i = 0; i < m_ByFilespec.GetCount(); i++)
		CExtension::DebugDump(m_ByFilespec[i]);
	}

bool CExtensionCollection::FindAdventureFromDesc (DWORD dwUNID, DWORD dwFlags, CExtension **retpExtension)

//	FindAdventureFromDesc
//
//	Look for the adventure that has the given adventure descriptor.

	{
	CSmartLock Lock(m_cs);
	int i;

	bool bDebugMode = ((dwFlags & FLAG_DEBUG_MODE) == FLAG_DEBUG_MODE);

	//	Look for the adventure

	CExtension *pBest = NULL;
	for (i = 0; i < m_Extensions.GetCount(); i++)
		{
		CExtension *pExtension = m_Extensions[i];

		//	If not the right adventure then skip

		if (pExtension->GetAdventureDesc() == NULL || pExtension->GetAdventureDesc()->GetUNID() != dwUNID)
			continue;

		//	If this is debug only and we're not in debug mode then skip.

		if (pExtension->IsDebugOnly() && !bDebugMode)
			continue;

		//	Compute the best extension out of the list.

		if (pBest == NULL || (pBest->GetUNID() == pExtension->GetUNID() && Compare(pExtension, pBest, bDebugMode) == 1))
			pBest = pExtension;
		}

	//	Not found

	if (pBest == NULL)
		return false;

	//	Found

	if (retpExtension)
		*retpExtension = pBest;

	return true;
	}

bool CExtensionCollection::FindBestExtension (DWORD dwUNID, DWORD dwRelease, DWORD dwFlags, CExtension **retpExtension)

//	FindBestExtension
//
//	Look for the extension that meets the above criteria.

	{
	CSmartLock Lock(m_cs);
	int i;

	bool bDebugMode = ((dwFlags & FLAG_DEBUG_MODE) == FLAG_DEBUG_MODE);

	int iPos;
	if (!m_ByUNID.FindPos(dwUNID, &iPos))
		return false;

	CExtension *pBest = NULL;
	TArray<CExtension *> &List = m_ByUNID.GetValue(iPos);
	for (i = 0; i < List.GetCount(); i++)
		{
		//	If this is debug only and we're not in debug mode then skip.

		if (List[i]->IsDebugOnly() && !bDebugMode)
			continue;

		//	If we're looking for a specific release, then check

		if (dwRelease && dwRelease != List[i]->GetRelease())
			continue;

		//	Compute the best extension out of the list.

		if (Compare(List[i], pBest, bDebugMode) == 1)
			pBest = List[i];
		}

	//	Not found

	if (pBest == NULL)
		return false;

	//	If we're looking for an adventure and this is not it, then fail
	//
	//	NOTE: We assume that two extensions with the same UNID are always of the
	//	same type.

	if ((dwFlags & FLAG_ADVENTURE_ONLY) && pBest->GetType() != extAdventure)
		return false;

	//	Found

	if (retpExtension)
		*retpExtension = pBest;

	return true;
	}

bool CExtensionCollection::FindExtension (DWORD dwUNID, DWORD dwRelease, CExtension::EFolderTypes iFolder, CExtension **retpExtension)

//	FindExtension
//
//	Finds the given extension. If dwRelease is 0 then we return the latest
//	release. If iFolder is folderUnknown then we return either folder.

	{
	CSmartLock Lock(m_cs);
	int i;

	int iPos;
	if (!m_ByUNID.FindPos(dwUNID, &iPos))
		return false;

	CExtension *pBest = NULL;
	TArray<CExtension *> &List = m_ByUNID.GetValue(iPos);
	for (i = 0; i < List.GetCount(); i++)
		{
		//	Look in the proper folder

		if (iFolder != CExtension::folderUnknown && List[i]->GetFolderType() != iFolder)
			continue;

		//	Look for the appropriate release

		if (dwRelease != 0 && List[i]->GetRelease() != dwRelease)
			continue;

		//	Pick the best one so far

		if (pBest == NULL)
			pBest = List[i];
		else
			{
			if (List[i]->GetRelease() > pBest->GetRelease())
				pBest = List[i];
			}
		}

	//	Not found

	if (pBest == NULL)
		return false;

	//	Found

	if (retpExtension)
		*retpExtension = pBest;

	return true;
	}

void CExtensionCollection::FreeDeleted (void)

//	FreeDeleted
//
//	Frees deleted list

	{
	DEBUG_TRY

	CSmartLock Lock(m_cs);
	int i;

	for (i = 0; i < m_Deleted.GetCount(); i++)
		delete m_Deleted[i];

	m_Deleted.DeleteAll();

	DEBUG_CATCH
	}

CString CExtensionCollection::GetEntityName (DWORD dwUNID)

//	GetEntityName
//
//	Look up the name of an entity by its value

	{
	CString sName;

	//	Look through all of our extensions

	for (int i = 0; i < m_Extensions.GetCount(); i++)
		{
		sName = m_Extensions[i]->GetEntityName(dwUNID);
		
		if (!sName.IsBlank())
			return sName;
		}
	return NULL_STR;
	}

DWORD CExtensionCollection::GetEntityValue (const CString &sName)

//	GetEntityValue
//
//	Look up the value of an entity by its name

	{
	DWORD dwUNID;

	//	Look through all of our extensions

	for (int i = 0; i < m_Extensions.GetCount(); i++)
		{
		dwUNID = strToInt(m_Extensions[i]->GetEntities()->ResolveExternalEntity(sName), 0);
		if (dwUNID)
			return dwUNID;
		}
	return NULL;
	}

CString CExtensionCollection::GetExternalResourceFilespec (CExtension *pExtension, const CString &sFilename) const

//	GetExternalResourceFilespec
//
//	Returns a filespec to an external resource. NOTE: This is only valid for
//	extensions in the Collection folder.

	{
	ASSERT(pExtension && pExtension->GetFolderType() == CExtension::folderCollection);

	//	We look in a subdirectory of the Collection folder

	CString sExternalFolder = pathAddComponent(m_sCollectionFolder, strPatternSubst(CONSTLIT("%08X"), pExtension->GetUNID()));

	//	Compose the path

	return pathAddComponent(sExternalFolder, sFilename);
	}

bool CExtensionCollection::GetRequiredResources (TArray<CString> *retFilespecs)

//	GetRequiredResources
//
//	Returns a list of resources that we need to download.
//	NOTE: The filespec includes the path where the file is expected to be.

	{
	CSmartLock Lock(m_cs);
	int i, j;

	retFilespecs->DeleteAll();

	//	Loop over all extensions and return a list of missing TDB resources
	//	(generally music files that we download later).

	for (i = 0; i < m_Extensions.GetCount(); i++)
		{
		CExtension *pExtension = m_Extensions[i];
		if (pExtension == NULL
				|| pExtension->GetFolderType() != CExtension::folderCollection
				|| pExtension->GetLoadState() != CExtension::loadComplete
				|| pExtension->IsDisabled()
				|| m_DisabledExtensions.Find(pExtension->GetUNID())
				|| !pExtension->IsRegistrationVerified())
			continue;

		//	Look for any deferred resources

		const TArray<CString> &Resources = pExtension->GetExternalResources();

		//	If any files don't exist, add them

		for (j = 0; j < Resources.GetCount(); j++)
			{
			if (!::pathExists(Resources[j]))
				{
				retFilespecs->Insert(Resources[j]);
				::kernelDebugLogPattern("Request download: %s", Resources[j]);
				}
			}
		}

	return (retFilespecs->GetCount() > 0);
	}

void CExtensionCollection::InitEntityResolver (CExtension *pExtension, DWORD dwFlags, CEntityResolverList *retResolver)

//	InitEntityResolver
//
//	Initializes an entity resolver for the given extension.

	{
	int i;

	//	Add all core libraries

	TArray<CExtension *> CoreLibraries;
	ComputeCoreLibraries(pExtension, &CoreLibraries);

	for (i = 0; i < CoreLibraries.GetCount(); i++)
		retResolver->AddResolver(CoreLibraries[i]->GetEntities());

	//	Next we add any libraries used by the extension

	for (i = 0; i < pExtension->GetLibraryCount(); i++)
		{
		const CExtension::SLibraryDesc &LibraryDesc = pExtension->GetLibrary(i);

		CExtension *pLibrary;
		if (!FindBestExtension(LibraryDesc.dwUNID, LibraryDesc.dwRelease, dwFlags, &pLibrary))
			continue;

		retResolver->AddResolver(pLibrary->GetEntities());
		}

	//	Finally, add entities defined in the extension

	retResolver->AddResolver(pExtension->GetEntities());
	}

bool CExtensionCollection::IsRegisteredGame (CExtension *pAdventure, const TArray<CExtension *> &DesiredExtensions, DWORD dwFlags)

//	IsRegisteredGame
//
//	Returns TRUE if the given adventure and set of extensions can be used
//	to create a registered game. We check to make sure all extensions and 
//	libraries are registered and verified.

	{
	CSmartLock Lock(m_cs);
	int i;

	//	Compute the full set of extensions that we need to load. This will include 
	//	any libraries.

	TArray<CExtension *> BindOrder;
	CString sError;
	if (ComputeBindOrder(pAdventure, DesiredExtensions, dwFlags, &BindOrder, &sError) != NOERROR)
		return false;

	//	Loop over all extensions and make sure all are verified.

	for (i = 0; i < BindOrder.GetCount(); i++)
		if (!BindOrder[i]->IsRegistrationVerified())
			return false;

	return true;
	}

ALERROR CExtensionCollection::Load (const CString &sFilespec, const TSortMap<DWORD, bool> &DisabledExtensions, DWORD dwFlags, CString *retsError)

//	Load
//
//	Loads all extension files. This may be called at any time from any thread
//	to load or reload files on disk. It will only load new or modified files.

	{
	CSmartLock Lock(m_cs);
	ALERROR error;
	int i;

	//	If we don't need to load, then we're done.

	if (!m_bReloadNeeded)
		return NOERROR;

	m_bLoadedInDebugMode = ((dwFlags & FLAG_DEBUG_MODE) == FLAG_DEBUG_MODE);
	m_DisabledExtensions = DisabledExtensions;

	//	Default adventure can never be disabled.

	if (m_DisabledExtensions.Find(DEFAULT_ADVENTURE_EXTENSION_UNID))
		m_DisabledExtensions.DeleteAt(DEFAULT_ADVENTURE_EXTENSION_UNID);

	//	Load base file

	if (error = LoadBaseFile(sFilespec, dwFlags, retsError))
		return error;

	//	We begin by loading stubs for all extension (i.e., only basic extension
	//	information and entities).

    if (!(dwFlags & FLAG_NO_COLLECTION))
        {
	    if (error = LoadFolderStubsOnly(m_sCollectionFolder, CExtension::folderCollection, dwFlags, retsError))
		    return error;
        }

	for (i = 0; i < m_ExtensionFolders.GetCount(); i++)
		{
		if (error = LoadFolderStubsOnly(m_ExtensionFolders[i], CExtension::folderExtensions, dwFlags, retsError))
			return error;
		}

	//	Now that we know about all the extensions that we have, continue loading.

	for (i = 0; i < m_Extensions.GetCount(); i++)
		{
		CExtension *pExtension = m_Extensions[i];

		//	If this extension is already disabled (possibly because it requires 
		//	a future API) then we don't bother trying to load.

		if (pExtension->IsDisabled())
			continue;

		//	Generate a resolver so that we can look up entities. We always add
		//	the base file and the extension itself.

		CLibraryResolver Resolver(*this);
		Resolver.AddDefaults(pExtension);

		CExtension::SLoadOptions LoadOptions;
		LoadOptions.bNoResources = ((dwFlags & FLAG_NO_RESOURCES) == FLAG_NO_RESOURCES);
		LoadOptions.bNoDigestCheck = ((dwFlags & FLAG_NO_COLLECTION_CHECK) == FLAG_NO_COLLECTION_CHECK);

		//	If this extension has been manually disabled, then don't bother with
		//	the digest because it is expensive. We'll compute it later.

		if (m_DisabledExtensions.Find(pExtension->GetUNID()))
			LoadOptions.bNoDigestCheck = true;

		//	Load the basic elements of the extension (we load the extension fully
		//	only when we bind).

		if (error = pExtension->Load(CExtension::loadAdventureDesc, 
				&Resolver, 
				LoadOptions, 
				retsError))
			return error;
		}

	//	Done

	m_bReloadNeeded = false;

	return NOERROR;
	}

ALERROR CExtensionCollection::LoadBaseFile (const CString &sFilespec, DWORD dwFlags, CString *retsError)

//	LoadBaseFile
//
//	Loads the base file. This is stored as an extension of UNID = 0.

	{
	ALERROR error;
	int i;

	//	If already loaded, nothing to do

	if (m_pBase)
		return NOERROR;

	//	Open up the file

	CResourceDb Resources(sFilespec);
	Resources.SetDebugMode(g_pUniverse->InDebugMode());
	if (error = Resources.Open(DFOPEN_FLAG_READ_ONLY, retsError))
		return error;

	//	Log whether or not we're using the XML or TDB files.

	if (Resources.IsUsingExternalGameFile())
		kernelDebugLogPattern("Using external %s", sFilespec);

	if (Resources.IsUsingExternalResources())
		kernelDebugLogPattern("Using external resource files");

	//	Figure out what game we're running.

	CString sFilename = pathStripExtension(pathGetFilename(sFilespec));
	if (strEquals(sFilename, FILE_TRANSCENDENCE))
		m_iGame = gameTranscendence;
	else if (strEquals(sFilename, FILE_AMERICA))
		m_iGame = gameAmerica;
	else
		{
		*retsError = strPatternSubst(CONSTLIT("Unexpected base file type: %s."), sFilespec);
		return ERR_FAIL;
		}

	//	Check the signature on the file to see if we should verify the
	//	extensions loaded from the base file.
	//
	//	NOTE: Only TDB is verified; the XML is always considered unregistered.

	CIntegerIP Digest;
	Resources.ComputeFileDigest(&Digest);

	CIntegerIP CorrectDigest(DIGEST_SIZE, g_BaseFileDigest);
	bool bVerified = (Digest == CorrectDigest);

	//	Load the main XML file. Since this is the base file we don't need any
	//	additional entities. (But we do get a copy of the entities).

	CExternalEntityTable *pEntities = new CExternalEntityTable;
	CXMLElement *pGameFile;
	if (error = Resources.LoadGameFile(&pGameFile, NULL, retsError, pEntities))
		{
		delete pEntities;
		return error;
		}

	//	Create a load context

	SDesignLoadCtx Ctx;
	Ctx.sResDb = sFilespec;
	Ctx.pResDb = &Resources;
	Ctx.bNoResources = ((dwFlags & FLAG_NO_RESOURCES) ? true : false);
	Ctx.sErrorFilespec = sFilespec;

	//	Load it.
	//
	//	NOTE: CreateBaseFile takes ownership of pEntities on success (and 
	//	attaches it to the pBase result).

	CExtension *pBase;
	TArray<CXMLElement *> EmbeddedExtensions;
	if (error = CExtension::CreateBaseFile(Ctx, m_iGame, pGameFile, pEntities, &pBase, &EmbeddedExtensions))
		{
		delete pGameFile;
		delete pEntities;
		return CExtension::ComposeLoadError(Ctx, retsError);
		}

	//	Add the base file. Note that this will set m_pBase correctly.

	ASSERT(pBase->GetUNID() == 0);
	pBase->SetVerified(bVerified);
	AddOrReplace(pBase);

	//	Now we load any embedded extensions

	for (i = 0; i < EmbeddedExtensions.GetCount(); i++)
		{
		CExtension *pExtension;
		if (error = LoadEmbeddedExtension(Ctx, EmbeddedExtensions[i], &pExtension))
			return CExtension::ComposeLoadError(Ctx, retsError);

		//	Verified

		pExtension->SetVerified(bVerified);

		//	Add to list

		ASSERT(pExtension->GetUNID() != 0);
		AddOrReplace(pExtension);
		}

	//	Done

	return NOERROR;
	}

ALERROR CExtensionCollection::LoadEmbeddedExtension (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CExtension **retpExtension)

//	LoadEmbeddedExtension
//
//	Loads an extension defined in the base file.

	{
	ALERROR error;

	//	In some cases we need to load the extension XML from a separate file.
	//	We need to free this when done.

	CLibraryResolver Resolver(*this);
	IXMLParserController *pOldEntities = NULL;
	bool bOldEntitiesFree = false;
	CString sRootFolder;
	CXMLElement *pRoot = NULL;

	//	We also prepare an entity table which will get loaded with any
	//	entities defined in the embedded extension.

	CExternalEntityTable *pExtEntities = new CExternalEntityTable;
	pExtEntities->SetParent(m_pBase->GetEntities());

	//	If we have a filespec, then the extension is in a separate file inside
	//	the base TDB (or XML directory).

	CString sFilename;
	if (pDesc->FindAttribute(FILENAME_ATTRIB, &sFilename))
		{
		//	If we have a path, then we need to apply this to any resources 
		//	loaded by this file.

		sRootFolder = pathGetPath(sFilename);

		//	This extension might refer to other embedded libraries, so we need
		//	to give it a resolver

		Resolver.AddLibrary(m_pBase);
		Resolver.ReportLibraryErrors();

		//	Load the file

		CString sError;
		if (error = Ctx.pResDb->LoadEmbeddedGameFile(sFilename, &pRoot, &Resolver, pExtEntities, &sError))
			{
			delete pExtEntities;
			Ctx.sError = strPatternSubst(CONSTLIT("Unable to load embedded file: %s"), sError);
			return ERR_FAIL;
			}

		//	Add the entities from the root to the resolver.

		Resolver.AddTable(pExtEntities);

		//	Now temporarily replace the entity table for the resource db to
		//	use the resolver (so we can resolve entities in libraries, etc.)

		pOldEntities = Ctx.pResDb->GetEntitiesHandoff(&bOldEntitiesFree);
		Ctx.pResDb->SetEntities(&Resolver);

		//	This is the actual extension XML.

		pDesc = pRoot;
		}

	//	Otherwise, make a copy of pDesc because the new extension will own it.

	else
		{
		pRoot = pDesc->OrphanCopy();
		pDesc = pRoot;
		}

	//	Create the extension

	SDesignLoadCtx ExtCtx;
	ExtCtx.sResDb = Ctx.sResDb;
	ExtCtx.pResDb = Ctx.pResDb;
	ExtCtx.bNoResources = Ctx.bNoResources;
	ExtCtx.dwInheritAPIVersion = m_pBase->GetAPIVersion();
	//	No need to set bBindAsNewGame because it is only useful during Bind.
	//	AdvCtx.bBindAsNewGame = Ctx.bBindAsNewGame;

	//	We always load in full because we don't know how to load later.
	ExtCtx.bLoadAdventureDesc = false;

	//	Root folder

	if (!sRootFolder.IsBlank())
		ExtCtx.sFolder = pathAddComponent(Ctx.sFolder, sRootFolder);
	else
		ExtCtx.sFolder = Ctx.sFolder;

	//	Load the extension

	CExtension *pExtension;
	if (error = CExtension::CreateExtension(ExtCtx, pDesc, CExtension::folderBase, pExtEntities, &pExtension))
		{
		if (pRoot)
			delete pRoot;
		delete pExtEntities;
		Ctx.sError = ExtCtx.sError;
		Ctx.sErrorFilespec = ExtCtx.sErrorFilespec;
		return error;
		}

	//	Done

	if (pOldEntities)
		Ctx.pResDb->SetEntities(pOldEntities, bOldEntitiesFree);

	*retpExtension = pExtension;

	return NOERROR;
	}

ALERROR CExtensionCollection::LoadFile (const CString &sFilespec, CExtension::EFolderTypes iFolder, DWORD dwFlags, const CIntegerIP &CheckDigest, bool *retbReload, CString *retsError)

//	LoadFile
//
//	Loads the given extension file.

	{
	ALERROR error;

	//	Create the extension

	CExtension *pExtension;
	if (error = CExtension::CreateExtensionStub(sFilespec, iFolder, &pExtension, retsError))
		return error;

	//	Generate a resolver so that we can look up entities. We always add
	//	the base file and the extension itself.

	CLibraryResolver Resolver(*this);
	Resolver.AddDefaults(pExtension);

	CExtension::SLoadOptions LoadOptions;
	LoadOptions.bNoResources = ((dwFlags & FLAG_NO_RESOURCES) == FLAG_NO_RESOURCES);
	LoadOptions.bNoDigestCheck = ((dwFlags & FLAG_NO_COLLECTION_CHECK) == FLAG_NO_COLLECTION_CHECK);

	//	Load it

	if (error = pExtension->Load(((dwFlags & FLAG_DESC_ONLY) ? CExtension::loadAdventureDesc : CExtension::loadComplete),
			&Resolver,
			LoadOptions,
			retsError))
		{
		delete pExtension;
		return error;
		}

	//	Did the load succeed?

	if ((dwFlags & FLAG_ERROR_ON_DISABLE)
			&& pExtension->IsDisabled())
		{
		if (retsError)
			*retsError = strPatternSubst(CONSTLIT("Extension %s: %s"), pExtension->GetFilespec(), pExtension->GetDisabledReason());
		delete pExtension;
		return ERR_FAIL;
		}

	//	Check the digest before we accept it.

	if (pExtension->IsRegistered() 
			&& !CheckDigest.IsEmpty())
		{
		//	NOTE: We set the verification flag even if the file failed to load.
		//	If the digest matches then the file is good--it probably failed because
		//	of a missing library.

		if (pExtension->GetDigest() == CheckDigest)
			pExtension->SetVerified();

		//	Otherwise, we have an error

		else
			{
			if (retsError)
				*retsError = strPatternSubst(CONSTLIT("Extension file corrupt: %s"), pExtension->GetFilespec());
			delete pExtension;
			return ERR_FAIL;
			}
		}

	//	If we just added a new library, then tell our caller that it might want
	//	to try reloading disabled extensions that rely on this library.

	if (retbReload)
		*retbReload = (pExtension->GetType() == extLibrary 
				&& !pExtension->IsDisabled());

	//	Add the extensions to our list. We lock because we expect this function
	//	to be called without a lock; we don't want to lock while doing the
	//	expensive load operation.

	AddOrReplace(pExtension);

	//	Done

	return NOERROR;
	}

ALERROR CExtensionCollection::LoadFolderStubsOnly (const CString &sFilespec, CExtension::EFolderTypes iFolder, DWORD dwFlags, CString *retsError)

//	LoadFolder
//
//	Loads all the extensions in the given folder.

	{
	ALERROR error;
	int i;

	//	Make sure the folder exists; if not, create it.

	if (!pathExists(sFilespec))
		{
		if (iFolder == CExtension::folderCollection)
			pathCreate(sFilespec);
		return NOERROR;
		}

	//	Make a list of all the files that we want to load.

	TSortMap<CString, int> FilesToLoad;
	if (error = ComputeFilesToLoad(sFilespec, iFolder, FilesToLoad, retsError))
		return error;

	//	Now loop over the files and load them (if necessary)

	for (i = 0; i < FilesToLoad.GetCount(); i++)
		{
		const CString &sExtensionFilespec = FilesToLoad.GetKey(i);

		//	If this file is already loaded then we don't need to do anything

		CExtension *pExtension;
		if (m_ByFilespec.Find(sExtensionFilespec, &pExtension))
			{
			CTimeDate FileTime = fileGetModifiedTime(sExtensionFilespec);

			//	If the file has not changed then skip this file.

			if (FileTime.Compare(pExtension->GetModifiedTime()) == 0)
				continue;
			}

		//	Create the extension

		if (error = CExtension::CreateExtensionStub(sExtensionFilespec, iFolder, &pExtension, retsError))
			return error;

		//	If this extension requires an API beyond our base file, then we disable it.
		//	This can happen when we do TransData on older Transcendence.tdb.

		if (!pExtension->IsDisabled()
				&& pExtension->GetAPIVersion() > m_pBase->GetAPIVersion())
			pExtension->SetDisabled(CONSTLIT("Requires a newer version of Transcendence.tdb"));

		//	Add the extensions to our list. We lock because we expect this function
		//	to be called without a lock; we don't want to lock while doing the
		//	expensive load operation.

		AddOrReplace(pExtension);
		}

	return NOERROR;
	}

ALERROR CExtensionCollection::LoadNewExtension (const CString &sFilespec, const CIntegerIP &FileDigest, CString *retsError)

//	LoadNewExtension
//
//	Loads a newly downloaded extension (in the Downloads folder) and moves it to
//	the Collection folder.

	{
	try
		{
		//	NOTE: We don't need to lock because LoadFile will lock appropriately.

		//	Delete the destination filespec

		CString sNewFilespec = pathAddComponent(m_sCollectionFolder, pathGetFilename(sFilespec));
		fileDelete(sNewFilespec);

		//	Generate a destination filespec and move the file.

		if (!fileMove(sFilespec, sNewFilespec))
			{
			*retsError = strPatternSubst(ERR_CANT_MOVE, sFilespec, sNewFilespec);
			return ERR_FAIL;
			}

		//	Load the file

		bool bReload;
		if (LoadFile(sNewFilespec, CExtension::folderCollection, FLAG_DESC_ONLY, FileDigest, &bReload, retsError) != NOERROR)
			return ERR_FAIL;

		//	If necessary, try reloading other extensions that might become enabled after
		//	this new file is loaded. We keep reloading until we've enabled no more
		//	extensions.

		if (bReload)
			{
			while (ReloadDisabledExtensions(FLAG_DESC_ONLY))
				;
			}

		return NOERROR;
		}
	catch (...)
		{
		*retsError = strPatternSubst(CONSTLIT("Crash loading new extension: %s."), sFilespec);
		return ERR_FAIL;
		}
	}

bool CExtensionCollection::ReloadDisabledExtensions (DWORD dwFlags)

//	ReloadDisabledExtensions
//
//	Loops over all disabled extensions and tries to reload any that required a
//	missing library.
//
//	We return TRUE if any extension was enabled in this way. 

	{
	int i;

	struct SEntry
		{
		CString sFilespec;
		CExtension::EFolderTypes iFolder;
		CIntegerIP FileDigest;
		};

	//	First make a list of all extensions that we want to reload

	m_cs.Lock();
	TArray<SEntry> ReloadList;
	for (i = 0; i < m_Extensions.GetCount(); i++)
		{
		CExtension *pExtension = m_Extensions[i];
		if (pExtension->IsDisabled())
			{
			SEntry *pEntry = ReloadList.Insert();
			pEntry->sFilespec = pExtension->GetFilespec();
			pEntry->iFolder = pExtension->GetFolderType();

			//	If the original load was verified, then we can
			//	verify a subsequent load (since it is the same file).

			if (pExtension->IsRegistrationVerified())
				pEntry->FileDigest = pExtension->GetDigest();
			}
		}
	m_cs.Unlock();

	//	Now try reloading each extension. We do this outside the lock because
	//	it is time-consuming. The calls will lock appropriately.

	bool bSuccess = false;
	for (i = 0; i < ReloadList.GetCount(); i++)
		{
		const SEntry &Entry = ReloadList[i];

		//	Load the extension. If we fail, then ignore it--it means we could
		//	not fix the problem.

		if (LoadFile(Entry.sFilespec, 
				Entry.iFolder, 
				FLAG_DESC_ONLY | FLAG_ERROR_ON_DISABLE, 
				Entry.FileDigest, 
				NULL, 
				NULL) != NOERROR)
			continue;

		//	Otherwise, we succeeded at least on one extension

		bSuccess = true;
		}

	//	Done

	return bSuccess;
	}

void CExtensionCollection::SetExtensionEnabled (DWORD dwUNID, bool bEnabled)

//	SetExtensionEnabled
//
//	Add or removes an extension from the disabled list.

	{
	if (bEnabled)
		m_DisabledExtensions.DeleteAt(dwUNID);
	else
		{
		//	Default adventure cannot be disabled.

		if (dwUNID == DEFAULT_ADVENTURE_EXTENSION_UNID)
			return;

		//	Add to list.

		m_DisabledExtensions.SetAt(dwUNID, true);
		}
	}

void CExtensionCollection::SweepImages (void)

//	SweepImages
//
//	Frees images that we're no longer using.

	{
	DEBUG_TRY

	CSmartLock Lock(m_cs);
	int i;

	for (i = 0; i < m_Extensions.GetCount(); i++)
		m_Extensions[i]->SweepImages();

	DEBUG_CATCH
	}

void CExtensionCollection::UpdateCollectionStatus (TArray<CMultiverseCatalogEntry> &Collection, const SCollectionStatusOptions &Options)

//	UpdateCollectionStatus
//
//	Updates the status of the entries in the given collection based on our list 
//	of local extensions.

	{
	CSmartLock Lock(m_cs);
	int i;

	for (i = 0; i < Collection.GetCount(); i++)
		{
		CMultiverseCatalogEntry &Entry = Collection[i];
		CExtension *pExtension;

		//	Figure out which folder to look in

		CExtension::EFolderTypes iFolder;
        if (Entry.GetLicenseType() == CMultiverseCatalogEntry::licenseCore)
            iFolder = CExtension::folderBase;
        else if (Entry.GetLicenseType() == CMultiverseCatalogEntry::licenseSteamUGC)
            iFolder = CExtension::folderExtensions;
        else
			iFolder = CExtension::folderCollection;

		//	If this extension has been manually removed, then we mark it as 
		//	such.

		if (m_DisabledExtensions.Find(Entry.GetUNID()))
			Entry.SetStatus(CMultiverseCatalogEntry::statusPlayerDisabled);

		//	Look for this extension in our list.

		else if (FindExtension(Entry.GetUNID(), 0, iFolder, &pExtension))
			{
			if (pExtension->IsDisabled())
				Entry.SetStatus(CMultiverseCatalogEntry::statusError, pExtension->GetDisabledReason());
			else if (pExtension->IsRegistrationVerified()
                    || Entry.GetLicenseType() == CMultiverseCatalogEntry::licenseSteamUGC
					|| !pExtension->IsRegistered())
				Entry.SetStatus(CMultiverseCatalogEntry::statusLoaded);
			else
				Entry.SetStatus(CMultiverseCatalogEntry::statusCorrupt);

			//	Set the icon

			if (Entry.GetIcon() == NULL && pExtension->HasIcon())
				{
				CG32bitImage *pIcon;
				pExtension->CreateIcon(Options.cxIconSize, Options.cyIconSize, &pIcon);
				Entry.SetIcon(pIcon);
				}

			//	Set version

			Entry.SetVersion(pExtension->GetVersion());
			}

		//	If we can't find it, then we know that it's not loaded

		else
			Entry.SetStatus(CMultiverseCatalogEntry::statusNotAvailable);
		}
	}

void CExtensionCollection::UpdateRegistrationStatus (const TArray<CMultiverseCatalogEntry *> &Collection)

//	UpdateRegistrationStatus
//
//	Given the user's collection, set the registered bit on all appropriate
//	extensions.

	{
	CSmartLock Lock(m_cs);
	int i;

	for (i = 0; i < Collection.GetCount(); i++)
		{
		CMultiverseCatalogEntry *pEntry = Collection[i];

		//	Skip core entries

		if (pEntry->GetLicenseType() == CMultiverseCatalogEntry::licenseCore)
			continue;

        //  Skip Steam UGC

        if (pEntry->GetLicenseType() == CMultiverseCatalogEntry::licenseSteamUGC)
            continue;

		//	Look for this extension in our list. If we found it then compare
		//	the signature to make sure that we have the right version.

		CExtension *pExtension;
		if (FindExtension(pEntry->GetUNID(), pEntry->GetRelease(), CExtension::folderCollection, &pExtension))
			{
			//	Steam versions are always verified

			if (pEntry->GetLicenseType() == CMultiverseCatalogEntry::licenseSteam)
				pExtension->SetVerified();

			//	Compare the digests. If they match, then this is a registered
			//	extension.

			else if (pEntry->GetTDBFileRef().GetDigest() == pExtension->GetDigest())
				pExtension->SetVerified();

			//	If this is an unregistered version, then check to see if the version
			//	number is the same. If not, the we re-download.

			else if (!pExtension->IsRegistered()
					&& strEquals(pEntry->GetTDBFileRef().GetVersion(), pExtension->GetVersion()))
				{ }
			}
		}
	}

//	CLibraryResolver -----------------------------------------------------------

void CLibraryResolver::AddDefaults (CExtension *pExtension)

//	AddDefault
//
//	Add default library references for the given extension.

	{
	int i;
	CString sError;

	TArray<CExtension *> CoreLibraries;
	m_Extensions.ComputeCoreLibraries(pExtension, &CoreLibraries);

	for (i = 0; i < CoreLibraries.GetCount(); i++)
		AddLibrary(CoreLibraries[i]);

	//	Add the extension's entity table

	AddLibrary(pExtension);
	}

ALERROR CLibraryResolver::AddLibrary (DWORD dwUNID, DWORD dwRelease, bool bOptional, CString *retsError)

//	AddLibrary
//
//	Adds the library by UNID.

	{
	//	Get the best extension with this UNID. If we don't find it, then
	//	continue (we will report an error later when we can't find
	//	the entity).

	CExtension *pLibrary;
	if (!m_Extensions.FindBestExtension(dwUNID, dwRelease, (m_Extensions.LoadedInDebugMode() ? CExtensionCollection::FLAG_DEBUG_MODE : 0), &pLibrary))
		{
		//	If this is an optional library, then OK to skip it.

		if (bOptional)
			return NOERROR;

		//	Otherwise, ERROR

		*retsError = strPatternSubst(CONSTLIT("Unable to find library: %08x"), dwUNID);

		//	We return ERR_CANCEL to indicate that we should continue loading,
		//	even in debug mode.

		return ERR_CANCEL;
		}

	//	Is this a library?

	if (pLibrary->GetType() != extLibrary)
		{
		*retsError = strPatternSubst(CONSTLIT("Expected %s (%08x) to be a library"), pLibrary->GetName(), pLibrary->GetUNID());
		return ERR_FAIL;
		}

	//	Must at least have stubs

	if (pLibrary->GetLoadState() == CExtension::loadNone)
		{
		*retsError = strPatternSubst(CONSTLIT("Unable to find library: %08x"), dwUNID);
		return ERR_FAIL;
		}

	//	Add it to our list so that we can use it to resolve entities.

	AddLibrary(pLibrary);

	return NOERROR;
	}

ALERROR CLibraryResolver::OnOpenTag (CXMLElement *pElement, CString *retsError)

//	OnOpenTag
//
//	Parses a <Library> element

	{
	if (strEquals(pElement->GetTag(), LIBRARY_TAG))
		{
		DWORD dwUNID = pElement->GetAttributeInteger(UNID_ATTRIB);
		DWORD dwRelease = pElement->GetAttributeInteger(RELEASE_ATTRIB);
		bool bOptional = pElement->GetAttributeBool(OPTIONAL_ATTRIB);

		return AddLibrary(dwUNID, dwRelease, bOptional, retsError);
		}

	return NOERROR;
	}

CString CLibraryResolver::ResolveExternalEntity (const CString &sName, bool *retbFound)

//	ResolveExternalEntity
//
//	Resolves an entity.

	{
	int i;

	for (i = 0; i < m_Tables.GetCount(); i++)
		{
		bool bFound;
		CString sResult = m_Tables[i]->ResolveExternalEntity(sName, &bFound);
		if (bFound)
			{
			if (retbFound)
				*retbFound = true;
			return sResult;
			}
		}

	//	Not found

	if (retbFound)
		*retbFound = false;

	return NULL_STR;
	}
