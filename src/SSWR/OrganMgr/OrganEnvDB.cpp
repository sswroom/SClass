#include "Stdafx.h"
#include "Crypto/Hash/CRC32R.h"
#include "Crypto/Hash/MD5.h"
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
#include "IO/Path.h"
#include "IO/StreamRecorder.h"
#include "IO/StmData/FileData.h"
#include "IO/StmData/MemoryData.h"
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
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "Text/XML.h"
#include "UI/MessageDialog.h"
#include "UI/GUICore.h"

SSWR::OrganMgr::OrganEnvDB::OrganEnvDB() : OrganEnv()
{
	UOSInt i;
	UOSInt j;
	Category *cate;

	this->cfg = IO::IniFile::ParseProgConfig(0);
	NEW_CLASS(this->log, IO::LogTool());
	this->db = 0;

	if (cfg == 0)
	{
		this->errType = ERR_CONFIG;
		return;
	}
	Text::String *cfgMySQLHost;
	Text::String *cfgMySQLDB;
	Text::String *cfgDSN;
	Text::String *cfgUID;
	Text::String *cfgPassword;

	cfgMySQLHost = cfg->GetValue((const UTF8Char*)"MySQLHost");
	cfgMySQLDB = cfg->GetValue((const UTF8Char*)"MySQLDB");
	cfgDSN = cfg->GetValue((const UTF8Char*)"DBDSN");
	cfgUID = cfg->GetValue((const UTF8Char*)"DBUID");
	cfgPassword = cfg->GetValue((const UTF8Char*)"DBPwd");
	this->cfgImgDirBase = cfg->GetValue((const UTF8Char*)"ImageDir");
	this->cfgDataPath = cfg->GetValue((const UTF8Char*)"DataDir");
	this->cfgCacheDir = cfg->GetValue((const UTF8Char*)"CacheDir");
	Text::String *userId = cfg->GetValue((const UTF8Char*)"WebUser");

	if (this->cfgImgDirBase == 0 || this->cfgImgDirBase->leng == 0 || this->cfgDataPath == 0 || this->cfgDataPath->leng == 0 || this->cfgCacheDir == 0 || this->cfgCacheDir->leng == 0)
	{
		this->errType = ERR_CONFIG;
		return;
	}
	if (!userId->ToInt32(&this->userId))
	{
		this->errType = ERR_CONFIG;
		return;
	}

	i = this->cfgImgDirBase->leng;
	if (this->cfgImgDirBase->v[i - 1] == IO::Path::PATH_SEPERATOR)
	{
		this->cfgImgDirBase->v[i - 1] = 0;
	}
	log->AddFileLog((const UTF8Char*)"OrganMgr.log", IO::ILogHandler::LOG_TYPE_SINGLE_FILE, IO::ILogHandler::LOG_GROUP_TYPE_NO_GROUP, IO::ILogHandler::LOG_LEVEL_RAW, 0, false);
	if (cfgMySQLDB && cfgMySQLHost)
	{
		this->db = Net::MySQLTCPClient::CreateDBTool(this->sockf, cfgMySQLHost->v, cfgMySQLDB->v, Text::String::OrEmpty(cfgUID)->v, Text::String::OrEmpty(cfgPassword)->v, log, 0);
	}
	else if (cfgDSN)
	{
		this->db = DB::ODBCConn::CreateDBTool(cfgDSN->v, Text::String::OrEmpty(cfgUID)->v, Text::String::OrEmpty(cfgPassword)->v, 0, log, 0);
	}
	if (db == 0)
	{
		this->errType = ERR_DB;
		return;
	}

	DB::DBReader *r = this->db->ExecuteReaderC(UTF8STRC("select cate_id, chi_name, dirName, srcDir from category"));
	if (r)
	{
		while (r->ReadNext())
		{
			cate = MemAlloc(Category, 1);
			cate->cateId = r->GetInt32(0);
			cate->chiName = r->GetNewStr(1);
			cate->dirName = r->GetNewStr(2);
			cate->srcDir = r->GetNewStr(3);
			this->categories->Add(cate);
		}
		this->db->CloseReader(r);
	}
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("select id from webuser where id = "));
	sql.AppendInt32(this->userId);
	r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
	{
		if (r->ReadNext())
		{
			this->db->CloseReader(r);
		}
		else
		{
			this->db->CloseReader(r);
			this->errType = ERR_DB;
			return;
		}
	}
	else
	{
		this->errType = ERR_DB;
		return;
	}
	r = this->db->ExecuteReaderC(UTF8STRC("select id, fileType, startTime, endTime, oriFileName, dataFileName, webuser_id from datafile order by id"));
	if (r)
	{
		Data::DateTime dt;
		DataFileInfo *dataFile;
		WebUserInfo *webUser;

		while (r->ReadNext())
		{
			dataFile = MemAlloc(DataFileInfo, 1);
			dataFile->id = r->GetInt32(0);
			dataFile->fileType = r->GetInt32(1);
			r->GetDate(2, &dt);
			dataFile->startTimeTicks = dt.ToTicks();
			r->GetDate(3, &dt);
			dataFile->endTimeTicks = dt.ToTicks();
			dataFile->oriFileName = r->GetNewStr(4);
			dataFile->fileName = r->GetNewStr(5);
			dataFile->webUserId = r->GetInt32(6);
			this->dataFiles->Add(dataFile);

			if (dataFile->fileType == 1)
			{
				webUser = this->GetWebUser(dataFile->webUserId);
				i = webUser->gpsFileIndex->SortedInsert(dataFile->startTimeTicks);
				webUser->gpsFileObj->Insert(i, dataFile);
			}
		}
		this->db->CloseReader(r);
	}

	r = this->db->ExecuteReaderC(UTF8STRC("select id, fileType, oriFileName, fileTime, lat, lon, webuser_id, species_id, captureTime, dataFileName, crcVal, rotType, camera, descript, cropLeft, cropTop, cropRight, cropBottom, location from userfile order by id"));
	if (r)
	{
		UOSInt k;
		Data::DateTime dt;
		SpeciesInfo *species;
		UserFileInfo *userFile;
		WebUserInfo *webUser;
		Data::ArrayList<UserFileInfo*> userFileList;
		userFileList.EnsureCapacity(102400);

		while (r->ReadNext())
		{
			userFile = MemAlloc(UserFileInfo, 1);
			userFile->id = r->GetInt32(0);
			userFile->fileType = r->GetInt32(1);
			userFile->oriFileName = r->GetNewStr(2);
			r->GetDate(3, &dt);
			userFile->fileTimeTicks = dt.ToTicks();
			userFile->lat = r->GetDbl(4);
			userFile->lon = r->GetDbl(5);
			userFile->webuserId = r->GetInt32(6);
			userFile->speciesId = r->GetInt32(7);
			r->GetDate(8, &dt);
			userFile->captureTimeTicks = dt.ToTicks();
			userFile->dataFileName = r->GetNewStr(9);
			userFile->crcVal = (UInt32)r->GetInt32(10);
			userFile->rotType = r->GetInt32(11);
			userFile->camera = r->GetNewStr(12);
			userFile->descript = r->GetNewStr(13);
			userFile->cropLeft = r->GetDbl(14);
			userFile->cropTop = r->GetDbl(15);
			userFile->cropRight = r->GetDbl(16);
			userFile->cropBottom = r->GetDbl(17);
			userFile->location = r->GetNewStr(18);
			userFileList.Add(userFile);
		}
		this->db->CloseReader(r);

		UserFileComparator comparator;
		Data::Sort::ArtificialQuickSort::Sort(&userFileList, &comparator);
		this->userFileMap->AllocSize(userFileList.GetCount());
		i = 0;
		j = userFileList.GetCount();
		while (i < j)
		{
			userFile = userFileList.GetItem(i);
			this->userFileMap->Put(userFile->id, userFile);
			i++;
		}

		UserFileTimeComparator tcomparator;
		Data::Sort::ArtificialQuickSort::Sort(&userFileList, &tcomparator);
		webUser = 0;
		i = 0;
		j = userFileList.GetCount();
		while (i < j)
		{
			userFile = userFileList.GetItem(i);
			if (webUser == 0 || webUser->id != userFile->webuserId)
			{
				webUser = this->GetWebUser(userFile->webuserId);
			}
			k = webUser->userFileIndex->SortedInsert(userFile->fileTimeTicks);
			webUser->userFileObj->Insert(k, userFile);

			i++;
		}

		UserFileSpeciesComparator scomparator;
		Data::Sort::ArtificialQuickSort::Sort(&userFileList, &scomparator);
		species = 0;
		i = 0;
		j = userFileList.GetCount();
		while (i < j)
		{
			userFile = userFileList.GetItem(i);
			if (species == 0 || species->id != userFile->speciesId)
			{
				species = this->GetSpeciesInfo(userFile->speciesId, true);
			}
			if (species)
			{
				species->files->Add(userFile);
			}

			i++;
		}
	}

	r = this->db->ExecuteReaderC(UTF8STRC("select id, species_id, crcVal, imgUrl, srcUrl, prevUpdated, cropLeft, cropTop, cropRight, cropBottom, location from webfile"));
	if (r)
	{
		SpeciesInfo *species;
		WebFileInfo *wfile;
		Data::ArrayList<WebFileInfo*> fileList;

		while (r->ReadNext())
		{
			wfile = MemAlloc(WebFileInfo, 1);
			wfile->id = r->GetInt32(0);
			wfile->speciesId = r->GetInt32(1);
			wfile->crcVal = (UInt32)r->GetInt32(2);
			wfile->imgUrl = r->GetNewStr(3);
			wfile->srcUrl = r->GetNewStr(4);
			wfile->cropLeft = r->GetDbl(6);
			wfile->cropTop = r->GetDbl(7);
			wfile->cropRight = r->GetDbl(8);
			wfile->cropBottom = r->GetDbl(9);
			wfile->location = r->GetNewStr(10);
			fileList.Add(wfile);
		}
		this->db->CloseReader(r);

		WebFileSpeciesComparator comparator;
		Data::Sort::ArtificialQuickSort::Sort(&fileList, &comparator);
		species = 0;
		i = 0;
		j = fileList.GetCount();
		while (i < j)
		{
			wfile = fileList.GetItem(i);
			if (species == 0 || species->id != wfile->speciesId)
			{
				species = this->GetSpeciesInfo(wfile->speciesId, true);
			}
			species->wfileMap->Put(wfile->id, wfile);
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
	DEL_CLASS(this->log);
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

UOSInt SSWR::OrganMgr::OrganEnvDB::GetGroupItems(Data::ArrayList<OrganGroupItem*> *items, OrganGroup *grp)
{
	DB::DBReader *r;
	Int32 grpId;
	Int32 photoGrp;
	Int32 photoSp;
	if (grp == 0)
	{
		grpId = 0;
		photoGrp = 0;
		photoSp = 0;
	}
	else if (grp->GetGroupId() == -1)
	{
		return 0;
	}
	else
	{
		grpId = grp->GetGroupId();
		photoGrp = grp->GetPhotoGroup();
		photoSp = grp->GetPhotoSpecies();
	}
	DB::SQLBuilder sql(this->db);
	Text::StringBuilderUTF8 sb;
	Bool found = false;
	UOSInt retCnt = 0;
	sql.AppendCmdC(UTF8STRC("select id, group_type, eng_name, chi_name, description, photo_group, photo_species, idKey, flags from groups where parent_id = "));
	sql.AppendInt32(grpId);
	sql.AppendCmdC(UTF8STRC(" and cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	sql.AppendCmdC(UTF8STRC(" order by eng_name"));
	r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
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
			OrganGroup *newGrp;
			NEW_CLASS(newGrp, OrganGroup());
			newGrp->SetGroupId(r->GetInt32(0));
			sb.ClearStr();
			r->GetStr(3, &sb);
			newGrp->SetCName(sb.ToString());
			sb.ClearStr();
			r->GetStr(2, &sb);
			newGrp->SetEName(sb.ToString());
			newGrp->SetGroupType(r->GetInt32(1));
			sb.ClearStr();
			r->GetStr(4, &sb);
			newGrp->SetDesc(sb.ToString());
			newGrp->SetPhotoGroup(photoGroup);
			newGrp->SetPhotoSpecies(photoSpecies);
			sb.ClearStr();
			r->GetStr(7, &sb);
			newGrp->SetIDKey(sb.ToString());
			if (newGrp->GetGroupId() == photoGrp)
				newGrp->SetIsDefault(true);
			flags = r->GetInt32(8);
			newGrp->SetAdminOnly(flags & 1);
			items->Add(newGrp);
			retCnt++;
		}
		this->db->CloseReader(r);
	}

    if (!found)
	{
		sql.Clear();
		sql.AppendCmdC(UTF8STRC("SELECT id, chi_name, sci_name, eng_name, description, dirName, photo, idKey, flags, photoId, mapColor, photoWId FROM species where group_id = "));
		sql.AppendInt32(grpId);
		sql.AppendCmdC(UTF8STRC(" and cate_id = "));
		sql.AppendInt32(this->currCate->cateId);
		sql.AppendCmdC(UTF8STRC(" order by sci_name"));
		r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
		if (r)
		{
			while (r->ReadNext())
			{
				found = true;
				OrganSpecies *sp;
				NEW_CLASS(sp, OrganSpecies());
				
				sp->SetSpeciesId(r->GetInt32(0));
				sb.ClearStr();
				r->GetStr(1, &sb);
				sp->SetCName(sb.ToString());
				sb.ClearStr();
				r->GetStr(2, &sb);
				sp->SetSName(sb.ToString());
				sb.ClearStr();
				r->GetStr(3, &sb);
				sp->SetEName(sb.ToString());
				sb.ClearStr();
				r->GetStr(4, &sb);
				sp->SetDesc(sb.ToString());
				sb.ClearStr();
				r->GetStr(5, &sb);
				sp->SetDirName(sb.ToString());
				sb.ClearStr();
				r->GetStr(6, &sb);
				sp->SetPhoto(sb.ToString());
				sb.ClearStr();
				r->GetStr(7, &sb);
				sp->SetIDKey(sb.ToString());
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

UOSInt SSWR::OrganMgr::OrganEnvDB::GetGroupImages(Data::ArrayList<OrganImageItem*> *items, OrganGroup *grp)
{
	UTF8Char u8buff[512];
	UTF8Char *u8ptr;
	DB::DBReader *r;
	if (grp == 0 || grp->GetGroupId() == -1)
		return 0;
	DB::SQLBuilder sql(this->db);
	Text::StringBuilderUTF8 sb;
	Bool exist;
	UOSInt retCnt = 0;

	sql.Clear();
	sql.AppendCmdC(UTF8STRC("SELECT id, chi_name, sci_name, eng_name, description, dirName, photo, idKey, flags, photoId, photoWId FROM species where group_id = "));
	sql.AppendInt32(grp->GetGroupId());
	sql.AppendCmdC(UTF8STRC(" and cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	sql.AppendCmdC(UTF8STRC(" order by sci_name"));
	r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
	{
		while (r->ReadNext())
		{
			Int32 photoId;
			Int32 photoWId;
			Int32 spId = r->GetInt32(0);

			OrganImageItem *item;
			NEW_CLASS(item, OrganImageItem(this->userId));
			
			sb.ClearStr();
			r->GetStr(1, &sb);
			sb.AppendC(UTF8STRC(" ("));
			r->GetStr(2, &sb);
			sb.AppendC(UTF8STRC(")"));
			item->SetDispName(sb.ToString());

			if (spId == grp->GetPhotoSpecies())
				item->SetIsCoverPhoto(true);

			photoId = r->GetInt32(9);
			photoWId = r->GetInt32(10);
			if (photoId != 0)
			{
				UserFileInfo *userFile = this->userFileMap->Get(photoId);
				if (userFile != 0)
				{
					item->SetFullName(userFile->dataFileName);
					item->SetFileType(OrganImageItem::FT_USERFILE);
					item->SetUserFile(userFile);
				}
			}
			else if (photoWId != 0)
			{
				SpeciesInfo *sp = this->speciesMap->Get(spId);
				if (sp)
				{
					WebFileInfo *webFile = sp->wfileMap->Get(photoWId);
					if (webFile != 0)
					{
						item->SetFullName(webFile->imgUrl);
						item->SetFileType(OrganImageItem::FT_WEBFILE);
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
				sb.Append(this->currCate->srcDir->v);
				sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
				r->GetStr(5, &sb);
				sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
				r->GetStr(6, &sb);
				u8ptr = Text::StrConcat(u8buff, sb.ToString());

				exist = false;
				if (!exist)
				{
					Text::StrConcat(u8ptr, (const UTF8Char*)".jpg");
					if (IO::Path::GetPathType(u8buff) == IO::Path::PathType::File)
					{
						exist = true;
						item->SetFullName(u8buff);
						item->SetFileType(OrganImageItem::FT_JPG);
					}
				}
				if (!exist)
				{
					Text::StrConcat(u8ptr, (const UTF8Char*)".tif");
					if (IO::Path::GetPathType(u8buff) == IO::Path::PathType::File)
					{
						exist = true;
						item->SetFullName(u8buff);
						item->SetFileType(OrganImageItem::FT_TIF);
					}
				}
				if (!exist)
				{
					Text::StrConcat(u8ptr, (const UTF8Char*)".wav");
					if (IO::Path::GetPathType(u8buff) == IO::Path::PathType::File)
					{
						exist = true;
						item->SetFullName(u8buff);
						item->SetFileType(OrganImageItem::FT_WAV);
					}
				}
				if (!exist)
				{
					Text::StrConcat(u8ptr, (const UTF8Char*)".avi");
					if (IO::Path::GetPathType(u8buff) == IO::Path::PathType::File)
					{
						exist = true;
						item->SetFullName(u8buff);
						item->SetFileType(OrganImageItem::FT_AVI);
					}
				}
				if (!exist)
				{
					Text::StrConcat(u8ptr, (const UTF8Char*)".mts");
					if (IO::Path::GetPathType(u8buff) == IO::Path::PathType::File)
					{
						exist = true;
						item->SetFullName(u8buff);
						item->SetFileType(OrganImageItem::FT_AVI);
					}
				}
				if (!exist)
				{
					Text::StrConcat(u8ptr, (const UTF8Char*)".m2ts");
					if (IO::Path::GetPathType(u8buff) == IO::Path::PathType::File)
					{
						exist = true;
						item->SetFullName(u8buff);
						item->SetFileType(OrganImageItem::FT_AVI);
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

UOSInt SSWR::OrganMgr::OrganEnvDB::GetSpeciesImages(Data::ArrayList<OrganImageItem*> *items, OrganSpecies *sp)
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;
	UTF8Char *cols[4];
	Int32 newFlags = 0;
	const UTF8Char *coverName = sp->GetPhoto();
	Int32 coverId = sp->GetPhotoId();
	Int32 coverWId = sp->GetPhotoWId();
	IO::Path::FindFileSession *sess;
	Bool isCoverPhoto;
	IO::Path::PathType pt;
	UOSInt i;
	UOSInt j;
	UOSInt retCnt = 0;
	OrganImageItem *imgItem;
	if (coverId != 0)
	{
		coverName = 0;
	}
	else
	{
		if (coverName && coverName[0] == '*')
		{
			coverName = &coverName[1];
		}
		if (coverName && coverName[0] == 0)
		{
			coverName = 0;
		}
	}

	SpeciesInfo *spInfo = this->speciesMap->Get(sp->GetSpeciesId());
	if (spInfo)
	{
		UserFileInfo *userFile;
		i = 0;
		j = spInfo->files->GetCount();
		while (i < j)
		{
			userFile = spInfo->files->GetItem(i);
			if (coverId == userFile->id)
			{
				isCoverPhoto = true;
			}
			else
			{
				isCoverPhoto = false;
			}

			NEW_CLASS(imgItem, OrganImageItem(userFile->webuserId));
			imgItem->SetDispName(userFile->oriFileName);
			imgItem->SetIsCoverPhoto(isCoverPhoto);
			imgItem->SetFullName(userFile->dataFileName);
			imgItem->SetPhotoDate(userFile->captureTimeTicks / 1000);
			imgItem->SetRotateType(OrganImageItem::RT_NONE);
			imgItem->SetFileType(OrganImageItem::FT_USERFILE);
			imgItem->SetUserFile(userFile);
			items->Add(imgItem);
			if (userFile->fileType == 1)
			{
				newFlags |= 1;
			}
			i++;
		}

		Data::ArrayList<WebFileInfo*> *webFiles = spInfo->wfileMap->GetValues();
		WebFileInfo *webFile;

		i = 0;
		j = webFiles->GetCount();
		while (i < j)
		{
			webFile = webFiles->GetItem(i);

			NEW_CLASS(imgItem, OrganImageItem(0));
			Text::StrConcat(Text::StrInt32(Text::StrConcat(sbuff, (const UTF8Char*)"web\\"), webFile->id), (const UTF8Char*)".jpg");
			imgItem->SetDispName(sbuff);
			imgItem->SetIsCoverPhoto(webFile->id == coverWId);

			sptr = this->cfgDataPath->ConcatTo(sbuff2);
			if (sptr[-1] != IO::Path::PATH_SEPERATOR)
			{
				*sptr++ = IO::Path::PATH_SEPERATOR;
			}
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"WebFile");
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrInt32(sptr, webFile->id >> 10);
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrInt32(sptr, webFile->id);
			sptr = Text::StrConcat(sptr, (const UTF8Char*)".jpg");
			imgItem->SetFullName(sbuff2);
			imgItem->SetPhotoDate(0);
			imgItem->SetRotateType(OrganImageItem::RT_NONE);
			imgItem->SetFileType(OrganImageItem::FT_WEBFILE);
			imgItem->SetSrcURL(webFile->srcUrl);
			imgItem->SetImgURL(webFile->imgUrl);
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
	sptr = Text::StrConcatC(sptr, this->currCate->srcDir->v, this->currCate->srcDir->leng);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcat(sptr, sp->GetDirName());
	*sptr++ = IO::Path::PATH_SEPERATOR;
	Text::StrConcat(sptr, IO::Path::ALL_FILES);
	sess = IO::Path::FindFile(sbuff);
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

		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, 0))
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

				i = Text::StrLastIndexOf(sptr, '.');
				if (i == INVALID_INDEX)
				{

				}
				else if (Text::StrCompareICase(&sptr[i], (const UTF8Char*)".JPG") == 0)
				{
					Media::EXIFData *exif = ParseJPGExif(sbuff);
					NEW_CLASS(imgItem, OrganImageItem(this->userId));
					imgItem->SetDispName(sptr);
					imgItem->SetIsCoverPhoto(isCoverPhoto);
					imgItem->SetFullName(sbuff);
					if (exif)
					{
						Data::DateTime dt;
						dt.ToLocalTime();
						if (exif->GetPhotoDate(&dt))
						{
							imgItem->SetPhotoDate(dt.ToUnixTimestamp());
						}
						DEL_CLASS(exif);
					}
					imgItem->SetRotateType(OrganImageItem::RT_NONE);
					imgItem->SetFileType(OrganImageItem::FT_JPG);
					items->Add(imgItem);
/*
					if ((imgSet = OrganUtil::GetImageSetting(settings, imgItem->dispName)) != 0)
					{
						imgItem->rotateType = imgSet->rotateType;
					}*/
					newFlags |= 1;
					i++;
				}
				else if (Text::StrCompareICase(&sptr[i], (const UTF8Char *)".TIF") == 0)
				{
					Media::EXIFData *exif = ParseTIFExif(sbuff);
					NEW_CLASS(imgItem, OrganImageItem(this->userId));
					imgItem->SetDispName(sptr);
					imgItem->SetIsCoverPhoto(isCoverPhoto);
					imgItem->SetFullName(sbuff);
					if (exif)
					{
						Data::DateTime dt;
						if (exif->GetPhotoDate(&dt))
						{
							imgItem->SetPhotoDate(dt.ToUnixTimestamp());
						}
						DEL_CLASS(exif);
					}
					imgItem->SetRotateType(OrganImageItem::RT_NONE);
					imgItem->SetFileType(OrganImageItem::FT_TIF);
					items->Add(imgItem);
/*
					if ((imgSet = OrganUtil::GetImageSetting(settings, imgItem->dispName)) != 0)
					{
						imgItem->rotateType = imgSet->rotateType;
					}*/
					newFlags |= 1;
					retCnt++;
				}
				else if (Text::StrCompareICase(&sptr[i], (const UTF8Char *)".PCX") == 0 || Text::StrCompareICase(&sptr[i], (const UTF8Char *)".GIF") == 0 || Text::StrCompareICase(&sptr[i], (const UTF8Char *)".PNG") == 0)
				{
					NEW_CLASS(imgItem, OrganImageItem(this->userId));
					imgItem->SetDispName(sptr);
					imgItem->SetIsCoverPhoto(isCoverPhoto);
					imgItem->SetFullName(sbuff);
					imgItem->SetRotateType(OrganImageItem::RT_NONE);
					imgItem->SetFileType(OrganImageItem::FT_TIF);
					items->Add(imgItem);
					newFlags |= 1;
					retCnt++;
				}
				else if (Text::StrCompareICase(&sptr[i], (const UTF8Char *)".AVI") == 0 || Text::StrCompareICase(&sptr[i], (const UTF8Char *)".MOV") == 0 || Text::StrCompareICase(&sptr[i], (const UTF8Char *)".MTS") == 0 || Text::StrCompareICase(&sptr[i], (const UTF8Char *)".M2TS") == 0)
				{
					NEW_CLASS(imgItem, OrganImageItem(this->userId));
					imgItem->SetDispName(sptr);
					imgItem->SetIsCoverPhoto(isCoverPhoto);
					imgItem->SetFullName(sbuff);
					imgItem->SetRotateType(OrganImageItem::RT_NONE);
					imgItem->SetFileType(OrganImageItem::FT_AVI);
					items->Add(imgItem);
					newFlags |= 2;
					retCnt++;
				}
				else if (Text::StrCompareICase(&sptr[i], (const UTF8Char *)".WAV") == 0)
				{
					NEW_CLASS(imgItem, OrganImageItem(this->userId));
					imgItem->SetDispName(sptr);
					imgItem->SetIsCoverPhoto(isCoverPhoto);
					imgItem->SetFullName(sbuff);
					imgItem->SetRotateType(OrganImageItem::RT_NONE);
					imgItem->SetFileType(OrganImageItem::FT_WAV);
					items->Add(imgItem);
					newFlags |= 4;
					retCnt++;
				}
			}
		}
		IO::Path::FindFileClose(sess);

		Text::StrConcat(sptr, (const UTF8Char *)"web.txt");
		if (IO::Path::GetPathType(sbuff) == IO::Path::PathType::File)
		{
			Text::UTF8Reader *reader;
			IO::FileStream *fs;
			NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
			NEW_CLASS(reader, Text::UTF8Reader(fs));

			while (reader->ReadLine(sbuff2, 511))
			{
				if (Text::StrSplit(cols, 4, sbuff2, '\t') == 3)
				{
					NEW_CLASS(imgItem, OrganImageItem(0));
					Text::StrConcat(Text::StrConcat(sptr, (const UTF8Char *)"web\\"), cols[0]);
					imgItem->SetDispName(sptr);
					imgItem->SetIsCoverPhoto(false);
					if (coverName)
					{
						if (Text::StrStartsWith(sptr, coverName))
						{
							imgItem->SetIsCoverPhoto(true);
						}
					}
					imgItem->SetFullName(sbuff);
					imgItem->SetRotateType(OrganImageItem::RT_NONE);
					imgItem->SetFileType(OrganImageItem::FT_WEB_IMAGE);
					imgItem->SetImgURL(cols[1]);
					imgItem->SetSrcURL(cols[2]);
					newFlags |= 8;
					items->Add(imgItem);
					retCnt++;
				}
			}
			DEL_CLASS(reader);
			DEL_CLASS(fs);
		}
	}

	if (sp->GetFlags() != newFlags)
	{
		DB::SQLBuilder sql(this->db);
		sp->SetFlags(newFlags);
		sql.AppendCmdC(UTF8STRC("update species set flags = "));
		sql.AppendInt32(newFlags);
		sql.AppendCmdC(UTF8STRC(" where id = "));
		sql.AppendInt32(sp->GetSpeciesId());
		this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength());
	}
	return 0;
}

UOSInt SSWR::OrganMgr::OrganEnvDB::GetGroupAllSpecies(Data::ArrayList<OrganSpecies*> *items, OrganGroup *grp)
{
	UOSInt i;
	UOSInt j;
	Data::ArrayListInt32 allId;
	Data::ArrayListInt32 thisId;
	DB::DBReader *r;
	DB::SQLBuilder sql(this->db);
	Text::StringBuilderUTF8 sb;
	Int32 v;
	UOSInt cnt = 0;

	if (grp == 0)
	{
		thisId.Add(0);
	}
	else
	{
		allId.Add(grp->GetGroupId());
		thisId.Add(grp->GetGroupId());
	}
	while (thisId.GetCount() > 0)
	{
		sql.Clear();
		sql.AppendCmdC(UTF8STRC("select id from groups where parent_id in ("));
		i = 0;
		j = thisId.GetCount();
		while (i < j)
		{
			if (i > 0)
			{
				sql.AppendCmdC(UTF8STRC(", "));
			}
			sql.AppendInt32(thisId.GetItem(i));
			i++;
		}
		sql.AppendCmdC(UTF8STRC(") and cate_id = "));
		sql.AppendInt32(this->currCate->cateId);
		thisId.Clear();
		r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
		if (r)
		{
			while (r->ReadNext())
			{
				v = r->GetInt32(0);
				allId.Add(v);
				thisId.Add(v);
			}
			this->db->CloseReader(r);
		}
	}

	sql.Clear();
	sql.AppendCmdC(UTF8STRC("SELECT id, chi_name, sci_name, eng_name, description, dirName, photo, idKey, flags, photoId, mapColor, photoWId FROM species where group_id in ("));
	i = 0;
	j = allId.GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sql.AppendCmdC(UTF8STRC(", "));
		}
		sql.AppendInt32(allId.GetItem(i));
		i++;
	}
	sql.AppendCmdC(UTF8STRC(") order by sci_name"));
	r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
	{
		OrganSpecies *sp;
		while (r->ReadNext())
		{
			NEW_CLASS(sp, OrganSpecies());
			sp->SetSpeciesId(r->GetInt32(0));
			sb.ClearStr();
			r->GetStr(1, &sb);
			sp->SetCName(sb.ToString());
			sb.ClearStr();
			r->GetStr(2, &sb);
			sp->SetSName(sb.ToString());
			sb.ClearStr();
			r->GetStr(3, &sb);
			sp->SetEName(sb.ToString());
			sb.ClearStr();
			r->GetStr(4, &sb);
			sp->SetDesc(sb.ToString());
			sb.ClearStr();
			r->GetStr(5, &sb);
			sp->SetDirName(sb.ToString());
			sb.ClearStr();
			r->GetStr(6, &sb);
			sp->SetPhoto(sb.ToString());
			sb.ClearStr();
			r->GetStr(7, &sb);
			sp->SetIDKey(sb.ToString());
			sp->SetFlags(r->GetInt32(8));
			sp->SetPhotoId(r->GetInt32(9));
			sp->SetMapColor((UInt32)r->GetInt32(10));
			sp->SetPhotoWId(r->GetInt32(11));
			items->Add(sp);
			cnt++;
		}
		this->db->CloseReader(r);
	}
	return cnt;
}

UOSInt SSWR::OrganMgr::OrganEnvDB::GetGroupAllUserFile(Data::ArrayList<UserFileInfo*> *items, Data::ArrayList<UInt32> *colors, OrganGroup *grp)
{
	UOSInt i;
	UOSInt j;
	Data::ArrayListInt32 allId;
	Data::ArrayListInt32 thisId;
	DB::DBReader *r;
	DB::SQLBuilder sql(this->db);
	Int32 v;
	UInt32 c;
	UOSInt cnt = items->GetCount();

	if (grp == 0)
	{
		thisId.Add(0);
	}
	else
	{
		allId.Add(grp->GetGroupId());
		thisId.Add(grp->GetGroupId());
	}
	while (thisId.GetCount() > 0)
	{
		sql.Clear();
		sql.AppendCmdC(UTF8STRC("select id from groups where parent_id in ("));
		i = 0;
		j = thisId.GetCount();
		while (i < j)
		{
			if (i > 0)
			{
				sql.AppendCmdC(UTF8STRC(", "));
			}
			sql.AppendInt32(thisId.GetItem(i));
			i++;
		}
		sql.AppendCmdC(UTF8STRC(") and cate_id = "));
		sql.AppendInt32(this->currCate->cateId);
		thisId.Clear();
		r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
		if (r)
		{
			while (r->ReadNext())
			{
				v = r->GetInt32(0);
				allId.Add(v);
				thisId.Add(v);
			}
			this->db->CloseReader(r);
		}
	}

	sql.Clear();
	sql.AppendCmdC(UTF8STRC("SELECT id, mapColor FROM species where group_id in ("));
	i = 0;
	j = allId.GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sql.AppendCmdC(UTF8STRC(", "));
		}
		sql.AppendInt32(allId.GetItem(i));
		i++;
	}
	sql.AppendCmdC(UTF8STRC(") order by sci_name"));
	r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
	{
		SpeciesInfo *species;
		while (r->ReadNext())
		{
			v = r->GetInt32(0);
			c = (UInt32)r->GetInt32(1);
			species = this->speciesMap->Get(v);
			if (species != 0)
			{
				items->AddAll(species->files);
				i = species->files->GetCount();
				colors->EnsureCapacity(i);
				while (i-- > 0)
				{
					colors->Add(c);
				}
			}
		}
		this->db->CloseReader(r);
	}
	return items->GetCount() - cnt;
}

UOSInt SSWR::OrganMgr::OrganEnvDB::GetSpeciesItems(Data::ArrayList<OrganGroupItem*> *items, Data::ArrayList<Int32> *speciesIds)
{
	DB::DBReader *r;
	UOSInt i;
	UOSInt j;
	DB::SQLBuilder sql(this->db);
	Text::StringBuilderUTF8 sb;
	UOSInt retCnt = 0;
	if (speciesIds->GetCount() <= 0)
		return 0;

	sql.Clear();
	sql.AppendCmdC(UTF8STRC("SELECT id, chi_name, sci_name, eng_name, description, dirName, photo, idKey, flags, photoId, mapColor, photoWId FROM species where id in ("));
	i = 0;
	j = speciesIds->GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sql.AppendCmdC(UTF8STRC(", "));
		}
		sql.AppendInt32(speciesIds->GetItem(i));
		i++;
	}
	sql.AppendCmdC(UTF8STRC(") order by sci_name"));
	r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
	{
		while (r->ReadNext())
		{
			OrganSpecies *sp;
			NEW_CLASS(sp, OrganSpecies());
			
			sp->SetSpeciesId(r->GetInt32(0));
			sb.ClearStr();
			r->GetStr(1, &sb);
			sp->SetCName(sb.ToString());
			sb.ClearStr();
			r->GetStr(2, &sb);
			sp->SetSName(sb.ToString());
			sb.ClearStr();
			r->GetStr(3, &sb);
			sp->SetEName(sb.ToString());
			sb.ClearStr();
			r->GetStr(4, &sb);
			sp->SetDesc(sb.ToString());
			sb.ClearStr();
			r->GetStr(5, &sb);
			sp->SetDirName(sb.ToString());
			sb.ClearStr();
			r->GetStr(6, &sb);
			sp->SetPhoto(sb.ToString());
			sb.ClearStr();
			r->GetStr(7, &sb);
			sp->SetIDKey(sb.ToString());
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

SSWR::OrganMgr::OrganGroup *SSWR::OrganMgr::OrganEnvDB::GetGroup(Int32 groupId, Int32 *parentId)
{
	DB::SQLBuilder sql(this->db);
	DB::DBReader *r;
	Int32 photoGroup;
	Int32 photoSpecies;
	OrganGroup *foundGroup = 0;
	sql.AppendCmdC(UTF8STRC("select id, group_type, eng_name, chi_name, description, parent_id, photo_group, photo_species, idKey, flags from groups where id = "));
	sql.AppendInt32(groupId);
	r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
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
			r->GetStr(3, &sb);//chiName
			foundGroup->SetCName(sb.ToString());
			sb.ClearStr();
			r->GetStr(2, &sb);//engName
			foundGroup->SetEName(sb.ToString());
			foundGroup->SetGroupType(r->GetInt32(1));
			sb.ClearStr();
			r->GetStr(4, &sb);
			foundGroup->SetDesc(sb.ToString());
			foundGroup->SetPhotoGroup(photoGroup);
			foundGroup->SetPhotoSpecies(photoSpecies);
			sb.ClearStr();
			r->GetStr(8, &sb);
			foundGroup->SetIDKey(sb.ToString());
			flags = r->GetInt32(9);
			foundGroup->SetAdminOnly(flags & 1);

			*parentId = r->GetInt32(5);
		}
		this->db->CloseReader(r);
	}
	return foundGroup;
}

SSWR::OrganMgr::OrganSpecies *SSWR::OrganMgr::OrganEnvDB::GetSpecies(Int32 speciesId)
{
	DB::SQLBuilder sql(this->db);
	DB::DBReader *r;
	OrganSpecies *sp = 0;
	sql.AppendCmdC(UTF8STRC("SELECT id, chi_name, sci_name, eng_name, group_id, description, dirName, photo, idKey, flags, photoId, mapColor FROM species where id = "));
	sql.AppendInt32(speciesId);
	sql.AppendCmdC(UTF8STRC(" and cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
	{
		if (r->ReadNext())
		{
			Text::StringBuilderUTF8 sb;
			NEW_CLASS(sp, OrganSpecies());
			sp->SetSpeciesId(r->GetInt32(0));
			sb.ClearStr();
			r->GetStr(1, &sb);
			sp->SetCName(sb.ToString());
			sb.ClearStr();
			r->GetStr(2, &sb);
			sp->SetSName(sb.ToString());
			sb.ClearStr();
			r->GetStr(3, &sb);
			sp->SetEName(sb.ToString());
			sp->SetGroupId(r->GetInt32(4));
			sb.ClearStr();
			r->GetStr(5, &sb);
			sp->SetDesc(sb.ToString());
			sb.ClearStr();
			r->GetStr(6, &sb);
			sp->SetDirName(sb.ToString());
			sb.ClearStr();
			r->GetStr(7, &sb);
			sp->SetPhoto(sb.ToString());
			sb.ClearStr();
			r->GetStr(8, &sb);
			sp->SetIDKey(sb.ToString());
			sp->SetFlags(r->GetInt32(9));
			sp->SetPhotoId(r->GetInt32(10));
			sp->SetMapColor((UInt32)r->GetInt32(11));
		}
		this->db->CloseReader(r);
	}
	return sp;
}

UTF8Char *SSWR::OrganMgr::OrganEnvDB::GetSpeciesDir(OrganSpecies *sp, UTF8Char *sbuff)
{
	UTF8Char *sptr;
	if (this->currCate->srcDir->IndexOf((const UTF8Char*)":\\") != INVALID_INDEX)
	{
		sptr = Text::StrConcatC(sbuff, this->currCate->srcDir->v, this->currCate->srcDir->leng);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		return Text::StrConcat(sptr, sp->GetDirName());
	}
	else
	{
		sptr = this->cfgImgDirBase->ConcatTo(sbuff);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrConcatC(sptr, this->currCate->srcDir->v, this->currCate->srcDir->leng);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrConcat(sptr, sp->GetDirName());
		return sptr;
	}
}

Bool SSWR::OrganMgr::OrganEnvDB::CreateSpeciesDir(OrganSpecies *sp)
{
	UTF8Char sbuff[512];
	GetSpeciesDir(sp, sbuff);
	if (IO::Path::GetPathType(sbuff) == IO::Path::PathType::Directory)
		return true;
	return IO::Path::CreateDirectory(sbuff);
}

Bool SSWR::OrganMgr::OrganEnvDB::IsSpeciesExist(const UTF8Char *sName)
{
	DB::SQLBuilder sql(this->db);
	DB::DBReader *r;
	Bool found = false;
	sql.AppendCmdC(UTF8STRC("select id from species where sci_name = "));
	sql.AppendStrUTF8(sName);
	r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
	{
		found = r->ReadNext();
		this->db->CloseReader(r);
	}
	return found;
}

Bool SSWR::OrganMgr::OrganEnvDB::IsBookSpeciesExist(const UTF8Char *sName, Text::StringBuilderUTF *sb)
{
	Data::ArrayListInt32 spList;
	DB::SQLBuilder sql(this->db);
	DB::DBReader *r;
	OSInt si;
	UOSInt i;
	UOSInt j;
	Int32 spId;
	Bool found;
	sql.AppendCmdC(UTF8STRC("select species_id from species_book where dispName = "));
	sql.AppendStrUTF8(sName);
	r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
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
		this->db->CloseReader(r);
	}
	if ((j = spList.GetCount()) > 0)
	{
		sql.Clear();
		sql.AppendCmdC(UTF8STRC("select sci_name from species where id in ("));
		i = 0;
		while (i < j)
		{
			if (i > 0)
			{
				sql.AppendCmdC(UTF8STRC(", "));
			}
			sql.AppendInt32(spList.GetItem(i));
			i++;
		}
		sql.AppendCmdC(UTF8STRC(")"));
		r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
		if (r)
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
			this->db->CloseReader(r);
		}
	}
	return spList.GetCount() > 0;

}

Bool SSWR::OrganMgr::OrganEnvDB::AddSpecies(OrganSpecies *sp)
{
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("insert into species (eng_name, chi_name, sci_name, group_id, description, dirName, idKey, cate_id, mapColor) values ("));
	sql.AppendStrUTF8(sp->GetEName());
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(sp->GetCName());
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(sp->GetSName());
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt32(sp->GetGroupId());
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(sp->GetDesc());
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(sp->GetDirName());
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(sp->GetIDKey());
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt32(this->currCate->cateId);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt32((Int32)0xff4040ff);
	sql.AppendCmdC(UTF8STRC(")"));
	return this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) >= -1;
}

Bool SSWR::OrganMgr::OrganEnvDB::DelSpecies(OrganSpecies *sp)
{
	DB::SQLBuilder sql(this->db);
	Bool succ = true;
	sql.Clear();
	sql.AppendCmdC(UTF8STRC("update groups set photo_group=NULL, photo_species=NULL where photo_species="));
	sql.AppendInt32(sp->GetSpeciesId());
	if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) < -1)
		succ = false;

	sql.Clear();
	sql.AppendCmdC(UTF8STRC("delete from species where id="));
	sql.AppendInt32(sp->GetSpeciesId());
	if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) < -1)
		succ = false;

	sql.Clear();
	sql.AppendCmdC(UTF8STRC("delete from species_book where species_id="));
	sql.AppendInt32(sp->GetSpeciesId());
	if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) < -1)
		succ = false;
	return succ;
}

