#ifndef _SM_MEDIA_DECODER_G711MULAWDECODER
#define _SM_MEDIA_DECODER_G711MULAWDECODER
#include "Media/Decoder/ADecoderBase.h"

namespace Media
{
	namespace Decoder
	{
		class G711muLawDecoder : public Media::Decoder::ADecoderBase
		{
		private:
			UOSInt align;

			UInt8 *readBuff;
			UOSInt readBuffSize;
			Sync::Event *readEvt;
		public:
			G711muLawDecoder(IAudioSource *sourceAudio);
			virtual ~G711muLawDecoder();

			virtual void GetFormat(AudioFormat *format);

			virtual UInt32 SeekToTime(UInt32 time); //ms, ret actual time
			virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
			virtual void Stop();
			virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size
			virtual UOSInt GetMinBlockSize();
		};
	}
}
#endif
