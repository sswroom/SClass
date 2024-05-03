#include "Stdafx.h"
#include "Crypto/Hash/CRC32R.h"
#include "Crypto/Hash/MD5.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "DB/DBReader.h"
#include "DB/ODBCConn.h"
#include "Exporter/GUIJPGExporter.h"
#include "Exporter/MDBExporter.h"
#include "IO/FileStream.h"
#include "IO/FileUtil.h"
#include "IO/IniFile.h"
#include "IO/MemoryStream.h"
#include "IO/NullStream.h"
#include "IO/Path.h"
#include "IO/StreamRecorder.h"
#include "IO/StmData/FileData.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Map/GPSTrack.h"
#include "Media/FrequencyGraph.h"
#include "Media/JPEGFile.h"
#include "Media/MediaFile.h"
#include "Media/StaticImage.h"
#include "Media/Resizer/LanczosResizerH8_8.h"
#include "Net/MySQLTCPClient.h"
#include "Parser/FullParserList.h"
#include "SSWR/OrganMgr/OrganImageItem.h"
#include "SSWR/OrganMgr/OrganImages.h"
#include "SSWR/OrganMgr/OrganEnvDB.h"
#include "SSWR/OrganMgr/OrganTripForm.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringTool.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "Text/XML.h"
#include "UI/GUICore.h"

SSWR::OrganMgr::OrganEnvDB::OrganEnvDB() : OrganEnv()
{
	UOSInt i;
	UOSInt j;
	NN<Category> cate;

	this->cfg = IO::IniFile::ParseProgConfig(0);
	this->db = 0;

	if (cfg == 0)
	{
		this->errType = ERR_CONFIG;
		return;
	}
	NN<Text::String> cfgMySQLHost;
	NN<Text::String> cfgMySQLDB;
	NN<Text::String> cfgDSN;
	Optional<Text::String> cfgUID;
	Optional<Text::String> cfgPassword;

	cfgUID = cfg->GetValue(CSTR("DBUID"));
	cfgPassword = cfg->GetValue(CSTR("DBPwd"));
	this->cfgImgDirBase = cfg->GetValue(CSTR("ImageDir")).OrNull();
	this->cfgDataPath = cfg->GetValue(CSTR("DataDir")).OrNull();
	this->cfgCacheDir = cfg->GetValue(CSTR("CacheDir")).OrNull();
	this->cfgMapFont = cfg->GetValue(CSTR("MapFont")).OrNull();
	NN<Text::String> userId;

	if (this->cfgImgDirBase == 0 || this->cfgImgDirBase->leng == 0 || this->cfgDataPath == 0 || this->cfgDataPath->leng == 0 || this->cfgCacheDir == 0 || this->cfgCacheDir->leng == 0)
	{
		this->errType = ERR_CONFIG;
		return;
	}
	if (!cfg->GetValue(CSTR("WebUser")).SetTo(userId) || !userId->ToInt32(this->userId))
	{
		this->errType = ERR_CONFIG;
		return;
	}

	i = this->cfgImgDirBase->leng;
	if (this->cfgImgDirBase->v[i - 1] == IO::Path::PATH_SEPERATOR)
	{
		this->cfgImgDirBase->v[i - 1] = 0;
	}
	this->log.AddFileLog(CSTR("OrganMgr.log"), IO::LogHandler::LogType::SingleFile, IO::LogHandler::LogGroup::NoGroup, IO::LogHandler::LogLevel::Raw, 0, false);
	if (cfg->GetValue(CSTR("MySQLDB")).SetTo(cfgMySQLDB) && cfg->GetValue(CSTR("MySQLHost")).SetTo(cfgMySQLHost))
	{
		this->db = Net::MySQLTCPClient::CreateDBTool(this->sockf, cfgMySQLHost, cfgMySQLDB, Text::String::OrEmpty(cfgUID), Text::String::OrEmpty(cfgPassword), this->log, CSTR_NULL);
	}
	else if (cfg->GetValue(CSTR("DBDSN")).SetTo(cfgDSN))
	{
		this->db = DB::ODBCConn::CreateDBTool(cfgDSN, cfgUID, cfgPassword, 0, this->log, CSTR_NULL).OrNull();
	}
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
	{
		this->errType = ERR_DB;
		return;
	}

	NN<DB::DBReader> r;
	if (db->ExecuteReader(CSTR("select cate_id, chi_name, dirName, srcDir from category")).SetTo(r))
	{
		while (r->ReadNext())
		{
			cate = MemAllocNN(Category);
			cate->cateId = r->GetInt32(0);
			cate->chiName = r->GetNewStrNN(1);
			cate->dirName = r->GetNewStrNN(2);
			cate->srcDir = r->GetNewStr(3);
			this->categories.Add(cate);
		}
		db->CloseReader(r);
	}
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("select id from webuser where id = "));
	sql.AppendInt32(this->userId);
	if (db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		if (r->ReadNext())
		{
			db->CloseReader(r);
		}
		else
		{
			db->CloseReader(r);
			this->errType = ERR_DB;
			return;
		}
	}
	else
	{
		this->errType = ERR_DB;
		return;
	}
	if (db->ExecuteReader(CSTR("select id, fileType, startTime, endTime, oriFileName, dataFileName, webuser_id from datafile order by id")).SetTo(r))
	{
		Data::DateTime dt;
		NN<DataFileInfo> dataFile;
		NN<WebUserInfo> webUser;

		while (r->ReadNext())
		{
			dataFile = MemAllocNN(DataFileInfo);
			dataFile->id = r->GetInt32(0);
			dataFile->fileType = r->GetInt32(1);
			dataFile->startTime = r->GetTimestamp(2);
			dataFile->endTime = r->GetTimestamp(3);
			dataFile->oriFileName = r->GetNewStrNN(4);
			dataFile->fileName = r->GetNewStrNN(5);
			dataFile->webUserId = r->GetInt32(6);
			this->dataFiles.Add(dataFile);

			if (dataFile->fileType == 1)
			{
				webUser = this->GetWebUser(dataFile->webUserId);
				i = webUser->gpsFileIndex.SortedInsert(dataFile->startTime);
				webUser->gpsFileObj.Insert(i, dataFile);
			}
		}
		db->CloseReader(r);
	}

	if (db->ExecuteReader(CSTR("select id, fileType, oriFileName, fileTime, lat, lon, webuser_id, species_id, captureTime, dataFileName, crcVal, rotType, camera, descript, cropLeft, cropTop, cropRight, cropBottom, location, locType from userfile order by id")).SetTo(r))
	{
		UOSInt k;
		Data::DateTime dt;
		Optional<SpeciesInfo> species;
		NN<SpeciesInfo> nnspecies;
		NN<UserFileInfo> userFile;
		Optional<WebUserInfo> webUser;
		NN<WebUserInfo> nnwebUser;
		Data::ArrayListNN<UserFileInfo> userFileList;
		userFileList.EnsureCapacity(102400);

		while (r->ReadNext())
		{
			userFile = MemAllocNN(UserFileInfo);
			userFile->id = r->GetInt32(0);
			userFile->fileType = r->GetInt32(1);
			userFile->oriFileName = r->GetNewStrNN(2);
			userFile->fileTime = r->GetTimestamp(3);
			userFile->lat = r->GetDbl(4);
			userFile->lon = r->GetDbl(5);
			userFile->webuserId = r->GetInt32(6);
			userFile->speciesId = r->GetInt32(7);
			userFile->captureTime = r->GetTimestamp(8);
			userFile->dataFileName = r->GetNewStrNN(9);
			userFile->crcVal = (UInt32)r->GetInt32(10);
			userFile->rotType = r->GetInt32(11);
			userFile->camera = r->GetNewStr(12);
			userFile->descript = r->GetNewStr(13);
			userFile->cropLeft = r->GetDbl(14);
			userFile->cropTop = r->GetDbl(15);
			userFile->cropRight = r->GetDbl(16);
			userFile->cropBottom = r->GetDbl(17);
			userFile->location = r->GetNewStr(18);
			userFile->locType = (LocType)r->GetInt32(19);
			userFileList.Add(userFile);
		}
		db->CloseReader(r);

		UserFileComparator comparator;
		Data::Sort::ArtificialQuickSort::Sort<NN<UserFileInfo>>(&userFileList, comparator);
		this->userFileMap.AllocSize(userFileList.GetCount());
		i = 0;
		j = userFileList.GetCount();
		while (i < j)
		{
			userFile = userFileList.GetItemNoCheck(i);
			this->userFileMap.Put(userFile->id, userFile);
			i++;
		}

		UserFileTimeComparator tcomparator;
		Data::Sort::ArtificialQuickSort::Sort<NN<UserFileInfo>>(&userFileList, tcomparator);
		webUser = 0;
		i = 0;
		j = userFileList.GetCount();
		while (i < j)
		{
			userFile = userFileList.GetItemNoCheck(i);
			if (!webUser.SetTo(nnwebUser) || nnwebUser->id != userFile->webuserId)
			{
				nnwebUser = this->GetWebUser(userFile->webuserId);
				webUser = nnwebUser;
			}
			k = nnwebUser->userFileIndex.SortedInsert(userFile->fileTime);
			nnwebUser->userFileObj.Insert(k, userFile);

			i++;
		}

		UserFileSpeciesComparator scomparator;
		Data::Sort::ArtificialQuickSort::Sort<NN<UserFileInfo>>(&userFileList, scomparator);
		species = 0;
		i = 0;
		j = userFileList.GetCount();
		while (i < j)
		{
			userFile = userFileList.GetItemNoCheck(i);
			if (!species.SetTo(nnspecies) || nnspecies->id != userFile->speciesId)
			{
				species = this->GetSpeciesInfoCreate(userFile->speciesId);
			}
			if (species.SetTo(nnspecies))
			{
				nnspecies->files.Add(userFile);
			}

			i++;
		}
	}

	if (db->ExecuteReader(CSTR("select id, species_id, crcVal, imgUrl, srcUrl, prevUpdated, cropLeft, cropTop, cropRight, cropBottom, location from webfile")).SetTo(r))
	{
		Optional<SpeciesInfo> species;
		NN<SpeciesInfo> nnspecies;
		NN<WebFileInfo> wfile;
		Data::ArrayListNN<WebFileInfo> fileList;

		while (r->ReadNext())
		{
			wfile = MemAllocNN(WebFileInfo);
			wfile->id = r->GetInt32(0);
			wfile->speciesId = r->GetInt32(1);
			wfile->crcVal = (UInt32)r->GetInt32(2);
			wfile->imgUrl = r->GetNewStrNN(3);
			wfile->srcUrl = r->GetNewStrNN(4);
			wfile->cropLeft = r->GetDbl(6);
			wfile->cropTop = r->GetDbl(7);
			wfile->cropRight = r->GetDbl(8);
			wfile->cropBottom = r->GetDbl(9);
			wfile->location = r->GetNewStrNN(10);
			fileList.Add(wfile);
		}
		db->CloseReader(r);

		WebFileSpeciesComparator comparator;
		Data::Sort::ArtificialQuickSort::Sort<NN<WebFileInfo>>(&fileList, comparator);
		species = 0;
		i = 0;
		j = fileList.GetCount();
		while (i < j)
		{
			wfile = fileList.GetItemNoCheck(i);
			if (!species.SetTo(nnspecies) || nnspecies->id != wfile->speciesId)
			{
				species = this->GetSpeciesInfoCreate(wfile->speciesId);
			}
			if (species.SetTo(nnspecies))
			{
				nnspecies->wfileMap.Put(wfile->id, wfile);
			}
			i++;
		}
	}

	BooksInit();
	this->errType = ERR_NONE;
}

SSWR::OrganMgr::OrganEnvDB::~OrganEnvDB()
{
	if (this->db)
	{
		DEL_CLASS(this->db);
		this->db = 0;
	}
	DEL_CLASS(this->cfg);
}

Text::String *SSWR::OrganMgr::OrganEnvDB::GetDataDir()
{
	return this->cfgDataPath;
}

Text::String *SSWR::OrganMgr::OrganEnvDB::GetCacheDir()
{
	return this->cfgCacheDir;
}

Text::CString SSWR::OrganMgr::OrganEnvDB::GetMapFont()
{
	if (this->cfgMapFont)
	{
		return this->cfgMapFont->ToCString();
	}
	else
	{
		return CSTR("Arial");
	}
}

