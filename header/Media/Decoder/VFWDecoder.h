#ifndef _SM_MEDIA_DECODER_VFWDECODER
#define _SM_MEDIA_DECODER_VFWDECODER
#include "Data/ArrayListInt32.h"
#include "Media/IVideoSource.h"
#include "Media/Decoder/VDecoderBase.h"
//require vfw32.lib

namespace Media
{
	namespace Decoder
	{
		class VFWDecoder : public VDecoderBase
		{
		public:
			typedef enum
			{
				ET_UNKNOWN,
				ET_MP4V,
				ET_H264,
				ET_MP2V
			} EncodingType;

			typedef struct
			{
				Int32 frameNum;
				Int32 frameTime;
				Media::FrameType frameType;
			} BFrameStatus;
		private:
			Int32 frameRateNorm;
			Int32 frameRateDenorm;
			Int32 maxFrameSize;
			void *bmihSrc;
			void *bmihDest;
			void *hic;
			UInt8 *frameBuff;

			ImageCallback imgCb;
			void *imgCbData;
			EncodingType encType;
			Bool frameChg;
			Int32 sourceFCC;

//			Bool hasBFrame;
//			Bool bFrameDrop;
//			Bool lastIsB;
//			UInt32 lastFrameNum;
//			UInt32 lastFrameTime;
//			Media::FrameType lastFrameType;
//			Media::IVideoSource::FrameFlag lastFrameFlags;
			Media::YCOffset lastYCOfst;

			Int32 bCnt;
			Int32 bDiscard;
			BFrameStatus bBuff[20];
			Int32 uOfst;
			Int32 vOfst;
			Bool endProcessing;

			static Bool GetFCCHandlers(Int32 fourcc, Data::ArrayListInt32 *fccHdlrs, Data::ArrayListInt32 *outFccs, EncodingType *encType);

			virtual void ProcVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			VFWDecoder(IVideoSource *sourceAudio);
			virtual ~VFWDecoder();

			virtual Bool CaptureImage(ImageCallback imgCb, void *userData);
			virtual const UTF8Char *GetFilterName();

			virtual Bool GetVideoInfo(Media::FrameInfo *info, Int32 *frameRateNorm, Int32 *frameRateDenorm, UOSInt *maxFrameSize);
			virtual void Stop();

			virtual OSInt GetFrameCount(); //-1 = unknown;
			virtual UInt32 GetFrameTime(UOSInt frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, void *userData);

			virtual void OnFrameChanged(Media::IVideoSource::FrameChange fc);
		};
	}
}
#endif
