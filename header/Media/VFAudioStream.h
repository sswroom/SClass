#ifndef _SM_MEDIA_VFAUDIOSTREAM
#define _SM_MEDIA_VFAUDIOSTREAM
#include "Media/VFPManager.h"
#include "Media/AudioSource.h"

namespace Media
{
	class VFAudioStream : public AudioSource
	{
	private:
		NN<Media::VFMediaFile> mfile;

		Media::AudioFormat fmt;
		UInt64 sampleCnt;
		UInt64 currSample;
		Optional<Sync::Event> readEvt;

	public:
		VFAudioStream(NN<Media::VFMediaFile> mfile);
		~VFAudioStream();

		virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff);
		virtual Bool CanSeek();
		virtual Data::Duration GetStreamTime();
		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime);

		virtual void GetFormat(NN<AudioFormat> format);

		virtual Bool Start(Optional<Sync::Event> evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(Data::ByteArray blk);
		virtual UOSInt GetMinBlockSize();
		virtual Data::Duration GetCurrTime();
		virtual Bool IsEnd();
	};
}
#endif
