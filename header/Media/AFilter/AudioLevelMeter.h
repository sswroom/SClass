#ifndef _SM_MEDIA_AFILTER_AUDIOLEVELMETER
#define _SM_MEDIA_AFILTER_AUDIOLEVELMETER
#include "Media/AudioFilter.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace AFilter
	{
		class AudioLevelMeter : public AudioFilter
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
			AudioLevelMeter(NN<AudioSource> sourceAudio);
			virtual ~AudioLevelMeter();

			virtual Data::Duration SeekToTime(Data::Duration time);
			virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size

			Double GetLevel(UOSInt channel);
		};
	}
}
#endif
