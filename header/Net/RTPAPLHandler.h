#ifndef _SM_NET_RTPAPLHANDLER
#define _SM_NET_RTPAPLHANDLER
#include "Media/AudioSource.h"
#include "Net/RTPPayloadHandler.h"

namespace Net
{
	class RTPAPLHandler : public Media::AudioSource, public RTPPayloadHandler 
	{
	};
};
#endif
