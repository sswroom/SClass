#ifndef _SM_MEDIA_CS_CSAYUV444_10_LRGBC
#define _SM_MEDIA_CS_CSAYUV444_10_LRGBC
#include "AnyType.h"
#include "Media/CS/CSYUV16_LRGBC.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSAYUV444_10_LRGBC : public Media::CS::CSConverter
		{
		protected:
			typedef struct
			{
				NN<Sync::Event> evt;
				Int32 status; // 0 = not running, 1 = idling, 2 = toExit, 3 = converting, 4 = finished
				UnsafeArray<UInt8> yPtr;
				UOSInt yBpl;
				UnsafeArray<UInt8> dest;
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
			UnsafeArray<Int64> yuv2rgb;
			UnsafeArray<Int64> rgbGammaCorr;

			UOSInt currId;
			UOSInt nThread;
			Sync::Event evtMain;
			UnsafeArray<THREADSTAT> stats;

			void SetupRGB13_LR();
			void SetupYUV_RGB13();

			static UInt32 __stdcall WorkerThread(AnyType obj);
			void WaitForWorker(Int32 jobStatus);
		public:
			CSAYUV444_10_LRGBC(NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess);
			virtual ~CSAYUV444_10_LRGBC();

			virtual void ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual UOSInt GetSrcFrameSize(UOSInt width, UOSInt height);

			virtual void UpdateTable();

			virtual void YUVParamChanged(NN<const Media::IColorHandler::YUVPARAM> yuv);
			virtual void RGBParamChanged(NN<const Media::IColorHandler::RGBPARAM2> rgb);
			virtual UOSInt GetDestFrameSize(UOSInt width, UOSInt height);
		};
	}
}
#endif
