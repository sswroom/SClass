#ifndef _SM_MEDIA_AUDIOFRAMESOURCE
#define _SM_MEDIA_AUDIOFRAMESOURCE
#include "IO/StreamData.h"
#include "Media/IAudioFrameSource.h"
#include "Sync/Event.h"

namespace Media
{
	class AudioFrameSource : public IAudioFrameSource
	{
	private:
		typedef struct
		{
			UInt64 offset;
			UInt32 length;
			UOSInt sampleOffset;
			UInt32 decodedSample;
		} AudioFrame;

	protected:
		Media::AudioFormat format;
		NotNullPtr<IO::StreamData> data;
		NotNullPtr<Text::String> name;

		UOSInt readBlock;
		UOSInt readBlockOfst;
		Sync::Event *readEvt;

		AudioFrame *blocks;
		UInt32 blockCnt;
		UInt32 maxBlockCnt;
		UOSInt maxBlockSize;
		UOSInt totalSampleCnt;
		UInt64 totalSize;
	public:
		AudioFrameSource(NotNullPtr<IO::StreamData> fd, NotNullPtr<const Media::AudioFormat> format, NotNullPtr<Text::String> name);
		AudioFrameSource(NotNullPtr<IO::StreamData> fd, NotNullPtr<const Media::AudioFormat> format, Text::CStringNN name);
		virtual ~AudioFrameSource();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Bool CanSeek();
		virtual Int32 GetStreamTime(); //ms
		virtual UInt32 SeekToTime(UInt32 time); //ms, ret actual time
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime);

		virtual void GetFormat(NotNullPtr<AudioFormat> format);

		virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size
		virtual UOSInt GetMinBlockSize();
		virtual UInt32 GetCurrTime();
		virtual Bool IsEnd();

		virtual void AddBlock(UInt64 offset, UInt32 length, UInt32 decodedSample);
	};
}
#endif
