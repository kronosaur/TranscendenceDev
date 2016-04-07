//	CAttributeDataBlock.cpp
//
//	CAttributeDataBlock class

#include "PreComp.h"

#define DATA_ATTRIB								CONSTLIT("data")
#define ID_ATTRIB								CONSTLIT("id")

CAttributeDataBlock::CAttributeDataBlock (void) :
		m_pObjRefData(NULL)

//	CAttributeDataBlock constructor

	{
	}

CAttributeDataBlock::CAttributeDataBlock (const CAttributeDataBlock &Src) :
		m_pObjRefData(NULL)

//	CAttributeDataBlock constructor

	{
	Copy(Src);
	}

CAttributeDataBlock::~CAttributeDataBlock (void)

//	CAttributeDataBlock destructor

	{
	CleanUp();
	}

CAttributeDataBlock &CAttributeDataBlock::operator= (const CAttributeDataBlock &Src)

//	CAttributeDataBlock equals operator

	{
	Copy(Src);
	return *this;
	}

void CAttributeDataBlock::CleanUp (void)

//	CleanUp
//
//	Destroy all entries

	{
    m_Data.DeleteAll();
    CleanUpObjRefs();
	}

void CAttributeDataBlock::CleanUpObjRefs (void)

//  CleanUpObjRefs
//
//  Clean up

    {
	SObjRefEntry *pNext = m_pObjRefData;
	while (pNext)
		{
		SObjRefEntry *pDeleteMe = pNext;
		pNext = pNext->pNext;
		delete pDeleteMe;
		}

	m_pObjRefData = NULL;
    }

void CAttributeDataBlock::Copy (const CAttributeDataBlock &Copy)

//	Copy
//
//	Copies

	{
	CleanUp();

    m_Data = Copy.m_Data;
    CopyObjRefs(Copy.m_pObjRefData);
	}

void CAttributeDataBlock::Copy (const CAttributeDataBlock &Src, const TSortMap<CString, STransferDesc> &Options)

//  Copy
//
//  Copies selectively

    {
    int i;

    //  Transfer data from source appropriately

    for (i = 0; i < Src.m_Data.GetCount(); i++)
        {
        STransferDesc *pDesc = Options.GetAt(Src.m_Data.GetKey(i));
        ETransferOptions iTrans = (pDesc ? pDesc->iOption : transCopy);

        switch (iTrans)
            {
            case transCopy:
                SetData(Src.m_Data.GetKey(i), Src.m_Data[i].sData);
                break;

            case transIgnore:
                break;

            default:
                ASSERT(false);
            }
        }

    //  Copy object references

    CleanUpObjRefs();
    CopyObjRefs(Src.m_pObjRefData);
    }

void CAttributeDataBlock::CopyObjRefs (SObjRefEntry *pSrc)

//  CopyObjRefs
//
//  Copy object references

    {
    SObjRefEntry *pSrcNext = pSrc;
	SObjRefEntry *pDest = NULL;
	while (pSrcNext)
		{
		SObjRefEntry *pNew = new SObjRefEntry;
		pNew->sName = pSrcNext->sName;
		pNew->pObj = pSrcNext->pObj;
		pNew->dwObjID = pSrcNext->dwObjID;
		pNew->pNext = NULL;

		if (pDest == NULL)
			m_pObjRefData = pNew;
		else
			pDest->pNext = pNew;

		pDest = pNew;
		pSrcNext = pSrcNext->pNext;
		}
    }

bool CAttributeDataBlock::FindData (const CString &sAttrib, const CString **retpData) const

//	FindData
//
//	Finds string data

	{
    SDataEntry *pEntry = m_Data.GetAt(sAttrib);
    if (pEntry == NULL)
        return false;

    if (retpData)
        *retpData = &pEntry->sData;

    return !pEntry->sData.IsBlank();
	}

bool CAttributeDataBlock::FindObjRefData (CSpaceObject *pObj, CString *retsAttrib) const

//	FindObjRefData
//
//	Find the attribute that has the given object

	{
	SObjRefEntry *pNext = m_pObjRefData;
	while (pNext)
		{
		if (pNext->pObj == pObj)
			{
			if (retsAttrib)
				*retsAttrib = pNext->sName;
			return true;
			}
		pNext = pNext->pNext;
		}

	return false;
	}

const CString &CAttributeDataBlock::GetData (const CString &sAttrib) const

//	GetData
//
//	Returns string data associated with attribute

	{
    SDataEntry *pEntry = m_Data.GetAt(sAttrib);
    if (pEntry == NULL)
        return NULL_STR;

    return pEntry->sData;
	}

