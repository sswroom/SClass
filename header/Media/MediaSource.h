#ifndef _SM_MEDIA_MEDIASOURCE
#define _SM_MEDIA_MEDIASOURCE
#include "Data/Duration.h"

namespace Media
{
	typedef enum
	{
		MEDIA_TYPE_UNKNOWN,
		MEDIA_TYPE_AUDIO,
		MEDIA_TYPE_VIDEO,
		MEDIA_TYPE_SUBTITLE
	} MediaType;

	class MediaSource
	{
	public:
		virtual ~MediaSource(){};

		virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff) = 0;

		virtual void Stop() = 0;
		virtual Data::Duration GetStreamTime() = 0;
		virtual Bool CanSeek() = 0;
		virtual Data::Duration SeekToTime(Data::Duration time) = 0;
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime) = 0;

		virtual MediaType GetMediaType() = 0;
	};
}
#endif