SSWR::OrganMgr::OrganEnvDB::FileStatus SSWR::OrganMgr::OrganEnvDB::AddSpeciesFile(OrganSpecies *sp, const UTF8Char *fileName, Bool firstPhoto, Bool moveFile, Int32 *fileId)
{
	UOSInt i;
	UOSInt j;
	Int32 fileType = 0;
	i = Text::StrLastIndexOf(fileName, IO::Path::PATH_SEPERATOR);
	j = Text::StrLastIndexOf(&fileName[i + 1], '.');
	if (j == INVALID_INDEX)
	{
		return FS_NOTSUPPORT;
	}
	if (Text::StrCompareICase(&fileName[i + j + 2], (const UTF8Char*)"JPG") == 0)
	{
		fileType = 1;
	}
	else if (Text::StrCompareICase(&fileName[i + j + 2], (const UTF8Char*)"TIF") == 0)
	{
		fileType = 1;
	}
	else if (Text::StrCompareICase(&fileName[i + j + 2], (const UTF8Char*)"PCX") == 0)
	{
		fileType = 1;
	}
	else if (Text::StrCompareICase(&fileName[i + j + 2], (const UTF8Char*)"GIF") == 0)
	{
		fileType = 1;
	}
	else if (Text::StrCompareICase(&fileName[i + j + 2], (const UTF8Char*)"PNG") == 0)
	{
		fileType = 1;
	}
	else if (Text::StrCompareICase(&fileName[i + j + 2], (const UTF8Char*)"AVI") == 0)
	{
		fileType = 2;
	}
	else if (Text::StrCompareICase(&fileName[i + j + 2], (const UTF8Char*)"MOV") == 0)
	{
		fileType = 2;
	}
	else if (Text::StrCompareICase(&fileName[i + j + 2], (const UTF8Char*)"WAV") == 0)
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
		IO::FileStream *fs;
		UInt8 *readBuff;
		UOSInt readSize;
		NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoBuffer));
		if (fs->IsError())
		{
			DEL_CLASS(fs);
			return FS_ERROR;
		}
		readBuff = MemAlloc(UInt8, 1048576);
		NEW_CLASS(mstm, IO::MemoryStream((UOSInt)fs->GetLength(), (const UTF8Char*)"OrganEnv.AddSpeciesFile"));
		while (true)
		{
			readSize = fs->Read(readBuff, 1048576);
			if (readSize == 0)
				break;
			mstm->Write(readBuff, readSize);
		}
		MemFree(readBuff);
		DEL_CLASS(fs);

		readBuff = mstm->GetBuff(&readSize);
		IO::StmData::MemoryData *md;
		IO::ParserType t;
		IO::ParsedObject *pobj;
		Bool valid = false;
		Data::DateTime fileTime;
		Double lat = 0;
		Double lon = 0;
		UserFileInfo *userFile;
		Text::String *camera = 0;
		UInt32 crcVal = 0;
		fileTime.SetTicks(0);
		fileTime.ToLocalTime();

		NEW_CLASS(md, IO::StmData::MemoryData(readBuff, readSize));
		pobj = this->parsers->ParseFile(md, &t);
		DEL_CLASS(md);
		if (pobj)
		{
			if (t == IO::ParserType::ImageList)
			{
				valid = true;

				Media::ImageList *imgList = (Media::ImageList*)pobj;
				Media::Image *img = imgList->GetImage(0, 0);
				if (img)
				{
					Media::EXIFData *exif = img->exif;
					if (exif)
					{
						exif->GetPhotoDate(&fileTime);
						if (fileTime.GetYear() >= 2000)
						{
							this->GetGPSPos(this->userId, &fileTime, &lat, &lon);
						}
						const UTF8Char *csptr;
						const UTF8Char *csptr2;
						csptr = (const UTF8Char*)exif->GetPhotoMake();
						csptr2 = (const UTF8Char*)exif->GetPhotoModel();
						if (csptr && csptr2)
						{
							if (Text::StrStartsWithICase(csptr2, csptr))
							{
								camera = Text::String::NewNotNull(csptr2);
							}
							else
							{
								Text::StringBuilderUTF8 sb;
								sb.Append(csptr);
								sb.AppendC(UTF8STRC(" "));
								sb.Append(csptr2);
								camera = Text::String::NewNotNull(sb.ToString());
							}
						}
						else if (csptr)
						{
							camera = Text::String::NewNotNull(csptr);
						}
						else if (csptr2)
						{
							camera = Text::String::NewNotNull(csptr2);
						}
					}
				}

				UInt8 crcBuff[4];
				Crypto::Hash::CRC32R crc;
				crc.Calc(readBuff, readSize);
				crc.GetValue(crcBuff);
				crcVal = ReadMUInt32(crcBuff);
			}
			DEL_CLASS(pobj);
		}
		DEL_CLASS(mstm);
		if (valid)
		{
			WebUserInfo *webUser = this->GetWebUser(this->userId);
			Int64 ticks = fileTime.ToTicks();
			OSInt si;
			UOSInt j;
			UOSInt k;
			si = webUser->userFileIndex->SortedIndexOf(ticks);
			if (si >= 0)
			{
				while (si > 0)
				{
					if (webUser->userFileIndex->GetItem((UOSInt)si - 1) == ticks)
					{
						si--;
					}
					else
					{
						break;
					}
				}
				j = (UOSInt)si;
				k = webUser->userFileIndex->GetCount();
				while (j < k)
				{
					if (webUser->userFileIndex->GetItem(j) != ticks)
						break;

					userFile = webUser->userFileObj->GetItem(j);
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
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"UserFile");
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = Text::StrInt32(sptr, this->userId);
				*sptr++ = IO::Path::PATH_SEPERATOR;
				fileTime.ToUTCTime();
				sptr = fileTime.ToString(sptr, "yyyyMM");
				IO::Path::CreateDirectory(sbuff);
				*sptr++ = IO::Path::PATH_SEPERATOR;
				dataFileName = sptr;
				sptr = Text::StrInt64(sptr, ticks);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"_");
				sptr = Text::StrHexVal32(sptr, crcVal);
				j = Text::StrLastIndexOf(&fileName[i + 1], '.');
				sptr = Text::StrConcat(sptr, &fileName[i + j + 1]);
				Bool succ;
				if (moveFile)
				{
					succ = IO::FileUtil::MoveFile(fileName, sbuff, IO::FileUtil::FileExistAction::Fail, 0, 0);
				}
				else
				{
					succ = IO::FileUtil::CopyFile(fileName, sbuff, IO::FileUtil::FileExistAction::Fail, 0, 0);
				}
				if (succ)
				{
					DB::SQLBuilder sql(this->db);
					sql.AppendCmdC(UTF8STRC("insert into userfile (fileType, oriFileName, fileTime, lat, lon, webuser_id, species_id, captureTime, dataFileName, crcVal, camera, cropLeft, cropTop, cropRight, cropBottom) values ("));
					sql.AppendInt32(fileType);
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendStrUTF8(&fileName[i + 1]);
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendDate(&fileTime);
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendDbl(lat);
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendDbl(lon);
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendInt32(this->userId);
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendInt32(sp->GetSpeciesId());
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendDate(&fileTime);
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendStrUTF8(dataFileName);
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendInt32((Int32)crcVal);
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendStrUTF8(camera->v);
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendDbl(0);
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendDbl(0);
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendDbl(0);
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendDbl(0);
					sql.AppendCmdC(UTF8STRC(")"));
					if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) > 0)
					{
						userFile = MemAlloc(UserFileInfo, 1);
						userFile->id = this->db->GetLastIdentity32();
						userFile->fileType = fileType;
						userFile->oriFileName = Text::String::NewNotNull(&fileName[i + 1]);
						userFile->fileTimeTicks = fileTime.ToTicks();
						userFile->lat = lat;
						userFile->lon = lon;
						userFile->webuserId = this->userId;
						userFile->speciesId = sp->GetSpeciesId();
						userFile->captureTimeTicks = userFile->fileTimeTicks;
						userFile->dataFileName = Text::String::NewNotNull(dataFileName);
						userFile->crcVal = crcVal;
						userFile->rotType = 0;
						userFile->camera = camera;
						userFile->descript = 0;
						userFile->cropLeft = 0;
						userFile->cropTop = 0;
						userFile->cropRight = 0;
						userFile->cropBottom = 0;
						userFile->location = 0;
						this->userFileMap->Put(userFile->id, userFile);

						SpeciesInfo *species = this->GetSpeciesInfo(userFile->speciesId, true);
						species->files->Add(userFile);

						webUser = this->GetWebUser(userFile->webuserId);
						k = webUser->userFileIndex->SortedInsert(userFile->fileTimeTicks);
						webUser->userFileObj->Insert(k, userFile);
						
						if (firstPhoto)
						{
							sp->SetPhotoId(userFile->id);
							this->SaveSpecies(sp);
						}
						if (fileId)
						{
							*fileId = userFile->id;
						}
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
	else if (fileType == 3)
	{
		Crypto::Hash::CRC32R crc;
		IO::FileStream *fs;
		UInt8 *readBuff;
		UOSInt readSize;
		IO::StmData::FileData *fd;
		UInt32 crcVal;
		IO::ParsedObject *pobj;
		IO::ParserType t;
		Data::DateTime fileTime;
		UserFileInfo *userFile;
		Bool valid = false;
		Media::DrawImage *graphImg = 0;
		fileTime.SetTicks(0);
		NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoBuffer));
		if (fs->IsError())
		{
			DEL_CLASS(fs);
			return FS_ERROR;
		}
		readBuff = MemAlloc(UInt8, 1048576);
		while (true)
		{
			readSize = fs->Read(readBuff, 1048576);
			if (readSize == 0)
				break;
			crc.Calc(readBuff, readSize);
		}
		MemFree(readBuff);
		DEL_CLASS(fs);
		UInt8 crcBuff[4];
		crc.GetValue(crcBuff);
		crcVal = ReadMUInt32(crcBuff);

		NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
		pobj = this->parsers->ParseFile(fd, &t);
		DEL_CLASS(fd);
		if (pobj)
		{
			if (t == IO::ParserType::MediaFile)
			{
				Media::MediaFile *mediaFile = (Media::MediaFile*)pobj;
				Media::IMediaSource *msrc = mediaFile->GetStream(0, 0);
				if (msrc && msrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
				{
					graphImg = Media::FrequencyGraph::CreateGraph(this->drawEng, (Media::IAudioSource *)msrc, 2048, 2048, Math::FFTCalc::WT_BLACKMANN_HARRIS, 12);
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
			WebUserInfo *webUser = this->GetWebUser(this->userId);
			Int64 ticks = 0;
			UOSInt j;
			UOSInt k;
			OSInt si;
			si = webUser->userFileIndex->SortedIndexOf(ticks);
			if (si >= 0)
			{
				while (si > 0)
				{
					if (webUser->userFileIndex->GetItem((UOSInt)si - 1) == ticks)
					{
						si--;
					}
					else
					{
						break;
					}
				}
				j = (UOSInt)si;
				k = webUser->userFileIndex->GetCount();
				while (j < k)
				{
					if (webUser->userFileIndex->GetItem(j) != ticks)
						break;

					userFile = webUser->userFileObj->GetItem(j);
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
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"UserFile");
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = Text::StrInt32(sptr, this->userId);
				*sptr++ = IO::Path::PATH_SEPERATOR;
				fileTime.ToUTCTime();
				sptr = fileTime.ToString(sptr, "yyyyMM");
				IO::Path::CreateDirectory(sbuff);
				*sptr++ = IO::Path::PATH_SEPERATOR;
				dataFileName = sptr;
				sptr = Text::StrInt64(sptr, ticks);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"_");
				sptr = Text::StrHexVal32(sptr, crcVal);
				j = Text::StrLastIndexOf(&fileName[i + 1], '.');
				sptr = Text::StrConcat(sptr, &fileName[i + j + 1]);
				Bool succ;
				if (moveFile)
				{
					succ = IO::FileUtil::MoveFile(fileName, sbuff, IO::FileUtil::FileExistAction::Fail, 0, 0);
				}
				else
				{
					succ = IO::FileUtil::CopyFile(fileName, sbuff, IO::FileUtil::FileExistAction::Fail, 0, 0);
				}
				if (succ)
				{
					DB::SQLBuilder sql(this->db);
					sql.AppendCmdC(UTF8STRC("insert into userfile (fileType, oriFileName, fileTime, lat, lon, webuser_id, species_id, captureTime, dataFileName, crcVal, camera) values ("));
					sql.AppendInt32(fileType);
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendStrUTF8(&fileName[i + 1]);
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendDate(&fileTime);
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendDbl(0);
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendDbl(0);
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendInt32(this->userId);
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendInt32(sp->GetSpeciesId());
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendDate(&fileTime);
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendStrUTF8(dataFileName);
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendInt32((Int32)crcVal);
					sql.AppendCmdC(UTF8STRC(", "));
					sql.AppendStrUTF8(0);
					sql.AppendCmdC(UTF8STRC(")"));
					if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) > 0)
					{
						userFile = MemAlloc(UserFileInfo, 1);
						userFile->id = this->db->GetLastIdentity32();
						userFile->fileType = fileType;
						userFile->oriFileName = Text::String::NewNotNull(&fileName[i + 1]);
						userFile->fileTimeTicks = fileTime.ToTicks();
						userFile->lat = 0;
						userFile->lon = 0;
						userFile->webuserId = this->userId;
						userFile->speciesId = sp->GetSpeciesId();
						userFile->captureTimeTicks = userFile->fileTimeTicks;
						userFile->dataFileName = Text::String::NewNotNull(dataFileName);
						userFile->crcVal = crcVal;
						userFile->rotType = 0;
						userFile->camera = 0;
						userFile->descript = 0;
						userFile->cropLeft = 0;
						userFile->cropTop = 0;
						userFile->cropRight = 0;
						userFile->cropBottom = 0;
						userFile->location = 0;
						this->userFileMap->Put(userFile->id, userFile);

						SpeciesInfo *species = this->GetSpeciesInfo(userFile->speciesId, true);
						species->files->Add(userFile);

						webUser = this->GetWebUser(userFile->webuserId);
						j = webUser->userFileIndex->SortedInsert(userFile->fileTimeTicks);
						webUser->userFileObj->Insert(j, userFile);
						
						if (firstPhoto)
						{
							sp->SetPhotoId(userFile->id);
							this->SaveSpecies(sp);
						}
						if (fileId)
						{
							*fileId = userFile->id;
						}

						sptr = this->cfgDataPath->ConcatTo(sbuff);
						if (sptr[-1] != IO::Path::PATH_SEPERATOR)
						{
							*sptr++ = IO::Path::PATH_SEPERATOR;
						}
						sptr = Text::StrConcat(sptr, (const UTF8Char*)"UserFile");
						*sptr++ = IO::Path::PATH_SEPERATOR;
						sptr = Text::StrInt32(sptr, this->userId);
						*sptr++ = IO::Path::PATH_SEPERATOR;
						fileTime.ToUTCTime();
						sptr = fileTime.ToString(sptr, "yyyyMM");
						IO::Path::CreateDirectory(sbuff);
						*sptr++ = IO::Path::PATH_SEPERATOR;
						sptr = Text::StrInt64(sptr, ticks);
						sptr = Text::StrConcat(sptr, (const UTF8Char*)"_");
						sptr = Text::StrHexVal32(sptr, crcVal);
						sptr = Text::StrConcat(sptr, (const UTF8Char*)".png");
						NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
						graphImg->SavePng(fs);
						DEL_CLASS(fs);
						this->drawEng->DeleteImage(graphImg);

						return FS_SUCCESS;
					}
					else
					{
						if (graphImg)
						{
							this->drawEng->DeleteImage(graphImg);
						}
						return FS_ERROR;
					}
				}
				else
				{
					if (graphImg)
					{
						this->drawEng->DeleteImage(graphImg);
					}
					return FS_ERROR;
				}
			}
			else
			{
				if (graphImg)
				{
					this->drawEng->DeleteImage(graphImg);
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
		sptr = Text::StrConcat(sptr, &fileName[i + 1]);
		if (IO::FileUtil::CopyFile(fileName, sbuff, IO::FileUtil::FileExistAction::Fail, 0, 0))
		{
			if (firstPhoto)
			{
				Text::StrConcat(sbuff, &fileName[i + 1]);
				sbuff[j] = 0;
				sp->SetPhoto(sbuff);
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

SSWR::OrganMgr::OrganEnvDB::FileStatus SSWR::OrganMgr::OrganEnvDB::AddSpeciesWebFile(OrganSpecies *sp, const UTF8Char *srcURL, const UTF8Char *imgURL, IO::Stream *stm, UTF8Char *webFileName)
{
	UTF8Char sbuff2[2048];
	UTF8Char *sptr2;
	Int32 id;
	Bool firstPhoto = false;
	Bool found = false;
	UOSInt i;
	Data::ArrayList<OrganImageItem*> *imgItems;
	OrganImageItem *imgItem;
	WebFileInfo *wfile;
	SpeciesInfo *spInfo;
	NEW_CLASS(imgItems, Data::ArrayList<OrganImageItem*>());
	this->GetSpeciesImages(imgItems, sp);
	i = imgItems->GetCount();
	firstPhoto = i == 0;
	while (i-- > 0)
	{
		imgItem = imgItems->GetItem(i);
		if (imgItem->GetFileType() == OrganImageItem::FT_WEB_IMAGE || imgItem->GetFileType() == OrganImageItem::FT_WEBFILE)
		{
			if (imgItem->GetImgURL()->Equals(imgURL))
			{
				found = true;
			}
		}
		DEL_CLASS(imgItem);
	}
	DEL_CLASS(imgItems);

	if (found)
	{
		return SSWR::OrganMgr::OrganEnvDB::FS_ERROR;
	}

	i = Text::StrCharCnt(imgURL);
	UInt32 crcVal;
	UInt8 crcBuff[4];
	Crypto::Hash::CRC32R crc;
	crc.Calc(imgURL, i);
	crc.GetValue(crcBuff);
	crcVal = ReadMUInt32(crcBuff);
	if (crcVal == 0)
	{
		return SSWR::OrganMgr::OrganEnvDB::FS_ERROR;
	}

	IO::MemoryStream *mstm;
	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR.OrganMgr.OrganEnvDB.AddSpeciesWebFile.mstm"));
	while ((i = stm->Read(sbuff2, sizeof(sbuff2))) > 0)
	{
		mstm->Write(sbuff2, i);
	}
	if (mstm->GetLength() <= 0)
	{
		DEL_CLASS(mstm);
		return SSWR::OrganMgr::OrganEnvDB::FS_ERROR;
	}

	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("insert into webfile (species_id, crcVal, imgUrl, srcUrl, prevUpdated, cropLeft, cropTop, cropRight, cropBottom, location) values ("));
	sql.AppendInt32(sp->GetSpeciesId());
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt32((Int32)crcVal);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(imgURL);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(srcURL);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt32(0);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendDbl(0);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendDbl(0);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendDbl(0);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendDbl(0);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8((const UTF8Char*)"");
	sql.AppendCmdC(UTF8STRC(")"));
	if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) > 0)
	{
		id = this->db->GetLastIdentity32();
		
		wfile = MemAlloc(WebFileInfo, 1);
		wfile->id = id;
		wfile->speciesId = sp->GetSpeciesId();
		wfile->imgUrl = Text::String::NewNotNull(imgURL);
		wfile->srcUrl = Text::String::NewNotNull(srcURL);
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
		sptr2 = Text::StrConcat(sptr2, (const UTF8Char*)"WebFile");
		*sptr2++ = IO::Path::PATH_SEPERATOR;
		sptr2 = Text::StrInt32(sptr2, id >> 10);
		IO::Path::CreateDirectory(sbuff2);

		*sptr2++ = IO::Path::PATH_SEPERATOR;
		sptr2 = Text::StrInt32(sptr2, id);
		sptr2 = Text::StrConcat(sptr2, (const UTF8Char*)".jpg");

		IO::FileStream *fs;
		UInt8 *buff = mstm->GetBuff(&i);
		NEW_CLASS(fs, IO::FileStream(sbuff2, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		fs->Write(buff, i);
		DEL_CLASS(fs);
		DEL_CLASS(mstm);

		if (firstPhoto)
		{
			sp->SetPhotoWId(id);
			this->SaveSpecies(sp);
		}
		spInfo = this->GetSpeciesInfo(sp->GetSpeciesId(), true);
		spInfo->wfileMap->Put(wfile->id, wfile);

		if (webFileName)
		{
			Text::StrConcat(Text::StrInt32(Text::StrConcat(webFileName, (const UTF8Char*)"web\\"), id), (const UTF8Char*)".jpg");
		}
	}
	else
	{
		DEL_CLASS(mstm);
		return SSWR::OrganMgr::OrganEnvDB::FS_ERROR;
	}
	return SSWR::OrganMgr::OrganEnvDB::FS_SUCCESS;
}

SSWR::OrganMgr::OrganEnvDB::FileStatus SSWR::OrganMgr::OrganEnvDB::AddSpeciesWebFileOld(OrganSpecies *sp, const UTF8Char *srcURL, const UTF8Char *imgURL, IO::Stream *stm, UTF8Char *webFileName)
{
	UTF8Char sbuff[512];
	UTF8Char fileName[32];
	UTF8Char *sptr;
	sptr = this->GetSpeciesDir(sp, sbuff);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"web");
	if (IO::Path::GetPathType(sbuff) == IO::Path::PathType::Unknown)
	{
		IO::Path::CreateDirectory(sbuff);
	}

	Bool firstPhoto = false;
	Bool found = false;
	UOSInt i;
	Data::ArrayList<OrganImageItem*> *imgItems;
	OrganImageItem *imgItem;
	NEW_CLASS(imgItems, Data::ArrayList<OrganImageItem*>());
	this->GetSpeciesImages(imgItems, sp);
	i = imgItems->GetCount();
	firstPhoto = i == 0;
	while (i-- > 0)
	{
		imgItem = imgItems->GetItem(i);
		if (imgItem->GetFileType() == OrganImageItem::FT_WEB_IMAGE || imgItem->GetFileType() == OrganImageItem::FT_WEBFILE)
		{
			if (imgItem->GetImgURL()->Equals(imgURL))
			{
				found = true;
			}
		}
		DEL_CLASS(imgItem);
	}
	DEL_CLASS(imgItems);

	if (found)
	{
		return SSWR::OrganMgr::OrganEnvDB::FS_ERROR;
	}

	i = Text::StrCharCnt(imgURL);
	UInt32 crcVal;
	UInt8 crcBuff[4];
	Crypto::Hash::CRC32R crc;
	crc.Calc(imgURL, i);
	crc.GetValue(crcBuff);
	crcVal = ReadMUInt32(crcBuff);
	
	i = Text::StrLastIndexOf(imgURL, '.');
	if (Text::StrCharCnt(&imgURL[i + 1]) > 4)
	{
		Text::StrConcat(Text::StrHexVal32(fileName, crcVal), (const UTF8Char*)".jpg");
	}
	else
	{
		Text::StrConcat(Text::StrHexVal32(fileName, crcVal), &imgURL[i]);
	}
	if (crcVal == 0)
	{
		return SSWR::OrganMgr::OrganEnvDB::FS_ERROR;
	}

	Bool succ;
	sptr[0] = IO::Path::PATH_SEPERATOR;
	Text::StrConcat(sptr + 1, fileName);
	IO::StreamRecorder *recorder;
	NEW_CLASS(recorder, IO::StreamRecorder(sbuff));
	succ = recorder->AppendStream(stm);
	succ = succ && (recorder->GetRecordedLength() > 0);
	DEL_CLASS(recorder);

	if (!succ)
	{
		IO::Path::DeleteFile(sbuff);
		return SSWR::OrganMgr::OrganEnvDB::FS_ERROR;
	}

	IO::FileStream *fs;
	Text::UTF8Writer *writer;
	Text::StringBuilderUTF8 sb;
	Text::StrConcat(sptr, (const UTF8Char*)".txt");
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(writer, Text::UTF8Writer(fs));
	writer->WriteSignature();
	sb.Append(fileName);
	sb.AppendC(UTF8STRC("\t"));
	sb.Append(imgURL);
	sb.AppendC(UTF8STRC("\t"));
	sb.Append(srcURL);
	writer->WriteLine(sb.ToString());
	DEL_CLASS(writer);
	DEL_CLASS(fs);

	if (firstPhoto)
	{
		i = Text::StrLastIndexOf(fileName, '.');
		fileName[i] = 0;
		sptr = Text::StrConcat(sbuff, (const UTF8Char*)"web");
		*sptr++ = IO::Path::PATH_SEPERATOR;
		Text::StrConcat(sptr, fileName);
		sp->SetPhoto(sbuff);
		this->SaveSpecies(sp);
	}
	if (webFileName)
	{
		UTF8Char *u8ptr = Text::StrConcat(webFileName, (const UTF8Char*)"web");
		*u8ptr++ = IO::Path::PATH_SEPERATOR;
		Text::StrConcat(u8ptr, fileName);
	}
	return SSWR::OrganMgr::OrganEnvDB::FS_SUCCESS;
}

Bool SSWR::OrganMgr::OrganEnvDB::UpdateSpeciesWebFile(OrganSpecies *sp, WebFileInfo *wfile, const UTF8Char *srcURL, const UTF8Char *location)
{
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("update webfile set srcUrl="));
	sql.AppendStrUTF8(srcURL);
	sql.AppendCmdC(UTF8STRC(", location="));
	sql.AppendStrUTF8(location);
	sql.AppendCmdC(UTF8STRC(" where id="));
	sql.AppendInt32(wfile->id);
	if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) >= 0)
	{
		SDEL_STRING(wfile->srcUrl);
		SDEL_STRING(wfile->location);
		wfile->srcUrl = Text::String::NewNotNull(srcURL);
		wfile->location = Text::String::NewNotNull(location);
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::OrganMgr::OrganEnvDB::UpdateSpeciesWebFileOld(OrganSpecies *sp, const UTF8Char *webFileName, const UTF8Char *srcURL)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = this->GetSpeciesDir(sp, sbuff);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"web");
	Text::StrConcat(sptr, (const UTF8Char*)".txt");

	IO::MemoryStream *mstm;
	IO::FileStream *fs;
	Text::UTF8Writer *writer;
	IO::StreamReader *reader;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	UTF8Char *sarr[3];

	if (Text::StrStartsWith(webFileName, (const UTF8Char*)"web") && webFileName[3] == IO::Path::PATH_SEPERATOR)
	{
		webFileName = &webFileName[4];
	}

	Bool found = false;
	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR.OrganMgr.OrganEnv.UpdateSpeciesWebFile.mstm"));
	NEW_CLASS(writer, Text::UTF8Writer(mstm));
	writer->WriteSignature();

	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(reader, IO::StreamReader(fs, 65001));
	sb.ClearStr();
	while (reader->ReadLine(&sb, 4095))
	{
		sb2.ClearStr();
		sb2.AppendC(sb.ToString(), sb.GetLength());
		if (Text::StrSplit(sarr, 3, sb.ToString(), '\t') == 3)
		{
			if (Text::StrEquals(sarr[0], webFileName))
			{
				found = true;
				sb2.ClearStr();
				sb2.Append(sarr[0]);
				sb2.AppendC(UTF8STRC("\t"));
				sb2.Append(sarr[1]);
				sb2.AppendC(UTF8STRC("\t"));
				sb2.Append(srcURL);
			}
		}
		writer->WriteLine(sb2.ToString());
		sb.ClearStr();
	}
	DEL_CLASS(reader);
	DEL_CLASS(fs);

	DEL_CLASS(writer);
	if (found)
	{
		NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		UOSInt size;
		UInt8 *buff = mstm->GetBuff(&size);
		fs->Write(buff, size);
		DEL_CLASS(fs);
	}
	DEL_CLASS(mstm);
	return true;
}

