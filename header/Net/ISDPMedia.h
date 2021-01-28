#ifndef _SM_NET_ISDPMEDIA
#define _SM_NET_ISDPMEDIA
#include "Media/IMediaSource.h"
#include "Net/ISDPData.h"

namespace Net
{
	class ISDPMedia
	{
	public:
		virtual Media::MediaType GetSDPMediaType() = 0;
		virtual Int32 GetSDPMediaPort() = 0;
		virtual const UTF8Char *GetSDPProtocol() = 0;
		virtual const UTF8Char *GetSDPControlURL(const UTF8Char *reqUserAgent) = 0;
		virtual OSInt GetSDPDataCount() = 0;
		virtual Net::ISDPData *GetSDPData(OSInt index) = 0;
	};
};
#endif
