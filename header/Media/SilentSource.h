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

		Optional<Text::String> name;
		UInt64 readOfst;
		UInt64 currSample;
		Sync::Event *readEvt;

	public:
		SilentSource(UInt32 sampleRate, UInt16 nChannels, UInt16 bitCount, Text::CString name, UInt64 sampleCnt);
		virtual ~SilentSource();

		virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff);
		virtual Bool CanSeek();
		virtual Data::Duration GetStreamTime();
		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime);

		virtual void GetFormat(NN<AudioFormat> format);

		virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size
		virtual UOSInt GetMinBlockSize();
		virtual Data::Duration GetCurrTime();
		virtual Bool IsEnd();
	};
}
#endif
