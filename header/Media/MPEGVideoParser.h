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
		static Bool GetFrameInfo(UInt8 *frame, UOSInt frameSize, Media::FrameInfo *frameInfo, Int32 *frameRateNorm, Int32 *frameRateDenorm, UInt64 *bitRate, Bool decoderFix);
		static Bool GetFrameProp(const UInt8 *frame, UOSInt frameSize, MPEGFrameProp *prop);
	};
}
#endif
