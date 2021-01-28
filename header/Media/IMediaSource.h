#ifndef _SM_MEDIA_IMEDIASOURCE
#define _SM_MEDIA_IMEDIASOURCE

namespace Media
{
	typedef enum
	{
		MEDIA_TYPE_UNKNOWN,
		MEDIA_TYPE_AUDIO,
		MEDIA_TYPE_VIDEO,
		MEDIA_TYPE_SUBTITLE
	} MediaType;

	class IMediaSource
	{
	public:
		virtual ~IMediaSource(){};

		virtual UTF8Char *GetSourceName(UTF8Char *buff) = 0;

		virtual void Stop() = 0;
		virtual Int32 GetStreamTime() = 0; //ms, -1 = infinity
		virtual Bool CanSeek() = 0;
		virtual Int32 SeekToTime(Int32 time) = 0; //ms, ret actual time
		virtual Bool TrimStream(Int32 trimTimeStart, Int32 trimTimeEnd, Int32 *syncTime) = 0;

		virtual MediaType GetMediaType() = 0;
	};
};
#endif
