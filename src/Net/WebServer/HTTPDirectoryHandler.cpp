#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "Data/Sort/ArtificialQuickSortFunc.hpp"
#include "IO/DirectoryPackage.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Net/MIME.h"
#include "Net/WebServer/HTTPDirectoryHandler.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/RWMutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Text/JSText.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "Text/XML.h"
#include "Text/TextBinEnc/URIEncoding.h"

typedef struct
{
	NN<Text::String> fileName;
	UInt64 fileSize;
	Data::Timestamp modTime;
	IO::Path::PathType pt;
	UInt32 cnt;
} DirectoryEntry;

OSInt __stdcall HTTPDirectoryHandler_CompareFuncName(DirectoryEntry *obj1, DirectoryEntry *obj2)
{
	if (obj1->pt != obj2->pt)
	{
		if (obj1->pt == IO::Path::PathType::Directory)
		{
			return -1;
		}
		else if (obj2->pt == IO::Path::PathType::Directory)
		{
			return 1;
		}
	}
	OSInt ret = Text::StrCompareICase(obj1->fileName->v, obj2->fileName->v);
	if (ret != 0)
		return ret;
	if (obj1->fileSize > obj2->fileSize)
	{
		return 1;
	}
	else if (obj1->fileSize < obj2->fileSize)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

OSInt __stdcall HTTPDirectoryHandler_CompareFuncSize(DirectoryEntry *obj1, DirectoryEntry *obj2)
{
	if (obj1->fileSize > obj2->fileSize)
	{
		return 1;
	}
	else if (obj1->fileSize < obj2->fileSize)
	{
		return -1;
	}
	else
	{
		return Text::StrCompareICase(obj1->fileName->v, obj2->fileName->v);
	}
}

OSInt __stdcall HTTPDirectoryHandler_CompareFuncCount(DirectoryEntry *obj1, DirectoryEntry *obj2)
{
	if (obj1->cnt > obj2->cnt)
	{
		return 1;
	}
	else if (obj1->cnt < obj2->cnt)
	{
		return -1;
	}
	else
	{
		return HTTPDirectoryHandler_CompareFuncName(obj1, obj2);
	}
}

void Net::WebServer::HTTPDirectoryHandler::AddCacheHeader(NN<Net::WebServer::WebResponse> resp)
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
}

void Net::WebServer::HTTPDirectoryHandler::ResponsePackageFile(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<IO::PackageFile> packageFile)
{
	if (!this->allowBrowsing)
	{
		resp->ResponseError(req, Net::WebStatus::SC_FORBIDDEN);
		return;
	}
	
	Text::StringBuilderUTF8 sb2;
	Text::StringBuilderUTF8 sbOut;
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;

	sb2.ClearStr();
	sb2.Append(req->GetRequestURI());
	sptr = sb2.v;
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

	this->AddResponseHeaders(req, resp);
	resp->AddContentType(CSTR("text/html; charset=UTF-8"));
	AddCacheHeader(resp);

	UTF8Char sbuff[512];
	NN<Text::String> s;

	sbOut.AppendNE(UTF8STRC("<html><head><title>Index of "));
	Text::TextBinEnc::URIEncoding::URIDecode(sbuff, sptr);
	s = Text::XML::ToNewHTMLElementText(sbuff);
	sbOut.Append(s);
	sbOut.AppendNE(UTF8STRC("</title></head>\r\n<body>\r\n"));
	sbOut.AppendNE(UTF8STRC("<h2>Index Of "));
	sbOut.Append(s);
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
	UnsafeArray<UTF8Char> sptr2;
	IO::PackageFile::PackObjectType pot;
	i = 0;
	j = packageFile->GetCount();
	while (i < j)
	{
		pot = packageFile->GetItemType(i);
		if (pot == IO::PackageFile::PackObjectType::StreamData || pot == IO::PackageFile::PackObjectType::PackageFileType)
		{
			sbOut.AppendNE(UTF8STRC("<tr><td>"));
			sbOut.AppendNE(UTF8STRC("<a href=\""));
			if (!packageFile->GetItemName(sbuff, i).SetTo(sptr))
				sbuff[0] = 0;
			sptr2 = Text::TextBinEnc::URIEncoding::URIEncode(sbuff2, sbuff);
			sbOut.AppendNE(sbuff2, (UOSInt)(sptr2 - sbuff2));
			if (pot == IO::PackageFile::PackObjectType::PackageFileType)
			{
				sbOut.AppendNE(UTF8STRC("/"));
			}
			sbOut.AppendNE(UTF8STRC("\">"));
			sptr2 = Text::XML::ToXMLText(sbuff2, sbuff);
			sbOut.AppendNE(sbuff2, (UOSInt)(sptr2 - sbuff2));
			sbOut.AppendNE(UTF8STRC("</a></td><td>"));
			if (pot == IO::PackageFile::PackObjectType::PackageFileType)
			{
				sbOut.AppendNE(UTF8STRC("Directory"));
				sbOut.AppendNE(UTF8STRC("</td><td>"));
				sbOut.AppendNE(UTF8STRC("-"));
			}
			else
			{
				sptr2 = IO::Path::GetFileExt(sbuff2, sbuff, (UOSInt)(sptr - sbuff));
				Text::CStringNN mime = Net::MIME::GetMIMEFromExt(CSTRP(sbuff2, sptr2));
				sbOut.AppendNE(mime.v, mime.leng);
				sbOut.AppendNE(UTF8STRC("</td><td>"));
				sbOut.AppendU64(packageFile->GetItemSize(i));
			}

			sbOut.AppendNE(UTF8STRC("</td><td>"));
			sbOut.AppendTSNoZone(packageFile->GetItemModTime(i));
			sbOut.AppendNE(UTF8STRC("</td></tr>\r\n"));
		}
		
		i++;
	}
	sbOut.AppendC(UTF8STRC("</table></body></html>"));
	Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("text/html"), sbOut.GetLength(), sbOut.ToString());
	return ;
}

