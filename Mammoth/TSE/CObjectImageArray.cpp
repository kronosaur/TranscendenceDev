//	CObjectImageArray.cpp
//
//	CObjectImageArray

#include "PreComp.h"

#define ANIMATION_COLUMNS_ATTRIB		CONSTLIT("animationColumns")
#define BLENDING_ATTRIB					CONSTLIT("blend")
#define FLASH_TICKS_ATTRIB				CONSTLIT("flashTicks")
#define IMAGE_WIDTH_ATTRIB				CONSTLIT("imageWidth")
#define IMAGE_HEIGHT_ATTRIB				CONSTLIT("imageHeight")
#define ROTATION_COLUMNS_ATTRIB			CONSTLIT("rotationColumns")
#define ROTATION_COUNT_ATTRIB			CONSTLIT("rotationCount")
#define ROTATE_OFFSET_ATTRIB			CONSTLIT("rotationOffset")
#define VIEWPORT_RATIO_ATTRIB			CONSTLIT("viewportRatio")
#define VIEWPORT_SIZE_ATTRIB			CONSTLIT("viewportSize")
#define X_OFFSET_ATTRIB					CONSTLIT("xOffset")
#define Y_OFFSET_ATTRIB					CONSTLIT("yOffset")

#define LIGHTEN_BLENDING				CONSTLIT("brighten")

#define	GLOW_SIZE						4
#define FILTER_SIZE						5
#define FIXED_POINT						65536
#define MT_REQUIRED_PIXEL_COUNT_SHIFT	12
#define MT_REQUIRED_PIXELS				1 << MT_REQUIRED_PIXEL_COUNT_SHIFT

static int g_FilterOffset[FILTER_SIZE] =
	{
	-GLOW_SIZE,
	-(GLOW_SIZE / 2),
	0,
	+(GLOW_SIZE / 2),
	+GLOW_SIZE
	};

static int g_Filter[FILTER_SIZE][FILTER_SIZE] =
	{
		{	   57,   454,   907,   454,    57	},
		{	  454,  3628,  7257,  3628,   454	},
		{	  907,  7257, 14513,  7257,   907	},
		{	  454,  3628,  7257,  3628,   454	},
		{	   57,   454,   907,   454,    57	},
	};

static char g_ImageIDAttrib[] = "imageID";
static char g_ImageXAttrib[] = "imageX";
static char g_ImageYAttrib[] = "imageY";
static char g_ImageFrameCountAttrib[] = "imageFrameCount";
static char g_ImageTicksPerFrameAttrib[] = "imageTicksPerFrame";

// CSpritePaintWorker ----------------------------------------------------------------------------------

class CSpritePaintWorker : public IThreadPoolTask
	{
	public:
		enum ePaintMode
			{
			eDebugTask,					//	This task does not actually paint anything, it just runs the collatz conjecture
			ePaintImage,
			
			ePaintModeCount
			};
		struct SCtx
			{
			//	Call Ctx
			int iMode = ePaintMode::ePaintImage;

			//	Dest Ctx
			CG32bitImage *pDest = NULL;
			int xDest = 0;
			int yDest = 0;

			//	Src Ctx
			const CObjectImageArray *pSrc = NULL;
			int xSrc = 0;
			int ySrc = 0;
			int iTick = 0;
			int iRotation = 0;

			//	Flags
			DWORD fComposite : 1 = 0;
			DWORD fDbgShowPaintLocation : 1 = 0;
			DWORD dwSpare : 30 = 0;

			};

		CSpritePaintWorker (int yOffset, int cyHeight, const SCtx &Ctx):
			m_y(yOffset),
			m_cyHeight(cyHeight),
			m_Ctx(Ctx)
			{ }

		virtual void Run (void) override
			{
			switch (m_Ctx.iMode)
				{
				case (ePaintImage):
					{
					RunPaintImage();
					break;
					}
				case (eDebugTask):
					{
					RunDebugTask();
					break;
					}
				default:
					ASSERT(false);
				}
			}

	private:

		//	Task code

		void RunPaintImage (void)
			{
			if (m_Ctx.pSrc)
				m_Ctx.pSrc->WorkerPaintImage(
					*m_Ctx.pDest,
					m_Ctx.xDest,
					m_Ctx.yDest,
					m_Ctx.iTick,
					m_Ctx.iRotation,
					m_Ctx.fComposite,
					m_y,
					m_cyHeight,
					m_Ctx.fDbgShowPaintLocation
				);
			}

		//	Debug task code

		int DebugTask (int i)
			{
			//	Give the threads some busy work (Collatz conjecture)
			int count = 0;
			while (i > 1)
				{
				if (i > 3000)
					i = 3000;
				if (i & 0x00000001)
					i = i * 3 + 1;
				else
					i = i / 2;
				count++;
				}
			return count;
			}

		void RunDebugTask (void)
			{
			DebugTask(m_Ctx.xDest + m_Ctx.yDest + m_Ctx.iTick + m_Ctx.iRotation + m_Ctx.fComposite + m_y + m_cyHeight);
			}

		const SCtx m_Ctx;
		const int m_y;
		const int m_cyHeight;
	};

// CObjectImageArray ---------------------------------------------------------------------------------

CG32bitImage CObjectImageArray::m_NullImage;
CObjectImageArray CObjectImageArray::m_Null;

CObjectImageArray::CObjectImageArray (const CObjectImageArray &Source)

//	CObjectImageArray copy constructor

	{
	CopyFrom(Source);
	}

CObjectImageArray::~CObjectImageArray (void)

//	CObjectImageArray destructor

	{
	CleanUp();
	}

CObjectImageArray &CObjectImageArray::operator= (const CObjectImageArray &Source)

//	Operator =

	{
	CleanUp();
	CopyFrom(Source);
	return *this;
	}

bool CObjectImageArray::CalcVolumetricShadowLine (SLightingCtx &Ctx, int iTick, int iRotation, int *retxCenter, int *retyCenter, int *retiWidth, int *retiLength) const

//	CalcVolumetricShadowLine
//
//	Computes the shadow line for this image.

	{
	int i;

	if (m_pImage == NULL)
		return false;

	//	First see if we have a dedicated shadow mask. If so, we use that.

	CG32bitImage *pSource = m_pImage->GetShadowMask();

	//	If we don't then use the normal image

	if (pSource == NULL)
		pSource = m_pImage->GetRawImage(NULL_STR);

	//	If we still can't find an image, then no shadow

	if (pSource == NULL)
		return false;

	//	Compute the position of the frame

	int cxWidth = RectWidth(m_rcImage);
	int cyHeight = RectHeight(m_rcImage);
	int xSrc;
	int ySrc;
	ComputeSourceXY(iTick, iRotation, &xSrc, &ySrc);

	//	Compute the center point of the object in image coordinates

	int xObjCenter = xSrc + (cxWidth / 2);
	int yObjCenter = ySrc + (cyHeight / 2);

	//	Adjust for rotation

	if (m_pRotationOffset)
		{
		xObjCenter -= m_pRotationOffset[iRotation % m_iRotationCount].x;
		yObjCenter += m_pRotationOffset[iRotation % m_iRotationCount].y;
		}

	//	Compute the upper-left corner of the image rect in lighting coordinates

	CVector vUL(xSrc, ySrc);
	CVector vObjCenter(xObjCenter, yObjCenter);

	CVector vULRelativeToObjCenter = vUL - vObjCenter;
	CVector vLightingRow = CVector(vULRelativeToObjCenter.Dot(Ctx.vSw), vULRelativeToObjCenter.Dot(Ctx.vSl));

	//	Keep track of each line perpendicular to the lighting axis

	struct SLine
		{
		SLine (void) :
				rMin(0.0),
				rMax(0.0)
			{ }

		CVector vMin;
		Metric rMin;

		CVector vMax;
		Metric rMax;
		};

	TSortMap<int, SLine> Slices;
	
	//	Loop over all the pixels in the image

	int yRow = ySrc;
	CG32bitPixel *pRow = pSource->GetPixelPos(xSrc, ySrc);
	CG32bitPixel *pRowEnd = pSource->GetPixelPos(xSrc, ySrc + cyHeight);
	while (pRow < pRowEnd)
		{
		int xPos = xSrc;
		CG32bitPixel *pPos = pRow;
		CG32bitPixel *pPosEnd = pRow + cxWidth;
		CVector vLightingPos = vLightingRow;

		while (pPos < pPosEnd)
			{
			if (pPos->GetAlpha() >= 0x80)
				{
				SLine *pSlice = Slices.SetAt((int)vLightingPos.GetY());

				Metric rDist = vLightingPos.GetX();
				if (rDist > pSlice->rMax)
					{
					pSlice->vMax = CVector(xPos, yRow);
					pSlice->rMax = rDist;
					}
				else if (rDist < pSlice->rMin)
					{
					pSlice->vMin = CVector(xPos, yRow);
					pSlice->rMin = rDist;
					}
				}

			xPos++;
			pPos++;
			vLightingPos = vLightingPos + Ctx.vIncX;
			}

		yRow++;
		pRow = pSource->NextRow(pRow);
		vLightingRow = vLightingRow + Ctx.vIncY;
		}

	//	Find the widest slice

	Metric rBestSize = 0.0;
	SLine *pBestSlice = NULL;
	for (i = 0; i < Slices.GetCount(); i++)
		{
		SLine *pSlice = &Slices[i];
		if (pBestSlice == NULL || (pSlice->rMax - pSlice->rMin) > rBestSize)
			{
			pBestSlice = pSlice;
			rBestSize = (pSlice->rMax - pSlice->rMin);
			}
		}

	//	Make sure we succeed.

	if (pBestSlice == NULL)
		return false;

	//	Compute the center point of the winning line (relative to the object center)

	CVector vLineCenter = ((pBestSlice->vMax + pBestSlice->vMin) / 2) - vObjCenter;

	//	Return data

	*retxCenter = (int)vLineCenter.GetX();
	*retyCenter = (int)vLineCenter.GetY();
	*retiWidth = (int)(pBestSlice->vMax - pBestSlice->vMin).Length();
	*retiLength = (*retiWidth) * 8;

	//	Done

	return true;
	}

