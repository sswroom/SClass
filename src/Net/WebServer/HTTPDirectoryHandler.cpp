#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/Sort/ArtificialQuickSortC.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Net/MIME.h"
#include "Net/WebServer/HTTPDirectoryHandler.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "Text/XML.h"
#include "Text/TextEnc/URIEncoding.h"

typedef struct
{
	const UTF8Char *fileName;
	UInt64 fileSize;
	Int64 modTime;
	IO::Path::PathType pt;
	UInt32 cnt;
} DirectoryEntry;

OSInt __stdcall HTTPDirectoryHandler_CompareFuncName(void *obj1, void *obj2)
{
	DirectoryEntry *ent1 = (DirectoryEntry *)obj1;
	DirectoryEntry *ent2 = (DirectoryEntry *)obj2;
	if (ent1->pt != ent2->pt)
	{
		if (ent1->pt == IO::Path::PT_DIRECTORY)
		{
			return -1;
		}
		else if (ent2->pt == IO::Path::PT_DIRECTORY)
		{
			return 1;
		}
	}
	OSInt ret = Text::StrCompareICase(ent1->fileName, ent2->fileName);
	if (ret != 0)
		return ret;
	if (ent1->fileSize > ent2->fileSize)
	{
		return 1;
	}
	else if (ent1->fileSize < ent2->fileSize)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

OSInt __stdcall HTTPDirectoryHandler_CompareFuncSize(void *obj1, void *obj2)
{
	DirectoryEntry *ent1 = (DirectoryEntry *)obj1;
	DirectoryEntry *ent2 = (DirectoryEntry *)obj2;

	if (ent1->fileSize > ent2->fileSize)
	{
		return 1;
	}
	else if (ent1->fileSize < ent2->fileSize)
	{
		return -1;
	}
	else
	{
		return Text::StrCompareICase(ent1->fileName, ent2->fileName);
	}
}

OSInt __stdcall HTTPDirectoryHandler_CompareFuncCount(void *obj1, void *obj2)
{
	DirectoryEntry *ent1 = (DirectoryEntry *)obj1;
	DirectoryEntry *ent2 = (DirectoryEntry *)obj2;

	if (ent1->cnt > ent2->cnt)
	{
		return 1;
	}
	else if (ent1->cnt < ent2->cnt)
	{
		return -1;
	}
	else
	{
		return HTTPDirectoryHandler_CompareFuncName(ent1, ent2);
	}
}

void Net::WebServer::HTTPDirectoryHandler::AddCacheHeader(Net::WebServer::IWebResponse *resp)
{
	switch (this->ctype)
	{
	case CT_PUBLIC:
		resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"public");
		break;
	case CT_PRIVATE:
		resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"private");
		break;
	case CT_NO_CACHE:
		resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
		break;
	default:
		break;
	}
	Int32 period = this->expirePeriod;
	if (period)
	{
		Data::DateTime t;
		t.SetCurrTimeUTC();
		t.AddSecond(period);
		resp->AddExpireTime(&t);
	}
	if (this->allowOrigin)
	{
		resp->AddHeader((const UTF8Char*)"Access-Control-Allow-Origin", this->allowOrigin);
	}
}

void Net::WebServer::HTTPDirectoryHandler::ResponsePackageFile(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, IO::PackageFile *packageFile)
{
	if (!this->allowBrowsing)
	{
		resp->ResponseError(req, Net::WebStatus::SC_FORBIDDEN);
		return;
	}
	
	Text::StringBuilderUTF8 sb2;
	Text::StringBuilderUTF8 sbOut;
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	OSInt si;

	sb2.ClearStr();
	sb2.Append(req->GetRequestURI());
	sptr = sb2.ToString();
	si = Text::StrIndexOf(sptr, '?');
	if (si >= 0)
	{
		sptr[si] = 0;
		i = (UOSInt)si;
	}
	else
	{
		i = Text::StrCharCnt(sptr);
	}
/*	if (sptr[i - 1] != '/')
	{
		sb.Append(IO::Path::PATH_SEPERATOR, 1);
	}*/
/*	if (this->allowUpload && req->GetReqMethod() == Net::WebServer::IWebRequest::REQMETH_HTTP_POST)
	{
		const UInt8 *uplfile;
		OSInt uplSize;
		req->ParseHTTPForm();
		uplfile = req->GetHTTPFormFile(L"uploadfile", sbuff, &uplSize);
		if (uplfile)
		{
			Text::StringBuilder sbTmp;
			sbTmp.Append(sb.ToString());
			sbTmp.Append(sbuff);
			if (IO::Path::GetPathType(sbTmp.ToString()) == IO::Path::PT_UNKNOWN)
			{
				IO::FileStream *uplFS;
				NEW_CLASS(uplFS, IO::FileStream(sbTmp.ToString(), IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NO_WRITE_BUFFER));
				uplFS->Write(uplfile, uplSize);
				DEL_CLASS(uplFS);
			}
		}
	}*/

	resp->AddDefHeaders(req);
	resp->AddContentType((const UTF8Char*)"text/html; charset=UTF-8");
	AddCacheHeader(resp);

	UTF8Char u8buff[512];
	const UTF8Char *csptr;

	sbOut.Append((const UTF8Char*)"<html><head><title>Index of ");
	Text::TextEnc::URIEncoding::URIDecode(u8buff, sptr);
	csptr = Text::XML::ToNewHTMLText(u8buff);
	sbOut.Append(csptr);
	sbOut.Append((const UTF8Char*)"</title></head>\r\n<body>\r\n");
	sbOut.Append((const UTF8Char*)"<h2>Index Of ");
	sbOut.Append(csptr);
	Text::XML::FreeNewText(csptr);
	sbOut.Append((const UTF8Char*)"</h2>\r\n");
	sbOut.Append((const UTF8Char*)"<a href=\"..\">Up one level</a><br/>\r\n");
/*	if (this->allowUpload)
	{
		const WChar *csptr = Text::XML::ToNewAttrText(sptr);
		sbOut.Append((const UTF8Char*)"<form name=\"upload\" method=\"POST\" action=");
		sbOut.Append(csptr);
		sbOut.Append((const UTF8Char*)" enctype=\"multipart/form-data\">");
		Text::XML::FreeNewText(csptr);
		sbOut.Append((const UTF8Char*)"Upload: <input type=\"file\" name=\"uploadfile\"/><br/><input type=\"submit\"/>");
		sbOut.Append((const UTF8Char*)"</form>");
	}*/
	sbOut.Append((const UTF8Char*)"<table><tr><th>Name</th><th>MIME</th><th>Size</th><th>Modified Date</th></tr>\r\n");

	UTF8Char u8buff2[256];
	Data::DateTime modTime;
	IO::PackageFile::PackObjectType pot;
	i = 0;
	j = packageFile->GetCount();
	while (i < j)
	{
		pot = packageFile->GetItemType(i);
		if (pot == IO::PackageFile::POT_STREAMDATA || pot == IO::PackageFile::POT_PACKAGEFILE)
		{
			sbOut.Append((const UTF8Char*)"<tr><td>");
			sbOut.Append((const UTF8Char*)"<a href=\"");
			packageFile->GetItemName(u8buff, i);
			Text::TextEnc::URIEncoding::URIEncode(u8buff2, u8buff);
			sbOut.Append(u8buff2);
			if (pot == IO::PackageFile::POT_PACKAGEFILE)
			{
				sbOut.Append((const UTF8Char*)"/");
			}
			sbOut.Append((const UTF8Char*)"\">");
			Text::XML::ToXMLText(u8buff2, u8buff);
			sbOut.Append(u8buff2);
			sbOut.Append((const UTF8Char*)"</a></td><td>");
			if (pot == IO::PackageFile::POT_PACKAGEFILE)
			{
				sbOut.Append((const UTF8Char*)"Directory");
				sbOut.Append((const UTF8Char*)"</td><td>");
				sbOut.Append((const UTF8Char*)"-");
			}
			else
			{
				IO::Path::GetFileExt(u8buff2, u8buff);
				sbOut.Append(Net::MIME::GetMIMEFromExt(u8buff2));
				sbOut.Append((const UTF8Char*)"</td><td>");
				sbOut.AppendU64(packageFile->GetItemSize(i));
			}

			modTime.SetTicks(packageFile->GetItemModTimeTick(i));
			sbOut.Append((const UTF8Char*)"</td><td>");
			sbOut.AppendDate(&modTime);
			sbOut.Append((const UTF8Char*)"</td></tr>\r\n");
		}
		
		i++;
	}
	sbOut.Append((const UTF8Char*)"</table></body></html>");
	Net::WebServer::HTTPServerUtil::SendContent(req, resp, (const UTF8Char*)"text/html", sbOut.GetLength(), sbOut.ToString());
	return ;
}

