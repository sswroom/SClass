#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/Unit/Distance.h"
#include "Manage/HiResClock.h"
#include "Media/DShow/DShowVideoCapture.h"
#include "Media/DShow/DShowVideoFilter.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

/*#if defined(WIN32_LEAN_AND_MEAN)
#undef WIN32_LEAN_AND_MEAN
#endif*/



/*void DeleteMediaType(AM_MEDIA_TYPE *mt)
{
    if (mt->cbFormat != 0)
    {
        CoTaskMemFree((PVOID)mt->pbFormat);
        mt->cbFormat = 0;
        mt->pbFormat = NULL;
    }
    if (mt->pUnk != NULL)
    {
        // pUnk should not be used.
        mt->pUnk->Release();
        mt->pUnk = NULL;
    }
	CoTaskMemFree(mt);
}*/

Media::DShow::DShowVideoCapture::DShowVideoCapture(IBaseFilter *baseFilter, IPropertyBag *pPropBag, const WChar *devName, const WChar *displayName)
{
	this->devName = Text::StrCopyNew(devName);
	if (displayName)
	{
		this->displayName = Text::StrCopyNew(displayName);
	}
	else
	{
		this->displayName = 0;
	}
	this->baseFilter = baseFilter;
	this->pPropBag = pPropBag;
	this->captureFilter = 0;
	this->graph = 0;
	this->cb = 0;

	IGraphBuilder *pGraph = NULL;
	HRESULT hr =  CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);
	if (SUCCEEDED(hr))
	{
		NEW_CLASS(captureFilter, Media::DShow::DShowVideoFilter(false));
		pGraph->AddFilter(baseFilter, L"BaseFilter");
		pGraph->AddFilter((DShowVideoFilter*)captureFilter, L"DShowVideoCapture");
		IEnumPins *enums;
		if (((IBaseFilter*)baseFilter)->EnumPins(&enums) == S_OK)
		{
			IPin *pin;
			enums->Next(1, &pin, 0);
			this->pin1 = pin;
			this->pin2 = ((DShowVideoFilter*)captureFilter)->GetPin(0);
			pGraph->Connect(pin1, pin2);
			enums->Release();
		}
		this->graph = pGraph;
	}
}

Media::DShow::DShowVideoCapture::~DShowVideoCapture()
{
	if (this->baseFilter)
	{
		this->baseFilter->Stop();
	}
	if (this->captureFilter)
	{
		this->captureFilter->Stop();
	}
	if (graph)
	{
		((IGraphBuilder*)graph)->Release();
	}
	if (this->baseFilter)
	{
		//this->baseFilter->Release();
	}
	if (this->pPropBag)
	{
		this->pPropBag->Release();
	}
	SDEL_TEXT(this->devName);
	SDEL_TEXT(this->displayName);
}

UTF8Char *Media::DShow::DShowVideoCapture::GetSourceName(UTF8Char *buff)
{
/*	FILTER_INFO finfo;
	if (this->baseFilter->QueryFilterInfo(&finfo) != S_OK)
		return 0;
	return Text::StrConcat(buff, finfo.achName);*/
	return Text::StrWChar_UTF8(buff, this->devName);
}

const UTF8Char *Media::DShow::DShowVideoCapture::GetFilterName()
{
	return (const UTF8Char*)"DShowVideoCapture";
}

