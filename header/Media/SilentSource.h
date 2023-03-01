#ifndef _SM_MEDIA_SILENTSOURCE
#define _SM_MEDIA_SILENTSOURCE
#include "IO/StreamData.h"
#include "Media/IAudioSource.h"

namespace Media
{
	class SilentSource : public IAudioSource
	{
	protected:
		AudioFormat format;
		UInt64 sampleCnt;

		Text::String *name;
		UInt64 readOfst;
		UInt64 currSample;
		Sync::Event *readEvt;

	public:
		SilentSource(UInt32 sampleRate, UInt16 nChannels, UInt16 bitCount, Text::CString name, UInt64 sampleCnt);
		virtual ~SilentSource();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Bool CanSeek();
		virtual Int32 GetStreamTime(); //ms
		virtual UInt32 SeekToTime(UInt32 time); //ms, ret actual time
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime);

		virtual void GetFormat(AudioFormat *format);

		virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(UInt8 *buff, UOSInt blkSize); //ret actual block size
		virtual UOSInt GetMinBlockSize();
		virtual UInt32 GetCurrTime();
		virtual Bool IsEnd();
	};
}
#endif