void CObjectImageArray::CleanUp (void)

//	CleanUp
//
//	Cleans up the image to free resources

	{
	if (m_pRotationOffset)
		{
		delete [] m_pRotationOffset;
		m_pRotationOffset = NULL;
		}

	if (m_pGlowImages)
		{
		delete [] m_pGlowImages;
		m_pGlowImages = NULL;
		}

	if (m_pScaledImages)
		{
		delete [] m_pScaledImages;
		m_pScaledImages = NULL;
		m_cxScaledImage = -1;
		}

	m_pImage = NULL;
	}

void CObjectImageArray::CalcRequiredImageSize (int &cxRequired, int &cyRequired) const

//	CalcRequiredImageSize
//
//	Returns the required width and height depending on our frame configuration.

	{
	//	If we're animating...

	if (m_iFrameCount > 0 && m_iTicksPerFrame > 0)
		{
		//	If we've got multi-row animations, then we deal with that.

		if (m_iFramesPerRow != m_iFrameCount)
			{
			//	HACK: m_iFramesPerRow is really "frames per column"

			int iCols = m_iFrameCount / m_iFramesPerRow;

			cxRequired = m_rcImage.left + (iCols * RectWidth(m_rcImage));
			cyRequired = m_rcImage.top + (m_iFramesPerRow * RectHeight(m_rcImage));
			}

		//	If we've got multi-column rotations, then we need to deal with that.

		else if (m_iRotationCount != m_iFramesPerColumn)
			{
			int iCols = (m_iRotationCount + m_iFramesPerColumn - 1) / m_iFramesPerColumn;

			cxRequired = m_rcImage.left + (m_iFrameCount * iCols * RectWidth(m_rcImage));
			cyRequired = m_rcImage.top + (m_iFramesPerColumn * RectHeight(m_rcImage));
			}

		//	Otherwise, we expect a single column per frame

		else
			{
			cxRequired = m_rcImage.left + (m_iFrameCount * RectWidth(m_rcImage));
			cyRequired = m_rcImage.top + (m_iRotationCount * RectHeight(m_rcImage));
			}
		}

	//	If we've got multi-column rotations but no animations, then we just
	//	compute the proper column

	else if (m_iRotationCount != m_iFramesPerColumn)
		{
		int iCols = (m_iRotationCount + m_iFramesPerColumn - 1) / m_iFramesPerColumn;

		cxRequired = m_rcImage.left + (iCols * RectHeight(m_rcImage));
		cyRequired = m_rcImage.top + (m_iFramesPerColumn * RectWidth(m_rcImage));
		}
	
	//	Otherwise, a single column

	else
		{
		cxRequired = m_rcImage.left + RectWidth(m_rcImage);
		cyRequired = m_rcImage.top + (m_iRotationCount * RectHeight(m_rcImage));
		}
	}

void CObjectImageArray::ComputeSourceXY (int iTick, int iRotation, int *retxSrc, int *retySrc) const

//	ComputeSourceXY
//
//	Computes the frame source position, based on tick and rotation.

	{
	//	If we're animating, then we expect a set of columns with one frame
	//	per column.

	if (m_iFrameCount > 0 && m_iTicksPerFrame > 0)
		{
		int iFrame;

		//	Compute the frame number.

		if (m_iFlashTicks > 0)
			{
			int iTotal = m_iFlashTicks + m_iTicksPerFrame;
			iFrame = (((iTick % iTotal) < m_iFlashTicks) ? 1 : 0);
			}
		else
			iFrame = ((iTick / m_iTicksPerFrame) % m_iFrameCount);

		//	If we've got multi-row animations, then we deal with that.

		if (m_iFramesPerRow != m_iFrameCount)
			{
			int iFrameCol = (iFrame / m_iFramesPerRow);
			int iFrameRow = (iFrame % m_iFramesPerRow);

			*retxSrc = m_rcImage.left + (iFrameCol * RectWidth(m_rcImage));
			*retySrc = m_rcImage.top + (iFrameRow * RectHeight(m_rcImage));
			}

		//	If we've got multi-column rotations, then we need to deal with that.

		else if (m_iRotationCount != m_iFramesPerColumn)
			{
			int iColsPerFrame = (m_iRotationCount + m_iFramesPerColumn - 1) / m_iFramesPerColumn;
			int iRotationCol = (iRotation / m_iFramesPerColumn);
			int iRotationRow = (iRotation % m_iFramesPerColumn);

			*retxSrc = m_rcImage.left + (((iFrame * iColsPerFrame) + iRotationCol) * RectWidth(m_rcImage));
			*retySrc = m_rcImage.top + (iRotationRow * RectHeight(m_rcImage));
			}

		//	Otherwise, we expect a single column per frame

		else
			{
			*retxSrc = m_rcImage.left + (iFrame * RectWidth(m_rcImage));
			*retySrc = m_rcImage.top + (iRotation * RectHeight(m_rcImage));
			}
		}

	//	If we've got multi-column rotations but no animations, then we just
	//	compute the proper column

	else if (m_iRotationCount != m_iFramesPerColumn)
		{
		int iRotationCol = (iRotation / m_iFramesPerColumn);
		int iRotationRow = (iRotation % m_iFramesPerColumn);

		*retxSrc = m_rcImage.left + (iRotationCol * RectWidth(m_rcImage));
		*retySrc = m_rcImage.top + (iRotationRow * RectHeight(m_rcImage));
		}
	
	//	Otherwise, a single columne

	else if (iRotation > 0)
		{
		*retxSrc = m_rcImage.left;
		*retySrc = m_rcImage.top + (iRotation * RectWidth(m_rcImage));
		}

	//	Otherwise, it's simple

	else
		{
		*retxSrc = m_rcImage.left;
		*retySrc = m_rcImage.top;
		}
	}

void CObjectImageArray::ComputeRotationOffsets (void)

//	ComputeRotationOffsets
//
//	Compute rotation offsets

	{
	if (m_iRotationOffset != 0 && m_iRotationCount > 0)
		{
		if (m_pRotationOffset)
			delete [] m_pRotationOffset;

		m_pRotationOffset = new OffsetStruct[m_iRotationCount];
		for (int i = 0; i < m_iRotationCount; i++)
			{
			int iRotationAngle = 360 / m_iRotationCount;
			int iAngleAdj = (m_iRotationCount / 4) + 1;
			int iAngle = iRotationAngle * (((m_iRotationCount - (i+1)) + iAngleAdj) % m_iRotationCount);
			CVector vOffset = PolarToVector(iAngle, (Metric)m_iRotationOffset);
			m_pRotationOffset[i].x = (int)vOffset.GetX();
			m_pRotationOffset[i].y = (int)vOffset.GetY();
			}
		}
	else
		{
		if (m_pRotationOffset)
			delete [] m_pRotationOffset;

		m_pRotationOffset = NULL;
		}
	}

void CObjectImageArray::ComputeRotationOffsets (int xOffset, int yOffset)

//	ComputeRotationOffsets
//
//	Computer rotation offsets from a fixed offset

	{
	if (m_iRotationCount != 0)
		{
		if (m_pRotationOffset)
			delete [] m_pRotationOffset;

		m_pRotationOffset = new OffsetStruct[m_iRotationCount];
		for (int i = 0; i < m_iRotationCount; i++)
			{
			m_pRotationOffset[i].x = xOffset;
			m_pRotationOffset[i].y = yOffset;
			}
		}
	}

void CObjectImageArray::CopyFrom (const CObjectImageArray &Source)

//	CopyFrom
//
//	Copy image. NOTE: We expect the object to be initialized to be empty.

	{
	m_dwBitmapUNID = Source.m_dwBitmapUNID;
	m_pImage = Source.m_pImage;
	m_rcImage = Source.m_rcImage;
	m_iFrameCount = Source.m_iFrameCount;
	m_iTicksPerFrame = Source.m_iTicksPerFrame;
	m_iFlashTicks = Source.m_iFlashTicks;

	m_iBlending = Source.m_iBlending;
	m_iViewportSize = Source.m_iViewportSize;
	m_iFramesPerColumn = Source.m_iFramesPerColumn;
	m_iFramesPerRow = Source.m_iFramesPerRow;
	m_bDefaultSize = Source.m_bDefaultSize;

	m_iRotationCount = Source.m_iRotationCount;
	m_iRotationOffset = Source.m_iRotationOffset;
	if (Source.m_pRotationOffset)
		{
		m_pRotationOffset = new OffsetStruct[m_iRotationCount];
		for (int i = 0; i < m_iRotationCount; i++)
			m_pRotationOffset[i] = Source.m_pRotationOffset[i];
		}
	else
		m_pRotationOffset = NULL;

	m_pGlowImages = NULL;
	m_pScaledImages = NULL;
	m_cxScaledImage = -1;

	m_cs = CCriticalSection();
	}