Bool Media::DShow::DShowVideoCapture::GetVideoInfo(Media::FrameInfo *info, UInt32 *frameRateNorm, UInt32 *frameRateDenorm, UOSInt *maxFrameSize)
{
	if (captureFilter == 0)
	{
		info->fourcc = 0;
		return false;
	}
	DShowVideoFilter *filter = (DShowVideoFilter*)captureFilter;
	CBasePin *pin = filter->GetPin(0);
	AM_MEDIA_TYPE mt;
	if (pin->ConnectionMediaType(&mt) == S_OK)
	{
		if (mt.formattype == FORMAT_VideoInfo)
		{
			VIDEOINFOHEADER *format = (VIDEOINFOHEADER *)mt.pbFormat;
			info->fourcc = format->bmiHeader.biCompression;
			if (info->fourcc == 0)
			{
				info->fourcc = *(UInt32*)"DIBS";
			}
			info->storeBPP = format->bmiHeader.biBitCount;
			info->pf = Media::PixelFormatGetDef(format->bmiHeader.biCompression, format->bmiHeader.biBitCount);
			info->byteSize = format->bmiHeader.biSizeImage;
			info->ftype = Media::FT_NON_INTERLACE;
			info->dispWidth = (ULONG)format->bmiHeader.biWidth;
			info->dispHeight = (ULONG)format->bmiHeader.biHeight;
			info->storeWidth = info->dispWidth;
			info->storeHeight = info->dispHeight;
			info->par2 = 1;
			info->hdpi = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, format->bmiHeader.biXPelsPerMeter);
			info->color->SetCommonProfile(Media::ColorProfile::CPT_VUNKNOWN);
			info->yuvType = Media::ColorProfile::YUVT_UNKNOWN;
			info->ycOfst = Media::YCOFST_C_TOP_LEFT;

			if ((::Math::Double2Int32(1000000000 / (Double)format->AvgTimePerFrame) % 100) == 0)
			{
				*frameRateNorm = (UInt32)::Math::Double2Int32(10000000 / (Double)format->AvgTimePerFrame);
				*frameRateDenorm = 1;
			}
			else if ((::Math::Double2Int32(1001000000 / (Double)format->AvgTimePerFrame) % 100) == 0)
			{
				*frameRateNorm = (UInt32)::Math::Double2Int32(10010000000 / (Double)format->AvgTimePerFrame);
				*frameRateDenorm = 1001;
			}
			else
			{
				*frameRateNorm = 10000000;
				*frameRateDenorm = (UInt32)format->AvgTimePerFrame;
			}
			if (info->byteSize == 0)
			{
				*maxFrameSize = MulDivUOS(info->dispWidth * info->dispHeight, filter->GetFrameMul(), 8);
			}
			else
			{
				*maxFrameSize = info->byteSize;
			}
		}
		else
		{
			info->fourcc = 0;
			*maxFrameSize = 0;
		}
	}
	else
	{
		info->fourcc = 0;
		*maxFrameSize = 0;
	}
	return true;
}

