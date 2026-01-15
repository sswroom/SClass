#include "Stdafx.h"
#include "Data/StringMapNative.hpp"
#include "DB/DBReader.h"
#include "DB/MDBFileConn.h"
#include "DB/ODBCConn.h"
#include "DB/SQLBuilder.h"
#include "IO/FileCheck.h"
#include "IO/IniFile.h"
#include "IO/Path.h"
#include "Net/MySQLTCPClient.h"
#include "Net/OSSocketFactory.h"
#include "Parser/FileParser/MD5Parser.h"
#include "SSWR/DiscDB/DiscDBEnv.h"
#include "Text/StringBuilderUTF8.h"

void SSWR::DiscDB::DiscDBEnv::LoadDB()
{
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
	{
		return;
	}
	NN<DB::DBReader> r;
	Text::StringBuilderUTF8 sb;
	NN<BurntDiscInfo> disc;
	Data::DateTime dt;
	if (db->ExecuteReader(CSTR("select DiscID, DiscTypeID, BurntDate, Status from BurntDisc")).SetTo(r))
	{
		while (r->ReadNext())
		{
			disc = MemAllocNN(BurntDiscInfo);
			disc->discId = Text::String::OrEmpty(r->GetNewStr(0));
			disc->discTypeId = Text::String::OrEmpty(r->GetNewStr(1));
			disc->burntDate = r->GetTimestamp(2);
			disc->status = r->GetInt32(3);
			if (this->discMap.PutNN(disc->discId, disc).SetTo(disc))
			{
				disc->discId->Release();
				disc->discTypeId->Release();
				MemFreeNN(disc);
			}
		}
		db->CloseReader(r);
	}

	NN<DVDTypeInfo> dvdType;
	if (db->ExecuteReader(CSTR("select DiscTypeID, Name, Description from DVDType")).SetTo(r))
	{
		while (r->ReadNext())
		{
			dvdType = MemAllocNN(DVDTypeInfo);
			dvdType->discTypeID = Text::String::OrEmpty(r->GetNewStr(0));
			dvdType->name = Text::String::OrEmpty(r->GetNewStr(1));
			dvdType->description = Text::String::OrEmpty(r->GetNewStr(2));
			this->dvdTypeMap.PutNN(dvdType->discTypeID, dvdType);
		}
		db->CloseReader(r);
	}

	NN<CategoryInfo> cate;
	if (db->ExecuteReader(CSTR("select ID, Name from Category")).SetTo(r))
	{
		while (r->ReadNext())
		{
			cate = MemAllocNN(CategoryInfo);
			cate->id = r->GetNewStrNN(0);
			cate->name = Text::String::OrEmpty(r->GetNewStr(1));
			this->cateMap.PutNN(cate->id, cate);
		}
		db->CloseReader(r);
	}

	NN<DiscTypeInfo> discType;
	if (db->ExecuteReader(CSTR("select DiscTypeID, Brand, Name, Speed, DVDType, MadeIn, MID, TID, Revision, QCTest, Remark from DiscType")).SetTo(r))
	{
		while (r->ReadNext())
		{
			discType = MemAllocNN(DiscTypeInfo);
			discType->discTypeId = r->GetNewStrNN(0);
			discType->brand = r->GetNewStrNN(1);
			sb.ClearStr();
			r->GetStr(2, sb);
			discType->name = Text::StrCopyNew(sb.ToString()).Ptr();
			discType->speed = r->GetDblOrNAN(3);
			sb.ClearStr();
			r->GetStr(4, sb);
			discType->dvdType = Text::StrCopyNew(sb.ToString()).Ptr();
			sb.ClearStr();
			r->GetStr(5, sb);
			discType->madeIn = Text::StrCopyNew(sb.ToString()).Ptr();
			sb.ClearStr();
			r->GetStr(6, sb);
			discType->mid = Text::StrCopyNew(sb.ToString()).Ptr();
			sb.ClearStr();
			r->GetStr(7, sb);
			discType->tid = Text::StrCopyNew(sb.ToString()).Ptr();
			sb.ClearStr();
			r->GetStr(8, sb);
			discType->revision = Text::StrCopyNew(sb.ToString()).Ptr();
			discType->qcTest = r->GetBool(9);
			sb.ClearStr();
			r->GetStr(10, sb);
			discType->remark = Text::StrCopyNew(sb.ToString()).Ptr();
			this->discTypeMap.PutNN(discType->discTypeId, discType);
		}
		db->CloseReader(r);
	}

	NN<DVDVideoInfo> dvdVideo;
	if (db->ExecuteReader(CSTR("select VIDEOID, ANIME, SERIES, VOLUME, DISCTYPE from DVDVIDEO order by VIDEOID")).SetTo(r))
	{
		while (r->ReadNext())
		{
			dvdVideo = MemAllocNN(DVDVideoInfo);
			dvdVideo->videoId = r->GetInt32(0);
			dvdVideo->anime = Text::String::OrEmpty(r->GetNewStr(1));
			dvdVideo->series = r->GetNewStr(2);
			dvdVideo->volume = r->GetNewStr(3);
			dvdVideo->dvdType = Text::String::OrEmpty(r->GetNewStr(4));
			this->dvdVideoMap.Put(dvdVideo->videoId, dvdVideo);
		}
		db->CloseReader(r);
	}
}