UOSInt SSWR::OrganMgr::OrganEnvDB::GetGroupItems(NN<Data::ArrayListNN<OrganGroupItem>> items, Optional<OrganGroup> grp)
{
	NN<DB::DBReader> r;
	Int32 grpId;
	Int32 photoGrp;
	Int32 photoSp;
	NN<OrganGroup> nngrp;
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return 0;
	if (!grp.SetTo(nngrp))
	{
		grpId = 0;
		photoGrp = 0;
		photoSp = 0;
	}
	else if (nngrp->GetGroupId() == -1)
	{
		return 0;
	}
	else
	{
		grpId = nngrp->GetGroupId();
		photoGrp = nngrp->GetPhotoGroup();
		photoSp = nngrp->GetPhotoSpecies();
	}
	DB::SQLBuilder sql(db);
	Text::StringBuilderUTF8 sb;
	Bool found = false;
	UOSInt retCnt = 0;
	sql.AppendCmdC(CSTR("select id, group_type, eng_name, chi_name, description, photo_group, photo_species, idKey, flags from "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" where parent_id = "));
	sql.AppendInt32(grpId);
	sql.AppendCmdC(CSTR(" and cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	sql.AppendCmdC(CSTR(" order by eng_name"));
	if (db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		while (r->ReadNext())
		{
			found = true;
			Int32 photoGroup;
			Int32 photoSpecies;
			Int32 flags;
			photoGroup = -1;
			if (!r->IsNull(5))
				photoGroup = r->GetInt32(5);
			photoSpecies = -1;
			if (!r->IsNull(6))
				photoSpecies = r->GetInt32(6);
			NN<OrganGroup> newGrp;
			NEW_CLASSNN(newGrp, OrganGroup());
			newGrp->SetGroupId(r->GetInt32(0));
			sb.ClearStr();
			r->GetStr(3, sb);
			newGrp->SetCName(sb.ToCString());
			sb.ClearStr();
			r->GetStr(2, sb);
			newGrp->SetEName(sb.ToCString());
			newGrp->SetGroupType(r->GetInt32(1));
			sb.ClearStr();
			r->GetStr(4, sb);
			newGrp->SetDesc(sb.ToCString());
			newGrp->SetPhotoGroup(photoGroup);
			newGrp->SetPhotoSpecies(photoSpecies);
			sb.ClearStr();
			r->GetStr(7, sb);
			newGrp->SetIDKey(sb.ToCString());
			if (newGrp->GetGroupId() == photoGrp)
				newGrp->SetIsDefault(true);
			flags = r->GetInt32(8);
			newGrp->SetAdminOnly(flags & 1);
			items->Add(newGrp);
			retCnt++;
		}
		db->CloseReader(r);
	}

    if (!found)
	{
		sql.Clear();
		sql.AppendCmdC(CSTR("SELECT id, chi_name, sci_name, eng_name, description, dirName, photo, idKey, flags, photoId, mapColor, photoWId FROM species where group_id = "));
		sql.AppendInt32(grpId);
		sql.AppendCmdC(CSTR(" and cate_id = "));
		sql.AppendInt32(this->currCate->cateId);
		sql.AppendCmdC(CSTR(" order by sci_name"));
		if (db->ExecuteReader(sql.ToCString()).SetTo(r))
		{
			while (r->ReadNext())
			{
				found = true;
				NN<OrganSpecies> sp;
				NEW_CLASSNN(sp, OrganSpecies());
				
				sp->SetSpeciesId(r->GetInt32(0));
				sb.ClearStr();
				r->GetStr(1, sb);
				sp->SetCName(sb.ToCString());
				sb.ClearStr();
				r->GetStr(2, sb);
				sp->SetSName(sb.ToCString());
				sb.ClearStr();
				r->GetStr(3, sb);
				sp->SetEName(sb.ToCString());
				sb.ClearStr();
				r->GetStr(4, sb);
				sp->SetDesc(sb.ToCString());
				sb.ClearStr();
				r->GetStr(5, sb);
				sp->SetDirName(sb.ToCString());
				sb.ClearStr();
				r->GetStr(6, sb);
				sp->SetPhoto(sb.ToCString());
				sb.ClearStr();
				r->GetStr(7, sb);
				sp->SetIDKey(sb.ToCString());
				sp->SetFlags(r->GetInt32(8));
				sp->SetPhotoId(r->GetInt32(9));
				sp->SetMapColor((UInt32)r->GetInt32(10));
				sp->SetPhotoWId(r->GetInt32(11));
				if (sp->GetSpeciesId() == photoSp)
					sp->SetIsDefault(true);
				items->Add(sp);
				retCnt++;
			}
			db->CloseReader(r);
		}
	}
	return retCnt;
}

UOSInt SSWR::OrganMgr::OrganEnvDB::GetGroupImages(NN<Data::ArrayListNN<OrganImageItem>> items, NN<OrganGroup> grp)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	NN<DB::DBReader> r;
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return 0;
	if (grp->GetGroupId() == -1)
		return 0;
	DB::SQLBuilder sql(db);
	Text::StringBuilderUTF8 sb;
	Bool exist;
	UOSInt retCnt = 0;

	sql.Clear();
	sql.AppendCmdC(CSTR("SELECT id, chi_name, sci_name, eng_name, description, dirName, photo, idKey, flags, photoId, photoWId FROM species where group_id = "));
	sql.AppendInt32(grp->GetGroupId());
	sql.AppendCmdC(CSTR(" and cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	sql.AppendCmdC(CSTR(" order by sci_name"));
	if (db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		while (r->ReadNext())
		{
			Int32 photoId;
			Int32 photoWId;
			Int32 spId = r->GetInt32(0);

			NN<OrganImageItem> item;
			NEW_CLASSNN(item, OrganImageItem(this->userId));
			
			sb.ClearStr();
			r->GetStr(1, sb);
			sb.AppendC(UTF8STRC(" ("));
			r->GetStr(2, sb);
			sb.AppendC(UTF8STRC(")"));
			item->SetDispName(sb.ToCString());

			if (spId == grp->GetPhotoSpecies())
				item->SetIsCoverPhoto(true);

			photoId = r->GetInt32(9);
			photoWId = r->GetInt32(10);
			if (photoId != 0)
			{
				NN<UserFileInfo> userFile;
				if (this->userFileMap.Get(photoId).SetTo(userFile))
				{
					item->SetFullName(userFile->dataFileName.Ptr());
					item->SetFileType(OrganImageItem::FileType::UserFile);
					item->SetUserFile(userFile);
				}
			}
			else if (photoWId != 0)
			{
				NN<SpeciesInfo> sp;
				if (this->speciesMap.Get(spId).SetTo(sp))
				{
					NN<WebFileInfo> webFile;
					if (sp->wfileMap.Get(photoWId).SetTo(webFile))
					{
						item->SetFullName(webFile->imgUrl.Ptr());
						item->SetFileType(OrganImageItem::FileType::WebFile);
						item->SetWebFile(webFile);
					}
				}
			}
			else
			{
				sb.ClearStr();
				if (!this->cateIsFullDir)
				{
					sb.Append(this->cfgImgDirBase);
					sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
				}
				sb.AppendOpt(this->currCate->srcDir);
				sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
				r->GetStr(5, sb);
				sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
				r->GetStr(6, sb);
				sptr = Text::StrConcatC(sbuff, sb.ToString(), sb.GetLength());

				exist = false;
				if (!exist)
				{
					sptr2 = Text::StrConcatC(sptr, UTF8STRC(".jpg"));
					if (IO::Path::GetPathType(CSTRP(sbuff, sptr2)) == IO::Path::PathType::File)
					{
						exist = true;
						item->SetFullName(CSTRP(sbuff, sptr2));
						item->SetFileType(OrganImageItem::FileType::JPEG);
					}
				}
				if (!exist)
				{
					sptr2 = Text::StrConcatC(sptr, UTF8STRC(".tif"));
					if (IO::Path::GetPathType(CSTRP(sbuff, sptr2)) == IO::Path::PathType::File)
					{
						exist = true;
						item->SetFullName(CSTRP(sbuff, sptr2));
						item->SetFileType(OrganImageItem::FileType::TIFF);
					}
				}
				if (!exist)
				{
					sptr2 = Text::StrConcatC(sptr, UTF8STRC(".wav"));
					if (IO::Path::GetPathType(CSTRP(sbuff, sptr2)) == IO::Path::PathType::File)
					{
						exist = true;
						item->SetFullName(CSTRP(sbuff, sptr2));
						item->SetFileType(OrganImageItem::FileType::WAV);
					}
				}
				if (!exist)
				{
					sptr2 = Text::StrConcatC(sptr, UTF8STRC(".avi"));
					if (IO::Path::GetPathType(CSTRP(sbuff, sptr2)) == IO::Path::PathType::File)
					{
						exist = true;
						item->SetFullName(CSTRP(sbuff, sptr2));
						item->SetFileType(OrganImageItem::FileType::AVI);
					}
				}
				if (!exist)
				{
					sptr2 = Text::StrConcatC(sptr, UTF8STRC(".mts"));
					if (IO::Path::GetPathType(CSTRP(sbuff, sptr2)) == IO::Path::PathType::File)
					{
						exist = true;
						item->SetFullName(CSTRP(sbuff, sptr2));
						item->SetFileType(OrganImageItem::FileType::AVI);
					}
				}
				if (!exist)
				{
					sptr2 = Text::StrConcatC(sptr, UTF8STRC(".m2ts"));
					if (IO::Path::GetPathType(CSTRP(sbuff, sptr2)) == IO::Path::PathType::File)
					{
						exist = true;
						item->SetFullName(CSTRP(sbuff, sptr2));
						item->SetFileType(OrganImageItem::FileType::AVI);
					}
				}
			}
			items->Add(item);
			retCnt++;
		}
		db->CloseReader(r);
	}
	return retCnt;
}

UOSInt SSWR::OrganMgr::OrganEnvDB::GetSpeciesImages(NN<Data::ArrayListNN<OrganImageItem>> items, NN<OrganSpecies> sp)
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	Text::PString cols[4];
	Int32 newFlags = 0;
	Text::CString coverName = OPTSTR_CSTR(sp->GetPhoto());
	Int32 coverId = sp->GetPhotoId();
	Int32 coverWId = sp->GetPhotoWId();
	IO::Path::FindFileSession *sess;
	Bool isCoverPhoto;
	IO::Path::PathType pt;
	UOSInt i;
	UOSInt j;
	UOSInt retCnt = 0;
	NN<OrganImageItem> imgItem;
	if (coverId != 0)
	{
		coverName = CSTR_NULL;
	}
	else
	{
		if (coverName.v && coverName.v[0] == '*')
		{
			coverName = coverName.Substring(1);
		}
		if (coverName.v && coverName.leng == 0)
		{
			coverName = CSTR_NULL;
		}
	}

	NN<SpeciesInfo> spInfo;
	if (this->speciesMap.Get(sp->GetSpeciesId()).SetTo(spInfo))
	{
		NN<UserFileInfo> userFile;
		i = 0;
		j = spInfo->files.GetCount();
		while (i < j)
		{
			userFile = spInfo->files.GetItemNoCheck(i);
			if (coverId == userFile->id)
			{
				isCoverPhoto = true;
			}
			else
			{
				isCoverPhoto = false;
			}

			NEW_CLASSNN(imgItem, OrganImageItem(userFile->webuserId));
			imgItem->SetDispName(userFile->oriFileName.Ptr());
			imgItem->SetIsCoverPhoto(isCoverPhoto);
			imgItem->SetFullName(userFile->dataFileName.Ptr());
			imgItem->SetPhotoDate(userFile->captureTime);
			imgItem->SetRotateType(OrganImageItem::RotateType::None);
			imgItem->SetFileType(OrganImageItem::FileType::UserFile);
			imgItem->SetUserFile(userFile);
			items->Add(imgItem);
			if (userFile->fileType == 1)
			{
				newFlags |= 1;
			}
			i++;
		}

		NN<WebFileInfo> webFile;

		i = 0;
		j = spInfo->wfileMap.GetCount();
		while (i < j)
		{
			webFile = spInfo->wfileMap.GetItemNoCheck(i);

			NEW_CLASSNN(imgItem, OrganImageItem(0));
			sptr = Text::StrConcatC(Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("web\\")), webFile->id), UTF8STRC(".jpg"));
			imgItem->SetDispName(CSTRP(sbuff, sptr));
			imgItem->SetIsCoverPhoto(webFile->id == coverWId);

			sptr = this->cfgDataPath->ConcatTo(sbuff2);
			if (sptr[-1] != IO::Path::PATH_SEPERATOR)
			{
				*sptr++ = IO::Path::PATH_SEPERATOR;
			}
			sptr = Text::StrConcatC(sptr, UTF8STRC("WebFile"));
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrInt32(sptr, webFile->id >> 10);
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrInt32(sptr, webFile->id);
			sptr = Text::StrConcatC(sptr, UTF8STRC(".jpg"));
			imgItem->SetFullName(CSTRP(sbuff2, sptr));
			imgItem->SetPhotoDate(0);
			imgItem->SetRotateType(OrganImageItem::RotateType::None);
			imgItem->SetFileType(OrganImageItem::FileType::WebFile);
			imgItem->SetSrcURL(webFile->srcUrl.Ptr());
			imgItem->SetImgURL(webFile->imgUrl.Ptr());
			imgItem->SetUserFile(0);
			imgItem->SetWebFile(webFile);
			items->Add(imgItem);

			newFlags |= 8;
			i++;
		}
	}

	sptr = sbuff;
	if (!this->cateIsFullDir)
	{
		sptr = this->cfgImgDirBase->ConcatTo(sptr);
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = Text::String::OrEmpty(this->currCate->srcDir)->ConcatTo(sptr);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::String::OrEmpty(sp->GetDirName())->ConcatTo(sptr);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
	if (sess)
	{
		/*
            currImageDir = di->FullName;
			System::IO::FileInfo *fis[] = di->GetFiles();
			System::IO::FileInfo *fi;
			System::String *dispName;
			System::Int32 i = 0;
			System::Int32 j = fis->Length;
			System::Boolean isCoverPhoto;
			ImageItem *imgItem;
			System::Collections::ArrayList *settings = OrganUtil::LoadSettings(System::String::Concat(currImageDir, S"\\setting.txt"));
			ImageSetting *imgSet;
		*/

		while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
		{
			if (pt == IO::Path::PathType::File)
			{
				if (coverName.v && Text::StrStartsWithICase(sptr, coverName.v))
				{
					isCoverPhoto = true;
				}
				else
				{
					isCoverPhoto = false;
				}

				i = Text::StrLastIndexOfCharC(sptr, (UOSInt)(sptr2 - sptr), '.');
				if (i == INVALID_INDEX)
				{

				}
				else if (Text::StrEqualsICaseC(&sptr[i], (UOSInt)(sptr2 - &sptr[i]), UTF8STRC(".JPG")))
				{
					Optional<Media::EXIFData> exif = ParseJPGExif(CSTRP(sbuff, sptr2));
					NEW_CLASSNN(imgItem, OrganImageItem(this->userId));
					imgItem->SetDispName(CSTRP(sptr, sptr2));
					imgItem->SetIsCoverPhoto(isCoverPhoto);
					imgItem->SetFullName(CSTRP(sbuff, sptr2));
					NN<Media::EXIFData> nnexif;
					if (exif.SetTo(nnexif))
					{
						Data::DateTime dt;
						dt.ToLocalTime();
						if (nnexif->GetPhotoDate(dt))
						{
							imgItem->SetPhotoDate(Data::Timestamp(dt.ToInstant(), dt.GetTimeZoneQHR()));
						}
						nnexif.Delete();
					}
					imgItem->SetRotateType(OrganImageItem::RotateType::None);
					imgItem->SetFileType(OrganImageItem::FileType::JPEG);
					items->Add(imgItem);
/*
					if ((imgSet = OrganUtil::GetImageSetting(settings, imgItem->dispName)) != 0)
					{
						imgItem->rotateType = imgSet->rotateType;
					}*/
					newFlags |= 1;
					i++;
				}
				else if (Text::StrEqualsICaseC(&sptr[i], (UOSInt)(sptr2 - &sptr[i]), UTF8STRC(".TIF")))
				{
					Optional<Media::EXIFData> exif = ParseTIFExif(CSTRP(sbuff, sptr2));
					NEW_CLASSNN(imgItem, OrganImageItem(this->userId));
					imgItem->SetDispName(CSTRP(sptr, sptr2));
					imgItem->SetIsCoverPhoto(isCoverPhoto);
					imgItem->SetFullName(CSTRP(sbuff, sptr2));
					NN<Media::EXIFData> nnexif;
					if (exif.SetTo(nnexif))
					{
						Data::DateTime dt;
						if (nnexif->GetPhotoDate(dt))
						{
							imgItem->SetPhotoDate(Data::Timestamp(dt.ToInstant(), dt.GetTimeZoneQHR()));
						}
						nnexif.Delete();
					}
					imgItem->SetRotateType(OrganImageItem::RotateType::None);
					imgItem->SetFileType(OrganImageItem::FileType::TIFF);
					items->Add(imgItem);
/*
					if ((imgSet = OrganUtil::GetImageSetting(settings, imgItem->dispName)) != 0)
					{
						imgItem->rotateType = imgSet->rotateType;
					}*/
					newFlags |= 1;
					retCnt++;
				}
				else if (Text::StrEqualsICaseC(&sptr[i], (UOSInt)(sptr2 - &sptr[i]), UTF8STRC(".PCX")) ||
					Text::StrEqualsICaseC(&sptr[i], (UOSInt)(sptr2 - &sptr[i]), UTF8STRC(".GIF")) ||
					Text::StrEqualsICaseC(&sptr[i], (UOSInt)(sptr2 - &sptr[i]), UTF8STRC(".HEIC")) ||
					Text::StrEqualsICaseC(&sptr[i], (UOSInt)(sptr2 - &sptr[i]), UTF8STRC(".HEIF")) ||
					 Text::StrEqualsICaseC(&sptr[i], (UOSInt)(sptr2 - &sptr[i]), UTF8STRC(".PNG")))
				{
					NEW_CLASSNN(imgItem, OrganImageItem(this->userId));
					imgItem->SetDispName(CSTRP(sptr, sptr2));
					imgItem->SetIsCoverPhoto(isCoverPhoto);
					imgItem->SetFullName(CSTRP(sbuff, sptr2));
					imgItem->SetRotateType(OrganImageItem::RotateType::None);
					imgItem->SetFileType(OrganImageItem::FileType::TIFF);
					items->Add(imgItem);
					newFlags |= 1;
					retCnt++;
				}
				else if (Text::StrEqualsICaseC(&sptr[i], (UOSInt)(sptr2 - &sptr[i]), UTF8STRC(".AVI")) || Text::StrEqualsICaseC(&sptr[i], (UOSInt)(sptr2 - &sptr[i]), UTF8STRC(".MOV")) == 0 || Text::StrEqualsICaseC(&sptr[i], (UOSInt)(sptr2 - &sptr[i]), UTF8STRC(".MTS")) || Text::StrEqualsICaseC(&sptr[i], (UOSInt)(sptr2 - &sptr[i]), UTF8STRC(".M2TS")))
				{
					NEW_CLASSNN(imgItem, OrganImageItem(this->userId));
					imgItem->SetDispName(CSTRP(sptr, sptr2));
					imgItem->SetIsCoverPhoto(isCoverPhoto);
					imgItem->SetFullName(CSTRP(sbuff, sptr2));
					imgItem->SetRotateType(OrganImageItem::RotateType::None);
					imgItem->SetFileType(OrganImageItem::FileType::AVI);
					items->Add(imgItem);
					newFlags |= 2;
					retCnt++;
				}
				else if (Text::StrEqualsICaseC(&sptr[i], (UOSInt)(sptr2 - &sptr[i]), UTF8STRC(".WAV")))
				{
					NEW_CLASSNN(imgItem, OrganImageItem(this->userId));
					imgItem->SetDispName(CSTRP(sptr, sptr2));
					imgItem->SetIsCoverPhoto(isCoverPhoto);
					imgItem->SetFullName(CSTRP(sbuff, sptr2));
					imgItem->SetRotateType(OrganImageItem::RotateType::None);
					imgItem->SetFileType(OrganImageItem::FileType::WAV);
					items->Add(imgItem);
					newFlags |= 4;
					retCnt++;
				}
			}
		}
		IO::Path::FindFileClose(sess);

		sptr2 = Text::StrConcatC(sptr, UTF8STRC("web.txt"));
		if (IO::Path::GetPathType(CSTRP(sbuff, sptr2)) == IO::Path::PathType::File)
		{
			IO::FileStream fs(CSTRP(sbuff, sptr2), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
			Text::UTF8Reader reader(fs);

			while ((sptr2 = reader.ReadLine(sbuff2, 511)) != 0)
			{
				if (Text::StrSplitP(cols, 4, {sbuff2, (UOSInt)(sptr2 - sbuff2)}, '\t') == 3)
				{
					NEW_CLASSNN(imgItem, OrganImageItem(0));
					sptr2 = cols[0].ConcatTo(Text::StrConcatC(sptr, UTF8STRC("web\\")));
					imgItem->SetDispName(CSTRP(sptr, sptr2));
					imgItem->SetIsCoverPhoto(false);
					if (coverName.v)
					{
						if (Text::StrStartsWith(sptr, coverName.v))
						{
							imgItem->SetIsCoverPhoto(true);
						}
					}
					imgItem->SetFullName(CSTRP(sbuff, sptr2));
					imgItem->SetRotateType(OrganImageItem::RotateType::None);
					imgItem->SetFileType(OrganImageItem::FileType::Webimage);
					imgItem->SetImgURL(cols[1].ToCString());
					imgItem->SetSrcURL(cols[2].ToCString());
					newFlags |= 8;
					items->Add(imgItem);
					retCnt++;
				}
			}
		}
	}

	if (sp->GetFlags() != newFlags)
	{
		NN<DB::DBTool> db;
		if (db.Set(this->db))
		{
			DB::SQLBuilder sql(db);
			sp->SetFlags(newFlags);
			sql.AppendCmdC(CSTR("update species set flags = "));
			sql.AppendInt32(newFlags);
			sql.AppendCmdC(CSTR(" where id = "));
			sql.AppendInt32(sp->GetSpeciesId());
			db->ExecuteNonQuery(sql.ToCString());
		}
	}
	return retCnt;
}

UOSInt SSWR::OrganMgr::OrganEnvDB::GetGroupAllSpecies(NN<Data::ArrayListNN<OrganSpecies>> items, Optional<OrganGroup> grp)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return 0;
	UOSInt i;
	UOSInt j;
	Data::ArrayListInt32 allId;
	Data::ArrayListInt32 thisId;
	NN<DB::DBReader> r;
	DB::SQLBuilder sql(db);
	Text::StringBuilderUTF8 sb;
	Int32 v;
	UOSInt cnt = 0;
	NN<OrganGroup> nngrp;
	if (!grp.SetTo(nngrp))
	{
		thisId.Add(0);
	}
	else
	{
		allId.Add(nngrp->GetGroupId());
		thisId.Add(nngrp->GetGroupId());
	}
	while (thisId.GetCount() > 0)
	{
		sql.Clear();
		sql.AppendCmdC(CSTR("select id from "));
		sql.AppendCol((const UTF8Char*)"groups");
		sql.AppendCmdC(CSTR(" where parent_id in ("));
		i = 0;
		j = thisId.GetCount();
		while (i < j)
		{
			if (i > 0)
			{
				sql.AppendCmdC(CSTR(", "));
			}
			sql.AppendInt32(thisId.GetItem(i));
			i++;
		}
		sql.AppendCmdC(CSTR(") and cate_id = "));
		sql.AppendInt32(this->currCate->cateId);
		thisId.Clear();
		if (db->ExecuteReader(sql.ToCString()).SetTo(r))
		{
			while (r->ReadNext())
			{
				v = r->GetInt32(0);
				allId.Add(v);
				thisId.Add(v);
			}
			db->CloseReader(r);
		}
	}

	sql.Clear();
	sql.AppendCmdC(CSTR("SELECT id, chi_name, sci_name, eng_name, description, dirName, photo, idKey, flags, photoId, mapColor, photoWId FROM species where group_id in ("));
	i = 0;
	j = allId.GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sql.AppendCmdC(CSTR(", "));
		}
		sql.AppendInt32(allId.GetItem(i));
		i++;
	}
	sql.AppendCmdC(CSTR(") order by sci_name"));
	if (db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		NN<OrganSpecies> sp;
		while (r->ReadNext())
		{
			NEW_CLASSNN(sp, OrganSpecies());
			sp->SetSpeciesId(r->GetInt32(0));
			sb.ClearStr();
			r->GetStr(1, sb);
			sp->SetCName(sb.ToCString());
			sb.ClearStr();
			r->GetStr(2, sb);
			sp->SetSName(sb.ToCString());
			sb.ClearStr();
			r->GetStr(3, sb);
			sp->SetEName(sb.ToCString());
			sb.ClearStr();
			r->GetStr(4, sb);
			sp->SetDesc(sb.ToCString());
			sb.ClearStr();
			r->GetStr(5, sb);
			sp->SetDirName(sb.ToCString());
			sb.ClearStr();
			r->GetStr(6, sb);
			sp->SetPhoto(sb.ToCString());
			sb.ClearStr();
			r->GetStr(7, sb);
			sp->SetIDKey(sb.ToCString());
			sp->SetFlags(r->GetInt32(8));
			sp->SetPhotoId(r->GetInt32(9));
			sp->SetMapColor((UInt32)r->GetInt32(10));
			sp->SetPhotoWId(r->GetInt32(11));
			items->Add(sp);
			cnt++;
		}
		db->CloseReader(r);
	}
	return cnt;
}

UOSInt SSWR::OrganMgr::OrganEnvDB::GetGroupAllUserFile(NN<Data::ArrayListNN<UserFileInfo>> items, NN<Data::ArrayList<UInt32>> colors, Optional<OrganGroup> grp)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return 0;
	UOSInt i;
	UOSInt j;
	Data::ArrayListInt32 allId;
	Data::ArrayListInt32 thisId;
	NN<DB::DBReader> r;
	DB::SQLBuilder sql(db);
	Int32 v;
	UInt32 c;
	UOSInt cnt = items->GetCount();

	NN<OrganGroup> nngrp;
	if (!grp.SetTo(nngrp))
	{
		thisId.Add(0);
	}
	else
	{
		allId.Add(nngrp->GetGroupId());
		thisId.Add(nngrp->GetGroupId());
	}
	while (thisId.GetCount() > 0)
	{
		sql.Clear();
		sql.AppendCmdC(CSTR("select id from "));
		sql.AppendCol((const UTF8Char*)"groups");
		sql.AppendCmdC(CSTR(" where parent_id in ("));
		i = 0;
		j = thisId.GetCount();
		while (i < j)
		{
			if (i > 0)
			{
				sql.AppendCmdC(CSTR(", "));
			}
			sql.AppendInt32(thisId.GetItem(i));
			i++;
		}
		sql.AppendCmdC(CSTR(") and cate_id = "));
		sql.AppendInt32(this->currCate->cateId);
		thisId.Clear();
		if (db->ExecuteReader(sql.ToCString()).SetTo(r))
		{
			while (r->ReadNext())
			{
				v = r->GetInt32(0);
				allId.Add(v);
				thisId.Add(v);
			}
			db->CloseReader(r);
		}
	}

	sql.Clear();
	sql.AppendCmdC(CSTR("SELECT id, mapColor FROM species where group_id in ("));
	i = 0;
	j = allId.GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sql.AppendCmdC(CSTR(", "));
		}
		sql.AppendInt32(allId.GetItem(i));
		i++;
	}
	sql.AppendCmdC(CSTR(") order by sci_name"));
	if (db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		NN<SpeciesInfo> species;
		while (r->ReadNext())
		{
			v = r->GetInt32(0);
			c = (UInt32)r->GetInt32(1);
			if (this->speciesMap.Get(v).SetTo(species))
			{
				items->AddAll(species->files);
				i = species->files.GetCount();
				colors->EnsureCapacity(i);
				while (i-- > 0)
				{
					colors->Add(c);
				}
			}
		}
		db->CloseReader(r);
	}
	return items->GetCount() - cnt;
}

UOSInt SSWR::OrganMgr::OrganEnvDB::GetSpeciesItems(NN<Data::ArrayListNN<OrganGroupItem>> items, NN<Data::ArrayList<Int32>> speciesIds)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return 0;
	NN<DB::DBReader> r;
	UOSInt i;
	UOSInt j;
	DB::SQLBuilder sql(db);
	Text::StringBuilderUTF8 sb;
	UOSInt retCnt = 0;
	if (speciesIds->GetCount() <= 0)
		return 0;

	sql.Clear();
	sql.AppendCmdC(CSTR("SELECT id, chi_name, sci_name, eng_name, description, dirName, photo, idKey, flags, photoId, mapColor, photoWId FROM species where id in ("));
	i = 0;
	j = speciesIds->GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sql.AppendCmdC(CSTR(", "));
		}
		sql.AppendInt32(speciesIds->GetItem(i));
		i++;
	}
	sql.AppendCmdC(CSTR(") order by sci_name"));
	if (db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		while (r->ReadNext())
		{
			NN<OrganSpecies> sp;
			NEW_CLASSNN(sp, OrganSpecies());
			
			sp->SetSpeciesId(r->GetInt32(0));
			sb.ClearStr();
			r->GetStr(1, sb);
			sp->SetCName(sb.ToCString());
			sb.ClearStr();
			r->GetStr(2, sb);
			sp->SetSName(sb.ToCString());
			sb.ClearStr();
			r->GetStr(3, sb);
			sp->SetEName(sb.ToCString());
			sb.ClearStr();
			r->GetStr(4, sb);
			sp->SetDesc(sb.ToCString());
			sb.ClearStr();
			r->GetStr(5, sb);
			sp->SetDirName(sb.ToCString());
			sb.ClearStr();
			r->GetStr(6, sb);
			sp->SetPhoto(sb.ToCString());
			sb.ClearStr();
			r->GetStr(7, sb);
			sp->SetIDKey(sb.ToCString());
			sp->SetFlags(r->GetInt32(8));
			sp->SetPhotoId(r->GetInt32(9));
			sp->SetMapColor((UInt32)r->GetInt32(10));
			sp->SetPhotoWId(r->GetInt32(11));
			sp->SetIsDefault(false);
			items->Add(sp);
			retCnt++;
		}
		db->CloseReader(r);
	}
	return retCnt;
}