CSpaceObject *CAttributeDataBlock::GetObjRefData (const CString &sAttrib) const

//	GetObjRefData
//
//	Returns CSpaceObject data associated with attribute

	{
	SObjRefEntry *pEntry = m_pObjRefData;
	while (pEntry)
		{
		if (strEquals(sAttrib, pEntry->sName))
			{
			if (pEntry->pObj == NULL && pEntry->dwObjID != OBJID_NULL)
				pEntry->pObj = g_pUniverse->FindObject(pEntry->dwObjID);

			return pEntry->pObj;
			}

		pEntry = pEntry->pNext;
		}

	return NULL;
	}

void CAttributeDataBlock::IncData (const CString &sAttrib, ICCItem *pValue, ICCItem **retpNewValue)

//  IncData
//
//  Increments the given attribute, handling both integer and double values,
//  and optionally returning the new value (which must be discarded by the
//  caller).

    {
    CCodeChain &CC = g_pUniverse->GetCC();
    SDataEntry *pEntry = m_Data.SetAt(sAttrib);

    //  If pValue is NULL, we default to 1. We add ref no matter what so that
    //  we can discard unconditionally.

    if (pValue == NULL)
        pValue = CC.CreateInteger(1);
    else
        pValue->Reference();

    //  If the entry is currently blank, then we just take the increment.

    ICCItem *pResult = NULL;
    if (pEntry->sData.IsBlank())
        pResult = pValue->Reference();

    //  Otherwise, we need to get the data value

    else
        {
		ICCItem *pOriginal = CC.Link(pEntry->sData, 0, NULL);

        if (pOriginal->IsDouble() || pValue->IsDouble())
            pResult = CC.CreateDouble(pOriginal->GetDoubleValue() + pValue->GetDoubleValue());
        else
            pResult = CC.CreateInteger(pOriginal->GetIntegerValue() + pValue->GetIntegerValue());

        pOriginal->Discard(&CC);
        }

    //  Store

	pEntry->sData = CC.Unlink(pResult);

    //  Done

    if (retpNewValue)
        *retpNewValue = pResult;
    else
        pResult->Discard(&CC);

    pValue->Discard(&CC);
    }

bool CAttributeDataBlock::IsEqual (const CAttributeDataBlock &Src)

//	IsEqual
//
//	Returns TRUE if the two data blocks are the same

	{
	int i;

	if (m_Data.GetCount() != Src.m_Data.GetCount())
		return false;

	for (i = 0; i < m_Data.GetCount(); i++)
		{
		if (!strEquals(m_Data.GetKey(i), Src.m_Data.GetKey(i)))
			return false;

        const SDataEntry &DestEntry = m_Data[i];
        const SDataEntry &SrcEntry = Src.m_Data[i];

		if (!strEquals(DestEntry.sData, SrcEntry.sData))
			return false;
		}

	SObjRefEntry *pDest = m_pObjRefData;
	SObjRefEntry *pSrc = Src.m_pObjRefData;
	while (pDest && pSrc)
		{
		if (!strEquals(pDest->sName, pSrc->sName))
			return false;

		if (pDest->dwObjID != pSrc->dwObjID)
			return false;

		pDest = pDest->pNext;
		pSrc = pSrc->pNext;
		}

	return (pDest == NULL && pSrc == NULL);
	}

bool CAttributeDataBlock::IsXMLText (const CString &sData) const

//	IsXMLText
//
//	Returns TRUE if the given data value is an XML source stream.

	{
	char *pPos = sData.GetASCIIZPointer();

	while (strIsWhitespace(pPos))
		pPos++;

	return (*pPos == '<');
	}

void CAttributeDataBlock::LoadObjReferences (CSystem *pSystem)

//	LoadObjReferences
//
//	Load all the object references now that it's safe

	{
	SObjRefEntry *pEntry = m_pObjRefData;
	while (pEntry)
		{
		if (pEntry->pObj == NULL && pEntry->dwObjID != OBJID_NULL)
			pEntry->pObj = pSystem->FindObject(pEntry->dwObjID);

		pEntry = pEntry->pNext;
		}
	}

void CAttributeDataBlock::MergeFrom (const CAttributeDataBlock &Src)

//	MergeFrom
//
//	Merges data from source

	{
	int i;

	//	Plain data

	for (i = 0; i < Src.m_Data.GetCount(); i++)
		{
        const SDataEntry &Entry = Src.m_Data[i];
		SetData(Src.m_Data.GetKey(i), Entry.sData);
		}

	//	Object reference

	SObjRefEntry *pEntry = Src.m_pObjRefData;
	while (pEntry)
		{
		SetObjRefData(pEntry->sName, pEntry->pObj);

		pEntry = pEntry->pNext;
		}
	}