void CObjectImageArray::CopyImage (CG32bitImage &Dest, int x, int y, int iFrame, int iRotation) const

//	CopyImage
//
//	Copies entire image to the destination

	{
	if (m_pImage)
		{
		CG32bitImage *pSource = m_pImage->GetRawImage(NULL_STR);
		if (pSource == NULL)
			return;

		int xSrc;
		int ySrc;
		ComputeSourceXY(iFrame, iRotation, &xSrc, &ySrc);

		Dest.Copy(xSrc,
				ySrc,
				RectWidth(m_rcImage),
				RectHeight(m_rcImage),
				*pSource,
				x,
				y);
		}
	}

void CObjectImageArray::GenerateGlowImage (int iRotation) const

//	GenerateGlowImage
//
//	Generates a mask that looks like a glow. The mask is 0 for all image pixels
//	and for all pixels where there is no glow (thus we can optimize painting
//	of the glow by ignoring 0 values)

	{
	DEBUG_TRY

	ASSERT(iRotation >= 0 && iRotation < m_iRotationCount);

	//	Source

	if (m_pImage == NULL)
		return;

	CG32bitImage *pSource = m_pImage->GetRawImage(NULL_STR);
	if (pSource == NULL)
		return;

	//	Allocate the array of images (if not already allocated)

	if (m_pGlowImages == NULL)
		m_pGlowImages = new CG8bitImage [m_iRotationCount];

	//	If the image for this rotation has already been initialized, then
	//	we're done

	if (!m_pGlowImages[iRotation].IsEmpty())
		return;

	//	Otherwise we need to create the glow mask. The glow image is larger
	//	than the object image (by GLOW_SIZE)

	int cxSrcWidth = RectWidth(m_rcImage);
	int cySrcHeight = RectHeight(m_rcImage);
	int cxGlowWidth = cxSrcWidth + 2 * GLOW_SIZE;
	int cyGlowHeight = cySrcHeight + 2 * GLOW_SIZE;
	m_pGlowImages[iRotation].Create(cxGlowWidth, cyGlowHeight);

	//	Get the extent of the source image

	RECT rcSrc;
	ComputeSourceXY(0, iRotation, &rcSrc.left, &rcSrc.top);
	rcSrc.right = rcSrc.left + cxSrcWidth;
	rcSrc.bottom = rcSrc.top + cySrcHeight;

	//	Loop over every pixel of the destination

	BYTE *pDestRow = m_pGlowImages[iRotation].GetPixelPos(0, 0);
	BYTE *pDestRowEnd = m_pGlowImages[iRotation].GetPixelPos(0, cyGlowHeight);
	int ySrc = rcSrc.top - GLOW_SIZE;
	while (pDestRow < pDestRowEnd)
		{
		BYTE *pDest = pDestRow;
		BYTE *pDestEnd = pDest + cxGlowWidth;
		int xSrc = rcSrc.left - GLOW_SIZE;
		while (pDest < pDestEnd)
			{
			//	If the source image is using this pixel then we don't
			//	do anything.

			CG32bitPixel rgbColor;
			if ((xSrc >= rcSrc.left && xSrc < rcSrc.right && ySrc >= rcSrc.top && ySrc < rcSrc.bottom)
					&& ((rgbColor = pSource->GetPixel(xSrc, ySrc)).GetAlpha()))
				{
				if (CG32bitPixel::Desaturate(rgbColor).GetRed() < 0x40)
					*pDest = 0x60;
				else
					*pDest = 0x00;
				}

			//	Otherwise we process the pixel

			else
				{
				int xStart = (rcSrc.left > (xSrc - GLOW_SIZE) ? ((rcSrc.left - (xSrc - GLOW_SIZE) + (GLOW_SIZE / 2 - 1)) / (GLOW_SIZE / 2)) : 0);
				int xEnd = ((xSrc + GLOW_SIZE) >= rcSrc.right ? (FILTER_SIZE - (((GLOW_SIZE / 2 + 1) + xSrc + GLOW_SIZE - rcSrc.right) / (GLOW_SIZE / 2))) : FILTER_SIZE);
				int yStart = (rcSrc.top > (ySrc - GLOW_SIZE) ? ((rcSrc.top - (ySrc - GLOW_SIZE) + (GLOW_SIZE / 2 - 1)) / (GLOW_SIZE / 2)) : 0);
				int yEnd = ((ySrc + GLOW_SIZE) >= rcSrc.bottom ? (FILTER_SIZE - (((GLOW_SIZE / 2 + 1) + ySrc + GLOW_SIZE - rcSrc.bottom) / (GLOW_SIZE / 2))) : FILTER_SIZE);

				int iTotal = 0;
				for (int i = yStart; i < yEnd; i++)
					for (int j = xStart; j < xEnd; j++)
						if (pSource->GetPixel(xSrc + g_FilterOffset[j], ySrc + g_FilterOffset[i]).GetAlpha())
							iTotal += g_Filter[i][j];

				int iValue = (512 * iTotal / FIXED_POINT);
				*pDest = (iValue > 0xf8 ? 0xf8 : (BYTE)iValue);
				}

			//	Next

			pDest++;
			xSrc++;
			}

		pDestRow = m_pGlowImages[iRotation].NextRow(pDestRow);
		ySrc++;
		}

	DEBUG_CATCH
	}

void CObjectImageArray::GenerateScaledImages (int iRotation, int cxWidth, int cyHeight) const

//	GenerateScaledImages
//
//	Generate scaled images

	{
	ASSERT(iRotation >= 0 && iRotation < m_iRotationCount);

	//	Allocate the array of images (if not already allocated)

	if (m_pScaledImages == NULL || cxWidth != m_cxScaledImage)
		{
		if (m_pScaledImages)
			delete [] m_pScaledImages;

		m_pScaledImages = new CG32bitImage [m_iRotationCount];
		m_cxScaledImage = cxWidth;
		}

	//	If the image for this rotation has already been initialized, then
	//	we're done

	else if (!m_pScaledImages[iRotation].IsEmpty())
		return;

	//	Get the extent of the source image

	int cxSrcWidth = RectWidth(m_rcImage);
	int cySrcHeight = RectHeight(m_rcImage);

	CG32bitImage *pSource = m_pImage->GetRawImage(NULL_STR);
	if (pSource == NULL || cxSrcWidth == 0 || cySrcHeight == 0)
		return;

	RECT rcSrc;
	ComputeSourceXY(0, iRotation, &rcSrc.left, &rcSrc.top);
	rcSrc.right = rcSrc.left + cxSrcWidth;
	rcSrc.bottom = rcSrc.top + cySrcHeight;

	//	Scale

	m_pScaledImages[iRotation].CreateFromImageTransformed(*pSource,
			rcSrc.left,
			rcSrc.top,
			cxSrcWidth,
			cySrcHeight,
			(Metric)cxWidth / (Metric)cxSrcWidth,
			(Metric)cyHeight / (Metric)cySrcHeight,
			0.0);
	}

CString CObjectImageArray::GetFilename (void) const

//	GetFilename
//
//	Returns the filename used for the image

	{
	if (m_pImage == NULL)
		return NULL_STR;

	return m_pImage->GetImageFilename();
	}

CG32bitImage *CObjectImageArray::GetHitMask (void) const

//	GetHitMask
//
//	Returns the hit mask to use.

	{
	CG32bitImage *pSource = m_pImage->GetHitMask();
	if (pSource == NULL)
		pSource = m_pImage->GetRawImage(CONSTLIT("PointInImage"));

	return pSource;
	}

bool CObjectImageArray::GetImageOffset (int iTick, int iRotation, int *retx, int *rety) const

//	GetImageOffset
//
//	Returns the image offset. Also returns FALSE if there is no image offset.
//	(But the return variables are always initialized.)

	{
	if (m_pRotationOffset)
		{
		*retx = m_pRotationOffset[iRotation % m_iRotationCount].x;
		*rety = -m_pRotationOffset[iRotation % m_iRotationCount].y;
		return true;
		}
	else
		{
		*retx = 0;
		*rety = 0;
		return false;
		}
	}

RECT CObjectImageArray::GetImageRect (int iTick, int iRotation, int *retxCenter, int *retyCenter) const

//	GetImageRect
//
//	Returns the rect of the image

	{
	RECT rcRect;

	ComputeSourceXY(iTick, iRotation, &rcRect.left, &rcRect.top);
	rcRect.right = rcRect.left + RectWidth(m_rcImage);
	rcRect.bottom = rcRect.top + RectHeight(m_rcImage);

	if (retxCenter)
		*retxCenter = rcRect.left + ((RectWidth(m_rcImage) / 2) - (m_pRotationOffset ? m_pRotationOffset[iRotation % m_iRotationCount].x : 0));

	if (retyCenter)
		*retyCenter = rcRect.top + ((RectHeight(m_rcImage) / 2) + (m_pRotationOffset ? m_pRotationOffset[iRotation % m_iRotationCount].y : 0));

	return rcRect;
	}

RECT CObjectImageArray::GetImageRectAtPoint (int x, int y) const

//	GetImageRectAtPoint
//
//	Returns the rect of the image centered at the given coordinates

	{
	RECT rcRect;
	int cxWidth = RectWidth(m_rcImage);
	int cyHeight = RectHeight(m_rcImage);

	rcRect.left = x - (cxWidth / 2);
	rcRect.right = rcRect.left + cxWidth;
	rcRect.top = y - (cyHeight / 2);
	rcRect.bottom = rcRect.top + cyHeight;

	return rcRect;
	}

int CObjectImageArray::GetImageViewportSize (void) const

