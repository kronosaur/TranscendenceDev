//	CGImageCache.cpp
//
//	CGImageCache Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "PixelMacros.h"

CGImageCache::~CGImageCache (void)

//	CGImageCache destructor

	{
	int i;

	for (i = 0; i < m_Cache8.GetCount(); i++)
		delete m_Cache8[i];

	for (i = 0; i < m_Cache32.GetCount(); i++)
		delete m_Cache32[i];
	}

CG8bitImage *CGImageCache::Alloc8 (void)

//	Alloc8
//
//	Allocated a new image and returns it. We still maintain ownership of the
//	object, however. The caller MUST NOT free the pointer (though they are free
//	to call Free or Sweep.

	{
	CG8bitImage *pNewImage = new CG8bitImage;
	m_Cache8.Insert(pNewImage);
	return pNewImage;
	}

CG32bitImage *CGImageCache::Alloc32 (void)

//	Alloc32
//
//	Allocated a new image and returns it. We still maintain ownership of the
//	object, however. The caller MUST NOT free the pointer (though they are free
//	to call Free or Sweep.

	{
	CG32bitImage *pNewImage = new CG32bitImage;
	m_Cache32.Insert(pNewImage);
	return pNewImage;
	}

void CGImageCache::ClearMarks (void)

//	ClearMarks
//
//	Clears all marks

	{
	int i;

	for (i = 0; i < m_Cache8.GetCount(); i++)
		m_Cache8[i]->SetMarked(false);

	for (i = 0; i < m_Cache32.GetCount(); i++)
		m_Cache32[i]->SetMarked(false);
	}

void CGImageCache::Free8 (CG8bitImage *pImage)

//	Free8
//
//	Frees a given image previously created with Alloc. This should only be 
//	called when the caller can guarantee that no one else holds the pointer.

	{
	int i;

	for (i = 0; i < m_Cache8.GetCount(); i++)
		if (m_Cache8[i] == pImage)
			{
			m_Cache8.Delete(i);
			delete pImage;
			break;
			}
	}

void CGImageCache::Free32 (CG32bitImage *pImage)

//	Free32
//
//	Frees a given image previously created with Alloc. This should only be 
//	called when the caller can guarantee that no one else holds the pointer.

	{
	int i;

	for (i = 0; i < m_Cache32.GetCount(); i++)
		if (m_Cache32[i] == pImage)
			{
			m_Cache32.Delete(i);
			delete pImage;
			break;
			}
	}

void CGImageCache::Sweep (void)

//	Sweep
//
//	Releases memory from all images that are not marked. But we DO NOT delete 
//	the image. Callers may recreate the image using the same pointer returned by
//	Alloc.

	{
	int i;

	for (i = 0; i < m_Cache8.GetCount(); i++)
		if (!m_Cache8[i]->IsMarked())
			m_Cache8[i]->CleanUp();

	for (i = 0; i < m_Cache32.GetCount(); i++)
		if (!m_Cache32[i]->IsMarked())
			m_Cache32[i]->CleanUp();
	}
