#ifndef _SM_MEDIA_AUDIOFRAMESOURCE
#define _SM_MEDIA_AUDIOFRAMESOURCE
#include "IO/IStreamData.h"
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
		IO::IStreamData *data;
		const UTF8Char *name;

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
		AudioFrameSource(IO::IStreamData *fd, Media::AudioFormat *format, const UTF8Char *name);
		virtual ~AudioFrameSource();

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

		virtual void AddBlock(UInt64 offset, UInt32 length, UInt32 decodedSample);
	};
}
#endif
