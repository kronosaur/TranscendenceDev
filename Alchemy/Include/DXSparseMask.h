//	DXSparseMask.h
//
//	TSparseMask Template
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CG8bitSparseImage : public TImagePlane<CG8bitSparseImage>
	{
	public:
		CG8bitSparseImage (void);
		CG8bitSparseImage (const CG8bitSparseImage &Src);
		~CG8bitSparseImage (void);

		CG8bitSparseImage &operator= (const CG8bitSparseImage &Src);

		//	Basic Interface

		bool Create (int cxWidth, int cyHeight, BYTE InitialValue = 0);
		BYTE GetPixel (int x, int y) const;
		bool IsEmpty (void) const { return (m_Tiles.GetType() == typeByte); }

		//	Basic Drawing Interface

		void Fill (int x, int y, int cxWidth, int cyHeight, BYTE Value);
		void FillLine (int x, int y, int cxWidth, BYTE *pValues);
		void MaskFill (CG32bitImage &Dest, int xDest, int yDest, int cxDest, int cyDest, int xSrc, int ySrc, CG32bitPixel rgbColor) const;
		void SetPixel (int x, int y, BYTE Value);

	private:
		enum ETypes
			{
			typeNodeArray =		0x00000000,
			typeByte =			0x00000001,
			typeByteArray =		0x00000002,

			TYPE_MASK =			0x00000003,
			};

		class CNode
			{
			public:
				CNode (void) : m_pItems(EncodeByte(0))
					{ }

				~CNode (void)
					{
					FreeAll();
					}

				BYTE GetByte (void) const { return DecodeByte(m_pItems); }
				BYTE *GetByteArray (void) const { return DecodeByteArray(m_pItems); }
				void *GetCode (void) const { return m_pItems; }
				CNode *GetNodeArray (void) const { return DecodeNodeArray(m_pItems); }
				ETypes GetType (void) const { return DecodeType(m_pItems); }

				void SetByte (BYTE Value)
					{
					FreeAll();
					m_pItems = EncodeByte(Value);
					}

				BYTE *SetByteArray (int iCount)
					{
					FreeAll();
					m_pItems = EncodeByteArray(new BYTE [iCount]);
					return GetByteArray();
					}

				CNode *SetNodeArray (int iCount)
					{
					FreeAll();
					m_pItems = EncodeNodeArray(new CNode [iCount]);
					return GetNodeArray();
					}

			private:
				void FreeAll (void)
					{
					switch (GetType())
						{
						case typeByteArray:
							delete [] GetByteArray();
							break;

						case typeNodeArray:
							delete [] GetNodeArray();
							break;
						}
					}

				//	m_pItems is one of the following, depending on the type
				//	(which is encode in the lower 2-bits).
				//
				//	0: A pointer to an array of CNode
				//	1: A constant BYTE value
				//	2: A pointer to an array of BYTEs.

				void *m_pItems;
			};

		void CleanUp (void);
		void Copy (const CG8bitSparseImage &Src);
		const CNode &GetTile (int x, int y) const;
		static void *EncodeByte (BYTE Value) { DWORD dwEncoded = ((DWORD)Value << 24) | typeByte; return (void *)dwEncoded; }
		static void *EncodeByteArray (BYTE *pValue) { DWORD dwEncoded = ((DWORD)pValue) | typeByteArray; return (void *)dwEncoded; }
		static void *EncodeNodeArray (CNode *pValue) { DWORD dwEncoded = ((DWORD)pValue) | typeNodeArray; return (void *)dwEncoded; }
		static BYTE DecodeByte (void *pCode) { return (BYTE)((DWORD)pCode >> 24); }
		static BYTE *DecodeByteArray (void *pCode) { return (BYTE *)((DWORD)pCode & ~TYPE_MASK); }
		static CNode *DecodeNodeArray (void *pCode) { return (CNode *)((DWORD)pCode & ~TYPE_MASK); }
		static ETypes DecodeType (void *pCode) { return (ETypes)((DWORD)pCode & TYPE_MASK); }
		CNode &SetTileAt (int x, int y);
		CNode *SetTileRowsAt (int x, int y);

		CNode m_Tiles;
		int m_cxTile;
		int m_cyTile;
		int m_xTileCount;
		int m_yTileCount;
	};

