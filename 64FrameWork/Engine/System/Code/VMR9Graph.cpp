// VMR9Graph.cpp: implementation of the CVMR9Graph class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "VMR9Graph.h"
#include <atlbase.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

USING(Engine)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Function name	: CVMR9Graph::CVMR9Graph
// Description	    : constructor
// Return type		: 
CVMR9Graph::CVMR9Graph()
{
	InitDefaultValues();
}


// Function name	: CVMR9Graph
// Description	    : constructor
// Return type		: 
// Argument         : HWND MediaWindow
// Argument         : int NumberOfStream
CVMR9Graph::CVMR9Graph(HWND MediaWindow, int NumberOfStream)
{
	InitDefaultValues();
	
	if (::IsWindow(MediaWindow)) {
		m_hMediaWindow = MediaWindow;
	}

	if (NumberOfStream > 0 && NumberOfStream < 11) {
		m_nNumberOfStream = NumberOfStream;
	}
}

// Function name	: CVMR9Graph::~CVMR9Graph
// Description	    : destructor
// Return type		: 
CVMR9Graph::~CVMR9Graph()
{
	ReleaseAllInterfaces();
}

HRESULT CVMR9Graph::Ready_Video(HWND hWnd, const char * pVideoPath)
{
	SetMediaWindow(hWnd);
	SetMediaFile(pVideoPath);

	return S_OK;
}

void CVMR9Graph::Free(void)
{
}


// Function name	: CVMR9Graph::InitDefaultValues
// Description	    : initialize all default values
// Return type		: void 
void CVMR9Graph::InitDefaultValues()
{
	ZeroMemory(m_pszErrorDescription, 1024);
	m_dwRotId				= -1;
	m_nNumberOfStream		= 4;		// default VMR9 config
	m_hMediaWindow			= NULL;
	// SRC interfaces
	for (int i=0; i<10; i++) {
		m_srcFilterArray[i] = NULL;
	}
	// SOUND interface
	m_pDirectSoundFilter	= NULL;
	// GRAPH interfaces
	m_pGraphUnknown			= NULL;
	m_pGraphBuilder			= NULL;
	m_pFilterGraph			= NULL;
	m_pFilterGraph2			= NULL;
	m_pMediaControl			= NULL;
	m_pMediaEvent			= NULL;
	m_pMediaEventEx			= NULL;
	// VMR9 interfaces
	m_pVMRBaseFilter		= NULL;
	m_pVMRFilterConfig		= NULL;
	m_pVMRMixerBitmap		= NULL;
	m_pVMRMixerControl		= NULL;
	m_pVMRMonitorConfig		= NULL;
	m_pVMRWindowlessControl	= NULL;
	// DIRECT3D interfaces
	m_pD3DDirect3d			= NULL;
	m_pD3DDevice			= NULL;
	m_pD3DSurface			= NULL;
}