void Media::DShow::DShowVideoCapture::SetPreferSize(UOSInt width, UOSInt height, UInt32 fourcc, UInt32 bpp, UInt32 frameRateNumer, UInt32 fraemRateDenom)
{
	Bool found = false;
	UInt32 minRate = 0;
	IEnumMediaTypes *mediaTypes;
	UInt32 fcc = 0;

	UOSInt nearDiff = (width + 1) * (height + 1);
	UOSInt nearWidth = 0;
	UOSInt nearHeight = 0;
	UInt32 nearRate = 0;
	UOSInt thisDiff;
	UOSInt absWidth;
	UOSInt absHeight;
	AM_MEDIA_TYPE *foundMediaType = 0;

	if (this->pin1->EnumMediaTypes(&mediaTypes) == S_OK)
	{
		AM_MEDIA_TYPE *mediaType;
		while (mediaTypes->Next(1, &mediaType, 0) == S_OK)
		{
			Bool isMatch = false;
			if (mediaType->formattype == FORMAT_VideoInfo)
			{
				VIDEOINFOHEADER *format = (VIDEOINFOHEADER *)mediaType->pbFormat;
				if (format->bmiHeader.biCompression == fourcc && format->bmiHeader.biBitCount == bpp)
				{
					if (format->bmiHeader.biWidth == (LONG)width && format->bmiHeader.biHeight == (LONG)height)
					{
						if (found)
						{
							if ((UInt32)format->AvgTimePerFrame < minRate)
							{
								minRate = (UInt32)format->AvgTimePerFrame;
								fcc = format->bmiHeader.biCompression;
								isMatch = true;
							}
							else if (format->AvgTimePerFrame == minRate)
							{
								if (fcc == 0)
								{
									fcc = format->bmiHeader.biCompression;
									isMatch = true;
								}
							}
						}
						else
						{
							found = true;
							minRate = (UInt32)format->AvgTimePerFrame;
							fcc = format->bmiHeader.biCompression;
							isMatch = true;
						}
					}
					else if (!found)
					{
						if (format->bmiHeader.biWidth > (LONG)width)
						{
							absWidth = (ULONG)format->bmiHeader.biWidth - width + 1;
						}
						else
						{
							absWidth = width - (ULONG)format->bmiHeader.biWidth + 1;
						}
						if (format->bmiHeader.biHeight > (LONG)height)
						{
							absHeight = (ULONG)format->bmiHeader.biHeight - height + 1;
						}
						else
						{
							absHeight = height - (ULONG)format->bmiHeader.biHeight + 1;
						}
						thisDiff = absWidth * absHeight;
						if (nearDiff > thisDiff)
						{
							nearDiff = thisDiff;
							nearWidth = (ULONG)format->bmiHeader.biWidth;
							nearHeight = (ULONG)format->bmiHeader.biHeight;
							nearRate = (UInt32)format->AvgTimePerFrame;
							fcc = format->bmiHeader.biCompression;
							isMatch = true;
						}
						else if (nearDiff == thisDiff)
						{
							if (format->AvgTimePerFrame < nearRate)
							{
								nearRate = (UInt32)format->AvgTimePerFrame;
								fcc = format->bmiHeader.biCompression;
								nearWidth = (ULONG)format->bmiHeader.biWidth;
								nearHeight = (ULONG)format->bmiHeader.biHeight;
								isMatch = true;
							}
						}
					}
				}
			}
			if (isMatch)
			{
				if (foundMediaType)
				{
					DeleteMediaType(foundMediaType);
				}
				foundMediaType = mediaType;
			}
			else
			{
				DeleteMediaType(mediaType);
			}
		}
		mediaTypes->Release();
	}

	if (found)
	{
		((Media::DShow::DShowVideoFilter*)captureFilter)->SetPreferSize(width, height, minRate, fcc, bpp);
		if (foundMediaType)
		{
			AM_MEDIA_TYPE connMT;
			graph->Reconnect(pin1);
			if (pin2->ConnectionMediaType(&connMT) == S_OK)
			{
				FreeMediaType(connMT);
			}
			else
			{
				graph->ConnectDirect(pin1, pin2, foundMediaType);
			}
			DeleteMediaType(foundMediaType);
		}
		else
		{
			graph->Reconnect(pin1);
		}
	}
	else if (nearWidth != 0 && nearHeight != 0)
	{
		((Media::DShow::DShowVideoFilter*)captureFilter)->SetPreferSize(nearWidth, nearHeight, nearRate, fcc, bpp);
		if (foundMediaType)
		{
			AM_MEDIA_TYPE connMT;
			graph->Reconnect(pin1);
			if (pin2->ConnectionMediaType(&connMT) == S_OK)
			{
				FreeMediaType(connMT);
			}
			else
			{
				graph->ConnectDirect(pin1, pin2, foundMediaType);
			}
			DeleteMediaType(foundMediaType);
		}
		else
		{
			graph->Reconnect(pin1);
		}
	}
	else
	{
		if (foundMediaType)
		{
			DeleteMediaType(foundMediaType);
		}
	}
}

Bool Media::DShow::DShowVideoCapture::Init(FrameCallback cb, FrameChangeCallback fcCb, void *userData)
{
	this->captureFilter->SetFrameCallback(cb, fcCb, userData);
	return true;
}

Bool Media::DShow::DShowVideoCapture::Start()
{
	HRESULT hres1;
	HRESULT hres2;
	this->captureFilter->BeginCapture();
	hres1 = this->captureFilter->Run(0);
	if (hres1 == S_OK)
	{
		hres2 = this->baseFilter->Run(0);
		if (hres2 == S_OK)
		{
			return true;
		}
		else
		{
			this->captureFilter->Stop();
			return false;
		}
	}
	return false;
}

void Media::DShow::DShowVideoCapture::Stop()
{
	if (this->baseFilter)
	{
		this->baseFilter->Stop();
	}
	if (this->captureFilter)
	{
		this->captureFilter->Stop();
	}
}

Bool Media::DShow::DShowVideoCapture::IsRunning()
{
	FILTER_STATE state;
	if (this->baseFilter->GetState(500, &state) != S_OK)
		return false;
	return state == State_Running;
}

