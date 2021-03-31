#ifndef _SM_MEDIA_DECODER_XADECODER
#define _SM_MEDIA_DECODER_XADECODER
#include "Media/Decoder/ADecoderBase.h"

namespace Media
{
	namespace Decoder
	{
		class XADecoder : public Media::Decoder::ADecoderBase
		{
		private:
			Int32 nChannels;
			Int64 adxSample1[2];
			Int64 adxSample2[2];

			UInt8 *readBuff;
			UOSInt readBuffSize;
			Sync::Event *readEvt;
		public:
			XADecoder(IAudioSource *sourceAudio);
			virtual ~XADecoder();

			virtual void GetFormat(AudioFormat *format);

			virtual UInt32 SeekToTime(UInt32 time); //ms, ret actual time
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
