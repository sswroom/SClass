#ifndef _SM_MEDIA_AUDIOFILTER_FILEMIXFILTER
#define _SM_MEDIA_AUDIOFILTER_FILEMIXFILTER
#include "Media/IAudioFilter.h"
#include "Parser/ParserList.h"
#include "Text/String.h"

namespace Media
{
	namespace AudioFilter
	{
		class FileMixFilter : public IAudioFilter
		{
		private:
			AudioFormat format;
			NotNullPtr<Parser::ParserList> parsers;
			Media::IAudioSource *fileSrc;
			Bool mixing;
			UInt64 mixOfst;
			Bool chMix;

		public:
			FileMixFilter(NotNullPtr<IAudioSource> sourceAudio, NotNullPtr<Parser::ParserList> parsers);
			virtual ~FileMixFilter();

			virtual void GetFormat(AudioFormat *format);
			virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size

			Bool LoadFile(NotNullPtr<Text::String> fileName);
			Bool StartMix();
			Bool StopMix();
		};
	}
}
#endif
