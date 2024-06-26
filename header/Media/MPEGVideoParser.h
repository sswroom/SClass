#ifndef _SM_MEDIA_MPEGVIDEOPARSER
#define _SM_MEDIA_MPEGVIDEOPARSER
#include "Media/FrameInfo.h"

namespace Media
{
	class MPEGVideoParser
	{
	public:
		typedef enum
		{
			PS_FRAME,
			PS_TOPFIELD,
			PS_BOTTOMFIELD
		} PicStruct;

		typedef struct
		{
			Char pictureCodingType;
			UInt8 dcBits;
			Bool tff;
			Bool progressive;
			Bool rff;
			PicStruct pictureStruct;
		} MPEGFrameProp;

	public:
		static Bool GetFrameInfo(UnsafeArray<UInt8> frame, UOSInt frameSize, NN<Media::FrameInfo> frameInfo, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OptOut<UInt64> bitRate, Bool decoderFix);
		static Bool GetFrameProp(UnsafeArray<const UInt8> frame, UOSInt frameSize, NN<MPEGFrameProp> prop);
	};
}
#endif
