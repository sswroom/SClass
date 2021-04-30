#ifndef _SM_MEDIA_ISTREAMCONTROL
#define _SM_MEDIA_ISTREAMCONTROL

namespace Media
{
	class IStreamControl
	{
	public:
		virtual ~IStreamControl(){};

		virtual UTF8Char *GetMediaName(UTF8Char *buff) = 0;
		virtual Int32 GetStreamTime() = 0;
		virtual Bool StartAudio() = 0;
		virtual Bool StopAudio() = 0;
		virtual Bool StartVideo() = 0;
		virtual Bool StopVideo() = 0;
		virtual Bool IsRunning() = 0;
		virtual UInt32 SeekToTime(UInt32 mediaTime) = 0;
		virtual Bool IsRealTimeSrc() = 0;
		virtual Bool CanSeek() = 0;
		virtual UOSInt GetDataSeekCount() = 0;
	};
}
#endif
