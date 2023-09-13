#ifndef _SM_MEDIA_IAUDIOFILTER
#define _SM_MEDIA_IAUDIOFILTER
#include "Media/IAudioSource.h"

namespace Media
{
	class IAudioFilter : public IAudioSource
	{
	protected:
		NotNullPtr<Media::IAudioSource> sourceAudio;

	public:
		IAudioFilter(NotNullPtr<IAudioSource> sourceAudio);
		virtual ~IAudioFilter();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Bool CanSeek();
		virtual Int32 GetStreamTime(); //ms
		virtual UInt32 SeekToTime(UInt32 time); //ms, ret actual time
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime);

		virtual void GetFormat(NotNullPtr<AudioFormat> format);

		virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
		virtual void Stop();
		//virtual OSInt ReadBlock(UInt8 *buff, OSInt blkSize); //ret actual block size
		virtual UOSInt GetMinBlockSize();
		virtual UInt32 GetCurrTime();
		virtual Bool IsEnd();
	};
}
#endif
