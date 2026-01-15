#ifndef _SM_MEDIA_DECODER_MP2DECODER
#define _SM_MEDIA_DECODER_MP2DECODER
#include "Media/Decoder/ADecoderBase.h"

namespace Media
{
	namespace Decoder
	{
		class MP2Decoder : public Media::Decoder::ADecoderBase
		{
		private:
			UInt32 nChannel;
			void *context;
			UIntOS blkSize;
			UInt64 totalReadSize;

			Optional<Sync::Event> readEvt;
		public:
			MP2Decoder(NN<AudioSource> sourceAudio);
			virtual ~MP2Decoder();

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
