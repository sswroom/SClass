#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/DShow/DShowVMR9Filter.h"
#include <windows.h>
#include <dshow.h>
#include <d3d9.h>
#include <Vmr9.h>

Media::DShow::DShowVMR9Filter::DShowVMR9Filter(Media::DShow::DShowManager *mgr, void *hwnd) : Media::DShow::DShowFilter(mgr)
{
	IBaseFilter *cphVMR9 = 0;
	IVMRWindowlessControl9 *cphDisplay = 0;
	IVMRFilterConfig9 * pConfig = NULL; 
	IVMRWindowlessControl9 *pWC = NULL;
	HRESULT hr;
	hr = CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&cphVMR9));
	if (cphVMR9)
	{
		hr = cphVMR9->QueryInterface(IID_PPV_ARGS(&pConfig)); 
		if (pConfig)
		{
			pConfig->SetRenderingMode(VMR9Mode_Windowless);
			cphVMR9->QueryInterface(IID_PPV_ARGS(&pWC));
			if (pWC)
			{
				RECT rc;
				pWC->SetVideoClippingWindow((HWND)hwnd);
				pWC->SetAspectRatioMode(VMR9ARMode_LetterBox);
				GetClientRect((HWND)hwnd, &rc);
				hr = pWC->SetVideoPosition(NULL, &rc);
				pWC->Release();
				pWC = 0;
			}

			pConfig->Release();
			pConfig = 0;
		}
		this->filter = cphVMR9;
	}
}

Media::DShow::DShowVMR9Filter::~DShowVMR9Filter()
{
}

const WChar *Media::DShow::DShowVMR9Filter::GetName()
{
	return L"VMR-9";
}
