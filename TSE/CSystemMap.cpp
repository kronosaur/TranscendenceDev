//	CSystemMap.cpp
//
//	CSystemMap class

#include "PreComp.h"

#define DEFINE_ZONE_TAG						CONSTLIT("DefineZone")
#define MAP_EFFECT_TAG						CONSTLIT("MapEffect")
#define NODE_TAG							CONSTLIT("Node")
#define RANDOM_TOPOLOGY_TAG					CONSTLIT("RandomTopology")
#define ROOT_NODE_TAG						CONSTLIT("RootNode")
#define STARGATE_TAG						CONSTLIT("Stargate")
#define STARGATES_TAG						CONSTLIT("Stargates")
#define SYSTEM_DISTRIBUTION_TAG				CONSTLIT("SystemTypes")
#define SYSTEM_TOPOLOGY_TAG					CONSTLIT("SystemTopology")
#define TOPOLOGY_CREATOR_TAG				CONSTLIT("TopologyCreator")
#define TOPOLOGY_PROCESSOR_TAG				CONSTLIT("TopologyProcessor")
#define USES_TAG							CONSTLIT("Uses")

#define BACKGROUND_IMAGE_ATTRIB				CONSTLIT("backgroundImage")
#define BACKGROUND_IMAGE_SCALE_ATTRIB		CONSTLIT("backgroundImageScale")
#define DEBUG_SHOW_ATTRIBUTES_ATTRIB		CONSTLIT("debugShowAttributes")
#define PRIMARY_MAP_ATTRIB					CONSTLIT("displayOn")
#define GRADIENT_RANGE_ATTRIB				CONSTLIT("gradientRange")
#define ID_ATTRIB							CONSTLIT("id")
#define ID_PREFIX_ATTRIB					CONSTLIT("idPrefix")
#define INITIAL_SCALE_ATTRIB				CONSTLIT("initialScale")
#define LIGHT_YEARS_PER_PIXEL_ATTRIB		CONSTLIT("lightYearsPerPixel")
#define MAX_SCALE_ATTRIB					CONSTLIT("maxScale")
#define MIN_SCALE_ATTRIB					CONSTLIT("minScale")
#define NAME_ATTRIB							CONSTLIT("name")
#define NODE_COUNT_ATTRIB					CONSTLIT("nodeCount")
#define PATTERN_ATTRIB						CONSTLIT("pattern")
#define STARGATE_PREFIX_ATTRIB				CONSTLIT("stargatePrefix")
#define STARGATE_LINE_COLOR_ATTRIB		    CONSTLIT("stargateLineColor")
#define STARTING_MAP_ATTRIB					CONSTLIT("startingMap")
#define UNID_ATTRIB							CONSTLIT("unid")
#define X_ATTRIB							CONSTLIT("x")
#define Y_ATTRIB							CONSTLIT("y")
#define ZONE_ATTRIB							CONSTLIT("zone")

#define CIRCULAR_PATTERN					CONSTLIT("circular")

const int MAX_NODE_PLACEMENT_LOOPS =		100;

const CG32bitPixel RGB_STARGATE_LINE_DEFAULT =	    CG32bitPixel(178, 217, 255);    //  H:210 S:30 B:100

CSystemMap::~CSystemMap (void)

//	CSystemMap destructor

	{
	int i;

	for (i = 0; i < m_Processors.GetCount(); i++)
		delete m_Processors[i];
	}

bool CSystemMap::AddAnnotation (const CString &sNodeID, CEffectCreator *pEffect, int x, int y, int iRotation, DWORD *retdwID)

//	AddAnnotation
//
//	Adds an annotation to the map. We take ownership of the painter. If adding 
//	fails, we return FALSE.

	{
	//	Create a new painter for the effect. If that fails, then there's nothing
	//	we can do.

	IEffectPainter *pPainter = pEffect->CreatePainter(CCreatePainterCtx());
	if (pPainter == NULL)
		return false;

	//	Create the annotation.

	SMapAnnotation *pAnnotation = m_Annotations.Insert();
	pAnnotation->dwID = g_pUniverse->CreateGlobalID();
	pAnnotation->sNodeID = sNodeID;
	pAnnotation->pPainter = pPainter;
	pAnnotation->xOffset = x;
	pAnnotation->yOffset = y;
	pAnnotation->iTick = 0;
	pAnnotation->iRotation = iRotation;

	//	If we have a node ID then we only show this annotation when the node
	//	is known to the player.

	pAnnotation->fHideIfNodeUnknown = !sNodeID.IsBlank();

	//	Done

	if (retdwID)
		*retdwID = pAnnotation->dwID;

	return true;
	}

