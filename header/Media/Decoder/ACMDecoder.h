#ifndef _SM_MEDIA_DECODER_ACMDECODER
#define _SM_MEDIA_DECODER_ACMDECODER
#include "Media/Decoder/ADecoderBase.h"

//require msacm32.lib

namespace Media
{
	namespace Decoder
	{
		class ACMDecoder : public Media::Decoder::ADecoderBase
		{
		private:
			void *hAcmStream;
			void *acmFmt;
			Media::AudioFormat *decFmt;
			UInt8 *acmsh;
			UInt8 *acmOupBuff;
			UOSInt acmOupBuffSize;
			UOSInt acmOupBuffLeft;
			UInt8 *acmInpBuff;
			UOSInt acmInpBuffSize;
			Bool seeked;
			UInt32 srcFormatTag;

			Optional<Sync::Event> readEvt;

			void FreeACM();
			void InitACM();
		public:
			ACMDecoder(NN<IAudioSource> sourceAudio);
			virtual ~ACMDecoder();

			virtual void GetFormat(NN<AudioFormat> format);

			virtual Data::Duration SeekToTime(Data::Duration time);
			virtual Bool Start(Optional<Sync::Event> evt, UOSInt blkSize);
			virtual void Stop();
			virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size
			virtual UOSInt GetMinBlockSize();
		};
	}
}
#endif
