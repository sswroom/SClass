#ifndef _SM_MEDIA_ENCODER_G711MULAWENCODER
#define _SM_MEDIA_ENCODER_G711MULAWENCODER
#include "Media/AudioSource.h"

namespace Media
{
	namespace Encoder
	{
		class G711muLawEncoder : public AudioSource
		{
		private:
			Media::AudioSource *sourceAudio;
			Int32 align;

			UInt8 *readBuff;
			IntOS readBuffSize;
		public:
			G711muLawEncoder(AudioSource *sourceAudio);
			virtual ~G711muLawEncoder();

			virtual WChar *GetName(WChar *buff);
			virtual Bool CanSeek();
			virtual Data::Duration GetStreamTime(); //ms
			virtual void GetFormat(AudioFormat *format);

			virtual Data::Duration SeekToTime(Data::Duration time);
			virtual Bool Start(Sync::Event *evt, Int32 blkSize);
			virtual void Stop();
			virtual IntOS ReadBlock(UInt8 *buff, IntOS blkSize); //ret actual block size
			virtual UIntOS GetMinBlockSize();
		};
	};
};
#endif
