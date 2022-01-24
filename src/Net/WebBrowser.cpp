#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/CRC32.h"
#include "Crypto/Hash/CRC32R.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "IO/StmData/MemoryData2.h"
#include "Net/HTTPData.h"
#include "Net/MIME.h"
#include "Net/WebBrowser.h"
#include "Text/MyString.h"
#include "Text/URLString.h"
#include "Text/TextBinEnc/Base64Enc.h"
#include "Text/TextBinEnc/URIEncoding.h"

UTF8Char *Net::WebBrowser::GetLocalFileName(UTF8Char *sbuff, const UTF8Char *url, UOSInt urlLen)
{
	UTF8Char buff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UInt8 hashResult[4];
	sptr = Text::StrConcat(buff, this->cacheDir);
	if (sptr != buff && sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr2 = Text::URLString::GetURIScheme(sptr, url, urlLen);
	if (Text::StrEqualsICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC("HTTP")) || Text::StrEqualsICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC("HTTPS")))
	{
		const UTF8Char *urlEnd = &url[urlLen];
		sptr = sptr2;
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr2 = Text::URLString::GetURLHost(sptr, url, urlLen);
		Text::StrReplace(sptr, ':', '+');
		IO::Path::CreateDirectory(buff);
		sptr = sptr2;
		*sptr++ = IO::Path::PATH_SEPERATOR;
		url = &url[Text::StrIndexOfCharC(url, urlLen, ':') + 3];
		url = &url[Text::StrIndexOfCharC(url, (UOSInt)(urlEnd - url), '/')];
		this->hash->Clear();
		this->hash->Calc(url, (UOSInt)(urlEnd - url));
		this->hash->GetValue(hashResult);
		Text::StrHexBytes(sptr, hashResult, 4, 0);
		return Text::StrConcat(sbuff, buff);
	}
	else
	{
		return 0;
	}
}

Net::WebBrowser::WebBrowser(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *cacheDir)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->cacheDir = Text::StrCopyNew(cacheDir);
	NEW_CLASS(this->hash, Crypto::Hash::CRC32R(Crypto::Hash::CRC32::GetPolynormialIEEE()));
	NEW_CLASS(this->queue, Net::HTTPQueue(sockf, ssl));
}

Net::WebBrowser::~WebBrowser()
{
	DEL_CLASS(this->queue);
	Text::StrDelNew(this->cacheDir);
	DEL_CLASS(this->hash);
}

IO::IStreamData *Net::WebBrowser::GetData(const UTF8Char *url, UOSInt urlLen, Bool forceReload, UTF8Char *contentType)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::Path::PathType pt = IO::Path::GetPathType(url, urlLen);
	/////////////////////////////////////////////
	if (pt == IO::Path::PathType::File)
	{
		IO::StmData::FileData *fd;
		NEW_CLASS(fd, IO::StmData::FileData(url, false));
		if (contentType)
		{
			Text::CString mime = Net::MIME::GetMIMEFromFileName(url, urlLen);
			Text::StrConcatC(contentType, mime.v, mime.leng);
		}
		return fd;
	}
	if ((sptr = Text::URLString::GetURIScheme(sbuff, url, urlLen)) == 0)
		return 0;
	if (Text::StrEqualsICaseC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("FILE")))
	{
		Text::URLString::GetURLFilePath(sbuff, url, urlLen);
		IO::StmData::FileData *fd;
		NEW_CLASS(fd, IO::StmData::FileData(sbuff, false));
		if (contentType)
		{
			Text::CString mime = Net::MIME::GetMIMEFromFileName(url, urlLen);
			Text::StrConcatC(contentType, mime.v, mime.leng);
		}
		return fd;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("HTTP")))
	{
		Net::HTTPData *data;
		GetLocalFileName(sbuff, url, urlLen);
		NEW_CLASS(data, Net::HTTPData(this->sockf, this->ssl, this->queue, url, sbuff, forceReload));
		return data;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("HTTPS")))
	{
		Net::HTTPData *data;
		GetLocalFileName(sbuff, url, urlLen);
		NEW_CLASS(data, Net::HTTPData(this->sockf, this->ssl, this->queue, url, sbuff, forceReload));
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
		IO::StmData::MemoryData2 *fd;
		WChar c;
		const UTF8Char *urlEnd = &url[urlLen];
		url = &url[5];
		if (contentType)
		{
			while (true)
			{
				c = *url++;
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
				c = *url++;
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
		if (Text::StrStartsWithC(url, (UOSInt)(urlEnd - url), UTF8STRC("base64,")))
		{
			Text::TextBinEnc::Base64Enc b64;
			UOSInt textSize;
			UOSInt binSize;
			UTF8Char *strTemp;
			UTF8Char *sptr;
			UInt8 *binTemp;
			textSize = (UOSInt)(urlEnd - url + 7);
			strTemp = MemAlloc(UTF8Char, textSize + 1);
			sptr = Text::TextBinEnc::URIEncoding::URIDecode(strTemp, url + 7);
			binSize = b64.CalcBinSize(strTemp, (UOSInt)(sptr - strTemp));
			binTemp = MemAlloc(UInt8, binSize);
			b64.DecodeBin(strTemp, (UOSInt)(sptr - strTemp), binTemp);
			NEW_CLASS(fd, IO::StmData::MemoryData2(binTemp, binSize));
			MemFree(binTemp);
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
