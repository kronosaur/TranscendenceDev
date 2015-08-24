//	DXScreenMgr3D.h
//
//	Implements CScreenMgr3D class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

struct SDXLayerCreate
	{
	SDXLayerCreate (void) :
			cxWidth(-1),
			cyHeight(-1),
			xPos(0),
			yPos(0),
			zPos(0)
		{ }

	int cxWidth;					//	Width of layer
	int cyHeight;					//	Height of layer
	int xPos;
	int yPos;
	int zPos;						//	ZPos
	};

class CDXScreen
	{
	public:
		enum EFlags
			{
			//	Init

			FLAG_NO_TEXTURES =				0x00000001,
			};

		CDXScreen (void);
		~CDXScreen (void) { CleanUp(); }

		void CleanUp (void);
		bool CreateLayer (const SDXLayerCreate &Create, int *retiLayerID, CString *retsError = NULL);
		void DebugOutputStats (void);
		inline CG32bitImage &GetLayerBuffer (int iLayerID) { return m_Layers[iLayerID].BackBuffer; }
		bool Init (HWND hWnd, int cxWidth, int cyHeight, DWORD dwFlags, CString *retsError = NULL);
		inline bool IsUsingTextures (void) const { return m_bUseTextures; }
		void Render (void);
		void SwapBuffers (void);

	private:
		struct SLayer
			{
			SLayer (void) :
					pVertices(NULL),
					pTexture(NULL),
					pFrontBuffer(NULL),
					pBackBuffer(NULL)
				{ }

			~SLayer (void)
				{
				if (pVertices)
					pVertices->Release();

				if (pTexture)
					pTexture->Release();

				if (pFrontBuffer)
					pFrontBuffer->Release();

				if (pBackBuffer)
					pBackBuffer->Release();
				}

			CG32bitImage FrontBuffer;
			CG32bitImage BackBuffer;

			int cxWidth;
			int cyHeight;
			int xPos;
			int yPos;
			int zPos;

			IDirect3DVertexBuffer9 *pVertices;
			IDirect3DTexture9 *pTexture;
			IDirect3DTexture9 *pFrontBuffer;
			IDirect3DTexture9 *pBackBuffer;
			};

		void BltToSurface (const CG32bitImage &Src, IDirect3DSurface9 *pDest);
		void BltToSurface (IDirect3DTexture9 *pSrc, IDirect3DSurface9 *pDest);
		inline bool CanUseDynamicTextures (void) const { return ((m_DeviceCaps.Caps2 & D3DCAPS2_DYNAMICTEXTURES) ? true : false); }

		IDirect3D9 *m_pD3D;					//	Used to create the D3DDevice
		IDirect3DDevice9 *m_pD3DDevice;		//	Our rendering device
		D3DCAPS9 m_DeviceCaps;				//	Device caps for current device

		int m_cxTarget;						//	Width of target surface (usually the screen)
		int m_cyTarget;						//	Height of target surface

		int m_cxSource;						//	Width of source image (usually layer size)
		int m_cySource;

		bool m_bUseTextures;				//	Use textures for layers.

		TArray<SLayer> m_Layers;
		TSortMap<int, int> m_PaintOrder;
	};

class CDXBackgroundBlt
	{
	public:
		CDXBackgroundBlt (CDXScreen &DX);
		~CDXBackgroundBlt (void) { CleanUp(); }

		void CleanUp (void);
		bool Init (int cxWidth, int cyHeight, CString *retsError = NULL);
		inline bool IsEnabled (void) const { return (m_hBackgroundThread != INVALID_HANDLE_VALUE); }
		void Render (void);

	private:
		static DWORD WINAPI BackgroundThread (LPVOID pData);

		CDXScreen &m_DX;

		CCriticalSection m_cs;
		HANDLE m_hBackgroundThread;		//	Background thread
		HANDLE m_hWorkEvent;			//	Blt
		HANDLE m_hQuitEvent;			//	Time to quit
	};

class CScreenMgr3D
	{
	public:
		CScreenMgr3D (void);
		~CScreenMgr3D (void);

		bool CheckIsReady (void);
		void CleanUp (void);
		void ClientToScreen (int x, int y, int *retx, int *rety);
		void Flip (void);
		inline int GetHeight (void) const { return m_cyScreen; }
		inline bool GetInvalidRect (RECT *retrcRect) { retrcRect->left = 0; retrcRect->top = 0; retrcRect->right = m_cxScreen; retrcRect->bottom = m_cyScreen; return true; }
		inline CG32bitImage &GetScreen (void) { return m_DX.GetLayerBuffer(m_iDefaultLayer); }
		inline int GetWidth (void) const { return m_cxScreen; }
		ALERROR Init (SScreenMgrOptions &Options, CString *retsError);
		void Invalidate (void);
		void Invalidate (const RECT &rcRect);
		inline bool IsMinimized (void) const { return m_bMinimized; }
		void OnWMActivateApp (bool bActivate);
		void OnWMDisplayChange (int iBitDepth, int cxWidth, int cyHeight);
		void OnWMMove (int x, int y);
		void OnWMSize (int cxWidth, int cyHeight, int iSize);
		void Render (void);
		void StopDX (void);
		void Validate (void);

	private:
		void DebugOutputStats (void);

		//	DX state

		HWND m_hWnd;
		CDXScreen m_DX;					//	DX screen object
		bool m_bWindowedMode;
		bool m_bMultiMonitorMode;
		bool m_bMinimized;
		bool m_bDebugVideo;

		//	Buffer

		int m_cxScreen;					//	Size of the buffer; screen size may be different.
		int m_cyScreen;
		Metric m_rScale;				//	Downsample resolution for super hi-res screens (1.0 = normal)
		int m_iDefaultLayer;

		CDXBackgroundBlt m_Blitter;		//	Background blitter.
	};