// Function name	: CVMR9Graph::ReleaseAllInterfaces
// Description	    : release all of the graph interfaces
// Return type		: void 
void CVMR9Graph::ReleaseAllInterfaces()
{
	// CALLBACK handle
	if (m_pMediaEventEx != NULL) {
		m_pMediaEventEx->SetNotifyWindow(NULL, WM_MEDIA_NOTIF, NULL);
	}
	// SRC interfaces
	for (int i=0; i<10; i++) {
		IBaseFilter* pSrcFilter = m_srcFilterArray[i];
		if (pSrcFilter != NULL) {
			pSrcFilter->Release();
			m_srcFilterArray[i] = NULL;
		}
	}
	// SOUND interfaces
	if (m_pDirectSoundFilter != NULL) {
		m_pDirectSoundFilter->Release();
		m_pDirectSoundFilter = NULL;
	}
	// VMR9 interfaces
	if (m_pVMRFilterConfig != NULL) {
		m_pVMRFilterConfig->Release();
		m_pVMRFilterConfig = NULL;
	}
	if (m_pVMRMixerBitmap != NULL) {
		m_pVMRMixerBitmap->Release();
		m_pVMRMixerBitmap = NULL;
	}
	if (m_pVMRMixerControl != NULL) {
		m_pVMRMixerControl->Release();
		m_pVMRMixerControl = NULL;
	}
	if (m_pVMRMonitorConfig != NULL) {
		m_pVMRMonitorConfig->Release();
		m_pVMRMonitorConfig = NULL;
	}
	if (m_pVMRWindowlessControl != NULL) {
		m_pVMRWindowlessControl->Release();
		m_pVMRWindowlessControl = NULL;
	}
	if (m_pVMRBaseFilter != NULL) {
		m_pVMRBaseFilter->Release();
		m_pVMRBaseFilter = NULL;
	}
	// GRAPH interfaces
	if (m_pGraphBuilder != NULL) {
		m_pGraphBuilder->Release();
		m_pGraphBuilder = NULL;
	}
	if (m_pFilterGraph != NULL) {
		m_pFilterGraph->Release();
		m_pFilterGraph = NULL;
	}
	if (m_pFilterGraph2 != NULL) {
		m_pFilterGraph2->Release();
		m_pFilterGraph2 = NULL;
	}
	if (m_pMediaControl != NULL) {
		m_pMediaControl->Release();
		m_pMediaControl = NULL;
	}
	if (m_pMediaEvent != NULL) {
		m_pMediaEvent->Release();
		m_pMediaEvent = NULL;
	}
	if (m_pMediaEventEx != NULL) {
		m_pMediaEventEx->Release();
		m_pMediaEventEx = NULL;
	}
	if (m_pGraphUnknown != NULL) {
		m_pGraphUnknown->Release();
		m_pGraphUnknown = NULL;
	}
	// DIRECT3D interfaces
	if (m_pD3DSurface != NULL) {
		m_pD3DSurface->Release();
		m_pD3DSurface = NULL;
	}
	if (m_pD3DDevice != NULL) {
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}
	if (m_pD3DDirect3d != NULL) {
		m_pD3DDirect3d->Release();
		m_pD3DDirect3d = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// Helper Functions
//////////////////////////////////////////////////////////////////////


// Function name	: CVMR9Graph::GetLastError
// Description	    : get the last error description
// Return type		: LPCTSTR 
LPCTSTR CVMR9Graph::GetLastError()
{
	return (const wchar_t*)m_pszErrorDescription;
}


// Function name	: CVMR9Graph::AddToRot
// Description	    : let the graph instance be accessible from graphedit
// Return type		: HRESULT 
// Argument         : IUnknown *pUnkGraph
// Argument         : DWORD *pdwRegister
HRESULT CVMR9Graph::AddToRot(IUnknown *pUnkGraph) 
{
	if (pUnkGraph == NULL) {
		return E_INVALIDARG;
	}

    IMoniker * pMoniker;
    IRunningObjectTable *pROT;
    if (FAILED(GetRunningObjectTable(0, &pROT))) {
        return E_FAIL;
    }
    WCHAR wsz[256];
    wsprintfW(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, GetCurrentProcessId());
    HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
    if (SUCCEEDED(hr)) {
        hr = pROT->Register(0, pUnkGraph, pMoniker, &m_dwRotId);
        pMoniker->Release();
    }
    pROT->Release();

    return hr;
}


// Function name	: CVMR9Graph::RemoveFromRot
// Description	    : remove the graph instance accessibility from graphedit
// Return type		: void 
void CVMR9Graph::RemoveFromRot()
{
	if (m_dwRotId != -1) {
		IRunningObjectTable *pROT;
		if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) {
			pROT->Revoke(m_dwRotId);
			m_dwRotId = -1;
			pROT->Release();
		}
	}
}


// Function name	: CVMR9Graph::GetPin
// Description	    : return the desired pin
// Return type		: IPin* 
// Argument         : IBaseFilter *pFilter
// Argument         : PIN_DIRECTION PinDir
IPin* CVMR9Graph::GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir)
{
    BOOL       bFound = FALSE;
    IEnumPins  *pEnum;
    IPin       *pPin;

    pFilter->EnumPins(&pEnum);
    while(pEnum->Next(1, &pPin, 0) == S_OK) {
        PIN_DIRECTION PinDirThis;
        pPin->QueryDirection(&PinDirThis);
        if (bFound = (PinDir == PinDirThis))
            break;
        pPin->Release();
    }
    pEnum->Release();

    return (bFound ? pPin : 0);
}


// Function name	: CVMR9Graph::ReportError
// Description	    : report an error in the dump device
// Return type		: void 
// Argument         : const char* pszError
// Argument         : HRESULT hrCode
void CVMR9Graph::ReportError(const wchar_t* pszError, HRESULT hrCode)
{
	TCHAR szErr[MAX_ERROR_TEXT_LEN];
	DWORD res = AMGetErrorText(hrCode, szErr, MAX_ERROR_TEXT_LEN);
	const wchar_t* pString = reinterpret_cast<wchar_t*>(__FILE__);

	if (res != 0) {
		swprintf_s(m_pszErrorDescription, L"[ERROR in %s, line %d] %s : COM Error 0x%x, %s", pString, __LINE__, pszError, hrCode, szErr);
 		//sprintf(m_pszErrorDescription, "[ERROR in %s, line %d] %s : COM Error 0x%x, %s", __FILE__, __LINE__, pszError, hrCode, szErr);
	} else {
		swprintf_s(m_pszErrorDescription, L"[ERROR in %s, line %d] %s : COM Error 0x%x", pString, __LINE__, pszError, hrCode);
		//sprintf(m_pszErrorDescription, "[ERROR in %s, line %d] %s : COM Error 0x%x", __FILE__, __LINE__, pszError, hrCode);
	}
	//TRACE(L"%s \r\n", m_pszErrorDescription);
}


// Function name	: CVMR9Graph::GetNextFilter
// Description	    : 
// Return type		: HRESULT 
// Argument         : IBaseFilter *pFilter
// Argument         : PIN_DIRECTION Dir
// Argument         : IBaseFilter **ppNext
HRESULT CVMR9Graph::GetNextFilter(IBaseFilter *pFilter, PIN_DIRECTION Dir, IBaseFilter **ppNext)
{
    if (!pFilter || !ppNext) return E_POINTER;

    IEnumPins *pEnum = 0;
    IPin *pPin = 0;
    HRESULT hr = pFilter->EnumPins(&pEnum);
    if (FAILED(hr)) return hr;
    while (S_OK == pEnum->Next(1, &pPin, 0)) {
        // See if this pin matches the specified direction.
        PIN_DIRECTION ThisPinDir;
        hr = pPin->QueryDirection(&ThisPinDir);
        if (FAILED(hr)) {
            // Something strange happened.
            hr = E_UNEXPECTED;
            pPin->Release();
            break;
        }
        if (ThisPinDir == Dir) {
            // Check if the pin is connected to another pin.
            IPin *pPinNext = 0;
            hr = pPin->ConnectedTo(&pPinNext);
            if (SUCCEEDED(hr))
            {
                // Get the filter that owns that pin.
                PIN_INFO PinInfo;
                hr = pPinNext->QueryPinInfo(&PinInfo);
                pPinNext->Release();
                pPin->Release();
                pEnum->Release();
                if (FAILED(hr) || (PinInfo.pFilter == NULL))
                {
                    // Something strange happened.
                    return E_UNEXPECTED;
                }
                // This is the filter we're looking for.
                *ppNext = PinInfo.pFilter; // Client must release.
                return S_OK;
            }
        }
        pPin->Release();
    }
    pEnum->Release();
    // Did not find a matching filter.
    return E_FAIL;
}