void CAttributeDataBlock::OnObjDestroyed (CSpaceObject *pObj)

//	OnObjDestroyed
//
//	Object has been destroyed

	{
	DWORD dwObjID = pObj->GetID();
	SObjRefEntry *pEntry = m_pObjRefData;
	while (pEntry)
		{
		if (pEntry->dwObjID == dwObjID || pObj == pEntry->pObj)
			{
			pEntry->dwObjID = OBJID_NULL;
			pEntry->pObj = NULL;
			}

		pEntry = pEntry->pNext;
		}
	}

void CAttributeDataBlock::OnSystemChanged (CSystem *pSystem)

//	OnSystemChanged
//
//	We are now in a different system, so we need to clear out any pointers that
//	we might have to old system objects.

	{
	SObjRefEntry *pEntry = m_pObjRefData;
	while (pEntry)
		{
		pEntry->pObj = NULL;
		pEntry = pEntry->pNext;
		}
	}

void CAttributeDataBlock::ReadDataEntries (IReadStream *pStream)

//  ReadDataEntries
//
//  Read the m_Data table.

    {
    int i;

	//	Load the sentinel string, which is either "ADB" or a flattened table
    //  (for the old version).

    CString sData;
    sData.ReadFromStream(pStream);

    //  If this is empty, then we've got no data.

    if (sData.IsBlank())
        { }

    //  If this is the sentinel string, then we have the new format

    else if (strEquals(sData, CONSTLIT("ADB")))
        {
        DWORD dwCount;
    	pStream->Read((char *)&dwCount, sizeof(DWORD));
        for (i = 0; i < (int)dwCount; i++)
            {
            CString sKey;
            sKey.ReadFromStream(pStream);
            SDataEntry *pEntry = m_Data.SetAt(sKey);

            pEntry->sData.ReadFromStream(pStream);
            }
        }

    //  Otherwise, this is the old, backward compatible format.

    else
        {
        CSymbolTable *pData = NULL;
	    CObject::Unflatten(sData, (CObject **)&pData);
        if (pData)
            {
            for (i = 0; i < pData->GetCount(); i++)
                {
                SDataEntry *pEntry = m_Data.SetAt(pData->GetKey(i));
                CString *pDest = (CString *)pData->GetValue(i);
                pEntry->sData = *pDest;
                }

            delete pData;
            }
        }
    }

void CAttributeDataBlock::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Reads from a stream
//
//  CString     "ADB"
//	DWORD       No of data entries
//  CString     sKey
//  CString     sData
//
//	DWORD		No of obj references
//	CString		ref: name
//	DWORD		ref: pointer (CSpaceObject ref)

	{
    int i;
	DWORD dwCount;

	DeleteAll();

	//	Load the opaque data table

    ReadDataEntries(Ctx.pStream);

	//	Load object reference

	Ctx.pStream->Read((char *)&dwCount, sizeof(DWORD));
	if (dwCount)
		{
		SObjRefEntry *pPrev = NULL;
		for (i = 0; i < (int)dwCount; i++)
			{
			SObjRefEntry *pEntry = new SObjRefEntry;
			if (pPrev)
				pPrev->pNext = pEntry;
			else
				m_pObjRefData = pEntry;

			pEntry->sName.ReadFromStream(Ctx.pStream);

			//	For later versions don't bind the object until someone asks for it

			if (Ctx.dwVersion >= 41)
				{
				Ctx.pStream->Read((char *)&pEntry->dwObjID, sizeof(DWORD));
				pEntry->pObj = NULL;
				}
			else
				{
				CSystem::ReadObjRefFromStream(Ctx, &pEntry->pObj);
				pEntry->dwObjID = OBJID_NULL;
				}

			pEntry->pNext = NULL;

			pPrev = pEntry;
			}
		}
	}

void CAttributeDataBlock::ReadFromStream (IReadStream *pStream)