//	GetImageViewportSize
//
//	Returns the size of the image viewport (in pixels). This is used to compute
//	perspective distortion.

	{
	return m_iViewportSize;
	}

size_t CObjectImageArray::GetMemoryUsage (void) const

//	GetMemoryUsage
//
//	Returns the amount of memory used by bitmaps.

	{
	int i;
	size_t dwTotal = 0;

	if (m_pImage)
		dwTotal += m_pImage->GetMemoryUsage();

	if (m_pGlowImages)
		{
		for (i = 0; i < m_iRotationCount; i++)
			dwTotal += m_pGlowImages[i].GetMemoryUsage();
		}

	if (m_pScaledImages)
		{
		for (i = 0; i < m_iRotationCount; i++)
			dwTotal += m_pScaledImages[i].GetMemoryUsage();
		}

	return dwTotal;
	}

bool CObjectImageArray::ImagesIntersect (int iTick, int iRotation, int x, int y, const CObjectImageArray &Image2, int iTick2, int iRotation2) const

//	ImagesIntersect
//
//	Returns TRUE if the given image intersects with this image

	{
	DEBUG_TRY

	if (m_pImage == NULL || Image2.m_pImage == NULL)
		return false;

	//	Compute the rectangle of image1

	RECT rcRect;
	int cxWidth = RectWidth(m_rcImage);
	int cyHeight = RectHeight(m_rcImage);
	ComputeSourceXY(iTick, iRotation, &rcRect.left, &rcRect.top);
	rcRect.right = rcRect.left + cxWidth;
	rcRect.bottom = rcRect.top + cyHeight;

	//	Compute the rectangle of image2

	RECT rcRect2;
	int cxWidth2 = RectWidth(Image2.m_rcImage);
	int cyHeight2 = RectHeight(Image2.m_rcImage);
	Image2.ComputeSourceXY(iTick2, iRotation2, &rcRect2.left, &rcRect2.top);
	rcRect2.right = rcRect2.left + cxWidth2;
	rcRect2.bottom = rcRect2.top + cyHeight2;
	
	//	Now figure out the position of image2 on the image1 coordinate space

	int xCenter = rcRect.left + (cxWidth / 2);
	int yCenter = rcRect.top + (cyHeight / 2);

	RECT rcImage2On1;
	rcImage2On1.left = xCenter + x - (cxWidth2 / 2);
	rcImage2On1.top = yCenter + y - (cyHeight2 / 2);

	if (m_pRotationOffset)
		{
		rcImage2On1.left -= m_pRotationOffset[iRotation % m_iRotationCount].x;
		rcImage2On1.top += m_pRotationOffset[iRotation % m_iRotationCount].y;
		}

	if (Image2.m_pRotationOffset)
		{
		rcImage2On1.left += Image2.m_pRotationOffset[iRotation2 % Image2.m_iRotationCount].x;
		rcImage2On1.top -= Image2.m_pRotationOffset[iRotation2 % Image2.m_iRotationCount].y;
		}

	rcImage2On1.right = rcImage2On1.left + cxWidth2;
	rcImage2On1.bottom = rcImage2On1.top + cyHeight2;

	//	Intersect the rectangles

	RECT rcRectInt;
	if (!::IntersectRect(&rcRectInt, &rcRect, &rcImage2On1))
		return false;

	//	Figure out the position of image1 on the image2 coordinate space

	int xOffset = rcRect.left - rcImage2On1.left;
	int yOffset = rcRect.top - rcImage2On1.top;

	RECT rcImage1On2;
	rcImage1On2.left = rcRect2.left + xOffset;
	rcImage1On2.top = rcRect2.top + yOffset;
	rcImage1On2.right = rcImage1On2.left + cxWidth;
	rcImage1On2.bottom = rcImage1On2.top + cyHeight;

	//	Intersect the rectangles

	RECT rcRectInt2;
	::IntersectRect(&rcRectInt2, &rcRect2, &rcImage1On2);
	ASSERT(RectWidth(rcRectInt) == RectWidth(rcRectInt2));
	ASSERT(RectHeight(rcRectInt) == RectHeight(rcRectInt2));

	//	Images

	CG32bitImage *pSrc1 = GetHitMask();
	CG32bitImage *pSrc2 = Image2.GetHitMask();
	if (pSrc1 == NULL || pSrc2 == NULL)
		return false;

	//	Now iterate over the intersection area and see if there
	//	are any pixels in common.

	CG32bitPixel *pRow = pSrc1->GetPixelPos(rcRectInt.left, rcRectInt.top);
	CG32bitPixel *pRowEnd = pSrc1->GetPixelPos(rcRectInt.left, rcRectInt.bottom);
	CG32bitPixel *pRow2 = pSrc2->GetPixelPos(rcRectInt2.left, rcRectInt2.top);

	int cxWidthInt = RectWidth(rcRectInt);
	while (pRow < pRowEnd)
		{
		CG32bitPixel *pPos = pRow;
		CG32bitPixel *pEnd = pPos + cxWidthInt;
		CG32bitPixel *pPos2 = pRow2;

		while (pPos < pEnd)
			{
			if (pPos->GetAlpha() && pPos2->GetAlpha())
				return true;

			pPos++;
			pPos2++;
			}

		pRow = pSrc1->NextRow(pRow);
		pRow2 = pSrc2->NextRow(pRow2);
		}

	//	If we get this far then we did not intersect

	return false;

	DEBUG_CATCH
	}

ALERROR CObjectImageArray::Init (CUniverse &Universe, DWORD dwBitmapUNID, int iFrameCount, int iTicksPerFrame, bool bResolveNow)

//  Init
//
//  Create from parameters

    {
	CleanUp();

	//	Initialize basic info

	m_dwBitmapUNID = dwBitmapUNID;

    //  Get the actual image, if we want it now

    if (bResolveNow)
        {
        m_pImage = Universe.FindLibraryImage(m_dwBitmapUNID);

        //  We assume the RECT is the entire bitmap

        CG32bitImage *pImage = (m_pImage ? m_pImage->GetRawImage(CONSTLIT("CObjectImageArray::Init")) : NULL);
        m_rcImage.left = 0;
        m_rcImage.top = 0;
        if (pImage)
            {
            m_rcImage.right = pImage->GetWidth();
            m_rcImage.bottom = pImage->GetHeight();
            }
        else
            {
            m_rcImage.right = 0;
            m_rcImage.bottom = 0;
            }
        }
    else
        {
        m_rcImage.left = 0;
        m_rcImage.top = 0;
        m_rcImage.right = 0;
        m_rcImage.bottom = 0;
        }

    //  Initialize the rest

	m_iFrameCount = iFrameCount;
	m_iRotationCount = 1;
	m_iFramesPerColumn = m_iRotationCount;
	m_iFramesPerRow = iFrameCount;
	m_iTicksPerFrame = iTicksPerFrame;
	m_iFlashTicks = 0;
	m_iRotationOffset = 0;
	m_pRotationOffset = NULL;
	m_iBlending = blendNormal;
	m_iViewportSize = RectWidth(m_rcImage);
	m_bDefaultSize = false;

	return NOERROR;
    }

ALERROR CObjectImageArray::Init (CObjectImage *pImage, const RECT &rcImage, int iFrameCount, int iTicksPerFrame)

//	Init
//
//	Create from parameters

	{
	if (!pImage)
		throw CException(ERR_FAIL);

	CleanUp();

	//	Initialize basic info

	m_dwBitmapUNID = pImage->GetUNID();
	//	LATER: pImage should be a smart pointer
	m_pImage = TSharedPtr<CObjectImage>(pImage->AddRef());
	m_rcImage = rcImage;
	m_iFrameCount = iFrameCount;
	m_iRotationCount = 1;
	m_iFramesPerColumn = 1;
	m_iFramesPerRow = iFrameCount;
	m_iTicksPerFrame = iTicksPerFrame;
	m_iFlashTicks = 0;
	m_iRotationOffset = 0;
	m_pRotationOffset = NULL;
	m_iBlending = blendNormal;
	m_iViewportSize = RectWidth(rcImage);
	m_bDefaultSize = false;

	return NOERROR;
	}

ALERROR CObjectImageArray::Init (CUniverse &Universe, DWORD dwBitmapUNID, const RECT &rcImage, int iFrameCount, int iTicksPerFrame)

//	Init
//
//	Create from parameters

	{
	CleanUp();

	//	Initialize basic info

	m_dwBitmapUNID = dwBitmapUNID;
	m_pImage = Universe.FindLibraryImage(m_dwBitmapUNID);
	m_rcImage = rcImage;
	m_iFrameCount = iFrameCount;
	m_iRotationCount = 1;
	m_iFramesPerColumn = m_iRotationCount;
	m_iFramesPerRow = iFrameCount;
	m_iTicksPerFrame = iTicksPerFrame;
	m_iFlashTicks = 0;
	m_iRotationOffset = 0;
	m_pRotationOffset = NULL;
	m_iBlending = blendNormal;
	m_iViewportSize = RectWidth(rcImage);
	m_bDefaultSize = false;

	return NOERROR;
	}

ALERROR CObjectImageArray::InitFromBitmap (CG32bitImage *pBitmap, const RECT &rcImage, int iFrameCount, int iTicksPerFrame, bool bFreeBitmap, int xOffset, int yOffset)

