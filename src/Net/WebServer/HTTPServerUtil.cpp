#include "Stdafx.h"
#include "Crypto/Hash/CRC32RIEEE.h"
#include "Data/ByteTool.h"
#include "Data/Compress/DeflateStream.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "Net/MIME.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Text/StringBuilderUTF8.h"

#define BUFFSIZE 2048

Bool Net::WebServer::HTTPServerUtil::MIMEToCompress(const UTF8Char *umime)
{
	if (Text::StrStartsWith(umime, (const UTF8Char*)"application/"))
	{
		if (Text::StrEquals(&umime[12], (const UTF8Char*)"javascript"))
		{
			return true;
		}
		else if (Text::StrEquals(&umime[12], (const UTF8Char*)"json"))
		{
			return true;
		}
		else if (Text::StrEquals(&umime[12], (const UTF8Char*)"xml"))
		{
			return true;
		}
		else if (Text::StrEquals(&umime[12], (const UTF8Char*)"xhtml+xml"))
		{
			return true;
		}
	}
	else if (Text::StrStartsWith(umime, (const UTF8Char*)"text/"))
	{
		if (Text::StrEquals(&umime[5], (const UTF8Char*)"javascript"))
		{
			return true;
		}
		else if (Text::StrEquals(&umime[5], (const UTF8Char*)"html"))
		{
			return true;
		}
		else if (Text::StrEquals(&umime[5], (const UTF8Char*)"css"))
		{
			return true;
		}
		else if (Text::StrEquals(&umime[5], (const UTF8Char*)"plain"))
		{
			return true;
		}
		else if (Text::StrEquals(&umime[5], (const UTF8Char*)"xml"))
		{
			return true;
		}
	}
	return false;
}

Bool Net::WebServer::HTTPServerUtil::SendContent(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *mime, UInt64 contLeng, IO::Stream *fs)
{
	UOSInt i;
	UOSInt j;
	UInt8 compBuff[BUFFSIZE];
	Bool contSent = false;
	Bool succ = true;
	if (contLeng > 1024)
	{
		Bool needComp = MIMEToCompress(mime);
		UTF8Char *sarr[10];
		Text::StringBuilderUTF8 sb;

		if (needComp && req->GetHeader(&sb, (const UTF8Char*)"Accept-Encoding"))
		{
			Net::BrowserInfo::BrowserType browser = req->GetBrowser();
			Manage::OSInfo::OSType os = req->GetOS();
			j = Text::StrSplitTrim(sarr, 10, sb.ToString(), ',');
			i = 0;
			while (i < j)
			{
				if (Text::StrEqualsICase(sarr[i], (const UTF8Char*)"gzip"))
				{
					if (browser != Net::BrowserInfo::BT_IE && browser != Net::BrowserInfo::BT_SAFARI && os != Manage::OSInfo::OT_IPHONE && os != Manage::OSInfo::OT_IPAD)
					{
						resp->AddHeaderC(UTF8STRC("Content-Encoding"), UTF8STRC("gzip"));
						resp->AddHeaderC(UTF8STRC("Transfer-Encoding"), UTF8STRC("chunked"));

						compBuff[0] = 0x1F;
						compBuff[1] = 0x8B;
						compBuff[2] = 8;
						compBuff[3] = 0;
						compBuff[4] = 0;
						compBuff[5] = 0;
						compBuff[6] = 0;
						compBuff[7] = 0;
						compBuff[8] = 0;
						compBuff[9] = 0;
						succ = (resp->Write(compBuff, 8) == 8);

						Crypto::Hash::CRC32RIEEE crc;
						Data::Compress::DeflateStream dstm(fs, contLeng, &crc, Data::Compress::DeflateStream::CompLevel::MaxSpeed, true);
						UOSInt readSize;
						while ((readSize = dstm.Read(compBuff, BUFFSIZE)) != 0)
						{
							succ = succ && (resp->Write(compBuff, readSize) == readSize);
						}

						WriteUInt32(&compBuff[0], crc.GetValueU32());
						WriteInt32(&compBuff[4], (Int32)contLeng);
						succ = succ && (resp->Write(compBuff, 8) == 8);

						contSent = true;
						break;
					}
				}
				else if (Text::StrEqualsICase(sarr[i], (const UTF8Char*)"deflate"))
				{
					if (browser != Net::BrowserInfo::BT_IE)
					{
						resp->AddHeaderC(UTF8STRC("Content-Encoding"), UTF8STRC("deflate"));
						resp->AddHeaderC(UTF8STRC("Transfer-Encoding"), UTF8STRC("chunked"));

						Data::Compress::DeflateStream dstm(fs, contLeng, 0, Data::Compress::DeflateStream::CompLevel::MaxSpeed, true);
						UOSInt readSize;
						while ((readSize = dstm.Read(compBuff, BUFFSIZE)) != 0)
						{
							succ = succ && (resp->Write(compBuff, readSize) == readSize);
						}

						contSent = true;
						break;
					}
				}
				i++;
			}
		}
	}
	if (!contSent)
	{
		resp->AddContentLength(contLeng);
		while (contLeng > 0)
		{
			UOSInt writeSize;
			UOSInt readSize = fs->Read(compBuff, BUFFSIZE);
			if (readSize <= 0)
			{
				break;
			}
			writeSize = resp->Write(compBuff, readSize);
			if (writeSize != readSize)
			{
				succ = false;
				break;
			}
			contLeng -= readSize;
		}
		if (contLeng != 0)
		{
			succ = false;
		}
	}
	return succ;
}