Bool SSWR::OrganMgr::OrganEnvDB::SaveSpecies(OrganSpecies *sp)
{
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("update species set eng_name="));
	sql.AppendStrUTF8(sp->GetEName());
	sql.AppendCmdC(UTF8STRC(", chi_name="));
	sql.AppendStrUTF8(sp->GetCName());
	sql.AppendCmdC(UTF8STRC(", sci_name="));
	sql.AppendStrUTF8(sp->GetSName());
	sql.AppendCmdC(UTF8STRC(", description="));
	sql.AppendStrUTF8(sp->GetDesc());
	sql.AppendCmdC(UTF8STRC(",dirName="));
	sql.AppendStrUTF8(sp->GetDirName());
	sql.AppendCmdC(UTF8STRC(", photo="));
	sql.AppendStrUTF8(sp->GetPhoto());
	sql.AppendCmdC(UTF8STRC(", photoId="));
	sql.AppendInt32(sp->GetPhotoId());
	sql.AppendCmdC(UTF8STRC(", photoWId="));
	sql.AppendInt32(sp->GetPhotoWId());
	sql.AppendCmdC(UTF8STRC(", idKey="));
	sql.AppendStrUTF8(sp->GetIDKey());
	sql.AppendCmdC(UTF8STRC(", mapColor="));
	sql.AppendInt32((Int32)sp->GetMapColor());
	sql.AppendCmdC(UTF8STRC(" where id="));
	sql.AppendInt32(sp->GetSpeciesId());
	return this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) >= -1;
}

