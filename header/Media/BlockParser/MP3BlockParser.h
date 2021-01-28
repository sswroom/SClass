#ifndef _SM_MEDIA_BLOCKPARSER_MP3BLOCKPARSER
#define _SM_MEDIA_BLOCKPARSER_MP3BLOCKPARSER
#include "Media/AudioBlockParser.h"

namespace Media
{
	namespace BlockParser
	{
		class MP3BlockParser : public Media::AudioBlockParser
		{
		public:
			MP3BlockParser();
			virtual ~MP3BlockParser();

			virtual Media::AudioBlockSource *ParseStreamData(IO::IStreamData *stmData);
			Bool ParseStreamFormat(UInt8 *buff, OSInt buffSize, Media::AudioFormat *fmt);
		};
	};
};
#endif