//	InitFromBitmap
//
//	Create from parameters

	{
	CleanUp();

	//	Initialize basic info

	m_dwBitmapUNID = 0;
	m_pImage = TSharedPtr<CObjectImage>(new CObjectImage(pBitmap, bFreeBitmap));
	m_rcImage = rcImage;
	m_iFrameCount = iFrameCount;
	m_iRotationCount = 1;
	m_iFramesPerColumn = m_iRotationCount;
	m_iFramesPerRow = iFrameCount;
	m_iTicksPerFrame = iTicksPerFrame;
	m_iFlashTicks = 0;
	m_iRotationOffset = 0;
	m_pRotationOffset = NULL;
	m_iBlending = blendNormal;
	m_iViewportSize = RectWidth(rcImage);
	m_bDefaultSize = false;

    if (xOffset != 0 || yOffset != 0)
        ComputeRotationOffsets(xOffset, -yOffset);

	return NOERROR;
	}

ALERROR CObjectImageArray::InitFromFrame (const CObjectImageArray &Source, int iTick, int iRotationIndex)

//	InitFromFrame
//
//	Initializes from a single frame in the source image.

	{
	//	NOTE: Only works for global CObjectImage (whose lifetime is managed by
	//	the Universe).

	if (Source.m_dwBitmapUNID == 0)
		{
		ASSERT(false);
		return ERR_FAIL;
		}

	CleanUp();

	m_dwBitmapUNID = Source.m_dwBitmapUNID;
	m_pImage = Source.m_pImage;
	m_rcImage = Source.GetImageRect(iTick, iRotationIndex);
	m_iFrameCount = 1;
	m_iFramesPerColumn = 1;
	m_iFramesPerRow = 1;
	m_iTicksPerFrame = 0;
	m_iFlashTicks = 0;
	m_iRotationCount = 0;
	m_pRotationOffset = NULL;
	m_iBlending = blendNormal;
	m_iViewportSize = Source.m_iViewportSize;
	m_bDefaultSize = false;

	return NOERROR;
	}

ALERROR CObjectImageArray::InitFromRotated (const CObjectImageArray &Source, const RECT &rcSrc, int iRotation)

//	InitFromRotated
//
//	Creates a new image from the source.

	{
	if (Source.m_pImage == NULL)
		return NOERROR;

	CG32bitImage &SourceImage = Source.GetImage(CONSTLIT("Rotated image"));

	//	If we have a shadow mask, make a copy

	CG32bitImage *pShadowMask = NULL;
	if (Source.m_pImage->GetShadowMask())
		{
		pShadowMask = new CG32bitImage;
		pShadowMask->CreateFromImageTransformed(*Source.m_pImage->GetShadowMask(), rcSrc.left, rcSrc.top, RectWidth(rcSrc), RectHeight(rcSrc), 1.0, 1.0, iRotation);
		}

	//	Create a rotated image

	CG32bitImage *pDest = new CG32bitImage;
	pDest->CreateFromImageTransformed(SourceImage, rcSrc.left, rcSrc.top, RectWidth(rcSrc), RectHeight(rcSrc), 1.0, 1.0, iRotation);

	//	Clean up

	CleanUp();

	//	Initialize

	m_dwBitmapUNID = 0;
	m_pImage = TSharedPtr<CObjectImage>(new CObjectImage(pDest, true, pShadowMask));
	m_rcImage.left = 0;
	m_rcImage.top = 0;
	m_rcImage.right = pDest->GetWidth();
	m_rcImage.bottom = pDest->GetHeight();
	m_iFrameCount = 1;
	m_iRotationCount = 1;
	m_iFramesPerColumn = 1;
	m_iFramesPerRow = 1;
	m_iTicksPerFrame = 0;
	m_iFlashTicks = 0;
	m_iRotationOffset = 0;
	m_pRotationOffset = NULL;
	m_iBlending = blendNormal;
	m_iViewportSize = RectWidth(m_rcImage);
	m_bDefaultSize = false;

	return NOERROR;
	}

ALERROR CObjectImageArray::InitFromRotated (const CObjectImageArray &Source, int iTick, int iVariant, int iRotation)

//	InitFromRotated
//
//	Creates a new image from the source

	{
	return InitFromRotated(Source, Source.GetImageRect(iTick, iVariant), iRotation);
	}

ALERROR CObjectImageArray::InitFromXML (const CXMLElement &Desc)

//	InitFromXML

	{
	SDesignLoadCtx Ctx;

	return InitFromXML(Ctx, Desc, true);
	}

ALERROR CObjectImageArray::InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc, bool bResolveNow, int iDefaultRotationCount)

//	InitFromXML
//
//	Create from XML description

	{
	ALERROR error;

	//	Initialize basic info

	m_rcImage.left = Desc.GetAttributeInteger(CONSTLIT(g_ImageXAttrib));
	m_rcImage.top = Desc.GetAttributeInteger(CONSTLIT(g_ImageYAttrib));

	//	If we have a width and height, then initialize it.

	int cxWidth;
	if (Desc.FindAttributeInteger(IMAGE_WIDTH_ATTRIB, &cxWidth))
		{
		m_rcImage.right = m_rcImage.left + cxWidth;
		m_rcImage.bottom = m_rcImage.top + Desc.GetAttributeInteger(IMAGE_HEIGHT_ATTRIB);
		m_bDefaultSize = false;
		}

	//	Otherwise, get the size from the image

	else
		{
		m_rcImage.right = m_rcImage.left;
		m_rcImage.bottom = m_rcImage.top;
		m_bDefaultSize = true;
		}

	m_iFrameCount = Desc.GetAttributeInteger(CONSTLIT(g_ImageFrameCountAttrib));
	m_iRotationCount = Desc.GetAttributeIntegerBounded(ROTATION_COUNT_ATTRIB, 1, -1, iDefaultRotationCount);

	int iRotationCols = Desc.GetAttributeIntegerBounded(ROTATION_COLUMNS_ATTRIB, 0, m_iRotationCount, 0);
	if (iRotationCols > 0)
		m_iFramesPerColumn = (m_iRotationCount + iRotationCols - 1) / iRotationCols;
	else
		m_iFramesPerColumn = m_iRotationCount;

	int iAnimationCols = Desc.GetAttributeIntegerBounded(ANIMATION_COLUMNS_ATTRIB, 0, m_iFrameCount, 0);
	if (iAnimationCols > 0)
		m_iFramesPerRow = (m_iFrameCount + iAnimationCols - 1) / iAnimationCols;
	else
		m_iFramesPerRow = m_iFrameCount;

	m_iTicksPerFrame = Desc.GetAttributeInteger(CONSTLIT(g_ImageTicksPerFrameAttrib));
	if (m_iTicksPerFrame <= 0 && m_iFrameCount > 1)
		m_iTicksPerFrame = 1;
	m_iFlashTicks = Desc.GetAttributeInteger(FLASH_TICKS_ATTRIB);

	CString sBlending = Desc.GetAttribute(BLENDING_ATTRIB);
	if (strEquals(sBlending, LIGHTEN_BLENDING))
		m_iBlending = blendLighten;
	else
		m_iBlending = blendNormal;

	//	Viewport

	Metric rViewportRatio;
	if (Desc.FindAttributeDouble(VIEWPORT_RATIO_ATTRIB, &rViewportRatio))
		{
		if (rViewportRatio > 0.0)
			m_iViewportSize = Max(1, mathRound(RectWidth(m_rcImage) / (2.0 * rViewportRatio)));
		else
			m_iViewportSize = Max(1, RectWidth(m_rcImage));
		}
	else if (Desc.FindAttributeInteger(VIEWPORT_SIZE_ATTRIB, &m_iViewportSize))
		m_iViewportSize = Max(1, m_iViewportSize);
	else
		m_iViewportSize = Max(1, RectWidth(m_rcImage));

	//	Compute rotation offsets

	m_iRotationOffset = Desc.GetAttributeInteger(ROTATE_OFFSET_ATTRIB);
	if (m_iRotationOffset)
		ComputeRotationOffsets();
	else
		{
		int xOffset = Desc.GetAttributeInteger(X_OFFSET_ATTRIB);
		int yOffset = Desc.GetAttributeInteger(Y_OFFSET_ATTRIB);
		if (xOffset != 0 || yOffset != 0)
			ComputeRotationOffsets(xOffset, yOffset);
		}

	//	Get the image from the universe

	if (error = LoadUNID(Ctx, Desc.GetAttribute(CONSTLIT(g_ImageIDAttrib)), &m_dwBitmapUNID))
		return error;

	if (bResolveNow)
		m_pImage = Ctx.GetUniverse().FindLibraryImage(m_dwBitmapUNID);
	else
		m_pImage = NULL;

	return NOERROR;
	}

void CObjectImageArray::MarkImage (void) const

//	MarkImage
//
//	Mark image so that the sweeper knows that it is in use

	{
    //  Nothing to do if no image

    if (m_pImage == NULL)
        return;

    //  Mark (and load) the underlying image

	m_pImage->Mark();

    //  If we're in debug mode, we take this opportunity to validate the image
    //  rect against the actual image.

    if (m_pImage->GetUniverse().InDebugMode())
		ValidateImageSize(m_pImage->GetWidth(), m_pImage->GetHeight());
	}

ALERROR CObjectImageArray::OnDesignLoadComplete (SDesignLoadCtx& Ctx)