Optional<SSWR::OrganMgr::OrganGroup> SSWR::OrganMgr::OrganEnvDB::GetGroup(Int32 groupId, OutParam<Int32> parentId)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return 0;
	DB::SQLBuilder sql(db);
	NN<DB::DBReader> r;
	Int32 photoGroup;
	Int32 photoSpecies;
	OrganGroup *foundGroup = 0;
	sql.AppendCmdC(CSTR("select id, group_type, eng_name, chi_name, description, parent_id, photo_group, photo_species, idKey, flags from "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(groupId);
	if (db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		if (r->ReadNext())
		{
			Int32 flags;
			Text::StringBuilderUTF8 sb;
			photoGroup = -1;
			if (!r->IsNull(6))
				photoGroup = r->GetInt32(6);
			
			photoSpecies = -1;
			if (!r->IsNull(7))
				photoSpecies = r->GetInt32(7);

			NEW_CLASS(foundGroup, OrganGroup());
			foundGroup->SetGroupId(r->GetInt32(0));
			sb.ClearStr();
			r->GetStr(3, sb);//chiName
			foundGroup->SetCName(sb.ToCString());
			sb.ClearStr();
			r->GetStr(2, sb);//engName
			foundGroup->SetEName(sb.ToCString());
			foundGroup->SetGroupType(r->GetInt32(1));
			sb.ClearStr();
			r->GetStr(4, sb);
			foundGroup->SetDesc(sb.ToCString());
			foundGroup->SetPhotoGroup(photoGroup);
			foundGroup->SetPhotoSpecies(photoSpecies);
			sb.ClearStr();
			r->GetStr(8, sb);
			foundGroup->SetIDKey(sb.ToCString());
			flags = r->GetInt32(9);
			foundGroup->SetAdminOnly(flags & 1);

			parentId.Set(r->GetInt32(5));
		}
		db->CloseReader(r);
	}
	return foundGroup;
}

Optional<SSWR::OrganMgr::OrganSpecies> SSWR::OrganMgr::OrganEnvDB::GetSpecies(Int32 speciesId)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return 0;
	DB::SQLBuilder sql(db);
	NN<DB::DBReader> r;
	OrganSpecies *sp = 0;
	sql.AppendCmdC(CSTR("SELECT id, chi_name, sci_name, eng_name, group_id, description, dirName, photo, idKey, flags, photoId, mapColor FROM species where id = "));
	sql.AppendInt32(speciesId);
	sql.AppendCmdC(CSTR(" and cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	if (db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		if (r->ReadNext())
		{
			Text::StringBuilderUTF8 sb;
			NEW_CLASS(sp, OrganSpecies());
			sp->SetSpeciesId(r->GetInt32(0));
			sb.ClearStr();
			r->GetStr(1, sb);
			sp->SetCName(sb.ToCString());
			sb.ClearStr();
			r->GetStr(2, sb);
			sp->SetSName(sb.ToCString());
			sb.ClearStr();
			r->GetStr(3, sb);
			sp->SetEName(sb.ToCString());
			sp->SetGroupId(r->GetInt32(4));
			sb.ClearStr();
			r->GetStr(5, sb);
			sp->SetDesc(sb.ToCString());
			sb.ClearStr();
			r->GetStr(6, sb);
			sp->SetDirName(sb.ToCString());
			sb.ClearStr();
			r->GetStr(7, sb);
			sp->SetPhoto(sb.ToCString());
			sb.ClearStr();
			r->GetStr(8, sb);
			sp->SetIDKey(sb.ToCString());
			sp->SetFlags(r->GetInt32(9));
			sp->SetPhotoId(r->GetInt32(10));
			sp->SetMapColor((UInt32)r->GetInt32(11));
		}
		db->CloseReader(r);
	}
	return sp;
}

UTF8Char *SSWR::OrganMgr::OrganEnvDB::GetSpeciesDir(NN<OrganSpecies> sp, UTF8Char *sbuff)
{
	UTF8Char *sptr;
	if (Text::String::OrEmpty(this->currCate->srcDir)->IndexOf(UTF8STRC(":\\")) != INVALID_INDEX)
	{
		sptr = Text::String::OrEmpty(this->currCate->srcDir)->ConcatTo(sbuff);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		return Text::String::OrEmpty(sp->GetDirName())->ConcatTo(sptr);
	}
	else
	{
		sptr = this->cfgImgDirBase->ConcatTo(sbuff);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::String::OrEmpty(this->currCate->srcDir)->ConcatTo(sptr);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::String::OrEmpty(sp->GetDirName())->ConcatTo(sptr);
		return sptr;
	}
}

Bool SSWR::OrganMgr::OrganEnvDB::CreateSpeciesDir(NN<OrganSpecies> sp)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = GetSpeciesDir(sp, sbuff);
	if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) == IO::Path::PathType::Directory)
		return true;
	return IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
}

Bool SSWR::OrganMgr::OrganEnvDB::IsSpeciesExist(const UTF8Char *sName)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	DB::SQLBuilder sql(db);
	NN<DB::DBReader> r;
	Bool found = false;
	sql.AppendCmdC(CSTR("select id from species where sci_name = "));
	sql.AppendStrUTF8(sName);
	if (db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		found = r->ReadNext();
		db->CloseReader(r);
	}
	return found;
}

Bool SSWR::OrganMgr::OrganEnvDB::IsBookSpeciesExist(const UTF8Char *sName, NN<Text::StringBuilderUTF8> sb)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	Data::ArrayListInt32 spList;
	DB::SQLBuilder sql(db);
	NN<DB::DBReader> r;
	OSInt si;
	UOSInt i;
	UOSInt j;
	Int32 spId;
	Bool found;
	sql.AppendCmdC(CSTR("select species_id from species_book where dispName = "));
	sql.AppendStrUTF8(sName);
	if (db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		while (r->ReadNext())
		{
			spId = r->GetInt32(0);
			si = spList.SortedIndexOf(spId);
			if (si < 0)
			{
				spList.Insert((UOSInt)~si, spId);
			}
		}
		db->CloseReader(r);
	}
	if ((j = spList.GetCount()) > 0)
	{
		sql.Clear();
		sql.AppendCmdC(CSTR("select sci_name from species where id in ("));
		i = 0;
		while (i < j)
		{
			if (i > 0)
			{
				sql.AppendCmdC(CSTR(", "));
			}
			sql.AppendInt32(spList.GetItem(i));
			i++;
		}
		sql.AppendCmdC(CSTR(")"));
		if (db->ExecuteReader(sql.ToCString()).SetTo(r))
		{
			found = false;
			while (r->ReadNext())
			{
				if (found)
				{
					sb->AppendC(UTF8STRC("\r\n"));
				}
				r->GetStr(0, sb);

				found = true;
			}
			db->CloseReader(r);
		}
	}
	return spList.GetCount() > 0;

}

Bool SSWR::OrganMgr::OrganEnvDB::AddSpecies(NN<OrganSpecies> sp)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("insert into species (eng_name, chi_name, sci_name, group_id, description, dirName, idKey, cate_id, mapColor) values ("));
	sql.AppendStr(sp->GetEName());
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStr(sp->GetCName());
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStr(sp->GetSName());
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(sp->GetGroupId());
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStr(sp->GetDesc());
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStr(sp->GetDirName());
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStr(sp->GetIDKey());
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(this->currCate->cateId);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32((Int32)0xff4040ff);
	sql.AppendCmdC(CSTR(")"));
	return db->ExecuteNonQuery(sql.ToCString()) >= -1;
}

Bool SSWR::OrganMgr::OrganEnvDB::DelSpecies(NN<OrganSpecies> sp)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	DB::SQLBuilder sql(db);
	Bool succ = true;
	sql.Clear();
	sql.AppendCmdC(CSTR("update "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" set photo_group=NULL, photo_species=NULL where photo_species="));
	sql.AppendInt32(sp->GetSpeciesId());
	if (db->ExecuteNonQuery(sql.ToCString()) < -1)
		succ = false;

	sql.Clear();
	sql.AppendCmdC(CSTR("delete from species where id="));
	sql.AppendInt32(sp->GetSpeciesId());
	if (db->ExecuteNonQuery(sql.ToCString()) < -1)
		succ = false;

	sql.Clear();
	sql.AppendCmdC(CSTR("delete from species_book where species_id="));
	sql.AppendInt32(sp->GetSpeciesId());
	if (db->ExecuteNonQuery(sql.ToCString()) < -1)
		succ = false;
	return succ;
}

SSWR::OrganMgr::OrganEnvDB::FileStatus SSWR::OrganMgr::OrganEnvDB::AddSpeciesFile(NN<OrganSpecies> sp, Text::CStringNN fileName, Bool firstPhoto, Bool moveFile, OptOut<Int32> fileId)
{
	UOSInt i = fileName.LastIndexOf(IO::Path::PATH_SEPERATOR);
	UOSInt j;
	Int32 fileType = 0;
	if (fileName.EndsWithICase(UTF8STRC(".JPG")))
	{
		fileType = 1;
	}
	else if (fileName.EndsWithICase(UTF8STRC(".TIF")))
	{
		fileType = 1;
	}
	else if (fileName.EndsWithICase(UTF8STRC(".PCX")))
	{
		fileType = 1;
	}
	else if (fileName.EndsWithICase(UTF8STRC(".GIF")))
	{
		fileType = 1;
	}
	else if (fileName.EndsWithICase(UTF8STRC(".PNG")))
	{
		fileType = 1;
	}
	else if (fileName.EndsWithICase(UTF8STRC(".HEIC")))
	{
		fileType = 1;
	}
	else if (fileName.EndsWithICase(UTF8STRC(".HEIF")))
	{
		fileType = 1;
	}
	else if (fileName.EndsWithICase(UTF8STRC(".AVI")))
	{
		fileType = 2;
	}
	else if (fileName.EndsWithICase(UTF8STRC(".MOV")))
	{
		fileType = 2;
	}
	else if (fileName.EndsWithICase(UTF8STRC(".WAV")))
	{
		fileType = 3;
	}
	else
	{
		return FS_NOTSUPPORT;
	}
	if (fileType == 1)
	{
		IO::MemoryStream *mstm;
		{
			IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoBuffer);
			if (fs.IsError())
			{
				return FS_ERROR;
			}
			Data::ByteBuffer readBuff(1048576);
			UOSInt readSize;
			NEW_CLASS(mstm, IO::MemoryStream((UOSInt)fs.GetLength()));
			while (true)
			{
				readSize = fs.Read(readBuff);
				if (readSize == 0)
					break;
				mstm->Write(readBuff.Ptr(), readSize);
			}
		}

		Data::ByteArray readBuff = mstm->GetArray();
		IO::ParsedObject *pobj;
		Bool valid = false;
		Data::Timestamp fileTime = Data::Timestamp(0, Data::DateTimeUtil::GetLocalTzQhr());
		Math::Coord2DDbl pos = Math::Coord2DDbl(0, 0);
		NN<UserFileInfo> userFile;
		Text::String *camera = 0;
		UInt32 crcVal = 0;
		LocType locType = LocType::Unknown;

		{
			IO::StmData::MemoryDataRef md(readBuff);
			pobj = this->parsers.ParseFile(md);
		}
		if (pobj)
		{
			if (pobj->GetParserType() == IO::ParserType::ImageList)
			{
				valid = true;

				Media::ImageList *imgList = (Media::ImageList*)pobj;
				Media::RasterImage *img = imgList->GetImage(0, 0);
				if (img)
				{
					NN<Media::EXIFData> exif;
					if (img->exif.SetTo(exif))
					{
						exif->GetPhotoDate(fileTime);
						fileTime = fileTime.SetTimeZoneQHR(Data::DateTimeUtil::GetLocalTzQhr());
						if (fileTime.ToUnixTimestamp() >= 946684800) //Y2000
						{
							if (this->GetGPSPos(this->userId, fileTime, pos))
							{
								locType = LocType::GPSTrack;
							}
						}
						Text::CString cstr;
						Text::CString cstr2;
						cstr = exif->GetPhotoMake();
						cstr2 = exif->GetPhotoModel();
						if (cstr.v && cstr2.v)
						{
							if (cstr2.StartsWithICase(cstr.v, cstr.leng))
							{
								camera = Text::String::New(cstr2).Ptr();
							}
							else
							{
								Text::StringBuilderUTF8 sb;
								sb.Append(cstr);
								sb.AppendC(UTF8STRC(" "));
								sb.Append(cstr2);
								camera = Text::String::New(sb.ToCString()).Ptr();
							}
						}
						else if (cstr.v)
						{
							camera = Text::String::New(cstr).Ptr();
						}
						else if (cstr2.v)
						{
							camera = Text::String::New(cstr2).Ptr();
						}
						if (exif->GetPhotoLocation(pos.y, pos.x, 0, 0))
						{
							locType = LocType::PhotoExif;
						}
					}
				}

				UInt8 crcBuff[4];
				Crypto::Hash::CRC32R crc;
				crc.Calc(readBuff.Ptr(), readBuff.GetSize());
				crc.GetValue(crcBuff);
				crcVal = ReadMUInt32(crcBuff);
			}
			DEL_CLASS(pobj);
		}
		DEL_CLASS(mstm);
		if (valid)
		{
			NN<WebUserInfo> webUser = this->GetWebUser(this->userId);
			Data::Timestamp ts = fileTime;
			OSInt si;
			UOSInt j;
			UOSInt k;
			si = webUser->userFileIndex.SortedIndexOf(ts);
			if (si >= 0)
			{
				while (si > 0)
				{
					if (webUser->userFileIndex.GetItem((UOSInt)si - 1) == ts)
					{
						si--;
					}
					else
					{
						break;
					}
				}
				j = (UOSInt)si;
				k = webUser->userFileIndex.GetCount();
				while (j < k)
				{
					if (webUser->userFileIndex.GetItem(j) != ts)
						break;

					userFile = webUser->userFileObj.GetItemNoCheck(j);
					if (userFile->fileType == fileType && userFile->crcVal == crcVal)
					{
						valid = false;
						break;
					}
					j++;
				}
			}
			if (valid)
			{
				UTF8Char sbuff[512];
				UTF8Char *sptr;
				UTF8Char *dataFileName;
				sptr = this->cfgDataPath->ConcatTo(sbuff);
				if (sptr[-1] != IO::Path::PATH_SEPERATOR)
				{
					*sptr++ = IO::Path::PATH_SEPERATOR;
				}
				sptr = Text::StrConcatC(sptr, UTF8STRC("UserFile"));
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = Text::StrInt32(sptr, this->userId);
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = fileTime.ToUTCTime().ToString(sptr, "yyyyMM");
				IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
				*sptr++ = IO::Path::PATH_SEPERATOR;
				dataFileName = sptr;
				sptr = Text::StrInt64(sptr, ts.ToTicks());
				sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
				sptr = Text::StrHexVal32(sptr, crcVal);
				j = fileName.LastIndexOf('.');
				sptr = fileName.Substring(j + 1).ConcatTo(sptr);
				Bool succ;
				if (moveFile)
				{
					succ = IO::FileUtil::MoveFile(fileName, CSTRP(sbuff, sptr), IO::FileUtil::FileExistAction::Fail, 0, 0);
				}
				else
				{
					succ = IO::FileUtil::CopyFile(fileName, CSTRP(sbuff, sptr), IO::FileUtil::FileExistAction::Fail, 0, 0);
				}
				if (succ)
				{
					NN<DB::DBTool> db;
					if (db.Set(this->db))
					{
						DB::SQLBuilder sql(db);
						sql.AppendCmdC(CSTR("insert into userfile (fileType, oriFileName, fileTime, lat, lon, webuser_id, species_id, captureTime, dataFileName, crcVal, camera, cropLeft, cropTop, cropRight, cropBottom, locType) values ("));
						sql.AppendInt32(fileType);
						sql.AppendCmdC(CSTR(", "));
						sql.AppendStrC(fileName.Substring(i + 1));
						sql.AppendCmdC(CSTR(", "));
						sql.AppendTS(fileTime);
						sql.AppendCmdC(CSTR(", "));
						sql.AppendDbl(pos.GetLat());
						sql.AppendCmdC(CSTR(", "));
						sql.AppendDbl(pos.GetLon());
						sql.AppendCmdC(CSTR(", "));
						sql.AppendInt32(this->userId);
						sql.AppendCmdC(CSTR(", "));
						sql.AppendInt32(sp->GetSpeciesId());
						sql.AppendCmdC(CSTR(", "));
						sql.AppendTS(fileTime);
						sql.AppendCmdC(CSTR(", "));
						sql.AppendStrUTF8(dataFileName);
						sql.AppendCmdC(CSTR(", "));
						sql.AppendInt32((Int32)crcVal);
						sql.AppendCmdC(CSTR(", "));
						sql.AppendStrUTF8(camera->v);
						sql.AppendCmdC(CSTR(", "));
						sql.AppendDbl(0);
						sql.AppendCmdC(CSTR(", "));
						sql.AppendDbl(0);
						sql.AppendCmdC(CSTR(", "));
						sql.AppendDbl(0);
						sql.AppendCmdC(CSTR(", "));
						sql.AppendDbl(0);
						sql.AppendCmdC(CSTR(", "));
						sql.AppendInt32((Int32)locType);
						sql.AppendCmdC(CSTR(")"));
						if (db->ExecuteNonQuery(sql.ToCString()) > 0)
						{
							userFile = MemAllocNN(UserFileInfo);
							userFile->id = db->GetLastIdentity32();
							userFile->fileType = fileType;
							userFile->oriFileName = Text::String::New(fileName.Substring(i + 1));
							userFile->fileTime = fileTime;
							userFile->lat = pos.GetLat();
							userFile->lon = pos.GetLon();
							userFile->webuserId = this->userId;
							userFile->speciesId = sp->GetSpeciesId();
							userFile->captureTime = userFile->fileTime;
							userFile->dataFileName = Text::String::NewP(dataFileName, sptr);
							userFile->crcVal = crcVal;
							userFile->rotType = 0;
							userFile->camera = camera;
							userFile->descript = 0;
							userFile->cropLeft = 0;
							userFile->cropTop = 0;
							userFile->cropRight = 0;
							userFile->cropBottom = 0;
							userFile->location = 0;
							userFile->locType = locType;
							this->userFileMap.Put(userFile->id, userFile);

							NN<SpeciesInfo> species = this->GetSpeciesInfoCreate(userFile->speciesId);
							species->files.Add(userFile);

							webUser = this->GetWebUser(userFile->webuserId);
							k = webUser->userFileIndex.SortedInsert(userFile->fileTime);
							webUser->userFileObj.Insert(k, userFile);
							
							if (firstPhoto)
							{
								sp->SetPhotoId(userFile->id);
								this->SaveSpecies(sp);
							}
							fileId.Set(userFile->id);
							return FS_SUCCESS;
						}
						else
						{
							SDEL_STRING(camera);
							return FS_ERROR;
						}
					}
					else
					{
						SDEL_STRING(camera);
						return FS_ERROR;
					}
				}
				else
				{
					SDEL_STRING(camera);
					return FS_ERROR;
				}
			}
			else
			{
				SDEL_STRING(camera);
				return FS_ERROR;
			}
		}
		else
		{
			SDEL_STRING(camera);
			return FS_ERROR;
		}
	}
	else if (fileType == 3)
	{
		Crypto::Hash::CRC32R crc;
		UInt32 crcVal;
		IO::ParsedObject *pobj;
		Data::Timestamp fileTime = 0;
		NN<UserFileInfo> userFile;
		Bool valid = false;
		Media::DrawImage *graphImg = 0;
		NN<Media::DrawImage> img;
		{
			IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoBuffer);
			if (fs.IsError())
			{
				return FS_ERROR;
			}
			Data::ByteBuffer readBuff(1048576);
			UOSInt readSize;
			while (true)
			{
				readSize = fs.Read(readBuff);
				if (readSize == 0)
					break;
				crc.Calc(readBuff.Ptr(), readSize);
			}
		}
		UInt8 crcBuff[4];
		crc.GetValue(crcBuff);
		crcVal = ReadMUInt32(crcBuff);

		{
			IO::StmData::FileData fd(fileName, false);
			pobj = this->parsers.ParseFile(fd);
		}
		if (pobj)
		{
			if (pobj->GetParserType() == IO::ParserType::MediaFile)
			{
				Media::MediaFile *mediaFile = (Media::MediaFile*)pobj;
				NN<Media::IMediaSource> msrc;
				if (mediaFile->GetStream(0, 0).SetTo(msrc) && msrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
				{
					graphImg = Media::FrequencyGraph::CreateGraph(this->drawEng, (Media::IAudioSource *)msrc.Ptr(), 2048, 2048, Math::FFTCalc::WT_BLACKMANN_HARRIS, 12);
					if (graphImg)
					{
						valid = true;
					}
				}
			}
			DEL_CLASS(pobj);
		}
		if (valid)
		{
			NN<WebUserInfo> webUser = this->GetWebUser(this->userId);
			Data::Timestamp ts = Data::Timestamp(0);
			UOSInt j;
			UOSInt k;
			OSInt si;
			si = webUser->userFileIndex.SortedIndexOf(ts);
			if (si >= 0)
			{
				while (si > 0)
				{
					if (webUser->userFileIndex.GetItem((UOSInt)si - 1) == ts)
					{
						si--;
					}
					else
					{
						break;
					}
				}
				j = (UOSInt)si;
				k = webUser->userFileIndex.GetCount();
				while (j < k)
				{
					if (webUser->userFileIndex.GetItem(j) != ts)
						break;

					userFile = webUser->userFileObj.GetItemNoCheck(j);
					if (userFile->fileType == fileType && userFile->crcVal == crcVal)
					{
						valid = false;
						break;
					}
					j++;
				}
			}
			if (valid)
			{
				UTF8Char sbuff[512];
				UTF8Char *sptr;
				UTF8Char *dataFileName;
				sptr = this->cfgDataPath->ConcatTo(sbuff);
				if (sptr[-1] != IO::Path::PATH_SEPERATOR)
				{
					*sptr++ = IO::Path::PATH_SEPERATOR;
				}
				sptr = Text::StrConcatC(sptr, UTF8STRC("UserFile"));
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = Text::StrInt32(sptr, this->userId);
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = fileTime.ToUTCTime().ToString(sptr, "yyyyMM");
				IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
				*sptr++ = IO::Path::PATH_SEPERATOR;
				dataFileName = sptr;
				sptr = Text::StrInt64(sptr, ts.ToTicks());
				sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
				sptr = Text::StrHexVal32(sptr, crcVal);
				j = fileName.LastIndexOf('.');
				sptr = fileName.Substring(j + 1).ConcatTo(sptr);
				Bool succ;
				if (moveFile)
				{
					succ = IO::FileUtil::MoveFile(fileName, CSTRP(sbuff, sptr), IO::FileUtil::FileExistAction::Fail, 0, 0);
				}
				else
				{
					succ = IO::FileUtil::CopyFile(fileName, CSTRP(sbuff, sptr), IO::FileUtil::FileExistAction::Fail, 0, 0);
				}
				if (succ)
				{
					NN<DB::DBTool> db;
					if (db.Set(this->db))
					{
						DB::SQLBuilder sql(db);
						sql.AppendCmdC(CSTR("insert into userfile (fileType, oriFileName, fileTime, lat, lon, webuser_id, species_id, captureTime, dataFileName, crcVal, camera) values ("));
						sql.AppendInt32(fileType);
						sql.AppendCmdC(CSTR(", "));
						sql.AppendStrC(fileName.Substring(i + 1));
						sql.AppendCmdC(CSTR(", "));
						sql.AppendTS(fileTime);
						sql.AppendCmdC(CSTR(", "));
						sql.AppendDbl(0);
						sql.AppendCmdC(CSTR(", "));
						sql.AppendDbl(0);
						sql.AppendCmdC(CSTR(", "));
						sql.AppendInt32(this->userId);
						sql.AppendCmdC(CSTR(", "));
						sql.AppendInt32(sp->GetSpeciesId());
						sql.AppendCmdC(CSTR(", "));
						sql.AppendTS(fileTime);
						sql.AppendCmdC(CSTR(", "));
						sql.AppendStrUTF8(dataFileName);
						sql.AppendCmdC(CSTR(", "));
						sql.AppendInt32((Int32)crcVal);
						sql.AppendCmdC(CSTR(", "));
						sql.AppendStrUTF8(0);
						sql.AppendCmdC(CSTR(")"));
						if (db->ExecuteNonQuery(sql.ToCString()) > 0)
						{
							userFile = MemAllocNN(UserFileInfo);
							userFile->id = db->GetLastIdentity32();
							userFile->fileType = fileType;
							userFile->oriFileName = Text::String::New(fileName.Substring(i + 1));
							userFile->fileTime = fileTime;
							userFile->lat = 0;
							userFile->lon = 0;
							userFile->webuserId = this->userId;
							userFile->speciesId = sp->GetSpeciesId();
							userFile->captureTime = userFile->fileTime;
							userFile->dataFileName = Text::String::NewP(dataFileName, sptr);
							userFile->crcVal = crcVal;
							userFile->rotType = 0;
							userFile->camera = 0;
							userFile->descript = 0;
							userFile->cropLeft = 0;
							userFile->cropTop = 0;
							userFile->cropRight = 0;
							userFile->cropBottom = 0;
							userFile->location = 0;
							userFile->locType = LocType::Unknown;
							this->userFileMap.Put(userFile->id, userFile);

							NN<SpeciesInfo> species = this->GetSpeciesInfoCreate(userFile->speciesId);
							species->files.Add(userFile);

							webUser = this->GetWebUser(userFile->webuserId);
							j = webUser->userFileIndex.SortedInsert(userFile->fileTime);
							webUser->userFileObj.Insert(j, userFile);
							
							if (firstPhoto)
							{
								sp->SetPhotoId(userFile->id);
								this->SaveSpecies(sp);
							}
							fileId.Set(userFile->id);

							sptr = this->cfgDataPath->ConcatTo(sbuff);
							if (sptr[-1] != IO::Path::PATH_SEPERATOR)
							{
								*sptr++ = IO::Path::PATH_SEPERATOR;
							}
							sptr = Text::StrConcatC(sptr, UTF8STRC("UserFile"));
							*sptr++ = IO::Path::PATH_SEPERATOR;
							sptr = Text::StrInt32(sptr, this->userId);
							*sptr++ = IO::Path::PATH_SEPERATOR;
							sptr = fileTime.ToUTCTime().ToString(sptr, "yyyyMM");
							IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
							*sptr++ = IO::Path::PATH_SEPERATOR;
							sptr = Text::StrInt64(sptr, ts.ToTicks());
							sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
							sptr = Text::StrHexVal32(sptr, crcVal);
							sptr = Text::StrConcatC(sptr, UTF8STRC(".png"));
							if (img.Set(graphImg))
							{
								IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
								img->SavePng(fs);
								this->drawEng->DeleteImage(img);
							}

							return FS_SUCCESS;
						}
						else
						{
							if (img.Set(graphImg))
							{
								this->drawEng->DeleteImage(img);
							}
							return FS_ERROR;
						}
					}
					else
					{
						if (img.Set(graphImg))
						{
							this->drawEng->DeleteImage(img);
						}
						return FS_ERROR;
					}
				}
				else
				{
					if (img.Set(graphImg))
					{
						this->drawEng->DeleteImage(img);
					}
					return FS_ERROR;
				}
			}
			else
			{
				if (img.Set(graphImg))
				{
					this->drawEng->DeleteImage(img);
				}
				return FS_ERROR;
			}
		}
		else
		{
			return FS_ERROR;
		}
	}
	else
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		sptr = this->GetSpeciesDir(sp, sbuff);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = fileName.Substring(i + 1).ConcatTo(sptr);
		if (IO::FileUtil::CopyFile(fileName, CSTRP(sbuff, sptr), IO::FileUtil::FileExistAction::Fail, 0, 0))
		{
			if (firstPhoto)
			{
				sptr = fileName.Substring(i + 1).ConcatTo(sbuff);
				j = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '.');
				sbuff[j] = 0;
				sp->SetPhoto({sbuff, j});
				this->SaveSpecies(sp);
			}
			return FS_SUCCESS;
		}
		else
		{
			return FS_ERROR;
		}
	}
}

