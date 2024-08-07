#ifndef _SM_MEDIA_AUDIOBLOCKSOURCE
#define _SM_MEDIA_AUDIOBLOCKSOURCE
#include "Sync/Event.h"
#include "IO/StreamData.h"
#include "Media/IMediaSource.h"
#include "Media/IAudioSource.h"

namespace Media
{
	class AudioBlockSource : public IAudioSource
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
		Sync::Event *readEvt;

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
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime);

		virtual void GetFormat(NN<AudioFormat> format);

		virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
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
