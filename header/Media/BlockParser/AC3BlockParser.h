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

			virtual Media::AudioBlockSource *ParseStreamData(IO::IStreamData *stmData);
			Bool ParseStreamFormat(UInt8 *buff, UOSInt buffSize, Media::AudioFormat *fmt);
		};
	}
}
#endif