Bool SSWR::OrganMgr::OrganEnvDB::SaveGroup(OrganGroup *grp)
{
	Int32 flags = 0;
	if (grp->GetAdminOnly())
	{
		flags |= 1;
	}
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("update groups set group_type="));
	sql.AppendInt32(grp->GetGroupType());
	sql.AppendCmdC(UTF8STRC(", eng_name="));
	sql.AppendStrUTF8(grp->GetEName());
	sql.AppendCmdC(UTF8STRC(", chi_name="));
	sql.AppendStrUTF8(grp->GetCName());
	sql.AppendCmdC(UTF8STRC(", description="));
	sql.AppendStrUTF8(grp->GetDesc());
	sql.AppendCmdC(UTF8STRC(",idKey="));
	sql.AppendStrUTF8(grp->GetIDKey());
	sql.AppendCmdC(UTF8STRC(",flags="));
	sql.AppendInt32(flags);
	sql.AppendCmdC(UTF8STRC(" where id="));
	sql.AppendInt32(grp->GetGroupId());
	return this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) >= -1;
}

UOSInt SSWR::OrganMgr::OrganEnvDB::GetGroupCount(Int32 groupId)
{
	DB::SQLBuilder sql(this->db);
	DB::DBReader *r;
	sql.AppendCmdC(UTF8STRC("select count(*) from groups where parent_id = "));
	sql.AppendInt32(groupId);
	sql.AppendCmdC(UTF8STRC(" and cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
	{
		UOSInt cnt = 0;
		if (r->ReadNext())
		{
			cnt = (UInt32)r->GetInt32(0);
		}
		this->db->CloseReader(r);
		return cnt;
	}
	else
	{
		return 0;
	}
}

UOSInt SSWR::OrganMgr::OrganEnvDB::GetSpeciesCount(Int32 groupId)
{
	DB::SQLBuilder sql(this->db);
	DB::DBReader *r;
	sql.AppendCmdC(UTF8STRC("select count(*) from species where group_id = "));
	sql.AppendInt32(groupId);
	sql.AppendCmdC(UTF8STRC(" and cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
 	r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
	{
		UOSInt cnt = 0;
		if (r->ReadNext())
		{
			cnt = (UInt32)r->GetInt32(0);
		}
		this->db->CloseReader(r);
		return cnt;
	}
	else
	{
		return 0;
	}
}

Bool SSWR::OrganMgr::OrganEnvDB::AddGroup(OrganGroup *grp, Int32 parGroupId)
{
	Int32 flags = 0;
	if (grp->GetAdminOnly())
	{
		flags |= 1;
	}
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("insert into groups (group_type, eng_name, chi_name, description, parent_id, idKey, cate_id, flags) values ("));
	sql.AppendInt32(grp->GetGroupType());
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(grp->GetEName());
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(grp->GetCName());
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(grp->GetDesc());
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt32(parGroupId);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(grp->GetIDKey());
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt32(this->currCate->cateId);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt32(flags);
	sql.AppendCmdC(UTF8STRC(")"));
	return this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) == 1;
}

Bool SSWR::OrganMgr::OrganEnvDB::DelGroup(Int32 groupId)
{
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("delete from groups where id = "));
	sql.AppendInt32(groupId);
	sql.AppendCmdC(UTF8STRC(" and cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	return this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) == 1;
}

Bool SSWR::OrganMgr::OrganEnvDB::SetGroupDefSp(OrganGroup *grp, OrganImageItem *img)
{
/*	DB::SQLBuilder sql(this->db);
	Int32 oldId = grp->GetPhotoSpecies();
	grp->SetPhotoGroup(-1);
	grp->SetPhotoSpecies(img->->GetSpeciesId());
	if (oldId == -1)
	{
		sql.AppendCmdC(UTF8STRC("update groups set photo_group=NULL, photo_species=");
		sql.AppendInt32(sp->GetSpeciesId());
		sql.AppendCmdC(UTF8STRC(" where id=");
		sql.AppendInt32(grp->GetGroupId());
		this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength());
	}
	else
	{
		sql.AppendCmdC(UTF8STRC("update groups set photo_group=NULL, photo_species=");
		sql.AppendInt32(sp->GetSpeciesId());
		sql.AppendCmdC(UTF8STRC(" where photo_species=");
		sql.AppendInt32(oldId);
		this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength());
	}
	sp->SetIsDefault(true);*/
	return true;
}

Bool SSWR::OrganMgr::OrganEnvDB::MoveGroups(Data::ArrayList<OrganGroup*> *grpList, OrganGroup *destGroup)
{
	DB::SQLBuilder sql(this->db);
	Bool found;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = grpList->GetCount();
	sql.AppendCmdC(UTF8STRC("update groups set parent_id="));
	sql.AppendInt32(destGroup->GetGroupId());
	sql.AppendCmdC(UTF8STRC(" where id in ("));
	found = false;
	while (i < j)
	{
		if (found)
		{
			sql.AppendCmdC(UTF8STRC(", "));
		}
		else
		{
			found = true;
		}
		sql.AppendInt32(grpList->GetItem(i)->GetGroupId());
		i++;
	}
	sql.AppendCmdC(UTF8STRC(")"));
	return this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) >= 0;
}

Bool SSWR::OrganMgr::OrganEnvDB::MoveSpecies(Data::ArrayList<OrganSpecies*> *spList, OrganGroup *destGroup)
{
	DB::SQLBuilder sql(this->db);
	DB::SQLBuilder sql2(this->db);
	UOSInt i;
	UOSInt j;
	Bool found;
	OrganSpecies *sp;
	sql.AppendCmdC(UTF8STRC("update species set group_id="));
	sql.AppendInt32(destGroup->GetGroupId());
	sql.AppendCmdC(UTF8STRC(" where id in ("));
	sql2.AppendCmdC(UTF8STRC("update groups set photo_group=NULL, photo_species=NULL where photo_species in ("));
	found = false;
	i = 0;
	j = spList->GetCount();
	while (i < j)
	{
		if (found)
		{
			sql.AppendCmdC(UTF8STRC(", "));
			sql2.AppendCmdC(UTF8STRC(", "));
		}
		else
		{
			found = true;
		}
		sp = spList->GetItem(i);
		sql.AppendInt32(sp->GetSpeciesId());
		sql2.AppendInt32(sp->GetSpeciesId());
		i++;
	}
	sql.AppendCmdC(UTF8STRC(")"));
	sql2.AppendCmdC(UTF8STRC(")"));
	db->ExecuteNonQueryC(sql2.ToString(), sql2.GetLength());
	return db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) > 0;
}

Bool SSWR::OrganMgr::OrganEnvDB::MoveImages(Data::ArrayList<OrganImages*> *imgList, OrganSpecies *destSp, UI::GUIForm *frm)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	sptr = this->GetSpeciesDir(destSp, sbuff);
	Bool moveWeb = false;
	UOSInt i;
	UOSInt j;
	OrganImages *img;
	i = 0;
	j = imgList->GetCount();
	if (j <= 0)
	{
		return true;
	}
	while (i < j)
	{
		img = imgList->GetItem(i);
		if (img->GetImgItem()->GetFileType() == OrganImageItem::FT_USERFILE)
		{
			UserFileInfo *userFile = img->GetImgItem()->GetUserFile();
			if (userFile->speciesId != destSp->GetSpeciesId())
			{
				SpeciesInfo *sp = this->speciesMap->Get(userFile->speciesId);
				UOSInt oldInd;
				if (sp)
				{
					oldInd = sp->files->GetCount();
					while (oldInd-- > 0)
					{
						if (sp->files->GetItem(oldInd)->id == userFile->id)
						{
							sp->files->RemoveAt(oldInd);
							break;
						}
					}
				}
				userFile->speciesId = destSp->GetSpeciesId();
				sp = this->GetSpeciesInfo(destSp->GetSpeciesId(), true);
				sp->files->Add(userFile);
				DB::SQLBuilder sql(this->db);
				sql.AppendCmdC(UTF8STRC("update userfile set species_id = "));
				sql.AppendInt32(userFile->speciesId);
				sql.AppendCmdC(UTF8STRC(" where id = "));
				sql.AppendInt32(userFile->id);
				this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength());
			}
		}
		else if (img->GetImgItem()->GetFileType() == OrganImageItem::FT_WEBFILE)
		{
			WebFileInfo *wfile = img->GetImgItem()->GetWebFile();
			if (wfile->speciesId != destSp->GetSpeciesId())
			{
				SpeciesInfo *sp = this->speciesMap->Get(wfile->speciesId);
				if (sp)
				{
					sp->wfileMap->Remove(wfile->id);
				}
				wfile->speciesId = destSp->GetSpeciesId();
				sp = this->GetSpeciesInfo(destSp->GetSpeciesId(), true);
				sp->wfileMap->Put(wfile->id, wfile);
				DB::SQLBuilder sql(this->db);
				sql.AppendCmdC(UTF8STRC("update webfile set species_id = "));
				sql.AppendInt32(wfile->speciesId);
				sql.AppendCmdC(UTF8STRC(" where id = "));
				sql.AppendInt32(wfile->id);
				this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength());
			}
		}
		else
		{
			if (img->GetImgItem()->GetFileType() == OrganImageItem::FT_WEB_IMAGE)
			{
				sptr2 = sptr;
				if (!moveWeb)
				{
					moveWeb = true;
					sptr[0] = IO::Path::PATH_SEPERATOR;
					Text::StrConcat(sptr + 1, (const UTF8Char*)"web");
					IO::Path::CreateDirectory(sbuff);
				}
			}
			else
			{
				IO::Path::CreateDirectory(sbuff);
				sptr[0] = 0;
				sptr2 = sptr;
			}
			*sptr2++ = IO::Path::PATH_SEPERATOR;
			sptr2 = img->GetImgItem()->GetDispName()->ConcatTo(sptr2);
			if (img->GetImgItem()->GetFullName()->Equals(sbuff))
			{
				break;
			}
			if (!IO::FileUtil::MoveFile(img->GetImgItem()->GetFullName()->v, sbuff, IO::FileUtil::FileExistAction::Fail, 0, 0))
			{
				Text::StringBuilderUTF8 sb;
				const UTF8Char *csptr;
				csptr = Text::StrToUTF8New(L"移動");
				sb.Append(csptr);
				Text::StrDelNew(csptr);
				sb.Append(img->GetImgItem()->GetDispName()->v);
				csptr = Text::StrToUTF8New(L"時出錯, 要繼續?");
				sb.Append(csptr);
				Text::StrDelNew(csptr);
				csptr = Text::StrToUTF8New(L"錯誤");
				if (!UI::MessageDialog::ShowYesNoDialog(sb.ToString(), csptr, frm))
				{
					Text::StrDelNew(csptr);
					break;
				}
				Text::StrDelNew(csptr);
			}
		}
		i++;
	}

	if (moveWeb)
	{
		Text::StringBuilderUTF8 sb;
		IO::FileStream *fs;
		Text::UTF8Writer *writer;
		const UTF8Char *name;
		const UTF8Char *srcDir = 0;
		sptr[0] = IO::Path::PATH_SEPERATOR;
		Text::StrConcat(sptr + 1, (const UTF8Char*)"web.txt");
		j = i;
		i = 0;
		NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::Append, IO::FileShare::DenyAll, IO::FileStream::BufferType::Normal));
		NEW_CLASS(writer, Text::UTF8Writer(fs));
		while (i < j)
		{
			img = imgList->GetItem(i);
			if (img->GetImgItem()->GetFileType() == OrganImageItem::FT_WEB_IMAGE)
			{
				srcDir = img->GetSrcImgDir();
				name = img->GetImgItem()->GetDispName()->v;
				name = &name[Text::StrLastIndexOf(name, IO::Path::PATH_SEPERATOR) + 1];
				sb.ClearStr();
				sb.Append(name);
				sb.AppendC(UTF8STRC("\t"));
				sb.Append(img->GetImgItem()->GetImgURL()->v);
				sb.AppendC(UTF8STRC("\t"));
				sb.Append(img->GetImgItem()->GetSrcURL()->v);
				writer->WriteLine(sb.ToString());
			}
			i++;
		}
		DEL_CLASS(writer);
		DEL_CLASS(fs);

		if (srcDir)
		{
			IO::StreamReader *reader;
			Text::StringBuilderUTF8 sb2;
			sptr = Text::StrConcat(sbuff, srcDir);
			*sptr++ = IO::Path::PATH_SEPERATOR;
			Text::StrConcat(sptr, (const UTF8Char*)"web.txt");
			Data::ArrayList<const UTF8Char *> webLines;
			UTF8Char *sarr[4];
			Bool found;
			NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
			NEW_CLASS(reader, IO::StreamReader(fs, 65001));
			sb.ClearStr();
			while (reader->ReadLine(&sb, 512))
			{
				sb2.AppendC(sb.ToString(), sb.GetLength());
				if (Text::StrSplit(sarr, 4, sb2.ToString(), '\t') == 3)
				{
					found = false;
					i = j;
					while (i-- > 0)
					{
						img = imgList->GetItem(i);
						if (img->GetImgItem()->GetFileType() == OrganImageItem::FT_WEB_IMAGE)
						{
							if (img->GetImgItem()->GetDispName()->EndsWith(sarr[0]))
							{
								found = true;
								break;
							}
						}
					}
					if (!found)
					{
						webLines.Add(Text::StrCopyNew(sb.ToString()));
					}
				}
				sb.ClearStr();
				sb2.ClearStr();
			}
			DEL_CLASS(reader);
			DEL_CLASS(fs);

			sptr = Text::StrConcat(sbuff, srcDir);
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"web.txt");
			if (webLines.GetCount() > 0)
			{
				NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
				NEW_CLASS(writer, Text::UTF8Writer(fs));
				i = 0;
				j = webLines.GetCount();
				while (i < j)
				{
					writer->WriteLine(webLines.GetItem(i));
					Text::StrDelNew(webLines.GetItem(i));
					i++;
				}
				DEL_CLASS(writer);
				DEL_CLASS(fs);
			}
			else
			{
				IO::Path::DeleteFile(sbuff);
			}
		}
	}
	return true;
}

