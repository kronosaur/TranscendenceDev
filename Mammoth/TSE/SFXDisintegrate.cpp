//	SFXDisintegrate.cpp
//
//	CDisintegrateEffectCreator class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define PRIMARY_COLOR_ATTRIB			CONSTLIT("primaryColor")
#define SECONDARY_COLOR_ATTRIB			CONSTLIT("secondaryColor")
#define STYLE_ATTRIB					CONSTLIT("style")

class CDisintegrateEffectPainter : public IEffectPainter
	{
	public:
		CDisintegrateEffectPainter (CCreatePainterCtx &Ctx, CEffectCreator *pCreator);
		~CDisintegrateEffectPainter (void);

		//	IEffectPainter virtuals

		virtual CEffectCreator *GetCreator (void) override { return m_pCreator; }
		virtual int GetLifetime (void) override;
		virtual void GetParam (const CString &sParam, CEffectParamDesc *retValue) override;
		virtual bool GetParamList (TArray<CString> *retList) const override;
		virtual void GetRect (RECT *retRect) const override;
		virtual void OnUpdate (SEffectUpdateCtx &Ctx) override;
		virtual void Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual bool PointInImage (int x, int y, int iTick, int iVariant = 0, int iRotation = 0) const;

	protected:
		virtual void OnSetParam (CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value) override;

	private:
		static constexpr int LIFETIME = 180;
		static constexpr int DISPERSE_POINT = LIFETIME / 2;
		static constexpr int FIXED_POINT = 256;
		static constexpr int PARTICLE_COUNT = 5000;

		enum EStyles
			{
			styleUnknown =			0,

			stylePlain =			1,

			styleMax =				1,
			};

		struct SParticle
			{
			int x;							//	Position in 256th of a pixel
			int y;
			int xV;							//	Velocity in 256th of a pixel
			int yV;							//	per tick
			int iTicksLeft;
			CG32bitPixel rgbColor;
			bool bInMask;					//	TRUE if we're inside the mask
			};

		void InitParticle (SParticle &Particle) const;

		CEffectCreator *m_pCreator = NULL;

		CObjectImageArray m_MaskImage;
		int m_iMaskTick = 0;
		int m_iMaskRotation = 0;

		TArray<SParticle> m_Particles;
	};

static LPSTR STYLE_TABLE[] =
	{
	//	Must be same order as EStyles
		"",

		"plain",

		NULL,
	};

CDisintegrateEffectCreator::CDisintegrateEffectCreator (void) :
			m_pSingleton(NULL)

//	CDisintegrateEffectCreator constructor

	{
	}

CDisintegrateEffectCreator::~CDisintegrateEffectCreator (void)

//	CDisintegrateEffectCreator destructor

	{
	if (m_pSingleton)
		delete m_pSingleton;
	}

IEffectPainter *CDisintegrateEffectCreator::OnCreatePainter (CCreatePainterCtx &Ctx)

//	CreatePainter
//
//	Creates a new painter
	
	{
	//	If we have a singleton, return that

	if (m_pSingleton)
		return m_pSingleton;

	//	Otherwise we need to create a painter with the actual
	//	parameters.

	IEffectPainter *pPainter = new CDisintegrateEffectPainter(Ctx, this);

	//	Initialize the painter parameters


	//	Initialize via GetParameters, if necessary

	InitPainterParameters(Ctx, pPainter);

	//	If we are a singleton, then we only need to create this once.

	if (GetInstance() == instGame)
		{
		pPainter->SetSingleton(true);
		m_pSingleton = pPainter;
		}

	return pPainter;
	}

ALERROR CDisintegrateEffectCreator::OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID)

//	OnEffectCreateFromXML
//
//	Initialize type-specific data

	{
	ALERROR error;

	if (error = m_PrimaryColor.InitColorFromXML(Ctx, pDesc->GetAttribute(PRIMARY_COLOR_ATTRIB)))
		return error;

	if (error = m_SecondaryColor.InitColorFromXML(Ctx, pDesc->GetAttribute(SECONDARY_COLOR_ATTRIB)))
		return error;

	if (error = m_Style.InitIdentifierFromXML(Ctx, pDesc->GetAttribute(STYLE_ATTRIB), STYLE_TABLE))
		return error;

	return NOERROR;
	}

ALERROR CDisintegrateEffectCreator::OnEffectBindDesign (SDesignLoadCtx &Ctx)

//	OnEffectBindDesign
//
//	Resolve loading

	{
	//	Clean up, because we might want to recompute for next time.

	if (m_pSingleton)
		{
		delete m_pSingleton;
		m_pSingleton = NULL;
		}

	return NOERROR;
	}

//	CDisintegrateEffectPainter -------------------------------------------------


CDisintegrateEffectPainter::CDisintegrateEffectPainter (CCreatePainterCtx &Ctx, CEffectCreator *pCreator)

//	CDisintegrateEffectPainter constructor

	{
	int i;

	m_pCreator = pCreator;

	//	Get the mask to use

	CSpaceObject *pAnchor = Ctx.GetAnchor();
	if (pAnchor)
		{
		m_MaskImage = pAnchor->GetImage();
		CShip *pShip = pAnchor->AsShip();
		if (pShip)
			{
			m_iMaskTick = 0;
			m_iMaskRotation = pShip->GetRotationState().GetFrameIndex();
			}
		}

	//	Initialize particles

	m_Particles.InsertEmpty(PARTICLE_COUNT);
	for (i = 0; i < m_Particles.GetCount(); i++)
		InitParticle(m_Particles[i]);
	}

