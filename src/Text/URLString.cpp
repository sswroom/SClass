#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/TextBinEnc/URIEncoding.h"
#include "Text/URLString.h"

UTF8Char *Text::URLString::GetURLFilePath(UTF8Char *sbuff, const UTF8Char *url, UOSInt urlLen)
{
	if (!Text::StrStartsWithICaseC(url, urlLen, UTF8STRC("FILE:///")))
		return 0;
	if (IO::Path::PATH_SEPERATOR == '\\')
	{
		UTF8Char *sptr = Text::TextBinEnc::URIEncoding::URIDecode(sbuff, &url[8]);
		Text::StrReplace(sbuff, '/', '\\');
		return sptr;
	}
	else
	{
		return Text::TextBinEnc::URIEncoding::URIDecode(sbuff, &url[7]);
	}
}

UTF8Char *Text::URLString::GetURLDomain(UTF8Char *sbuff, Text::CString url, UInt16 *port)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	i = url.IndexOf(UTF8STRC("://"));
	if (i != INVALID_INDEX)
	{
		url = url.Substring(i + 3);
	}
	k = url.IndexOf('@');
	i = url.IndexOf('/');
	if (k != INVALID_INDEX && i != INVALID_INDEX && k < i)
	{
		url = url.Substring(k + 1);
		i -= k + 1;
	}
	j = url.IndexOf(':');
	if (i != INVALID_INDEX && j != INVALID_INDEX && j < i)
	{
		if (port)
		{
			MemCopyNO(sbuff, &url.v[j + 1], (i - j - 1) * sizeof(UTF8Char));
			sbuff[i - j - 1] = 0;
			Text::StrToUInt16S(sbuff, port, 0);
		}
		if (i < j)
		{
			MemCopyNO(sbuff, url.v, sizeof(UTF8Char) * i);
			sbuff[i] = 0;
			return &sbuff[i];
		}
		else
		{
			MemCopyNO(sbuff, url.v, sizeof(UTF8Char) * j);
			sbuff[j] = 0;
			return &sbuff[j];
		}
	}
	else if (i != INVALID_INDEX)
	{
		if (port)
		{
			*port = 0;
		}
		MemCopyNO(sbuff, url.v, sizeof(UTF8Char) * i);
		sbuff[i] = 0;
		return &sbuff[i];
	}
	else if (j != INVALID_INDEX)
	{
		if (port)
		{
			Text::StrToUInt16S(&url.v[j + 1], port, 0);
		}
		MemCopyNO(sbuff, url.v, sizeof(UTF8Char) * j);
		sbuff[j] = 0;
		return &sbuff[j];
	}
	else
	{
		if (port)
		{
			*port = 0;
		}
		return url.ConcatTo(sbuff);
	}
}

UTF8Char *Text::URLString::GetURIScheme(UTF8Char *sbuff, const UTF8Char *url, UOSInt urlLen)
{
	UOSInt i = Text::StrIndexOfCharC(url, urlLen, ':');
	if (i == INVALID_INDEX)
	{
		return 0;
	}
	return Text::StrToUpperC(sbuff, url, i);
}

UTF8Char *Text::URLString::GetURLHost(UTF8Char *sbuff, const UTF8Char *url, UOSInt urlLen)
{
	UOSInt i;
	i = Text::StrIndexOfC(url, urlLen, UTF8STRC("://"));
	if (i != INVALID_INDEX)
	{
		url = &url[i + 3];
		urlLen -= i + 3;
	}
	i = Text::StrIndexOfCharC(url, urlLen, '/');
	if (i != INVALID_INDEX)
	{
		MemCopyNO(sbuff, url, sizeof(UTF8Char) * i);
		sbuff[i] = 0;
		return &sbuff[i];
	}
	else
	{
		return Text::StrConcatC(sbuff, url, urlLen);
	}
}

