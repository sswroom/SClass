#ifndef _SM_MEDIA_AUDIOFILTER_SOUNDGEN_ISOUNDGEN
#define _SM_MEDIA_AUDIOFILTER_SOUNDGEN_ISOUNDGEN
#include "Sync/Mutex.h"

namespace Media
{
	namespace AudioFilter
	{
		namespace SoundGen
		{
			class ISoundGen
			{
			public:
				typedef enum
				{
					ST_BELL
				} SoundType;
			protected:
				Int32 freq;

			public:
				ISoundGen(Int32 freq);
				virtual ~ISoundGen();

				virtual void GenSignals(Double *buff, OSInt sampleCnt) = 0;
				virtual SoundType GetSoundType() = 0;

				virtual Bool GenSound(Double sampleVol) = 0;
			};
		}
	}
}
#endif
