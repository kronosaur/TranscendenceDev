//	CDXScreen.cpp
//
//	CDXScreen class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include <windows.h>
#include <dsound.h>
#include "Alchemy.h"
#include "DirectXUtil.h"
#include "DX3DImpl.h"

#pragma comment(lib, "d3d9.lib")

CDXScreen::CDXScreen (void) :
		m_pD3D(NULL),
		m_pD3DDevice(NULL)

//	CDXScreen constructor

	{
	}

void CDXScreen::CleanUp (void)

//	CleanUp
//
//	Clean up the class
	
	{
	m_Layers.DeleteAll();
	m_PaintOrder.DeleteAll();

    if (m_pD3DDevice != NULL)
		{
        m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
		}

    if (m_pD3D != NULL)
		{
        m_pD3D->Release();
		m_pD3D = NULL;
		}
	}

bool CDXScreen::CreateLayer (const SDXLayerCreate &Create, int *retiLayerID, CString *retsError)

//	CreateLayer
//
//	Creates a paint layer, which will be composited with all the other layers.

	{
	//	Check some parameters

	if (Create.cxWidth <= 0 || Create.cyHeight <= 0)
		{
		ASSERT(false);
		if (retsError) *retsError = CONSTLIT("Invalid layer size.");
		return false;
		}

	//	Optimistically insert a new layer. We always add layers to the end of 
	//	our array, do we know the ID.

	int iID = m_Layers.GetCount();
	SLayer *pLayer = m_Layers.Insert();
	pLayer->cxWidth = Create.cxWidth;
	pLayer->cyHeight = Create.cyHeight;
	pLayer->xPos = Create.xPos;
	pLayer->yPos = Create.yPos;
	pLayer->zPos = Create.zPos;

	//	Create the vertices

	if (FAILED(m_pD3DDevice->CreateVertexBuffer(4 * sizeof(SVertexFormatStd), 
			D3DUSAGE_WRITEONLY,
			D3DFVF_VERTEXFORMAT_STD, 
			D3DPOOL_MANAGED, 
			&pLayer->pVertices, 
			NULL)))
		{
		if (retsError) *retsError = CONSTLIT("Unable to create vertex buffer.");
		return false;
		}

	//	Initialize vertices

	SVertexFormatStd *pVertexList;
	if (FAILED(pLayer->pVertices->Lock(0, 4 * sizeof(SVertexFormatStd), (void **)&pVertexList, 0)))
		{
		m_Layers.Delete(iID);
		if (retsError) *retsError = CONSTLIT("Unable to lock vertex buffer.");
		return false;
		}

	//	Set all the colors to white

	pVertexList[0].color = pVertexList[1].color = pVertexList[2].color = pVertexList[3].color = 0xffffffff;

	//	Set positions and texture coordinates

	pVertexList[0].x = pVertexList[3].x = -Create.cxWidth / 2.0f;
	pVertexList[1].x = pVertexList[2].x = Create.cxWidth / 2.0f;

	pVertexList[0].y = pVertexList[1].y = Create.cyHeight / 2.0f;
	pVertexList[2].y = pVertexList[3].y = -Create.cyHeight / 2.0f;

	pVertexList[0].z = pVertexList[1].z = pVertexList[2].z = pVertexList[3].z = 1.0f;

	pVertexList[1].u = pVertexList[2].u = 1.0f;
	pVertexList[0].u = pVertexList[3].u = 0.0f;

	pVertexList[0].v = pVertexList[1].v = 0.0f;
	pVertexList[2].v = pVertexList[3].v = 1.0f;

	pLayer->pVertices->Unlock();

	//	Create a couple of textures

	if (FAILED(m_pD3DDevice->CreateTexture(Create.cxWidth, 
			Create.cyHeight, 
			1,
			D3DUSAGE_DYNAMIC,
			D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT,
			&pLayer->pTexture,
			NULL)))
		{
		m_Layers.Delete(iID);
		if (retsError) *retsError = CONSTLIT("Unable to create texture.");
		return false;
		}

	if (FAILED(m_pD3DDevice->CreateTexture(Create.cxWidth, 
			Create.cyHeight, 
			1,
			D3DUSAGE_DYNAMIC,
			D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT,
			&pLayer->pBackBuffer,
			NULL)))
		{
		m_Layers.Delete(iID);
		if (retsError) *retsError = CONSTLIT("Unable to create texture.");
		return false;
		}

	//	Create an image object that paints to the backbuffer texture. We leave
	//	the backbuffer locked until we swap.

	D3DLOCKED_RECT Locked;
	if (FAILED(pLayer->pBackBuffer->LockRect(0, &Locked, NULL, D3DLOCK_DISCARD)))
		{
		m_Layers.Delete(iID);
		if (retsError) *retsError = CONSTLIT("Unable to lock texture.");
		return false;
		}

	pLayer->BackBuffer.CreateFromExternalBuffer(Locked.pBits, Create.cxWidth, Create.cyHeight, Locked.Pitch);

	//	Now that we've succeeded, add it to the paint order

	m_PaintOrder.Insert(pLayer->zPos, iID);

	//	Done

	if (retiLayerID)
		*retiLayerID = iID;

	return true;
	}

