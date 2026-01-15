#ifndef _SM_MEDIA_LPCMSOURCE
#define _SM_MEDIA_LPCMSOURCE
#include "IO/StreamData.h"
#include "Media/AudioSource.h"

namespace Media
{
	class LPCMSource : public AudioSource
	{
	protected:
		AudioFormat format;
		Optional<IO::StreamData> data;
		NN<Text::String> name;

		UInt64 readOfst;
		Optional<Sync::Event> readEvt;

	protected:
		LPCMSource(NN<Text::String> name);
		LPCMSource(Text::CStringNN name);
		void SetData(NN<IO::StreamData> fd, UInt64 ofst, UInt64 length, NN<const Media::AudioFormat> format);
	public:
		LPCMSource(NN<IO::StreamData> fd, UInt64 ofst, UInt64 length, NN<const Media::AudioFormat> format, NN<Text::String> name);
		LPCMSource(NN<IO::StreamData> fd, UInt64 ofst, UInt64 length, NN<const Media::AudioFormat> format, Text::CStringNN name);
		virtual ~LPCMSource();

		virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff);
		virtual Bool CanSeek();
		virtual Data::Duration GetStreamTime();
		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime);

		virtual void GetFormat(NN<AudioFormat> format);

		virtual Bool Start(Optional<Sync::Event> evt, UIntOS blkSize);
		virtual void Stop();
		virtual UIntOS ReadBlock(Data::ByteArray blk); //ret actual block size
		virtual UIntOS GetMinBlockSize();
		virtual Data::Duration GetCurrTime();
		virtual Bool IsEnd();

		virtual Bool SupportSampleRead();
		virtual UIntOS ReadSample(UInt64 sampleOfst, UIntOS sampleCount, Data::ByteArray buff);
		virtual Int64 GetSampleCount();
	};
}
#endif
