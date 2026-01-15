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
			UIntOS align;

			UInt8 *readBuff;
			UIntOS readBuffSize;
			Optional<Sync::Event> readEvt;
		public:
			G711muLawDecoder(NN<AudioSource> sourceAudio);
			virtual ~G711muLawDecoder();

			virtual void GetFormat(NN<AudioFormat> format);

			virtual Data::Duration SeekToTime(Data::Duration time);
			virtual Bool Start(Optional<Sync::Event> evt, UIntOS blkSize);
			virtual void Stop();
			virtual UIntOS ReadBlock(Data::ByteArray blk); //ret actual block size
			virtual UIntOS GetMinBlockSize();
		};
	}
}
#endif
