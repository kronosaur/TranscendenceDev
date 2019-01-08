//	CG8bitSparseImage.cpp
//
//	CG8bitSparseImage Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CG8bitSparseImage::CG8bitSparseImage (void) :
		m_cxTile(1024),
		m_cyTile(1024),
		m_xTileCount(0),
		m_yTileCount(0)

//	CG8bitSparseImage constructor

	{
	}

CG8bitSparseImage::CG8bitSparseImage (const CG8bitSparseImage &Src)

//	CG8bitSparseImage constructor

	{
	Copy(Src);
	}

CG8bitSparseImage::~CG8bitSparseImage (void)

//	CG8bitSparseImage destructor

	{
	}

CG8bitSparseImage &CG8bitSparseImage::operator= (const CG8bitSparseImage &Src)

//	CG8bitSparseImage operator =

	{
	CleanUp();
	Copy(Src);
	return *this;
	}

void CG8bitSparseImage::CleanUp (void)

//	CleanUp
//
//	Free all structures and restore to initial (empty) state.

	{
	m_Tiles.SetByte(0);
	m_xTileCount = 0;
	m_yTileCount = 0;
	}

void CG8bitSparseImage::Copy (const CG8bitSparseImage &Src)

//	Copy
//
//	Copy

	{
	int i, j;

	switch (Src.m_Tiles.GetType())
		{
		case typeByte:
			m_Tiles.SetByte(Src.m_Tiles.GetByte());
			m_xTileCount = 0;
			m_yTileCount = 0;
			break;

		case typeNodeArray:
			{
			int iTileCount = Src.m_xTileCount * Src.m_yTileCount;
			CNode *SrcNodes = Src.m_Tiles.GetNodeArray();

			m_Tiles.SetNodeArray(iTileCount);
			CNode *DestNodes = m_Tiles.GetNodeArray();

			for (i = 0; i < iTileCount; i++)
				{
				switch (SrcNodes[i].GetType())
					{
					//	The entire tile is a single value.

					case typeByte:
						DestNodes[i].SetByte(SrcNodes[i].GetByte());
						break;

					//	The tile is an array of rows, each of which may be 
					//	either a BYTE array or a value.

					case typeNodeArray:
						{
						CNode *SrcRows = SrcNodes[i].GetNodeArray();

						DestNodes[i].SetNodeArray(Src.m_cyTile);
						CNode *DestRows = DestNodes[i].GetNodeArray();

						for (j = 0; j < Src.m_cyTile; j++)
							{
							switch (SrcRows[j].GetType())
								{
								case typeByte:
									DestRows[j].SetByte(SrcRows[j].GetByte());
									break;

								case typeByteArray:
									{
									DestRows[j].SetByteArray(m_cxTile);
									utlMemCopy((char *)DestRows[j].GetByteArray(), (char *)SrcRows[j].GetByteArray(), m_cxTile);
									break;
									}

								default:
									ASSERT(false);
								}
							}

						break;
						}

					default:
						ASSERT(false);
					}
				}

			m_xTileCount = Src.m_xTileCount;
			m_yTileCount = Src.m_yTileCount;
			break;
			}

		default:
			ASSERT(false);
		}

	m_cxTile = Src.m_cxTile;
	m_cyTile = Src.m_cyTile;

	m_cxWidth = Src.m_cxWidth;
	m_cyHeight = Src.m_cyHeight;
	m_rcClip = Src.m_rcClip;
	}

bool CG8bitSparseImage::Create (int cxWidth, int cyHeight, BYTE InitialValue)

//	Create
//
//	Create an images of the given size and initial value

	{
	//	Compute the number of tiles that we need for this image

	m_xTileCount = AlignUp(cxWidth, m_cxTile) / m_cxTile;
	m_yTileCount = AlignUp(cyHeight, m_cyTile) / m_cyTile;

	//	Set the initial value

	m_Tiles.SetByte(InitialValue);

	//	Done

	m_cxWidth = cxWidth;
	m_cyHeight = cyHeight;
	ResetClipRect();

	return true;
	}

BYTE CG8bitSparseImage::GetPixel (int x, int y) const

//	GetPixel
//
//	Returns the pixel at the given coordinates

	{
	return 0;
	}

const CG8bitSparseImage::CNode &CG8bitSparseImage::GetTile (int x, int y) const

