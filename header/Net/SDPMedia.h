#ifndef _SM_NET_SDPMEDIA
#define _SM_NET_SDPMEDIA
#include "Media/MediaSource.h"
#include "Net/SDPData.h"
#include "Text/CString.h"

namespace Net
{
	class SDPMedia
	{
	public:
		virtual Media::MediaType GetSDPMediaType() = 0;
		virtual UInt16 GetSDPMediaPort() = 0;
		virtual Text::CStringNN GetSDPProtocol() = 0;
		virtual Text::CString GetSDPControlURL(Text::CString reqUserAgent) = 0;
		virtual UIntOS GetSDPDataCount() = 0;
		virtual Optional<Net::SDPData> GetSDPData(UIntOS index) = 0;
	};
}
#endif
