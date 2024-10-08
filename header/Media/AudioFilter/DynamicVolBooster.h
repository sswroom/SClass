#ifndef _SM_MEDIA_AUDIOFILTER_DYNAMICVOLBOOSTER
#define _SM_MEDIA_AUDIOFILTER_DYNAMICVOLBOOSTER
#include "Media/IAudioFilter.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace AudioFilter
	{
		class DynamicVolBooster : public IAudioFilter
		{
		private:
			UInt32 soundBuffLeng;
			Int32 *soundBuff;
			Int32 maxVol;
			Int32 lastVol;
			Int32 thisVol;
			UOSInt maxIndex;
			UOSInt soundIndex;
			UInt32 nChannels;
			UInt32 bitCount;
			Bool enabled;
			Double bgLevel;
			Sync::Mutex mut;
		private:
			void ResetStatus();
		public:
			DynamicVolBooster(NN<IAudioSource> sourceAudio);
			virtual ~DynamicVolBooster();

			virtual void GetFormat(NN<AudioFormat> format);
			virtual Data::Duration SeekToTime(Data::Duration time);
			virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size

			void SetEnabled(Bool enabled);
			void SetBGLevel(Double bgLevel);
		};
	}
}
#endif
