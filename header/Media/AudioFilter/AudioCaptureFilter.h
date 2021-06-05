#ifndef _SM_MEDIA_AUDIOFILTER_AUDIOCAPTUREFILTER
#define _SM_MEDIA_AUDIOFILTER_AUDIOCAPTUREFILTER
#include "IO/FileStream.h"
#include "Media/IAudioFilter.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace AudioFilter
	{
		class AudioCaptureFilter : public IAudioFilter
		{
		private:
			IO::FileStream *waveStm;
			UInt32 dataOfst;
			UInt64 dataSize;
			UInt64 fileSize;

			UInt8 *readBuff;
			UOSInt readBuffSize;
			UInt8 *writeBuff;
			Sync::Mutex *readMut;
			Sync::Mutex *writeMut;
			Bool writing;
			Bool running;
			Bool toStop;
			Sync::Event *evt;

			static UInt32 __stdcall CaptureThread(void *userObj);
		public:
			AudioCaptureFilter(IAudioSource *sourceAudio);
			virtual ~AudioCaptureFilter();

			virtual UInt32 SeekToTime(UInt32 time); //ms, ret actual time
			virtual UOSInt ReadBlock(UInt8 *buff, UOSInt blkSize); //ret actual block size

			Bool StartCapture(const UTF8Char *fileName);
			void StopCapture();
		};
	}
}
#endif
