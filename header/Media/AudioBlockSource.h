#ifndef _SM_MEDIA_AUDIOBLOCKSOURCE
#define _SM_MEDIA_AUDIOBLOCKSOURCE
#include "Sync/Event.h"
#include "IO/IStreamData.h"
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
		IO::IStreamData *data;
		const UTF8Char *name;

		UInt32 readBlock;
		Sync::Event *readEvt;

		AudioBlock *blocks;
		UInt32 blockCnt;
		UInt32 maxBlockCnt;
		UInt32 samplePerBlock;
		UOSInt maxBlockSize;
	public:
		AudioBlockSource(IO::IStreamData *fd, Media::AudioFormat *format, const UTF8Char *name, UInt32 samplePerBlock);
		virtual ~AudioBlockSource();

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

		void AddBlock(UInt64 offset, UInt32 length);
		void UpdateBitRate(Int32 bitRate);
	};
}
#endif