UOSInt Media::DShow::DShowVideoCapture::GetDataSeekCount()
{
	return 0;
}

Bool Media::DShow::DShowVideoCapture::HasFrameCount()
{
	return false;
}

UOSInt Media::DShow::DShowVideoCapture::GetFrameCount()
{
	return 0;
}

UInt32 Media::DShow::DShowVideoCapture::GetFrameTime(UOSInt frameIndex)
{
	return 0;
}

void Media::DShow::DShowVideoCapture::EnumFrameInfos(FrameInfoCallback cb, void *userData)
{
}

UOSInt Media::DShow::DShowVideoCapture::GetSupportedFormats(VideoFormat *fmtArr, UOSInt maxCnt)
{
	if (captureFilter == 0)
	{
		return 0;
	}
	UOSInt outCnt = 0;
	IEnumMediaTypes *mediaTypes;
	AM_MEDIA_TYPE *mediaType;
	if (this->pin1->EnumMediaTypes(&mediaTypes) == S_OK)
	{
		HRESULT hres;
		while (outCnt < maxCnt && (hres = mediaTypes->Next(1, &mediaType, 0)) == S_OK)
		{
			if (mediaType->formattype == FORMAT_VideoInfo)
			{
				VIDEOINFOHEADER *format = (VIDEOINFOHEADER *)mediaType->pbFormat;

				if ((::Math::Double2Int32(1000000000 / (Double)format->AvgTimePerFrame) % 100) == 0)
				{
					fmtArr[outCnt].frameRateNorm = (UInt32)::Math::Double2Int32(10000000 / (Double)format->AvgTimePerFrame);
					fmtArr[outCnt].frameRateDenorm = 1;
				}
				else if ((::Math::Double2Int32(1001000000 / (Double)format->AvgTimePerFrame) % 100) == 0)
				{
					fmtArr[outCnt].frameRateNorm = (UInt32)::Math::Double2Int32(10010000000 / (Double)format->AvgTimePerFrame);
					fmtArr[outCnt].frameRateDenorm = 1001;
				}
				else
				{
					fmtArr[outCnt].frameRateNorm = 10000000;
					fmtArr[outCnt].frameRateDenorm = (UInt32)format->AvgTimePerFrame;
				}
				fmtArr[outCnt].info.dispWidth = (ULONG)format->bmiHeader.biWidth;
				fmtArr[outCnt].info.dispHeight = (ULONG)format->bmiHeader.biHeight;
				fmtArr[outCnt].info.storeWidth = fmtArr[outCnt].info.dispWidth;
				fmtArr[outCnt].info.storeHeight = fmtArr[outCnt].info.dispWidth;
				fmtArr[outCnt].info.fourcc = format->bmiHeader.biCompression;
				fmtArr[outCnt].info.storeBPP = format->bmiHeader.biBitCount;
				fmtArr[outCnt].info.pf = Media::PixelFormatGetDef(format->bmiHeader.biCompression, format->bmiHeader.biBitCount);
				fmtArr[outCnt].info.byteSize = format->bmiHeader.biSizeImage;
				fmtArr[outCnt].info.ftype = Media::FT_NON_INTERLACE;
				fmtArr[outCnt].info.par2 = 1;
				fmtArr[outCnt].info.hdpi = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, format->bmiHeader.biXPelsPerMeter);
				fmtArr[outCnt].info.color->SetCommonProfile(Media::ColorProfile::CPT_VUNKNOWN);
				fmtArr[outCnt].info.yuvType = Media::ColorProfile::YUVT_UNKNOWN;
				fmtArr[outCnt].info.ycOfst = Media::YCOFST_C_CENTER_LEFT;

				outCnt++;
			}
			DeleteMediaType(mediaType);
		}
		mediaTypes->Release();
	}
	return outCnt;
}