Bool Net::WebServer::HTTPServerUtil::SendContent(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *mime, UInt64 contLeng, const UInt8 *buff)
{
	UOSInt i;
	UOSInt j;
	UInt8 compBuff[BUFFSIZE];
	Bool succ = true;
	Bool contSent = false;
	if (contLeng > 1024)
	{
		Bool needComp = MIMEToCompress(mime);
		UTF8Char *sarr[10];
		Text::StringBuilderUTF8 sb;

		if (needComp && req->GetHeader(&sb, (const UTF8Char*)"Accept-Encoding"))
		{
			Net::BrowserInfo::BrowserType browser = req->GetBrowser();
			j = Text::StrSplitTrim(sarr, 10, sb.ToString(), ',');
			i = 0;
			while (i < j)
			{
				if (Text::StrEqualsICase(sarr[i], (const UTF8Char*)"gzip"))
				{
					resp->AddHeaderC(UTF8STRC("Content-Encoding"), UTF8STRC("gzip"));
					resp->AddHeaderC(UTF8STRC("Transfer-Encoding"), UTF8STRC("chunked"));

					compBuff[0] = 0x1F;
					compBuff[1] = 0x8B;
					compBuff[2] = 8;
					compBuff[3] = 0;
					compBuff[4] = 0;
					compBuff[5] = 0;
					compBuff[6] = 0;
					compBuff[7] = 0;
					compBuff[8] = 0;
					succ = (resp->Write(compBuff, 8) == 8);

					Crypto::Hash::CRC32RIEEE crc;
					crc.Calc(buff, (UOSInt)contLeng);

					IO::MemoryStream mstm((UInt8*)buff, (UOSInt)contLeng, UTF8STRC("Net.HTTPServerUtil.SendContent"));
					Data::Compress::DeflateStream dstm(&mstm, contLeng, 0, Data::Compress::DeflateStream::CompLevel::MaxSpeed, true);
					UOSInt readSize;
					while ((readSize = dstm.Read(compBuff, BUFFSIZE)) != 0)
					{
						succ = succ && (resp->Write(compBuff, readSize) == readSize);
					}
					WriteUInt32(&compBuff[0], crc.GetValueU32());
					WriteInt32(&compBuff[4], (Int32)contLeng);
					succ = succ && (resp->Write(compBuff, 8) == 8);
					contSent = true;
					break;
				}
				else if (Text::StrEqualsICase(sarr[i], (const UTF8Char*)"deflate"))
				{
					if (browser != Net::BrowserInfo::BT_IE)
					{
						resp->AddHeaderC(UTF8STRC("Content-Encoding"), UTF8STRC("deflate"));
						resp->AddHeaderC(UTF8STRC("Transfer-Encoding"), UTF8STRC("chunked"));

						IO::MemoryStream mstm((UInt8*)buff, (UOSInt)contLeng, UTF8STRC("Net.HTTPServerUtil.SendContent"));
						Data::Compress::DeflateStream dstm(&mstm, contLeng, 0, Data::Compress::DeflateStream::CompLevel::MaxSpeed, true);
						UOSInt readSize;
						while ((readSize = dstm.Read(compBuff, BUFFSIZE)) != 0)
						{
							succ = succ && (resp->Write(compBuff, readSize) == readSize);
						}
						contSent = true;
						break;
					}
				}
				i++;
			}
		}
	}
	if (!contSent)
	{
		resp->AddContentLength(contLeng);
		succ = (resp->Write(buff, (UOSInt)contLeng) == contLeng);
	}
	return succ;
}

