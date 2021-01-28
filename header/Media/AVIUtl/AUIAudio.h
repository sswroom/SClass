#ifndef _SM_MEDIA_AVIUTL_AUIAUDIO
#define _SM_MEDIA_AVIUTL_AUIAUDIO
#include "Media/IAudioSource.h"
#include "Media/AVIUtl/AUIPlugin.h"

namespace Media
{
	namespace AVIUtl
	{
		class AUIAudio : public Media::IAudioSource
		{
		private:
			Media::AVIUtl::AUIPlugin *plugin;
			Media::AVIUtl::AUIPlugin::AUIInput *input;
			Media::AudioFormat *format;
			UOSInt nSamples;
			UOSInt currSample;
			Sync::Event *playEvt;

		public:
			AUIAudio(Media::AVIUtl::AUIPlugin *plugin, Media::AVIUtl::AUIPlugin::AUIInput *input, Media::AudioFormat *format, UOSInt nSamples);
			~AUIAudio();

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
}
#endif