//	GetTile
//
//	Returns the tile at this position. We assume that we're in range

	{
	ASSERT(x >= 0 && x < m_xTileCount && y >= 0 && y < m_yTileCount);

	if (m_Tiles.GetType() == typeByte)
		return m_Tiles;

	return m_Tiles.GetNodeArray()[y * m_xTileCount + x];
	}

void CG8bitSparseImage::Fill (int x, int y, int cxWidth, int cyHeight, BYTE Value)

//	Fill
//
//	Fills a rect

	{
	//	Make sure we're in bounds

	if (!AdjustCoords(NULL, NULL, 0, 0, 
			&x, &y, 
			&cxWidth, &cyHeight))
		return;

	int yPos = y;
	int yPosEnd = y + cyHeight;
	int yTile = yPos / m_cyTile;
	int yOffset = ClockMod(yPos, m_cyTile);
	int xTileStart = x / m_cxTile;
	int xOffsetStart = ClockMod(x, m_cxTile);
	while (yPos < yPosEnd)
		{
		int cyTileLeft = Min(m_cyTile - yOffset, yPosEnd - yPos);

		//	Is this a valid tile row, then continue

		if (yTile >= 0 && yTile < m_yTileCount)
			{
			int xPos = x;
			int xPosEnd = x + cxWidth;
			int xTile = xTileStart;
			int xOffset = xOffsetStart;
			while (xPos < xPosEnd)
				{
				int cxTileLeft = Min(m_cxTile - xOffset, xPosEnd - xPos);

				//	If this is a valid tile, then we can continue

				if (xTile >= 0 && xTile < m_xTileCount)
					{
					//	If we're covering the whole tile, then we set it to the
					//	proper value.

					if (cyTileLeft == m_cyTile && cxTileLeft == m_cxTile)
						SetTileAt(xTile, yTile).SetByte(Value);

					//	Otherwise we need to make sure we have an array of
					//	rows.

					else
						{
						int yRow;
						int yRowEnd = yOffset + cyTileLeft;

						CNode *Rows = SetTileRowsAt(xTile, yTile);

						//	If we're covering the whole row, then we set the 
						//	proper value for all rows in our range

						if (cxTileLeft == m_cxTile)
							{
							for (yRow = yOffset; yRow < yRowEnd; yRow++)
								Rows[yRow].SetByte(Value);
							}

						//	Otherwise we need to blt each row

						else
							{
							for (yRow = yOffset; yRow < yRowEnd; yRow++)
								{
								if (Rows[yRow].GetType() == typeByte)
									{
									BYTE OldValue = Rows[yRow].GetByte();
									BYTE *pRow = Rows[yRow].SetByteArray(m_cxTile);
									BYTE *pRowEnd = pRow + m_cxTile;

									BYTE *pFillStart = pRow + xOffset;
									while (pRow < pFillStart)
										*pRow++ = OldValue;

									BYTE *pFillEnd = pFillStart + cxTileLeft;
									while (pRow < pFillEnd)
										*pRow++ = Value;

									while (pRow < pRowEnd)
										*pRow++ = OldValue;
									}
								else
									{
									BYTE *pFill = Rows[yRow].GetByteArray() + xOffset;
									BYTE *pFillEnd = pFill + cxTileLeft;
									while (pFill < pFillEnd)
										*pFill++ = Value;
									}
								}
							}
						}
					}

				//	Move to the next tile

				xTile++;
				xOffset = 0;
				xPos += cxTileLeft;
				}
			}

		//	Next tile row

		yTile++;
		yOffset = 0;
		yPos += cyTileLeft;
		}
	}

void CG8bitSparseImage::FillLine (int x, int y, int cxWidth, BYTE *pValues)

