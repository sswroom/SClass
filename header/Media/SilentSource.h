#ifndef _SM_MEDIA_SILENTSOURCE
#define _SM_MEDIA_SILENTSOURCE
#include "IO/IStreamData.h"
#include "Media/IAudioSource.h"

namespace Media
{
	class SilentSource : public IAudioSource
	{
	protected:
		AudioFormat format;
		UInt64 sampleCnt;

		const UTF8Char *name;
		UInt64 readOfst;
		UInt64 currSample;
		Sync::Event *readEvt;

	public:
		SilentSource(UInt32 sampleRate, UInt32 nChannels, UInt32 bitCount, const UTF8Char *name, UInt64 sampleCnt);
		virtual ~SilentSource();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Bool CanSeek();
		virtual Int32 GetStreamTime(); //ms
		virtual Int32 SeekToTime(Int32 time); //ms, ret actual time
		virtual Bool TrimStream(Int32 trimTimeStart, Int32 trimTimeEnd, Int32 *syncTime);

		virtual void GetFormat(AudioFormat *format);

		virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(UInt8 *buff, UOSInt blkSize); //ret actual block size
		virtual UOSInt GetMinBlockSize();
		virtual Int32 GetCurrTime();
		virtual Bool IsEnd();
	};
}
#endif
