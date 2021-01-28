#ifndef _SM_MEDIA_DECODER_PSSADECODER
#define _SM_MEDIA_DECODER_PSSADECODER
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

			UInt8 *readBuff;
			UOSInt readBuffSize;
			UOSInt buffSize;
			Sync::Event *readEvt;
		public:
			PSSADecoder(IAudioSource *sourceAudio);
			virtual ~PSSADecoder();

			virtual void GetFormat(AudioFormat *format);

			virtual Int32 SeekToTime(Int32 time); //ms, ret actual time
			virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
			virtual void Stop();
			virtual UOSInt ReadBlock(UInt8 *buff, UOSInt blkSize); //ret actual block size
			virtual UOSInt GetMinBlockSize();

		private:
			void Convert(UInt8 *src, UInt8 *dest, Int32 sampleByte, Int32 channel);
		};
	}
}
#endif
