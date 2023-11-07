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
			UOSInt blkSize;
			UInt64 totalReadSize;

			Sync::Event *readEvt;
		public:
			MP2Decoder(NotNullPtr<IAudioSource> sourceAudio);
			virtual ~MP2Decoder();

			virtual void GetFormat(NotNullPtr<AudioFormat> format);

			virtual Data::Duration SeekToTime(Data::Duration time);
			virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
			virtual void Stop();
			virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size
			virtual UOSInt GetMinBlockSize();
		};
	}
}
#endif
