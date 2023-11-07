#ifndef _SM_MEDIA_ENCODER_G711MULAWENCODER
#define _SM_MEDIA_ENCODER_G711MULAWENCODER
#include "Media/IAudioSource.h"

namespace Media
{
	namespace Encoder
	{
		class G711muLawEncoder : public IAudioSource
		{
		private:
			Media::IAudioSource *sourceAudio;
			Int32 align;

			UInt8 *readBuff;
			OSInt readBuffSize;
		public:
			G711muLawEncoder(IAudioSource *sourceAudio);
			virtual ~G711muLawEncoder();

			virtual WChar *GetName(WChar *buff);
			virtual Bool CanSeek();
			virtual Data::Duration GetStreamTime(); //ms
			virtual void GetFormat(AudioFormat *format);

			virtual Data::Duration SeekToTime(Data::Duration time);
			virtual Bool Start(Sync::Event *evt, Int32 blkSize);
			virtual void Stop();
			virtual OSInt ReadBlock(UInt8 *buff, OSInt blkSize); //ret actual block size
			virtual UOSInt GetMinBlockSize();
		};
	};
};
#endif
