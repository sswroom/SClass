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
		Bool AppendSilent(Int32 time);

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Bool CanSeek();
		virtual Int32 GetStreamTime(); //ms
		virtual void GetFormat(AudioFormat *format);
		virtual Bool TrimStream(Int32 trimTimeStart, Int32 trimTimeEnd, Int32 *syncTime);

		virtual Int32 SeekToTime(Int32 time); //ms, ret actual time
		virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(UInt8 *buff, UOSInt blkSize); //ret actual block size
		virtual Int32 GetCurrTime();
		virtual UOSInt GetMinBlockSize();
		virtual Bool IsEnd();
	};
}
#endif
