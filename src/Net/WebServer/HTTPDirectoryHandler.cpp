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
#include "Text/TextBinEnc/URIEncoding.h"

typedef struct
{
	Text::String *fileName;
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
		if (ent1->pt == IO::Path::PathType::Directory)
		{
			return -1;
		}
		else if (ent2->pt == IO::Path::PathType::Directory)
		{
			return 1;
		}
	}
	OSInt ret = Text::StrCompareICase(ent1->fileName->v, ent2->fileName->v);
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
		return Text::StrCompareICase(ent1->fileName->v, ent2->fileName->v);
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
		resp->AddHeader(CSTR("Cache-Control"), CSTR("public"));
		break;
	case CT_PRIVATE:
		resp->AddHeader(CSTR("Cache-Control"), CSTR("private"));
		break;
	case CT_NO_CACHE:
		resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
		break;
	case CT_DEFAULT:
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
		resp->AddHeader(CSTR("Access-Control-Allow-Origin"), this->allowOrigin->ToCString());
	}
}

void Net::WebServer::HTTPDirectoryHandler::ResponsePackageFile(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, IO::PackageFile *packageFile)
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

	sb2.ClearStr();
	sb2.Append(req->GetRequestURI());
	sptr = sb2.ToString();
	i = Text::StrIndexOfCharC(sptr, sb2.GetLength(), '?');
	if (i != INVALID_INDEX)
	{
		sptr[i] = 0;
	}
	else
	{
		i = sb2.GetLength();
	}
/*	if (sptr[i - 1] != '/')
	{
		sb.Append(IO::Path::PATH_SEPERATOR, 1);
	}*/
/*	if (this->allowUpload && req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
	{
		const UInt8 *uplfile;
		OSInt uplSize;
		req->ParseHTTPForm();
		uplfile = req->GetHTTPFormFile(L"uploadfile", sbuff, &uplSize);
		if (uplfile)
		{
			Text::StringBuilder sbTmp;
			sbTmp.AppendC(sb.ToString(), sb.GetLength());
			sbTmp.Append(sbuff);
			if (IO::Path::GetPathType(sbTmp.ToString()) == IO::Path::PathType::Unknown)
			{
				IO::FileStream *uplFS;
				NEW_CLASS(uplFS, IO::FileStream(sbTmp.ToString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
				uplFS->Write(uplfile, uplSize);
				DEL_CLASS(uplFS);
			}
		}
	}*/

	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html; charset=UTF-8"));
	AddCacheHeader(resp);

	UTF8Char sbuff[512];
	Text::String *s;

	sbOut.AppendNE(UTF8STRC("<html><head><title>Index of "));
	Text::TextBinEnc::URIEncoding::URIDecode(sbuff, sptr);
	s = Text::XML::ToNewHTMLText(sbuff);
	sbOut.AppendNE(s);
	sbOut.AppendNE(UTF8STRC("</title></head>\r\n<body>\r\n"));
	sbOut.AppendNE(UTF8STRC("<h2>Index Of "));
	sbOut.AppendNE(s);
	s->Release();
	sbOut.AppendNE(UTF8STRC("</h2>\r\n"));
	sbOut.AppendNE(UTF8STRC("<a href=\"..\">Up one level</a><br/>\r\n"));
