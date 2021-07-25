#include "Stdafx.h"
#include "Crypto/Hash/CRC32R.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "Net/MIME.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Text/StringBuilderUTF8.h"
#include "miniz.h"

Bool Net::WebServer::HTTPServerUtil::MIMEToCompress(const UTF8Char *umime)
{
	const Char *mime = (const Char *)umime;
	if (Text::StrEquals(mime, "text/javascript"))
	{
		return true;
	}
	else if (Text::StrEquals(mime, "text/html"))
	{
		return true;
	}
	else if (Text::StrEquals(mime, "text/css"))
	{
		return true;
	}
	else if (Text::StrEquals(mime, "text/plain"))
	{
		return true;
	}
	else if (Text::StrEquals(mime, "text/xml"))
	{
		return true;
	}
	else if (Text::StrEquals(mime, "application/javascript"))
	{
		return true;
	}
	else if (Text::StrEquals(mime, "application/json"))
	{
		return true;
	}
	else if (Text::StrEquals(mime, "application/xml"))
	{
		return true;
	}
	else if (Text::StrEquals(mime, "application/xhtml+xml"))
	{
		return true;
	}
	return false;
}

void Net::WebServer::HTTPServerUtil::SendContent(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *mime, UInt64 contLeng, IO::Stream *fs)
{
	UOSInt i;
	UOSInt j;
	UInt8 buff[2048];
	UInt8 compBuff[2048];
	Bool contSent = false;
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
					if (browser != Net::BrowserInfo::BT_CHROME && browser != Net::BrowserInfo::BT_IE && browser != Net::BrowserInfo::BT_SAFARI && os != Manage::OSInfo::OT_IPHONE && os != Manage::OSInfo::OT_IPAD)
					{
						resp->AddHeader((const UTF8Char*)"Content-Encoding", (const UTF8Char*)"gzip");

						compBuff[0] = 0x1F;
						compBuff[1] = 0x8B;
						compBuff[2] = 8;
						compBuff[3] = 0;
						compBuff[4] = 0;
						compBuff[5] = 0;
						compBuff[6] = 0;
						compBuff[7] = 0;
						compBuff[8] = 0;
						resp->Write(compBuff, 8);

						mz_stream stm;
						Bool err = false;
						Int32 ret;
						Int32 oriLeng = (Int32)contLeng;
						Crypto::Hash::CRC32R crc;
						MemClear(&stm, sizeof(stm));

						stm.next_in = buff;
						stm.avail_in = 2048;
						stm.next_out = compBuff;
						stm.avail_out = 2048;
						mz_deflateInit2(&stm, MZ_BEST_COMPRESSION, MZ_DEFLATED, MZ_DEFAULT_WINDOW_BITS, 1, MZ_DEFAULT_STRATEGY);
						
						stm.avail_in = 0;
						while (!err && contLeng > 0)
						{
							stm.next_in = buff;
							if (contLeng > 2048)
							{
								stm.avail_in = (UInt32)fs->Read(buff, 2048);
							}
							else
							{
								stm.avail_in = (UInt32)fs->Read(buff, (UOSInt)contLeng);
							}
							if (stm.avail_in == 0)
							{
								break;
							}
							crc.Calc(buff, stm.avail_in);
							contLeng -= stm.avail_in;
							while (stm.avail_in > 0)
							{
								stm.next_out = compBuff;
								stm.avail_out = 2048;
								ret = mz_deflate(&stm, MZ_NO_FLUSH);
								if (stm.avail_out == 2048)
								{
									if (ret != 0)
									{
										err = true;
										break;
									}
								}
								else
								{
									resp->Write(compBuff, 2048 - stm.avail_out);
									stm.next_out = compBuff;
									stm.avail_out = 2048;
								}
							}
						}

						while (!err)
						{
							ret = mz_deflate(&stm, MZ_FINISH);
							if (stm.avail_out == 2048)
							{
								err = true;
							}
							else
							{
								resp->Write(compBuff, 2048 - stm.avail_out);
								stm.next_out = compBuff;
								stm.avail_out = 2048;

								if (ret == MZ_STREAM_END)
								{
									break;
								}
							}
						}
						mz_deflateEnd(&stm);

						crc.GetValue(&compBuff[8]);
						WriteInt32(&compBuff[0], ReadMInt32(&compBuff[8]));
						WriteInt32(&compBuff[4], (Int32)oriLeng);
						resp->Write(compBuff, 8);
						resp->ShutdownSend();

						contSent = true;
						break;
					}
				}
				else if (Text::StrEqualsICase(sarr[i], (const UTF8Char*)"deflate"))
				{
					if (browser != Net::BrowserInfo::BT_IE)
					{
						resp->AddHeader((const UTF8Char*)"Content-Encoding", (const UTF8Char*)"deflate");

						mz_stream stm;
						Bool err = false;
						Int32 ret;
						MemClear(&stm, sizeof(stm));

						stm.next_in = buff;
						stm.avail_in = 2048;
						stm.next_out = compBuff;
						stm.avail_out = 2048;
						mz_deflateInit2(&stm, MZ_BEST_COMPRESSION, MZ_DEFLATED, MZ_DEFAULT_WINDOW_BITS, 1, MZ_DEFAULT_STRATEGY);
						
						stm.avail_in = 0;
						while (!err && contLeng > 0)
						{
							stm.next_in = buff;
							if (contLeng > 2048)
							{
								stm.avail_in = (UInt32)fs->Read(buff, 2048);
							}
							else
							{
								stm.avail_in = (UInt32)fs->Read(buff, (UOSInt)contLeng);
							}
							if (stm.avail_in == 0)
							{
								break;
							}
							contLeng -= stm.avail_in;
							while (stm.avail_in > 0)
							{
								stm.next_out = compBuff;
								stm.avail_out = 2048;
								ret = mz_deflate(&stm, MZ_NO_FLUSH);
								if (stm.avail_out == 2048)
								{
									if (ret != 0)
									{
										err = true;
										break;
									}
								}
								else
								{
									resp->Write(compBuff, 2048 - stm.avail_out);
									stm.next_out = compBuff;
									stm.avail_out = 2048;
								}
							}
						}

						while (!err)
						{
							ret = mz_deflate(&stm, MZ_FINISH);
							if (stm.avail_out == 2048)
							{
								err = true;
							}
							else
							{
								resp->Write(compBuff, 2048 - stm.avail_out);
								stm.next_out = compBuff;
								stm.avail_out = 2048;

								if (ret == MZ_STREAM_END)
								{
									break;
								}
							}
						}
						mz_deflateEnd(&stm);
						resp->ShutdownSend();
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
			UOSInt readSize = fs->Read(buff, 2048);
			if (readSize <= 0)
			{
				break;
			}
			writeSize = resp->Write(buff, readSize);
			if (writeSize != readSize)
				break;
			contLeng -= readSize;
		}
	}
}

