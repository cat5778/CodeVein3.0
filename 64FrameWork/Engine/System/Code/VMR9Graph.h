// VMR9Graph.h: interface for the CVMR9Graph class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VMR9GRAPH_H__449FDB5B_6719_4134_B5A7_B651C08D109E__INCLUDED_)
#define AFX_VMR9GRAPH_H__449FDB5B_6719_4134_B5A7_B651C08D109E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <dshow.h>
#include <D3d9.h>
#include <Vmr9.h>
#include <d3dx9tex.h>

#pragma comment( lib, "strmiids.lib" )
#pragma comment( lib, "Quartz.lib" )
//#pragma comment( lib, "d3d9.lib" )
//#pragma comment( lib, "d3dx9.lib" )

#define WM_MEDIA_NOTIF		WM_APP + 1

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CVMR9Graph : public CBase
{
	// Constructor / destructor
public:
	CVMR9Graph();
	CVMR9Graph(HWND MediaWindow, int NumberOfStream = 4);
	virtual ~CVMR9Graph();

public:
	HRESULT Ready_Video(HWND hWnd, const char* pVideoPath);

public:
	virtual void Free(void) override;

	// Methods
public:
	// Graph configuration
	//void SetNumberOfLayer(int nNumberOfLayer);
	BOOL SetMediaWindow(HWND MediaWindow);
	BOOL SetMediaFile(const char* pszFileName, int nLayer = 0);
	BOOL PreserveAspectRatio(BOOL bPreserve = TRUE);
	IBaseFilter* AddFilter(const char* pszName, const GUID& clsid);

	// Graph control
	BOOL PlayGraph();
	BOOL StopGraph();
	BOOL ResetGraph();
	IMediaEvent* GetPtrMediaEvent();
	IMediaControl* GetPtrMediaControl();
	IMediaSeeking* GetPtrMediaSeeking();
	IBasicAudio* GetPtrBasicAudio();


	// Layer control
	BOOL GetVideoRect(LPRECT pRect);
	int GetAlphaLayer(int nLayer);
	BOOL SetAlphaLayer(int nLayer, int nAlpha);
	DWORD GetLayerZOrder(int nLayer);
	BOOL SetLayerZOrder(int nLayer, DWORD dwZOrder);
	BOOL SetLayerRect(int nLayer, RECT layerRect);

	// Bitmap control
	BOOL SetBitmap(const wchar_t* pszBitmapFileName, int nAlpha, COLORREF cTransColor, RECT bitmapRect);
	BOOL SetBitmapParams(int nAlpha, COLORREF cTransColor, RECT bitmapRect);

	// Reflected from window
	BOOL Repaint();
	BOOL Resize();

	// helper
	LPCTSTR GetLastError();

protected:
	// INIT helper methods
	void InitDefaultValues();
	void ReleaseAllInterfaces();

	// GRAPH methods
	BOOL BuildFilterGraph();
	BOOL BuildVMR();
	BOOL BuildSoundRenderer();
	BOOL RenderGraph();

	// DIRECT3D methods
	BOOL BuildDirect3d();
	BOOL GetBitmapSize(const wchar_t* pszBitmapFileName,long& lHeight, long& lWidth);


	// LAYER helper methods
	BOOL IsValidLayer(int nLayer);
	VMR9NormalizedRect NormalizeRect(LPRECT pRect);

	// DSOW helper methods
	HRESULT AddToRot(IUnknown *pUnkGraph);
	void RemoveFromRot();
	IPin* GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir);
	void ReportError(const wchar_t* pszError, HRESULT hrCode);
	HRESULT GetNextFilter(IBaseFilter *pFilter, PIN_DIRECTION Dir, IBaseFilter **ppNext);
	BOOL RemoveFilterChain(IBaseFilter* pFilter, IBaseFilter* pStopFilter);
	HRESULT AddFilterByClsid(IGraphBuilder *pGraph, LPCWSTR wszName, const GUID& clsid, IBaseFilter **ppF);

	// Attributes
public:

protected:
	DWORD						m_dwRotId;
	wchar_t						m_pszErrorDescription[1024+MAX_ERROR_TEXT_LEN];
	int							m_nNumberOfStream;
	// MEDIA WINDOW
	HWND						m_hMediaWindow;
	// SRC interfaces array
	IBaseFilter*				m_srcFilterArray[10];
	// SOUND interfaces
	IBaseFilter*				m_pDirectSoundFilter;
	// GRAPH interfaces
	IUnknown*					m_pGraphUnknown;
	IGraphBuilder*				m_pGraphBuilder;
	IFilterGraph*				m_pFilterGraph;
	IFilterGraph2*				m_pFilterGraph2;
	IMediaControl*				m_pMediaControl;
	IMediaEvent*				m_pMediaEvent;
	IMediaEventEx*				m_pMediaEventEx;
	// VMR9 interfaces
	IBaseFilter*				m_pVMRBaseFilter;
	IVMRFilterConfig9*			m_pVMRFilterConfig;
	IVMRMixerBitmap9*			m_pVMRMixerBitmap;
	IVMRMixerControl9*			m_pVMRMixerControl;
	IVMRMonitorConfig9*			m_pVMRMonitorConfig;
	IVMRWindowlessControl9*		m_pVMRWindowlessControl;
	// DIRECT3D interfaces
	IDirect3D9*					m_pD3DDirect3d;
	IDirect3DDevice9*			m_pD3DDevice;
	IDirect3DSurface9*			m_pD3DSurface;
};
END
#endif // !defined(AFX_VMR9GRAPH_H__449FDB5B_6719_4134_B5A7_B651C08D109E__INCLUDED_)