SSWR::DiscDB::DiscDBEnv::DiscDBEnv()
{
	NN<IO::ConfigFile> cfg;
	this->db = nullptr;
	NEW_CLASSNN(this->sockf, Net::OSSocketFactory(false));
	NEW_CLASSNN(this->clif, Net::TCPClientFactory(this->sockf));
	NEW_CLASS(this->monMgr, Media::MonitorMgr());

	if (IO::IniFile::ParseProgConfig(0).SetTo(cfg))
	{
		NN<Text::String> str;
		if (cfg->GetValue(CSTR("DSN")).SetTo(str))
		{
			this->db = DB::ODBCConn::CreateDBTool(str,
				cfg->GetValue(CSTR("UID")),
				cfg->GetValue(CSTR("PWD")),
				cfg->GetValue(CSTR("Schema")),
				this->log, CSTR("DB: ")).OrNull();
		}
		else if (cfg->GetValue(CSTR("MySQLServer")).SetTo(str))
		{
			this->db = Net::MySQLTCPClient::CreateDBTool(this->clif,
				str,
				Text::String::OrEmpty(cfg->GetValue(CSTR("MySQLDB"))).Ptr(),
				Text::String::OrEmpty(cfg->GetValue(CSTR("UID"))),
				Text::String::OrEmpty(cfg->GetValue(CSTR("PWD"))),
				this->log, CSTR("DB: "));
		}
		else if (cfg->GetValue(CSTR("MDBFile")).SetTo(str))
		{
			this->db = DB::MDBFileConn::CreateDBTool(str->ToCString(), this->log, CSTR("DB: "));
		}
		cfg.Delete();

		if (this->db.NotNull())
		{
			this->LoadDB();
			this->err = ERR_NONE;
		}
		else
		{
			this->err = ERR_DB;
		}
	}
	else
	{
		this->err = ERR_CONFIG;
	}
}

