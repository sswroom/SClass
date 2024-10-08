#include "Stdafx.h"
#include "Net/WebUtil.h"

Net::WebUtil::RequestMethod Net::WebUtil::Str2RequestMethod(Text::CStringNN s)
{
	switch (s.leng)
	{
	case 3:
		if (s.Equals(UTF8STRC("GET")))
		{
			return RequestMethod::HTTP_GET;
		}
		else if (s.Equals(UTF8STRC("PUT")))
		{
			return RequestMethod::HTTP_PUT;
		}
		break;
	case 4:
		if (s.Equals(UTF8STRC("POST")))
		{
			return RequestMethod::HTTP_POST;
		}
		else if (s.Equals(UTF8STRC("HEAD")))
		{
			return RequestMethod::HTTP_HEAD;
		}
		else if (s.Equals(UTF8STRC("PLAY")))
		{
			return RequestMethod::RTSP_PLAY;
		}
		break;
	case 5:
		if (s.Equals(UTF8STRC("PATCH")))
		{
			return RequestMethod::HTTP_PATCH;
		}
		else if (s.Equals(UTF8STRC("TRACE")))
		{
			return RequestMethod::HTTP_PATCH;
		}
		else if (s.Equals(UTF8STRC("SETUP")))
		{
			return RequestMethod::RTSP_SETUP;
		}
		else if (s.Equals(UTF8STRC("PAUSE")))
		{
			return RequestMethod::RTSP_PAUSE;
		}
		break;
	case 6:
		if (s.Equals(UTF8STRC("DELETE")))
		{
			return RequestMethod::HTTP_DELETE;
		}
		else if (s.Equals(UTF8STRC("RECORD")))
		{
			return RequestMethod::RTSP_RECORD;
		}
		break;
	case 7:
		if (s.Equals(UTF8STRC("CONNECT")))
		{
			return RequestMethod::HTTP_CONNECT;
		}
		else if (s.Equals(UTF8STRC("OPTIONS")))
		{
			return RequestMethod::HTTP_OPTIONS;
		}
		break;
	case 8:
		if (s.Equals(UTF8STRC("DESCRIBE")))
		{
			return RequestMethod::RTSP_DESCRIBE;
		}
		else if (s.Equals(UTF8STRC("ANNOUNCE")))
		{
			return RequestMethod::RTSP_ANNOUNCE;
		}
		else if (s.Equals(UTF8STRC("REDIRECT")))
		{
			return RequestMethod::RTSP_REDIRECT;
		}
		else if (s.Equals(UTF8STRC("TEARDOWN")))
		{
			return RequestMethod::RTSP_TEARDOWN;
		}
		break;
	case 13:
		if (s.Equals(UTF8STRC("GET_PARAMETER")))
		{
			return RequestMethod::RTSP_GET_PARAMETER;
		}
		else if (s.Equals(UTF8STRC("SET_PARAMETER")))
		{
			return RequestMethod::RTSP_SET_PARAMETER;
		}
		break;
	}
	return RequestMethod::Unknown;
}

Text::CStringNN Net::WebUtil::RequestMethodGetName(RequestMethod reqMeth)
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
	case RequestMethod::HTTP_HEAD:
		return CSTR("HEAD");
	case RequestMethod::HTTP_OPTIONS:
		return CSTR("OPTIONS");
	case RequestMethod::HTTP_TRACE:
		return CSTR("TRACE");
	case RequestMethod::RTSP_DESCRIBE:
		return CSTR("DESCRIBE");
	case RequestMethod::RTSP_ANNOUNCE:
		return CSTR("ANNOUNCE");
	case RequestMethod::RTSP_GET_PARAMETER:
		return CSTR("GET_PARAMETER");
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

UnsafeArray<UTF8Char> Net::WebUtil::Date2Str(UnsafeArray<UTF8Char> sbuff, NN<Data::DateTime> dt)
{
	const Char *wds[] = {"Sun, ", "Mon, ", "Tue, ", "Wed, ", "Thu, ", "Fri, ", "Sat, "};
	Data::DateTime t(dt);
	t.ToUTCTime();
	Int32 wd = (Int32)t.GetWeekday();
	return Text::StrConcatC(t.ToString(Text::StrConcatC(sbuff, (const UTF8Char*)wds[wd], 5), "dd MMM yyyy HH:mm:ss"), UTF8STRC(" GMT"));
}

UnsafeArray<UTF8Char> Net::WebUtil::Date2Str(UnsafeArray<UTF8Char> sbuff, const Data::Timestamp &ts)
{
	const Char *wds[] = {"Sun, ", "Mon, ", "Tue, ", "Wed, ", "Thu, ", "Fri, ", "Sat, "};
	Data::DateTime t(ts.inst, 0);
	Int32 wd = (Int32)t.GetWeekday();
	return Text::StrConcatC(t.ToString(Text::StrConcatC(sbuff, (const UTF8Char*)wds[wd], 5), "dd MMM yyyy HH:mm:ss"), UTF8STRC(" GMT"));	
}

void Net::WebUtil::Date2Str(NN<Text::StringBuilderUTF8> sb, const Data::Timestamp &ts)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	const Char *wds[] = {"Sun, ", "Mon, ", "Tue, ", "Wed, ", "Thu, ", "Fri, ", "Sat, "};
	Data::DateTime t(ts.inst, 0);
	Int32 wd = (Int32)t.GetWeekday();
	sb->AppendC((const UTF8Char*)wds[wd], 5);
	sptr = t.ToString(sbuff, "dd MMM yyyy HH:mm:ss");
	sb->AppendP(sbuff, sptr);
	sb->AppendC(UTF8STRC(" GMT"));
}
