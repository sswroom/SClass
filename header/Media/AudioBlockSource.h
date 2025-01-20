#ifndef _SM_MEDIA_AUDIOBLOCKSOURCE
#define _SM_MEDIA_AUDIOBLOCKSOURCE
#include "Sync/Event.h"
#include "IO/StreamData.h"
#include "Media/MediaSource.h"
#include "Media/AudioSource.h"

namespace Media
{
	class AudioBlockSource : public AudioSource
	{
	private:
		typedef struct
		{
			UInt64 offset;
			UInt32 length;
		} AudioBlock;

	private:
		Media::AudioFormat format;
		NN<IO::StreamData> data;
		NN<Text::String> name;

		UInt32 readBlock;
		Optional<Sync::Event> readEvt;

		AudioBlock *blocks;
		UInt32 blockCnt;
		UInt32 maxBlockCnt;
		UInt32 samplePerBlock;
		UOSInt maxBlockSize;
	public:
		AudioBlockSource(NN<IO::StreamData> fd, NN<const Media::AudioFormat> format, NN<Text::String> name, UInt32 samplePerBlock);
		virtual ~AudioBlockSource();

		virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff);
		virtual Bool CanSeek();
		virtual Data::Duration GetStreamTime();
		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime);

		virtual void GetFormat(NN<AudioFormat> format);

		virtual Bool Start(Optional<Sync::Event> evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size
		virtual UOSInt GetMinBlockSize();
		virtual Data::Duration GetCurrTime();
		virtual Bool IsEnd();

		void AddBlock(UInt64 offset, UInt32 length);
		void UpdateBitRate(UInt32 bitRate);
	};
}
#endif
