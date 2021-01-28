#include "Stdafx.h"
#include "Net/WebStatus.h"

Net::WebStatus::StatusInfo Net::WebStatus::statInfo[] = {
	{100, "Continue"},
	{101, "Switch Protocols"},
	{200, "OK"},
	{201, "Created"},
	{202, "Accepted"},
	{203, "Non-Authoritative Information"},
	{204, "No Content"},
	{205, "Reset Content"},
	{206, "Partial Content"},
	{300, "Multiple Choices"},
	{301, "Moved Permanently"},
	{302, "Moved Temporarily"},
	{303, "See Other"},
	{304, "Not Modified"},
	{305, "Use Proxy"},
	{306, "(Unused)"},
	{307, "Temporary Redirect"},
	{400, "Bad Request"},
	{401, "Unauthorized"},
	{402, "Payment Required"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{405, "Method Not Allowed"},
	{406, "Not Acceptable"},
	{407, "Proxy Authentication Required"},
	{408, "Request Timeout"},
	{409, "Conflict"},
	{410, "Gone"},
	{411, "Length Required"},
	{412, "Precondition Failed"},
	{413, "Request Entity Too Large"},
	{414, "Request-URI Too Long"},
	{415, "Unsupported Media Type"},
	{416, "Requested Range Not Satisfiable"},
	{417, "Expectation Failed"},
	{451, "Parameter Not Understood"},
	{452, "Conference Not Found"},
	{453, "Not Enough Bandwidth"},
/*45x" 	; Session Not Found
  	| 	"45x" 	; Method Not Valid in This State
  	| 	"45x" 	; Header Field Not Valid for Resource
  	| 	"45x" 	; Invalid Range
  	| 	"45x" 	; Parameter Is Read-Only
  	| 	"45x" 	; Aggregate operation not allowed
  	| 	"45x" 	; Only aggregate operation allowed*/
	{461, "Unsupported transport"},
	{500, "Internal Server Error"},
	{501, "Not Implemented"},
	{502, "Bad Gateway"},
	{503, "Service Unavailable"},
	{504, "Gateway Timeout"},
	{505, "Version Not Supported"}
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

const UTF8Char *Net::WebStatus::GetCodeName(StatusCode code)
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
			return (const UTF8Char*)statInfo[k].name;
		}
	}
	return 0;
}
