#ifndef _SM_MEDIA_AUDIOBLOCKPARSER
#define _SM_MEDIA_AUDIOBLOCKPARSER
#include "Media/AudioBlockSource.h"

namespace Media
{
	class AudioBlockParser
	{
	public:
		AudioBlockParser();
		virtual ~AudioBlockParser();

		virtual Media::AudioBlockSource *ParseStreamData(NN<IO::StreamData> stmData) = 0;
		static Media::AudioBlockParser *CreateParser(NN<IO::StreamData> stmData);
	};
};
#endif
