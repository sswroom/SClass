#include "Stdafx.h"
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
	DB::DBReader *r;
	Text::StringBuilderUTF8 sb;
	BurntDiscInfo *disc;
	Data::DateTime dt;
	r = this->db->ExecuteReaderC(UTF8STRC("select DiscID, DiscTypeID, BurntDate, Status from BurntDisc"));
	if (r)
	{
		while (r->ReadNext())
		{
			disc = MemAlloc(BurntDiscInfo, 1);
			sb.ClearStr();
			r->GetStr(0, &sb);
			disc->discId = Text::StrCopyNew(sb.ToString());
			sb.ClearStr();
			r->GetStr(1, &sb);
			disc->discTypeId = Text::StrCopyNew(sb.ToString());
			if (r->GetDate(2, &dt) == DB::DBReader::DET_OK)
			{
				disc->burntDate = dt.ToTicks();
			}
			else
			{
				disc->burntDate = 0;
			}
			disc->status = r->GetInt32(3);
			disc = this->discMap->Put(disc->discId, disc);
			if (disc)
			{
				Text::StrDelNew(disc->discId);
				Text::StrDelNew(disc->discTypeId);
				MemFree(disc);
			}
		}
		this->db->CloseReader(r);
	}

	DVDTypeInfo *dvdType;
	r = this->db->ExecuteReaderC(UTF8STRC("select DiscTypeID, Name, Description from DVDType"));
	if (r)
	{
		while (r->ReadNext())
		{
			dvdType = MemAlloc(DVDTypeInfo, 1);
			sb.ClearStr();
			r->GetStr(0, &sb);
			dvdType->discTypeID = Text::StrCopyNew(sb.ToString());
			sb.ClearStr();
			r->GetStr(1, &sb);
			dvdType->name = Text::StrCopyNew(sb.ToString());
			sb.ClearStr();
			r->GetStr(2, &sb);
			dvdType->description = Text::StrCopyNew(sb.ToString());
			this->dvdTypeMap->Put(dvdType->discTypeID, dvdType);
		}
		this->db->CloseReader(r);
	}

	CategoryInfo *cate;
	r = this->db->ExecuteReaderC(UTF8STRC("select ID, Name from Category"));
	if (r)
	{
		while (r->ReadNext())
		{
			cate = MemAlloc(CategoryInfo, 1);
			sb.ClearStr();
			r->GetStr(0, &sb);
			cate->id = Text::StrCopyNew(sb.ToString());
			cate->name = r->GetNewStr(1);
			this->cateMap->Put(cate->id, cate);
		}
		this->db->CloseReader(r);
	}

	DiscTypeInfo *discType;
	r = this->db->ExecuteReaderC(UTF8STRC("select DiscTypeID, Brand, Name, Speed, DVDType, MadeIn, MID, TID, Revision, QCTest, Remark from DiscType"));
	if (r)
	{
		while (r->ReadNext())
		{
			discType = MemAlloc(DiscTypeInfo, 1);
			sb.ClearStr();
			r->GetStr(0, &sb);
			discType->discTypeId = Text::StrCopyNew(sb.ToString());
			discType->brand = r->GetNewStr(1);
			sb.ClearStr();
			r->GetStr(2, &sb);
			discType->name = Text::StrCopyNew(sb.ToString());
			discType->speed = r->GetDbl(3);
			sb.ClearStr();
			r->GetStr(4, &sb);
			discType->dvdType = Text::StrCopyNew(sb.ToString());
			sb.ClearStr();
			r->GetStr(5, &sb);
			discType->madeIn = Text::StrCopyNew(sb.ToString());
			sb.ClearStr();
			r->GetStr(6, &sb);
			discType->mid = Text::StrCopyNew(sb.ToString());
			sb.ClearStr();
			r->GetStr(7, &sb);
			discType->tid = Text::StrCopyNew(sb.ToString());
			sb.ClearStr();
			r->GetStr(8, &sb);
			discType->revision = Text::StrCopyNew(sb.ToString());
			discType->qcTest = r->GetBool(9);
			sb.ClearStr();
			r->GetStr(10, &sb);
			discType->remark = Text::StrCopyNew(sb.ToString());
			this->discTypeMap->Put(discType->discTypeId, discType);
		}
		this->db->CloseReader(r);
	}

	DVDVideoInfo *dvdVideo;
	r = this->db->ExecuteReaderC(UTF8STRC("select VIDEOID, ANIME, SERIES, VOLUME, DISCTYPE from DVDVIDEO order by VIDEOID"));
	if (r)
	{
		while (r->ReadNext())
		{
			dvdVideo = MemAlloc(DVDVideoInfo, 1);
			dvdVideo->videoId = r->GetInt32(0);
			dvdVideo->anime = r->GetNewStr(1);
			dvdVideo->series = r->GetNewStr(2);
			dvdVideo->volume = r->GetNewStr(3);
			sb.ClearStr();
			r->GetStr(4, &sb);
			dvdVideo->dvdType = Text::StrCopyNew(sb.ToString());
			this->dvdVideoMap->Put(dvdVideo->videoId, dvdVideo);
		}
		this->db->CloseReader(r);
	}
}

