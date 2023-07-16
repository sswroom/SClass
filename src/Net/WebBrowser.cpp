#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/CRC32.h"
#include "Crypto/Hash/CRC32R.h"
#include "Data/ByteBuffer.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "IO/StmData/MemoryDataCopy.h"
#include "Net/HTTPData.h"
#include "Net/MIME.h"
#include "Net/WebBrowser.h"
#include "Text/MyString.h"
#include "Text/URLString.h"
#include "Text/TextBinEnc/Base64Enc.h"
#include "Text/TextBinEnc/URIEncoding.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

UTF8Char *Net::WebBrowser::GetLocalFileName(UTF8Char *sbuff, const UTF8Char *url, UOSInt urlLen)
{
	UTF8Char buff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UInt8 hashResult[4];
	sptr = this->cacheDir->ConcatTo(buff);
	if (sptr != buff && sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr2 = Text::URLString::GetURIScheme(sptr, url, urlLen);
	if (Text::StrEqualsC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC("HTTP")) || Text::StrEqualsC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC("HTTPS")))
	{
		const UTF8Char *urlEnd = &url[urlLen];
		sptr = sptr2;
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr2 = Text::URLString::GetURLHost(sptr, url, urlLen);
		Text::StrReplace(sptr, ':', '+');
		IO::Path::CreateDirectory(CSTRP(buff, sptr2));
		sptr = sptr2;
		*sptr++ = IO::Path::PATH_SEPERATOR;
		url = &url[Text::StrIndexOfCharC(url, urlLen, ':') + 3];
		url = &url[Text::StrIndexOfCharC(url, (UOSInt)(urlEnd - url), '/')];
		this->hash.Clear();
		this->hash.Calc(url, (UOSInt)(urlEnd - url));
		this->hash.GetValue(hashResult);
		Text::StrHexBytes(sptr, hashResult, 4, 0);
		return Text::StrConcat(sbuff, buff);
	}
	else
	{
		return 0;
	}
}

Net::WebBrowser::WebBrowser(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString cacheDir) : queue(sockf, ssl)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->cacheDir = Text::String::New(cacheDir);
}

Net::WebBrowser::~WebBrowser()
{
	this->queue.Clear();
	this->cacheDir->Release();
}

IO::StreamData *Net::WebBrowser::GetData(Text::CString url, Bool forceReload, UTF8Char *contentType)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::Path::PathType pt = IO::Path::GetPathType(url);
	/////////////////////////////////////////////
	if (pt == IO::Path::PathType::File)
	{
		IO::StmData::FileData *fd;
		NEW_CLASS(fd, IO::StmData::FileData(url, false));
		if (contentType)
		{
			Text::CString mime = Net::MIME::GetMIMEFromFileName(url.v, url.leng);
			Text::StrConcatC(contentType, mime.v, mime.leng);
		}
		return fd;
	}
	if ((sptr = Text::URLString::GetURIScheme(sbuff, url.v, url.leng)) == 0)
		return 0;
	if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("FILE")))
	{
		sptr = Text::URLString::GetURLFilePath(sbuff, url.v, url.leng);
		IO::StmData::FileData *fd;
		NEW_CLASS(fd, IO::StmData::FileData(CSTRP(sbuff, sptr), false));
		if (contentType)
		{
			Text::CString mime = Net::MIME::GetMIMEFromFileName(url.v, url.leng);
			Text::StrConcatC(contentType, mime.v, mime.leng);
		}
		return fd;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("HTTP")))
	{
		Net::HTTPData *data;
		sptr = GetLocalFileName(sbuff, url.v, url.leng);
		NEW_CLASS(data, Net::HTTPData(this->sockf, this->ssl, &this->queue, url, CSTRP(sbuff, sptr), forceReload));
		return data;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("HTTPS")))
	{
		Net::HTTPData *data;
		sptr = GetLocalFileName(sbuff, url.v, url.leng);
#if defined(VERBOSE)
		printf("WebBrowser: Loading HTTPS: %s\r\n", url.v);
#endif
		NEW_CLASS(data, Net::HTTPData(this->sockf, this->ssl, &this->queue, url, CSTRP(sbuff, sptr), forceReload));
		return data;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("FTP")))
	{
/*		IO::Stream *stm = Net::URL::OpenStream(url, this->sockf);
		IO::StmData::StreamDataStream *data;
		if (stm == 0)
			return 0;
		NEW_CLASS(data, IO::StmData::*/
		return 0;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("DATA")))
	{
		IO::StmData::MemoryDataCopy *fd;
		const UTF8Char *urlPtr;
		WChar c;
		const UTF8Char *urlEnd = url.v + url.leng;
		urlPtr = &url.v[5];
		if (contentType)
		{
			while (true)
			{
				c = *urlPtr++;
				if (c == 0)
				{
					*contentType = 0;
					return 0;
				}
				else if (c == ';')
				{
					break;
				}
				else
				{
					*contentType++ = (UTF8Char)c;
				}
			}
		}
		else
		{
			while (true)
			{
				c = *urlPtr++;
				if (c == 0)
				{
					return 0;
				}
				else if (c == ';')
				{
					break;
				}
			}
		}
		if (Text::StrStartsWithC(urlPtr, (UOSInt)(urlEnd - urlPtr), UTF8STRC("base64,")))
		{
			Text::TextBinEnc::Base64Enc b64;
			UOSInt textSize;
			UOSInt binSize;
			UTF8Char *strTemp;
			UTF8Char *sptr;
			textSize = (UOSInt)(urlEnd - urlPtr + 7);
			strTemp = MemAlloc(UTF8Char, textSize + 1);
			sptr = Text::TextBinEnc::URIEncoding::URIDecode(strTemp, urlPtr + 7);
			binSize = b64.CalcBinSize(strTemp, (UOSInt)(sptr - strTemp));
			Data::ByteBuffer binTemp(binSize);
			b64.DecodeBin(strTemp, (UOSInt)(sptr - strTemp), binTemp.Ptr());
			NEW_CLASS(fd, IO::StmData::MemoryDataCopy(binTemp));
			MemFree(strTemp);
			return fd;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}