ALERROR CSystemMap::AddFixedTopology (CTopology &Topology, TSortMap<DWORD, CTopologyNodeList> &NodesAdded, CString *retsError)

//	AddFixedTopology
//
//	Adds all the nodes in its fixed topology

	{
	ALERROR error;
	int i;

	//	If we already added this map, then we're done

	if (m_bAdded)
		return NOERROR;

	//	Mark this map as added so we don't recurse back here when we
	//	process all the Uses statments.

	m_bAdded = true;

	//	Load all the maps that this map requires

	for (i = 0; i < m_Uses.GetCount(); i++)
		{
		if (error = m_Uses[i]->AddFixedTopology(Topology, NodesAdded, retsError))
			return error;
		}

	//	Add background annotations

	if (!m_pBackgroundEffect.IsEmpty())
		AddAnnotation(m_pBackgroundEffect, 0, 0, 0);

	//	Iterate over all creators and execute them

	STopologyCreateCtx Ctx;
	Ctx.pMap = GetDisplayMap();
	Ctx.pNodesAdded = NodesAdded.SetAt((Ctx.pMap ? Ctx.pMap->GetUNID() : GetUNID()));

	//	We need to include any maps that we use.

	Ctx.Tables.Insert(&m_FixedTopology);
	for (i = 0; i < m_Uses.GetCount(); i++)
		Ctx.Tables.Insert(&m_Uses[i]->m_FixedTopology);

	for (i = 0; i < m_Creators.GetCount(); i++)
		{
		if (error = ExecuteCreator(Ctx, Topology, m_Creators[i]))
			{
			*retsError = strPatternSubst(CONSTLIT("SystemMap (%x): %s"), GetUNID(), Ctx.sError);
			return error;
			}
		}

	//	Add any additional nodes marked as "root" (this is here only for backwards compatibility)
	//	NOTE: This call only worries about the first table (Ctx.Tables[0])

	if (error = Topology.AddTopology(Ctx))
		{
		*retsError = strPatternSubst(CONSTLIT("SystemMap (%x): %s"), GetUNID(), Ctx.sError);
		return error;
		}

	return NOERROR;
	}

CG32bitImage *CSystemMap::CreateBackgroundImage (Metric *retrImageScale)

