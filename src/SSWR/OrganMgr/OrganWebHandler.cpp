#include "Stdafx.h"
#include "Crypto/Hash/CRC32R.h"
#include "Crypto/Hash/MD5.h"
#include "Data/ArrayListICaseStrUTF8.h"
#include "Data/ArrayListString.h"
#include "Data/ByteTool.h"
#include "Data/Int64Map.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "DB/DBReader.h"
#include "Exporter/GUIJPGExporter.h"
#include "IO/FileStream.h"
#include "IO/IniFile.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/StreamReader.h"
#include "IO/StmData/FileData.h"
#include "IO/StmData/MemoryData.h"
#include "Map/IMapDrawLayer.h"
#include "Math/Math.h"
#include "Media/FrequencyGraph.h"
#include "Media/IAudioSource.h"
#include "Media/ICCProfile.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "Media/LRGBLimiter.h"
#include "Media/MediaFile.h"
#include "Media/PhotoInfo.h"
#include "Net/WebServer/WebSessionUsage.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Parser/FullParserList.h"
#include "SSWR/OrganMgr/OrganWebHandler.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "Text/XML.h"
#include "Text/TextEnc/URIEncoding.h"

#define SP_PER_PAGE_DESKTOP 100
#define SP_PER_PAGE_MOBILE 90
#define PREVIEW_SIZE 320

SSWR::OrganMgr::OrganWebHandler::SpeciesSciNameComparator::~SpeciesSciNameComparator()
{
}

OSInt SSWR::OrganMgr::OrganWebHandler::SpeciesSciNameComparator::Compare(SpeciesInfo *a, SpeciesInfo *b)
{
	return a->sciName->CompareTo(b->sciName);
}

SSWR::OrganMgr::OrganWebHandler::UserFileTimeComparator::~UserFileTimeComparator()
{

}

OSInt SSWR::OrganMgr::OrganWebHandler::UserFileTimeComparator::Compare(UserFileInfo *a, UserFileInfo *b)
{
	if (a->webuserId > b->webuserId)
	{
		return 1;
	}
	else if (a->webuserId < b->webuserId)
	{
		return -1;
	}
	else if (a->captureTimeTicks > b->captureTimeTicks)
	{
		return 1;
	}
	else if (a->captureTimeTicks < b->captureTimeTicks)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

void SSWR::OrganMgr::OrganWebHandler::LoadLangs()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::Path::FindFileSession *sess;
	UInt32 langId;
	IO::Path::PathType pt;
	UOSInt i;
	IO::ConfigFile *lang;

	IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, (const UTF8Char*)"Langs");
	*sptr++ = IO::Path::PATH_SEPERATOR;
	Text::StrConcat(sptr, IO::Path::ALL_FILES);
	sess = IO::Path::FindFile(sbuff);
	if (sess)
	{
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, 0))
		{
			if (pt == IO::Path::PathType::File)
			{
//				printf("Found file %s\r\n", sptr);
				i = Text::StrCharCnt(sptr);
				if (i > 4 && Text::StrEquals(&sptr[i - 4], (const UTF8Char*)".txt"))
				{
					sptr[i - 4] = 0;
					langId = Text::StrToUInt32(sptr);
					sptr[i - 4] = '.';
//					printf("LangId = %d\r\n", langId);
					if (langId)
					{
						lang = IO::IniFile::Parse(sbuff, 65001);
						lang = this->langMap->Put(langId, lang);
						if (lang)
						{
							DEL_CLASS(lang);
						}
					}
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}
}

void SSWR::OrganMgr::OrganWebHandler::LoadCategory()
{
	SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
	Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupTypeInfo*> *grpTypeList;
	SSWR::OrganMgr::OrganWebHandler::GroupTypeInfo *grpType;
	Int32 cateId;
	UOSInt i;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	DB::DBReader *r = this->db->ExecuteReader((const UTF8Char*)"select cate_id, chi_name, dirName, srcDir, flags from category");
	if (r != 0)
	{
		Text::StringBuilderUTF8 sb;
		while (r->ReadNext())
		{
			cateId = r->GetInt32(0);
			cate = this->cateMap->Get(cateId);
			if (cate == 0)
			{
				cate = MemAlloc(SSWR::OrganMgr::OrganWebHandler::CategoryInfo, 1);
				cate->cateId = cateId;
				sb.ClearStr();
				r->GetStr(1, &sb);
				cate->chiName = Text::StrCopyNew(sb.ToString());
				sb.ClearStr();
				r->GetStr(2, &sb);
				cate->dirName = Text::StrCopyNew(sb.ToString());
				sb.ClearStr();
				r->GetStr(3, &sb);
				Text::StrConcat(sbuff, this->imageDir);
				sptr = IO::Path::AppendPath(sbuff, sb.ToString());
				if (sptr[-1] != IO::Path::PATH_SEPERATOR)
				{
					sptr[0] = IO::Path::PATH_SEPERATOR;
					sptr[1] = 0;
				}
				cate->srcDir = Text::StrCopyNew(sbuff);
				cate->flags = r->GetInt32(4);
				NEW_CLASS(cate->groups, Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupInfo*>());
				NEW_CLASS(cate->groupTypes, Data::Int32Map<SSWR::OrganMgr::OrganWebHandler::GroupTypeInfo*>());
				this->cateMap->Put(cate->cateId, cate);
				this->cateSMap->Put(cate->dirName, cate);
			}
			else
			{
				grpTypeList = cate->groupTypes->GetValues();
				i = grpTypeList->GetCount();
				while (i-- > 0)
				{
					grpType = grpTypeList->GetItem(i);
					Text::StrDelNew(grpType->chiName);
					Text::StrDelNew(grpType->engName);
					MemFree(grpType);
				}
				cate->groupTypes->Clear();
			}
		}
		this->db->CloseReader(r);
	}
	r = this->db->ExecuteReader((const UTF8Char*)"select seq, eng_name, chi_name, cate_id from group_type");
	if (r != 0)
	{
		Text::StringBuilderUTF8 sb;
		while (r->ReadNext())
		{
			cateId = r->GetInt32(3);
			cate = this->cateMap->Get(cateId);
			if (cate == 0)
			{
			}
			else
			{
				grpType = MemAlloc(SSWR::OrganMgr::OrganWebHandler::GroupTypeInfo, 1);
				grpType->id = r->GetInt32(0);
				sb.ClearStr();
				r->GetStr(1, &sb);
				grpType->engName = Text::StrCopyNew(sb.ToString());
				sb.ClearStr();
				r->GetStr(2, &sb);
				grpType->chiName = Text::StrCopyNew(sb.ToString());
				cate->groupTypes->Put(grpType->id, grpType);
			}
		}
		this->db->CloseReader(r);
	}

}

void SSWR::OrganMgr::OrganWebHandler::LoadSpecies()
{
	FreeSpecies();

	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
	SSWR::OrganMgr::OrganWebHandler::WebFileInfo *wfile;
	DB::DBReader *r = this->db->ExecuteReader((const UTF8Char*)"select id, eng_name, chi_name, sci_name, group_id, description, dirName, photo, idKey, cate_id, flags, photoId, photoWId from species");
	if (r != 0)
	{
		while (r->ReadNext())
		{
			sp = MemAlloc(SSWR::OrganMgr::OrganWebHandler::SpeciesInfo, 1);
			sp->speciesId = r->GetInt32(0);
			sp->engName = Text::String::OrEmpty(r->GetNewStr(1));
			sp->chiName = Text::String::OrEmpty(r->GetNewStr(2));
			sp->sciName = r->GetNewStr(3);
			sp->groupId = r->GetInt32(4);
			sp->descript = Text::String::OrEmpty(r->GetNewStr(5));
			sp->dirName = Text::String::OrEmpty(r->GetNewStr(6));
			sp->photo = r->GetNewStr(7);
			sp->idKey = Text::String::OrEmpty(r->GetNewStr(8));
			sp->cateId = r->GetInt32(9);
			sp->flags = (SpeciesFlags)r->GetInt32(10);
			sp->photoId = r->GetInt32(11);
			sp->photoWId = r->GetInt32(12);

			NEW_CLASS(sp->books, Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::BookSpInfo*>());
			NEW_CLASS(sp->files, Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::UserFileInfo*>());
			NEW_CLASS(sp->wfiles, Data::Int32Map<SSWR::OrganMgr::OrganWebHandler::WebFileInfo*>());
			this->spMap->Put(sp->speciesId, sp);
		}
		this->db->CloseReader(r);

		SpeciesSciNameComparator comparator;
		Data::ArrayList<SpeciesInfo*> speciesList(this->spMap->GetCount());
		speciesList.AddAll(this->spMap->GetValues());
		Data::Sort::ArtificialQuickSort::Sort(&speciesList, &comparator);
		UOSInt i = 0;
		UOSInt j = speciesList.GetCount();
		while (i < j)
		{
			sp = speciesList.GetItem(i);
			this->spNameMap->Put(sp->sciName->v, sp);
			i++;
		}
	}

	r = this->db->ExecuteReader((const UTF8Char*)"select id, species_id, crcVal, imgUrl, srcUrl, prevUpdated, cropLeft, cropTop, cropRight, cropBottom, location from webfile");
	if (r != 0)
	{
		while (r->ReadNext())
		{
			sp = this->spMap->Get(r->GetInt32(1));
			if (sp)
			{
				wfile = MemAlloc(SSWR::OrganMgr::OrganWebHandler::WebFileInfo, 1);
				wfile->id = r->GetInt32(0);
				wfile->crcVal = r->GetInt32(2);
				wfile->imgUrl = r->GetNewStr(3);
				wfile->srcUrl = r->GetNewStr(4);
				wfile->prevUpdated = r->GetBool(5);
				wfile->cropLeft = r->GetDbl(6);
				wfile->cropTop = r->GetDbl(7);
				wfile->cropRight = r->GetDbl(8);
				wfile->cropBottom = r->GetDbl(9);
				wfile->location = r->GetNewStr(10);
				sp->wfiles->Put(wfile->id, wfile);
			}
		}
		this->db->CloseReader(r);
	}
}

void SSWR::OrganMgr::OrganWebHandler::LoadGroups()
{
	FreeGroups();

	Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> *spList;
	Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupInfo*> *groupList;
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *pGroup;
	SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
	UOSInt i;
	DB::DBReader *r = this->db->ExecuteReader((const UTF8Char*)"select id, group_type, eng_name, chi_name, description, parent_id, photo_group, photo_species, idKey, cate_id, flags from groups");
	if (r != 0)
	{
		while (r->ReadNext())
		{
			group = MemAlloc(SSWR::OrganMgr::OrganWebHandler::GroupInfo, 1);
			group->id = r->GetInt32(0);
			group->groupType = r->GetInt32(1);
			group->engName = r->GetNewStr(2);
			group->chiName = r->GetNewStr(3);
			group->descript = Text::String::OrEmpty(r->GetNewStr(4));
			group->parentId = r->GetInt32(5);
			group->photoGroup = r->GetInt32(6);
			group->photoSpecies = r->GetInt32(7);
			group->idKey = Text::String::OrEmpty(r->GetNewStr(8));
			group->cateId = r->GetInt32(9);
			group->flags = (GroupFlags)r->GetInt32(10);
			NEW_CLASS(group->species, Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*>());
			NEW_CLASS(group->groups, Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupInfo*>());
			group->photoCount = (UOSInt)-1;
			group->myPhotoCount = (UOSInt)-1;
			group->totalCount = (UOSInt)-1;
			group->photoSpObj = 0;

			this->groupMap->Put(group->id, group);
		}
		this->db->CloseReader(r);

		spList = this->spMap->GetValues();
		i = spList->GetCount();
		while (i-- > 0)
		{
			sp = spList->GetItem(i);
			group = this->groupMap->Get(sp->groupId);
			if (group)
			{
				group->species->Add(sp);
			}
		}

		groupList = this->groupMap->GetValues();
		i = groupList->GetCount();
		while (i-- > 0)
		{
			group = groupList->GetItem(i);
			if (group->parentId)
			{
				pGroup = this->groupMap->Get(group->parentId);
				if (pGroup)
				{
					pGroup->groups->Add(group);
				}
			}
			else
			{
				cate = this->cateMap->Get(group->cateId);
				if (cate)
				{
					cate->groups->Add(group);
				}
			}
		}
	}
}

void SSWR::OrganMgr::OrganWebHandler::LoadBooks()
{
	FreeBooks();

	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
	SSWR::OrganMgr::OrganWebHandler::BookInfo *book;
	SSWR::OrganMgr::OrganWebHandler::BookSpInfo *bookSp;
	Data::DateTime dt;

	DB::DBReader *r = this->db->ExecuteReader((const UTF8Char*)"select id, title, dispAuthor, press, publishDate, url from book");
	if (r != 0)
	{
		while (r->ReadNext())
		{
			book = MemAlloc(SSWR::OrganMgr::OrganWebHandler::BookInfo, 1);
			book->id = r->GetInt32(0);
			book->title = r->GetNewStr(1);
			book->author = r->GetNewStr(2);
			book->press = r->GetNewStr(3);
			r->GetDate(4, &dt);
			book->publishDate = dt.ToTicks();
			book->url = r->GetNewStr(5);
			NEW_CLASS(book->species, Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::BookSpInfo*>());

			this->bookMap->Put(book->id, book);
		}
		this->db->CloseReader(r);
	}

	r = this->db->ExecuteReader((const UTF8Char*)"select species_id, book_id, dispName from species_book");
	if (r != 0)
	{
		while (r->ReadNext())
		{
			sp = this->spMap->Get(r->GetInt32(0));
			book = this->bookMap->Get(r->GetInt32(1));
			if (sp != 0 && book != 0)
			{
				bookSp = MemAlloc(SSWR::OrganMgr::OrganWebHandler::BookSpInfo, 1);
				bookSp->bookId = book->id;
				bookSp->speciesId = sp->speciesId;
				bookSp->dispName = r->GetNewStr(2);
				book->species->Add(bookSp);
				sp->books->Add(bookSp);
			}
		}
		this->db->CloseReader(r);
	}
}

void SSWR::OrganMgr::OrganWebHandler::LoadUsers()
{
	this->ClearUsers();

	Int32 userId;
	SSWR::OrganMgr::OrganWebHandler::WebUserInfo *user;
	DB::DBReader *r = this->db->ExecuteReader((const UTF8Char*)"select id, userName, pwd, watermark, userType from webuser");
	if (r != 0)
	{
		while (r->ReadNext())
		{
			userId = r->GetInt32(0);
			user = this->userMap->Get(userId);
			if (user)
			{
				this->userNameMap->Remove(user->userName->v);
				SDEL_STRING(user->userName);
				user->userName = r->GetNewStr(1);
				SDEL_STRING(user->pwd);
				user->pwd = r->GetNewStr(2);
				SDEL_STRING(user->watermark);
				user->watermark = r->GetNewStr(3);
				user->userType = r->GetInt32(4);
				this->userNameMap->Put(user->userName->v, user);
			}
			else
			{
				user = MemAlloc(SSWR::OrganMgr::OrganWebHandler::WebUserInfo, 1);
				user->id = userId;
				user->userName = r->GetNewStr(1);
				user->pwd = r->GetNewStr(2);
				user->watermark = r->GetNewStr(3);
				user->userType = r->GetInt32(4);
				user->unorganSpId = 0;
				NEW_CLASS(user->userFileIndex, Data::ArrayListInt64());
				NEW_CLASS(user->userFileObj, Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::UserFileInfo*>());
				NEW_CLASS(user->tripCates, Data::Int32Map<Data::Int64Map<SSWR::OrganMgr::OrganWebHandler::TripInfo*>*>());
				this->userMap->Put(user->id, user);
				this->userNameMap->Put(user->userName->v, user);
			}
		}
		this->db->CloseReader(r);
	}

	r = this->db->ExecuteReader((const UTF8Char*)"select id, fileType, oriFileName, fileTime, lat, lon, webuser_id, species_id, captureTime, dataFileName, crcVal, rotType, prevUpdated, cropLeft, cropTop, cropRight, cropBottom, descript, location from userfile");
	if (r != 0)
	{
		SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile;
		SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species;
		Data::DateTime dt;
		UOSInt i;
		UOSInt j;
		UOSInt k;
		user = 0;
		while (r->ReadNext())
		{
			userId = r->GetInt32(6);
			if (user == 0 || user->id != userId)
			{
				user = this->userMap->Get(userId);
			}
			if (user != 0)
			{
				userFile = MemAlloc(SSWR::OrganMgr::OrganWebHandler::UserFileInfo, 1);
				userFile->id = r->GetInt32(0);
				userFile->fileType = r->GetInt32(1);
				userFile->oriFileName = r->GetNewStr(2);
				r->GetDate(3, &dt);
				userFile->fileTimeTicks = dt.ToTicks();
				userFile->lat = r->GetDbl(4);
				userFile->lon = r->GetDbl(5);
				userFile->webuserId = userId;
				userFile->speciesId = r->GetInt32(7);
				r->GetDate(8, &dt);
				userFile->captureTimeTicks = dt.ToTicks();
				userFile->dataFileName = r->GetNewStr(9);
				userFile->crcVal = (UInt32)r->GetInt32(10);
				userFile->rotType = r->GetInt32(11);
				userFile->prevUpdated = r->GetInt32(12);
				userFile->cropLeft = r->GetDbl(13);
				userFile->cropTop = r->GetDbl(14);
				userFile->cropRight = r->GetDbl(15);
				userFile->cropBottom = r->GetDbl(16);
				userFile->descript = r->GetNewStr(17);
				userFile->location = r->GetNewStr(18);
				species = this->spMap->Get(userFile->speciesId);
				if (species != 0)
				{
					species->files->Add(userFile);
				}
				this->userFileMap->Put(userFile->id, userFile);
			}
		}
		this->db->CloseReader(r);

		UserFileTimeComparator comparator;
		Data::ArrayList<UserFileInfo*> userFileList(this->userFileMap->GetCount());
		userFileList.AddAll(this->userFileMap->GetValues());
		Data::Sort::ArtificialQuickSort::Sort(&userFileList, &comparator);
		i = 0;
		j = userFileList.GetCount();
		while (i < j)
		{
			userFile = userFileList.GetItem(i);
			if (user == 0 || user->id != userId)
			{
				user = this->userMap->Get(userId);
			}
			if (user != 0)
			{
				k = user->userFileIndex->SortedInsert(userFile->captureTimeTicks);
				user->userFileObj->Insert(k, userFile);
			}
			i++;
		}
	}

	r = this->db->ExecuteReader((const UTF8Char*)"select fromDate, toDate, locId, cate_id, webuser_id from trip");
	if (r != 0)
	{
		Int32 cateId;
		Int64 fromDate;
		Data::Int64Map<SSWR::OrganMgr::OrganWebHandler::TripInfo*> *tripCate;
		SSWR::OrganMgr::OrganWebHandler::TripInfo *trip;
		Data::DateTime dt;
		user = 0;
		while (r->ReadNext())
		{
			userId = r->GetInt32(4);
			cateId = r->GetInt32(3);
			r->GetDate(0, &dt);
			fromDate = dt.ToTicks();
			if (user == 0 || user->id != userId)
			{
				user = this->userMap->Get(userId);
			}
			if (user != 0)
			{
				tripCate = user->tripCates->Get(cateId);
				if (tripCate == 0)
				{
					NEW_CLASS(tripCate, Data::Int64Map<SSWR::OrganMgr::OrganWebHandler::TripInfo*>());
					user->tripCates->Put(cateId, tripCate);
				}
				trip = tripCate->Get(fromDate);
				if (trip == 0)
				{
					trip = MemAlloc(SSWR::OrganMgr::OrganWebHandler::TripInfo, 1);
					trip->fromDate = fromDate;
					r->GetDate(1, &dt);
					trip->toDate = dt.ToTicks();
					trip->cateId = cateId;
					trip->locId = r->GetInt32(2);
					tripCate->Put(fromDate, trip);
				}
			}
		}
		this->db->CloseReader(r);
	}

	if (this->unorganizedGroupId)
	{
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
		group = this->groupMap->Get(this->unorganizedGroupId);
		if (group != 0)
		{
			Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::WebUserInfo*> *userList = this->userMap->GetValues();
			UOSInt i = userList->GetCount();
			UOSInt j;
			SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species;
			while (i-- > 0)
			{
				user = userList->GetItem(i);
				if (user->unorganSpId == 0)
				{
					Text::StringBuilderUTF8 sbSName;
					sbSName.Append((const UTF8Char *)"Unorganized ");
					sbSName.Append(user->userName);
					j = group->species->GetCount();
					while (j-- > 0)
					{
						species = group->species->GetItem(j);
						if (species->sciName->Equals(sbSName.ToString()))
						{
							user->unorganSpId = species->speciesId;
							break;
						}
					}
					if (user->unorganSpId == 0)
					{
						Text::StringBuilderUTF8 sb;
						sb.Append(sbSName.ToString());
						sb.ToLower();
						sb.Replace((const UTF8Char*)" ", (const UTF8Char*)"_");
						sb.Replace((const UTF8Char*)".", (const UTF8Char*)"");
						user->unorganSpId = this->SpeciesAdd((const UTF8Char *)"", user->userName->v, sbSName.ToString(), group->id, (const UTF8Char*)"", sb.ToString(), (const UTF8Char*)"", group->cateId);
					}
				}
			}
		}
	}
}

void SSWR::OrganMgr::OrganWebHandler::LoadLocations()
{
	SSWR::OrganMgr::OrganWebHandler::LocationInfo *loc;
	DB::DBReader *r = this->db->ExecuteReader((const UTF8Char*)"select id, parentId, cname, ename, lat, lon, cate_id, locType from location");
	Int32 id;
	if (r != 0)
	{
		while (r->ReadNext())
		{
			id = r->GetInt32(0);
			loc = this->locMap->Get(id);
			if (loc == 0)
			{
				loc = MemAlloc(SSWR::OrganMgr::OrganWebHandler::LocationInfo, 1);
				loc->id = id;
				loc->parentId = r->GetInt32(1);
				loc->cname = r->GetNewStr(2);
				loc->ename = r->GetNewStr(3);
				loc->lat = r->GetDbl(4);
				loc->lon = r->GetDbl(5);
				loc->cateId = r->GetInt32(6);
				loc->locType = r->GetInt32(7);
				this->locMap->Put(id, loc);
			}
		}
		this->db->CloseReader(r);
	}
}

void SSWR::OrganMgr::OrganWebHandler::FreeSpecies()
{
	Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> *spList;
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
	SSWR::OrganMgr::OrganWebHandler::WebFileInfo *wfile;
	Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::WebFileInfo*> *wfiles;
	UOSInt i;
	UOSInt j;

	spList = this->spMap->GetValues();
	i = spList->GetCount();
	while (i-- > 0)
	{
		sp = spList->GetItem(i);
		sp->engName->Release();
		sp->chiName->Release();
		sp->sciName->Release();
		sp->descript->Release();
		sp->dirName->Release();
		SDEL_STRING(sp->photo);
		sp->idKey->Release();

		DEL_CLASS(sp->books);
		DEL_CLASS(sp->files);
		wfiles = sp->wfiles->GetValues();
		j = wfiles->GetCount();
		while (j-- > 0)
		{
			wfile = wfiles->GetItem(j);
			wfile->imgUrl->Release();
			wfile->srcUrl->Release();
			wfile->location->Release();
			MemFree(wfile);
		}
		DEL_CLASS(sp->wfiles);
		MemFree(sp);
	}
	this->spMap->Clear();
	this->spNameMap->Clear();
}

void SSWR::OrganMgr::OrganWebHandler::FreeGroups()
{
	Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::CategoryInfo*> *cateList;
	Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupInfo*> *groupList;
	SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
	UOSInt i;
	cateList = this->cateMap->GetValues();
	i = cateList->GetCount();
	while (i-- > 0)
	{
		cate = cateList->GetItem(i);
		cate->groups->Clear();
	}

	groupList = this->groupMap->GetValues();
	i = groupList->GetCount();
	while (i-- > 0)
	{
		group = groupList->GetItem(i);
		FreeGroup(group);
	}
	this->groupMap->Clear();
}

void SSWR::OrganMgr::OrganWebHandler::FreeGroup(GroupInfo *group)
{
	group->engName->Release();
	group->chiName->Release();
	group->descript->Release();
	SDEL_STRING(group->idKey);
	DEL_CLASS(group->species);
	DEL_CLASS(group->groups);

	MemFree(group);
}

void SSWR::OrganMgr::OrganWebHandler::FreeBooks()
{
	Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::BookInfo*> *bookList;
	SSWR::OrganMgr::OrganWebHandler::BookInfo *book;
	SSWR::OrganMgr::OrganWebHandler::BookSpInfo *bookSp;
	UOSInt i;
	UOSInt j;

	bookList = this->bookMap->GetValues();
	i = bookList->GetCount();
	while (i-- > 0)
	{
		book = bookList->GetItem(i);
		book->title->Release();
		book->author->Release();
		book->press->Release();
		SDEL_STRING(book->url);
		j = book->species->GetCount();
		while (j-- > 0)
		{
			bookSp = book->species->GetItem(j);
			bookSp->dispName->Release();
			MemFree(bookSp);
		}
		DEL_CLASS(book->species);
		MemFree(book);
	}
	this->bookMap->Clear();
}

void SSWR::OrganMgr::OrganWebHandler::FreeUsers()
{
	Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::WebUserInfo*> *userList = this->userMap->GetValues();
	SSWR::OrganMgr::OrganWebHandler::WebUserInfo *user;
	SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile;
	Data::ArrayList<Data::Int64Map<SSWR::OrganMgr::OrganWebHandler::TripInfo*>*> *tripCateList;
	Data::Int64Map<SSWR::OrganMgr::OrganWebHandler::TripInfo*> *tripCate;
	Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::TripInfo*> *tripList;
	SSWR::OrganMgr::OrganWebHandler::TripInfo *trip;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	i = userList->GetCount();
	while (i-- > 0)
	{
		user = userList->GetItem(i);
		user->userName->Release();
		user->watermark->Release();
		SDEL_STRING(user->pwd);

		j = user->userFileObj->GetCount();
		while (j-- > 0)
		{
			userFile = user->userFileObj->GetItem(j);
			userFile->oriFileName->Release();
			userFile->dataFileName->Release();
			SDEL_STRING(userFile->descript);
			SDEL_STRING(userFile->location);
			MemFree(userFile);
		}
		DEL_CLASS(user->userFileIndex);
		DEL_CLASS(user->userFileObj);

		tripCateList = user->tripCates->GetValues();
		j = tripCateList->GetCount();
		while (j-- > 0)
		{
			tripCate = tripCateList->GetItem(j);
			tripList = tripCate->GetValues();
			k = tripList->GetCount();
			while (k-- > 0)
			{
				trip = tripList->GetItem(k);
				MemFree(trip);
			}
			DEL_CLASS(tripCate);
		}
		DEL_CLASS(user->tripCates);
		MemFree(user);
	}
	this->userMap->Clear();
	this->userNameMap->Clear();
	this->userFileMap->Clear();
}

void SSWR::OrganMgr::OrganWebHandler::ClearUsers()
{
	Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::WebUserInfo*> *userList = this->userMap->GetValues();
	SSWR::OrganMgr::OrganWebHandler::WebUserInfo *user;
	SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile;
	UOSInt i;
	UOSInt j;
	i = userList->GetCount();
	while (i-- > 0)
	{
		user = userList->GetItem(i);

		j = user->userFileObj->GetCount();
		while (j-- > 0)
		{
			userFile = user->userFileObj->GetItem(j);
			userFile->oriFileName->Release();
			userFile->dataFileName->Release();
			SDEL_STRING(userFile->descript);
			MemFree(userFile);
		}
		user->userFileIndex->Clear();
		user->userFileObj->Clear();
	}
	this->userFileMap->Clear();
}

void SSWR::OrganMgr::OrganWebHandler::UserFilePrevUpdated(SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile)
{
	if (userFile->prevUpdated)
	{
		DB::SQLBuilder sql(this->db);
		sql.AppendCmd((const UTF8Char*)"update userfile set prevUpdated = 0 where id = ");
		sql.AppendInt32(userFile->id);
		if (this->db->ExecuteNonQuery(sql.ToString()) < 0)
		{
			this->db->ExecuteNonQuery(sql.ToString());
		}
		userFile->prevUpdated = 0;
	}
}

void SSWR::OrganMgr::OrganWebHandler::WebFilePrevUpdated(SSWR::OrganMgr::OrganWebHandler::WebFileInfo *wfile)
{
	if (wfile->prevUpdated)
	{
		DB::SQLBuilder sql(this->db);
		sql.AppendCmd((const UTF8Char*)"update webfile set prevUpdated = 0 where id = ");
		sql.AppendInt32(wfile->id);
		if (this->db->ExecuteNonQuery(sql.ToString()) < 0)
		{
			this->db->ExecuteNonQuery(sql.ToString());
		}
		wfile->prevUpdated = 0;
	}
}

void SSWR::OrganMgr::OrganWebHandler::CalcGroupCount(SSWR::OrganMgr::OrganWebHandler::GroupInfo *group)
{
	UOSInt i;
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *sgroup;
	if (group->myPhotoCount != (UOSInt)-1)
		return;

	group->myPhotoCount = 0;
	group->photoCount = 0;
	group->totalCount = 0;

	group->totalCount += group->species->GetCount();
	i = group->species->GetCount();
	while (i-- > 0)
	{
		sp = group->species->GetItem(i);
		if (sp->flags & 9)
		{
			group->photoCount++;
			if (sp->photoId != 0 || sp->photoWId != 0 || sp->photo != 0)
			{
				if (group->photoSpObj == 0 || group->photoSpecies == sp->speciesId)
				{
					group->photoSpObj = sp;
				}
			}
		}
		if (sp->flags & 1)
		{
			group->myPhotoCount++;
		}
	}

	i = group->groups->GetCount();
	while (i-- > 0)
	{
		sgroup = group->groups->GetItem(i);
		this->CalcGroupCount(sgroup);
		group->myPhotoCount += sgroup->myPhotoCount;
		group->photoCount += sgroup->photoCount;
		group->totalCount += sgroup->totalCount;
		if (group->photoSpObj == 0 || group->photoGroup == sgroup->id)
		{
			group->photoSpObj = sgroup->photoSpObj;
		}
	}
}

void SSWR::OrganMgr::OrganWebHandler::GetGroupSpecies(SSWR::OrganMgr::OrganWebHandler::GroupInfo *group, Data::StringUTF8Map<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> *spMap, SSWR::OrganMgr::OrganWebHandler::WebUserInfo *user)
{
	UOSInt i;
	UOSInt j;
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *sgroup;
	i = 0;
	j = group->species->GetCount();
	while (i < j)
	{
		sp = group->species->GetItem(i);
		spMap->Put(sp->sciName->v, sp);
		i++;
	}
	i = group->groups->GetCount();
	while (i-- > 0)
	{
		sgroup = group->groups->GetItem(i);
		if ((sgroup->flags & 1) == 0 || user != 0)
		{
			GetGroupSpecies(sgroup, spMap, user);
		}
	}
}

void SSWR::OrganMgr::OrganWebHandler::SearchInGroup(SSWR::OrganMgr::OrganWebHandler::GroupInfo *group, const UTF8Char *searchStr, Data::ArrayListDbl *speciesIndice, Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> *speciesObjs, Data::ArrayListDbl *groupIndice, Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupInfo*> *groupObjs, SSWR::OrganMgr::OrganWebHandler::WebUserInfo *user)
{
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species;
	SSWR::OrganMgr::OrganWebHandler::BookSpInfo *bookSp;
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *subGroup;
	Double rating;
	Double currRating;
	UOSInt strLen = Text::StrCharCnt(searchStr);
	UOSInt i;
	UOSInt j;
/*
h = b
o = a
i = l
e = c
*/
	i = group->species->GetCount();
	while (i-- > 0)
	{
		rating = 0;
		species = group->species->GetItem(i);
		if (species->sciName->Equals(searchStr) || species->chiName->Equals(searchStr))
		{
			speciesIndice->Add(1.0);
			speciesObjs->Add(species);
		}
		else
		{
			if (rating < (currRating = species->sciName->MatchRating(searchStr, strLen)))
				rating = currRating;
			if (rating < (currRating = species->chiName->MatchRating(searchStr, strLen)))
				rating = currRating;
			if (rating < (currRating = species->engName->MatchRating(searchStr, strLen)))
				rating = currRating;
			if (rating < (currRating = species->descript->MatchRating(searchStr, strLen)))
				rating = currRating;
			j = species->books->GetCount();
			while (j-- > 0)
			{
				bookSp = species->books->GetItem(j);
				if (bookSp->dispName->Equals(searchStr))
				{
					rating = 1.0;
					break;
				}
				else
				{
					if (rating < (currRating = bookSp->dispName->MatchRating(searchStr, strLen)))
						rating = currRating;
				}
			}
			if (rating > 0)
			{
				j = speciesIndice->SortedInsert(rating);
				speciesObjs->Insert(j, species);
			}
		}
	}
	i = group->groups->GetCount();
	while (i-- > 0)
	{
		rating = 0;
		subGroup = group->groups->GetItem(i);
		if (user == 0 && (subGroup->flags & 1))
		{

		}
		else
		{
			if (subGroup->engName->Equals(searchStr) || subGroup->chiName->Equals(searchStr))
			{
				groupIndice->Add(1.0);
				groupObjs->Add(subGroup);
			}
			else
			{
				if (rating < (currRating = subGroup->engName->MatchRating(searchStr, strLen)))
					rating = currRating;
				if (rating < (currRating = subGroup->chiName->MatchRating(searchStr, strLen)))
					rating = currRating;
				if (rating > 0)
				{
					j = groupIndice->SortedInsert(rating);
					groupObjs->Insert(j, subGroup);
				}
			}
			SearchInGroup(subGroup, searchStr, speciesIndice, speciesObjs, groupIndice, groupObjs, user);
		}
	}
}

Bool SSWR::OrganMgr::OrganWebHandler::GroupIsAdmin(SSWR::OrganMgr::OrganWebHandler::GroupInfo *group)
{
	while (group)
	{
		if (group->flags & 1)
		{
			return true;
		}
		group = this->groupMap->Get(group->parentId);
	}
	return false;
}

UTF8Char *SSWR::OrganMgr::OrganWebHandler::PasswordEnc(UTF8Char *buff, const UTF8Char *pwd)
{
	UOSInt strLen = Text::StrCharCnt(pwd);
	UInt8 md5Val[16];
	Crypto::Hash::MD5 md5;
	md5.Calc(pwd, strLen);
	md5.GetValue(md5Val);
	Text::StrHexBytes(buff, md5Val, 16, 0);
	return Text::StrToUpper(buff, buff);
}

Bool SSWR::OrganMgr::OrganWebHandler::BookFileExist(BookInfo *book)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = Text::StrConcat(sbuff, this->dataDir);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"BookFile");
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, book->id);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)".pdf");
	return IO::Path::GetPathType(sbuff) == IO::Path::PathType::File;
}

Bool SSWR::OrganMgr::OrganWebHandler::UserGPSGetPos(Int32 userId, Data::DateTime *t, Double *lat, Double *lon)
{
/*	OSInt i;
	WebUserInfo *webUser;
	DataFileInfo *dataFile;
	UTF8Char u8buff[512];
	UTF8Char *sptr;
	IO::StmData::FileData *fd;
	if (this->gpsTrk == 0 || this->gpsUserId != userId || this->gpsStartTime->CompareTo(t) > 0 || this->gpsEndTime->CompareTo(t) < 0)
	{
		SDEL_CLASS(this->gpsTrk);
		this->gpsUserId = userId;
		webUser = this->userMap->Get(userId);
		i = webUser->gpsFileIndex->SortedIndexOf(t->ToTicks());
		if (i < 0)
		{
			i = ~i - 1;
		}
		dataFile = webUser->gpsFileObj->GetItem(i);
		if (dataFile != 0)
		{
			this->gpsStartTime->SetTicks(dataFile->startTimeTicks);
			this->gpsEndTime->SetTicks(dataFile->endTimeTicks);
			sptr = Text::StrConcat(u8buff, this->dataDir);
			if (sptr[-1] != IO::Path::PATH_SEPERATOR)
			{
				*sptr++ = IO::Path::PATH_SEPERATOR;
			}
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"DataFile");
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrConcat(sptr, dataFile->fileName);
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
	{*/
		*lat = 0;
		*lon = 0;
		return false;
//	}
}

Int32 SSWR::OrganMgr::OrganWebHandler::SpeciesAdd(const UTF8Char *engName, const UTF8Char *chiName, const UTF8Char *sciName, Int32 groupId, const UTF8Char *description, const UTF8Char *dirName, const UTF8Char *idKey, Int32 cateId)
{
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"insert into species (eng_name, chi_name, sci_name, group_id, description, dirName, idKey, cate_id, mapColor) values (");
	sql.AppendStrUTF8(engName);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(chiName);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(sciName);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendInt32(groupId);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(description);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(dirName);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(idKey);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendInt32(cateId);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendInt32((Int32)0xff4040ff);
	sql.AppendCmd((const UTF8Char*)")");
	if (this->db->ExecuteNonQuery(sql.ToString()) == 1)
	{
		SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species = MemAlloc(SSWR::OrganMgr::OrganWebHandler::SpeciesInfo, 1);
		species->speciesId = this->db->GetLastIdentity32();
		species->engName = Text::String::New(engName);
		species->chiName = Text::String::New(chiName);
		species->sciName = Text::String::New(sciName);
		species->groupId = groupId;
		species->descript = Text::String::New(description);
		species->dirName = Text::String::New(dirName);
		species->photo = 0;
		species->idKey = Text::String::New(idKey);
		species->cateId = cateId;
		species->flags = SF_NONE;
		species->photoId = 0;
		species->photoWId = 0;

		NEW_CLASS(species->books, Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::BookSpInfo*>());
		NEW_CLASS(species->files, Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::UserFileInfo*>());
		NEW_CLASS(species->wfiles, Data::Int32Map<SSWR::OrganMgr::OrganWebHandler::WebFileInfo*>());
		this->spMap->Put(species->speciesId, species);
		this->spNameMap->Put(species->sciName->v, species);

		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group = this->groupMap->Get(species->groupId);
		if (group)
		{
			group->species->Add(species);
			this->GroupAddCounts(group->id, 1, 0, 0);
		}
		return species->speciesId;
	}
	else
	{
		return 0;
	}
}

Bool SSWR::OrganMgr::OrganWebHandler::SpeciesSetPhotoId(Int32 speciesId, Int32 photoId)
{
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species = this->spMap->Get(speciesId);
	if (species == 0)
		return false;
	if (species->photoId == photoId)
	{
		return true;
	}
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"update species set photoId = ");
	sql.AppendInt32(photoId);
	sql.AppendCmd((const UTF8Char*)" where id = ");
	sql.AppendInt32(speciesId);
	if (this->db->ExecuteNonQuery(sql.ToString()) == 1)
	{
		species->photoId = photoId;
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::OrganMgr::OrganWebHandler::SpeciesSetFlags(Int32 speciesId, SpeciesFlags flags)
{

	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species = this->spMap->Get(speciesId);
	if (species == 0)
		return false;
	if (species->flags == flags)
	{
		return true;
	}
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"update species set flags = ");
	sql.AppendInt32(flags);
	sql.AppendCmd((const UTF8Char*)" where id = ");
	sql.AppendInt32(speciesId);
	if (this->db->ExecuteNonQuery(sql.ToString()) == 1)
	{
		species->flags = flags;
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::OrganMgr::OrganWebHandler::SpeciesMove(Int32 speciesId, Int32 groupId, Int32 cateId)
{
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species = this->spMap->Get(speciesId);
	if (species == 0)
		return false;
	if (species->groupId == groupId)
	{
		return true;
	}
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"update species set group_id = ");
	sql.AppendInt32(groupId);
	sql.AppendCmd((const UTF8Char*)", cate_id = ");
	sql.AppendInt32(cateId);
	sql.AppendCmd((const UTF8Char*)" where id = ");
	sql.AppendInt32(speciesId);
	if (this->db->ExecuteNonQuery(sql.ToString()) == 1)
	{
		UOSInt totalCount = 1;
		UOSInt photoCount = 0;
		UOSInt myPhotoCount = 0;
		if (species->flags & 9)
		{
			photoCount = 1;
			if (species->flags & 1)
			{
				myPhotoCount = 1;
			}
		}
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group = this->groupMap->Get(species->groupId);
		if (group)
		{
			group->species->Remove(species);
			if (group->photoSpecies == species->speciesId)
			{
				group->photoSpObj = 0;
				this->GroupSetPhotoSpecies(group->id, 0);
			}
			this->GroupAddCounts(group->id, -totalCount, -photoCount, -myPhotoCount);
		}
		species->groupId = groupId;
		species->cateId = cateId;
		group = this->groupMap->Get(species->groupId);
		if (group)
		{
			group->species->Add(species);
			if (group->photoSpObj == 0 && (species->photoId != 0 || species->photo != 0 || species->photoWId != 0))
			{
				group->photoSpObj = species;
			}
			this->GroupAddCounts(group->id, totalCount, photoCount, myPhotoCount);
		}
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::OrganMgr::OrganWebHandler::SpeciesModify(Int32 speciesId, const UTF8Char *engName, const UTF8Char *chiName, const UTF8Char *sciName, const UTF8Char *description, const UTF8Char *dirName)
{
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species = this->spMap->Get(speciesId);
	if (species == 0)
		return false;
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"update species set eng_name = ");
	sql.AppendStrUTF8(engName);
	sql.AppendCmd((const UTF8Char*)", chi_name = ");
	sql.AppendStrUTF8(chiName);
	sql.AppendCmd((const UTF8Char*)", sci_name = ");
	sql.AppendStrUTF8(sciName);
	sql.AppendCmd((const UTF8Char*)", description = ");
	sql.AppendStrUTF8(description);
	sql.AppendCmd((const UTF8Char*)", dirName = ");
	sql.AppendStrUTF8(dirName);
	sql.AppendCmd((const UTF8Char*)" where id = ");
	sql.AppendInt32(speciesId);
	if (this->db->ExecuteNonQuery(sql.ToString()) >= 0)
	{
		SDEL_STRING(species->engName);
		species->engName = Text::String::New(engName);
		SDEL_STRING(species->chiName);
		species->chiName = Text::String::New(chiName);
		SDEL_STRING(species->sciName);
		species->sciName = Text::String::New(sciName);
		SDEL_STRING(species->descript);
		species->descript = Text::String::New(description);
		SDEL_STRING(species->dirName);
		species->dirName = Text::String::New(dirName);
		return true;
	}
	else
	{
		return false;
	}
}

Int32 SSWR::OrganMgr::OrganWebHandler::UserfileAdd(Int32 userId, Int32 spId, const UTF8Char *fileName, const UInt8 *fileCont, UOSInt fileSize)
{
	UOSInt j;
	UOSInt i;
	Int32 fileType = 0;
	i = Text::StrLastIndexOf(fileName, '.');
	if (i == INVALID_INDEX)
	{
		return 0;
	}
	if (Text::StrEqualsICase(&fileName[i + 1], (const UTF8Char*)"JPG"))
	{
		fileType = 1;
	}
	else if (Text::StrEqualsICase(&fileName[i + 1], (const UTF8Char*)"TIF"))
	{
		fileType = 1;
	}
	else if (Text::StrEqualsICase(&fileName[i + 1], (const UTF8Char*)"PCX"))
	{
		fileType = 1;
	}
	else if (Text::StrEqualsICase(&fileName[i + 1], (const UTF8Char*)"GIF"))
	{
		fileType = 1;
	}
	else if (Text::StrEqualsICase(&fileName[i + 1], (const UTF8Char*)"PNG"))
	{
		fileType = 1;
	}
	else if (Text::StrEqualsICase(&fileName[i + 1], (const UTF8Char*)"AVI"))
	{
		fileType = 2;
	}
	else if (Text::StrEqualsICase(&fileName[i + 1], (const UTF8Char*)"MOV"))
	{
		fileType = 2;
	}
	else if (Text::StrEqualsICase(&fileName[i + 1], (const UTF8Char*)"WAV"))
	{
		fileType = 3;
	}
	else
	{
		return 0;
	}
	if (fileType == 1)
	{
		IO::StmData::MemoryData *md;
		IO::ParserType t;
		IO::ParsedObject *pobj;
		Bool valid = false;
		Data::DateTime fileTime;
		Double lat = 0;
		Double lon = 0;
		SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile;
		const UTF8Char *camera = 0;
		UInt32 crcVal = 0;
		fileTime.SetTicks(0);
		fileTime.ToLocalTime();

		NEW_CLASS(md, IO::StmData::MemoryData(fileCont, fileSize));
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
							this->UserGPSGetPos(userId, &fileTime, &lat, &lon);
						}
						const UTF8Char *csptr;
						const UTF8Char *csptr2;
						csptr = (const UTF8Char*)exif->GetPhotoMake();
						csptr2 = (const UTF8Char*)exif->GetPhotoModel();
						if (csptr && csptr2)
						{
							if (Text::StrStartsWithICase(csptr2, csptr))
							{
								camera = Text::StrCopyNew(csptr2);
							}
							else
							{
								Text::StringBuilderUTF8 sb;
								sb.Append(csptr);
								sb.Append((const UTF8Char*)" ");
								sb.Append(csptr2);
								camera = Text::StrCopyNew(sb.ToString());
							}
						}
						else if (csptr)
						{
							camera = Text::StrCopyNew(csptr);
						}
						else if (csptr2)
						{
							camera = Text::StrCopyNew(csptr2);
						}
					}
				}

				UInt8 crcBuff[4];
				Crypto::Hash::CRC32R crc;
				crc.Calc(fileCont, fileSize);
				crc.GetValue(crcBuff);
				crcVal = ReadMUInt32(crcBuff);
			}
			DEL_CLASS(pobj);
		}
		if (valid)
		{
			SSWR::OrganMgr::OrganWebHandler::WebUserInfo *webUser = this->userMap->Get(userId);
			Int64 ticks = fileTime.ToTicks();
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
				sptr = Text::StrConcat(sbuff, this->dataDir);
				if (sptr[-1] != IO::Path::PATH_SEPERATOR)
				{
					*sptr++ = IO::Path::PATH_SEPERATOR;
				}
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"UserFile");
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = Text::StrInt32(sptr, userId);
				*sptr++ = IO::Path::PATH_SEPERATOR;
				fileTime.ToUTCTime();
				sptr = fileTime.ToString(sptr, "yyyyMM");
				IO::Path::CreateDirectory(sbuff);
				*sptr++ = IO::Path::PATH_SEPERATOR;
				dataFileName = sptr;
				sptr = Text::StrInt64(sptr, ticks);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"_");
				sptr = Text::StrHexVal32(sptr, crcVal);
				i = Text::StrLastIndexOf(fileName, '.');
				if (i != INVALID_INDEX)
				{
					sptr = Text::StrConcat(sptr, &fileName[i]);
				}

				IO::FileStream *fs;
				NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
				Bool succ = (fs->Write(fileCont, fileSize) == fileSize);
				DEL_CLASS(fs);
				if (succ)
				{
					DB::SQLBuilder sql(this->db);
					sql.AppendCmd((const UTF8Char*)"insert into userfile (fileType, oriFileName, fileTime, lat, lon, webuser_id, species_id, captureTime, dataFileName, crcVal, camera, cropLeft, cropTop, cropRight, cropBottom) values (");
					sql.AppendInt32(fileType);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendStrUTF8(fileName);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendDate(&fileTime);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendDbl(lat);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendDbl(lon);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendInt32(userId);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendInt32(spId);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendDate(&fileTime);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendStrUTF8(dataFileName);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendInt32((Int32)crcVal);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendStrUTF8(camera);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendDbl(0);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendDbl(0);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendDbl(0);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendDbl(0);
					sql.AppendCmd((const UTF8Char*)")");
					if (this->db->ExecuteNonQuery(sql.ToString()) > 0)
					{
						userFile = MemAlloc(SSWR::OrganMgr::OrganWebHandler::UserFileInfo, 1);
						userFile->id = this->db->GetLastIdentity32();
						userFile->fileType = fileType;
						userFile->oriFileName = Text::String::New(fileName);
						userFile->fileTimeTicks = fileTime.ToTicks();
						userFile->lat = lat;
						userFile->lon = lon;
						userFile->webuserId = userId;
						userFile->speciesId = spId;
						userFile->captureTimeTicks = userFile->fileTimeTicks;
						userFile->dataFileName = Text::String::New(dataFileName);
						userFile->crcVal = crcVal;
						userFile->rotType = 0;
						userFile->prevUpdated = 0;
						//userFile->camera = camera;
						userFile->cropLeft = 0;
						userFile->cropTop = 0;
						userFile->cropRight = 0;
						userFile->cropBottom = 0;
						userFile->descript = 0;
						userFile->location = 0;
						this->userFileMap->Put(userFile->id, userFile);

						SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species = this->spMap->Get(userFile->speciesId);
						if (species)
						{
							species->files->Add(userFile);
							if (species->photoId == 0)
							{
								this->SpeciesSetPhotoId(species->speciesId, userFile->id);
							}
						}

						webUser = this->userMap->Get(userFile->webuserId);
						j = webUser->userFileIndex->SortedInsert(userFile->fileTimeTicks);
						webUser->userFileObj->Insert(j, userFile);
						
						SDEL_TEXT(camera);
						return userFile->id;
					}
					else
					{
						SDEL_TEXT(camera);
						return 0;
					}
				}
				else
				{
					SDEL_TEXT(camera);
					return 0;
				}
			}
			else
			{
				SDEL_TEXT(camera);
				return 0;
			}
		}
		else
		{
			SDEL_TEXT(camera);
			return 0;
		}
	}
	else if (fileType == 3)
	{
		Crypto::Hash::CRC32R crc;
		IO::StmData::FileData *fd;
		UInt32 crcVal;
		IO::ParsedObject *pobj;
		IO::ParserType t;
		Data::DateTime fileTime;
		SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile;
		Bool valid = false;
		Media::DrawImage *graphImg = 0;
		fileTime.SetTicks(0);
		UInt8 crcBuff[4];
		crc.Calc(fileCont, fileSize);
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
					graphImg = Media::FrequencyGraph::CreateGraph(this->eng, (Media::IAudioSource *)msrc, 2048, 2048, Math::FFTCalc::WT_BLACKMANN_HARRIS, 12);
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
			SSWR::OrganMgr::OrganWebHandler::WebUserInfo *webUser = this->userMap->Get(userId);
			Int64 ticks = 0;
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
				sptr = Text::StrConcat(sbuff, this->dataDir);
				if (sptr[-1] != IO::Path::PATH_SEPERATOR)
				{
					*sptr++ = IO::Path::PATH_SEPERATOR;
				}
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"UserFile");
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = Text::StrInt32(sptr, userId);
				*sptr++ = IO::Path::PATH_SEPERATOR;
				fileTime.ToUTCTime();
				sptr = fileTime.ToString(sptr, "yyyyMM");
				IO::Path::CreateDirectory(sbuff);
				*sptr++ = IO::Path::PATH_SEPERATOR;
				dataFileName = sptr;
				sptr = Text::StrInt64(sptr, ticks);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"_");
				sptr = Text::StrHexVal32(sptr, crcVal);
				i = Text::StrLastIndexOf(fileName, '.');
				if (i != INVALID_INDEX)
				{
					sptr = Text::StrConcat(sptr, &fileName[i]);
				}
				IO::FileStream *fs;
				NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
				Bool succ = (fs->Write(fileCont, fileSize) == fileSize);
				DEL_CLASS(fs);
				if (succ)
				{
					DB::SQLBuilder sql(this->db);
					sql.AppendCmd((const UTF8Char*)"insert into userfile (fileType, oriFileName, fileTime, lat, lon, webuser_id, species_id, captureTime, dataFileName, crcVal, camera) values (");
					sql.AppendInt32(fileType);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendStrUTF8(fileName);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendDate(&fileTime);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendDbl(0);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendDbl(0);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendInt32(userId);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendInt32(spId);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendDate(&fileTime);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendStrUTF8(dataFileName);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendInt32((Int32)crcVal);
					sql.AppendCmd((const UTF8Char*)", ");
					sql.AppendStrUTF8(0);
					sql.AppendCmd((const UTF8Char*)")");
					if (this->db->ExecuteNonQuery(sql.ToString()) > 0)
					{
						userFile = MemAlloc(SSWR::OrganMgr::OrganWebHandler::UserFileInfo, 1);
						userFile->id = this->db->GetLastIdentity32();
						userFile->fileType = fileType;
						userFile->oriFileName = Text::String::New(fileName);
						userFile->fileTimeTicks = fileTime.ToTicks();
						userFile->lat = 0;
						userFile->lon = 0;
						userFile->webuserId = userId;
						userFile->speciesId = spId;
						userFile->captureTimeTicks = userFile->fileTimeTicks;
						userFile->dataFileName = Text::String::New(dataFileName);
						userFile->crcVal = crcVal;
						userFile->rotType = 0;
						//userFile->camera = 0;
						userFile->descript = 0;
						userFile->cropLeft = 0;
						userFile->cropTop = 0;
						userFile->cropRight = 0;
						userFile->cropBottom = 0;
						this->userFileMap->Put(userFile->id, userFile);

						SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species = this->spMap->Get(userFile->speciesId);
						if (species)
						{
							species->files->Add(userFile);
							if (species->photoId == 0)
							{
								this->SpeciesSetPhotoId(species->speciesId, userFile->id);
							}
						}

						webUser = this->userMap->Get(userFile->webuserId);
						j = webUser->userFileIndex->SortedInsert(userFile->fileTimeTicks);
						webUser->userFileObj->Insert(j, userFile);
						
						sptr = Text::StrConcat(sbuff, this->dataDir);
						if (sptr[-1] != IO::Path::PATH_SEPERATOR)
						{
							*sptr++ = IO::Path::PATH_SEPERATOR;
						}
						sptr = Text::StrConcat(sptr, (const UTF8Char*)"UserFile");
						*sptr++ = IO::Path::PATH_SEPERATOR;
						sptr = Text::StrInt32(sptr, userId);
						*sptr++ = IO::Path::PATH_SEPERATOR;
						fileTime.ToUTCTime();
						sptr = fileTime.ToString(sptr, "yyyyMM");
						IO::Path::CreateDirectory(sbuff);
						*sptr++ = IO::Path::PATH_SEPERATOR;
						sptr = Text::StrInt64(sptr, ticks);
						sptr = Text::StrConcat(sptr, (const UTF8Char*)"_");
						sptr = Text::StrHexVal32(sptr, crcVal);
						sptr = Text::StrConcat(sptr, (const UTF8Char*)".png");
						NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
						graphImg->SavePng(fs);
						DEL_CLASS(fs);
						this->eng->DeleteImage(graphImg);

						return userFile->id;
					}
					else
					{
						if (graphImg)
						{
							this->eng->DeleteImage(graphImg);
						}
						return 0;
					}
				}
				else
				{
					if (graphImg)
					{
						this->eng->DeleteImage(graphImg);
					}
					return 0;
				}
			}
			else
			{
				if (graphImg)
				{
					this->eng->DeleteImage(graphImg);
				}
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
	else
	{
/*		UTF8Char sbuff[512];
		UTF8Char *sptr;
		sptr = this->GetSpeciesDir(sp, sbuff);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrConcat(sptr, &fileName[i + 1]);
		if (IO::FileUtil::CopyFile(fileName, sbuff, IO::FileUtil::FEA_FAIL, 0, 0))
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
		}*/
		return 0;
	}
}

Bool SSWR::OrganMgr::OrganWebHandler::UserfileMove(Int32 userfileId, Int32 speciesId, Int32 cateId)
{
	UserFileInfo *userFile = this->userFileMap->Get(userfileId);
	if (userFile == 0)
	{
		return false;
	}
	if (userFile->speciesId == speciesId)
	{
		return true;
	}
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *srcSpecies = this->spMap->Get(userFile->speciesId);
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *destSpecies = this->spMap->Get(speciesId);
	if (srcSpecies == 0 || destSpecies == 0)
	{
		return false;
	}
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"update userfile set species_id = ");
	sql.AppendInt32(speciesId);
	sql.AppendCmd((const UTF8Char*)" where id = ");
	sql.AppendInt32(userfileId);
	if (this->db->ExecuteNonQuery(sql.ToString()) > 0)
	{
		userFile->speciesId = speciesId;

		UOSInt i = srcSpecies->files->GetCount();
		while (i-- > 0)
		{
			if (srcSpecies->files->GetItem(i) == userFile)
			{
				srcSpecies->files->RemoveAt(i);
				break;
			}
		}

		destSpecies->files->Add(userFile);
		if ((destSpecies->flags & 1) == 0)
		{
			this->SpeciesSetFlags(destSpecies->speciesId, (SpeciesFlags)(destSpecies->flags | SF_HAS_MYPHOTO));
			this->GroupAddCounts(destSpecies->groupId, 0, 1, 1);
			SSWR::OrganMgr::OrganWebHandler::GroupInfo *group = this->groupMap->Get(destSpecies->groupId);
			while (group && group->photoSpObj == 0)
			{
				group->photoSpObj = destSpecies;
				group = this->groupMap->Get(group->parentId);
			}
		}
		if (destSpecies->photoId == 0)
		{
			this->SpeciesSetPhotoId(destSpecies->speciesId, userFile->id);
		}
		return true;
	}
	return false;
}

Bool SSWR::OrganMgr::OrganWebHandler::UserfileUpdateDesc(Int32 userfileId, const UTF8Char *descr)
{
	UserFileInfo *userFile = this->userFileMap->Get(userfileId);
	if (userFile == 0)
	{
		return false;
	}
	if (descr && descr[0] == 0)
	{
		descr = 0;
	}
	if (userFile->descript == 0 && descr == 0)
	{
		return true;
	}
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"update userfile set descript = ");
	sql.AppendStrUTF8(descr);
	sql.AppendCmd((const UTF8Char*)" where id = ");
	sql.AppendInt32(userfileId);
	if (this->db->ExecuteNonQuery(sql.ToString()) > 0)
	{
		SDEL_STRING(userFile->descript);
		userFile->descript = Text::String::New(descr);
		return true;
	}
	return false;
}

Bool SSWR::OrganMgr::OrganWebHandler::UserfileUpdateRotType(Int32 userfileId, Int32 rotType)
{
	UserFileInfo *userFile = this->userFileMap->Get(userfileId);
	if (userFile == 0)
	{
		return false;
	}
	if (rotType < 0 || rotType >= 4)
	{
		rotType = 0;
	}
	if (userFile->rotType == rotType)
	{
		return true;
	}
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"update userfile set rotType = ");
	sql.AppendInt32(rotType);
	sql.AppendCmd((const UTF8Char*)", prevUpdated = ");
	sql.AppendInt32(1);
	sql.AppendCmd((const UTF8Char*)" where id = ");
	sql.AppendInt32(userfileId);
	if (this->db->ExecuteNonQuery(sql.ToString()) > 0)
	{
		userFile->rotType = rotType;
		userFile->prevUpdated = 1;
		return true;
	}
	return false;
}

Bool SSWR::OrganMgr::OrganWebHandler::SpeciesBookIsExist(const UTF8Char *speciesName, Text::StringBuilderUTF *bookNameOut)
{
	Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::BookInfo*> *bookList = this->bookMap->GetValues();
	SSWR::OrganMgr::OrganWebHandler::BookInfo *book;
	SSWR::OrganMgr::OrganWebHandler::BookSpInfo *bookSp;
	UOSInt i = 0;
	UOSInt j = bookList->GetCount();
	UOSInt k;
	while (i < j)
	{
		book = bookList->GetItem(i);
		k = book->species->GetCount();
		while (k-- > 0)
		{
			bookSp = book->species->GetItem(k);
			if (bookSp->dispName && bookSp->dispName->Equals(speciesName))
			{
				bookNameOut->Append(book->title);
				return true;
			}
		}
		i++;
	}
	return false;
}

Int32 SSWR::OrganMgr::OrganWebHandler::GroupAdd(const UTF8Char* engName, const UTF8Char *chiName, Int32 parentId, const UTF8Char *descr, Int32 groupTypeId, Int32 cateId, GroupFlags flags)
{
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *group = this->groupMap->Get(parentId);
	if (group == 0)
		return 0;
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"insert into groups (group_type, eng_name, chi_name, description, parent_id, idKey, cate_id, flags) values (");
	sql.AppendInt32(groupTypeId);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(engName);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(chiName);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(descr);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendInt32(parentId);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(0);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendInt32(cateId);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendInt32(flags);
	sql.AppendCmd((const UTF8Char*)")");
	if (this->db->ExecuteNonQuery(sql.ToString()) == 1)
	{
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *newGroup = MemAlloc(SSWR::OrganMgr::OrganWebHandler::GroupInfo, 1);
		newGroup->id = this->db->GetLastIdentity32();
		newGroup->groupType = groupTypeId;
		newGroup->engName = Text::String::New(engName);
		newGroup->chiName = Text::String::New(chiName);
		newGroup->descript = Text::String::New(descr);
		newGroup->parentId = parentId;
		newGroup->photoGroup = 0;
		newGroup->photoSpecies = 0;
		newGroup->idKey = 0;
		newGroup->cateId = cateId;
		newGroup->flags = flags;

		NEW_CLASS(newGroup->species, Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*>());
		NEW_CLASS(newGroup->groups, Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupInfo*>());
		newGroup->photoCount = (UOSInt)-1;
		newGroup->myPhotoCount = (UOSInt)-1;
		newGroup->totalCount = (UOSInt)-1;
		newGroup->photoSpObj = 0;
		this->groupMap->Put(newGroup->id, newGroup);
		group->groups->Add(newGroup);

		return newGroup->id;
	}
	return 0;
}

Bool SSWR::OrganMgr::OrganWebHandler::GroupModify(Int32 id, const UTF8Char *engName, const UTF8Char *chiName, const UTF8Char *descr, Int32 groupTypeId, GroupFlags flags)
{
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *group = this->groupMap->Get(id);
	if (group == 0)
		return false;
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"update groups set group_type = ");
	sql.AppendInt32(groupTypeId);
	sql.AppendCmd((const UTF8Char*)", eng_name = ");
	sql.AppendStrUTF8(engName);
	sql.AppendCmd((const UTF8Char*)", chi_name = ");
	sql.AppendStrUTF8(chiName);
	sql.AppendCmd((const UTF8Char*)", description = ");
	sql.AppendStrUTF8(descr);
	sql.AppendCmd((const UTF8Char*)", flags = ");
	sql.AppendInt32(flags);
	sql.AppendCmd((const UTF8Char*)" where id = ");
	sql.AppendInt32(id);
	if (this->db->ExecuteNonQuery(sql.ToString()) >= 0)
	{
		group->groupType = groupTypeId;
		SDEL_STRING(group->engName);
		group->engName = Text::String::New(engName);
		SDEL_STRING(group->chiName);
		group->chiName = Text::String::New(chiName);
		SDEL_STRING(group->descript);
		group->descript = Text::String::New(descr);
		group->flags = flags;
		return true;
	}
	return false;
}

Bool SSWR::OrganMgr::OrganWebHandler::GroupDelete(Int32 id)
{
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *group = this->groupMap->Get(id);
	if (group == 0)
		return false;
	if (group->groups->GetCount() > 0)
		return false;
	if (group->species->GetCount() > 0)
		return false;
	SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate = this->cateMap->Get(group->cateId);
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *parentGroup = this->groupMap->Get(group->parentId);
	if (parentGroup == 0)
		return false;
	if (cate == 0)
		return false;

	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"delete from groups where id = ");
	sql.AppendInt32(id);
	if (this->db->ExecuteNonQuery(sql.ToString()) == 1)
	{
		parentGroup->groups->Remove(group);
		cate->groups->Remove(group);
		this->groupMap->Remove(group->id);
		FreeGroup(group);
		return true;
	}
	return false;
}

Bool SSWR::OrganMgr::OrganWebHandler::GroupMove(Int32 groupId, Int32 destGroupId, Int32 cateId)
{
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *group = this->groupMap->Get(groupId);
	if (group == 0)
		return false;
	if (group->parentId == destGroupId)
	{
		return true;
	}
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *parentGroup = group;
	while (parentGroup)
	{
		if (parentGroup->id == destGroupId)
		{
			return false;
		}
		parentGroup = this->groupMap->Get(parentGroup->parentId);
	}
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"update groups set parent_id = ");
	sql.AppendInt32(destGroupId);
	sql.AppendCmd((const UTF8Char*)", cate_id = ");
	sql.AppendInt32(cateId);
	sql.AppendCmd((const UTF8Char*)" where id = ");
	sql.AppendInt32(groupId);
	if (this->db->ExecuteNonQuery(sql.ToString()) == 1)
	{
		parentGroup = this->groupMap->Get(group->parentId);
		if (parentGroup)
		{
			parentGroup->groups->Remove(group);
			if (parentGroup->photoGroup == group->id)
			{
				this->GroupSetPhotoGroup(parentGroup->id, 0);
			}
			if (parentGroup->groups->GetCount() == 0)
			{
				parentGroup->photoSpObj = 0;
			}
			if (group->myPhotoCount != (UOSInt)-1)
			{
				this->GroupAddCounts(parentGroup->id, -group->totalCount, -group->photoCount, -group->myPhotoCount);
			}
		}
		group->parentId = destGroupId;
		group->cateId = cateId;
		parentGroup = this->groupMap->Get(group->parentId);
		if (parentGroup)
		{
			parentGroup->groups->Add(group);
			if (group->myPhotoCount != (UOSInt)-1)
			{
				this->GroupAddCounts(parentGroup->id, group->totalCount, group->photoCount, group->myPhotoCount);
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::OrganMgr::OrganWebHandler::GroupAddCounts(Int32 groupId, UOSInt totalCount, UOSInt photoCount, UOSInt myPhotoCount)
{
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *group = this->groupMap->Get(groupId);
	if (group == 0)
		return false;
	if (group->myPhotoCount != (UOSInt)-1)
	{
		group->totalCount += totalCount;
		group->myPhotoCount += myPhotoCount;
		group->photoCount += photoCount;
		GroupAddCounts(group->parentId, totalCount, photoCount, myPhotoCount);
		return true;
	}
	return false;
}


Bool SSWR::OrganMgr::OrganWebHandler::GroupSetPhotoSpecies(Int32 groupId, Int32 photoSpeciesId)
{
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *group = this->groupMap->Get(groupId);
	if (group == 0)
		return false;
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *photoSpecies = this->spMap->Get(photoSpeciesId);
	if (photoSpeciesId != 0 && photoSpecies == 0)
		return false;

	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"update groups set photo_species = ");
	sql.AppendInt32(photoSpeciesId);
	sql.AppendCmd((const UTF8Char*)" where id = ");
	sql.AppendInt32(groupId);
	if (this->db->ExecuteNonQuery(sql.ToString()) >= 0)
	{
		group->photoSpecies = photoSpeciesId;
		if (photoSpecies == 0)
		{
			group->photoSpObj = 0;
			this->CalcGroupCount(group);
		}
		else
		{
			group->photoSpObj = photoSpecies;
		}
		return true;
	}
	return false;
}

Bool SSWR::OrganMgr::OrganWebHandler::GroupSetPhotoGroup(Int32 groupId, Int32 photoGroupId)
{
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *group = this->groupMap->Get(groupId);
	if (group == 0)
		return false;
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *photoGroup = this->groupMap->Get(photoGroupId);
	if (photoGroupId != 0 && photoGroup == 0)
		return false;

	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"update groups set photo_group = ");
	sql.AppendInt32(photoGroupId);
	sql.AppendCmd((const UTF8Char*)" where id = ");
	sql.AppendInt32(groupId);
	if (this->db->ExecuteNonQuery(sql.ToString()) >= 0)
	{
		group->photoGroup = photoGroupId;
		if (photoGroup == 0)
		{
			group->photoSpObj = 0;
			this->CalcGroupCount(group);
		}
		else
		{
			group->photoSpObj = photoGroup->photoSpObj;
		}
		return true;
	}
	return false;
}

Net::WebServer::IWebSession *SSWR::OrganMgr::OrganWebHandler::ParseRequestEnv(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, SSWR::OrganMgr::OrganWebHandler::RequestEnv *env, Bool keepSess)
{
	env->scnWidth = this->scnSize;
	env->isMobile = false;
	env->user = 0;
	Manage::OSInfo::OSType os = req->GetOS();
	if (os == Manage::OSInfo::OT_ANDROID || os == Manage::OSInfo::OT_IPAD || os == Manage::OSInfo::OT_IPHONE)
	{
		env->isMobile = true;
		env->scnWidth = 1024;
	}
	Net::WebServer::IWebSession *sess = this->sessMgr->GetSession(req, resp);
	if (sess)
	{
		Data::DateTime *t;
		env->user = (SSWR::OrganMgr::OrganWebHandler::WebUserInfo*)sess->GetValuePtr("User");
		env->pickObjType = (SSWR::OrganMgr::OrganWebHandler::PickObjType)sess->GetValueInt32("PickObjType");
		env->pickObjs = (Data::ArrayListInt32*)sess->GetValuePtr("PickObjs");
		t = (Data::DateTime*)sess->GetValuePtr("LastUseTime");
		t->SetCurrTimeUTC();
		if (keepSess)
		{
			return sess;
		}
		sess->EndUse();
	}
	return 0;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcPhoto(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	UTF8Char sbuff[512];
	Int32 cateId;
	UInt32 width;
	UInt32 height;
	Int32 spId;
	Int32 id;
	if (req->GetQueryValueI32((const UTF8Char*)"id", &spId) &&
		req->GetQueryValueI32((const UTF8Char*)"cateId", &cateId) &&
		req->GetQueryValueU32((const UTF8Char*)"width", &width) &&
		req->GetQueryValueU32((const UTF8Char*)"height", &height) &&
		spId > 0 && width > 0 && height > 0 && cateId > 0 && width <= 10000 && height <= 10000
		)
	{
		if (req->GetQueryValueI32((const UTF8Char*)"fileId", &id))
		{
			me->ResponsePhotoId(req, resp, env.user, env.isMobile, spId, cateId, width, height, id);
			return true;
		}
		else if (req->GetQueryValueI32((const UTF8Char*)"fileWId", &id))
		{
			me->ResponsePhotoWId(req, resp, env.user, env.isMobile, spId, cateId, width, height, id);
			return true;
		}
		else if (req->GetQueryValueStr((const UTF8Char*)"file", sbuff, 512))
		{
			me->ResponsePhoto(req, resp, env.user, env.isMobile, spId, cateId, width, height, sbuff);
			return true;
		}
	}
	resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcPhotoDown(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 spId;
	Int32 cateId;
	Int32 fileId;
	if (req->GetQueryValueI32((const UTF8Char*)"id", &spId) &&
		req->GetQueryValueI32((const UTF8Char*)"cateId", &cateId) &&
		req->GetQueryValueI32((const UTF8Char*)"fileId", &fileId))
	{
		SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
		UTF8Char u8buff[512];
		UTF8Char *u8ptr;
		SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile;
		me->dataMut->LockRead();
		sp = me->spMap->Get(spId);
		userFile = me->userFileMap->Get(fileId);
		if (sp && sp->cateId == cateId && env.user && userFile && userFile->webuserId == env.user->id)
		{
			Data::DateTime dt;
			dt.SetTicks(userFile->fileTimeTicks);
			dt.ToUTCTime();

			u8ptr = Text::StrConcat(u8buff, me->dataDir);
			if (u8ptr[-1] != IO::Path::PATH_SEPERATOR)
			{
				*u8ptr++ = IO::Path::PATH_SEPERATOR;
			}
			u8ptr = Text::StrConcat(u8ptr, (const UTF8Char*)"UserFile");
			*u8ptr++ = IO::Path::PATH_SEPERATOR;
			u8ptr = Text::StrInt32(u8ptr, userFile->webuserId);
			*u8ptr++ = IO::Path::PATH_SEPERATOR;
			u8ptr = dt.ToString(u8ptr, "yyyyMM");
			*u8ptr++ = IO::Path::PATH_SEPERATOR;
			if (userFile->fileType == 3)
			{
				u8ptr = Text::StrInt64(u8ptr, userFile->fileTimeTicks);
				u8ptr = Text::StrConcat(u8ptr, (const UTF8Char*)"_");
				u8ptr = Text::StrHexVal32(u8ptr, userFile->crcVal);
				u8ptr = Text::StrConcat(u8ptr, (const UTF8Char*)".png");
			}
			else
			{
				u8ptr = userFile->dataFileName->ConcatTo(u8ptr);
			}
			me->dataMut->UnlockRead();

			UInt8 *buff;
			UOSInt buffSize;
			IO::StmData::FileData *fd;
			NEW_CLASS(fd, IO::StmData::FileData(u8buff, false));
			
			buffSize = fd->GetDataSize();
			buff = MemAlloc(UInt8, buffSize);
			fd->GetRealData(0, buffSize, buff);
			resp->AddDefHeaders(req);
			resp->AddContentLength(buffSize);
			if (userFile->fileType == 3)
			{
				resp->AddContentType((const UTF8Char*)"image/png");
			}
			else
			{
				resp->AddContentType((const UTF8Char*)"image/jpeg");
			}
			resp->Write(buff, buffSize);
			DEL_CLASS(fd);
			return true;
		}
		else
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcGroup(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	Net::WebServer::WebSessionUsage webSess(me->ParseRequestEnv(req, resp, &env, true));

	Int32 id;
	Int32 cateId;
	if (req->GetQueryValueI32((const UTF8Char*)"id", &id) &&
		req->GetQueryValueI32((const UTF8Char*)"cateId", &cateId))
	{
		const UTF8Char *txt;
		IO::MemoryStream *mstm;
		IO::Writer *writer;
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
		SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
		IO::ConfigFile *lang = me->LangGet(req);
		me->dataMut->LockRead();
		group = me->groupMap->Get(id);
		Bool notAdmin = (env.user == 0 || env.user->userType != 0);
		if (group == 0 || group->cateId != cateId)
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->cateMap->Get(group->cateId);
		if (cate == 0 || ((cate->flags & 1) && notAdmin))
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		if (me->GroupIsAdmin(group) && notAdmin)
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		if (req->GetReqMethod() == Net::WebServer::IWebRequest::RequestMethod::HTTP_POST && env.user != 0 && env.user->userType == 0)
		{
			req->ParseHTTPForm();
			const UTF8Char *action = req->GetHTTPFormStr((const UTF8Char*)"action");
			const UTF8Char *csptr;
			Int32 itemId;
			if (action && Text::StrEquals(action, (const UTF8Char*)"pickall"))
			{
				if (group->groups->GetCount() > 0)
				{
					env.pickObjType = POT_GROUP;
					webSess.GetSess()->SetValueInt32("PickObjType", env.pickObjType);
					env.pickObjs->Clear();
					i = 0;
					j = group->groups->GetCount();
					while (i < j)
					{
						env.pickObjs->SortedInsert(group->groups->GetItem(i)->id);
						i++;
					}
				}
				else if (group->species->GetCount() > 0)
				{
					env.pickObjType = POT_SPECIES;
					webSess.GetSess()->SetValueInt32("PickObjType", env.pickObjType);
					env.pickObjs->Clear();
					i = 0;
					j = group->species->GetCount();
					while (i < j)
					{
						env.pickObjs->SortedInsert(group->species->GetItem(i)->speciesId);
						i++;
					}
				}
			}
			else if (action && Text::StrEquals(action, (const UTF8Char*)"picksel"))
			{
				if (group->groups->GetCount() > 0)
				{
					env.pickObjType = POT_GROUP;
					webSess.GetSess()->SetValueInt32("PickObjType", env.pickObjType);
					env.pickObjs->Clear();
					i = 0;
					j = group->groups->GetCount();
					while (i < j)
					{
						itemId = group->groups->GetItem(i)->id;
						sb.ClearStr();
						sb.Append((const UTF8Char*)"group");
						sb.AppendI32(itemId);
						csptr = req->GetHTTPFormStr(sb.ToString());
						if (csptr && csptr[0] == '1')
						{
							env.pickObjs->SortedInsert(itemId);
						}
						i++;
					}
				}
				else if (group->species->GetCount() > 0)
				{
					env.pickObjType = POT_SPECIES;
					webSess.GetSess()->SetValueInt32("PickObjType", env.pickObjType);
					env.pickObjs->Clear();
					i = 0;
					j = group->species->GetCount();
					while (i < j)
					{
						itemId = group->species->GetItem(i)->speciesId;
						sb.ClearStr();
						sb.Append((const UTF8Char*)"species");
						sb.AppendI32(itemId);
						csptr = req->GetHTTPFormStr(sb.ToString());
						if (csptr && csptr[0] == '1')
						{
							env.pickObjs->SortedInsert(itemId);
						}
						i++;
					}
				}
			}
			else if (action && Text::StrEquals(action, (const UTF8Char*)"place"))
			{
				if (env.pickObjType == POT_GROUP && group->species->GetCount() == 0)
				{
					i = 0;
					j = env.pickObjs->GetCount();
					while (i < j)
					{
						itemId = env.pickObjs->GetItem(i);
						sb.ClearStr();
						sb.Append((const UTF8Char*)"group");
						sb.AppendI32(itemId);
						csptr = req->GetHTTPFormStr(sb.ToString());
						if (csptr && csptr[0] == '1')
						{
							if (me->GroupMove(itemId, id, cateId))
							{
								env.pickObjs->RemoveAt(i);
								i--;
							}
						}
						i++;
					}
					if (env.pickObjs->GetCount() == 0)
					{
						env.pickObjType = POT_UNKNOWN;
						webSess.GetSess()->SetValueInt32("PickObjType", env.pickObjType);
					}
				}
				else if (env.pickObjType == POT_SPECIES && group->groups->GetCount() == 0)
				{
					i = 0;
					j = env.pickObjs->GetCount();
					while (i < j)
					{
						itemId = env.pickObjs->GetItem(i);
						sb.ClearStr();
						sb.Append((const UTF8Char*)"species");
						sb.AppendI32(itemId);
						csptr = req->GetHTTPFormStr(sb.ToString());
						if (csptr && csptr[0] == '1')
						{
							if (me->SpeciesMove(itemId, id, cateId))
							{
								env.pickObjs->RemoveAt(i);
								i--;
							}
						}
						i++;
					}
					if (env.pickObjs->GetCount() == 0)
					{
						env.pickObjType = POT_UNKNOWN;
						webSess.GetSess()->SetValueInt32("PickObjType", env.pickObjType);
					}
				}
			}
			else if (action && Text::StrEquals(action, (const UTF8Char*)"placeall"))
			{
				if (env.pickObjType == POT_GROUP && group->species->GetCount() == 0)
				{
					i = 0;
					j = env.pickObjs->GetCount();
					while (i < j)
					{
						itemId = env.pickObjs->GetItem(i);
						if (me->GroupMove(itemId, id, cateId))
						{
							env.pickObjs->RemoveAt(i);
							i--;
						}
						i++;
					}
					if (env.pickObjs->GetCount() == 0)
					{
						env.pickObjType = POT_UNKNOWN;
						webSess.GetSess()->SetValueInt32("PickObjType", env.pickObjType);
					}
				}
				else if (env.pickObjType == POT_SPECIES && group->groups->GetCount() == 0)
				{
					i = 0;
					j = env.pickObjs->GetCount();
					while (i < j)
					{
						itemId = env.pickObjs->GetItem(i);
						if (me->SpeciesMove(itemId, id, cateId))
						{
							env.pickObjs->RemoveAt(i);
							i--;
						}
						i++;
					}
					if (env.pickObjs->GetCount() == 0)
					{
						env.pickObjType = POT_UNKNOWN;
						webSess.GetSess()->SetValueInt32("PickObjType", env.pickObjType);
					}
				}
			}
			else if (action && Text::StrEquals(action, (const UTF8Char*)"setphoto"))
			{
				me->GroupSetPhotoGroup(group->parentId, group->id);
			}
		}

		NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.SvcGroup"));
		NEW_CLASS(writer, Text::UTF8Writer(mstm));

		sb.ClearStr();
		sb.Append(cate->chiName);
		sb.Append((const UTF8Char*)" - ");
		sb.Append(group->chiName);
		sb.Append((const UTF8Char*)" ");
		sb.Append(group->engName);
		me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
		writer->Write((const UTF8Char*)"<center><h1>");
		txt = Text::XML::ToNewHTMLText(sb.ToString());
		writer->Write(txt);
		Text::XML::FreeNewText(txt);
		writer->WriteLine((const UTF8Char*)"</h1></center>");

		writer->WriteLine((const UTF8Char*)"<table border=\"0\" width=\"100%\">");
		writer->WriteLine((const UTF8Char*)"<tr>");
		writer->Write((const UTF8Char*)"<td><form method=\"POST\" action=\"searchinside.html?id=");
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.Append((const UTF8Char*)"&amp;cateId=");
		sb.AppendI32(group->cateId);
		writer->Write(sb.ToString(), sb.GetCharCnt());
		writer->Write((const UTF8Char*)"\">");
		writer->Write((const UTF8Char*)"Search inside: <input type=\"text\" name=\"searchStr\"/><input type=\"submit\"/>");
		writer->WriteLine((const UTF8Char*)"</form></td>");
		writer->WriteLine((const UTF8Char*)"</tr>");
		writer->WriteLine((const UTF8Char*)"</table>");

		me->WriteLocator(writer, group, cate);
		writer->WriteLine((const UTF8Char*)"<br/>");
		if (group->descript)
		{
			txt = Text::XML::ToNewHTMLText(group->descript->v);
			writer->Write(txt);
			Text::XML::FreeNewText(txt);
		}
		if (!notAdmin)
		{
			writer->WriteLine((const UTF8Char*)"<br/>");
			if (group->groups->GetCount() == 0)
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<a href=\"speciesmod.html?id=");
				sb.AppendI32(id);
				sb.Append((const UTF8Char*)"&amp;cateId=");
				sb.AppendI32(cateId);
				sb.Append((const UTF8Char*)"\">New Species</a>");
				writer->WriteLine(sb.ToString());
			}
			if (group->species->GetCount() == 0)
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<a href=\"groupmod.html?id=");
				sb.AppendI32(id);
				sb.Append((const UTF8Char*)"&amp;cateId=");
				sb.AppendI32(cateId);
				sb.Append((const UTF8Char*)"\">New Group</a>");
				writer->WriteLine(sb.ToString());
			}
			writer->Write((const UTF8Char*)"<a href=\"groupmod.html?id=");
			sb.ClearStr();
			sb.AppendI32(group->parentId);
			sb.Append((const UTF8Char*)"&amp;cateId=");
			sb.AppendI32(group->cateId);
			sb.Append((const UTF8Char*)"&amp;groupId=");
			sb.AppendI32(group->id);
			writer->Write(sb.ToString(), sb.GetCharCnt());
			writer->Write((const UTF8Char*)"\">");
			writer->Write((const UTF8Char*)"Modify Group");
			writer->Write((const UTF8Char*)"</a>");
			writer->WriteLine((const UTF8Char*)"<br/>");
		}
		writer->WriteLine((const UTF8Char*)"<hr/>");

		Bool found = false;
		if (!notAdmin)
		{
			sb.ClearStr();
			sb.Append((const UTF8Char*)"<form name=\"groupform\" method=\"POST\" action=\"group.html?id=");
			sb.AppendI32(id);
			sb.Append((const UTF8Char*)"&amp;cateId=");
			sb.AppendI32(cateId);
			sb.Append((const UTF8Char*)"\">");
			writer->WriteLine(sb.ToString());
			writer->WriteLine((const UTF8Char*)"<input type=\"hidden\" name=\"action\"/>");
		}
		if (group->groups->GetCount())
		{
			SSWR::OrganMgr::OrganWebHandler::GroupInfo *sgroup;
			Data::StringUTF8Map<SSWR::OrganMgr::OrganWebHandler::GroupInfo*> groups;
			i = group->groups->GetCount();
			while (i-- > 0)
			{
				sgroup = group->groups->GetItem(i);
				if ((sgroup->flags & 1) == 0 || !notAdmin)
				{
					groups.Put(sgroup->engName->v, sgroup);
				}
			}
			if (groups.GetCount() > 0)
			{
				me->WriteGroupTable(writer, groups.GetValues(), env.scnWidth, !notAdmin);
				writer->WriteLine((const UTF8Char*)"<hr/>");
				found = true;
			}
		}
		if (group->species->GetCount())
		{
			SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
			Data::StringUTF8Map<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> species;
			i = group->species->GetCount();
			while (i-- > 0)
			{
				sp = group->species->GetItem(i);
				species.Put(sp->sciName->v, sp);
			}
			me->WriteSpeciesTable(writer, species.GetValues(), env.scnWidth, group->cateId, !notAdmin);
			writer->WriteLine((const UTF8Char*)"<hr/>");
			found = true;
		}
		if (!found)
		{
			writer->WriteLine(LangGetValue(lang, (const UTF8Char*)"NoItems"));
			writer->WriteLine((const UTF8Char*)"<hr/>");
		}
		if (!notAdmin)
		{
			writer->WriteLine((const UTF8Char*)"</form>");
		}
		if (env.user != 0 && env.user->userType == 0)
		{
			writer->WriteLine((const UTF8Char*)"Pick Objects:<br/>");
			writer->WriteLine((const UTF8Char*)"<input type=\"button\" value=\"Pick Selected\" onclick=\"document.forms.groupform.action.value='picksel';document.forms.groupform.submit();\"/>");
			writer->WriteLine((const UTF8Char*)"<input type=\"button\" value=\"Pick All\" onclick=\"document.forms.groupform.action.value='pickall';document.forms.groupform.submit();\"/>");
			writer->WriteLine((const UTF8Char*)"<hr/>");
			writer->WriteLine((const UTF8Char*)"Picked Objects:<br/>");
			sb.ClearStr();
			sb.Append((const UTF8Char*)"group.html?id=");
			sb.AppendI32(id);
			sb.Append((const UTF8Char*)"&cateId=");
			sb.AppendI32(cateId);
			me->WritePickObjs(writer, &env, sb.ToString());
		}

		sb.ClearStr();
		sb.Append((const UTF8Char*)"<a href=\"list.html?id=");
		sb.AppendI32(group->id);
		sb.Append((const UTF8Char*)"&amp;cateId=");
		sb.AppendI32(group->cateId);
		sb.Append((const UTF8Char*)"\">");
		sb.Append(LangGetValue(lang, (const UTF8Char*)"ListAll"));
		sb.Append((const UTF8Char*)"</a><br/>");
		writer->WriteLine(sb.ToString(), sb.GetCharCnt());

		sb.ClearStr();
		sb.Append((const UTF8Char*)"<a href=\"listimage.html?id=");
		sb.AppendI32(group->id);
		sb.Append((const UTF8Char*)"&amp;cateId=");
		sb.AppendI32(group->cateId);
		sb.Append((const UTF8Char*)"\">");
		sb.Append(LangGetValue(lang, (const UTF8Char*)"ListImage"));
		sb.Append((const UTF8Char*)"</a><br/>");
		writer->WriteLine(sb.ToString());

		if (group->parentId == 0)
		{
			writer->Write((const UTF8Char*)"<a href=");
			sb.ClearStr();
			sb.Append((const UTF8Char*)"cate.html?cateName=");
			sb.Append(cate->dirName);
			txt = Text::XML::ToNewAttrText(sb.ToString());
			writer->Write(txt);
			Text::XML::FreeNewText(txt);
			writer->Write((const UTF8Char*)">");
			writer->Write(LangGetValue(lang, (const UTF8Char*)"Back"));
			writer->Write((const UTF8Char*)"</a>");
		}
		else
		{
			writer->Write((const UTF8Char*)"<a href=\"group.html?id=");
			sb.ClearStr();
			sb.AppendI32(group->parentId);
			sb.Append((const UTF8Char*)"&amp;cateId=");
			sb.AppendI32(group->cateId);
			writer->Write(sb.ToString(), sb.GetCharCnt());
			writer->Write((const UTF8Char*)"\">");
			writer->Write(LangGetValue(lang, (const UTF8Char*)"Back"));
			writer->Write((const UTF8Char*)"</a>");
		}
		if (env.user != 0 && env.user->userType == 0)
		{
			writer->WriteLine((const UTF8Char*)"<br/>");

			writer->WriteLine((const UTF8Char*)"<input type=\"button\" value=\"Set Parent Photo\" onclick=\"document.forms.groupform.action.value='setphoto';document.forms.groupform.submit();\"/>");
		}


		me->WriteFooter(writer);
		me->dataMut->UnlockRead();
		ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

		DEL_CLASS(writer);
		DEL_CLASS(mstm);
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}


Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcGroupMod(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	IO::MemoryStream *mstm;
	Text::UTF8Writer *writer;
	UOSInt i;
	UOSInt j;
	me->ParseRequestEnv(req, resp, &env, false);

	if (env.user == 0 || env.user->userType != 0)
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}

	Int32 id;
	Int32 cateId;
	Int32 groupId = 0;
	if (req->GetQueryValueI32((const UTF8Char*)"id", &id) &&
		req->GetQueryValueI32((const UTF8Char*)"cateId", &cateId))
	{
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
		SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
		Text::StringBuilderUTF8 sb;
		const UTF8Char *txt;
		IO::ConfigFile *lang = me->LangGet(req);

		me->dataMut->LockRead();
		group = me->groupMap->Get(id);
		if (group == 0)
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->cateMap->Get(cateId);
		if (cate == 0)
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		Text::StringBuilderUTF8 msg;
		const UTF8Char *ename = 0;
		const UTF8Char *cname = 0;
		const UTF8Char *descr = 0;
		GroupFlags groupFlags = GF_NONE;
		Int32 groupTypeId = 0;
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *modGroup = 0;
		if (req->GetQueryValueI32((const UTF8Char*)"groupId", &groupId))
		{
			modGroup = me->groupMap->Get(groupId);
			if (modGroup)
			{
				cname = modGroup->chiName->v;
				ename = modGroup->engName->v;
				descr = modGroup->descript->v;
				groupTypeId = modGroup->groupType;
			}
		}
		if (req->GetReqMethod() == Net::WebServer::IWebRequest::RequestMethod::HTTP_POST)
		{
			req->ParseHTTPForm();
			const UTF8Char *task = req->GetHTTPFormStr((const UTF8Char*)"task");
			cname = req->GetHTTPFormStr((const UTF8Char*)"cname");
			ename = req->GetHTTPFormStr((const UTF8Char*)"ename");
			descr = req->GetHTTPFormStr((const UTF8Char*)"descr");
			txt = req->GetHTTPFormStr((const UTF8Char*)"adminOnly");
			if (txt && txt[0] == '1')
			{
				groupFlags = (GroupFlags)(groupFlags | GF_ADMIN_ONLY);
			}
			if (task != 0 && cname != 0 && req->GetHTTPFormInt32((const UTF8Char*)"groupType", &groupTypeId) && ename != 0 && descr != 0 && ename[0] != 0 && cname[0] != 0)
			{
				if (Text::StrEquals(task, (const UTF8Char*)"new"))
				{
					sb.ClearStr();
					Bool found = false;
					i = group->groups->GetCount();
					while (i-- > 0)
					{
						if (group->groups->GetItem(i)->engName->Equals(ename))
						{
							found = true;
							break;
						}
					}
					if (found)
					{
						msg.Append((const UTF8Char*)"Group already exist");
					}
					else
					{
						me->dataMut->UnlockRead();
						me->dataMut->LockWrite();
						Int32 newGroupId = me->GroupAdd(ename, cname, id, descr, groupTypeId, cateId, groupFlags);
						if (newGroupId)
						{
							me->dataMut->UnlockWrite();
							sb.ClearStr();
							sb.Append((const UTF8Char*)"group.html?id=");
							sb.AppendI32(newGroupId);
							sb.Append((const UTF8Char*)"&cateId=");
							sb.AppendI32(cateId);

							resp->RedirectURL(req, sb.ToString(), 0);
							return true;
						}
						else
						{
							msg.Append((const UTF8Char*)"Error in adding group");
						}
						me->dataMut->UnlockWrite();
						me->dataMut->LockRead();
					}
				}
				else if (Text::StrEquals(task, (const UTF8Char*)"modify") && modGroup != 0 && modGroup->cateId == cateId)
				{
					Bool found = false;
					i = group->groups->GetCount();
					while (i-- > 0)
					{
						if (group->groups->GetItem(i) != modGroup && group->groups->GetItem(i)->engName->Equals(ename))
						{
							found = true;
							break;
						}
					}
					if (found)
					{
						msg.Append((const UTF8Char*)"Group name already exist");
					}
					else
					{
						me->dataMut->UnlockRead();
						me->dataMut->LockWrite();
						if (me->GroupModify(modGroup->id, ename, cname, descr, groupTypeId, groupFlags))
						{
							me->dataMut->UnlockWrite();
							sb.ClearStr();
							sb.Append((const UTF8Char*)"group.html?id=");
							sb.AppendI32(modGroup->id);
							sb.Append((const UTF8Char*)"&cateId=");
							sb.AppendI32(modGroup->cateId);

							resp->RedirectURL(req, sb.ToString(), 0);
							return true;
						}
						else
						{
							msg.Append((const UTF8Char*)"Error in modifying group");
						}
						me->dataMut->UnlockWrite();
						me->dataMut->LockRead();
					}
				}
				else if (Text::StrEquals(task, (const UTF8Char*)"delete") && modGroup != 0 && modGroup->groups->GetCount() == 0 && modGroup->species->GetCount() == 0)
				{
					Int32 id = modGroup->id;
					Int32 cateId = modGroup->cateId;
					me->dataMut->UnlockRead();
					me->dataMut->LockWrite();
					if (me->GroupDelete(modGroup->id))
					{
						me->dataMut->UnlockWrite();
						sb.ClearStr();
						sb.Append((const UTF8Char*)"group.html?id=");
						sb.AppendI32(id);
						sb.Append((const UTF8Char*)"&cateId=");
						sb.AppendI32(cateId);

						resp->RedirectURL(req, sb.ToString(), 0);
						return true;
					}
					else
					{
						msg.Append((const UTF8Char*)"Error in deleting group");
					}
					me->dataMut->UnlockWrite();
					me->dataMut->LockRead();
				}
			}
		}

		NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.SvcGroupMod"));
		NEW_CLASS(writer, Text::UTF8Writer(mstm));

		sb.ClearStr();
		sb.Append((const UTF8Char*)"New group in ");
		sb.Append(group->chiName);
		sb.Append((const UTF8Char*)" ");
		sb.Append(group->engName);
		me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
		writer->Write((const UTF8Char*)"<center><h1>");
		txt = Text::XML::ToNewHTMLText(sb.ToString());
		writer->Write(txt);
		Text::XML::FreeNewText(txt);
		writer->WriteLine((const UTF8Char*)"</h1></center>");

		writer->Write((const UTF8Char*)"<form name=\"newgroup\" method=\"POST\" action=\"groupmod.html?id=");
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.Append((const UTF8Char*)"&amp;cateId=");
		sb.AppendI32(group->cateId);
		if (groupId != 0)
		{
			sb.Append((const UTF8Char*)"&amp;groupId=");
			sb.AppendI32(groupId);
		}
		writer->Write(sb.ToString());
		writer->WriteLine((const UTF8Char*)"\">");
		writer->WriteLine((const UTF8Char*)"<input type=\"hidden\" name=\"task\"/>");
		writer->WriteLine((const UTF8Char*)"<table border=\"0\">");
		writer->WriteLine((const UTF8Char*)"<tr><td>Category</td><td><select name=\"groupType\">");
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupTypeInfo*> *groupTypes = cate->groupTypes->GetValues();
		SSWR::OrganMgr::OrganWebHandler::GroupTypeInfo *groupType;
		i = 0;
		j = groupTypes->GetCount();
		while (i < j)
		{
			groupType = groupTypes->GetItem(i);
			sb.ClearStr();
			sb.Append((const UTF8Char*)"<option value=\"");
			sb.AppendI32(groupType->id);
			sb.Append((const UTF8Char*)"\"");
			if (groupType->id == groupTypeId)
			{
				sb.Append((const UTF8Char*)" selected");
			}
			sb.Append((const UTF8Char*)">");
			txt = Text::XML::ToNewHTMLText(groupType->chiName);
			sb.Append(txt);
			Text::XML::FreeNewText(txt);
			sb.Append((const UTF8Char*)"</option>");
			writer->WriteLine(sb.ToString());
			i++;
		}
		writer->WriteLine((const UTF8Char*)"</select></td></tr>");
		writer->Write((const UTF8Char*)"<tr><td>English Name</td><td><input type=\"text\" name=\"ename\"");
		if (ename)
		{
			writer->Write((const UTF8Char*)" value=");
			txt = Text::XML::ToNewAttrText(ename);
			writer->Write(txt);
			Text::XML::FreeNewText(txt);
		}
		writer->WriteLine((const UTF8Char*)"/></td></tr>");
		writer->Write((const UTF8Char*)"<tr><td>Chinese Name</td><td><input type=\"text\" name=\"cname\"");
		if (cname)
		{
			writer->Write((const UTF8Char*)" value=");
			txt = Text::XML::ToNewAttrText(cname);
			writer->Write(txt);
			Text::XML::FreeNewText(txt);
		}
		writer->WriteLine((const UTF8Char*)"/></td></tr>");
		writer->Write((const UTF8Char*)"<tr><td>Flags</td><td><input type=\"checkbox\" id=\"adminOnly\" name=\"adminOnly\" value=\"1\"");
		if (groupFlags & GF_ADMIN_ONLY)
		{
			writer->Write((const UTF8Char*)" checked");
		}
		writer->WriteLine((const UTF8Char*)"/><label for=\"adminOnly\">Admin Only</label></td></tr>");
		writer->Write((const UTF8Char*)"<tr><td>Description</td><td><textarea name=\"descr\" rows=\"4\" cols=\"40\">");
		if (descr)
		{
			txt = Text::XML::ToNewHTMLText(descr);
			writer->Write(txt);
			Text::XML::FreeNewText(txt);
		}
		writer->WriteLine((const UTF8Char*)"</textarea></td></tr>");
		writer->Write((const UTF8Char*)"<tr><td></td><td>");
		if (groupId != 0)
		{
			writer->Write((const UTF8Char*)"<input type=\"button\" value=\"Modify\" onclick=\"document.forms.newgroup.task.value='modify';document.forms.newgroup.submit();\"/>");
		}
		writer->Write((const UTF8Char*)"<input type=\"button\" value=\"New\" onclick=\"document.forms.newgroup.task.value='new';document.forms.newgroup.submit();\"/>");
		if (group->species->GetCount() == 0 && group->groups->GetCount() == 0)
		{
			writer->Write((const UTF8Char*)"<input type=\"button\" value=\"Delete\" onclick=\"document.forms.newgroup.task.value='delete';document.forms.newgroup.submit();\"/>");
		}
		writer->Write((const UTF8Char*)"<a href=\"group.html?id=");
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.Append((const UTF8Char*)"&amp;cateId=");
		sb.AppendI32(group->cateId);
		writer->Write(sb.ToString());
		writer->Write((const UTF8Char*)"\">");
		writer->Write(LangGetValue(lang, (const UTF8Char*)"Back"));
		writer->Write((const UTF8Char*)"</a>");
		writer->WriteLine((const UTF8Char*)"</td></tr>");
		writer->WriteLine((const UTF8Char*)"</table></form>");
		me->WriteFooter(writer);
		me->dataMut->UnlockRead();
		ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

		DEL_CLASS(writer);
		DEL_CLASS(mstm);
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcSpecies(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	Net::WebServer::WebSessionUsage webSess(me->ParseRequestEnv(req, resp, &env, true));

	Int32 id;
	Int32 cateId;
	if (req->GetQueryValueI32((const UTF8Char*)"id", &id) &&
		req->GetQueryValueI32((const UTF8Char*)"cateId", &cateId))
	{
		const UTF8Char *txt;
		IO::MemoryStream *mstm;
		IO::Writer *writer;
		UOSInt i;
		UOSInt j;
		UTF8Char sbuff[512];
		UTF8Char sbuff2[512];
		UTF8Char *sptr;
		UTF8Char *sptr2;
		IO::Path::FindFileSession *sess;
		IO::Path::PathType pt;
		Text::StringBuilderUTF8 sb;
		SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species;
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
		SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
		SSWR::OrganMgr::OrganWebHandler::BookSpInfo *bookSp;
		SSWR::OrganMgr::OrganWebHandler::BookInfo *book;
		SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::WebFileInfo*> *wfiles;
		SSWR::OrganMgr::OrganWebHandler::WebFileInfo *wfile;
		IO::ConfigFile *lang = me->LangGet(req);
		Data::DateTime dt;

		me->dataMut->LockRead();
		species = me->spMap->Get(id);
		if (species == 0)
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		Bool notAdmin = (env.user == 0 || env.user->userType != 0);
		group = me->groupMap->Get(species->groupId);
		if (group == 0 || group->cateId != cateId || (me->GroupIsAdmin(group) && notAdmin))
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->cateMap->Get(group->cateId);
		if (cate == 0 || ((cate->flags & 1) && notAdmin))
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		if (req->GetReqMethod() == Net::WebServer::IWebRequest::RequestMethod::HTTP_POST && env.user != 0 && env.user->userType == 0)
		{
			req->ParseHTTPForm();
			const UTF8Char *action = req->GetHTTPFormStr((const UTF8Char*)"action");
			const UTF8Char *csptr;
			Int32 userfileId;
			if (action && Text::StrEquals(action, (const UTF8Char*)"pickall"))
			{
				env.pickObjType = POT_USERFILE;
				webSess.GetSess()->SetValueInt32("PickObjType", env.pickObjType);
				env.pickObjs->Clear();
				i = 0;
				j = species->files->GetCount();
				while (i < j)
				{
					env.pickObjs->SortedInsert(species->files->GetItem(i)->id);
					i++;
				}
			}
			else if (action && Text::StrEquals(action, (const UTF8Char*)"picksel"))
			{
				env.pickObjType = POT_USERFILE;
				webSess.GetSess()->SetValueInt32("PickObjType", env.pickObjType);
				env.pickObjs->Clear();
				i = 0;
				j = species->files->GetCount();
				while (i < j)
				{
					userfileId = species->files->GetItem(i)->id;
					sb.ClearStr();
					sb.Append((const UTF8Char*)"userfile");
					sb.AppendI32(userfileId);
					csptr = req->GetHTTPFormStr(sb.ToString());
					if (csptr && csptr[0] == '1')
					{
						env.pickObjs->SortedInsert(userfileId);
					}
					i++;
				}
			}
			else if (action && Text::StrEquals(action, (const UTF8Char*)"place"))
			{
				if (env.pickObjType == POT_USERFILE)
				{
					i = 0;
					j = env.pickObjs->GetCount();
					while (i < j)
					{
						userfileId = env.pickObjs->GetItem(i);
						sb.ClearStr();
						sb.Append((const UTF8Char*)"userfile");
						sb.AppendI32(userfileId);
						csptr = req->GetHTTPFormStr(sb.ToString());
						if (csptr && csptr[0] == '1')
						{
							if (me->UserfileMove(userfileId, id, cateId))
							{
								env.pickObjs->RemoveAt(i);
								i--;
							}
						}
						i++;
					}
					if (env.pickObjs->GetCount() == 0)
					{
						env.pickObjType = POT_UNKNOWN;
						webSess.GetSess()->SetValueInt32("PickObjType", env.pickObjType);
					}
				}
			}
			else if (action && Text::StrEquals(action, (const UTF8Char*)"placeall"))
			{
				if (env.pickObjType == POT_USERFILE)
				{
					i = 0;
					j = env.pickObjs->GetCount();
					while (i < j)
					{
						userfileId = env.pickObjs->GetItem(i);
						if (me->UserfileMove(userfileId, id, cateId))
						{
							env.pickObjs->RemoveAt(i);
							i--;
						}
						i++;
					}
					if (env.pickObjs->GetCount() == 0)
					{
						env.pickObjType = POT_UNKNOWN;
						webSess.GetSess()->SetValueInt32("PickObjType", env.pickObjType);
					}
				}
			}
		}

		NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.SvcSpecies"));
		NEW_CLASS(writer, Text::UTF8Writer(mstm));

		sb.ClearStr();
		sb.Append(cate->chiName);
		sb.Append((const UTF8Char*)" - ");
		sb.AppendC(species->sciName->v, species->sciName->leng);
		sb.Append((const UTF8Char*)" ");
		sb.AppendC(species->chiName->v, species->chiName->leng);
		sb.Append((const UTF8Char*)" ");
		sb.AppendC(species->engName->v, species->engName->leng);
		me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
		writer->Write((const UTF8Char*)"<center><h1>");
		txt = Text::XML::ToNewHTMLText(sb.ToString());
		writer->Write(txt);
		Text::XML::FreeNewText(txt);
		writer->WriteLine((const UTF8Char*)"</h1></center>");

		writer->WriteLine((const UTF8Char*)"<table border=\"0\"><tr><td>");
		if (species->descript)
		{
			txt = Text::XML::ToNewHTMLText(species->descript->v);
			writer->Write(txt);
			Text::XML::FreeNewText(txt);
		}
		if (species->files->GetCount() > 0)
		{
			Bool months[12];
			i = 12;
			while (i-- > 0)
			{
				months[i] = false;
			}
			i = species->files->GetCount();
			while (i-- > 0)
			{
				userFile = species->files->GetItem(i);
				if (userFile->captureTimeTicks)
				{
					dt.SetTicks(userFile->captureTimeTicks);
					months[dt.GetMonth() - 1] = true;
				}
			}
			writer->WriteLine((const UTF8Char*)"<br/><br/>Months");
			writer->WriteLine((const UTF8Char*)"<table><tr>");
			i = 0;
			while (i < 12)
			{
				if (months[i])
				{
					writer->Write((const UTF8Char*)"<td bgcolor=\"#4060ff\">");
				}
				else
				{
					writer->Write((const UTF8Char*)"<td>");
				}
				Text::StrUOSInt(sbuff, i + 1);
				writer->Write(sbuff);
				writer->Write((const UTF8Char*)"</td>");
				i++;
			}
			writer->WriteLine((const UTF8Char*)"</tr></table>");
		}
		if (species->books->GetCount() > 0)
		{
			Data::DateTime dt;
			writer->WriteLine((const UTF8Char*)"<br/><br/>");
			writer->Write(LangGetValue(lang, (const UTF8Char*)"Document"));
			writer->WriteLine((const UTF8Char*)":<br/>");
			writer->WriteLine((const UTF8Char*)"<table border=\"0\">");
			i = 0;
			j = species->books->GetCount();
			while (i < j)
			{
				bookSp = species->books->GetItem(i);
				book = me->bookMap->Get(bookSp->bookId);
				if (book != 0)
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)"<tr><td><a href=\"book.html?id=");
					sb.AppendI32(book->id);
					sb.Append((const UTF8Char*)"&cateId=");
					sb.AppendI32(cate->cateId);
					sb.Append((const UTF8Char*)"\" title=");
					txt = Text::XML::ToNewAttrText(book->title->v);
					sb.Append(txt);
					Text::XML::FreeNewText(txt);
					sb.Append((const UTF8Char*)"><i>");
					writer->Write(sb.ToString());
					txt = Text::XML::ToNewHTMLText(bookSp->dispName->v);
					writer->Write(txt);
					Text::XML::FreeNewText(txt);
					writer->Write((const UTF8Char*)"</i></a></td><td>");
					sb.ClearStr();
					sb.Append(book->author);
					sb.Append((const UTF8Char*)" (");
					dt.SetTicks(book->publishDate);
					sb.AppendU32(dt.GetYear());
					sb.Append((const UTF8Char*)")");
					txt = Text::XML::ToNewHTMLText(sb.ToString());
					writer->Write(txt);
					Text::XML::FreeNewText(txt);
					writer->WriteLine((const UTF8Char*)"</td></tr>");
				}
				i++;
			}
			writer->WriteLine((const UTF8Char*)"</table>");
		}
		writer->WriteLine((const UTF8Char*)"</td><td>");
		me->WriteLocator(writer, group, cate);
		writer->WriteLine((const UTF8Char*)"</td></tr></table>");
		writer->WriteLine((const UTF8Char*)"<hr/>");

		Data::ArrayListICaseStrUTF8 *fileNameList;
		Data::ArrayListString *refURLList;
		NEW_CLASS(fileNameList, Data::ArrayListICaseStrUTF8());
		NEW_CLASS(refURLList, Data::ArrayListString());
		sptr = Text::StrConcat(sbuff, cate->srcDir);
		if (IO::Path::PATH_SEPERATOR != '\\')
		{
			Text::StrReplace(sbuff, '\\', IO::Path::PATH_SEPERATOR);
		}
		sptr = species->dirName->ConcatTo(sptr);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		Text::StrConcat(sptr, IO::Path::ALL_FILES);
		sess = IO::Path::FindFile(sbuff);
		if (sess)
		{
			while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
			{
				if (pt == IO::Path::PathType::File)
				{
					if (Text::StrEndsWithICase(sptr, (const UTF8Char*)".JPG") || Text::StrEndsWithICase(sptr, (const UTF8Char*)".PCX") || Text::StrEndsWith(sptr, (const UTF8Char*)".TIF"))
					{
						sptr2[-4] = 0;
						fileNameList->SortedInsert(Text::StrCopyNew(sptr));
					}
				}
			}
			IO::Path::FindFileClose(sess);
		}
		Text::StrConcat(sptr, (const UTF8Char*)"web.txt");
		if (IO::Path::GetPathType(sbuff) == IO::Path::PathType::File)
		{
			Text::UTF8Reader *reader;
			IO::FileStream *fs;
			UTF8Char *sarr[4];
			NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
			NEW_CLASS(reader, Text::UTF8Reader(fs));
			sb.ClearStr();
			while (reader->ReadLine(&sb, 4096))
			{
				if (Text::StrSplit(sarr, 4, sb.ToString(), '\t') == 3)
				{
					if (refURLList->SortedIndexOfPtr(sarr[2]) < 0)
					{
						refURLList->SortedInsert(Text::String::New(sarr[2]));
					}
					sptr2 = Text::StrConcat(sptr, (const UTF8Char*)"web");
					*sptr2++ = IO::Path::PATH_SEPERATOR;
					sptr2 = Text::StrConcat(sptr2, sarr[0]);
					i = Text::StrLastIndexOf(sptr, '.');
					if (i != INVALID_INDEX)
					{
						sptr[i] = 0;
					}
					fileNameList->Add(Text::StrCopyNew(sptr));
				}
				sb.ClearStr();
			}
			DEL_CLASS(reader);
			DEL_CLASS(fs);
		}

		UInt32 colCount = env.scnWidth / PREVIEW_SIZE;
		UInt32 colWidth = 100 / colCount;
		UInt32 currColumn;
		if (fileNameList->GetCount() > 0 || species->files->GetCount() > 0 || species->wfiles->GetCount() > 0)
		{
			currColumn = 0;
			if (env.user != 0 && env.user->userType == 0)
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<form name=\"userfiles\" action=\"species.html?id=");
				sb.AppendI32(id);
				sb.Append((const UTF8Char*)"&amp;cateId=");
				sb.AppendI32(cateId);
				sb.Append((const UTF8Char*)"\" method=\"POST\"/>");
				writer->WriteLine(sb.ToString());
				writer->WriteLine((const UTF8Char*)"<input type=\"hidden\" name=\"action\"/>");
			}
			writer->WriteLine((const UTF8Char*)"<table border=\"0\" width=\"100%\">");

			i = 0;
			j = species->files->GetCount();
			while (i < j)
			{
				userFile = species->files->GetItem(i);
				if (currColumn == 0)
				{
					writer->WriteLine((const UTF8Char*)"<tr>");
				}
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<td width=\"");
				sb.AppendU32(colWidth);
				sb.Append((const UTF8Char*)"%\">");
				writer->WriteLine(sb.ToString());
				sb.ClearStr();
				sb.Append((const UTF8Char*)"photodetail.html?id=");
				sb.AppendI32(species->speciesId);
				sb.Append((const UTF8Char*)"&cateId=");
				sb.AppendI32(species->cateId);
				sb.Append((const UTF8Char*)"&fileId=");
				sb.AppendI32(userFile->id);
				txt = Text::XML::ToNewAttrText(sb.ToString());
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<center><a href=");
				sb.Append(txt);
				sb.Append((const UTF8Char*)">");
				writer->WriteLine(sb.ToString());
				Text::XML::FreeNewText(txt);

				writer->Write((const UTF8Char*)"<img src=");
				sb.ClearStr();
				sb.Append((const UTF8Char*)"photo.html?id=");
				sb.AppendI32(species->speciesId);
				sb.Append((const UTF8Char*)"&cateId=");
				sb.AppendI32(species->cateId);
				sb.Append((const UTF8Char*)"&width=");
				sb.AppendI32(PREVIEW_SIZE);
				sb.Append((const UTF8Char*)"&height=");
				sb.AppendI32(PREVIEW_SIZE);
				sb.Append((const UTF8Char*)"&fileId=");
				sb.AppendI32(userFile->id);
				txt = Text::XML::ToNewAttrText(sb.ToString());
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				writer->Write((const UTF8Char*)" border=\"0\">");
				writer->Write((const UTF8Char*)"</a><br/>");
				dt.SetTicks(userFile->fileTimeTicks);
				dt.ToLocalTime();
				if (env.user != 0 && (env.user->userType == 0 || userFile->webuserId == env.user->id))
				{
					if (env.user->userType == 0)
					{
						sb.ClearStr();
						sb.Append((const UTF8Char*)"<input type=\"checkbox\" id=\"userfile");
						sb.AppendI32(userFile->id);
						sb.Append((const UTF8Char*)"\" name=\"userfile");
						sb.AppendI32(userFile->id);
						sb.Append((const UTF8Char*)"\" value=\"1\"/><label for=\"userfile");
						sb.AppendI32(userFile->id);
						sb.Append((const UTF8Char*)"\">");
						writer->Write(sb.ToString());
					}
					dt.ToString(sbuff2, "yyyy-MM-dd HH:mm:ss zzzz");
					writer->Write(sbuff2);
					if (userFile->webuserId == env.user->id)
					{
						if (userFile->location)
						{
							writer->Write((const UTF8Char*)" ");
							txt = Text::XML::ToNewHTMLText(userFile->location->v);
							writer->Write(txt);
							Text::XML::FreeNewText(txt);
						}
	/*					Data::Int64Map<SSWR::OrganMgr::OrganWebHandler::TripInfo*> *tripCate = user->tripCates->Get(species->cateId);
						if (tripCate)
						{
							OSInt ind = tripCate->GetIndex(userFile->captureTime);
							if (ind < 0)
							{
								ind = ~ind - 1;
							}
							SSWR::OrganMgr::OrganWebHandler::TripInfo *trip = tripCate->GetValues()->GetItem(ind);
							if (trip != 0 && trip->fromDate <= userFile->captureTime && trip->toDate > userFile->captureTime)
							{
								SSWR::OrganMgr::OrganWebHandler::LocationInfo *loc = this->locMap->Get(trip->locId);
								if (loc)
								{
									writer->Write((const UTF8Char*)" ");
									txt = Text::XML::ToNewHTMLText(loc->cname);
									writer->Write(txt);
									Text::XML::FreeNewText(txt);
								}
							}
						}*/
					}
					if (userFile->descript && userFile->descript->leng > 0)
					{
						writer->Write((const UTF8Char*)"<br/>");
						txt = Text::XML::ToNewHTMLText(userFile->descript->v);
						writer->Write(txt);
						Text::XML::FreeNewText(txt);
					}
					if (userFile->webuserId == env.user->id)
					{
						writer->Write((const UTF8Char*)"<br/>");
						txt = Text::XML::ToNewHTMLText(userFile->oriFileName->v);
						writer->Write(txt);
						Text::XML::FreeNewText(txt);
					}
					if (env.user->userType == 0)
					{
						writer->Write((const UTF8Char*)"</label>");
					}
					if (userFile->lat != 0 || userFile->lon != 0)
					{
						sb.ClearStr();
						sb.Append((const UTF8Char*)"<br/>");
						sb.Append((const UTF8Char*)"<a href=\"https://www.google.com/maps/place/");
						Text::SBAppendF64(&sb, userFile->lat);
						sb.Append((const UTF8Char*)",");
						Text::SBAppendF64(&sb, userFile->lon);
						sb.Append((const UTF8Char*)"/@");
						Text::SBAppendF64(&sb, userFile->lat);
						sb.Append((const UTF8Char*)",");
						Text::SBAppendF64(&sb, userFile->lon);
						sb.Append((const UTF8Char*)",19z\">");
						sb.Append((const UTF8Char*)"Google Map</a>");
						writer->Write(sb.ToString());
					}
				}
				else
				{
					dt.ToString(sbuff2, "yyyy-MM-dd zzzz");
					writer->Write(sbuff2);
				}
				writer->WriteLine((const UTF8Char*)"</center></td>");

				currColumn++;
				if (currColumn >= colCount)
				{
					writer->WriteLine((const UTF8Char*)"</tr>");
					currColumn = 0;
				}
				i++;
			}

			wfiles = species->wfiles->GetValues();
			i = 0;
			j = wfiles->GetCount();
			while (i < j)
			{
				wfile = wfiles->GetItem(i);
				if (currColumn == 0)
				{
					writer->WriteLine((const UTF8Char*)"<tr>");
				}
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<td width=\"");
				sb.AppendU32(colWidth);
				sb.Append((const UTF8Char*)"%\">");
				writer->WriteLine(sb.ToString());
				sb.ClearStr();
				sb.Append((const UTF8Char*)"photodetail.html?id=");
				sb.AppendI32(species->speciesId);
				sb.Append((const UTF8Char*)"&cateId=");
				sb.AppendI32(species->cateId);
				sb.Append((const UTF8Char*)"&fileWId=");
				sb.AppendI32(wfile->id);
				txt = Text::XML::ToNewAttrText(sb.ToString());
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<center><a href=");
				sb.Append(txt);
				sb.Append((const UTF8Char*)">");
				writer->WriteLine(sb.ToString());
				Text::XML::FreeNewText(txt);

				writer->Write((const UTF8Char*)"<img src=");
				sb.ClearStr();
				sb.Append((const UTF8Char*)"photo.html?id=");
				sb.AppendI32(species->speciesId);
				sb.Append((const UTF8Char*)"&cateId=");
				sb.AppendI32(species->cateId);
				sb.Append((const UTF8Char*)"&width=");
				sb.AppendI32(PREVIEW_SIZE);
				sb.Append((const UTF8Char*)"&height=");
				sb.AppendI32(PREVIEW_SIZE);
				sb.Append((const UTF8Char*)"&fileWId=");
				sb.AppendI32(wfile->id);
				txt = Text::XML::ToNewAttrText(sb.ToString());
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				writer->Write((const UTF8Char*)" border=\"0\">");
				writer->Write((const UTF8Char*)"</a>");
				if (env.user && env.user->userType == 0)
				{
					if (wfile->location && wfile->location->leng > 0)
					{
						writer->Write((const UTF8Char*)"<br/>");
						txt = Text::XML::ToNewHTMLText(wfile->location->v);
						writer->Write(txt);
						Text::XML::FreeNewText(txt);
					}
				}
				writer->WriteLine((const UTF8Char*)"</center></td>");

				currColumn++;
				if (currColumn >= colCount)
				{
					writer->WriteLine((const UTF8Char*)"</tr>");
					currColumn = 0;
				}

				if (refURLList->SortedIndexOf(wfile->srcUrl) < 0)
				{
					refURLList->SortedInsert(wfile->srcUrl->Clone());
				}

				i++;
			}

			i = 0;
			j = fileNameList->GetCount();
			while (i < j)
			{
				Text::TextEnc::URIEncoding::URIEncode(sbuff2, fileNameList->GetItem(i));
				if (currColumn == 0)
				{
					writer->WriteLine((const UTF8Char*)"<tr>");
				}
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<td width=\"");
				sb.AppendU32(colWidth);
				sb.Append((const UTF8Char*)"%\">");
				writer->WriteLine(sb.ToString());
				sb.ClearStr();
				sb.Append((const UTF8Char*)"photodetail.html?id=");
				sb.AppendI32(species->speciesId);
				sb.Append((const UTF8Char*)"&cateId=");
				sb.AppendI32(species->cateId);
				sb.Append((const UTF8Char*)"&file=");
				sb.Append(sbuff2);
				txt = Text::XML::ToNewAttrText(sb.ToString());
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<center><a href=");
				sb.Append(txt);
				sb.Append((const UTF8Char*)">");
				writer->WriteLine(sb.ToString());
				Text::XML::FreeNewText(txt);

				writer->Write((const UTF8Char*)"<img src=");
				sb.ClearStr();
				sb.Append((const UTF8Char*)"photo.html?id=");
				sb.AppendI32(species->speciesId);
				sb.Append((const UTF8Char*)"&cateId=");
				sb.AppendI32(species->cateId);
				sb.Append((const UTF8Char*)"&width=");
				sb.AppendI32(PREVIEW_SIZE);
				sb.Append((const UTF8Char*)"&height=");
				sb.AppendI32(PREVIEW_SIZE);
				sb.Append((const UTF8Char*)"&file=");
				sb.Append(sbuff2);
				txt = Text::XML::ToNewAttrText(sb.ToString());
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				writer->Write((const UTF8Char*)" border=\"0\">");
				writer->WriteLine((const UTF8Char*)"</a></center></td>");

				currColumn++;
				if (currColumn >= colCount)
				{
					writer->WriteLine((const UTF8Char*)"</tr>");
					currColumn = 0;
				}

				Text::StrDelNew(fileNameList->GetItem(i));
				i++;
			}

			if (currColumn != 0)
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<td width=\"");
				sb.AppendU32(colWidth);
				sb.Append((const UTF8Char*)"%\"></td>");
				while (currColumn < colCount)
				{
					writer->WriteLine(sb.ToString());
					currColumn++;
				}
				writer->WriteLine((const UTF8Char*)"</tr>");
			}
			writer->WriteLine((const UTF8Char*)"</table><hr/>");
			if (env.user != 0 && env.user->userType == 0)
			{
				writer->WriteLine((const UTF8Char*)"</form>");
			}
		}
		DEL_CLASS(fileNameList);
		if (refURLList->GetCount() > 0)
		{
			writer->WriteLine((const UTF8Char*)"Reference URL:<br/>");
			Text::String *url;
			i = 0;
			j = refURLList->GetCount();
			while (i < j)
			{
				url = refURLList->GetItem(i);

				writer->Write((const UTF8Char*)"<a href=");
				txt = Text::XML::ToNewAttrText(url->v);
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				writer->Write((const UTF8Char*)">");
				txt = Text::XML::ToNewHTMLText(url->v);
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				writer->WriteLine((const UTF8Char*)"</a><br/>");

				url->Release();
				i++;
			}
			writer->WriteLine((const UTF8Char*)"<hr/>");
		}
		DEL_CLASS(refURLList);
		if (env.user != 0 && env.user->userType == 0)
		{
			writer->WriteLine((const UTF8Char*)"Pick Objects:<br/>");
			writer->WriteLine((const UTF8Char*)"<input type=\"button\" value=\"Pick Selected\" onclick=\"document.forms.userfiles.action.value='picksel';document.forms.userfiles.submit();\"/>");
			writer->WriteLine((const UTF8Char*)"<input type=\"button\" value=\"Pick All\" onclick=\"document.forms.userfiles.action.value='pickall';document.forms.userfiles.submit();\"/>");
			writer->WriteLine((const UTF8Char*)"<hr/>");
			writer->WriteLine((const UTF8Char*)"Picked Objects:<br/>");
			sb.ClearStr();
			sb.Append((const UTF8Char*)"species.html?id=");
			sb.AppendI32(id);
			sb.Append((const UTF8Char*)"&cateId=");
			sb.AppendI32(cateId);
			me->WritePickObjs(writer, &env, sb.ToString());
		}

		writer->Write((const UTF8Char*)"<a href=\"group.html?id=");
		sb.ClearStr();
		sb.AppendI32(species->groupId);
		sb.Append((const UTF8Char*)"&amp;cateId=");
		sb.AppendI32(group->cateId);
		writer->Write(sb.ToString());
		writer->Write((const UTF8Char*)"\">");
		writer->Write(LangGetValue(lang, (const UTF8Char*)"Back"));
		writer->Write((const UTF8Char*)"</a>");

		me->WriteFooter(writer);
		me->dataMut->UnlockRead();
		ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

		DEL_CLASS(writer);
		DEL_CLASS(mstm);
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcSpeciesMod(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	IO::MemoryStream *mstm;
	Text::UTF8Writer *writer;
	me->ParseRequestEnv(req, resp, &env, false);

	if (env.user == 0 || env.user->userType != 0)
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}

	Int32 id;
	Int32 cateId;
	Int32 spId = 0;
	if (req->GetQueryValueI32((const UTF8Char*)"id", &id) &&
		req->GetQueryValueI32((const UTF8Char*)"cateId", &cateId))
	{
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
		Text::StringBuilderUTF8 sb;
		const UTF8Char *txt;
		IO::ConfigFile *lang = me->LangGet(req);

		me->dataMut->LockRead();
		group = me->groupMap->Get(id);
		if (group == 0)
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		Text::StringBuilderUTF8 msg;
		const UTF8Char *cname = 0;
		const UTF8Char *sname = 0;
		const UTF8Char *ename = 0;
		const UTF8Char *descr = 0;
		const UTF8Char *bookIgn = 0;
		SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species = 0;
		if (req->GetQueryValueI32((const UTF8Char*)"spId", &spId))
		{
			species = me->spMap->Get(spId);
			if (species)
			{
				cname = species->chiName->v;
				sname = species->sciName->v;
				ename = species->engName->v;
				descr = species->descript->v;
			}
		}
		if (req->GetReqMethod() == Net::WebServer::IWebRequest::RequestMethod::HTTP_POST)
		{
			req->ParseHTTPForm();
			const UTF8Char *task = req->GetHTTPFormStr((const UTF8Char*)"task");
			cname = req->GetHTTPFormStr((const UTF8Char*)"cname");
			sname = req->GetHTTPFormStr((const UTF8Char*)"sname");
			ename = req->GetHTTPFormStr((const UTF8Char*)"ename");
			descr = req->GetHTTPFormStr((const UTF8Char*)"descr");
			bookIgn = req->GetHTTPFormStr((const UTF8Char*)"bookIgn");
			if (task != 0 && cname != 0 && sname != 0 && ename != 0 && descr != 0 && cname[0] != 0 && sname[0] != 0)
			{
				if (Text::StrEquals(task, (const UTF8Char*)"new"))
				{
					sb.ClearStr();
					if (me->spNameMap->Get(sname) != 0)
					{
						msg.Append((const UTF8Char*)"Species already exist");
					}
					else if ((bookIgn == 0 || bookIgn[0] != '1') && me->SpeciesBookIsExist(sname, &sb))
					{
						msg.Append((const UTF8Char *)"Species already exist in book: ");
						msg.Append(sb.ToString());
						msg.Append((const UTF8Char*)", continue?");
						bookIgn = (const UTF8Char*)"1";
					}
					else
					{
						me->dataMut->UnlockRead();
						me->dataMut->LockWrite();
						sb.ClearStr();
						sb.Append(sname);
						sb.ToLower();
						sb.Replace((const UTF8Char*)" ", (const UTF8Char*)"_");
						sb.Replace((const UTF8Char*)".", (const UTF8Char*)"");
						Int32 spId = me->SpeciesAdd(ename, cname, sname, id, descr, sb.ToString(), (const UTF8Char*)"", cateId);
						if (spId)
						{
							me->dataMut->UnlockWrite();
							sb.ClearStr();
							sb.Append((const UTF8Char*)"species.html?id=");
							sb.AppendI32(spId);
							sb.Append((const UTF8Char*)"&cateId=");
							sb.AppendI32(cateId);

							resp->RedirectURL(req, sb.ToString(), 0);
							return true;
						}
						else
						{
							msg.Append((const UTF8Char*)"Error in adding species");
						}
						me->dataMut->UnlockWrite();
						me->dataMut->LockRead();
					}
				}
				else if (Text::StrEquals(task, (const UTF8Char*)"modify") && species != 0)
				{
					Bool nameChg = !species->sciName->Equals(sname);
					sb.ClearStr();
					if (nameChg && me->spNameMap->Get(sname) != 0)
					{
						msg.Append((const UTF8Char*)"Species already exist");
					}
					else if (nameChg && (bookIgn == 0 || bookIgn[0] != '1') && me->SpeciesBookIsExist(sname, &sb))
					{
						msg.Append((const UTF8Char *)"Species already exist in book: ");
						msg.Append(sb.ToString());
						msg.Append((const UTF8Char*)", continue?");
						bookIgn = (const UTF8Char*)"1";
					}
					else
					{
						me->dataMut->UnlockRead();
						me->dataMut->LockWrite();
						sb.ClearStr();
						sb.Append(sname);
						sb.ToLower();
						sb.Replace((const UTF8Char*)" ", (const UTF8Char*)"_");
						sb.Replace((const UTF8Char*)".", (const UTF8Char*)"");
						if (me->SpeciesModify(spId, ename, cname, sname, descr, sb.ToString()))
						{
							me->dataMut->UnlockWrite();
							sb.ClearStr();
							sb.Append((const UTF8Char*)"species.html?id=");
							sb.AppendI32(spId);
							sb.Append((const UTF8Char*)"&cateId=");
							sb.AppendI32(cateId);

							resp->RedirectURL(req, sb.ToString(), 0);
							return true;
						}
						else
						{
							msg.Append((const UTF8Char*)"Error in modifying species");
						}
						me->dataMut->UnlockWrite();
						me->dataMut->LockRead();
					}
				}
			}
		}

		NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.SvcSpeciesMod"));
		NEW_CLASS(writer, Text::UTF8Writer(mstm));

		sb.ClearStr();
		sb.Append((const UTF8Char*)"New Species in ");
		sb.Append(group->chiName);
		sb.Append((const UTF8Char*)" ");
		sb.Append(group->engName);
		me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
		writer->Write((const UTF8Char*)"<center><h1>");
		txt = Text::XML::ToNewHTMLText(sb.ToString());
		writer->Write(txt);
		Text::XML::FreeNewText(txt);
		writer->WriteLine((const UTF8Char*)"</h1></center>");

		writer->Write((const UTF8Char*)"<form name=\"newspecies\" method=\"POST\" action=\"speciesmod.html?id=");
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.Append((const UTF8Char*)"&amp;cateId=");
		sb.AppendI32(group->cateId);
		if (spId != 0)
		{
			sb.Append((const UTF8Char*)"&amp;spId=");
			sb.AppendI32(spId);
		}
		if (bookIgn && bookIgn[0] == '1')
		{
			sb.Append((const UTF8Char*)"&amp;bookIgn=1");
		}
		writer->Write(sb.ToString());
		writer->WriteLine((const UTF8Char*)"\">");
		writer->WriteLine((const UTF8Char*)"<input type=\"hidden\" name=\"task\"/>");
		writer->WriteLine((const UTF8Char*)"<table border=\"0\">");
		writer->Write((const UTF8Char*)"<tr><td>Chinese Name</td><td><input type=\"text\" name=\"cname\"");
		if (cname)
		{
			writer->Write((const UTF8Char*)" value=");
			txt = Text::XML::ToNewAttrText(cname);
			writer->Write(txt);
			Text::XML::FreeNewText(txt);
		}
		writer->WriteLine((const UTF8Char*)"/></td></tr>");
		writer->Write((const UTF8Char*)"<tr><td>Science Name</td><td><input type=\"text\" name=\"sname\"");
		if (sname)
		{
			writer->Write((const UTF8Char*)" value=");
			txt = Text::XML::ToNewAttrText(sname);
			writer->Write(txt);
			Text::XML::FreeNewText(txt);
		}
		writer->WriteLine((const UTF8Char*)"/></td></tr>");
		writer->Write((const UTF8Char*)"<tr><td>English Name</td><td><input type=\"text\" name=\"ename\"");
		if (ename)
		{
			writer->Write((const UTF8Char*)" value=");
			txt = Text::XML::ToNewAttrText(ename);
			writer->Write(txt);
			Text::XML::FreeNewText(txt);
		}
		writer->WriteLine((const UTF8Char*)"/></td></tr>");
		writer->Write((const UTF8Char*)"<tr><td>Description</td><td><textarea name=\"descr\" rows=\"4\" cols=\"40\">");
		if (descr)
		{
			txt = Text::XML::ToNewHTMLText(descr);
			writer->Write(txt);
			Text::XML::FreeNewText(txt);
		}
		writer->WriteLine((const UTF8Char*)"</textarea></td></tr>");
		writer->Write((const UTF8Char*)"<tr><td></td><td>");
		if (spId != 0)
		{
			writer->Write((const UTF8Char*)"<input type=\"button\" value=\"Modify\" onclick=\"document.forms.newspecies.task.value='modify';document.forms.newspecies.submit();\"/>");
		}
		writer->Write((const UTF8Char*)"<input type=\"button\" value=\"New\" onclick=\"document.forms.newspecies.task.value='new';document.forms.newspecies.submit();\"/>");

		writer->Write((const UTF8Char*)"<a href=\"group.html?id=");
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.Append((const UTF8Char*)"&amp;cateId=");
		sb.AppendI32(group->cateId);
		writer->Write(sb.ToString());
		writer->Write((const UTF8Char*)"\">");
		writer->Write(LangGetValue(lang, (const UTF8Char*)"Back"));
		writer->Write((const UTF8Char*)"</a>");
		writer->WriteLine((const UTF8Char*)"</td></tr>");
		writer->WriteLine((const UTF8Char*)"</table></form>");
		me->WriteFooter(writer);
		me->dataMut->UnlockRead();
		ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

		DEL_CLASS(writer);
		DEL_CLASS(mstm);
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcList(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 id;
	UInt32 page = 0;
	Int32 cateId;
	req->GetQueryValueU32((const UTF8Char*)"page", &page);
	if (req->GetQueryValueI32((const UTF8Char*)"id", &id) &&
		req->GetQueryValueI32((const UTF8Char*)"cateId", &cateId))
	{
		Bool imageOnly = Text::StrEquals(subReq, (const UTF8Char*)"/listimage.html");
		const UTF8Char *txt;
		IO::MemoryStream *mstm;
		IO::Writer *writer;
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
		SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
		IO::ConfigFile *lang = me->LangGet(req);
		me->dataMut->LockRead();
		group = me->groupMap->Get(id);
		Bool notAdmin = (env.user == 0 || env.user->userType != 0);
		if (group == 0 || group->cateId != cateId)
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->cateMap->Get(group->cateId);
		if (cate == 0 || ((cate->flags & 1) && notAdmin))
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		if (me->GroupIsAdmin(group) && notAdmin)
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.SvcList"));
		NEW_CLASS(writer, Text::UTF8Writer(mstm));

		sb.ClearStr();
		sb.Append(cate->chiName);
		sb.Append((const UTF8Char*)" - ");
		sb.Append(group->chiName);
		sb.Append((const UTF8Char*)" ");
		sb.Append(group->engName);
		me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
		writer->Write((const UTF8Char*)"<center><h1>");
		txt = Text::XML::ToNewHTMLText(sb.ToString());
		writer->Write(txt);
		Text::XML::FreeNewText(txt);
		writer->WriteLine((const UTF8Char*)"</h1></center>");

		me->WriteLocator(writer, group, cate);
		writer->WriteLine((const UTF8Char*)"<br/>");
		if (group->descript)
		{
			txt = Text::XML::ToNewHTMLText(group->descript->v);
			writer->Write(txt);
			Text::XML::FreeNewText(txt);
		}
		writer->WriteLine((const UTF8Char*)"<br/>");
		writer->WriteLine((const UTF8Char*)"<hr/>");

		Data::StringUTF8Map<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> spMap;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> *spList;
		me->GetGroupSpecies(group, &spMap, env.user);
		spList = spMap.GetValues();
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> speciesTmp;
		if (imageOnly)
		{
			SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
			i = 0;
			j = spList->GetCount();
			while (i < j)
			{
				sp = spList->GetItem(i);
				if ((sp->flags & 9) != 0)
				{
					speciesTmp.Add(sp);
				}
				i++;
			}
			spList = &speciesTmp;
		}
		UOSInt perPage;
		if (env.isMobile)
		{
			perPage = SP_PER_PAGE_MOBILE;
		}
		else
		{
			perPage = SP_PER_PAGE_DESKTOP;
		}
		i = page * perPage;
		j = i + perPage;
		if (j > spList->GetCount())
		{
			j = spList->GetCount();
		}
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> species;
		while (i < j)
		{
			species.Add(spList->GetItem(i));
			i++;
		}
		me->WriteSpeciesTable(writer, &species, env.scnWidth, group->cateId, false);
		writer->WriteLine((const UTF8Char*)"<hr/>");

		if (imageOnly)
		{
			if (page > 0)
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<a href=\"listimage.html?id=");
				sb.AppendI32(id);
				sb.Append((const UTF8Char*)"&amp;cateId=");
				sb.AppendI32(group->cateId);
				sb.Append((const UTF8Char*)"&amp;page=");
				sb.AppendU32(page - 1);
				sb.Append((const UTF8Char*)"\">&lt;");
				sb.Append(LangGetValue(lang, (const UTF8Char*)"Previous"));
				sb.AppendUOSInt(perPage);
				sb.Append(LangGetValue(lang, (const UTF8Char*)"Items"));
				sb.Append((const UTF8Char*)"</a>");
				writer->WriteLine(sb.ToString());
			}
			if ((page + 1) * perPage < spList->GetCount())
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<a href=\"listimage.html?id=");
				sb.AppendI32(id);
				sb.Append((const UTF8Char*)"&amp;cateId=");
				sb.AppendI32(group->cateId);
				sb.Append((const UTF8Char*)"&amp;page=");
				sb.AppendU32(page + 1);
				sb.Append((const UTF8Char*)"\">");
				sb.Append(LangGetValue(lang, (const UTF8Char*)"Next"));
				sb.AppendUOSInt(perPage);
				sb.Append(LangGetValue(lang, (const UTF8Char*)"Items"));
				sb.Append((const UTF8Char*)"&gt;</a>");
				writer->WriteLine(sb.ToString());
			}
		}
		else
		{
			if (page > 0)
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<a href=\"list.html?id=");
				sb.AppendI32(id);
				sb.Append((const UTF8Char*)"&cateId=");
				sb.AppendI32(group->cateId);
				sb.Append((const UTF8Char*)"&page=");
				sb.AppendU32(page - 1);
				sb.Append((const UTF8Char*)"\">&lt;");
				sb.Append(LangGetValue(lang, (const UTF8Char*)"Previous"));
				sb.AppendUOSInt(perPage);
				sb.Append(LangGetValue(lang, (const UTF8Char*)"Items"));
				sb.Append((const UTF8Char*)"</a>");
				writer->WriteLine(sb.ToString());
			}
			if ((page + 1) * perPage < spMap.GetCount())
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<a href=\"list.html?id=");
				sb.AppendI32(id);
				sb.Append((const UTF8Char*)"&cateId=");
				sb.AppendI32(group->cateId);
				sb.Append((const UTF8Char*)"&page=");
				sb.AppendU32(page + 1);
				sb.Append((const UTF8Char*)"\">");
				sb.Append(LangGetValue(lang, (const UTF8Char*)"Next"));
				sb.AppendUOSInt(perPage);
				sb.Append(LangGetValue(lang, (const UTF8Char*)"Items"));
				sb.Append((const UTF8Char*)"&gt;</a>");
				writer->WriteLine(sb.ToString());
			}
		}
		writer->WriteLine((const UTF8Char*)"<br/>");
		writer->WriteLine((const UTF8Char*)"<br/>");
		writer->Write((const UTF8Char*)"<a href=\"group.html?id=");
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.Append((const UTF8Char*)"&cateId=");
		sb.AppendI32(group->cateId);
		writer->Write(sb.ToString());
		writer->Write((const UTF8Char*)"\">");
		writer->Write(LangGetValue(lang, (const UTF8Char*)"Back"));
		writer->Write((const UTF8Char*)"</a>");

		me->WriteFooter(writer);
		me->dataMut->UnlockRead();
		ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

		DEL_CLASS(writer);
		DEL_CLASS(mstm);
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcPhotoDetail(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 id;
	Int32 cateId;
	if (req->GetQueryValueI32((const UTF8Char*)"id", &id) &&
		req->GetQueryValueI32((const UTF8Char*)"cateId", &cateId))
	{
		UTF8Char fileName[512];
		Int32 fileId;
		const UTF8Char *txt;
		IO::MemoryStream *mstm;
		IO::Writer *writer;
		UOSInt i;
		UOSInt j;
		UTF8Char u8buff[512];
		UTF8Char u8buff2[512];
		UTF8Char *u8ptr;
		UTF8Char *u8ptr2;
		IO::Path::FindFileSession *sess;
		IO::Path::PathType pt;
		Text::StringBuilderUTF8 sb;
		SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species;
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
		SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
		Text::UTF8Reader *reader;
		IO::FileStream *fs;
		UTF8Char *sarr[4];
		SSWR::OrganMgr::OrganWebHandler::WebFileInfo *wfile;
		IO::ConfigFile *lang = me->LangGet(req);

		me->dataMut->LockRead();
		species = me->spMap->Get(id);
		if (species == 0 || species->cateId != cateId)
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		group = me->groupMap->Get(species->groupId);
		if (group == 0)
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->cateMap->Get(group->cateId);
		if (cate == 0)
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		u8ptr = Text::StrConcat(u8buff, cate->srcDir);
		if (IO::Path::PATH_SEPERATOR != '\\')
		{
			Text::StrReplace(u8buff, '\\', IO::Path::PATH_SEPERATOR);
		}
		u8ptr = species->dirName->ConcatTo(u8ptr);
		*u8ptr++ = IO::Path::PATH_SEPERATOR;

		if (req->GetQueryValueI32((const UTF8Char*)"fileId", &fileId))
		{
			Data::ArrayListICaseStrUTF8 *fileNameList;
			Bool found = false;
			SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile;
			i = 0;
			j = species->files->GetCount();
			while (i < j)
			{
				userFile = species->files->GetItem(i);
				if (userFile->id == fileId)
				{
					found = true;
					break;
				}
				i++;
			}
			if (found)
			{
				if (req->GetReqMethod() == Net::WebServer::IWebRequest::RequestMethod::HTTP_POST && env.user && (env.user->userType == 0 || env.user->id == userFile->webuserId))
				{
					req->ParseHTTPForm();
					const UTF8Char *action = req->GetHTTPFormStr((const UTF8Char*)"action");
					if (action && Text::StrEquals(action, (const UTF8Char*)"setdefault") && env.user->userType == 0)
					{
						me->SpeciesSetPhotoId(id, fileId);
					}
					else if (action && Text::StrEquals(action, (const UTF8Char*)"setname"))
					{
						const UTF8Char *desc = req->GetHTTPFormStr((const UTF8Char*)"descr");
						if (desc)
						{
							me->UserfileUpdateDesc(fileId, desc);
						}
					}
					else if (action && Text::StrEquals(action, (const UTF8Char*)"rotate"))
					{
						me->UserfileUpdateRotType(fileId, (userFile->rotType + 1) & 3);
					}
				}

				NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.SvcPhotoDetail"));
				NEW_CLASS(writer, Text::UTF8Writer(mstm));

				sb.ClearStr();
				sb.Append(cate->chiName);
				sb.Append((const UTF8Char*)" - ");
				sb.Append(species->sciName);
				sb.Append((const UTF8Char*)" ");
				sb.Append(species->chiName);
				sb.Append((const UTF8Char*)" ");
				sb.Append(species->engName);
				me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
				writer->Write((const UTF8Char*)"<center><h1>");
				txt = Text::XML::ToNewHTMLText(sb.ToString());
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				writer->WriteLine((const UTF8Char*)"</h1></center>");

				writer->WriteLine((const UTF8Char*)"<center>");
				writer->WriteLine((const UTF8Char*)"<table border=\"0\" width=\"100%\">");
				writer->WriteLine((const UTF8Char*)"<tr><td align=\"center\">");

				if (i < j - 1)
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)"photodetail.html?id=");
					sb.AppendI32(species->speciesId);
					sb.Append((const UTF8Char*)"&cateId=");
					sb.AppendI32(species->cateId);
					sb.Append((const UTF8Char*)"&fileId=");
					sb.AppendI32(species->files->GetItem(i + 1)->id);
				}
				else if (species->wfiles->GetCount() != 0)
				{
					wfile = species->wfiles->GetValues()->GetItem(0);
					sb.ClearStr();
					sb.Append((const UTF8Char*)"photodetail.html?id=");
					sb.AppendI32(species->speciesId);
					sb.Append((const UTF8Char*)"&cateId=");
					sb.AppendI32(species->cateId);
					sb.Append((const UTF8Char*)"&fileWId=");
					sb.AppendI32(wfile->id);
				}
				else
				{
					NEW_CLASS(fileNameList, Data::ArrayListICaseStrUTF8());

					Text::StrConcat(u8ptr, IO::Path::ALL_FILES);
					sess = IO::Path::FindFile(u8buff);
					if (sess)
					{

						while ((u8ptr2 = IO::Path::FindNextFile(u8ptr, sess, 0, &pt, 0)) != 0)
						{
							if (pt == IO::Path::PathType::File)
							{
								if (Text::StrEndsWithICase(u8ptr, (const UTF8Char*)".JPG") || Text::StrEndsWithICase(u8ptr, (const UTF8Char*)".PCX") || Text::StrEndsWith(u8ptr, (const UTF8Char*)".TIF"))
								{
									u8ptr2[-4] = 0;
									fileNameList->SortedInsert(Text::StrCopyNew(u8ptr));
								}
							}
						}
						IO::Path::FindFileClose(sess);
					}
					if (fileNameList->GetCount() > 0)
					{
						sb.ClearStr();
						sb.Append((const UTF8Char*)"photodetail.html?id=");
						sb.AppendI32(species->speciesId);
						sb.Append((const UTF8Char*)"&cateId=");
						sb.AppendI32(species->cateId);
						sb.Append((const UTF8Char*)"&file=");
						Text::TextEnc::URIEncoding::URIEncode(u8buff2, fileNameList->GetItem(0));
						sb.Append(u8buff2);
					}
					else
					{
						Bool found;
						Text::StrConcat(u8ptr, (const UTF8Char*)"web.txt");
						NEW_CLASS(fs, IO::FileStream(u8buff, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
						NEW_CLASS(reader, Text::UTF8Reader(fs));
						sb.ClearStr();
						found = false;
						while (reader->ReadLine(&sb, 4096))
						{
							if (Text::StrSplit(sarr, 4, sb.ToString(), '\t') == 3)
							{
								found = true;
								u8ptr2 = Text::StrConcat(u8buff2, (const UTF8Char*)"web");
								*u8ptr2++ = IO::Path::PATH_SEPERATOR;
								Text::StrConcat(u8ptr2, sarr[0]);
								i = Text::StrLastIndexOf(u8buff2, '.');
								if (i != INVALID_INDEX)
									u8buff2[i] = 0;
								break;
							}
							sb.ClearStr();
						}
						DEL_CLASS(reader);
						DEL_CLASS(fs);

						if (found)
						{
							sb.ClearStr();
							sb.Append(u8buff2);
							Text::TextEnc::URIEncoding::URIEncode(u8buff2, sb.ToString());
							sb.ClearStr();
							sb.Append((const UTF8Char*)"photodetail.html?id=");
							sb.AppendI32(species->speciesId);
							sb.Append((const UTF8Char*)"&cateId=");
							sb.AppendI32(species->cateId);
							sb.Append((const UTF8Char*)"&file=");
							sb.Append(u8buff2);
						}
						else
						{
							sb.ClearStr();
							sb.Append((const UTF8Char*)"species.html?id=");
							sb.AppendI32(species->speciesId);
							sb.Append((const UTF8Char*)"&cateId=");
							sb.AppendI32(species->cateId);
						}
					}
					i = fileNameList->GetCount();
					while (i-- > 0)
					{
						Text::StrDelNew(fileNameList->GetItem(i));
					}
					DEL_CLASS(fileNameList);
				}
				txt = Text::XML::ToNewAttrText(sb.ToString());
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<a href=");
				sb.Append(txt);
				Text::XML::FreeNewText(txt);
				sb.Append((const UTF8Char*)">");
				writer->WriteLine(sb.ToString());

				sb.ClearStr();
				sb.Append((const UTF8Char*)"photo.html?id=");
				sb.AppendI32(species->speciesId);
				sb.Append((const UTF8Char*)"&cateId=");
				sb.AppendI32(species->cateId);
				sb.Append((const UTF8Char*)"&width=");
				sb.AppendU32(env.scnWidth);
				sb.Append((const UTF8Char*)"&height=");
				sb.AppendU32(env.scnWidth);
				sb.Append((const UTF8Char*)"&fileId=");
				sb.AppendI32(fileId);
				txt = Text::XML::ToNewAttrText(sb.ToString());
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<img src=");
				sb.Append(txt);
				Text::XML::FreeNewText(txt);
				sb.Append((const UTF8Char*)" border=\"0\"></a>");
				writer->WriteLine(sb.ToString());
				writer->WriteLine((const UTF8Char*)"</td></tr>");
				writer->WriteLine((const UTF8Char*)"<tr><td align=\"center\">");


				if (userFile->fileType == 3)
				{
					Data::DateTime dt;
					IO::StmData::FileData *fd;
					UInt64 fileSize = 0;
					Media::MediaFile *mediaFile;
					u8ptr = Text::StrConcat(u8buff, me->dataDir);
					if (u8ptr[-1] != IO::Path::PATH_SEPERATOR)
					{
						*u8ptr++ = IO::Path::PATH_SEPERATOR;
					}
					u8ptr = Text::StrConcat(u8ptr, (const UTF8Char*)"UserFile");
					*u8ptr++ = IO::Path::PATH_SEPERATOR;
					u8ptr = Text::StrInt32(u8ptr, userFile->webuserId);
					*u8ptr++ = IO::Path::PATH_SEPERATOR;
					dt.SetTicks(userFile->fileTimeTicks);
					u8ptr = dt.ToString(u8ptr, "yyyyMM");
					*u8ptr++ = IO::Path::PATH_SEPERATOR;
					u8ptr = userFile->dataFileName->ConcatTo(u8ptr);
					Sync::MutexUsage mutUsage(me->parserMut);
					NEW_CLASS(fd, IO::StmData::FileData(u8buff, false));
					fileSize = fd->GetDataSize();
					mediaFile = (Media::MediaFile*)me->parsers->ParseFileType(fd, IO::ParserType::MediaFile);
					DEL_CLASS(fd);
					mutUsage.EndUse();

					if (mediaFile)
					{
						sb.ClearStr();
						sb.AppendU64(fileSize);
						sb.Append((const UTF8Char*)" bytes");
						Media::IMediaSource *msrc = mediaFile->GetStream(0, 0);
						Int32 stmTime;
						if (msrc)
						{
							stmTime = msrc->GetStreamTime();
							sb.Append((const UTF8Char*)", Length: ");
							sb.AppendI32(stmTime / 60000);
							sb.Append((const UTF8Char*)":");
							stmTime = stmTime % 60000;
							if (stmTime < 10000)
							{
								sb.Append((const UTF8Char*)"0");
							}
							Text::SBAppendF64(&sb, stmTime * 0.001);

							if (msrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
							{
								Media::IAudioSource *asrc = (Media::IAudioSource*)msrc;
								Media::AudioFormat format;
								asrc->GetFormat(&format);
								sb.Append((const UTF8Char*)" ");
								sb.AppendU32(format.frequency);
								sb.Append((const UTF8Char*)"Hz, ");
								sb.AppendU32(format.bitpersample);
								sb.Append((const UTF8Char*)"bits, ");
								sb.AppendU32(format.nChannels);
								sb.Append((const UTF8Char*)" ch");
							}
						}
						writer->Write((const UTF8Char*)"<b>");
						writer->Write(LangGetValue(lang, (const UTF8Char*)"PhotoSpec"));
						writer->Write((const UTF8Char*)"</b> ");
						writer->Write(sb.ToString());
						writer->Write((const UTF8Char*)"<br/>");
						DEL_CLASS(mediaFile);
					}
					if (userFile->captureTimeTicks != 0)
					{
						dt.SetTicks(userFile->captureTimeTicks);
						dt.ToLocalTime();
						writer->Write((const UTF8Char*)"<b>");
						writer->Write(LangGetValue(lang, (const UTF8Char*)"PhotoDate"));
						writer->Write((const UTF8Char*)"</b> ");
						dt.ToString(u8buff2, "yyyy-MM-dd HH:mm:ss zzzz");
						writer->Write(u8buff2);
						writer->Write((const UTF8Char*)"<br/>");
					}
				}
				else
				{
					Data::DateTime dt;
					u8ptr = Text::StrConcat(u8buff, me->dataDir);
					if (u8ptr[-1] != IO::Path::PATH_SEPERATOR)
					{
						*u8ptr++ = IO::Path::PATH_SEPERATOR;
					}
					u8ptr = Text::StrConcat(u8ptr, (const UTF8Char*)"UserFile");
					*u8ptr++ = IO::Path::PATH_SEPERATOR;
					u8ptr = Text::StrInt32(u8ptr, userFile->webuserId);
					*u8ptr++ = IO::Path::PATH_SEPERATOR;
					dt.SetTicks(userFile->fileTimeTicks);
					u8ptr = dt.ToString(u8ptr, "yyyyMM");
					*u8ptr++ = IO::Path::PATH_SEPERATOR;
					u8ptr = userFile->dataFileName->ConcatTo(u8ptr);

					IO::StmData::FileData *fd;
					Media::PhotoInfo *info;
					NEW_CLASS(fd, IO::StmData::FileData(u8buff, false));
					NEW_CLASS(info, Media::PhotoInfo(fd));
					DEL_CLASS(fd);
					if (info->HasInfo())
					{
						sb.ClearStr();
						writer->Write((const UTF8Char*)"<b>");
						writer->Write(LangGetValue(lang, (const UTF8Char*)"PhotoSpec"));
						writer->Write((const UTF8Char*)"</b> ");
						info->ToString(&sb);
						writer->Write(sb.ToString());
						writer->Write((const UTF8Char*)"<br/>");

						dt.SetTicks(userFile->captureTimeTicks);
						dt.ToLocalTime();
						writer->Write((const UTF8Char*)"<b>");
						writer->Write(LangGetValue(lang, (const UTF8Char*)"PhotoDate"));
						writer->Write((const UTF8Char*)"</b> ");
						dt.ToString(u8buff2, "yyyy-MM-dd HH:mm:ss zzzz");
						writer->Write(u8buff2);
						writer->Write((const UTF8Char*)"<br/>");
					}
					DEL_CLASS(info);
				}

				if (env.user && (userFile->webuserId == env.user->id || env.user->userType == 0))
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)"photodown.html?id=");
					sb.AppendI32(species->speciesId);
					sb.Append((const UTF8Char*)"&cateId=");
					sb.AppendI32(species->cateId);
					sb.Append((const UTF8Char*)"&fileId=");
					sb.AppendI32(userFile->id);
					txt = Text::XML::ToNewAttrText(sb.ToString());
					sb.ClearStr();
					sb.Append((const UTF8Char*)"<a href=");
					sb.Append(txt);
					Text::XML::FreeNewText(txt);
					sb.Append((const UTF8Char*)">");
					sb.Append(LangGetValue(lang, (const UTF8Char*)"Download"));
					sb.Append((const UTF8Char*)"</a>");
					writer->WriteLine(sb.ToString());

					sb.ClearStr();
					sb.Append((const UTF8Char*)"<form name=\"photo\" method=\"POST\" action=\"photodetail.html?id=");
					sb.AppendI32(id);
					sb.Append((const UTF8Char*)"&amp;cateId=");
					sb.AppendI32(cateId);
					sb.Append((const UTF8Char*)"&amp;fileId=");
					sb.AppendI32(fileId);
					sb.Append((const UTF8Char*)"\"><input type=\"hidden\" name=\"action\"/>");
					if (env.user->userType == 0)
					{
						sb.Append((const UTF8Char*)"<input type=\"button\" value=\"Set As Species Photo\" onclick=\"document.forms.photo.action.value='setdefault';document.forms.photo.submit();\"/>");
					}
					sb.Append((const UTF8Char*)"<input type=\"button\" value=\"Rotate\" onclick=\"document.forms.photo.action.value='rotate';document.forms.photo.submit();\"/>");
					sb.Append((const UTF8Char*)"<br/>");
					writer->WriteLine(sb.ToString());
					sb.ClearStr();
					sb.Append((const UTF8Char*)"Description<input type=\"text\" name=\"descr\"");
					if (userFile->descript)
					{
						sb.Append((const UTF8Char*)" value=");
						txt = Text::XML::ToNewAttrText(userFile->descript->v);
						sb.Append(txt);
						Text::XML::FreeNewText(txt);
					}
					sb.Append((const UTF8Char*)"/><input type=\"button\" value=\"Update\" onclick=\"document.forms.photo.action.value='setname';document.forms.photo.submit();\"/>");
					sb.Append((const UTF8Char*)"</form>");
					writer->WriteLine(sb.ToString());
				}

				if (env.user && env.user->userType == 0)

				sb.ClearStr();
				sb.Append((const UTF8Char*)"species.html?id=");
				sb.AppendI32(species->speciesId);
				sb.Append((const UTF8Char*)"&cateId=");
				sb.AppendI32(species->cateId);
				txt = Text::XML::ToNewAttrText(sb.ToString());
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<a href=");
				sb.Append(txt);
				Text::XML::FreeNewText(txt);
				sb.Append((const UTF8Char*)">");
				sb.Append(LangGetValue(lang, (const UTF8Char*)"Back"));
				sb.Append((const UTF8Char*)"</a>");
				writer->WriteLine(sb.ToString());
				writer->WriteLine((const UTF8Char*)"</td></tr>");
				writer->WriteLine((const UTF8Char*)"</table>");
				writer->WriteLine((const UTF8Char*)"</center>");

				me->WriteFooter(writer);
				me->dataMut->UnlockRead();
				ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

				DEL_CLASS(writer);
				DEL_CLASS(mstm);
				return true;
			}
			else
			{
				resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
				me->dataMut->UnlockRead();
				return true;
			}
		}
		else if (req->GetQueryValueI32((const UTF8Char*)"fileWId", &fileId))
		{
			Data::ArrayListICaseStrUTF8 *fileNameList;
			wfile = species->wfiles->Get(fileId);
			if (wfile)
			{
				NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.SvcPhotoDetail"));
				NEW_CLASS(writer, Text::UTF8Writer(mstm));

				sb.ClearStr();
				sb.Append(cate->chiName);
				sb.Append((const UTF8Char*)" - ");
				sb.Append(species->sciName);
				sb.Append((const UTF8Char*)" ");
				sb.Append(species->chiName);
				sb.Append((const UTF8Char*)" ");
				sb.Append(species->engName);
				me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
				writer->Write((const UTF8Char*)"<center><h1>");
				txt = Text::XML::ToNewHTMLText(sb.ToString());
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				writer->WriteLine((const UTF8Char*)"</h1></center>");

				writer->WriteLine((const UTF8Char*)"<center>");
				writer->WriteLine((const UTF8Char*)"<table border=\"0\" width=\"100%\">");
				writer->WriteLine((const UTF8Char*)"<tr><td align=\"center\">");

				i = (UOSInt)species->wfiles->GetIndex(fileId);
				j = species->wfiles->GetCount();
				if (i + 1 < j)
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)"photodetail.html?id=");
					sb.AppendI32(species->speciesId);
					sb.Append((const UTF8Char*)"&cateId=");
					sb.AppendI32(species->cateId);
					sb.Append((const UTF8Char*)"&fileWId=");
					sb.AppendI32(species->wfiles->GetKey(i + 1));
				}
				else
				{
					NEW_CLASS(fileNameList, Data::ArrayListICaseStrUTF8());

					Text::StrConcat(u8ptr, IO::Path::ALL_FILES);
					sess = IO::Path::FindFile(u8buff);
					if (sess)
					{

						while ((u8ptr2 = IO::Path::FindNextFile(u8ptr, sess, 0, &pt, 0)) != 0)
						{
							if (pt == IO::Path::PathType::File)
							{
								if (Text::StrEndsWithICase(u8ptr, (const UTF8Char*)".JPG") || Text::StrEndsWithICase(u8ptr, (const UTF8Char*)".PCX") || Text::StrEndsWith(u8ptr, (const UTF8Char*)".TIF"))
								{
									u8ptr2[-4] = 0;
									fileNameList->SortedInsert(Text::StrCopyNew(u8ptr));
								}
							}
						}
						IO::Path::FindFileClose(sess);
					}
					if (fileNameList->GetCount() > 0)
					{
						sb.ClearStr();
						sb.Append((const UTF8Char*)"photodetail.html?id=");
						sb.AppendI32(species->speciesId);
						sb.Append((const UTF8Char*)"&cateId=");
						sb.AppendI32(species->cateId);
						sb.Append((const UTF8Char*)"&file=");
						Text::TextEnc::URIEncoding::URIEncode(u8buff2, fileNameList->GetItem(0));
						sb.Append(u8buff2);
					}
					else
					{
						Bool found;
						Text::StrConcat(u8ptr, (const UTF8Char*)"web.txt");
						NEW_CLASS(fs, IO::FileStream(u8buff, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
						NEW_CLASS(reader, Text::UTF8Reader(fs));
						sb.ClearStr();
						found = false;
						while (reader->ReadLine(&sb, 4096))
						{
							if (Text::StrSplit(sarr, 4, sb.ToString(), '\t') == 3)
							{
								found = true;
								u8ptr2 = Text::StrConcat(u8buff2, (const UTF8Char*)"web");
								*u8ptr2++ = IO::Path::PATH_SEPERATOR;
								Text::StrConcat(u8ptr2, sarr[0]);
								i = Text::StrLastIndexOf(u8buff2, '.');
								if (i != INVALID_INDEX)
									u8buff2[i] = 0;
								break;
							}
							sb.ClearStr();
						}
						DEL_CLASS(reader);
						DEL_CLASS(fs);

						if (found)
						{
							sb.ClearStr();
							sb.Append(u8buff2);
							Text::TextEnc::URIEncoding::URIEncode(u8buff2, sb.ToString());
							sb.ClearStr();
							sb.Append((const UTF8Char*)"photodetail.html?id=");
							sb.AppendI32(species->speciesId);
							sb.Append((const UTF8Char*)"&cateId=");
							sb.AppendI32(species->cateId);
							sb.Append((const UTF8Char*)"&file=");
							sb.Append(u8buff2);
						}
						else
						{
							sb.ClearStr();
							sb.Append((const UTF8Char*)"species.html?id=");
							sb.AppendI32(species->speciesId);
							sb.Append((const UTF8Char*)"&cateId=");
							sb.AppendI32(species->cateId);
						}
					}
					i = fileNameList->GetCount();
					while (i-- > 0)
					{
						Text::StrDelNew(fileNameList->GetItem(i));
					}
					DEL_CLASS(fileNameList);
				}
				txt = Text::XML::ToNewAttrText(sb.ToString());
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<a href=");
				sb.Append(txt);
				Text::XML::FreeNewText(txt);
				sb.Append((const UTF8Char*)">");
				writer->WriteLine(sb.ToString());

				sb.ClearStr();
				sb.Append((const UTF8Char*)"photo.html?id=");
				sb.AppendI32(species->speciesId);
				sb.Append((const UTF8Char*)"&cateId=");
				sb.AppendI32(species->cateId);
				sb.Append((const UTF8Char*)"&width=");
				sb.AppendU32(env.scnWidth);
				sb.Append((const UTF8Char*)"&height=");
				sb.AppendU32(env.scnWidth);
				sb.Append((const UTF8Char*)"&fileWId=");
				sb.AppendI32(fileId);
				txt = Text::XML::ToNewAttrText(sb.ToString());
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<img src=");
				sb.Append(txt);
				Text::XML::FreeNewText(txt);
				sb.Append((const UTF8Char*)" border=\"0\"></a>");
				writer->WriteLine(sb.ToString());
				writer->WriteLine((const UTF8Char*)"</td></tr>");
				writer->WriteLine((const UTF8Char*)"<tr><td align=\"center\">");

				writer->Write((const UTF8Char*)"<b>Image URL:</b> <a href=");
				txt = Text::XML::ToNewAttrText(wfile->imgUrl->v);
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				writer->Write((const UTF8Char*)">");
				writer->Write(wfile->imgUrl->v, wfile->imgUrl->leng);
				writer->Write((const UTF8Char*)"</a><br/>");

				writer->Write((const UTF8Char*)"<b>Source URL:</b> <a href=");
				txt = Text::XML::ToNewAttrText(wfile->srcUrl->v);
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				writer->Write((const UTF8Char*)">");
				writer->Write(wfile->srcUrl->v, wfile->srcUrl->leng);
				writer->Write((const UTF8Char*)"</a><br/>");

				sb.ClearStr();
				sb.Append((const UTF8Char*)"species.html?id=");
				sb.AppendI32(species->speciesId);
				sb.Append((const UTF8Char*)"&cateId=");
				sb.AppendI32(species->cateId);
				txt = Text::XML::ToNewAttrText(sb.ToString());
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<a href=");
				sb.Append(txt);
				Text::XML::FreeNewText(txt);
				sb.Append(LangGetValue(lang, (const UTF8Char*)"Back"));
				sb.Append((const UTF8Char*)"</a>");
				writer->WriteLine(sb.ToString());
				writer->WriteLine((const UTF8Char*)"</td></tr>");
				writer->WriteLine((const UTF8Char*)"</table>");
				writer->WriteLine((const UTF8Char*)"</center>");

				me->WriteFooter(writer);
				me->dataMut->UnlockRead();
				ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

				DEL_CLASS(writer);
				DEL_CLASS(mstm);
				return true;
			}
			else
			{
				resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
				me->dataMut->UnlockRead();
				return true;
			}
		}
		else if (req->GetQueryValueStr((const UTF8Char*)"file", fileName, 512))
		{
			if (Text::StrStartsWith(fileName, (const UTF8Char*)"web") && fileName[3] == IO::Path::PATH_SEPERATOR)
			{
				const UTF8Char *srcURL = 0;
				const UTF8Char *imgURL = 0;
				Bool found;
				Bool foundNext;

				Text::StrConcat(u8ptr, (const UTF8Char*)"web.txt");

				Text::StrConcat(Text::StrConcat(u8buff2, &fileName[4]), (const UTF8Char*)".");
				Text::StrToUpper(u8buff2, u8buff2);
				NEW_CLASS(fs, IO::FileStream(u8buff, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
				NEW_CLASS(reader, Text::UTF8Reader(fs));
				sb.ClearStr();
				found = false;
				foundNext = false;
				while (reader->ReadLine(&sb, 4096))
				{
					if (Text::StrSplit(sarr, 4, sb.ToString(), '\t') == 3)
					{
						if (found)
						{
							foundNext = true;
							Text::StrConcat(u8buff2, sarr[0]);
							break;
						}
						else
						{
							Text::StrToUpper(sarr[0], sarr[0]);
							if (Text::StrStartsWith(sarr[0], u8buff2))
							{
								found = true;
								foundNext = false;
								srcURL = Text::StrCopyNew(sarr[2]);
								imgURL = Text::StrCopyNew(sarr[1]);
							}
						}
					}
					sb.ClearStr();
				}
				DEL_CLASS(reader);
				DEL_CLASS(fs);

				if (found)
				{
					NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.SvcPhotoDetail"));
					NEW_CLASS(writer, Text::UTF8Writer(mstm));

					sb.ClearStr();
					sb.Append(cate->chiName);
					sb.Append((const UTF8Char*)" - ");
					sb.Append(species->sciName);
					sb.Append((const UTF8Char*)" ");
					sb.Append(species->chiName);
					sb.Append((const UTF8Char*)" ");
					sb.Append(species->engName);
					me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
					writer->Write((const UTF8Char*)"<center><h1>");
					txt = Text::XML::ToNewHTMLText(sb.ToString());
					writer->Write(txt);
					Text::XML::FreeNewText(txt);
					writer->WriteLine((const UTF8Char*)"</h1></center>");

					writer->WriteLine((const UTF8Char*)"<center>");
					writer->WriteLine((const UTF8Char*)"<table border=\"0\" width=\"100%\">");
					writer->WriteLine((const UTF8Char*)"<tr><td align=\"center\">");
					if (foundNext)
					{
						sb.ClearStr();
						sb.Append((const UTF8Char*)"photodetail.html?id=");
						sb.AppendI32(species->speciesId);
						sb.Append((const UTF8Char*)"&cateId=");
						sb.AppendI32(species->cateId);
						sb.Append((const UTF8Char*)"&file=");
						u8ptr2 = Text::StrConcat(u8buff, (const UTF8Char*)"web");
						*u8ptr2++ = IO::Path::PATH_SEPERATOR;
						Text::StrConcat(u8ptr2, u8buff2);
						i = Text::StrLastIndexOf(u8buff, '.');
						if (i != INVALID_INDEX)
							u8buff[i] = 0;
						Text::TextEnc::URIEncoding::URIEncode(u8buff2, u8buff);
						sb.Append(u8buff2);
					}
					else
					{
						sb.ClearStr();
						sb.Append((const UTF8Char*)"species.html?id=");
						sb.AppendI32(species->speciesId);
						sb.Append((const UTF8Char*)"&cateId=");
						sb.AppendI32(species->cateId);
					}
					txt = Text::XML::ToNewAttrText(sb.ToString());
					sb.ClearStr();
					sb.Append((const UTF8Char*)"<a href=");
					sb.Append(txt);
					Text::XML::FreeNewText(txt);
					sb.Append((const UTF8Char*)">");
					writer->WriteLine(sb.ToString());

					sb.ClearStr();
					sb.Append((const UTF8Char*)"photo.html?id=");
					sb.AppendI32(species->speciesId);
					sb.Append((const UTF8Char*)"&cateId=");
					sb.AppendI32(species->cateId);
					sb.Append((const UTF8Char*)"&width=");
					sb.AppendU32(env.scnWidth);
					sb.Append((const UTF8Char*)"&height=");
					sb.AppendU32(env.scnWidth);
					sb.Append((const UTF8Char*)"&file=");
					sb.Append(fileName);
					txt = Text::XML::ToNewAttrText(sb.ToString());
					sb.ClearStr();
					sb.Append((const UTF8Char*)"<img src=");
					sb.Append(txt);
					Text::XML::FreeNewText(txt);
					sb.Append((const UTF8Char*)" border=\"0\"></a>");
					writer->WriteLine(sb.ToString());
					writer->WriteLine((const UTF8Char*)"</td></tr>");
					writer->WriteLine((const UTF8Char*)"<tr><td align=\"center\">");

					writer->Write((const UTF8Char*)"<b>Image URL:</b> <a href=");
					txt = Text::XML::ToNewAttrText(imgURL);
					writer->Write(txt);
					Text::XML::FreeNewText(txt);
					writer->Write((const UTF8Char*)">");
					writer->Write(imgURL);
					writer->Write((const UTF8Char*)"</a><br/>");

					writer->Write((const UTF8Char*)"<b>Source URL:</b> <a href=");
					txt = Text::XML::ToNewAttrText(srcURL);
					writer->Write(txt);
					Text::XML::FreeNewText(txt);
					writer->Write((const UTF8Char*)">");
					writer->Write(srcURL);
					writer->Write((const UTF8Char*)"</a><br/>");

					sb.ClearStr();
					sb.Append((const UTF8Char*)"species.html?id=");
					sb.AppendI32(species->speciesId);
					sb.Append((const UTF8Char*)"&cateId=");
					sb.AppendI32(species->cateId);
					txt = Text::XML::ToNewAttrText(sb.ToString());
					sb.ClearStr();
					sb.Append((const UTF8Char*)"<a href=");
					sb.Append(txt);
					Text::XML::FreeNewText(txt);
					sb.Append(LangGetValue(lang, (const UTF8Char*)"Back"));
					sb.Append((const UTF8Char*)"</a>");
					writer->WriteLine(sb.ToString());
					writer->WriteLine((const UTF8Char*)"</td></tr>");
					writer->WriteLine((const UTF8Char*)"</table>");
					writer->WriteLine((const UTF8Char*)"</center>");

					me->WriteFooter(writer);
					me->dataMut->UnlockRead();
					ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

					DEL_CLASS(writer);
					DEL_CLASS(mstm);
					Text::StrDelNew(srcURL);
					Text::StrDelNew(imgURL);
					return true;
				}
				else
				{
					resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
					me->dataMut->UnlockRead();
					return true;
				}
			}
			else
			{
				Data::ArrayListICaseStrUTF8 *fileNameList;
				NEW_CLASS(fileNameList, Data::ArrayListICaseStrUTF8());

				Text::StrConcat(u8ptr, IO::Path::ALL_FILES);
				sess = IO::Path::FindFile(u8buff);
				if (sess)
				{
					while ((u8ptr2 = IO::Path::FindNextFile(u8ptr, sess, 0, &pt, 0)) != 0)
					{
						if (pt == IO::Path::PathType::File)
						{
							if (Text::StrEndsWithICase(u8ptr, (const UTF8Char*)".JPG") || Text::StrEndsWithICase(u8ptr, (const UTF8Char*)".PCX") || Text::StrEndsWith(u8ptr, (const UTF8Char*)".TIF"))
							{
								u8ptr2[-4] = 0;
								fileNameList->SortedInsert(Text::StrCopyNew(u8ptr));
							}
						}
					}
					IO::Path::FindFileClose(sess);
					i = (UOSInt)fileNameList->SortedIndexOf(fileName);
					if ((OSInt)i < 0)
					{
						i = fileNameList->GetCount();
						while (i-- > 0)
						{
							Text::StrDelNew(fileNameList->GetItem(i));
						}
						DEL_CLASS(fileNameList);

						resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
						me->dataMut->UnlockRead();
						return true;
					}

					NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.SvcPhotoDetail"));
					NEW_CLASS(writer, Text::UTF8Writer(mstm));

					sb.ClearStr();
					sb.Append(cate->chiName);
					sb.Append((const UTF8Char*)" - ");
					sb.Append(species->sciName);
					sb.Append((const UTF8Char*)" ");
					sb.Append(species->chiName);
					sb.Append((const UTF8Char*)" ");
					sb.Append(species->engName);
					me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
					writer->Write((const UTF8Char*)"<center><h1>");
					txt = Text::XML::ToNewHTMLText(sb.ToString());
					writer->Write(txt);
					Text::XML::FreeNewText(txt);
					writer->WriteLine((const UTF8Char*)"</h1></center>");

					writer->WriteLine((const UTF8Char*)"<center>");
					writer->WriteLine((const UTF8Char*)"<table border=\"0\" width=\"100%\">");
					writer->WriteLine((const UTF8Char*)"<tr><td align=\"center\">");
					if (i < fileNameList->GetCount() - 1)
					{
						sb.ClearStr();
						sb.Append((const UTF8Char*)"photodetail.html?id=");
						sb.AppendI32(species->speciesId);
						sb.Append((const UTF8Char*)"&cateId=");
						sb.AppendI32(species->cateId);
						sb.Append((const UTF8Char*)"&file=");
						Text::TextEnc::URIEncoding::URIEncode(u8buff2, fileNameList->GetItem(i + 1));
						sb.Append(u8buff2);
					}
					else
					{
						Bool found;
						Text::StrConcat(u8ptr, (const UTF8Char*)"web.txt");
						NEW_CLASS(fs, IO::FileStream(u8buff, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
						NEW_CLASS(reader, Text::UTF8Reader(fs));
						sb.ClearStr();
						found = false;
						while (reader->ReadLine(&sb, 4096))
						{
							if (Text::StrSplit(sarr, 4, sb.ToString(), '\t') == 3)
							{
								found = true;
								u8ptr2 = Text::StrConcat(u8buff2, (const UTF8Char*)"web");
								*u8ptr2++ = IO::Path::PATH_SEPERATOR;
								Text::StrConcat(u8ptr2, sarr[0]);
								i = Text::StrLastIndexOf(u8buff2, '.');
								if (i != INVALID_INDEX)
									u8buff2[i] = 0;
								break;
							}
							sb.ClearStr();
						}
						DEL_CLASS(reader);
						DEL_CLASS(fs);

						if (found)
						{
							sb.ClearStr();
							sb.Append(u8buff2);
							Text::TextEnc::URIEncoding::URIEncode(u8buff2, sb.ToString());
							sb.ClearStr();
							sb.Append((const UTF8Char*)"photodetail.html?id=");
							sb.AppendI32(species->speciesId);
							sb.Append((const UTF8Char*)"&cateId=");
							sb.AppendI32(species->cateId);
							sb.Append((const UTF8Char*)"&file=");
							sb.Append(u8buff2);
						}
						else
						{
							sb.ClearStr();
							sb.Append((const UTF8Char*)"species.html?id=");
							sb.AppendI32(species->speciesId);
							sb.Append((const UTF8Char*)"&cateId=");
							sb.AppendI32(species->cateId);
						}
					}
					txt = Text::XML::ToNewAttrText(sb.ToString());
					sb.ClearStr();
					sb.Append((const UTF8Char*)"<a href=");
					sb.Append(txt);
					Text::XML::FreeNewText(txt);
					sb.Append((const UTF8Char*)">");
					writer->WriteLine(sb.ToString());

					sb.ClearStr();
					sb.Append((const UTF8Char*)"photo.html?id=");
					sb.AppendI32(species->speciesId);
					sb.Append((const UTF8Char*)"&cateId=");
					sb.AppendI32(species->cateId);
					sb.Append((const UTF8Char*)"&width=");
					sb.AppendU32(env.scnWidth);
					sb.Append((const UTF8Char*)"&height=");
					sb.AppendU32(env.scnWidth);
					sb.Append((const UTF8Char*)"&file=");
					sb.Append(fileName);
					txt = Text::XML::ToNewAttrText(sb.ToString());
					sb.ClearStr();
					sb.Append((const UTF8Char*)"<img src=");
					sb.Append(txt);
					Text::XML::FreeNewText(txt);
					sb.Append((const UTF8Char*)" border=\"0\"></a>");
					writer->WriteLine(sb.ToString());
					writer->WriteLine((const UTF8Char*)"</td></tr>");
					writer->WriteLine((const UTF8Char*)"<tr><td align=\"center\">");

					Text::StrConcat(Text::StrConcat(u8ptr, fileName), (const UTF8Char*)".jpg");
					IO::StmData::FileData *fd;
					Media::PhotoInfo *info;
					NEW_CLASS(fd, IO::StmData::FileData(u8buff, false));
					NEW_CLASS(info, Media::PhotoInfo(fd));
					DEL_CLASS(fd);
					if (info->HasInfo())
					{
						Data::DateTime dt;
						sb.ClearStr();
						writer->Write((const UTF8Char*)"<b>");
						writer->Write(LangGetValue(lang, (const UTF8Char*)"PhotoSpec"));
						writer->Write((const UTF8Char*)"</b> ");
						info->ToString(&sb);
						writer->Write(sb.ToString());
						writer->Write((const UTF8Char*)"<br/>");
						if (info->GetPhotoDate(&dt))
						{
							writer->Write((const UTF8Char*)"<b>");
							writer->Write(LangGetValue(lang, (const UTF8Char*)"PhotoDate"));
							writer->Write((const UTF8Char*)"</b> ");
							dt.ToString(u8buff2, "yyyy-MM-dd HH:mm:ss zzzz");
							writer->Write(u8buff2);
							writer->Write((const UTF8Char*)"<br/>");
						}
					}
					DEL_CLASS(info);

					sb.ClearStr();
					sb.Append((const UTF8Char*)"species.html?id=");
					sb.AppendI32(species->speciesId);
					sb.Append((const UTF8Char*)"&cateId=");
					sb.AppendI32(species->cateId);
					txt = Text::XML::ToNewAttrText(sb.ToString());
					sb.ClearStr();
					sb.Append((const UTF8Char*)"<a href=");
					sb.Append(txt);
					Text::XML::FreeNewText(txt);
					sb.Append(LangGetValue(lang, (const UTF8Char*)"Back"));
					sb.Append((const UTF8Char*)"</a>");
					writer->WriteLine(sb.ToString());
					writer->WriteLine((const UTF8Char*)"</td></tr>");
					writer->WriteLine((const UTF8Char*)"</table>");
					writer->WriteLine((const UTF8Char*)"</center>");

					me->WriteFooter(writer);
					me->dataMut->UnlockRead();
					ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

					DEL_CLASS(writer);
					DEL_CLASS(mstm);

					i = fileNameList->GetCount();
					while (i-- > 0)
					{
						Text::StrDelNew(fileNameList->GetItem(i));
					}
					DEL_CLASS(fileNameList);
					return true;
				}
				else
				{
					resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
					me->dataMut->UnlockRead();
					DEL_CLASS(fileNameList);
					return true;
				}
			}
		}
		me->dataMut->UnlockRead();
	}
	resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcPhotoDetailD(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 fileId;
	UInt32 index;
	if (env.user != 0 &&
		req->GetQueryValueI32((const UTF8Char*)"fileId", &fileId) &&
		req->GetQueryValueU32((const UTF8Char*)"index", &index))
	{
		const UTF8Char *txt;
		IO::MemoryStream *mstm;
		IO::Writer *writer;

		UTF8Char u8buff[512];
		UTF8Char u8buff2[512];
		UTF8Char *u8ptr;
		Text::StringBuilderUTF8 sb;
		SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species;
		Data::DateTime dt;
		IO::ConfigFile *lang = me->LangGet(req);

		me->dataMut->LockRead();
		SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile = 0;
		if (env.user)
		{
			userFile = env.user->userFileObj->GetItem(index);
			if (userFile)
			{
				if (userFile->id != fileId)
				{
					userFile = 0;
				}
			}
		}
		if (userFile)
		{
			SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile2;
			NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.SvcPhotoDetailD"));
			NEW_CLASS(writer, Text::UTF8Writer(mstm));

			dt.SetTicks(userFile->captureTimeTicks);
			dt.ToString(u8buff, "yyyy-MM-dd");

			me->WriteHeader(writer, u8buff, env.user, env.isMobile);
			writer->Write((const UTF8Char*)"<center><h1>");
			txt = Text::XML::ToNewHTMLText(u8buff);
			writer->Write(txt);
			Text::XML::FreeNewText(txt);
			writer->WriteLine((const UTF8Char*)"</h1></center>");

			writer->WriteLine((const UTF8Char*)"<center>");
			writer->WriteLine((const UTF8Char*)"<table border=\"0\" width=\"100%\">");
			writer->WriteLine((const UTF8Char*)"<tr><td align=\"center\">");
			species = me->spMap->Get(userFile->speciesId);

			userFile2 = env.user->userFileObj->GetItem(index + 1);
			if (userFile2 && (userFile->captureTimeTicks / 86400000LL) == (userFile2->captureTimeTicks / 86400000LL))
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"photodetaild.html?fileId=");
				sb.AppendI32(userFile2->id);
				sb.Append((const UTF8Char*)"&index=");
				sb.AppendU32(index + 1);
			}
			else
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"photoday.html?d=");
				sb.AppendI64(userFile->captureTimeTicks / 86400000LL);
			}
			txt = Text::XML::ToNewAttrText(sb.ToString());
			sb.ClearStr();
			sb.Append((const UTF8Char*)"<a href=");
			sb.Append(txt);
			Text::XML::FreeNewText(txt);
			sb.Append((const UTF8Char*)">");
			writer->WriteLine(sb.ToString());

			sb.ClearStr();
			sb.Append((const UTF8Char*)"photo.html?id=");
			sb.AppendI32(species->speciesId);
			sb.Append((const UTF8Char*)"&cateId=");
			sb.AppendI32(species->cateId);
			sb.Append((const UTF8Char*)"&width=");
			sb.AppendU32(env.scnWidth);
			sb.Append((const UTF8Char*)"&height=");
			sb.AppendU32(env.scnWidth);
			sb.Append((const UTF8Char*)"&fileId=");
			sb.AppendI32(fileId);
			txt = Text::XML::ToNewAttrText(sb.ToString());
			sb.ClearStr();
			sb.Append((const UTF8Char*)"<img src=");
			sb.Append(txt);
			Text::XML::FreeNewText(txt);
			sb.Append((const UTF8Char*)" border=\"0\"></a>");
			writer->WriteLine(sb.ToString());
			writer->WriteLine((const UTF8Char*)"</td></tr>");
			writer->WriteLine((const UTF8Char*)"<tr><td align=\"center\">");


			if (userFile->fileType == 3)
			{
				Data::DateTime dt;
				IO::StmData::FileData *fd;
				UInt64 fileSize = 0;
				Media::MediaFile *mediaFile;
				u8ptr = Text::StrConcat(u8buff, me->dataDir);
				if (u8ptr[-1] != IO::Path::PATH_SEPERATOR)
				{
					*u8ptr++ = IO::Path::PATH_SEPERATOR;
				}
				u8ptr = Text::StrConcat(u8ptr, (const UTF8Char*)"UserFile");
				*u8ptr++ = IO::Path::PATH_SEPERATOR;
				u8ptr = Text::StrInt32(u8ptr, userFile->webuserId);
				*u8ptr++ = IO::Path::PATH_SEPERATOR;
				dt.SetTicks(userFile->fileTimeTicks);
				u8ptr = dt.ToString(u8ptr, "yyyyMM");
				*u8ptr++ = IO::Path::PATH_SEPERATOR;
				u8ptr = userFile->dataFileName->ConcatTo(u8ptr);
				Sync::MutexUsage mutUsage(me->parserMut);
				NEW_CLASS(fd, IO::StmData::FileData(u8buff, false));
				fileSize = fd->GetDataSize();
				mediaFile = (Media::MediaFile*)me->parsers->ParseFileType(fd, IO::ParserType::MediaFile);
				DEL_CLASS(fd);
				mutUsage.EndUse();

				if (mediaFile)
				{
					sb.ClearStr();
					sb.AppendU64(fileSize);
					sb.Append((const UTF8Char*)" bytes");
					Media::IMediaSource *msrc = mediaFile->GetStream(0, 0);
					Int32 stmTime;
					if (msrc)
					{
						stmTime = msrc->GetStreamTime();
						sb.Append((const UTF8Char*)", Length: ");
						sb.AppendI32(stmTime / 60000);
						sb.Append((const UTF8Char*)":");
						stmTime = stmTime % 60000;
						if (stmTime < 10000)
						{
							sb.Append((const UTF8Char*)"0");
						}
						Text::SBAppendF64(&sb, stmTime * 0.001);

						if (msrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
						{
							Media::IAudioSource *asrc = (Media::IAudioSource*)msrc;
							Media::AudioFormat format;
							asrc->GetFormat(&format);
							sb.Append((const UTF8Char*)" ");
							sb.AppendU32(format.frequency);
							sb.Append((const UTF8Char*)"Hz, ");
							sb.AppendU32(format.bitpersample);
							sb.Append((const UTF8Char*)"bits, ");
							sb.AppendU32(format.nChannels);
							sb.Append((const UTF8Char*)" ch");
						}
					}
					writer->Write((const UTF8Char*)"<b>");
					writer->Write(LangGetValue(lang, (const UTF8Char*)"PhotoSpec"));
					writer->Write((const UTF8Char*)"</b> ");
					writer->Write(sb.ToString());
					writer->Write((const UTF8Char*)"<br/>");
					DEL_CLASS(mediaFile);
				}
				if (userFile->captureTimeTicks != 0)
				{
					dt.SetTicks(userFile->captureTimeTicks);
					dt.ToLocalTime();
					writer->Write((const UTF8Char*)"<b>");
					writer->Write(LangGetValue(lang, (const UTF8Char*)"PhotoDate"));
					writer->Write((const UTF8Char*)"</b> ");
					dt.ToString(u8buff2, "yyyy-MM-dd HH:mm:ss zzzz");
					writer->Write(u8buff2);
					writer->Write((const UTF8Char*)"<br/>");
				}
			}
			else
			{
				u8ptr = Text::StrConcat(u8buff, me->dataDir);
				if (u8ptr[-1] != IO::Path::PATH_SEPERATOR)
				{
					*u8ptr++ = IO::Path::PATH_SEPERATOR;
				}
				u8ptr = Text::StrConcat(u8ptr, (const UTF8Char*)"UserFile");
				*u8ptr++ = IO::Path::PATH_SEPERATOR;
				u8ptr = Text::StrInt32(u8ptr, userFile->webuserId);
				*u8ptr++ = IO::Path::PATH_SEPERATOR;
				dt.SetTicks(userFile->fileTimeTicks);
				u8ptr = dt.ToString(u8ptr, "yyyyMM");
				*u8ptr++ = IO::Path::PATH_SEPERATOR;
				u8ptr = userFile->dataFileName->ConcatTo(u8ptr);

				IO::StmData::FileData *fd;
				Media::PhotoInfo *info;
				NEW_CLASS(fd, IO::StmData::FileData(u8buff, false));
				NEW_CLASS(info, Media::PhotoInfo(fd));
				DEL_CLASS(fd);
				if (info->HasInfo())
				{
					sb.ClearStr();
					writer->Write((const UTF8Char*)"<b>");
					writer->Write(LangGetValue(lang, (const UTF8Char*)"PhotoSpec"));
					writer->Write((const UTF8Char*)"</b> ");
					info->ToString(&sb);
					writer->Write(sb.ToString());
					writer->Write((const UTF8Char*)"<br/>");

					dt.SetTicks(userFile->captureTimeTicks);
					dt.ToLocalTime();
					writer->Write((const UTF8Char*)"<b>");
					writer->Write(LangGetValue(lang, (const UTF8Char*)"PhotoDate"));
					writer->Write((const UTF8Char*)"</b> ");
					dt.ToString(u8buff2, "yyyy-MM-dd HH:mm:ss zzzz");
					writer->Write(u8buff2);
					writer->Write((const UTF8Char*)"<br/>");
				}
				DEL_CLASS(info);
			}

			sb.ClearStr();
			sb.Append((const UTF8Char*)"photoday.html?d=");
			sb.AppendI64(userFile->captureTimeTicks / 86400000LL);
			txt = Text::XML::ToNewAttrText(sb.ToString());
			sb.ClearStr();
			sb.Append((const UTF8Char*)"<a href=");
			sb.Append(txt);
			Text::XML::FreeNewText(txt);
			sb.Append(LangGetValue(lang, (const UTF8Char*)"Back"));
			sb.Append((const UTF8Char*)"</a>");
			writer->WriteLine(sb.ToString());
			writer->WriteLine((const UTF8Char*)"</td></tr>");
			writer->WriteLine((const UTF8Char*)"</table>");
			writer->WriteLine((const UTF8Char*)"</center>");

			me->WriteFooter(writer);
			me->dataMut->UnlockRead();
			ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

			DEL_CLASS(writer);
			DEL_CLASS(mstm);
			return true;
		}
		else
		{
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			me->dataMut->UnlockRead();
			return true;
		}
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcPhotoYear(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	UInt16 y;
	if (env.user != 0 &&
		req->GetQueryValueU16((const UTF8Char*)"y", &y))
	{
		IO::MemoryStream *mstm;
		IO::Writer *writer;
		Data::DateTime dt;
		Data::DateTime dt2;
		UTF8Char sbuff[32];

		NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.SvcPhotoYear"));
		NEW_CLASS(writer, Text::UTF8Writer(mstm));

		Text::StrUInt16(Text::StrConcat(sbuff, (const UTF8Char*)"Year "), y);
		me->WriteHeader(writer, sbuff, env.user, env.isMobile);
		writer->Write((const UTF8Char*)"<center><h1>");
		writer->Write(sbuff);
		writer->WriteLine((const UTF8Char*)"</h1></center>");

		me->dataMut->LockRead();
		Int64 startTime;
		Int64 endTime;
		OSInt startIndex;
		OSInt endIndex;
		dt.ToUTCTime();
		dt.SetValue((UInt16)(y + 1), 1, 1, 0, 0, 0, 0);
		endTime = dt.ToTicks();
		dt.SetValue(y, 1, 1, 0, 0, 0, 0);
		startTime = dt.ToTicks();

		startIndex = env.user->userFileIndex->SortedIndexOf(startTime);
		if (startIndex < 0)
		{
			startIndex = ~startIndex;
		}
		else
		{
			while (startIndex > 0 && env.user->userFileIndex->GetItem((UOSInt)startIndex - 1) == startTime)
			{
				startIndex--;
			}
		}
		endIndex = env.user->userFileIndex->SortedIndexOf(endTime);
		if (endIndex < 0)
		{
			endIndex = ~endIndex;
		}
		else
		{
			while (endIndex > 0 && env.user->userFileIndex->GetItem((UOSInt)endIndex - 1) == endTime)
			{
				endIndex--;
			}
		}
		UInt8 month = 0;
		UInt8 day = 0;
		OSInt dayStartIndex = 0;
		SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile;
		SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
		Text::StringBuilderUTF8 sb;
		UInt32 colCount = env.scnWidth / PREVIEW_SIZE;
		UInt32 colWidth = 100 / colCount;
		UInt32 currColumn;
		Data::ArrayListStrUTF8 locList;
		OSInt si;
		UOSInt i;
		UOSInt j;
		currColumn = 0;
		writer->WriteLine((const UTF8Char*)"<table border=\"0\" width=\"100%\">");

		while (startIndex < endIndex)
		{
			dt.SetTicks(env.user->userFileIndex->GetItem((UOSInt)startIndex));
			if (dt.GetMonth() != month || dt.GetDay() != day)
			{
				if (month != 0 && day != 0)
				{
					userFile = env.user->userFileObj->GetItem((UOSInt)(dayStartIndex + startIndex) >> 1);
					sp = me->spMap->Get(userFile->speciesId);

					if (currColumn == 0)
					{
						writer->WriteLine((const UTF8Char*)"<tr>");
					}

					sb.ClearStr();
					sb.Append((const UTF8Char*)"<td width=\"");
					sb.AppendU32(colWidth);
					sb.Append((const UTF8Char*)"%\"><center>");
					writer->Write(sb.ToString());

					writer->Write((const UTF8Char*)"<a href=\"photoday.html?d=");
					dt2.SetValue(y, month, day, 0, 0, 0, 0);
					Text::StrInt64(sbuff, dt2.ToTicks() / 86400000LL);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"\">");
					writer->Write((const UTF8Char*)"<img src=\"photo.html?id=");
					Text::StrInt32(sbuff, userFile->speciesId);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"&amp;cateId=");
					Text::StrInt32(sbuff, sp->cateId);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"&amp;width=");
					Text::StrInt32(sbuff, PREVIEW_SIZE);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"&amp;height=");
					Text::StrInt32(sbuff, PREVIEW_SIZE);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"&amp;fileId=");
					Text::StrInt32(sbuff, userFile->id);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"\" border=\"0\" ALT=\"");
					Text::StrUInt32(Text::StrConcat(Text::StrUInt32(Text::StrConcat(Text::StrInt32(sbuff, y), (const UTF8Char*)"-"), month), (const UTF8Char*)"-"), day);
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"\" title=\"");
					writer->Write(sbuff);
					writer->Write((const UTF8Char*)"\" /></a><br/>");
					writer->Write(sbuff);
					i = 0;
					j = locList.GetCount();
					while (i < j)
					{
						const UTF8Char *txt = Text::XML::ToNewHTMLText(locList.GetItem(i));
						writer->Write((const UTF8Char*)" ");
						writer->Write(txt);
						Text::XML::FreeNewText(txt);
						i++;
					}

					writer->WriteLine((const UTF8Char*)"</center></td>");

					currColumn++;
					if (currColumn >= colCount)
					{
						writer->WriteLine((const UTF8Char*)"</tr>");
						currColumn = 0;
					}
				}

				if (month != dt.GetMonth())
				{
					month = dt.GetMonth();
					if (currColumn != 0)
					{
						sb.ClearStr();
						sb.Append((const UTF8Char*)"<td width=\"");
						sb.AppendU32(colWidth);
						sb.Append((const UTF8Char*)"%\"></td>");
						while (currColumn < colCount)
						{
							writer->WriteLine(sb.ToString());
							currColumn++;
						}
						writer->WriteLine((const UTF8Char*)"</tr>");
					}
					writer->WriteLine((const UTF8Char*)"</table><hr/>");
					writer->Write((const UTF8Char*)"<h2>");
					Text::StrUInt32(Text::StrConcat(Text::StrInt32(sbuff, y), (const UTF8Char*)"-"), month);
					writer->Write(sbuff);
					writer->WriteLine((const UTF8Char*)"</h2>");
					writer->WriteLine((const UTF8Char*)"<table border=\"0\" width=\"100%\">");
					currColumn = 0;
				}
				day = dt.GetDay();
				dayStartIndex = startIndex;
				locList.Clear();
			}

			const UTF8Char *locName = (const UTF8Char *)"?";
			userFile = env.user->userFileObj->GetItem((UOSInt)startIndex);
			if (userFile->location)
			{
				locName = userFile->location->v;
			}
			/*
			sp = this->spMap->Get(userFile->speciesId);
			Data::Int64Map<SSWR::OrganMgr::OrganWebHandler::TripInfo*> *tripCate = user->tripCates->Get(sp->cateId);
			if (tripCate)
			{
				OSInt ind = tripCate->GetIndex(userFile->captureTime);
				if (ind < 0)
				{
					ind = ~ind - 1;
				}
				SSWR::OrganMgr::OrganWebHandler::TripInfo *trip = tripCate->GetValues()->GetItem(ind);
				if (trip != 0 && trip->fromDate <= userFile->captureTime && trip->toDate > userFile->captureTime)
				{
					SSWR::OrganMgr::OrganWebHandler::LocationInfo *loc = this->locMap->Get(trip->locId);
					if (loc)
					{
						locName = loc->cname;
					}
				}
			}*/
			si = locList.SortedIndexOf(locName);
			if (si < 0)
			{
				locList.SortedInsert(locName);
			}

			startIndex++;
		}
		if (month != 0 && day != 0)
		{
			userFile = env.user->userFileObj->GetItem((UOSInt)(dayStartIndex + startIndex) >> 1);
			sp = me->spMap->Get(userFile->speciesId);
			if (currColumn == 0)
			{
				writer->WriteLine((const UTF8Char*)"<tr>");
			}

			sb.ClearStr();
			sb.Append((const UTF8Char*)"<td width=\"");
			sb.AppendU32(colWidth);
			sb.Append((const UTF8Char*)"%\"><center>");
			writer->Write(sb.ToString());

			writer->Write((const UTF8Char*)"<a href=\"photoday.html?d=");
			dt2.SetValue(y, month, day, 0, 0, 0, 0);
			Text::StrInt64(sbuff, dt2.ToTicks() / 86400000LL);
			writer->Write(sbuff);
			writer->Write((const UTF8Char*)"\">");
			writer->Write((const UTF8Char*)"<img src=\"photo.html?id=");
			Text::StrInt32(sbuff, userFile->speciesId);
			writer->Write(sbuff);
			writer->Write((const UTF8Char*)"&amp;cateId=");
			Text::StrInt32(sbuff, sp->cateId);
			writer->Write(sbuff);
			writer->Write((const UTF8Char*)"&amp;width=");
			Text::StrInt32(sbuff, PREVIEW_SIZE);
			writer->Write(sbuff);
			writer->Write((const UTF8Char*)"&amp;height=");
			Text::StrInt32(sbuff, PREVIEW_SIZE);
			writer->Write(sbuff);
			writer->Write((const UTF8Char*)"&amp;fileId=");
			Text::StrInt32(sbuff, userFile->id);
			writer->Write(sbuff);
			writer->Write((const UTF8Char*)"\" border=\"0\" ALT=\"");
			Text::StrUInt32(Text::StrConcat(Text::StrUInt32(Text::StrConcat(Text::StrInt32(sbuff, y), (const UTF8Char*)"-"), month), (const UTF8Char*)"-"), day);
			writer->Write(sbuff);
			writer->Write((const UTF8Char*)"\" title=\"");
			writer->Write(sbuff);
			writer->Write((const UTF8Char*)"\" /></a><br/>");
			writer->Write(sbuff);
			i = 0;
			j = locList.GetCount();
			while (i < j)
			{
				const UTF8Char *txt = Text::XML::ToNewHTMLText(locList.GetItem(i));
				writer->Write((const UTF8Char*)" ");
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				i++;
			}

			writer->WriteLine((const UTF8Char*)"</center></td>");

			currColumn++;
			if (currColumn >= colCount)
			{
				writer->WriteLine((const UTF8Char*)"</tr>");
				currColumn = 0;
			}
		}
		if (currColumn != 0)
		{
			sb.ClearStr();
			sb.Append((const UTF8Char*)"<td width=\"");
			sb.AppendU32(colWidth);
			sb.Append((const UTF8Char*)"%\"></td>");
			while (currColumn < colCount)
			{
				writer->WriteLine(sb.ToString());
				currColumn++;
			}
			writer->WriteLine((const UTF8Char*)"</tr>");
		}
		writer->WriteLine((const UTF8Char*)"</table><hr/>");
		writer->WriteLine((const UTF8Char*)"<a href=\"/\">Index</a><br/>");
		me->dataMut->UnlockRead();

		me->WriteFooter(writer);
		ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

		DEL_CLASS(writer);
		DEL_CLASS(mstm);
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcPhotoDay(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 d;
	if (env.user != 0 &&
		req->GetQueryValueI32((const UTF8Char*)"d", &d))
	{
		IO::MemoryStream *mstm;
		IO::Writer *writer;
		Data::DateTime dt;
		UTF8Char sbuff[32];
		const UTF8Char *txt;

		NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.SvcPhotoDay"));
		NEW_CLASS(writer, Text::UTF8Writer(mstm));

		dt.SetTicks(d * 86400000LL);
		dt.ToString(sbuff, "yyyy-MM-dd");
		me->WriteHeader(writer, sbuff, env.user, env.isMobile);
		writer->Write((const UTF8Char*)"<center><h1>");
		writer->Write(sbuff);
		writer->WriteLine((const UTF8Char*)"</h1></center>");

		me->dataMut->LockRead();
		Int64 startTime;
		Int64 endTime;
		OSInt startIndex;
		OSInt endIndex;
		Text::StringBuilderUTF8 sb;
		startTime = d * 86400000LL;
		endTime = startTime + 86400000LL;

		startIndex = env.user->userFileIndex->SortedIndexOf(startTime);
		if (startIndex < 0)
		{
			startIndex = ~startIndex;
		}
		else
		{
			while (startIndex > 0 && env.user->userFileIndex->GetItem((UOSInt)startIndex - 1) == startTime)
			{
				startIndex--;
			}
		}
		endIndex = env.user->userFileIndex->SortedIndexOf(endTime);
		if (endIndex < 0)
		{
			endIndex = ~endIndex;
		}
		else
		{
			while (endIndex > 0 && env.user->userFileIndex->GetItem((UOSInt)endIndex - 1) == endTime)
			{
				endIndex--;
			}
		}
		SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile;
		SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
		UInt32 colCount = env.scnWidth / PREVIEW_SIZE;
		UInt32 colWidth = 100 / colCount;
		UInt32 currColumn;
		currColumn = 0;
		writer->WriteLine((const UTF8Char*)"<table border=\"0\" width=\"100%\">");
			
		while (startIndex < endIndex)
		{
			userFile = env.user->userFileObj->GetItem((UOSInt)startIndex);
			sp = me->spMap->Get(userFile->speciesId);
			if (currColumn == 0)
			{
				writer->WriteLine((const UTF8Char*)"<tr>");
			}

			sb.ClearStr();
			sb.Append((const UTF8Char*)"<td width=\"");
			sb.AppendU32(colWidth);
			sb.Append((const UTF8Char*)"%\">");
			writer->WriteLine(sb.ToString());
			sb.ClearStr();
			sb.Append((const UTF8Char*)"photodetaild.html?fileId=");
			sb.AppendI32(userFile->id);
			sb.Append((const UTF8Char*)"&index=");
			sb.AppendOSInt(startIndex);
			txt = Text::XML::ToNewAttrText(sb.ToString());
			sb.ClearStr();
			sb.Append((const UTF8Char*)"<center><a href=");
			sb.Append(txt);
			sb.Append((const UTF8Char*)">");
			writer->WriteLine(sb.ToString());
			Text::XML::FreeNewText(txt);

			writer->Write((const UTF8Char*)"<img src=");
			sb.ClearStr();
			sb.Append((const UTF8Char*)"photo.html?id=");
			sb.AppendI32(sp->speciesId);
			sb.Append((const UTF8Char*)"&cateId=");
			sb.AppendI32(sp->cateId);
			sb.Append((const UTF8Char*)"&width=");
			sb.AppendI32(PREVIEW_SIZE);
			sb.Append((const UTF8Char*)"&height=");
			sb.AppendI32(PREVIEW_SIZE);
			sb.Append((const UTF8Char*)"&fileId=");
			sb.AppendI32(userFile->id);
			txt = Text::XML::ToNewAttrText(sb.ToString());
			writer->Write(txt);
			Text::XML::FreeNewText(txt);
			writer->Write((const UTF8Char*)" border=\"0\">");
			writer->Write((const UTF8Char*)"</a>");

			dt.SetTicks(userFile->captureTimeTicks);
			dt.ToString(sbuff, "HH:mm:ss");
			writer->Write((const UTF8Char*)"<br/>");
			writer->Write(sbuff);

			if (userFile->location)
			{
				writer->Write((const UTF8Char*)" ");
				txt = Text::XML::ToNewHTMLText(userFile->location->v);
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
			}

	/*		Data::Int64Map<SSWR::OrganMgr::OrganWebHandler::TripInfo*> *tripCate = user->tripCates->Get(sp->cateId);
			if (tripCate)
			{
				OSInt ind = tripCate->GetIndex(userFile->captureTime);
				if (ind < 0)
				{
					ind = ~ind - 1;
				}
				SSWR::OrganMgr::OrganWebHandler::TripInfo *trip = tripCate->GetValues()->GetItem(ind);
				if (trip != 0 && trip->fromDate <= userFile->captureTime && trip->toDate > userFile->captureTime)
				{
					SSWR::OrganMgr::OrganWebHandler::LocationInfo *loc = this->locMap->Get(trip->locId);
					if (loc)
					{
						writer->Write((const UTF8Char*)" ");
						txt = Text::XML::ToNewHTMLText(loc->cname);
						writer->Write(txt);
						Text::XML::FreeNewText(txt);
					}
				}
			}*/

			if (userFile->descript)
			{
				writer->Write((const UTF8Char*)"<br/>");
				txt = Text::XML::ToNewHTMLText(userFile->descript->v);
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
			}
			if (userFile->lat != 0 || userFile->lon != 0)
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<br/>");
				sb.Append((const UTF8Char*)"<a href=\"https://www.google.com/maps/place/");
				Text::SBAppendF64(&sb, userFile->lat);
				sb.Append((const UTF8Char*)",");
				Text::SBAppendF64(&sb, userFile->lon);
				sb.Append((const UTF8Char*)"/@");
				Text::SBAppendF64(&sb, userFile->lat);
				sb.Append((const UTF8Char*)",");
				Text::SBAppendF64(&sb, userFile->lon);
				sb.Append((const UTF8Char*)",19z\">");
				sb.Append((const UTF8Char*)"Google Map</a>");
				writer->Write(sb.ToString());
			}
			writer->WriteLine((const UTF8Char*)"</center></td>");

			currColumn++;
			if (currColumn >= colCount)
			{
				writer->WriteLine((const UTF8Char*)"</tr>");
				currColumn = 0;
			}
			startIndex++;
		}

		if (currColumn != 0)
		{
			sb.ClearStr();
			sb.Append((const UTF8Char*)"<td width=\"");
			sb.AppendU32(colWidth);
			sb.Append((const UTF8Char*)"%\"></td>");
			while (currColumn < colCount)
			{
				writer->WriteLine(sb.ToString());
				currColumn++;
			}
			writer->WriteLine((const UTF8Char*)"</tr>");
		}
		writer->WriteLine((const UTF8Char*)"</table><hr/>");
		Text::StrUInt32(sbuff, dt.GetYear());
		writer->Write((const UTF8Char*)"<a href=\"photoyear.html?y=");
		writer->Write(sbuff);
		writer->Write((const UTF8Char*)"\">Year ");
		writer->Write(sbuff);
		writer->Write((const UTF8Char*)"</a><br/>");
		writer->WriteLine((const UTF8Char*)"<a href=\"/\">Index</a><br/>");
		me->dataMut->UnlockRead();

		me->WriteFooter(writer);
		ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

		DEL_CLASS(writer);
		DEL_CLASS(mstm);
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcPhotoUpload(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	if (env.user == 0)
	{
		resp->ResponseError(req, Net::WebStatus::SC_FORBIDDEN);
		return true;
	}
	UOSInt i = 0;
	UOSInt fileSize;
	UTF8Char fileName[512];
	UTF8Char sbuff[32];
	const UInt8 *fileCont;
	IO::MemoryStream *mstm;
	Text::UTF8Writer *writer;
	const UTF8Char *csptr;
	req->ParseHTTPForm();

	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.SvcPhotoUpload"));
	NEW_CLASS(writer, Text::UTF8Writer(mstm));

	me->WriteHeader(writer, (const UTF8Char *)"Photo Upload", env.user, env.isMobile);
	writer->WriteLine((const UTF8Char*)"<table border=\"1\">");
	writer->WriteLine((const UTF8Char*)"<tr><td>File Name</td><td>File Size</td><td>Image Size</td></tr>");
	while (true)
	{
		fileCont = req->GetHTTPFormFile((const UTF8Char *)"file", i, fileName, sizeof(fileName), &fileSize);
		if (fileCont == 0)
		{
			break;
		}
		writer->Write((const UTF8Char*)"<tr><td>");
		csptr = Text::XML::ToNewHTMLText(fileName);
		writer->Write(csptr);
		Text::XML::FreeNewText(csptr);
		writer->Write((const UTF8Char*)"</td><td>");
		Text::StrUOSInt(sbuff, fileSize);
		writer->Write(sbuff);
		writer->Write((const UTF8Char*)"</td><td>");
		me->dataMut->LockWrite();
		Int32 ret = me->UserfileAdd(env.user->id, env.user->unorganSpId, fileName, fileCont, fileSize);
		me->dataMut->UnlockWrite();
		if (ret == 0)
		{
			writer->Write((const UTF8Char*)"Failed");
		}
		else
		{
			writer->Write((const UTF8Char*)"Success");
		}
/*
		IO::StmData::MemoryData *fd;
		Media::ImageList *imgList;
		NEW_CLASS(fd, IO::StmData::MemoryData(fileCont, fileSize));
		me->parserMut->Lock();
		imgList = (Media::ImageList*)me->parsers->ParseFileType(fd, IO::ParserType::ImageList);
		me->parserMut->Unlock();
		DEL_CLASS(fd);
		if (imgList)
		{
			Int32 imgDelay;
			Media::Image *img = imgList->GetImage(0, &imgDelay);
			if (img)
			{
				Text::StrUOSInt(Text::StrConcat(Text::StrUOSInt(sbuff, img->info->dispWidth), (const UTF8Char*)" x "), img->info->dispHeight);
				writer->Write(sbuff);
			}
			else
			{
				writer->Write((const UTF8Char*)"-");
			}
			DEL_CLASS(imgList);
		}
		else
		{
			writer->Write((const UTF8Char*)"-");
		}*/
		writer->WriteLine((const UTF8Char*)"</td></tr>");
		i++;
	}
	writer->WriteLine((const UTF8Char*)"</table>");
	me->WriteFooter(writer);

	ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

	DEL_CLASS(writer);
	DEL_CLASS(mstm);
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcPhotoUploadD(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	if (env.user == 0)
	{
		resp->ResponseError(req, Net::WebStatus::SC_FORBIDDEN);
		return true;
	}

	Text::StringBuilderUTF8 sb;
	if (!req->GetHeader(&sb, (const UTF8Char*)"X-FileName"))
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}

	UOSInt dataSize;
	const UInt8 *imgData = req->GetReqData(&dataSize);
	if (imgData == 0 || dataSize < 100 || dataSize > 104857600)
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}

	me->dataMut->LockWrite();
	Int32 ret = me->UserfileAdd(env.user->id, env.user->unorganSpId, sb.ToString(), imgData, dataSize);
	me->dataMut->UnlockWrite();

	if (ret == 0)
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_ACCEPTABLE);
		return true;
	}
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	resp->SetStatusCode(Net::WebStatus::SC_OK);
	resp->AddDefHeaders(req);
	sptr = Text::StrInt32(sbuff, ret);
	resp->AddContentLength((UOSInt)(sptr - sbuff));
	resp->AddContentType((const UTF8Char*)"text/plain");
	resp->Write(sbuff, (UOSInt)(sptr - sbuff));
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcSearchInside(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 id;
	Int32 cateId;
	const UTF8Char *searchStr;
	req->ParseHTTPForm();
	if (req->GetQueryValueI32((const UTF8Char*)"id", &id) &&
		req->GetQueryValueI32((const UTF8Char*)"cateId", &cateId) &&
		(searchStr = req->GetHTTPFormStr((const UTF8Char *)"searchStr")) != 0)
	{
		const UTF8Char *txt;
		IO::MemoryStream *mstm;
		IO::Writer *writer;
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
		SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
		UTF8Char sbuff[512];
		IO::ConfigFile *lang = me->LangGet(req);
		me->dataMut->LockRead();
		group = me->groupMap->Get(id);
		Bool notAdmin = (env.user == 0 || env.user->userType != 0);
		if (group == 0 || group->cateId != cateId || (me->GroupIsAdmin(group) && notAdmin))
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->cateMap->Get(group->cateId);
		if (cate == 0 || ((cate->flags & 1) && notAdmin))
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.SvcSearchInside"));
		NEW_CLASS(writer, Text::UTF8Writer(mstm));

		sb.ClearStr();
		sb.Append(cate->chiName);
		sb.Append((const UTF8Char*)" - ");
		sb.Append(group->chiName);
		sb.Append((const UTF8Char*)" ");
		sb.Append(group->engName);
		me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
		writer->Write((const UTF8Char*)"<center><h1>");
		txt = Text::XML::ToNewHTMLText(sb.ToString());
		writer->Write(txt);
		Text::XML::FreeNewText(txt);
		writer->WriteLine((const UTF8Char*)"</h1></center>");

		writer->WriteLine((const UTF8Char*)"<table border=\"0\" width=\"100%\">");
		writer->WriteLine((const UTF8Char*)"<tr>");
		writer->Write((const UTF8Char*)"<td><form method=\"POST\" action=\"searchinside.html?id=");
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.Append((const UTF8Char*)"&cateId=");
		sb.AppendI32(group->cateId);
		writer->Write(sb.ToString());
		writer->Write((const UTF8Char*)"\">");
		writer->Write((const UTF8Char*)"Search inside: <input type=\"text\" name=\"searchStr\"/><input type=\"submit\"/>");
		writer->WriteLine((const UTF8Char*)"</form></td>");
		writer->WriteLine((const UTF8Char*)"</tr>");
		writer->WriteLine((const UTF8Char*)"</table>");

		me->WriteLocator(writer, group, cate);
		writer->WriteLine((const UTF8Char*)"<br/>");
		if (group->descript)
		{
			txt = Text::XML::ToNewHTMLText(group->descript->v);
			writer->Write(txt);
			Text::XML::FreeNewText(txt);
		}
		writer->WriteLine((const UTF8Char*)"<br/>");
		writer->WriteLine((const UTF8Char*)"<hr/>");

		Data::ArrayListDbl speciesIndice;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> speciesObjs;
		Data::ArrayListDbl groupIndice;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupInfo*> groupObjs;
		sb.ClearStr();
		sb.Append(searchStr);
		sb.Trim();
		writer->Write((const UTF8Char*)"Result for \"");
		txt = Text::XML::ToNewHTMLText(sb.ToString());
		writer->Write(txt);
		Text::XML::FreeNewText(txt);
		writer->WriteLine((const UTF8Char*)"\"<br/>");
		me->SearchInGroup(group, sb.ToString(), &speciesIndice, &speciesObjs, &groupIndice, &groupObjs, env.user);

		Bool found = false;

		if (speciesObjs.GetCount() > 0)
		{
			Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> speciesList;
			i = speciesObjs.GetCount();
			if (i > 50)
			{
				j = i - 50;
			}
			else
			{
				j = 0;
			}
			while (i-- > j)
			{
				speciesList.Add(speciesObjs.GetItem(i));
			}
			me->WriteSpeciesTable(writer, &speciesList, env.scnWidth, group->cateId, false);
			if (j > 0)
			{
				Text::TextEnc::URIEncoding::URIEncode(sbuff, searchStr);
				writer->Write((const UTF8Char*)"<a href=");
				sb.ClearStr();
				sb.Append((const UTF8Char*)"searchinsidemores.html?id=");
				sb.AppendI32(group->id);
				sb.Append((const UTF8Char*)"&cateId=");
				sb.AppendI32(group->cateId);
				sb.Append((const UTF8Char*)"&searchStr=");
				sb.Append(sbuff);
				txt = Text::XML::ToNewAttrText(sb.ToString());
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				writer->Write((const UTF8Char*)">More</a><br/>");
			}
			writer->WriteLine((const UTF8Char*)"<hr/>");
			found = true;
		}
		if (groupObjs.GetCount() > 0)
		{
			Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupInfo*> groupList;
			i = groupObjs.GetCount();
			if (i > 50)
			{
				j = i - 50;
			}
			else
			{
				j = 0;
			}
			while (i-- > j)
			{
				groupList.Add(groupObjs.GetItem(i));
			}
			me->WriteGroupTable(writer, &groupList, env.scnWidth, false);
			if (j > 0)
			{
				Text::TextEnc::URIEncoding::URIEncode(sbuff, searchStr);
				writer->Write((const UTF8Char*)"<a href=");
				sb.ClearStr();
				sb.Append((const UTF8Char*)"searchinsidemoreg.html?id=");
				sb.AppendI32(group->id);
				sb.Append((const UTF8Char*)"&cateId=");
				sb.AppendI32(group->cateId);
				sb.Append((const UTF8Char*)"&searchStr=");
				sb.Append(sbuff);
				txt = Text::XML::ToNewAttrText(sb.ToString());
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				writer->Write((const UTF8Char*)">More</a><br/>");
			}
			writer->WriteLine((const UTF8Char*)"<hr/>");
			found = true;
		}
		if (!found)
		{
			writer->WriteLine((const UTF8Char*)"No object found<br/>");
		}
		writer->WriteLine((const UTF8Char*)"<br/>");
		writer->Write((const UTF8Char*)"<a href=\"group.html?id=");
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.Append((const UTF8Char*)"&cateId=");
		sb.AppendI32(group->cateId);
		writer->Write(sb.ToString());
		writer->Write((const UTF8Char*)"\">");
		writer->Write(LangGetValue(lang, (const UTF8Char*)"Back"));
		writer->Write((const UTF8Char*)"</a>");

		me->WriteFooter(writer);
		me->dataMut->UnlockRead();
		ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

		DEL_CLASS(writer);
		DEL_CLASS(mstm);
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcSearchInsideMoreS(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 id;
	UInt32 pageNo;
	Int32 cateId;
	const UTF8Char *searchStr;
	if (req->GetQueryValueI32((const UTF8Char*)"id", &id) &&
		req->GetQueryValueI32((const UTF8Char*)"cateId", &cateId) &&
		req->GetQueryValueU32((const UTF8Char*)"pageNo", &pageNo) &&
		(searchStr = req->GetQueryValue((const UTF8Char *)"searchStr")) != 0)
	{
		const UTF8Char *txt;
		IO::MemoryStream *mstm;
		IO::Writer *writer;
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
		SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
		UTF8Char sbuff[512];
		IO::ConfigFile *lang = me->LangGet(req);
		me->dataMut->LockRead();
		group = me->groupMap->Get(id);
		Bool notAdmin = (env.user == 0 || env.user->userType != 0);
		if (group == 0 || group->cateId != cateId || (me->GroupIsAdmin(group) && notAdmin))
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->cateMap->Get(group->cateId);
		if (cate == 0 || ((cate->flags & 1) && notAdmin))
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.SvcSearchInsideMoreS"));
		NEW_CLASS(writer, Text::UTF8Writer(mstm));

		sb.ClearStr();
		sb.Append(cate->chiName);
		sb.Append((const UTF8Char*)" - ");
		sb.Append(group->chiName);
		sb.Append((const UTF8Char*)" ");
		sb.Append(group->engName);
		me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
		writer->Write((const UTF8Char*)"<center><h1>");
		txt = Text::XML::ToNewHTMLText(sb.ToString());
		writer->Write(txt);
		Text::XML::FreeNewText(txt);
		writer->WriteLine((const UTF8Char*)"</h1></center>");

		writer->WriteLine((const UTF8Char*)"<table border=\"0\" width=\"100%\">");
		writer->WriteLine((const UTF8Char*)"<tr>");
		writer->Write((const UTF8Char*)"<td><form method=\"POST\" action=\"searchinside.html?id=");
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.Append((const UTF8Char*)"&cateId=");
		sb.AppendI32(group->cateId);
		writer->Write(sb.ToString());
		writer->Write((const UTF8Char*)"\">");
		writer->Write((const UTF8Char*)"Search inside: <input type=\"text\" name=\"searchStr\"/><input type=\"submit\"/>");
		writer->WriteLine((const UTF8Char*)"</form></td>");
		writer->WriteLine((const UTF8Char*)"</tr>");
		writer->WriteLine((const UTF8Char*)"</table>");

		me->WriteLocator(writer, group, cate);
		writer->WriteLine((const UTF8Char*)"<br/>");
		if (group->descript)
		{
			txt = Text::XML::ToNewHTMLText(group->descript->v);
			writer->Write(txt);
			Text::XML::FreeNewText(txt);
		}
		writer->WriteLine((const UTF8Char*)"<br/>");
		writer->WriteLine((const UTF8Char*)"<hr/>");

		Data::ArrayListDbl speciesIndice;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> speciesObjs;
		Data::ArrayListDbl groupIndice;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupInfo*> groupObjs;
		sb.ClearStr();
		sb.Append(searchStr);
		sb.Trim();
		writer->Write((const UTF8Char*)"Result for \"");
		txt = Text::XML::ToNewHTMLText(sb.ToString());
		writer->Write(txt);
		Text::XML::FreeNewText(txt);
		writer->WriteLine((const UTF8Char*)"\"<br/>");
		me->SearchInGroup(group, sb.ToString(), &speciesIndice, &speciesObjs, &groupIndice, &groupObjs, env.user);

		Bool found = false;

		if (speciesObjs.GetCount() > 0)
		{
			Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> speciesList;
			j = speciesObjs.GetCount() - pageNo * 50;
			i = j - 50;
			if (i < 0)
			{
				i = 0;
			}
			while (j-- > i)
			{
				speciesList.Add(speciesObjs.GetItem(j));
			}
			me->WriteSpeciesTable(writer, &speciesList, env.scnWidth, group->cateId, false);
			if (pageNo > 0)
			{
				Text::TextEnc::URIEncoding::URIEncode(sbuff, searchStr);
				writer->Write((const UTF8Char*)"<a href=");
				sb.ClearStr();
				sb.Append((const UTF8Char*)"searchinsidemores.html?id=");
				sb.AppendI32(group->id);
				sb.Append((const UTF8Char*)"&cateId=");
				sb.AppendI32(group->cateId);
				sb.Append((const UTF8Char*)"&pageNo=");
				sb.AppendU32(pageNo - 1);
				sb.Append((const UTF8Char*)"&searchStr=");
				sb.Append(sbuff);
				txt = Text::XML::ToNewAttrText(sb.ToString());
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				writer->Write((const UTF8Char*)">Prev</a> ");
			}
			if (i > 0)
			{
				Text::TextEnc::URIEncoding::URIEncode(sbuff, searchStr);
				writer->Write((const UTF8Char*)" <a href=");
				sb.ClearStr();
				sb.Append((const UTF8Char*)"searchinsidemores.html?id=");
				sb.AppendI32(group->id);
				sb.Append((const UTF8Char*)"&cateId=");
				sb.AppendI32(group->cateId);
				sb.Append((const UTF8Char*)"&pageNo=");
				sb.AppendU32(pageNo + 1);
				sb.Append((const UTF8Char*)"&searchStr=");
				sb.Append(sbuff);
				txt = Text::XML::ToNewAttrText(sb.ToString());
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				writer->Write((const UTF8Char*)">Next</a>");
			}
			writer->WriteLine((const UTF8Char*)"<br/><hr/>");
			found = true;
		}
		if (!found)
		{
			writer->WriteLine((const UTF8Char*)"No object found<br/>");
		}
		writer->WriteLine((const UTF8Char*)"<br/>");
		writer->Write((const UTF8Char*)"<a href=\"group.html?id=");
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.Append((const UTF8Char*)"&cateId=");
		sb.AppendI32(group->cateId);
		writer->Write(sb.ToString());
		writer->Write((const UTF8Char*)"\">");
		writer->Write(LangGetValue(lang, (const UTF8Char*)"Back"));
		writer->Write((const UTF8Char*)"</a>");

		me->WriteFooter(writer);
		me->dataMut->UnlockRead();
		ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

		DEL_CLASS(writer);
		DEL_CLASS(mstm);
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcSearchInsideMoreG(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 id;
	UInt32 pageNo;
	Int32 cateId;
	const UTF8Char *searchStr;
	if (req->GetQueryValueI32((const UTF8Char*)"id", &id) &&
		req->GetQueryValueI32((const UTF8Char*)"cateId", &cateId) &&
		req->GetQueryValueU32((const UTF8Char*)"pageNo", &pageNo) &&
		(searchStr = req->GetQueryValue((const UTF8Char *)"searchStr")) != 0)
	{
		const UTF8Char *txt;
		IO::MemoryStream *mstm;
		IO::Writer *writer;
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
		SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
		UTF8Char sbuff[512];
		IO::ConfigFile *lang = me->LangGet(req);
		me->dataMut->LockRead();
		group = me->groupMap->Get(id);
		Bool notAdmin = (env.user == 0 || env.user->userType != 0);
		if (group == 0 || group->cateId != cateId || (me->GroupIsAdmin(group) && notAdmin))
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->cateMap->Get(group->cateId);
		if (cate == 0 || ((cate->flags & 1) && notAdmin))
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.SvcSearchInsideMoreG"));
		NEW_CLASS(writer, Text::UTF8Writer(mstm));

		sb.ClearStr();
		sb.Append(cate->chiName);
		sb.Append((const UTF8Char*)" - ");
		sb.Append(group->chiName);
		sb.Append((const UTF8Char*)" ");
		sb.Append(group->engName);
		me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
		writer->Write((const UTF8Char*)"<center><h1>");
		txt = Text::XML::ToNewHTMLText(sb.ToString());
		writer->Write(txt);
		Text::XML::FreeNewText(txt);
		writer->WriteLine((const UTF8Char*)"</h1></center>");

		writer->WriteLine((const UTF8Char*)"<table border=\"0\" width=\"100%\">");
		writer->WriteLine((const UTF8Char*)"<tr>");
		writer->Write((const UTF8Char*)"<td><form method=\"POST\" action=\"searchinside.html?id=");
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.Append((const UTF8Char*)"&cateId=");
		sb.AppendI32(group->cateId);
		writer->Write(sb.ToString());
		writer->Write((const UTF8Char*)"\">");
		writer->Write((const UTF8Char*)"Search inside: <input type=\"text\" name=\"searchStr\"/><input type=\"submit\"/>");
		writer->WriteLine((const UTF8Char*)"</form></td>");
		writer->WriteLine((const UTF8Char*)"</tr>");
		writer->WriteLine((const UTF8Char*)"</table>");

		me->WriteLocator(writer, group, cate);
		writer->WriteLine((const UTF8Char*)"<br/>");
		if (group->descript)
		{
			txt = Text::XML::ToNewHTMLText(group->descript->v);
			writer->Write(txt);
			Text::XML::FreeNewText(txt);
		}
		writer->WriteLine((const UTF8Char*)"<br/>");
		writer->WriteLine((const UTF8Char*)"<hr/>");

		Data::ArrayListDbl speciesIndice;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> speciesObjs;
		Data::ArrayListDbl groupIndice;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupInfo*> groupObjs;
		sb.ClearStr();
		sb.Append(searchStr);
		sb.Trim();
		writer->Write((const UTF8Char*)"Result for \"");
		txt = Text::XML::ToNewHTMLText(sb.ToString());
		writer->Write(txt);
		Text::XML::FreeNewText(txt);
		writer->WriteLine((const UTF8Char*)"\"<br/>");
		me->SearchInGroup(group, sb.ToString(), &speciesIndice, &speciesObjs, &groupIndice, &groupObjs, env.user);

		Bool found = false;

		if (groupObjs.GetCount() > 0)
		{
			Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupInfo*> groupList;
			j = groupObjs.GetCount() - pageNo * 50;
			i = j - 50;
			if (i < 0)
			{
				i = 0;
			}
			while (j-- > i)
			{
				groupList.Add(groupObjs.GetItem(j));
			}
			me->WriteGroupTable(writer, &groupList, env.scnWidth, false);
			if (pageNo > 0)
			{
				Text::TextEnc::URIEncoding::URIEncode(sbuff, searchStr);
				writer->Write((const UTF8Char*)"<a href=");
				sb.ClearStr();
				sb.Append((const UTF8Char*)"searchinsidemoreg.html?id=");
				sb.AppendI32(group->id);
				sb.Append((const UTF8Char*)"&cateId=");
				sb.AppendI32(group->cateId);
				sb.Append((const UTF8Char*)"&pageNo=");
				sb.AppendU32(pageNo - 1);
				sb.Append((const UTF8Char*)"&searchStr=");
				sb.Append(sbuff);
				txt = Text::XML::ToNewAttrText(sb.ToString());
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				writer->Write((const UTF8Char*)">Prev</a> ");
			}
			if (i > 0)
			{
				Text::TextEnc::URIEncoding::URIEncode(sbuff, searchStr);
				writer->Write((const UTF8Char*)" <a href=");
				sb.ClearStr();
				sb.Append((const UTF8Char*)"searchinsidemoreg.html?id=");
				sb.AppendI32(group->id);
				sb.Append((const UTF8Char*)"&cateId=");
				sb.AppendI32(group->cateId);
				sb.Append((const UTF8Char*)"&pageNo=");
				sb.AppendU32(pageNo + 1);
				sb.Append((const UTF8Char*)"&searchStr=");
				sb.Append(sbuff);
				txt = Text::XML::ToNewAttrText(sb.ToString());
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				writer->Write((const UTF8Char*)">Next</a>");
			}
			writer->WriteLine((const UTF8Char*)"<hr/>");
			found = true;
		}
		if (!found)
		{
			writer->WriteLine((const UTF8Char*)"No object found<br/>");
		}
		writer->WriteLine((const UTF8Char*)"<br/>");
		writer->Write((const UTF8Char*)"<a href=\"group.html?id=");
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.Append((const UTF8Char*)"&cateId=");
		sb.AppendI32(group->cateId);
		writer->Write(sb.ToString());
		writer->Write((const UTF8Char*)"\">");
		writer->Write(LangGetValue(lang, (const UTF8Char*)"Back"));
		writer->Write((const UTF8Char*)"</a>");

		me->WriteFooter(writer);
		me->dataMut->UnlockRead();
		ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

		DEL_CLASS(writer);
		DEL_CLASS(mstm);
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcBookList(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 id;
	if (req->GetQueryValueI32((const UTF8Char*)"id", &id))
	{
		const UTF8Char *txt;
		IO::MemoryStream *mstm;
		IO::Writer *writer;
		Data::Int64Map<BookInfo*> sortBookMap;
		Data::DateTime dt;
		UTF8Char sbuff[32];
		SSWR::OrganMgr::OrganWebHandler::BookInfo *book;
		SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::BookInfo*> *bookList;
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		IO::ConfigFile *lang = me->LangGet(req);
		me->dataMut->LockRead();
		cate = me->cateMap->Get(id);
		if (cate == 0)
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.SvcBookList"));
		NEW_CLASS(writer, Text::UTF8Writer(mstm));

		sb.ClearStr();
		sb.Append(cate->chiName);
		sb.Append((const UTF8Char*)" - ");
		sb.Append((const UTF8Char*)"Book List");
		me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
		writer->Write((const UTF8Char*)"<center><h1>");
		txt = Text::XML::ToNewHTMLText(sb.ToString());
		writer->Write(txt);
		Text::XML::FreeNewText(txt);
		writer->WriteLine((const UTF8Char*)"</h1></center>");

		writer->WriteLine((const UTF8Char*)"<table border=\"0\" width=\"100%\">");
		writer->WriteLine((const UTF8Char*)"<tr>");
		writer->WriteLine((const UTF8Char*)"<td>Book Name</td>");
		writer->WriteLine((const UTF8Char*)"<td>Author</td>");
		writer->WriteLine((const UTF8Char*)"<td>Press</td>");
		writer->WriteLine((const UTF8Char*)"<td>Publish Date</td>");
		writer->WriteLine((const UTF8Char*)"</tr>");

		bookList = me->bookMap->GetValues();
		i = 0;
		j = bookList->GetCount();
		while (i < j)
		{
			book = bookList->GetItem(i);
			sortBookMap.Put(book->publishDate, book);
			i++;
		}

		bookList = sortBookMap.GetValues();
		i = 0;
		j = bookList->GetCount();
		while (i < j)
		{
			book = bookList->GetItem(i);

			writer->WriteLine((const UTF8Char*)"<tr>");
			writer->Write((const UTF8Char*)"<td><a href=\"book.html?id=");
			Text::StrInt32(sbuff, book->id);
			writer->Write(sbuff);
			writer->Write((const UTF8Char*)"&amp;cateId=");
			Text::StrInt32(sbuff, cate->cateId);
			writer->Write(sbuff);
			writer->Write((const UTF8Char*)"\">");
			sb.ClearStr();
			txt = Text::XML::ToNewHTMLText(book->title->v);
			sb.Append(txt);
			sb.Replace((const UTF8Char*)"[i]", (const UTF8Char*)"<i>");
			sb.Replace((const UTF8Char*)"[/i]", (const UTF8Char*)"</i>");
			writer->Write(sb.ToString());
			Text::XML::FreeNewText(txt);
			writer->WriteLine((const UTF8Char*)"</a></td>");
			writer->Write((const UTF8Char*)"<td>");
			txt = Text::XML::ToNewHTMLText(book->author->v);
			writer->Write(txt);
			Text::XML::FreeNewText(txt);
			writer->WriteLine((const UTF8Char*)"</td>");
			writer->Write((const UTF8Char*)"<td>");
			txt = Text::XML::ToNewHTMLText(book->press->v);
			writer->Write(txt);
			Text::XML::FreeNewText(txt);
			writer->WriteLine((const UTF8Char*)"</td>");
			writer->Write((const UTF8Char*)"<td>");
			dt.SetTicks(book->publishDate);
			dt.ToString(sbuff, "yyyy-MM-dd");
			writer->Write(sbuff);
			writer->WriteLine((const UTF8Char*)"</td>");
			writer->WriteLine((const UTF8Char*)"</tr>");

			i++;
		}


		writer->WriteLine((const UTF8Char*)"</table>");
		writer->WriteLine((const UTF8Char*)"<br/>");
		writer->Write((const UTF8Char*)"<a href=");
		txt = Text::XML::ToNewAttrText(cate->dirName);
		writer->Write(txt);
		Text::XML::FreeNewText(txt);
		writer->Write((const UTF8Char*)">");
		writer->Write(LangGetValue(lang, (const UTF8Char*)"Back"));
		writer->Write((const UTF8Char*)"</a>");

		me->WriteFooter(writer);
		me->dataMut->UnlockRead();
		ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

		DEL_CLASS(writer);
		DEL_CLASS(mstm);
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcBook(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 id;
	UInt32 pageNo = 0;
	Int32 cateId;
	req->GetQueryValueU32((const UTF8Char*)"pageNo", &pageNo);
	if (req->GetQueryValueI32((const UTF8Char*)"id", &id) &&
		req->GetQueryValueI32((const UTF8Char*)"cateId", &cateId))
	{
		const UTF8Char *txt;
		IO::MemoryStream *mstm;
		IO::Writer *writer;
		Data::Int64Map<BookInfo*> sortBookMap;
		Data::DateTime dt;
		UTF8Char sbuff[32];
		BookInfo *book;
		SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
		BookSpInfo *bookSp;
		SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species;
		Data::StringUTF8Map<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> speciesMap;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> *speciesList;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> tempList;
		UOSInt i;
		UOSInt j;
		IO::ConfigFile *lang = me->LangGet(req);
		Text::StringBuilderUTF8 sb;
		me->dataMut->LockRead();
		cate = me->cateMap->Get(cateId);
		if (cate == 0)
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		book = me->bookMap->Get(id);
		if (book == 0)
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.SvcBook"));
		NEW_CLASS(writer, Text::UTF8Writer(mstm));

		sb.ClearStr();
		sb.Append(cate->chiName);
		me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
		writer->Write((const UTF8Char*)"<center><h1>");
		txt = Text::XML::ToNewHTMLText(sb.ToString());
		writer->Write(txt);
		Text::XML::FreeNewText(txt);
		writer->WriteLine((const UTF8Char*)"</h1></center>");

		writer->Write((const UTF8Char*)"<b>Book Name:</b> ");
		txt = Text::XML::ToNewHTMLText(book->title->v);
		sb.ClearStr();
		sb.Append(txt);
		Text::XML::FreeNewText(txt);
		sb.Replace((const UTF8Char*)"[i]", (const UTF8Char*)"<i>");
		sb.Replace((const UTF8Char*)"[/i]", (const UTF8Char*)"</i>");
		writer->Write(sb.ToString());
		writer->WriteLine((const UTF8Char*)"<br/>");

		writer->Write((const UTF8Char*)"<b>Author:</b> ");
		txt = Text::XML::ToNewHTMLText(book->author->v);
		writer->Write(txt);
		Text::XML::FreeNewText(txt);
		writer->WriteLine((const UTF8Char*)"<br/>");

		writer->Write((const UTF8Char*)"<b>Press:</b> ");
		txt = Text::XML::ToNewHTMLText(book->press->v);
		writer->Write(txt);
		Text::XML::FreeNewText(txt);
		writer->WriteLine((const UTF8Char*)"<br/>");

		if (book->url)
		{
			writer->Write((const UTF8Char*)"<b>URL:</b> <a href=");
			txt = Text::XML::ToNewAttrText(book->url->v);
			writer->Write(txt);
			Text::XML::FreeNewText(txt);
			writer->Write((const UTF8Char*)">");
			txt = Text::XML::ToNewHTMLText(book->url->v);
			writer->Write(txt);
			Text::XML::FreeNewText(txt);
			writer->WriteLine((const UTF8Char*)"</a><br/>");
		}

		if (env.user && env.user->userType == 0)
		{
			if (me->BookFileExist(book))
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<b>View:</b> <a href=\"bookview.html?id=");
				sb.AppendI32(id);
				sb.Append((const UTF8Char*)"\">here</a><br/>");
				writer->WriteLine(sb.ToString());
			}
		}

		i = 0;
		j = book->species->GetCount();
		while (i < j)
		{
			bookSp = book->species->GetItem(i);
			species = me->spMap->Get(bookSp->speciesId);
			if (species)
			{
				speciesMap.Put(species->sciName->v, species);
			}
			i++;
		}
		UInt32 perPage;
		if (env.isMobile)
		{
			perPage = SP_PER_PAGE_MOBILE;
		}
		else
		{
			perPage = SP_PER_PAGE_DESKTOP;
		}
		writer->WriteLine((const UTF8Char*)"<br/>");
		if (pageNo > 0)
		{
			sb.ClearStr();
			sb.Append((const UTF8Char*)"<a href=\"book.html?id=");
			sb.AppendI32(id);
			sb.Append((const UTF8Char*)"&amp;cateId=");
			sb.AppendI32(cateId);
			sb.Append((const UTF8Char*)"&amp;page=");
			sb.AppendU32(pageNo - 1);
			sb.Append((const UTF8Char*)"\">&lt;");
			sb.Append(LangGetValue(lang, (const UTF8Char*)"Previous"));
			sb.AppendU32(perPage);
			sb.Append(LangGetValue(lang, (const UTF8Char*)"Items"));
			sb.Append((const UTF8Char*)"</a>");
			writer->WriteLine(sb.ToString());
		}
		if ((pageNo + 1) * perPage < speciesMap.GetCount())
		{
			sb.ClearStr();
			sb.Append((const UTF8Char*)"<a href=\"book.html?id=");
			sb.AppendI32(id);
			sb.Append((const UTF8Char*)"&amp;cateId=");
			sb.AppendI32(cateId);
			sb.Append((const UTF8Char*)"&amp;page=");
			sb.AppendU32(pageNo + 1);
			sb.Append((const UTF8Char*)"\">");
			sb.Append(LangGetValue(lang, (const UTF8Char*)"Next"));
			sb.AppendU32(perPage);
			sb.Append(LangGetValue(lang, (const UTF8Char*)"Items"));
			sb.Append((const UTF8Char*)"&gt;</a>");
			writer->WriteLine(sb.ToString());
		}
		speciesList = speciesMap.GetValues();
		i = pageNo * perPage;
		j = i + perPage;
		if (j > speciesList->GetCount())
		{
			j = speciesList->GetCount();
		}
		while (i < j)
		{
			tempList.Add(speciesList->GetItem(i));
			i++;
		}
		writer->WriteLine((const UTF8Char*)"<hr/>");
		me->WriteSpeciesTable(writer, &tempList, env.scnWidth, cateId, false);
		writer->WriteLine((const UTF8Char*)"<hr/>");

		writer->Write((const UTF8Char*)"<a href=\"booklist.html?id=");
		Text::StrInt32(sbuff, cate->cateId);
		writer->Write(sbuff);
		writer->Write((const UTF8Char*)"\">");
		writer->Write((const UTF8Char*)"Book List</a>");

		me->WriteFooter(writer);
		me->dataMut->UnlockRead();
		ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

		DEL_CLASS(writer);
		DEL_CLASS(mstm);
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcBookView(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 id;
	if (req->GetQueryValueI32((const UTF8Char*)"id", &id))
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		BookInfo *book;
		IO::FileStream *fs;
		me->dataMut->LockRead();
		book = me->bookMap->Get(id);
		if (env.user == 0 || env.user->userType != 0)
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_UNAUTHORIZED);
			return true;
		}
		else if (book == 0)
		{
			me->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		sptr = Text::StrConcat(sbuff, me->dataDir);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"BookFile");
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrInt32(sptr, book->id);
		sptr = Text::StrConcat(sptr, (const UTF8Char*)".pdf");
		NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		UInt64 fileLen = fs->GetLength();
		if (fileLen <= 16)
		{
			me->dataMut->UnlockRead();
			DEL_CLASS(fs);
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;

		}

		me->dataMut->UnlockRead();

		resp->AddDefHeaders(req);
		resp->AddContentType((const UTF8Char*)"application/pdf");
		resp->AddContentLength(fileLen);
		UOSInt readSize;
		UInt64 sizeLeft = fileLen;
		while (sizeLeft > 0)
		{
			readSize = fs->Read(sbuff, 512);
			if (readSize <= 0)
			{
				break;
			}

			resp->Write(sbuff, readSize);
			sizeLeft -= readSize;
		}

		DEL_CLASS(fs);
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcLogin(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	IO::MemoryStream *mstm;
	IO::Writer *writer;
	Data::DateTime dt;
	UTF8Char sbuff[128];

	if (env.user)
	{
		resp->RedirectURL(req, (const UTF8Char*)"/", 0);
		return true;
	}
	if (req->GetReqMethod() == Net::WebServer::IWebRequest::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		const UTF8Char *userName = req->GetHTTPFormStr((const UTF8Char*)"userName");
		const UTF8Char *pwd = req->GetHTTPFormStr((const UTF8Char*)"password");
		if (userName && pwd)
		{
			me->PasswordEnc(sbuff, pwd);
			me->dataMut->LockRead();
			env.user = me->userNameMap->Get(userName);
			if (env.user && env.user->pwd->Equals(sbuff))
			{
				me->dataMut->UnlockRead();
				Net::WebServer::IWebSession *sess = me->sessMgr->CreateSession(req, resp);
				Data::DateTime *t;
				Data::ArrayListInt32 *pickObjs;
				NEW_CLASS(t, Data::DateTime());
				NEW_CLASS(pickObjs, Data::ArrayListInt32());
				sess->SetValuePtr("LastUseTime", t);
				sess->SetValuePtr("User", env.user);
				sess->SetValuePtr("PickObjs", pickObjs);
				sess->SetValueInt32("PickObjType", 0);
				sess->EndUse();

				NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.SvcLogin"));
				NEW_CLASS(writer, Text::UTF8Writer(mstm));

				me->dataMut->LockRead();
				me->WriteHeaderPart1(writer, (const UTF8Char*)"Index", env.isMobile);
				writer->WriteLine((const UTF8Char*)"<script type=\"text/javascript\">");
				writer->WriteLine((const UTF8Char*)"function afterLoad()");
				writer->WriteLine((const UTF8Char*)"{");
				writer->WriteLine((const UTF8Char*)"	document.location.replace('/');");
				writer->WriteLine((const UTF8Char*)"}");
				writer->WriteLine((const UTF8Char*)"</script>");
				me->WriteHeaderPart2(writer, 0, (const UTF8Char*)"afterLoad()");
				writer->WriteLine((const UTF8Char*)"Login succeeded");
				me->dataMut->UnlockRead();

				me->WriteFooter(writer);
				ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

				DEL_CLASS(writer);
				DEL_CLASS(mstm);
				return true;
			}
			env.user = 0;
			me->dataMut->UnlockRead();
		}
	}

	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.SvcLogin"));
	NEW_CLASS(writer, Text::UTF8Writer(mstm));

	me->dataMut->LockRead();
	me->WriteHeader(writer, (const UTF8Char*)"Index", env.user, env.isMobile);
	writer->WriteLine((const UTF8Char*)"<center><h1>Login</h1></center>");

	writer->WriteLine((const UTF8Char*)"<form method=\"POST\" action=\"login.html\">");
	writer->WriteLine((const UTF8Char*)"User Name: <input type=\"text\" name=\"userName\"/><br/>");
	writer->WriteLine((const UTF8Char*)"Password: <input type=\"password\" name=\"password\"/><br/>");
	writer->WriteLine((const UTF8Char*)"<input type=\"submit\" /><br/>");
	writer->WriteLine((const UTF8Char*)"</form>");
	me->dataMut->UnlockRead();

	me->WriteFooter(writer);
	ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

	DEL_CLASS(writer);
	DEL_CLASS(mstm);
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcLogout(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	me->sessMgr->DeleteSession(req, resp);
	resp->RedirectURL(req, (const UTF8Char*)"/", 0);
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcReload(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	if (me->reloadPwd)
	{
		IO::MemoryStream *mstm;
		IO::Writer *writer;
		Data::DateTime dt;

		NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.SvcReload"));
		NEW_CLASS(writer, Text::UTF8Writer(mstm));

		me->WriteHeader(writer, (const UTF8Char*)"Reload", env.user, env.isMobile);
		writer->WriteLine((const UTF8Char*)"<center><h1>Reload</h1></center>");

		Bool showPwd = true;;
		if (req->GetReqMethod() == Net::WebServer::IWebRequest::RequestMethod::HTTP_POST)
		{
			const UTF8Char *pwd;
			req->ParseHTTPForm();
			pwd = req->GetHTTPFormStr((const UTF8Char*)"pwd");
			if (pwd)
			{
				if (Text::StrEquals(pwd, me->reloadPwd))
				{
					writer->WriteLine((const UTF8Char*)"Reloaded<br/>");
					showPwd = false;
					me->Reload();
				}
				else
				{
					writer->WriteLine((const UTF8Char*)"Password Error<br/>");
				}
			}
		}
		if (showPwd)
		{
			writer->WriteLine((const UTF8Char*)"<form name=\"pwd\" method=\"POST\" action=\"reload\">");
			writer->WriteLine((const UTF8Char*)"Reload Password:");
			writer->WriteLine((const UTF8Char*)"<input name=\"pwd\" type=\"password\" /><br/>");
			writer->WriteLine((const UTF8Char*)"<input type=\"submit\" />");
			writer->WriteLine((const UTF8Char*)"</form>");
		}

		me->WriteFooter(writer);
		ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

		DEL_CLASS(writer);
		DEL_CLASS(mstm);
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}
}


Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcRestart(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	if (me->reloadPwd)
	{
		IO::MemoryStream *mstm;
		IO::Writer *writer;
		Data::DateTime dt;

		NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR.OrganMgr.OrganWebHandler.SvcRestart"));
		NEW_CLASS(writer, Text::UTF8Writer(mstm));

		me->WriteHeader(writer, (const UTF8Char*)"Restart", env.user, env.isMobile);
		writer->WriteLine((const UTF8Char*)"<center><h1>Restart</h1></center>");

		Bool showPwd = true;;
		if (req->GetReqMethod() == Net::WebServer::IWebRequest::RequestMethod::HTTP_POST)
		{
			const UTF8Char *pwd;
			req->ParseHTTPForm();
			pwd = req->GetHTTPFormStr((const UTF8Char*)"pwd");
			if (pwd)
			{
				if (Text::StrEquals(pwd, me->reloadPwd))
				{
					writer->WriteLine((const UTF8Char*)"Restarting<br/>");
					showPwd = false;
					me->Restart();
				}
				else
				{
					writer->WriteLine((const UTF8Char*)"Password Error<br/>");
				}
			}
		}
		if (showPwd)
		{
			writer->WriteLine((const UTF8Char*)"<form name=\"pwd\" method=\"POST\" action=\"restart\">");
			writer->WriteLine((const UTF8Char*)"Restart Password:");
			writer->WriteLine((const UTF8Char*)"<input name=\"pwd\" type=\"password\" /><br/>");
			writer->WriteLine((const UTF8Char*)"<input type=\"submit\" />");
			writer->WriteLine((const UTF8Char*)"</form>");
		}

		me->WriteFooter(writer);
		ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

		DEL_CLASS(writer);
		DEL_CLASS(mstm);
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcIndex(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	IO::MemoryStream *mstm;
	IO::Writer *writer;
	Data::DateTime dt;
	UTF8Char sbuff[32];

	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.SvcIndex"));
	NEW_CLASS(writer, Text::UTF8Writer(mstm));

	me->WriteHeader(writer, (const UTF8Char*)"Index", env.user, env.isMobile);
	writer->WriteLine((const UTF8Char*)"<center><h1>Index</h1></center>");

	me->dataMut->LockRead();
	Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::CategoryInfo*> *cateList = me->cateMap->GetValues();
	SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
	SSWR::OrganMgr::OrganWebHandler::CategoryInfo *firstCate = 0;
	UOSInt i;
	UOSInt j;
	const UTF8Char *txt;
	Text::StringBuilderUTF8 sb;
	Bool notAdmin = (env.user == 0 || env.user->userType != 0);
	i = 0;
	j = cateList->GetCount();
	while (i < j)
	{
		cate = cateList->GetItem(i);
		if ((cate->flags & 1) == 0 || !notAdmin)
		{
			writer->Write((const UTF8Char*)"<a href=");
			sb.ClearStr();
			sb.Append((const UTF8Char*)"cate.html?cateName=");
			sb.Append(cate->dirName);
			txt = Text::XML::ToNewAttrText(sb.ToString());
			writer->Write(txt);
			writer->Write((const UTF8Char*)">");
			Text::XML::FreeNewText(txt);
			txt = Text::XML::ToNewHTMLText(cate->chiName);
			writer->Write(txt);
			Text::XML::FreeNewText(txt);
			writer->WriteLine((const UTF8Char*)"</a><br/>");

			if (firstCate == 0)
				firstCate = cate;
		}
		i++;
	}
	if (env.user)
	{
		OSInt endIndex = (OSInt)env.user->userFileIndex->GetCount();
		OSInt startIndex;
		Int64 currTime = env.user->userFileIndex->GetItem((UOSInt)endIndex - 1);
		Int64 thisTicks;
		if (endIndex > 0)
		{
			writer->WriteLine((const UTF8Char*)"<hr/>");
			dt.ToUTCTime();
			while (true)
			{
				dt.SetTicks(currTime);
				dt.SetValue(dt.GetYear(), 1, 1, 0, 0, 0, 0);
				thisTicks = dt.ToTicks();
				startIndex = env.user->userFileIndex->SortedIndexOf(thisTicks);
				if (startIndex < 0)
				{
					startIndex = ~startIndex;
				}
				else
				{
					while (startIndex > 0 && env.user->userFileIndex->GetItem((UOSInt)startIndex - 1) == thisTicks)
					{
						startIndex--;
					}
				}
//				printf("Index startIndex = %d, endIndex = %d, currTime = %ld, year = %d\r\n", startIndex, endIndex, currTime, dt.GetYear());
				Text::StrUInt32(sbuff, dt.GetYear());
				writer->Write((const UTF8Char*)"<a href=\"photoyear.html?y=");
				writer->Write(sbuff);
				writer->Write((const UTF8Char*)"\">Year ");
				writer->Write(sbuff);
				writer->WriteLine((const UTF8Char*)"</a><br/>");
				if (startIndex <= 0)
					break;
				endIndex = startIndex;
				currTime = env.user->userFileIndex->GetItem((UOSInt)endIndex - 1);
			}
		}
		writer->WriteLine((const UTF8Char*)"<hr/>");
		writer->WriteLine((const UTF8Char*)"<h3>Photo Upload</h3>");
		writer->WriteLine((const UTF8Char*)"<form name=\"upload\" method=\"POST\" action=\"photoupload.html\" enctype=\"multipart/form-data\">Files:<input type=\"file\" name=\"file\" multiple/>");
		writer->WriteLine((const UTF8Char*)"<input type=\"submit\"></form>");
	}
	writer->WriteLine((const UTF8Char*)"<hr/>");
	writer->Write((const UTF8Char*)"<a href=\"booklist.html?id=");
	sb.ClearStr();
	sb.AppendI32(firstCate->cateId);
	writer->Write(sb.ToString());
	writer->WriteLine((const UTF8Char*)"\">Book List</a><br/>");
	if (env.user == 0)
	{
		writer->Write((const UTF8Char*)"<a href=\"login.html\">Login</a><br/>");
	}
	me->dataMut->UnlockRead();

	me->WriteFooter(writer);
	ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

	DEL_CLASS(writer);
	DEL_CLASS(mstm);
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcCate(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
	const UTF8Char *cateName = req->GetQueryValue((const UTF8Char*)"cateName");
	if (cateName != 0 && (cate = me->cateSMap->Get(cateName)) != 0)
	{
		const UTF8Char *txt;
		IO::MemoryStream *mstm;
		IO::Writer *writer;
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupInfo*> groups;
		IO::ConfigFile *lang = me->LangGet(req);

		NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.SvcCate"));
		NEW_CLASS(writer, Text::UTF8Writer(mstm));

		me->WriteHeader(writer, cate->chiName, env.user, env.isMobile);
		writer->Write((const UTF8Char*)"<center><h1>");
		txt = Text::XML::ToNewHTMLText(cate->chiName);
		writer->Write(txt);
		Text::XML::FreeNewText(txt);
		writer->WriteLine((const UTF8Char*)"</h1></center>");

		Bool notAdmin = (env.user == 0 || env.user->userType != 0);
		i = 0;
		j = cate->groups->GetCount();
		while (i < j)
		{
			group = cate->groups->GetItem(i);
			me->CalcGroupCount(group);
			if (group->totalCount > 0 && ((group->flags & 1) == 0 || !notAdmin))
			{
	/*			sb.ClearStr();
				sb.Append((const UTF8Char*)"<a href=\"group.html?id=");
				sb.AppendI32(group->id);
				sb.Append((const UTF8Char*)"&amp;cateId=");
				sb.AppendI32(group->cateId);
				sb.Append((const UTF8Char*)"\">");
				txt = Text::XML::ToNewHTMLText(group->chiName);
				sb.Append(txt);
				sb.Append((const UTF8Char*)" ");
				Text::XML::FreeNewText(txt);
				txt = Text::XML::ToNewHTMLText(group->engName);
				sb.Append(txt);
				Text::XML::FreeNewText(txt);
				sb.Append((const UTF8Char*)" (");
				sb.AppendOSInt(group->myPhotoCount);
				sb.Append((const UTF8Char*)"/");
				sb.AppendOSInt(group->photoCount);
				sb.Append((const UTF8Char*)"/");
				sb.AppendOSInt(group->totalCount);
				sb.Append((const UTF8Char*)")</a><br/>");
				writer->WriteLine(sb.ToString());*/
				groups.Add(group);
			}
			i++;
		}

	/*	writer->WriteLine((const UTF8Char*)"<br/>");
		writer->Write((const UTF8Char*)"<a href=\"booklist.html?id=");
		sb.ClearStr();
		sb.AppendI32(cate->cateId);
		writer->Write(sb.ToString());
		writer->WriteLine((const UTF8Char*)"\">Book List</a>");*/
		me->WriteLocator(writer, 0, 0);
		writer->WriteLine((const UTF8Char*)"<hr/>");
		me->WriteGroupTable(writer, &groups, env.scnWidth, false);
		writer->WriteLine((const UTF8Char*)"<hr/>");
		writer->Write((const UTF8Char*)"<a href=\"/\">");
		writer->Write(LangGetValue(lang, (const UTF8Char*)"Back"));
		writer->WriteLine((const UTF8Char*)"</a>");
		me->WriteFooter(writer);
		ResponseMstm(req, resp, mstm, (const UTF8Char*)"text/html");

		DEL_CLASS(writer);
		DEL_CLASS(mstm);
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcFavicon(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	return true;
}

void SSWR::OrganMgr::OrganWebHandler::ResponsePhoto(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, SSWR::OrganMgr::OrganWebHandler::WebUserInfo *user, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, const UTF8Char *fileName)
{
	SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
	UTF8Char u8buff[512];
	UTF8Char u8buff2[512];
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	Int32 rotateType = 0;
	UTF8Char *u8ptr;
	this->dataMut->LockRead();
	sp = this->spMap->Get(speciesId);
	Bool notAdmin = (user == 0 || user->userType != 0);
	if (sp && sp->cateId == cateId)
	{
		cate = this->cateMap->Get(sp->cateId);
		if (cate && ((cate->flags & 1) == 0 || !notAdmin))
		{
			Text::StringBuilderUTF8 sb;
			if (this->cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE)
			{
				Text::StrConcat(u8buff, this->cacheDir);
				Text::StrInt32(u8buff2, cate->cateId);
				u8ptr = IO::Path::AppendPath(u8buff, u8buff2);
				*u8ptr++ = IO::Path::PATH_SEPERATOR;
				u8ptr = sp->dirName->ConcatTo(u8ptr);
				IO::Path::CreateDirectory(u8buff);
				*u8ptr++ = IO::Path::PATH_SEPERATOR;
				if (Text::StrStartsWith(fileName, (const UTF8Char*)"web") && (fileName[3] == IO::Path::PATH_SEPERATOR || fileName[3] == '\\'))
				{
					Text::StrConcat(u8ptr, (const UTF8Char*)"web");
					IO::Path::CreateDirectory(u8buff);
					u8ptr[3] = IO::Path::PATH_SEPERATOR;
					Text::StrConcat(Text::StrConcat(&u8ptr[4], &fileName[4]), (const UTF8Char*)".jpg");
				}
				else
				{
					Text::StrConcat(Text::StrConcat(u8ptr, fileName), (const UTF8Char*)".jpg");
				}

				NEW_CLASS(fs, IO::FileStream(u8buff, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
				if (fs->IsError())
				{
					DEL_CLASS(fs);
				}
				else
				{
					UInt8 *buff;
					UOSInt buffSize = (UOSInt)fs->GetLength();
					if (buffSize > 0)
					{
						buff = MemAlloc(UInt8, buffSize);
						fs->Read(buff, buffSize);
						DEL_CLASS(fs);
						resp->AddDefHeaders(req);
						resp->AddContentLength(buffSize);
						resp->AddContentType((const UTF8Char*)"image/jpeg");
						resp->Write(buff, buffSize);
						this->dataMut->UnlockRead();
						MemFree(buff);
						return;
					}
					else
					{
						DEL_CLASS(fs);
					}
				}
			}

			sb.ClearStr();
			sb.Append(cate->srcDir);
			sb.Append(sp->dirName);
			sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
			sb.Append((const UTF8Char*)"setting.txt");
			NEW_CLASS(fs, IO::FileStream(sb.ToString(), IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
			if (!fs->IsError())
			{
				UTF8Char *sarr[3];
				sb.ClearStr();
				sb.Append(fileName);
				sb.Append((const UTF8Char*)".");

				NEW_CLASS(reader, Text::UTF8Reader(fs));
				while (reader->ReadLine(u8buff2, 511))
				{
					if (Text::StrSplit(sarr, 3, u8buff2, '\t') == 2)
					{
						if (Text::StrStartsWithICase(sarr[0], sb.ToString()))
						{
							if (sarr[1][0] == 'R')
							{
								rotateType = Text::StrToInt32(&sarr[1][1]);
								break;
							}
						}
					}
				}
				DEL_CLASS(reader);
			}
			DEL_CLASS(fs);

			sb.ClearStr();
			sb.Append(cate->srcDir);
			sb.Append(sp->dirName);
			sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
			sb.Append(fileName);
			if (IO::Path::PATH_SEPERATOR == '/')
			{
				sb.Replace('\\', '/');
			}
			sb.Append((const UTF8Char*)".jpg");
			this->dataMut->UnlockRead();
			if (IO::Path::GetPathType(sb.ToString()) != IO::Path::PathType::File)
			{
				sb.RemoveChars(4);
				sb.Append((const UTF8Char*)".pcx");
				if (IO::Path::GetPathType(sb.ToString()) != IO::Path::PathType::File)
				{
					sb.RemoveChars(4);
					sb.Append((const UTF8Char*)".tif");
					if (IO::Path::GetPathType(sb.ToString()) != IO::Path::PathType::File)
					{
						sb.RemoveChars(4);
						sb.Append((const UTF8Char*)".png");
					}
				}
			}

			Media::ImageList *imgList;
			Media::StaticImage *simg;
			Media::StaticImage *lrimg;
			Media::StaticImage *dimg;
			IO::StmData::FileData *fd;
			Sync::MutexUsage mutUsage(this->parserMut);
			NEW_CLASS(fd, IO::StmData::FileData(sb.ToString(), false));
			imgList = (Media::ImageList*)this->parsers->ParseFileType(fd, IO::ParserType::ImageList);
			DEL_CLASS(fd);
			mutUsage.EndUse();
			if (imgList)
			{
				simg = imgList->GetImage(0, 0)->CreateStaticImage();
				DEL_CLASS(imgList);
				Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
				NEW_CLASS(lrimg, Media::StaticImage(simg->info->dispWidth, simg->info->dispHeight, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, 0, &color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
				Sync::MutexUsage mutUsage(this->csconvMut);
				if (this->csconv == 0 || this->csconvFCC != simg->info->fourcc || this->csconvBpp != simg->info->storeBPP || this->csconvPF != simg->info->pf || !simg->info->color->Equals(this->csconvColor))
				{
					SDEL_CLASS(this->csconv);
					this->csconvFCC = simg->info->fourcc;
					this->csconvBpp = simg->info->storeBPP;
					this->csconvPF = simg->info->pf;
					this->csconvColor->Set(simg->info->color);
					this->csconv = Media::CS::CSConverter::NewConverter(this->csconvFCC, this->csconvBpp, this->csconvPF, this->csconvColor, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, &color, Media::ColorProfile::YUVT_UNKNOWN, this->colorSess);
				}
				if (this->csconv)
				{
					this->csconv->ConvertV2(&simg->data, lrimg->data, simg->info->dispWidth, simg->info->dispHeight, simg->info->storeWidth, simg->info->storeHeight, (OSInt)lrimg->GetDataBpl(), Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);
				}
				else
				{
					SDEL_CLASS(lrimg);
				}
				mutUsage.EndUse();
				DEL_CLASS(simg);

				if (lrimg)
				{
					LRGBLimiter_LimitImageLRGB(lrimg->data, lrimg->info->dispWidth, lrimg->info->dispHeight);
					Sync::MutexUsage mutUsage(this->resizerMut);
					resizerLR->SetResizeAspectRatio(Media::IImgResizer::RAR_SQUAREPIXEL);
					resizerLR->SetTargetWidth(imgWidth);
					resizerLR->SetTargetHeight(imgHeight);
					dimg = resizerLR->ProcessToNew(lrimg);
					mutUsage.EndUse();
					DEL_CLASS(lrimg)
				}
				else
				{
					dimg = 0;
				}
				if (dimg)
				{
					IO::MemoryStream *mstm;
					UInt8 *buff;
					UOSInt buffSize;
					dimg->info->color->SetRAWICC(Media::ICCProfile::GetSRGBICCData());
					if (rotateType == 1)
					{
						dimg->RotateImage(Media::StaticImage::RT_CW90);
					}
					else if (rotateType == 2)
					{
						dimg->RotateImage(Media::StaticImage::RT_CW180);
					}
					else if (rotateType == 3)
					{
						dimg->RotateImage(Media::StaticImage::RT_CW270);
					}

					if (false)//this->watermark && !Text::StrStartsWith(fileName, (const UTF8Char*)"web") && fileName[3] != IO::Path::PATH_SEPERATOR && fileName[3] != '\\')
					{
/*						Int32 xRand;
						Int32 yRand;
						Int16 fontSize = imgWidth / 12;
						OSInt leng = Text::StrCharCnt(this->watermark);
						Double sz[2];
						Int32 iWidth;
						Int32 iHeight;
						Media::DrawImage *gimg = (Media::DrawImage*)this->eng->ConvImage(dimg);
						Media::DrawImage *gimg2;
						Media::DrawBrush *b = gimg->NewBrushARGB(0xffffffff);
						Media::DrawFont *f;
						while (true)
						{
							f = gimg->NewFontW(L"Arial", fontSize, Media::DrawEngine::DFS_NORMAL);
							if (!gimg->GetTextSize(f, this->watermark, leng, sz))
							{
								gimg->DelFont(f);
								break;
							}
							if (sz[0] <= dimg->info->dispWidth && sz[1] <= dimg->info->dispHeight)
							{
								xRand = Math::Double2Int32(dimg->info->dispWidth - sz[0]);
								yRand = Math::Double2Int32(dimg->info->dispHeight - sz[1]);
								iWidth = Math::Double2Int32(sz[0]);
								iHeight = Math::Double2Int32(sz[1]);
								gimg2 = this->eng->CreateImage32(iWidth, iHeight, Media::AT_NO_ALPHA);
								gimg2->DrawString(0, 0, this->watermark, f, b);
								gimg2->SetAlphaType(Media::AT_ALPHA);
								{
									Bool revOrder;
									UInt8 *bits = gimg2->GetImgBits(&revOrder);
									Int32 col = (this->random->NextInt30() & 0xffffff) | 0x5f808080;
									ImageUtil_ColorReplace32(bits, iWidth, iHeight, col);
								}
								gimg->DrawImagePt(gimg2, Math::Double2Int32(this->random->NextDouble() * xRand), Math::Double2Int32(this->random->NextDouble() * yRand));
								this->eng->DeleteImage(gimg2);
								gimg->DelFont(f);
								break;

							}
							else
							{
								gimg->DelFont(f);
								fontSize--;
							}
						}
						gimg->DelBrush(b);
						NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.WebRequest"));
						gimg->SaveJPG(mstm);
						buff = mstm->GetBuff(&buffSize);
						resp->AddDefHeaders(req);
						resp->AddContentLength(buffSize);
						resp->AddContentType((const UTF8Char*)"image/jpeg");
						resp->Write(buff, buffSize);

						if (this->cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE && buffSize > 0)
						{
							NEW_CLASS(fs, IO::FileStream(u8buff, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
							fs->Write(buff, buffSize);
							DEL_CLASS(fs);
						}

						DEL_CLASS(mstm);
						DEL_CLASS(dimg);
						this->eng->DeleteImage(gimg);*/
					}
					else
					{
						void *param;
						Media::ImageList nimgList((const UTF8Char*)"Temp");
						NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.WebRequest"));
						nimgList.AddImage(dimg, 0);
						Exporter::GUIJPGExporter exporter;
						param = exporter.CreateParam(&nimgList);
						exporter.SetParamInt32(param, 0, 95);
						exporter.ExportFile(mstm, (const UTF8Char*)"", &nimgList, param);
						exporter.DeleteParam(param);
						ResponseMstm(req, resp, mstm, (const UTF8Char*)"image/jpeg");

						if (this->cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE && mstm->GetLength() > 0)
						{
							NEW_CLASS(fs, IO::FileStream(u8buff, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
							buff = mstm->GetBuff(&buffSize);
							fs->Write(buff, buffSize);
							DEL_CLASS(fs);
						}
						DEL_CLASS(mstm);
					}
				}
				else
				{
					resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
					return;
				}
			}
			else
			{
				resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
				return;
			}
		}
		else
		{
			this->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return;
		}
	}
	else
	{
		this->dataMut->UnlockRead();
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return;
	}
}

void SSWR::OrganMgr::OrganWebHandler::ResponsePhotoId(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, SSWR::OrganMgr::OrganWebHandler::WebUserInfo *reqUser, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, Int32 fileId)
{
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
	UTF8Char u8buff2[512];
	UTF8Char u8buff[512];
	UTF8Char *u8ptr;
	IO::FileStream *fs;
	SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile;
	Int32 rotateType = 0;
	this->dataMut->LockRead();
	sp = this->spMap->Get(speciesId);
	userFile = this->userFileMap->Get(fileId);
	if (sp && sp->cateId == cateId && userFile && (userFile->fileType == 1 || userFile->fileType == 3))
	{
		Data::DateTime dt;
		SSWR::OrganMgr::OrganWebHandler::WebUserInfo *user;
		user = this->userMap->Get(userFile->webuserId);
		dt.SetTicks(userFile->fileTimeTicks);
		dt.ToUTCTime();
		rotateType = userFile->rotType;

		Text::StrConcat(u8buff2, this->cacheDir);
		u8ptr = IO::Path::AppendPath(u8buff2, (const UTF8Char*)"UserFile");
		*u8ptr++ = IO::Path::PATH_SEPERATOR;
		u8ptr = Text::StrInt32(u8ptr, userFile->webuserId);
		*u8ptr++ = IO::Path::PATH_SEPERATOR;
		u8ptr = dt.ToString(u8ptr, "yyyyMM");
		IO::Path::CreateDirectory(u8buff2);
		*u8ptr++ = IO::Path::PATH_SEPERATOR;
		userFile->dataFileName->ConcatTo(u8ptr);

		if (this->cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE && userFile->prevUpdated == 0)
		{
			Data::DateTime dt2;
			Data::DateTime dt3;
			if (req->GetIfModifiedSince(&dt2) && IO::Path::GetFileTime(u8buff2, &dt3, 0, 0))
			{
				Int64 tdiff = dt2.ToTicks() - dt3.ToTicks();
				if (tdiff >= -1000 && tdiff <= 1000)
				{
					this->dataMut->UnlockRead();
					resp->ResponseNotModified(req, -1);
					return;
				}
			}
			NEW_CLASS(fs, IO::FileStream(u8buff2, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			UOSInt buffSize = (UOSInt)fs->GetLength();
			if (fs->IsError() || buffSize == 0)
			{
				DEL_CLASS(fs);
			}
			else
			{
				UInt8 *buff;
				buff = MemAlloc(UInt8, buffSize);
				fs->Read(buff, buffSize);
				fs->GetFileTimes(0, 0, &dt2);
				DEL_CLASS(fs);
				resp->AddDefHeaders(req);
				resp->AddContentLength(buffSize);
				resp->AddContentType((const UTF8Char*)"image/jpeg");
				resp->AddLastModified(&dt2);
				resp->Write(buff, buffSize);
				this->dataMut->UnlockRead();
				MemFree(buff);
				return;
			}
		}

		u8ptr = Text::StrConcat(u8buff, this->dataDir);
		if (u8ptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*u8ptr++ = IO::Path::PATH_SEPERATOR;
		}
		u8ptr = Text::StrConcat(u8ptr, (const UTF8Char*)"UserFile");
		*u8ptr++ = IO::Path::PATH_SEPERATOR;
		u8ptr = Text::StrInt32(u8ptr, userFile->webuserId);
		*u8ptr++ = IO::Path::PATH_SEPERATOR;
		u8ptr = dt.ToString(u8ptr, "yyyyMM");
		*u8ptr++ = IO::Path::PATH_SEPERATOR;
		if (userFile->fileType == 3)
		{
			u8ptr = Text::StrInt64(u8ptr, userFile->fileTimeTicks);
			u8ptr = Text::StrConcat(u8ptr, (const UTF8Char*)"_");
			u8ptr = Text::StrHexVal32(u8ptr, userFile->crcVal);
			u8ptr = Text::StrConcat(u8ptr, (const UTF8Char*)".png");
		}
		else
		{
			u8ptr = userFile->dataFileName->ConcatTo(u8ptr);
		}
		this->dataMut->UnlockRead();

		Media::ImageList *imgList;
		Media::StaticImage *simg;
		Media::StaticImage *lrimg;
		Media::StaticImage *dimg;
		IO::StmData::FileData *fd;
		Sync::MutexUsage mutUsage(this->parserMut);
		NEW_CLASS(fd, IO::StmData::FileData(u8buff, false));
		imgList = (Media::ImageList*)this->parsers->ParseFileType(fd, IO::ParserType::ImageList);
		DEL_CLASS(fd);
		mutUsage.EndUse();
		if (imgList)
		{
			simg = imgList->GetImage(0, 0)->CreateStaticImage();
			DEL_CLASS(imgList);
			Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
			NEW_CLASS(lrimg, Media::StaticImage(simg->info->dispWidth, simg->info->dispHeight, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, 0, &color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			Sync::MutexUsage mutUsage(this->csconvMut);
			if (this->csconv == 0 || this->csconvFCC != simg->info->fourcc || this->csconvBpp != simg->info->storeBPP || this->csconvPF != simg->info->pf || !simg->info->color->Equals(this->csconvColor))
			{
				SDEL_CLASS(this->csconv);
				this->csconvFCC = simg->info->fourcc;
				this->csconvBpp = simg->info->storeBPP;
				this->csconvPF = simg->info->pf;
				this->csconvColor->Set(simg->info->color);
				this->csconv = Media::CS::CSConverter::NewConverter(this->csconvFCC, this->csconvBpp, this->csconvPF, this->csconvColor, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, &color, Media::ColorProfile::YUVT_UNKNOWN, this->colorSess);
			}
			if (this->csconv)
			{
				this->csconv->ConvertV2(&simg->data, lrimg->data, simg->info->dispWidth, simg->info->dispHeight, simg->info->storeWidth, simg->info->storeHeight, (OSInt)lrimg->GetDataBpl(), Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);
			}
			else
			{
				SDEL_CLASS(lrimg);
			}
			mutUsage.EndUse();
			DEL_CLASS(simg);

			if (lrimg)
			{
				LRGBLimiter_LimitImageLRGB(lrimg->data, lrimg->info->dispWidth, lrimg->info->dispHeight);
				if (imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE)
				{
					Sync::MutexUsage mutUsage(this->resizerMut);
					resizerLR->SetResizeAspectRatio(Media::IImgResizer::RAR_SQUAREPIXEL);
					resizerLR->SetTargetWidth(imgWidth);
					resizerLR->SetTargetHeight(imgHeight);
					Double x1 = userFile->cropLeft;
					Double y1 = userFile->cropTop;
					Double x2 = Math::UOSInt2Double(lrimg->info->dispWidth) - userFile->cropRight;
					Double y2 = Math::UOSInt2Double(lrimg->info->dispHeight) - userFile->cropBottom;
					if (userFile->cropLeft < 0)
					{
						x1 = 0;
						x2 = Math::UOSInt2Double(lrimg->info->dispWidth) - userFile->cropRight - userFile->cropLeft;
					}
					else if (userFile->cropRight < 0)
					{
						x1 = userFile->cropLeft + userFile->cropRight;
						x2 = Math::UOSInt2Double(lrimg->info->dispWidth);
					}
					if (userFile->cropTop < 0)
					{
						y1 = 0;
						y2 = Math::UOSInt2Double(lrimg->info->dispHeight) - userFile->cropBottom - userFile->cropTop;
					}
					else if (userFile->cropBottom < 0)
					{
						y1 = userFile->cropBottom + userFile->cropTop;
						y2 = Math::UOSInt2Double(lrimg->info->dispHeight);
					}
					dimg = resizerLR->ProcessToNewPartial(lrimg, x1, y1, x2, y2);
					mutUsage.EndUse();
				}
				else
				{
					Sync::MutexUsage mutUsage(this->resizerMut);
					resizerLR->SetResizeAspectRatio(Media::IImgResizer::RAR_SQUAREPIXEL);
					resizerLR->SetTargetWidth(imgWidth);
					resizerLR->SetTargetHeight(imgHeight);
					dimg = resizerLR->ProcessToNew(lrimg);
					mutUsage.EndUse();
				}
				DEL_CLASS(lrimg)
			}
			else
			{
				dimg = 0;
			}
			if (dimg)
			{
				IO::MemoryStream *mstm;
				UInt8 *buff;
				UOSInt buffSize;
				dimg->info->color->SetRAWICC(Media::ICCProfile::GetSRGBICCData());

				if (rotateType == 1)
				{
					dimg->RotateImage(Media::StaticImage::RT_CW90);
				}
				else if (rotateType == 2)
				{
					dimg->RotateImage(Media::StaticImage::RT_CW180);
				}
				else if (rotateType == 3)
				{
					dimg->RotateImage(Media::StaticImage::RT_CW270);
				}

				if (user && user->watermark)
				{
					Media::DrawImage *gimg = this->eng->ConvImage(dimg);
					if ((this->cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE) || user != reqUser)
					{
						Int32 xRand;
						Int32 yRand;
						UInt32 fontSizePx = imgWidth / 12;
						UOSInt leng = user->watermark->leng;
						Double sz[2];
						UInt32 iWidth;
						UInt32 iHeight;
						Media::DrawImage *gimg2;
						Media::DrawBrush *b = gimg->NewBrushARGB(0xffffffff);
						Media::DrawFont *f;
						while (true)
						{
							f = gimg->NewFontPx((const UTF8Char*)"Arial", fontSizePx, Media::DrawEngine::DFS_NORMAL, 0);
							if (!gimg->GetTextSizeC(f, user->watermark->v, leng, sz))
							{
								gimg->DelFont(f);
								break;
							}
							if (sz[0] <= Math::UOSInt2Double(dimg->info->dispWidth) && sz[1] <= Math::UOSInt2Double(dimg->info->dispHeight))
							{
								xRand = Math::Double2Int32(Math::UOSInt2Double(dimg->info->dispWidth) - sz[0]);
								yRand = Math::Double2Int32(Math::UOSInt2Double(dimg->info->dispHeight) - sz[1]);
								iWidth = (UInt32)Math::Double2Int32(sz[0]);
								iHeight = (UInt32)Math::Double2Int32(sz[1]);
								gimg2 = this->eng->CreateImage32(iWidth, iHeight, Media::AT_NO_ALPHA);
								gimg2->DrawString(0, 0, user->watermark->v, f, b);
								gimg2->SetAlphaType(Media::AT_ALPHA);
								{
									Bool revOrder;
									UInt8 *bits = gimg2->GetImgBits(&revOrder);
									UInt32 col = (this->random->NextInt30() & 0xffffff) | 0x5f808080;
									if (bits)
									{
										ImageUtil_ColorReplace32(bits, iWidth, iHeight, col);
									}
								}
								gimg->DrawImagePt(gimg2, Math::Double2Int32(this->random->NextDouble() * xRand), Math::Double2Int32(this->random->NextDouble() * yRand));
								this->eng->DeleteImage(gimg2);
								gimg->DelFont(f);
								break;

							}
							else
							{
								gimg->DelFont(f);
								fontSizePx--;
							}
						}
						gimg->DelBrush(b);
					}	
					NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.WebRequest"));
					gimg->SaveJPG(mstm);
					ResponseMstm(req, resp, mstm, (const UTF8Char*)"image/jpeg");

					if (this->cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE)
					{
						NEW_CLASS(fs, IO::FileStream(u8buff2, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
						buff = mstm->GetBuff(&buffSize);
						fs->Write(buff, buffSize);
						DEL_CLASS(fs);
						if (userFile->prevUpdated)
						{
							this->UserFilePrevUpdated(userFile);
						}
					}

					DEL_CLASS(mstm);
					DEL_CLASS(dimg);
					this->eng->DeleteImage(gimg);
				}
				else
				{
					void *param;
					Media::ImageList nimgList((const UTF8Char*)"Temp");
					NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.WebRequest"));
					nimgList.AddImage(dimg, 0);
					Exporter::GUIJPGExporter exporter;
					param = exporter.CreateParam(&nimgList);
					exporter.SetParamInt32(param, 0, 95);
					exporter.ExportFile(mstm, (const UTF8Char*)"", &nimgList, param);
					exporter.DeleteParam(param);
					ResponseMstm(req, resp, mstm, (const UTF8Char*)"image/jpeg");

					if (this->cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE)
					{
						NEW_CLASS(fs, IO::FileStream(u8buff2, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
						buff = mstm->GetBuff(&buffSize);
						fs->Write(buff, buffSize);
						DEL_CLASS(fs);
						if (userFile->prevUpdated)
						{
							this->UserFilePrevUpdated(userFile);
						}
					}
					DEL_CLASS(mstm);
				}
			}
			else
			{
				resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
				return;
			}
		}
		else
		{
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return;
		}
	}
	else
	{
		this->dataMut->UnlockRead();
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return;
	}
}

void SSWR::OrganMgr::OrganWebHandler::ResponsePhotoWId(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, SSWR::OrganMgr::OrganWebHandler::WebUserInfo *reqUser, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, Int32 fileWId)
{
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
	UTF8Char u8buff2[512];
	UTF8Char u8buff[512];
	UTF8Char *u8ptr;
	IO::FileStream *fs;
	SSWR::OrganMgr::OrganWebHandler::WebFileInfo *wfile;
	Int32 rotateType = 0;
	this->dataMut->LockRead();
	sp = this->spMap->Get(speciesId);
	if (sp && sp->cateId == cateId)
	{
		wfile = sp->wfiles->Get(fileWId);
		if (wfile)
		{
			Data::DateTime dt;

			Text::StrConcat(u8buff2, this->cacheDir);
			u8ptr = IO::Path::AppendPath(u8buff2, (const UTF8Char*)"WebFile");
			*u8ptr++ = IO::Path::PATH_SEPERATOR;
			u8ptr = Text::StrInt32(u8ptr, wfile->id >> 10);
			IO::Path::CreateDirectory(u8buff2);
			*u8ptr++ = IO::Path::PATH_SEPERATOR;
			u8ptr = Text::StrInt32(u8ptr, wfile->id);
			Text::StrConcat(u8ptr, (const UTF8Char*)".jpg");

			if (this->cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE && wfile->prevUpdated == 0)
			{
				NEW_CLASS(fs, IO::FileStream(u8buff2, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
				UOSInt buffSize = (UOSInt)fs->GetLength();
				if (fs->IsError() || buffSize == 0)
				{
					DEL_CLASS(fs);
				}
				else
				{
					UInt8 *buff;
					buff = MemAlloc(UInt8, buffSize);
					fs->Read(buff, buffSize);
					DEL_CLASS(fs);
					resp->AddDefHeaders(req);
					resp->AddContentLength(buffSize);
					resp->AddContentType((const UTF8Char*)"image/jpeg");
					resp->Write(buff, buffSize);
					this->dataMut->UnlockRead();
					MemFree(buff);
					return;
				}
			}

			u8ptr = Text::StrConcat(u8buff, this->dataDir);
			if (u8ptr[-1] != IO::Path::PATH_SEPERATOR)
			{
				*u8ptr++ = IO::Path::PATH_SEPERATOR;
			}
			u8ptr = Text::StrConcat(u8ptr, (const UTF8Char*)"WebFile");
			*u8ptr++ = IO::Path::PATH_SEPERATOR;
			u8ptr = Text::StrInt32(u8ptr, wfile->id >> 10);
			*u8ptr++ = IO::Path::PATH_SEPERATOR;
			u8ptr = Text::StrInt32(u8ptr, wfile->id);
			u8ptr = Text::StrConcat(u8ptr, (const UTF8Char*)".jpg");
			this->dataMut->UnlockRead();

			Media::ImageList *imgList;
			Media::StaticImage *simg;
			Media::StaticImage *lrimg;
			Media::StaticImage *dimg;
			IO::StmData::FileData *fd;
			Sync::MutexUsage mutUsage(this->parserMut);
			NEW_CLASS(fd, IO::StmData::FileData(u8buff, false));
			imgList = (Media::ImageList*)this->parsers->ParseFileType(fd, IO::ParserType::ImageList);
			DEL_CLASS(fd);
			mutUsage.EndUse();
			if (imgList)
			{
				simg = imgList->GetImage(0, 0)->CreateStaticImage();
				DEL_CLASS(imgList);
				Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
				NEW_CLASS(lrimg, Media::StaticImage(simg->info->dispWidth, simg->info->dispHeight, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, 0, &color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
				Sync::MutexUsage mutUsage(this->csconvMut);
				if (this->csconv == 0 || this->csconvFCC != simg->info->fourcc || this->csconvBpp != simg->info->storeBPP || this->csconvPF != simg->info->pf || !simg->info->color->Equals(this->csconvColor))
				{
					SDEL_CLASS(this->csconv);
					this->csconvFCC = simg->info->fourcc;
					this->csconvBpp = simg->info->storeBPP;
					this->csconvPF = simg->info->pf;
					this->csconvColor->Set(simg->info->color);
					this->csconv = Media::CS::CSConverter::NewConverter(this->csconvFCC, this->csconvBpp, this->csconvPF, this->csconvColor, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, &color, Media::ColorProfile::YUVT_UNKNOWN, this->colorSess);
				}
				if (this->csconv)
				{
					this->csconv->ConvertV2(&simg->data, lrimg->data, simg->info->dispWidth, simg->info->dispHeight, simg->info->storeWidth, simg->info->storeHeight, (OSInt)lrimg->GetDataBpl(), Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);
				}
				else
				{
					SDEL_CLASS(lrimg);
				}
				mutUsage.EndUse();
				DEL_CLASS(simg);

				if (lrimg)
				{
					LRGBLimiter_LimitImageLRGB(lrimg->data, lrimg->info->dispWidth, lrimg->info->dispHeight);
					if (imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE)
					{
						Sync::MutexUsage mutUsage(this->resizerMut);
						resizerLR->SetResizeAspectRatio(Media::IImgResizer::RAR_SQUAREPIXEL);
						resizerLR->SetTargetWidth(imgWidth);
						resizerLR->SetTargetHeight(imgHeight);
						Double x1 = wfile->cropLeft;
						Double y1 = wfile->cropTop;
						Double x2 = Math::UOSInt2Double(lrimg->info->dispWidth) - wfile->cropRight;
						Double y2 = Math::UOSInt2Double(lrimg->info->dispHeight) - wfile->cropBottom;
						if (wfile->cropLeft < 0)
						{
							x1 = 0;
							x2 = Math::UOSInt2Double(lrimg->info->dispWidth) - wfile->cropRight - wfile->cropLeft;
						}
						else if (wfile->cropRight < 0)
						{
							x1 = wfile->cropLeft + wfile->cropRight;
							x2 = Math::UOSInt2Double(lrimg->info->dispWidth);
						}
						if (wfile->cropTop < 0)
						{
							y1 = 0;
							y2 = Math::UOSInt2Double(lrimg->info->dispHeight) - wfile->cropBottom - wfile->cropTop;
						}
						else if (wfile->cropBottom < 0)
						{
							y1 = wfile->cropBottom + wfile->cropTop;
							y2 = Math::UOSInt2Double(lrimg->info->dispHeight);
						}
						dimg = resizerLR->ProcessToNewPartial(lrimg, x1, y1, x2, y2);
						mutUsage.EndUse();
					}
					else
					{
						Sync::MutexUsage mutUsage(this->resizerMut);
						resizerLR->SetResizeAspectRatio(Media::IImgResizer::RAR_SQUAREPIXEL);
						resizerLR->SetTargetWidth(imgWidth);
						resizerLR->SetTargetHeight(imgHeight);
						dimg = resizerLR->ProcessToNew(lrimg);
						mutUsage.EndUse();
					}
					DEL_CLASS(lrimg)
				}
				else
				{
					dimg = 0;
				}
				if (dimg)
				{
					IO::MemoryStream *mstm;
					UInt8 *buff;
					UOSInt buffSize;
					dimg->info->color->SetRAWICC(Media::ICCProfile::GetSRGBICCData());

					if (rotateType == 1)
					{
						dimg->RotateImage(Media::StaticImage::RT_CW90);
					}
					else if (rotateType == 2)
					{
						dimg->RotateImage(Media::StaticImage::RT_CW180);
					}
					else if (rotateType == 3)
					{
						dimg->RotateImage(Media::StaticImage::RT_CW270);
					}

					void *param;
					Media::ImageList nimgList((const UTF8Char*)"Temp");
					NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR::OrganMgr::OrganWebHandler.WebRequest"));
					nimgList.AddImage(dimg, 0);
					Exporter::GUIJPGExporter exporter;
					param = exporter.CreateParam(&nimgList);
					exporter.SetParamInt32(param, 0, 95);
					exporter.ExportFile(mstm, (const UTF8Char*)"", &nimgList, param);
					exporter.DeleteParam(param);
					ResponseMstm(req, resp, mstm, (const UTF8Char*)"image/jpeg");

					if (this->cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE)
					{
						NEW_CLASS(fs, IO::FileStream(u8buff2, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
						buff = mstm->GetBuff(&buffSize);
						fs->Write(buff, buffSize);
						DEL_CLASS(fs);
						if (wfile->prevUpdated)
						{
							this->WebFilePrevUpdated(wfile);
						}
					}
					DEL_CLASS(mstm);
				}
				else
				{
					resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
					return;
				}
			}
			else
			{
				resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
				return;
			}
		}
		else
		{
			this->dataMut->UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return;
		}
	}
	else
	{
		this->dataMut->UnlockRead();
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return;
	}
}

void SSWR::OrganMgr::OrganWebHandler::ResponseMstm(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, IO::MemoryStream *mstm, const UTF8Char *contType)
{
	resp->AddDefHeaders(req);
	resp->AddContentType(contType);
	mstm->SeekFromBeginning(0);
	Net::WebServer::HTTPServerUtil::SendContent(req, resp, contType, mstm->GetLength(), mstm);
}

void SSWR::OrganMgr::OrganWebHandler::WriteHeaderPart1(IO::Writer *writer, const UTF8Char *title, Bool isMobile)
{
	const UTF8Char *txt;
	writer->WriteLine((const UTF8Char*)"<HTML>");
	writer->WriteLine((const UTF8Char*)"<HEAD>");
	writer->WriteLine((const UTF8Char*)"<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=utf8\">");
	if (isMobile)
	{
		writer->WriteLine((const UTF8Char*)"<meta name=\"viewport\" content=\"width=1024\">");
	}
	writer->Write((const UTF8Char*)"<title>");
	txt = Text::XML::ToNewHTMLText(title);
	writer->Write(txt);
	Text::XML::FreeNewText(txt);
	writer->WriteLine((const UTF8Char*)"</title>");
}

void SSWR::OrganMgr::OrganWebHandler::WriteHeaderPart2(IO::Writer *writer, SSWR::OrganMgr::OrganWebHandler::WebUserInfo *user, const UTF8Char *onLoadFunc)
{
	const UTF8Char *txt;
	writer->WriteLine((const UTF8Char*)"</HEAD>");
	writer->WriteLine();
	writer->Write((const UTF8Char*)"<BODY TEXT=\"#c0e0ff\" LINK=\"#6080ff\" VLINK=\"#4060ff\" ALINK=\"#4040FF\" bgcolor=\"#000000\"");
	if (onLoadFunc)
	{
		writer->Write((const UTF8Char*)" onLoad=");
		txt = Text::XML::ToNewAttrText(onLoadFunc);
		writer->Write(txt);
		Text::XML::FreeNewText(txt);
	}
	writer->WriteLine((const UTF8Char*)">");
	if (user)
	{
		writer->Write((const UTF8Char*)"<p align=\"right\">");
		txt = Text::XML::ToNewHTMLText(user->userName->v);
		writer->Write(txt);
		Text::XML::FreeNewText(txt);
		writer->WriteLine((const UTF8Char*)"<a href=\"logout\">Logout</a></p>");
	}
}

void SSWR::OrganMgr::OrganWebHandler::WriteHeader(IO::Writer *writer, const UTF8Char *title, SSWR::OrganMgr::OrganWebHandler::WebUserInfo *user, Bool isMobile)
{
	this->WriteHeaderPart1(writer, title, isMobile);
	this->WriteHeaderPart2(writer, user, 0);
}

void SSWR::OrganMgr::OrganWebHandler::WriteFooter(IO::Writer *writer)
{
	writer->WriteLine((const UTF8Char*)"</BODY>");
	writer->WriteLine((const UTF8Char*)"</HTML>");
}

void SSWR::OrganMgr::OrganWebHandler::WriteLocator(IO::Writer *writer, SSWR::OrganMgr::OrganWebHandler::GroupInfo *group, SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate)
{
	SSWR::OrganMgr::OrganWebHandler::GroupTypeInfo *grpType;
	const UTF8Char *txt;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[12];
	Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupInfo *> groupList;
	UOSInt i;
	while (group)
	{
		groupList.Add(group);
		group = this->groupMap->Get(group->parentId);
	}


	writer->WriteLine((const UTF8Char*)"<center><table border=\"0\" cellpadding=\"0\" cellspacing=\"4\">");
	writer->Write((const UTF8Char*)"<tr><td></td><td></td><td><a href=\"/\">Index");
	writer->Write((const UTF8Char*)"</a></td></tr>");
	if (cate)
	{
		writer->Write((const UTF8Char*)"<tr><td></td><td></td><td><a href=");
		sb.ClearStr();
		sb.Append((const UTF8Char*)"cate.html?cateName=");
		sb.Append(cate->dirName);
		txt = Text::XML::ToNewAttrText(sb.ToString());
		writer->Write(txt);
		Text::XML::FreeNewText(txt);
		writer->Write((const UTF8Char*)">");
		txt = Text::XML::ToNewHTMLText(cate->chiName);
		writer->Write(txt);
		Text::XML::FreeNewText(txt);
		writer->Write((const UTF8Char*)"</a></td></tr>");
	}

	i = groupList.GetCount();
	while (i-- > 0)
	{
		group = groupList.GetItem(i);
		grpType = cate->groupTypes->Get(group->groupType);
		writer->WriteLine((const UTF8Char*)"<tr>");
		if (grpType)
		{
			writer->Write((const UTF8Char*)"<td>");
			txt = Text::XML::ToNewHTMLText(grpType->chiName);
			writer->Write(txt);
			Text::XML::FreeNewText(txt);
			writer->WriteLine((const UTF8Char*)"</td>");
			writer->Write((const UTF8Char*)"<td>");
			txt = Text::XML::ToNewHTMLText(grpType->engName);
			writer->Write(txt);
			Text::XML::FreeNewText(txt);
			writer->WriteLine((const UTF8Char*)"</td>");
		}
		else
		{
			writer->WriteLine((const UTF8Char*)"<td>?</td>");
			writer->WriteLine((const UTF8Char*)"<td>?</td>");
		}
		writer->Write((const UTF8Char*)"<td><a href=\"group.html?id=");
		Text::StrInt32(sbuff, group->id);
		writer->Write(sbuff);
		writer->Write((const UTF8Char*)"&amp;cateId=");
		Text::StrInt32(sbuff, group->cateId);
		writer->Write(sbuff);
		writer->Write((const UTF8Char*)"\">");
		sb.ClearStr();
		sb.Append(group->engName);
		sb.Append((const UTF8Char*)" ");
		sb.Append(group->chiName);
		txt = Text::XML::ToNewHTMLText(sb.ToString());
		writer->Write(txt);
		Text::XML::FreeNewText(txt);
		writer->WriteLine((const UTF8Char*)"</a></td>");
		writer->WriteLine((const UTF8Char*)"</tr>");

	}
	writer->WriteLine((const UTF8Char*)"</table>");
	WriteLocatorText(writer, group, cate);
	writer->WriteLine((const UTF8Char*)"</center>");
}

void SSWR::OrganMgr::OrganWebHandler::WriteLocatorText(IO::Writer *writer, SSWR::OrganMgr::OrganWebHandler::GroupInfo *group, SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate)
{
	Text::StringBuilderUTF8 sb;
	Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupInfo *> groupList;
	UOSInt i;
	Bool found = false;
	while (group)
	{
		groupList.Add(group);
		if (group->groupType == 21)
		{
			found = true;
			break;
		}
		group = this->groupMap->Get(group->parentId);
	}

	if (!found)
		return;

	found = false;
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Text Form: ");
	i = groupList.GetCount();
	while (i-- > 0)
	{
		if (found)
		{
			sb.Append((const UTF8Char*)", ");
		}
		group = groupList.GetItem(i);
		sb.Append(group->engName);
		found = true;
	}
	sb.Append((const UTF8Char*)"<br/>");
	writer->WriteLine(sb.ToString());
}

void SSWR::OrganMgr::OrganWebHandler::WriteGroupTable(IO::Writer *writer, Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupInfo *> *groupList, UInt32 scnWidth, Bool showSelect)
{
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
	const UTF8Char *txt;
	UTF8Char sbuff[512];
	Text::StringBuilderUTF8 sb;
	UInt32 colCount = scnWidth / PREVIEW_SIZE;
	UInt32 colWidth = 100 / colCount;
	UInt32 currColumn;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = groupList->GetCount();
	if (j > 0)
	{
		writer->WriteLine((const UTF8Char*)"<table border=\"0\" width=\"100%\">");
		currColumn = 0;
		while (i < j)
		{
			group = groupList->GetItem(i);
			this->CalcGroupCount(group);
			if (group->totalCount != 0 || showSelect)
			{
				if (currColumn == 0)
				{
					writer->WriteLine((const UTF8Char*)"<tr>");
				}
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<td width=\"");
				sb.AppendU32(colWidth);
				sb.Append((const UTF8Char*)"%\">");
				writer->WriteLine(sb.ToString(), sb.GetCharCnt());
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<center><a href=\"group.html?id=");
				sb.AppendI32(group->id);
				sb.Append((const UTF8Char*)"&amp;cateId=");
				sb.AppendI32(group->cateId);
				sb.Append((const UTF8Char*)"\">");
				writer->WriteLine(sb.ToString(), sb.GetCharCnt());

				if (group->photoSpObj && (group->photoSpObj->photo != 0 || group->photoSpObj->photoId != 0 || group->photoSpObj->photoWId != 0))
				{
					if (group->photoSpObj->photoId != 0)
					{
						writer->Write((const UTF8Char*)"<img src=");
						sb.ClearStr();
						sb.Append((const UTF8Char*)"photo.html?id=");
						sb.AppendI32(group->photoSpObj->speciesId);
						sb.Append((const UTF8Char*)"&cateId=");
						sb.AppendI32(group->cateId);
						sb.Append((const UTF8Char*)"&width=");
						sb.AppendI32(PREVIEW_SIZE);
						sb.Append((const UTF8Char*)"&height=");
						sb.AppendI32(PREVIEW_SIZE);
						sb.Append((const UTF8Char*)"&fileId=");
						sb.AppendI32(group->photoSpObj->photoId);
						txt = Text::XML::ToNewAttrText(sb.ToString());
						writer->Write(txt);
						Text::XML::FreeNewText(txt);
						writer->Write((const UTF8Char*)" border=\"0\" ALT=");
						txt = Text::XML::ToNewAttrText(group->engName->v);
						writer->Write(txt);
						Text::XML::FreeNewText(txt);
						writer->WriteLine((const UTF8Char*)"><br/>");
					}
					else if (group->photoSpObj->photoWId != 0)
					{
						writer->Write((const UTF8Char*)"<img src=");
						sb.ClearStr();
						sb.Append((const UTF8Char*)"photo.html?id=");
						sb.AppendI32(group->photoSpObj->speciesId);
						sb.Append((const UTF8Char*)"&cateId=");
						sb.AppendI32(group->cateId);
						sb.Append((const UTF8Char*)"&width=");
						sb.AppendI32(PREVIEW_SIZE);
						sb.Append((const UTF8Char*)"&height=");
						sb.AppendI32(PREVIEW_SIZE);
						sb.Append((const UTF8Char*)"&fileWId=");
						sb.AppendI32(group->photoSpObj->photoWId);
						txt = Text::XML::ToNewAttrText(sb.ToString());
						writer->Write(txt);
						Text::XML::FreeNewText(txt);
						writer->Write((const UTF8Char*)" border=\"0\" ALT=");
						txt = Text::XML::ToNewAttrText(group->engName->v);
						writer->Write(txt);
						Text::XML::FreeNewText(txt);
						writer->WriteLine((const UTF8Char*)"><br/>");
					}
					else
					{
						writer->Write((const UTF8Char*)"<img src=");
						sb.ClearStr();
						sb.Append((const UTF8Char*)"photo.html?id=");
						sb.AppendI32(group->photoSpObj->speciesId);
						sb.Append((const UTF8Char*)"&cateId=");
						sb.AppendI32(group->cateId);
						sb.Append((const UTF8Char*)"&width=");
						sb.AppendI32(PREVIEW_SIZE);
						sb.Append((const UTF8Char*)"&height=");
						sb.AppendI32(PREVIEW_SIZE);
						sb.Append((const UTF8Char*)"&file=");
						Text::TextEnc::URIEncoding::URIEncode(sbuff, group->photoSpObj->photo->v);
						sb.Append(sbuff);
						txt = Text::XML::ToNewAttrText(sb.ToString());
						writer->Write(txt);
						Text::XML::FreeNewText(txt);
						writer->Write((const UTF8Char*)" border=\"0\" ALT=");
						txt = Text::XML::ToNewAttrText(group->engName->v);
						writer->Write(txt);
						Text::XML::FreeNewText(txt);
						writer->WriteLine((const UTF8Char*)"><br/>");
					}
				}
				else
				{
					txt = Text::XML::ToNewHTMLText(group->engName->v);
					writer->Write(txt);
					Text::XML::FreeNewText(txt);
				}
				if (showSelect)
				{
					writer->WriteLine((const UTF8Char*)"</a>");
					
					sb.ClearStr();
					sb.Append((const UTF8Char*)"<input type=\"checkbox\" name=\"group");
					sb.AppendI32(group->id);
					sb.Append((const UTF8Char*)"\" id=\"group");
					sb.AppendI32(group->id);
					sb.Append((const UTF8Char*)"\" value=\"1\"/><label for=\"group");
					sb.AppendI32(group->id);
					sb.Append((const UTF8Char*)"\">");
					writer->Write(sb.ToString());
					sb.ClearStr();
					sb.Append(group->chiName);
					sb.Append((const UTF8Char*)" ");
					sb.Append(group->engName);
					sb.Append((const UTF8Char*)" (");
					sb.AppendUOSInt(group->myPhotoCount);
					sb.Append((const UTF8Char*)"/");
					sb.AppendUOSInt(group->photoCount);
					sb.Append((const UTF8Char*)"/");
					sb.AppendUOSInt(group->totalCount);
					sb.Append((const UTF8Char*)")");
					txt = Text::XML::ToNewHTMLText(sb.ToString());
					writer->Write(txt);
					writer->WriteLine((const UTF8Char*)"</label></center></td>");
					Text::XML::FreeNewText(txt);
				}
				else
				{
					sb.ClearStr();
					sb.Append(group->chiName);
					sb.Append((const UTF8Char*)" ");
					sb.Append(group->engName);
					sb.Append((const UTF8Char*)" (");
					sb.AppendUOSInt(group->myPhotoCount);
					sb.Append((const UTF8Char*)"/");
					sb.AppendUOSInt(group->photoCount);
					sb.Append((const UTF8Char*)"/");
					sb.AppendUOSInt(group->totalCount);
					sb.Append((const UTF8Char*)")");
					txt = Text::XML::ToNewHTMLText(sb.ToString());
					writer->Write(txt);
					writer->WriteLine((const UTF8Char*)"</a></center></td>");
					Text::XML::FreeNewText(txt);
				}

				currColumn++;
				if (currColumn >= colCount)
				{
					writer->WriteLine((const UTF8Char*)"</tr>");
					currColumn = 0;
				}
			}
			i++;
		}
		if (currColumn != 0)
		{
			sb.ClearStr();
			sb.Append((const UTF8Char*)"<td width=\"");
			sb.AppendU32(colWidth);
			sb.Append((const UTF8Char*)"%\"></td>");
			while (currColumn < colCount)
			{
				writer->WriteLine(sb.ToString(), sb.GetCharCnt());
				currColumn++;
			}
			writer->WriteLine((const UTF8Char*)"</tr>");
		}
		writer->WriteLine((const UTF8Char*)"</table>");
	}
}

void SSWR::OrganMgr::OrganWebHandler::WriteSpeciesTable(IO::Writer *writer, Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *> *spList, UInt32 scnWidth, Int32 cateId, Bool showSelect)
{
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
	const UTF8Char *txt;
	UTF8Char sbuff[512];
	Text::StringBuilderUTF8 sb;
	UInt32 colCount = scnWidth / PREVIEW_SIZE;
	UInt32 colWidth = 100 / colCount;
	UInt32 currColumn;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = spList->GetCount();
	if (j > 0)
	{
		writer->WriteLine((const UTF8Char*)"<table border=\"0\" width=\"100%\">");
		currColumn = 0;
		while (i < j)
		{
			sp = spList->GetItem(i);
			if (currColumn == 0)
			{
				writer->WriteLine((const UTF8Char*)"<tr>");
			}
			sb.ClearStr();
			sb.Append((const UTF8Char*)"<td width=\"");
			sb.AppendU32(colWidth);
			sb.Append((const UTF8Char*)"%\">");
			writer->WriteLine(sb.ToString());
			sb.ClearStr();
			if (sp->cateId == cateId || showSelect)
			{
				sb.Append((const UTF8Char*)"<center><a href=\"species.html?id=");
				sb.AppendI32(sp->speciesId);
				sb.Append((const UTF8Char*)"&amp;cateId=");
				sb.AppendI32(sp->cateId);
				sb.Append((const UTF8Char*)"\">");
				writer->WriteLine(sb.ToString());
			}
			else
			{
				writer->WriteLine((const UTF8Char*)"<center>");
			}

			if (sp->photoId != 0)
			{
				writer->Write((const UTF8Char*)"<img src=");
				sb.ClearStr();
				sb.Append((const UTF8Char*)"photo.html?id=");
				sb.AppendI32(sp->speciesId);
				sb.Append((const UTF8Char*)"&cateId=");
				sb.AppendI32(sp->cateId);
				sb.Append((const UTF8Char*)"&width=");
				sb.AppendI32(PREVIEW_SIZE);
				sb.Append((const UTF8Char*)"&height=");
				sb.AppendI32(PREVIEW_SIZE);
				sb.Append((const UTF8Char*)"&fileId=");
				sb.AppendI32(sp->photoId);
				txt = Text::XML::ToNewAttrText(sb.ToString());
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				writer->Write((const UTF8Char*)" border=\"0\" ALT=");
				txt = Text::XML::ToNewAttrText(sp->sciName->v);
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				writer->WriteLine((const UTF8Char*)"><br/>");
			}
			else if (sp->photoWId)
			{
				writer->Write((const UTF8Char*)"<img src=");
				sb.ClearStr();
				sb.Append((const UTF8Char*)"photo.html?id=");
				sb.AppendI32(sp->speciesId);
				sb.Append((const UTF8Char*)"&cateId=");
				sb.AppendI32(sp->cateId);
				sb.Append((const UTF8Char*)"&width=");
				sb.AppendI32(PREVIEW_SIZE);
				sb.Append((const UTF8Char*)"&height=");
				sb.AppendI32(PREVIEW_SIZE);
				sb.Append((const UTF8Char*)"&fileWId=");
				sb.AppendI32(sp->photoWId);
				txt = Text::XML::ToNewAttrText(sb.ToString());
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				writer->Write((const UTF8Char*)" border=\"0\" ALT=");
				txt = Text::XML::ToNewAttrText(sp->sciName->v);
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				writer->WriteLine((const UTF8Char*)"><br/>");
			}
			else if (sp->photo)
			{
				writer->Write((const UTF8Char*)"<img src=");
				sb.ClearStr();
				sb.Append((const UTF8Char*)"photo.html?id=");
				sb.AppendI32(sp->speciesId);
				sb.Append((const UTF8Char*)"&cateId=");
				sb.AppendI32(sp->cateId);
				sb.Append((const UTF8Char*)"&width=");
				sb.AppendI32(PREVIEW_SIZE);
				sb.Append((const UTF8Char*)"&height=");
				sb.AppendI32(PREVIEW_SIZE);
				sb.Append((const UTF8Char*)"&file=");
				Text::TextEnc::URIEncoding::URIEncode(sbuff, sp->photo->v);
				sb.Append(sbuff);
				txt = Text::XML::ToNewAttrText(sb.ToString());
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				writer->Write((const UTF8Char*)" border=\"0\" ALT=");
				txt = Text::XML::ToNewAttrText(sp->sciName->v);
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				writer->WriteLine((const UTF8Char*)"><br/>");
			}
			else
			{
				txt = Text::XML::ToNewHTMLText(sp->sciName->v);
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				writer->WriteLine((const UTF8Char*)"<br/>");
			}
			if (showSelect)
			{
				writer->WriteLine((const UTF8Char*)"</a>");
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<input type=\"checkbox\" name=\"species");
				sb.AppendI32(sp->speciesId);
				sb.Append((const UTF8Char*)"\" id=\"species");
				sb.AppendI32(sp->speciesId);
				sb.Append((const UTF8Char*)"\" value=\"1\"/><label for=\"species");
				sb.AppendI32(sp->speciesId);
				sb.Append((const UTF8Char*)"\">");
				writer->Write(sb.ToString());

				sb.ClearStr();
				sb.Append(sp->sciName);
				sb.Append((const UTF8Char*)" ");
				sb.Append(sp->chiName);
				sb.Append((const UTF8Char*)" ");
				sb.Append(sp->engName);
				txt = Text::XML::ToNewHTMLText(sb.ToString());
				writer->Write(txt);
				writer->WriteLine((const UTF8Char*)"</label>");
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<br/><a href=\"speciesmod.html?id=");
				sb.AppendI32(sp->groupId);
				sb.Append((const UTF8Char*)"&amp;cateId=");
				sb.AppendI32(sp->cateId);
				sb.Append((const UTF8Char*)"&amp;spId=");
				sb.AppendI32(sp->speciesId);
				sb.Append((const UTF8Char*)"\">Modify</a>");
				writer->WriteLine(sb.ToString());
				writer->WriteLine((const UTF8Char*)"</center></td>");
			}
			else
			{
				sb.ClearStr();
				sb.Append(sp->sciName);
				sb.Append((const UTF8Char*)" ");
				sb.Append(sp->chiName);
				sb.Append((const UTF8Char*)" ");
				sb.Append(sp->engName);
				txt = Text::XML::ToNewHTMLText(sb.ToString());
				writer->Write(txt);
				if (sp->cateId == cateId)
				{
					writer->WriteLine((const UTF8Char*)"</a></center></td>");
				}
				else
				{
					writer->WriteLine((const UTF8Char*)"</center></td>");
				}
			}
			Text::XML::FreeNewText(txt);

			currColumn++;
			if (currColumn >= colCount)
			{
				writer->WriteLine((const UTF8Char*)"</tr>");
				currColumn = 0;
			}
			i++;
		}
		if (currColumn != 0)
		{
			sb.ClearStr();
			sb.Append((const UTF8Char*)"<td width=\"");
			sb.AppendU32(colWidth);
			sb.Append((const UTF8Char*)"%\"></td>");
			while (currColumn < colCount)
			{
				writer->WriteLine(sb.ToString());
				currColumn++;
			}
			writer->WriteLine((const UTF8Char*)"</tr>");
		}
		writer->WriteLine((const UTF8Char*)"</table>");
	}
}

void SSWR::OrganMgr::OrganWebHandler::WritePickObjs(IO::Writer *writer, SSWR::OrganMgr::OrganWebHandler::RequestEnv *env, const UTF8Char *url)
{
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;
	const UTF8Char *txt;
	UInt32 colCount = env->scnWidth / PREVIEW_SIZE;
	UInt32 colWidth = 100 / colCount;
	UInt32 currColumn;
	SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile;
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species;
	Data::DateTime dt;
	UTF8Char sbuff2[64];
	if (env->pickObjType == POT_USERFILE && env->pickObjs->GetCount() > 0)
	{
		currColumn = 0;
		sb.ClearStr();
		sb.Append((const UTF8Char*)"<form name=\"pickfiles\" action=");
		txt = Text::XML::ToNewAttrText(url);
		sb.Append(txt);
		Text::XML::FreeNewText(txt);
		sb.Append((const UTF8Char*)" method=\"POST\"/>");
		writer->WriteLine(sb.ToString());
		writer->WriteLine((const UTF8Char*)"<input type=\"hidden\" name=\"action\" value=\"place\"/>");
		writer->WriteLine((const UTF8Char*)"<table border=\"0\" width=\"100%\">");

		i = 0;
		j = env->pickObjs->GetCount();
		while (i < j)
		{
			userFile = this->userFileMap->Get(env->pickObjs->GetItem(i));
			species = 0;
			if (userFile) species = this->spMap->Get(userFile->speciesId);
			if (userFile && species)
			{
				if (currColumn == 0)
				{
					writer->WriteLine((const UTF8Char*)"<tr>");
				}
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<td width=\"");
				sb.AppendU32(colWidth);
				sb.Append((const UTF8Char*)"%\">");
				writer->WriteLine(sb.ToString());
				sb.ClearStr();
				sb.Append((const UTF8Char*)"photodetail.html?id=");
				sb.AppendI32(userFile->speciesId);
				sb.Append((const UTF8Char*)"&cateId=");
				sb.AppendI32(species->cateId);
				sb.Append((const UTF8Char*)"&fileId=");
				sb.AppendI32(userFile->id);
				txt = Text::XML::ToNewAttrText(sb.ToString());
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<center><a href=");
				sb.Append(txt);
				sb.Append((const UTF8Char*)">");
				writer->WriteLine(sb.ToString());
				Text::XML::FreeNewText(txt);

				writer->Write((const UTF8Char*)"<img src=");
				sb.ClearStr();
				sb.Append((const UTF8Char*)"photo.html?id=");
				sb.AppendI32(species->speciesId);
				sb.Append((const UTF8Char*)"&cateId=");
				sb.AppendI32(species->cateId);
				sb.Append((const UTF8Char*)"&width=");
				sb.AppendI32(PREVIEW_SIZE);
				sb.Append((const UTF8Char*)"&height=");
				sb.AppendI32(PREVIEW_SIZE);
				sb.Append((const UTF8Char*)"&fileId=");
				sb.AppendI32(userFile->id);
				txt = Text::XML::ToNewAttrText(sb.ToString());
				writer->Write(txt);
				Text::XML::FreeNewText(txt);
				writer->Write((const UTF8Char*)" border=\"0\">");
				writer->Write((const UTF8Char*)"</a><br/>");
				dt.SetTicks(userFile->fileTimeTicks);
				dt.ToLocalTime();

				sb.ClearStr();
				sb.Append((const UTF8Char*)"<input type=\"checkbox\" id=\"userfile");
				sb.AppendI32(userFile->id);
				sb.Append((const UTF8Char*)"\" name=\"userfile");
				sb.AppendI32(userFile->id);
				sb.Append((const UTF8Char*)"\" value=\"1\"/><label for=\"userfile");
				sb.AppendI32(userFile->id);
				sb.Append((const UTF8Char*)"\">");
				writer->Write(sb.ToString());

				dt.ToString(sbuff2, "yyyy-MM-dd HH:mm:ss zzzz");
				writer->Write(sbuff2);
				if (userFile->webuserId == env->user->id)
				{
					if (userFile->location)
					{
						writer->Write((const UTF8Char*)" ");
						txt = Text::XML::ToNewHTMLText(userFile->location->v);
						writer->Write(txt);
						Text::XML::FreeNewText(txt);
					}
				}
				if (userFile->descript && userFile->descript->leng > 0)
				{
					writer->Write((const UTF8Char*)"<br/>");
					txt = Text::XML::ToNewHTMLText(userFile->descript->v);
					writer->Write(txt);
					Text::XML::FreeNewText(txt);
				}
				if (userFile->webuserId == env->user->id)
				{
					writer->Write((const UTF8Char*)"<br/>");
					txt = Text::XML::ToNewHTMLText(userFile->oriFileName->v);
					writer->Write(txt);
					Text::XML::FreeNewText(txt);
				}
				writer->Write((const UTF8Char*)"</label>");
				if (userFile->lat != 0 || userFile->lon != 0)
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)"<br/>");
					sb.Append((const UTF8Char*)"<a href=\"https://www.google.com/maps/place/");
					Text::SBAppendF64(&sb, userFile->lat);
					sb.Append((const UTF8Char*)",");
					Text::SBAppendF64(&sb, userFile->lon);
					sb.Append((const UTF8Char*)"/@");
					Text::SBAppendF64(&sb, userFile->lat);
					sb.Append((const UTF8Char*)",");
					Text::SBAppendF64(&sb, userFile->lon);
					sb.Append((const UTF8Char*)",19z\">");
					sb.Append((const UTF8Char*)"Google Map</a>");
					writer->Write(sb.ToString());
				}
				writer->WriteLine((const UTF8Char*)"</center></td>");

				currColumn++;
				if (currColumn >= colCount)
				{
					writer->WriteLine((const UTF8Char*)"</tr>");
					currColumn = 0;
				}
			}

			i++;
		}

		if (currColumn != 0)
		{
			sb.ClearStr();
			sb.Append((const UTF8Char*)"<td width=\"");
			sb.AppendU32(colWidth);
			sb.Append((const UTF8Char*)"%\"></td>");
			while (currColumn < colCount)
			{
				writer->WriteLine(sb.ToString());
				currColumn++;
			}
			writer->WriteLine((const UTF8Char*)"</tr>");
		}
		writer->WriteLine((const UTF8Char*)"</table>");
		writer->WriteLine((const UTF8Char*)"<input type=\"submit\" value=\"Place Selected\"/>");
		writer->WriteLine((const UTF8Char*)"<input type=\"button\" value=\"Place All\" onclick=\"document.forms.pickfiles.action.value='placeall';document.forms.pickfiles.submit();\"/>");
		writer->WriteLine((const UTF8Char*)"</form>");
		writer->WriteLine((const UTF8Char*)"<hr/>");
	}
	else if (env->pickObjType == POT_SPECIES && env->pickObjs->GetCount() > 0)
	{
		sb.ClearStr();
		sb.Append((const UTF8Char*)"<form name=\"pickfiles\" action=");
		txt = Text::XML::ToNewAttrText(url);
		sb.Append(txt);
		Text::XML::FreeNewText(txt);
		sb.Append((const UTF8Char*)" method=\"POST\"/>");
		writer->WriteLine(sb.ToString());
		writer->WriteLine((const UTF8Char*)"<input type=\"hidden\" name=\"action\" value=\"place\"/>");
		SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> spList;
		i = 0;
		j = env->pickObjs->GetCount();
		while (i < j)
		{
			species = this->spMap->Get(env->pickObjs->GetItem(i));
			if (species)
			{
				spList.Add(species);
			}
			i++;
		}
		WriteSpeciesTable(writer, &spList, scnSize, 0, true);
		writer->WriteLine((const UTF8Char*)"<input type=\"submit\" value=\"Place Selected\"/>");
		writer->WriteLine((const UTF8Char*)"<input type=\"button\" value=\"Place All\" onclick=\"document.forms.pickfiles.action.value='placeall';document.forms.pickfiles.submit();\"/>");
		writer->WriteLine((const UTF8Char*)"</form>");
		writer->WriteLine((const UTF8Char*)"<hr/>");
	}
	else if (env->pickObjType == POT_GROUP && env->pickObjs->GetCount() > 0)
	{
		sb.ClearStr();
		sb.Append((const UTF8Char*)"<form name=\"pickfiles\" action=");
		txt = Text::XML::ToNewAttrText(url);
		sb.Append(txt);
		Text::XML::FreeNewText(txt);
		sb.Append((const UTF8Char*)" method=\"POST\"/>");
		writer->WriteLine(sb.ToString());
		writer->WriteLine((const UTF8Char*)"<input type=\"hidden\" name=\"action\" value=\"place\"/>");
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupInfo*> groupList;
		i = 0;
		j = env->pickObjs->GetCount();
		while (i < j)
		{
			group = this->groupMap->Get(env->pickObjs->GetItem(i));
			if (group)
			{
				groupList.Add(group);
			}
			i++;
		}
		WriteGroupTable(writer, &groupList, scnSize, true);
		writer->WriteLine((const UTF8Char*)"<input type=\"submit\" value=\"Place Selected\"/>");
		writer->WriteLine((const UTF8Char*)"<input type=\"button\" value=\"Place All\" onclick=\"document.forms.pickfiles.action.value='placeall';document.forms.pickfiles.submit();\"/>");
		writer->WriteLine((const UTF8Char*)"</form>");
		writer->WriteLine((const UTF8Char*)"<hr/>");
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::OnSessionDel(Net::WebServer::IWebSession* sess, void *userObj)
{
	Data::DateTime *t;
	Data::ArrayListInt32 *pickObjs;
	t = (Data::DateTime *)sess->GetValuePtr("LastUseTime");
	pickObjs = (Data::ArrayListInt32*)sess->GetValuePtr("PickObjs");
	DEL_CLASS(t);
	DEL_CLASS(pickObjs);
	return false;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::OnSessionCheck(Net::WebServer::IWebSession* sess, void *userObj)
{
	Data::DateTime *t;
	Data::DateTime currTime;
	t = (Data::DateTime*)sess->GetValuePtr("LastUseTime");
	currTime.SetCurrTimeUTC();
	if (currTime.DiffMS(t) >= 1800000)
		return true;
	return false;
}

IO::ConfigFile *SSWR::OrganMgr::OrganWebHandler::LangGet(Net::WebServer::IWebRequest *req)
{
	Text::StringBuilderUTF8 sb;
	IO::ConfigFile *lang;
	UTF8Char *sarr[2];
	UTF8Char *sarr2[2];
	UOSInt i;
	Text::Locale::LocaleEntry *ent;
	if (req->GetHeader(&sb, (const UTF8Char*)"Accept-Language"))
	{
		sarr[1] = sb.ToString();
		i = 2;
		while (i == 2)
		{
			i = Text::StrSplit(sarr, 2, sarr[1], ',');
			Text::StrSplit(sarr2, 2, sarr[0], ';');
			ent = this->locale->GetLocaleEntryByName(sarr2[0]);
			if (ent)
			{
				lang = this->langMap->Get(ent->lcid);
				if (lang)
					return lang;
			}
		}
	}
	lang = this->langMap->Get(0x409);
	if (lang)
		return lang;
	return this->langMap->GetValues()->GetItem(0);
}

const UTF8Char *SSWR::OrganMgr::OrganWebHandler::LangGetValue(IO::ConfigFile *lang, const UTF8Char *name)
{
	if (lang == 0)
		return name;
	const UTF8Char *csptr = lang->GetValue(name);
	if (csptr)
		return csptr;
	return name;
}

SSWR::OrganMgr::OrganWebHandler::OrganWebHandler(Net::SocketFactory *sockf, Net::SSLEngine *ssl, IO::LogTool *log, DB::DBTool *db, const UTF8Char *imageDir, UInt16 port, const UTF8Char *cacheDir, const UTF8Char *dataDir, UInt32 scnSize, const UTF8Char *reloadPwd, Int32 unorganizedGroupId, Media::DrawEngine *eng)
{
	this->imageDir = Text::StrCopyNew(imageDir);
	this->sockf = sockf;
	this->ssl = ssl;
	this->log = log;
	this->scnSize = scnSize;
	this->dataDir = Text::StrCopyNew(dataDir);
	this->unorganizedGroupId = unorganizedGroupId;
	NEW_CLASS(this->random, Data::RandomOS());
	if (cacheDir)
	{
		this->cacheDir = Text::StrCopyNew(cacheDir);
	}
	else
	{
		this->cacheDir = 0;
	}
	if (reloadPwd)
	{
		this->reloadPwd = Text::StrCopyNew(reloadPwd);
	}
	else
	{
		this->reloadPwd = 0;
	}

	Media::ColorProfile destProfile(Media::ColorProfile::CPT_SRGB);
	NEW_CLASS(this->parsers, Parser::FullParserList());
	NEW_CLASS(this->parserMut, Sync::Mutex());
	NEW_CLASS(this->colorMgr, Media::ColorManager());
	this->colorSess = this->colorMgr->CreateSess(0);
	NEW_CLASS(this->resizerMut, Sync::Mutex());
	NEW_CLASS(this->resizerLR, Media::Resizer::LanczosResizerLR_C32(3, 3, &destProfile, this->colorSess, Media::AT_NO_ALPHA, 0, Media::PF_B8G8R8A8));
	NEW_CLASS(this->csconvMut, Sync::Mutex());
	this->csconv = 0;
	NEW_CLASS(this->csconvColor, Media::ColorProfile(Media::ColorProfile::CPT_SRGB));
	this->csconvFCC = 0;
	this->csconvBpp = 0;
	this->csconvPF = Media::PF_UNKNOWN;
	this->eng = eng;

	NEW_CLASS(this->cateMap, Data::Int32Map<SSWR::OrganMgr::OrganWebHandler::CategoryInfo*>());
	NEW_CLASS(this->cateSMap, Data::StringUTF8Map<SSWR::OrganMgr::OrganWebHandler::CategoryInfo*>());
	NEW_CLASS(this->dataMut, Sync::RWMutex());
	NEW_CLASS(this->spMap, Data::Int32Map<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*>());
	NEW_CLASS(this->spNameMap, Data::StringUTF8Map<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*>());
	NEW_CLASS(this->groupMap, Data::Int32Map<SSWR::OrganMgr::OrganWebHandler::GroupInfo*>());
	NEW_CLASS(this->bookMap, Data::Int32Map<SSWR::OrganMgr::OrganWebHandler::BookInfo*>());
	NEW_CLASS(this->userMap, Data::Int32Map<SSWR::OrganMgr::OrganWebHandler::WebUserInfo*>());
	NEW_CLASS(this->userNameMap, Data::StringUTF8Map<SSWR::OrganMgr::OrganWebHandler::WebUserInfo*>());
	NEW_CLASS(this->userFileMap, Data::Int32Map<SSWR::OrganMgr::OrganWebHandler::UserFileInfo*>());
	NEW_CLASS(this->langMap, Data::UInt32Map<IO::ConfigFile*>());
	NEW_CLASS(this->locMap, Data::Int32Map<SSWR::OrganMgr::OrganWebHandler::LocationInfo*>());
	NEW_CLASS(this->sessMgr, Net::WebServer::MemoryWebSessionManager((const UTF8Char*)"/", OnSessionDel, this, 30000, OnSessionCheck, this));
	NEW_CLASS(this->locale, Text::Locale());

	this->db = db;
	if (this->db == 0)
	{
		this->listener = 0;
		return;
	}
	this->LoadLangs();

	if (port == 0)
	{
		this->listener = 0;
	}
	else
	{
		this->AddService((const UTF8Char*)"/photo.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcPhoto);
		this->AddService((const UTF8Char*)"/photodown.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcPhotoDown);
		this->AddService((const UTF8Char*)"/group.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcGroup);
		this->AddService((const UTF8Char*)"/group.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_POST, SvcGroup);
		this->AddService((const UTF8Char*)"/groupmod.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcGroupMod);
		this->AddService((const UTF8Char*)"/groupmod.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_POST, SvcGroupMod);
		this->AddService((const UTF8Char*)"/species.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcSpecies);
		this->AddService((const UTF8Char*)"/species.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_POST, SvcSpecies);
		this->AddService((const UTF8Char*)"/speciesmod.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcSpeciesMod);
		this->AddService((const UTF8Char*)"/speciesmod.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_POST, SvcSpeciesMod);
		this->AddService((const UTF8Char*)"/list.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcList);
		this->AddService((const UTF8Char*)"/listimage.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcList);
		this->AddService((const UTF8Char*)"/photodetail.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcPhotoDetail);
		this->AddService((const UTF8Char*)"/photodetail.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_POST, SvcPhotoDetail);
		this->AddService((const UTF8Char*)"/photodetaild.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcPhotoDetailD);
		this->AddService((const UTF8Char*)"/photoyear.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcPhotoYear);
		this->AddService((const UTF8Char*)"/photoday.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcPhotoDay);
		this->AddService((const UTF8Char*)"/photoupload.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_POST, SvcPhotoUpload);
		this->AddService((const UTF8Char*)"/photouploadd.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_POST, SvcPhotoUploadD);
		this->AddService((const UTF8Char*)"/searchinside.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_POST, SvcSearchInside);
		this->AddService((const UTF8Char*)"/searchinsidemores.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcSearchInsideMoreS);
		this->AddService((const UTF8Char*)"/searchinsidemoreg.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcSearchInsideMoreG);
		this->AddService((const UTF8Char*)"/booklist.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcBookList);
		this->AddService((const UTF8Char*)"/book.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcBook);
		this->AddService((const UTF8Char*)"/bookview.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcBookView);
		this->AddService((const UTF8Char*)"/login.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcLogin);
		this->AddService((const UTF8Char*)"/login.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_POST, SvcLogin);
		this->AddService((const UTF8Char*)"/logout", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcLogout);
		this->AddService((const UTF8Char*)"/reload", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcReload);
		this->AddService((const UTF8Char*)"/reload", Net::WebServer::IWebRequest::RequestMethod::HTTP_POST, SvcReload);
		this->AddService((const UTF8Char*)"/restart", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcRestart);
		this->AddService((const UTF8Char*)"/restart", Net::WebServer::IWebRequest::RequestMethod::HTTP_POST, SvcRestart);
		this->AddService((const UTF8Char*)"/", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcIndex);
		this->AddService((const UTF8Char*)"/index.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcIndex);
		this->AddService((const UTF8Char*)"/cate.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcCate);
		this->AddService((const UTF8Char*)"/favicon.ico", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SvcFavicon);

		NEW_CLASS(this->listener, Net::WebServer::WebListener(this->sockf, this->ssl, this, port, 30, 10, (const UTF8Char*)"OrganWeb/1.0", false, true));
		this->Reload();
	}
}

SSWR::OrganMgr::OrganWebHandler::~OrganWebHandler()
{
	SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
	Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::CategoryInfo*> *cateList;
	Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupTypeInfo*> *grpTypeList;
	Data::ArrayList<IO::ConfigFile*> *langList;
	Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::LocationInfo*> *locList;
	SSWR::OrganMgr::OrganWebHandler::GroupTypeInfo *grpType;
	SSWR::OrganMgr::OrganWebHandler::LocationInfo *loc;
	IO::ConfigFile *lang;
	UOSInt i;
	UOSInt j;

	SDEL_CLASS(this->listener);
	SDEL_CLASS(this->db);
	SDEL_CLASS(this->sessMgr);

	FreeGroups();
	FreeSpecies();
	FreeBooks();
	FreeUsers();
	DEL_CLASS(this->spMap);
	DEL_CLASS(this->spNameMap);
	DEL_CLASS(this->groupMap);
	DEL_CLASS(this->bookMap);
	DEL_CLASS(this->userMap);
	DEL_CLASS(this->userNameMap);
	DEL_CLASS(this->userFileMap);
	DEL_CLASS(this->dataMut);

	cateList = this->cateMap->GetValues();
	i = cateList->GetCount();
	while (i-- > 0)
	{
		cate = cateList->GetItem(i);
		Text::StrDelNew(cate->chiName);
		Text::StrDelNew(cate->dirName);
		Text::StrDelNew(cate->srcDir);
		DEL_CLASS(cate->groups);

		grpTypeList = cate->groupTypes->GetValues();
		j = grpTypeList->GetCount();
		while (j-- > 0)
		{
			grpType = grpTypeList->GetItem(j);
			Text::StrDelNew(grpType->chiName);
			Text::StrDelNew(grpType->engName);
			MemFree(grpType);
		}
		DEL_CLASS(cate->groupTypes);

		MemFree(cate);
	}
	DEL_CLASS(this->cateMap);
	DEL_CLASS(this->cateSMap);

	langList = this->langMap->GetValues();
	i = langList->GetCount();
	while (i-- > 0)
	{
		lang = langList->GetItem(i);
		DEL_CLASS(lang);
	}
	DEL_CLASS(this->langMap);

	locList = this->locMap->GetValues();
	i = locList->GetCount();
	while (i-- > 0)
	{
		loc = locList->GetItem(i);
		SDEL_STRING(loc->cname);
		SDEL_STRING(loc->ename);
		MemFree(loc);
	}
	DEL_CLASS(this->locMap);
	DEL_CLASS(this->parsers);
	DEL_CLASS(this->resizerLR);
	DEL_CLASS(this->resizerMut);
	DEL_CLASS(this->csconvMut);
	SDEL_CLASS(this->csconv);
	DEL_CLASS(this->csconvColor);
	this->colorMgr->DeleteSess(this->colorSess);
	DEL_CLASS(this->colorMgr);
	DEL_CLASS(this->parserMut);
	DEL_CLASS(this->eng);
	DEL_CLASS(this->locale);

	Text::StrDelNew(this->imageDir);
	Text::StrDelNew(this->dataDir);
	DEL_CLASS(this->random);
	SDEL_TEXT(this->cacheDir);
	SDEL_TEXT(this->reloadPwd);
}

Bool SSWR::OrganMgr::OrganWebHandler::IsError()
{
	if (this->listener == 0)
		return true;
	if (this->listener->IsError())
		return true;
	return false;
}

void SSWR::OrganMgr::OrganWebHandler::Reload()
{
	this->dataMut->LockWrite();
	this->LoadCategory();
	this->LoadLocations();
	this->LoadSpecies();
	this->LoadGroups();
	this->LoadBooks();
	this->LoadUsers();
	this->dataMut->UnlockWrite();
}

void SSWR::OrganMgr::OrganWebHandler::Restart()
{
	///////////////////////////
}
