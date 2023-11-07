#ifndef _SM_MEDIA_DECODER_VFWDECODER
#define _SM_MEDIA_DECODER_VFWDECODER
#include "Data/ArrayListUInt32.h"
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
				UInt32 frameNum;
				UInt32 frameTime;
				Media::FrameType frameType;
			} BFrameStatus;
		private:
			UInt32 frameRateNorm;
			UInt32 frameRateDenorm;
			UOSInt maxFrameSize;
			void *bmihSrc;
			void *bmihDest;
			void *hic;
			UInt8 *frameBuff;

			ImageCallback imgCb;
			void *imgCbData;
			EncodingType encType;
			Bool frameChg;
			UInt32 sourceFCC;

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

			static Bool GetFCCHandlers(UInt32 fourcc, Data::ArrayListUInt32 *fccHdlrs, Data::ArrayListUInt32 *outFccs, EncodingType *encType);

			virtual void ProcVideoFrame(Data::Duration frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			VFWDecoder(NotNullPtr<IVideoSource> sourceAudio);
			virtual ~VFWDecoder();

			virtual Bool CaptureImage(ImageCallback imgCb, void *userData);
			virtual Text::CStringNN GetFilterName();

			virtual Bool GetVideoInfo(NotNullPtr<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize);
			virtual void Stop();

			virtual Bool HasFrameCount();
			virtual UOSInt GetFrameCount();
			virtual Data::Duration GetFrameTime(UOSInt frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, void *userData);

			virtual void OnFrameChanged(Media::IVideoSource::FrameChange fc);
		};
	}
}
#endif