SSWR::OrganMgr::OrganEnvDB::FileStatus SSWR::OrganMgr::OrganEnvDB::AddSpeciesWebFile(NN<OrganSpecies> sp, NN<Text::String> srcURL, NN<Text::String> imgURL, IO::Stream *stm, UTF8Char *webFileName)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return SSWR::OrganMgr::OrganEnvDB::FS_ERROR;
	UTF8Char sbuff2[2048];
	UTF8Char *sptr2;
	Int32 id;
	Bool firstPhoto = false;
	Bool found = false;
	UOSInt i;
	NN<OrganImageItem> imgItem;
	NN<WebFileInfo> wfile;
	NN<SpeciesInfo> spInfo;
	{
		Data::ArrayListNN<OrganImageItem> imgItems;
		this->GetSpeciesImages(imgItems, sp);
		i = imgItems.GetCount();
		firstPhoto = i == 0;
		while (i-- > 0)
		{
			imgItem = imgItems.GetItemNoCheck(i);
			if (imgItem->GetFileType() == OrganImageItem::FileType::Webimage || imgItem->GetFileType() == OrganImageItem::FileType::WebFile)
			{
				if (imgItem->GetImgURL()->Equals(imgURL))
				{
					found = true;
				}
			}
			imgItem.Delete();
		}
	}

	if (found)
	{
		return SSWR::OrganMgr::OrganEnvDB::FS_ERROR;
	}

	UInt32 crcVal;
	UInt8 crcBuff[4];
	Crypto::Hash::CRC32R crc;
	crc.Calc(imgURL->v, imgURL->leng);
	crc.GetValue(crcBuff);
	crcVal = ReadMUInt32(crcBuff);
	if (crcVal == 0)
	{
		return SSWR::OrganMgr::OrganEnvDB::FS_ERROR;
	}

	IO::MemoryStream mstm;
	while ((i = stm->Read(BYTEARR(sbuff2))) > 0)
	{
		mstm.Write(sbuff2, i);
	}
	if (mstm.GetLength() <= 0)
	{
		return SSWR::OrganMgr::OrganEnvDB::FS_ERROR;
	}

	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("insert into webfile (species_id, crcVal, imgUrl, srcUrl, prevUpdated, cropLeft, cropTop, cropRight, cropBottom, location) values ("));
	sql.AppendInt32(sp->GetSpeciesId());
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32((Int32)crcVal);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStr(imgURL);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStr(srcURL);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(0);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendDbl(0);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendDbl(0);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendDbl(0);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendDbl(0);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrUTF8((const UTF8Char*)"");
	sql.AppendCmdC(CSTR(")"));
	if (db->ExecuteNonQuery(sql.ToCString()) > 0)
	{
		id = db->GetLastIdentity32();
		
		wfile = MemAllocNN(WebFileInfo);
		wfile->id = id;
		wfile->speciesId = sp->GetSpeciesId();
		wfile->imgUrl = imgURL->Clone();
		wfile->srcUrl = srcURL->Clone();
		wfile->location = Text::String::NewEmpty();
		wfile->crcVal = crcVal;
		wfile->cropLeft = 0;
		wfile->cropTop = 0;
		wfile->cropRight = 0;
		wfile->cropBottom = 0;

		sptr2 = this->cfgDataPath->ConcatTo(sbuff2);
		if (sptr2[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr2++ = IO::Path::PATH_SEPERATOR;
		}
		sptr2 = Text::StrConcatC(sptr2, UTF8STRC("WebFile"));
		*sptr2++ = IO::Path::PATH_SEPERATOR;
		sptr2 = Text::StrInt32(sptr2, id >> 10);
		IO::Path::CreateDirectory(CSTRP(sbuff2, sptr2));

		*sptr2++ = IO::Path::PATH_SEPERATOR;
		sptr2 = Text::StrInt32(sptr2, id);
		sptr2 = Text::StrConcatC(sptr2, UTF8STRC(".jpg"));

		UInt8 *buff = mstm.GetBuff(i);
		{
			IO::FileStream fs(CSTRP(sbuff2, sptr2), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			fs.Write(buff, i);
		}

		if (firstPhoto)
		{
			sp->SetPhotoWId(id);
			this->SaveSpecies(sp);
		}
		spInfo = this->GetSpeciesInfoCreate(sp->GetSpeciesId());
		spInfo->wfileMap.Put(wfile->id, wfile);

		if (webFileName)
		{
			Text::StrConcatC(Text::StrInt32(Text::StrConcatC(webFileName, UTF8STRC("web\\")), id), UTF8STRC(".jpg"));
		}
	}
	else
	{
		return SSWR::OrganMgr::OrganEnvDB::FS_ERROR;
	}
	return SSWR::OrganMgr::OrganEnvDB::FS_SUCCESS;
}

SSWR::OrganMgr::OrganEnvDB::FileStatus SSWR::OrganMgr::OrganEnvDB::AddSpeciesWebFileOld(NN<OrganSpecies> sp, Text::String *srcURL, Text::String *imgURL, NN<IO::Stream> stm, UTF8Char *webFileName)
{
	UTF8Char sbuff[512];
	UTF8Char fileName[32];
	UTF8Char *fileNameEnd;
	UTF8Char *sptr;
	UTF8Char *sptrEnd;
	sptr = this->GetSpeciesDir(sp, sbuff);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(sptr, UTF8STRC("web"));
	if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) == IO::Path::PathType::Unknown)
	{
		IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
	}

	Bool firstPhoto = false;
	Bool found = false;
	UOSInt i;
	NN<OrganImageItem> imgItem;
	{
		Data::ArrayListNN<OrganImageItem> imgItems;
		this->GetSpeciesImages(imgItems, sp);
		i = imgItems.GetCount();
		firstPhoto = i == 0;
		while (i-- > 0)
		{
			imgItem = imgItems.GetItemNoCheck(i);
			if (imgItem->GetFileType() == OrganImageItem::FileType::Webimage || imgItem->GetFileType() == OrganImageItem::FileType::WebFile)
			{
				if (Text::StringTool::Equals(imgItem->GetImgURL(), imgURL))
				{
					found = true;
				}
			}
			imgItem.Delete();
		}
	}

	if (found)
	{
		return SSWR::OrganMgr::OrganEnvDB::FS_ERROR;
	}

	UInt32 crcVal;
	UInt8 crcBuff[4];
	Crypto::Hash::CRC32R crc;
	crc.Calc(imgURL->v, imgURL->leng);
	crc.GetValue(crcBuff);
	crcVal = ReadMUInt32(crcBuff);
	
	i = Text::StrLastIndexOfCharC(imgURL->v, imgURL->leng, '.');
	if ((imgURL->leng - i - 1) > 4)
	{
		fileNameEnd = Text::StrConcatC(Text::StrHexVal32(fileName, crcVal), UTF8STRC(".jpg"));
	}
	else
	{
		fileNameEnd = Text::StrConcatC(Text::StrHexVal32(fileName, crcVal), &imgURL->v[i], imgURL->leng - i);
	}
	if (crcVal == 0)
	{
		return SSWR::OrganMgr::OrganEnvDB::FS_ERROR;
	}

	Bool succ;
	sptr[0] = IO::Path::PATH_SEPERATOR;
	sptrEnd = Text::StrConcatC(sptr + 1, fileName, (UOSInt)(fileNameEnd - fileName));
	IO::StreamRecorder *recorder;
	NEW_CLASS(recorder, IO::StreamRecorder({sbuff, (UOSInt)(sptrEnd - sbuff)}));
	succ = recorder->AppendStream(stm);
	succ = succ && (recorder->GetRecordedLength() > 0);
	DEL_CLASS(recorder);

	if (!succ)
	{
		IO::Path::DeleteFile(sbuff);
		return SSWR::OrganMgr::OrganEnvDB::FS_ERROR;
	}

	{
		Text::StringBuilderUTF8 sb;
		sptr = Text::StrConcatC(sptr, UTF8STRC(".txt"));
		IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Text::UTF8Writer writer(fs);
		writer.WriteSignature();
		sb.AppendP(fileName, fileNameEnd);
		sb.AppendC(UTF8STRC("\t"));
		sb.Append(imgURL);
		sb.AppendC(UTF8STRC("\t"));
		sb.Append(srcURL);
		writer.WriteLine(sb.ToCString());
	}

	if (firstPhoto)
	{
		i = Text::StrLastIndexOfCharC(fileName, (UOSInt)(fileNameEnd - fileName), '.');
		fileName[i] = 0;
		fileNameEnd = &fileName[i];
		sptr = Text::StrConcatC(sbuff, UTF8STRC("web"));
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrConcatC(sptr, fileName, i);
		sp->SetPhoto(CSTRP(sbuff, sptr));
		this->SaveSpecies(sp);
	}
	if (webFileName)
	{
		UTF8Char *sptr = Text::StrConcatC(webFileName, UTF8STRC("web"));
		*sptr++ = IO::Path::PATH_SEPERATOR;
		Text::StrConcatC(sptr, fileName, (UOSInt)(fileNameEnd - fileName));
	}
	return SSWR::OrganMgr::OrganEnvDB::FS_SUCCESS;
}

Bool SSWR::OrganMgr::OrganEnvDB::UpdateSpeciesWebFile(NN<OrganSpecies> sp, NN<WebFileInfo> wfile, Text::String *srcURL, Text::String *location)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("update webfile set srcUrl="));
	sql.AppendStr(srcURL);
	sql.AppendCmdC(CSTR(", location="));
	sql.AppendStr(location);
	sql.AppendCmdC(CSTR(" where id="));
	sql.AppendInt32(wfile->id);
	if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		wfile->srcUrl->Release();
		wfile->location->Release();
		wfile->srcUrl = srcURL->Clone();
		wfile->location = location->Clone();
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::OrganMgr::OrganEnvDB::UpdateSpeciesWebFileOld(NN<OrganSpecies> sp, const UTF8Char *webFileName, const UTF8Char *srcURL)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = this->GetSpeciesDir(sp, sbuff);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(sptr, UTF8STRC("web"));
	sptr = Text::StrConcatC(sptr, UTF8STRC(".txt"));

	UOSInt webFileNameLen = Text::StrCharCnt(webFileName);
	UOSInt srcURLLen = Text::StrCharCnt(srcURL);
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	Text::PString sarr[3];

	if (Text::StrStartsWith(webFileName, (const UTF8Char*)"web") && webFileName[3] == IO::Path::PATH_SEPERATOR)
	{
		webFileName = &webFileName[4];
	}

	Bool found = false;
	IO::MemoryStream mstm;
	{
		Text::UTF8Writer writer(mstm);
		writer.WriteSignature();

		IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Text::UTF8Reader reader(fs);
		sb.ClearStr();
		while (reader.ReadLine(sb, 4095))
		{
			sb2.ClearStr();
			sb2.AppendC(sb.ToString(), sb.GetLength());
			if (Text::StrSplitP(sarr, 3, sb, '\t') == 3)
			{
				if (Text::StrEqualsC(sarr[0].v, sarr[0].leng, webFileName, webFileNameLen))
				{
					found = true;
					sb2.ClearStr();
					sb2.AppendC(sarr[0].v, sarr[0].leng);
					sb2.AppendC(UTF8STRC("\t"));
					sb2.AppendC(sarr[1].v, sarr[1].leng);
					sb2.AppendC(UTF8STRC("\t"));
					sb2.AppendC(srcURL, srcURLLen);
				}
			}
			writer.WriteLine(sb2.ToCString());
			sb.ClearStr();
		}
	}
	if (found)
	{
		IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		UOSInt size;
		UInt8 *buff = mstm.GetBuff(size);
		fs.Write(buff, size);
	}
	return true;
}

Bool SSWR::OrganMgr::OrganEnvDB::SaveSpecies(NN<OrganSpecies> sp)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("update species set eng_name="));
	sql.AppendStr(sp->GetEName());
	sql.AppendCmdC(CSTR(", chi_name="));
	sql.AppendStr(sp->GetCName());
	sql.AppendCmdC(CSTR(", sci_name="));
	sql.AppendStr(sp->GetSName());
	sql.AppendCmdC(CSTR(", description="));
	sql.AppendStr(sp->GetDesc());
	sql.AppendCmdC(CSTR(",dirName="));
	sql.AppendStr(sp->GetDirName());
	sql.AppendCmdC(CSTR(", photo="));
	sql.AppendStr(sp->GetPhoto());
	sql.AppendCmdC(CSTR(", photoId="));
	sql.AppendInt32(sp->GetPhotoId());
	sql.AppendCmdC(CSTR(", photoWId="));
	sql.AppendInt32(sp->GetPhotoWId());
	sql.AppendCmdC(CSTR(", idKey="));
	sql.AppendStr(sp->GetIDKey());
	sql.AppendCmdC(CSTR(", mapColor="));
	sql.AppendInt32((Int32)sp->GetMapColor());
	sql.AppendCmdC(CSTR(" where id="));
	sql.AppendInt32(sp->GetSpeciesId());
	return db->ExecuteNonQuery(sql.ToCString()) >= -1;
}

Bool SSWR::OrganMgr::OrganEnvDB::SaveGroup(NN<OrganGroup> grp)
{
	Int32 flags = 0;
	if (grp->GetAdminOnly())
	{
		flags |= 1;
	}
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("update "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" set group_type="));
	sql.AppendInt32(grp->GetGroupType());
	sql.AppendCmdC(CSTR(", eng_name="));
	sql.AppendStr(grp->GetEName());
	sql.AppendCmdC(CSTR(", chi_name="));
	sql.AppendStr(grp->GetCName());
	sql.AppendCmdC(CSTR(", description="));
	sql.AppendStr(grp->GetDesc());
	sql.AppendCmdC(CSTR(",idKey="));
	sql.AppendStr(grp->GetIDKey());
	sql.AppendCmdC(CSTR(",flags="));
	sql.AppendInt32(flags);
	sql.AppendCmdC(CSTR(" where id="));
	sql.AppendInt32(grp->GetGroupId());
	return db->ExecuteNonQuery(sql.ToCString()) >= -1;
}

UOSInt SSWR::OrganMgr::OrganEnvDB::GetGroupCount(Int32 groupId)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return 0;
	DB::SQLBuilder sql(db);
	NN<DB::DBReader> r;
	sql.AppendCmdC(CSTR("select count(*) from "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" where parent_id = "));
	sql.AppendInt32(groupId);
	sql.AppendCmdC(CSTR(" and cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	if (db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		UOSInt cnt = 0;
		if (r->ReadNext())
		{
			cnt = (UInt32)r->GetInt32(0);
		}
		db->CloseReader(r);
		return cnt;
	}
	else
	{
		return 0;
	}
}

UOSInt SSWR::OrganMgr::OrganEnvDB::GetSpeciesCount(Int32 groupId)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return 0;
	DB::SQLBuilder sql(db);
	NN<DB::DBReader> r;
	sql.AppendCmdC(CSTR("select count(*) from species where group_id = "));
	sql.AppendInt32(groupId);
	sql.AppendCmdC(CSTR(" and cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	if (db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		UOSInt cnt = 0;
		if (r->ReadNext())
		{
			cnt = (UInt32)r->GetInt32(0);
		}
		db->CloseReader(r);
		return cnt;
	}
	else
	{
		return 0;
	}
}

Bool SSWR::OrganMgr::OrganEnvDB::AddGroup(NN<OrganGroup> grp, Int32 parGroupId)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	Int32 flags = 0;
	if (grp->GetAdminOnly())
	{
		flags |= 1;
	}
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("insert into "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" (group_type, eng_name, chi_name, description, parent_id, idKey, cate_id, flags) values ("));
	sql.AppendInt32(grp->GetGroupType());
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStr(grp->GetEName());
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStr(grp->GetCName());
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStr(grp->GetDesc());
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(parGroupId);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStr(grp->GetIDKey());
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(this->currCate->cateId);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(flags);
	sql.AppendCmdC(CSTR(")"));
	return db->ExecuteNonQuery(sql.ToCString()) == 1;
}

