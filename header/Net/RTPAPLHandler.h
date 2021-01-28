#ifndef _SM_NET_RTPAPLHANDLER
#define _SM_NET_RTPAPLHANDLER
#include "Media/IAudioSource.h"
#include "Net/IRTPPLHandler.h"

namespace Net
{
	class RTPAPLHandler : public Media::IAudioSource, public IRTPPLHandler 
	{
	};
};
#endif