//	ReadFromStream
//
//	Reads from a stream
//
//	DWORD       No of data entries
//  CString     sKey
//  CString     sData
//
//	DWORD		No of obj references
//	CString		ref: name
//	DWORD		ref: pointer (CSpaceObject ref)

	{
	DeleteAll();

	//	Load the opaque data table

    ReadDataEntries(pStream);

	//	Load object reference (since we don't have a system,
	//	we ignore all references)

	DWORD dwCount;
	pStream->Read((char *)&dwCount, sizeof(DWORD));
	if (dwCount)
		{
		ASSERT(false);

		for (int i = 0; i < (int)dwCount; i++)
			{
			CString sDummy;
			DWORD dwDummy;

			sDummy.ReadFromStream(pStream);
			pStream->Read((char *)&dwDummy, sizeof(DWORD));
			}
		}
	}

void CAttributeDataBlock::SetData (const CString &sAttrib, const CString &sData)

//	SetData
//
//	Sets string data associated with attribute

	{
    SDataEntry *pEntry = m_Data.SetAt(sAttrib);
    pEntry->sData = sData;
	}

void CAttributeDataBlock::SetFromXML (CXMLElement *pData)

//	SetFromXML
//
//	Initializes data from an XML element

	{
	if (pData)
		{
		for (int i = 0; i < pData->GetContentElementCount(); i++)
			{
			CXMLElement *pItem = pData->GetContentElement(i);
			CString sID;
			CString sData;

			//	ID of data is either in id= attrib or element tag.

			if (!pItem->FindAttribute(ID_ATTRIB, &sID))
				sID = pItem->GetTag();

			//	Dats is either in data= attrib or content text

			sData = pItem->GetAttribute(DATA_ATTRIB);
			if (sData.IsBlank())
				{
				sData = pItem->GetContentText(0);

				//	If this item begins with '<' then we quote it (and escape it) so that it
				//	can be a TLisp string.

				if (IsXMLText(sData))
					sData = CCString::Print(sData);
				}

			//	Store

			SetData(sID, sData);
			}
		}
	}

void CAttributeDataBlock::SetObjRefData (const CString &sAttrib, CSpaceObject *pObj)

//	SetObjRefData
//
//	Sets CSpaceObject data associated with attribute

	{
	DWORD dwObjID = (pObj ? pObj->GetID() : OBJID_NULL);

	SObjRefEntry *pEntry = m_pObjRefData;
	while (pEntry)
		{
		if (strEquals(sAttrib, pEntry->sName))
			{
			pEntry->pObj = pObj;
			pEntry->dwObjID = dwObjID;
			return;
			}

		pEntry = pEntry->pNext;
		}

	pEntry = new SObjRefEntry;
	pEntry->sName = sAttrib;
	pEntry->pObj = pObj;
	pEntry->dwObjID = dwObjID;
	pEntry->pNext = m_pObjRefData;
	m_pObjRefData = pEntry;
	}

void CAttributeDataBlock::WriteToStream (IWriteStream *pStream, CSystem *pSystem)

//	WriteToStream
//
//	Write the object to a stream
//
//  CString     "ADB"
//	DWORD       No of data entries
//  CString     sKey
//  CString     sData
//
//	DWORD		No of obj references
//	CString		ref: name
//	DWORD		ref: pointer (CSpaceObject ref)

	{
    int i;
    DWORD dwCount;

    //  If we have no data, we write out an empty string, which is valid
    //  in both old and new formats.

    if (m_Data.GetCount() == 0)
        NULL_STR.WriteToStream(pStream);

    //  Otherwise, we write out the new format

    else
        {
        //  Write out a string to indicate that we're using the new  format.

        CONSTLIT("ADB").WriteToStream(pStream);

	    //	Write out the opaque data

        dwCount = m_Data.GetCount();
	    pStream->Write((char *)&dwCount, sizeof(DWORD));
        for (i = 0; i < m_Data.GetCount(); i++)
            {
            m_Data.GetKey(i).WriteToStream(pStream);
            m_Data[i].sData.WriteToStream(pStream);
            }
        }

	//	Write out object references

	dwCount = 0;
	if (pSystem)
		{
		SObjRefEntry *pEntry = m_pObjRefData;
		while (pEntry)
			{
			dwCount++;
			pEntry = pEntry->pNext;
			}

		pStream->Write((char *)&dwCount, sizeof(DWORD));

		pEntry = m_pObjRefData;
		while (pEntry)
			{
			pEntry->sName.WriteToStream(pStream);

			//	For previous versions we sometimes load the object and don't
			//	yet have the ID

			if (pEntry->pObj && pEntry->dwObjID == OBJID_NULL)
				pEntry->dwObjID = pEntry->pObj->GetID();

			pStream->Write((char *)&pEntry->dwObjID, sizeof(DWORD));
			pEntry = pEntry->pNext;
			}
		}
	else
		pStream->Write((char *)&dwCount, sizeof(DWORD));
	}
