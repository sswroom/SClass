#ifndef _SM_MEDIA_APE_APEFILE
#define _SM_MEDIA_APE_APEFILE
#include "IO/StreamData.h"
#include "Media/MediaFile.h"

namespace Media
{
	namespace APE
	{
		class APEFile
		{
		public:
			static Media::MediaFile *ParseData(NN<IO::StreamData> data);
		};
	};
};
#endif
