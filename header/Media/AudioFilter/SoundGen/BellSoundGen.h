#ifndef _SM_MEDIA_AUDIOFILTER_SOUNDGEN_BELLSOUNDGEN
#define _SM_MEDIA_AUDIOFILTER_SOUNDGEN_BELLSOUNDGEN
#include "Media/AudioFilter/SoundGen/ISoundGen.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace AudioFilter
	{
		namespace SoundGen
		{
			class BellSoundGen : public ISoundGen
			{
			protected:
				Int32 currSample;
				Double sampleVol;
				Sync::Mutex *soundMut;
		
			public:
				BellSoundGen(Int32 freq);
				virtual ~BellSoundGen();

				virtual void GenSignals(Double *buff, OSInt sampleCnt);
				virtual SoundType GetSoundType();

				virtual Bool GenSound(Double sampleVol);
			};
		}
	}
}
#endif
