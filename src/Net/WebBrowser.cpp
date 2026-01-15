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

UnsafeArrayOpt<UTF8Char> Net::WebBrowser::GetLocalFileName(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> url, UIntOS urlLen)
{
	UTF8Char buff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	UInt8 hashResult[4];
	sptr = this->cacheDir->ConcatTo(buff);
	if (sptr != buff && sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	if (!Text::URLString::GetURIScheme(sptr, url, urlLen).SetTo(sptr2))
		return nullptr;
	if (Text::StrEqualsC(sptr, (UIntOS)(sptr2 - sptr), UTF8STRC("HTTP")) || Text::StrEqualsC(sptr, (UIntOS)(sptr2 - sptr), UTF8STRC("HTTPS")))
	{
		UnsafeArray<const UTF8Char> urlEnd = &url[urlLen];
		sptr = sptr2;
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr2 = Text::URLString::GetURLHost(sptr, url, urlLen);
		Text::StrReplace(sptr, ':', '+');
		IO::Path::CreateDirectory(CSTRP(buff, sptr2));
		sptr = sptr2;
		*sptr++ = IO::Path::PATH_SEPERATOR;
		url = &url[Text::StrIndexOfCharC(url, urlLen, ':') + 3];
		url = &url[Text::StrIndexOfCharC(url, (UIntOS)(urlEnd - url), '/')];
		this->hash.Clear();
		this->hash.Calc(url.Ptr(), (UIntOS)(urlEnd - url));
		this->hash.GetValue(hashResult);
		Text::StrHexBytes(sptr, hashResult, 4, 0);
		return Text::StrConcat(sbuff, buff);
	}
	else
	{
		return nullptr;
	}
}

Net::WebBrowser::WebBrowser(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN cacheDir) : queue(clif, ssl)
{
	this->clif = clif;
	this->ssl = ssl;
	this->cacheDir = Text::String::New(cacheDir);
}

Net::WebBrowser::~WebBrowser()
{
	this->queue.Clear();
	this->cacheDir->Release();
}

Optional<IO::StreamData> Net::WebBrowser::GetData(Text::CStringNN url, Bool forceReload, UnsafeArrayOpt<UTF8Char> contentType)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> nncontentType;
	IO::Path::PathType pt = IO::Path::GetPathType(url);
	/////////////////////////////////////////////
	if (pt == IO::Path::PathType::File)
	{
		IO::StmData::FileData *fd;
		NEW_CLASS(fd, IO::StmData::FileData(url, false));
		if (contentType.SetTo(nncontentType))
		{
			Text::CStringNN mime = Net::MIME::GetMIMEFromFileName(url.v, url.leng);
			Text::StrConcatC(nncontentType, mime.v, mime.leng);
		}
		return fd;
	}
	if (!Text::URLString::GetURIScheme(sbuff, url.v, url.leng).SetTo(sptr))
		return nullptr;
	if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("FILE")))
	{
		if (Text::URLString::GetURLFilePath(sbuff, url.v, url.leng).SetTo(sptr))
			return nullptr;
		IO::StmData::FileData *fd;
		NEW_CLASS(fd, IO::StmData::FileData(CSTRP(sbuff, sptr), false));
		if (contentType.SetTo(nncontentType))
		{
			Text::CStringNN mime = Net::MIME::GetMIMEFromFileName(url.v, url.leng);
			Text::StrConcatC(nncontentType, mime.v, mime.leng);
		}
		return fd;
	}
	else if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("HTTP")))
	{
		if (!GetLocalFileName(sbuff, url.v, url.leng).SetTo(sptr))
			return nullptr;
		Net::HTTPData *data;
		NEW_CLASS(data, Net::HTTPData(this->clif, this->ssl, &this->queue, url, CSTRP(sbuff, sptr), forceReload));
		return data;
	}
	else if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("HTTPS")))
	{
		if (!GetLocalFileName(sbuff, url.v, url.leng).SetTo(sptr))
			return nullptr;
		Net::HTTPData *data;
#if defined(VERBOSE)
		printf("WebBrowser: Loading HTTPS: %s\r\n", url.v);
#endif
		NEW_CLASS(data, Net::HTTPData(this->clif, this->ssl, &this->queue, url, CSTRP(sbuff, sptr), forceReload));
		return data;
	}
	else if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("FTP")))
	{
/*		IO::Stream *stm = Net::URL::OpenStream(url, this->sockf);
		IO::StmData::StreamDataStream *data;
		if (stm == 0)
			return nullptr;
		NEW_CLASS(data, IO::StmData::*/
		return nullptr;
	}
	else if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("DATA")))
	{
		IO::StmData::MemoryDataCopy *fd;
		UnsafeArray<const UTF8Char> urlPtr;
		WChar c;
		UnsafeArray<const UTF8Char> urlEnd = url.v + url.leng;
		urlPtr = &url.v[5];
		if (contentType.SetTo(nncontentType))
		{
			while (true)
			{
				c = *urlPtr++;
				if (c == 0)
				{
					*nncontentType = 0;
					return nullptr;
				}
				else if (c == ';')
				{
					break;
				}
				else
				{
					*nncontentType++ = (UTF8Char)c;
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
					return nullptr;
				}
				else if (c == ';')
				{
					break;
				}
			}
		}
		if (Text::StrStartsWithC(urlPtr, (UIntOS)(urlEnd - urlPtr), UTF8STRC("base64,")))
		{
			Text::TextBinEnc::Base64Enc b64;
			UIntOS textSize;
			UIntOS binSize;
			UnsafeArray<UTF8Char> strTemp;
			UnsafeArray<UTF8Char> sptr;
			textSize = (UIntOS)(urlEnd - urlPtr + 7);
			strTemp = MemAllocArr(UTF8Char, textSize + 1);
			sptr = Text::TextBinEnc::URIEncoding::URIDecode(strTemp, urlPtr + 7);
			binSize = b64.CalcBinSize(CSTRP(strTemp, sptr));
			Data::ByteBuffer binTemp(binSize);
			b64.DecodeBin(CSTRP(strTemp, sptr), binTemp.Arr().Ptr());
			NEW_CLASS(fd, IO::StmData::MemoryDataCopy(binTemp));
			MemFreeArr(strTemp);
			return fd;
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
}
