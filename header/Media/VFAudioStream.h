#ifndef _SM_MEDIA_VFAUDIOSTREAM
#define _SM_MEDIA_VFAUDIOSTREAM
#include "Media/VFPManager.h"
#include "Media/IAudioSource.h"

namespace Media
{
	class VFAudioStream : public IAudioSource
	{
	private:
		Media::VFMediaFile *mfile;

		Media::AudioFormat fmt;
		UInt64 sampleCnt;
		UInt64 currSample;
		Sync::Event *readEvt;

	public:
		VFAudioStream(Media::VFMediaFile *mfile);
		~VFAudioStream();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Bool CanSeek();
		virtual Int32 GetStreamTime();
		virtual UInt32 SeekToTime(UInt32 time);
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime);

		virtual void GetFormat(NotNullPtr<AudioFormat> format);

		virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(Data::ByteArray blk);
		virtual UOSInt GetMinBlockSize();
		virtual UInt32 GetCurrTime();
		virtual Bool IsEnd();
	};
}
#endif