Bool SSWR::OrganMgr::OrganEnvDB::CombineSpecies(OrganSpecies *destSp, OrganSpecies *srcSp)
{
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("update species_book set species_id = "));
	sql.AppendInt32(destSp->GetSpeciesId());
	sql.AppendCmdC(UTF8STRC(" where species_id = "));
	sql.AppendInt32(srcSp->GetSpeciesId());
	if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) < 0)
	{
		return false;
	}
	sql.Clear();
	sql.AppendCmdC(UTF8STRC("update userfile set species_id = "));
	sql.AppendInt32(destSp->GetSpeciesId());
	sql.AppendCmdC(UTF8STRC(" where species_id = "));
	sql.AppendInt32(srcSp->GetSpeciesId());
	if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) < 0)
	{
		return false;
	}
	sql.Clear();
	sql.AppendCmdC(UTF8STRC("delete from species where id = "));
	sql.AppendInt32(srcSp->GetSpeciesId());
	if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) < 0)
	{
		return false;
	}
	SpeciesInfo *spInfos = this->speciesMap->Get(srcSp->GetSpeciesId());
	SpeciesInfo *spInfod = this->speciesMap->Get(destSp->GetSpeciesId());
	if (spInfos && spInfod)
	{
		spInfod->files->AddAll(spInfos->files);
		spInfos->files->Clear();
	}
	return true;
}

UOSInt SSWR::OrganMgr::OrganEnvDB::GetWebUsers(Data::ArrayList<OrganWebUser*> *userList)
{
	UOSInt initCnt = userList->GetCount();
	OrganWebUser *user;
	DB::DBReader *r = this->db->ExecuteReaderC(UTF8STRC("select id, userName, watermark, userType from webuser"));
	if (r)
	{
		while (r->ReadNext())
		{
			user = MemAlloc(OrganWebUser, 1);
			user->id = r->GetInt32(0);
			user->userName = r->GetNewStr(1);
			user->watermark = r->GetNewStr(2);
			user->userType = (UserType)r->GetInt32(3);
			userList->Add(user);
		}
		this->db->CloseReader(r);
	}
	return userList->GetCount() - initCnt;
}

Bool SSWR::OrganMgr::OrganEnvDB::AddWebUser(const UTF8Char *userName, const UTF8Char *pwd, const UTF8Char *watermark, UserType userType)
{
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("select id from webuser where userName = "));
	sql.AppendStrUTF8(userName);
	DB::DBReader *r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r == 0)
		return false;
	if (r->ReadNext())
	{
		this->db->CloseReader(r);
		return false;
	}
	this->db->CloseReader(r);
	sql.Clear();
	sql.AppendCmdC(UTF8STRC("insert into webuser (userName, pwd, watermark, userType) values ("));
	sql.AppendStrUTF8(userName);
	sql.AppendCmdC(UTF8STRC(", "));
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
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(watermark);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt32(userType);
	sql.AppendCmdC(UTF8STRC(")"));
	return this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) >= 1;
}

Bool SSWR::OrganMgr::OrganEnvDB::ModifyWebUser(Int32 id, const UTF8Char *userName, const UTF8Char *pwd, const UTF8Char *watermark)
{
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("select id from webuser where userName = "));
	sql.AppendStrUTF8(userName);
	DB::DBReader *r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r == 0)
		return false;
	if (r->ReadNext())
	{
		if (r->GetInt32(0) != id)
		{
			this->db->CloseReader(r);
			return false;
		}
	}
	this->db->CloseReader(r);
	sql.Clear();
	sql.AppendCmdC(UTF8STRC("update webuser set userName = "));
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
		sql.AppendCmdC(UTF8STRC(", pwd = "));
		sql.AppendStrUTF8(sbuff);
	}
	sql.AppendCmdC(UTF8STRC(", watermark = "));
	sql.AppendStrUTF8(watermark);
	sql.AppendCmdC(UTF8STRC(" where id = "));
	sql.AppendInt32(id);
	return this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) >= 0;
}

void SSWR::OrganMgr::OrganEnvDB::ReleaseWebUsers(Data::ArrayList<OrganWebUser*> *userList)
{
	OrganWebUser *user;
	UOSInt i = userList->GetCount();
	while (i-- > 0)
	{
		user = userList->GetItem(i);
		user->userName->Release();
		user->watermark->Release();
		MemFree(user);
	}
	userList->Clear();
}

Bool SSWR::OrganMgr::OrganEnvDB::IsSpeciesBookExist(Int32 speciesId, Int32 bookId)
{
	DB::SQLBuilder sql(this->db);
	DB::DBReader *r;
	sql.AppendCmdC(UTF8STRC("select dispName from species_book where species_id = "));
	sql.AppendInt32(speciesId);
	sql.AppendCmdC(UTF8STRC(" and book_id = "));
	sql.AppendInt32(bookId);
	r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r == 0)
	{
		r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
		if (r == 0)
		{
			return false;
		}
	}
	Bool found = false;
	found = r->ReadNext();
	this->db->CloseReader(r);
	return found;
}

Bool SSWR::OrganMgr::OrganEnvDB::NewSpeciesBook(Int32 speciesId, Int32 bookId, const UTF8Char *dispName)
{
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("insert into species_book (species_id, book_id, dispName) values ("));
	sql.AppendInt32(speciesId);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt32(bookId);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(dispName);
	sql.AppendCmdC(UTF8STRC(")"));
	if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) == -2)
	{
		return false;
	}
	else
	{
		return true;
	}
}

UOSInt SSWR::OrganMgr::OrganEnvDB::GetSpeciesBooks(Data::ArrayList<SpeciesBook*> *items, Int32 speciesId)
{
	DB::SQLBuilder sql(this->db);
	DB::DBReader *r;
	OrganBook *book;
	SpeciesBook *spBook;
	OSInt i;
	UOSInt j;

	sql.AppendCmdC(UTF8STRC("select book_id, dispName, id from species_book where species_id = "));
	sql.AppendInt32(speciesId);
	r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r == 0)
		return 0;
	j = 0;
	while (r->ReadNext())
	{
		i = this->bookIds->SortedIndexOf(r->GetInt32(0));
		if (i >= 0)
		{
			book = this->bookObjs->GetItem((UOSInt)i);
			spBook = MemAlloc(SpeciesBook, 1);
			spBook->book = book;
			spBook->dispName = r->GetNewStr(1);
			spBook->id = r->GetInt32(2);
			items->Add(spBook);
			j++;
		}
	}
	this->db->CloseReader(r);
	return j;
}

void SSWR::OrganMgr::OrganEnvDB::ReleaseSpeciesBooks(Data::ArrayList<SpeciesBook*> *items)
{
	SpeciesBook *spBook;
	UOSInt i;
	i = items->GetCount();
	while (i-- > 0)
	{
		spBook = items->GetItem(i);
		SDEL_STRING(spBook->dispName);
		MemFree(spBook);
	}
}

Int32 SSWR::OrganMgr::OrganEnvDB::NewBook(const UTF8Char *title, const UTF8Char *author, const UTF8Char *press, Data::DateTime *publishDate, const UTF8Char *url)
{
	OrganBook *book;
	DB::SQLBuilder sql(this->db);
	UOSInt i;

	sql.AppendCmdC(UTF8STRC("insert into book (title, dispAuthor, press, publishDate, groupId, url) values ("));
	sql.AppendStrUTF8(title);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(author);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(press);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendDate(publishDate);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt32(0);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(url);
	sql.AppendCmdC(UTF8STRC(")"));
	if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) < -1)
	{
		return 0;
	}
	Int32 id = db->GetLastIdentity32();
	NEW_CLASS(book, OrganBook());
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