SSWR::DiscDB::DiscDBEnv::~DiscDBEnv()
{
	this->db.Delete();
	DEL_CLASS(this->monMgr);

	UIntOS i;
	NN<BurntDiscInfo> disc;
	i = this->discMap.GetCount();
	while (i-- > 0)
	{
		disc = this->discMap.GetItemNoCheck(i);
		disc->discId->Release();
		disc->discTypeId->Release();
		MemFreeNN(disc);
	}

	NN<DVDTypeInfo> dvdType;
	i = this->dvdTypeMap.GetCount();
	while (i-- > 0)
	{
		dvdType = this->dvdTypeMap.GetItemNoCheck(i);
		dvdType->discTypeID->Release();
		dvdType->name->Release();
		dvdType->description->Release();
		MemFreeNN(dvdType);
	}

	NN<CategoryInfo> cate;
	i = this->cateMap.GetCount();
	while (i-- > 0)
	{
		cate = this->cateMap.GetItemNoCheck(i);
		cate->id->Release();
		cate->name->Release();
		MemFreeNN(cate);
	}

	NN<DiscTypeInfo> discType;
	i = this->discTypeMap.GetCount();
	while (i-- > 0)
	{
		discType = this->discTypeMap.GetItemNoCheck(i);
		discType->discTypeId->Release();
		discType->brand->Release();
		Text::StrDelNew(discType->name);
		Text::StrDelNew(discType->dvdType);
		Text::StrDelNew(discType->madeIn);
		SDEL_TEXT(discType->mid);
		SDEL_TEXT(discType->tid);
		SDEL_TEXT(discType->revision);
		SDEL_TEXT(discType->remark);
		MemFreeNN(discType);
	}

	NN<DVDVideoInfo> dvdVideo;
	i = this->dvdVideoMap.GetCount();
	while (i-- > 0)
	{
		dvdVideo = this->dvdVideoMap.GetItemNoCheck(i);
		dvdVideo->anime->Release();
		OPTSTR_DEL(dvdVideo->series);
		OPTSTR_DEL(dvdVideo->volume);
		dvdVideo->dvdType->Release();
		MemFreeNN(dvdVideo);
	}
	this->clif.Delete();
	this->sockf.Delete();
}

SSWR::DiscDB::DiscDBEnv::ErrorType SSWR::DiscDB::DiscDBEnv::GetErrorType()
{
	return this->err;
}

Double SSWR::DiscDB::DiscDBEnv::GetMonitorHDPI(Optional<MonitorHandle> hMon)
{
	return this->monMgr->GetMonitorHDPI(hMon);
}

Double SSWR::DiscDB::DiscDBEnv::GetMonitorDDPI(Optional<MonitorHandle> hMon)
{
	return this->monMgr->GetMonitorDDPI(hMon);
}

Optional<const SSWR::DiscDB::DiscDBEnv::BurntDiscInfo> SSWR::DiscDB::DiscDBEnv::NewBurntDisc(Text::CStringNN discId, Text::CStringNN discTypeId, const Data::Timestamp &ts)
{
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return nullptr;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("insert into BurntDisc (DiscID, DiscTypeID, BurntDate, Status) values ("));
	sql.AppendStrUTF8(discId.v);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrUTF8(discTypeId.v);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendTS(ts);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(0);
	sql.AppendCmdC(CSTR(")"));
	if (db->ExecuteNonQuery(sql.ToCString()) > 0)
	{
		NN<BurntDiscInfo> disc;
		disc = MemAllocNN(BurntDiscInfo);
		disc->discId = Text::String::New(discId);
		disc->discTypeId = Text::String::New(discTypeId);
		disc->burntDate = ts;
		disc->status = 0;
		this->discMap.PutNN(disc->discId, disc);
		return disc;
	}
	else
	{
		return nullptr;
	}
}

UIntOS SSWR::DiscDB::DiscDBEnv::GetBurntDiscs(NN<Data::ArrayListNN<BurntDiscInfo>> discList)
{
	discList->AddAll(this->discMap);
	return this->discMap.GetCount();
}

Optional<const SSWR::DiscDB::DiscDBEnv::BurntDiscInfo> SSWR::DiscDB::DiscDBEnv::GetBurntDisc(Text::CStringNN discId)
{
	return this->discMap.GetC(discId);
}

IntOS SSWR::DiscDB::DiscDBEnv::GetBurntDiscIndex(Text::CStringNN discId)
{
	return this->discMap.IndexOfC(discId);
}

Bool SSWR::DiscDB::DiscDBEnv::NewBurntFile(UnsafeArray<const UTF8Char> discId, UIntOS fileId, UnsafeArray<const UTF8Char> name, UInt64 fileSize, Text::CString category, Int32 videoId)
{
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return false;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("insert into BurntFile (DiscID, FileID, Name, FileSize, Category, VIDEOID) values ("));
	sql.AppendStrUTF8(discId);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32((Int32)fileId);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrUTF8(name);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt64((Int64)fileSize);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrC(category);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(videoId);
	sql.AppendCmdC(CSTR(")"));
	return db->ExecuteNonQuery(sql.ToCString()) > 0;
}