CDisintegrateEffectPainter::~CDisintegrateEffectPainter (void)

//	CDisintegrateEffectPainter destructor

	{
	}

//	IEffectPainter virtuals

int CDisintegrateEffectPainter::GetLifetime (void)

//	GetLifetime
//
//	Returns the lifetime

	{
	return LIFETIME;
	}

void CDisintegrateEffectPainter::GetParam (const CString &sParam, CEffectParamDesc *retValue)

//	GetParam
//
//	Returns the current param value

	{
	}

bool CDisintegrateEffectPainter::GetParamList (TArray<CString> *retList) const

//	GetParamList
//
//	Returns the list of parameters

	{
	return true;
	}

void CDisintegrateEffectPainter::GetRect (RECT *retRect) const

//	GetRect
//
//	Returns the current rect

	{
	*retRect = m_MaskImage.GetImageRect();
	}

void CDisintegrateEffectPainter::InitParticle (SParticle &Particle) const

//	InitParticle
//
//	Initializes a particle with a random velocity.

	{
	Particle.iTicksLeft = mathRandom(10, 30);
	Particle.x = 0;
	Particle.y = 0;
	IntPolarToVector(mathRandom(0, 359),
			(Metric)(mathRandom(0, (FIXED_POINT * 2))),
			&Particle.xV,
			&Particle.yV);

	int iFade = mathRandom(25, 100);
	Particle.rgbColor = CG32bitPixel(
			(iFade * 0x80 / 100),
			(iFade * 0xff / 100),
			(iFade * 0xff / 100));

	Particle.bInMask = true;
	}

void CDisintegrateEffectPainter::OnSetParam (CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value)

//	OnSetParam
//
//	Set a parameter

	{
	}

void CDisintegrateEffectPainter::OnUpdate (SEffectUpdateCtx &Ctx)

//	OnUpdate
//
//	Update

	{
	int i;

	//	Update the particles

	for (i = 0; i < m_Particles.GetCount(); i++)
		{
		SParticle &Particle = m_Particles[i];
		if (Particle.iTicksLeft > 0)
			{
			Particle.iTicksLeft--;
			Particle.x += Particle.xV;
			Particle.y += Particle.yV;

			if (Ctx.iTick <= DISPERSE_POINT)
				{
				Particle.bInMask = m_MaskImage.PointInImage(Particle.x / FIXED_POINT, Particle.y / FIXED_POINT, m_iMaskTick, m_iMaskRotation);

				if (!Particle.bInMask && Ctx.iTick == DISPERSE_POINT)
					InitParticle(Particle);
				}
			else
				Particle.bInMask = true;
			}
		else if (Ctx.iTick < DISPERSE_POINT)
			InitParticle(Particle);
		}
	}

void CDisintegrateEffectPainter::Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	Paint
//
//	Paint

	{
	int i;

	//	Paint the flashing silhouette

	if ((Ctx.iTick < (DISPERSE_POINT / 2))
			&& (Ctx.iTick % 2))
		{
		int iFade = 100 - ((Ctx.iTick * 100) / (DISPERSE_POINT / 2));
		CG32bitPixel rgbColor = CG32bitPixel(
				iFade * 0x80 / 100,
				iFade * 0xff / 100,
				iFade * 0xff / 100);
		m_MaskImage.PaintSilhoutte(Dest, x, y, m_iMaskTick, m_iMaskRotation, rgbColor);
		}

	//	Paint the noise

	if (Ctx.iTick < DISPERSE_POINT)
		{
		for (i = 0; i < m_Particles.GetCount(); i++)
			{
			const SParticle &Particle = m_Particles[i];
			if (Particle.iTicksLeft > 0
					&& Particle.bInMask)
				Dest.SetPixel(x + (Particle.x / FIXED_POINT),
						y + (Particle.y / FIXED_POINT),
						Particle.rgbColor);
			}
		}
	else if (Ctx.iTick == DISPERSE_POINT)
		{
		for (i = 0; i < m_Particles.GetCount(); i++)
			{
			const SParticle &Particle = m_Particles[i];
			if (Particle.iTicksLeft > 0
					&& Particle.bInMask)
				Dest.SetPixel(x + (Particle.x / FIXED_POINT),
						y + (Particle.y / FIXED_POINT),
						Particle.rgbColor);
			}
		}
	else
		{
		for (i = 0; i < m_Particles.GetCount(); i++)
			{
			const SParticle &Particle = m_Particles[i];
			if (Particle.iTicksLeft > 0)
				Dest.SetPixel(x + (Particle.x / FIXED_POINT),
						y + (Particle.y / FIXED_POINT),
						Particle.rgbColor);
			}
		}
	}

bool CDisintegrateEffectPainter::PointInImage (int x, int y, int iTick, int iVariant, int iRotation) const

//	PointInImage
//
//	Returns TRUE if the given point is in the image

	{
	return m_MaskImage.PointInImage(x, y, m_iMaskTick, m_iMaskRotation);
	}
