#include "Stdafx.h"
#include "Net/WebStatus.h"

Net::WebStatus::StatusInfo Net::WebStatus::statInfo[] = {
	{100, UTF8STRC("Continue")},
	{101, UTF8STRC("Switch Protocols")},
	{200, UTF8STRC("OK")},
	{201, UTF8STRC("Created")},
	{202, UTF8STRC("Accepted")},
	{203, UTF8STRC("Non-Authoritative Information")},
	{204, UTF8STRC("No Content")},
	{205, UTF8STRC("Reset Content")},
	{206, UTF8STRC("Partial Content")},
	{300, UTF8STRC("Multiple Choices")},
	{301, UTF8STRC("Moved Permanently")},
	{302, UTF8STRC("Moved Temporarily")},
	{303, UTF8STRC("See Other")},
	{304, UTF8STRC("Not Modified")},
	{305, UTF8STRC("Use Proxy")},
	{306, UTF8STRC("(Unused)")},
	{307, UTF8STRC("Temporary Redirect")},
	{400, UTF8STRC("Bad Request")},
	{401, UTF8STRC("Unauthorized")},
	{402, UTF8STRC("Payment Required")},
	{403, UTF8STRC("Forbidden")},
	{404, UTF8STRC("Not Found")},
	{405, UTF8STRC("Method Not Allowed")},
	{406, UTF8STRC("Not Acceptable")},
	{407, UTF8STRC("Proxy Authentication Required")},
	{408, UTF8STRC("Request Timeout")},
	{409, UTF8STRC("Conflict")},
	{410, UTF8STRC("Gone")},
	{411, UTF8STRC("Length Required")},
	{412, UTF8STRC("Precondition Failed")},
	{413, UTF8STRC("Request Entity Too Large")},
	{414, UTF8STRC("Request-URI Too Long")},
	{415, UTF8STRC("Unsupported Media Type")},
	{416, UTF8STRC("Requested Range Not Satisfiable")},
	{417, UTF8STRC("Expectation Failed")},
	{451, UTF8STRC("Parameter Not Understood")},
	{452, UTF8STRC("Conference Not Found")},
	{453, UTF8STRC("Not Enough Bandwidth")},
/*45x" 	; Session Not Found
  	| 	"45x" 	; Method Not Valid in This State
  	| 	"45x" 	; Header Field Not Valid for Resource
  	| 	"45x" 	; Invalid Range
  	| 	"45x" 	; Parameter Is Read-Only
  	| 	"45x" 	; Aggregate operation not allowed
  	| 	"45x" 	; Only aggregate operation allowed*/
	{461, UTF8STRC("Unsupported transport")},
	{500, UTF8STRC("Internal Server Error")},
	{501, UTF8STRC("Not Implemented")},
	{502, UTF8STRC("Bad Gateway")},
	{503, UTF8STRC("Service Unavailable")},
	{504, UTF8STRC("Gateway Timeout")},
	{505, UTF8STRC("Version Not Supported")}
};

Bool Net::WebStatus::IsExist(Int32 code)
{
	OSInt i = 0;
	OSInt j = (sizeof(statInfo) / sizeof(statInfo[0])) - 1;
	OSInt k;
	while (i <= j)
	{
		k = (i + j) >> 1;
		if (statInfo[k].code > code)
		{
			j = k - 1;
		}
		else if (statInfo[k].code < code)
		{
			i = k + 1;
		}
		else
		{
			return true;
		}
	}
	return false;
}

Text::CString Net::WebStatus::GetCodeName(StatusCode code)
{
	OSInt i = 0;
	OSInt j = (sizeof(statInfo) / sizeof(statInfo[0])) - 1;
	OSInt k;
	while (i <= j)
	{
		k = (i + j) >> 1;
		if (statInfo[k].code > code)
		{
			j = k - 1;
		}
		else if (statInfo[k].code < code)
		{
			i = k + 1;
		}
		else
		{
			return {statInfo[k].name, statInfo[k].nameLen};
		}
	}
	return CSTR_NULL;
}
