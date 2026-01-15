#ifndef _SM_MEDIA_DECODER_VFWDECODER
#define _SM_MEDIA_DECODER_VFWDECODER
#include "Data/ArrayListUInt32.h"
#include "Data/CallbackStorage.h"
#include "Media/VideoSource.h"
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
				UInt32 frameNum;
				Data::Duration frameTime;
				Media::FrameType frameType;
			} BFrameStatus;
		private:
			UInt32 frameRateNorm;
			UInt32 frameRateDenorm;
			UIntOS maxFrameSize;
			void *bmihSrc;
			void *bmihDest;
			void *hic;
			UnsafeArray<UInt8> frameBuff;

			Data::CallbackStorage<ImageCallback> imgCb;
			EncodingType encType;
			Bool frameChg;
			UInt32 sourceFCC;

//			Bool hasBFrame;
//			Bool bFrameDrop;
//			Bool lastIsB;
//			UInt32 lastFrameNum;
//			UInt32 lastFrameTime;
//			Media::FrameType lastFrameType;
//			Media::VideoSource::FrameFlag lastFrameFlags;
			Media::YCOffset lastYCOfst;

			Int32 bCnt;
			Int32 bDiscard;
			BFrameStatus bBuff[20];
			Int32 uOfst;
			Int32 vOfst;
			Bool endProcessing;

			static Bool GetFCCHandlers(UInt32 fourcc, Data::ArrayListUInt32 *fccHdlrs, Data::ArrayListUInt32 *outFccs, EncodingType *encType);

			virtual void ProcVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			VFWDecoder(NN<VideoSource> sourceAudio);
			virtual ~VFWDecoder();

			virtual Bool CaptureImage(ImageCallback imgCb, AnyType userData);
			virtual Text::CStringNN GetFilterName();

			virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UIntOS> maxFrameSize);
			virtual void Stop();

			virtual Bool HasFrameCount();
			virtual UIntOS GetFrameCount();
			virtual Data::Duration GetFrameTime(UIntOS frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, AnyType userData);

			virtual void OnFrameChanged(Media::VideoSource::FrameChange fc);
		};
	}
}
#endif