Bool Net::WebServer::HTTPDirectoryHandler::ResponsePackageFileItem(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<IO::VirtualPackageFile> packageFile, NN<const IO::PackFileItem> pitem)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UInt8 compBuff[10];
	sptr = IO::Path::GetFileExt(sbuff, pitem->name->v, pitem->name->leng);
	Text::CStringNN mime = Net::MIME::GetMIMEFromExt(CSTRP(sbuff, sptr));

	if (pitem->itemType == IO::PackFileItem::PackItemType::Compressed && pitem->compInfo->compMethod == Data::Compress::Decompressor::CM_DEFLATE)
	{
		Net::BrowserInfo::BrowserType browser = req->GetBrowser();
		Manage::OSInfo::OSType os = req->GetOS();
		NN<Text::String> enc;
		if (pitem->compInfo->checkMethod == Crypto::Hash::HashType::CRC32R_IEEE && req->GetSHeader(CSTR("Accept-Encoding")).SetTo(enc) && enc->IndexOf(UTF8STRC("gzip")) != INVALID_INDEX && Net::WebServer::HTTPServerUtil::AllowGZip(browser, os))
		{
			resp->EnableWriteBuffer();
			this->AddResponseHeaders(req, resp);
			resp->AddLastModified(pitem->modTime);
			resp->AddContentType(mime);
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
			compBuff[8] = 0;
			compBuff[9] = 0xff;
			resp->Write(Data::ByteArrayR(compBuff, 10));

			UInt64 dataSize = pitem->dataLength;
			UInt64 ofst;
			UOSInt readSize;
			if (dataSize < 1048576)
			{
				Data::ByteBuffer buff((UOSInt)dataSize);
				pitem->fullFd->GetRealData(packageFile->GetPItemDataOfst(pitem), (UOSInt)dataSize, buff);
				resp->Write(buff);
			}
			else
			{
				ofst = packageFile->GetPItemDataOfst(pitem);
				Data::ByteBuffer buff(1048576);
				while (dataSize > 0)
				{
					if (dataSize > 1048576)
					{
						readSize = 1048576;
					}
					else
					{
						readSize = (UOSInt)dataSize;
					}
					pitem->fullFd->GetRealData(ofst, readSize, buff);
					resp->Write(buff.WithSize(readSize));
					ofst += readSize;
					dataSize -= readSize;
				}
			}
			WriteUInt32(&compBuff[0], ReadMUInt32(pitem->compInfo->checkBytes));
			WriteUInt32(&compBuff[4], (UInt32)pitem->compInfo->decSize);
			resp->Write(Data::ByteArrayR(compBuff, 8));

			return true;
		}
		else if (req->GetSHeader(CSTR("Accept-Encoding")).SetTo(enc) && enc->IndexOf(UTF8STRC("deflate")) != INVALID_INDEX && Net::WebServer::HTTPServerUtil::AllowDeflate(browser, os))
		{
			resp->EnableWriteBuffer();
			this->AddResponseHeaders(req, resp);
			resp->AddLastModified(pitem->modTime);
			resp->AddContentType(mime);
			resp->AddHeader(CSTR("Content-Encoding"), CSTR("deflate"));
			resp->AddHeader(CSTR("Transfer-Encoding"), CSTR("chunked"));

			UInt64 dataSize = pitem->dataLength;
			UInt64 ofst;
			UOSInt readSize;
			if (dataSize < 1048576)
			{
				Data::ByteBuffer buff((UOSInt)dataSize);
				pitem->fullFd->GetRealData(packageFile->GetPItemDataOfst(pitem), (UOSInt)dataSize, buff);
				resp->Write(buff);
			}
			else
			{
				ofst = packageFile->GetPItemDataOfst(pitem);
				Data::ByteBuffer buff(1048576);
				while (dataSize > 0)
				{
					if (dataSize > 1048576)
					{
						readSize = 1048576;
					}
					else
					{
						readSize = (UOSInt)dataSize;
					}
					pitem->fullFd->GetRealData(ofst, readSize, buff);
					resp->Write(buff.WithSize(readSize));
					ofst += readSize;
					dataSize -= readSize;
				}
			}
			return true;
		}
	}
	NN<IO::StreamData> stmData;
	if (packageFile->GetPItemStmDataNew(pitem).SetTo(stmData))
	{
		UOSInt dataLen = (UOSInt)stmData->GetDataSize();
		Data::ByteBuffer dataBuff(dataLen);
		stmData->GetRealData(0, dataLen, dataBuff);
		stmData.Delete();

		resp->EnableWriteBuffer();
		this->AddResponseHeaders(req, resp);
		resp->AddLastModified(pitem->modTime);
		resp->AddContentType(mime);
		Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, dataLen, dataBuff.Arr());
		return true;
	}
	return false;
}

void Net::WebServer::HTTPDirectoryHandler::StatLoad(NN<Net::WebServer::HTTPDirectoryHandler::StatInfo> stat)
{
	Text::StringBuilderUTF8 sb;
	IO::FileStream fs(stat->statFileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		Text::PString sarr[2];
		Text::UTF8Reader reader(fs);
		sb.ClearStr();
		while (reader.ReadLine(sb, 1024))
		{
			if (Text::StrSplitP(sarr, 2, sb, '\t') == 2)
			{
				stat->cntMap->PutC(sarr[1].ToCString(), Text::StrToUInt32(sarr[0].v));
			}
			sb.ClearStr();
		}
	}
}

void Net::WebServer::HTTPDirectoryHandler::StatSave(NN<Net::WebServer::HTTPDirectoryHandler::StatInfo> stat)
{
	Text::StringBuilderUTF8 sb;
	stat->updated = false;
	if (stat->cntMap->GetCount() > 0)
	{
		IO::FileStream fs(stat->statFileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!fs.IsError())
		{
			Text::UTF8Writer writer(fs);
			UOSInt i;
			UOSInt j;
			i = 0;
			j = stat->cntMap->GetCount();
			while (i < j)
			{
				sb.ClearStr();
				sb.AppendU32(stat->cntMap->GetItem(i));
				sb.AppendUTF8Char('\t');
				sb.AppendOpt(stat->cntMap->GetKey(i));
				writer.WriteLine(sb.ToCString());
				i++;
			}
		}
	}
}

Net::WebServer::HTTPDirectoryHandler::HTTPDirectoryHandler(NN<Text::String> rootDir, Bool allowBrowsing, UInt64 fileCacheSize, Bool allowUpload)
{
	this->rootDir = rootDir->Clone();
	this->allowBrowsing = allowBrowsing;
	this->allowUpload = allowUpload;
	this->ctype = CT_DEFAULT;
	this->expirePeriod = 0;
	this->fileCacheSize = fileCacheSize;
	this->fileCacheUsing = 0;
	this->packageMap = 0;
	this->packageMut = 0;
	this->statMap = 0;
	this->statMut = 0;
}

Net::WebServer::HTTPDirectoryHandler::HTTPDirectoryHandler(Text::CStringNN rootDir, Bool allowBrowsing, UInt64 fileCacheSize, Bool allowUpload)
{
	this->rootDir = Text::String::New(rootDir);
	this->allowBrowsing = allowBrowsing;
	this->allowUpload = allowUpload;
	this->ctype = CT_DEFAULT;
	this->expirePeriod = 0;
	this->fileCacheSize = fileCacheSize;
	this->fileCacheUsing = 0;
	this->packageMap = 0;
	this->packageMut = 0;
	this->statMap = 0;
	this->statMut = 0;
}

