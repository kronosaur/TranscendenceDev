//	CEffectGroupCreator.cpp
//
//	CEffectGroupCreator class

#include "PreComp.h"

#define EFFECTS_TAG								CONSTLIT("Effects")

#define ANGLE_OFFSET_ATTRIB						CONSTLIT("angleOffset")
#define EFFECTS_ATTRIB							CONSTLIT("effects")
#define RADIUS_OFFSET_ATTRIB					CONSTLIT("radiusOffset")
#define ROTATION_ADJ_ATTRIB						CONSTLIT("rotationAdj")
#define X_OFFSET_ATTRIB							CONSTLIT("xOffset")
#define Y_OFFSET_ATTRIB							CONSTLIT("yOffset")

//	CEffectGroupPainter -------------------------------------------------------

class CEffectGroupPainter : public IEffectPainter
	{
	public:
		CEffectGroupPainter (CEffectGroupCreator *pCreator, CCreatePainterCtx &Ctx);

		virtual ~CEffectGroupPainter (void) override;
		virtual bool CanPaintComposite (void) override;
		virtual CEffectCreator *GetCreator (void) override { return m_pCreator; }
		virtual int GetFadeLifetime (bool bHit) const override;
		virtual int GetLifetime (void) override;
		virtual void GetRect (RECT *retRect) const override;
		virtual void OnBeginFade (void) override;
		virtual void OnMove (SEffectMoveCtx &Ctx, bool *retbBoundsChanged) override;
		virtual void OnUpdate (SEffectUpdateCtx &Ctx) override;
		virtual void Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void PaintComposite (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void PaintFade (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void PaintHit (CG32bitImage &Dest, int x, int y, const CVector &vHitPos, SViewportPaintCtx &Ctx) override;
		virtual void PaintLine (CG32bitImage &Dest, const CVector &vHead, const CVector &vTail, SViewportPaintCtx &Ctx) override;
		virtual bool PointInImage (int x, int y, int iTick, int iVariant = 0, int iRotation = 0) const override;
		virtual void SetParamStruct (CCreatePainterCtx &Ctx, const CString &sParam, ICCItem *pValue) override;
		virtual void SetVariants (int iVariants) override;
		virtual bool UsesOrigin (void) const override;

	protected:
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		inline bool IsAlive (IEffectPainter *pPainter, int iTick)
			{
			if (pPainter)
				{
				int iLifetime = pPainter->GetLifetime();
				return (iLifetime <= 0 || iTick < iLifetime);
				}
			else
				return false;
			}

		SViewportPaintCtx *AdjustCtx (SViewportPaintCtx &Ctx, SViewportPaintCtx &NewCtx, int *retx, int *rety);
		SViewportPaintCtx *AdjustCtx (SViewportPaintCtx &Ctx, SViewportPaintCtx &NewCtx, CVector &vHead, CVector &vTail);

		CEffectGroupCreator *m_pCreator;
		TArray<CEffectPainterRef> m_Painters;
	};

CEffectGroupPainter::CEffectGroupPainter (CEffectGroupCreator *pCreator, CCreatePainterCtx &Ctx) : m_pCreator(pCreator)

//	CEffectGroupPainter constructor

	{
	m_Painters.InsertEmpty(m_pCreator->GetCount());

	for (int i = 0; i < m_Painters.GetCount(); i++)
		m_Painters[i].Set(m_pCreator->CreateSubPainter(Ctx, i));
	}

CEffectGroupPainter::~CEffectGroupPainter (void)

//	CEffectGroupPainter destructor

	{
	for (int i = 0; i < m_Painters.GetCount(); i++)
		m_Painters[i].Delete();
	}

SViewportPaintCtx *CEffectGroupPainter::AdjustCtx (SViewportPaintCtx &Ctx, SViewportPaintCtx &NewCtx, int *retx, int *rety)

//	AdjustCtx
//
//	Adjusts the context to account for any offsets

	{
	if (m_pCreator->HasOffsets())
		{
		NewCtx = Ctx;

		int xOffset;
		int yOffset;
		m_pCreator->ApplyOffsets(&NewCtx, &xOffset, &yOffset);

		*retx += xOffset;
		*rety += yOffset;

		return &NewCtx;
		}
	else
		return &Ctx;
	}

SViewportPaintCtx *CEffectGroupPainter::AdjustCtx (SViewportPaintCtx &Ctx, SViewportPaintCtx &NewCtx, CVector &vHead, CVector &vTail)

//	AdjustCtx
//
//	Adjusts the context to account for any offsets

	{
	if (m_pCreator->HasOffsets())
		{
		NewCtx = Ctx;

		int xOffset;
		int yOffset;
		m_pCreator->ApplyOffsets(&NewCtx, &xOffset, &yOffset);

		CVector vOffset = CVector(xOffset, -yOffset) * g_KlicksPerPixel;
		vHead = vHead + vOffset;
		vTail = vTail + vOffset;

		return &NewCtx;
		}
	else
		return &Ctx;
	}

bool CEffectGroupPainter::CanPaintComposite (void)

//	CanPaintComposite
//
//	Returns TRUE if we can paint composite.

	{
	for (int i = 0; i < m_Painters.GetCount(); i++)
		if (m_Painters[i])
			if (!m_Painters[i]->CanPaintComposite())
				return false;

	//	If we get this far, all painters can paint composite

	return true;
	}

int CEffectGroupPainter::GetFadeLifetime (bool bHit) const

//	GetFadeLifetime
//
//	Returns the lifetime to paint fading state

	{
	int iMaxLifetime = 0;

	for (int i = 0; i < m_Painters.GetCount(); i++)
		if (m_Painters[i])
			iMaxLifetime = Max(iMaxLifetime, m_Painters[i]->GetFadeLifetime(bHit));

	return iMaxLifetime;
	}

int CEffectGroupPainter::GetLifetime (void)

//	GetLifetime
//
//	Returns the lifetime of the painter

	{
	int iMaxLifetime = 0;

	for (int i = 0; i < m_Painters.GetCount(); i++)
		if (m_Painters[i])
			iMaxLifetime = Max(iMaxLifetime, m_Painters[i]->GetLifetime());

	return iMaxLifetime;
	}

void CEffectGroupPainter::GetRect (RECT *retRect) const

//	GetRect
//
//	Return the bounding rect

	{
	retRect->left = 0;
	retRect->top = 0;
	retRect->bottom = 0;
	retRect->right = 0;

	for (int i = 0; i < m_Painters.GetCount(); i++)
		if (m_Painters[i])
			{
			RECT rcRect;
			m_Painters[i]->GetRect(&rcRect);

			::UnionRect(retRect, retRect, &rcRect);
			}
	}

void CEffectGroupPainter::OnBeginFade (void)

//	OnBeginFade

	{
	for (int i = 0; i < m_Painters.GetCount(); i++)
		if (m_Painters[i])
			m_Painters[i]->OnBeginFade();
	}

void CEffectGroupPainter::OnMove (SEffectMoveCtx &Ctx, bool *retbBoundsChanged)

//	OnMove

	{
	bool bBoundsChanged = false;

	for (int i = 0; i < m_Painters.GetCount(); i++)
		if (m_Painters[i])
			{
			bool bSingleBoundsChanged;

			m_Painters[i]->OnMove(Ctx, &bSingleBoundsChanged);

			if (bSingleBoundsChanged)
				bBoundsChanged = true;
			}

	if (retbBoundsChanged)
		*retbBoundsChanged = bBoundsChanged;
	}

void CEffectGroupPainter::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream

	{
	int i;
	DWORD dwLoad;

	int iCount;
	if (Ctx.dwVersion >= 93)
		{
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
		iCount = dwLoad;
		}
	else
		iCount = m_Painters.GetCount();

	//	Read all the painters

	for (i = 0; i < iCount; i++)
		{
		//	The UNID is ignored (because it is the UNID of the creator)

		ReadUNID(Ctx);

		//	Validate the class

		CString sExpected = (i < m_Painters.GetCount() ? m_Painters[i]->GetCreator()->GetTag() : NULL_STR);
		if (IEffectPainter::ValidateClass(Ctx, sExpected) != NOERROR)
			{
			if (i < m_Painters.GetCount())
				m_Painters[i].Delete();

			continue;
			}

		//	Read the painter

		m_Painters[i]->ReadFromStream(Ctx);
		}

	//	If we have extra painters, then delete them

	for (i = iCount; i < m_Painters.GetCount(); i++)
		m_Painters[i].Delete();
	}

void CEffectGroupPainter::OnUpdate (SEffectUpdateCtx &Ctx)

//	OnUpdate
//
//	Update painters

	{
	for (int i = 0; i < m_Painters.GetCount(); i++)
		if (IsAlive(m_Painters[i], Ctx.iTick))
			{
			//	If we have offsets, we need to modify the emit position

			if (m_pCreator->HasOffsets())
				{
				SEffectUpdateCtx AdjustCtx = Ctx;
				AdjustCtx.vEmitPos = Ctx.vEmitPos + m_pCreator->GetOffsetPos(Ctx.iRotation);
				AdjustCtx.iRotation = AngleMod(Ctx.iRotation + m_pCreator->GetRotationAdj());

				m_Painters[i]->OnUpdate(AdjustCtx);
				}
			else
				m_Painters[i]->OnUpdate(Ctx);
			}
	}

void CEffectGroupPainter::OnWriteToStream (IWriteStream *pStream)

//	OnWriteToStream

	{
	DWORD dwSave = m_Painters.GetCount();
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	for (int i = 0; i < m_Painters.GetCount(); i++)
		CEffectCreator::WritePainterToStream(pStream, m_Painters[i]);
	}

void CEffectGroupPainter::Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	Paint
//
//	Paint

	{
	SViewportPaintCtx NewCtx;
	SViewportPaintCtx *pCtx = AdjustCtx(Ctx, NewCtx, &x, &y);

	for (int i = 0; i < m_Painters.GetCount(); i++)
		if (IsAlive(m_Painters[i], Ctx.iTick))
			m_Painters[i]->Paint(Dest, x, y, *pCtx);
	}

void CEffectGroupPainter::PaintComposite (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	PaintComposite
//
//	Paint the effect

	{
	SViewportPaintCtx NewCtx;
	SViewportPaintCtx *pCtx = AdjustCtx(Ctx, NewCtx, &x, &y);

	for (int i = 0; i < m_Painters.GetCount(); i++)
		if (m_Painters[i])
			m_Painters[i]->PaintComposite(Dest, x, y, *pCtx);
	}

void CEffectGroupPainter::PaintFade (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	PaintFade
//
//	Paint

	{
	SViewportPaintCtx NewCtx;
	SViewportPaintCtx *pCtx = AdjustCtx(Ctx, NewCtx, &x, &y);

	for (int i = 0; i < m_Painters.GetCount(); i++)
		if (m_Painters[i])
			m_Painters[i]->PaintFade(Dest, x, y, *pCtx);
	}

void CEffectGroupPainter::PaintHit (CG32bitImage &Dest, int x, int y, const CVector &vHitPos, SViewportPaintCtx &Ctx)

//	PaintHit
//
//	Paints a hit

	{
	SViewportPaintCtx NewCtx;
	SViewportPaintCtx *pCtx = AdjustCtx(Ctx, NewCtx, &x, &y);

	for (int i = 0; i < m_Painters.GetCount(); i++)
		if (m_Painters[i])
			m_Painters[i]->PaintHit(Dest, x, y, vHitPos, *pCtx);
	}

void CEffectGroupPainter::PaintLine (CG32bitImage &Dest, const CVector &vHead, const CVector &vTail, SViewportPaintCtx &Ctx)

//	PaintLine
//
//	Paints a continuous line

	{
	SViewportPaintCtx NewCtx;
	CVector vNewHead = vHead;
	CVector vNewTail = vTail;
	SViewportPaintCtx *pCtx = AdjustCtx(Ctx, NewCtx, vNewHead, vNewTail);

	for (int i = 0; i < m_Painters.GetCount(); i++)
		if (m_Painters[i])
			m_Painters[i]->PaintLine(Dest, vNewHead, vNewTail, *pCtx);
	}

bool CEffectGroupPainter::PointInImage (int x, int y, int iTick, int iVariant, int iRotation) const

//	PointInImage
//
//	Returns TRUE if the point is in the image

	{
	for (int i = 0; i < m_Painters.GetCount(); i++)
		if (m_Painters[i])
			if (m_Painters[i]->PointInImage(x, y, iTick, iVariant, iRotation))
				return true;

	return false;
	}

void CEffectGroupPainter::SetParamStruct (CCreatePainterCtx &Ctx, const CString &sParam, ICCItem *pValue)

//	SetParamStruct
//
//	Sets special parameters

	{
	if (strEquals(sParam, EFFECTS_ATTRIB))
		{
		//	This allows us to programmatically create sub painters.
		//	(But only if we don't have any.)

		if (m_Painters.GetCount() != 0)
			return;

		//	LATER: Create a set of painters from the structures.
		//	NOTE: We will need to save and load painters by tag (without
		//	creators).
		}
	}

void CEffectGroupPainter::SetVariants (int iVariants)

//	SetVariants
//
//	Sets the variants

	{
	for (int i = 0; i < m_Painters.GetCount(); i++)
		if (m_Painters[i])
			m_Painters[i]->SetVariants(iVariants);
	}

//	CEffectGroupCreator --------------------------------------------------------

CEffectGroupCreator::CEffectGroupCreator (void) : 
		m_pCreators(NULL), 
		m_iCount(0),
		m_iAngleOffset(0),
		m_iRadiusOffset(0),
		m_xOffset(0),
		m_yOffset(0),
		m_iRotationAdj(0),
		m_bHasOffsets(false)

//	CEffectGroupCreator constructor

	{
	}

CEffectGroupCreator::~CEffectGroupCreator (void)

//	CEffectGroupCreator destructor

	{
	if (m_pCreators)
		delete [] m_pCreators;
	}

void CEffectGroupCreator::ApplyOffsets (SViewportPaintCtx *ioCtx, int *retx, int *rety)

//	ApplyOffsets
//
//	Adds appropriate offsets to Ctx

	{
	int xOffset = (int)(m_xOffset * ioCtx->rOffsetScale);
	int yOffset = (int)(m_yOffset * ioCtx->rOffsetScale);
	if (m_iAngleOffset || m_iRadiusOffset)
		{
		CVector vVec = PolarToVector(360 + ioCtx->iRotation + m_iAngleOffset, (Metric)m_iRadiusOffset * ioCtx->rOffsetScale);
		xOffset += (int)vVec.GetX();
		yOffset += (int)vVec.GetY();
		}

	//	Adjust the Xform
	//
	//	NOTE: We alter the relative Xform, not the main one because sometimes
	//	effects still need to know the global Xform.

	ioCtx->XFormRel.Offset(xOffset, -yOffset);

	//	Adjust the rotation

	ioCtx->iRotation = AngleMod(ioCtx->iRotation + m_iRotationAdj);

	//	Done

	if (retx)
		*retx = xOffset;

	if (rety)
		*rety = -yOffset;
	}

ALERROR CEffectGroupCreator::CreateEffect (CSystem *pSystem,
										   CSpaceObject *pAnchor,
										   const CVector &vPos,
										   const CVector &vVel,
										   int iRotation,
										   int iVariant,
										   CSpaceObject **retpEffect)

//	CreateEffect
//
//	Creates an effect object

	{
	ALERROR error;
	CSpaceObject *pLastEffect = NULL;

	//	Create the sub effects

	for (int i = 0; i < m_iCount; i++)
		{
		if (error = m_pCreators[i]->CreateEffect(pSystem, pAnchor, vPos, vVel, iRotation, iVariant, &pLastEffect))
			return error;
		}

	//	Play sound associated with the group (if any)
	//	(We use the effect object only to get a position).

	if (pLastEffect)
		CEffectCreator::PlaySound(pLastEffect);

	//	Done

	if (retpEffect)
		*retpEffect = pLastEffect;

	return NOERROR;
	}

IEffectPainter *CEffectGroupCreator::OnCreatePainter (CCreatePainterCtx &Ctx)

//	CreatePainter
//
//	Creates a painter

	{
	IEffectPainter *pPainter = new CEffectGroupPainter(this, Ctx);

	//	Initialize via GetParameters, if necessary

	InitPainterParameters(Ctx, pPainter);

	//	Done

	return pPainter;
	}

int CEffectGroupCreator::GetLifetime (void)

//	GetLifetime
//
//	Returns the lifetime of the effect
	
	{
	int iTotalLifetime = 0;
	for (int i = 0; i < m_iCount; i++)
		{
		int iLifetime = m_pCreators[i]->GetLifetime();
		if (iLifetime == -1)
			{
			iTotalLifetime = -1;
			break;
			}
		else if (iLifetime > iTotalLifetime)
			iTotalLifetime = iLifetime;
		}

	return iTotalLifetime;
	}

CVector CEffectGroupCreator::GetOffsetPos (int iRotation)

//	GetOffsetPos
//
//	Returns the offset as a vector (in game coordinates)

	{
	CVector vOffset((Metric)m_xOffset, (Metric)m_yOffset);
	if (m_iAngleOffset || m_iRadiusOffset)
		vOffset = vOffset + PolarToVector(360 + iRotation + m_iAngleOffset, (Metric)m_iRadiusOffset);

	return vOffset * g_KlicksPerPixel;
	}

ALERROR CEffectGroupCreator::OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID)

//	OnEffectCreateFromXML
//
//	Creates from XML

	{
	ALERROR error;
	int i;

	ASSERT(m_pCreators == NULL);

	//	It's OK if we have a NULL pDesc. It just means that we are going to create
	//	the group dynamically.

	if (pDesc == NULL)
		return NOERROR;

	//	If we have the special tag <Effects> then we look there. We need this 
	//	in case we want to also have an <Events> tag.

	CXMLElement *pEffectList = pDesc->GetContentElementByTag(EFFECTS_TAG);
	if (pEffectList == NULL)
		pEffectList = pDesc;

	//	Allocate the creator array

	m_iCount = pEffectList->GetContentElementCount();
	if (m_iCount > 0)
		{
		m_pCreators = new CEffectCreatorRef [m_iCount];

		for (i = 0; i < m_iCount; i++)
			{
			CString sSubUNID = strPatternSubst(CONSTLIT("%s/%d"), sUNID, i);

			if (error = m_pCreators[i].LoadSimpleEffect(Ctx, sSubUNID, pEffectList->GetContentElement(i)))
				return error;
			}
		}

	//	Load any offsets or transforms

	m_iAngleOffset = pDesc->GetAttributeIntegerBounded(ANGLE_OFFSET_ATTRIB, -360, 360, 0);
	m_iRadiusOffset = pDesc->GetAttributeInteger(RADIUS_OFFSET_ATTRIB);
	m_xOffset = pDesc->GetAttributeInteger(X_OFFSET_ATTRIB);
	m_yOffset = pDesc->GetAttributeInteger(Y_OFFSET_ATTRIB);
	m_iRotationAdj = pDesc->GetAttributeIntegerBounded(ROTATION_ADJ_ATTRIB, -360, 360, 0);

	m_bHasOffsets = (m_iAngleOffset != 0 || m_iRadiusOffset != 0
			|| m_xOffset != 0 || m_yOffset != 0 || m_iRotationAdj != 0);

	return NOERROR;
	}

ALERROR CEffectGroupCreator::OnEffectBindDesign (SDesignLoadCtx &Ctx)

//	OnEffectBindDesign
//
//	Resolve loading

	{
	ALERROR error;

	for (int i = 0; i < m_iCount; i++)
		if (error = m_pCreators[i].Bind(Ctx))
			return error;

	return NOERROR;
	}

void CEffectGroupCreator::OnEffectPlaySound (CSpaceObject *pSource)

//	OnEffectPlaySound
//
//	Play sounds

	{
	//	Let subclass play sounds associated with the group

	CEffectCreator::OnEffectPlaySound(pSource);

	//	Play additional sounds

	for (int i = 0; i < m_iCount; i++)
		m_pCreators[i]->PlaySound(pSource);
	}

void CEffectGroupCreator::OnEffectMarkResources (void)

//	OnEffectMarkResources
//
//	Mark images used by this effect

	{
	for (int i = 0; i < m_iCount; i++)
		m_pCreators[i]->MarkImages();
	}

void CEffectGroupCreator::SetLifetime (int iLifetime)

//	SetLifetime
//
//	Sets the lifetime

	{
	for (int i = 0; i < m_iCount; i++)
		m_pCreators[i]->SetLifetime(iLifetime);
	}

void CEffectGroupCreator::SetVariants (int iVariants)

//	SetVariants
//
//	Sets the variants

	{
	for (int i = 0; i < m_iCount; i++)
		m_pCreators[i]->SetVariants(iVariants);
	}

bool CEffectGroupPainter::UsesOrigin (void) const

//	UsesOrigin
//
//	Returns TRUE if any of our children need an origin.

	{
	for (int i = 0; i < m_Painters.GetCount(); i++)
		if (m_Painters[i])
			if (m_Painters[i]->UsesOrigin())
				return true;

	//	If we get this far, none of the painters need it.

	return false;
	}