Bool SSWR::OrganMgr::OrganEnvDB::AddDataFile(const UTF8Char *fileName)
{
	IO::StmData::FileData *fd;
	IO::ParserType t;
	IO::ParsedObject *pobj;
	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	pobj = this->parsers->ParseFile(fd, &t);
	DEL_CLASS(fd);
	Data::DateTime startDT;
	Data::DateTime endDT;
	const UTF8Char *oriFileName;
	UTF8Char sbuff[512];
	const UTF8Char *dataFileName;
	Int32 fileType = 0;
	DataFileInfo *dataFile;
	Bool chg = false;
	Data::DateTime dt;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;

	i = Text::StrLastIndexOf(fileName, IO::Path::PATH_SEPERATOR);
	oriFileName = &fileName[i + 1];


	if (pobj)
	{
		if (t == IO::ParserType::MapLayer)
		{
			Map::IMapDrawLayer *lyr = (Map::IMapDrawLayer*)pobj;
			if (lyr->GetObjectClass() == Map::IMapDrawLayer::OC_GPS_TRACK)
			{
				Bool found = false;
				Map::GPSTrack *gpsTrk = (Map::GPSTrack*)lyr;
				Map::GPSTrack::GPSRecord *recArr;
				i = 0;
				j = gpsTrk->GetTrackCnt();
				while (i < j)
				{
					recArr = gpsTrk->GetTrack(i, &l);
					k = 0;
					while (k < l)
					{
						dt.SetTicks(recArr[k].utcTimeTicks);
						if (found)
						{
							if (startDT.CompareTo(&dt) > 0)
							{
								startDT.SetValue(&dt);
							}
							if (endDT.CompareTo(&dt) < 0)
							{
								endDT.SetValue(&dt);
							}
						}
						else
						{
							found = true;
							startDT.SetValue(&dt);
							endDT.SetValue(&dt);
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
			Data::ArrayList<const UTF8Char *> tables;
			db->GetTableNames(&tables);
			if (tables.GetCount() == 2)
			{
				if (Text::StrEquals(tables.GetItem(0), (const UTF8Char*)"Setting") && Text::StrEquals(tables.GetItem(1), (const UTF8Char*)"Records"))
				{
					DB::DBReader *reader = db->GetTableData((const UTF8Char*)"Records", 0, 0, 0, 0, 0);
					Bool found = false;
					if (reader)
					{
						while (reader->ReadNext())
						{
							if (!found)
							{
								found = true;
								reader->GetDate(1, &startDT);
							}
							reader->GetDate(1, &endDT);
						}
						db->CloseReader(reader);
					}

					if (found)
					{
						fileType = 2;
					}
				}
			}
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
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"DataFile");
		IO::Path::CreateDirectory(sbuff);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		dataFileName = sptr;
		sptr = Text::StrInt32(sptr, this->userId);
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"_");
		sptr = Text::StrInt64(sptr, startDT.ToTicks());
		i = Text::StrLastIndexOf(fileName, '.');
		if (i != INVALID_INDEX)
		{
			Text::StrConcat(sptr, &fileName[i]);
		}
		if (IO::FileUtil::CopyFile(fileName, sbuff, IO::FileUtil::FileExistAction::Fail, 0, 0))
		{
			DB::SQLBuilder sql(this->db);
			sql.AppendCmdC(UTF8STRC("insert into datafile (fileType, startTime, endTime, oriFileName, dataFileName, webuser_id) values ("));
			sql.AppendInt32(fileType);
			sql.AppendCmdC(UTF8STRC(", "));
			sql.AppendDate(&startDT);
			sql.AppendCmdC(UTF8STRC(", "));
			sql.AppendDate(&endDT);
			sql.AppendCmdC(UTF8STRC(", "));
			sql.AppendStrUTF8(oriFileName);
			sql.AppendCmdC(UTF8STRC(", "));
			sql.AppendStrUTF8(dataFileName);
			sql.AppendCmdC(UTF8STRC(", "));
			sql.AppendInt32(this->userId);
			sql.AppendCmdC(UTF8STRC(")"));
			if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) >= 1)
			{
				chg = true;
				dataFile = MemAlloc(DataFileInfo, 1);
				dataFile->id = this->db->GetLastIdentity32();
				dataFile->fileType = fileType;
				dataFile->startTimeTicks = startDT.ToTicks();
				dataFile->endTimeTicks = endDT.ToTicks();
				dataFile->webUserId = this->userId;
				dataFile->oriFileName = Text::String::NewNotNull(oriFileName);
				dataFile->fileName = Text::String::NewNotNull(dataFileName);
				this->dataFiles->Add(dataFile);

				if (fileType == 1)
				{
					WebUserInfo *webUser = this->GetWebUser(dataFile->webUserId);
					i = webUser->gpsFileIndex->SortedInsert(dataFile->startTimeTicks);
					webUser->gpsFileObj->Insert(i, dataFile);
					
					NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
					pobj = this->parsers->ParseFileType(fd, IO::ParserType::MapLayer);
					DEL_CLASS(fd);

					if (pobj)
					{
						Map::GPSTrack *gpsTrk = (Map::GPSTrack*)pobj;

						OSInt startIndex = webUser->userFileIndex->SortedIndexOf(dataFile->startTimeTicks);
						OSInt endIndex = webUser->userFileIndex->SortedIndexOf(dataFile->endTimeTicks);
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
							SSWR::OrganMgr::UserFileInfo *userFile = webUser->userFileObj->GetItem((UOSInt)startIndex);
							if (userFile->lat == 0 && userFile->lon == 0)
							{
								Double lat = 0;
								Double lon = 0;
								dt.SetTicks(userFile->captureTimeTicks);
								gpsTrk->GetLatLonByTime(&dt, &lat, &lon);
								this->UpdateUserFilePos(userFile, &dt, lat, lon);
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

Bool SSWR::OrganMgr::OrganEnvDB::DelDataFile(DataFileInfo *dataFile)
{
	Bool found = false;
	UTF8Char sbuff[512];
	UOSInt i;
	i = this->dataFiles->GetCount();
	while (i-- > 0)
	{
		if (dataFile == this->dataFiles->GetItem(i))
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
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"DataFile");
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(sptr, dataFile->fileName->v, dataFile->fileName->leng);
	if (IO::Path::GetPathType(sbuff) != IO::Path::PathType::File)
		return false;
	IO::Path::DeleteFile(sbuff);
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("delete from datafile where id = "));
	sql.AppendInt32(dataFile->id);
	this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength());
	this->dataFiles->RemoveAt(i);
	if (dataFile->fileType == 1)
	{
		OSInt j;
		WebUserInfo *webUser = this->userMap->Get(dataFile->webUserId);
		j = webUser->gpsFileIndex->SortedIndexOf(dataFile->startTimeTicks);
		if (j >= 0)
		{
			webUser->gpsFileIndex->RemoveAt((UOSInt)j);
			webUser->gpsFileObj->RemoveAt((UOSInt)j);
		}
	}
	ReleaseDataFile(dataFile);
	return true;
}

Bool SSWR::OrganMgr::OrganEnvDB::GetGPSPos(Int32 userId, Data::DateTime *t, Double *lat, Double *lon)
{
	OSInt i;
	WebUserInfo *webUser;
	DataFileInfo *dataFile;
	UTF8Char u8buff[512];
	UTF8Char *sptr;
	IO::StmData::FileData *fd;
	if (this->gpsTrk == 0 || this->gpsUserId != userId || this->gpsStartTime->CompareTo(t) > 0 || this->gpsEndTime->CompareTo(t) < 0)
	{
		SDEL_CLASS(this->gpsTrk);
		this->gpsUserId = userId;
		webUser = this->GetWebUser(userId);
		i = webUser->gpsFileIndex->SortedIndexOf(t->ToTicks());
		if (i < 0)
		{
			i = ~i - 1;
		}
		dataFile = webUser->gpsFileObj->GetItem((UOSInt)i);
		if (dataFile != 0)
		{
			this->gpsStartTime->SetTicks(dataFile->startTimeTicks);
			this->gpsEndTime->SetTicks(dataFile->endTimeTicks);
			sptr = this->cfgDataPath->ConcatTo(u8buff);
			if (sptr[-1] != IO::Path::PATH_SEPERATOR)
			{
				*sptr++ = IO::Path::PATH_SEPERATOR;
			}
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"DataFile");
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrConcatC(sptr, dataFile->fileName->v, dataFile->fileName->leng);
			NEW_CLASS(fd, IO::StmData::FileData(u8buff, false));
			Map::IMapDrawLayer *lyr = (Map::IMapDrawLayer*)this->parsers->ParseFileType(fd, IO::ParserType::MapLayer);
			DEL_CLASS(fd);
			if (lyr)
			{
				if (lyr->GetObjectClass() == Map::IMapDrawLayer::OC_GPS_TRACK)
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
		this->gpsTrk->GetLatLonByTime(t, lat, lon);
		return true;
	}
	else
	{
		*lat = 0;
		*lon = 0;
		return false;
	}
}

Map::GPSTrack *SSWR::OrganMgr::OrganEnvDB::OpenGPSTrack(DataFileInfo *dataFile)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::StmData::FileData *fd;

	sptr = this->cfgDataPath->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"DataFile");
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = dataFile->fileName->ConcatTo(sptr);
	NEW_CLASS(fd, IO::StmData::FileData(sbuff, false));
	Map::GPSTrack *trk = 0;
	Map::IMapDrawLayer *lyr = (Map::IMapDrawLayer*)this->parsers->ParseFileType(fd, IO::ParserType::MapLayer);
	if (lyr->GetObjectClass() == Map::IMapDrawLayer::OC_GPS_TRACK)
	{
		trk = (Map::GPSTrack*)lyr;
	}
	else
	{
		trk = 0;
		DEL_CLASS(lyr);
	}
	DEL_CLASS(fd);
	return trk;
}

void SSWR::OrganMgr::OrganEnvDB::UpdateUserFileCrop(UserFileInfo *userFile, Double cropLeft, Double cropTop, Double cropRight, Double cropBottom)
{
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("update userfile set cropLeft = "));
	sql.AppendDbl(cropLeft);
	sql.AppendCmdC(UTF8STRC(", cropTop = "));
	sql.AppendDbl(cropTop);
	sql.AppendCmdC(UTF8STRC(", cropRight = "));
	sql.AppendDbl(cropRight);
	sql.AppendCmdC(UTF8STRC(", cropBottom = "));
	sql.AppendDbl(cropBottom);
	sql.AppendCmdC(UTF8STRC(", prevUpdated = 1"));
	sql.AppendCmdC(UTF8STRC(" where id = "));
	sql.AppendInt32(userFile->id);
	if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) >= 0)
	{
		userFile->cropLeft = cropLeft;
		userFile->cropTop = cropTop;
		userFile->cropRight = cropRight;
		userFile->cropBottom = cropBottom;
	}
}

void SSWR::OrganMgr::OrganEnvDB::UpdateUserFileRot(UserFileInfo *userFile, Int32 rotType)
{
	if (userFile->rotType != rotType)
	{
		DB::SQLBuilder sql(this->db);
		sql.AppendCmdC(UTF8STRC("update userfile set rotType = "));
		sql.AppendInt32(rotType);
		sql.AppendCmdC(UTF8STRC(", prevUpdated = 1"));
		sql.AppendCmdC(UTF8STRC(" where id = "));
		sql.AppendInt32(userFile->id);
		this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength());
		userFile->rotType = rotType;
	}
}

Bool SSWR::OrganMgr::OrganEnvDB::UpdateUserFilePos(UserFileInfo *userFile, Data::DateTime *captureTime, Double lat, Double lon)
{
	Bool succ = false;
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("update userfile set captureTime = "));
	sql.AppendDate(captureTime);
	sql.AppendCmdC(UTF8STRC(", lat = "));
	sql.AppendDbl(lat);
	sql.AppendCmdC(UTF8STRC(", lon = "));
	sql.AppendDbl(lon);
	sql.AppendCmdC(UTF8STRC(" where id = "));
	sql.AppendInt32(userFile->id);
	if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) >= 0)
	{
		succ = true;
		userFile->captureTimeTicks = captureTime->ToTicks();
		userFile->lat = lat;
		userFile->lon = lon;
	}
	return succ;
}

Bool SSWR::OrganMgr::OrganEnvDB::GetUserFilePath(UserFileInfo *userFile, Text::StringBuilderUTF *sb)
{
	Data::DateTime dt;
	dt.SetTicks(userFile->fileTimeTicks);
	dt.ToUTCTime();
	sb->Append(this->cfgDataPath);
	if (!sb->EndsWith((Char)IO::Path::PATH_SEPERATOR))
	{
		sb->AppendChar(IO::Path::PATH_SEPERATOR, 1);
	}
	sb->AppendC(UTF8STRC("UserFile"));
	sb->AppendChar(IO::Path::PATH_SEPERATOR, 1);
	sb->AppendI32(userFile->webuserId);
	sb->AppendChar(IO::Path::PATH_SEPERATOR, 1);
	UTF8Char u8buff[10];
	dt.ToString(u8buff, "yyyyMM");
	sb->Append(u8buff);
	sb->AppendChar(IO::Path::PATH_SEPERATOR, 1);
	sb->AppendC(userFile->dataFileName->v, userFile->dataFileName->leng);
	return true;
}

Bool SSWR::OrganMgr::OrganEnvDB::UpdateUserFileDesc(UserFileInfo *userFile, const UTF8Char *descript)
{
	Bool succ = false;
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("update userfile set descript = "));
	sql.AppendStrUTF8(descript);
	sql.AppendCmdC(UTF8STRC(" where id = "));
	sql.AppendInt32(userFile->id);
	if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) >= 0)
	{
		succ = true;
		SDEL_STRING(userFile->descript);
		if (descript)
		{
			userFile->descript = Text::String::NewNotNull(descript);
		}
	}
	return succ;
}

Bool SSWR::OrganMgr::OrganEnvDB::UpdateUserFileLoc(UserFileInfo *userFile, const UTF8Char *location)
{
	Bool succ = false;
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("update userfile set location = "));
	sql.AppendStrUTF8(location);
	sql.AppendCmdC(UTF8STRC(" where id = "));
	sql.AppendInt32(userFile->id);
	if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) >= 0)
	{
		succ = true;
		SDEL_STRING(userFile->location);
		if (location)
		{
			userFile->location = Text::String::NewNotNull(location);
		}
	}
	return succ;
}

void SSWR::OrganMgr::OrganEnvDB::UpdateWebFileCrop(WebFileInfo *wfile, Double cropLeft, Double cropTop, Double cropRight, Double cropBottom)
{
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("update webfile set cropLeft = "));
	sql.AppendDbl(cropLeft);
	sql.AppendCmdC(UTF8STRC(", cropTop = "));
	sql.AppendDbl(cropTop);
	sql.AppendCmdC(UTF8STRC(", cropRight = "));
	sql.AppendDbl(cropRight);
	sql.AppendCmdC(UTF8STRC(", cropBottom = "));
	sql.AppendDbl(cropBottom);
	sql.AppendCmdC(UTF8STRC(", prevUpdated = 1"));
	sql.AppendCmdC(UTF8STRC(" where id = "));
	sql.AppendInt32(wfile->id);
	if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) >= 0)
	{
		wfile->cropLeft = cropLeft;
		wfile->cropTop = cropTop;
		wfile->cropRight = cropRight;
		wfile->cropBottom = cropBottom;
	}
}

void SSWR::OrganMgr::OrganEnvDB::TripReload(Int32 cateId)
{
	DB::DBReader *r;
	DB::SQLBuilder sql(this->db);
	UTF8Char sbuff[256];
	UTF8Char sbuff2[256];
	this->TripRelease();

	LocationType *locT;
	Location *loc;
	Trip *t;
	
	sql.Clear();
	sql.AppendCmdC(UTF8STRC("select locType, engName, chiName from location_type where cate_id = "));
	sql.AppendInt32(cateId);
	sql.AppendCmdC(UTF8STRC(" order by locType"));
	r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
	{
		while (r->ReadNext())
		{
			r->GetStr(1, sbuff, sizeof(sbuff));
			r->GetStr(2, sbuff2, sizeof(sbuff2));
			NEW_CLASS(locT, LocationType(r->GetInt32(0), sbuff, sbuff2))
			this->locType->Add(locT);
		}
		this->db->CloseReader(r);
	}

	sql.Clear();
	sql.AppendCmdC(UTF8STRC("select id, parentId, ename, cname, locType from location where cate_id = "));
	sql.AppendInt32(cateId);
	sql.AppendCmdC(UTF8STRC(" order by id"));
	r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
	{
		while (r->ReadNext())
		{
			r->GetStr(2, sbuff, sizeof(sbuff));
			r->GetStr(3, sbuff2, sizeof(sbuff2));
			NEW_CLASS(loc, Location(r->GetInt32(0), r->GetInt32(1), sbuff, sbuff2, r->GetInt32(4)));
			this->locs->Add(loc);
		}
		this->db->CloseReader(r);
	}

	sql.Clear();
	sql.AppendCmdC(UTF8STRC("select fromDate, toDate, locId from trip where cate_id = "));
	sql.AppendInt32(cateId);
	sql.AppendCmdC(UTF8STRC(" order by fromDate"));
	r = db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
	{
		Data::DateTime dt;
		Data::DateTime dt2;

		while (r->ReadNext())
		{
			r->GetDate(0, &dt);
			r->GetDate(1, &dt2);
			NEW_CLASS(t, Trip(&dt, &dt2, r->GetInt32(2)));
			this->trips->Add(t);
		}
		this->db->CloseReader(r);
	}
}

Bool SSWR::OrganMgr::OrganEnvDB::TripAdd(Data::DateTime *fromDate, Data::DateTime *toDate, Int32 locId)
{
	OSInt i = this->TripGetIndex(fromDate);
	OSInt j = this->TripGetIndex(toDate);
	OSInt k = this->LocationGetIndex(locId);
	if (i < 0 && j < 0 && k >= 0)
	{
		if (i == j)
		{
			DB::SQLBuilder sql(this->db);
			sql.AppendCmdC(UTF8STRC("insert into trip (fromDate, toDate, locId, cate_id) values ("));
			sql.AppendDate(fromDate);
			sql.AppendCmdC(UTF8STRC(", "));
			sql.AppendDate(toDate);
			sql.AppendCmdC(UTF8STRC(", "));
			sql.AppendInt32(locId);
			sql.AppendCmdC(UTF8STRC(", "));
			sql.AppendInt32(this->currCate->cateId);
			sql.AppendCmdC(UTF8STRC(")"));
			if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) != -2)
			{
				Trip *t;
				NEW_CLASS(t, Trip(fromDate, toDate, locId));
				this->trips->Insert((UOSInt)(-i - 1), t);
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

Bool SSWR::OrganMgr::OrganEnvDB::LocationUpdate(Int32 locId, const UTF8Char *engName, const UTF8Char *chiName)
{
	Location *loc = this->LocationGet(locId);
	if (loc == 0)
		return false;

	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("update location set ename="));
	sql.AppendStrUTF8(engName);
	sql.AppendCmdC(UTF8STRC(", cname="));
	sql.AppendStrUTF8(chiName);
	sql.AppendCmdC(UTF8STRC(" where id="));
	sql.AppendInt32(locId);
	if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) == -2)
		return false;
	else
	{
		SDEL_STRING(loc->ename);
		SDEL_STRING(loc->cname);
		loc->ename = Text::String::NewOrNull(engName);
		loc->cname = Text::String::NewOrNull(chiName);
		return true;
	}
}