void Media::DShow::DShowVideoCapture::GetInfo(Text::StringBuilderUTF *sb)
{
	if (this->baseFilter)
	{
		FILTER_INFO finfo;
		WChar *vendorName;
		if (S_OK == this->baseFilter->QueryFilterInfo(&finfo))
		{
			sb->Append((const UTF8Char*)"Base Filter Name: ");
			const UTF8Char *csptr = Text::StrToUTF8New(finfo.achName);
			sb->Append(csptr);
			Text::StrDelNew(csptr);
			sb->Append((const UTF8Char*)"\r\n");
			if (finfo.pGraph)
			{
				finfo.pGraph->Release();
			}
		}

		if (S_OK == this->baseFilter->QueryVendorInfo(&vendorName))
		{
			sb->Append((const UTF8Char*)"Vendor Info: ");
			const UTF8Char *csptr = Text::StrToUTF8New(vendorName);
			sb->Append(csptr);
			Text::StrDelNew(csptr);
			sb->Append((const UTF8Char*)"\r\n");
		}
	}
	if (this->pPropBag)
	{
		VARIANT var;
		HRESULT hr;
		VariantInit(&var);

		hr = this->pPropBag->Read(L"Description", &var, 0);
		if (SUCCEEDED(hr))
		{
			sb->Append((const UTF8Char*)"Description: ");
			if (var.vt == VT_BSTR)
			{
				const UTF8Char *csptr = Text::StrToUTF8New(var.bstrVal);
				sb->Append(csptr);
				Text::StrDelNew(csptr);
			}
			sb->Append((const UTF8Char*)"\r\n");
			VariantClear(&var);
		}

		hr = this->pPropBag->Read(L"FriendlyName", &var, 0);
		if (SUCCEEDED(hr))
		{
			sb->Append((const UTF8Char*)"FriendlyName: ");
			if (var.vt == VT_BSTR)
			{
				const UTF8Char *csptr = Text::StrToUTF8New(var.bstrVal);
				sb->Append(csptr);
				Text::StrDelNew(csptr);
			}
			sb->Append((const UTF8Char*)"\r\n");
			VariantClear(&var);
		}

		hr = this->pPropBag->Read(L"CLSID", &var, 0);
		if (SUCCEEDED(hr))
		{
			sb->Append((const UTF8Char*)"CLSID: ");
			if (var.vt == VT_BSTR)
			{
				const UTF8Char *csptr = Text::StrToUTF8New(var.bstrVal);
				sb->Append(csptr);
				Text::StrDelNew(csptr);
			}
			sb->Append((const UTF8Char*)"\r\n");
			VariantClear(&var);
		}
	}
	if (this->displayName)
	{
		sb->Append((const UTF8Char*)"DisplayName: ");
		const UTF8Char *csptr = Text::StrToUTF8New(this->displayName);
		sb->Append(csptr);
		Text::StrDelNew(csptr);
		sb->Append((const UTF8Char*)"\r\n");
	}
}

Media::DShow::DShowVideoCaptureMgr::DShowVideoCaptureMgr()
{
    ICreateDevEnum *pDevEnum;
	IEnumMoniker *pEnum;

	CoInitializeEx(0, COINIT_MULTITHREADED);
	this->pEnum = 0;

    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (LPVOID*)&pDevEnum);

    if (SUCCEEDED(hr))
    {
        hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
        if (hr == S_FALSE)
        {
            hr = VFW_E_NOT_FOUND;  // The category is empty. Treat as an error.
        }
		else
		{
			this->pEnum = pEnum;
		}
        pDevEnum->Release();
    }

}

Media::DShow::DShowVideoCaptureMgr::~DShowVideoCaptureMgr()
{
	if (pEnum)
	{
		((IEnumMoniker*)pEnum)->Release();
	}
	CoUninitialize();
}

UOSInt Media::DShow::DShowVideoCaptureMgr::GetDeviceCount()
{
	IEnumMoniker *pEnum = (IEnumMoniker*)this->pEnum;
	IMoniker *pMoniker;
	UOSInt cnt;
	if (pEnum == 0)
	{
		return 0;
	}
	pEnum->Reset();
	cnt = 0;
	while (pEnum->Next(1, &pMoniker, 0) == S_OK)
	{
        pMoniker->Release();
		cnt++;
	}
	return cnt;
}

