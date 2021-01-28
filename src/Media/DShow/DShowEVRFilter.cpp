#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/DShow/DShowEVRFilter.h"
#include <windows.h>
#include <dshow.h>
#include <Evr.h>

Media::DShow::DShowEVRFilter::DShowEVRFilter(Media::DShow::DShowManager *mgr, void *hwnd) : Media::DShow::DShowFilter(mgr)
{
	IBaseFilter *vrhEVR = 0;
	IMFGetService *pGS = 0;
	IMFVideoDisplayControl *vrhDisplay = 0;
	HRESULT hr;
	hr = CoCreateInstance(CLSID_EnhancedVideoRenderer, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&vrhEVR));
	if (vrhEVR)
	{
		hr = vrhEVR->QueryInterface(IID_PPV_ARGS(&pGS)); 
		if (pGS)
		{
			hr = pGS->GetService(MR_VIDEO_RENDER_SERVICE, IID_PPV_ARGS(&vrhDisplay));
			if (vrhDisplay)
			{
				RECT rc;
				hr = vrhDisplay->SetVideoWindow((HWND)hwnd);
				hr = vrhDisplay->SetAspectRatioMode(MFVideoARMode_PreservePicture);
				GetClientRect((HWND)hwnd, &rc);
				hr = vrhDisplay->SetVideoPosition(NULL, &rc);

				vrhDisplay->Release();
				vrhDisplay = 0;
			}
			pGS->Release();
			pGS = 0;
		}
		this->filter = vrhEVR;
	}
}

Media::DShow::DShowEVRFilter::~DShowEVRFilter()
{
}

const WChar *Media::DShow::DShowEVRFilter::GetName()
{
	return L"EVR";
}