// Function name	: CVMR9Graph::RemoveFilterChain
// Description	    : remove a chain of filter, stopping at pStopFilter
// Return type		: BOOL 
// Argument         : IBaseFilter* pFilter
// Argument         : IBaseFilter* pStopFilter
BOOL CVMR9Graph::RemoveFilterChain(IBaseFilter* pFilter, IBaseFilter* pStopFilter)
{
	HRESULT hr;

	IBaseFilter* pFilterFound = NULL;
	
	hr = GetNextFilter(pFilter, PINDIR_OUTPUT, &pFilterFound);
	if (SUCCEEDED(hr) && pFilterFound != pStopFilter) {
		RemoveFilterChain(pFilterFound, pStopFilter);
		pFilterFound->Release();
	}

	m_pFilterGraph->RemoveFilter(pFilter);

	return TRUE;
}


// Function name	: CVMR9Graph::AddFilterByClsid
// Description	    : add a filter in the chain
// Return type		: HRESULT 
// Argument         : IGraphBuilder *pGraph
// Argument         : LPCWSTR wszName
// Argument         : const GUID& clsid
// Argument         : IBaseFilter **ppF
HRESULT CVMR9Graph::AddFilterByClsid(IGraphBuilder *pGraph, LPCWSTR wszName, const GUID& clsid, IBaseFilter **ppF)
{
    *ppF = NULL;
    HRESULT hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)ppF);
    if (SUCCEEDED(hr))
    {
        hr = pGraph->AddFilter((*ppF), wszName);
    }
    return hr;
}


//////////////////////////////////////////////////////////////////////
// Layer helper methods
//////////////////////////////////////////////////////////////////////


// Function name	: CVMR9Graph::IsValidLayer
// Description	    : check for valid layer
// Return type		: BOOL 
// Argument         : int nLayer
BOOL CVMR9Graph::IsValidLayer(int nLayer)
{
	if (nLayer > 9 || nLayer < 0) return FALSE;
	
	IBaseFilter* pBaseFilter = m_srcFilterArray[nLayer];
	if (pBaseFilter == NULL) 
		return FALSE;
	else
		return TRUE;
}


// Function name	: CVMR9Graph::NormalizeRect
// Description	    : normalize a standard rect
// Return type		: VMR9NormalizedRect 
// Argument         : LPRECT pRect
VMR9NormalizedRect CVMR9Graph::NormalizeRect(LPRECT pRect)
{
	VMR9NormalizedRect nRect;

	// VIDEO size
	RECT videoRect;
	if (GetVideoRect(&videoRect)) {
		int nWidth	= videoRect.right - videoRect.left;
		int nHeight	= videoRect.bottom - videoRect.top;
		// NORMALIZEDRECT
		try {
			nRect.left = ( (float) ( (pRect->left * 100) / nWidth ) ) / 100.0f;
		} catch(...) {
			nRect.left = 0.0f;
		}
		try {
			nRect.right = ( (float) ( (pRect->right * 100) / videoRect.right ) ) / 100.0f;
		} catch(...) {
			nRect.right = 1.0f;
		}
		try {
			nRect.top = ( (float) ( (pRect->top * 100) / nHeight ) ) / 100.0f;
		} catch(...) {
			nRect.top = 0.0f;
		}
		try {
			nRect.bottom = ( (float) ( (pRect->bottom * 100) / videoRect.bottom ) ) / 100.0f;
		} catch(...) {
			nRect.bottom = 1.0f;
		}
	}

	return nRect;
}

//////////////////////////////////////////////////////////////////////
// Graph Construction / Render
//////////////////////////////////////////////////////////////////////


// Function name	: CVMR9Graph::SetMediaWindow
// Description	    : set the window needed bye the VMR9
// Return type		: BOOL 
// Argument         : HWND MediaWindow
BOOL CVMR9Graph::SetMediaWindow(HWND MediaWindow)
{
	if (!::IsWindow(MediaWindow)) {
		ReportError(L"The window handle is invalid", E_INVALIDARG);
		return FALSE;
	}

	m_hMediaWindow = MediaWindow;

	return TRUE;
}

