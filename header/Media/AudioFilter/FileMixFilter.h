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
			AudioFormat *format;
			Parser::ParserList *parsers;
			Media::IAudioSource *fileSrc;
			Bool mixing;
			UInt64 mixOfst;
			Bool chMix;

		public:
			FileMixFilter(IAudioSource *sourceAudio, Parser::ParserList *parsers);
			virtual ~FileMixFilter();

			virtual void GetFormat(AudioFormat *format);
			virtual UOSInt ReadBlock(UInt8 *buff, UOSInt blkSize); //ret actual block size

			Bool LoadFile(NotNullPtr<Text::String> fileName);
			Bool StartMix();
			Bool StopMix();
		};
	}
}
#endif