SSWR::DiscDB::DiscDBEnv::DiscDBEnv()
{
	IO::ConfigFile *cfg;
	this->db = 0;
	NEW_CLASS(this->sockf, Net::OSSocketFactory(false));
	NEW_CLASS(this->log, IO::LogTool());
	NEW_CLASS(this->monMgr, Media::MonitorMgr());
	NEW_CLASS(this->discMap, Data::StringUTF8Map<BurntDiscInfo*>());
	NEW_CLASS(this->dvdTypeMap, Data::StringUTF8Map<DVDTypeInfo*>());
	NEW_CLASS(this->cateMap, Data::StringUTF8Map<CategoryInfo*>());
	NEW_CLASS(this->discTypeMap, Data::StringUTF8Map<DiscTypeInfo*>());
	NEW_CLASS(this->dvdVideoMap, Data::Int32Map<DVDVideoInfo*>());

	cfg = IO::IniFile::ParseProgConfig(0);
	if (cfg)
	{
		Text::String *s;
		if ((s = cfg->GetValue(UTF8STRC("DSN"))) != 0)
		{
			this->db = DB::ODBCConn::CreateDBTool(s->v,
				Text::String::OrEmpty(cfg->GetValue(UTF8STRC("UID")))->v,
				Text::String::OrEmpty(cfg->GetValue(UTF8STRC("PWD")))->v,
				Text::String::OrEmpty(cfg->GetValue(UTF8STRC("Schema")))->v,
				this->log, CSTR("DB: "));
		}
		else if ((s = cfg->GetValue(UTF8STRC("MySQLServer"))) != 0)
		{
			this->db = Net::MySQLTCPClient::CreateDBTool(this->sockf,
				s,
				Text::String::OrEmpty(cfg->GetValue(UTF8STRC("MySQLDB"))),
				Text::String::OrEmpty(cfg->GetValue(UTF8STRC("UID"))),
				Text::String::OrEmpty(cfg->GetValue(UTF8STRC("PWD"))),
				this->log, CSTR("DB: "));
		}
		else if ((s = cfg->GetValue(UTF8STRC("MDBFile"))) != 0)
		{
			this->db = DB::MDBFileConn::CreateDBTool(s->v, this->log, CSTR("DB: "));
		}
		DEL_CLASS(cfg);

		if (this->db)
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
	SDEL_CLASS(this->db);
	DEL_CLASS(this->log);
	DEL_CLASS(this->monMgr);

	UOSInt i;
	BurntDiscInfo *disc;
	Data::ArrayList<BurntDiscInfo *> *discList = this->discMap->GetValues();
	i = discList->GetCount();
	while (i-- > 0)
	{
		disc = discList->GetItem(i);
		Text::StrDelNew(disc->discId);
		Text::StrDelNew(disc->discTypeId);
		MemFree(disc);
	}
	DEL_CLASS(this->discMap);

	DVDTypeInfo *dvdType;
	Data::ArrayList<DVDTypeInfo*> *dvdTypeList = this->dvdTypeMap->GetValues();
	i = dvdTypeList->GetCount();
	while (i-- > 0)
	{
		dvdType = dvdTypeList->GetItem(i);
		Text::StrDelNew(dvdType->discTypeID);
		Text::StrDelNew(dvdType->name);
		Text::StrDelNew(dvdType->description);
		MemFree(dvdType);
	}
	DEL_CLASS(this->dvdTypeMap);

	CategoryInfo *cate;
	Data::ArrayList<CategoryInfo*> *cateList = this->cateMap->GetValues();
	i = cateList->GetCount();
	while (i-- > 0)
	{
		cate = cateList->GetItem(i);
		Text::StrDelNew(cate->id);
		cate->name->Release();
		MemFree(cate);
	}
	DEL_CLASS(this->cateMap);

	DiscTypeInfo *discType;
	Data::ArrayList<DiscTypeInfo*> *discTypeList = this->discTypeMap->GetValues();
	i = discTypeList->GetCount();
	while (i-- > 0)
	{
		discType = discTypeList->GetItem(i);
		Text::StrDelNew(discType->discTypeId);
		discType->brand->Release();
		Text::StrDelNew(discType->name);
		Text::StrDelNew(discType->dvdType);
		Text::StrDelNew(discType->madeIn);
		SDEL_TEXT(discType->mid);
		SDEL_TEXT(discType->tid);
		SDEL_TEXT(discType->revision);
		SDEL_TEXT(discType->remark);
		MemFree(discType);
	}
	DEL_CLASS(this->discTypeMap);

	DVDVideoInfo *dvdVideo;
	Data::ArrayList<DVDVideoInfo*> *dvdVideoList = this->dvdVideoMap->GetValues();
	i = dvdVideoList->GetCount();
	while (i-- > 0)
	{
		dvdVideo = dvdVideoList->GetItem(i);
		dvdVideo->anime->Release();
		SDEL_STRING(dvdVideo->series);
		SDEL_STRING(dvdVideo->volume);
		Text::StrDelNew(dvdVideo->dvdType);
		MemFree(dvdVideo);
	}
	DEL_CLASS(this->dvdVideoMap);
	DEL_CLASS(this->sockf);
}

SSWR::DiscDB::DiscDBEnv::ErrorType SSWR::DiscDB::DiscDBEnv::GetErrorType()
{
	return this->err;
}

Double SSWR::DiscDB::DiscDBEnv::GetMonitorHDPI(MonitorHandle *hMon)
{
	return this->monMgr->GetMonitorHDPI(hMon);
}

Double SSWR::DiscDB::DiscDBEnv::GetMonitorDDPI(MonitorHandle *hMon)
{
	return this->monMgr->GetMonitorDDPI(hMon);
}

const SSWR::DiscDB::DiscDBEnv::BurntDiscInfo *SSWR::DiscDB::DiscDBEnv::NewBurntDisc(const UTF8Char *discId, const UTF8Char *discTypeId, Data::DateTime *dt)
{
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("insert into BurntDisc (DiscID, DiscTypeID, BurntDate, Status) values ("));
	sql.AppendStrUTF8(discId);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(discTypeId);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendDate(dt);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt32(0);
	sql.AppendCmdC(UTF8STRC(")"));
	if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) > 0)
	{
		BurntDiscInfo *disc;
		disc = MemAlloc(BurntDiscInfo, 1);
		disc->discId = Text::StrCopyNew(discId);
		disc->discTypeId = Text::StrCopyNew(discTypeId);
		disc->burntDate = dt->ToTicks();
		disc->status = 0;
		this->discMap->Put(disc->discId, disc);
		return disc;
	}
	else
	{
		return 0;
	}
}

