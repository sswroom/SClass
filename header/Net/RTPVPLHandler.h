#ifndef _SM_NET_RTPVPLHANDLER
#define _SM_NET_RTPVPLHANDLER
#include "Media/VideoSourceBase.h"
#include "Net/RTPPayloadHandler.h"

namespace Net
{
	class RTPVPLHandler : public Media::VideoSourceBase, public RTPPayloadHandler 
	{
	};
};
#endif
