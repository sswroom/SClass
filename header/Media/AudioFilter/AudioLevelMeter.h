#ifndef _SM_MEDIA_AUDIOFILTER_AUDIOLEVELMETER
#define _SM_MEDIA_AUDIOFILTER_AUDIOLEVELMETER
#include "Media/IAudioFilter.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace AudioFilter
	{
		class AudioLevelMeter : public IAudioFilter
		{
		private:
			typedef struct
			{
				Int32 maxLevel;
				Int32 minLevel;
				Bool levelChanged;
			} ChannelStatus;
		private:
			UInt32 nChannel;
			UInt32 bitCount;
			UInt32 soundBuffLeng;
			Int32 *soundBuff;
			UInt32 soundBuffOfst;
			ChannelStatus *status;
			Sync::Mutex mut;
		private:
			void ResetStatus();
		public:
			AudioLevelMeter(IAudioSource *sourceAudio);
			virtual ~AudioLevelMeter();

			virtual UInt32 SeekToTime(UInt32 time); //ms, ret actual time
			virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size

			Double GetLevel(UOSInt channel);
		};
	}
}
#endif