UOSInt SSWR::DiscDB::DiscDBEnv::GetBurntDiscs(Data::ArrayList<BurntDiscInfo*> *discList)
{
	discList->AddAll(this->discMap->GetValues());
	return this->discMap->GetCount();
}

const SSWR::DiscDB::DiscDBEnv::BurntDiscInfo *SSWR::DiscDB::DiscDBEnv::GetBurntDisc(const UTF8Char *discId)
{
	return this->discMap->Get(discId);
}

OSInt SSWR::DiscDB::DiscDBEnv::GetBurntDiscIndex(const UTF8Char *discId)
{
	return this->discMap->GetIndex(discId);
}

Bool SSWR::DiscDB::DiscDBEnv::NewBurntFile(const UTF8Char *discId, UOSInt fileId, const UTF8Char *name, UInt64 fileSize, const UTF8Char *category, Int32 videoId)
{
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("insert into BurntFile (DiscID, FileID, Name, FileSize, Category, VIDEOID) values ("));
	sql.AppendStrUTF8(discId);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt32((Int32)fileId);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(name);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt64((Int64)fileSize);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(category);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt32(videoId);
	sql.AppendCmdC(UTF8STRC(")"));
	return this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) > 0;
}

UOSInt SSWR::DiscDB::DiscDBEnv::GetBurntFiles(const UTF8Char *discId, Data::ArrayList<DiscFileInfo*> *fileList)
{
	DiscFileInfo *file;
	UOSInt ret = 0;
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("select FileID, Name, FileSize, Category, VIDEOID from BurntFile where DiscID = "));
	sql.AppendStrUTF8(discId);
	sql.AppendCmdC(UTF8STRC(" order by FileID"));
	DB::DBReader *r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
	{
		Text::StringBuilderUTF8 sb;

		while (r->ReadNext())
		{
			file = MemAlloc(DiscFileInfo, 1);
			file->fileId = (UInt32)r->GetInt32(0);
			file->fileName = r->GetNewStr(1);
			file->fileSize = (UInt64)r->GetInt64(2);
			sb.ClearStr();
			r->GetStr(3, &sb);
			Text::StrConcatC(file->category, sb.ToString(), sb.GetLength());
			file->videoId = r->GetInt32(4);
			fileList->Add(file);
			ret++;
		}
		this->db->CloseReader(r);
	}
	return ret;
}

