#ifndef _SM_MEDIA_AFILTER_SOUNDGEN_ISOUNDGEN
#define _SM_MEDIA_AFILTER_SOUNDGEN_ISOUNDGEN
#include "Sync/Mutex.h"

namespace Media
{
	namespace AFilter
	{
		namespace SoundGen
		{
			class SoundTypeGen
			{
			public:
				typedef enum
				{
					ST_BELL
				} SoundType;
			protected:
				UInt32 freq;

			public:
				SoundTypeGen(UInt32 freq);
				virtual ~SoundTypeGen();

				virtual void GenSignals(UnsafeArray<Double> buff, UIntOS sampleCnt) = 0;
				virtual SoundType GetSoundType() = 0;

				virtual Bool GenSound(Double sampleVol) = 0;
			};
		}
	}
}
#endif
