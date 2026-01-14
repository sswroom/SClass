#include "Stdafx.h"
#include "Crypto/Hash/CRC32RIEEE.h"
#include "Core/ByteTool_C.h"
#include "IO/FileStream.h"
#include "IO/MemoryReadingStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "Net/MIME.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Text/StringBuilderUTF8.h"

#define BUFFSIZE 8192

Data::Compress::Deflater::CompLevel Net::WebServer::HTTPServerUtil::compLevel = Data::Compress::Deflater::CompLevel::BestSpeed;

Bool Net::WebServer::HTTPServerUtil::MIMEToCompress(Text::CStringNN umime)
{
	if (umime.StartsWith(UTF8STRC("application/")))
	{
		Text::CStringNN mine2 = umime.Substring(12);
		if (mine2.Equals(UTF8STRC("javascript")))
		{
			return true;
		}
		else if (mine2.Equals(UTF8STRC("json")))
		{
			return true;
		}
		else if (mine2.Equals(UTF8STRC("xml")))
		{
			return true;
		}
		else if (mine2.Equals(UTF8STRC("octet-stream")))
		{
			return true;
		}
		else if (mine2.Equals(UTF8STRC("xhtml+xml")))
		{
			return true;
		}
		else if (mine2.Equals(UTF8STRC("pdf")))
		{
			return true;
		}
		else if (mine2.Equals(UTF8STRC("vnd.ms-excel")))
		{
			return true;
		}
		else if (mine2.Equals(UTF8STRC("samlmetadata+xml")))
		{
			return true;
		}
		else if (mine2.Equals(UTF8STRC("gpx+xml")))
		{
			return true;
		}
	}
	else if (umime.StartsWith(UTF8STRC("text/")))
	{
		Text::CStringNN mine2 = umime.Substring(5);
		if (mine2.Equals(UTF8STRC("javascript")))
		{
			return true;
		}
		else if (mine2.Equals(UTF8STRC("html")))
		{
			return true;
		}
		else if (mine2.Equals(UTF8STRC("css")))
		{
			return true;
		}
		else if (mine2.Equals(UTF8STRC("plain")))
		{
			return true;
		}
		else if (mine2.Equals(UTF8STRC("xml")))
		{
			return true;
		}
	}
	return false;
}