//	FillLine
//
//	Fills a line with the given values and combines them using a blend 
//	operation.

	{
	//	Make sure we're in bounds

	if (y < m_rcClip.top || y >= m_rcClip.bottom)
		return;

	if (x < m_rcClip.left)
		{
		int xDiff = m_rcClip.left - x;
		if (xDiff >= cxWidth)
			return;

		x = m_rcClip.left;
		cxWidth -= xDiff;
		pValues += xDiff;
		}

	if (x + cxWidth > m_rcClip.right)
		{
		cxWidth = m_rcClip.right - x;
		if (cxWidth <= 0)
			return;
		}

	//	Compute tiles

	int yTile = y / m_cyTile;
	int yOffset = ClockMod(y, m_cyTile);

	int xTileStart = x / m_cxTile;
	int xOffsetStart = ClockMod(x, m_cxTile);

	//	Is this not a valid tile row, then skip

	if (yTile < 0 || yTile >= m_yTileCount)
		return;

	//	Loop over all tiles in the row

	int xPos = x;
	int xPosEnd = x + cxWidth;
	int xTile = xTileStart;
	int xOffset = xOffsetStart;
	while (xPos < xPosEnd)
		{
		int cxTileLeft = Min(m_cxTile - xOffset, xPosEnd - xPos);

		//	If this is a valid tile, then we can continue

		if (xTile >= 0 && xTile < m_xTileCount)
			{
			CNode *Rows = SetTileRowsAt(xTile, yTile);
			int yRow = yOffset;

			if (Rows[yRow].GetType() == typeByte)
				{
				BYTE OldValue = Rows[yRow].GetByte();
				BYTE *pRow = Rows[yRow].SetByteArray(m_cxTile);
				BYTE *pRowEnd = pRow + m_cxTile;

				BYTE *pFillStart = pRow + xOffset;
				while (pRow < pFillStart)
					*pRow++ = OldValue;

				BYTE *pSrc = pValues;
				BYTE *pFillEnd = pFillStart + cxTileLeft;
				while (pRow < pFillEnd)
					{
					*pRow++ = (BYTE)((WORD)OldValue * (WORD)(*pSrc) / 255);
					pSrc++;
					}

				while (pRow < pRowEnd)
					*pRow++ = OldValue;
				}
			else
				{
				BYTE *pSrc = pValues;
				BYTE *pFill = Rows[yRow].GetByteArray() + xOffset;
				BYTE *pFillEnd = pFill + cxTileLeft;

				while (pFill < pFillEnd)
					{
					*pFill++ = (BYTE)((WORD)(*pFill) * (WORD)(*pSrc) / 255);
					pSrc++;
					}
				}
			}

		//	Move to the next tile

		xTile++;
		xOffset = 0;
		xPos += cxTileLeft;
		pValues += cxTileLeft;
		}
	}

void CG8bitSparseImage::MaskFill (CG32bitImage &Dest, int xDest, int yDest, int cxDest, int cyDest, int xSrc, int ySrc, CG32bitPixel rgbColor) const