Bool SSWR::OrganMgr::OrganEnvDB::DelGroup(Int32 groupId)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("delete from "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(groupId);
	sql.AppendCmdC(CSTR(" and cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	return db->ExecuteNonQuery(sql.ToCString()) == 1;
}

Bool SSWR::OrganMgr::OrganEnvDB::SetGroupDefSp(NN<OrganGroup> grp, NN<OrganImageItem> img)
{
/*
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	DB::SQLBuilder sql(db);
	Int32 oldId = grp->GetPhotoSpecies();
	grp->SetPhotoGroup(-1);
	grp->SetPhotoSpecies(img->->GetSpeciesId());
	if (oldId == -1)
	{
		sql.AppendCmdC(CSTR("update "));
		sql.AppendCol((const UTF8Char*)"groups");
		sql.AppendCmdC(CSTR(" set photo_group=NULL, photo_species="));
		sql.AppendInt32(sp->GetSpeciesId());
		sql.AppendCmdC(CSTR(" where id="));
		sql.AppendInt32(grp->GetGroupId());
		db->ExecuteNonQuery(sql.ToCString());
	}
	else
	{
		sql.AppendCmdC(CSTR("update "));
		sql.AppendCml((const UTF8Char*)"groups");
		sql.AppendCmdC(CSTR(" set photo_group=NULL, photo_species="));
		sql.AppendInt32(sp->GetSpeciesId());
		sql.AppendCmdC(CSTR(" where photo_species="));
		sql.AppendInt32(oldId);
		db->ExecuteNonQuery(sql.ToCString());
	}
	sp->SetIsDefault(true);*/
	return true;
}

Bool SSWR::OrganMgr::OrganEnvDB::MoveGroups(NN<Data::ArrayListNN<OrganGroup>> grpList, NN<OrganGroup> destGroup)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	DB::SQLBuilder sql(db);
	Bool found;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = grpList->GetCount();
	sql.AppendCmdC(CSTR("update "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" set parent_id="));
	sql.AppendInt32(destGroup->GetGroupId());
	sql.AppendCmdC(CSTR(" where id in ("));
	found = false;
	while (i < j)
	{
		if (found)
		{
			sql.AppendCmdC(CSTR(", "));
		}
		else
		{
			found = true;
		}
		sql.AppendInt32(grpList->GetItemNoCheck(i)->GetGroupId());
		i++;
	}
	sql.AppendCmdC(CSTR(")"));
	return db->ExecuteNonQuery(sql.ToCString()) >= 0;
}

Bool SSWR::OrganMgr::OrganEnvDB::MoveSpecies(NN<Data::ArrayListNN<OrganSpecies>> spList, NN<OrganGroup> destGroup)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	DB::SQLBuilder sql(db);
	DB::SQLBuilder sql2(db);
	UOSInt i;
	UOSInt j;
	Bool found;
	NN<OrganSpecies> sp;
	sql.AppendCmdC(CSTR("update species set group_id="));
	sql.AppendInt32(destGroup->GetGroupId());
	sql.AppendCmdC(CSTR(" where id in ("));
	sql2.AppendCmdC(CSTR("update "));
	sql2.AppendCol((const UTF8Char*)"groups");
	sql2.AppendCmdC(CSTR(" set photo_group=NULL, photo_species=NULL where photo_species in ("));
	found = false;
	i = 0;
	j = spList->GetCount();
	while (i < j)
	{
		if (found)
		{
			sql.AppendCmdC(CSTR(", "));
			sql2.AppendCmdC(CSTR(", "));
		}
		else
		{
			found = true;
		}
		sp = spList->GetItemNoCheck(i);
		sql.AppendInt32(sp->GetSpeciesId());
		sql2.AppendInt32(sp->GetSpeciesId());
		i++;
	}
	sql.AppendCmdC(CSTR(")"));
	sql2.AppendCmdC(CSTR(")"));
	db->ExecuteNonQuery(sql2.ToCString());
	return db->ExecuteNonQuery(sql.ToCString()) > 0;
}

Bool SSWR::OrganMgr::OrganEnvDB::MoveImages(NN<Data::ArrayListNN<OrganImages>> imgList, NN<OrganSpecies> destSp, NN<UI::GUIForm> frm)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptrEnd;
	UTF8Char *sptr2;
	sptr = this->GetSpeciesDir(destSp, sbuff);
	Bool moveWeb = false;
	UOSInt i;
	UOSInt j;
	NN<OrganImages> img;
	NN<UserFileInfo> userFile;
	NN<WebFileInfo> wfile;
	i = 0;
	j = imgList->GetCount();
	if (j <= 0)
	{
		return true;
	}
	while (i < j)
	{
		img = imgList->GetItemNoCheck(i);
		if (img->GetImgItem()->GetFileType() == OrganImageItem::FileType::UserFile && img->GetImgItem()->GetUserFile().SetTo(userFile))
		{
			if (userFile->speciesId != destSp->GetSpeciesId())
			{
				UOSInt oldInd;
				NN<SpeciesInfo> sp;
				if (this->speciesMap.Get(userFile->speciesId).SetTo(sp))
				{
					oldInd = sp->files.GetCount();
					while (oldInd-- > 0)
					{
						if (sp->files.GetItemNoCheck(oldInd)->id == userFile->id)
						{
							sp->files.RemoveAt(oldInd);
							break;
						}
					}
				}
				userFile->speciesId = destSp->GetSpeciesId();
				sp = this->GetSpeciesInfoCreate(destSp->GetSpeciesId());
				sp->files.Add(userFile);
				DB::SQLBuilder sql(db);
				sql.AppendCmdC(CSTR("update userfile set species_id = "));
				sql.AppendInt32(userFile->speciesId);
				sql.AppendCmdC(CSTR(" where id = "));
				sql.AppendInt32(userFile->id);
				db->ExecuteNonQuery(sql.ToCString());
			}
		}
		else if (img->GetImgItem()->GetFileType() == OrganImageItem::FileType::WebFile && img->GetImgItem()->GetWebFile().SetTo(wfile))
		{
			if (wfile->speciesId != destSp->GetSpeciesId())
			{
				NN<SpeciesInfo> sp;
				if (this->speciesMap.Get(wfile->speciesId).SetTo(sp))
				{
					sp->wfileMap.Remove(wfile->id);
				}
				wfile->speciesId = destSp->GetSpeciesId();
				sp = this->GetSpeciesInfoCreate(destSp->GetSpeciesId());
				sp->wfileMap.Put(wfile->id, wfile);
				DB::SQLBuilder sql(db);
				sql.AppendCmdC(CSTR("update webfile set species_id = "));
				sql.AppendInt32(wfile->speciesId);
				sql.AppendCmdC(CSTR(" where id = "));
				sql.AppendInt32(wfile->id);
				db->ExecuteNonQuery(sql.ToCString());
			}
		}
		else
		{
			if (img->GetImgItem()->GetFileType() == OrganImageItem::FileType::Webimage)
			{
				sptr2 = sptr;
				if (!moveWeb)
				{
					moveWeb = true;
					sptr[0] = IO::Path::PATH_SEPERATOR;
					sptrEnd = Text::StrConcatC(sptr + 1, UTF8STRC("web"));
					IO::Path::CreateDirectory(CSTRP(sbuff, sptrEnd));
				}
			}
			else
			{
				sptr[0] = 0;
				IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
				sptr2 = sptr;
			}
			*sptr2++ = IO::Path::PATH_SEPERATOR;
			sptr2 = img->GetImgItem()->GetDispName()->ConcatTo(sptr2);
			if (img->GetImgItem()->GetFullName()->Equals(sbuff, (UOSInt)(sptr2 - sbuff)))
			{
				break;
			}
			if (!IO::FileUtil::MoveFile(img->GetImgItem()->GetFullName()->ToCString(), CSTRP(sbuff, sptr2), IO::FileUtil::FileExistAction::Fail, 0, 0))
			{
				Text::StringBuilderUTF8 sb;
				NN<Text::String> s;
				s = Text::String::NewNotNull(L"移動");
				sb.Append(s);
				s->Release();
				sb.Append(img->GetImgItem()->GetDispName());
				s = Text::String::NewNotNull(L"時出錯, 要繼續?");
				sb.Append(s);
				s->Release();
				s = Text::String::NewNotNull(L"錯誤");
				if (!frm->GetUI()->ShowMsgYesNo(sb.ToCString(), s->ToCString(), frm))
				{
					s->Release();
					break;
				}
				s->Release();
			}
		}
		i++;
	}

	if (moveWeb)
	{
		Text::StringBuilderUTF8 sb;
		Text::CString name;
		const UTF8Char *srcDir = 0;
		sptr[0] = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrConcatC(sptr + 1, UTF8STRC("web.txt"));
		{
			j = i;
			i = 0;
			IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Append, IO::FileShare::DenyAll, IO::FileStream::BufferType::Normal);
			Text::UTF8Writer writer(fs);
			while (i < j)
			{
				img = imgList->GetItemNoCheck(i);
				if (img->GetImgItem()->GetFileType() == OrganImageItem::FileType::Webimage)
				{
					srcDir = img->GetSrcImgDir();
					name = img->GetImgItem()->GetDispName()->ToCString();
					name = name.Substring(name.LastIndexOf(IO::Path::PATH_SEPERATOR) + 1);
					sb.ClearStr();
					sb.Append(name);
					sb.AppendC(UTF8STRC("\t"));
					sb.Append(img->GetImgItem()->GetImgURL());
					sb.AppendC(UTF8STRC("\t"));
					sb.Append(img->GetImgItem()->GetSrcURL());
					writer.WriteLine(sb.ToCString());
				}
				i++;
			}
		}

		if (srcDir)
		{
			Text::StringBuilderUTF8 sb2;
			sptr = Text::StrConcat(sbuff, srcDir);
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrConcatC(sptr, UTF8STRC("web.txt"));
			Data::ArrayListStringNN webLines;
			Text::PString sarr[4];
			Bool found;
			{
				IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
				Text::UTF8Reader reader(fs);
				sb.ClearStr();
				while (reader.ReadLine(sb, 512))
				{
					sb2.AppendC(sb.ToString(), sb.GetLength());
					if (Text::StrSplitP(sarr, 4, sb2, '\t') == 3)
					{
						found = false;
						i = j;
						while (i-- > 0)
						{
							img = imgList->GetItemNoCheck(i);
							if (img->GetImgItem()->GetFileType() == OrganImageItem::FileType::Webimage)
							{
								if (img->GetImgItem()->GetDispName()->EndsWith(sarr[0].v, sarr[0].leng))
								{
									found = true;
									break;
								}
							}
						}
						if (!found)
						{
							webLines.Add(Text::String::New(sb.ToCString()));
						}
					}
					sb.ClearStr();
					sb2.ClearStr();
				}
			}

			sptr = Text::StrConcat(sbuff, srcDir);
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrConcatC(sptr, UTF8STRC("web.txt"));
			if (webLines.GetCount() > 0)
			{
				IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				Text::UTF8Writer writer(fs);
				writer.WriteLines(webLines.Iterator());
				webLines.FreeAll();
			}
			else
			{
				IO::Path::DeleteFile(sbuff);
			}
		}
	}
	return true;
}

Bool SSWR::OrganMgr::OrganEnvDB::CombineSpecies(NN<OrganSpecies> destSp, NN<OrganSpecies> srcSp)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("update species_book set species_id = "));
	sql.AppendInt32(destSp->GetSpeciesId());
	sql.AppendCmdC(CSTR(" where species_id = "));
	sql.AppendInt32(srcSp->GetSpeciesId());
	if (db->ExecuteNonQuery(sql.ToCString()) < 0)
	{
		return false;
	}
	sql.Clear();
	sql.AppendCmdC(CSTR("update userfile set species_id = "));
	sql.AppendInt32(destSp->GetSpeciesId());
	sql.AppendCmdC(CSTR(" where species_id = "));
	sql.AppendInt32(srcSp->GetSpeciesId());
	if (db->ExecuteNonQuery(sql.ToCString()) < 0)
	{
		return false;
	}
	sql.Clear();
	sql.AppendCmdC(CSTR("delete from species where id = "));
	sql.AppendInt32(srcSp->GetSpeciesId());
	if (db->ExecuteNonQuery(sql.ToCString()) < 0)
	{
		return false;
	}
	NN<SpeciesInfo> spInfos;
	NN<SpeciesInfo> spInfod;
	if (this->speciesMap.Get(srcSp->GetSpeciesId()).SetTo(spInfos) && this->speciesMap.Get(destSp->GetSpeciesId()).SetTo(spInfod))
	{
		spInfod->files.AddAll(spInfos->files);
		spInfos->files.Clear();
	}
	return true;
}

UOSInt SSWR::OrganMgr::OrganEnvDB::GetWebUsers(NN<Data::ArrayListNN<OrganWebUser>> userList)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return 0;
	UOSInt initCnt = userList->GetCount();
	NN<OrganWebUser> user;
	NN<DB::DBReader> r;
	if (db->ExecuteReader(CSTR("select id, userName, watermark, userType from webuser")).SetTo(r))
	{
		while (r->ReadNext())
		{
			user = MemAllocNN(OrganWebUser);
			user->id = r->GetInt32(0);
			user->userName = r->GetNewStrNN(1);
			user->watermark = r->GetNewStrNN(2);
			user->userType = (UserType)r->GetInt32(3);
			userList->Add(user);
		}
		db->CloseReader(r);
	}
	return userList->GetCount() - initCnt;
}

Bool SSWR::OrganMgr::OrganEnvDB::AddWebUser(const UTF8Char *userName, const UTF8Char *pwd, const UTF8Char *watermark, UserType userType)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("select id from webuser where userName = "));
	sql.AppendStrUTF8(userName);
	NN<DB::DBReader> r;
	if (!db->ExecuteReader(sql.ToCString()).SetTo(r))
		return false;
	if (r->ReadNext())
	{
		db->CloseReader(r);
		return false;
	}
	db->CloseReader(r);
	sql.Clear();
	sql.AppendCmdC(CSTR("insert into webuser (userName, pwd, watermark, userType) values ("));
	sql.AppendStrUTF8(userName);
	sql.AppendCmdC(CSTR(", "));
	{
		UOSInt len = Text::StrCharCnt(pwd);
		UInt8 md5Value[16];
		UTF8Char sbuff[33];
		Crypto::Hash::MD5 md5;
		md5.Calc(pwd, len);
		md5.GetValue(md5Value);
		Text::StrHexBytes(sbuff, md5Value, 16, 0);
		sql.AppendStrUTF8(sbuff);
	}
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrUTF8(watermark);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(userType);
	sql.AppendCmdC(CSTR(")"));
	return db->ExecuteNonQuery(sql.ToCString()) >= 1;
}

Bool SSWR::OrganMgr::OrganEnvDB::ModifyWebUser(Int32 id, const UTF8Char *userName, const UTF8Char *pwd, const UTF8Char *watermark)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("select id from webuser where userName = "));
	sql.AppendStrUTF8(userName);
	NN<DB::DBReader> r;
	if (!db->ExecuteReader(sql.ToCString()).SetTo(r))
		return false;
	if (r->ReadNext())
	{
		if (r->GetInt32(0) != id)
		{
			db->CloseReader(r);
			return false;
		}
	}
	db->CloseReader(r);
	sql.Clear();
	sql.AppendCmdC(CSTR("update webuser set userName = "));
	sql.AppendStrUTF8(userName);
	if (pwd != 0)
	{
		UOSInt len = Text::StrCharCnt(pwd);
		UInt8 md5Value[16];
		UTF8Char sbuff[33];
		Crypto::Hash::MD5 md5;
		md5.Calc(pwd, len);
		md5.GetValue(md5Value);
		Text::StrHexBytes(sbuff, md5Value, 16, 0);
		sql.AppendCmdC(CSTR(", pwd = "));
		sql.AppendStrUTF8(sbuff);
	}
	sql.AppendCmdC(CSTR(", watermark = "));
	sql.AppendStrUTF8(watermark);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(id);
	return db->ExecuteNonQuery(sql.ToCString()) >= 0;
}

void SSWR::OrganMgr::OrganEnvDB::ReleaseWebUsers(NN<Data::ArrayListNN<OrganWebUser>> userList)
{
	NN<OrganWebUser> user;
	UOSInt i = userList->GetCount();
	while (i-- > 0)
	{
		user = userList->GetItemNoCheck(i);
		user->userName->Release();
		user->watermark->Release();
		MemFreeNN(user);
	}
	userList->Clear();
}

Bool SSWR::OrganMgr::OrganEnvDB::IsSpeciesBookExist(Int32 speciesId, Int32 bookId)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	DB::SQLBuilder sql(db);
	NN<DB::DBReader> r;
	sql.AppendCmdC(CSTR("select dispName from species_book where species_id = "));
	sql.AppendInt32(speciesId);
	sql.AppendCmdC(CSTR(" and book_id = "));
	sql.AppendInt32(bookId);
	if (!db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		if (!db->ExecuteReader(sql.ToCString()).SetTo(r))
		{
			return false;
		}
	}
	Bool found = false;
	found = r->ReadNext();
	db->CloseReader(r);
	return found;
}

Bool SSWR::OrganMgr::OrganEnvDB::NewSpeciesBook(Int32 speciesId, Int32 bookId, const UTF8Char *dispName)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("insert into species_book (species_id, book_id, dispName) values ("));
	sql.AppendInt32(speciesId);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(bookId);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrUTF8(dispName);
	sql.AppendCmdC(CSTR(")"));
	if (db->ExecuteNonQuery(sql.ToCString()) == -2)
	{
		return false;
	}
	else
	{
		return true;
	}
}

UOSInt SSWR::OrganMgr::OrganEnvDB::GetSpeciesBooks(NN<Data::ArrayListNN<SpeciesBook>> items, Int32 speciesId)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	DB::SQLBuilder sql(db);
	NN<DB::DBReader> r;
	NN<OrganBook> book;
	NN<SpeciesBook> spBook;
	OSInt i;
	UOSInt j;

	sql.AppendCmdC(CSTR("select book_id, dispName, id from species_book where species_id = "));
	sql.AppendInt32(speciesId);
	if (!db->ExecuteReader(sql.ToCString()).SetTo(r))
		return 0;
	j = 0;
	while (r->ReadNext())
	{
		i = this->bookIds->SortedIndexOf(r->GetInt32(0));
		if (i >= 0)
		{
			book = this->bookObjs->GetItemNoCheck((UOSInt)i);
			spBook = MemAllocNN(SpeciesBook);
			spBook->book = book;
			spBook->dispName = r->GetNewStrNN(1);
			spBook->id = r->GetInt32(2);
			items->Add(spBook);
			j++;
		}
	}
	db->CloseReader(r);
	return j;
}

void SSWR::OrganMgr::OrganEnvDB::ReleaseSpeciesBooks(NN<Data::ArrayListNN<SpeciesBook>> items)
{
	NN<SpeciesBook> spBook;
	UOSInt i;
	i = items->GetCount();
	while (i-- > 0)
	{
		spBook = items->GetItemNoCheck(i);
		spBook->dispName->Release();
		MemFreeNN(spBook);
	}
}

Int32 SSWR::OrganMgr::OrganEnvDB::NewBook(Text::CString title, Text::CString author, Text::CString press, const Data::Timestamp &publishDate, Text::CString url)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return 0;
	NN<OrganBook> book;
	DB::SQLBuilder sql(db);
	UOSInt i;

	sql.AppendCmdC(CSTR("insert into book (title, dispAuthor, press, publishDate, groupId, url) values ("));
	sql.AppendStrUTF8(title.v);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrUTF8(author.v);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrUTF8(press.v);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendTS(publishDate);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(0);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrUTF8(url.v);
	sql.AppendCmdC(CSTR(")"));
	if (db->ExecuteNonQuery(sql.ToCString()) < -1)
	{
		return 0;
	}
	Int32 id = db->GetLastIdentity32();
	NEW_CLASSNN(book, OrganBook());
	book->SetBookId(id);
	book->SetTitle(title);
	book->SetDispAuthor(author);
	book->SetPress(press);
	book->SetPublishDate(publishDate);
	book->SetGroupId(0);
	book->SetURL(url);
	i = this->bookIds->SortedInsert(book->GetBookId());
	this->bookObjs->Insert(i, book);
	return id;
}