UIntOS SSWR::DiscDB::DiscDBEnv::GetBurntFiles(Text::CString discId, NN<Data::ArrayListNN<DiscFileInfo>> fileList)
{
	NN<DiscFileInfo> file;
	UIntOS ret = 0;
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return 0;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("select FileID, Name, FileSize, Category, VIDEOID from BurntFile where DiscID = "));
	sql.AppendStrUTF8(discId.v);
	sql.AppendCmdC(CSTR(" order by FileID"));
	NN<DB::DBReader> r;
	if (db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		Text::StringBuilderUTF8 sb;

		while (r->ReadNext())
		{
			file = MemAllocNN(DiscFileInfo);
			file->fileId = (UInt32)r->GetInt32(0);
			file->fileName = Text::String::OrEmpty(r->GetNewStr(1));
			file->fileSize = (UInt64)r->GetInt64(2);
			sb.ClearStr();
			r->GetStr(3, sb);
			Text::StrConcatC(file->category, sb.ToString(), sb.GetLength());
			file->videoId = r->GetInt32(4);
			fileList->Add(file);
			ret++;
		}
		db->CloseReader(r);
	}
	return ret;
}

void SSWR::DiscDB::DiscDBEnv::FreeBurntFiles(NN<Data::ArrayListNN<DiscFileInfo>> fileList)
{
	UIntOS i;
	NN<DiscFileInfo> file;
	i = fileList->GetCount();
	while (i-- > 0)
	{
		file = fileList->GetItemNoCheck(i);
		file->fileName->Release();
		MemFreeNN(file);
	}
	fileList->Clear();
}

UIntOS SSWR::DiscDB::DiscDBEnv::GetDVDTypeCount()
{
	return this->dvdTypeMap.GetCount();
}

Optional<const SSWR::DiscDB::DiscDBEnv::DVDTypeInfo> SSWR::DiscDB::DiscDBEnv::GetDVDType(UIntOS index)
{
	return this->dvdTypeMap.GetItem(index);
}

IntOS SSWR::DiscDB::DiscDBEnv::GetDVDTypeIndex(Text::CStringNN discTypeID)
{
	return this->dvdTypeMap.IndexOfC(discTypeID);
}

Bool SSWR::DiscDB::DiscDBEnv::ModifyDVDType(Text::CStringNN discTypeID, Text::CStringNN name, Text::CStringNN desc)
{
	NN<DVDTypeInfo> dvdType;
	if (!this->dvdTypeMap.GetC(discTypeID).SetTo(dvdType))
		return false;
	if (name.Equals(dvdType->name) && desc.Equals(dvdType->description))
	{
		return true;
	}
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return false;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("update DVDType set Name = "));
	sql.AppendStrUTF8(name.v);
	sql.AppendCmdC(CSTR(", Description = "));
	sql.AppendStrUTF8(desc.v);
	sql.AppendCmdC(CSTR(" where DiscTypeID = "));
	sql.AppendStrUTF8(discTypeID.v);
	if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		dvdType->name->Release();
		dvdType->description->Release();
		dvdType->name = Text::String::New(name);
		dvdType->description = Text::String::New(desc);
		return true;
	}
	return false;
}

Optional<const SSWR::DiscDB::DiscDBEnv::DVDTypeInfo> SSWR::DiscDB::DiscDBEnv::NewDVDType(Text::CStringNN discTypeID, Text::CStringNN name, Text::CStringNN desc)
{
	NN<DVDTypeInfo> dvdType;
	if (this->dvdTypeMap.GetC(discTypeID).SetTo(dvdType))
		return nullptr;
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return nullptr;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("insert into DVDType (DiscTypeID, Name, Description) values ("));
	sql.AppendStrUTF8(discTypeID.v);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrUTF8(name.v);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrUTF8(desc.v);
	sql.AppendCmdC(CSTR(")"));
	if (db->ExecuteNonQuery(sql.ToCString()) > 0)
	{
		dvdType = MemAllocNN(DVDTypeInfo);
		dvdType->discTypeID = Text::String::New(discTypeID.v, discTypeID.leng);
		dvdType->name = Text::String::New(name);
		dvdType->description = Text::String::New(desc);
		this->dvdTypeMap.PutNN(dvdType->discTypeID, dvdType);
		return dvdType;
	}
	return nullptr;
}

