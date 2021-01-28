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
			virtual Int32 GetStreamTime(); //ms
			virtual void GetFormat(AudioFormat *format);

			virtual Int32 SeekToTime(Int32 time); //ms, ret actual time
			virtual Bool Start(Sync::Event *evt, Int32 blkSize);
			virtual void Stop();
			virtual OSInt ReadBlock(UInt8 *buff, OSInt blkSize); //ret actual block size
			virtual OSInt GetMinBlockSize();
		};
	};
};
#endif