// Function name	: CVMR9Graph::SetMediaFile
// Description	    : set a media source
// Return type		: BOOL 
// Argument         : const char* pszFileName
// Argument         : int nLayer = 0
BOOL CVMR9Graph::SetMediaFile(const char* pszFileName, int nLayer)
{
	USES_CONVERSION;

	HRESULT hr;
	BOOL	bHaveToRun = FALSE;

	if (pszFileName == NULL) {
		ReportError(L"Could not load a file with an empty file name", E_INVALIDARG);
		return FALSE;
	}

	// ENSURE that a valid graph builder is available
	if (m_pGraphBuilder == NULL) {
		BOOL bRet = BuildFilterGraph();
		if (!bRet) return bRet;
	}

	// ENSURE that the filter graph is in a stop state
	OAFilterState filterState;
	m_pMediaControl->GetState(500, &filterState);
	if (filterState != State_Stopped) {
		m_pMediaControl->Stop();
		bHaveToRun = TRUE;
	}

	// CHECK a source filter availaibility for the layer
	if (m_srcFilterArray[nLayer] == NULL) {
		char pszFilterName[10];
		sprintf_s(pszFilterName, "SRC%02d", nLayer);
		IBaseFilter* pBaseFilter = NULL;
		hr = m_pGraphBuilder->AddSourceFilter(A2W(pszFileName), A2W(pszFilterName), &pBaseFilter);
		if (FAILED(hr)) {
			ReportError(L"Could not find a source filter for this file", hr);
			return FALSE;
		}
		m_srcFilterArray[nLayer] = pBaseFilter;
	} else {
		// suppress the old src filter
		IBaseFilter* pBaseFilter = m_srcFilterArray[nLayer];
		RemoveFilterChain(pBaseFilter, m_pVMRBaseFilter);
		pBaseFilter->Release();
		m_srcFilterArray[nLayer] = NULL;
		// create a new src filter
		char pszFilterName[10];
		sprintf_s(pszFilterName, "SRC%02d", nLayer);
		hr = m_pGraphBuilder->AddSourceFilter(A2W(pszFileName), A2W(pszFilterName), &pBaseFilter);
		m_srcFilterArray[nLayer] = pBaseFilter;
		if (FAILED(hr)) {
			m_srcFilterArray[nLayer] = NULL;
			ReportError(L"Could not load the file", hr);
			return FALSE;
		}
	}

	// RENDER the graph
	BOOL bRet = RenderGraph();
	if (!bRet) return bRet;

	// RUN the graph if previous state was running
	if (bHaveToRun) {
		m_pMediaControl->Run();
	}

	return TRUE;
}


// Function name	: CVMR9Graph::BuildDirect3d
// Description	    : construct the Direct3D
// Return type		: BOOL 
BOOL CVMR9Graph::BuildDirect3d()
{
	HRESULT hr;

	m_pD3DDirect3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pD3DDirect3d == NULL) {
		ReportError(L"Cannot initialize Direct3D", E_FAIL);
		return FALSE;
	}

    D3DDISPLAYMODE d3ddm;
    hr = m_pD3DDirect3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	if (FAILED(hr)) {
		ReportError(L"Cannot get display adaptater infos from Direct3d", hr);
		return FALSE;
	}

    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed         = TRUE;
    d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = d3ddm.Format;
    d3dpp.hDeviceWindow    = m_hMediaWindow;

    hr = m_pD3DDirect3d->CreateDevice(	D3DADAPTER_DEFAULT, // always the primary display adapter
										D3DDEVTYPE_HAL,
										NULL,
										D3DCREATE_SOFTWARE_VERTEXPROCESSING,
										&d3dpp,
										&m_pD3DDevice);
	if (FAILED(hr)) {
		ReportError(L"Cannot create the Direct3d device", hr);
	}

	return TRUE;
}

// Function name	: CVMR9Graph::BuildFilterGraph
// Description	    : construct the filter graph
// Return type		: BOOL 
BOOL CVMR9Graph::BuildFilterGraph()
{
	HRESULT hr;

	ReleaseAllInterfaces();
	RemoveFromRot();
	
	// BUILD the filter graph
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IUnknown, (void**) &m_pGraphUnknown);
	if (FAILED(hr)) {
		ReportError(L"Could not build the graph", hr);
		return FALSE;
	}
	// QUERY the filter graph interfaces
	hr = m_pGraphUnknown->QueryInterface(IID_IGraphBuilder, (void**) &m_pGraphBuilder);
	hr = m_pGraphUnknown->QueryInterface(IID_IFilterGraph, (void**) &m_pFilterGraph);
	hr = m_pGraphUnknown->QueryInterface(IID_IFilterGraph2, (void**) &m_pFilterGraph2);
	hr = m_pGraphUnknown->QueryInterface(IID_IMediaControl, (void**) & m_pMediaControl);
	hr = m_pGraphUnknown->QueryInterface(IID_IMediaEvent, (void**) &m_pMediaEvent);
	hr = m_pGraphUnknown->QueryInterface(IID_IMediaEventEx, (void**) &m_pMediaEventEx);

	// SET the graph state window callback
	if (m_pMediaEventEx != NULL) {
		m_pMediaEventEx->SetNotifyWindow((OAHWND)m_hMediaWindow, WM_MEDIA_NOTIF, NULL);
	}

	BuildSoundRenderer();

// Don't known what's wrong... but RenderEx crash when playing whith graphedit build 021204 ...
	AddToRot(m_pGraphUnknown);

	return BuildVMR();
}