/*	if (this->allowUpload)
	{
		const WChar *csptr = Text::XML::ToNewAttrText(sptr);
		sbOut.AppendC(UTF8STRC("<form name=\"upload\" method=\"POST\" action="));
		sbOut.Append(csptr);
		sbOut.AppendC(UTF8STRC(" enctype=\"multipart/form-data\">"));
		Text::XML::FreeNewText(csptr);
		sbOut.AppendC(UTF8STRC("Upload: <input type=\"file\" name=\"uploadfile\"/><br/><input type=\"submit\"/>"));
		sbOut.AppendC(UTF8STRC("</form>"));
	}*/
	sbOut.AppendNE(UTF8STRC("<table><tr><th>Name</th><th>MIME</th><th>Size</th><th>Modified Date</th></tr>\r\n"));

	UTF8Char sbuff2[256];
	UTF8Char *sptr2;
	Data::DateTime modTime;
	IO::PackageFile::PackObjectType pot;
	i = 0;
	j = packageFile->GetCount();
	while (i < j)
	{
		pot = packageFile->GetItemType(i);
		if (pot == IO::PackageFile::POT_STREAMDATA || pot == IO::PackageFile::POT_PACKAGEFILE)
		{
			sbOut.AppendNE(UTF8STRC("<tr><td>"));
			sbOut.AppendNE(UTF8STRC("<a href=\""));
			sptr = packageFile->GetItemName(sbuff, i);
			sptr2 = Text::TextBinEnc::URIEncoding::URIEncode(sbuff2, sbuff);
			sbOut.AppendNE(sbuff2, (UOSInt)(sptr2 - sbuff2));
			if (pot == IO::PackageFile::POT_PACKAGEFILE)
			{
				sbOut.AppendNE(UTF8STRC("/"));
			}
			sbOut.AppendNE(UTF8STRC("\">"));
			sptr2 = Text::XML::ToXMLText(sbuff2, sbuff);
			sbOut.AppendNE(sbuff2, (UOSInt)(sptr2 - sbuff2));
			sbOut.AppendNE(UTF8STRC("</a></td><td>"));
			if (pot == IO::PackageFile::POT_PACKAGEFILE)
			{
				sbOut.AppendNE(UTF8STRC("Directory"));
				sbOut.AppendNE(UTF8STRC("</td><td>"));
				sbOut.AppendNE(UTF8STRC("-"));
			}
			else
			{
				sptr2 = IO::Path::GetFileExt(sbuff2, sbuff, (UOSInt)(sptr - sbuff));
				Text::CString mime = Net::MIME::GetMIMEFromExt(CSTRP(sbuff2, sptr2));
				sbOut.AppendNE(mime.v, mime.leng);
				sbOut.AppendNE(UTF8STRC("</td><td>"));
				sbOut.AppendU64(packageFile->GetItemSize(i));
			}

			modTime.SetTicks(packageFile->GetItemModTimeTick(i));
			sbOut.AppendNE(UTF8STRC("</td><td>"));
			sbOut.AppendDate(&modTime);
			sbOut.AppendNE(UTF8STRC("</td></tr>\r\n"));
		}
		
		i++;
	}
	sbOut.AppendC(UTF8STRC("</table></body></html>"));
	Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("text/html"), sbOut.GetLength(), sbOut.ToString());
	return ;
}