Bool Net::WebServer::HTTPServerUtil::SendContent(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN mime, UInt64 contLeng, NN<IO::Stream> fs)
{
	UOSInt i;
	UOSInt j;
	UInt8 compBuff[BUFFSIZE];
	Bool contSent = false;
	Bool succ = true;
	if (contLeng > 1024)
	{
		Bool needComp = MIMEToCompress(mime);
		Text::PString sarr[10];
		Text::StringBuilderUTF8 sb;

		if (needComp && req->GetHeaderC(sb, CSTR("Accept-Encoding")))
		{
			Net::BrowserInfo::BrowserType browser = req->GetBrowser();
			Manage::OSInfo::OSType os = req->GetOS();
			j = Text::StrSplitTrimP(sarr, 10, sb, ',');
			i = 0;
			while (i < j)
			{
				if (Text::StrEqualsICaseC(sarr[i].v, sarr[i].leng, UTF8STRC("gzip")))
				{
					if (AllowGZip(browser, os))
					{
						resp->AddHeader(CSTR("Content-Encoding"), CSTR("gzip"));
						resp->AddHeader(CSTR("Transfer-Encoding"), CSTR("chunked"));
						resp->EnableWriteBuffer();

						compBuff[0] = 0x1F;
						compBuff[1] = 0x8B;
						compBuff[2] = 8;
						compBuff[3] = 0;
						compBuff[4] = 0;
						compBuff[5] = 0;
						compBuff[6] = 0;
						compBuff[7] = 0;
						compBuff[8] = 0;
						compBuff[9] = 0xff;
						succ = (resp->Write(Data::ByteArrayR(compBuff, 10)) == 10);

						Crypto::Hash::CRC32RIEEE crc;
						Data::Compress::Deflater dstm(fs, contLeng, crc, compLevel, false);
						UOSInt readSize;
						while ((readSize = dstm.Read(BYTEARR(compBuff))) != 0)
						{
							succ = succ && (resp->Write(Data::ByteArrayR(compBuff, readSize)) == readSize);
						}

						WriteUInt32(&compBuff[0], crc.GetValueU32());
						WriteInt32(&compBuff[4], (Int32)contLeng);
						succ = succ && (resp->Write(Data::ByteArrayR(compBuff, 8)) == 8);

						contSent = true;
						break;
					}
				}
				else if (Text::StrEqualsICaseC(sarr[i].v, sarr[i].leng, UTF8STRC("deflate")))
				{
					if (browser != Net::BrowserInfo::BT_IE)
					{
						resp->AddHeader(CSTR("Content-Encoding"), CSTR("deflate"));
						resp->AddHeader(CSTR("Transfer-Encoding"), CSTR("chunked"));
						resp->EnableWriteBuffer();

						Data::Compress::Deflater dstm(fs, contLeng, nullptr, compLevel, true);
						UOSInt readSize;
						while ((readSize = dstm.Read(BYTEARR(compBuff))) != 0)
						{
							succ = succ && (resp->Write(Data::ByteArrayR(compBuff, readSize)) == readSize);
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
			UOSInt readSize = fs->Read(BYTEARR(compBuff));
			if (readSize <= 0)
			{
				break;
			}
			writeSize = resp->Write(Data::ByteArrayR(compBuff, readSize));
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

Bool Net::WebServer::HTTPServerUtil::SendContent(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN mime, UInt64 contLeng, UnsafeArray<const UInt8> buff)
{
	UOSInt i;
	UOSInt j;
	UInt8 compBuff[BUFFSIZE];
	Bool succ = true;
	Bool contSent = false;
	if (contLeng > 1024)
	{
		Bool needComp = MIMEToCompress(mime);
		Text::PString sarr[10];
		Text::StringBuilderUTF8 sb;

		if (needComp && req->GetHeaderC(sb, CSTR("Accept-Encoding")))
		{
			Net::BrowserInfo::BrowserType browser = req->GetBrowser();
			j = Text::StrSplitTrimP(sarr, 10, sb, ',');
			i = 0;
			while (i < j)
			{
				if (Text::StrEqualsICaseC(sarr[i].v, sarr[i].leng, UTF8STRC("gzip")))
				{
					resp->AddHeader(CSTR("Content-Encoding"), CSTR("gzip"));
					resp->AddHeader(CSTR("Transfer-Encoding"), CSTR("chunked"));

					compBuff[0] = 0x1F;
					compBuff[1] = 0x8B;
					compBuff[2] = 8;
					compBuff[3] = 0;
					compBuff[4] = 0;
					compBuff[5] = 0;
					compBuff[6] = 0;
					compBuff[7] = 0;
					compBuff[8] = 4;
					compBuff[9] = 11; //0xff;
					succ = (resp->Write(Data::ByteArrayR(compBuff, 10)) == 10);

					Crypto::Hash::CRC32RIEEE crc;
					crc.Calc(buff.Ptr(), (UOSInt)contLeng);

					IO::MemoryReadingStream mstm(buff, (UOSInt)contLeng);
					Data::Compress::Deflater dstm(mstm, nullptr, compLevel, false);
					UOSInt readSize;
					while ((readSize = dstm.Read(BYTEARR(compBuff))) != 0)
					{
						succ = succ && (resp->Write(Data::ByteArrayR(compBuff, readSize)) == readSize);
					}
					WriteUInt32(&compBuff[0], crc.GetValueU32());
					WriteInt32(&compBuff[4], (Int32)contLeng);
					succ = succ && (resp->Write(Data::ByteArrayR(compBuff, 8)) == 8);
					contSent = true;
					break;
				}
				else if (Text::StrEqualsICaseC(sarr[i].v, sarr[i].leng, UTF8STRC("deflate")))
				{
					if (browser != Net::BrowserInfo::BT_IE)
					{
						resp->AddHeader(CSTR("Content-Encoding"), CSTR("deflate"));
						resp->AddHeader(CSTR("Transfer-Encoding"), CSTR("chunked"));

						IO::MemoryReadingStream mstm(buff, (UOSInt)contLeng);
						Data::Compress::Deflater dstm(mstm, nullptr, compLevel, true);
						UOSInt readSize;
						while ((readSize = dstm.Read(BYTEARR(compBuff))) != 0)
						{
							succ = succ && (resp->Write(Data::ByteArrayR(compBuff, readSize)) == readSize);
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
		succ = (resp->Write(Data::ByteArrayR(buff, (UOSInt)contLeng)) == contLeng);
	}
	return succ;
}

Bool Net::WebServer::HTTPServerUtil::SendContent(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN mime, Text::CStringNN cont)
{
	return SendContent(req, resp, mime, cont.leng, cont.v);
}

Bool Net::WebServer::HTTPServerUtil::ResponseFile(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN fileName, OSInt cacheAge)
{
	Text::StringBuilderUTF8 sb2;
	Data::DateTime t;
	Text::CStringNN mime;
	UInt64 sizeLeft;
	UInt8 sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
	fs.GetFileTimes(nullptr, nullptr, t);

	if (req->GetHeaderC(sb2, CSTR("If-Modified-Since")))
	{
		Data::DateTime t2;
		t2.SetValue(sb2.ToCString());
		t2.AddMS(t.GetMS());
		if (t2.DiffMS(t) == 0)
		{
			resp->SetStatusCode(Net::WebStatus::SC_NOT_MODIFIED);
			resp->AddDefHeaders(req);
			resp->AddCacheControl(cacheAge);
			resp->AddContentLength(0);
			resp->Write(Data::ByteArrayR(sbuff, 0));
			return true;
		}
	}

	sptr2 = IO::Path::GetFileExt(sbuff, fileName.v, fileName.leng);
	mime = Net::MIME::GetMIMEFromExt(CSTRP(sbuff, sptr2));

	sizeLeft = fs.GetLength();
	sb2.ClearStr();
	if (req->GetHeaderC(sb2, CSTR("Range")))
	{
		UInt64 fileSize = sizeLeft;
		if (!sb2.StartsWith(UTF8STRC("bytes=")))
		{
			resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
			resp->AddDefHeaders(req);
			resp->AddCacheControl(cacheAge);
			resp->AddContentLength(0);
			resp->Write(Data::ByteArrayR(sbuff, 0));
			return true;
		}
		if (sb2.IndexOf(',') != INVALID_INDEX)
		{
			resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
			resp->AddDefHeaders(req);
			resp->AddCacheControl(cacheAge);
			resp->AddContentLength(0);
			resp->Write(Data::ByteArrayR(sbuff, 0));
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
			resp->Write(Data::ByteArrayR(sbuff, 0));
			return true;
		}
		sptr = sb2.v;
		sptr[i] = 0;
		if (!Text::StrToUInt64(&sptr[6], start))
		{
			resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
			resp->AddDefHeaders(req);
			resp->AddCacheControl(cacheAge);
			resp->AddContentLength(0);
			resp->Write(Data::ByteArrayR(sbuff, 0));
			return true;
		}
		if (i + 1 < sb2.GetLength())
		{
			if (!Text::StrToInt64(&sptr[i + 1], end))
			{
				resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
				resp->AddDefHeaders(req);
				resp->AddCacheControl(cacheAge);
				resp->AddContentLength(0);
				resp->Write(Data::ByteArrayR(sbuff, 0));
				return true;
			}
			if (end <= (Int64)start || end > (Int64)sizeLeft)
			{
				resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
				resp->AddDefHeaders(req);
				resp->AddCacheControl(cacheAge);
				resp->AddContentLength(0);
				resp->Write(Data::ByteArrayR(sbuff, 0));
				return true;
			}
			sizeLeft = (UInt64)end - start;
		}
		else
		{
			sizeLeft = sizeLeft - start;
		}
		fs.SeekFromBeginning(start);
		resp->SetStatusCode(Net::WebStatus::SC_PARTIAL_CONTENT);
		UTF8Char sbuff[128];
		UnsafeArray<UTF8Char> sptr;
		sptr = Text::StrConcatC(sbuff, UTF8STRC("bytes "));
		sptr = Text::StrUInt64(sptr, start);
		*sptr++ = '-';
		sptr = Text::StrUInt64(sptr, start + sizeLeft - 1);
		*sptr++ = '/';
		sptr = Text::StrUInt64(sptr, fileSize);
		resp->AddHeader(CSTR("Content-Range"), CSTRP(sbuff, sptr));
	}
	resp->AddDefHeaders(req);
	resp->AddCacheControl(cacheAge);
	resp->AddLastModified(t);
	resp->AddContentType(mime);
	resp->AddHeader(CSTR("Accept-Ranges"), CSTR("bytes"));
	if (sizeLeft <= 0)
	{
		UInt8 buff[BUFFSIZE];
		UOSInt readSize;
		readSize = fs.Read(BYTEARR(buff));
		if (readSize == 0)
		{
			Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, sizeLeft, fs);
		}
		else
		{
			IO::MemoryStream mstm;
			while (readSize > 0)
			{
				sizeLeft += mstm.Write(Data::ByteArrayR(buff, readSize));
				readSize = fs.Read(BYTEARR(buff));
			}
			mstm.SeekFromBeginning(0);
			Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, sizeLeft, mstm);
		}
	}
	else
	{
		Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, sizeLeft, fs);
	}
	return true;
}

void Net::WebServer::HTTPServerUtil::SetCompLevel(Data::Compress::Deflater::CompLevel compLevel)
{
	Net::WebServer::HTTPServerUtil::compLevel = compLevel;
}

Bool Net::WebServer::HTTPServerUtil::AllowGZip(Net::BrowserInfo::BrowserType browser, Manage::OSInfo::OSType os)
{
	return browser != Net::BrowserInfo::BT_IE && browser != Net::BrowserInfo::BT_SAFARI && os != Manage::OSInfo::OT_IPHONE && os != Manage::OSInfo::OT_IPAD;
}

Bool Net::WebServer::HTTPServerUtil::AllowDeflate(Net::BrowserInfo::BrowserType browser, Manage::OSInfo::OSType os)
{
	return browser != Net::BrowserInfo::BT_IE;
}

Bool Net::WebServer::HTTPServerUtil::AllowKA(Net::BrowserInfo::BrowserType browser)
{
	return false;// browser == Net::BrowserInfo::BT_FIREFOX;
}
