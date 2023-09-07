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
	this->db = 0;

	if (cfg == 0)
	{
		this->errType = ERR_CONFIG;
		return;
	}
	NotNullPtr<Text::String> nns;
	Text::String *cfgMySQLHost;
	Text::String *cfgMySQLDB;
	Text::String *cfgDSN;
	Text::String *cfgUID;
	Text::String *cfgPassword;

	cfgMySQLHost = cfg->GetValue(CSTR("MySQLHost"));
	cfgMySQLDB = cfg->GetValue(CSTR("MySQLDB"));
	cfgDSN = cfg->GetValue(CSTR("DBDSN"));
	cfgUID = cfg->GetValue(CSTR("DBUID"));
	cfgPassword = cfg->GetValue(CSTR("DBPwd"));
	this->cfgImgDirBase = cfg->GetValue(CSTR("ImageDir"));
	this->cfgDataPath = cfg->GetValue(CSTR("DataDir"));
	this->cfgCacheDir = cfg->GetValue(CSTR("CacheDir"));
	this->cfgMapFont = cfg->GetValue(CSTR("MapFont"));
	Text::String *userId = cfg->GetValue(CSTR("WebUser"));

	if (this->cfgImgDirBase == 0 || this->cfgImgDirBase->leng == 0 || this->cfgDataPath == 0 || this->cfgDataPath->leng == 0 || this->cfgCacheDir == 0 || this->cfgCacheDir->leng == 0)
	{
		this->errType = ERR_CONFIG;
		return;
	}
	if (!userId->ToInt32(this->userId))
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
	if (cfgMySQLDB && nns.Set(cfgMySQLHost))
	{
		this->db = Net::MySQLTCPClient::CreateDBTool(this->sockf, nns, cfgMySQLDB, Text::String::OrEmpty(cfgUID), Text::String::OrEmpty(cfgPassword), &this->log, CSTR_NULL);
	}
	else if (cfgDSN)
	{
		this->db = DB::ODBCConn::CreateDBTool(Text::String::OrEmpty(cfgDSN), cfgUID, cfgPassword, 0, &this->log, CSTR_NULL);
	}
	if (db == 0)
	{
		this->errType = ERR_DB;
		return;
	}

	DB::DBReader *r = this->db->ExecuteReader(CSTR("select cate_id, chi_name, dirName, srcDir from category"));
	if (r)
	{
		while (r->ReadNext())
		{
			cate = MemAlloc(Category, 1);
			cate->cateId = r->GetInt32(0);
			cate->chiName = r->GetNewStrNN(1);
			cate->dirName = r->GetNewStrNN(2);
			cate->srcDir = r->GetNewStr(3);
			this->categories.Add(cate);
		}
		this->db->CloseReader(r);
	}
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("select id from webuser where id = "));
	sql.AppendInt32(this->userId);
	r = this->db->ExecuteReader(sql.ToCString());
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
	r = this->db->ExecuteReader(CSTR("select id, fileType, startTime, endTime, oriFileName, dataFileName, webuser_id from datafile order by id"));
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
		this->db->CloseReader(r);
	}

	r = this->db->ExecuteReader(CSTR("select id, fileType, oriFileName, fileTime, lat, lon, webuser_id, species_id, captureTime, dataFileName, crcVal, rotType, camera, descript, cropLeft, cropTop, cropRight, cropBottom, location from userfile order by id"));
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
			userFileList.Add(userFile);
		}
		this->db->CloseReader(r);

		UserFileComparator comparator;
		Data::Sort::ArtificialQuickSort::Sort(&userFileList, &comparator);
		this->userFileMap.AllocSize(userFileList.GetCount());
		i = 0;
		j = userFileList.GetCount();
		while (i < j)
		{
			userFile = userFileList.GetItem(i);
			this->userFileMap.Put(userFile->id, userFile);
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
			k = webUser->userFileIndex.SortedInsert(userFile->fileTime);
			webUser->userFileObj.Insert(k, userFile);

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
				species->files.Add(userFile);
			}

			i++;
		}
	}

	r = this->db->ExecuteReader(CSTR("select id, species_id, crcVal, imgUrl, srcUrl, prevUpdated, cropLeft, cropTop, cropRight, cropBottom, location from webfile"));
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
			wfile->imgUrl = r->GetNewStrNN(3);
			wfile->srcUrl = r->GetNewStrNN(4);
			wfile->cropLeft = r->GetDbl(6);
			wfile->cropTop = r->GetDbl(7);
			wfile->cropRight = r->GetDbl(8);
			wfile->cropBottom = r->GetDbl(9);
			wfile->location = r->GetNewStrNN(10);
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
			species->wfileMap.Put(wfile->id, wfile);
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
	sql.AppendCmdC(CSTR("select id, group_type, eng_name, chi_name, description, photo_group, photo_species, idKey, flags from "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" where parent_id = "));
	sql.AppendInt32(grpId);
	sql.AppendCmdC(CSTR(" and cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	sql.AppendCmdC(CSTR(" order by eng_name"));
	r = this->db->ExecuteReader(sql.ToCString());
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
		this->db->CloseReader(r);
	}

    if (!found)
	{
		sql.Clear();
		sql.AppendCmdC(CSTR("SELECT id, chi_name, sci_name, eng_name, description, dirName, photo, idKey, flags, photoId, mapColor, photoWId FROM species where group_id = "));
		sql.AppendInt32(grpId);
		sql.AppendCmdC(CSTR(" and cate_id = "));
		sql.AppendInt32(this->currCate->cateId);
		sql.AppendCmdC(CSTR(" order by sci_name"));
		r = this->db->ExecuteReader(sql.ToCString());
		if (r)
		{
			while (r->ReadNext())
			{
				found = true;
				OrganSpecies *sp;
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

UOSInt SSWR::OrganMgr::OrganEnvDB::GetGroupImages(Data::ArrayList<OrganImageItem*> *items, OrganGroup *grp)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	DB::DBReader *r;
	if (grp == 0 || grp->GetGroupId() == -1)
		return 0;
	DB::SQLBuilder sql(this->db);
	Text::StringBuilderUTF8 sb;
	Bool exist;
	UOSInt retCnt = 0;

	sql.Clear();
	sql.AppendCmdC(CSTR("SELECT id, chi_name, sci_name, eng_name, description, dirName, photo, idKey, flags, photoId, photoWId FROM species where group_id = "));
	sql.AppendInt32(grp->GetGroupId());
	sql.AppendCmdC(CSTR(" and cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	sql.AppendCmdC(CSTR(" order by sci_name"));
	r = this->db->ExecuteReader(sql.ToCString());
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
				UserFileInfo *userFile = this->userFileMap.Get(photoId);
				if (userFile != 0)
				{
					item->SetFullName(userFile->dataFileName.Ptr());
					item->SetFileType(OrganImageItem::FileType::UserFile);
					item->SetUserFile(userFile);
				}
			}
			else if (photoWId != 0)
			{
				SpeciesInfo *sp = this->speciesMap.Get(spId);
				if (sp)
				{
					WebFileInfo *webFile = sp->wfileMap.Get(photoWId);
					if (webFile != 0)
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
				sb.Append(this->currCate->srcDir);
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

UOSInt SSWR::OrganMgr::OrganEnvDB::GetSpeciesImages(Data::ArrayList<OrganImageItem*> *items, OrganSpecies *sp)
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	Text::PString cols[4];
	Int32 newFlags = 0;
	const UTF8Char *coverName = STR_PTR(sp->GetPhoto());
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

	SpeciesInfo *spInfo = this->speciesMap.Get(sp->GetSpeciesId());
	if (spInfo)
	{
		UserFileInfo *userFile;
		i = 0;
		j = spInfo->files.GetCount();
		while (i < j)
		{
			userFile = spInfo->files.GetItem(i);
			if (coverId == userFile->id)
			{
				isCoverPhoto = true;
			}
			else
			{
				isCoverPhoto = false;
			}

			NEW_CLASS(imgItem, OrganImageItem(userFile->webuserId));
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

		WebFileInfo *webFile;

		i = 0;
		j = spInfo->wfileMap.GetCount();
		while (i < j)
		{
			webFile = spInfo->wfileMap.GetItem(i);

			NEW_CLASS(imgItem, OrganImageItem(0));
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
	sptr = Text::StrConcatC(sptr, this->currCate->srcDir->v, this->currCate->srcDir->leng);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = sp->GetDirName()->ConcatTo(sptr);
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
				if (coverName && Text::StrStartsWithICase(sptr, coverName))
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
					Media::EXIFData *exif = ParseJPGExif(CSTRP(sbuff, sptr2));
					NEW_CLASS(imgItem, OrganImageItem(this->userId));
					imgItem->SetDispName(CSTRP(sptr, sptr2));
					imgItem->SetIsCoverPhoto(isCoverPhoto);
					imgItem->SetFullName(CSTRP(sbuff, sptr2));
					if (exif)
					{
						Data::DateTime dt;
						dt.ToLocalTime();
						if (exif->GetPhotoDate(&dt))
						{
							imgItem->SetPhotoDate(Data::Timestamp(dt.ToTicks(), dt.GetTimeZoneQHR()));
						}
						DEL_CLASS(exif);
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
					Media::EXIFData *exif = ParseTIFExif(CSTRP(sbuff, sptr2));
					NEW_CLASS(imgItem, OrganImageItem(this->userId));
					imgItem->SetDispName(CSTRP(sptr, sptr2));
					imgItem->SetIsCoverPhoto(isCoverPhoto);
					imgItem->SetFullName(CSTRP(sbuff, sptr2));
					if (exif)
					{
						Data::DateTime dt;
						if (exif->GetPhotoDate(&dt))
						{
							imgItem->SetPhotoDate(Data::Timestamp(dt.ToTicks(), dt.GetTimeZoneQHR()));
						}
						DEL_CLASS(exif);
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
					NEW_CLASS(imgItem, OrganImageItem(this->userId));
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
					NEW_CLASS(imgItem, OrganImageItem(this->userId));
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
					NEW_CLASS(imgItem, OrganImageItem(this->userId));
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
					NEW_CLASS(imgItem, OrganImageItem(0));
					sptr2 = cols[0].ConcatTo(Text::StrConcatC(sptr, UTF8STRC("web\\")));
					imgItem->SetDispName(CSTRP(sptr, sptr2));
					imgItem->SetIsCoverPhoto(false);
					if (coverName)
					{
						if (Text::StrStartsWith(sptr, coverName))
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
		DB::SQLBuilder sql(this->db);
		sp->SetFlags(newFlags);
		sql.AppendCmdC(CSTR("update species set flags = "));
		sql.AppendInt32(newFlags);
		sql.AppendCmdC(CSTR(" where id = "));
		sql.AppendInt32(sp->GetSpeciesId());
		this->db->ExecuteNonQuery(sql.ToCString());
	}
	return retCnt;
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
		r = this->db->ExecuteReader(sql.ToCString());
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
	r = this->db->ExecuteReader(sql.ToCString());
	if (r)
	{
		OrganSpecies *sp;
		while (r->ReadNext())
		{
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
		r = this->db->ExecuteReader(sql.ToCString());
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
	r = this->db->ExecuteReader(sql.ToCString());
	if (r)
	{
		SpeciesInfo *species;
		while (r->ReadNext())
		{
			v = r->GetInt32(0);
			c = (UInt32)r->GetInt32(1);
			species = this->speciesMap.Get(v);
			if (species != 0)
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
	r = this->db->ExecuteReader(sql.ToCString());
	if (r)
	{
		while (r->ReadNext())
		{
			OrganSpecies *sp;
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

SSWR::OrganMgr::OrganGroup *SSWR::OrganMgr::OrganEnvDB::GetGroup(Int32 groupId, Int32 *parentId)
{
	DB::SQLBuilder sql(this->db);
	DB::DBReader *r;
	Int32 photoGroup;
	Int32 photoSpecies;
	OrganGroup *foundGroup = 0;
	sql.AppendCmdC(CSTR("select id, group_type, eng_name, chi_name, description, parent_id, photo_group, photo_species, idKey, flags from "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(groupId);
	r = this->db->ExecuteReader(sql.ToCString());
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
	sql.AppendCmdC(CSTR("SELECT id, chi_name, sci_name, eng_name, group_id, description, dirName, photo, idKey, flags, photoId, mapColor FROM species where id = "));
	sql.AppendInt32(speciesId);
	sql.AppendCmdC(CSTR(" and cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	r = this->db->ExecuteReader(sql.ToCString());
	if (r)
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
		this->db->CloseReader(r);
	}
	return sp;
}

UTF8Char *SSWR::OrganMgr::OrganEnvDB::GetSpeciesDir(OrganSpecies *sp, UTF8Char *sbuff)
{
	UTF8Char *sptr;
	if (this->currCate->srcDir->IndexOf(UTF8STRC(":\\")) != INVALID_INDEX)
	{
		sptr = Text::StrConcatC(sbuff, this->currCate->srcDir->v, this->currCate->srcDir->leng);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		return sp->GetDirName()->ConcatTo(sptr);
	}
	else
	{
		sptr = this->cfgImgDirBase->ConcatTo(sbuff);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrConcatC(sptr, this->currCate->srcDir->v, this->currCate->srcDir->leng);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = sp->GetDirName()->ConcatTo(sptr);
		return sptr;
	}
}

Bool SSWR::OrganMgr::OrganEnvDB::CreateSpeciesDir(OrganSpecies *sp)
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
	DB::SQLBuilder sql(this->db);
	DB::DBReader *r;
	Bool found = false;
	sql.AppendCmdC(CSTR("select id from species where sci_name = "));
	sql.AppendStrUTF8(sName);
	r = this->db->ExecuteReader(sql.ToCString());
	if (r)
	{
		found = r->ReadNext();
		this->db->CloseReader(r);
	}
	return found;
}

Bool SSWR::OrganMgr::OrganEnvDB::IsBookSpeciesExist(const UTF8Char *sName, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	Data::ArrayListInt32 spList;
	DB::SQLBuilder sql(this->db);
	DB::DBReader *r;
	OSInt si;
	UOSInt i;
	UOSInt j;
	Int32 spId;
	Bool found;
	sql.AppendCmdC(CSTR("select species_id from species_book where dispName = "));
	sql.AppendStrUTF8(sName);
	r = this->db->ExecuteReader(sql.ToCString());
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
		r = this->db->ExecuteReader(sql.ToCString());
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
	return this->db->ExecuteNonQuery(sql.ToCString()) >= -1;
}

Bool SSWR::OrganMgr::OrganEnvDB::DelSpecies(OrganSpecies *sp)
{
	DB::SQLBuilder sql(this->db);
	Bool succ = true;
	sql.Clear();
	sql.AppendCmdC(CSTR("update "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" set photo_group=NULL, photo_species=NULL where photo_species="));
	sql.AppendInt32(sp->GetSpeciesId());
	if (this->db->ExecuteNonQuery(sql.ToCString()) < -1)
		succ = false;

	sql.Clear();
	sql.AppendCmdC(CSTR("delete from species where id="));
	sql.AppendInt32(sp->GetSpeciesId());
	if (this->db->ExecuteNonQuery(sql.ToCString()) < -1)
		succ = false;

	sql.Clear();
	sql.AppendCmdC(CSTR("delete from species_book where species_id="));
	sql.AppendInt32(sp->GetSpeciesId());
	if (this->db->ExecuteNonQuery(sql.ToCString()) < -1)
		succ = false;
	return succ;
}

SSWR::OrganMgr::OrganEnvDB::FileStatus SSWR::OrganMgr::OrganEnvDB::AddSpeciesFile(OrganSpecies *sp, Text::CStringNN fileName, Bool firstPhoto, Bool moveFile, Int32 *fileId)
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
		IO::ParserType t;
		IO::ParsedObject *pobj;
		Bool valid = false;
		Data::Timestamp fileTime = Data::Timestamp(0, Data::DateTimeUtil::GetLocalTzQhr());
		Math::Coord2DDbl pos = Math::Coord2DDbl(0, 0);
		UserFileInfo *userFile;
		Text::String *camera = 0;
		UInt32 crcVal = 0;

		{
			IO::StmData::MemoryDataRef md(readBuff);
			pobj = this->parsers.ParseFile(md, &t);
		}
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
						fileTime = fileTime.SetTimeZoneQHR(Data::DateTimeUtil::GetLocalTzQhr());
						if (fileTime.ToUnixTimestamp() >= 946684800) //Y2000
						{
							this->GetGPSPos(this->userId, fileTime, &pos);
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
			WebUserInfo *webUser = this->GetWebUser(this->userId);
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

					userFile = webUser->userFileObj.GetItem(j);
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
					DB::SQLBuilder sql(this->db);
					sql.AppendCmdC(CSTR("insert into userfile (fileType, oriFileName, fileTime, lat, lon, webuser_id, species_id, captureTime, dataFileName, crcVal, camera, cropLeft, cropTop, cropRight, cropBottom) values ("));
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
					sql.AppendCmdC(CSTR(")"));
					if (this->db->ExecuteNonQuery(sql.ToCString()) > 0)
					{
						userFile = MemAlloc(UserFileInfo, 1);
						userFile->id = this->db->GetLastIdentity32();
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
						this->userFileMap.Put(userFile->id, userFile);

						SpeciesInfo *species = this->GetSpeciesInfo(userFile->speciesId, true);
						species->files.Add(userFile);

						webUser = this->GetWebUser(userFile->webuserId);
						k = webUser->userFileIndex.SortedInsert(userFile->fileTime);
						webUser->userFileObj.Insert(k, userFile);
						
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
		UInt32 crcVal;
		IO::ParsedObject *pobj;
		IO::ParserType t;
		Data::Timestamp fileTime = 0;
		UserFileInfo *userFile;
		Bool valid = false;
		Media::DrawImage *graphImg = 0;
		NotNullPtr<Media::DrawImage> img;
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
			pobj = this->parsers.ParseFile(fd, &t);
		}
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

					userFile = webUser->userFileObj.GetItem(j);
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
					DB::SQLBuilder sql(this->db);
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
					if (this->db->ExecuteNonQuery(sql.ToCString()) > 0)
					{
						userFile = MemAlloc(UserFileInfo, 1);
						userFile->id = this->db->GetLastIdentity32();
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
						this->userFileMap.Put(userFile->id, userFile);

						SpeciesInfo *species = this->GetSpeciesInfo(userFile->speciesId, true);
						species->files.Add(userFile);

						webUser = this->GetWebUser(userFile->webuserId);
						j = webUser->userFileIndex.SortedInsert(userFile->fileTime);
						webUser->userFileObj.Insert(j, userFile);
						
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

SSWR::OrganMgr::OrganEnvDB::FileStatus SSWR::OrganMgr::OrganEnvDB::AddSpeciesWebFile(OrganSpecies *sp, NotNullPtr<Text::String> srcURL, NotNullPtr<Text::String> imgURL, IO::Stream *stm, UTF8Char *webFileName)
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
		if (imgItem->GetFileType() == OrganImageItem::FileType::Webimage || imgItem->GetFileType() == OrganImageItem::FileType::WebFile)
		{
			if (imgItem->GetImgURL()->Equals(imgURL.Ptr()))
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

	DB::SQLBuilder sql(this->db);
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
	if (this->db->ExecuteNonQuery(sql.ToCString()) > 0)
	{
		id = this->db->GetLastIdentity32();
		
		wfile = MemAlloc(WebFileInfo, 1);
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

		UInt8 *buff = mstm.GetBuff(&i);
		{
			IO::FileStream fs(CSTRP(sbuff2, sptr2), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			fs.Write(buff, i);
		}

		if (firstPhoto)
		{
			sp->SetPhotoWId(id);
			this->SaveSpecies(sp);
		}
		spInfo = this->GetSpeciesInfo(sp->GetSpeciesId(), true);
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

SSWR::OrganMgr::OrganEnvDB::FileStatus SSWR::OrganMgr::OrganEnvDB::AddSpeciesWebFileOld(OrganSpecies *sp, Text::String *srcURL, Text::String *imgURL, IO::Stream *stm, UTF8Char *webFileName)
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
	Data::ArrayList<OrganImageItem*> *imgItems;
	OrganImageItem *imgItem;
	NEW_CLASS(imgItems, Data::ArrayList<OrganImageItem*>());
	this->GetSpeciesImages(imgItems, sp);
	i = imgItems->GetCount();
	firstPhoto = i == 0;
	while (i-- > 0)
	{
		imgItem = imgItems->GetItem(i);
		if (imgItem->GetFileType() == OrganImageItem::FileType::Webimage || imgItem->GetFileType() == OrganImageItem::FileType::WebFile)
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
		writer.WriteLineC(sb.ToString(), sb.GetLength());
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

Bool SSWR::OrganMgr::OrganEnvDB::UpdateSpeciesWebFile(OrganSpecies *sp, WebFileInfo *wfile, Text::String *srcURL, Text::String *location)
{
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("update webfile set srcUrl="));
	sql.AppendStr(srcURL);
	sql.AppendCmdC(CSTR(", location="));
	sql.AppendStr(location);
	sql.AppendCmdC(CSTR(" where id="));
	sql.AppendInt32(wfile->id);
	if (this->db->ExecuteNonQuery(sql.ToCString()) >= 0)
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

Bool SSWR::OrganMgr::OrganEnvDB::UpdateSpeciesWebFileOld(OrganSpecies *sp, const UTF8Char *webFileName, const UTF8Char *srcURL)
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
			writer.WriteLineC(sb2.ToString(), sb2.GetLength());
			sb.ClearStr();
		}
	}
	if (found)
	{
		IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		UOSInt size;
		UInt8 *buff = mstm.GetBuff(&size);
		fs.Write(buff, size);
	}
	return true;
}

Bool SSWR::OrganMgr::OrganEnvDB::SaveSpecies(OrganSpecies *sp)
{
	DB::SQLBuilder sql(this->db);
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
	return this->db->ExecuteNonQuery(sql.ToCString()) >= -1;
}

Bool SSWR::OrganMgr::OrganEnvDB::SaveGroup(OrganGroup *grp)
{
	Int32 flags = 0;
	if (grp->GetAdminOnly())
	{
		flags |= 1;
	}
	DB::SQLBuilder sql(this->db);
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
	return this->db->ExecuteNonQuery(sql.ToCString()) >= -1;
}

UOSInt SSWR::OrganMgr::OrganEnvDB::GetGroupCount(Int32 groupId)
{
	DB::SQLBuilder sql(this->db);
	DB::DBReader *r;
	sql.AppendCmdC(CSTR("select count(*) from "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" where parent_id = "));
	sql.AppendInt32(groupId);
	sql.AppendCmdC(CSTR(" and cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	r = this->db->ExecuteReader(sql.ToCString());
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
	sql.AppendCmdC(CSTR("select count(*) from species where group_id = "));
	sql.AppendInt32(groupId);
	sql.AppendCmdC(CSTR(" and cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
 	r = this->db->ExecuteReader(sql.ToCString());
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
	return this->db->ExecuteNonQuery(sql.ToCString()) == 1;
}

Bool SSWR::OrganMgr::OrganEnvDB::DelGroup(Int32 groupId)
{
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("delete from "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(groupId);
	sql.AppendCmdC(CSTR(" and cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	return this->db->ExecuteNonQuery(sql.ToCString()) == 1;
}

Bool SSWR::OrganMgr::OrganEnvDB::SetGroupDefSp(OrganGroup *grp, OrganImageItem *img)
{
/*	DB::SQLBuilder sql(this->db);
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
		this->db->ExecuteNonQuery(sql.ToCString());
	}
	else
	{
		sql.AppendCmdC(CSTR("update "));
		sql.AppendCml((const UTF8Char*)"groups");
		sql.AppendCmdC(CSTR(" set photo_group=NULL, photo_species="));
		sql.AppendInt32(sp->GetSpeciesId());
		sql.AppendCmdC(CSTR(" where photo_species="));
		sql.AppendInt32(oldId);
		this->db->ExecuteNonQuery(sql.ToCString());
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
		sql.AppendInt32(grpList->GetItem(i)->GetGroupId());
		i++;
	}
	sql.AppendCmdC(CSTR(")"));
	return this->db->ExecuteNonQuery(sql.ToCString()) >= 0;
}

Bool SSWR::OrganMgr::OrganEnvDB::MoveSpecies(Data::ArrayList<OrganSpecies*> *spList, OrganGroup *destGroup)
{
	DB::SQLBuilder sql(this->db);
	DB::SQLBuilder sql2(this->db);
	UOSInt i;
	UOSInt j;
	Bool found;
	OrganSpecies *sp;
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
		sp = spList->GetItem(i);
		sql.AppendInt32(sp->GetSpeciesId());
		sql2.AppendInt32(sp->GetSpeciesId());
		i++;
	}
	sql.AppendCmdC(CSTR(")"));
	sql2.AppendCmdC(CSTR(")"));
	db->ExecuteNonQuery(sql2.ToCString());
	return db->ExecuteNonQuery(sql.ToCString()) > 0;
}

Bool SSWR::OrganMgr::OrganEnvDB::MoveImages(Data::ArrayList<OrganImages*> *imgList, OrganSpecies *destSp, UI::GUIForm *frm)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptrEnd;
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
		if (img->GetImgItem()->GetFileType() == OrganImageItem::FileType::UserFile)
		{
			UserFileInfo *userFile = img->GetImgItem()->GetUserFile();
			if (userFile->speciesId != destSp->GetSpeciesId())
			{
				SpeciesInfo *sp = this->speciesMap.Get(userFile->speciesId);
				UOSInt oldInd;
				if (sp)
				{
					oldInd = sp->files.GetCount();
					while (oldInd-- > 0)
					{
						if (sp->files.GetItem(oldInd)->id == userFile->id)
						{
							sp->files.RemoveAt(oldInd);
							break;
						}
					}
				}
				userFile->speciesId = destSp->GetSpeciesId();
				sp = this->GetSpeciesInfo(destSp->GetSpeciesId(), true);
				sp->files.Add(userFile);
				DB::SQLBuilder sql(this->db);
				sql.AppendCmdC(CSTR("update userfile set species_id = "));
				sql.AppendInt32(userFile->speciesId);
				sql.AppendCmdC(CSTR(" where id = "));
				sql.AppendInt32(userFile->id);
				this->db->ExecuteNonQuery(sql.ToCString());
			}
		}
		else if (img->GetImgItem()->GetFileType() == OrganImageItem::FileType::WebFile)
		{
			WebFileInfo *wfile = img->GetImgItem()->GetWebFile();
			if (wfile->speciesId != destSp->GetSpeciesId())
			{
				SpeciesInfo *sp = this->speciesMap.Get(wfile->speciesId);
				if (sp)
				{
					sp->wfileMap.Remove(wfile->id);
				}
				wfile->speciesId = destSp->GetSpeciesId();
				sp = this->GetSpeciesInfo(destSp->GetSpeciesId(), true);
				sp->wfileMap.Put(wfile->id, wfile);
				DB::SQLBuilder sql(this->db);
				sql.AppendCmdC(CSTR("update webfile set species_id = "));
				sql.AppendInt32(wfile->speciesId);
				sql.AppendCmdC(CSTR(" where id = "));
				sql.AppendInt32(wfile->id);
				this->db->ExecuteNonQuery(sql.ToCString());
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
				NotNullPtr<Text::String> s;
				s = Text::String::NewNotNull(L"移動");
				sb.Append(s);
				s->Release();
				sb.Append(img->GetImgItem()->GetDispName());
				s = Text::String::NewNotNull(L"時出錯, 要繼續?");
				sb.Append(s);
				s->Release();
				s = Text::String::NewNotNull(L"錯誤");
				if (!UI::MessageDialog::ShowYesNoDialog(sb.ToCString(), s->ToCString(), frm))
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
				img = imgList->GetItem(i);
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
					writer.WriteLineC(sb.ToString(), sb.GetLength());
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
			Data::ArrayListNN<Text::String> webLines;
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
							img = imgList->GetItem(i);
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
				i = 0;
				j = webLines.GetCount();
				while (i < j)
				{
					writer.WriteLine(webLines.GetItem(i)->ToCString());
					webLines.GetItem(i)->Release();
					i++;
				}
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
	sql.AppendCmdC(CSTR("update species_book set species_id = "));
	sql.AppendInt32(destSp->GetSpeciesId());
	sql.AppendCmdC(CSTR(" where species_id = "));
	sql.AppendInt32(srcSp->GetSpeciesId());
	if (this->db->ExecuteNonQuery(sql.ToCString()) < 0)
	{
		return false;
	}
	sql.Clear();
	sql.AppendCmdC(CSTR("update userfile set species_id = "));
	sql.AppendInt32(destSp->GetSpeciesId());
	sql.AppendCmdC(CSTR(" where species_id = "));
	sql.AppendInt32(srcSp->GetSpeciesId());
	if (this->db->ExecuteNonQuery(sql.ToCString()) < 0)
	{
		return false;
	}
	sql.Clear();
	sql.AppendCmdC(CSTR("delete from species where id = "));
	sql.AppendInt32(srcSp->GetSpeciesId());
	if (this->db->ExecuteNonQuery(sql.ToCString()) < 0)
	{
		return false;
	}
	SpeciesInfo *spInfos = this->speciesMap.Get(srcSp->GetSpeciesId());
	SpeciesInfo *spInfod = this->speciesMap.Get(destSp->GetSpeciesId());
	if (spInfos && spInfod)
	{
		spInfod->files.AddAll(spInfos->files);
		spInfos->files.Clear();
	}
	return true;
}

UOSInt SSWR::OrganMgr::OrganEnvDB::GetWebUsers(Data::ArrayList<OrganWebUser*> *userList)
{
	UOSInt initCnt = userList->GetCount();
	OrganWebUser *user;
	DB::DBReader *r = this->db->ExecuteReader(CSTR("select id, userName, watermark, userType from webuser"));
	if (r)
	{
		while (r->ReadNext())
		{
			user = MemAlloc(OrganWebUser, 1);
			user->id = r->GetInt32(0);
			user->userName = r->GetNewStrNN(1);
			user->watermark = r->GetNewStrNN(2);
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
	sql.AppendCmdC(CSTR("select id from webuser where userName = "));
	sql.AppendStrUTF8(userName);
	DB::DBReader *r = this->db->ExecuteReader(sql.ToCString());
	if (r == 0)
		return false;
	if (r->ReadNext())
	{
		this->db->CloseReader(r);
		return false;
	}
	this->db->CloseReader(r);
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
	return this->db->ExecuteNonQuery(sql.ToCString()) >= 1;
}

Bool SSWR::OrganMgr::OrganEnvDB::ModifyWebUser(Int32 id, const UTF8Char *userName, const UTF8Char *pwd, const UTF8Char *watermark)
{
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("select id from webuser where userName = "));
	sql.AppendStrUTF8(userName);
	DB::DBReader *r = this->db->ExecuteReader(sql.ToCString());
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
	return this->db->ExecuteNonQuery(sql.ToCString()) >= 0;
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
	sql.AppendCmdC(CSTR("select dispName from species_book where species_id = "));
	sql.AppendInt32(speciesId);
	sql.AppendCmdC(CSTR(" and book_id = "));
	sql.AppendInt32(bookId);
	r = this->db->ExecuteReader(sql.ToCString());
	if (r == 0)
	{
		r = this->db->ExecuteReader(sql.ToCString());
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
	sql.AppendCmdC(CSTR("insert into species_book (species_id, book_id, dispName) values ("));
	sql.AppendInt32(speciesId);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(bookId);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrUTF8(dispName);
	sql.AppendCmdC(CSTR(")"));
	if (this->db->ExecuteNonQuery(sql.ToCString()) == -2)
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

	sql.AppendCmdC(CSTR("select book_id, dispName, id from species_book where species_id = "));
	sql.AppendInt32(speciesId);
	r = this->db->ExecuteReader(sql.ToCString());
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
			spBook->dispName = r->GetNewStrNN(1);
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
		spBook->dispName->Release();
		MemFree(spBook);
	}
}

Int32 SSWR::OrganMgr::OrganEnvDB::NewBook(Text::CString title, Text::CString author, Text::CString press, const Data::Timestamp &publishDate, Text::CString url)
{
	OrganBook *book;
	DB::SQLBuilder sql(this->db);
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
	if (this->db->ExecuteNonQuery(sql.ToCString()) < -1)
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

Bool SSWR::OrganMgr::OrganEnvDB::AddDataFile(Text::CStringNN fileName)
{
	IO::ParserType t;
	IO::ParsedObject *pobj;
	{
		IO::StmData::FileData fd(fileName, false);
		pobj = this->parsers.ParseFile(fd, &t);
	}
	Data::Timestamp startTime;
	Data::Timestamp endTime;
	const UTF8Char *oriFileName;
	const UTF8Char *oriFileNameEnd;
	UTF8Char sbuff[512];
	const UTF8Char *dataFileName;
	Int32 fileType = 0;
	DataFileInfo *dataFile;
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
					recArr = gpsTrk->GetTrack(i, &l);
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
			Data::ArrayListNN<Text::String> tables;
			db->QueryTableNames(CSTR_NULL, &tables);
			if (tables.GetCount() == 2)
			{
				if (tables.GetItem(0)->Equals(UTF8STRC("Setting")) && tables.GetItem(1)->Equals(UTF8STRC("Records")))
				{
					DB::DBReader *reader = db->QueryTableData(CSTR_NULL, CSTR("Records"), 0, 0, 0, CSTR_NULL, 0);
					Bool found = false;
					if (reader)
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
			LIST_FREE_STRING(&tables);
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
			DB::SQLBuilder sql(this->db);
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
			if (this->db->ExecuteNonQuery(sql.ToCString()) >= 1)
			{
				chg = true;
				dataFile = MemAlloc(DataFileInfo, 1);
				dataFile->id = this->db->GetLastIdentity32();
				dataFile->fileType = fileType;
				dataFile->startTime = startTime;
				dataFile->endTime = endTime;
				dataFile->webUserId = this->userId;
				dataFile->oriFileName = Text::String::NewP(oriFileName, oriFileNameEnd);
				dataFile->fileName = Text::String::NewP(dataFileName, sptr);
				this->dataFiles.Add(dataFile);

				if (fileType == 1)
				{
					WebUserInfo *webUser = this->GetWebUser(dataFile->webUserId);
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
							SSWR::OrganMgr::UserFileInfo *userFile = webUser->userFileObj.GetItem((UOSInt)startIndex);
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

Bool SSWR::OrganMgr::OrganEnvDB::DelDataFile(DataFileInfo *dataFile)
{
	Bool found = false;
	UTF8Char sbuff[512];
	UOSInt i;
	i = this->dataFiles.GetCount();
	while (i-- > 0)
	{
		if (dataFile == this->dataFiles.GetItem(i))
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
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("delete from datafile where id = "));
	sql.AppendInt32(dataFile->id);
	this->db->ExecuteNonQuery(sql.ToCString());
	this->dataFiles.RemoveAt(i);
	if (dataFile->fileType == 1)
	{
		OSInt j;
		WebUserInfo *webUser = this->userMap.Get(dataFile->webUserId);
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

Bool SSWR::OrganMgr::OrganEnvDB::GetGPSPos(Int32 userId, const Data::Timestamp &ts, Math::Coord2DDbl *pos)
{
	OSInt i;
	WebUserInfo *webUser;
	DataFileInfo *dataFile;
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
		dataFile = webUser->gpsFileObj.GetItem((UOSInt)i);
		if (dataFile != 0)
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
		*pos = this->gpsTrk->GetPosByTime(ts);
		return true;
	}
	else
	{
		*pos = Math::Coord2DDbl(0, 0);
		return false;
	}
}

Map::GPSTrack *SSWR::OrganMgr::OrganEnvDB::OpenGPSTrack(DataFileInfo *dataFile)
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

void SSWR::OrganMgr::OrganEnvDB::UpdateUserFileCrop(UserFileInfo *userFile, Double cropLeft, Double cropTop, Double cropRight, Double cropBottom)
{
	DB::SQLBuilder sql(this->db);
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
	if (this->db->ExecuteNonQuery(sql.ToCString()) >= 0)
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
		sql.AppendCmdC(CSTR("update userfile set rotType = "));
		sql.AppendInt32(rotType);
		sql.AppendCmdC(CSTR(", prevUpdated = 1"));
		sql.AppendCmdC(CSTR(" where id = "));
		sql.AppendInt32(userFile->id);
		this->db->ExecuteNonQuery(sql.ToCString());
		userFile->rotType = rotType;
	}
}

Bool SSWR::OrganMgr::OrganEnvDB::UpdateUserFilePos(UserFileInfo *userFile, const Data::Timestamp &captureTime, Double lat, Double lon)
{
	Bool succ = false;
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("update userfile set captureTime = "));
	sql.AppendTS(captureTime);
	sql.AppendCmdC(CSTR(", lat = "));
	sql.AppendDbl(lat);
	sql.AppendCmdC(CSTR(", lon = "));
	sql.AppendDbl(lon);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(userFile->id);
	if (this->db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		succ = true;
		userFile->captureTime = captureTime;
		userFile->lat = lat;
		userFile->lon = lon;
	}
	return succ;
}

Bool SSWR::OrganMgr::OrganEnvDB::GetUserFilePath(UserFileInfo *userFile, NotNullPtr<Text::StringBuilderUTF8> sb)
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

Bool SSWR::OrganMgr::OrganEnvDB::UpdateUserFileDesc(UserFileInfo *userFile, const UTF8Char *descript)
{
	Bool succ = false;
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("update userfile set descript = "));
	sql.AppendStrUTF8(descript);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(userFile->id);
	if (this->db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		succ = true;
		SDEL_STRING(userFile->descript);
		if (descript)
		{
			userFile->descript = Text::String::NewNotNullSlow(descript).Ptr();
		}
	}
	return succ;
}

Bool SSWR::OrganMgr::OrganEnvDB::UpdateUserFileLoc(UserFileInfo *userFile, const UTF8Char *location)
{
	Bool succ = false;
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("update userfile set location = "));
	sql.AppendStrUTF8(location);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(userFile->id);
	if (this->db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		succ = true;
		SDEL_STRING(userFile->location);
		if (location)
		{
			userFile->location = Text::String::NewNotNullSlow(location).Ptr();
		}
	}
	return succ;
}

void SSWR::OrganMgr::OrganEnvDB::UpdateWebFileCrop(WebFileInfo *wfile, Double cropLeft, Double cropTop, Double cropRight, Double cropBottom)
{
	DB::SQLBuilder sql(this->db);
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
	if (this->db->ExecuteNonQuery(sql.ToCString()) >= 0)
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
	UTF8Char *sbuffEnd;
	UTF8Char sbuff2[256];
	UTF8Char *sbuff2End;
	this->TripRelease();

	LocationType *locT;
	Location *loc;
	Trip *t;
	
	sql.Clear();
	sql.AppendCmdC(CSTR("select locType, engName, chiName from location_type where cate_id = "));
	sql.AppendInt32(cateId);
	sql.AppendCmdC(CSTR(" order by locType"));
	r = this->db->ExecuteReader(sql.ToCString());
	if (r)
	{
		while (r->ReadNext())
		{
			sbuffEnd = r->GetStr(1, sbuff, sizeof(sbuff));
			sbuff2End = r->GetStr(2, sbuff2, sizeof(sbuff2));
			NEW_CLASS(locT, LocationType(r->GetInt32(0), CSTRP(sbuff, sbuffEnd), CSTRP(sbuff2, sbuff2End)))
			this->locType.Add(locT);
		}
		this->db->CloseReader(r);
	}

	sql.Clear();
	sql.AppendCmdC(CSTR("select id, parentId, ename, cname, locType from location where cate_id = "));
	sql.AppendInt32(cateId);
	sql.AppendCmdC(CSTR(" order by id"));
	r = this->db->ExecuteReader(sql.ToCString());
	if (r)
	{
		while (r->ReadNext())
		{
			sbuffEnd = r->GetStr(2, sbuff, sizeof(sbuff));
			sbuff2End = r->GetStr(3, sbuff2, sizeof(sbuff2));
			NEW_CLASS(loc, Location(r->GetInt32(0), r->GetInt32(1), CSTRP(sbuff, sbuffEnd), CSTRP(sbuff2, sbuff2End), r->GetInt32(4)));
			this->locs.Add(loc);
		}
		this->db->CloseReader(r);
	}

	sql.Clear();
	sql.AppendCmdC(CSTR("select fromDate, toDate, locId from trip where cate_id = "));
	sql.AppendInt32(cateId);
	sql.AppendCmdC(CSTR(" order by fromDate"));
	r = db->ExecuteReader(sql.ToCString());
	if (r)
	{
		while (r->ReadNext())
		{
			NEW_CLASS(t, Trip(r->GetTimestamp(0), r->GetTimestamp(1), r->GetInt32(2)));
			this->trips.Add(t);
		}
		this->db->CloseReader(r);
	}
}

Bool SSWR::OrganMgr::OrganEnvDB::TripAdd(const Data::Timestamp &fromDate, const Data::Timestamp &toDate, Int32 locId)
{
	OSInt i = this->TripGetIndex(fromDate);
	OSInt j = this->TripGetIndex(toDate);
	OSInt k = this->LocationGetIndex(locId);
	if (i < 0 && j < 0 && k >= 0)
	{
		if (i == j)
		{
			DB::SQLBuilder sql(this->db);
			sql.AppendCmdC(CSTR("insert into trip (fromDate, toDate, locId, cate_id) values ("));
			sql.AppendTS(fromDate);
			sql.AppendCmdC(CSTR(", "));
			sql.AppendTS(toDate);
			sql.AppendCmdC(CSTR(", "));
			sql.AppendInt32(locId);
			sql.AppendCmdC(CSTR(", "));
			sql.AppendInt32(this->currCate->cateId);
			sql.AppendCmdC(CSTR(")"));
			if (this->db->ExecuteNonQuery(sql.ToCString()) != -2)
			{
				Trip *t;
				NEW_CLASS(t, Trip(fromDate, toDate, locId));
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
	Location *loc = this->LocationGet(locId);
	if (loc == 0)
		return false;

	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("update location set ename="));
	sql.AppendStrUTF8(engName.v);
	sql.AppendCmdC(CSTR(", cname="));
	sql.AppendStrUTF8(chiName.v);
	sql.AppendCmdC(CSTR(" where id="));
	sql.AppendInt32(locId);
	if (this->db->ExecuteNonQuery(sql.ToCString()) == -2)
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
	
	if ((UOSInt)lType + 1 >= this->locType.GetCount())
		return false;
	lType = this->locType.GetItem((UInt32)lType + 1)->id;
	DB::SQLBuilder sql(this->db);
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
	if (this->db->ExecuteNonQuery(sql.ToCString()) == -2)
		return false;
	else
	{
		DB::DBReader *r;
		sql.Clear();
		sql.AppendCmdC(CSTR("select max(id) from location where parentId = "));
		sql.AppendInt32(locId);
		r = this->db->ExecuteReader(sql.ToCString());
		if (r)
		{
			if (r->ReadNext())
			{
				Location *l;
				NEW_CLASS(l, Location(r->GetInt32(0), locId, engName, chiName, lType));
				this->locs.Add(l);
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
	r = this->db->ExecuteReader(CSTR("select id, title, dispAuthor, press, publishDate, groupId, url from book"));
	while (r->ReadNext())
	{
		NEW_CLASS(book, OrganBook());
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

Media::ImageList *SSWR::OrganMgr::OrganEnvDB::ParseImage(OrganImageItem *img, UserFileInfo **outUserFile, WebFileInfo **outWebFile)
{
	IO::ParserType pt;
	if (img->GetFileType() == OrganImageItem::FileType::UserFile)
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
			if (userFile->fileType == 3)
			{
				sptr = Text::StrInt64(sptr, userFile->fileTime.ToTicks());
				sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
				sptr = Text::StrHexVal32(sptr, userFile->crcVal);
				sptr = Text::StrConcatC(sptr, UTF8STRC(".png"));
			}
			else
			{
				sptr = userFile->dataFileName->ConcatTo(sptr);
			}
			IO::ParsedObject *pobj;
			{
				IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
				pobj = this->parsers.ParseFile(fd, &pt);
			}
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
		return 0;
	}
	else if (img->GetFileType() == OrganImageItem::FileType::WebFile)
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
			sptr = Text::StrConcatC(sptr, UTF8STRC("WebFile"));
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrInt32(sptr, wfile->id >> 10);
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrInt32(sptr, wfile->id);
			sptr = Text::StrConcatC(sptr, UTF8STRC(".jpg"));

			IO::ParsedObject *pobj;
			{
				IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
				pobj = this->parsers.ParseFile(fd, &pt);
			}
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
		if (outUserFile)
		{
			*outUserFile = 0;
		}
		if (outWebFile)
		{
			*outWebFile = 0;
		}
		IO::ParsedObject *pobj;
		{
			IO::StmData::FileData fd(Text::String::OrEmpty(img->GetFullName()), false);
			pobj = this->parsers.ParseFile(fd, &pt);
		}
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
	const UTF8Char *coverName = STR_PTR(sp->GetPhoto());
	IO::Path::FindFileSession *sess;
	IO::Path::PathType pt;
	UOSInt i;
	IO::ParsedObject *pobj = 0;
	if (sp->GetPhotoId() != 0)
	{
		UserFileInfo *userFile = this->userFileMap.Get(sp->GetPhotoId());
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
		SpeciesInfo *spInfo = this->speciesMap.Get(sp->GetSpeciesId());
		if (spInfo)
		{
			WebFileInfo *wfile = spInfo->wfileMap.Get(sp->GetPhotoWId());
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
	sptr = sp->GetDirName()->ConcatTo(sptr);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr2 = Text::StrConcatC(Text::StrConcat(sptr, coverName), UTF8STRC(".*"));
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
					pobj = this->parsers.ParseFile(fd, 0);
				}
				else if (Text::StrCompareICase(&sptr[i], (const UTF8Char*)".TIF") == 0)
				{
					IO::StmData::FileData fd(CSTRP(sbuff, sptr2), false);
					pobj = this->parsers.ParseFile(fd, 0);
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
						if (Text::StrStartsWith(sptr, coverName))
						{
							IO::StmData::FileData fd(CSTRP(sbuff, sptr2), false);
							pobj = this->parsers.ParseFile(fd, 0);
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

Media::ImageList *SSWR::OrganMgr::OrganEnvDB::ParseFileImage(UserFileInfo *userFile)
{
	IO::ParserType pt;
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
		pobj = this->parsers.ParseFile(fd, &pt);
	}
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

Media::ImageList *SSWR::OrganMgr::OrganEnvDB::ParseWebImage(WebFileInfo *wfile)
{
	IO::ParserType pt;
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
		pobj = this->parsers.ParseFile(fd, &pt);
	}
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
	DB::DBReader *r;
	DB::SQLBuilder sql(this->db);
	Text::StringBuilderUTF8 sb;

	sql.AppendCmdC(CSTR("select group_id, sci_name from species where chi_name = "));
	sql.AppendStrUTF8(searchStr);
	sql.AppendCmdC(CSTR(" or sci_name = "));
	sql.AppendStrUTF8(searchStr);
	r =this->db->ExecuteReader(sql.ToCString());
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
		sql.AppendCmdC(CSTR("select group_id, sci_name from species s, species_book sb where sb.dispName = "));
		sql.AppendStrUTF8(searchStr);
		sql.AppendCmdC(CSTR(" and s.id = sb.species_id group by sb.species_id"));
		r = this->db->ExecuteReader(sql.ToCString());
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
	r = this->db->ExecuteReader(sql.ToCString());
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
		this->db->CloseReader(r);
	}
	return foundGroup;
}

void SSWR::OrganMgr::OrganEnvDB::LoadGroupTypes()
{
	OrganGroupType *grpType;
	DB::DBReader *r;
	UOSInt i;
	Int32 seq;
	UTF8Char cname[64];
	UTF8Char *cnameEnd;
	UTF8Char ename[64];
	UTF8Char *enameEnd;
	i = this->grpTypes.GetCount();
	while (i-- > 0)
	{
		grpType = this->grpTypes.GetItem(i);
		DEL_CLASS(grpType);
	}
	this->grpTypes.Clear();

	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("Select seq, chi_name, eng_name from group_type where cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	sql.AppendCmdC(CSTR(" order by seq"));
	r = this->db->ExecuteReader(sql.ToCString());
	while (r->ReadNext())
	{
		seq = r->GetInt32(0);
		cnameEnd = r->GetStr(1, cname, sizeof(cname));
		enameEnd = r->GetStr(2, ename, sizeof(ename));
		NEW_CLASS(grpType, OrganGroupType(seq, {cname, (UOSInt)(cnameEnd - cname)}, {ename, (UOSInt)(enameEnd - ename)}));
		this->grpTypes.Add(grpType);
	}
	this->db->CloseReader(r);
}

Data::FastMap<Int32, Data::ArrayList<SSWR::OrganMgr::OrganGroup*>*> *SSWR::OrganMgr::OrganEnvDB::GetGroupTree()
{
	Data::ArrayList<OrganGroup*> *grps;
	Data::FastMap<Int32, Data::ArrayList<OrganGroup*>*> *grpTree;
	DB::DBReader *r;
	Int32 parId;

	NEW_CLASS(grpTree, Data::Int32FastMap<Data::ArrayList<OrganGroup*>*>());

	DB::SQLBuilder sql(this->db);
	Text::StringBuilderUTF8 sb;
	sql.AppendCmdC(CSTR("select id, group_type, eng_name, chi_name, description, photo_group, photo_species, idKey, parent_id, flags from "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" where cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	sql.AppendCmdC(CSTR(" order by parent_id, eng_name"));
	r = this->db->ExecuteReader(sql.ToCString());
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

Data::FastMap<Int32, Data::ArrayList<SSWR::OrganMgr::OrganSpecies*>*> *SSWR::OrganMgr::OrganEnvDB::GetSpeciesTree()
{
	Data::ArrayList<OrganSpecies*> *sps;
	Data::FastMap<Int32, Data::ArrayList<OrganSpecies*>*> *spTree;
	DB::DBReader *r;
	Int32 parId;

	NEW_CLASS(spTree, Data::Int32FastMap<Data::ArrayList<OrganSpecies*>*>());

	DB::SQLBuilder sql(this->db);
	Text::StringBuilderUTF8 sb;
	sql.Clear();
	sql.AppendCmdC(CSTR("SELECT id, chi_name, sci_name, eng_name, description, dirName, photo, idKey, flags, group_id, photoId, mapColor FROM species where cate_id = "));
	sql.AppendInt32(this->currCate->cateId);
	sql.AppendCmdC(CSTR(" order by group_id, sci_name"));
	r = this->db->ExecuteReader(sql.ToCString());
	if (r)
	{
		while (r->ReadNext())
		{
			OrganSpecies *sp;
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
	Data::FastMap<Int32, OrganSpecies*> *speciesMap;
	Data::ArrayListInt32 speciesList;
	UserFileInfo *userFile;
	OrganSpecies *species;
	Data::DateTime dt;
	Trip *tr;
	NEW_CLASS(speciesMap, Data::Int32FastMap<OrganSpecies*>());
	UOSInt i = 0;
	UOSInt j = this->userFileMap.GetCount();
	while (i < j)
	{
		userFile = this->userFileMap.GetItem(i);
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
				tr = this->TripGet(userId, userFile->captureTime);
				if (tr)
				{
					this->UpdateUserFileLoc(userFile, this->LocationGet(tr->locId)->cname->v);
				}
			}
		}
		i++;
	}
	i = speciesMap->GetCount();
	while (i-- > 0)
	{
		species = speciesMap->GetItem(i);
		DEL_CLASS(species);
	}
	DEL_CLASS(speciesMap);

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
	DB::DBReader *r;
	UInt8 crcBuff[4];
	Crypto::Hash::CRC32R crc;
	OSInt coverFound;
	WebFileInfo *wfile;
	SpeciesInfo *spInfo;
	Bool isCover;

	DB::SQLBuilder sql(this->db);
	r = this->db->ExecuteReader(CSTR("select id, dirName, photo, photoId, photoWId from species"));
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
							if (this->db->ExecuteNonQuery(sql.ToCString()) > 0)
							{
								id = this->db->GetLastIdentity32();
								
								if (sp->photoId == 0 && sp->photoWId == 0 && sp->photoName && sp->photoName->StartsWith(UTF8STRC("web\\")) && cols[0].StartsWith(&sp->photoName->v[4], sp->photoName->leng - 4))
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
									MemFree(wfile);
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
										this->db->ExecuteNonQuery(sql.ToCString());
									}
									spInfo = this->GetSpeciesInfo(sp->id, true);
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
			if (sp->photoId == 0 && sp->photoWId == 0 && sp->photoName && sp->photoName->StartsWith(UTF8STRC("web\\")))
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
		sp->photoName->Release();
		MemFree(sp);
		i++;
	}
}

void SSWR::OrganMgr::OrganEnvDB::UpgradeFileStruct(OrganSpecies *sp)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	const UTF8Char *coverName = STR_PTR(sp->GetPhoto());
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
	sptr = sp->GetDirName()->ConcatTo(sptr);
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
					this->AddSpeciesFile(sp, CSTRP(sbuff, sptr2), isCoverPhoto, true, &fileId);
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
			writer.WriteLineC(UTF8STRC("ScreenSize=1200"));
			writer.WriteLineC(UTF8STRC("MDBFile=OrganWeb.mdb"));
			writer.WriteLineC(UTF8STRC("ImageDir=Image\\"));
			writer.WriteLineC(UTF8STRC("SvrPort=8080"));
			writer.WriteLineC(UTF8STRC("Watermark=sswroom"));
			writer.WriteLineC(UTF8STRC("CacheDir=Cache\\"));
			writer.WriteLineC(UTF8STRC("DataDir=Data"));
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
	DB::DBReader *r = this->db->ExecuteReader(CSTR("select srcDir from category"));
	if (r)
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

	r = this->db->ExecuteReader(CSTR("select fileType, fileTime, webuser_id, dataFileName from userfile"));
	if (r)
	{
		Data::DateTime dt;
		void *param;
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
			r->GetAsDate(1, &dt);
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
					{
						IO::StmData::FileData fd({sbuff2, (UOSInt)(sptr3End - sbuff2)}, false);
						imgList = (Media::ImageList*)this->parsers.ParseFileType(fd, IO::ParserType::ImageList);
					}

					if (imgList)
					{
						UInt32 delay;
						imgList->ToStaticImage(0);
						Media::StaticImage *newImg;
						NotNullPtr<Media::StaticImage> simg;
						if (simg.Set((Media::StaticImage*)imgList->GetImage(0, &delay)))
						{
							if (simg->info.dispSize.x > 1920 || simg->info.dispSize.y > 1920)
							{
								resizer.SetTargetSize(Math::Size2D<UOSInt>(1920, 1920));
								resizer.SetResizeAspectRatio(Media::IImgResizer::RAR_KEEPAR);
								newImg = resizer.ProcessToNew(simg);
								if (newImg)
								{
									imgList->ReplaceImage(0, newImg);
								}
							}

						}
						param = exporter.CreateParam(imgList);
						exporter.SetParamInt32(param, 0, 95);
						{
							IO::FileStream fs(CSTRP(sbuff, sptr2End), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
							exporter.ExportFile(fs, CSTRP(sbuff, sptr2End), imgList, param);
						}
						DEL_CLASS(imgList);
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