void SSWR::DiscDB::DiscDBEnv::FreeBurntFiles(Data::ArrayList<DiscFileInfo*> *fileList)
{
	UOSInt i;
	DiscFileInfo *file;
	i = fileList->GetCount();
	while (i-- > 0)
	{
		file = fileList->GetItem(i);
		file->fileName->Release();
		MemFree(file);
	}
	fileList->Clear();
}

UOSInt SSWR::DiscDB::DiscDBEnv::GetDVDTypeCount()
{
	return this->dvdTypeMap->GetCount();
}

const SSWR::DiscDB::DiscDBEnv::DVDTypeInfo *SSWR::DiscDB::DiscDBEnv::GetDVDType(UOSInt index)
{
	return this->dvdTypeMap->GetValues()->GetItem(index);
}

OSInt SSWR::DiscDB::DiscDBEnv::GetDVDTypeIndex(const UTF8Char *discTypeID)
{
	return this->dvdTypeMap->GetIndex(discTypeID);
}

Bool SSWR::DiscDB::DiscDBEnv::ModifyDVDType(const UTF8Char *discTypeID, const UTF8Char *name, const UTF8Char *desc)
{
	DVDTypeInfo *dvdType = this->dvdTypeMap->Get(discTypeID);
	if (dvdType == 0)
		return false;
	if (Text::StrEquals(dvdType->name, name) && Text::StrEquals(dvdType->description, desc))
	{
		return true;
	}
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("update DVDType set Name = "));
	sql.AppendStrUTF8(name);
	sql.AppendCmdC(UTF8STRC(", Description = "));
	sql.AppendStrUTF8(desc);
	sql.AppendCmdC(UTF8STRC(" where DiscTypeID = "));
	sql.AppendStrUTF8(discTypeID);
	if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) >= 0)
	{
		Text::StrDelNew(dvdType->name);
		Text::StrDelNew(dvdType->description);
		dvdType->name = Text::StrCopyNew(name);
		dvdType->description = Text::StrCopyNew(desc);
		return true;
	}
	return false;
}