Bool SSWR::OrganMgr::OrganEnvDB::AddDataFile(Text::CStringNN fileName)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	IO::ParsedObject *pobj;
	{
		IO::StmData::FileData fd(fileName, false);
		pobj = this->parsers.ParseFile(fd);
	}
	Data::Timestamp startTime;
	Data::Timestamp endTime;
	const UTF8Char *oriFileName;
	const UTF8Char *oriFileNameEnd;
	UTF8Char sbuff[512];
	const UTF8Char *dataFileName;
	Int32 fileType = 0;
	NN<DataFileInfo> dataFile;
	Bool chg = false;
	Data::Timestamp ts;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;

	i = fileName.LastIndexOf(IO::Path::PATH_SEPERATOR);
	oriFileName = &fileName.v[i + 1];
	oriFileNameEnd = fileName.GetEndPtr();

	if (pobj)
	{
		IO::ParserType t = pobj->GetParserType();
		if (t == IO::ParserType::MapLayer)
		{
			Map::MapDrawLayer *lyr = (Map::MapDrawLayer*)pobj;
			if (lyr->GetObjectClass() == Map::MapDrawLayer::OC_GPS_TRACK)
			{
				Bool found = false;
				Map::GPSTrack *gpsTrk = (Map::GPSTrack*)lyr;
				Map::GPSTrack::GPSRecord3 *recArr;
				i = 0;
				j = gpsTrk->GetTrackCnt();
				while (i < j)
				{
					recArr = gpsTrk->GetTrack(i, l);
					k = 0;
					while (k < l)
					{
						ts = Data::Timestamp(recArr[k].recTime, 0);
						if (found)
						{
							if (startTime.CompareTo(ts) > 0)
							{
								startTime = ts;
							}
							if (endTime.CompareTo(ts) < 0)
							{
								endTime = ts;
							}
						}
						else
						{
							found = true;
							startTime = ts;
							endTime = ts;
						}
						k++;
					}
					i++;
				}
				fileType = 1;
			}
		}
		else if (t == IO::ParserType::ReadingDB)
		{
			DB::ReadingDB *db = (DB::ReadingDB*)pobj;
			Data::ArrayListStringNN tables;
			db->QueryTableNames(CSTR_NULL, tables);
			if (tables.GetCount() == 2)
			{
				NN<Text::String> t1;
				NN<Text::String> t2;
				if (tables.GetItem(0).SetTo(t1) && t1->Equals(UTF8STRC("Setting")) && tables.GetItem(1).SetTo(t2) && t2->Equals(UTF8STRC("Records")))
				{
					NN<DB::DBReader> reader;
					Bool found = false;
					if (db->QueryTableData(CSTR_NULL, CSTR("Records"), 0, 0, 0, CSTR_NULL, 0).SetTo(reader))
					{
						while (reader->ReadNext())
						{
							if (!found)
							{
								found = true;
								startTime = reader->GetTimestamp(1);
							}
							endTime = reader->GetTimestamp(1);
						}
						db->CloseReader(reader);
					}

					if (found)
					{
						fileType = 2;
					}
				}
			}
			tables.FreeAll();
		}
		DEL_CLASS(pobj);
	}

	if (fileType != 0)
	{
		UTF8Char *sptr = this->cfgDataPath->ConcatTo(sbuff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC("DataFile"));
		IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
		*sptr++ = IO::Path::PATH_SEPERATOR;
		dataFileName = sptr;
		sptr = Text::StrInt32(sptr, this->userId);
		sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
		sptr = Text::StrInt64(sptr, startTime.ToTicks());
		i = fileName.LastIndexOf('.');
		if (i != INVALID_INDEX)
		{
			sptr = Text::StrConcatC(sptr, &fileName.v[i], fileName.leng - i);
		}
		if (IO::FileUtil::CopyFile(fileName, CSTRP(sbuff, sptr), IO::FileUtil::FileExistAction::Fail, 0, 0))
		{
			DB::SQLBuilder sql(db);
			sql.AppendCmdC(CSTR("insert into datafile (fileType, startTime, endTime, oriFileName, dataFileName, webuser_id) values ("));
			sql.AppendInt32(fileType);
			sql.AppendCmdC(CSTR(", "));
			sql.AppendTS(startTime);
			sql.AppendCmdC(CSTR(", "));
			sql.AppendTS(endTime);
			sql.AppendCmdC(CSTR(", "));
			sql.AppendStrUTF8(oriFileName);
			sql.AppendCmdC(CSTR(", "));
			sql.AppendStrUTF8(dataFileName);
			sql.AppendCmdC(CSTR(", "));
			sql.AppendInt32(this->userId);
			sql.AppendCmdC(CSTR(")"));
			if (db->ExecuteNonQuery(sql.ToCString()) >= 1)
			{
				chg = true;
				dataFile = MemAllocNN(DataFileInfo);
				dataFile->id = db->GetLastIdentity32();
				dataFile->fileType = fileType;
				dataFile->startTime = startTime;
				dataFile->endTime = endTime;
				dataFile->webUserId = this->userId;
				dataFile->oriFileName = Text::String::NewP(oriFileName, oriFileNameEnd);
				dataFile->fileName = Text::String::NewP(dataFileName, sptr);
				this->dataFiles.Add(dataFile);

				if (fileType == 1)
				{
					NN<WebUserInfo> webUser = this->GetWebUser(dataFile->webUserId);
					i = webUser->gpsFileIndex.SortedInsert(dataFile->startTime);
					webUser->gpsFileObj.Insert(i, dataFile);
					
					{
						IO::StmData::FileData fd(fileName, false);
						pobj = this->parsers.ParseFileType(fd, IO::ParserType::MapLayer);
					}

					if (pobj)
					{
						Map::GPSTrack *gpsTrk = (Map::GPSTrack*)pobj;

						OSInt startIndex = webUser->userFileIndex.SortedIndexOf(dataFile->startTime);
						OSInt endIndex = webUser->userFileIndex.SortedIndexOf(dataFile->endTime);
						if (startIndex < 0)
						{
							startIndex = ~startIndex;
						}
						if (endIndex < 0)
						{
							endIndex = ~endIndex - 1;
						}
						while (startIndex <= endIndex)
						{
							NN<SSWR::OrganMgr::UserFileInfo> userFile = webUser->userFileObj.GetItemNoCheck((UOSInt)startIndex);
							if (userFile->lat == 0 && userFile->lon == 0)
							{
								Math::Coord2DDbl pos = Math::Coord2DDbl(0, 0);
								pos = gpsTrk->GetPosByTime(userFile->captureTime);
								this->UpdateUserFilePos(userFile, userFile->captureTime, pos.GetLat(), pos.GetLon());
							}
							startIndex++;
						}
						DEL_CLASS(pobj);
					}
				}
			}
		}
	}
	return chg;
}

Bool SSWR::OrganMgr::OrganEnvDB::DelDataFile(NN<DataFileInfo> dataFile)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	Bool found = false;
	UTF8Char sbuff[512];
	UOSInt i;
	i = this->dataFiles.GetCount();
	while (i-- > 0)
	{
		if (dataFile == this->dataFiles.GetItemNoCheck(i))
		{
			found = true;
			break;
		}
	}
	if (!found)
		return false;

	UTF8Char *sptr = this->cfgDataPath->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = Text::StrConcatC(sptr, UTF8STRC("DataFile"));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(sptr, dataFile->fileName->v, dataFile->fileName->leng);
	if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) != IO::Path::PathType::File)
		return false;
	IO::Path::DeleteFile(sbuff);
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("delete from datafile where id = "));
	sql.AppendInt32(dataFile->id);
	db->ExecuteNonQuery(sql.ToCString());
	this->dataFiles.RemoveAt(i);
	NN<WebUserInfo> webUser;
	if (dataFile->fileType == 1 && this->userMap.Get(dataFile->webUserId).SetTo(webUser))
	{
		OSInt j;
		j = webUser->gpsFileIndex.SortedIndexOf(dataFile->startTime);
		if (j >= 0)
		{
			webUser->gpsFileIndex.RemoveAt((UOSInt)j);
			webUser->gpsFileObj.RemoveAt((UOSInt)j);
		}
	}
	ReleaseDataFile(dataFile);
	return true;
}

Bool SSWR::OrganMgr::OrganEnvDB::GetGPSPos(Int32 userId, const Data::Timestamp &ts, OutParam<Math::Coord2DDbl> pos)
{
	OSInt i;
	NN<WebUserInfo> webUser;
	NN<DataFileInfo> dataFile;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	if (this->gpsTrk == 0 || this->gpsUserId != userId || this->gpsStartTime > ts || this->gpsEndTime < ts)
	{
		SDEL_CLASS(this->gpsTrk);
		this->gpsUserId = userId;
		webUser = this->GetWebUser(userId);
		i = webUser->gpsFileIndex.SortedIndexOf(ts);
		if (i < 0)
		{
			i = ~i - 1;
		}
		if (webUser->gpsFileObj.GetItem((UOSInt)i).SetTo(dataFile))
		{
			this->gpsStartTime = dataFile->startTime;
			this->gpsEndTime = dataFile->endTime;
			sptr = this->cfgDataPath->ConcatTo(sbuff);
			if (sptr[-1] != IO::Path::PATH_SEPERATOR)
			{
				*sptr++ = IO::Path::PATH_SEPERATOR;
			}
			sptr = Text::StrConcatC(sptr, UTF8STRC("DataFile"));
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrConcatC(sptr, dataFile->fileName->v, dataFile->fileName->leng);
			Map::MapDrawLayer *lyr;
			{
				IO::StmData::FileData fd({sbuff, (UOSInt)(sptr - sbuff)}, false);
				lyr = (Map::MapDrawLayer*)this->parsers.ParseFileType(fd, IO::ParserType::MapLayer);
			}
			if (lyr)
			{
				if (lyr->GetObjectClass() == Map::MapDrawLayer::OC_GPS_TRACK)
				{
					this->gpsTrk = (Map::GPSTrack*)lyr;
				}
				else
				{
					DEL_CLASS(lyr);
				}
			}
		}
	}

	if (this->gpsTrk)
	{
		pos.Set(this->gpsTrk->GetPosByTime(ts));
		return true;
	}
	else
	{
		return false;
	}
}

Map::GPSTrack *SSWR::OrganMgr::OrganEnvDB::OpenGPSTrack(NN<DataFileInfo> dataFile)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;

	sptr = this->cfgDataPath->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = Text::StrConcatC(sptr, UTF8STRC("DataFile"));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = dataFile->fileName->ConcatTo(sptr);
	IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
	Map::GPSTrack *trk = 0;
	Map::MapDrawLayer *lyr = (Map::MapDrawLayer*)this->parsers.ParseFileType(fd, IO::ParserType::MapLayer);
	if (lyr)
	{
		if (lyr->GetObjectClass() == Map::MapDrawLayer::OC_GPS_TRACK)
		{
			trk = (Map::GPSTrack*)lyr;
		}
		else
		{
			trk = 0;
			DEL_CLASS(lyr);
		}
	}
	else
	{
		lyr = 0;
	}
	return trk;
}

void SSWR::OrganMgr::OrganEnvDB::UpdateUserFileCrop(NN<UserFileInfo> userFile, Double cropLeft, Double cropTop, Double cropRight, Double cropBottom)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("update userfile set cropLeft = "));
	sql.AppendDbl(cropLeft);
	sql.AppendCmdC(CSTR(", cropTop = "));
	sql.AppendDbl(cropTop);
	sql.AppendCmdC(CSTR(", cropRight = "));
	sql.AppendDbl(cropRight);
	sql.AppendCmdC(CSTR(", cropBottom = "));
	sql.AppendDbl(cropBottom);
	sql.AppendCmdC(CSTR(", prevUpdated = 1"));
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(userFile->id);
	if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		userFile->cropLeft = cropLeft;
		userFile->cropTop = cropTop;
		userFile->cropRight = cropRight;
		userFile->cropBottom = cropBottom;
	}
}

void SSWR::OrganMgr::OrganEnvDB::UpdateUserFileRot(NN<UserFileInfo> userFile, Int32 rotType)
{
	if (userFile->rotType != rotType)
	{
		NN<DB::DBTool> db;
		if (!db.Set(this->db))
			return;
		DB::SQLBuilder sql(db);
		sql.AppendCmdC(CSTR("update userfile set rotType = "));
		sql.AppendInt32(rotType);
		sql.AppendCmdC(CSTR(", prevUpdated = 1"));
		sql.AppendCmdC(CSTR(" where id = "));
		sql.AppendInt32(userFile->id);
		db->ExecuteNonQuery(sql.ToCString());
		userFile->rotType = rotType;
	}
}

Bool SSWR::OrganMgr::OrganEnvDB::UpdateUserFilePos(NN<UserFileInfo> userFile, const Data::Timestamp &captureTime, Double lat, Double lon)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	Bool succ = false;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("update userfile set captureTime = "));
	sql.AppendTS(captureTime);
	sql.AppendCmdC(CSTR(", lat = "));
	sql.AppendDbl(lat);
	sql.AppendCmdC(CSTR(", lon = "));
	sql.AppendDbl(lon);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(userFile->id);
	if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		succ = true;
		userFile->captureTime = captureTime;
		userFile->lat = lat;
		userFile->lon = lon;
	}
	return succ;
}

Bool SSWR::OrganMgr::OrganEnvDB::GetUserFilePath(NN<UserFileInfo> userFile, NN<Text::StringBuilderUTF8> sb)
{
	Data::Timestamp ts = userFile->fileTime.ToUTCTime();
	sb->Append(this->cfgDataPath);
	if (!sb->EndsWith(IO::Path::PATH_SEPERATOR))
	{
		sb->AppendChar(IO::Path::PATH_SEPERATOR, 1);
	}
	sb->AppendC(UTF8STRC("UserFile"));
	sb->AppendChar(IO::Path::PATH_SEPERATOR, 1);
	sb->AppendI32(userFile->webuserId);
	sb->AppendChar(IO::Path::PATH_SEPERATOR, 1);
	UTF8Char sbuff[10];
	UTF8Char *sptr;
	sptr = ts.ToString(sbuff, "yyyyMM");
	sb->AppendP(sbuff, sptr);
	sb->AppendChar(IO::Path::PATH_SEPERATOR, 1);
	sb->AppendC(userFile->dataFileName->v, userFile->dataFileName->leng);
	return true;
}

Bool SSWR::OrganMgr::OrganEnvDB::UpdateUserFileDesc(NN<UserFileInfo> userFile, const UTF8Char *descript)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	Bool succ = false;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("update userfile set descript = "));
	sql.AppendStrUTF8(descript);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(userFile->id);
	if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		succ = true;
		OPTSTR_DEL(userFile->descript);
		userFile->descript = Text::String::NewOrNullSlow(descript);
	}
	return succ;
}

Bool SSWR::OrganMgr::OrganEnvDB::UpdateUserFileLoc(NN<UserFileInfo> userFile, const UTF8Char *location)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	Bool succ = false;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("update userfile set location = "));
	sql.AppendStrUTF8(location);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(userFile->id);
	if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		succ = true;
		OPTSTR_DEL(userFile->location);
		userFile->location = Text::String::NewOrNullSlow(location);
	}
	return succ;
}

void SSWR::OrganMgr::OrganEnvDB::UpdateWebFileCrop(NN<WebFileInfo> wfile, Double cropLeft, Double cropTop, Double cropRight, Double cropBottom)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("update webfile set cropLeft = "));
	sql.AppendDbl(cropLeft);
	sql.AppendCmdC(CSTR(", cropTop = "));
	sql.AppendDbl(cropTop);
	sql.AppendCmdC(CSTR(", cropRight = "));
	sql.AppendDbl(cropRight);
	sql.AppendCmdC(CSTR(", cropBottom = "));
	sql.AppendDbl(cropBottom);
	sql.AppendCmdC(CSTR(", prevUpdated = 1"));
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(wfile->id);
	if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		wfile->cropLeft = cropLeft;
		wfile->cropTop = cropTop;
		wfile->cropRight = cropRight;
		wfile->cropBottom = cropBottom;
	}
}

void SSWR::OrganMgr::OrganEnvDB::TripReload(Int32 cateId)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return;
	NN<DB::DBReader> r;
	DB::SQLBuilder sql(db);
	UTF8Char sbuff[256];
	UTF8Char *sbuffEnd;
	UTF8Char sbuff2[256];
	UTF8Char *sbuff2End;
	this->TripRelease();

	NN<LocationType> locT;
	NN<Location> loc;
	NN<Trip> t;
	
	sql.Clear();
	sql.AppendCmdC(CSTR("select locType, engName, chiName from location_type where cate_id = "));
	sql.AppendInt32(cateId);
	sql.AppendCmdC(CSTR(" order by locType"));
	if (db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		while (r->ReadNext())
		{
			sbuffEnd = r->GetStr(1, sbuff, sizeof(sbuff));
			sbuff2End = r->GetStr(2, sbuff2, sizeof(sbuff2));
			NEW_CLASSNN(locT, LocationType(r->GetInt32(0), CSTRP(sbuff, sbuffEnd), CSTRP(sbuff2, sbuff2End)))
			this->locType.Add(locT);
		}
		db->CloseReader(r);
	}

	sql.Clear();
	sql.AppendCmdC(CSTR("select id, parentId, ename, cname, locType from location where cate_id = "));
	sql.AppendInt32(cateId);
	sql.AppendCmdC(CSTR(" order by id"));
	if (db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		while (r->ReadNext())
		{
			sbuffEnd = r->GetStr(2, sbuff, sizeof(sbuff));
			sbuff2End = r->GetStr(3, sbuff2, sizeof(sbuff2));
			NEW_CLASSNN(loc, Location(r->GetInt32(0), r->GetInt32(1), CSTRP(sbuff, sbuffEnd), CSTRP(sbuff2, sbuff2End), r->GetInt32(4)));
			this->locs.Add(loc);
		}
		db->CloseReader(r);
	}

	sql.Clear();
	sql.AppendCmdC(CSTR("select fromDate, toDate, locId from trip where cate_id = "));
	sql.AppendInt32(cateId);
	sql.AppendCmdC(CSTR(" order by fromDate"));
	if (db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		while (r->ReadNext())
		{
			NEW_CLASSNN(t, Trip(r->GetTimestamp(0), r->GetTimestamp(1), r->GetInt32(2)));
			this->trips.Add(t);
		}
		db->CloseReader(r);
	}
}