bool CDXScreen::Init (HWND hWnd, int cxWidth, int cyHeight, CString *retsError)

//	Init
//
//	Initialize

	{
    //	Create the D3D object, which is needed to create the D3DDevice.

    if ((m_pD3D = ::Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		{
		if (retsError) *retsError = CONSTLIT("Unable to create D3D object.");
        return false;
		}

    //	Set up the structure used to create the D3DDevice. Most parameters are
    //	zeroed out. We set Windowed to TRUE, since we want to do D3D in a
    //	window, and then set the SwapEffect to "discard", which is the most
    //	efficient method of presenting the back buffer to the display.  And 
    //	we request a back buffer format that matches the current desktop display 
    //	format.

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof( d3dpp ) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

    //	Create the Direct3D device. Here we are using the default adapter (most
    //	systems only have one, unless they have multiple graphics hardware cards
    //	installed) and requesting the HAL (which is saying we want the hardware
    //	device rather than a software one). Software vertex processing is 
    //	specified since we know it will work on all cards. On cards that support 
    //	hardware vertex processing, though, we would see a big performance gain 
    //	by specifying hardware vertex processing.

    if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
            D3DCREATE_SOFTWARE_VERTEXPROCESSING,
            &d3dpp,
			&m_pD3DDevice)))
	    {
		if (retsError) *retsError = CONSTLIT("Unable to create D3D device.");
        return false;
		}

	//	Set up our scene

    CDXMatrix Identity;
	Identity.SetIdentity();

	CDXMatrix Ortho2D;
	Ortho2D.SetOrthoLH((float)cxWidth, (float)cyHeight, 0.0f, 1.0f);
    
    m_pD3DDevice->SetTransform(D3DTS_PROJECTION, Ortho2D);
    m_pD3DDevice->SetTransform(D3DTS_WORLD, Identity);
    m_pD3DDevice->SetTransform(D3DTS_VIEW, Identity);

	//	No lighting needed

	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	//	Done

	return true;
	}

void CDXScreen::LayerUpdateTexture (SLayer &Layer)

//	LayerUpdateTexture
//
//	Updates the texture from the layer's image

	{
#if 0
	D3DLOCKED_RECT Locked;
	if (FAILED(Layer.pTexture->LockRect(0, &Locked, NULL, D3DLOCK_DISCARD)))
		return;

	CG32bitPixel *pSrcRow = Layer.Primary.GetPixelPos(0, 0);
	CG32bitPixel *pSrcRowEnd = Layer.Primary.GetPixelPos(0, Layer.Primary.GetHeight());
	BYTE *pDestRow = (BYTE *)Locked.pBits;
	while (pSrcRow < pSrcRowEnd)
		{
		CG32bitPixel *pSrc = pSrcRow;
		CG32bitPixel *pSrcEnd = pSrcRow + Layer.Primary.GetWidth();
		DWORD *pDest = (DWORD *)pDestRow;

		while (pSrc < pSrcEnd)
			{
			*pDest = pSrc->AsDWORD();

			pDest++;
			pSrc++;
			}

		//	Next

		pSrcRow = Layer.Primary.NextRow(pSrcRow);
		pDestRow += Locked.Pitch;
		}

	Layer.pTexture->UnlockRect(0);
#endif
	}

void CDXScreen::Render (void)

//	Present
//
//	Draw

	{
	int i;

    //	Begin the scene

    if (FAILED(m_pD3DDevice->BeginScene()))
		return;

	//	Render all layers in order (back to front)

	for (i = 0; i < m_PaintOrder.GetCount(); i++)
		{
		SLayer &Layer = m_Layers[m_PaintOrder[i]];

		//	Copy the image bits to the texture

		if (Layer.pTexture)
			{
//			LayerUpdateTexture(Layer);
			m_pD3DDevice->SetTexture(0, Layer.pTexture);
			m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

			m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, (i == 0 ? FALSE : TRUE));
			m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			}

		m_pD3DDevice->SetFVF(D3DFVF_VERTEXFORMAT_STD);
		m_pD3DDevice->SetStreamSource(0, Layer.pVertices, 0, sizeof(SVertexFormatStd));
		m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
		}

	//	End the scene

	m_pD3DDevice->EndScene();

    //	Let the GPU render

    m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
	}

void CDXScreen::SwapBuffers (void)

//	SwapBuffers
//
//	Swap back buffer to primary, in preparation for rendering.

	{
	int i;

	for (i = 0; i < m_Layers.GetCount(); i++)
		{
		SLayer &Layer = m_Layers[i];

		//	Unlock the back buffer

		Layer.pBackBuffer->UnlockRect(0);

		//	Swap textures

		Swap(Layer.pBackBuffer, Layer.pTexture);

		//	Lock again

		D3DLOCKED_RECT Locked;
		if (FAILED(Layer.pBackBuffer->LockRect(0, &Locked, NULL, D3DLOCK_DISCARD)))
			continue;

		Layer.BackBuffer.CreateFromExternalBuffer(Locked.pBits, Layer.cxWidth, Layer.cyHeight, Locked.Pitch);
		}
	}
