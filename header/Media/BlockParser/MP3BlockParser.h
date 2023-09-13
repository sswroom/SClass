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

			virtual Media::AudioBlockSource *ParseStreamData(NotNullPtr<IO::StreamData> stmData);
			Bool ParseStreamFormat(UInt8 *buff, UOSInt buffSize, NotNullPtr<Media::AudioFormat> fmt);
		};
	}
}
#endif