void Net::WebServer::HTTPDirectoryHandler::StatLoad(Net::WebServer::HTTPDirectoryHandler::StatInfo *stat)
{
	IO::FileStream *fs;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(fs, IO::FileStream(stat->statFileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (!fs->IsError())
	{
		UTF8Char *sarr[2];
		Text::UTF8Reader *reader;
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		sb.ClearStr();
		while (reader->ReadLine(&sb, 1024))
		{
			if (Text::StrSplit(sarr, 2, sb.ToString(), '\t') == 2)
			{
				stat->cntMap->Put(sarr[1], Text::StrToInt32(sarr[0]));
			}
			sb.ClearStr();
		}
		DEL_CLASS(reader);
	}
	DEL_CLASS(fs);
}

void Net::WebServer::HTTPDirectoryHandler::StatSave(Net::WebServer::HTTPDirectoryHandler::StatInfo *stat)
{
	IO::FileStream *fs;
	Text::StringBuilderUTF8 sb;
	Data::ArrayList<const UTF8Char *> *nameList = stat->cntMap->GetKeys();
	Data::ArrayList<Int32> *cntList = stat->cntMap->GetValues();
	stat->updated = false;
	if (nameList->GetCount() > 0)
	{
		NEW_CLASS(fs, IO::FileStream(stat->statFileName, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
		if (!fs->IsError())
		{
			Text::UTF8Writer *writer;
			NEW_CLASS(writer, Text::UTF8Writer(fs));
			UOSInt i;
			UOSInt j;
			i = 0;
			j = nameList->GetCount();
			while (i < j)
			{
				sb.ClearStr();
				sb.AppendI32(cntList->GetItem(i));
				sb.AppendChar('\t', 1);
				sb.Append(nameList->GetItem(i));
				writer->WriteLine(sb.ToString());
				i++;
			}
			DEL_CLASS(writer);
		}
		DEL_CLASS(fs);
	}
}

Net::WebServer::HTTPDirectoryHandler::HTTPDirectoryHandler(const UTF8Char *rootDir, Bool allowBrowsing, UInt64 fileCacheSize, Bool allowUpload)
{
	this->rootDir = Text::StrCopyNew(rootDir);
	this->allowBrowsing = allowBrowsing;
	this->allowUpload = allowUpload;
	this->ctype = CT_DEFAULT;
	this->expirePeriod = 0;
	this->fileCacheSize = fileCacheSize;
	this->fileCacheUsing = 0;
	this->allowOrigin = 0;
	NEW_CLASS(this->fileCache, Data::BTreeUTF8Map<CacheInfo*>());
	NEW_CLASS(this->fileCacheMut, Sync::Mutex());
	this->packageMap = 0;
	this->packageMut = 0;
	this->statMap = 0;
	this->statMut = 0;
}

Net::WebServer::HTTPDirectoryHandler::~HTTPDirectoryHandler()
{
	Text::StrDelNew(this->rootDir);
	UOSInt cacheCnt;
	CacheInfo **cacheList = this->fileCache->ToArray(&cacheCnt);
	while (cacheCnt-- > 0)
	{
		MemFree(cacheList[cacheCnt]->buff);
		MemFree(cacheList[cacheCnt]);
	}
	MemFree(cacheList);
	DEL_CLASS(this->fileCache);
	DEL_CLASS(this->fileCacheMut);
	if (this->packageMap)
	{
		Data::ArrayList<PackageInfo*> *packageList = this->packageMap->GetValues();
		PackageInfo *package;
		UOSInt i;
		i = packageList->GetCount();
		while (i-- > 0)
		{
			package = packageList->GetItem(i);
			Text::StrDelNew(package->fileName);
			DEL_CLASS(package->packageFile);
			MemFree(package);
		}
		DEL_CLASS(this->packageMap);
		DEL_CLASS(this->packageMut);
	}
	SDEL_TEXT(this->allowOrigin);
	if (this->statMap)
	{
		Data::ArrayList<Net::WebServer::HTTPDirectoryHandler::StatInfo*> *statList = this->statMap->GetValues();
		Net::WebServer::HTTPDirectoryHandler::StatInfo *stat;
		UOSInt i = statList->GetCount();
		while (i-- > 0)
		{
			stat = statList->GetItem(i);
			this->StatSave(stat);

			Text::StrDelNew(stat->reqPath);
			Text::StrDelNew(stat->statFileName);
			DEL_CLASS(stat->cntMap);
			MemFree(stat);
		}
		DEL_CLASS(this->statMap);
		DEL_CLASS(this->statMut);
	}
}

Bool Net::WebServer::HTTPDirectoryHandler::ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq)
{
	UInt8 buff[2048];
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[1024];
	UTF8Char sbuff2[1024];
	UTF8Char *sptr;
	Data::DateTime t;
	const UTF8Char *mime;
	OSInt i;
	if (this->DoRequest(req, resp, subReq))
	{
		return true;
	}

	if (req->GetProtocol() != Net::WebServer::IWebRequest::REQPROTO_HTTP1_0 && req->GetProtocol() != Net::WebServer::IWebRequest::REQPROTO_HTTP1_1)
	{
		resp->ResponseError(req, Net::WebStatus::SC_METHOD_NOT_ALLOWED);
		return true;
	}
	if (this->packageMap)
	{
		i = Text::StrIndexOf(subReq, '?');
		if (i >= 0)
		{
			sb.ClearStr();
			sb.AppendC(subReq, (UOSInt)i);
		}
		else
		{
			sb.Append(subReq);
		}
		i = Text::StrIndexOf(&sb.ToString()[1], '/');
		if (i >= 0)
		{
			sb.ToString()[i + 1] = 0;
		}
		PackageInfo *package;
		this->packageMut->LockRead();
		package = this->packageMap->Get(&sb.ToString()[1]);
		this->packageMut->UnlockRead();
		if (package)
		{
			if (i < 0)
			{
				sb.ClearStr();
				sb.Append(subReq);
				ResponsePackageFile(req, resp, sb.ToString(), package->packageFile);
				return true;
			}
			
			sptr = &sb.ToString()[i + 2];
			if (sptr[0] == 0)
			{
				sb.ClearStr();
				sb.Append(subReq);
				ResponsePackageFile(req, resp, sb.ToString(), package->packageFile);
				return true;
			}
			const IO::PackFileItem *pitem = package->packageFile->GetPackFileItem(sptr);
			if (pitem)
			{
				IO::PackageFile::PackObjectType pot = package->packageFile->GetPItemType(pitem);
				if (pot == IO::PackageFile::POT_STREAMDATA)
				{
					IO::IStreamData *stmData = package->packageFile->GetPItemStmData(pitem);
					if (stmData)
					{
						UOSInt dataLen = (UOSInt)stmData->GetDataSize();
						UInt8 *dataBuff = MemAlloc(UInt8, dataLen);
						stmData->GetRealData(0, dataLen, dataBuff);
						DEL_CLASS(stmData);
						IO::Path::GetFileExt(sbuff, sptr);
						mime = Net::MIME::GetMIMEFromExt(sbuff);

						resp->AddDefHeaders(req);
						t.SetTicks(pitem->modTimeTick);
						t.ToLocalTime();
						resp->AddLastModified(&t);
						if (this->allowOrigin)
						{
							resp->AddHeader((const UTF8Char*)"Access-Control-Allow-Origin", this->allowOrigin);
						}
						resp->AddContentType(mime);
						Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, dataLen, dataBuff);
						MemFree(dataBuff);
						return true;
					}
				}
				else if (pot == IO::PackageFile::POT_PACKAGEFILE)
				{
					IO::PackageFile *innerPF = package->packageFile->GetPItemPack(pitem);
					if (innerPF)
					{
						const IO::PackFileItem *pitem2 = innerPF->GetPackFileItem((const UTF8Char*)"index.html");
						if (pitem2 && innerPF->GetPItemType(pitem2) == IO::PackageFile::POT_STREAMDATA)
						{
							IO::IStreamData *stmData = innerPF->GetPItemStmData(pitem2);
							if (stmData)
							{
								UOSInt dataLen = (UOSInt)stmData->GetDataSize();
								UInt8 *dataBuff = MemAlloc(UInt8, dataLen);
								stmData->GetRealData(0, dataLen, dataBuff);
								DEL_CLASS(stmData);
								mime = Net::MIME::GetMIMEFromExt((const UTF8Char*)"html");

								resp->AddDefHeaders(req);
								t.SetTicks(pitem2->modTimeTick);
								t.ToLocalTime();
								resp->AddLastModified(&t);
								if (this->allowOrigin)
								{
									resp->AddHeader((const UTF8Char*)"Access-Control-Allow-Origin", this->allowOrigin);
								}
								resp->AddContentType(mime);
								Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, dataLen, dataBuff);
								MemFree(dataBuff);
							}
							else
							{
								sb.ClearStr();
								sb.Append(subReq);
								ResponsePackageFile(req, resp, sb.ToString(), innerPF);
							}
						}
						else
						{
							sb.ClearStr();
							sb.Append(subReq);
							ResponsePackageFile(req, resp, sb.ToString(), innerPF);
						}
						DEL_CLASS(innerPF);
						return true;
					}
				}
			}
		}
	}
	CacheInfo *cache;
	Sync::MutexUsage mutUsage(this->fileCacheMut);
	cache = this->fileCache->Get(subReq);
	if (cache != 0)
	{
		Sync::Interlocked::Increment(&this->fileCacheUsing);
		mutUsage.EndUse();
		if (this->statMap)
		{
			Net::WebServer::HTTPDirectoryHandler::StatInfo *stat;
			sb.ClearStr();
			sb.Append(subReq);
			i = sb.LastIndexOf('/');
			sb.TrimToLength((UOSInt)i);
			Sync::MutexUsage statMutUsage(this->statMut);
			stat = this->statMap->Get(sb.ToString());
			if (stat)
			{
				sb.Append(&subReq[i + 1]);
				i = sb.IndexOf('?');
				if (i >= 0)
				{
					sb.TrimToLength((UOSInt)i);
				}
				if (sb.ToString()[0] == 0)
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)"index.html");
				}
				Int32 cnt = stat->cntMap->Get(sb.ToString());
				stat->cntMap->Put(sb.ToString(), cnt + 1);
				stat->updated = true;
			}
			statMutUsage.EndUse();
		}
		resp->AddDefHeaders(req);
		AddCacheHeader(resp);
		t.SetTicks(cache->t);
		t.ToLocalTime();
		resp->AddLastModified(&t);
		i = Text::StrIndexOf(subReq, '?');
		if (i >= 0)
		{
			Text::StringBuilderUTF8 sbc;
			sbc.AppendC(subReq, (UOSInt)i);
			if (sbc.EndsWith('/'))
			{
				resp->AddContentType(mime = Net::MIME::GetMIMEFromExt((const UTF8Char*)"html"));
			}
			else
			{
				i = Text::StrLastIndexOf(sbc.ToString(), '.');
				resp->AddContentType(mime = Net::MIME::GetMIMEFromExt(sbc.ToString() + i + 1));
			}
		}
		else
		{
			if (Text::StrEndsWith(subReq, (const UTF8Char*)"/"))
			{
				resp->AddContentType(mime = Net::MIME::GetMIMEFromExt((const UTF8Char*)"html"));
			}
			else
			{
				i = Text::StrLastIndexOf(subReq, '.');
				resp->AddContentType(mime = Net::MIME::GetMIMEFromExt(subReq + i + 1));
			}
		}
		Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, cache->buffSize, cache->buff);
		Sync::Interlocked::Decrement(&this->fileCacheUsing);
		return true;
	}
	mutUsage.EndUse();

	sb.ClearStr();
	sb.Append(this->rootDir);
	sptr = sb.GetEndPtr();
	if (sptr[-1] == '\\' || sptr[-1] == '/')
	{
		sb.RemoveChars(1);
	}
	const UTF8Char *reqTarget = subReq;
	sb.Append(reqTarget);
	sptr = sb.ToString();
	UTF8Char *sptr2 = 0;
	i = Text::StrIndexOf(sptr, '?');
	if (i >= 0)
	{
		sptr2 = &sptr[i + 1];
		sptr2[-1] = 0;
	}
	if (IO::Path::PATH_SEPERATOR != '/')
	{
		Text::StrReplace(sptr, '/', IO::Path::PATH_SEPERATOR);
	}

	IO::Path::PathType pt = IO::Path::GetPathType(sptr);
	if (pt == IO::Path::PT_UNKNOWN)
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}
	else if (pt == IO::Path::PT_DIRECTORY)
	{
		Text::StringBuilderUTF8 sb2;
		sb2.Append(sb.ToString());
		if (sb.EndsWith((Char)IO::Path::PATH_SEPERATOR))
		{
			sb2.Append((const UTF8Char*)"index.html");
		}
		else
		{
			sb2.AppendChar(IO::Path::PATH_SEPERATOR, 1);
			sb2.Append((const UTF8Char*)"index.html");
		}
		pt = IO::Path::GetPathType(sb2.ToString());
		if (pt == IO::Path::PT_FILE)
		{
			sb.ClearStr();
			sb.Append(sb2.ToString());
			sptr = sb.ToString();
			IO::FileStream *fs;
			UInt64 sizeLeft;

			IO::Path::GetFileExt(sbuff, sptr);
			NEW_CLASS(fs, IO::FileStream(sptr, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_SEQUENTIAL));
			fs->GetFileTimes(0, 0, &t);

			sb2.ClearStr();
			if (req->GetHeader(&sb2, (const UTF8Char*)"If-Modified-Since"))
			{
				Data::DateTime t2;
				t2.SetValue(sb2.ToString());
				t2.AddMS(t.GetMS());
				if (t2.DiffMS(&t) == 0)
				{
					resp->SetStatusCode(Net::WebStatus::SC_NOT_MODIFIED);
					resp->AddDefHeaders(req);
					AddCacheHeader(resp);
					resp->AddContentLength(0);
					resp->Write(buff, 0);
					DEL_CLASS(fs);
					return true;
				}
			}

			resp->AddDefHeaders(req);
			AddCacheHeader(resp);
			resp->AddLastModified(&t);
			resp->AddContentType(mime = Net::MIME::GetMIMEFromExt(sbuff));
			sizeLeft = fs->GetLength();
			if (sizeLeft < this->fileCacheSize)
			{
				UOSInt readSize;
				cache = MemAlloc(CacheInfo, 1);
				cache->buff = MemAlloc(UInt8, (UOSInt)sizeLeft);
				cache->buffSize = (UOSInt)sizeLeft;
				cache->t = t.ToTicks();
				readSize = fs->Read(cache->buff, (UOSInt)sizeLeft);
				if (readSize == sizeLeft)
				{
					Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, sizeLeft, cache->buff);
					Sync::MutexUsage mutUsage(this->fileCacheMut);
					this->fileCache->Put(subReq, cache);
					mutUsage.EndUse();
				}
				else
				{
					resp->AddContentLength(sizeLeft);
					resp->Write(cache->buff, readSize);
					MemFree(cache->buff);
					MemFree(cache);
				}
			}
			else
			{
				Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, sizeLeft, fs);
			}
			DEL_CLASS(fs);
			return true;
		}
		else
		{
			if (this->allowBrowsing)
			{
				Text::StringBuilderUTF8 sb2;
				Text::StringBuilderUTF8 sbOut;
				Net::WebServer::HTTPDirectoryHandler::StatInfo *stat = 0;
				Int32 sort = 0;

				sb2.ClearStr();
				sb2.Append(req->GetRequestURI());
				i = sb2.IndexOf('?');
				if (i >= 0)
				{
					sb2.TrimToLength((UOSInt)i);
				}
				if (!sb2.EndsWith('/'))
				{
					sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
				}
				if (this->allowUpload && req->GetReqMethod() == Net::WebServer::IWebRequest::REQMETH_HTTP_POST)
				{
					const UInt8 *uplfile;
					UOSInt uplSize;
					UOSInt fileId;
					req->ParseHTTPForm();
					fileId = 0;
					while ((uplfile = req->GetHTTPFormFile((const UTF8Char *)"uploadfile", fileId, (UTF8Char*)buff, sizeof(buff), &uplSize)) != 0)
					{
						Text::StringBuilderUTF8 sbTmp;
						sbTmp.Append(sb.ToString());
						sbTmp.Append((UTF8Char*)buff);
						if (IO::Path::GetPathType(sbTmp.ToString()) == IO::Path::PT_UNKNOWN)
						{
							IO::FileStream *uplFS;
							NEW_CLASS(uplFS, IO::FileStream(sbTmp.ToString(), IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NO_WRITE_BUFFER));
							uplFS->Write(uplfile, uplSize);
							DEL_CLASS(uplFS);
						}
						fileId++;
					}
				}

				resp->AddDefHeaders(req);
				resp->AddContentType((const UTF8Char*)"text/html; charset=UTF-8");
				AddCacheHeader(resp);

				Bool isRoot = false;
				const UTF8Char *csptr;
				sbOut.AppendC((const UTF8Char*)"<html><head><title>Index of ", 28);
				Text::TextEnc::URIEncoding::URIDecode(sbuff, sb2.ToString());
				csptr = Text::XML::ToNewHTMLText(sbuff);
				sbOut.Append(csptr);
				sbOut.AppendC((const UTF8Char*)"</title></head>\r\n<body>\r\n",25);
				sbOut.AppendC((const UTF8Char*)"<h2>Index Of ", 13);
				sbOut.Append(csptr);
				Text::XML::FreeNewText(csptr);
				sbOut.AppendC((const UTF8Char*)"</h2>\r\n", 7);
				if (!sb2.Equals((const UTF8Char*)"/"))
				{
					sbOut.AppendC((const UTF8Char*)"<a href=\"..\">Up one level</a><br/>\r\n", 36);
				}
				else
				{
					isRoot = true;
				}
				if (this->allowUpload)
				{
					csptr = Text::XML::ToNewAttrText(sb2.ToString());
					sbOut.AppendC((const UTF8Char*)"<form name=\"upload\" method=\"POST\" action=", 41);
					sbOut.Append(csptr);
					sbOut.AppendC((const UTF8Char*)" enctype=\"multipart/form-data\">", 31);
					Text::XML::FreeNewText(csptr);
					sbOut.AppendC((const UTF8Char*)"Upload: <input type=\"file\" name=\"uploadfile\" multiple/><br/><input type=\"submit\"/>", 82);
					sbOut.AppendC((const UTF8Char*)"</form>", 7);
				}

				csptr = req->GetQueryValue((const UTF8Char *)"sort");
				if (csptr)
				{
					sort = Text::StrToInt32(csptr);
				}

				Text::StringBuilderUTF8 sb3;
				sb3.Append(sb2.ToString());
				sb3.AppendC((const UTF8Char*)"?sort=1", 7);
				csptr = Text::XML::ToNewAttrText(sb3.ToString());
				sbOut.AppendC((const UTF8Char*)"<table><tr><th><a href=", 23);
				sbOut.Append(csptr);
				Text::XML::FreeNewText(csptr);
				sbOut.AppendC((const UTF8Char*)">Name</a></th><th>MIME</th><th><a href=", 39);
				sb3.ClearStr();
				sb3.AppendC(sb2.ToString(), sb2.GetLength());
				sb3.AppendC((const UTF8Char*)"?sort=2", 7);
				csptr = Text::XML::ToNewAttrText(sb3.ToString());
				sbOut.Append(csptr);
				Text::XML::FreeNewText(csptr);
				sbOut.AppendC((const UTF8Char*)">File Size</a></th>", 19);
				if (this->statMap)
				{
					sbOut.AppendC((const UTF8Char*)"<th><a href=", 12);
					sb3.ClearStr();
					sb3.AppendC(sb2.ToString(), sb2.GetLength());
					sb3.AppendC((const UTF8Char*)"?sort=3", 7);
					csptr = Text::XML::ToNewAttrText(sb3.ToString());
					sbOut.Append(csptr);
					Text::XML::FreeNewText(csptr);
					sbOut.AppendC((const UTF8Char*)">Download Count</a></th>", 24);
				}
				sbOut.AppendC((const UTF8Char*)"<th>Modified Date</th></tr>\r\n", 29);

				if (isRoot)
				{
					Data::DateTime modTime;
					if (this->packageMap)
					{
						Data::ArrayList<PackageInfo*> *packageList;
						PackageInfo *package;
						UOSInt i;
						UOSInt j;
						this->packageMut->LockRead();
						packageList = this->packageMap->GetValues();
						i = 0;
						j = packageList->GetCount();
						while (i < j)
						{
							package = packageList->GetItem(i);
							sbOut.AppendC((const UTF8Char*)"<tr><td>", 8);
							sbOut.AppendC((const UTF8Char*)"<a href=\"", 9);
							Text::TextEnc::URIEncoding::URIEncode(sbuff2, package->fileName);
							sbOut.Append(sbuff2);
							sbOut.AppendChar('/', 1);
							sbOut.AppendC((const UTF8Char*)"\">", 2);
							Text::XML::ToXMLText(sbuff2, package->fileName);
							sbOut.Append(sbuff2);
							sbOut.AppendC((const UTF8Char*)"</a></td><td>", 13);
							sbOut.AppendC((const UTF8Char*)"Directory", 9);
							sbOut.AppendC((const UTF8Char*)"</td><td>", 9);
							sbOut.AppendChar('-', 1);
							if (this->statMap)
							{
								sbOut.AppendC((const UTF8Char*)"</td><td>0", 10);
							}
							modTime.SetTicks(package->modTime);
							sbOut.AppendC((const UTF8Char*)"</td><td>", 9);
							sbOut.AppendDate(&modTime);
							sbOut.AppendC((const UTF8Char*)"</td></tr>\r\n", 12);
							
							i++;
						}
						this->packageMut->UnlockRead();
					}
				}

				sptr2 = Text::StrConcat(sbuff, sb.ToString());
				Text::StrConcat(sptr2, IO::Path::ALL_FILES);
				IO::Path::FindFileSession *sess = IO::Path::FindFile(sbuff);
				if (sess)
				{
					Sync::MutexUsage mutUsage(this->statMut);
					if (this->statMap)
					{
						if (sb2.GetLength() > 1 && sb2.EndsWith('/'))
						{
							sb2.RemoveChars(1);
						}
						stat = this->statMap->Get(sb2.ToString());
						if (stat == 0)
						{
							stat = MemAlloc(Net::WebServer::HTTPDirectoryHandler::StatInfo, 1);
							stat->reqPath = Text::StrCopyNew(sb2.ToString());
							NEW_CLASS(stat->cntMap, Data::StringUTF8Map<Int32>());
							stat->updated = true;
							sb2.ClearStr();
							sb2.AppendC(sb.ToString(), sb.GetLength());
							sb2.AppendC((const UTF8Char*)".counts", 7);
							stat->statFileName = Text::StrCopyNew(sb2.ToString());
							this->statMap->Put(stat->reqPath, stat);
							this->StatLoad(stat);
						}
					}
					else
					{
						mutUsage.EndUse();
					}

					Int32 cnt;
					UInt64 fileSize;
					IO::Path::PathType pt;
					Data::DateTime modTime;
					if (sort == 0)
					{
						while (IO::Path::FindNextFile(sptr2, sess, &modTime, &pt, &fileSize))
						{
							if (Text::StrEquals(sptr2, (const UTF8Char*)".") || Text::StrEquals(sptr2, (const UTF8Char*)".."))
							{
							}
							else
							{
								if (stat)
								{
									cnt = stat->cntMap->Get(sptr2);
								}
								else
								{
									cnt = 0;
								}
								sbOut.AppendC((const UTF8Char*)"<tr><td>", 8);
								sbOut.AppendC((const UTF8Char*)"<a href=\"", 9);
								Text::TextEnc::URIEncoding::URIEncode(sbuff2, sptr2);
								sbOut.Append(sbuff2);
								if (pt == IO::Path::PT_DIRECTORY)
								{
									sbOut.AppendChar('/', 1);
								}
								sbOut.AppendC((const UTF8Char*)"\">", 2);
								if (cnt > 0)
								{
									sbOut.AppendC((const UTF8Char*)"<font color=\"#ff0000\">", 22);
								}
								Text::XML::ToXMLText(sbuff2, sptr2);
								sbOut.Append(sbuff2);
								if (cnt > 0)
								{
									sbOut.AppendC((const UTF8Char*)"</font>", 7);
								}
								sbOut.AppendC((const UTF8Char*)"</a></td><td>", 13);
								if (pt == IO::Path::PT_DIRECTORY)
								{
									sbOut.AppendC((const UTF8Char*)"Directory", 9);
									sbOut.AppendC((const UTF8Char*)"</td><td>", 9);
									sbOut.AppendChar('-', 1);
								}
								else
								{
									IO::Path::GetFileExt(sbuff2, sptr2);
									sbOut.Append(Net::MIME::GetMIMEFromExt(sbuff2));
									sbOut.AppendC((const UTF8Char*)"</td><td>", 9);
									sbOut.AppendU64(fileSize);
								}
								if (this->statMap)
								{
									sbOut.AppendC((const UTF8Char*)"</td><td>", 9);
									sbOut.AppendI32(cnt);
								}
								sbOut.AppendC((const UTF8Char*)"</td><td>", 9);
								sbOut.AppendDate(&modTime);
								sbOut.AppendC((const UTF8Char*)"</td></tr>\r\n", 12);
							}
						}
						IO::Path::FindFileClose(sess);
						if (stat)
						{
							mutUsage.EndUse();
						}
					}
					else
					{
						Data::ArrayList<DirectoryEntry *> entList;
						DirectoryEntry *ent;
						while (IO::Path::FindNextFile(sptr2, sess, &modTime, &pt, &fileSize))
						{
							if (Text::StrEquals(sptr2, (const UTF8Char*)".") || Text::StrEquals(sptr2, (const UTF8Char*)".."))
							{
							}
							else
							{
								ent = MemAlloc(DirectoryEntry, 1);
								if (stat)
								{
									ent->cnt = stat->cntMap->Get(sptr2);
								}
								else
								{
									ent->cnt = 0;
								}
								ent->fileName = Text::StrCopyNew(sptr2);
								ent->fileSize = fileSize;
								ent->pt = pt;
								ent->modTime = modTime.ToTicks();
								entList.Add(ent);
							}
						}
						IO::Path::FindFileClose(sess);
						if (stat)
						{
							mutUsage.EndUse();
						}

						UOSInt i;
						UOSInt j;
						DirectoryEntry **entArr = entList.GetArray(&j);
						if (sort == 1)
						{
							ArtificialQuickSort_SortCmp((void**)entArr, HTTPDirectoryHandler_CompareFuncName, 0, (OSInt)j - 1);
						}
						else if (sort == 2)
						{
							ArtificialQuickSort_SortCmp((void**)entArr, HTTPDirectoryHandler_CompareFuncSize, 0, (OSInt)j - 1);
						}
						else if (sort == 3)
						{
							ArtificialQuickSort_SortCmp((void**)entArr, HTTPDirectoryHandler_CompareFuncCount, 0, (OSInt)j - 1);
						}

						i = 0;
						while (i < j)
						{
							ent = entList.GetItem(i);
							sbOut.Append((const UTF8Char*)"<tr><td>");
							sbOut.Append((const UTF8Char*)"<a href=\"");
							Text::TextEnc::URIEncoding::URIEncode(sbuff2, ent->fileName);
							sbOut.Append(sbuff2);
							if (ent->pt == IO::Path::PT_DIRECTORY)
							{
								sbOut.Append((const UTF8Char*)"/");
							}
							sbOut.Append((const UTF8Char*)"\">");
							if (ent->cnt > 0)
							{
								sbOut.Append((const UTF8Char*)"<font color=\"#ff0000\">");
							}
							Text::XML::ToXMLText(sbuff2, ent->fileName);
							sbOut.Append(sbuff2);
							if (ent->cnt > 0)
							{
								sbOut.Append((const UTF8Char*)"</font>");
							}
							sbOut.Append((const UTF8Char*)"</a></td><td>");
							if (ent->pt == IO::Path::PT_DIRECTORY)
							{
								sbOut.Append((const UTF8Char*)"Directory");
								sbOut.Append((const UTF8Char*)"</td><td>");
								sbOut.Append((const UTF8Char*)"-");
							}
							else
							{
								IO::Path::GetFileExt(sbuff2, ent->fileName);
								sbOut.Append(Net::MIME::GetMIMEFromExt(sbuff2));
								sbOut.Append((const UTF8Char*)"</td><td>");
								sbOut.AppendI64(ent->fileSize);
							}
							if (this->statMap)
							{
								sbOut.Append((const UTF8Char*)"</td><td>");
								sbOut.AppendI32(ent->cnt);
							}
							sbOut.Append((const UTF8Char*)"</td><td>");
							modTime.SetTicks(ent->modTime);
							sbOut.AppendDate(&modTime);
							sbOut.Append((const UTF8Char*)"</td></tr>\r\n");
							Text::StrDelNew(ent->fileName);
							MemFree(ent);
							i++;
						}
					}
				}
				sbOut.AppendC((const UTF8Char*)"</table></body></html>", 22);

				Net::WebServer::HTTPServerUtil::SendContent(req, resp, (const UTF8Char*)"text/html", sbOut.GetLength(), sbOut.ToString());
				return true;
			}
			else
			{
				resp->ResponseError(req, Net::WebStatus::SC_FORBIDDEN);
				return true;
			}
		}
	}
	else if (pt == IO::Path::PT_FILE)
	{
		Text::StringBuilderUTF8 sb2;
		IO::FileStream *fs;
		UInt64 sizeLeft;

		NEW_CLASS(fs, IO::FileStream(sptr, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_SEQUENTIAL));
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
				AddCacheHeader(resp);
				resp->AddContentLength(0);
				resp->Write(buff, 0);
				DEL_CLASS(fs);
				return true;
			}
		}

		if (this->statMap)
		{
			sb2.ClearStr();
			sb2.Append(subReq);
			i = sb2.IndexOf('?');
			if (i >= 0)
			{
				sb2.TrimToLength((UOSInt)i);
			}
			i = sb2.LastIndexOf('/');
			sptr2 = sb2.ToString() + i + 1;
			sb2.ToString()[i] = 0;
			Sync::MutexUsage mutUsage(this->statMut);
			Net::WebServer::HTTPDirectoryHandler::StatInfo *stat;
			if (sb2.ToString()[0] == 0)
			{
				stat = this->statMap->Get((const UTF8Char*)"/");
			}
			else
			{
				stat = this->statMap->Get(sb2.ToString());
			}
			if (stat == 0)
			{
				stat = MemAlloc(Net::WebServer::HTTPDirectoryHandler::StatInfo, 1);
				if (sb2.ToString()[0] == 0)
				{
					stat->reqPath = Text::StrCopyNew((const UTF8Char*)"/");
				}
				else
				{
					stat->reqPath = Text::StrCopyNew(sb2.ToString());
				}
				stat->reqPath = Text::StrCopyNew(sb2.ToString());
				NEW_CLASS(stat->cntMap, Data::StringUTF8Map<Int32>());
				stat->updated = true;
				Text::StrConcat(sbuff, sptr);
				i = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
				Text::StrConcat(&sbuff[i + 1], (const UTF8Char*)".counts");
				stat->statFileName = Text::StrCopyNew(sbuff);
				this->statMap->Put(stat->reqPath, stat);
				this->StatLoad(stat);
			}
			stat->cntMap->Put(sptr2, stat->cntMap->Get(sptr2) + 1);
			stat->updated = true;
			mutUsage.EndUse();
		}

		IO::Path::GetFileExt(sbuff, sptr);

		Bool partial = false;
		sizeLeft = fs->GetLength();
		sb2.ClearStr();
		if (req->GetHeader(&sb2, (const UTF8Char*)"Range"))
		{
			UInt64 fileSize = sizeLeft;
			if (!sb2.StartsWith((const UTF8Char*)"bytes="))
			{
				resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
				resp->AddDefHeaders(req);
				AddCacheHeader(resp);
				resp->AddContentLength(0);
				resp->Write(buff, 0);
				DEL_CLASS(fs);
				return true;
			}
			if (sb2.IndexOf(',') >= 0)
			{
				resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
				resp->AddDefHeaders(req);
				AddCacheHeader(resp);
				resp->AddContentLength(0);
				resp->Write(buff, 0);
				DEL_CLASS(fs);
				return true;
			}
			Int64 start = 0;
			Int64 end = -1;
			i = sb2.IndexOf('-');
			if (i < 0)
			{
				resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
				resp->AddDefHeaders(req);
				AddCacheHeader(resp);
				resp->AddContentLength(0);
				resp->Write(buff, 0);
				DEL_CLASS(fs);
				return true;
			}
			sptr = sb2.ToString();
			sptr[i] = 0;
			if (!Text::StrToInt64(&sptr[6], &start))
			{
				resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
				resp->AddDefHeaders(req);
				AddCacheHeader(resp);
				resp->AddContentLength(0);
				resp->Write(buff, 0);
				DEL_CLASS(fs);
				return true;
			}
			if ((UOSInt)(i + 1) < sb2.GetLength())
			{
				if (!Text::StrToInt64(&sptr[i + 1], &end))
				{
					resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
					resp->AddDefHeaders(req);
					AddCacheHeader(resp);
					resp->AddContentLength(0);
					resp->Write(buff, 0);
					DEL_CLASS(fs);
					return true;
				}
				if (end <= start || (UInt64)end > sizeLeft)
				{
					resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
					resp->AddDefHeaders(req);
					AddCacheHeader(resp);
					resp->AddContentLength(0);
					resp->Write(buff, 0);
					DEL_CLASS(fs);
					return true;
				}
				sizeLeft = (UInt64)(end - start);
			}
			else
			{
				sizeLeft = sizeLeft - (UInt64)start;
			}
			fs->Seek(IO::SeekableStream::ST_BEGIN, start);
			resp->SetStatusCode(Net::WebStatus::SC_PARTIAL_CONTENT);
			UTF8Char u8buff[128];
			UTF8Char *u8ptr;
			u8ptr = Text::StrConcat(u8buff, (const UTF8Char*)"bytes ");
			u8ptr = Text::StrInt64(u8ptr, start);
			*u8ptr++ = '-';
			u8ptr = Text::StrUInt64(u8ptr, (UInt64)start + sizeLeft - 1);
			*u8ptr++ = '/';
			u8ptr = Text::StrUInt64(u8ptr, fileSize);
			resp->AddHeader((const UTF8Char*)"Content-Range", u8buff);
			partial = true;
		}
		resp->AddDefHeaders(req);
		AddCacheHeader(resp);
		resp->AddLastModified(&t);
		resp->AddContentType(mime = Net::MIME::GetMIMEFromExt(sbuff));
		resp->AddHeader((const UTF8Char*)"Accept-Ranges", (const UTF8Char*)"bytes");
		if (sizeLeft <= 0)
		{
			UOSInt readSize;
			readSize = fs->Read(buff, 2048);
			if (readSize == 0)
			{
				Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, sizeLeft, fs);
			}
			else
			{
				IO::MemoryStream *mstm;
				NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"Net.WebServer.HTTPDirectoryHandler.ProcessRequest.mstm"));
				while (readSize > 0)
				{
					sizeLeft += mstm->Write(buff, readSize);
					readSize = fs->Read(buff, 2048);
				}
				mstm->Seek(IO::SeekableStream::ST_BEGIN, 0);
				Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, sizeLeft, mstm);
				DEL_CLASS(mstm);
			}
		}
		else if (!partial && sizeLeft < this->fileCacheSize)
		{
			UOSInt readSize;
			cache = MemAlloc(CacheInfo, 1);
			cache->buff = MemAlloc(UInt8, (UOSInt)sizeLeft);
			cache->buffSize = (UOSInt)sizeLeft;
			cache->t = t.ToTicks();
			readSize = fs->Read(cache->buff, (UOSInt)sizeLeft);
			if (readSize == sizeLeft)
			{
				Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, sizeLeft, cache->buff);
				Sync::MutexUsage mutUsage(this->fileCacheMut);
				cache = this->fileCache->Put(subReq, cache);
				mutUsage.EndUse();
				if (cache)
				{
					MemFree(cache->buff);
					MemFree(cache);
				}
			}
			else
			{
				//this->SendContent(req, resp, mime, readSize, cache->buff);
				resp->AddContentLength(sizeLeft);
				resp->Write(cache->buff, readSize);
				MemFree(cache->buff);
				MemFree(cache);
			}
		}
		else
		{
			Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, sizeLeft, fs);
		}
		DEL_CLASS(fs);
		return true;
	}
	return false;
}

