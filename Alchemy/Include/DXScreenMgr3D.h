//	DXScreenMgr3D.h
//
//	Implements CScreenMgr3D class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

struct SScreenMgrOptions
	{
	HWND m_hWnd = NULL;

	bool m_bWindowedMode = false;
	bool m_bMultiMonitorMode = false;			//	If TRUE, screen spans all monitors

	bool m_bForceDX = false;
	bool m_bForceNonDX = false;
	bool m_bForceExclusiveMode = false;
	bool m_bForceNonExclusiveMode = false;
	bool m_bForceScreenSize = false;			//	If in exclusive mode, switch to desired res
	bool m_bNoGPUAcceleration = false;

	int m_cyMaxScreen = -1;						//	Scales screen if we're bigger than this (-1 = no scaling)
	int m_cxScreen = 1024;						//	Used if exclusive or windowed
	int m_cyScreen = 768;						//	Used if exclusive or windowed
	int m_iColorDepth = 16;						//	Used if exclusive

	bool m_bDebugVideo = false;
	};

struct SDXLayerCreate
	{
	int cxWidth = -1;							//	Width of layer
	int cyHeight = -1;							//	Height of layer
	int xPos = 0;
	int yPos = 0;
	int zPos = 0;								//	ZPos
	};

class CDXScreen
	{
	public:
		enum EFlags
			{
			//	Init

			FLAG_NO_TEXTURES =				0x00000001,
			FLAG_FORCE_GDI =				0x00000002,
			};

		CDXScreen (void);
		~CDXScreen (void) { CleanUp(); }

		void CleanUp (void);
		bool CreateLayer (const SDXLayerCreate &Create, int *retiLayerID, CString *retsError = NULL);
		void DebugOutputStats (void);
		inline CG32bitImage &GetLayerBuffer (int iLayerID) { return ((!IsReady() || m_bDeviceLost || iLayerID < 0 || iLayerID >= m_Layers.GetCount()) ? CG32bitImage::Null() : m_Layers[iLayerID].BackBuffer); }
		bool Init (HWND hWnd, int cxWidth, int cyHeight, DWORD dwFlags, CString *retsError = NULL);
		inline bool IsReady (void) const { return (m_bUseGDI || m_pD3DDevice); }
		inline bool IsUsingDirectX (void) const { return !m_bUseGDI; }
		inline bool IsUsingTextures (void) const { return m_bUseTextures; }
		void Render (void);
		void SwapBuffers (void);

	private:
		struct SLayer
			{
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

			int cxWidth = -1;
			int cyHeight = -1;
			int xPos = 0;
			int yPos = 0;
			int zPos = 0;

			IDirect3DVertexBuffer9 *pVertices = NULL;
			IDirect3DTexture9 *pTexture = NULL;
			IDirect3DTexture9 *pFrontBuffer = NULL;
			IDirect3DTexture9 *pBackBuffer = NULL;
			};

		bool BeginScene (void);
		void BltToSurface (const CG32bitImage &Src, IDirect3DSurface9 *pDest);
		inline bool CanUseDynamicTextures (void) const { return ((m_DeviceCaps.Caps2 & D3DCAPS2_DYNAMICTEXTURES) ? true : false); }
		bool CreateLayerResources (SLayer &Layer, CString *retsError = NULL);
		bool InitDevice (CString *retsError = NULL);
		bool InitLayerResources (void);
		bool Present (void);
		void RenderError (const CString &sError);
		bool ResetDevice (void);

		HWND m_hWnd = NULL;						//	Window
		IDirect3D9 *m_pD3D = NULL;				//	Used to create the D3DDevice
		IDirect3DDevice9 *m_pD3DDevice = NULL;	//	Our rendering device
		D3DPRESENT_PARAMETERS m_Present;		//	Present parameters
		D3DCAPS9 m_DeviceCaps;					//	Device caps for current device

		int m_cxTarget = 0;						//	Width of target surface (usually the screen)
		int m_cyTarget = 0;						//	Height of target surface

		int m_cxSource = 0;						//	Width of source image (usually layer size)
		int m_cySource = 0;

		bool m_bUseGDI = false;					//	Use GDI instead of Direct3D
		bool m_bNoGPUAcceleration = false;		//	Do not use textures, even if available
		bool m_bUseTextures = false;			//	Use textures for layers.
		bool m_bEndSceneNeeded = false;			//	If TRUE, we need an EndScene call
		bool m_bErrorReported = false;			//	If TRUE, we've already reported an error
		bool m_bDeviceLost = false;				//	If TRUE, we need to reset the device

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
		HANDLE m_hBackgroundThread = INVALID_HANDLE_VALUE;	//	Background thread
		HANDLE m_hWorkEvent = INVALID_HANDLE_VALUE;			//	Blt
		HANDLE m_hQuitEvent = INVALID_HANDLE_VALUE;			//	Time to quit
	};

class CScreenMgr3D
	{
	public:
		CScreenMgr3D (void);
		~CScreenMgr3D (void);

		bool CheckIsReady (void);
		void CleanUp (void);
		void ClientToLocal (int x, int y, int *retx, int *rety) const;
		void Flip (void);
		inline int GetHeight (void) const { return m_cyScreen; }
		inline bool GetInvalidRect (RECT *retrcRect) { retrcRect->left = 0; retrcRect->top = 0; retrcRect->right = m_cxScreen; retrcRect->bottom = m_cyScreen; return true; }
		inline CG32bitImage &GetScreen (void) { return m_DX.GetLayerBuffer(m_iDefaultLayer); }
		inline int GetWidth (void) const { return m_cxScreen; }
		void GlobalToLocal (int x, int y, int *retx, int *rety) const;
		ALERROR Init (SScreenMgrOptions &Options, CString *retsError);
		void Invalidate (void);
		void Invalidate (const RECT &rcRect);
		inline bool IsMinimized (void) const { return m_bMinimized; }
		void LocalToClient (int x, int y, int *retx, int *rety) const;
		void LocalToGlobal (int x, int y, int *retx, int *rety) const;
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

		HWND m_hWnd = NULL;
		CDXScreen m_DX;						//	DX screen object
		bool m_bWindowedMode = false;
		bool m_bMultiMonitorMode = false;
		bool m_bMinimized = false;
		bool m_bDebugVideo = false;

		//	Buffer

		int m_cxDevice = 0;					//	Size of screen device
		int m_cyDevice = 0;
		int m_cxScreen = 0;					//	Size of the buffer; device size may be different.
		int m_cyScreen = 0;
		Metric m_rScale = 1.0;				//	Downsample resolution for super hi-res screens (1.0 = normal)
		int m_iDefaultLayer = -1;

		CDXBackgroundBlt m_Blitter;			//	Background blitter.
	};
