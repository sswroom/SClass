#ifndef _SM_MEDIA_DECODER_ADXDECODER
#define _SM_MEDIA_DECODER_ADXDECODER
#include "Data/ByteBuffer.h"
#include "Media/Decoder/ADecoderBase.h"

namespace Media
{
	namespace Decoder
	{
		class ADXDecoder : public Media::Decoder::ADecoderBase
		{
		private:
			Int32 nChannels;
			Int64 adxSample1[2];
			Int64 adxSample2[2];

			Data::ByteBuffer readBuff;
			Sync::Event *readEvt;
		public:
			ADXDecoder(NotNullPtr<IAudioSource> sourceAudio);
			virtual ~ADXDecoder();

			virtual void GetFormat(NotNullPtr<AudioFormat> format);

			virtual Data::Duration SeekToTime(Data::Duration time);
			virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
			virtual void Stop();
			virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size
			virtual UOSInt GetMinBlockSize();

		private:
			void Convert(Data::ByteArray src, Data::ByteArray dest, Int32 sampleByte, Int32 channel);
		};
	}
}
#endif
