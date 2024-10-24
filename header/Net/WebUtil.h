#ifndef _SM_NET_WEBUTIL
#define _SM_NET_WEBUTIL
#include "Data/DateTime.h"
#include "Data/Timestamp.h"
#include "Text/CString.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

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
			HTTP_HEAD,
			HTTP_OPTIONS,
			HTTP_TRACE,
			RTSP_DESCRIBE,
			RTSP_ANNOUNCE,
			RTSP_GET_PARAMETER,
			RTSP_PAUSE,
			RTSP_PLAY,
			RTSP_RECORD,
			RTSP_REDIRECT,
			RTSP_SETUP,
			RTSP_SET_PARAMETER,
			RTSP_TEARDOWN
		};

	public:
		static RequestMethod Str2RequestMethod(Text::CStringNN s);
		static Text::CStringNN RequestMethodGetName(RequestMethod reqMeth);
		static UnsafeArray<UTF8Char> Date2Str(UnsafeArray<UTF8Char> sbuff, NN<Data::DateTime> dt);
		static UnsafeArray<UTF8Char> Date2Str(UnsafeArray<UTF8Char> sbuff, const Data::Timestamp &ts);
		static void Date2Str(NN<Text::StringBuilderUTF8> sb, const Data::Timestamp &ts);
	};
}
#endif
