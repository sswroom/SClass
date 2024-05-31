#ifndef _SM_MEDIA_ISTREAMCONTROL
#define _SM_MEDIA_ISTREAMCONTROL
#include "Data/Duration.h"

namespace Media
{
	class IStreamControl
	{
	public:
		virtual ~IStreamControl(){};

		virtual UnsafeArrayOpt<UTF8Char> GetMediaName(UnsafeArray<UTF8Char> buff) = 0;
		virtual Data::Duration GetStreamTime() = 0;
		virtual Bool StartAudio() = 0;
		virtual Bool StopAudio() = 0;
		virtual Bool StartVideo() = 0;
		virtual Bool StopVideo() = 0;
		virtual Bool IsRunning() = 0;
		virtual Data::Duration SeekToTime(Data::Duration mediaTime) = 0;
		virtual Bool IsRealTimeSrc() = 0;
		virtual Bool CanSeek() = 0;
		virtual UOSInt GetDataSeekCount() = 0;
	};
}
#endif
