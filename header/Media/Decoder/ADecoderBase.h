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
			Media::IAudioSource *sourceAudio;
		public:
			ADecoderBase();
			virtual ~ADecoderBase();

			virtual UTF8Char *GetSourceName(UTF8Char *buff);
			virtual Bool CanSeek();
			virtual Int32 GetStreamTime(); //ms
			virtual Int32 GetCurrTime();
			virtual Bool IsEnd();
			virtual Bool TrimStream(Int32 trimTimeStart, Int32 trimTimeEnd, Int32 *syncTime);
		};
	};
};
#endif