Bool SSWR::OrganMgr::OrganEnvDB::LocationAdd(Int32 locId, const UTF8Char *engName, const UTF8Char *chiName)
{
	Location *loc;
	Int32 lType;
	if (locId == 0)
	{
		loc = 0;
		lType = -1;
	}
	else
	{
        loc = this->LocationGet(locId);
		if (loc == 0)
			return false;
        lType = (Int32)this->LocationGetTypeIndex(loc->locType);
        if (lType < 0)
			return false;
	}
	
	if ((UOSInt)lType + 1 >= this->locType->GetCount())
		return false;
	lType = this->locType->GetItem((UInt32)lType + 1)->id;
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(UTF8STRC("insert into location (parentId, ename, cname, cate_id, locType) values ("));
	sql.AppendInt32(locId);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(engName);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendStrUTF8(chiName);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt32(this->currCate->cateId);
	sql.AppendCmdC(UTF8STRC(", "));
	sql.AppendInt32(lType);
	sql.AppendCmdC(UTF8STRC(")"));
	if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) == -2)
		return false;
	else
	{
		DB::DBReader *r;
		sql.Clear();
		sql.AppendCmdC(UTF8STRC("select max(id) from location where parentId = "));
		sql.AppendInt32(locId);
		r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
		if (r)
		{
			if (r->ReadNext())
			{
				Location *l;
				NEW_CLASS(l, Location(r->GetInt32(0), locId, engName, chiName, lType));
				this->locs->Add(l);
				this->db->CloseReader(r);
				return true;
			}
			else
			{
				this->db->CloseReader(r);
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
	OrganBook *book;
	Text::StringBuilderUTF8 sb;
	DB::DBReader *r;
	Data::DateTime dt;
	UOSInt i;

	NEW_CLASS(this->bookIds, Data::ArrayListInt32());
	NEW_CLASS(this->bookObjs, Data::ArrayList<OrganBook*>());
	r = this->db->ExecuteReaderC(UTF8STRC("select id, title, dispAuthor, press, publishDate, groupId, url from book"));
	while (r->ReadNext())
	{
		NEW_CLASS(book, OrganBook());
		book->SetBookId(r->GetInt32(0));
		sb.ClearStr();
		r->GetStr(1, &sb);
		book->SetTitle(sb.ToString());
		sb.ClearStr();
		r->GetStr(2, &sb);
		book->SetDispAuthor(sb.ToString());
		sb.ClearStr();
		r->GetStr(3, &sb);
		book->SetPress(sb.ToString());
		r->GetDate(4, &dt);
		book->SetPublishDate(&dt);
		book->SetGroupId(r->GetInt32(5));
		sb.ClearStr();
		r->GetStr(6, &sb);
		book->SetURL(sb.ToString());
		i = this->bookIds->SortedInsert(book->GetBookId());
		this->bookObjs->Insert(i, book);
	}
	this->db->CloseReader(r);
}

Media::ImageList *SSWR::OrganMgr::OrganEnvDB::ParseImage(OrganImageItem *img, UserFileInfo **outUserFile, WebFileInfo **outWebFile)
{
	IO::StmData::FileData *fd;
	IO::ParserType pt;
	if (img->GetFileType() == OrganImageItem::FT_USERFILE)
	{
		UserFileInfo *userFile = img->GetUserFile();
		if (outUserFile)
		{
			*outUserFile = userFile;
		}
		if (outWebFile)
		{
			*outWebFile = 0;
		}
		if (userFile)
		{
			UTF8Char sbuff[512];
			UTF8Char *sptr;
			Data::DateTime dt;
			dt.SetTicks(userFile->fileTimeTicks);
			dt.ToUTCTime();
			sptr = this->cfgDataPath->ConcatTo(sbuff);
			if (sptr[-1] != IO::Path::PATH_SEPERATOR)
			{
				*sptr++ = IO::Path::PATH_SEPERATOR;
			}
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"UserFile");
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrInt32(sptr, userFile->webuserId);
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = dt.ToString(sptr, "yyyyMM");
			*sptr++ = IO::Path::PATH_SEPERATOR;
			if (userFile->fileType == 3)
			{
				sptr = Text::StrInt64(sptr, userFile->fileTimeTicks);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"_");
				sptr = Text::StrHexVal32(sptr, userFile->crcVal);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)".png");
			}
			else
			{
				sptr = userFile->dataFileName->ConcatTo(sptr);
			}
			NEW_CLASS(fd, IO::StmData::FileData(sbuff, false));
			IO::ParsedObject *pobj = parsers->ParseFile(fd, &pt);
			DEL_CLASS(fd);
			if (pobj == 0)
			{
				return 0;
			}
			if (pt == IO::ParserType::ImageList)
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
							simg->RotateImage(Media::StaticImage::RT_CW90);
						}
						else if (userFile->rotType == 2)
						{
							simg->RotateImage(Media::StaticImage::RT_CW180);
						}
						else if (userFile->rotType == 3)
						{
							simg->RotateImage(Media::StaticImage::RT_CW270);
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
	else if (img->GetFileType() == OrganImageItem::FT_WEBFILE)
	{
		WebFileInfo *wfile = img->GetWebFile();
		if (outUserFile)
		{
			*outUserFile = 0;
		}
		if (outWebFile)
		{
			*outWebFile = wfile;
		}
		if (wfile)
		{
			UTF8Char sbuff[512];
			UTF8Char *sptr;
			sptr = this->cfgDataPath->ConcatTo(sbuff);
			if (sptr[-1] != IO::Path::PATH_SEPERATOR)
			{
				*sptr++ = IO::Path::PATH_SEPERATOR;
			}
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"WebFile");
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrInt32(sptr, wfile->id >> 10);
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrInt32(sptr, wfile->id);
			sptr = Text::StrConcat(sptr, (const UTF8Char*)".jpg");

			NEW_CLASS(fd, IO::StmData::FileData(sbuff, false));
			IO::ParsedObject *pobj = parsers->ParseFile(fd, &pt);
			DEL_CLASS(fd);
			if (pobj == 0)
			{
				return 0;
			}
			if (pt == IO::ParserType::ImageList)
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
							simg->RotateImage(Media::StaticImage::RT_CW90);
						}
						else if (userFile->rotType == 2)
						{
							simg->RotateImage(Media::StaticImage::RT_CW180);
						}
						else if (userFile->rotType == 3)
						{
							simg->RotateImage(Media::StaticImage::RT_CW270);
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
	else
	{
		if (outUserFile)
		{
			*outUserFile = 0;
		}
		if (outWebFile)
		{
			*outWebFile = 0;
		}
		NEW_CLASS(fd, IO::StmData::FileData(img->GetFullName()->v, false));
		IO::ParsedObject *pobj = parsers->ParseFile(fd, &pt);
		DEL_CLASS(fd);
		if (pobj == 0)
		{
			return 0;
		}
		if (pt == IO::ParserType::ImageList)
		{
			return (Media::ImageList*)pobj;
		}
		DEL_CLASS(pobj);
		return 0;
	}
}

Media::ImageList *SSWR::OrganMgr::OrganEnvDB::ParseSpImage(OrganSpecies *sp)
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UTF8Char *cols[4];
	const UTF8Char *coverName = sp->GetPhoto();
	IO::Path::FindFileSession *sess;
	IO::Path::PathType pt;
	UOSInt i;
	IO::StmData::FileData *fd;
	IO::ParsedObject *pobj = 0;
	if (sp->GetPhotoId() != 0)
	{
		UserFileInfo *userFile = this->userFileMap->Get(sp->GetPhotoId());
		if (userFile)
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
		SpeciesInfo *spInfo = this->speciesMap->Get(sp->GetSpeciesId());
		if (spInfo)
		{
			WebFileInfo *wfile = spInfo->wfileMap->Get(sp->GetPhotoWId());
			if (wfile)
			{
				return this->ParseWebImage(wfile);
			}
		}
		return 0;
	}

	if (coverName && coverName[0] == '*')
	{
		coverName = &coverName[1];
	}

	sptr = sbuff;
	if (!this->cateIsFullDir)
	{
		sptr = this->cfgImgDirBase->ConcatTo(sptr);
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = Text::StrConcatC(sptr, this->currCate->srcDir->v, this->currCate->srcDir->leng);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcat(sptr, sp->GetDirName());
	*sptr++ = IO::Path::PATH_SEPERATOR;
	Text::StrConcat(Text::StrConcat(sptr, coverName), (const UTF8Char*)".*");
	sess = IO::Path::FindFile(sbuff);
	if (sess)
	{
		if (IO::Path::FindNextFile(sptr, sess, 0, &pt, 0))
		{
			if (pt == IO::Path::PathType::File)
			{
				i = Text::StrLastIndexOf(sptr, '.');
				if (i == INVALID_INDEX)
				{

				}
				else if (Text::StrCompareICase(&sptr[i], (const UTF8Char*)".JPG") == 0)
				{
					NEW_CLASS(fd, IO::StmData::FileData(sbuff, false));
					pobj = this->parsers->ParseFile(fd, 0);
					DEL_CLASS(fd);
				}
				else if (Text::StrCompareICase(&sptr[i], (const UTF8Char*)".TIF") == 0)
				{
					NEW_CLASS(fd, IO::StmData::FileData(sbuff, false));
					pobj = this->parsers->ParseFile(fd, 0);
					DEL_CLASS(fd);
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
			Text::StrConcat(sptr, (const UTF8Char*)"web.txt");
			if (IO::Path::GetPathType(sbuff) == IO::Path::PathType::File)
			{
				Text::UTF8Reader *reader;
				IO::FileStream *fs;
				NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
				NEW_CLASS(reader, Text::UTF8Reader(fs));

				while (reader->ReadLine(sbuff2, 511))
				{
					if (Text::StrSplit(cols, 4, sbuff2, '\t') == 3)
					{
						sptr2 = Text::StrConcat(sptr, (const UTF8Char*)"web");
						*sptr2++ = IO::Path::PATH_SEPERATOR;
						sptr2 = Text::StrConcat(sptr2, cols[0]);
						if (Text::StrStartsWith(sptr, coverName))
						{
							NEW_CLASS(fd, IO::StmData::FileData(sbuff, false));
							pobj = this->parsers->ParseFile(fd, 0);
							DEL_CLASS(fd);
							break;
						}
					}
				}
				DEL_CLASS(reader);
				DEL_CLASS(fs);
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

Media::ImageList *SSWR::OrganMgr::OrganEnvDB::ParseFileImage(UserFileInfo *userFile)
{
	IO::StmData::FileData *fd;
	IO::ParserType pt;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Data::DateTime dt;
	dt.SetTicks(userFile->fileTimeTicks);
	dt.ToUTCTime();
	sptr = this->cfgDataPath->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"UserFile");
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, userFile->webuserId);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = dt.ToString(sptr, "yyyyMM");
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = userFile->dataFileName->ConcatTo(sptr);
	NEW_CLASS(fd, IO::StmData::FileData(sbuff, false));
	IO::ParsedObject *pobj = parsers->ParseFile(fd, &pt);
	DEL_CLASS(fd);
	if (pobj == 0)
	{
		return 0;
	}
	if (pt == IO::ParserType::ImageList)
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
					simg->RotateImage(Media::StaticImage::RT_CW90);
				}
				else if (userFile->rotType == 2)
				{
					simg->RotateImage(Media::StaticImage::RT_CW180);
				}
				else if (userFile->rotType == 3)
				{
					simg->RotateImage(Media::StaticImage::RT_CW270);
				}
			}
		}
		return imgList;
	}
	DEL_CLASS(pobj);
	return 0;
}

Media::ImageList *SSWR::OrganMgr::OrganEnvDB::ParseWebImage(WebFileInfo *wfile)
{
	IO::StmData::FileData *fd;
	IO::ParserType pt;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = this->cfgDataPath->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"WebFile");
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, wfile->id >> 10);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, wfile->id);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)".jpg");
	NEW_CLASS(fd, IO::StmData::FileData(sbuff, false));
	IO::ParsedObject *pobj = parsers->ParseFile(fd, &pt);
	DEL_CLASS(fd);
	if (pobj == 0)
	{
		return 0;
	}
	if (pt == IO::ParserType::ImageList)
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
					simg->RotateImage(Media::StaticImage::RT_CW90);
				}
				else if (userFile->rotType == 2)
				{
					simg->RotateImage(Media::StaticImage::RT_CW180);
				}
				else if (userFile->rotType == 3)
				{
					simg->RotateImage(Media::StaticImage::RT_CW270);
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
	Bool found = false;
	UTF8Char sbuff[256];
	UTF8Char sbuff2[256];
	Int32 photoGroup;
	Int32 photoSpecies;
	DB::DBReader *r;
	DB::SQLBuilder sql(this->db);
	Text::StringBuilderUTF8 sb;

	sql.AppendCmdC(UTF8STRC("select group_id, sci_name from species where chi_name = "));
	sql.AppendStrUTF8(searchStr);
	sql.AppendCmdC(UTF8STRC(" or sci_name = "));
	sql.AppendStrUTF8(searchStr);
	r =this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
	{
		if (r->ReadNext())
		{
			found = true;
			*parentId = r->GetInt32(0);
			r->GetStr(1, resultStr, resultStrBuffSize);
		}
		this->db->CloseReader(r);
	}
	if (!found)
	{
		sql.Clear();
		sql.AppendCmdC(UTF8STRC("select group_id, sci_name from species s, species_book sb where sb.dispName = "));
		sql.AppendStrUTF8(searchStr);
		sql.AppendCmdC(UTF8STRC(" and s.id = sb.species_id group by sb.species_id"));
		r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
		if (r)
		{
			if (r->ReadNext())
			{
				found = true;
				*parentId = r->GetInt32(0);
				r->GetStr(1, resultStr, resultStrBuffSize);
			}
			this->db->CloseReader(r);
		}
	}

	sql.Clear();
	if (found)
	{
		sql.AppendCmdC(UTF8STRC("select id, group_type, eng_name, chi_name, description, parent_id, photo_group, photo_species, idKey, flags from groups where id = "));
		sql.AppendInt32(*parentId);
	}
	else
	{
		sql.AppendCmdC(UTF8STRC("select id, group_type, eng_name, chi_name, description, parent_id, photo_group, photo_species, idKey, flags from groups where eng_name like "));
		Text::StrConcat(Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"%"), searchStr), (const UTF8Char*)"%");
		sql.AppendStrUTF8(sbuff);
		sql.AppendCmdC(UTF8STRC(" or chi_name like "));
		sql.AppendStrUTF8(sbuff);
		Text::StrConcat(resultStr, searchStr);
	}
	r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
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

			r->GetStr(2, sbuff, sizeof(sbuff));//engName
			r->GetStr(3, sbuff2, sizeof(sbuff2));//chiName
			NEW_CLASS(foundGroup, OrganGroup());
			foundGroup->SetGroupId(r->GetInt32(0));
			foundGroup->SetCName(sbuff2);
			foundGroup->SetEName(sbuff);
			foundGroup->SetGroupType(r->GetInt32(1));
			sb.ClearStr();
			r->GetStr(4, &sb);
			foundGroup->SetDesc(sb.ToString());
			foundGroup->SetPhotoGroup(photoGroup);
			foundGroup->SetPhotoSpecies(photoSpecies);
			sb.ClearStr();
			r->GetStr(8, &sb);
			foundGroup->SetIDKey(sb.ToString());
			flags = r->GetInt32(9);
			foundGroup->SetAdminOnly(flags & 1);

			*parentId = r->GetInt32(5);

			if (Text::StrEquals(sbuff, searchStr) || Text::StrEquals(sbuff2, searchStr))
			{
			}
			else
			{
				while (r->ReadNext())
				{
					r->GetStr(2, sbuff, sizeof(sbuff));//engName
					r->GetStr(3, sbuff2, sizeof(sbuff2));//chiName
					if (Text::StrEquals(sbuff, searchStr) || Text::StrEquals(sbuff2, searchStr))
					{
						photoGroup = -1;
						if (!r->IsNull(6))
							photoGroup = r->GetInt32(6);
						photoSpecies = -1;
						if (!r->IsNull(7))
							photoSpecies = r->GetInt32(7);


						foundGroup->SetGroupId(r->GetInt32(0));
						sb.ClearStr();
						sb.Append(sbuff2);
						foundGroup->SetCName(sb.ToString());
						sb.ClearStr();
						sb.Append(sbuff);
						foundGroup->SetEName(sb.ToString());
						foundGroup->SetGroupType(r->GetInt32(1));
						sb.ClearStr();
						r->GetStr(4, &sb);
						foundGroup->SetDesc(sb.ToString());
						foundGroup->SetPhotoGroup(photoGroup);
						foundGroup->SetPhotoSpecies(photoSpecies);
						sb.ClearStr();
						r->GetStr(8, &sb);
						foundGroup->SetIDKey(sb.ToString());
						flags = r->GetInt32(9);
						foundGroup->SetAdminOnly(flags & 1);

						*parentId = r->GetInt32(5);
						break;
					}
				}
			}
		}
		this->db->CloseReader(r);
	}
	return foundGroup;
}

void SSWR::OrganMgr::OrganEnvDB::LoadGroupTypes()
{
	OrganGroupType *grpType;
	DB::DBReader *r;
	DB::SQLBuilder *sql;
	UOSInt i;
	Int32 seq;
	UTF8Char cname[64];
	UTF8Char ename[64];
	i = this->grpTypes->GetCount();
	while (i-- > 0)
	{
		grpType = this->grpTypes->GetItem(i);
		DEL_CLASS(grpType);
	}
	this->grpTypes->Clear();

	NEW_CLASS(sql, DB::SQLBuilder(this->db));
	sql->AppendCmdC(UTF8STRC("Select seq, chi_name, eng_name from group_type where cate_id = "));
	sql->AppendInt32(this->currCate->cateId);
	sql->AppendCmdC(UTF8STRC(" order by seq"));
	r = this->db->ExecuteReaderC(sql->ToString(), sql->GetLength());
	while (r->ReadNext())
	{
		seq = r->GetInt32(0);
		r->GetStr(1, cname, sizeof(cname));
		r->GetStr(2, ename, sizeof(ename));
		NEW_CLASS(grpType, OrganGroupType(seq, cname, ename));
		this->grpTypes->Add(grpType);
	}
	this->db->CloseReader(r);
	DEL_CLASS(sql);	
}

Data::Int32Map<Data::ArrayList<SSWR::OrganMgr::OrganGroup*>*> *SSWR::OrganMgr::OrganEnvDB::GetGroupTree()
{
	Data::ArrayList<OrganGroup*> *grps;
	Data::Int32Map<Data::ArrayList<OrganGroup*>*> *grpTree;
	DB::DBReader *r;
	Int32 parId;

	NEW_CLASS(grpTree, Data::Int32Map<Data::ArrayList<OrganGroup*>*>());

	DB::SQLBuilder sql(this->db);
	Text::StringBuilderUTF8 sb;
	sql.AppendCmdC(UTF8STRC("select id, group_type, eng_name, chi_name, description, photo_group, photo_species, idKey, parent_id, flags from groups where cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	sql.AppendCmdC(UTF8STRC(" order by parent_id, eng_name"));
	r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
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
			OrganGroup *newGrp;
			NEW_CLASS(newGrp, OrganGroup());
			newGrp->SetGroupId(r->GetInt32(0));
			sb.ClearStr();
			r->GetStr(3, &sb);
			newGrp->SetCName(sb.ToString());
			sb.ClearStr();
			r->GetStr(2, &sb);
			newGrp->SetEName(sb.ToString());
			newGrp->SetGroupType(r->GetInt32(1));
			sb.ClearStr();
			r->GetStr(4, &sb);
			newGrp->SetDesc(sb.ToString());
			newGrp->SetPhotoGroup(photoGroup);
			newGrp->SetPhotoSpecies(photoSpecies);
			sb.ClearStr();
			r->GetStr(7, &sb);
			newGrp->SetIDKey(sb.ToString());
			flags = r->GetInt32(9);
			newGrp->SetAdminOnly(flags & 1);

			parId = r->GetInt32(8);
			grps = grpTree->Get(parId);
			if (grps == 0)
			{
				NEW_CLASS(grps, Data::ArrayList<OrganGroup*>());
				grpTree->Put(parId, grps);
			}
			grps->Add(newGrp);
		}
		this->db->CloseReader(r);
	}
	return grpTree;
}