const SSWR::DiscDB::DiscDBEnv::DVDTypeInfo *SSWR::DiscDB::DiscDBEnv::NewDVDType(const UTF8Char *discTypeID, const UTF8Char *name, const UTF8Char *desc)
{
	DVDTypeInfo *dvdType = this->dvdTypeMap->Get(discTypeID);
	if (dvdType != 0)
		return 0;
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("insert into DVDType (DiscTypeID, Name, Description) values ("));
	sql.AppendStrUTF8(discTypeID);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(name);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(desc);
	sql.AppendCmdC(UTF8STRC(")"));
	if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) > 0)
	{
		dvdType = MemAlloc(DVDTypeInfo, 1);
		dvdType->discTypeID = Text::StrCopyNew(discTypeID);
		dvdType->name = Text::StrCopyNew(name);
		dvdType->description = Text::StrCopyNew(desc);
		this->dvdTypeMap->Put(dvdType->discTypeID, dvdType);
		return dvdType;
	}
	return 0;
}

UOSInt SSWR::DiscDB::DiscDBEnv::GetCategories(Data::ArrayList<CategoryInfo*> *cateList)
{
	cateList->AddAll(this->cateMap->GetValues());
	return this->cateMap->GetCount();
}

const SSWR::DiscDB::DiscDBEnv::DiscTypeInfo *SSWR::DiscDB::DiscDBEnv::GetDiscType(const UTF8Char *discTypeId)
{
	return this->discTypeMap->Get(discTypeId);
}

UOSInt SSWR::DiscDB::DiscDBEnv::GetDiscTypes(Data::ArrayList<DiscTypeInfo*> *discTypeList)
{
	discTypeList->AddAll(this->discTypeMap->GetValues());
	return this->discTypeMap->GetCount();
}

UOSInt SSWR::DiscDB::DiscDBEnv::GetDiscTypesByBrand(Data::ArrayList<const DiscTypeInfo*> *discTypeList, const UTF8Char *brand, UOSInt brandLen)
{
	UOSInt ret;
	UOSInt i;
	UOSInt j;
	Data::ArrayList<DiscTypeInfo *> *myDiscTypes = this->discTypeMap->GetValues();
	DiscTypeInfo *discType;
	ret = 0;
	i = 0;
	j = myDiscTypes->GetCount();
	while (i < j)
	{
		discType = myDiscTypes->GetItem(i);
		if (discType->brand->EqualsICase(brand, brandLen))
		{
			discTypeList->Add(discType);
			ret++;
		}
		i++;
	}
	return ret;
}

Int32 SSWR::DiscDB::DiscDBEnv::NewDVDVideo(const UTF8Char *anime, const UTF8Char *series, const UTF8Char *volume, const UTF8Char *dvdType)
{
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("insert into DVDVIDEO (ANIME, SERIES, VOLUME, DISCTYPE) values ("));
	sql.AppendStrUTF8(anime);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(series);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(volume);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(dvdType);
	sql.AppendCmdC(UTF8STRC(")"));
	if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) > 0)
	{
		DVDVideoInfo *dvdVideo = MemAlloc(DVDVideoInfo, 1);
		dvdVideo->videoId = this->db->GetLastIdentity32();
		dvdVideo->anime = Text::String::NewNotNull(anime);
		if (series)
		{
			dvdVideo->series = Text::String::NewNotNull(series);
		}
		else
		{
			dvdVideo->series = 0;
		}
		if (volume)
		{
			dvdVideo->volume = Text::String::NewNotNull(volume);
		}
		else
		{
			dvdVideo->volume = 0;
		}
		dvdVideo->dvdType = Text::StrCopyNew(dvdType);
		this->dvdVideoMap->Put(dvdVideo->videoId, dvdVideo);
		return dvdVideo->videoId;
	}
	else
	{
		return -1;
	}
}

UOSInt SSWR::DiscDB::DiscDBEnv::GetDVDVideos(Data::ArrayList<DVDVideoInfo*> *dvdVideoList)
{
	dvdVideoList->AddAll(this->dvdVideoMap->GetValues());
	return this->dvdVideoMap->GetCount();
}

