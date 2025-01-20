#ifndef _SM_MEDIA_AFILTER_AUDIOCAPTUREFILTER
#define _SM_MEDIA_AFILTER_AUDIOCAPTUREFILTER
#include "AnyType.h"
#include "IO/FileStream.h"
#include "Media/AudioFilter.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace AFilter
	{
		class AudioCaptureFilter : public AudioFilter
		{
		private:
			IO::FileStream *waveStm;
			UInt32 dataOfst;
			UInt64 dataSize;
			UInt64 fileSize;

			UInt8 *readBuff;
			UOSInt readBuffSize;
			UInt8 *writeBuff;
			Sync::Mutex readMut;
			Sync::Mutex writeMut;
			Bool writing;
			Bool running;
			Bool toStop;
			Sync::Event evt;

			static UInt32 __stdcall CaptureThread(AnyType userObj);
		public:
			AudioCaptureFilter(NN<AudioSource> sourceAudio);
			virtual ~AudioCaptureFilter();

			virtual Data::Duration SeekToTime(Data::Duration time);
			virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size

			Bool StartCapture(Text::CStringNN fileName);
			void StopCapture();
		};
	}
}
#endif
