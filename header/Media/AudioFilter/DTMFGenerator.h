#ifndef _SM_MEDIA_AUDIOFILTER_DTMFGENERATOR
#define _SM_MEDIA_AUDIOFILTER_DTMFGENERATOR
#include "Media/IAudioFilter.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace AudioFilter
	{
		class DTMFGenerator : public IAudioFilter
		{
		protected:
			AudioFormat format;

			Double freq1Curr;
			Double freq1Step;
			Double freq2Curr;
			Double freq2Step;
			Double vol;

			Sync::Mutex *tonesMut;
			UInt32 tonesSignalSamples;
			UInt32 tonesBreakSamples;
			Double tonesVol;
			UInt32 tonesCurrSample;
			const UTF8Char *tonesVals;
			
		public:
			DTMFGenerator(IAudioSource *sourceAudio);
			virtual ~DTMFGenerator();

			virtual void GetFormat(AudioFormat *format);
			virtual UOSInt ReadBlock(UInt8 *buff, UOSInt blkSize); //ret actual block size

			void SetTone(UTF8Char tone); // 0-9, *#, ABCD
			void SetVolume(Double vol);

			Bool GenTones(UInt32 signalTime, UInt32 breakTime, Double vol, const UTF8Char *tones);
		};
	}
}
#endif
