#ifndef _SM_MEDIA_H265PARSER
#define _SM_MEDIA_H265PARSER
#include "Data/ArrayListInt32.h"
#include "IO/BitReaderMSB.h"
#include "Media/FrameInfo.h"

namespace Media
{
	class H265Parser
	{
	public:
		static Bool GetFrameInfoSPS(const UInt8 *frame, UOSInt frameSize, NotNullPtr<Media::FrameInfo> info); //Only update defined values
	};
}
#endif
