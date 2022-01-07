#ifndef _SM_TEXT_URLSTRING
#define _SM_TEXT_URLSTRING

namespace Text
{
	class URLString
	{
	public:
		static UTF8Char *GetURLFilePath(UTF8Char *sbuff, const UTF8Char *url);
		static UTF8Char *GetURLDomain(UTF8Char *sbuff, const UTF8Char *url, UInt16 *port);

		static UTF8Char *GetURIScheme(UTF8Char *sbuff, const UTF8Char *url);
		static UTF8Char *GetURLHost(UTF8Char *sbuff, const UTF8Char *url);
		static UTF8Char *GetURLPath(UTF8Char *sbuff, const UTF8Char *url);
		static UTF8Char *GetURLPathSvr(UTF8Char *sbuff, const UTF8Char *url, UOSInt urlLen);
		static UTF8Char *GetURLQueryString(UTF8Char *sbuff, const UTF8Char *url);

		static UTF8Char *AppendURLPath(UTF8Char *sbuff, const UTF8Char *path);
	};
}
#endif