Net::WebServer::HTTPDirectoryHandler::~HTTPDirectoryHandler()
{
	NN<Data::FastStringMapNN<PackageInfo>> packageMap;
	this->rootDir->Release();
	UOSInt cacheCnt;
	CacheInfo **cacheList = this->fileCache.ToArray(cacheCnt);
	while (cacheCnt-- > 0)
	{
		MemFree(cacheList[cacheCnt]->buff);
		MemFree(cacheList[cacheCnt]);
	}
	MemFree(cacheList);
	if (this->packageMap.SetTo(packageMap))
	{
		NN<PackageInfo> package;
		UOSInt i;
		i = packageMap->GetCount();
		while (i-- > 0)
		{
			package = packageMap->GetItemNoCheck(i);
			package->fileName->Release();
			package->packageFile.Delete();
			MemFreeNN(package);
		}
		this->packageMap.Delete();
		this->packageMut.Delete();
	}
	if (this->statMap)
	{
		NN<Net::WebServer::HTTPDirectoryHandler::StatInfo> stat;
		UOSInt i = this->statMap->GetCount();
		while (i-- > 0)
		{
			stat = this->statMap->GetItemNoCheck(i);
			this->StatSave(stat);

			stat->reqPath->Release();
			stat->statFileName->Release();
			DEL_CLASS(stat->cntMap);
			MemFreeNN(stat);
		}
		DEL_CLASS(this->statMap);
		DEL_CLASS(this->statMut);
	}
}

Bool Net::WebServer::HTTPDirectoryHandler::FileValid(Text::CStringNN subReq)
{
	return true;
}

Bool Net::WebServer::HTTPDirectoryHandler::ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq)
{
	if (this->DoRequest(req, resp, subReq))
	{
		return true;
	}
	return this->DoFileRequest(req, resp, subReq);
}