Bool SSWR::OrganMgr::OrganEnvDB::TripAdd(const Data::Timestamp &fromDate, const Data::Timestamp &toDate, Int32 locId)
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	OSInt i = this->TripGetIndex(fromDate);
	OSInt j = this->TripGetIndex(toDate);
	OSInt k = this->LocationGetIndex(locId);
	if (i < 0 && j < 0 && k >= 0)
	{
		if (i == j)
		{
			DB::SQLBuilder sql(db);
			sql.AppendCmdC(CSTR("insert into trip (fromDate, toDate, locId, cate_id) values ("));
			sql.AppendTS(fromDate);
			sql.AppendCmdC(CSTR(", "));
			sql.AppendTS(toDate);
			sql.AppendCmdC(CSTR(", "));
			sql.AppendInt32(locId);
			sql.AppendCmdC(CSTR(", "));
			sql.AppendInt32(this->currCate->cateId);
			sql.AppendCmdC(CSTR(")"));
			if (db->ExecuteNonQuery(sql.ToCString()) != -2)
			{
				NN<Trip> t;
				NEW_CLASSNN(t, Trip(fromDate, toDate, locId));
				this->trips.Insert((UOSInt)(-i - 1), t);
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

Bool SSWR::OrganMgr::OrganEnvDB::LocationUpdate(Int32 locId, Text::CString engName, Text::CString chiName)
{
	NN<Location> loc;
	if (!this->LocationGet(locId).SetTo(loc))
		return false;
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;

	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("update location set ename="));
	sql.AppendStrUTF8(engName.v);
	sql.AppendCmdC(CSTR(", cname="));
	sql.AppendStrUTF8(chiName.v);
	sql.AppendCmdC(CSTR(" where id="));
	sql.AppendInt32(locId);
	if (db->ExecuteNonQuery(sql.ToCString()) == -2)
		return false;
	else
	{
		loc->ename->Release();
		loc->cname->Release();
		loc->ename = Text::String::New(engName);
		loc->cname = Text::String::New(chiName);
		return true;
	}
}

Bool SSWR::OrganMgr::OrganEnvDB::LocationAdd(Int32 locId, Text::CString engName, Text::CString chiName)
{
	Int32 lType;
	if (locId == 0)
	{
		lType = -1;
	}
	else
	{
		NN<Location> loc;
		if (!this->LocationGet(locId).SetTo(loc))
			return false;
        lType = (Int32)this->LocationGetTypeIndex(loc->locType);
        if (lType < 0)
			return false;
	}
	
	if ((UOSInt)lType + 1 >= this->locType.GetCount())
		return false;
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return false;
	lType = this->locType.GetItemNoCheck((UInt32)lType + 1)->id;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("insert into location (parentId, ename, cname, cate_id, locType) values ("));
	sql.AppendInt32(locId);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrUTF8(engName.v);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrUTF8(chiName.v);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(this->currCate->cateId);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(lType);
	sql.AppendCmdC(CSTR(")"));
	if (db->ExecuteNonQuery(sql.ToCString()) == -2)
		return false;
	else
	{
		NN<DB::DBReader> r;
		sql.Clear();
		sql.AppendCmdC(CSTR("select max(id) from location where parentId = "));
		sql.AppendInt32(locId);
		if (db->ExecuteReader(sql.ToCString()).SetTo(r))
		{
			if (r->ReadNext())
			{
				NN<Location> l;
				NEW_CLASSNN(l, Location(r->GetInt32(0), locId, engName, chiName, lType));
				this->locs.Add(l);
				db->CloseReader(r);
				return true;
			}
			else
			{
				db->CloseReader(r);
				return false;
			}
		}
		else
		{
			return false;
		}
	}
}

void SSWR::OrganMgr::OrganEnvDB::BooksInit()
{
	NN<OrganBook> book;
	Text::StringBuilderUTF8 sb;
	NN<DB::DBReader> r;
	Data::DateTime dt;
	UOSInt i;

	NEW_CLASS(this->bookIds, Data::ArrayListInt32());
	NEW_CLASS(this->bookObjs, Data::ArrayListNN<OrganBook>());
	if (this->db->ExecuteReader(CSTR("select id, title, dispAuthor, press, publishDate, groupId, url from book")).SetTo(r))
	{
		while (r->ReadNext())
		{
			NEW_CLASSNN(book, OrganBook());
			book->SetBookId(r->GetInt32(0));
			sb.ClearStr();
			r->GetStr(1, sb);
			book->SetTitle(&sb);
			sb.ClearStr();
			r->GetStr(2, sb);
			book->SetDispAuthor(&sb);
			sb.ClearStr();
			r->GetStr(3, sb);
			book->SetPress(&sb);
			book->SetPublishDate(r->GetTimestamp(4));
			book->SetGroupId(r->GetInt32(5));
			sb.ClearStr();
			r->GetStr(6, sb);
			book->SetURL(&sb);
			i = this->bookIds->SortedInsert(book->GetBookId());
			this->bookObjs->Insert(i, book);
		}
		this->db->CloseReader(r);
	}
}

Media::ImageList *SSWR::OrganMgr::OrganEnvDB::ParseImage(NN<OrganImageItem> img, OptOut<Optional<UserFileInfo>> outUserFile, OptOut<Optional<WebFileInfo>> outWebFile)
{
	if (img->GetFileType() == OrganImageItem::FileType::UserFile)
	{
		Optional<UserFileInfo> userFile = img->GetUserFile();
		NN<UserFileInfo> nnuserFile;
		outUserFile.Set(userFile);
		outWebFile.Set(0);
		if (userFile.SetTo(nnuserFile))
		{
			UTF8Char sbuff[512];
			UTF8Char *sptr;
			Data::Timestamp ts = nnuserFile->fileTime.ToUTCTime();
			sptr = this->cfgDataPath->ConcatTo(sbuff);
			if (sptr[-1] != IO::Path::PATH_SEPERATOR)
			{
				*sptr++ = IO::Path::PATH_SEPERATOR;
			}
			sptr = Text::StrConcatC(sptr, UTF8STRC("UserFile"));
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrInt32(sptr, nnuserFile->webuserId);
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = ts.ToString(sptr, "yyyyMM");
			*sptr++ = IO::Path::PATH_SEPERATOR;
			if (nnuserFile->fileType == 3)
			{
				sptr = Text::StrInt64(sptr, nnuserFile->fileTime.ToTicks());
				sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
				sptr = Text::StrHexVal32(sptr, nnuserFile->crcVal);
				sptr = Text::StrConcatC(sptr, UTF8STRC(".png"));
			}
			else
			{
				sptr = nnuserFile->dataFileName->ConcatTo(sptr);
			}
			IO::ParsedObject *pobj;
			{
				IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
				pobj = this->parsers.ParseFile(fd);
			}
			if (pobj == 0)
			{
				return 0;
			}
			if (pobj->GetParserType() == IO::ParserType::ImageList)
			{
				Media::ImageList *imgList = (Media::ImageList*)pobj;
				if (nnuserFile->rotType != 0)
				{
					UOSInt i = imgList->GetCount();
					Media::StaticImage *simg;
					while (i-- > 0)
					{
						imgList->ToStaticImage(i);
						simg = (Media::StaticImage*)imgList->GetImage(i, 0);
						if (nnuserFile->rotType == 1)
						{
							simg->RotateImage(Media::StaticImage::RotateType::CW90);
						}
						else if (nnuserFile->rotType == 2)
						{
							simg->RotateImage(Media::StaticImage::RotateType::CW180);
						}
						else if (nnuserFile->rotType == 3)
						{
							simg->RotateImage(Media::StaticImage::RotateType::CW270);
						}
					}
				}
				return imgList;
			}
			DEL_CLASS(pobj);
			return 0;
		}
		return 0;
	}
	else if (img->GetFileType() == OrganImageItem::FileType::WebFile)
	{
		Optional<WebFileInfo> wfile = img->GetWebFile();
		NN<WebFileInfo> nnwfile;
		outUserFile.Set(0);
		outWebFile.Set(wfile);
		if (wfile.SetTo(nnwfile))
		{
			UTF8Char sbuff[512];
			UTF8Char *sptr;
			sptr = this->cfgDataPath->ConcatTo(sbuff);
			if (sptr[-1] != IO::Path::PATH_SEPERATOR)
			{
				*sptr++ = IO::Path::PATH_SEPERATOR;
			}
			sptr = Text::StrConcatC(sptr, UTF8STRC("WebFile"));
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrInt32(sptr, nnwfile->id >> 10);
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrInt32(sptr, nnwfile->id);
			sptr = Text::StrConcatC(sptr, UTF8STRC(".jpg"));

			IO::ParsedObject *pobj;
			{
				IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
				pobj = this->parsers.ParseFile(fd);
			}
			if (pobj == 0)
			{
				return 0;
			}
			if (pobj->GetParserType() == IO::ParserType::ImageList)
			{
				Media::ImageList *imgList = (Media::ImageList*)pobj;
/*				if (userFile->rotType != 0)
				{
					OSInt i = imgList->GetCount();
					Media::StaticImage *simg;
					while (i-- > 0)
					{
						imgList->ToStaticImage(i);
						simg = (Media::StaticImage*)imgList->GetImage(i, 0);
						if (userFile->rotType == 1)
						{
							simg->RotateImage(Media::StaticImage::RotateType::CW90);
						}
						else if (userFile->rotType == 2)
						{
							simg->RotateImage(Media::StaticImage::RotateType::CW180);
						}
						else if (userFile->rotType == 3)
						{
							simg->RotateImage(Media::StaticImage::RotateType::CW270);
						}
					}
				}*/
				return imgList;
			}
			DEL_CLASS(pobj);
			return 0;
		}
		return 0;
	}
	else if (img->GetFullName())
	{
		outUserFile.Set(0);
		outWebFile.Set(0);
		IO::ParsedObject *pobj;
		{
			IO::StmData::FileData fd(Text::String::OrEmpty(img->GetFullName()), false);
			pobj = this->parsers.ParseFile(fd);
		}
		if (pobj == 0)
		{
			return 0;
		}
		if (pobj->GetParserType() == IO::ParserType::ImageList)
		{
			return (Media::ImageList*)pobj;
		}
		DEL_CLASS(pobj);
		return 0;
	}
	else
	{
		outUserFile.Set(0);
		outWebFile.Set(0);
		return 0;
	}
}

Media::ImageList *SSWR::OrganMgr::OrganEnvDB::ParseSpImage(NN<OrganSpecies> sp)
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UTF8Char *cols[4];
	Text::CString coverName = OPTSTR_CSTR(sp->GetPhoto());
	IO::Path::FindFileSession *sess;
	IO::Path::PathType pt;
	UOSInt i;
	IO::ParsedObject *pobj = 0;
	if (sp->GetPhotoId() != 0)
	{
		NN<UserFileInfo> userFile;
		if (this->userFileMap.Get(sp->GetPhotoId()).SetTo(userFile))
		{
			return this->ParseFileImage(userFile);
		}
		else
		{
			return 0;
		}
	}
	else if (sp->GetPhotoWId() != 0)
	{
		NN<SpeciesInfo> spInfo;
		if (this->speciesMap.Get(sp->GetSpeciesId()).SetTo(spInfo))
		{
			NN<WebFileInfo> wfile;
			if (spInfo->wfileMap.Get(sp->GetPhotoWId()).SetTo(wfile))
			{
				return this->ParseWebImage(wfile);
			}
		}
		return 0;
	}

	if (coverName.v && coverName.v[0] == '*')
	{
		coverName = coverName.Substring(1);
	}

	sptr = sbuff;
	if (!this->cateIsFullDir)
	{
		sptr = this->cfgImgDirBase->ConcatTo(sptr);
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = Text::String::OrEmpty(this->currCate->srcDir)->ConcatTo(sptr);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::String::OrEmpty(sp->GetDirName())->ConcatTo(sptr);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr2 = Text::StrConcatC(coverName.ConcatTo(sptr), UTF8STRC(".*"));
	sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
	if (sess)
	{
		if ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
		{
			if (pt == IO::Path::PathType::File)
			{
				i = Text::StrLastIndexOfChar(sptr, '.');
				if (i == INVALID_INDEX)
				{

				}
				else if (Text::StrCompareICase(&sptr[i], (const UTF8Char*)".JPG") == 0)
				{
					IO::StmData::FileData fd(CSTRP(sbuff, sptr2), false);
					pobj = this->parsers.ParseFile(fd);
				}
				else if (Text::StrCompareICase(&sptr[i], (const UTF8Char*)".TIF") == 0)
				{
					IO::StmData::FileData fd(CSTRP(sbuff, sptr2), false);
					pobj = this->parsers.ParseFile(fd);
				}
				else if (Text::StrCompareICase(&sptr[i], (const UTF8Char*)".AVI") == 0)
				{
				}
				else if (Text::StrCompareICase(&sptr[i], (const UTF8Char*)".WAV") == 0)
				{
				}
			}
		}
		IO::Path::FindFileClose(sess);

		if (pobj == 0)
		{
			sptr2 = Text::StrConcatC(sptr, UTF8STRC("web.txt"));
			if (IO::Path::GetPathType(CSTRP(sbuff, sptr2)) == IO::Path::PathType::File)
			{
				IO::FileStream fs(CSTRP(sbuff, sptr2), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
				Text::UTF8Reader reader(fs);
				
				while (reader.ReadLine(sbuff2, 511))
				{
					if (Text::StrSplit(cols, 4, sbuff2, '\t') == 3)
					{
						sptr2 = Text::StrConcatC(sptr, UTF8STRC("web"));
						*sptr2++ = IO::Path::PATH_SEPERATOR;
						sptr2 = Text::StrConcat(sptr2, cols[0]);
						if (Text::StrStartsWith(sptr, coverName.v))
						{
							IO::StmData::FileData fd(CSTRP(sbuff, sptr2), false);
							pobj = this->parsers.ParseFile(fd);
							break;
						}
					}
				}
			}
		}
	}
	Media::ImageList *imgList = 0;
	if (pobj)
	{
		if (pobj->GetParserType() == IO::ParserType::ImageList)
		{
			imgList = (Media::ImageList*)pobj;
		}
		else
		{
			DEL_CLASS(pobj);
		}
	}
	return imgList;
}

Media::ImageList *SSWR::OrganMgr::OrganEnvDB::ParseFileImage(NN<UserFileInfo> userFile)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::ParsedObject *pobj;
	Data::Timestamp ts = userFile->fileTime.ToUTCTime();
	sptr = this->cfgDataPath->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = Text::StrConcatC(sptr, UTF8STRC("UserFile"));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, userFile->webuserId);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = ts.ToString(sptr, "yyyyMM");
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = userFile->dataFileName->ConcatTo(sptr);
	{
		IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
		pobj = this->parsers.ParseFile(fd);
	}
	if (pobj == 0)
	{
		return 0;
	}
	if (pobj->GetParserType() == IO::ParserType::ImageList)
	{
		Media::ImageList *imgList = (Media::ImageList*)pobj;
		if (userFile->rotType != 0)
		{
			UOSInt i = imgList->GetCount();
			Media::StaticImage *simg;
			while (i-- > 0)
			{
				imgList->ToStaticImage(i);
				simg = (Media::StaticImage*)imgList->GetImage(i, 0);
				if (userFile->rotType == 1)
				{
					simg->RotateImage(Media::StaticImage::RotateType::CW90);
				}
				else if (userFile->rotType == 2)
				{
					simg->RotateImage(Media::StaticImage::RotateType::CW180);
				}
				else if (userFile->rotType == 3)
				{
					simg->RotateImage(Media::StaticImage::RotateType::CW270);
				}
			}
		}
		return imgList;
	}
	DEL_CLASS(pobj);
	return 0;
}

Media::ImageList *SSWR::OrganMgr::OrganEnvDB::ParseWebImage(NN<WebFileInfo> wfile)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::ParsedObject *pobj;
	sptr = this->cfgDataPath->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = Text::StrConcatC(sptr, UTF8STRC("WebFile"));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, wfile->id >> 10);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, wfile->id);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".jpg"));
	{
		IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
		pobj = this->parsers.ParseFile(fd);
	}
	if (pobj == 0)
	{
		return 0;
	}
	if (pobj->GetParserType() == IO::ParserType::ImageList)
	{
		Media::ImageList *imgList = (Media::ImageList*)pobj;
/*		if (userFile->rotType != 0)
		{
			OSInt i = imgList->GetCount();
			Media::StaticImage *simg;
			while (i-- > 0)
			{
				imgList->ToStaticImage(i);
				simg = (Media::StaticImage*)imgList->GetImage(i, 0);
				if (userFile->rotType == 1)
				{
					simg->RotateImage(Media::StaticImage::RotateType::CW90);
				}
				else if (userFile->rotType == 2)
				{
					simg->RotateImage(Media::StaticImage::RotateType::CW180);
				}
				else if (userFile->rotType == 3)
				{
					simg->RotateImage(Media::StaticImage::RotateType::CW270);
				}
			}
		}*/
		return imgList;
	}
	DEL_CLASS(pobj);
	return 0;
}

SSWR::OrganMgr::OrganGroup *SSWR::OrganMgr::OrganEnvDB::SearchObject(const UTF8Char *searchStr, UTF8Char *resultStr, UOSInt resultStrBuffSize, Int32 *parentId)
{
	OrganGroup *foundGroup = 0;
	UOSInt searchStrLen = Text::StrCharCnt(searchStr);
	Bool found = false;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UTF8Char sbuff2[256];
	UTF8Char *sptr2;
	Int32 photoGroup;
	Int32 photoSpecies;
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return 0;
	NN<DB::DBReader> r;
	DB::SQLBuilder sql(db);
	Text::StringBuilderUTF8 sb;

	sql.AppendCmdC(CSTR("select group_id, sci_name from species where chi_name = "));
	sql.AppendStrUTF8(searchStr);
	sql.AppendCmdC(CSTR(" or sci_name = "));
	sql.AppendStrUTF8(searchStr);
	if (db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		if (r->ReadNext())
		{
			found = true;
			*parentId = r->GetInt32(0);
			r->GetStr(1, resultStr, resultStrBuffSize);
		}
		db->CloseReader(r);
	}
	if (!found)
	{
		sql.Clear();
		sql.AppendCmdC(CSTR("select group_id, sci_name from species s, species_book sb where sb.dispName = "));
		sql.AppendStrUTF8(searchStr);
		sql.AppendCmdC(CSTR(" and s.id = sb.species_id group by sb.species_id"));
		if (db->ExecuteReader(sql.ToCString()).SetTo(r))
		{
			if (r->ReadNext())
			{
				found = true;
				*parentId = r->GetInt32(0);
				r->GetStr(1, resultStr, resultStrBuffSize);
			}
			db->CloseReader(r);
		}
	}

	sql.Clear();
	if (found)
	{
		sql.AppendCmdC(CSTR("select id, group_type, eng_name, chi_name, description, parent_id, photo_group, photo_species, idKey, flags from "));
		sql.AppendCol((const UTF8Char*)"groups");
		sql.AppendCmdC(CSTR(" where id = "));
		sql.AppendInt32(*parentId);
	}
	else
	{
		sql.AppendCmdC(CSTR("select id, group_type, eng_name, chi_name, description, parent_id, photo_group, photo_species, idKey, flags from "));
		sql.AppendCol((const UTF8Char*)"groups");
		sql.AppendCmdC(CSTR(" where eng_name like "));
		Text::StrConcatC(Text::StrConcatC(Text::StrConcatC(sbuff, UTF8STRC("%")), searchStr, searchStrLen), UTF8STRC("%"));
		sql.AppendStrUTF8(sbuff);
		sql.AppendCmdC(CSTR(" or chi_name like "));
		sql.AppendStrUTF8(sbuff);
		Text::StrConcatC(resultStr, searchStr, searchStrLen);
	}
	if (db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		if (r->ReadNext())
		{
			Int32 flags;
			found = true;

			photoGroup = -1;
			if (!r->IsNull(6))
				photoGroup = r->GetInt32(6);
			
			photoSpecies = -1;
			if (!r->IsNull(7))
				photoSpecies = r->GetInt32(7);

			sptr = r->GetStr(2, sbuff, sizeof(sbuff));//engName
			sptr2 = r->GetStr(3, sbuff2, sizeof(sbuff2));//chiName
			NEW_CLASS(foundGroup, OrganGroup());
			foundGroup->SetGroupId(r->GetInt32(0));
			foundGroup->SetCName(CSTRP(sbuff2, sptr2));
			foundGroup->SetEName(CSTRP(sbuff, sptr));
			foundGroup->SetGroupType(r->GetInt32(1));
			sb.ClearStr();
			r->GetStr(4, sb);
			foundGroup->SetDesc(sb.ToCString());
			foundGroup->SetPhotoGroup(photoGroup);
			foundGroup->SetPhotoSpecies(photoSpecies);
			sb.ClearStr();
			r->GetStr(8, sb);
			foundGroup->SetIDKey(sb.ToCString());
			flags = r->GetInt32(9);
			foundGroup->SetAdminOnly(flags & 1);

			*parentId = r->GetInt32(5);

			if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), searchStr, searchStrLen) || Text::StrEqualsC(sbuff2, (UOSInt)(sptr2 - sbuff2), searchStr, searchStrLen))
			{
			}
			else
			{
				while (r->ReadNext())
				{
					sptr = r->GetStr(2, sbuff, sizeof(sbuff));//engName
					sptr2 = r->GetStr(3, sbuff2, sizeof(sbuff2));//chiName
					if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), searchStr, searchStrLen) || Text::StrEqualsC(sbuff2, (UOSInt)(sptr2 - sbuff2), searchStr, searchStrLen))
					{
						photoGroup = -1;
						if (!r->IsNull(6))
							photoGroup = r->GetInt32(6);
						photoSpecies = -1;
						if (!r->IsNull(7))
							photoSpecies = r->GetInt32(7);


						foundGroup->SetGroupId(r->GetInt32(0));
						sb.ClearStr();
						sb.AppendP(sbuff2, sptr2);
						foundGroup->SetCName(sb.ToCString());
						sb.ClearStr();
						sb.AppendP(sbuff, sptr);
						foundGroup->SetEName(sb.ToCString());
						foundGroup->SetGroupType(r->GetInt32(1));
						sb.ClearStr();
						r->GetStr(4, sb);
						foundGroup->SetDesc(sb.ToCString());
						foundGroup->SetPhotoGroup(photoGroup);
						foundGroup->SetPhotoSpecies(photoSpecies);
						sb.ClearStr();
						r->GetStr(8, sb);
						foundGroup->SetIDKey(sb.ToCString());
						flags = r->GetInt32(9);
						foundGroup->SetAdminOnly(flags & 1);

						*parentId = r->GetInt32(5);
						break;
					}
				}
			}
		}
		db->CloseReader(r);
	}
	return foundGroup;
}