const SSWR::DiscDB::DiscDBEnv::DVDVideoInfo *SSWR::DiscDB::DiscDBEnv::GetDVDVideo(Int32 videoId)
{
	return this->dvdVideoMap->Get(videoId);
}

Bool SSWR::DiscDB::DiscDBEnv::NewMovies(const UTF8Char *discId, UOSInt fileId, const UTF8Char *mainTitle, const UTF8Char *type, const UTF8Char *chapter, const UTF8Char *chapterTitle, const UTF8Char *videoFormat, Int32 width, Int32 height, Int32 fps, Int32 length, const UTF8Char *audioFormat, Int32 samplingRate, Int32 bitRate, const UTF8Char *aspectRatio, const UTF8Char *remark)
{
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("insert into Movies (DiscID, FileID, MainTitle, Type, Chapter, ChapterTitle, VideoFormat, Width, Height, fps, length, AudioFormat, SamplingRate, Bitrate, AspectRatio, Remarks) values ("));
	sql.AppendStrUTF8(discId);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt32((Int32)(UInt32)fileId);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(mainTitle);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(type);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(chapter);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(chapterTitle);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(videoFormat);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt32(width);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt32(height);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt32(fps);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt32(length);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(audioFormat);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt32(samplingRate);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt32(bitRate);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(aspectRatio);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(remark);
	sql.AppendCmdC(UTF8STRC(")"));
	return this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) > 0;
}

Bool SSWR::DiscDB::DiscDBEnv::AddMD5(IO::IStreamData *fd)
{
	Parser::FileParser::MD5Parser parser;
	parser.SetCodePage(65001);
	IO::FileCheck *fileChk = (IO::FileCheck*)parser.ParseFile(fd, 0, IO::ParserType::FileCheck);
	if (fileChk == 0)
	{
		return false;
	}
	if (fileChk->GetCheckType() != IO::FileCheck::CheckType::MD5)
	{
		DEL_CLASS(fileChk);
		return false;
	}
	Text::StringBuilderUTF8 sbDiscId;
	Text::String *s = fd->GetFullName();
	UOSInt i;
	UOSInt j;
	UOSInt k;
	i = s->LastIndexOf(IO::Path::PATH_SEPERATOR);
	sbDiscId.AppendC(&s->v[i + 1], s->leng - i - 1);
	i = sbDiscId.IndexOf('.');
	if (i != INVALID_INDEX)
	{
		sbDiscId.RemoveChars(sbDiscId.GetCharCnt() - i);
	}

	Data::StringUTF8Map<Int32> nameMap;
	Text::StringBuilderUTF8 sb;
	DB::SQLBuilder sql(this->db);
	DB::DBReader *r;
	sql.Clear();
	sql.AppendCmdC(UTF8STRC("select Name, FileID from BurntFile where DiscID = "));
	sql.AppendStrUTF8(sbDiscId.ToString());
	r = db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
	{
		while (r->ReadNext())
		{
			sb.ClearStr();
			r->GetStr(0, &sb);
			nameMap.Put(sb.ToString(), r->GetInt32(1));
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
		sql.AppendCmdC(UTF8STRC("update BurntFile set filemd5 = "));
		sql.AppendStrUTF8(sbuff);
		sql.AppendCmdC(UTF8STRC(" where DiscID = "));
		sql.AppendStrUTF8(sbDiscId.ToString());
		const UTF8Char *fileName = fileChk->GetEntryName(i);
		k = Text::StrIndexOfChar(&fileName[1], '\\');
		if (nameMap.GetIndex(&fileName[k + 2]) >= 0)
		{
			sql.AppendCmdC(UTF8STRC(" and FileID = "));
			sql.AppendInt32(nameMap.Get(&fileName[k + 2]));
		}
		else
		{
			sql.AppendCmdC(UTF8STRC(" and Name = "));
			sql.AppendStrUTF8(&fileName[k + 2]);
		}
		db->ExecuteNonQueryC(sql.ToString(), sql.GetLength());
		i++;
	}
	DEL_CLASS(fileChk);
	return true;
}
