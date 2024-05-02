#ifndef _SM_MEDIA_RGBCOLORFILTER
#define _SM_MEDIA_RGBCOLORFILTER
#include "AnyType.h"
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
			UOSInt width;
			UOSInt height;
			OSInt sAdd;
			OSInt dAdd;
			Int32 threadStat;
			Media::RGBColorFilter *me;
			Sync::Event *evt;
		} ThreadStat;

	private:
		NN<Media::ColorManager> colorMgr;
		Double brightness;
		Double contrast;
		Double gamma;
		UInt32 bpp;
		Media::PixelFormat pf;
		UInt32 hdrLev;
		UInt8 *lut;
		Bool hasSSE41;
		Double *gammaParam;
		UOSInt gammaCnt;

		UOSInt nThread;
		Sync::Event threadEvt;
		ThreadStat *threadStats;

		static UInt32 __stdcall ProcessThread(AnyType userObj);
		void WaitForThread(Int32 stat);
	public:
		RGBColorFilter(NN<Media::ColorManager> colorMgr);
		~RGBColorFilter();

		void SetGammaCorr(Double *gammaParam, UOSInt gammaCnt);
		void SetParameter(Double brightness, Double contrast, Double gamma, NN<const Media::ColorProfile> color, UInt32 bpp, Media::PixelFormat pf, UInt32 hdrLev);
		void ProcessImage(UInt8 *srcPtr, UInt8 *destPtr, UOSInt width, UOSInt height, UOSInt sbpl, UOSInt dbpl, Bool upsideDown);
	};
}
#endif
