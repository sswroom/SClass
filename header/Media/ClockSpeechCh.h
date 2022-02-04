#ifndef _SM_MEDIA_CLOCKSPEECHCH
#define _SM_MEDIA_CLOCKSPEECHCH
#include "Data/DateTime.h"
#include "Media/IAudioSource.h"
#include "Media/AudioConcatSource.h"
#include "Parser/FileParser/WAVParser.h"

namespace Media
{
	class ClockSpeechCh
	{
	private:
		static void AppendWAV(Media::AudioConcatSource *source, Parser::FileParser::WAVParser *parser, Text::CString fileName);
	public:
		static Media::IAudioSource *GetSpeech(Data::DateTime *time);
	};
};
#endif
