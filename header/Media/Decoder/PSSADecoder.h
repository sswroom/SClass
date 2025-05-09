#ifndef _SM_MEDIA_DECODER_PSSADECODER
#define _SM_MEDIA_DECODER_PSSADECODER
#include "Data/ByteBuffer.h"
#include "Media/Decoder/ADecoderBase.h"

namespace Media
{
	namespace Decoder
	{
		class PSSADecoder : public Media::Decoder::ADecoderBase
		{
		private:
			UInt32 nChannels;
			UInt32 nBlockAlign;
			Int64 adxSample1[2];
			Int64 adxSample2[2];

			Data::ByteBuffer readBuff;
			UOSInt readBuffSize;
			UOSInt buffSize;
			Optional<Sync::Event> readEvt;
		public:
			PSSADecoder(NN<AudioSource> sourceAudio);
			virtual ~PSSADecoder();

			virtual void GetFormat(NN<AudioFormat> format);

			virtual Data::Duration SeekToTime(Data::Duration time);
			virtual Bool Start(Optional<Sync::Event> evt, UOSInt blkSize);
			virtual void Stop();
			virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size
			virtual UOSInt GetMinBlockSize();

		private:
			void Convert(Data::ByteArray src, Data::ByteArray dest, Int32 sampleByte, Int32 channel);
		};
	}
}
#endif