void SSWR::OrganMgr::OrganEnvDB::LoadGroupTypes()
{
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return;
	NN<OrganGroupType> grpType;
	NN<DB::DBReader> r;
	Int32 seq;
	UTF8Char cname[64];
	UTF8Char *cnameEnd;
	UTF8Char ename[64];
	UTF8Char *enameEnd;
	this->grpTypes.DeleteAll();

	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("Select seq, chi_name, eng_name from group_type where cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	sql.AppendCmdC(CSTR(" order by seq"));
	if (db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		while (r->ReadNext())
		{
			seq = r->GetInt32(0);
			cnameEnd = r->GetStr(1, cname, sizeof(cname));
			enameEnd = r->GetStr(2, ename, sizeof(ename));
			NEW_CLASSNN(grpType, OrganGroupType(seq, {cname, (UOSInt)(cnameEnd - cname)}, {ename, (UOSInt)(enameEnd - ename)}));
			this->grpTypes.Add(grpType);
		}
		db->CloseReader(r);
	}
}

NN<Data::FastMapNN<Int32, Data::ArrayListNN<SSWR::OrganMgr::OrganGroup>>> SSWR::OrganMgr::OrganEnvDB::GetGroupTree()
{
	NN<Data::ArrayListNN<OrganGroup>> grps;
	NN<Data::FastMapNN<Int32, Data::ArrayListNN<OrganGroup>>> grpTree;
	NN<DB::DBReader> r;
	Int32 parId;

	NEW_CLASSNN(grpTree, Data::Int32FastMapNN<Data::ArrayListNN<OrganGroup>>());
	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return grpTree;

	DB::SQLBuilder sql(db);
	Text::StringBuilderUTF8 sb;
	sql.AppendCmdC(CSTR("select id, group_type, eng_name, chi_name, description, photo_group, photo_species, idKey, parent_id, flags from "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" where cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	sql.AppendCmdC(CSTR(" order by parent_id, eng_name"));
	if (db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		while (r->ReadNext())
		{
			Int32 photoGroup;
			Int32 photoSpecies;
			Int32 flags;
			photoGroup = -1;
			if (!r->IsNull(5))
				photoGroup = r->GetInt32(5);
			photoSpecies = -1;
			if (!r->IsNull(6))
				photoSpecies = r->GetInt32(6);
			NN<OrganGroup> newGrp;
			NEW_CLASSNN(newGrp, OrganGroup());
			newGrp->SetGroupId(r->GetInt32(0));
			sb.ClearStr();
			r->GetStr(3, sb);
			newGrp->SetCName(sb.ToCString());
			sb.ClearStr();
			r->GetStr(2, sb);
			newGrp->SetEName(sb.ToCString());
			newGrp->SetGroupType(r->GetInt32(1));
			sb.ClearStr();
			r->GetStr(4, sb);
			newGrp->SetDesc(sb.ToCString());
			newGrp->SetPhotoGroup(photoGroup);
			newGrp->SetPhotoSpecies(photoSpecies);
			sb.ClearStr();
			r->GetStr(7, sb);
			newGrp->SetIDKey(sb.ToCString());
			flags = r->GetInt32(9);
			newGrp->SetAdminOnly(flags & 1);

			parId = r->GetInt32(8);
			if (!grpTree->Get(parId).SetTo(grps))
			{
				NEW_CLASSNN(grps, Data::ArrayListNN<OrganGroup>());
				grpTree->Put(parId, grps);
			}
			grps->Add(newGrp);
		}
		db->CloseReader(r);
	}
	return grpTree;
}

NN<Data::FastMapNN<Int32, Data::ArrayListNN<SSWR::OrganMgr::OrganSpecies>>> SSWR::OrganMgr::OrganEnvDB::GetSpeciesTree()
{
	NN<Data::ArrayListNN<OrganSpecies>> sps;
	NN<Data::FastMapNN<Int32, Data::ArrayListNN<OrganSpecies>>> spTree;
	NN<DB::DBReader> r;
	Int32 parId;

	NEW_CLASSNN(spTree, Data::Int32FastMapNN<Data::ArrayListNN<OrganSpecies>>());

	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return spTree;
	DB::SQLBuilder sql(db);
	Text::StringBuilderUTF8 sb;
	sql.Clear();
	sql.AppendCmdC(CSTR("SELECT id, chi_name, sci_name, eng_name, description, dirName, photo, idKey, flags, group_id, photoId, mapColor FROM species where cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	sql.AppendCmdC(CSTR(" order by group_id, sci_name"));
	if (db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		while (r->ReadNext())
		{
			NN<OrganSpecies> sp;
			NEW_CLASSNN(sp, OrganSpecies());
			
			sp->SetSpeciesId(r->GetInt32(0));
			sb.ClearStr();
			r->GetStr(1, sb);
			sp->SetCName(sb.ToCString());
			sb.ClearStr();
			r->GetStr(2, sb);
			sp->SetSName(sb.ToCString());
			sb.ClearStr();
			r->GetStr(3, sb);
			sp->SetEName(sb.ToCString());
			sb.ClearStr();
			r->GetStr(4, sb);
			sp->SetDesc(sb.ToCString());
			sb.ClearStr();
			r->GetStr(5, sb);
			sp->SetDirName(sb.ToCString());
			sb.ClearStr();
			r->GetStr(6, sb);
			sp->SetPhoto(sb.ToCString());
			sb.ClearStr();
			r->GetStr(7, sb);
			sp->SetIDKey(sb.ToCString());
			sp->SetFlags(r->GetInt32(8));
			sp->SetPhotoId(r->GetInt32(10));
			sp->SetMapColor((UInt32)r->GetInt32(11));

			parId = r->GetInt32(9);

			if (!spTree->Get(parId).SetTo(sps))
			{
				NEW_CLASSNN(sps, Data::ArrayListNN<OrganSpecies>());
				spTree->Put(parId, sps);
			}
			sps->Add(sp);
		}
		db->CloseReader(r);
	}

	return spTree;
}

void SSWR::OrganMgr::OrganEnvDB::Test()
{
	NN<Data::FastMapNN<Int32, OrganSpecies>> speciesMap;
	Data::ArrayListInt32 speciesList;
	NN<UserFileInfo> userFile;
	Optional<OrganSpecies> species;
	NN<OrganSpecies> nnspecies;
	Data::DateTime dt;
	NN<Trip> tr;
	NN<Location> loc;
	NEW_CLASSNN(speciesMap, Data::Int32FastMapNN<OrganSpecies>());
	UOSInt i = 0;
	UOSInt j = this->userFileMap.GetCount();
	while (i < j)
	{
		userFile = this->userFileMap.GetItemNoCheck(i);
		if (userFile->location.IsNull())
		{
			if (!(species = speciesMap->Get(userFile->speciesId)).SetTo(nnspecies) && speciesList.SortedIndexOf(userFile->speciesId) < 0)
			{
				speciesList.SortedInsert(userFile->speciesId);
				if ((species = this->GetSpecies(userFile->speciesId)).SetTo(nnspecies))
				{
					speciesMap->Put(nnspecies->GetSpeciesId(), nnspecies);
				}
			}
			if (species.SetTo(nnspecies))
			{
				if (this->TripGet(userId, userFile->captureTime).SetTo(tr) && this->LocationGet(tr->locId).SetTo(loc))
				{
					this->UpdateUserFileLoc(userFile, loc->cname->v);
				}
			}
		}
		i++;
	}
	speciesMap->DeleteAll();
	speciesMap.Delete();

	//this->UpgradeDB();
	//this->UpgradeDB2();

	/*	Data::ArrayList<UserFileInfo *> *userFileList = this->userFileMap->GetValues();
	UserFileInfo *userFile;
	DB::SQLBuilder sql(this->db);
	WChar wbuff[512];
	WChar *wptr;
	Data::DateTime dt;
	Text::StringBuilderW sb;
	OSInt i = 0;
	OSInt j = userFileList->GetCount();
	dt.ToUTCTime();

	while (i < j)
	{
		userFile = userFileList->GetItem(i);
		dt.SetTicks(userFile->fileTimeTicks);
		wptr = Text::StrConcat(wbuff, this->cfgDataPath);
		if (wptr[-1] != '\\')
		{
			*wptr++ = '\\';
		}
		wptr = Text::StrConcat(wptr, L"UserFile\\");
		wptr = Text::StrInt32(wptr, userFile->webuserId);
		wptr = Text::StrConcat(wptr, L"\\");
		wptr = dt.ToString(wptr, L"yyyyMM");
		wptr = Text::StrConcat(wptr, L"\\");
		Text::StrConcat(wptr, userFile->dataFileName);
		if (userFile->fileType == 1)
		{
			Media::EXIFData *exif = ParseJPGExif(wbuff);
			if (exif)
			{
				const Char *csptr;
				const Char *csptr2;
				csptr = exif->GetPhotoMake();
				csptr2 = exif->GetPhotoModel();
				sb.ClearStr();
				if (csptr && csptr2)
				{
					if (Text::StrStartsWithICase(csptr2, csptr))
					{
						sb.AppendASCII(csptr2);
					}
					else
					{
						sb.AppendASCII(csptr);
						sb.AppendC(UTF8STRC(" ");
						sb.AppendASCII(csptr2);
					}
				}
				else if (csptr)
				{
					sb.AppendASCII(csptr);
				}
				else if (csptr2)
				{
					sb.AppendASCII(csptr2);
				}
				DEL_CLASS(exif);
				if (sb.GetLength() > 0)
				{
					sql.Clear();
					sql.AppendCmdC(CSTR("update userfile set camera = ");
					sql.AppendStr(sb.ToString());
					sql.AppendCmdC(CSTR(" where id = ");
					sql.AppendInt32(userFile->id);
					this->db->ExecuteNonQuery(sql.ToCString());
				}
			}
		}
		
		i++;
	}*/
}

void SSWR::OrganMgr::OrganEnvDB::UpgradeDB()
{
	Data::ArrayListNN<OrganSpecies> spList;
	UOSInt i;
	NN<OrganSpecies> sp;
	this->GetGroupAllSpecies(spList, 0);
	i = spList.GetCount();
	while (i-- > 0)
	{
		sp = spList.GetItemNoCheck(i);
		this->UpgradeFileStruct(sp);
		sp.Delete();
	}
}

typedef struct
{
	Int32 id;
	Int32 photoId;
	Int32 photoWId;
	NN<Text::String> dirName;
	Optional<Text::String> photoName;
} UpgradeDBSpInfo;

void SSWR::OrganMgr::OrganEnvDB::UpgradeDB2()
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;
	UTF8Char *sptrEnd;
	UTF8Char *sptr2;
	Text::PString cols[4];
	UInt32 crcVal;
	UOSInt i;
	UOSInt j;
	Int32 id;
	Bool allSucc;
	Data::ArrayList<UpgradeDBSpInfo*> spList;
	UpgradeDBSpInfo *sp;
	Text::StringBuilderUTF8 sb;
	NN<DB::DBReader> r;
	UInt8 crcBuff[4];
	Crypto::Hash::CRC32R crc;
	OSInt coverFound;
	NN<WebFileInfo> wfile;
	NN<SpeciesInfo> spInfo;
	Bool isCover;
	NN<Text::String> s;

	NN<DB::DBTool> db;
	if (!db.Set(this->db))
		return;
	DB::SQLBuilder sql(db);
	if (db->ExecuteReader(CSTR("select id, dirName, photo, photoId, photoWId from species")).SetTo(r))
	{
		while (r->ReadNext())
		{
			sp = MemAlloc(UpgradeDBSpInfo, 1);
			sp->id = r->GetInt32(0);
			sp->dirName = r->GetNewStrNN(1);
			sp->photoName = r->GetNewStr(2);
			sp->photoId = r->GetInt32(3);
			sp->photoWId = r->GetInt32(4);
			spList.Add(sp);
		}
		db->CloseReader(r);
	}

	i = 0;
	j = spList.GetCount();
	while (i < j)
	{
		sp = spList.GetItem(i);
		coverFound = 0;

		sptr = sbuff;
		if (!this->cateIsFullDir)
		{
			sptr = this->cfgImgDirBase->ConcatTo(sptr);
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		sptr = Text::String::OrEmpty(this->currCate->srcDir)->ConcatTo(sptr);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = sp->dirName->ConcatTo(sptr);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr2 = Text::StrConcatC(sptr, UTF8STRC("web.txt"));
		if (IO::Path::GetPathType(CSTRP(sbuff, sptr2)) == IO::Path::PathType::File)
		{
			allSucc = true;
			{
				IO::FileStream fs(CSTRP(sbuff, sptr2), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
				Text::UTF8Reader reader(fs);

				while ((sptr2 = reader.ReadLine(sbuff2, 511)) != 0)
				{
					if (Text::StrSplitP(cols, 4, {sbuff2, (UOSInt)(sptr2 - sbuff2)}, '\t') == 3)
					{
						if (cols[0].leng > 8 && cols[0].v[8] == '.')
						{
							sptr2 = Text::StrConcatC(sptr, UTF8STRC("web"));
							*sptr2++ = IO::Path::PATH_SEPERATOR;
							sptr2 = cols[0].ConcatTo(sptr2);

							crc.Clear();
							crc.Calc(cols[1].v, cols[1].leng);
							crc.GetValue(crcBuff);
							crcVal = ReadMUInt32(crcBuff);

							sql.Clear();
							sql.AppendCmdC(CSTR("insert into webfile (species_id, crcVal, imgUrl, srcUrl, prevUpdated, cropLeft, cropTop, cropRight, cropBottom, location) values ("));
							sql.AppendInt32(sp->id);
							sql.AppendCmdC(CSTR(", "));
							sql.AppendInt32((Int32)crcVal);
							sql.AppendCmdC(CSTR(", "));
							sql.AppendStrUTF8(cols[1].v);
							sql.AppendCmdC(CSTR(", "));
							sql.AppendStrUTF8(cols[2].v);
							sql.AppendCmdC(CSTR(", "));
							sql.AppendInt32(0);
							sql.AppendCmdC(CSTR(", "));
							sql.AppendDbl(0);
							sql.AppendCmdC(CSTR(", "));
							sql.AppendDbl(0);
							sql.AppendCmdC(CSTR(", "));
							sql.AppendDbl(0);
							sql.AppendCmdC(CSTR(", "));
							sql.AppendDbl(0);
							sql.AppendCmdC(CSTR(", "));
							sql.AppendStrUTF8((const UTF8Char*)"");
							sql.AppendCmdC(CSTR(")"));
							if (db->ExecuteNonQuery(sql.ToCString()) > 0)
							{
								id = db->GetLastIdentity32();
								
								if (sp->photoId == 0 && sp->photoWId == 0 && sp->photoName.SetTo(s) && s->StartsWith(UTF8STRC("web\\")) && cols[0].StartsWith(&s->v[4], s->leng - 4))
								{
									isCover = true;
								}
								else
								{
									isCover = false;
								}

								wfile = MemAllocNN(WebFileInfo);
								wfile->id = id;
								wfile->speciesId = sp->id;
								wfile->imgUrl = Text::String::New(cols[1].ToCString());
								wfile->srcUrl = Text::String::New(cols[2].ToCString());
								wfile->location = Text::String::NewEmpty();
								wfile->crcVal = crcVal;
								wfile->cropLeft = 0;
								wfile->cropTop = 0;
								wfile->cropRight = 0;
								wfile->cropBottom = 0;

								sptrEnd = sptr2;
								sptr2 = this->cfgDataPath->ConcatTo(sbuff2);
								if (sptr2[-1] != IO::Path::PATH_SEPERATOR)
								{
									*sptr2++ = IO::Path::PATH_SEPERATOR;
								}
								sptr2 = Text::StrConcatC(sptr2, UTF8STRC("WebFile"));
								*sptr2++ = IO::Path::PATH_SEPERATOR;
								sptr2 = Text::StrInt32(sptr2, id >> 10);
								IO::Path::CreateDirectory(CSTRP(sbuff2, sptr2));

								*sptr2++ = IO::Path::PATH_SEPERATOR;
								sptr2 = Text::StrInt32(sptr2, id);
								sptr2 = Text::StrConcatC(sptr2, UTF8STRC(".jpg"));
								if (!IO::FileUtil::MoveFile(CSTRP(sbuff, sptrEnd), CSTRP(sbuff2, sptr2), IO::FileUtil::FileExistAction::Fail, 0, 0))
								{
									allSucc = false;

									wfile->imgUrl->Release();
									wfile->srcUrl->Release();
									wfile->location->Release();
									MemFreeNN(wfile);
								}
								else
								{
									if (isCover)
									{
										coverFound++;

										sql.Clear();
										sql.AppendCmdC(CSTR("update species set photo = "));
										sql.AppendStrUTF8(0);
										sql.AppendCmdC(CSTR(", photoWId = "));
										sql.AppendInt32(id);
										sql.AppendCmdC(CSTR(" where id = "));
										sql.AppendInt32(sp->id);
										db->ExecuteNonQuery(sql.ToCString());
									}
									spInfo = this->GetSpeciesInfoCreate(sp->id);
									spInfo->wfileMap.Put(wfile->id, wfile);
								}
							}
							else
							{
								allSucc = false;
							}
						}
						else
						{
							allSucc = false;
						}
					}
					else
					{
						allSucc = false;
					}
				}
			}
			if (sp->photoId == 0 && sp->photoWId == 0 && sp->photoName.SetTo(s) && s->StartsWith(UTF8STRC("web\\")))
			{
				if (coverFound != 1)
				{
					allSucc = false;
				}
			}

			if (allSucc)
			{
				Text::StrConcatC(sptr, UTF8STRC("web"));
				if (IO::Path::RemoveDirectory(sbuff))
				{
					Text::StrConcatC(sptr, UTF8STRC("web.txt"));
					IO::Path::DeleteFile(sbuff);
				}
			}
		}

		sp->dirName->Release();
		OPTSTR_DEL(sp->photoName);
		MemFree(sp);
		i++;
	}
}

void SSWR::OrganMgr::OrganEnvDB::UpgradeFileStruct(NN<OrganSpecies> sp)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	const UTF8Char *coverName = OPTSTR_CSTR(sp->GetPhoto()).v;
	IO::Path::FindFileSession *sess;
	Bool isCoverPhoto;
	IO::Path::PathType pt;
	UOSInt i;
	if (coverName && coverName[0] == '*')
	{
		coverName = &coverName[1];
	}
	if (coverName && coverName[0] == 0)
	{
		coverName = 0;
	}

	sptr = sbuff;
	if (!this->cateIsFullDir)
	{
		sptr = this->cfgImgDirBase->ConcatTo(sptr);
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = Text::String::OrEmpty(this->currCate->srcDir)->ConcatTo(sptr);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::String::OrEmpty(sp->GetDirName())->ConcatTo(sptr);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
	if (sess)
	{
		while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
		{
			if (pt == IO::Path::PathType::File)
			{
				if (coverName && Text::StrStartsWithICase(sptr, coverName))
				{
					isCoverPhoto = true;
				}
				else
				{
					isCoverPhoto = false;
				}

				i = Text::StrLastIndexOfChar(sptr, '.');
				if (i == INVALID_INDEX)
				{

				}
				else if (Text::StrEqualsICaseC(&sptr[i], (UOSInt)(sptr2 - &sptr[i]), UTF8STRC(".JPG")) || Text::StrEqualsICaseC(&sptr[i], (UOSInt)(sptr2 - &sptr[i]), UTF8STRC(".PCX")) || Text::StrEqualsICaseC(&sptr[i], (UOSInt)(sptr2 - &sptr[i]), UTF8STRC(".WAV")))
				{
					Int32 fileId = 0;
					this->AddSpeciesFile(sp, CSTRP(sbuff, sptr2), isCoverPhoto, true, fileId);
				}
			}
		}
		IO::Path::FindFileClose(sess);

	}
	return;
}

void SSWR::OrganMgr::OrganEnvDB::ExportLite(const UTF8Char *folder)
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UTF8Char *sptr2End;
	UTF8Char *sptr3;
	UTF8Char *sptr3End;
	Text::StringBuilderUTF8 sb;
	Bool valid = true;
	sptr = Text::StrConcat(sbuff, folder);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr2 = Text::StrConcatC(sptr, UTF8STRC("OrganWeb64.cfg"));
	{
		IO::FileStream fs(CSTRP(sbuff, sptr2), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (fs.IsError())
		{
			valid = false;
		}
		else
		{
			Text::UTF8Writer writer(fs);
			writer.WriteLine(CSTR("ScreenSize=1200"));
			writer.WriteLine(CSTR("MDBFile=OrganWeb.mdb"));
			writer.WriteLine(CSTR("ImageDir=Image\\"));
			writer.WriteLine(CSTR("SvrPort=8080"));
			writer.WriteLine(CSTR("Watermark=sswroom"));
			writer.WriteLine(CSTR("CacheDir=Cache\\"));
			writer.WriteLine(CSTR("DataDir=Data"));
		}
	}
	if (!valid)
		return;
	sptr2 = Text::StrConcatC(sptr, UTF8STRC("Cache"));
	IO::Path::CreateDirectory(CSTRP(sbuff, sptr2));
	Exporter::MDBExporter exporter;
	sptr2 = Text::StrConcatC(sptr, UTF8STRC("OrganWeb.mdb"));
	
	IO::NullStream nstm;
	exporter.ExportFile(nstm, CSTRP(sbuff, sptr2), this->db->GetDBConn(), 0);

	sptr2 = Text::StrConcatC(sptr, UTF8STRC("Image"));
	IO::Path::CreateDirectory(CSTRP(sbuff, sptr2));
	*sptr2++ = IO::Path::PATH_SEPERATOR;
	sptr3 = this->cfgImgDirBase->ConcatTo(sbuff2);
	*sptr3++ = IO::Path::PATH_SEPERATOR;
	NN<DB::DBReader> r;
	if (this->db->ExecuteReader(CSTR("select srcDir from category")).SetTo(r))
	{
		while (r->ReadNext())
		{
			sb.ClearStr();
			r->GetStr(0, sb);
			if (sb.GetLength() > 0)
			{
				sptr2End = Text::StrConcatC(sptr2, sb.ToString(), sb.GetLength());
				sptr3End = Text::StrConcatC(sptr3, sb.ToString(), sb.GetLength());
				IO::FileUtil::CopyDir(CSTRP(sbuff2, sptr3End), CSTRP(sbuff, sptr2End), IO::FileUtil::FileExistAction::Fail, 0, 0);
			}
		}
		this->db->CloseReader(r);
	}

	sptr2 = Text::StrConcatC(sptr, UTF8STRC("Data"));
	IO::Path::CreateDirectory(CSTRP(sbuff, sptr2));
	*sptr2++ = IO::Path::PATH_SEPERATOR;
	sptr2End = Text::StrConcatC(sptr2, UTF8STRC("DataFile"));

	sptr3 = this->cfgDataPath->ConcatTo(sbuff2);
	*sptr3++ = IO::Path::PATH_SEPERATOR;
	sptr3End = Text::StrConcatC(sptr3, UTF8STRC("DataFile"));
	IO::FileUtil::CopyDir(CSTRP(sbuff2, sptr3End), CSTRP(sbuff, sptr2End), IO::FileUtil::FileExistAction::Fail, 0, 0);

	if (this->db->ExecuteReader(CSTR("select fileType, fileTime, webuser_id, dataFileName from userfile")).SetTo(r))
	{
		Data::DateTime dt;
		Optional<IO::FileExporter::ParamData> param;
		Exporter::GUIJPGExporter exporter;
		Media::Resizer::LanczosResizerH8_8 resizer(4, 3, Media::AT_NO_ALPHA);
		
		sptr2 = Text::StrConcatC(sptr2, UTF8STRC("UserFile"));
		*sptr2++ = IO::Path::PATH_SEPERATOR;

		sptr3 = this->cfgDataPath->ConcatTo(sbuff2);
		*sptr3++ = IO::Path::PATH_SEPERATOR;
		sptr3 = Text::StrConcatC(sptr3, UTF8STRC("UserFile"));
		*sptr3++ = IO::Path::PATH_SEPERATOR;

		while (r->ReadNext())
		{
			sb.ClearStr();
			Int32 userId = r->GetInt32(2);
			Int32 fileType = r->GetInt32(0);
			sb.AppendI32(userId);
			sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
			r->GetAsDateTime(1, dt);
			sptr2End = dt.ToString(sptr2, "yyyyMM");
			sb.AppendP(sptr2, sptr2End);
			sptr2End = Text::StrConcatC(sptr2, sb.ToString(), sb.GetLength());
			IO::Path::CreateDirectory(CSTRP(sbuff, sptr2End));

			sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
			r->GetStr(3, sb);
			
			sptr2End = Text::StrConcatC(sptr2, sb.ToString(), sb.GetLength());
			sptr3End = Text::StrConcatC(sptr3, sb.ToString(), sb.GetLength());
			if (IO::Path::GetPathType(CSTRP(sbuff, sptr2End)) == IO::Path::PathType::Unknown)
			{
				if (fileType == 1 && Text::StrEndsWithICaseC(sptr2, (UOSInt)(sptr2End - sptr2), UTF8STRC(".jpg")))
				{
					Media::ImageList *imgList;
					NN<Media::ImageList> nnimgList;
					{
						IO::StmData::FileData fd({sbuff2, (UOSInt)(sptr3End - sbuff2)}, false);
						imgList = (Media::ImageList*)this->parsers.ParseFileType(fd, IO::ParserType::ImageList);
					}

					if (nnimgList.Set(imgList))
					{
						nnimgList->ToStaticImage(0);
						Media::StaticImage *newImg;
						NN<Media::StaticImage> simg;
						if (simg.Set((Media::StaticImage*)nnimgList->GetImage(0, 0)))
						{
							if (simg->info.dispSize.x > 1920 || simg->info.dispSize.y > 1920)
							{
								resizer.SetTargetSize(Math::Size2D<UOSInt>(1920, 1920));
								resizer.SetResizeAspectRatio(Media::IImgResizer::RAR_KEEPAR);
								newImg = resizer.ProcessToNew(simg);
								if (newImg)
								{
									nnimgList->ReplaceImage(0, newImg);
								}
							}

						}
						param = exporter.CreateParam(nnimgList);
						exporter.SetParamInt32(param, 0, 95);
						{
							IO::FileStream fs(CSTRP(sbuff, sptr2End), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
							exporter.ExportFile(fs, CSTRP(sbuff, sptr2End), nnimgList, param);
						}
						nnimgList.Delete();
					}
					else
					{
						imgList = 0;
					}
				}
				else
				{
					IO::FileUtil::CopyFile(CSTRP(sbuff2, sptr3End), CSTRP(sbuff, sptr2End), IO::FileUtil::FileExistAction::Overwrite, 0, 0);
				}
			}
		}
		this->db->CloseReader(r);
	}
}