// Function name	: CVMR9Graph::BuildVMR
// Description	    : construct and add the VMR9 renderer to the graph
// Return type		: BOOL 
BOOL CVMR9Graph::BuildVMR()
{
	HRESULT hr;

	if (m_hMediaWindow == NULL) {
		ReportError(L"Could not operate without a Window", E_FAIL);
		return FALSE;
	}

	if (m_pGraphBuilder == NULL) {
		ReportError(L"Could not build the VMR, the graph isn't valid", E_FAIL);
		return FALSE;
	}

	// BUILD the VMR9
	IBaseFilter *pVmr = NULL;
	hr = CoCreateInstance(CLSID_VideoMixingRenderer9, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**) &m_pVMRBaseFilter);
	if (FAILED(hr)) {
		ReportError(L"Could not create an instance ofthe VMR9", hr);
		return FALSE;
	}

	// ADD the VMR9 to the graph
	hr = m_pGraphBuilder->AddFilter(m_pVMRBaseFilter, L"VMR9");
	if (FAILED(hr)) {
		ReportError(L"Could not add the VMR9 to the Graph", hr);
		return FALSE;
	}

	// DIRECT3D
	BOOL bD3D = BuildDirect3d();

	// QUERY the VMR9 interfaces
	hr = m_pVMRBaseFilter->QueryInterface(IID_IVMRFilterConfig9, (void**) &m_pVMRFilterConfig);
	if (SUCCEEDED(hr)) {
		// CONFIGURE the VMR9
		m_pVMRFilterConfig->SetRenderingMode(VMR9Mode_Windowless);
		m_pVMRFilterConfig->SetNumberOfStreams(m_nNumberOfStream);
	}

	hr = m_pVMRBaseFilter->QueryInterface(IID_IVMRWindowlessControl9, (void**) &m_pVMRWindowlessControl);
	if (SUCCEEDED(hr)) {
		// CONFIGURE the VMR9
		m_pVMRWindowlessControl->SetVideoClippingWindow(m_hMediaWindow);
		m_pVMRWindowlessControl->SetAspectRatioMode(VMR9ARMode_LetterBox);
	}

	hr = m_pVMRBaseFilter->QueryInterface(IID_IVMRMixerBitmap9, (void**) &m_pVMRMixerBitmap);
	hr = m_pVMRBaseFilter->QueryInterface(IID_IVMRMixerControl9, (void**) &m_pVMRMixerControl);
	hr = m_pVMRBaseFilter->QueryInterface(IID_IVMRMonitorConfig9, (void**) &m_pVMRMonitorConfig);

	return TRUE;
}


// Function name	: CVMR9Graph::BuildSoundRendered
// Description	    : build the DirectSound renderer
// Return type		: BOOL 
BOOL CVMR9Graph::BuildSoundRenderer()
{
	HRESULT hr;

	hr = AddFilterByClsid(m_pGraphBuilder, L"DirectSound", CLSID_DSoundRender, &m_pDirectSoundFilter);
	if (FAILED(hr)) {
		ReportError(L"Could not add the DirectSoundRenderer", hr);
		return FALSE;
	}
	return TRUE;
}

// Function name	: CVMR9Graph::RenderGraph
// Description	    : render the graph
// Return type		: BOOL 
BOOL CVMR9Graph::RenderGraph()
{
	HRESULT hr;

	if (m_pFilterGraph2 == NULL) {
		ReportError(L"Could not render the graph because it is not fully constructed", E_FAIL);
		return FALSE;
	}

	for (int i=0; i<10; i++) {
		IBaseFilter* pBaseFilter = m_srcFilterArray[i];
		if (pBaseFilter != NULL) {
			IPin* pPin = GetPin(pBaseFilter, PINDIR_OUTPUT);
			if (pPin != NULL) {
				try {
				hr = m_pFilterGraph2->RenderEx(pPin, AM_RENDEREX_RENDERTOEXISTINGRENDERERS, NULL);
				} catch(...) {
				}
			}
		}
	}
	return TRUE;
}


// Function name	: CVMR9Graph::PreserveAspectRatio
// Description	    : set aspect ratio mode
// Return type		: BOOL 
// Argument         : BOOL bPreserve
BOOL CVMR9Graph::PreserveAspectRatio(BOOL bPreserve)
{
	if (m_pVMRWindowlessControl == NULL) {
		ReportError(L"Can't set aspect ratio, no VMR", E_FAIL);
		return FALSE;
	}

	if (bPreserve)
		m_pVMRWindowlessControl->SetAspectRatioMode(VMR9ARMode_LetterBox);
	else
		m_pVMRWindowlessControl->SetAspectRatioMode(VMR9ARMode_None);

	return TRUE;
}


// Function name	: CVMR9Graph::AddFilter
// Description	    : manually add a filter in the graph
// Return type		: IBaseFilter* : caller responsible of release
// Argument         : const char* pszName
// Argument         : const GUID& clsid
IBaseFilter* CVMR9Graph::AddFilter(const char* pszName, const GUID& clsid)
{
	USES_CONVERSION;

	HRESULT hr;

	IBaseFilter* pBaseFilter = NULL;

	if (pszName == NULL) {
		ReportError(L"Can't add filter, no valid name", E_INVALIDARG);
		return NULL;
	}

	hr = AddFilterByClsid(m_pGraphBuilder, A2W(pszName), clsid, &pBaseFilter);
	if (FAILED(hr)) {
		ReportError(L"Can't add filter", hr);
		return NULL;
	}

	return pBaseFilter;
}

// Function name	: CVMR9Graph::PlayGraph
// Description	    : run the graph
// Return type		: BOOL 
BOOL CVMR9Graph::PlayGraph()
{
	if (m_pMediaControl == NULL) {
		ReportError(L"Can't play, no graph", E_FAIL);
		return FALSE;
	}
	if (m_pVMRWindowlessControl == NULL) {
		ReportError(L"Can't play, no VMR", E_FAIL);
		return FALSE;
	}

	// MEDIA SIZE
	LONG  Width;
	LONG  Height;
	LONG  ARWidth;
	LONG  ARHeight;
	m_pVMRWindowlessControl->GetNativeVideoSize(&Width, &Height, &ARWidth, &ARHeight);
	RECT mediaRect;
	mediaRect.left = 0;
	mediaRect.top = 0;
	mediaRect.right = Width;
	mediaRect.bottom = Height;
	RECT wndRect;
	GetClientRect(m_hMediaWindow, &wndRect);
	m_pVMRWindowlessControl->SetVideoPosition(&mediaRect, &wndRect);

	// RUN
	m_pMediaControl->Run();

	return TRUE;
}