//	MaskFill
//
//	Fills the destination image with the given color through our mask.

	{
	//	Make sure we're in bounds

	if (!Dest.AdjustCoords(&xSrc, &ySrc, GetWidth(), GetHeight(), 
			&xDest, &yDest,
			&cxDest, &cyDest))
		return;

	//	Short-circuit

	if (rgbColor.GetAlpha() == 0x00)
		return;

	//	Compute a pre-multiplied color

	BYTE *pAlpha = CG32bitPixel::AlphaTable(rgbColor.GetAlpha());
	CG32bitPixel rgbPreMultColor = CG32bitPixel(pAlpha[rgbColor.GetRed()], pAlpha[rgbColor.GetGreen()], pAlpha[rgbColor.GetBlue()], rgbColor.GetAlpha());

	//	Loop

	int yPos = ySrc;
	int yPosEnd = ySrc + cyDest;
	int yTile = yPos / m_cyTile;
	int yOffset = ClockMod(yPos, m_cyTile);
	int xTileStart = xSrc / m_cxTile;
	int xOffsetStart = ClockMod(xSrc, m_cxTile);

	int yDestPos = yDest;
	while (yPos < yPosEnd)
		{
		int cyTileLeft = Min(m_cyTile - yOffset, yPosEnd - yPos);

		//	Is this a valid tile row, then continue

		if (yTile >= 0 && yTile < m_yTileCount)
			{
			int xPos = xSrc;
			int xPosEnd = xSrc + cxDest;
			int xTile = xTileStart;
			int xOffset = xOffsetStart;

			int xDestPos = xDest;
			while (xPos < xPosEnd)
				{
				int cxTileLeft = Min(m_cxTile - xOffset, xPosEnd - xPos);

				//	If this is a valid tile, then we can continue

				if (xTile >= 0 && xTile < m_xTileCount)
					{
					const CNode &Tile = GetTile(xTile, yTile);

					switch (Tile.GetType())
						{
						case typeByte:
							{
							BYTE byAlpha = Tile.GetByte();
							if (byAlpha == 0x00)
								{}
							else if (byAlpha == 0xff)
								Dest.Fill(xDestPos, yDestPos, cxTileLeft, cyTileLeft, rgbColor);
							else
								//	We just modify the alpha on the original color (Fill will take care of the rest)
								Dest.Fill(xDestPos, yDestPos, cxTileLeft, cyTileLeft, CG32bitPixel(rgbColor, CG32bitPixel::BlendAlpha(rgbColor.GetAlpha(), byAlpha)));
							break;
							}

						case typeNodeArray:
							{
							CNode *Rows = Tile.GetNodeArray();
							CNode *pSrcRow = Rows + yOffset;
							CNode *pSrcRowEnd = pSrcRow + cyTileLeft;

							CG32bitPixel *pDestRow = Dest.GetPixelPos(xDestPos, yDestPos);

							while (pSrcRow < pSrcRowEnd)
								{
								CG32bitPixel *pDest = pDestRow;
								CG32bitPixel *pDestEnd = pDestRow + cxTileLeft;

								switch (pSrcRow->GetType())
									{
									case typeByte:
										{
										BYTE byAlpha = pSrcRow->GetByte();
										if (byAlpha == 0x00)
											{}
										else if (byAlpha == 0xff)
											{
											BYTE byFillAlpha = rgbColor.GetAlpha();
											if (byFillAlpha == 0xff)
												{
												while (pDest < pDestEnd)
													*pDest++ = rgbColor;
												}
											else
												{
												BYTE *pAlphaInv = CG32bitPixel::AlphaTable(byFillAlpha ^ 0xff);

												BYTE byRed = rgbPreMultColor.GetRed();
												BYTE byGreen = rgbPreMultColor.GetGreen();
												BYTE byBlue = rgbPreMultColor.GetBlue();

												while (pDest < pDestEnd)
													{
													BYTE byRedResult = pAlphaInv[pDest->GetRed()] + byRed;
													BYTE byGreenResult = pAlphaInv[pDest->GetGreen()] + byGreen;
													BYTE byBlueResult = pAlphaInv[pDest->GetBlue()] + byBlue;

													*pDest++ = CG32bitPixel(byRedResult, byGreenResult, byBlueResult);
													}
												}
											}
										else
											{
											//	Blend the alphas together
											BYTE byFillAlpha = CG32bitPixel::BlendAlpha(byAlpha, rgbColor.GetAlpha());

											BYTE *pAlpha = CG32bitPixel::AlphaTable(byFillAlpha);
											BYTE *pAlphaInv = CG32bitPixel::AlphaTable(byFillAlpha ^ 0xff);

											BYTE byRed = pAlpha[rgbColor.GetRed()];
											BYTE byGreen = pAlpha[rgbColor.GetBlue()];
											BYTE byBlue = pAlpha[rgbColor.GetGreen()];

											while (pDest < pDestEnd)
												{
												BYTE byRedResult = pAlphaInv[pDest->GetRed()] + byRed;
												BYTE byGreenResult = pAlphaInv[pDest->GetGreen()] + byGreen;
												BYTE byBlueResult = pAlphaInv[pDest->GetBlue()] + byBlue;

												*pDest++ = CG32bitPixel(byRedResult, byGreenResult, byBlueResult);
												}
											}
										break;
										}

									case typeByteArray:
										{
										BYTE *pSrcAlpha = pSrcRow->GetByteArray() + xOffset;
										BYTE *pSrcAlphaEnd = pSrcAlpha + cxTileLeft;

										BYTE byColorAlpha = rgbColor.GetAlpha();
										if (byColorAlpha == 0xff)
											{
											while (pSrcAlpha < pSrcAlphaEnd)
												{
												if (*pSrcAlpha == 0x00)
													{}
												else if (*pSrcAlpha == 0xff)
													*pDest = rgbColor;
												else
													{
													BYTE *pAlpha = CG32bitPixel::AlphaTable(*pSrcAlpha);
													BYTE *pAlphaInv = CG32bitPixel::AlphaTable(*pSrcAlpha ^ 0xff);

													BYTE byRedResult = pAlphaInv[pDest->GetRed()] + pAlpha[rgbColor.GetRed()];
													BYTE byGreenResult = pAlphaInv[pDest->GetGreen()] + pAlpha[rgbColor.GetGreen()];
													BYTE byBlueResult = pAlphaInv[pDest->GetBlue()] + pAlpha[rgbColor.GetBlue()];

													*pDest = CG32bitPixel(byRedResult, byGreenResult, byBlueResult);
													}

												pSrcAlpha++;
												pDest++;
												}
											}
										else
											{
											BYTE *pAlphaFFInv = CG32bitPixel::AlphaTable(byColorAlpha ^ 0xff);

											while (pSrcAlpha < pSrcAlphaEnd)
												{
												if (*pSrcAlpha == 0x00)
													{}
												else if (*pSrcAlpha == 0xff)
													{
													BYTE byRedResult = pAlphaFFInv[pDest->GetRed()] + rgbPreMultColor.GetRed();
													BYTE byGreenResult = pAlphaFFInv[pDest->GetGreen()] + rgbPreMultColor.GetGreen();
													BYTE byBlueResult = pAlphaFFInv[pDest->GetBlue()] + rgbPreMultColor.GetBlue();

													*pDest = CG32bitPixel(byRedResult, byGreenResult, byBlueResult);
													}
												else
													{
													BYTE byFillAlpha = CG32bitPixel::BlendAlpha(*pSrcAlpha, byColorAlpha);

													BYTE *pAlpha = CG32bitPixel::AlphaTable(byFillAlpha);
													BYTE *pAlphaInv = CG32bitPixel::AlphaTable(byFillAlpha ^ 0xff);

													BYTE byRedResult = pAlphaInv[pDest->GetRed()] + pAlpha[rgbColor.GetRed()];
													BYTE byGreenResult = pAlphaInv[pDest->GetGreen()] + pAlpha[rgbColor.GetGreen()];
													BYTE byBlueResult = pAlphaInv[pDest->GetBlue()] + pAlpha[rgbColor.GetBlue()];

													*pDest = CG32bitPixel(byRedResult, byGreenResult, byBlueResult);
													}

												pSrcAlpha++;
												pDest++;
												}
											}
										break;
										}
									}

								pSrcRow++;
								pDestRow = Dest.NextRow(pDestRow);
								}

							break;
							}
						}
					}

				//	Move to the next tile

				xTile++;
				xOffset = 0;
				xPos += cxTileLeft;
				xDestPos += cxTileLeft;
				}
			}

		//	Next tile row

		yTile++;
		yOffset = 0;
		yPos += cyTileLeft;
		yDestPos += cyTileLeft;
		}
	}

