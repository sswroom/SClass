#ifndef _SM_MEDIA_BLOCKPARSER_AC3BLOCKPARSER
#define _SM_MEDIA_BLOCKPARSER_AC3BLOCKPARSER
#include "Media/AudioBlockParser.h"

namespace Media
{
	namespace BlockParser
	{
		class AC3BlockParser : public Media::AudioBlockParser
		{
		public:
			AC3BlockParser();
			virtual ~AC3BlockParser();

			virtual Optional<Media::AudioBlockSource> ParseStreamData(NN<IO::StreamData> stmData);
			Bool ParseStreamFormat(UnsafeArray<UInt8> buff, UOSInt buffSize, NN<Media::AudioFormat> fmt);
		};
	}
}
#endif