Data::Int32Map<Data::ArrayList<SSWR::OrganMgr::OrganSpecies*>*> *SSWR::OrganMgr::OrganEnvDB::GetSpeciesTree()
{
	Data::ArrayList<OrganSpecies*> *sps;
	Data::Int32Map<Data::ArrayList<OrganSpecies*>*> *spTree;
	DB::DBReader *r;
	Int32 parId;

	NEW_CLASS(spTree, Data::Int32Map<Data::ArrayList<OrganSpecies*>*>());

	DB::SQLBuilder sql(this->db);
	Text::StringBuilderUTF8 sb;
	sql.Clear();
	sql.AppendCmdC(UTF8STRC("SELECT id, chi_name, sci_name, eng_name, description, dirName, photo, idKey, flags, group_id, photoId, mapColor FROM species where cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	sql.AppendCmdC(UTF8STRC(" order by group_id, sci_name"));
	r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
	{
		while (r->ReadNext())
		{
			OrganSpecies *sp;
			NEW_CLASS(sp, OrganSpecies());
			
			sp->SetSpeciesId(r->GetInt32(0));
			sb.ClearStr();
			r->GetStr(1, &sb);
			sp->SetCName(sb.ToString());
			sb.ClearStr();
			r->GetStr(2, &sb);
			sp->SetSName(sb.ToString());
			sb.ClearStr();
			r->GetStr(3, &sb);
			sp->SetEName(sb.ToString());
			sb.ClearStr();
			r->GetStr(4, &sb);
			sp->SetDesc(sb.ToString());
			sb.ClearStr();
			r->GetStr(5, &sb);
			sp->SetDirName(sb.ToString());
			sb.ClearStr();
			r->GetStr(6, &sb);
			sp->SetPhoto(sb.ToString());
			sb.ClearStr();
			r->GetStr(7, &sb);
			sp->SetIDKey(sb.ToString());
			sp->SetFlags(r->GetInt32(8));
			sp->SetPhotoId(r->GetInt32(10));
			sp->SetMapColor((UInt32)r->GetInt32(11));

			parId = r->GetInt32(9);

			sps = spTree->Get(parId);
			if (sps == 0)
			{
				NEW_CLASS(sps, Data::ArrayList<OrganSpecies*>());
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
	Data::ArrayList<UserFileInfo *> *userFileList = this->userFileMap->GetValues();
	Data::Int32Map<OrganSpecies*> *speciesMap;
	Data::ArrayListInt32 speciesList;
	UserFileInfo *userFile;
	OrganSpecies *species;
	Data::DateTime dt;
	Trip *tr;
	NEW_CLASS(speciesMap, Data::Int32Map<OrganSpecies*>());
	UOSInt i = 0;
	UOSInt j = userFileList->GetCount();
	while (i < j)
	{
		userFile = userFileList->GetItem(i);
		if (userFile->location == 0)
		{
			species = speciesMap->Get(userFile->speciesId);
			if (species == 0 && speciesList.SortedIndexOf(userFile->speciesId) < 0)
			{
				speciesList.SortedInsert(userFile->speciesId);
				species = this->GetSpecies(userFile->speciesId);
				if (species)
				{
					speciesMap->Put(species->GetSpeciesId(), species);
				}
			}
			if (species)
			{
				dt.SetTicks(userFile->captureTimeTicks);
				tr = this->TripGet(userId, &dt);
				if (tr)
				{
					this->UpdateUserFileLoc(userFile, this->LocationGet(tr->locId)->cname->v);
				}
			}
		}
		i++;
	}
	Data::ArrayList<OrganSpecies*> *spList = speciesMap->GetValues();
	i = spList->GetCount();
	while (i-- > 0)
	{
		species = spList->GetItem(i);
		DEL_CLASS(species);
	}
	DEL_CLASS(speciesMap);

	//this->UpgradeDB();
	//this->UpgradeDB2();

	/*	Data::ArrayList<UserFileInfo *> *userFileList = this->userFileMap->GetValues();
	UserFileInfo *userFile;
	DB::SQLBuilder sql(this->db);
	WChar sbuff[512];
	WChar *sptr;
	Data::DateTime dt;
	Text::StringBuilderW sb;
	OSInt i = 0;
	OSInt j = userFileList->GetCount();
	dt.ToUTCTime();

	while (i < j)
	{
		userFile = userFileList->GetItem(i);
		dt.SetTicks(userFile->fileTimeTicks);
		sptr = Text::StrConcat(sbuff, this->cfgDataPath);
		if (sptr[-1] != '\\')
		{
			*sptr++ = '\\';
		}
		sptr = Text::StrConcat(sptr, L"UserFile\\");
		sptr = Text::StrInt32(sptr, userFile->webuserId);
		sptr = Text::StrConcat(sptr, L"\\");
		sptr = dt.ToString(sptr, L"yyyyMM");
		sptr = Text::StrConcat(sptr, L"\\");
		Text::StrConcat(sptr, userFile->dataFileName);
		if (userFile->fileType == 1)
		{
			Media::EXIFData *exif = ParseJPGExif(sbuff);
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
					sql.AppendCmdC(UTF8STRC("update userfile set camera = ");
					sql.AppendStr(sb.ToString());
					sql.AppendCmdC(UTF8STRC(" where id = ");
					sql.AppendInt32(userFile->id);
					this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength());
				}
			}
		}
		
		i++;
	}*/
}

void SSWR::OrganMgr::OrganEnvDB::UpgradeDB()
{
	Data::ArrayList<OrganSpecies*> spList;
	UOSInt i;
	OrganSpecies *sp;
	this->GetGroupAllSpecies(&spList, 0);
	i = spList.GetCount();
	while (i-- > 0)
	{
		sp = spList.GetItem(i);
		this->UpgradeFileStruct(sp);
		DEL_CLASS(sp);
	}
}

typedef struct
{
	Int32 id;
	Int32 photoId;
	Int32 photoWId;
	Text::String *dirName;
	Text::String *photoName;
} UpgradeDBSpInfo;

void SSWR::OrganMgr::OrganEnvDB::UpgradeDB2()
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UTF8Char *cols[4];
	UInt32 crcVal;
	UOSInt i;
	UOSInt j;
	Int32 id;
	Bool allSucc;
	Data::ArrayList<UpgradeDBSpInfo*> spList;
	UpgradeDBSpInfo *sp;
	Text::StringBuilderUTF8 sb;
	DB::DBReader *r;
	UInt8 crcBuff[4];
	Crypto::Hash::CRC32R crc;
	OSInt coverFound;
	WebFileInfo *wfile;
	SpeciesInfo *spInfo;
	Bool isCover;

	DB::SQLBuilder sql(this->db);
	r = this->db->ExecuteReaderC(UTF8STRC("select id, dirName, photo, photoId, photoWId from species"));
	if (r)
	{
		while (r->ReadNext())
		{
			sp = MemAlloc(UpgradeDBSpInfo, 1);
			sp->id = r->GetInt32(0);
			sp->dirName = r->GetNewStr(1);
			sp->photoName = r->GetNewStr(2);
			sp->photoId = r->GetInt32(3);
			sp->photoWId = r->GetInt32(4);
			spList.Add(sp);
		}
		this->db->CloseReader(r);
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
		sptr = Text::StrConcatC(sptr, this->currCate->srcDir->v, this->currCate->srcDir->leng);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = sp->dirName->ConcatTo(sptr);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		Text::StrConcat(sptr, (const UTF8Char *)"web.txt");
		if (IO::Path::GetPathType(sbuff) == IO::Path::PathType::File)
		{
			allSucc = true;
			Text::UTF8Reader *reader;
			IO::FileStream *fs;
			NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
			NEW_CLASS(reader, Text::UTF8Reader(fs));

			while (reader->ReadLine(sbuff2, 511))
			{
				if (Text::StrSplit(cols, 4, sbuff2, '\t') == 3)
				{
					if (cols[0][8] == '.')
					{
						sptr2 = Text::StrConcat(sptr, (const UTF8Char*)"web");
						*sptr2++ = IO::Path::PATH_SEPERATOR;
						sptr2 = Text::StrConcat(sptr2, cols[0]);

						crc.Clear();
						crc.Calc(cols[1], Text::StrCharCnt(cols[1]));
						crc.GetValue(crcBuff);
						crcVal = ReadMUInt32(crcBuff);

						sql.Clear();
						sql.AppendCmdC(UTF8STRC("insert into webfile (species_id, crcVal, imgUrl, srcUrl, prevUpdated, cropLeft, cropTop, cropRight, cropBottom, location) values ("));
						sql.AppendInt32(sp->id);
						sql.AppendCmdC(UTF8STRC(", "));
						sql.AppendInt32((Int32)crcVal);
						sql.AppendCmdC(UTF8STRC(", "));
						sql.AppendStrUTF8(cols[1]);
						sql.AppendCmdC(UTF8STRC(", "));
						sql.AppendStrUTF8(cols[2]);
						sql.AppendCmdC(UTF8STRC(", "));
						sql.AppendInt32(0);
						sql.AppendCmdC(UTF8STRC(", "));
						sql.AppendDbl(0);
						sql.AppendCmdC(UTF8STRC(", "));
						sql.AppendDbl(0);
						sql.AppendCmdC(UTF8STRC(", "));
						sql.AppendDbl(0);
						sql.AppendCmdC(UTF8STRC(", "));
						sql.AppendDbl(0);
						sql.AppendCmdC(UTF8STRC(", "));
						sql.AppendStrUTF8((const UTF8Char*)"");
						sql.AppendCmdC(UTF8STRC(")"));
						if (this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) > 0)
						{
							id = this->db->GetLastIdentity32();
							
							if (sp->photoId == 0 && sp->photoWId == 0 && sp->photoName && sp->photoName->StartsWith((const UTF8Char*)"web\\") && Text::StrStartsWith(cols[0], &sp->photoName->v[4]))
							{
								isCover = true;
							}
							else
							{
								isCover = false;
							}

							wfile = MemAlloc(WebFileInfo, 1);
							wfile->id = id;
							wfile->speciesId = sp->id;
							wfile->imgUrl = Text::String::NewNotNull(cols[1]);
							wfile->srcUrl = Text::String::NewNotNull(cols[2]);
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
							sptr2 = Text::StrConcat(sptr2, (const UTF8Char*)"WebFile");
							*sptr2++ = IO::Path::PATH_SEPERATOR;
							sptr2 = Text::StrInt32(sptr2, id >> 10);
							IO::Path::CreateDirectory(sbuff2);

							*sptr2++ = IO::Path::PATH_SEPERATOR;
							sptr2 = Text::StrInt32(sptr2, id);
							sptr2 = Text::StrConcat(sptr2, (const UTF8Char*)".jpg");
							if (!IO::FileUtil::MoveFile(sbuff, sbuff2, IO::FileUtil::FileExistAction::Fail, 0, 0))
							{
								allSucc = false;

								wfile->imgUrl->Release();
								wfile->srcUrl->Release();
								wfile->location->Release();
								MemFree(wfile);
							}
							else
							{
								if (isCover)
								{
									coverFound++;

									sql.Clear();
									sql.AppendCmdC(UTF8STRC("update species set photo = "));
									sql.AppendStrUTF8(0);
									sql.AppendCmdC(UTF8STRC(", photoWId = "));
									sql.AppendInt32(id);
									sql.AppendCmdC(UTF8STRC(" where id = "));
									sql.AppendInt32(sp->id);
									this->db->ExecuteNonQueryC(sql.ToString(), sql.GetLength());
								}
								spInfo = this->GetSpeciesInfo(sp->id, true);
								spInfo->wfileMap->Put(wfile->id, wfile);
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
			DEL_CLASS(reader);
			DEL_CLASS(fs);

			if (sp->photoId == 0 && sp->photoWId == 0 && sp->photoName && sp->photoName->StartsWith((const UTF8Char*)"web\\"))
			{
				if (coverFound != 1)
				{
					allSucc = false;
				}
			}

			if (allSucc)
			{
				Text::StrConcat(sptr, (const UTF8Char *)"web");
				if (IO::Path::RemoveDirectory(sbuff))
				{
					Text::StrConcat(sptr, (const UTF8Char *)"web.txt");
					IO::Path::DeleteFile(sbuff);
				}
			}
		}

		sp->dirName->Release();
		sp->photoName->Release();
		MemFree(sp);
		i++;
	}
}

void SSWR::OrganMgr::OrganEnvDB::UpgradeFileStruct(OrganSpecies *sp)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	const UTF8Char *coverName = sp->GetPhoto();
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
	sptr = Text::StrConcatC(sptr, this->currCate->srcDir->v, this->currCate->srcDir->leng);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcat(sptr, sp->GetDirName());
	*sptr++ = IO::Path::PATH_SEPERATOR;
	Text::StrConcat(sptr, IO::Path::ALL_FILES);
	sess = IO::Path::FindFile(sbuff);
	if (sess)
	{
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, 0))
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

				i = Text::StrLastIndexOf(sptr, '.');
				if (i == INVALID_INDEX)
				{

				}
				else if (Text::StrEqualsICase(&sptr[i], (const UTF8Char*)".JPG") || Text::StrEqualsICase(&sptr[i], (const UTF8Char*)".PCX") || Text::StrEqualsICase(&sptr[i], (const UTF8Char*)".WAV"))
				{
					Int32 fileId = 0;
					this->AddSpeciesFile(sp, sbuff, isCoverPhoto, true, &fileId);
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
	UTF8Char *sptr3;
	IO::FileStream *fs;
	Text::UTF8Writer *writer;
	Text::StringBuilderUTF8 sb;
	Bool valid = true;
	sptr = Text::StrConcat(sbuff, folder);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	Text::StrConcat(sptr, (const UTF8Char*)"OrganWeb64.cfg");
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		valid = false;
	}
	else
	{
		NEW_CLASS(writer, Text::UTF8Writer(fs));
		writer->WriteLine((const UTF8Char*)"ScreenSize=1200");
		writer->WriteLine((const UTF8Char*)"MDBFile=OrganWeb.mdb");
		writer->WriteLine((const UTF8Char*)"ImageDir=Image\\");
		writer->WriteLine((const UTF8Char*)"SvrPort=8080");
		writer->WriteLine((const UTF8Char*)"Watermark=sswroom");
		writer->WriteLine((const UTF8Char*)"CacheDir=Cache\\");
		writer->WriteLine((const UTF8Char*)"DataDir=Data");
		DEL_CLASS(writer);
	}
	DEL_CLASS(fs);
	if (!valid)
		return;
	Text::StrConcat(sptr, (const UTF8Char*)"Cache");
	IO::Path::CreateDirectory(sbuff);
	Exporter::MDBExporter exporter;
	Text::StrConcat(sptr, (const UTF8Char*)"OrganWeb.mdb");
	exporter.ExportFile(0, sbuff, this->db->GetConn(), 0);

	sptr2 = Text::StrConcat(sptr, (const UTF8Char*)"Image");
	IO::Path::CreateDirectory(sbuff);
	*sptr2++ = IO::Path::PATH_SEPERATOR;
	sptr3 = this->cfgImgDirBase->ConcatTo(sbuff2);
	*sptr3++ = IO::Path::PATH_SEPERATOR;
	DB::DBReader *r = this->db->ExecuteReaderC(UTF8STRC("select srcDir from category"));
	if (r)
	{
		while (r->ReadNext())
		{
			sb.ClearStr();
			r->GetStr(0, &sb);
			if (sb.GetLength() > 0)
			{
				Text::StrConcat(sptr2, sb.ToString());
				Text::StrConcat(sptr3, sb.ToString());
				IO::FileUtil::CopyDir(sbuff2, sbuff, IO::FileUtil::FileExistAction::Fail, 0, 0);
			}
		}
		this->db->CloseReader(r);
	}

	sptr2 = Text::StrConcat(sptr, (const UTF8Char*)"Data");
	IO::Path::CreateDirectory(sbuff);
	*sptr2++ = IO::Path::PATH_SEPERATOR;
	Text::StrConcat(sptr2, (const UTF8Char*)"DataFile");

	sptr3 = this->cfgDataPath->ConcatTo(sbuff2);
	*sptr3++ = IO::Path::PATH_SEPERATOR;
	Text::StrConcat(sptr3, (const UTF8Char*)"DataFile");
	IO::FileUtil::CopyDir(sbuff2, sbuff, IO::FileUtil::FileExistAction::Fail, 0, 0);

	r = this->db->ExecuteReaderC(UTF8STRC("select fileType, fileTime, webuser_id, dataFileName from userfile"));
	if (r)
	{
		Media::Resizer::LanczosResizerH8_8 *resizer;
		Exporter::GUIJPGExporter *exporter;
		Data::DateTime dt;
		void *param;
		NEW_CLASS(exporter, Exporter::GUIJPGExporter());
		NEW_CLASS(resizer, Media::Resizer::LanczosResizerH8_8(4, 3, Media::AT_NO_ALPHA));
		
		sptr2 = Text::StrConcat(sptr2, (const UTF8Char*)"UserFile");
		*sptr2++ = IO::Path::PATH_SEPERATOR;

		sptr3 = this->cfgDataPath->ConcatTo(sbuff2);
		*sptr3++ = IO::Path::PATH_SEPERATOR;
		sptr3 = Text::StrConcat(sptr3, (const UTF8Char*)"UserFile");
		*sptr3++ = IO::Path::PATH_SEPERATOR;

		while (r->ReadNext())
		{
			sb.ClearStr();
			Int32 userId = r->GetInt32(2);
			Int32 fileType = r->GetInt32(0);
			sb.AppendI32(userId);
			sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
			r->GetDate(1, &dt);
			dt.ToString(sptr2, "yyyyMM");
			sb.Append(sptr2);
			Text::StrConcat(sptr2, sb.ToString());
			IO::Path::CreateDirectory(sbuff);

			sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
			r->GetStr(3, &sb);
			
			Text::StrConcat(sptr2, sb.ToString());
			Text::StrConcat(sptr3, sb.ToString());
			if (IO::Path::GetPathType(sbuff) == IO::Path::PathType::Unknown)
			{
				if (fileType == 1 && Text::StrEndsWithICase(sptr2, (const UTF8Char*)".jpg"))
				{
					IO::FileStream *fs;
					IO::StmData::FileData *fd;
					NEW_CLASS(fd, IO::StmData::FileData(sbuff2, false));
					Media::ImageList *imgList = (Media::ImageList*)this->parsers->ParseFileType(fd, IO::ParserType::ImageList);
					DEL_CLASS(fd);

					if (imgList)
					{
						UInt32 delay;
						imgList->ToStaticImage(0);
						Media::StaticImage *simg = (Media::StaticImage*)imgList->GetImage(0, &delay);
						Media::StaticImage *newImg;
						if (simg)
						{
							if (simg->info->dispWidth > 1920 || simg->info->dispHeight > 1920)
							{
								resizer->SetTargetWidth(1920);
								resizer->SetTargetHeight(1920);
								resizer->SetResizeAspectRatio(Media::IImgResizer::RAR_KEEPAR);
								newImg = resizer->ProcessToNew(simg);
								if (newImg)
								{
									imgList->ReplaceImage(0, newImg);
								}
							}

						}
						param = exporter->CreateParam(imgList);
						exporter->SetParamInt32(param, 0, 95);
						NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
						exporter->ExportFile(fs, sbuff, imgList, param);
						DEL_CLASS(fs);
						DEL_CLASS(imgList);
					}
					else
					{
						imgList = 0;
					}
				}
				else
				{
					IO::FileUtil::CopyFile(sbuff2, sbuff, IO::FileUtil::FileExistAction::Overwrite, 0, 0);
				}
			}
		}
		this->db->CloseReader(r);
		DEL_CLASS(resizer);
		DEL_CLASS(exporter);
	}
}
