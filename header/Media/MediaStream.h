#ifndef _SM_MEDIA_MEDIASTREAM
#define _SM_MEDIA_MEDIASTREAM
#include "Data/Duration.h"

namespace Media
{
	class MediaStream
	{
	public:
		virtual ~MediaStream(){};

		virtual void DetectStreamInfo(UInt8 *header, UOSInt headerSize) = 0;
		virtual void ClearFrameBuff() = 0;
		virtual void SetStreamTime(Data::Duration time) = 0;
		virtual void WriteFrameStream(UInt8 *buff, UOSInt buffSize) = 0;
		virtual Data::Duration GetFrameStreamTime() = 0;
		virtual void EndFrameStream() = 0;
		virtual UInt64 GetBitRate() = 0;
	};
}
#endif
