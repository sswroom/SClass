#ifndef _SM_MEDIA_DSHOW_DSHOWVIDEOFILTER
#define _SM_MEDIA_DSHOW_DSHOWVIDEOFILTER

#include "Manage/HiResClock.h"
#include "Media/H264Parser.h"
#include "Media/IRealtimeVideoSource.h"
#include "Math/Math.h"

#include "Media/DShow/DShowInit.h"
#include <windows.h>
#include <dshow.h>
#include <streams.h>
#include <dvdmedia.h>
#include <initguid.h>
#if defined(_MSC_VER)
#pragma comment(lib, "strmiids")
#endif

namespace Media
{
	namespace DShow
	{
		// {92FC5435-4F65-4d85-8C37-832AA71D0F45}
		DEFINE_GUID(DShowVideoFilterGUID, 0x92fc5435, 0x4f65, 0x4d85, 0x8c, 0x37, 0x83, 0x2a, 0xa7, 0x1d, 0xf, 0x45);

		class DShowVideoFilter : public CBaseRenderer
		{
		private:
			Int32 initRes;
			UOSInt preferWidth;
			UOSInt preferHeight;
			UInt32 preferRate;
			UInt32 preferFCC;
			UInt32 preferBPP;
			UInt32 frameNum;
			UInt32 frameMul;
			
			UInt32 frameFCC;
			UInt32 frameWidth;
			UInt32 frameHeight;
			UInt32 frameSize;
			Manage::HiResClock *clk;
			Media::IRealtimeVideoSource::FrameCallback cb;
			Media::IRealtimeVideoSource::FrameChangeCallback fcCb;
			void *ud;

		public:
			DShowVideoFilter(Bool useSourceTime) : CBaseRenderer(DShowVideoFilterGUID, L"DShowVideoFilter", 0, (HRESULT*)&initRes)
			{
				this->cb = 0;
				this->fcCb = 0;
				this->ud = 0;
				this->preferWidth = 0;
				this->preferHeight = 0;
				this->preferRate = 0;
				this->preferFCC = (UInt32)-1;
				this->preferBPP = 0;
				this->frameMul = 8;
				this->frameNum = 0;
				this->frameFCC = 0;
				if (useSourceTime)
				{
					this->clk = 0;
				}
				else
				{
					NEW_CLASS(clk, Manage::HiResClock());
				}

			}

			~DShowVideoFilter()
			{
				if (clk)
				{
					DEL_CLASS(clk);
				}
			}

			virtual HRESULT CheckMediaType(const CMediaType *pmt)
			{
				if (*pmt->FormatType() != FORMAT_VideoInfo)
				{
					return S_FALSE;
				}
				VIDEOINFOHEADER *format = (VIDEOINFOHEADER *)pmt->Format();

				if (this->preferWidth)
				{
					if ((ULONG)format->bmiHeader.biWidth != this->preferWidth)
						return S_FALSE;
				}
				this->frameWidth = (ULONG)format->bmiHeader.biWidth;
				if (this->preferHeight)
				{
					if ((ULONG)format->bmiHeader.biHeight != this->preferHeight)
						return S_FALSE;
				}
				this->frameHeight = (ULONG)format->bmiHeader.biHeight;
				if (this->preferRate)
				{
					if (format->AvgTimePerFrame != this->preferRate)
						return S_FALSE;
				}
				if (this->preferBPP)
				{
					if (format->bmiHeader.biBitCount != this->preferBPP)
						return S_FALSE;
				}
				this->frameFCC = format->bmiHeader.biCompression;
				if (this->preferFCC != 0xFFFFFFFF)
				{
					if (this->preferFCC != format->bmiHeader.biCompression)
						return S_FALSE;
					return S_OK;
				}

				if (format->bmiHeader.biCompression == 0)
				{
					if (format->bmiHeader.biBitCount == 16)
					{
						this->frameMul = 16;
						return S_OK;
					}
					else if (format->bmiHeader.biBitCount == 24)
					{
						this->frameMul = 24;
						return S_OK;
					}
					else if (format->bmiHeader.biBitCount == 32)
					{
						this->frameMul = 32;
						return S_OK;
					}
					return S_FALSE;
				}
				else if (format->bmiHeader.biCompression == *(UInt32*)"YUY2")
				{
					this->frameMul = 16;
					return S_OK;
				}
				else if (format->bmiHeader.biCompression == *(UInt32*)"UYVY")
				{
					this->frameMul = 16;
					return S_OK;
				}
				else if (format->bmiHeader.biCompression == *(UInt32*)"I420")
				{
					this->frameMul = 12;
					return S_OK;
				}
				else if (format->bmiHeader.biCompression == *(UInt32*)"MJPG")
				{
					this->frameMul = 32;
					return S_OK;
				}
				else if (format->bmiHeader.biCompression == *(UInt32*)"H264")
				{
					this->frameMul = 32;
					return S_OK;
				}
				return S_FALSE;
			}