UIntOS SSWR::DiscDB::DiscDBEnv::GetCategories(NN<Data::ArrayListNN<CategoryInfo>> cateList)
{
	cateList->AddAll(this->cateMap);
	return this->cateMap.GetCount();
}

Optional<const SSWR::DiscDB::DiscDBEnv::DiscTypeInfo> SSWR::DiscDB::DiscDBEnv::GetDiscType(Text::CStringNN discTypeId)
{
	return this->discTypeMap.GetC(discTypeId);
}

UIntOS SSWR::DiscDB::DiscDBEnv::GetDiscTypes(NN<Data::ArrayListNN<DiscTypeInfo>> discTypeList)
{
	discTypeList->AddAll(this->discTypeMap);
	return this->discTypeMap.GetCount();
}

UIntOS SSWR::DiscDB::DiscDBEnv::GetDiscTypesByBrand(NN<Data::ArrayListNN<const DiscTypeInfo>> discTypeList, UnsafeArray<const UTF8Char> brand, UIntOS brandLen)
{
	UIntOS ret;
	UIntOS i;
	UIntOS j;
	NN<DiscTypeInfo> discType;
	ret = 0;
	i = 0;
	j = this->discTypeMap.GetCount();
	while (i < j)
	{
		discType = this->discTypeMap.GetItemNoCheck(i);
		if (discType->brand->EqualsICase(brand, brandLen))
		{
			discTypeList->Add(discType);
			ret++;
		}
		i++;
	}
	return ret;
}

Int32 SSWR::DiscDB::DiscDBEnv::NewDVDVideo(UnsafeArray<const UTF8Char> anime, UnsafeArrayOpt<const UTF8Char> series, UnsafeArrayOpt<const UTF8Char> volume, UnsafeArray<const UTF8Char> dvdType)
{
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return -1;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("insert into DVDVIDEO (ANIME, SERIES, VOLUME, DISCTYPE) values ("));
	sql.AppendStrUTF8(anime);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrUTF8(series);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrUTF8(volume);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrUTF8(dvdType);
	sql.AppendCmdC(CSTR(")"));
	if (db->ExecuteNonQuery(sql.ToCString()) > 0)
	{
		NN<DVDVideoInfo> dvdVideo = MemAllocNN(DVDVideoInfo);
		dvdVideo->videoId = db->GetLastIdentity32();
		dvdVideo->anime = Text::String::NewNotNullSlow(anime);
		dvdVideo->series = Text::String::NewOrNullSlow(series);
		dvdVideo->volume = Text::String::NewOrNullSlow(volume);
		dvdVideo->dvdType = Text::String::NewNotNullSlow(dvdType);
		this->dvdVideoMap.Put(dvdVideo->videoId, dvdVideo);
		return dvdVideo->videoId;
	}
	else
	{
		return -1;
	}
}

UIntOS SSWR::DiscDB::DiscDBEnv::GetDVDVideos(NN<Data::ArrayListNN<DVDVideoInfo>> dvdVideoList)
{
	dvdVideoList->AddAll(this->dvdVideoMap);
	return this->dvdVideoMap.GetCount();
}

Optional<const SSWR::DiscDB::DiscDBEnv::DVDVideoInfo> SSWR::DiscDB::DiscDBEnv::GetDVDVideo(Int32 videoId)
{
	return this->dvdVideoMap.Get(videoId);
}