//	CreateBackgroundImage
//
//	Creates an image containing the map.
//	Caller is responsible for freeing the returned image.

	{
	int i;

	CG32bitImage *pImage = g_pUniverse->GetLibraryBitmapCopy(m_dwBackgroundImage);
	if (pImage == NULL)
		return NULL;

	int xCenter = pImage->GetWidth() / 2;
	int yCenter = pImage->GetHeight() / 2;

	//	Paint all annotations

	if (m_Annotations.GetCount() > 0)
		{
		SViewportPaintCtx Ctx;

		for (i = 0; i < m_Annotations.GetCount(); i++)
			{
			SMapAnnotation *pAnnotation = &m_Annotations[i];

			//	For annotations associated with a node, see if the player knows about
			//	the node first.

			if (!pAnnotation->sNodeID.IsBlank()
					&& pAnnotation->fHideIfNodeUnknown)
				{
				CTopologyNode *pNode = g_pUniverse->FindTopologyNode(pAnnotation->sNodeID);
				if (pNode == NULL
						|| !pNode->IsKnown())
					continue;
				}

			//	Adjust coordinates based on background image scale (sometimes, a background image
			//	is higher resolution than 1 pixel per map unit to deal with zooming).

			int xPos = xCenter + (int)(pAnnotation->xOffset * m_rBackgroundImageScale);
			int yPos = yCenter - (int)(pAnnotation->yOffset * m_rBackgroundImageScale);

			//	Paint the effect

			Ctx.iTick = pAnnotation->iTick;
			Ctx.iRotation = pAnnotation->iRotation;
			Ctx.rOffsetScale = m_rBackgroundImageScale;

			pAnnotation->pPainter->Paint(*pImage, xPos, yPos, Ctx);
			}
		}

	//	Let the topology processors paint stuff on top (usually debug information)

	for (i = 0; i < m_Processors.GetCount(); i++)
		m_Processors[i]->Paint(*pImage, xCenter, yCenter, m_rBackgroundImageScale);

	//	Paint any area highlights (again, usually for debugging).

	for (i = 0; i < m_AreaHighlights.GetCount(); i++)
		m_AreaHighlights[i].Paint(*pImage, xCenter, yCenter, m_rBackgroundImageScale);

	//	Return the scale

	if (retrImageScale)
		*retrImageScale = m_rBackgroundImageScale;

#ifdef DEBUG
#if 0
	//	Test noise function
	CG32bitImage Alpha;
	Alpha.CreateBlankAlpha(pImage->GetWidth(), pImage->GetHeight());
	DrawNebulosity8bit(Alpha, 0, 0, pImage->GetWidth(), pImage->GetHeight(), 256, 0, 255);

	pImage->Fill(0, 0, pImage->GetWidth(), pImage->GetHeight(), 0);
	pImage->FillMask(0, 0, pImage->GetWidth(), pImage->GetHeight(), Alpha, CG32bitPixel(255, 255, 255), 0, 0);
#endif
#endif

	return pImage;
	}

ALERROR CSystemMap::ExecuteCreator (STopologyCreateCtx &Ctx, CTopology &Topology, CXMLElement *pCreator)

//	ExecuteCreator
//
//	Runs a specific creator

	{
	ALERROR error;
	int i;

	//	If this is a root node tag then we add it and all its connections.

	if (strEquals(pCreator->GetTag(), ROOT_NODE_TAG))
		{
		if (error = Topology.AddTopologyNode(Ctx, pCreator->GetAttribute(ID_ATTRIB)))
			return error;
		}

	//	Otherwise we process the creator element

	else
		{
		for (i = 0; i < pCreator->GetContentElementCount(); i++)
			{
			CXMLElement *pDirective = pCreator->GetContentElement(i);

			if (strEquals(pDirective->GetTag(), NODE_TAG))
				{
				if (error = Topology.AddTopologyNode(Ctx, pDirective->GetAttribute(ID_ATTRIB)))
					return error;
				}
			else if (strEquals(pDirective->GetTag(), STARGATE_TAG) || strEquals(pDirective->GetTag(), STARGATES_TAG))
				{
				if (error = Topology.AddStargateFromXML(Ctx, pDirective))
					return error;
				}
			else
				{
				Ctx.sError = strPatternSubst(CONSTLIT("Unknown TopologyCreator directive: %s."), pDirective->GetTag());
				return ERR_FAIL;
				}
			}
		}

	return NOERROR;
	}

void CSystemMap::GetBackgroundImageSize (int *retcx, int *retcy)

//	GetBackgroundImageSize
//
//	Returns the size of the background image (in galactic coordinates).

	{
	CG32bitImage *pImage = g_pUniverse->GetLibraryBitmap(m_dwBackgroundImage);
	if (pImage)
		{
		*retcx = (int)(pImage->GetWidth() / m_rBackgroundImageScale);
		*retcy = (int)(pImage->GetHeight() / m_rBackgroundImageScale);
		}
	else
		{
		*retcx = 0;
		*retcy = 0;
		}
	}

ALERROR CSystemMap::OnBindDesign (SDesignLoadCtx &Ctx)

//	OnBindDesign
//
//	Bind the design

	{
	ALERROR error;
	int i;

	if (error = m_pPrimaryMap.Bind(Ctx))
		return error;

	for (i = 0; i < m_Uses.GetCount(); i++)
		if (error = m_Uses[i].Bind(Ctx))
			return error;

	if (error = m_FixedTopology.BindDesign(Ctx))
		return error;

	for (i = 0; i < m_Processors.GetCount(); i++)
		if (error = m_Processors[i]->BindDesign(Ctx))
			return error;

	if (error = m_pBackgroundEffect.Bind(Ctx))
		return error;

	return NOERROR;
	}

