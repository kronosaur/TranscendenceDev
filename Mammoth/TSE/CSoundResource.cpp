//	CSoundResource.cpp
//
//	CSoundResource class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define FILENAME_ATTRIB							CONSTLIT("filename")

CSoundResource::CSoundResource (void) :
		m_iChannel(-1),
		m_bMarked(false)

//	CSoundResource constructor

	{
	}

int CSoundResource::GetSound (void) const

//	GetSound
//
//	Returns the channel for the given resource (or -1 if we could not load the
//	sound).

	{
	if (m_iChannel == -1)
		LoadResource();

	return m_iChannel;
	}

void CSoundResource::LoadResource (void) const

//	LoadResource
//
//	Loads resource into memory.

	{
	CString sError;

	//	If no sounds, then skip

	CSoundMgr *pSoundMgr = GetUniverse().GetSoundMgr();
	if (pSoundMgr == NULL)
		return;

	//	If already loaded, nothing to do

	if (m_iChannel != -1)
		return;

	//	Open the database

	CResourceDb ResDb(m_sResourceDb, !strEquals(m_sResourceDb, g_pUniverse->GetResourceDb()));
	ResDb.SetDebugMode(GetUniverse().InDebugMode());
	if (ResDb.Open(DFOPEN_FLAG_READ_ONLY, &sError) != NOERROR)
		{
		::kernelDebugLogPattern("Unable to open resource db: %s", m_sResourceDb);
		return;
		}

	//	Load the sound

	if (ResDb.LoadSound(*pSoundMgr, NULL_STR, m_sFilename, &m_iChannel) != NOERROR)
		{
		::kernelDebugLogPattern("Unable to load sound: %s", m_sFilename);
		m_iChannel = -1;
		return;
		}

	//	Done
	}

ALERROR CSoundResource::OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	OnCreateFromXML
//
//	Initialize

	{
	//	Get the file paths

	m_sResourceDb = Ctx.sResDb;
	if (Ctx.sFolder.IsBlank())
		{
		m_sFilename = pDesc->GetAttribute(FILENAME_ATTRIB);
		}
	else
		{
		CString sFilespec;
		if (pDesc->FindAttribute(FILENAME_ATTRIB, &sFilespec))
			m_sFilename = pathAddComponent(Ctx.sFolder, sFilespec);
		}

	//	Done

	return NOERROR;
	}

void CSoundResource::OnSweep (void)

//	OnSweep
//
//	Unload unmarked resources

	{
	DEBUG_TRY

	if (!m_bMarked)
		UnloadResource();

	DEBUG_CATCH
	}

void CSoundResource::OnUnbindDesign (void)

//	OnUnbindDesign
//
//	Unbind

	{
	UnloadResource();
	}

void CSoundResource::UnloadResource (void)

//	UnloadResource
//
//	Unloads the resource

	{
	//	If already unloaded, nothing to do

	if (m_iChannel == -1)
		return;

	//	Unload

	GetUniverse().GetSoundMgr()->Delete(m_iChannel);
	m_iChannel = -1;
	}