// Function name	: CVMR9Graph::StopGraph
// Description	    : stop the graph
// Return type		: BOOL 
BOOL CVMR9Graph::StopGraph()
{
	if (m_pMediaControl == NULL) {
		ReportError(L"Can't stop, no graph", E_FAIL);
		return FALSE;
	}

	m_pMediaControl->Stop();

	return TRUE;
}


// Function name	: CVMR9Graph::ResetGraph
// Description	    : reset the graph - clean interfaces
// Return type		: BOOL 
BOOL CVMR9Graph::ResetGraph()
{
	// STOP the graph
	if (m_pMediaControl != NULL) {
		m_pMediaControl->Stop();
	}

	try {
		ReleaseAllInterfaces();
	} catch(...) {
		ReportError(L"Can't reset graph, we have serious bugs...", E_FAIL);
		return FALSE;
	}

	return TRUE;
}


// Function name	: CVMR9Graph::GetPtrMediaEvent
// Description	    : return IMediaEvent interface
// Return type		: IMediaEvent* 
IMediaEvent* CVMR9Graph::GetPtrMediaEvent()
{
	if (m_pGraphBuilder == NULL) {
		ReportError(L"Can't return IMediaEvent, no graph", E_FAIL);
		return NULL;
	}

	if (m_pMediaEvent != NULL) {
		m_pMediaEvent->AddRef();
	}

	return m_pMediaEvent;
}


// Function name	: CVMR9Graph::GetPtrMediaControl
// Description	    : return IMediaControl interface
// Return type		: IMediaControl* 
IMediaControl* CVMR9Graph::GetPtrMediaControl()
{
	if (m_pGraphBuilder == NULL) {
		ReportError(L"Can't return IMediaControl, no graph", E_FAIL);
		return NULL;
	}

	if (m_pMediaControl != NULL) {
		m_pMediaControl->AddRef();
	}

	return m_pMediaControl;
}


// Function name	: CVMR9Graph::GetPtrMediaSeeking
// Description	    : return IMediaSeeking interface
// Return type		: IMediaSeeking* 
IMediaSeeking* CVMR9Graph::GetPtrMediaSeeking()
{
	IMediaSeeking* pMediaSeeking = NULL;

	if (m_pGraphBuilder == NULL) {
		ReportError(L"Can't return IMediaSeeking, no graph", E_FAIL);
		return NULL;
	}

	m_pGraphBuilder->QueryInterface(IID_IMediaSeeking, (void**) &pMediaSeeking);

	return pMediaSeeking;
}


// Function name	: CVMR9Graph::GetPtrBasicAudio
// Description	    : return IBasicAudio interface
// Return type		: IBasicAudio* 
IBasicAudio* CVMR9Graph::GetPtrBasicAudio()
{
	IBasicAudio* pBasicAudio = NULL;

	if (m_pGraphBuilder == NULL) {
		ReportError(L"Can't return IBasicAudio, no graph", E_FAIL);
		return NULL;
	}

	m_pGraphBuilder->QueryInterface(IID_IBasicAudio, (void**) &pBasicAudio);

	return pBasicAudio;
}

// Function name	: CVMR9Graph::Repaint
// Description	    : repaint the media - use when reflected bye the window
// Return type		: void 
BOOL CVMR9Graph::Repaint()
{
	if (m_pVMRWindowlessControl == NULL) {
		ReportError(L"Can't repaint, no VMR", E_FAIL);
		return FALSE;
	}

	HDC hdc = GetDC(m_hMediaWindow);
	m_pVMRWindowlessControl->RepaintVideo(m_hMediaWindow, hdc);

	return TRUE;
}


// Function name	: CVMR9Graph::Resize
// Description	    : resize the media - use when reflected bye the window
// Return type		: BOOL 
// Argument         : int cx
// Argument         : int cy
BOOL CVMR9Graph::Resize()
{
	if (m_pVMRWindowlessControl == NULL) {
		ReportError(L"Can't resize, no VMR", E_FAIL);
		return FALSE;
	}

	// MEDIA SIZE
	LONG  Width;
	LONG  Height;
	LONG  ARWidth;
	LONG  ARHeight;
	m_pVMRWindowlessControl->GetNativeVideoSize(&Width, &Height, &ARWidth, &ARHeight);
	RECT mediaRect;
	mediaRect.left = 0;
	mediaRect.top = 0;
	mediaRect.right = Width;
	mediaRect.bottom = Height;
	RECT wndRect;
	GetClientRect(m_hMediaWindow, &wndRect);
	m_pVMRWindowlessControl->SetVideoPosition(&mediaRect, &wndRect);

	return TRUE;
}


// Function name	: CVMR9Graph::GetVideoRect
// Description	    : return size of video composition
// Return type		: RECT 
BOOL CVMR9Graph::GetVideoRect(LPRECT pRect)
{
	HRESULT hr;

	if (pRect == NULL) {
		ReportError(L"Can't return video size, rect struct invalid", E_INVALIDARG);
		return FALSE;
	}

	if (m_pVMRWindowlessControl == NULL) {
		ReportError(L"Can't return video size, no VMR", E_FAIL);
		return FALSE;
	}

	// MEDIA SIZE
	LONG  Width;
	LONG  Height;
	LONG  ARWidth;
	LONG  ARHeight;
	hr = m_pVMRWindowlessControl->GetNativeVideoSize(&Width, &Height, &ARWidth, &ARHeight);

	if (FAILED(hr)) {
		ReportError(L"Can't return video size, VMR error", hr);
		return FALSE;
	}

	// RECT structure
	pRect->left		= 0;
	pRect->top		= 0;
	pRect->right	= Width;
	pRect->bottom	= Height;

	return TRUE;
}