//	OnDesignLoadComplete
//
//	All design elements have been loaded

	{
	DEBUG_TRY

#ifdef NO_RESOURCES
		if (Ctx.bNoResources)
			return NOERROR;
#endif

	if (m_dwBitmapUNID)
		{
		m_pImage = Ctx.GetUniverse().FindLibraryImageUnbound(m_dwBitmapUNID);
		if (m_pImage)
			{
			if (!m_pImage->IsBound())
				{
				if (ALERROR error = m_pImage->BindDesign(Ctx))
					return error;
				}
			}
		else
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Unknown image: %x"), m_dwBitmapUNID);
			return ERR_FAIL;
			}

		//  If we don't have a RECT, initialize it now that we have the image

		if (m_bDefaultSize)
			{
			CG32bitImage* pImage = m_pImage->GetRawImage(CONSTLIT("Resolve image size"));
			if (pImage)
				{
				m_rcImage.right = pImage->GetWidth();
				m_rcImage.bottom = pImage->GetHeight();

				m_iViewportSize = RectWidth(m_rcImage);
				}
			}
		}

	return NOERROR;

	DEBUG_CATCH
	}

//	PaintImage
//	
//	Paints the image with MT if Ctx is not NULL and defines m_pThreadPool
//	Otherwise uses standard painting
//	
void CObjectImageArray::PaintImage (CG32bitImage& Dest, int x, int y, int iTick, int iRotation, bool bComposite, SViewportPaintCtx* Ctx, int iOffsetY, int iOffsetCY) const
	{
	DEBUG_TRY

	m_cs.Lock();
	CG32bitImage* pSource = m_pImage->GetRawImage(NULL_STR);
	m_cs.Unlock();

	if (pSource == NULL)
		return;

	if (m_pImage)
		{
		
		//	capture draw performance if that debug setting is configured

		LARGE_INTEGER iPerfStart;
		LARGE_INTEGER iPerfEnd;
		if (Ctx && Ctx->bDbgShowPaintTime)
			QueryPerformanceCounter(&iPerfStart);

		//	If we are on the main thread and we have a thread pool in the paint context, use multithreading
		//	Due to the performance impact of starting individual threads though, we want to make sure that
		//	we do not use excessive threads on a small ship
		int iScanLines = RectHeight(m_rcImage);
		int iScanLineLength = RectWidth(m_rcImage);
		int iProjectedExtraWorkers = (Ctx && Ctx->pThreadPool && !Ctx->bForceSTPaint) ? (iScanLineLength >> Ctx->dwMinChunkSizePow ? iScanLineLength - 1 : (iScanLines * iScanLineLength) >> Ctx->dwMinChunkSizePow) : 0;

		if (iProjectedExtraWorkers)
			{

			//	Group scanlines per worker
			int iNumWorkers = min(Ctx->pThreadPool->GetThreadCount(), 1 + iProjectedExtraWorkers);
			int iScanLinesPerWorker = iScanLines / iNumWorkers;
			int iScanLinesRemainder = iScanLines % iNumWorkers;
			int iYOffset = 0;
			CSpritePaintWorker::ePaintMode iPaintMode = CSpritePaintWorker::ePaintImage;

			//	Create context
			CSpritePaintWorker::SCtx WorkerCtx = CSpritePaintWorker::SCtx();
			WorkerCtx.pSrc = this;
			WorkerCtx.pDest = &Dest;
			WorkerCtx.xDest = x;
			WorkerCtx.xSrc = m_rcImage.left;
			WorkerCtx.yDest = y;
			WorkerCtx.ySrc = m_rcImage.top;
			WorkerCtx.iTick = iTick;
			WorkerCtx.iRotation = iRotation;
			WorkerCtx.iMode = iPaintMode;
			WorkerCtx.fComposite = bComposite ? 1 : 0;
			WorkerCtx.fDbgShowPaintLocation = Ctx->bDbgShowPaintLocations ? 1 : 0;

			//	Create tasks

			for (int i = 0; i < iNumWorkers; i++)
				{
				int iCY = iScanLinesPerWorker + (i < iScanLinesRemainder ? 1 : 0);
				CSpritePaintWorker *pTask = new CSpritePaintWorker(iYOffset, iCY, WorkerCtx);	//	Gets cleaned up by CThreadPool.Run()
				iYOffset += iCY;
				Ctx->pThreadPool->AddTask(pTask);
				}

			//	Run tasks

			Ctx->pThreadPool->Run();

			//	This is debug code that will normally be optimized out as unreachable by the compiler
			//	It is left in here for easy debugging of changes to blt or CThreadPool in case someone
			//	accidentally adds thread-unsafe code to them and needs to debug it in isolation from
			//	isolate it from the rest of the multithreading code, using the safe eDebugTask function
			//	to simulate a load
			if (iPaintMode == CSpritePaintWorker::eDebugTask)
				{
				//	if debugging, we need to paint normally since the debug task is just math
				WorkerPaintImage(Dest, x, y, iTick, iRotation, bComposite, -1, -1, Ctx->bDbgShowPaintLocations);
				}
			}

		//	Otherwise we are painting on one thread

		else
			{
			WorkerPaintImage(Dest, x, y, iTick, iRotation, bComposite, -1, -1, Ctx ? Ctx->bDbgShowPaintLocations : false);
			}

		//	Show total execution time if this debug option is enabled

		if (Ctx && Ctx->bDbgShowPaintTime)
			{
			QueryPerformanceCounter(&iPerfEnd);
			LARGE_INTEGER iFrequency;
			QueryPerformanceFrequency(&iFrequency);
			UINT64 iPerfElapsed = (iPerfEnd.QuadPart - iPerfStart.QuadPart) * 1000000 / iFrequency.QuadPart;
			const CG16bitFont *pMediumFont = &g_pUniverse->GetFont(CONSTLIT("Medium"));
			int iDestX = x - RectWidth(m_rcImage) / 2;
			int iDestY = y - RectHeight(m_rcImage) / 2;
			CString sPerf = strPatternSubst(CONSTLIT("%l05d"), iPerfElapsed);
			pMediumFont->DrawText(Dest, iDestX, iDestY, CG32bitPixel(255, 0, 0), sPerf);
			}
		
		}
	DEBUG_CATCH_MT
	}

//	PaintImage
//	
//	Paints the image with MT if Ctx is not NULL and defines m_pThreadPool
//	Otherwise uses standard painting
//	
void CObjectImageArray::WorkerPaintImage (CG32bitImage& Dest, int x, int y, int iTick, int iRotation, bool bComposite, int iOffsetY, int iOffsetCY, bool bDbgShowPaintLocation) const
	{
	DEBUG_TRY

	m_cs.Lock();
	CG32bitImage* pSource = m_pImage->GetRawImage(NULL_STR);
	m_cs.Unlock();

	if (pSource == NULL)
		return;

	if (m_pImage)
		{
		int xSrc;
		int ySrc;
		ComputeSourceXY(iTick, iRotation, &xSrc, &ySrc);

		ySrc += (iOffsetY > 0) ? iOffsetY : 0;

		if (m_pRotationOffset)
			{
			x += m_pRotationOffset[iRotation % m_iRotationCount].x;
			y -= m_pRotationOffset[iRotation % m_iRotationCount].y;
			}

		int ySprite = RectHeight(m_rcImage);
		int cy = (iOffsetCY > 0) ? min(iOffsetCY, ySprite) : ySprite;
		int cx = RectWidth(m_rcImage);
		int yDest = y - (ySprite / 2) + iOffsetY;
		int xDest = x - (cx / 2);

		if (bComposite)
			{
			Dest.Composite(xSrc,
				ySrc,
				cx,
				cy,
				255,
				*pSource,
				xDest,
				yDest);
			}
		else if (m_iBlending == blendLighten)
			{
			CGDraw::BltLighten(Dest,
				xDest,
				yDest,
				*pSource,
				xSrc,
				ySrc,
				cx,
				cy);
			}
		else
			{
			Dest.Blt(xSrc,
				ySrc,
				cx,
				cy,
				255,
				*pSource,
				xDest,
				yDest);
			}

		if (g_pUniverse->GetDebugOptions().IsShowPaintLocationEnabled())
			{
			//	Put a magenta marker on the first pixel
			Dest.SetPixel(xDest, yDest, CG32bitPixel(255, 0, 255));

			//	Put a green marker on the last pixel
			Dest.SetPixel(xDest + cx, yDest + cy -1, CG32bitPixel(0, 255, 0));
			}
			
		}
	DEBUG_CATCH
	}

void CObjectImageArray::PaintImageShimmering (CG32bitImage &Dest, int x, int y, int iTick, int iRotation, DWORD byOpacity) const

//	PaintImageShimmering
//
//	Paint a distorted/invisible image.
//
//	byOpacity = 0 means the image is invisible.
//	byOpacity = 256 means the image is fully visible.

	{
	if (m_pImage)
		{
		CG32bitImage *pSource = m_pImage->GetRawImage(NULL_STR);
		if (pSource == NULL)
			return;

		int xSrc;
		int ySrc;
		ComputeSourceXY(iTick, iRotation, &xSrc, &ySrc);

		if (m_pRotationOffset)
			{
			x += m_pRotationOffset[iRotation % m_iRotationCount].x;
			y -= m_pRotationOffset[iRotation % m_iRotationCount].y;
			}

		CGDraw::BltShimmer(Dest,
				x - (RectWidth(m_rcImage) / 2),
				y - (RectHeight(m_rcImage) / 2),
				*pSource,
				xSrc,
				ySrc,
				RectWidth(m_rcImage),
				RectHeight(m_rcImage),
				(BYTE)byOpacity,
				iTick);
		}
	}

void CObjectImageArray::PaintImageGrayed (CG32bitImage &Dest, int x, int y, int iTick, int iRotation) const

