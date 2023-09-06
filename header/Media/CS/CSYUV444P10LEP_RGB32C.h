#ifndef _SM_MEDIA_CS_CSYUV444P10LEP_RGB32C
#define _SM_MEDIA_CS_CSYUV444P10LEP_RGB32C
#include "Media/CS/CSYUV16_RGB32C.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSYUV444P10LEP_RGB32C : public Media::CS::CSConverter
		{
		protected:
			typedef struct
			{
				Sync::Event *evt;
				Int32 status; // 0 = not running, 1 = idling, 2 = toExit, 3 = converting, 4 = finished
				UInt8 *yPtr;
				UInt8 *uPtr;
				UInt8 *vPtr;
				UOSInt yBpl;
				UInt8 *dest;
				UOSInt width;
				UOSInt height;
				OSInt dbpl;
			} THREADSTAT;

			YUVPARAM yuvParam;
			RGBPARAM2 rgbParam;
			Media::ColorProfile::YUVType yuvType;
			Media::ColorProfile srcProfile;
			Media::ColorProfile destProfile;

			Bool yuvUpdated;
			Bool rgbUpdated;
			Int64 *yuv2rgb;
			Int64 *rgbGammaCorr;
			Media::PixelFormat destPF;

			UOSInt currId;
			UOSInt nThread;
			Sync::Event evtMain;
			THREADSTAT *stats;

			void SetupRGB13_LR();
			void SetupYUV_RGB13();

			static UInt32 __stdcall WorkerThread(void *obj);
			void WaitForWorker(Int32 jobStatus);
		public:
			CSYUV444P10LEP_RGB32C(NotNullPtr<const Media::ColorProfile> srcProfile, NotNullPtr<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess, Media::PixelFormat destPF);
			virtual ~CSYUV444P10LEP_RGB32C();

			virtual void ConvertV2(UInt8 *const*srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual UOSInt GetSrcFrameSize(UOSInt width, UOSInt height);

			virtual void UpdateTable();
			virtual void YUVParamChanged(NotNullPtr<const Media::IColorHandler::YUVPARAM> yuv);
			virtual void RGBParamChanged(NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgb);
			virtual UOSInt GetDestFrameSize(UOSInt width, UOSInt height);
		};
	}
}
#endif
