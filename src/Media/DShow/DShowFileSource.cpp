#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/DShow/DShowFileSource.h"

#include <dvdmedia.h>
//#include <MpegType.h>

// {65788D69-909B-4c03-B0BE-39E3A881C9DB}
DEFINE_GUID(DShowSourceCaptureGUID, 0x65788d69, 0x909b, 0x4c03, 0xb0, 0xbe, 0x39, 0xe3, 0xa8, 0x81, 0xc9, 0xdb);

Media::DShow::DShowVideoPin::DShowVideoPin(DShowSourceCapture *cap, const WChar *pinName, HRESULT *res) : CBaseInputPin(L"VPin", cap, cap->GetLock(), res, pinName)
{
	this->cap = cap;
}

Media::DShow::DShowVideoPin::~DShowVideoPin()
{
}

HRESULT Media::DShow::DShowVideoPin::CheckMediaType(const CMediaType *pmt)
{
	if (*pmt->FormatType() == FORMAT_VideoInfo)
	{
		return S_OK;
	}
	else if (*pmt->FormatType() == FORMAT_VideoInfo2)
	{
		return S_OK;
	}
	return S_FALSE;
}

HRESULT Media::DShow::DShowVideoPin::GetMediaType(int iPosition, CMediaType *pMediaType)
{
	pMediaType->SetFormatType(&FORMAT_VideoInfo2);
	////////////////////////////////////
	return S_FALSE;
}

Media::DShow::DShowSourceCapture::DShowSourceCapture() : CBaseFilter(L"Source Capture", 0, &lock, DShowSourceCaptureGUID)
{
	NN<DShowVideoPin> pin;
	HRESULT res;
	NEW_CLASSNN(this->vPins, Data::ArrayListNN<DShowVideoPin>());
	NEW_CLASSNN(pin, DShowVideoPin(this, L"Pin1", &res));
	this->vPins->Add(pin);
}

Media::DShow::DShowSourceCapture::~DShowSourceCapture()
{
	IntOS i = this->vPins->GetCount();;
	NN<DShowVideoPin> pin;
	while (i-- > 0)
	{
		pin = this->vPins->GetItemNoCheck(i);
		pin.Delete();
	}
	this->vPins.Delete();
}

CBasePin* Media::DShow::DShowSourceCapture::GetPin(int n)
{
	return this->vPins->GetItem(n).OrNull();
}

int Media::DShow::DShowSourceCapture::GetPinCount()
{
	return (int)this->vPins->GetCount();
}

CCritSec *Media::DShow::DShowSourceCapture::GetLock()
{
	return &this->lock;
}

Media::DShow::DShowFileSource::DShowFileSource(UnsafeArray<const WChar> fileName)
{
	IGraphBuilder *pGraph = NULL;
	this->capFilter = 0;
	CoInitializeEx(0, COINIT_MULTITHREADED);
	HRESULT hr =  CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);
	if (SUCCEEDED(hr))
	{
		NEW_CLASS(this->capFilter, Media::DShow::DShowSourceCapture());
		pGraph->AddFilter(this->capFilter, L"Source Capture");
		if (pGraph->RenderFile(fileName.Ptr(), 0) == S_OK)
		{
			hr = 0;
		}

/*		IBaseFilter *pFilter = 0;
		pGraph->AddSourceFilter(fileName, 0, &pFilter);

		if (pFilter)
		{
			IEnumPins *pEnum;
			IPin *pPin;
			IEnumMediaTypes *pEnumType;
			AM_MEDIA_TYPE *mediaType;

			if (pFilter->EnumPins(&pEnum) == S_OK)
			{
				while (pEnum->Next(1, &pPin, 0) == S_OK)
				{
					if (pPin->EnumMediaTypes(&pEnumType) == S_OK)
					{
						while (pEnumType->Next(1, &mediaType, 0) == S_OK)
						{
							if (mediaType->formattype == FORMAT_VideoInfo)
							{
								VIDEOINFOHEADER *vInfo = (VIDEOINFOHEADER *)mediaType->pbFormat;
								Int32 comp = vInfo->bmiHeader.biCompression;
							}
							else if (mediaType->formattype == FORMAT_VideoInfo2)
							{
								VIDEOINFOHEADER2 *vInfo = (VIDEOINFOHEADER2 *)mediaType->pbFormat;
								Int32 comp = vInfo->bmiHeader.biCompression;
							}
							else if (mediaType->formattype == FORMAT_MPEGVideo)
							{
								MPEG1VIDEOINFO *vInfo = (MPEG1VIDEOINFO *)mediaType->pbFormat;
								Int32 comp = vInfo->hdr.bmiHeader.biCompression;
							}
							else if (mediaType->formattype == FORMAT_MPEG2Video)
							{
								MPEG2VIDEOINFO *vInfo = (MPEG2VIDEOINFO *)mediaType->pbFormat;
								Int32 comp = vInfo->hdr.bmiHeader.biCompression;
							}
							else if (mediaType->formattype == FORMAT_DvInfo)
							{
								DVINFO *vInfo = (DVINFO *)mediaType->pbFormat;
							}
							else if (mediaType->formattype == FORMAT_WaveFormatEx)
							{
								WAVEFORMATEX *vInfo = (WAVEFORMATEX *)mediaType->pbFormat;
							}
							else
							{
							}
							DeleteMediaType(mediaType);
						}
						pEnumType->Release();
					}
				}
				pEnum->Release();
			}
			pFilter->Release();
		}*/
		pGraph->Release();
//		DEL_CLASS(this->capFilter);
	}
}

Media::DShow::DShowFileSource::~DShowFileSource()
{
	CoUninitialize();
}
