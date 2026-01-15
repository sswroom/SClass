#ifndef _SM_MEDIA_AFILTER_FILEMIXFILTER
#define _SM_MEDIA_AFILTER_FILEMIXFILTER
#include "Media/AudioFilter.h"
#include "Parser/ParserList.h"
#include "Text/String.h"

namespace Media
{
	namespace AFilter
	{
		class FileMixFilter : public AudioFilter
		{
		private:
			AudioFormat format;
			NN<Parser::ParserList> parsers;
			Optional<Media::AudioSource> fileSrc;
			Bool mixing;
			UInt64 mixOfst;
			Bool chMix;

		public:
			FileMixFilter(NN<AudioSource> sourceAudio, NN<Parser::ParserList> parsers);
			virtual ~FileMixFilter();

			virtual void GetFormat(NN<AudioFormat> format);
			virtual UIntOS ReadBlock(Data::ByteArray blk); //ret actual block size

			Bool LoadFile(NN<Text::String> fileName);
			Bool StartMix();
			Bool StopMix();
		};
	}
}
#endif
