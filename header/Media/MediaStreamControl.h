#ifndef _SM_MEDIA_MEDIASTREAMCONTROL
#define _SM_MEDIA_MEDIASTREAMCONTROL
#include "Data/Duration.h"

namespace Media
{
	class MediaStreamControl
	{
	public:
		virtual ~MediaStreamControl(){};

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
		virtual UIntOS GetDataSeekCount() = 0;
	};
}
#endif