// Function name	: CVMR9Graph::SetAlphaLayer
// Description	    : set alpha on layer
// Return type		: BOOL 
// Argument         : int nLayer
// Argument         : float fApha
BOOL CVMR9Graph::SetAlphaLayer(int nLayer, int nAlpha)
{
	HRESULT hr;

	if (!IsValidLayer(nLayer)) {
		ReportError(L"Can't set alpha, incorect layer", E_INVALIDARG);
		return FALSE;
	}

	if (nAlpha < 0 || nAlpha > 100) {
		ReportError(L"Can't set alpha, incorrect alpha value", E_INVALIDARG);
		return FALSE;
	}

	if (m_pVMRMixerControl == NULL) {
		ReportError(L"Can't set alpha, no VMR", E_FAIL);
		return FALSE;
	}

	float fAlpha = ((float)nAlpha) / 100.0f;
	hr = m_pVMRMixerControl->SetAlpha(nLayer, fAlpha);

	return TRUE;
}


// Function name	: CVMR9Graph::GetAlphaLayer
// Description	    : return current alpha layer level
// Return type		: float 
// Argument         : int nLayer
int CVMR9Graph::GetAlphaLayer(int nLayer)
{
	HRESULT hr;

	if (!IsValidLayer(nLayer)) {
		ReportError(L"Can't set alpha, incorect layer", E_INVALIDARG);
		return 1;
	}

	if (m_pVMRMixerControl == NULL) {
		ReportError(L"Can't set alpha, no VMR", E_FAIL);
		return 1;
	}

	float fAlpha;
	hr = m_pVMRMixerControl->GetAlpha(nLayer, &fAlpha);

	return (int)(fAlpha*100);
}

// Function name	: GetLayerZOrder
// Description	    : return z order of the layer
// Return type		: DWORD 
// Argument         : int nLayer	: bigger is away
DWORD CVMR9Graph::GetLayerZOrder(int nLayer)
{
	HRESULT hr;
	DWORD	dwZOrder;

	if (!IsValidLayer(nLayer)) {
		ReportError(L"Can't return order, incorect layer", E_INVALIDARG);
		return FALSE;
	}

	if (m_pVMRMixerControl == NULL) {
		ReportError(L"Can't return order, no VMR", E_FAIL);
		return FALSE;
	}

	hr = m_pVMRMixerControl->GetZOrder(nLayer, &dwZOrder);

	return dwZOrder;
}


// Function name	: SetLayerZOrder
// Description	    : set z order of the layer
// Return type		: BOOL 
// Argument         : int nLayer
// Argument         : DWORD dwZOrder	: bigger is away
BOOL CVMR9Graph::SetLayerZOrder(int nLayer, DWORD dwZOrder)
{
	HRESULT hr;

	if (!IsValidLayer(nLayer)) {
		ReportError(L"Can't set order, incorect layer", E_INVALIDARG);
		return FALSE;
	}

	if (m_pVMRMixerControl == NULL) {
		ReportError(L"Can't set order, no VMR", E_FAIL);
		return FALSE;
	}

	hr = m_pVMRMixerControl->SetZOrder(nLayer, dwZOrder);
	if (FAILED(hr)) {
		ReportError(L"Can't set ZOrder", hr);
		return FALSE;
	}

	return TRUE;
}


// Function name	: SetLayerRect
// Description	    : set the layer media size and position
// Return type		: BOOL 
// Argument         : int nLayer
// Argument         : LPRECT rect
BOOL CVMR9Graph::SetLayerRect(int nLayer, RECT layerRect)
{
	HRESULT hr;

	if (!IsValidLayer(nLayer)) {
		ReportError(L"Can't set layer rect, incorect layer", E_INVALIDARG);
		return FALSE;
	}

	if (m_pVMRMixerControl == NULL) {
		ReportError(L"Can't set layer rect, no VMR", E_FAIL);
		return FALSE;
	}

	if (m_pVMRWindowlessControl == NULL) {
		ReportError(L"Can't set layer rect, no VMR", E_FAIL);
		return FALSE;
	}

	// VIDEO size
	VMR9NormalizedRect nRect = NormalizeRect(&layerRect);
	hr = m_pVMRMixerControl->SetOutputRect(nLayer, &nRect);
	if (FAILED(hr)) {
		ReportError(L"Can't set layer rect", hr);
		return FALSE;
	}

	return TRUE;
}


// Function name	: CVMR9Graph::GetBitmapSize
// Description	    : return the size of a bitmap
// Return type		: BOOL 
// Argument         : const char* pszBitmapFileName
// Argument         : long& lHeight
// Argument         : long& lWidth
BOOL CVMR9Graph::GetBitmapSize(const wchar_t* pszBitmapFileName,long& lHeight, long& lWidth)
{
    BOOL ret = FALSE;

	// LOAD Bitmap file
    HANDLE hBitmap = LoadImage(	NULL,
								pszBitmapFileName,
								IMAGE_BITMAP,
								0,0,
								LR_DEFAULTCOLOR|LR_DEFAULTSIZE|LR_LOADFROMFILE);

	// GET size
    if( hBitmap ) {
        BITMAP bmpInfo;
        if(::GetObject(hBitmap, sizeof(bmpInfo), &bmpInfo)) {
            lHeight = bmpInfo.bmHeight;
            lWidth  = bmpInfo.bmWidth;
            ret = TRUE;
        }    
    }

	// CLEAN image resource
    DeleteObject(hBitmap);
    return ret;
}

