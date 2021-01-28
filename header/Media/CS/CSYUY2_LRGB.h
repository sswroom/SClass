#ifndef _SM_MEDIA_CS_CSYUY2_LRGB
#define _SM_MEDIA_CS_CSYUY2_LRGB
#include "Media/CS/CSYUV_LRGB.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSYUY2_LRGB : public Media::CS::CSYUV_LRGB
		{
		private:
			typedef struct
			{
				Sync::Event *evt;
				Int32 status; // 0 = not running, 1 = idling, 2 = toExit, 3 = converting, 4 = finished
				UInt8 *yPtr;
				UInt8 *dest;
				OSInt width;
				OSInt height;
				OSInt dbpl;
			} THREADSTAT;

			OSInt currId;
			OSInt nThread;
			Sync::Event *evtMain;
			THREADSTAT *stats;

			void do_yuy2rgb(UInt8 *src, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl);
			static UInt32 __stdcall WorkerThread(void *obj);
		public:
			CSYUY2_LRGB(Media::ColorProfile *srcColor, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess);
			virtual ~CSYUY2_LRGB();
			virtual void ConvertV2(UInt8 **srcPtr, UInt8 *destPtr, OSInt dispWidth, OSInt dispHeight, OSInt srcStoreWidth, OSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual Int32 GetSrcFrameSize(Int32 width, Int32 height);
		};
	};
};
#endif