UTF8Char *Media::DShow::DShowVideoCaptureMgr::GetDeviceName(UTF8Char *buff, UOSInt devNo)
{
	IEnumMoniker *pEnum = (IEnumMoniker*)this->pEnum;
	IMoniker *pMoniker;
	IPropertyBag *pPropBag;
	UInt32 cnt;
	UTF8Char *sptr = 0;

	if (pEnum == 0)
	{
		return 0;
	}
	pEnum->Reset();
	cnt = 0;
	while (pEnum->Next(1, &pMoniker, 0) == S_OK)
	{
		if (devNo == cnt)
		{
	        HRESULT hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (LPVOID*)&pPropBag);
			if (FAILED(hr))
			{
				pMoniker->Release();
				return 0;
			} 
			VARIANT var;
			VariantInit(&var);

			hr = pPropBag->Read(L"Description", &var, 0);
			if (FAILED(hr))
			{
				hr = pPropBag->Read(L"FriendlyName", &var, 0);
			}
			if (SUCCEEDED(hr))
			{
				sptr = Text::StrWChar_UTF8(buff, var.bstrVal);
				VariantClear(&var);
			}

	        pPropBag->Release();
			pMoniker->Release();
			return sptr;
		}
        pMoniker->Release();
		cnt++;
	}
	return 0;
}

WChar *Media::DShow::DShowVideoCaptureMgr::GetDeviceId(WChar *buff, UOSInt devNo)
{
	IEnumMoniker *pEnum = (IEnumMoniker*)this->pEnum;
	IMoniker *pMoniker;
	IPropertyBag *pPropBag;
	UInt32 cnt;
	WChar *sptr = 0;

	if (pEnum == 0)
	{
		return 0;
	}
	pEnum->Reset();
	cnt = 0;
	while (pEnum->Next(1, &pMoniker, 0) == S_OK)
	{
		if (devNo == cnt)
		{
	        HRESULT hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (LPVOID*)&pPropBag);
			if (FAILED(hr))
			{
				pMoniker->Release();
				return 0;
			} 
			VARIANT var;
			VariantInit(&var);

			hr = pPropBag->Read(L"DevicePath", &var, 0);
			if (SUCCEEDED(hr))
			{
				sptr = Text::StrConcat(buff, var.bstrVal);
				VariantClear(&var);
			}

	        pPropBag->Release();
			pMoniker->Release();
			return sptr;
		}
        pMoniker->Release();
		cnt++;
	}
	return 0;
}

Media::DShow::DShowVideoCapture *Media::DShow::DShowVideoCaptureMgr::GetDevice(UOSInt devNo)
{
	IEnumMoniker *pEnum = (IEnumMoniker*)this->pEnum;
	IMoniker *pMoniker;
	IPropertyBag *pPropBag;
	UOSInt cnt;
	WChar sbuff[256];

	if (pEnum == 0)
	{
		return 0;
	}
	pEnum->Reset();
	cnt = 0;
	while (pEnum->Next(1, &pMoniker, 0) == S_OK)
	{
		if (devNo == cnt)
		{
			IBaseFilter *pCap = 0;
			Media::DShow::DShowVideoCapture *capture;

	        HRESULT hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (LPVOID*)&pPropBag);
			if (FAILED(hr))
			{
				pMoniker->Release();
				return 0;
			} 
			VARIANT var;
			VariantInit(&var);

			sbuff[0] = 0;

			hr = pPropBag->Read(L"Description", &var, 0);
			if (FAILED(hr))
			{
				hr = pPropBag->Read(L"FriendlyName", &var, 0);
			}
			if (SUCCEEDED(hr))
			{
				Text::StrConcat(sbuff, var.bstrVal);
				VariantClear(&var);
			}

			hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (LPVOID*)&pCap);
			if (FAILED(hr))
			{
				pMoniker->Release();
		        pPropBag->Release();
				return 0;
			} 

			LPOLESTR dispName;
			IMalloc *mem;
			CoGetMalloc(1, &mem);
			if (SUCCEEDED(pMoniker->GetDisplayName(0, 0, &dispName)))
			{
				NEW_CLASS(capture, Media::DShow::DShowVideoCapture(pCap, pPropBag, sbuff, dispName));
				mem->Free(dispName);
			}
			else
			{
				NEW_CLASS(capture, Media::DShow::DShowVideoCapture(pCap, pPropBag, sbuff, 0));
			}
			pMoniker->Release();
			return capture;
		}
        pMoniker->Release();
		cnt++;
	}
	return 0;
}