			virtual HRESULT DoRenderSample(IMediaSample *sample)
			{
				BYTE *dataPtr;
				if (sample->GetPointer(&dataPtr) == S_OK)
				{
					UInt32 frameSize = (UInt32)sample->GetSize();
					if (frameSize > this->frameWidth * this->frameHeight * this->frameMul >> 3)
					{
						frameSize = this->frameWidth * this->frameHeight * this->frameMul >> 3;
					}
					Media::FrameType ftype = Media::FT_NON_INTERLACE;
					Media::IVideoSource::FrameStruct fs = Media::IVideoSource::FS_I;
					AM_MEDIA_TYPE *mediaType;
					if (sample->GetMediaType(&mediaType) == S_OK)
					{
						if (mediaType->formattype == FORMAT_VideoInfo2)
						{
							VIDEOINFOHEADER2 *fmt = (VIDEOINFOHEADER2*)mediaType->pbFormat;
							if ((fmt->dwInterlaceFlags & AMINTERLACE_IsInterlaced) == 0)
							{
								ftype = Media::FT_NON_INTERLACE;
							}
							else
							{
								if ((fmt->dwInterlaceFlags & AMINTERLACE_1FieldPerSample) != 0)
								{
									if ((fmt->dwInterlaceFlags & AMINTERLACE_Field1First) != 0)
									{
										ftype = Media::FT_FIELD_BF;
									}
									else
									{
										ftype = Media::FT_FIELD_TF;
									}
								}
								else
								{
									if ((fmt->dwInterlaceFlags & AMINTERLACE_Field1First) != 0)
									{
										ftype = Media::FT_INTERLACED_BFF;
									}
									else
									{
										ftype = Media::FT_INTERLACED_TFF;
									}
								}
							}
						}
						FreeMediaType(*mediaType);
					}
					else if (this->frameFCC == *(UInt32*)"H264")
					{
						OSInt i = 0;
						OSInt j = frameSize - 4;
						while (i < j)
						{
							if (dataPtr[i] == 0 && dataPtr[i+1] == 0 && dataPtr[i+2] == 0 && dataPtr[i+3] == 1)
							{
								if ((dataPtr[i + 4] & 0x1f) == 7) //sps
								{
								}
								else if ((dataPtr[i + 4] & 0x1f) == 8) //pps
								{
								}
								else if ((dataPtr[i + 4] & 0x1f) == 5) //idr
								{
									fs = Media::IVideoSource::FS_I;
									break;
								}
								else if ((dataPtr[i + 4] & 0x1f) == 1)
								{
									fs = Media::IVideoSource::FS_P;
									break;
								}
								else
								{
									i = 0;
									break;
								}
								i += 4;
							}
							i++;
						}
					}
					
					Int32 t;
					if (this->clk)
					{
						t = Double2Int32(clk->GetTimeDiff() * 1000);
					}
					else
					{
						Int64 t1 = 0;
						Int64 t2;
						sample->GetTime(&t1, &t2);
						t = (Int32)(t1 / 10);
					}
					cb((UInt32)t, this->frameNum++, &dataPtr, frameSize, fs, ud, ftype, (sample->IsDiscontinuity() == S_OK)?((Media::IVideoSource::FrameFlag)(Media::IVideoSource::FF_DISCONTTIME | Media::IVideoSource::FF_REALTIME)):(Media::IVideoSource::FF_REALTIME), Media::YCOFST_C_CENTER_LEFT);
					return S_OK;
				}
				else
				{
					return S_FALSE;
				}
			}

			void SetPreferSize(UOSInt width, UOSInt height, UInt32 rate, UInt32 fcc, UInt32 bpp)
			{
				this->preferWidth = width;
				this->preferHeight = height;
				this->preferRate = rate;
				this->preferFCC = fcc;
				this->preferBPP = bpp;
			}

			void SetFrameCallback(Media::IRealtimeVideoSource::FrameCallback cb, Media::IRealtimeVideoSource::FrameChangeCallback fcCb, void *ud)
			{
				this->cb = cb;
				this->fcCb = fcCb;
				this->ud = ud;
			}

			void BeginCapture()
			{
				if (this->clk)
				{
					this->clk->Start();
				}
			}

			UInt32 GetFrameMul()
			{
				return this->frameMul;
			}
		};
	}
}

#endif
