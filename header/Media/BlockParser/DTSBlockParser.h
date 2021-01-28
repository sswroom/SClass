#ifndef _SM_MEDIA_BLOCKPARSER_DTSBLOCKPARSER
#define _SM_MEDIA_BLOCKPARSER_DTSBLOCKPARSER
#include "Media/AudioBlockParser.h"

namespace Media
{
	namespace BlockParser
	{
		class DTSBlockParser : public Media::AudioBlockParser
		{
		public:
			DTSBlockParser();
			virtual ~DTSBlockParser();

			virtual Media::AudioBlockSource *ParseStreamData(IO::IStreamData *stmData);
		};
	};
};
#endif