void Net::WebServer::HTTPServerUtil::SendContent(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *mime, UInt64 contLeng, const UInt8 *buff)
{
	UOSInt i;
	UOSInt j;
	UInt8 compBuff[2048];
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
					if (browser != Net::BrowserInfo::BT_CHROME)
					{
						resp->AddHeader((const UTF8Char*)"Content-Encoding", (const UTF8Char*)"gzip");

						compBuff[0] = 0x1F;
						compBuff[1] = 0x8B;
						compBuff[2] = 8;
						compBuff[3] = 0;
						compBuff[4] = 0;
						compBuff[5] = 0;
						compBuff[6] = 0;
						compBuff[7] = 0;
						compBuff[8] = 0;
						resp->Write(compBuff, 8);

						mz_stream stm;
						Bool err = false;
						Int32 ret;
						Crypto::Hash::CRC32R crc;
						crc.Calc(buff, (UOSInt)contLeng);
						MemClear(&stm, sizeof(stm));

						stm.next_in = buff;
						stm.avail_in = (UInt32)contLeng;
						stm.next_out = compBuff;
						stm.avail_out = 2048;
						mz_deflateInit2(&stm, MZ_BEST_COMPRESSION, MZ_DEFLATED, MZ_DEFAULT_WINDOW_BITS, 1, MZ_DEFAULT_STRATEGY);
						
						while (!err)
						{
							ret = mz_deflate(&stm, MZ_FINISH);
							if (stm.avail_out == 2048)
							{
								err = true;
							}
							else
							{
								resp->Write(compBuff, 2048 - stm.avail_out);
								stm.next_out = compBuff;
								stm.avail_out = 2048;

								if (ret == MZ_STREAM_END)
								{
									break;
								}
							}
						}
						mz_deflateEnd(&stm);
						crc.GetValue(&compBuff[8]);
						WriteInt32(&compBuff[0], ReadMInt32(&compBuff[8]));
						WriteInt32(&compBuff[4], (Int32)contLeng);
						resp->Write(compBuff, 8);
						resp->ShutdownSend();
						contSent = true;
						break;
					}
				}
				else if (Text::StrEqualsICase(sarr[i], (const UTF8Char*)"deflate"))
				{
					if (browser != Net::BrowserInfo::BT_IE)
					{
						resp->AddHeader((const UTF8Char*)"Content-Encoding", (const UTF8Char*)"deflate");

						mz_stream stm;
						Bool err = false;
						Int32 ret;
						MemClear(&stm, sizeof(stm));

						stm.next_in = buff;
						stm.avail_in = (UInt32)contLeng;
						stm.next_out = compBuff;
						stm.avail_out = 2048;
						mz_deflateInit2(&stm, MZ_BEST_COMPRESSION, MZ_DEFLATED, MZ_DEFAULT_WINDOW_BITS, 1, MZ_DEFAULT_STRATEGY);
						
						while (!err)
						{
							ret = mz_deflate(&stm, MZ_FINISH);
							if (stm.avail_out == 2048)
							{
								err = true;
							}
							else
							{
								resp->Write(compBuff, 2048 - stm.avail_out);
								stm.next_out = compBuff;
								stm.avail_out = 2048;

								if (ret == MZ_STREAM_END)
								{
									break;
								}
							}
						}
						mz_deflateEnd(&stm);
						resp->ShutdownSend();
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
		resp->Write(buff, (UOSInt)contLeng);
	}
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
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_SEQUENTIAL));
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
		u8ptr = Text::StrConcat(u8buff, (const UTF8Char*)"bytes ");
		u8ptr = Text::StrUInt64(u8ptr, start);
		*u8ptr++ = '-';
		u8ptr = Text::StrUInt64(u8ptr, start + sizeLeft - 1);
		*u8ptr++ = '/';
		u8ptr = Text::StrUInt64(u8ptr, fileSize);
		resp->AddHeader((const UTF8Char*)"Content-Range", u8buff);
	}
	resp->AddDefHeaders(req);
	resp->AddCacheControl(cacheAge);
	resp->AddLastModified(&t);
	resp->AddContentType(mime = Net::MIME::GetMIMEFromExt(sbuff));
	resp->AddHeader((const UTF8Char*)"Accept-Ranges", (const UTF8Char*)"bytes");
	if (sizeLeft <= 0)
	{
		UInt8 buff[2048];
		UOSInt readSize;
		readSize = fs->Read(buff, 2048);
		if (readSize == 0)
		{
			Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, sizeLeft, fs);
		}
		else
		{
			IO::MemoryStream *mstm;
			NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"Net.WebServer.HTTPUtil.ResponseFile.mstm"));
			while (readSize > 0)
			{
				sizeLeft += mstm->Write(buff, readSize);
				readSize = fs->Read(buff, 2048);
			}
			mstm->SeekFromBeginning(0);
			Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, sizeLeft, mstm);
			DEL_CLASS(mstm);
		}
	}
	else
	{
		Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, sizeLeft, fs);
	}
	DEL_CLASS(fs);
	return true;
}
