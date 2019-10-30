//	TSEResourceDb.h
//
//	Transcendence Space Engine
//	Copyright 2019 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CResourceDb
	{
	public:
		CResourceDb (const CString &sDefFilespec, bool bExtension = false);
		~CResourceDb (void);

		void ComputeFileDigest (CIntegerIP *retDigest);
		IXMLParserController *GetEntities (void) const { return m_pEntities; }
		IXMLParserController *GetEntitiesHandoff (bool *retbFree) { IXMLParserController *pEntities = m_pEntities; *retbFree = m_bFreeEntities; m_pEntities = NULL; m_bFreeEntities = false; return pEntities; }
		const CString &GetFilespec (void) const { return m_sFilespec; }
		CString GetRootTag (void);
		bool ImageExists (const CString &sFolder, const CString &sFilename);
		bool IsTDB (void) const { return (m_pDb != NULL); }
		bool IsUsingExternalGameFile (void) const { return !m_bGameFileInDb; }
		bool IsUsingExternalResources (void) const { return !m_bResourcesInDb; }
		ALERROR LoadEmbeddedGameFile (const CString &sFilename, CXMLElement **retpData, IXMLParserController *pResolver, CExternalEntityTable *ioEntityTable, CString *retsError);
		ALERROR LoadEntities (CString *retsError, CExternalEntityTable **retEntities = NULL);
		ALERROR LoadGameFile (CXMLElement **retpData, IXMLParserController *pEntities, CString *retsError, CExternalEntityTable *ioEntityTable = NULL);
		ALERROR LoadGameFileStub (CXMLElement **retpData, CExternalEntityTable *ioEntityTable, CString *retsError);
		ALERROR LoadImage (const CString &sFolder, const CString &sFilename, HBITMAP *rethImage, EBitmapTypes *retiImageType = NULL);
		ALERROR LoadImageFile (const CString &sImageFilename, const CString &sMaskFilename, TUniquePtr<CG32bitImage> &pImage, bool bPreMult = false, CString *retsError = NULL);
		ALERROR LoadMaskFile (const CString &sMaskFilename, TUniquePtr<CG32bitImage> &pImage, CString *retsError = NULL);
		ALERROR LoadModule (const CString &sFolder, const CString &sFilename, CXMLElement **retpData, CString *retsError);
		ALERROR LoadModuleEntities (const CString &sFolder, const CString &sFilename, CExternalEntityTable **retpEntities, CString *retsError);
		ALERROR LoadSound (CSoundMgr &SoundMgr, const CString &sFolder, const CString &sFilename, int *retiChannel);
		ALERROR Open (DWORD dwFlags, CString *retsError);
		CString ResolveFilespec (const CString &sFolder, const CString &sFilename) const;
		void SetDebugMode (bool bValue) { m_bDebugMode = bValue; }
		void SetEntities (IXMLParserController *pEntities, bool bFree = false);

		CString GetResourceFilespec (int iIndex);
		int GetResourceCount (void);
		int GetVersion (void) { return m_iVersion; }
		ALERROR ExtractMain (CString *retsData);
		ALERROR ExtractResource (const CString sFilespec, CString *retsData);

	private:
		enum EFlags
			{
			//	SResourceEntry flags
			FLAG_COMPRESS_ZLIB =		0x00000001,
			};

		struct SResourceEntry
			{
			CString sFilename;
			int iEntryID;
			DWORD dwFlags;
			};

		ALERROR OpenDb (void);
		ALERROR LoadImageFileAndMask (const CString &sImageFilename, const CString &sMaskFilename, TUniquePtr<CG32bitImage> &pImage, bool bPreMult = false, CString *retsError = NULL);
		ALERROR LoadPNGFile (const CString &sImageFilename, TUniquePtr<CG32bitImage> &pImage, CString *retsError = NULL);
		ALERROR ReadEntry (const CString &sFilespec, CString *retsData);

		int m_iVersion;
		bool m_bGameFileInDb;
		bool m_bResourcesInDb;
		bool m_bDebugMode;

		//	If we're just using the file system
		CString m_sFilespec;
		CString m_sRoot;
		CString m_sGameFile;

		//	If we're using a TDB
		CDataFile *m_pDb;
		TSortMap<CString, SResourceEntry> m_ResourceMap;
		int m_iGameFile;

		IXMLParserController *m_pEntities;			//	Entities to use in parsing
		bool m_bFreeEntities;						//	If TRUE, we own m_pEntities;
	};