Bool Net::WebServer::HTTPServerUtil::ResponseFile(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *fileName, OSInt cacheAge)
{
	Text::StringBuilderUTF8 sb2;
	Data::DateTime t;
	IO::FileStream *fs;
	const UTF8Char *mime;
	UInt64 sizeLeft;
	UInt8 sbuff[32];
	UTF8Char *sptr;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
	fs->GetFileTimes(0, 0, &t);

	if (req->GetHeader(&sb2, (const UTF8Char*)"If-Modified-Since"))
	{
		Data::DateTime t2;
		t2.SetValue(sb2.ToString());
		t2.AddMS(t.GetMS());
		if (t2.DiffMS(&t) == 0)
		{
			resp->SetStatusCode(Net::WebStatus::SC_NOT_MODIFIED);
			resp->AddDefHeaders(req);
			resp->AddCacheControl(cacheAge);
			resp->AddContentLength(0);
			resp->Write(sbuff, 0);
			DEL_CLASS(fs);
			return true;
		}
	}

	IO::Path::GetFileExt(sbuff, fileName);

	sizeLeft = fs->GetLength();
	sb2.ClearStr();
	if (req->GetHeader(&sb2, (const UTF8Char*)"Range"))
	{
		UInt64 fileSize = sizeLeft;
		if (!sb2.StartsWith((const UTF8Char*)"bytes="))
		{
			resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
			resp->AddDefHeaders(req);
			resp->AddCacheControl(cacheAge);
			resp->AddContentLength(0);
			resp->Write(sbuff, 0);
			DEL_CLASS(fs);
			return true;
		}
		if (sb2.IndexOf(',') != INVALID_INDEX)
		{
			resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
			resp->AddDefHeaders(req);
			resp->AddCacheControl(cacheAge);
			resp->AddContentLength(0);
			resp->Write(sbuff, 0);
			DEL_CLASS(fs);
			return true;
		}
		UInt64 start = 0;
		Int64 end = -1;
		UOSInt i = sb2.IndexOf('-');
		if (i == INVALID_INDEX)
		{
			resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
			resp->AddDefHeaders(req);
			resp->AddCacheControl(cacheAge);
			resp->AddContentLength(0);
			resp->Write(sbuff, 0);
			DEL_CLASS(fs);
			return true;
		}
		sptr = sb2.ToString();
		sptr[i] = 0;
		if (!Text::StrToUInt64(&sptr[6], &start))
		{
			resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
			resp->AddDefHeaders(req);
			resp->AddCacheControl(cacheAge);
			resp->AddContentLength(0);
			resp->Write(sbuff, 0);
			DEL_CLASS(fs);
			return true;
		}
		if (i + 1 < sb2.GetLength())
		{
			if (!Text::StrToInt64(&sptr[i + 1], &end))
			{
				resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
				resp->AddDefHeaders(req);
				resp->AddCacheControl(cacheAge);
				resp->AddContentLength(0);
				resp->Write(sbuff, 0);
				DEL_CLASS(fs);
				return true;
			}
			if (end <= (Int64)start || end > (Int64)sizeLeft)
			{
				resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
				resp->AddDefHeaders(req);
				resp->AddCacheControl(cacheAge);
				resp->AddContentLength(0);
				resp->Write(sbuff, 0);
				DEL_CLASS(fs);
				return true;
			}
			sizeLeft = (UInt64)end - start;
		}
		else
		{
			sizeLeft = sizeLeft - start;
		}
		fs->SeekFromBeginning(start);
		resp->SetStatusCode(Net::WebStatus::SC_PARTIAL_CONTENT);
		UTF8Char u8buff[128];
		UTF8Char *u8ptr;
		u8ptr = Text::StrConcatC(u8buff, UTF8STRC("bytes "));
		u8ptr = Text::StrUInt64(u8ptr, start);
		*u8ptr++ = '-';
		u8ptr = Text::StrUInt64(u8ptr, start + sizeLeft - 1);
		*u8ptr++ = '/';
		u8ptr = Text::StrUInt64(u8ptr, fileSize);
		resp->AddHeaderC(UTF8STRC("Content-Range"), u8buff, (UOSInt)(u8ptr - u8buff));
	}
	resp->AddDefHeaders(req);
	resp->AddCacheControl(cacheAge);
	resp->AddLastModified(&t);
	mime = Net::MIME::GetMIMEFromExt(sbuff);
	resp->AddContentType(mime, Text::StrCharCnt(mime));
	resp->AddHeaderC(UTF8STRC("Accept-Ranges"), UTF8STRC("bytes"));
	if (sizeLeft <= 0)
	{
		UInt8 buff[BUFFSIZE];
		UOSInt readSize;
		readSize = fs->Read(buff, BUFFSIZE);
		if (readSize == 0)
		{
			Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, sizeLeft, fs);
		}
		else
		{
			IO::MemoryStream mstm(UTF8STRC("Net.WebServer.HTTPUtil.ResponseFile.mstm"));
			while (readSize > 0)
			{
				sizeLeft += mstm.Write(buff, readSize);
				readSize = fs->Read(buff, BUFFSIZE);
			}
			mstm.SeekFromBeginning(0);
			Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, sizeLeft, &mstm);
		}
	}
	else
	{
		Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, sizeLeft, fs);
	}
	DEL_CLASS(fs);
	return true;
}