//	PaintImageGrayed
//
//	Paints the image on the destination

	{
	if (m_pImage)
		{
		CG32bitImage *pSource = m_pImage->GetRawImage(NULL_STR);
		if (pSource == NULL)
			return;

		int xSrc;
		int ySrc;
		ComputeSourceXY(iTick, iRotation, &xSrc, &ySrc);

		if (m_pRotationOffset)
			{
			x += m_pRotationOffset[iRotation % m_iRotationCount].x;
			y -= m_pRotationOffset[iRotation % m_iRotationCount].y;
			}

		CGDraw::BltGray(Dest,
				x - (RectWidth(m_rcImage) / 2),
				y - (RectHeight(m_rcImage) / 2),
				*pSource,
				xSrc,
				ySrc,
				RectWidth(m_rcImage),
				RectHeight(m_rcImage),
				128);
		}
	}

void CObjectImageArray::PaintImageUL (CG32bitImage &Dest, int x, int y, int iTick, int iRotation) const

//	PaintImageUL
//
//	Paints the image. x,y is the upper-left corner of the destination
//
//	Note: This should not use the rotation offsets

	{
	if (m_pImage)
		{
		CG32bitImage *pSource = m_pImage->GetRawImage(NULL_STR);
		if (pSource == NULL)
			return;

		int xSrc;
		int ySrc;
		ComputeSourceXY(iTick, iRotation, &xSrc, &ySrc);

		if (m_iBlending == blendLighten)
			{
			CGDraw::BltLighten(Dest,
					x,
					y,
					*pSource,
					xSrc,
					ySrc,
					RectWidth(m_rcImage),
					RectHeight(m_rcImage));
			}
		else
			{
			Dest.Blt(xSrc,
					ySrc,
					RectWidth(m_rcImage),
					RectHeight(m_rcImage),
					255,
					*pSource,
					x,
					y);
			}
		}
	}

void CObjectImageArray::PaintImageWithGlow (CG32bitImage &Dest,
											int x,
											int y,
											int iTick,
											int iRotation,
											CG32bitPixel rgbGlowColor) const

//	PaintImageWithGlow
//
//	Paints the image on the destination with a pulsating glow around
//	it of the specified color.
//
//	This effect does not work with blending modes.

	{
	//	Paint the image

	PaintImage(Dest, x, y, iTick, iRotation);

	if (m_pRotationOffset)
		{
		x += m_pRotationOffset[iRotation % m_iRotationCount].x;
		y -= m_pRotationOffset[iRotation % m_iRotationCount].y;
		}

	//	Make sure we have the glow image

	GenerateGlowImage(iRotation);

	//	Glow strength

	int iStrength = 64 + (4 * Absolute((iTick % 65) - 32));
	if (iStrength > 255)
		iStrength = 255;

	//	Paint the glow

	Dest.FillMask(0,
			0,
			RectWidth(m_rcImage) + 2 * GLOW_SIZE,
			RectHeight(m_rcImage) + 2 * GLOW_SIZE,
			m_pGlowImages[iRotation],
			CG32bitPixel(rgbGlowColor, (BYTE)iStrength),
			x - (RectWidth(m_rcImage) / 2) - GLOW_SIZE,
			y - (RectHeight(m_rcImage) / 2) - GLOW_SIZE);
	}

void CObjectImageArray::PaintRotatedImage (CG32bitImage &Dest,
										   int x,
										   int y,
										   int iTick,
										   int iRotation,
										   bool bComposite) const

//	PaintRotatedImage
//
//	Paint rotated image

	{
	if (m_pImage == NULL)
		return;

	CG32bitImage *pSource = m_pImage->GetRawImage(NULL_STR);
	if (pSource == NULL)
		return;

	int xSrc;
	int ySrc;
	ComputeSourceXY(iTick, 0, &xSrc, &ySrc);
	int cxSrc = RectWidth(m_rcImage);
	int cySrc = RectHeight(m_rcImage);

	//	Calculate the position of the upper-left corner of
	//	the rotated image.

#if 0
	CVector vUL(-cxSrc / 2, cySrc / 2);
	vUL = vUL.Rotate(iRotation);
#endif

	//	LATER: Since we have an actual rotation angle, we can calculate
	//	any rotation offset directly (instead of of using m_pRotationOffset)
	//	(But we are too lazy to do that now)

	ASSERT(m_pRotationOffset == NULL);

	//	Blt

	CGDraw::BltTransformed(Dest, x, y, 1.0, 1.0, iRotation, *pSource, xSrc, ySrc, cxSrc, cySrc);
	}

void CObjectImageArray::PaintScaledImage (CG32bitImage &Dest, int x, int y, int iTick, int iRotation, int cxWidth, int cyHeight, DWORD dwFlags) const

//	PaintScaledImage
//
//	Paint scaled image

	{
	if (m_pImage == NULL)
		return;

	//	Compute source

	CG32bitImage *pSrc;
	bool bScale;
	int xSrc, ySrc, cxSrc, cySrc;
	if (dwFlags & FLAG_CACHED)
		{
		GenerateScaledImages(iRotation, cxWidth, cyHeight);

		pSrc = &m_pScaledImages[iRotation];
		xSrc = 0;
		ySrc = 0;
		cxSrc = cxWidth;
		cySrc = cyHeight;
		bScale = false;
		}
	else
		{
		pSrc = &GetImage(NULL_STR);
		ComputeSourceXY(iTick, iRotation, &xSrc, &ySrc);
		cxSrc = RectWidth(m_rcImage);
		cySrc = RectHeight(m_rcImage);
		bScale = (cxSrc != cxWidth || cySrc != cyHeight);
		}

	//	If necessary we paint a scaled image

	if (bScale)
		{
		//	For scaled images, we need to specify the destination as the center
		//	of the image.

		int xDest = x;
		int yDest = y;
		if (dwFlags & FLAG_UPPER_LEFT)
			{
			xDest += (cxWidth / 2);
			yDest += (cyHeight / 2);
			}

		//	Paint

		if (dwFlags & FLAG_GRAYED)
			{
			CGDraw::BltTransformedGray(Dest,
					xDest,
					yDest,
					(Metric)cxWidth / (Metric)cxSrc,
					(Metric)cyHeight / (Metric)cySrc,
					0.0,
					*pSrc,
					xSrc,
					ySrc,
					cxSrc,
					cySrc,
					128);
			}
		else
			{
			CGDraw::BltTransformed(Dest,
					xDest,
					yDest,
					(Metric)cxWidth / (Metric)cxSrc,
					(Metric)cyHeight / (Metric)cySrc,
					0.0,
					*pSrc,
					xSrc,
					ySrc,
					cxSrc,
					cySrc);
			}
		}
	else
		{
		//	x,y is center of the image, unless otherwise specified.

		int xDest = x;
		int yDest = y;

		if (!(dwFlags & FLAG_UPPER_LEFT))
			{
			xDest -= (cxWidth / 2);
			yDest -= (cyHeight / 2);
			}

		//	Paint

		if (dwFlags & FLAG_GRAYED)
			CGDraw::BltGray(Dest, xDest, yDest, *pSrc, xSrc, ySrc, cxSrc, cySrc, 128);
		else
			Dest.Blt(xSrc, ySrc, cxSrc, cySrc, 255, *pSrc, xDest, yDest);
		}
	}

void CObjectImageArray::PaintSilhoutte (CG32bitImage &Dest,
										int x,
										int y,
										int iTick,
										int iRotation,
										CG32bitPixel rgbColor) const

//	PaintSilhouette
//
//	Paints a silhouette of the object

	{
	if (m_pImage)
		{
		CG32bitImage *pSource = m_pImage->GetRawImage(NULL_STR);
		if (pSource == NULL)
			return;

		int xSrc;
		int ySrc;
		ComputeSourceXY(iTick, iRotation, &xSrc, &ySrc);

		if (m_pRotationOffset)
			{
			x += m_pRotationOffset[iRotation % m_iRotationCount].x;
			y -= m_pRotationOffset[iRotation % m_iRotationCount].y;
			}

		Dest.FillMask(xSrc,
				ySrc,
				RectWidth(m_rcImage),
				RectHeight(m_rcImage),
				*pSource,
				rgbColor,
				x - (RectWidth(m_rcImage) / 2),
				y - (RectHeight(m_rcImage) / 2));
		}
	}

bool CObjectImageArray::PointInImage (int x, int y, int iTick, int iRotation) const

//	PointInImage
//
//	Returns TRUE if the given point is inside the masked part of the image
//	x, y is relative to the center of the image (GDI directions)

	{
	if (m_pImage)
		{
		//	Get the hit mask. If we don't have a specific hit mask, then we use
		//	the image.

		CG32bitImage *pSource = GetHitMask();
		if (pSource == NULL)
			return false;

		//	Compute the position of the frame

		int cxWidth = RectWidth(m_rcImage);
		int cyHeight = RectHeight(m_rcImage);
		int xSrc;
		int ySrc;
		ComputeSourceXY(iTick, iRotation, &xSrc, &ySrc);

		//	Adjust the point so that it is relative to the
		//	frame origin (upper left)

		x = xSrc + x + (cxWidth / 2);
		y = ySrc + y + (cyHeight / 2);

		//	Adjust for rotation

		if (m_pRotationOffset)
			{
			x -= m_pRotationOffset[iRotation % m_iRotationCount].x;
			y += m_pRotationOffset[iRotation % m_iRotationCount].y;
			}

		//	Check bounds

		if (x < xSrc || y < ySrc || x >= (xSrc + cxWidth) || y >= (ySrc + cyHeight)
				|| x < 0 || x >= pSource->GetWidth() || y < 0 || y >= pSource->GetHeight())
			return false;

		//	Check to see if the point is inside or outside the mask

		return (pSource->GetPixel(x, y).GetAlpha() >= 200);
		}
	else
		return false;
	}

