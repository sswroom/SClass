#include "Stdafx.h"
#include "Net/WebUtil.h"

Net::WebUtil::RequestMethod Net::WebUtil::Str2RequestMethod(const UTF8Char *s, UOSInt len)
{
	switch (len)
	{
	case 3:
		if (Text::StrEqualsC(s, len, UTF8STRC("GET")))
		{
			return RequestMethod::HTTP_GET;
		}
		else if (Text::StrEqualsC(s, len, UTF8STRC("PUT")))
		{
			return RequestMethod::HTTP_PUT;
		}
		break;
	case 4:
		if (Text::StrEqualsC(s, len, UTF8STRC("POST")))
		{
			return RequestMethod::HTTP_POST;
		}
		else if (Text::StrEqualsC(s, len, UTF8STRC("PLAY")))
		{
			return RequestMethod::RTSP_PLAY;
		}
		break;
	case 5:
		if (Text::StrEqualsC(s, len, UTF8STRC("PATCH")))
		{
			return RequestMethod::HTTP_PATCH;
		}
		else if (Text::StrEqualsC(s, len, UTF8STRC("SETUP")))
		{
			return RequestMethod::RTSP_SETUP;
		}
		else if (Text::StrEqualsC(s, len, UTF8STRC("PAUSE")))
		{
			return RequestMethod::RTSP_PAUSE;
		}
		break;
	case 6:
		if (Text::StrEqualsC(s, len, UTF8STRC("DELETE")))
		{
			return RequestMethod::HTTP_DELETE;
		}
		else if (Text::StrEqualsC(s, len, UTF8STRC("RECORD")))
		{
			return RequestMethod::RTSP_RECORD;
		}
		break;
	case 7:
		if (Text::StrEqualsC(s, len, UTF8STRC("CONNECT")))
		{
			return RequestMethod::HTTP_CONNECT;
		}
		else if (Text::StrEqualsC(s, len, UTF8STRC("OPTIONS")))
		{
			return RequestMethod::RTSP_OPTIONS;
		}
		break;
	case 8:
		if (Text::StrEqualsC(s, len, UTF8STRC("DESCRIBE")))
		{
			return RequestMethod::RTSP_DESCRIBE;
		}
		else if (Text::StrEqualsC(s, len, UTF8STRC("ANNOUNCE")))
		{
			return RequestMethod::RTSP_ANNOUNCE;
		}
		else if (Text::StrEqualsC(s, len, UTF8STRC("REDIRECT")))
		{
			return RequestMethod::RTSP_REDIRECT;
		}
		else if (Text::StrEqualsC(s, len, UTF8STRC("TEARDOWN")))
		{
			return RequestMethod::RTSP_TEARDOWN;
		}
		break;
	case 13:
		if (Text::StrEqualsC(s, len, UTF8STRC("GET_PARAMETER")))
		{
			return RequestMethod::RTSP_GET_PARAMETER;
		}
		else if (Text::StrEqualsC(s, len, UTF8STRC("SET_PARAMETER")))
		{
			return RequestMethod::RTSP_SET_PARAMETER;
		}
		break;
	}
	return RequestMethod::Unknown;
}

Text::CString Net::WebUtil::RequestMethodGetName(RequestMethod reqMeth)
{
	switch (reqMeth)
	{
	case RequestMethod::HTTP_GET:
		return CSTR("GET");
	case RequestMethod::HTTP_POST:
		return CSTR("POST");
	case RequestMethod::HTTP_PUT:
		return CSTR("PUT");
	case RequestMethod::HTTP_PATCH:
		return CSTR("PATCH");
	case RequestMethod::HTTP_DELETE:
		return CSTR("DELETE");
	case RequestMethod::HTTP_CONNECT:
		return CSTR("CONNECT");
	case RequestMethod::RTSP_DESCRIBE:
		return CSTR("DESCRIBE");
	case RequestMethod::RTSP_ANNOUNCE:
		return CSTR("ANNOUNCE");
	case RequestMethod::RTSP_GET_PARAMETER:
		return CSTR("GET_PARAMETER");
	case RequestMethod::RTSP_OPTIONS:
		return CSTR("OPTIONS");
	case RequestMethod::RTSP_PAUSE:
		return CSTR("PAUSE");
	case RequestMethod::RTSP_PLAY:
		return CSTR("PLAY");
	case RequestMethod::RTSP_RECORD:
		return CSTR("RECORD");
	case RequestMethod::RTSP_REDIRECT:
		return CSTR("REDIRECT");
	case RequestMethod::RTSP_SETUP:
		return CSTR("SETUP");
	case RequestMethod::RTSP_SET_PARAMETER:
		return CSTR("SET_PARAMETER");
	case RequestMethod::RTSP_TEARDOWN:
		return CSTR("TEARDOWN");
	case RequestMethod::Unknown:
	default:
		return CSTR("?");
	}
}

UTF8Char *Net::WebUtil::Date2Str(UTF8Char *sbuff, Data::DateTime *dt)
{
	const Char *wds[] = {"Sun, ", "Mon, ", "Tue, ", "Wed, ", "Thu, ", "Fri, ", "Sat, "};
	Data::DateTime t;
	t.SetValue(dt);
	t.ToUTCTime();
	Int32 wd = (Int32)t.GetWeekday();
	return Text::StrConcatC(t.ToString(Text::StrConcatC(sbuff, (const UTF8Char*)wds[wd], 5), "dd MMM yyyy HH:mm:ss"), UTF8STRC(" GMT"));
}

UTF8Char *Net::WebUtil::Date2Str(UTF8Char *sbuff, Data::Timestamp ts)
{
	const Char *wds[] = {"Sun, ", "Mon, ", "Tue, ", "Wed, ", "Thu, ", "Fri, ", "Sat, "};
	Data::DateTime t;
	t.SetTicks(ts.ticks);
	t.ToUTCTime();
	Int32 wd = (Int32)t.GetWeekday();
	return Text::StrConcatC(t.ToString(Text::StrConcatC(sbuff, (const UTF8Char*)wds[wd], 5), "dd MMM yyyy HH:mm:ss"), UTF8STRC(" GMT"));	
}
