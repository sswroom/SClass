#ifndef _SM_MEDIA_AUDIOCONCATSOURCE
#define _SM_MEDIA_AUDIOCONCATSOURCE
#include "Media/IAudioSource.h"
#include "Data/ArrayList.h"

namespace Media
{
	class AudioConcatSource : public Media::IAudioSource
	{
	private:
		Media::AudioFormat format;
		Data::ArrayList<Media::IAudioSource*> *stmList;

		Int32 stmTime;
		Bool canSeek;

		UOSInt currStm;
		Int64 readOfst;
		UOSInt readBlkSize;
		Sync::Event *readEvt;

	public:
		AudioConcatSource();
		virtual ~AudioConcatSource();

		Bool AppendAudio(Media::IAudioSource *audio);
		Bool AppendSilent(UInt32 time);

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Bool CanSeek();
		virtual Int32 GetStreamTime(); //ms
		virtual void GetFormat(AudioFormat *format);
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime);

		virtual UInt32 SeekToTime(UInt32 time); //ms, ret actual time
		virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(UInt8 *buff, UOSInt blkSize); //ret actual block size
		virtual UInt32 GetCurrTime();
		virtual UOSInt GetMinBlockSize();
		virtual Bool IsEnd();
	};
}
#endif
