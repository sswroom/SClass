#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/CSConverter.h"
#include "Media/DShow/DShowVideoRecvFilter.h"
#include <windows.h>
#include <dshow.h>
#include <streams.h>
#include <dvdmedia.h>
#include <initguid.h>

// {AC5A3F54-8B18-44a9-9C46-0F1182F87531}
DEFINE_GUID(GUID_VideoRecv, 
0xac5a3f54, 0x8b18, 0x44a9, 0x9c, 0x46, 0xf, 0x11, 0x82, 0xf8, 0x75, 0x31);

class VideoRecvFilter : public CBaseVideoRenderer
{
private:
	Media::DShow::DShowVideoRecvFilter::VFrame32Hdlr hdlr;
	void *userObj;
	Media::CS::CSConverter *csConv;
	UInt8 *frameBuff;
	Int32 frameW;
	Int32 frameH;
	Int32 frameFmt;
public:
	VideoRecvFilter(Media::DShow::DShowVideoRecvFilter::VFrame32Hdlr hdlr, void *userObj, HRESULT *hr) : CBaseVideoRenderer(GUID_VideoRecv, L"VideoRecvFilter", 0, hr)
	{
		this->hdlr = hdlr;
		this->userObj = userObj;
		this->csConv = 0;
		this->frameBuff = 0;
		this->frameW = 0;
		this->frameH = 0;
		this->frameFmt = 0;
	}

	virtual ~VideoRecvFilter()
	{
		SDEL_CLASS(this->csConv);
		if (this->frameBuff)
		{
			MemFree(this->frameBuff);
			this->frameBuff = 0;
		}
	}

	virtual HRESULT CheckMediaType(const CMediaType *pmt)
	{
		const GUID *t = pmt->Type();
		if (*t == MEDIATYPE_Video)
		{
			Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
			const GUID *ft = pmt->FormatType();
			if (*ft == FORMAT_VideoInfo)
			{
				VIDEOINFOHEADER *format = (VIDEOINFOHEADER *)pmt->Format();
				Data::ArrayListInt32 csList;
				OSInt i;
				Media::CS::CSConverter::GetSupportedCS(&csList);
				i = csList.GetCount();
				while (i-- > 0)
				{
					if (format->bmiHeader.biCompression == csList.GetItem(i))
					{
						SDEL_CLASS(this->csConv);
						this->csConv = Media::CS::CSConverter::NewConverter(format->bmiHeader.biCompression, format->bmiHeader.biBitCount, Media::FrameInfo::GetDefPixelFormat(format->bmiHeader.biCompression, format->bmiHeader.biBitCount), &color, 0, 32, Media::PF_B8G8R8A8, &color, Media::ColorProfile::YUVT_BT709, 0);
						if (this->csConv)
						{
							this->frameW = format->bmiHeader.biWidth;
							this->frameH = format->bmiHeader.biHeight;
							if (this->frameBuff)
							{
								MemFree(this->frameBuff);
							}
							this->frameBuff = MemAlloc(UInt8, this->frameW * this->frameH * 4);
							return S_OK;
						}
					}
				}
			}
			else if (*ft == FORMAT_VideoInfo2)
			{
				VIDEOINFOHEADER2 *format = (VIDEOINFOHEADER2 *)pmt->Format();
				Data::ArrayListInt32 csList;
				OSInt i;
				Media::CS::CSConverter::GetSupportedCS(&csList);
				i = csList.GetCount();
				while (i-- > 0)
				{
					if (format->bmiHeader.biCompression == csList.GetItem(i))
					{
						if (this->frameFmt == format->bmiHeader.biCompression)
						{
							if (this->frameW == format->bmiHeader.biWidth && this->frameH == format->bmiHeader.biHeight)
							{
								return S_OK;
							}
						}
						else
						{
							SDEL_CLASS(this->csConv);
							this->csConv = Media::CS::CSConverter::NewConverter(format->bmiHeader.biCompression, format->bmiHeader.biBitCount, Media::FrameInfo::GetDefPixelFormat(format->bmiHeader.biCompression, format->bmiHeader.biBitCount), &color, 0, 32, Media::PF_B8G8R8A8, &color, Media::ColorProfile::YUVT_BT709, 0);
						}
						if (this->csConv)
						{
							this->frameW = format->bmiHeader.biWidth;
							this->frameH = format->bmiHeader.biHeight;
							this->frameFmt = format->bmiHeader.biCompression;
							if (this->frameBuff)
							{
								MemFree(this->frameBuff);
							}
							this->frameBuff = MemAlloc(UInt8, this->frameW * this->frameH * 4);
							return S_OK;
						}
					}
				}
			}
		}
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	virtual HRESULT DoRenderSample(IMediaSample *pMediaSample)
	{
		if (this->csConv)
		{
			BYTE *pSample = 0;
			pMediaSample->GetPointer(&pSample);
			if (pSample)
			{
				this->csConv->ConvertV2(&pSample, this->frameBuff, this->frameW, this->frameH, this->frameW, this->frameH, this->frameW << 2, Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);
				LONGLONG startTime;
				LONGLONG endTime;
				pMediaSample->GetMediaTime(&startTime, &endTime);
				this->hdlr(this->userObj, this->frameBuff, 0, this->frameW, this->frameH);
			}
		}
		return S_OK;
	}
};

Media::DShow::DShowVideoRecvFilter::DShowVideoRecvFilter(Media::DShow::DShowManager *mgr, VFrame32Hdlr hdlr, void *userObj) : Media::DShow::DShowFilter(mgr)
{
	HRESULT hr;
	VideoRecvFilter *f = new VideoRecvFilter(hdlr, userObj, &hr);
	IBaseFilter *bf = f;
	this->filter = bf;
}

Media::DShow::DShowVideoRecvFilter::~DShowVideoRecvFilter()
{
}

const WChar *Media::DShow::DShowVideoRecvFilter::GetName()
{
	return L"VideoRecv";
}