void CG8bitSparseImage::SetPixel (int x, int y, BYTE Value)

//	SetPixel
//
//	Sets a pixel

	{
	}

CG8bitSparseImage::CNode &CG8bitSparseImage::SetTileAt (int x, int y)

//	SetTileAt
//
//	Converts to a tile array and returns the tile at the given coordinates.

	{
	int i;
	ASSERT(x >= 0 && x < m_xTileCount && y >= 0 && y < m_yTileCount);

	CNode *Tiles;
	if (m_Tiles.GetType() == typeByte)
		{
		BYTE Value = m_Tiles.GetByte();
		int iTileCount = m_xTileCount * m_yTileCount;
		Tiles = m_Tiles.SetNodeArray(iTileCount);
		for (i = 0; i < iTileCount; i++)
			Tiles[i].SetByte(Value);
		}
	else
		{
		ASSERT(m_Tiles.GetType() == typeNodeArray);
		Tiles = m_Tiles.GetNodeArray();
		}

	return Tiles[y * m_xTileCount + x];
	}

CG8bitSparseImage::CNode *CG8bitSparseImage::SetTileRowsAt (int x, int y)

//	SetTileRowsAt
//
//	Makes sure the given tile is an array of rows and returns a pointer to the
//	row array.

	{
	int i;
	ASSERT(x >= 0 && x < m_xTileCount && y >= 0 && y < m_yTileCount);

	CNode &Tile = SetTileAt(x, y);

	if (Tile.GetType() == typeByte)
		{
		BYTE Value = Tile.GetByte();

		CNode *Rows = Tile.SetNodeArray(m_cyTile);
		for (i = 0; i < m_cyTile; i++)
			Rows[i].SetByte(Value);

		return Rows;
		}
	
	return Tile.GetNodeArray();
	}
