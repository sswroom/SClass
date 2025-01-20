#ifndef _SM_MEDIA_AVIUTL_AUIAUDIO
#define _SM_MEDIA_AVIUTL_AUIAUDIO
#include "Media/AudioSource.h"
#include "Media/AVIUtl/AUIPlugin.h"

namespace Media
{
	namespace AVIUtl
	{
		class AUIAudio : public Media::AudioSource
		{
		private:
			NN<Media::AVIUtl::AUIPlugin> plugin;
			NN<Media::AVIUtl::AUIPlugin::AUIInput> input;
			NN<Media::AudioFormat> format;
			UOSInt nSamples;
			UOSInt currSample;
			Optional<Sync::Event> playEvt;

		public:
			AUIAudio(NN<Media::AVIUtl::AUIPlugin> plugin, NN<Media::AVIUtl::AUIPlugin::AUIInput> input, NN<Media::AudioFormat> format, UOSInt nSamples);
			~AUIAudio();

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
}
#endif
