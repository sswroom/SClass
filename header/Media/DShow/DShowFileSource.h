#ifndef _SM_MEDIA_DSHOW_DSHOWFILESOURCE
#define _SM_MEDIA_DSHOW_DSHOWFILESOURCE
#include "Media/FrameInfo.h"
#include "Data/ArrayListNN.hpp"

__interface IPin;
__interface IGraphBuilder;
__interface IBaseFilter;

#include <windows.h>
#include <dshow.h>
#include <initguid.h>
#include <streams.h>

namespace Media
{
	namespace DShow
	{

		class DShowSourceCapture;
		class DShowVideoPin : public CBaseInputPin
		{
		private:
			DShowSourceCapture *cap;

		public:
			DShowVideoPin(DShowSourceCapture *cap, const WChar *pinName, HRESULT *res);
			virtual ~DShowVideoPin();

//			virtual HRESULT BeginFlush();
//			virtual HRESULT EndFlush();
//			virtual HRESULT Receive(IMediaSample *pSample);
			virtual HRESULT CheckMediaType(const CMediaType *pmt);
			virtual HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
		};

		class DShowSourceCapture : public CBaseFilter
		{
		private:
			CCritSec lock;
			NN<Data::ArrayListNN<DShowVideoPin>> vPins;
		public:
			DShowSourceCapture();
			virtual ~DShowSourceCapture();

			virtual CBasePin *GetPin(int n);
			virtual int GetPinCount();
			
			CCritSec *GetLock();
		};

		class DShowFileSource
		{
		private:
			DShowSourceCapture *capFilter;
			
		public:
			DShowFileSource(UnsafeArray<const WChar> fileName);
			~DShowFileSource();
		};

	};
};
#endif
