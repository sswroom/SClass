#include "Stdafx.h"
#include "DB/DBReader.h"
#include "DB/MDBFile.h"
#include "DB/MySQLConn.h"
#include "DB/ODBCConn.h"
#include "DB/SQLBuilder.h"
#include "IO/FileCheck.h"
#include "IO/IniFile.h"
#include "IO/Path.h"
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
	r = this->db->ExecuteReader((const UTF8Char*)"select DiscID, DiscTypeID, BurntDate, Status from BurntDisc");
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
	r = this->db->ExecuteReader((const UTF8Char*)"select DiscTypeID, Name, Description from DVDType");
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
	r = this->db->ExecuteReader((const UTF8Char*)"select ID, Name from Category");
	if (r)
	{
		while (r->ReadNext())
		{
			cate = MemAlloc(CategoryInfo, 1);
			sb.ClearStr();
			r->GetStr(0, &sb);
			cate->id = Text::StrCopyNew(sb.ToString());
			sb.ClearStr();
			r->GetStr(1, &sb);
			cate->name = Text::StrCopyNew(sb.ToString());
			this->cateMap->Put(cate->id, cate);
		}
		this->db->CloseReader(r);
	}

	DiscTypeInfo *discType;
	r = this->db->ExecuteReader((const UTF8Char*)"select DiscTypeID, Brand, Name, Speed, DVDType, MadeIn, MID, TID, Revision, QCTest, Remark from DiscType");
	if (r)
	{
		while (r->ReadNext())
		{
			discType = MemAlloc(DiscTypeInfo, 1);
			sb.ClearStr();
			r->GetStr(0, &sb);
			discType->discTypeId = Text::StrCopyNew(sb.ToString());
			sb.ClearStr();
			r->GetStr(1, &sb);
			discType->brand = Text::StrCopyNew(sb.ToString());
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
	r = this->db->ExecuteReader((const UTF8Char*)"select VIDEOID, ANIME, SERIES, VOLUME, DISCTYPE from DVDVIDEO order by VIDEOID");
	if (r)
	{
		while (r->ReadNext())
		{
			dvdVideo = MemAlloc(DVDVideoInfo, 1);
			dvdVideo->videoId = r->GetInt32(0);
			sb.ClearStr();
			r->GetStr(1, &sb);
			dvdVideo->anime = Text::StrCopyNew(sb.ToString());
			sb.ClearStr();
			r->GetStr(2, &sb);
			dvdVideo->series = Text::StrCopyNew(sb.ToString());
			sb.ClearStr();
			r->GetStr(3, &sb);
			dvdVideo->volume = Text::StrCopyNew(sb.ToString());
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
		const UTF8Char *csptr;
		if ((csptr = cfg->GetValue((const UTF8Char*)"DSN")) != 0)
		{
			this->db = DB::ODBCConn::CreateDBTool(csptr, cfg->GetValue((const UTF8Char*)"UID"), cfg->GetValue((const UTF8Char*)"PWD"), cfg->GetValue((const UTF8Char*)"Schema"), this->log, (const UTF8Char*)"DB: ");
		}
		else if ((csptr = cfg->GetValue((const UTF8Char*)"MySQLServer")) != 0)
		{
			this->db = DB::MySQLConn::CreateDBTool(this->sockf, csptr, cfg->GetValue((const UTF8Char*)"MySQLDB"), cfg->GetValue((const UTF8Char*)"UID"), cfg->GetValue((const UTF8Char*)"PWD"), this->log, (const UTF8Char*)"DB: ");
		}
		else if ((csptr = cfg->GetValue((const UTF8Char*)"MDBFile")) != 0)
		{
			this->db = DB::MDBFile::CreateDBTool(csptr, this->log, (const UTF8Char*)"DB: ");
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
		Text::StrDelNew(cate->name);
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
		Text::StrDelNew(discType->brand);
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
		Text::StrDelNew(dvdVideo->anime);
		SDEL_TEXT(dvdVideo->series);
		SDEL_TEXT(dvdVideo->volume);
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
	sql.AppendCmd((const UTF8Char *)"insert into BurntDisc (DiscID, DiscTypeID, BurntDate, Status) values (");
	sql.AppendStrUTF8(discId);
	sql.AppendCmd((const UTF8Char *)", ");
	sql.AppendStrUTF8(discTypeId);
	sql.AppendCmd((const UTF8Char *)", ");
	sql.AppendDate(dt);
	sql.AppendCmd((const UTF8Char *)", ");
	sql.AppendInt32(0);
	sql.AppendCmd((const UTF8Char *)")");
	if (this->db->ExecuteNonQuery(sql.ToString()) > 0)
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
	sql.AppendCmd((const UTF8Char*)"insert into BurntFile (DiscID, FileID, Name, FileSize, Category, VIDEOID) values (");
	sql.AppendStrUTF8(discId);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendInt32((Int32)fileId);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(name);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendInt64((Int64)fileSize);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(category);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendInt32(videoId);
	sql.AppendCmd((const UTF8Char*)")");
	return this->db->ExecuteNonQuery(sql.ToString()) > 0;
}

UOSInt SSWR::DiscDB::DiscDBEnv::GetBurntFiles(const UTF8Char *discId, Data::ArrayList<DiscFileInfo*> *fileList)
{
	DiscFileInfo *file;
	UOSInt ret = 0;
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"select FileID, Name, FileSize, Category, VIDEOID from BurntFile where DiscID = ");
	sql.AppendStrUTF8(discId);
	sql.AppendCmd((const UTF8Char*)" order by FileID");
	DB::DBReader *r = this->db->ExecuteReader(sql.ToString());
	if (r)
	{
		Text::StringBuilderUTF8 sb;

		while (r->ReadNext())
		{
			file = MemAlloc(DiscFileInfo, 1);
			file->fileId = (UInt32)r->GetInt32(0);
			sb.ClearStr();
			r->GetStr(1, &sb);
			file->fileName = Text::StrCopyNew(sb.ToString());
			file->fileSize = (UInt64)r->GetInt64(2);
			sb.ClearStr();
			r->GetStr(3, &sb);
			Text::StrConcat(file->category, sb.ToString());
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
		Text::StrDelNew(file->fileName);
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
	sql.AppendCmd((const UTF8Char*)"update DVDType set Name = ");
	sql.AppendStrUTF8(name);
	sql.AppendCmd((const UTF8Char*)", Description = ");
	sql.AppendStrUTF8(desc);
	sql.AppendCmd((const UTF8Char*)" where DiscTypeID = ");
	sql.AppendStrUTF8(discTypeID);
	if (this->db->ExecuteNonQuery(sql.ToString()) >= 0)
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
	sql.AppendCmd((const UTF8Char*)"insert into DVDType (DiscTypeID, Name, Description) values (");
	sql.AppendStrUTF8(discTypeID);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(name);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(desc);
	sql.AppendCmd((const UTF8Char*)")");
	if (this->db->ExecuteNonQuery(sql.ToString()) > 0)
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

UOSInt SSWR::DiscDB::DiscDBEnv::GetDiscTypesByBrand(Data::ArrayList<const DiscTypeInfo*> *discTypeList, const UTF8Char *brand)
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
		if (Text::StrEqualsICase(discType->brand, brand))
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
	sql.AppendCmd((const UTF8Char*)"insert into DVDVIDEO (ANIME, SERIES, VOLUME, DISCTYPE) values (");
	sql.AppendStrUTF8(anime);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(series);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(volume);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(dvdType);
	sql.AppendCmd((const UTF8Char*)")");
	if (this->db->ExecuteNonQuery(sql.ToString()) > 0)
	{
		DVDVideoInfo *dvdVideo = MemAlloc(DVDVideoInfo, 1);
		dvdVideo->videoId = this->db->GetLastIdentity32();
		dvdVideo->anime = Text::StrCopyNew(anime);
		if (series)
		{
			dvdVideo->series = Text::StrCopyNew(series);
		}
		else
		{
			dvdVideo->series = 0;
		}
		if (volume)
		{
			dvdVideo->volume = Text::StrCopyNew(volume);
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
	sql.AppendCmd((const UTF8Char*)"insert into Movies (DiscID, FileID, MainTitle, Type, Chapter, ChapterTitle, VideoFormat, Width, Height, fps, length, AudioFormat, SamplingRate, Bitrate, AspectRatio, Remarks) values (");
	sql.AppendStrUTF8(discId);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendInt32((Int32)(UInt32)fileId);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(mainTitle);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(type);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(chapter);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(chapterTitle);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(videoFormat);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendInt32(width);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendInt32(height);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendInt32(fps);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendInt32(length);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(audioFormat);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendInt32(samplingRate);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendInt32(bitRate);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(aspectRatio);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(remark);
	sql.AppendCmd((const UTF8Char*)")");
	return this->db->ExecuteNonQuery(sql.ToString()) > 0;
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
	const UTF8Char *fileName = fd->GetFullName();
	UOSInt i;
	UOSInt j;
	UOSInt k;
	i = Text::StrLastIndexOf(fileName, IO::Path::PATH_SEPERATOR);
	sbDiscId.Append(&fileName[i + 1]);
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
	sql.AppendCmd((const UTF8Char*)"select Name, FileID from BurntFile where DiscID = ");
	sql.AppendStrUTF8(sbDiscId.ToString());
	r = db->ExecuteReader(sql.ToString());
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
		sql.AppendCmd((const UTF8Char*)"update BurntFile set filemd5 = ");
		sql.AppendStrUTF8(sbuff);
		sql.AppendCmd((const UTF8Char*)" where DiscID = ");
		sql.AppendStrUTF8(sbDiscId.ToString());
		fileName = fileChk->GetEntryName(i);
		k = Text::StrIndexOf(&fileName[1], '\\');
		if (nameMap.GetIndex(&fileName[k + 2]) >= 0)
		{
			sql.AppendCmd((const UTF8Char*)" and FileID = ");
			sql.AppendInt32(nameMap.Get(&fileName[k + 2]));
		}
		else
		{
			sql.AppendCmd((const UTF8Char*)" and Name = ");
			sql.AppendStrUTF8(&fileName[k + 2]);
		}
		db->ExecuteNonQuery(sql.ToString());
		i++;
	}
	DEL_CLASS(fileChk);
	return true;
}