// Function name	: CVMR9Graph::SetBitmap
// Description	    : load and display an overlay bitmap
// Return type		: BOOL 
// Argument         : const char* pszBitmapFileName
// Argument         : float fAlpha
// Argument         : int nXFactor
// Argument         : int nYFactor
// Argument         : int nCXFactor
// Argument         : int nCYFactor
BOOL CVMR9Graph::SetBitmap(const wchar_t* pszBitmapFileName, int nAlpha, COLORREF cTransColor, RECT bitmapRect)
{
	HRESULT hr;

	// PARAMS check
	if (m_pVMRMixerBitmap == NULL) {
		ReportError(L"Can't set bitmap, no VMR", E_FAIL);
		return FALSE;
	}

	if (m_pVMRWindowlessControl == NULL) {
		ReportError(L"Can't set bitmap, no VMR", E_FAIL);
		return FALSE;
	}

	if (m_pD3DDevice == NULL) {
		ReportError(L"Can't set bitmap, no Direct3D device", E_FAIL);
		return FALSE;
	}

	// CLEAN-UP old suface
	if (m_pD3DSurface != NULL) {
		m_pD3DSurface->Release();
		m_pD3DSurface = NULL;
	}

	// CREATE a new surface
	long lWidth, lHeight;
	if ( !GetBitmapSize(pszBitmapFileName, lHeight, lWidth) ) {
		lWidth = 128;
		lHeight = 128;
	}
    hr = m_pD3DDevice->CreateOffscreenPlainSurface(	lWidth, lHeight,
													D3DFMT_X8R8G8B8,
													D3DPOOL_SYSTEMMEM,
													&m_pD3DSurface,
													NULL);
	if (FAILED(hr)) {
		ReportError(L"Can't create a Direct3d surface", hr);
		return FALSE;
	}

	// LOAD bitmap file
	D3DXLoadSurfaceFromFile(m_pD3DSurface,
							NULL,					// palette
							NULL,					// entire surface - created to be the proper height
							pszBitmapFileName,
							NULL,					// entire image source
							D3DX_DEFAULT, 
							0,						// disable color key
							NULL					// source info
							);

	// MIXER params
	VMR9NormalizedRect nRect = NormalizeRect(&bitmapRect);
	VMR9AlphaBitmap alphaBitmap;
    alphaBitmap.dwFlags			= VMR9AlphaBitmap_EntireDDS | VMR9AlphaBitmap_SrcColorKey;
    alphaBitmap.hdc				= NULL;
    alphaBitmap.pDDS			= m_pD3DSurface;
    alphaBitmap.clrSrcKey		= cTransColor;
    alphaBitmap.rDest.top		= nRect.top;
    alphaBitmap.rDest.left		= nRect.left;
    alphaBitmap.rDest.bottom	= nRect.bottom;
    alphaBitmap.rDest.right		= nRect.right;
    alphaBitmap.fAlpha			= nAlpha / 100.0f;

	// SHOW the bitmap
	hr = m_pVMRMixerBitmap->SetAlphaBitmap(&alphaBitmap);
	if (FAILED(hr)) {
		ReportError(L"Can't display the bitmap", hr);
		return FALSE;
	}

	return TRUE;
}


// Function name	: CVMR9Graph::SetBitmapParams
// Description	    : set parameters of the overlay bitmap
// Return type		: BOOL 
// Argument         : int nAlpha
// Argument         : COLORREF cTransColor
// Argument         : RECT bitmapRect
BOOL CVMR9Graph::SetBitmapParams(int nAlpha, COLORREF cTransColor, RECT bitmapRect)
{
	HRESULT hr;

	// PARAMS check
	if (m_pVMRMixerBitmap == NULL) {
		ReportError(L"Can't set bitmap, no VMR", E_FAIL);
		return FALSE;
	}

	if (m_pVMRWindowlessControl == NULL) {
		ReportError(L"Can't set bitmap, no VMR", E_FAIL);
		return FALSE;
	}

	if (m_pD3DDevice == NULL) {
		ReportError(L"Can't set bitmap, no Direct3D device", E_FAIL);
		return FALSE;
	}

	if (m_pD3DSurface == NULL) {
		ReportError(L"Can't set bitmap, no Direct3D surface", E_FAIL);
		return FALSE;
	}

	// MIXER params
	VMR9NormalizedRect nRect = NormalizeRect(&bitmapRect);
	VMR9AlphaBitmap alphaBitmap;
    alphaBitmap.dwFlags			= VMR9AlphaBitmap_EntireDDS | VMR9AlphaBitmap_SrcColorKey;
    alphaBitmap.hdc				= NULL;
    alphaBitmap.pDDS			= m_pD3DSurface;
    alphaBitmap.clrSrcKey		= cTransColor;
    alphaBitmap.rDest.top		= nRect.top;
    alphaBitmap.rDest.left		= nRect.left;
    alphaBitmap.rDest.bottom	= nRect.bottom;
    alphaBitmap.rDest.right		= nRect.right;
    alphaBitmap.fAlpha			= nAlpha / 100.0f;

	// SHOW the bitmap
	hr = m_pVMRMixerBitmap->SetAlphaBitmap(&alphaBitmap);
	if (FAILED(hr)) {
		ReportError(L"Can't display the bitmap", hr);
		return FALSE;
	}

	return TRUE;
}
