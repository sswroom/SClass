#ifndef _SM_MEDIA_CS_CSYUV444P10LEP_LRGBC
#define _SM_MEDIA_CS_CSYUV444P10LEP_LRGBC
#include "AnyType.h"
#include "Media/CS/CSYUV16_LRGBC.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSYUV444P10LEP_LRGBC : public Media::CS::CSConverter
		{
		protected:
			typedef struct
			{
				NN<Sync::Event> evt;
				Int32 status; // 0 = not running, 1 = idling, 2 = toExit, 3 = converting, 4 = finished
				UnsafeArray<UInt8> yPtr;
				UnsafeArray<UInt8> uPtr;
				UnsafeArray<UInt8> vPtr;
				UIntOS yBpl;
				UnsafeArray<UInt8> dest;
				UIntOS width;
				UIntOS height;
				IntOS dbpl;
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

			UIntOS currId;
			UIntOS nThread;
			Sync::Event evtMain;
			UnsafeArray<THREADSTAT> stats;

			void SetupRGB13_LR();
			void SetupYUV_RGB13();

			static UInt32 __stdcall WorkerThread(AnyType obj);
			void WaitForWorker(Int32 jobStatus);
		public:
			CSYUV444P10LEP_LRGBC(NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess);
			virtual ~CSYUV444P10LEP_LRGBC();

			virtual void ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UIntOS dispWidth, UIntOS dispHeight, UIntOS srcStoreWidth, UIntOS srcStoreHeight, IntOS destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual UIntOS GetSrcFrameSize(UIntOS width, UIntOS height);

			virtual void UpdateTable();

			virtual void YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuv);
			virtual void RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgb);
			virtual UIntOS GetDestFrameSize(UIntOS width, UIntOS height);
		};
	}
}
#endif
