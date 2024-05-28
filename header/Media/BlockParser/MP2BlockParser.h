#ifndef _SM_MEDIA_BLOCKPARSER_MP2BLOCKPARSER
#define _SM_MEDIA_BLOCKPARSER_MP2BLOCKPARSER
#include "Media/AudioBlockParser.h"

namespace Media
{
	namespace BlockParser
	{
		class MP2BlockParser : public Media::AudioBlockParser
		{
		public:
			MP2BlockParser();
			virtual ~MP2BlockParser();

			virtual Optional<Media::AudioBlockSource> ParseStreamData(NN<IO::StreamData> stmData);
			Bool ParseStreamFormat(UInt8 *buff, UOSInt buffSize, NN<Media::AudioFormat> fmt);
		};
	}
}
#endif