Bool SSWR::DiscDB::DiscDBEnv::NewMovies(UnsafeArray<const UTF8Char> discId, UIntOS fileId, UnsafeArray<const UTF8Char> mainTitle, NN<Text::String> type, UnsafeArrayOpt<const UTF8Char> chapter, UnsafeArrayOpt<const UTF8Char> chapterTitle, Text::CString videoFormat, Int32 width, Int32 height, Int32 fps, Int32 length, Text::CString audioFormat, Int32 samplingRate, Int32 bitRate, UnsafeArrayOpt<const UTF8Char> aspectRatio, UnsafeArrayOpt<const UTF8Char> remark)
{
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return false;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("insert into Movies (DiscID, FileID, MainTitle, Type, Chapter, ChapterTitle, VideoFormat, Width, Height, fps, length, AudioFormat, SamplingRate, Bitrate, AspectRatio, Remarks) values ("));
	sql.AppendStrUTF8(discId);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32((Int32)(UInt32)fileId);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrUTF8(mainTitle);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStr(type);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrUTF8(chapter);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrUTF8(chapterTitle);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrC(videoFormat);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(width);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(height);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(fps);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(length);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrC(audioFormat);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(samplingRate);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(bitRate);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrUTF8(aspectRatio);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrUTF8(remark);
	sql.AppendCmdC(CSTR(")"));
	return db->ExecuteNonQuery(sql.ToCString()) > 0;
}

Bool SSWR::DiscDB::DiscDBEnv::AddMD5(NN<IO::StreamData> fd)
{
	Parser::FileParser::MD5Parser parser;
	parser.SetCodePage(65001);
	NN<IO::FileCheck> fileChk;
	if (!Optional<IO::FileCheck>::ConvertFrom(parser.ParseFile(fd, nullptr, IO::ParserType::FileCheck)).SetTo(fileChk))
	{
		return false;
	}
	if (fileChk->GetCheckType() != Crypto::Hash::HashType::MD5)
	{
		fileChk.Delete();
		return false;
	}
	Text::StringBuilderUTF8 sbDiscId;
	NN<Text::String> s = fd->GetFullName();
	UIntOS i;
	UIntOS j;
	UIntOS k;
	i = s->LastIndexOf(IO::Path::PATH_SEPERATOR);
	sbDiscId.AppendC(&s->v[i + 1], s->leng - i - 1);
	i = sbDiscId.IndexOf('.');
	if (i != INVALID_INDEX)
	{
		sbDiscId.RemoveChars(sbDiscId.GetCharCnt() - i);
	}

	Data::StringMapNative<Int32> nameMap;
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return false;
	Text::StringBuilderUTF8 sb;
	DB::SQLBuilder sql(db);
	NN<DB::DBReader> r;
	sql.Clear();
	sql.AppendCmdC(CSTR("select Name, FileID from BurntFile where DiscID = "));
	sql.AppendStrUTF8(sbDiscId.ToString());
	if (db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		while (r->ReadNext())
		{
			sb.ClearStr();
			r->GetStr(0, sb);
			nameMap.Put(sb.ToCString(), r->GetInt32(1));
		}
		db->CloseReader(r);
	}

	UTF8Char sbuff[33];
	UInt8 md5Hash[16];
	i = 0;
	j = fileChk->GetCount();
	while (i < j)
	{
		fileChk->GetEntryHash(i, md5Hash);
		Text::StrHexBytes(sbuff, md5Hash, 16, 0);

		sql.Clear();
		sql.AppendCmdC(CSTR("update BurntFile set filemd5 = "));
		sql.AppendStrUTF8(sbuff);
		sql.AppendCmdC(CSTR(" where DiscID = "));
		sql.AppendStrUTF8(sbDiscId.ToString());
		NN<Text::String> fileName;
		if (fileChk->GetEntryName(i).SetTo(fileName))
		{
			k = fileName->IndexOf('\\', 1);
			if (nameMap.GetIndexC(fileName->ToCString().Substring(k + 1)) >= 0)
			{
				sql.AppendCmdC(CSTR(" and FileID = "));
				sql.AppendInt32(nameMap.Get(fileName->ToCString().Substring(k + 1)));
			}
			else
			{
				sql.AppendCmdC(CSTR(" and Name = "));
				sql.AppendStrUTF8(&fileName->v[k + 1]);
			}
		}
		db->ExecuteNonQuery(sql.ToCString());
		i++;
	}
	fileChk.Delete();
	return true;
}
