#ifndef _SM_MEDIA_RGBCOLORFILTER
#define _SM_MEDIA_RGBCOLORFILTER
#include "Media/ColorManager.h"
#include "Media/FrameInfo.h"
#include "Sync/Event.h"

namespace Media
{
	class RGBColorFilter
	{
	private:
		typedef struct
		{
			UInt8 *srcPtr;
			UInt8 *destPtr;
			OSInt width;
			OSInt height;
			OSInt sbpl;
			OSInt dbpl;
			Int32 threadStat;
			Media::RGBColorFilter *me;
			Sync::Event *evt;
		} ThreadStat;

	private:
		Media::ColorManager *colorMgr;
		Double brightness;
		Double contrast;
		Double gamma;
		Int32 bpp;
		Media::PixelFormat pf;
		Int32 hdrLev;
		UInt8 *lut;
		Bool hasSSE41;
		Double *gammaParam;
		OSInt gammaCnt;

		UOSInt nThread;
		Sync::Event *threadEvt;
		ThreadStat *threadStats;

		static UInt32 __stdcall ProcessThread(void *userObj);
		void WaitForThread(Int32 stat);
	public:
		RGBColorFilter(Media::ColorManager *colorMgr);
		~RGBColorFilter();

		void SetGammaCorr(Double *gammaParam, OSInt gammaCnt);
		void SetParameter(Double brightness, Double contrast, Double gamma, Media::ColorProfile *color, Int32 bpp, Media::PixelFormat pf, Int32 hdrLev);
		void ProcessImage(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt sbpl, OSInt dbpl);
	};
};
#endif