ALERROR CSystemMap::OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	OnCreateFromXML

	{
	ALERROR error;
	int i;

	//	Generate an UNID

	CString sUNID = strPatternSubst(CONSTLIT("%d"), GetUNID());

	//	Load some basic info

	m_sName = pDesc->GetAttribute(NAME_ATTRIB);
	m_dwBackgroundImage = pDesc->GetAttributeInteger(BACKGROUND_IMAGE_ATTRIB);
	if (error = m_pPrimaryMap.LoadUNID(Ctx, pDesc->GetAttribute(PRIMARY_MAP_ATTRIB)))
		return ComposeLoadError(Ctx, Ctx.sError);

	m_rBackgroundImageScale = pDesc->GetAttributeIntegerBounded(BACKGROUND_IMAGE_SCALE_ATTRIB, 10, 10000, 100) / 100.0;

	m_bStartingMap = pDesc->GetAttributeBool(STARTING_MAP_ATTRIB);

	//	If we have a primary map, then add it to the Uses list.

	if (m_pPrimaryMap.GetUNID() != 0)
		m_Uses.Insert(m_pPrimaryMap);

	//	Scale information

    m_rLightYearsPerPixel = pDesc->GetAttributeDoubleBounded(LIGHT_YEARS_PER_PIXEL_ATTRIB, 0.0, -1.0, 0.0);
	m_iInitialScale = pDesc->GetAttributeIntegerBounded(INITIAL_SCALE_ATTRIB, 10, 1000, 100);
	m_iMaxScale = pDesc->GetAttributeIntegerBounded(MAX_SCALE_ATTRIB, 100, 1000, 200);
	m_iMinScale = pDesc->GetAttributeIntegerBounded(MIN_SCALE_ATTRIB, 10, 100, 50);

    //  Other display information

    m_rgbStargateLines = ::LoadRGBColor(pDesc->GetAttribute(STARGATE_LINE_COLOR_ATTRIB), RGB_STARGATE_LINE_DEFAULT);

	//	Keep track of root nodes

	TArray<CString> RootNodes;

	//	Iterate over all child elements and process them

	for (i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pItem = pDesc->GetContentElement(i);

		if (strEquals(pItem->GetTag(), MAP_EFFECT_TAG))
			{
			if (m_pBackgroundEffect.IsEmpty())
				{
				if (error = m_pBackgroundEffect.LoadEffect(Ctx,
						strPatternSubst(CONSTLIT("%s:b"), sUNID),
						pItem,
						NULL_STR))
					return ComposeLoadError(Ctx, Ctx.sError);
				}
			}
		else if (strEquals(pItem->GetTag(), TOPOLOGY_CREATOR_TAG)
				|| strEquals(pItem->GetTag(), ROOT_NODE_TAG))
			{
			m_Creators.Insert(pItem);

			if (strEquals(pItem->GetTag(), ROOT_NODE_TAG))
				RootNodes.Insert(pItem->GetAttribute(ID_ATTRIB));
			}
		else if (strEquals(pItem->GetTag(), TOPOLOGY_PROCESSOR_TAG))
			{
			ITopologyProcessor *pNewProc;
			CString sProcessorUNID = strPatternSubst(CONSTLIT("%d:p%d"), GetUNID(), m_Processors.GetCount());

			if (error = ITopologyProcessor::CreateFromXMLAsGroup(Ctx, pItem, sProcessorUNID, &pNewProc))
				return ComposeLoadError(Ctx, Ctx.sError);

			m_Processors.Insert(pNewProc);
			}
		else if (strEquals(pItem->GetTag(), SYSTEM_TOPOLOGY_TAG))
			{
			if (error = m_FixedTopology.LoadFromXML(Ctx, pItem, this, sUNID, true))
				return ComposeLoadError(Ctx, Ctx.sError);
			}
		else if (strEquals(pItem->GetTag(), USES_TAG))
			{
			CSystemMapRef *pRef = m_Uses.Insert();

			if (error = pRef->LoadUNID(Ctx, pItem->GetAttribute(UNID_ATTRIB)))
				return ComposeLoadError(Ctx, Ctx.sError);
			}
		else if (IsValidLoadXML(pItem->GetTag()))
			{
			//	This is an element used by all design types; this is already handled by
			//	CDesignType, so we skip it here.
			}
		else
			{
			//	If it's none of the above, see if it is a node descriptor

			if (error = m_FixedTopology.LoadNodeFromXML(Ctx, pItem, this, sUNID))
				return ComposeLoadError(Ctx, Ctx.sError);
			}
		}

	//	Mark all the root nodes.
	//
	//	We need to do this for backwards compatibility because the old technique
	//	of having a root node with [Prev] for a stargate requires this. This was
	//	used by Huaramarca.

	for (i = 0; i < RootNodes.GetCount(); i++)
		if (error = m_FixedTopology.AddRootNode(Ctx, RootNodes[i]))
			return ComposeLoadError(Ctx, Ctx.sError);

	//	Init

	m_bAdded = false;

	//	Debug info

	m_bDebugShowAttributes = pDesc->GetAttributeBool(DEBUG_SHOW_ATTRIBUTES_ATTRIB);

	return NOERROR;
	}

