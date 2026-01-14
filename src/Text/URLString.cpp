#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/TextBinEnc/URIEncoding.h"
#include "Text/URLString.h"

UnsafeArrayOpt<UTF8Char> Text::URLString::GetURLFilePath(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> url, UOSInt urlLen)
{
	if (!Text::StrStartsWithICaseC(url, urlLen, UTF8STRC("FILE:///")))
		return nullptr;
	if (IO::Path::PATH_SEPERATOR == '\\')
	{
		UnsafeArray<UTF8Char> sptr = Text::TextBinEnc::URIEncoding::URIDecode(sbuff, &url[8]);
		Text::StrReplace(sbuff, '/', '\\');
		return sptr;
	}
	else
	{
		return Text::TextBinEnc::URIEncoding::URIDecode(sbuff, &url[7]);
	}
}

UnsafeArray<UTF8Char> Text::URLString::GetURLDomain(UnsafeArray<UTF8Char> sbuff, Text::CStringNN url, OptOut<UInt16> port)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UInt16 defPort = 0;
	if (url.StartsWith(UTF8STRC("http://")))
	{
		url = url.Substring(7);
		defPort = 80;
	}
	else if (url.StartsWith(UTF8STRC("https://")))
	{
		url = url.Substring(8);
		defPort = 443;
	}
	else
	{
		i = url.IndexOf(UTF8STRC("://"));
		if (i != INVALID_INDEX)
		{
			url = url.Substring(i + 3);
		}
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
		if (port.IsNotNull())
		{
			MemCopyNO(sbuff.Ptr(), &url.v[j + 1], (i - j - 1) * sizeof(UTF8Char));
			sbuff[i - j - 1] = 0;
			Text::StrToUInt16S(sbuff, port.Or(defPort), 0);
		}
		if (i < j)
		{
			MemCopyNO(sbuff.Ptr(), url.v.Ptr(), sizeof(UTF8Char) * i);
			sbuff[i] = 0;
			return &sbuff[i];
		}
		else
		{
			MemCopyNO(sbuff.Ptr(), url.v.Ptr(), sizeof(UTF8Char) * j);
			sbuff[j] = 0;
			return &sbuff[j];
		}
	}
	else if (i != INVALID_INDEX)
	{
		port.Set(defPort);
		MemCopyNO(sbuff.Ptr(), url.v.Ptr(), sizeof(UTF8Char) * i);
		sbuff[i] = 0;
		return &sbuff[i];
	}
	else if (j != INVALID_INDEX)
	{
		if (port.IsNotNull())
		{
			Text::StrToUInt16S(&url.v[j + 1], port.Or(defPort), 0);
		}
		MemCopyNO(sbuff.Ptr(), url.v.Ptr(), sizeof(UTF8Char) * j);
		sbuff[j] = 0;
		return &sbuff[j];
	}
	else
	{
		port.Set(defPort);
		return url.ConcatTo(sbuff);
	}
}

UnsafeArrayOpt<UTF8Char> Text::URLString::GetURIScheme(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> url, UOSInt urlLen)
{
	UOSInt i = Text::StrIndexOfCharC(url, urlLen, ':');
	if (i == INVALID_INDEX)
	{
		return nullptr;
	}
	return Text::StrToUpperC(sbuff, url, i);
}

UnsafeArray<UTF8Char> Text::URLString::GetURLHost(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> url, UOSInt urlLen)
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
		MemCopyNO(sbuff.Ptr(), url.Ptr(), sizeof(UTF8Char) * i);
		sbuff[i] = 0;
		return &sbuff[i];
	}
	else
	{
		return Text::StrConcatC(sbuff, url, urlLen);
	}
}

UnsafeArray<UTF8Char> Text::URLString::GetURLPath(UnsafeArray<UTF8Char> sbuff, Text::CStringNN url)
{
	UOSInt i;
	UnsafeArray<UTF8Char> tmpBuff;
	i = url.IndexOf(UTF8STRC("://"));
	if (i != INVALID_INDEX)
	{
		url = url.Substring(i + 3);
	}
	i = url.IndexOf('/');
	if (i != INVALID_INDEX)
	{
		tmpBuff = MemAllocArr(UTF8Char, url.leng + 1);
		UnsafeArray<UTF8Char> sptr = Text::TextBinEnc::URIEncoding::URIDecode(tmpBuff, &url.v[i]);
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
		MemFreeArr(tmpBuff);
		return sptr;
	}
	else
	{
		return Text::StrConcatC(sbuff, UTF8STRC("/"));
	}
}

UnsafeArray<UTF8Char> Text::URLString::GetURLPathSvr(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> url, UOSInt urlLen)
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
			UnsafeArray<UTF8Char> sptr = Text::TextBinEnc::URIEncoding::URIDecode(sbuff, &url[i]);
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
			UnsafeArray<UTF8Char> tmpbuff = MemAllocArr(UTF8Char, urlLen - i + 1);
			UnsafeArray<UTF8Char> sptr = Text::TextBinEnc::URIEncoding::URIDecode(tmpbuff, &url[i]);
			i = Text::StrIndexOfCharC(tmpbuff, (UOSInt)(sptr - tmpbuff), '?');
			if (i != INVALID_INDEX)
			{
				sptr = &tmpbuff[i];
				*sptr = 0;
			}
			sptr = Text::StrConcatC(sbuff, tmpbuff, (UOSInt)(sptr - tmpbuff));
			MemFreeArr(tmpbuff);
			return sptr;
		}
	}
	else
	{
		return Text::StrConcatC(sbuff, UTF8STRC("/"));
	}
}

Text::CStringNN Text::URLString::GetURLPathQuery(Text::CStringNN url)
{
	UOSInt i = url.IndexOf(UTF8STRC("://"));
	if (i == INVALID_INDEX)
		return url;
	url = url.Substring(i + 3);
	i = url.IndexOf('/');
	if (i == INVALID_INDEX)
		return CSTR("");
	return url.Substring(i);
}

UnsafeArrayOpt<UTF8Char> Text::URLString::AppendURLPath(UnsafeArray<UTF8Char> sbuff, UnsafeArray<UTF8Char> sbuffEnd, Text::CStringNN path)
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
			UnsafeArray<UTF8Char> sptr = IO::Path::AppendPath(sbuff, sbuffEnd, path);
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
		return nullptr;
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
