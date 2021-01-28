#ifndef _SM_MEDIA_IAUDIOFILTER
#define _SM_MEDIA_IAUDIOFILTER
#include "Media/IAudioSource.h"

namespace Media
{
	class IAudioFilter : public IAudioSource
	{
	protected:
		Media::IAudioSource *sourceAudio;

	public:
		IAudioFilter(IAudioSource *sourceAudio);
		virtual ~IAudioFilter();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Bool CanSeek();
		virtual Int32 GetStreamTime(); //ms
		virtual Int32 SeekToTime(Int32 time); //ms, ret actual time
		virtual Bool TrimStream(Int32 trimTimeStart, Int32 trimTimeEnd, Int32 *syncTime);

		virtual void GetFormat(AudioFormat *format);

		virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
		virtual void Stop();
		//virtual OSInt ReadBlock(UInt8 *buff, OSInt blkSize); //ret actual block size
		virtual UOSInt GetMinBlockSize();
		virtual Int32 GetCurrTime();
		virtual Bool IsEnd();
	};
}
#endif
