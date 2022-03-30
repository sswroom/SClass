#ifndef _SM_TEXT_URLSTRING
#define _SM_TEXT_URLSTRING
#include "Text/CString.h"

namespace Text
{
	class URLString
	{
	public:
		static UTF8Char *GetURLFilePath(UTF8Char *sbuff, const UTF8Char *url, UOSInt urlLen);
		static UTF8Char *GetURLDomain(UTF8Char *sbuff, const UTF8Char *url, UOSInt urlLen, UInt16 *port);

		static UTF8Char *GetURIScheme(UTF8Char *sbuff, const UTF8Char *url, UOSInt urlLen);
		static UTF8Char *GetURLHost(UTF8Char *sbuff, const UTF8Char *url, UOSInt urlLen);
		static UTF8Char *GetURLPath(UTF8Char *sbuff, const UTF8Char *url, UOSInt urlLen);
		static UTF8Char *GetURLPathSvr(UTF8Char *sbuff, const UTF8Char *url, UOSInt urlLen);
		static UTF8Char *GetURLQueryString(UTF8Char *sbuff, const UTF8Char *url, UOSInt urlLen);

		static UTF8Char *AppendURLPath(UTF8Char *sbuff, UTF8Char *sbuffEnd, Text::CString path);
	};
}
#endif