CEffectCreator *CSystemMap::OnFindEffectCreator (const CString &sUNID)

//	OnFindEffectCreator
//
//	Find the effect. We start after the map UNID.
//
//	{unid}/{nodeID}
//	{unid}:b
//	{unid}:p{processorIndex}
//		  ^

	{
	//	We start after the map class UNID

	char *pPos = sUNID.GetASCIIZPointer();

	//	If we have a slash, then dive into the topology nodes

	if (*pPos == '/')
		{
		pPos++;
		return m_FixedTopology.FindEffectCreator(CString(pPos));
		}

	//	A colon means that it is some other element

	else if (*pPos == ':')
		{
		pPos++;

		//	'b' is background effect

		if (*pPos == 'b')
			{
			return m_pBackgroundEffect;
			}

		//	'p' is a processor

		else if (*pPos == 'p')
			{
			pPos++;

			//	Get the processor index

			int iIndex = ::strParseInt(pPos, -1, &pPos);
			if (iIndex < 0 || iIndex >= m_Processors.GetCount())
				return NULL;

			//	Let the processor handle the effect

			return m_Processors[iIndex]->FindEffectCreator(CString(pPos));
			}

		//	Otherwise, we don't know

		else
			return NULL;
		}

	//	Else, we don't know what it is

	else
		return NULL;
	}

void CSystemMap::OnReadFromStream (SUniverseLoadCtx &Ctx)

//	OnReadFromStream
//
//	Read the variable parts
//
//	DWORD		No. of annotations
//	DWORD		annotation: dwID
//	CString		sNodeID
//	DWORD		flags
//	IEffectPainter	annotation: pPainter
//	DWORD		annotation: xOffset
//	DWORD		annotation: yOffset
//	DWORD		annotation: iTick
//	DWORD		annotation: iRotation
//
//	DWORD		No. of area highlights
//	CComplexArea

	{
	int i;
	DWORD dwLoad;

	if (Ctx.dwVersion < 9)
		return;

	Ctx.pStream->Read(dwLoad);
	m_Annotations.InsertEmpty(dwLoad);

	for (i = 0; i < m_Annotations.GetCount(); i++)
		{
		Ctx.pStream->Read(m_Annotations[i].dwID);

		//	Node ID and flags

		if (Ctx.dwSystemVersion >= 154)
			{
			Ctx.pStream->Read(m_Annotations[i].sNodeID);

			DWORD dwFlags;
			Ctx.pStream->Read(dwFlags);
			m_Annotations[i].fHideIfNodeUnknown = ((dwFlags & 0x00000001) ? true : false);

			}
		else
			m_Annotations[i].fHideIfNodeUnknown = false;

		//	To load the painter we need to cons up an SLoadCtx. Fortunately.
		//	we have the system version saved in the universe load ctx.

		m_Annotations[i].pPainter = CEffectCreator::CreatePainterFromStream(SLoadCtx(Ctx));

		//	Load remaining fields

		Ctx.pStream->Read(m_Annotations[i].xOffset);
		Ctx.pStream->Read(m_Annotations[i].yOffset);
		Ctx.pStream->Read(m_Annotations[i].iTick);
		Ctx.pStream->Read(m_Annotations[i].iRotation);
		}

	//	Read area highlights

	if (Ctx.dwVersion >= 33)
		{
		Ctx.pStream->Read(dwLoad);
		m_AreaHighlights.InsertEmpty(dwLoad);

		for (i = 0; i < m_AreaHighlights.GetCount(); i++)
			m_AreaHighlights[i].ReadFromStream(*Ctx.pStream);
		}

	//	Delete any anotations with a NULL painter. This can happen (unfortunately) when 
	//	we change the definition.

	for (i = 0; i < m_Annotations.GetCount(); i++)
		if (m_Annotations[i].pPainter == NULL)
			{
			m_Annotations.Delete(i);
			i--;
			}
	}

