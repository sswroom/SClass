#ifndef _SM_MEDIA_IAUDIOFILTER
#define _SM_MEDIA_IAUDIOFILTER
#include "Media/IAudioSource.h"

namespace Media
{
	class IAudioFilter : public IAudioSource
	{
	protected:
		NN<Media::IAudioSource> sourceAudio;

	public:
		IAudioFilter(NN<IAudioSource> sourceAudio);
		virtual ~IAudioFilter();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Bool CanSeek();
		virtual Data::Duration GetStreamTime();
		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime);

		virtual void GetFormat(NN<AudioFormat> format);

		virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
		virtual void Stop();
		//virtual OSInt ReadBlock(UInt8 *buff, OSInt blkSize); //ret actual block size
		virtual UOSInt GetMinBlockSize();
		virtual Data::Duration GetCurrTime();
		virtual Bool IsEnd();
	};
}
#endif