UTF8Char *Text::URLString::GetURLPath(UTF8Char *sbuff, Text::CString url)
{
	UOSInt i;
	UTF8Char *tmpBuff;
	i = url.IndexOf(UTF8STRC("://"));
	if (i != INVALID_INDEX)
	{
		url = url.Substring(i + 3);
	}
	i = url.IndexOf('/');
	if (i != INVALID_INDEX)
	{
		tmpBuff = MemAlloc(UTF8Char, url.leng + 1);
		UTF8Char *sptr = Text::TextBinEnc::URIEncoding::URIDecode(tmpBuff, &url.v[i]);
		i = Text::StrIndexOfCharC(tmpBuff, (UOSInt)(sptr - tmpBuff), '?');
		if (i != INVALID_INDEX)
		{
			sptr = &tmpBuff[i];
			*sptr = 0;
		}
		i = Text::StrIndexOfCharC(tmpBuff, (UOSInt)(sptr - tmpBuff), '#');
		if (i != INVALID_INDEX)
		{
			sptr = &tmpBuff[i];
			*sptr = 0;
		}
		sptr = Text::StrConcatC(sbuff, tmpBuff, (UOSInt)(sptr - tmpBuff));
		MemFree(tmpBuff);
		return sptr;
	}
	else
	{
		return Text::StrConcatC(sbuff, UTF8STRC("/"));
	}
}

UTF8Char *Text::URLString::GetURLPathSvr(UTF8Char *sbuff, const UTF8Char *url, UOSInt urlLen)
{
	UOSInt i;
	i = Text::StrIndexOfC(url, urlLen, UTF8STRC("://"));
	if (i != INVALID_INDEX)
	{
		url = &url[i + 3];
		urlLen -= i + 3;
	}
	i = Text::StrIndexOfCharC(url, urlLen, '/');
	if (i != INVALID_INDEX)
	{
		if (urlLen < 256)
		{
			UTF8Char *sptr = Text::TextBinEnc::URIEncoding::URIDecode(sbuff, &url[i]);
			i = Text::StrIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '?');
			if (i != INVALID_INDEX)
			{
				sptr = &sbuff[i];
				*sptr = 0;
			}
			return sptr;
		}
		else
		{
			UTF8Char *tmpbuff = MemAlloc(UTF8Char, urlLen - i + 1);
			UTF8Char *sptr = Text::TextBinEnc::URIEncoding::URIDecode(tmpbuff, &url[i]);
			i = Text::StrIndexOfCharC(tmpbuff, (UOSInt)(sptr - tmpbuff), '?');
			if (i != INVALID_INDEX)
			{
				sptr = &tmpbuff[i];
				*sptr = 0;
			}
			sptr = Text::StrConcatC(sbuff, tmpbuff, (UOSInt)(sptr - tmpbuff));
			MemFree(tmpbuff);
			return sptr;
		}
	}
	else
	{
		return Text::StrConcatC(sbuff, UTF8STRC("/"));
	}
}

UTF8Char *Text::URLString::AppendURLPath(UTF8Char *sbuff, UTF8Char *sbuffEnd, Text::CString path)
{
	UOSInt i = path.IndexOf(UTF8STRC("://"));
	if (i != INVALID_INDEX)
	{
		return path.ConcatTo(sbuff);
	}
	if (&sbuff[0] != 0)
	{
		if (sbuff[1] == ':' && sbuff[2] == '\\')
		{
			UTF8Char *sptr = IO::Path::AppendPath(sbuff, sbuffEnd, path);
			Text::StrReplace(sbuff, '/', '\\');
			return sptr;
		}
		IO::Path::PathType pt = IO::Path::GetPathType(CSTRP(sbuff, sbuffEnd));
		if (pt != IO::Path::PathType::Unknown)
		{
			return IO::Path::AppendPath(sbuff, sbuffEnd, path);
		}
	}
	i = Text::StrIndexOfC(sbuff, (UOSInt)(sbuffEnd - sbuff), UTF8STRC("://"));
	if (i == INVALID_INDEX)
		return 0;
	sbuff = &sbuff[3];
	if (path.v[0] == '/')
	{
		i = Text::StrIndexOfCharC(sbuff, (UOSInt)(sbuffEnd - sbuff), '/');
		if (i == INVALID_INDEX)
		{
			return path.ConcatTo(sbuffEnd);
		}
		else
		{
			return path.ConcatTo(&sbuff[i]);
		}
	}
	else
	{
		while (path.v[0] == '.' && path.v[1] == '.' && path.v[2] == '/')
		{
			i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sbuffEnd - sbuff), '/');
			if (i != INVALID_INDEX)
			{
				sbuff[i] = 0;
				sbuffEnd = &sbuff[i];
			}
			path = path.Substring(3);
		}
		i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sbuffEnd - sbuff), '/');
		if (i == INVALID_INDEX)
		{
			return path.ConcatTo(sbuffEnd);
		}
		else
		{
			return path.ConcatTo(&sbuff[i + 1]);
		}
	}
	////////////////////////////////
}
