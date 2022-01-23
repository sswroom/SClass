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
		return {UTF8STRC("GET")};
	case RequestMethod::HTTP_POST:
		return {UTF8STRC("POST")};
	case RequestMethod::HTTP_PUT:
		return {UTF8STRC("PUT")};
	case RequestMethod::HTTP_PATCH:
		return {UTF8STRC("PATCH")};
	case RequestMethod::HTTP_DELETE:
		return {UTF8STRC("DELETE")};
	case RequestMethod::HTTP_CONNECT:
		return {UTF8STRC("CONNECT")};
	case RequestMethod::RTSP_DESCRIBE:
		return {UTF8STRC("DESCRIBE")};
	case RequestMethod::RTSP_ANNOUNCE:
		return {UTF8STRC("ANNOUNCE")};
	case RequestMethod::RTSP_GET_PARAMETER:
		return {UTF8STRC("GET_PARAMETER")};
	case RequestMethod::RTSP_OPTIONS:
		return {UTF8STRC("OPTIONS")};
	case RequestMethod::RTSP_PAUSE:
		return {UTF8STRC("PAUSE")};
	case RequestMethod::RTSP_PLAY:
		return {UTF8STRC("PLAY")};
	case RequestMethod::RTSP_RECORD:
		return {UTF8STRC("RECORD")};
	case RequestMethod::RTSP_REDIRECT:
		return {UTF8STRC("REDIRECT")};
	case RequestMethod::RTSP_SETUP:
		return {UTF8STRC("SETUP")};
	case RequestMethod::RTSP_SET_PARAMETER:
		return {UTF8STRC("SET_PARAMETER")};
	case RequestMethod::RTSP_TEARDOWN:
		return {UTF8STRC("TEARDOWN")};
	case RequestMethod::Unknown:
	default:
		return {UTF8STRC("?")};
	}
}