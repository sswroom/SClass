#ifndef _SM_TEXT_URLSTRING
#define _SM_TEXT_URLSTRING
#include "Text/CString.h"

namespace Text
{
	class URLString
	{
	public:
		static UnsafeArrayOpt<UTF8Char> GetURLFilePath(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> url, UOSInt urlLen);
		static UnsafeArray<UTF8Char> GetURLDomain(UnsafeArray<UTF8Char> sbuff, Text::CStringNN url, OptOut<UInt16> port);

		static UnsafeArrayOpt<UTF8Char> GetURIScheme(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> url, UOSInt urlLen);
		static UnsafeArray<UTF8Char> GetURLHost(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> url, UOSInt urlLen);
		static UnsafeArray<UTF8Char> GetURLPath(UnsafeArray<UTF8Char> sbuff, Text::CStringNN url);
		static UnsafeArray<UTF8Char> GetURLPathSvr(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> url, UOSInt urlLen);
		static UnsafeArray<UTF8Char> GetURLQueryString(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> url, UOSInt urlLen);
		static Text::CStringNN GetURLPathQuery(Text::CStringNN url);

		static UnsafeArrayOpt<UTF8Char> AppendURLPath(UnsafeArray<UTF8Char> sbuff, UnsafeArray<UTF8Char> sbuffEnd, Text::CStringNN path);
	};
}
#endif