void Net::WebServer::HTTPDirectoryHandler::StatLoad(Net::WebServer::HTTPDirectoryHandler::StatInfo *stat)
{
	IO::FileStream *fs;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(fs, IO::FileStream(stat->statFileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!fs->IsError())
	{
		Text::PString sarr[2];
		Text::UTF8Reader *reader;
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		sb.ClearStr();
		while (reader->ReadLine(&sb, 1024))
		{
			if (Text::StrSplitP(sarr, 2, sb, '\t') == 2)
			{
				stat->cntMap->PutC(sarr[1].ToCString(), Text::StrToUInt32(sarr[0].v));
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
	stat->updated = false;
	if (stat->cntMap->GetCount() > 0)
	{
		NEW_CLASS(fs, IO::FileStream(stat->statFileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		if (!fs->IsError())
		{
			Text::UTF8Writer *writer;
			NEW_CLASS(writer, Text::UTF8Writer(fs));
			UOSInt i;
			UOSInt j;
			i = 0;
			j = stat->cntMap->GetCount();
			while (i < j)
			{
				sb.ClearStr();
				sb.AppendU32(stat->cntMap->GetItem(i));
				sb.AppendUTF8Char('\t');
				sb.Append(stat->cntMap->GetKey(i));
				writer->WriteLineC(sb.ToString(), sb.GetLength());
				i++;
			}
			DEL_CLASS(writer);
		}
		DEL_CLASS(fs);
	}
}

Net::WebServer::HTTPDirectoryHandler::HTTPDirectoryHandler(Text::String *rootDir, Bool allowBrowsing, UInt64 fileCacheSize, Bool allowUpload)
{
	this->rootDir = rootDir->Clone();
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

Net::WebServer::HTTPDirectoryHandler::HTTPDirectoryHandler(Text::CString rootDir, Bool allowBrowsing, UInt64 fileCacheSize, Bool allowUpload)
{
	this->rootDir = Text::String::New(rootDir);
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
	this->rootDir->Release();
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
		PackageInfo *package;
		UOSInt i;
		i = this->packageMap->GetCount();
		while (i-- > 0)
		{
			package = this->packageMap->GetItem(i);
			package->fileName->Release();
			DEL_CLASS(package->packageFile);
			MemFree(package);
		}
		DEL_CLASS(this->packageMap);
		DEL_CLASS(this->packageMut);
	}
	SDEL_STRING(this->allowOrigin);
	if (this->statMap)
	{
		Net::WebServer::HTTPDirectoryHandler::StatInfo *stat;
		UOSInt i = this->statMap->GetCount();
		while (i-- > 0)
		{
			stat = this->statMap->GetItem(i);
			this->StatSave(stat);

			stat->reqPath->Release();
			stat->statFileName->Release();
			DEL_CLASS(stat->cntMap);
			MemFree(stat);
		}
		DEL_CLASS(this->statMap);
		DEL_CLASS(this->statMut);
	}
}

Bool Net::WebServer::HTTPDirectoryHandler::ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq)
{
	UInt8 buff[2048];
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[1024];
	UTF8Char sbuff2[1024];
	UTF8Char *sptr;
	UOSInt sptrLen;
	UTF8Char *sptr3;
	UTF8Char *sptr4;
	Data::DateTime t;
	Text::CString mime;
	UOSInt i;
	if (this->DoRequest(req, resp, subReq))
	{
		return true;
	}

	if (req->GetProtocol() != Net::WebServer::IWebRequest::RequestProtocol::HTTP1_0 && req->GetProtocol() != Net::WebServer::IWebRequest::RequestProtocol::HTTP1_1)
	{
		resp->ResponseError(req, Net::WebStatus::SC_METHOD_NOT_ALLOWED);
		return true;
	}
	if (this->packageMap)
	{
		i = subReq.IndexOf('?');
		if (i != INVALID_INDEX)
		{
			sb.ClearStr();
			sb.AppendC(subReq.v, (UOSInt)i);
		}
		else
		{
			sb.Append(subReq);
		}
		i = Text::StrIndexOfCharC(&sb.ToString()[1], sb.GetLength() - 1, '/');
		if (i != INVALID_INDEX)
		{
			sb.ToString()[i + 1] = 0;
		}
		PackageInfo *package;
		this->packageMut->LockRead();
		package = this->packageMap->GetC(sb.ToCString().Substring(1));
		this->packageMut->UnlockRead();
		if (package)
		{
			if (i == INVALID_INDEX)
			{
				ResponsePackageFile(req, resp, subReq, package->packageFile);
				return true;
			}
			
			sptr = &sb.ToString()[i + 2];
			if (sptr[0] == 0)
			{
				ResponsePackageFile(req, resp, subReq, package->packageFile);
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
						sptr3 = IO::Path::GetFileExt(sbuff, sptr, sb.GetLength() - i - 2);
						mime = Net::MIME::GetMIMEFromExt(CSTRP(sbuff, sptr3));

						resp->EnableWriteBuffer();
						resp->AddDefHeaders(req);
						t.SetTicks(pitem->modTimeTick);
						t.ToLocalTime();
						resp->AddLastModified(&t);
						if (this->allowOrigin)
						{
							resp->AddHeader(CSTR("Access-Control-Allow-Origin"), this->allowOrigin->ToCString());
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
								mime = Net::MIME::GetMIMEFromExt(CSTR("html"));

								resp->EnableWriteBuffer();
								resp->AddDefHeaders(req);
								t.SetTicks(pitem2->modTimeTick);
								t.ToLocalTime();
								resp->AddLastModified(&t);
								if (this->allowOrigin)
								{
									resp->AddHeader(CSTR("Access-Control-Allow-Origin"), this->allowOrigin->ToCString());
								}
								resp->AddContentType(mime);
								Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, dataLen, dataBuff);
								MemFree(dataBuff);
							}
							else
							{
								ResponsePackageFile(req, resp, subReq, innerPF);
							}
						}
						else
						{
							ResponsePackageFile(req, resp, subReq, innerPF);
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
			sb.TrimToLength(i);
			Sync::MutexUsage statMutUsage(this->statMut);
			stat = this->statMap->GetC(sb.ToCString());
			if (stat)
			{
				sb.Append(subReq.Substring(i + 1));
				i = sb.IndexOf('?');
				if (i != INVALID_INDEX)
				{
					sb.TrimToLength(i);
				}
				if (sb.ToString()[0] == 0)
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("index.html"));
				}
				UInt32 cnt = stat->cntMap->GetC(sb.ToCString());
				stat->cntMap->PutC(sb.ToCString(), cnt + 1);
				stat->updated = true;
			}
			statMutUsage.EndUse();
		}
		resp->EnableWriteBuffer();
		resp->AddDefHeaders(req);
		AddCacheHeader(resp);
		t.SetTicks(cache->t);
		t.ToLocalTime();
		resp->AddLastModified(&t);
		i = subReq.IndexOf('?');
		if (i != INVALID_INDEX)
		{
			Text::StringBuilderUTF8 sbc;
			sbc.AppendC(subReq.v, (UOSInt)i);
			if (sbc.EndsWith('/'))
			{
				mime = Net::MIME::GetMIMEFromExt(CSTR("html"));
			}
			else
			{
				i = Text::StrLastIndexOfCharC(sbc.ToString(), sbc.GetLength(), '.');
				mime = Net::MIME::GetMIMEFromExt(sbc.ToCString().Substring(i + 1));
			}
		}
		else
		{
			if (subReq.EndsWith('/'))
			{
				mime = Net::MIME::GetMIMEFromExt(CSTR("html"));
			}
			else
			{
				i = subReq.LastIndexOf('.');
				mime = Net::MIME::GetMIMEFromExt(subReq.Substring(i + 1));
			}
		}
		resp->AddContentType(mime);
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
	const UTF8Char *reqTarget = subReq.v;
	sb.AppendC(reqTarget, subReq.leng);
	sptr = sb.ToString();
	sptrLen = sb.GetLength();
	UTF8Char *sptr2 = 0;
	i = Text::StrIndexOfCharC(sptr, sptrLen, '?');
	if (i != INVALID_INDEX)
	{
		sptr2 = &sptr[i + 1];
		sptr2[-1] = 0;
		sptrLen = i;
	}
	if (IO::Path::PATH_SEPERATOR != '/')
	{
		Text::StrReplace(sptr, '/', IO::Path::PATH_SEPERATOR);
	}

	IO::Path::PathType pt = IO::Path::GetPathType({sptr, sptrLen});
	if (pt == IO::Path::PathType::Unknown)
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}
	else if (pt == IO::Path::PathType::Directory)
	{
		Text::StringBuilderUTF8 sb2;
		sb2.AppendC(sb.ToString(), sb.GetLength());
		if (sb.EndsWith(IO::Path::PATH_SEPERATOR))
		{
			sb2.AppendC(UTF8STRC("index.html"));
		}
		else
		{
			sb2.AppendChar(IO::Path::PATH_SEPERATOR, 1);
			sb2.AppendC(UTF8STRC("index.html"));
		}
		pt = IO::Path::GetPathType(sb2.ToCString());
		if (pt == IO::Path::PathType::File)
		{
			sb.ClearStr();
			sb.AppendC(sb2.ToString(), sb2.GetLength());
			sptr = sb.ToString();
			IO::FileStream *fs;
			UInt64 sizeLeft;
			Text::String *hdrVal;

			sptr3 = IO::Path::GetFileExt(sbuff, sptr, sptrLen);
			NEW_CLASS(fs, IO::FileStream({sptr, sptrLen}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
			fs->GetFileTimes(0, 0, &t);

			if ((hdrVal = req->GetSHeader(CSTR("If-Modified-Since"))) != 0)
			{
				Data::DateTime t2;
				t2.SetValue(hdrVal->ToCString());
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

			resp->EnableWriteBuffer();
			resp->AddDefHeaders(req);
			AddCacheHeader(resp);
			resp->AddLastModified(&t);
			mime = Net::MIME::GetMIMEFromExt(CSTRP(sbuff, sptr3));
			resp->AddContentType(mime);
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
				if (i != INVALID_INDEX)
				{
					sb2.TrimToLength(i);
				}
				if (!sb2.EndsWith('/'))
				{
					sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
				}
				if (this->allowUpload && req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
				{
					const UInt8 *uplfile;
					UOSInt uplSize;
					UOSInt fileId;
					req->ParseHTTPForm();
					fileId = 0;
					while ((uplfile = req->GetHTTPFormFile(CSTR("uploadfile"), fileId, (UTF8Char*)buff, sizeof(buff), &sptr2, &uplSize)) != 0)
					{
						Text::StringBuilderUTF8 sbTmp;
						sbTmp.AppendC(sb.ToString(), sb.GetLength());
						sbTmp.AppendP((UTF8Char*)buff, sptr2);
						if (IO::Path::GetPathType(sbTmp.ToCString()) == IO::Path::PathType::Unknown)
						{
							IO::FileStream *uplFS;
							NEW_CLASS(uplFS, IO::FileStream(sbTmp.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
							uplFS->Write(uplfile, uplSize);
							DEL_CLASS(uplFS);
						}
						fileId++;
					}
				}

				resp->EnableWriteBuffer();
				resp->AddDefHeaders(req);
				resp->AddContentType(CSTR("text/html; charset=UTF-8"));
				AddCacheHeader(resp);

				Bool isRoot = false;
				Text::String *s;
				sbOut.AppendNE(UTF8STRC("<html><head><title>Index of "));
				Text::TextBinEnc::URIEncoding::URIDecode(sbuff, sb2.ToString());
				s = Text::XML::ToNewHTMLText(sbuff);
				sbOut.AppendNE(s);
				sbOut.AppendNE2(UTF8STRC("</title></head>\r\n<body>\r\n"),
							   UTF8STRC("<h2>Index Of "));
				sbOut.AppendNE(s);
				s->Release();
				sbOut.AppendNE(UTF8STRC("</h2>\r\n"));
				if (!sb2.Equals(UTF8STRC("/")))
				{
					sbOut.AppendNE(UTF8STRC("<a href=\"..\">Up one level</a><br/>\r\n"));
				}
				else
				{
					isRoot = true;
				}
				if (this->allowUpload)
				{
					s = Text::XML::ToNewAttrText(sb2.ToString());
					sbOut.AppendNE(UTF8STRC("<form name=\"upload\" method=\"POST\" action="));
					sbOut.AppendNE(s);
					sbOut.AppendNE(UTF8STRC(" enctype=\"multipart/form-data\">"));
					s->Release();
					sbOut.AppendNE2(UTF8STRC("Upload: <input type=\"file\" name=\"uploadfile\" multiple/><br/><input type=\"submit\"/>"),
								   UTF8STRC("</form>"));
				}

				s = req->GetQueryValue(CSTR("sort"));
				if (s)
				{
					sort = s->ToInt32();
				}

				Text::StringBuilderUTF8 sb3;
				sb3.AppendNE(sb2.ToString(), sb2.GetLength());
				sb3.AppendC(UTF8STRC("?sort=1"));
				s = Text::XML::ToNewAttrText(sb3.ToString());
				sbOut.AppendNE(UTF8STRC("<table><tr><th><a href="));
				sbOut.AppendNE(s);
				s->Release();
				sbOut.AppendNE(UTF8STRC(">Name</a></th><th>MIME</th><th><a href="));
				sb3.ClearStr();
				sb3.AppendC2(sb2.ToString(), sb2.GetLength(), UTF8STRC("?sort=2"));
				s = Text::XML::ToNewAttrText(sb3.ToString());
				sbOut.AppendNE(s);
				s->Release();
				sbOut.AppendNE(UTF8STRC(">File Size</a></th>"));
				if (this->statMap)
				{
					sbOut.AppendNE(UTF8STRC("<th><a href="));
					sb3.ClearStr();
					sb3.AppendC2(sb2.ToString(), sb2.GetLength(), UTF8STRC("?sort=3"));
					s = Text::XML::ToNewAttrText(sb3.ToString());
					sbOut.AppendNE(s);
					s->Release();
					sbOut.AppendNE(UTF8STRC(">Download Count</a></th>"));
				}
				sbOut.AppendNE(UTF8STRC("<th>Modified Date</th></tr>\r\n"));

				if (isRoot)
				{
					Data::DateTime modTime;
					if (this->packageMap)
					{
						PackageInfo *package;
						UOSInt i;
						UOSInt j;
						this->packageMut->LockRead();
						i = 0;
						j = this->packageMap->GetCount();
						while (i < j)
						{
							package = this->packageMap->GetItem(i);
							sbOut.AppendNE(UTF8STRC("<tr><td><a href=\""));
							sptr2 = Text::TextBinEnc::URIEncoding::URIEncode(sbuff2, package->fileName->v);
							sbOut.AppendNE(sbuff2, (UOSInt)(sptr2 - sbuff2));
							sbOut.AppendUTF8Char('/');
							sbOut.AppendNE(UTF8STRC("\">"));
							sptr2 = Text::XML::ToXMLText(sbuff2, package->fileName->v);
							sbOut.AppendNE2(sbuff2, (UOSInt)(sptr2 - sbuff2), UTF8STRC("</a></td><td>"));
							sbOut.AppendNE2(UTF8STRC("Directory"), UTF8STRC("</td><td>"));
							sbOut.AppendUTF8Char('-');
							if (this->statMap)
							{
								sbOut.AppendNE(UTF8STRC("</td><td>0"));
							}
							modTime.SetTicks(package->modTime);
							sbOut.AppendNE(UTF8STRC("</td><td>"));
							sbOut.AppendDate(&modTime);
							sbOut.AppendNE(UTF8STRC("</td></tr>\r\n"));
							
							i++;
						}
						this->packageMut->UnlockRead();
					}
				}

				sptr2 = Text::StrConcatC(sbuff, sb.ToString(), sb.GetLength());
				sptr3 = Text::StrConcatC(sptr2, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
				IO::Path::FindFileSession *sess = IO::Path::FindFile(CSTRP(sbuff, sptr3));
				if (sess)
				{
					Sync::MutexUsage mutUsage(this->statMut);
					if (this->statMap)
					{
						if (sb2.GetLength() > 1 && sb2.EndsWith('/'))
						{
							sb2.RemoveChars(1);
						}
						stat = this->statMap->GetC(sb2.ToCString());
						if (stat == 0)
						{
							stat = MemAlloc(Net::WebServer::HTTPDirectoryHandler::StatInfo, 1);
							stat->reqPath = Text::String::New(sb2.ToString(), sb2.GetLength());
							NEW_CLASS(stat->cntMap, Data::FastStringMap<UInt32>());
							stat->updated = true;
							sb2.ClearStr();
							sb2.AppendC2(sb.ToString(), sb.GetLength(), UTF8STRC(".counts"));
							stat->statFileName = Text::String::New(sb2.ToString(), sb2.GetLength());
							this->statMap->Put(stat->reqPath, stat);
							this->StatLoad(stat);
						}
					}
					else
					{
						mutUsage.EndUse();
					}

					UInt32 cnt;
					UInt64 fileSize;
					IO::Path::PathType pt;
					Data::DateTime modTime;
					if (sort == 0)
					{
						while ((sptr3 = IO::Path::FindNextFile(sptr2, sess, &modTime, &pt, &fileSize)) != 0)
						{
							if (Text::StrEqualsC(sptr2, (UOSInt)(sptr3 - sptr2), UTF8STRC(".")) || Text::StrEqualsC(sptr2, (UOSInt)(sptr3 - sptr2), UTF8STRC("..")))
							{
							}
							else
							{
								if (stat)
								{
									cnt = stat->cntMap->GetC({sptr2, (UOSInt)(sptr3 - sptr2)});
								}
								else
								{
									cnt = 0;
								}
								sbOut.AppendNE(UTF8STRC("<tr><td><a href=\""));
								sptr4 = Text::TextBinEnc::URIEncoding::URIEncode(sbuff2, sptr2);
								sbOut.AppendNE(sbuff2, (UOSInt)(sptr4 - sbuff2));
								if (pt == IO::Path::PathType::Directory)
								{
									sbOut.AppendUTF8Char('/');
								}
								sbOut.AppendNE(UTF8STRC("\">"));
								if (cnt > 0)
								{
									sbOut.AppendNE(UTF8STRC("<font color=\"#ff0000\">"));
								}
								sptr4 = Text::XML::ToXMLText(sbuff2, sptr2);
								sbOut.AppendNE(sbuff2, (UOSInt)(sptr4 - sbuff2));
								if (cnt > 0)
								{
									sbOut.AppendNE(UTF8STRC("</font>"));
								}
								sbOut.AppendNE(UTF8STRC("</a></td><td>"));
								if (pt == IO::Path::PathType::Directory)
								{
									sbOut.AppendNE2(UTF8STRC("Directory"), UTF8STRC("</td><td>"));
									sbOut.AppendUTF8Char('-');
								}
								else
								{
									sptr4 = IO::Path::GetFileExt(sbuff2, sptr2, (UOSInt)(sptr3 - sptr2));
									mime = Net::MIME::GetMIMEFromExt(CSTRP(sbuff2, sptr4));
									sbOut.AppendNE2(mime.v, mime.leng, UTF8STRC("</td><td>"));
									sbOut.AppendU64(fileSize);
								}
								if (this->statMap)
								{
									sbOut.AppendNE(UTF8STRC("</td><td>"));
									sbOut.AppendU32(cnt);
								}
								sbOut.AppendNE(UTF8STRC("</td><td>"));
								sbOut.AppendDate(&modTime);
								sbOut.AppendNE(UTF8STRC("</td></tr>\r\n"));
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
						while ((sptr3 = IO::Path::FindNextFile(sptr2, sess, &modTime, &pt, &fileSize)) != 0)
						{
							if (Text::StrEqualsC(sptr2, (UOSInt)(sptr3 - sptr2), UTF8STRC(".")) || Text::StrEqualsC(sptr2, (UOSInt)(sptr3 - sptr2), UTF8STRC("..")))
							{
							}
							else
							{
								ent = MemAlloc(DirectoryEntry, 1);
								if (stat)
								{
									ent->cnt = stat->cntMap->GetC({sptr2, (UOSInt)(sptr3 - sptr2)});
								}
								else
								{
									ent->cnt = 0;
								}
								ent->fileName = Text::String::New(sptr2, (UOSInt)(sptr3 - sptr2));
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
							sbOut.AppendNE(UTF8STRC("<tr><td><a href=\""));
							sptr3 = Text::TextBinEnc::URIEncoding::URIEncode(sbuff2, ent->fileName->v);
							sbOut.AppendNE(sbuff2, (UOSInt)(sptr3 - sbuff2));
							if (ent->pt == IO::Path::PathType::Directory)
							{
								sbOut.AppendNE(UTF8STRC("/"));
							}
							sbOut.AppendNE(UTF8STRC("\">"));
							if (ent->cnt > 0)
							{
								sbOut.AppendNE(UTF8STRC("<font color=\"#ff0000\">"));
							}
							sptr3 = Text::XML::ToXMLText(sbuff2, ent->fileName->v);
							sbOut.AppendNE(sbuff2, (UOSInt)(sptr3 - sbuff2));
							if (ent->cnt > 0)
							{
								sbOut.AppendNE(UTF8STRC("</font>"));
							}
							sbOut.AppendNE(UTF8STRC("</a></td><td>"));
							if (ent->pt == IO::Path::PathType::Directory)
							{
								sbOut.AppendNE2(UTF8STRC("Directory"), UTF8STRC("</td><td>"));
								sbOut.AppendUTF8Char('-');
							}
							else
							{
								sptr3 = IO::Path::GetFileExt(sbuff2, ent->fileName->v, ent->fileName->leng);
								mime = Net::MIME::GetMIMEFromExt(CSTRP(sbuff2, sptr3));
								sbOut.AppendNE2(mime.v, mime.leng, UTF8STRC("</td><td>"));
								sbOut.AppendU64(ent->fileSize);
							}
							if (this->statMap)
							{
								sbOut.AppendNE(UTF8STRC("</td><td>"));
								sbOut.AppendU32(ent->cnt);
							}
							sbOut.AppendNE(UTF8STRC("</td><td>"));
							modTime.SetTicks(ent->modTime);
							sbOut.AppendDate(&modTime);
							sbOut.AppendNE(UTF8STRC("</td></tr>\r\n"));
							ent->fileName->Release();
							MemFree(ent);
							i++;
						}
					}
				}
				sbOut.AppendC(UTF8STRC("</table></body></html>"));

				Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("text/html"), sbOut.GetLength(), sbOut.ToString());
				return true;
			}
			else
			{
				resp->ResponseError(req, Net::WebStatus::SC_FORBIDDEN);
				return true;
			}
		}
	}
	else if (pt == IO::Path::PathType::File)
	{
		Text::StringBuilderUTF8 sb2;
		IO::FileStream *fs;
		UInt64 sizeLeft;
		Text::String *hdrVal;

		NEW_CLASS(fs, IO::FileStream({sptr, sptrLen}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
		fs->GetFileTimes(0, 0, &t);

		if ((hdrVal = req->GetSHeader(CSTR("If-Modified-Since"))) != 0)
		{
			Data::DateTime t2;
			t2.SetValue(hdrVal->ToCString());
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
			if (i != INVALID_INDEX)
			{
				sb2.TrimToLength(i);
			}
			i = sb2.LastIndexOf('/');
			sptr2 = sb2.ToString() + i + 1;
			sb2.ToString()[i] = 0;
			Sync::MutexUsage mutUsage(this->statMut);
			Net::WebServer::HTTPDirectoryHandler::StatInfo *stat;
			if (sb2.ToString()[0] == 0)
			{
				stat = this->statMap->GetC(CSTR("/"));
			}
			else
			{
				stat = this->statMap->GetC({sb2.ToString(), i});
			}
			if (stat == 0)
			{
				stat = MemAlloc(Net::WebServer::HTTPDirectoryHandler::StatInfo, 1);
				if (sb2.ToString()[0] == 0)
				{
					stat->reqPath = Text::String::New(UTF8STRC("/"));
				}
				else
				{
					stat->reqPath = Text::String::New(sb2.ToString(), i);
				}
				NEW_CLASS(stat->cntMap, Data::FastStringMap<UInt32>());
				stat->updated = true;
				sptr3 = Text::StrConcatC(sbuff, sptr, sptrLen);
				i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr3 - sbuff), IO::Path::PATH_SEPERATOR);
				sptr3 = Text::StrConcatC(&sbuff[i + 1], UTF8STRC(".counts"));
				stat->statFileName = Text::String::New(sbuff, (UOSInt)(sptr3 - sbuff));
				this->statMap->Put(stat->reqPath, stat);
				this->StatLoad(stat);
			}
			stat->cntMap->PutC({sptr2, sb2.GetLength() - i - 1}, stat->cntMap->GetC({sptr2, sb2.GetLength() - i - 1}) + 1);
			stat->updated = true;
			mutUsage.EndUse();
		}

		sptr3 = IO::Path::GetFileExt(sbuff, sptr, sptrLen);

		Bool partial = false;
		sizeLeft = fs->GetLength();
		sb2.ClearStr();
		if (req->GetHeaderC(&sb2, CSTR("Range")))
		{
			UInt64 fileSize = sizeLeft;
			if (!sb2.StartsWith(UTF8STRC("bytes=")))
			{
				resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
				resp->AddDefHeaders(req);
				AddCacheHeader(resp);
				resp->AddContentLength(0);
				resp->Write(buff, 0);
				DEL_CLASS(fs);
				return true;
			}
			if (sb2.IndexOf(',') != INVALID_INDEX)
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
			if (i == INVALID_INDEX)
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
			if (i + 1 < sb2.GetLength())
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
			fs->SeekFromBeginning((UInt64)start);
			resp->SetStatusCode(Net::WebStatus::SC_PARTIAL_CONTENT);
			UTF8Char sbuff[128];
			UTF8Char *sptr;
			sptr = Text::StrConcatC(sbuff, UTF8STRC("bytes "));
			sptr = Text::StrInt64(sptr, start);
			*sptr++ = '-';
			sptr = Text::StrUInt64(sptr, (UInt64)start + sizeLeft - 1);
			*sptr++ = '/';
			sptr = Text::StrUInt64(sptr, fileSize);
			resp->AddHeader(CSTR("Content-Range"), CSTRP(sbuff, sptr));
			partial = true;
		}
		resp->EnableWriteBuffer();
		resp->AddDefHeaders(req);
		AddCacheHeader(resp);
		resp->AddLastModified(&t);
		mime = Net::MIME::GetMIMEFromExt(CSTRP(sbuff, sptr3));
		resp->AddContentType(mime);
		resp->AddHeader(CSTR("Accept-Ranges"), CSTR("bytes"));
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
				IO::MemoryStream mstm(UTF8STRC("Net.WebServer.HTTPDirectoryHandler.ProcessRequest.mstm"));
				while (readSize > 0)
				{
					sizeLeft += mstm.Write(buff, readSize);
					readSize = fs->Read(buff, 2048);
				}
				mstm.SeekFromBeginning(0);
				Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, sizeLeft, &mstm);
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
				//this->SendContent(req, resp, mime.v, mime.len, readSize, cache->buff);
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

void Net::WebServer::HTTPDirectoryHandler::SetAllowOrigin(Text::CString origin)
{
	SDEL_STRING(this->allowOrigin);
	this->allowOrigin = Text::String::NewOrNull(origin);
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
	UTF8Char *sptr2;
	IO::Path::FindFileSession *sess;
	NEW_CLASS(this->packageMut, Sync::RWMutex());
	this->packageMut->LockWrite();
	NEW_CLASS(this->packageMap, Data::FastStringMap<PackageInfo*>());
	sptr = this->rootDir->ConcatTo(sbuff);
	if (sptr[-1] == '/' || sptr[-1] == '\\')
	{
	}
	else
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr2 = Text::StrConcatC(sptr, UTF8STRC("*.spk"));
	sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
	if (sess)
	{
		Data::DateTime dt;
		IO::Path::PathType pt;
		IO::StmData::FileData *fd;
		IO::PackageFile *pf;
		UOSInt i;
		PackageInfo *package;

		while ((sptr2 = IO::Path::FindNextFile(sptr, sess, &dt, &pt, 0)) != 0)
		{
			if (pt == IO::Path::PathType::File)
			{
				NEW_CLASS(fd, IO::StmData::FileData(CSTRP(sbuff, sptr2), false));
				pf = (IO::PackageFile*)parsers->ParseFileType(fd, IO::ParserType::PackageFile);
				DEL_CLASS(fd);
				if (pf)
				{
					package = MemAlloc(PackageInfo, 1);
					package->packageFile = pf;
					package->modTime = dt.ToTicks();
					i = Text::StrLastIndexOfCharC(sptr, (UOSInt)(sptr2 - sptr), '.');
					if (i != INVALID_INDEX)
					{
						sptr[i] = 0;
						sptr2 = &sptr[i];
					}
					package->fileName = Text::String::NewP(sptr, sptr2);
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
		NEW_CLASS(this->statMap, Data::FastStringMap<Net::WebServer::HTTPDirectoryHandler::StatInfo*>());
	}
}

void Net::WebServer::HTTPDirectoryHandler::SaveStats()
{
	if (this->statMap == 0)
	{
		Net::WebServer::HTTPDirectoryHandler::StatInfo *stat;
		UOSInt i;
		Sync::MutexUsage mutUsage(this->statMut);
		i = this->statMap->GetCount();
		while (i-- > 0)
		{
			stat = this->statMap->GetItem(i);
			if (stat->updated)
			{
				this->StatSave(stat);
			}
		}
		mutUsage.EndUse();
	}
}