bool CObjectImageArray::PointInImage (SPointInObjectCtx &Ctx, int x, int y) const

//	PointInImage
//
//	Returns TRUE if the given point is in the image

	{
	if (Ctx.pImage)
		{
		//	Adjust to image coords

		x = x + Ctx.xImageOffset;
		y = y + Ctx.yImageOffset;

		//	Check bounds

		if (x < Ctx.rcImage.left 
				|| y < Ctx.rcImage.top 
				|| x >= Ctx.rcImage.right 
				|| y >= Ctx.rcImage.bottom)
			return false;

		//	Check to see if the point is inside or outside the mask

		return (Ctx.pImage->GetPixel(x, y).GetAlpha() >= 200);
		}
	else
		return false;
	}

void CObjectImageArray::PointInImageInit (SPointInObjectCtx &Ctx, int iTick, int iRotation) const

//	PointInImageInit
//
//	Initializes the context to optimize the PointInImage call

	{
	if (m_pImage)
		{
		Ctx.pImage = GetHitMask();
		if (Ctx.pImage == NULL)
			return;

		//	Compute the position of the frame

		int cxWidth = RectWidth(m_rcImage);
		int cyHeight = RectHeight(m_rcImage);
		ComputeSourceXY(iTick, iRotation, &Ctx.rcImage.left, &Ctx.rcImage.top);
		Ctx.rcImage.right = Ctx.rcImage.left + cxWidth;
		Ctx.rcImage.bottom = Ctx.rcImage.top + cyHeight;

		//	Compute the offset required to convert to image coordinates

		Ctx.xImageOffset = Ctx.rcImage.left + (cxWidth / 2);
		Ctx.yImageOffset = Ctx.rcImage.top + (cyHeight / 2);

		//	Adjust for rotation

		if (m_pRotationOffset)
			{
			Ctx.xImageOffset -= m_pRotationOffset[iRotation % m_iRotationCount].x;
			Ctx.yImageOffset += m_pRotationOffset[iRotation % m_iRotationCount].y;
			}

		//	Make sure the image rect is inside the image

		if (Ctx.rcImage.left < 0 || Ctx.rcImage.right > Ctx.pImage->GetWidth()
				|| Ctx.rcImage.top < 0 || Ctx.rcImage.bottom > Ctx.pImage->GetHeight())
			{
			Ctx.pImage = NULL;
			return;
			}
		}
	}

void CObjectImageArray::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Read the object from a stream

	{
	Ctx.pStream->Read((char *)&m_dwBitmapUNID, sizeof(DWORD));
	if (m_dwBitmapUNID)
		m_pImage = Ctx.GetUniverse().FindLibraryImage(m_dwBitmapUNID);
	else
		m_pImage = NULL;
	Ctx.pStream->Read((char *)&m_rcImage, sizeof(RECT));
	Ctx.pStream->Read((char *)&m_iFrameCount, sizeof(DWORD));
	Ctx.pStream->Read((char *)&m_iTicksPerFrame, sizeof(DWORD));
	Ctx.pStream->Read((char *)&m_iFlashTicks, sizeof(DWORD));
	Ctx.pStream->Read((char *)&m_iBlending, sizeof(DWORD));

	if (Ctx.dwVersion >= 17)
		Ctx.pStream->Read((char *)&m_iRotationCount, sizeof(DWORD));
	else
		m_iRotationCount = STD_ROTATION_COUNT;

	if (Ctx.dwVersion >= 104)
		Ctx.pStream->Read((char *)&m_iFramesPerColumn, sizeof(DWORD));
	else
		m_iFramesPerColumn = m_iRotationCount;

	if (Ctx.dwVersion >= 114)
		Ctx.pStream->Read((char *)&m_iFramesPerRow, sizeof(DWORD));
	else
		m_iFramesPerRow = m_iFrameCount;

	if (Ctx.dwVersion >= 90)
		Ctx.pStream->Read((char *)&m_iViewportSize, sizeof(DWORD));
	else
		m_iViewportSize = RectWidth(m_rcImage);

	DWORD dwLoad;
	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	if (dwLoad)
		{
		m_pRotationOffset = new OffsetStruct [dwLoad];
		Ctx.pStream->Read((char *)m_pRotationOffset, dwLoad * sizeof(OffsetStruct));
		}

	m_bDefaultSize = false;
	}

void CObjectImageArray::SetImage (TSharedPtr<CObjectImage> pImage)

//	SetImage
//
//	Sets the image and takes ownership

	{
	m_pImage = pImage;
	m_dwBitmapUNID = 0;
	}

void CObjectImageArray::SetRotationCount (int iRotationCount)

//	SetRotationCount
//
//	Sets the rotation count

	{
	if (iRotationCount != m_iRotationCount)
		{
		m_iRotationCount = iRotationCount;
		m_iFramesPerColumn = iRotationCount;

		ComputeRotationOffsets();

		if (m_pGlowImages)
			{
			delete [] m_pGlowImages;
			m_pGlowImages = NULL;
			}

		if (m_pScaledImages)
			{
			delete [] m_pScaledImages;
			m_pScaledImages = NULL;
			m_cxScaledImage = -1;
			}
		}
	}

void CObjectImageArray::TakeHandoff (CObjectImageArray &Source)

//	TakeHandoff
//
//	Take ownership of the bitmap (and clear out the source)

	{
	CleanUp();

	//	Take ownership

	m_pImage = Source.m_pImage;
	Source.m_pImage = NULL;

	m_pGlowImages = Source.m_pGlowImages;
	Source.m_pGlowImages = NULL;

	m_pScaledImages = Source.m_pScaledImages;
	m_cxScaledImage = Source.m_cxScaledImage;
	Source.m_pScaledImages = NULL;
	Source.m_cxScaledImage = -1;

	m_pRotationOffset = Source.m_pRotationOffset;
	Source.m_pRotationOffset = NULL;

	//	Copy the remainder

	m_dwBitmapUNID = Source.m_dwBitmapUNID;
	m_rcImage = Source.m_rcImage;
	m_iFrameCount = Source.m_iFrameCount;
	m_iRotationCount = Source.m_iRotationCount;
	m_iFramesPerColumn = Source.m_iFramesPerColumn;
	m_iFramesPerRow = Source.m_iFramesPerRow;
	m_iTicksPerFrame = Source.m_iTicksPerFrame;
	m_iFlashTicks = Source.m_iFlashTicks;
	m_iBlending = Source.m_iBlending;
	m_iViewportSize = Source.m_iViewportSize;
	m_iRotationOffset = Source.m_iRotationOffset;
	m_bDefaultSize = Source.m_bDefaultSize;
	}

bool CObjectImageArray::ValidateImageSize (int cxWidth, int cyHeight) const

//	ValidateImageSize
//
//	Returns TRUE if the given image width and height are large enough to handle
//	all frames in this array.

	{
	//	Compute the image size based on the configuration.

	int cxRequired, cyRequired;
	CalcRequiredImageSize(cxRequired, cyRequired);

	//	Now make sure we have enough image

	if ((cxWidth >= cxRequired) && (cyHeight >= cyRequired))
		return true;

	//	Otherwise, log it.

    ::kernelDebugLogPattern("[0x%08x %s]: Image not large enough; %d x %d needed.", m_pImage->GetUNID(), m_pImage->GetImageFilename(), cxRequired, cyRequired);

	return false;
	}

void CObjectImageArray::WriteToStream (IWriteStream *pStream) const

//	WriteToStream
//
//	Writes the object to a stream
//
//	DWORD		m_dwBitmapUNID
//	Rect		m_rcImage
//	DWORD		m_iFrameCount
//	DWORD		m_iTicksPerFrame
//	DWORD		m_iFlashTicks
//	DWORD		m_iBlending
//	DWORD		m_iRotationCount
//	DWORD		m_iFramesPerColumns
//	DWORD		m_iFramesPerRow
//	DWORD		m_iViewportSize

//	DWORD		No of rotation offsets
//	DWORD		x
//	DWORD		y
//				...

	{
	pStream->Write((char *)&m_dwBitmapUNID, sizeof(DWORD));
	pStream->Write((char *)&m_rcImage, sizeof(m_rcImage));
	pStream->Write((char *)&m_iFrameCount, sizeof(DWORD));
	pStream->Write((char *)&m_iTicksPerFrame, sizeof(DWORD));
	pStream->Write((char *)&m_iFlashTicks, sizeof(DWORD));
	pStream->Write((char *)&m_iBlending, sizeof(DWORD));
	pStream->Write((char *)&m_iRotationCount, sizeof(DWORD));
	pStream->Write((char *)&m_iFramesPerColumn, sizeof(DWORD));
	pStream->Write((char *)&m_iFramesPerRow, sizeof(DWORD));
	pStream->Write((char *)&m_iViewportSize, sizeof(DWORD));

	if (m_pRotationOffset)
		{
		DWORD dwSave = m_iRotationCount;
		pStream->Write((char *)&dwSave, sizeof(DWORD));

		pStream->Write((char *)m_pRotationOffset, m_iRotationCount * sizeof(OffsetStruct));
		}
	else
		{
		DWORD dwSave = 0;
		pStream->Write((char *)&dwSave, sizeof(DWORD));
		}
	}
