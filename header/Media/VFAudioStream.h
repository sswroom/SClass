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
		virtual Int32 SeekToTime(Int32 time);
		virtual Bool TrimStream(Int32 trimTimeStart, Int32 trimTimeEnd, Int32 *syncTime);

		virtual void GetFormat(AudioFormat *format);

		virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(UInt8 *buff, UOSInt blkSize);
		virtual UOSInt GetMinBlockSize();
		virtual Int32 GetCurrTime();
		virtual Bool IsEnd();
	};
}
#endif