void Net::WebServer::HTTPDirectoryHandler::SetCacheType(CacheType ctype)
{
	this->ctype = ctype;
}

void Net::WebServer::HTTPDirectoryHandler::SetExpirePeriod(Int32 period)
{
	this->expirePeriod = period;
}

void Net::WebServer::HTTPDirectoryHandler::SetAllowOrigin(const UTF8Char *origin)
{
	SDEL_TEXT(this->allowOrigin);
	if (origin)
	{
		this->allowOrigin = Text::StrCopyNew(origin);
	}
}

void Net::WebServer::HTTPDirectoryHandler::ClearFileCache()
{
	UOSInt cacheCnt;
	Sync::MutexUsage mutUsage(this->fileCacheMut);
	while (this->fileCacheUsing > 0)
	{
		Sync::Thread::Sleep(1);
	}
	CacheInfo **cacheList = this->fileCache->ToArray(&cacheCnt);
	while (cacheCnt-- > 0)
	{
		MemFree(cacheList[cacheCnt]->buff);
		MemFree(cacheList[cacheCnt]);
	}
	MemFree(cacheList);
	this->fileCache->Clear();
	mutUsage.EndUse();
}

void Net::WebServer::HTTPDirectoryHandler::ExpandPackageFiles(Parser::ParserList *parsers)
{
	if (this->packageMap)
	{
		return;
	}
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::Path::FindFileSession *sess;
	NEW_CLASS(this->packageMut, Sync::RWMutex());
	this->packageMut->LockWrite();
	NEW_CLASS(this->packageMap, Data::StringUTF8Map<PackageInfo*>());
	sptr = Text::StrConcat(sbuff, this->rootDir);
	if (sptr[-1] == '/' || sptr[-1] == '\\')
	{
	}
	else
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	Text::StrConcat(sptr, (const UTF8Char*)"*.spk");
	sess = IO::Path::FindFile(sbuff);
	if (sess)
	{
		Data::DateTime dt;
		IO::Path::PathType pt;
		IO::StmData::FileData *fd;
		IO::PackageFile *pf;
		OSInt i;
		PackageInfo *package;

		while (IO::Path::FindNextFile(sptr, sess, &dt, &pt, 0))
		{
			if (pt == IO::Path::PT_FILE)
			{
				NEW_CLASS(fd, IO::StmData::FileData(sbuff, false));
				pf = (IO::PackageFile*)parsers->ParseFileType(fd, IO::ParsedObject::PT_PACKAGE_PARSER);
				DEL_CLASS(fd);
				if (pf)
				{
					package = MemAlloc(PackageInfo, 1);
					package->packageFile = pf;
					package->modTime = dt.ToTicks();
					i = Text::StrLastIndexOf(sptr, '.');
					if (i > 0)
					{
						sptr[i] = 0;
					}
					package->fileName = Text::StrCopyNew(sptr);
					this->packageMap->Put(package->fileName, package);
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}

	this->packageMut->UnlockWrite();
}

void Net::WebServer::HTTPDirectoryHandler::EnableStats()
{
	if (this->statMap == 0)
	{
		NEW_CLASS(this->statMut, Sync::Mutex());
		NEW_CLASS(this->statMap, Data::StringUTF8Map<Net::WebServer::HTTPDirectoryHandler::StatInfo*>());
	}
}

void Net::WebServer::HTTPDirectoryHandler::SaveStats()
{
	if (this->statMap == 0)
	{
		Data::ArrayList<Net::WebServer::HTTPDirectoryHandler::StatInfo*> *statList;
		Net::WebServer::HTTPDirectoryHandler::StatInfo *stat;
		UOSInt i;
		Sync::MutexUsage mutUsage(this->statMut);
		statList = this->statMap->GetValues();
		i = statList->GetCount();
		while (i-- > 0)
		{
			stat = statList->GetItem(i);
			if (stat->updated)
			{
				this->StatSave(stat);
			}
		}
		mutUsage.EndUse();
	}
}