void CSystemMap::OnReinit (void)

//	OnReinit
//
//	Reinitialize for a new game

	{
	m_bAdded = false;
	m_Annotations.DeleteAll();
	m_AreaHighlights.DeleteAll();
	}

void CSystemMap::OnWriteToStream (IWriteStream *pStream)

//	OnWriteToStream
//
//	Write variable parts
//
//	DWORD		No. of annotations
//	DWORD		annotation: dwID
//	CString		sNodeID
//	DWORD		flags
//	IEffectPainter	annotation: pPainter
//	DWORD		annotation: xOffset
//	DWORD		annotation: yOffset
//	DWORD		annotation: iTick
//	DWORD		annotation: iRotation
//
//	DWORD		No. of area highlights
//	CComplexArea

	{
	int i;
	DWORD dwSave;

	dwSave = m_Annotations.GetCount();
	pStream->Write(dwSave);

	for (i = 0; i < m_Annotations.GetCount(); i++)
		{
		pStream->Write(m_Annotations[i].dwID);
		pStream->Write(m_Annotations[i].sNodeID);

		DWORD dwFlags = 0;
		dwFlags |= (m_Annotations[i].fHideIfNodeUnknown ? 0x00000001 : 0);
		pStream->Write(dwFlags);

		CEffectCreator::WritePainterToStream(pStream, m_Annotations[i].pPainter);
		pStream->Write(m_Annotations[i].xOffset);
		pStream->Write(m_Annotations[i].yOffset);
		pStream->Write(m_Annotations[i].iTick);
		pStream->Write(m_Annotations[i].iRotation);
		}

	dwSave = m_AreaHighlights.GetCount();
	pStream->Write(dwSave);

	for (i = 0; i < m_AreaHighlights.GetCount(); i++)
		m_AreaHighlights[i].WriteToStream(*pStream);
	}

ALERROR CSystemMap::ProcessTopology (CTopology &Topology, CSystemMap *pTargetMap, CTopologyNodeList &NodesAdded, CString *retsError)

//	ProcessTopology
//
//	Process the topology over any nodes added to this map.

	{
	ALERROR error;
	int i;

	//	Apply any topology processors (in order) on all the newly added nodes

	for (i = 0; i < m_Processors.GetCount(); i++)
		{
		//	Make a copy of the node list because each call will destroy it

		CTopologyNodeList NodeList = NodesAdded;

		//	Process

		if (error = m_Processors[i]->Process(pTargetMap, Topology, NodeList, retsError))
			{
			*retsError = strPatternSubst(CONSTLIT("SystemMap (%x): %s"), GetUNID(), *retsError);
			return error;
			}
		}

	return NOERROR;
	}

int KeyCompare (const CSystemMap::SSortEntry &Key1, const CSystemMap::SSortEntry &Key2)
	{
	if (Key1.iSort > Key2.iSort)
		return 1;
	else if (Key1.iSort < Key2.iSort)
		return -1;
	else
		return 0;
	}
