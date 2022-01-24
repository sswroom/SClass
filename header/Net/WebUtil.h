#ifndef _SM_NET_WEBUTIL
#define _SM_NET_WEBUTIL
#include "Text/CString.h"
#include "Text/MyString.h"

namespace Net
{
	class WebUtil
	{
	public:
		enum class RequestMethod
		{
			Unknown,
			HTTP_GET,
			HTTP_POST,
			HTTP_PUT,
			HTTP_PATCH,
			HTTP_DELETE,
			HTTP_CONNECT,
			RTSP_DESCRIBE,
			RTSP_ANNOUNCE,
			RTSP_GET_PARAMETER,
			RTSP_OPTIONS,
			RTSP_PAUSE,
			RTSP_PLAY,
			RTSP_RECORD,
			RTSP_REDIRECT,
			RTSP_SETUP,
			RTSP_SET_PARAMETER,
			RTSP_TEARDOWN
		};

	public:
		static RequestMethod Str2RequestMethod(const UTF8Char *s, UOSInt len);
		static Text::CString RequestMethodGetName(RequestMethod reqMeth);
	};
}
#endif