Bool Net::WebServer::HTTPDirectoryHandler::DoFileRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq)
{
	UInt8 buff[2048];
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[1024];
	UTF8Char sbuff2[1024];
	UnsafeArray<UTF8Char> sptr;
	UOSInt sptrLen;
	UnsafeArray<UTF8Char> sptr3;
	UnsafeArray<UTF8Char> sptr4;
	//Data::DateTime t;
	Text::CStringNN mime;
	UOSInt i;
	Bool dirPath;
	if (req->GetProtocol() != Net::WebServer::WebRequest::RequestProtocol::HTTP1_0 && req->GetProtocol() != Net::WebServer::WebRequest::RequestProtocol::HTTP1_1)
	{
		return resp->ResponseError(req, Net::WebStatus::SC_METHOD_NOT_ALLOWED);
	}
	if (subReq.leng == 0)
	{
		sb.ClearStr();
		sb.Append(req->GetRequestURI());
		i = sb.IndexOf('?');
		if (i != INVALID_INDEX)
		{
			sb.TrimToLength(i);
			sb.AppendUTF8Char('/');
			sb.Append(req->GetRequestURI()->ToCString().Substring(i));
		}
		else
		{
			sb.AppendUTF8Char('/');
		}
		return resp->RedirectURL(req, sb.ToCString(), 0);
	}
	if (!this->FileValid(subReq))
	{
		return resp->ResponseError(req, Net::WebStatus::SC_FORBIDDEN);
	}
	if (this->DoPackageRequest(req, resp, subReq))
	{
		return true;
	}
	CacheInfo *cache;
	Sync::MutexUsage mutUsage(this->fileCacheMut);
	cache = this->fileCache.Get(subReq);
	if (cache != 0)
	{
		Sync::Interlocked::IncrementI32(this->fileCacheUsing);
		mutUsage.EndUse();
		NN<Sync::Mutex> statMut;
		if (this->statMap && statMut.Set(this->statMut))
		{
			NN<Net::WebServer::HTTPDirectoryHandler::StatInfo> stat;
			sb.ClearStr();
			sb.Append(subReq);
			i = sb.LastIndexOf('/');
			sb.TrimToLength(i);
			Sync::MutexUsage statMutUsage(statMut);
			if (this->statMap->GetC(sb.ToCString()).SetTo(stat))
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
		this->AddResponseHeaders(req, resp);
		AddCacheHeader(resp);
		resp->AddLastModified(cache->t);
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
		Sync::Interlocked::DecrementI32(this->fileCacheUsing);
		return true;
	}
	sb.ClearStr();
	sb.Append(this->rootDir);
	mutUsage.EndUse();

	sptr = sb.GetEndPtr();
	if (sptr[-1] == '\\' || sptr[-1] == '/')
	{
		sb.RemoveChars(1);
	}
	UnsafeArray<const UTF8Char> reqTarget = subReq.v;
	sb.AppendC(reqTarget, subReq.leng);
	sptr = sb.v;
	sptrLen = sb.GetLength();
	UnsafeArray<UTF8Char> sptr2;
	i = Text::StrIndexOfCharC(sptr, sptrLen, '?');
	if (i != INVALID_INDEX)
	{
		sptr2 = &sptr[i + 1];
		sptr2[-1] = 0;
		sptrLen = i;
	}
	dirPath = sptr[sptrLen - 1] == '/';
	if (IO::Path::PATH_SEPERATOR != '/')
	{
		Text::StrReplace(sptr, '/', IO::Path::PATH_SEPERATOR);
	}

	IO::Path::PathType pt = IO::Path::GetPathType({sptr, sptrLen});
	if (pt == IO::Path::PathType::Unknown)
	{
		return false;
	}
	else if (pt == IO::Path::PathType::Directory)
	{
		if (!dirPath)
		{
			sb.ClearStr();
			sb.Append(req->GetRequestURI());
			i = sb.IndexOf('?');
			if (i != INVALID_INDEX)
			{
				sb.TrimToLength(i);
				sb.AppendUTF8Char('/');
				sb.Append(req->GetRequestURI()->ToCString().Substring(i));
			}
			else
			{
				sb.AppendUTF8Char('/');
			}
			return resp->RedirectURL(req, sb.ToCString(), 0);
		}
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
			sptr = sb.v;
			sptrLen = sb.GetLength();
			UInt64 sizeLeft;
			NN<Text::String> hdrVal;

			sptr3 = IO::Path::GetFileExt(sbuff, sptr, sptrLen);
			IO::FileStream fs({sptr, sptrLen}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
			Data::Timestamp t;
			t = fs.GetModifyTime();

			if (req->GetSHeader(CSTR("If-Modified-Since")).SetTo(hdrVal))
			{
				Data::DateTime t2;
				t2.SetValue(hdrVal->ToCString());
				t2.AddMS((OSInt)t.GetMS());
				if (t2.ToTicks() == t.ToTicks())
				{
					resp->SetStatusCode(Net::WebStatus::SC_NOT_MODIFIED);
					this->AddResponseHeaders(req, resp);
					AddCacheHeader(resp);
					resp->AddContentLength(0);
					resp->Write(Data::ByteArrayR(buff, 0));
					return true;
				}
			}

			resp->EnableWriteBuffer();
			this->AddResponseHeaders(req, resp);
			AddCacheHeader(resp);
			resp->AddLastModified(t);
			mime = Net::MIME::GetMIMEFromExt(CSTRP(sbuff, sptr3));
			resp->AddContentType(mime);
			sizeLeft = fs.GetLength();
			if (sizeLeft < this->fileCacheSize)
			{
				UOSInt readSize;
				cache = MemAlloc(CacheInfo, 1);
				cache->buff = MemAlloc(UInt8, (UOSInt)sizeLeft);
				cache->buffSize = (UOSInt)sizeLeft;
				cache->t = t;
				readSize = fs.Read(Data::ByteArray(cache->buff, (UOSInt)sizeLeft));
				if (readSize == sizeLeft)
				{
					Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, sizeLeft, cache->buff);
					Sync::MutexUsage mutUsage(this->fileCacheMut);
					this->fileCache.Put(subReq, cache);
					mutUsage.EndUse();
				}
				else
				{
					resp->AddContentLength(sizeLeft);
					resp->Write(Data::ByteArrayR(cache->buff, readSize));
					MemFree(cache->buff);
					MemFree(cache);
				}
			}
			else
			{
				Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, sizeLeft, fs);
			}
			return true;
		}
		else
		{
			if (this->allowBrowsing)
			{
				Text::StringBuilderUTF8 sb2;
				Text::StringBuilderUTF8 sbOut;
				Optional<Net::WebServer::HTTPDirectoryHandler::StatInfo> stat = 0;
				NN<StatInfo> nnstat;
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
					UnsafeArray<const UInt8> uplfile;
					UOSInt uplSize;
					UOSInt fileId;
					req->ParseHTTPForm();
					fileId = 0;
					while (req->GetHTTPFormFile(CSTR("uploadfile"), fileId, (UTF8Char*)buff, sizeof(buff), sptr2, uplSize).SetTo(uplfile))
					{
						Text::StringBuilderUTF8 sbTmp;
						sbTmp.AppendC(sb.ToString(), sb.GetLength());
						sbTmp.AppendP((UTF8Char*)buff, sptr2);
						if (IO::Path::GetPathType(sbTmp.ToCString()) == IO::Path::PathType::Unknown)
						{
							IO::FileStream *uplFS;
							NEW_CLASS(uplFS, IO::FileStream(sbTmp.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
							uplFS->Write(Data::ByteArrayR(uplfile, uplSize));
							DEL_CLASS(uplFS);
						}
						fileId++;
					}
					if (!req->GetQueryValue(CSTR("uploadDirect")).IsNull())
					{
						resp->EnableWriteBuffer();
						this->AddResponseHeaders(req, resp);
						resp->AddContentType(CSTR("text/plain"));
						AddCacheHeader(resp);
						resp->AddContentLength(2);
						resp->Write(Data::ByteArrayR((const UInt8*)"ok", 2));
						return true;
					}
				}

				resp->EnableWriteBuffer();
				this->AddResponseHeaders(req, resp);
				resp->AddContentType(CSTR("text/html; charset=UTF-8"));
				AddCacheHeader(resp);

				Bool isRoot = false;
				NN<Text::String> s;
				NN<Text::String> s2;
				sbOut.AppendNE(UTF8STRC("<html><head><title>Index of "));
				Text::TextBinEnc::URIEncoding::URIDecode(sbuff, sb2.ToString());
				s = Text::XML::ToNewHTMLElementText(sbuff);
				sbOut.Append(s);
				sbOut.AppendNE(UTF8STRC("</title><script type=\"application/javascript\">\r\n"
							    		"async function submitFile() {\r\n"
										"\tvar url = "));
				s2 = Text::JSText::ToNewJSTextDQuote(UnsafeArray<const UTF8Char>(req->GetRequestURI()->v));
				sbOut.Append(s2);
				s2->Release();
				sbOut.AppendNE(UTF8STRC(";\r\n"
							    		"\tif (url.indexOf(\"?\") >= 0)\r\n"
										"\t\turl = url + \"&uploadDirect=1\";\r\n"
										"\telse\r\n"
										"\t\turl = url + \"?uploadDirect=1\";\r\n"
							    		"\tvar fileupload = document.getElementById(\"uploadfile\");\r\n"
										"\tvar i = 0;\r\n"
										"\tvar j = fileupload.files.length;\r\n"
										"\twhile (i < j) {\r\n"
										"\t\tvar formData = new FormData();\r\n"
										"\t\tformData.append(\"uploadfile\", fileupload.files[i]);\r\n"
										"\t\tawait fetch(url, {\r\n"
										"\t\t\tmethod: \"POST\", \r\n"
										"\t\t\tbody: formData\r\n"
										"\t\t});\r\n"
										"\t\ti++;\r\n"
										"\t}\r\n"
										"\tdocument.location.reload();\r\n"
							    		"}\r\n"
										"</script></head>\r\n<body>\r\n"
							   			"<h2>Index Of "));
				sbOut.Append(s);
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
					sbOut.Append(s);
					sbOut.AppendNE(UTF8STRC(" enctype=\"multipart/form-data\">"));
					s->Release();
//					sbOut.AppendNE2(UTF8STRC("Upload: <input type=\"file\" id=\"uploadfile\" name=\"uploadfile\" multiple/><br/><input type=\"submit\"/>"),
//								   UTF8STRC("</form>"));
					sbOut.AppendNE2(UTF8STRC("Upload: <input type=\"file\" id=\"uploadfile\" name=\"uploadfile\" multiple/><br/><input type=\"button\" value=\"Upload\" onclick=\"submitFile()\"/>"),
								   UTF8STRC("</form>"));
				}

				req->GetQueryValueI32(CSTR("sort"), sort);
				Text::StringBuilderUTF8 sb3;
				sb3.AppendNE(sb2.ToString(), sb2.GetLength());
				sb3.AppendC(UTF8STRC("?sort=1"));
				s = Text::XML::ToNewAttrText(sb3.ToString());
				sbOut.AppendNE(UTF8STRC("<table><tr><th><a href="));
				sbOut.Append(s);
				s->Release();
				sbOut.AppendNE(UTF8STRC(">Name</a></th><th>MIME</th><th><a href="));
				sb3.ClearStr();
				sb3.AppendC2(sb2.ToString(), sb2.GetLength(), UTF8STRC("?sort=2"));
				s = Text::XML::ToNewAttrText(sb3.ToString());
				sbOut.Append(s);
				s->Release();
				sbOut.AppendNE(UTF8STRC(">File Size</a></th>"));
				if (this->statMap)
				{
					sbOut.AppendNE(UTF8STRC("<th><a href="));
					sb3.ClearStr();
					sb3.AppendC2(sb2.ToString(), sb2.GetLength(), UTF8STRC("?sort=3"));
					s = Text::XML::ToNewAttrText(sb3.ToString());
					sbOut.Append(s);
					s->Release();
					sbOut.AppendNE(UTF8STRC(">Download Count</a></th>"));
				}
				sbOut.AppendNE(UTF8STRC("<th>Modified Date</th></tr>\r\n"));

				if (isRoot)
				{
					NN<Sync::RWMutex> packageMut;
					NN<Data::FastStringMapNN<PackageInfo>> packageMap;
					if (this->packageMap.SetTo(packageMap) && this->packageMut.SetTo(packageMut))
					{
						NN<PackageInfo> package;
						UOSInt i;
						UOSInt j;
						Sync::RWMutexUsage packageMutUsage(packageMut, false);
						i = 0;
						j = packageMap->GetCount();
						while (i < j)
						{
							package = packageMap->GetItemNoCheck(i);
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
							sbOut.AppendNE(UTF8STRC("</td><td>"));
							sbOut.AppendTSNoZone(package->modTime);
							sbOut.AppendNE(UTF8STRC("</td></tr>\r\n"));
							
							i++;
						}
					}
				}

				sptr2 = Text::StrConcatC(sbuff, sb.ToString(), sb.GetLength());
				sptr3 = Text::StrConcatC(sptr2, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
				NN<IO::Path::FindFileSession> sess;
				if (IO::Path::FindFile(CSTRP(sbuff, sptr3)).SetTo(sess))
				{
					NN<Sync::Mutex> statMut;
					Sync::MutexUsage mutUsage;
					if (this->statMap && statMut.Set(this->statMut))
					{
						mutUsage.ReplaceMutex(statMut);
						if (sb2.GetLength() > 1 && sb2.EndsWith('/'))
						{
							sb2.RemoveChars(1);
						}
						if (!this->statMap->GetC(sb2.ToCString()).SetTo(nnstat))
						{
							nnstat = MemAllocNN(Net::WebServer::HTTPDirectoryHandler::StatInfo);
							nnstat->reqPath = Text::String::New(sb2.ToString(), sb2.GetLength());
							NEW_CLASS(nnstat->cntMap, Data::FastStringMap<UInt32>());
							nnstat->updated = true;
							sb2.ClearStr();
							sb2.AppendC2(sb.ToString(), sb.GetLength(), UTF8STRC(".counts"));
							nnstat->statFileName = Text::String::New(sb2.ToString(), sb2.GetLength());
							this->statMap->PutNN(nnstat->reqPath, nnstat);
							this->StatLoad(nnstat);
						}
						stat = nnstat;
					}

					UInt32 cnt;
					UInt64 fileSize;
					IO::Path::PathType pt;
					Data::Timestamp modTime;
					if (sort == 0)
					{
						while (IO::Path::FindNextFile(sptr2, sess, modTime, pt, fileSize).SetTo(sptr3))
						{
							if (Text::StrEqualsC(sptr2, (UOSInt)(sptr3 - sptr2), UTF8STRC(".")) || Text::StrEqualsC(sptr2, (UOSInt)(sptr3 - sptr2), UTF8STRC("..")))
							{
							}
							else
							{
								if (stat.SetTo(nnstat))
								{
									cnt = nnstat->cntMap->GetC({sptr2, (UOSInt)(sptr3 - sptr2)});
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
								sbOut.AppendTSNoZone(modTime);
								sbOut.AppendNE(UTF8STRC("</td></tr>\r\n"));
							}
						}
						IO::Path::FindFileClose(sess);
						if (stat.NotNull())
						{
							mutUsage.EndUse();
						}
					}
					else
					{
						Data::ArrayList<DirectoryEntry *> entList;
						DirectoryEntry *ent;
						while (IO::Path::FindNextFile(sptr2, sess, modTime, pt, fileSize).SetTo(sptr3))
						{
							if (Text::StrEqualsC(sptr2, (UOSInt)(sptr3 - sptr2), UTF8STRC(".")) || Text::StrEqualsC(sptr2, (UOSInt)(sptr3 - sptr2), UTF8STRC("..")))
							{
							}
							else
							{
								ent = MemAlloc(DirectoryEntry, 1);
								if (stat.SetTo(nnstat))
								{
									ent->cnt = nnstat->cntMap->GetC({sptr2, (UOSInt)(sptr3 - sptr2)});
								}
								else
								{
									ent->cnt = 0;
								}
								ent->fileName = Text::String::New(sptr2, (UOSInt)(sptr3 - sptr2));
								ent->fileSize = fileSize;
								ent->pt = pt;
								ent->modTime = modTime;
								entList.Add(ent);
							}
						}
						IO::Path::FindFileClose(sess);
						if (stat.NotNull())
						{
							mutUsage.EndUse();
						}

						UOSInt i;
						UOSInt j;
						UnsafeArray<DirectoryEntry*> entArr = entList.GetArr(j);
						if (sort == 1)
						{
							Data::Sort::ArtificialQuickSortFunc<DirectoryEntry*>::Sort(entArr, HTTPDirectoryHandler_CompareFuncName, 0, (OSInt)j - 1);
						}
						else if (sort == 2)
						{
							Data::Sort::ArtificialQuickSortFunc<DirectoryEntry*>::Sort(entArr, HTTPDirectoryHandler_CompareFuncSize, 0, (OSInt)j - 1);
						}
						else if (sort == 3)
						{
							Data::Sort::ArtificialQuickSortFunc<DirectoryEntry*>::Sort(entArr, HTTPDirectoryHandler_CompareFuncCount, 0, (OSInt)j - 1);
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
							sbOut.AppendTSNoZone(ent->modTime);
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
		UInt64 sizeLeft;
		NN<Text::String> hdrVal;
		NN<Sync::Mutex> statMut;

		IO::FileStream fs({sptr, sptrLen}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
		Data::Timestamp ts = fs.GetModifyTime();

		if (req->GetSHeader(CSTR("If-Modified-Since")).SetTo(hdrVal))
		{
			Data::DateTime t2;
			t2.SetValue(hdrVal->ToCString());
			t2.AddMS((OSInt)ts.GetMS());
			if (t2.ToTicks() == ts.ToTicks())
			{
				resp->SetStatusCode(Net::WebStatus::SC_NOT_MODIFIED);
				this->AddResponseHeaders(req, resp);
				AddCacheHeader(resp);
				resp->AddContentLength(0);
				resp->Write(Data::ByteArrayR(buff, 0));
				return true;
			}
		}

		if (this->statMap && statMut.Set(this->statMut))
		{
			sb2.ClearStr();
			sb2.Append(subReq);
			i = sb2.IndexOf('?');
			if (i != INVALID_INDEX)
			{
				sb2.TrimToLength(i);
			}
			i = sb2.LastIndexOf('/');
			sptr2 = sb2.v + i + 1;
			sb2.v[i] = 0;
			Sync::MutexUsage mutUsage(statMut);
			Optional<Net::WebServer::HTTPDirectoryHandler::StatInfo> stat;
			NN<Net::WebServer::HTTPDirectoryHandler::StatInfo> nnstat;
			if (sb2.ToString()[0] == 0)
			{
				stat = this->statMap->GetC(CSTR("/"));
			}
			else
			{
				stat = this->statMap->GetC({sb2.ToString(), i});
			}
			if (!stat.SetTo(nnstat))
			{
				nnstat = MemAllocNN(Net::WebServer::HTTPDirectoryHandler::StatInfo);
				if (sb2.ToString()[0] == 0)
				{
					nnstat->reqPath = Text::String::New(UTF8STRC("/"));
				}
				else
				{
					nnstat->reqPath = Text::String::New(sb2.ToString(), i);
				}
				NEW_CLASS(nnstat->cntMap, Data::FastStringMap<UInt32>());
				nnstat->updated = true;
				sptr3 = Text::StrConcatC(sbuff, sptr, sptrLen);
				i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr3 - sbuff), IO::Path::PATH_SEPERATOR);
				sptr3 = Text::StrConcatC(&sbuff[i + 1], UTF8STRC(".counts"));
				nnstat->statFileName = Text::String::New(sbuff, (UOSInt)(sptr3 - sbuff));
				this->statMap->PutNN(nnstat->reqPath, nnstat);
				this->StatLoad(nnstat);
			}
			nnstat->cntMap->PutC({sptr2, sb2.GetLength() - i - 1}, nnstat->cntMap->GetC({sptr2, sb2.GetLength() - i - 1}) + 1);
			nnstat->updated = true;
			mutUsage.EndUse();
		}

		sptr3 = IO::Path::GetFileExt(sbuff, sptr, sptrLen);

		Bool partial = false;
		sizeLeft = fs.GetLength();
		sb2.ClearStr();
		if (req->GetHeaderC(sb2, CSTR("Range")))
		{
			UInt64 fileSize = sizeLeft;
			if (!sb2.StartsWith(UTF8STRC("bytes=")))
			{
				resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
				this->AddResponseHeaders(req, resp);
				AddCacheHeader(resp);
				resp->AddContentLength(0);
				resp->Write(Data::ByteArrayR(buff, 0));
				return true;
			}
			if (sb2.IndexOf(',') != INVALID_INDEX)
			{
				resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
				this->AddResponseHeaders(req, resp);
				AddCacheHeader(resp);
				resp->AddContentLength(0);
				resp->Write(Data::ByteArrayR(buff, 0));
				return true;
			}
			Int64 start = 0;
			Int64 end = -1;
			i = sb2.IndexOf('-');
			if (i == INVALID_INDEX)
			{
				resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
				this->AddResponseHeaders(req, resp);
				AddCacheHeader(resp);
				resp->AddContentLength(0);
				resp->Write(Data::ByteArrayR(buff, 0));
				return true;
			}
			sptr = sb2.v;
			sptr[i] = 0;
			if (!Text::StrToInt64(&sptr[6], start))
			{
				resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
				this->AddResponseHeaders(req, resp);
				AddCacheHeader(resp);
				resp->AddContentLength(0);
				resp->Write(Data::ByteArrayR(buff, 0));
				return true;
			}
			if (i + 1 < sb2.GetLength())
			{
				if (!Text::StrToInt64(&sptr[i + 1], end))
				{
					resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
					this->AddResponseHeaders(req, resp);
					AddCacheHeader(resp);
					resp->AddContentLength(0);
					resp->Write(Data::ByteArrayR(buff, 0));
					return true;
				}
				if (end <= start || (UInt64)end > sizeLeft)
				{
					resp->SetStatusCode(Net::WebStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE);
					this->AddResponseHeaders(req, resp);
					AddCacheHeader(resp);
					resp->AddContentLength(0);
					resp->Write(Data::ByteArrayR(buff, 0));
					return true;
				}
				sizeLeft = (UInt64)(end - start);
			}
			else
			{
				sizeLeft = sizeLeft - (UInt64)start;
			}
			fs.SeekFromBeginning((UInt64)start);
			resp->SetStatusCode(Net::WebStatus::SC_PARTIAL_CONTENT);
			UTF8Char sbuff[128];
			UnsafeArray<UTF8Char> sptr;
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
		this->AddResponseHeaders(req, resp);
		AddCacheHeader(resp);
		resp->AddLastModified(ts);
		mime = Net::MIME::GetMIMEFromExt(CSTRP(sbuff, sptr3));
		resp->AddContentType(mime);
		resp->AddHeader(CSTR("Accept-Ranges"), CSTR("bytes"));
		if (sizeLeft <= 0)
		{
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
		else if (!partial && sizeLeft < this->fileCacheSize)
		{
			UOSInt readSize;
			cache = MemAlloc(CacheInfo, 1);
			cache->buff = MemAlloc(UInt8, (UOSInt)sizeLeft);
			cache->buffSize = (UOSInt)sizeLeft;
			cache->t = ts;
			readSize = fs.Read(Data::ByteArray(cache->buff, (UOSInt)sizeLeft));
			if (readSize == sizeLeft)
			{
				Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, sizeLeft, cache->buff);
				Sync::MutexUsage mutUsage(this->fileCacheMut);
				cache = this->fileCache.Put(subReq, cache);
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
				resp->Write(Data::ByteArrayR(cache->buff, readSize));
				MemFree(cache->buff);
				MemFree(cache);
			}
		}
		else
		{
			Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, sizeLeft, fs);
		}
		return true;
	}
	return false;
}

Bool Net::WebServer::HTTPDirectoryHandler::DoPackageRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq)
{
	NN<Sync::RWMutex> packageMut;
	NN<Data::FastStringMapNN<PackageInfo>> packageMap;
	UTF8Char sbuff[1024];
	UnsafeArray<UTF8Char> sptr;
	Text::CStringNN mime;
	UOSInt i;
	if (this->packageMap.SetTo(packageMap) && this->packageMut.SetTo(packageMut))
	{
		Bool dirPath;
		Text::StringBuilderUTF8 sb;
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
		dirPath = sb.EndsWith('/');
		i = Text::StrIndexOfCharC(&sb.ToString()[1], sb.GetLength() - 1, '/');
		Text::CStringNN dirName = sb.ToCString();
		if (i != INVALID_INDEX)
		{
			sb.v[i + 1] = 0;
			dirName.leng = i + 1;
		}
		NN<PackageInfo> package;
		Optional<PackageInfo> optpackage;
		Sync::RWMutexUsage packageMutUsage(packageMut, false);
		optpackage = packageMap->GetC(dirName.Substring(1));
		packageMutUsage.EndUse();
		if (optpackage.SetTo(package))
		{
			NN<IO::PackageFile> packageFile = package->packageFile;
			Bool needRelease = false;
			if (packageFile->GetCount() == 1 && packageFile->GetItemType(0) == IO::PackageFile::PackObjectType::PackageFileType)
			{
				if (packageFile->GetItemName(sbuff, 0).SetTo(sptr) && dirName.Substring(1).Equals(sbuff, (UOSInt)(sptr - sbuff)))
				{
					packageFile->GetItemPack(0, needRelease).SetTo(packageFile);
				}
			}

			sptr = &sb.v[i + 2];
			if (packageFile->GetFileType() == IO::PackageFileType::Virtual)
			{
				NN<IO::VirtualPackageFile> vpackageFile = NN<IO::VirtualPackageFile>::ConvertFrom(packageFile);
				if (i == INVALID_INDEX || sptr[0] == 0)
				{
					if (dirPath)
					{
						NN<const IO::PackFileItem> pitem2;
						if (vpackageFile->GetPackFileItem((const UTF8Char*)"index.html").SetTo(pitem2) && vpackageFile->GetPItemType(pitem2) == IO::PackageFile::PackObjectType::StreamData)
						{
							if (!ResponsePackageFileItem(req,resp, vpackageFile, pitem2))
							{
								ResponsePackageFile(req, resp, subReq, vpackageFile);
							}
						}
						else
						{
							ResponsePackageFile(req, resp, subReq, vpackageFile);
						}
						if (needRelease)
						{
							vpackageFile.Delete();
						}
						return true;
					}
					else
					{
						if (needRelease)
						{
							vpackageFile.Delete();
						}
						sb.ClearStr();
						sb.Append(req->GetRequestURI());
						i = sb.IndexOf('?');
						if (i != INVALID_INDEX)
						{
							sb.TrimToLength(i);
							sb.AppendUTF8Char('/');
							sb.Append(req->GetRequestURI()->ToCString().Substring(i));
						}
						else
						{
							sb.AppendUTF8Char('/');
						}
						return resp->RedirectURL(req, sb.ToCString(), 0);
					}
				}
				NN<const IO::PackFileItem> pitem;
				if (vpackageFile->GetPackFileItem(sptr).SetTo(pitem))
				{
					IO::PackageFile::PackObjectType pot = vpackageFile->GetPItemType(pitem);
					if (pot == IO::PackageFile::PackObjectType::StreamData)
					{
						if (ResponsePackageFileItem(req, resp, vpackageFile, pitem))
						{
							if (needRelease)
							{
								vpackageFile.Delete();
							}
							return true;
						}
					}
					else if (pot == IO::PackageFile::PackObjectType::PackageFileType)
					{
						Bool innerNeedRelease;
						NN<IO::PackageFile> innerPF;
						if (vpackageFile->GetPItemPack(pitem, innerNeedRelease).SetTo(innerPF))
						{
							if (dirPath)
							{
								UOSInt index2 = innerPF->GetItemIndex(CSTR("index.html"));
								if (index2 != INVALID_INDEX && innerPF->GetItemType(index2) == IO::PackageFile::PackObjectType::StreamData)
								{
									NN<IO::StreamData> stmData;
									if (innerPF->GetItemStmDataNew(index2).SetTo(stmData))
									{
										UOSInt dataLen = (UOSInt)stmData->GetDataSize();
										Data::ByteBuffer dataBuff(dataLen);
										stmData->GetRealData(0, dataLen, dataBuff);
										stmData.Delete();
										mime = Net::MIME::GetMIMEFromExt(CSTR("html"));

										resp->EnableWriteBuffer();
										this->AddResponseHeaders(req, resp);
										resp->AddLastModified(innerPF->GetItemModTime(index2));
										resp->AddContentType(mime);
										Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, dataLen, dataBuff.Arr());
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
								if (innerNeedRelease)
								{
									innerPF.Delete();
								}
								if (needRelease)
								{
									packageFile.Delete();
								}
								return true;
							}
							else
							{
								if (innerNeedRelease)
								{
									innerPF.Delete();
								}
								if (needRelease)
								{
									vpackageFile.Delete();
								}
								sb.ClearStr();
								sb.Append(req->GetRequestURI());
								i = sb.IndexOf('?');
								if (i != INVALID_INDEX)
								{
									sb.TrimToLength(i);
									sb.AppendUTF8Char('/');
									sb.Append(req->GetRequestURI()->ToCString().Substring(i));
								}
								else
								{
									sb.AppendUTF8Char('/');
								}
								return resp->RedirectURL(req, sb.ToCString(), 0);
							}
						}
					}
				}
			}
			if (needRelease)
			{
				packageFile.Delete();
			}
		}
	}
	return false;
}

Optional<IO::PackageFile> Net::WebServer::HTTPDirectoryHandler::GetPackageFile(Text::CStringNN path, OutParam<Bool> needRelease)
{
	if (path.StartsWith('/'))
	{
		path = path.Substring(1);
	}
	Text::StringBuilderUTF8 sb;
	NN<Data::FastStringMapNN<PackageInfo>> packageMap;
	NN<Sync::RWMutex> packageMut;
	if (this->packageMap.SetTo(packageMap) && this->packageMut.SetTo(packageMut))
	{
		Text::CStringNN subPath;
		UOSInt i = path.IndexOf('/');
		Optional<PackageInfo> optpkgInfo;
		NN<PackageInfo> pkgInfo;
		Text::CStringNN pkgName;
		if (i == INVALID_INDEX)
		{
			pkgName = path;
			subPath = CSTR("");
		}
		else
		{
			sb.ClearStr();
			sb.AppendC(path.v, i);
			pkgName = sb.ToCString();
			subPath = path.Substring(i + 1);
		}
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		Sync::RWMutexUsage packageMutUsage(packageMut, false);
		optpkgInfo = packageMap->GetC(pkgName);
		packageMutUsage.EndUse();
		if (optpkgInfo.SetTo(pkgInfo))
		{
			Bool thisNeedRelease = false;
			NN<IO::PackageFile> packageFile = pkgInfo->packageFile;
			if (packageFile->GetCount() == 1 && packageFile->GetItemType(0) == IO::PackageFile::PackObjectType::PackageFileType)
			{
				if (packageFile->GetItemName(sbuff, 0).SetTo(sptr) && pkgName.Equals(sbuff, (UOSInt)(sptr - sbuff)))
				{
					if (!packageFile->GetItemPack(0, thisNeedRelease).SetTo(packageFile))
						return 0;
				}
			}
			if (subPath.leng > 0)
			{
				Optional<IO::PackageFile> ret;
				ret = packageFile->GetItemPack(subPath, needRelease);
				if (thisNeedRelease)
					packageFile.Delete();
				return ret;
			}
			else
			{
				needRelease.Set(thisNeedRelease);
				return packageFile;
			}
		}
	}

	sb.ClearStr();
	sb.Append(this->rootDir);

	if (sb.EndsWith('\\') || sb.EndsWith('/'))
	{
		sb.RemoveChars(1);
	}
	sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
	sb.Append(path);
	if (IO::Path::PATH_SEPERATOR != '/')
	{
		sb.Replace('/', IO::Path::PATH_SEPERATOR);
	}

	IO::Path::PathType pt = IO::Path::GetPathType(sb.ToCString());
	if (pt == IO::Path::PathType::Directory)
	{
		IO::DirectoryPackage *dpkg;
		needRelease.Set(true);
		NEW_CLASS(dpkg, IO::DirectoryPackage(sb.ToCString()));
		return dpkg;
	}
	needRelease.Set(false);
	return 0;
}

void Net::WebServer::HTTPDirectoryHandler::SetRootDir(NN<Text::String> rootDir)
{
	Sync::MutexUsage mutUsage(this->fileCacheMut);
	this->rootDir->Release();
	this->rootDir = rootDir->Clone();
}

void Net::WebServer::HTTPDirectoryHandler::SetRootDir(Text::CStringNN rootDir)
{
	Sync::MutexUsage mutUsage(this->fileCacheMut);
	this->rootDir->Release();
	this->rootDir = Text::String::New(rootDir);
}

void Net::WebServer::HTTPDirectoryHandler::SetAllowBrowsing(Bool allowBrowsing)
{
	this->allowBrowsing = allowBrowsing;
}

void Net::WebServer::HTTPDirectoryHandler::SetCacheType(CacheType ctype)
{
	this->ctype = ctype;
}

void Net::WebServer::HTTPDirectoryHandler::SetExpirePeriod(Int32 period)
{
	this->expirePeriod = period;
}

void Net::WebServer::HTTPDirectoryHandler::ClearFileCache()
{
	UOSInt cacheCnt;
	Sync::MutexUsage mutUsage(this->fileCacheMut);
	while (this->fileCacheUsing > 0)
	{
		Sync::SimpleThread::Sleep(1);
	}
	CacheInfo **cacheList = this->fileCache.ToArray(cacheCnt);
	while (cacheCnt-- > 0)
	{
		MemFree(cacheList[cacheCnt]->buff);
		MemFree(cacheList[cacheCnt]);
	}
	MemFree(cacheList);
	this->fileCache.Clear();
}

void Net::WebServer::HTTPDirectoryHandler::ExpandPackageFiles(NN<Parser::ParserList> parsers, Text::CStringNN searchPattern)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	NN<IO::Path::FindFileSession> sess;
	NN<Data::FastStringMapNN<PackageInfo>> packageMap;
	NN<Sync::RWMutex> packageMut;
	Sync::RWMutexUsage packageMutUsage;
	if (!this->packageMut.SetTo(packageMut) || !this->packageMap.SetTo(packageMap))
	{
		NEW_CLASSNN(packageMut, Sync::RWMutex());
		NEW_CLASSNN(packageMap, Data::FastStringMapNN<PackageInfo>());
		this->packageMut = packageMut;
		this->packageMap = packageMap;
		packageMutUsage.ReplaceMutex(packageMut, true);
	}
	else
	{
		packageMutUsage.ReplaceMutex(packageMut, true);
	}
	sptr = this->rootDir->ConcatTo(sbuff);
	if (sptr[-1] == '/' || sptr[-1] == '\\')
	{
	}
	else
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr2 = searchPattern.ConcatTo(sptr);
	if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
	{
		Data::Timestamp ts;
		IO::Path::PathType pt;
		Optional<IO::PackageFile> pf;
		NN<IO::PackageFile> nnpf;
		UOSInt i;
		NN<PackageInfo> package;

		while (IO::Path::FindNextFile(sptr, sess, ts, pt, 0).SetTo(sptr2))
		{
			if (pt == IO::Path::PathType::File)
			{
				{
					IO::StmData::FileData fd(CSTRP(sbuff, sptr2), false);
					pf = Optional<IO::PackageFile>::ConvertFrom(parsers->ParseFileType(fd, IO::ParserType::PackageFile));
				}
				if (pf.SetTo(nnpf))
				{
					package = MemAllocNN(PackageInfo);
					package->packageFile = nnpf;
					package->modTime = ts;
					i = Text::StrLastIndexOfCharC(sptr, (UOSInt)(sptr2 - sptr), '.');
					if (i != INVALID_INDEX)
					{
						sptr[i] = 0;
						sptr2 = &sptr[i];
					}
					package->fileName = Text::String::NewP(sptr, sptr2);
					if (packageMap->PutNN(package->fileName, package).SetTo(package))
					{
						package->fileName->Release();
						package->packageFile.Delete();
						MemFreeNN(package);
					}
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}
}

void Net::WebServer::HTTPDirectoryHandler::EnableStats()
{
	if (this->statMap == 0)
	{
		NEW_CLASS(this->statMut, Sync::Mutex());
		NEW_CLASS(this->statMap, Data::FastStringMapNN<Net::WebServer::HTTPDirectoryHandler::StatInfo>());
	}
}

void Net::WebServer::HTTPDirectoryHandler::SaveStats()
{
	NN<Sync::Mutex> mut;
	if (this->statMap && mut.Set(this->statMut))
	{
		NN<Net::WebServer::HTTPDirectoryHandler::StatInfo> stat;
		UOSInt i;
		Sync::MutexUsage mutUsage(mut);
		i = this->statMap->GetCount();
		while (i-- > 0)
		{
			stat = this->statMap->GetItemNoCheck(i);
			if (stat->updated)
			{
				this->StatSave(stat);
			}
		}
		mutUsage.EndUse();
	}
}

void Net::WebServer::HTTPDirectoryHandler::AddPackage(Text::CStringNN path, NN<IO::PackageFile> pkgFile)
{
	if (path.StartsWith('/'))
	{
		path = path.Substring(1);
	}
	NN<Data::FastStringMapNN<PackageInfo>> packageMap;
	NN<Sync::RWMutex> packageMut;
	Sync::RWMutexUsage packageMutUsage;
	if (!this->packageMut.SetTo(packageMut) || !this->packageMap.SetTo(packageMap))
	{
		NEW_CLASSNN(packageMut, Sync::RWMutex());
		NEW_CLASSNN(packageMap, Data::FastStringMapNN<PackageInfo>());
		this->packageMut = packageMut;
		this->packageMap = packageMap;
	}

	NN<PackageInfo> package;
	package = MemAllocNN(PackageInfo);
	package->fileName = Text::String::New(path);
	package->modTime = Data::Timestamp::Now();
	package->packageFile = pkgFile;
	if (packageMap->PutC(path, package).SetTo(package))
	{
		package->fileName->Release();
		package->packageFile.Delete();
		MemFreeNN(package);
	}
}
