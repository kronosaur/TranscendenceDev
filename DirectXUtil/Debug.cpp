//	Debug.cpp
//
//	Debugging routines

#include <windows.h>
#include "Alchemy.h"
#include "DirectXUtil.h"

void DebugOutputPixelFormat (DDPIXELFORMAT &format)

//	DebugOutputPixelFormat
//
//	Outputs the pixel format for the given surface

	{
	kernelDebugLogPattern("PIXEL FORMAT");
	if (format.dwFlags & DDPF_ALPHA) kernelDebugLogPattern("\tDDPF_ALPHA");
	if (format.dwFlags & DDPF_ALPHAPIXELS) kernelDebugLogPattern("\tDDPF_ALPHAPIXELS");
	if (format.dwFlags & DDPF_COMPRESSED) kernelDebugLogPattern("\tDDPF_COMPRESSED");
	if (format.dwFlags & DDPF_FOURCC) kernelDebugLogPattern("\tDDPF_FOURCC");
	if (format.dwFlags & DDPF_PALETTEINDEXED1) kernelDebugLogPattern("\tDDPF_PALETTEINDEXED1");
	if (format.dwFlags & DDPF_PALETTEINDEXED2) kernelDebugLogPattern("\tDDPF_PALETTEINDEXED2");
	if (format.dwFlags & DDPF_PALETTEINDEXED4) kernelDebugLogPattern("\tDDPF_PALETTEINDEXED4");
	if (format.dwFlags & DDPF_PALETTEINDEXED2) kernelDebugLogPattern("\tDDPF_PALETTEINDEXED2");
	if (format.dwFlags & DDPF_PALETTEINDEXEDTO8) kernelDebugLogPattern("\tDDPF_PALETTEINDEXEDTO8");
	if (format.dwFlags & DDPF_RGB) kernelDebugLogPattern("\tDDPF_RGB");
	if (format.dwFlags & DDPF_RGBTOYUV) kernelDebugLogPattern("\tDDPF_RGBTOYUV");
	if (format.dwFlags & DDPF_YUV) kernelDebugLogPattern("\tDDPF_YUV");
	if (format.dwFlags & DDPF_ZBUFFER) kernelDebugLogPattern("\tDDPF_ZBUFFER");
#ifdef DX6
	if (format.dwFlags & DDPF_ALPHAPREMULT) kernelDebugLogPattern("\tDDPF_ALPHAPREMULT");
	if (format.dwFlags & DDPF_BUMPLUMINANCE) kernelDebugLogPattern("\tDDPF_BUMPLUMINANCE");
	if (format.dwFlags & DDPF_BUMPDUDV) kernelDebugLogPattern("\tDDPF_BUMPDUDV");
	if (format.dwFlags & DDPF_LUMINANCE) kernelDebugLogPattern("\tDDPF_LUMINANCE");
	if (format.dwFlags & DDPF_STENCILBUFFER) kernelDebugLogPattern("\tDDPF_STENCILBUFFER");
	if (format.dwFlags & DDPF_ZPIXELS) kernelDebugLogPattern("\tDDPF_ZPIXELS");
#endif

	kernelDebugLogPattern("dwRGBBitCount: %d", format.dwRGBBitCount);
	kernelDebugLogPattern("dwRBitMask: %x", format.dwRBitMask);
	kernelDebugLogPattern("dwGBitMask: %x", format.dwGBitMask);
	kernelDebugLogPattern("dwBBitMask: %x", format.dwBBitMask);
	kernelDebugLogPattern("dwRGBAlphaBitMask: %x", format.dwRGBAlphaBitMask);
	}

void DebugOutputSurfaceDesc (DDSURFACEDESC2 &surfacedesc)

//	DebugOutputSurfaceDesc
//
//	Output surface characteristics including pixel format

	{
	kernelDebugLogPattern("SURFACE CHARACTERISTICS");
	kernelDebugLogPattern("dwHeight: %d", surfacedesc.dwHeight);
	kernelDebugLogPattern("dwWidth: %d", surfacedesc.dwWidth);
	kernelDebugLogPattern("lPitch: %d", surfacedesc.lPitch);
#ifdef DX6
	kernelDebugLogPattern("dwLinearSize: %d", surfacedesc.dwLinearSize);
#endif
	kernelDebugLogPattern("dwBackBufferCount: %d", surfacedesc.dwBackBufferCount);
	kernelDebugLogPattern("dwMipMapCount: %d", surfacedesc.dwMipMapCount);
	kernelDebugLogPattern("dwRefreshRate: %d", surfacedesc.dwRefreshRate);
	kernelDebugLogPattern("dwAlphaBitDepth: %d", surfacedesc.dwAlphaBitDepth);

	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_3DDEVICE) kernelDebugLogPattern("\tDDSCAPS_3DDEVICE");
	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_ALPHA) kernelDebugLogPattern("\tDDSCAPS_ALPHA");
	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_BACKBUFFER) kernelDebugLogPattern("\tDDSCAPS_BACKBUFFER");
	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_COMPLEX) kernelDebugLogPattern("\tDDSCAPS_COMPLEX");
	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_FLIP) kernelDebugLogPattern("\tDDSCAPS_FLIP");
	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_FRONTBUFFER) kernelDebugLogPattern("\tDDSCAPS_FRONTBUFFER");
	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_MIPMAP) kernelDebugLogPattern("\tDDSCAPS_MIPMAP");
	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_MODEX) kernelDebugLogPattern("\tDDSCAPS_MODEX");
	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_OFFSCREENPLAIN) kernelDebugLogPattern("\tDDSCAPS_OFFSCREENPLAIN");
	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_OVERLAY) kernelDebugLogPattern("\tDDSCAPS_OVERLAY");
	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_OWNDC) kernelDebugLogPattern("\tDDSCAPS_OWNDC");
	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_PALETTE) kernelDebugLogPattern("\tDDSCAPS_PALETTE");
	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE) kernelDebugLogPattern("\tDDSCAPS_PRIMARYSURFACE");
	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACELEFT) kernelDebugLogPattern("\tDDSCAPS_PRIMARYSURFACELEFT");
	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY) kernelDebugLogPattern("\tDDSCAPS_SYSTEMMEMORY");
	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_TEXTURE) kernelDebugLogPattern("\tDDSCAPS_TEXTURE");
	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) kernelDebugLogPattern("\tDDSCAPS_VIDEOMEMORY");
	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_VISIBLE) kernelDebugLogPattern("\tDDSCAPS_VISIBLE");
	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_WRITEONLY) kernelDebugLogPattern("\tDDSCAPS_WRITEONLY");
	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_ZBUFFER) kernelDebugLogPattern("\tDDSCAPS_ZBUFFER");
#ifdef DX6
	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_LOCALVIDMEM) kernelDebugLogPattern("\tDDSCAPS_LOCALVIDMEM");
	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM) kernelDebugLogPattern("\tDDSCAPS_NONLOCALVIDMEM");
	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_STANDARDVGAMODE) kernelDebugLogPattern("\tDDSCAPS_STANDARDVGAMODE");
	if (surfacedesc.ddsCaps.dwCaps & DDSCAPS_VIDEOPORT) kernelDebugLogPattern("\tDDSCPAS_VIDEOPORT");
#endif

	DebugOutputPixelFormat(surfacedesc.ddpfPixelFormat);
	}
