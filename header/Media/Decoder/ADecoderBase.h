#ifndef _SM_MEDIA_DECODER_ADECODERBASE
#define _SM_MEDIA_DECODER_ADECODERBASE
#include "Media/IAudioSource.h"

namespace Media
{
	namespace Decoder
	{
		class ADecoderBase : public IAudioSource
		{
		protected:
			Optional<Media::IAudioSource> sourceAudio;
		public:
			ADecoderBase();
			virtual ~ADecoderBase();

			virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff);
			virtual Bool CanSeek();
			virtual Data::Duration GetStreamTime();
			virtual Data::Duration GetCurrTime();
			virtual Bool IsEnd();
			virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime);
		};
	}
}
#endif
