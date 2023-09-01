#include "Stdafx.h"
#include "Crypto/Hash/CRC32R.h"
#include "Crypto/Hash/CRC32RIEEE.h"
#include "Crypto/Hash/MD5.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "DB/DBReader.h"
#include "IO/IniFile.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Media/FrequencyGraph.h"
#include "Media/ImageList.h"
#include "Media/MediaFile.h"
#include "Net/HTTPClient.h"
#include "SSWR/OrganWeb/OrganWebEnv.h"

void SSWR::OrganWeb::OrganWebEnv::LoadLangs()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	IO::Path::FindFileSession *sess;
	UInt32 langId;
	IO::Path::PathType pt;
	UOSInt i;
	IO::ConfigFile *lang;

	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("Langs"));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
	if (sess)
	{
		while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
		{
			if (pt == IO::Path::PathType::File)
			{
//				printf("Found file %s\r\n", sptr);
				i = (UOSInt)(sptr2 - sptr);
				if (i > 4 && Text::StrEqualsC(&sptr[i - 4], 4, UTF8STRC(".txt")))
				{
					sptr[i - 4] = 0;
					langId = Text::StrToUInt32(sptr);
					sptr[i - 4] = '.';
//					printf("LangId = %d\r\n", langId);
					if (langId)
					{
						lang = IO::IniFile::Parse(CSTRP(sbuff, sptr), 65001);
						lang = this->langMap.Put(langId, lang);
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

void SSWR::OrganWeb::OrganWebEnv::LoadCategory()
{
	Text::StringBuilderUTF8 sb;
	CategoryInfo *cate;
	GroupTypeInfo *grpType;
	Int32 cateId;
	UOSInt i;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	DB::DBReader *r = this->db->ExecuteReader(CSTR("select cate_id, chi_name, dirName, srcDir, flags from category"));
	if (r != 0)
	{
		Text::StringBuilderUTF8 sb;
		while (r->ReadNext())
		{
			cateId = r->GetInt32(0);
			cate = this->cateMap.Get(cateId);
			if (cate == 0)
			{
				NEW_CLASS(cate, CategoryInfo());
				cate->cateId = cateId;
				cate->chiName = r->GetNewStrB(1, sb);
				cate->dirName = r->GetNewStrB(2, sb);
				sb.ClearStr();
				r->GetStr(3, sb);
				sptr = this->imageDir->ConcatTo(sbuff);
				sptr = IO::Path::AppendPath(sbuff, sptr, sb.ToCString());
				if (sptr[-1] != IO::Path::PATH_SEPERATOR)
				{
					sptr[0] = IO::Path::PATH_SEPERATOR;
					sptr[1] = 0;
				}
				cate->srcDir = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
				cate->flags = r->GetInt32(4);
				this->cateMap.Put(cate->cateId, cate);
				this->cateSMap.Put(cate->dirName, cate);
			}
			else
			{
				i = cate->groupTypes.GetCount();
				while (i-- > 0)
				{
					grpType = cate->groupTypes.GetItem(i);
					grpType->chiName->Release();
					grpType->engName->Release();
					MemFree(grpType);
				}
				cate->groupTypes.Clear();
			}
		}
		this->db->CloseReader(r);
	}
	r = this->db->ExecuteReader(CSTR("select seq, eng_name, chi_name, cate_id from group_type"));
	if (r != 0)
	{
		while (r->ReadNext())
		{
			cateId = r->GetInt32(3);
			cate = this->cateMap.Get(cateId);
			if (cate == 0)
			{
			}
			else
			{
				grpType = MemAlloc(GroupTypeInfo, 1);
				grpType->id = r->GetInt32(0);
				grpType->engName = r->GetNewStrB(1, sb);
				grpType->chiName = r->GetNewStrB(2, sb);
				cate->groupTypes.Put(grpType->id, grpType);
			}
		}
		this->db->CloseReader(r);
	}

}

void SSWR::OrganWeb::OrganWebEnv::LoadSpecies()
{
	FreeSpecies();

	SpeciesInfo *sp;
	WebFileInfo *wfile;
	Text::StringBuilderUTF8 sb;
	DB::DBReader *r = this->db->ExecuteReader(CSTR("select id, eng_name, chi_name, sci_name, group_id, description, dirName, photo, idKey, cate_id, flags, photoId, photoWId, poiImg from species"));
	if (r != 0)
	{
		while (r->ReadNext())
		{
			NEW_CLASS(sp, SpeciesInfo());
			sp->speciesId = r->GetInt32(0);
			sp->engName = r->GetNewStrBNN(1, sb);
			sp->chiName = r->GetNewStrBNN(2, sb);
			sp->sciName = r->GetNewStrBNN(3, sb);
			sp->groupId = r->GetInt32(4);
			sp->descript = r->GetNewStrBNN(5, sb);
			sp->dirName = r->GetNewStrBNN(6, sb);
			sp->photo = r->GetNewStrB(7, sb);
			sp->idKey = r->GetNewStrBNN(8, sb);
			sp->cateId = r->GetInt32(9);
			sp->flags = (SpeciesFlags)r->GetInt32(10);
			sp->photoId = r->GetInt32(11);
			sp->photoWId = r->GetInt32(12);
			sp->poiImg = r->GetNewStrB(13, sb);

			this->spMap.Put(sp->speciesId, sp);
			sp->sciNameHash = this->spNameMap.CalcHash(sp->sciName->v, sp->sciName->leng);
		}
		this->db->CloseReader(r);

		SpeciesSciNameComparator comparator;
		Data::ArrayList<SpeciesInfo*> speciesList(this->spMap.GetCount());
		speciesList.AddAll(this->spMap);
		Data::Sort::ArtificialQuickSort::Sort(&speciesList, &comparator);
		UOSInt i = 0;
		UOSInt j = speciesList.GetCount();
		while (i < j)
		{
			sp = speciesList.GetItem(i);
			this->spNameMap.PutNN(sp->sciName, sp);
			i++;
		}
	}

	r = this->db->ExecuteReader(CSTR("select id, species_id, crcVal, imgUrl, srcUrl, prevUpdated, cropLeft, cropTop, cropRight, cropBottom, location from webfile"));
	if (r != 0)
	{
		while (r->ReadNext())
		{
			sp = this->spMap.Get(r->GetInt32(1));
			if (sp)
			{
				wfile = MemAlloc(WebFileInfo, 1);
				wfile->id = r->GetInt32(0);
				wfile->crcVal = r->GetInt32(2);
				wfile->imgUrl = r->GetNewStrBNN(3, sb);
				wfile->srcUrl = r->GetNewStrBNN(4, sb);
				wfile->prevUpdated = r->GetBool(5);
				wfile->cropLeft = r->GetDbl(6);
				wfile->cropTop = r->GetDbl(7);
				wfile->cropRight = r->GetDbl(8);
				wfile->cropBottom = r->GetDbl(9);
				wfile->location = r->GetNewStrBNN(10, sb);
				sp->wfiles.Put(wfile->id, wfile);
			}
		}
		this->db->CloseReader(r);
	}
}

void SSWR::OrganWeb::OrganWebEnv::LoadGroups()
{
	FreeGroups();

	Text::StringBuilderUTF8 sb;
	SpeciesInfo *sp;
	GroupInfo *group;
	GroupInfo *pGroup;
	CategoryInfo *cate;
	UOSInt i;
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("select id, group_type, eng_name, chi_name, description, parent_id, photo_group, photo_species, idKey, cate_id, flags from "));
	sql.AppendCol((const UTF8Char*)"groups");
	DB::DBReader *r = this->db->ExecuteReader(sql.ToCString());
	if (r != 0)
	{
		while (r->ReadNext())
		{
			NEW_CLASS(group, GroupInfo());
			group->id = r->GetInt32(0);
			group->groupType = r->GetInt32(1);
			group->engName = r->GetNewStrBNN(2, sb);
			group->chiName = r->GetNewStrBNN(3, sb);
			group->descript = r->GetNewStrBNN(4, sb);
			group->parentId = r->GetInt32(5);
			group->photoGroup = r->GetInt32(6);
			group->photoSpecies = r->GetInt32(7);
			group->idKey = r->GetNewStrB(8, sb);
			group->cateId = r->GetInt32(9);
			group->flags = (GroupFlags)r->GetInt32(10);
			group->photoCount = (UOSInt)-1;
			group->myPhotoCount = (UOSInt)-1;
			group->totalCount = (UOSInt)-1;
			group->photoSpObj = 0;

			this->groupMap.Put(group->id, group);
		}
		this->db->CloseReader(r);

		i = this->spMap.GetCount();
		while (i-- > 0)
		{
			sp = this->spMap.GetItem(i);
			group = this->groupMap.Get(sp->groupId);
			if (group)
			{
				group->species.Add(sp);
			}
		}

		i = this->groupMap.GetCount();
		while (i-- > 0)
		{
			group = this->groupMap.GetItem(i);
			if (group->parentId)
			{
				pGroup = this->groupMap.Get(group->parentId);
				if (pGroup)
				{
					pGroup->groups.Add(group);
				}
			}
			else
			{
				cate = this->cateMap.Get(group->cateId);
				if (cate)
				{
					cate->groups.Add(group);
				}
			}
		}
	}
}

void SSWR::OrganWeb::OrganWebEnv::LoadBooks()
{
	FreeBooks();

	this->selectedBook = 0;
	Text::StringBuilderUTF8 sb;
	SpeciesInfo *sp;
	BookInfo *book;
	BookSpInfo *bookSp;
	Data::DateTime dt;

	DB::DBReader *r = this->db->ExecuteReader(CSTR("select id, title, dispAuthor, press, publishDate, url, userfile_id from book"));
	if (r != 0)
	{
		while (r->ReadNext())
		{
			NEW_CLASS(book, BookInfo());
			book->id = r->GetInt32(0);
			book->title = r->GetNewStrBNN(1, sb);
			book->author = r->GetNewStrBNN(2, sb);
			book->press = r->GetNewStrBNN(3, sb);
			book->publishDate = r->GetTimestamp(4).ToTicks();
			book->url = r->GetNewStrB(5, sb);
			book->userfileId = r->GetInt32(6);

			this->bookMap.Put(book->id, book);
		}
		this->db->CloseReader(r);
	}

	r = this->db->ExecuteReader(CSTR("select species_id, book_id, dispName from species_book"));
	if (r != 0)
	{
		while (r->ReadNext())
		{
			sp = this->spMap.Get(r->GetInt32(0));
			book = this->bookMap.Get(r->GetInt32(1));
			if (sp != 0 && book != 0)
			{
				bookSp = MemAlloc(BookSpInfo, 1);
				bookSp->bookId = book->id;
				bookSp->speciesId = sp->speciesId;
				bookSp->dispName = r->GetNewStrBNN(2, sb);
				book->species.Add(bookSp);
				sp->books.Add(bookSp);
			}
		}
		this->db->CloseReader(r);
	}
}

void SSWR::OrganWeb::OrganWebEnv::LoadUsers(NotNullPtr<Sync::RWMutexUsage> mutUsage)
{
	this->ClearUsers();

	Int32 userId;
	Text::StringBuilderUTF8 sb;
	WebUserInfo *user;
	DB::DBReader *r = this->db->ExecuteReader(CSTR("select id, userName, pwd, watermark, userType from webuser"));
	if (r != 0)
	{
		while (r->ReadNext())
		{
			userId = r->GetInt32(0);
			user = this->userMap.Get(userId);
			if (user)
			{
				this->userNameMap.RemoveNN(user->userName);
				user->userName->Release();
				user->userName = r->GetNewStrBNN(1, sb);
				SDEL_STRING(user->pwd);
				user->pwd = r->GetNewStrB(2, sb);
				user->watermark->Release();
				user->watermark = r->GetNewStrBNN(3, sb);
				user->userType = r->GetInt32(4);
				this->userNameMap.PutNN(user->userName, user);
			}
			else
			{
				NEW_CLASS(user, WebUserInfo());
				user->id = userId;
				user->userName = r->GetNewStrBNN(1, sb);
				user->pwd = r->GetNewStrB(2, sb);
				user->watermark = r->GetNewStrBNN(3, sb);
				user->userType = r->GetInt32(4);
				user->unorganSpId = 0;
				this->userMap.Put(user->id, user);
				this->userNameMap.PutNN(user->userName, user);
			}
		}
		this->db->CloseReader(r);
	}

	r = this->db->ExecuteReader(CSTR("select id, fileType, oriFileName, fileTime, lat, lon, webuser_id, species_id, captureTime, dataFileName, crcVal, rotType, prevUpdated, cropLeft, cropTop, cropRight, cropBottom, descript, location from userfile"));
	if (r != 0)
	{
		UserFileInfo *userFile;
		SpeciesInfo *species;
		UOSInt i;
		UOSInt j;
		UOSInt k;
		user = 0;
		while (r->ReadNext())
		{
			userId = r->GetInt32(6);
			if (user == 0 || user->id != userId)
			{
				user = this->userMap.Get(userId);
			}
			if (user != 0)
			{
				userFile = MemAlloc(UserFileInfo, 1);
				userFile->id = r->GetInt32(0);
				userFile->fileType = r->GetInt32(1);
				userFile->oriFileName = r->GetNewStrBNN(2, sb);
				userFile->fileTimeTicks = r->GetTimestamp(3).ToTicks();
				userFile->lat = r->GetDbl(4);
				userFile->lon = r->GetDbl(5);
				userFile->webuserId = userId;
				userFile->speciesId = r->GetInt32(7);
				userFile->captureTimeTicks = r->GetTimestamp(8).ToTicks();
				userFile->dataFileName = r->GetNewStrBNN(9, sb);
				userFile->crcVal = (UInt32)r->GetInt32(10);
				userFile->rotType = r->GetInt32(11);
				userFile->prevUpdated = r->GetInt32(12);
				userFile->cropLeft = r->GetDbl(13);
				userFile->cropTop = r->GetDbl(14);
				userFile->cropRight = r->GetDbl(15);
				userFile->cropBottom = r->GetDbl(16);
				userFile->descript = r->GetNewStrB(17, sb);
				userFile->location = r->GetNewStrB(18, sb);
				species = this->spMap.Get(userFile->speciesId);
				if (species != 0)
				{
					species->files.Add(userFile);
				}
				this->userFileMap.Put(userFile->id, userFile);
			}
		}
		this->db->CloseReader(r);

		UserFileTimeComparator comparator;
		Data::ArrayList<UserFileInfo*> userFileList(this->userFileMap.GetCount());
		userFileList.AddAll(this->userFileMap);
		Data::Sort::ArtificialQuickSort::Sort(&userFileList, &comparator);
		i = 0;
		j = userFileList.GetCount();
		while (i < j)
		{
			userFile = userFileList.GetItem(i);
			if (user == 0 || user->id != userFile->webuserId)
			{
				user = this->userMap.Get(userFile->webuserId);
			}
			if (user != 0)
			{
				k = user->userFileIndex.SortedInsert(userFile->captureTimeTicks);
				user->userFileObj.Insert(k, userFile);
			}
			i++;
		}
	}

	r = this->db->ExecuteReader(CSTR("select id, fileType, startTime, endTime, oriFileName, dataFileName, webuser_id from datafile order by webuser_id, startTime"));
	if (r != 0)
	{
		DataFileInfo *dataFile;
		user = 0;
		while (r->ReadNext())
		{
			userId = r->GetInt32(6);
			if (user == 0 || user->id != userId)
			{
				user = this->userMap.Get(userId);
			}
			dataFile = MemAlloc(DataFileInfo, 1);
			dataFile->id = r->GetInt32(0);
			dataFile->fileType = (DataFileType)r->GetInt32(1);
			dataFile->startTime = r->GetTimestamp(2);
			dataFile->endTime = r->GetTimestamp(3);
			dataFile->oriFileName = r->GetNewStr(4);
			dataFile->dataFileName = r->GetNewStr(5);
			dataFile->webuserId = userId;
			this->dataFileMap.Put(dataFile->id, dataFile);
			if (user != 0)
			{
				if (dataFile->fileType == DataFileType::GPSTrack)
				{
					user->gpsDataFiles.Put(dataFile->startTime, dataFile);
				}
				else
				{
					user->tempDataFiles.Put(dataFile->startTime, dataFile);
				}
			}
		}
		this->db->CloseReader(r);
	}

	r = this->db->ExecuteReader(CSTR("select fromDate, toDate, locId, cate_id, webuser_id from trip"));
	if (r != 0)
	{
		Int32 cateId;
		Int64 fromDate;
		Data::FastMap<Int64, TripInfo*> *tripCate;
		TripInfo *trip;
		user = 0;
		while (r->ReadNext())
		{
			userId = r->GetInt32(4);
			cateId = r->GetInt32(3);
			fromDate = r->GetTimestamp(0).ToTicks();
			if (user == 0 || user->id != userId)
			{
				user = this->userMap.Get(userId);
			}
			if (user != 0)
			{
				tripCate = user->tripCates.Get(cateId);
				if (tripCate == 0)
				{
					NEW_CLASS(tripCate, Data::Int64FastMap<TripInfo*>());
					user->tripCates.Put(cateId, tripCate);
				}
				trip = tripCate->Get(fromDate);
				if (trip == 0)
				{
					trip = MemAlloc(TripInfo, 1);
					trip->fromDate = fromDate;
					trip->toDate = r->GetTimestamp(1).ToTicks();
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
		GroupInfo *group;
		group = this->groupMap.Get(this->unorganizedGroupId);
		if (group != 0)
		{
			UOSInt i = this->userMap.GetCount();
			UOSInt j;
			SpeciesInfo *species;
			while (i-- > 0)
			{
				user = this->userMap.GetItem(i);
				if (user->unorganSpId == 0)
				{
					Text::StringBuilderUTF8 sbSName;
					sbSName.AppendC(UTF8STRC("Unorganized "));
					sbSName.Append(user->userName);
					j = group->species.GetCount();
					while (j-- > 0)
					{
						species = group->species.GetItem(j);
						if (species->sciName->Equals(sbSName.ToString(), sbSName.GetLength()))
						{
							user->unorganSpId = species->speciesId;
							break;
						}
					}
					if (user->unorganSpId == 0)
					{
						Text::StringBuilderUTF8 sb;
						sb.AppendC(sbSName.ToString(), sbSName.GetLength());
						sb.ToLower();
						sb.ReplaceStr(UTF8STRC(" "), UTF8STRC("_"));
						sb.ReplaceStr(UTF8STRC("."), UTF8STRC(""));
						user->unorganSpId = this->SpeciesAdd(mutUsage, CSTR(""), user->userName->ToCString(), sbSName.ToCString(), group->id, CSTR(""), sb.ToCString(), CSTR(""), group->cateId);
					}
				}
			}
		}
	}
}

void SSWR::OrganWeb::OrganWebEnv::LoadLocations()
{
	LocationInfo *loc;
	DB::DBReader *r = this->db->ExecuteReader(CSTR("select id, parentId, cname, ename, lat, lon, cate_id, locType from location"));
	Int32 id;
	if (r != 0)
	{
		Text::StringBuilderUTF8 sb;
		while (r->ReadNext())
		{
			id = r->GetInt32(0);
			loc = this->locMap.Get(id);
			if (loc == 0)
			{
				loc = MemAlloc(LocationInfo, 1);
				loc->id = id;
				loc->parentId = r->GetInt32(1);
				loc->cname = r->GetNewStrB(2, sb);
				loc->ename = r->GetNewStrB(3, sb);
				loc->lat = r->GetDbl(4);
				loc->lon = r->GetDbl(5);
				loc->cateId = r->GetInt32(6);
				loc->locType = r->GetInt32(7);
				this->locMap.Put(id, loc);
			}
		}
		this->db->CloseReader(r);
	}
}

void SSWR::OrganWeb::OrganWebEnv::FreeSpecies()
{
	SpeciesInfo *sp;
	WebFileInfo *wfile;
	UOSInt i;
	UOSInt j;

	i = this->spMap.GetCount();
	while (i-- > 0)
	{
		sp = this->spMap.GetItem(i);
		sp->engName->Release();
		sp->chiName->Release();
		sp->sciName->Release();
		sp->descript->Release();
		sp->dirName->Release();
		SDEL_STRING(sp->photo);
		sp->idKey->Release();
		SDEL_STRING(sp->poiImg);

		j = sp->wfiles.GetCount();
		while (j-- > 0)
		{
			wfile = sp->wfiles.GetItem(j);
			wfile->imgUrl->Release();
			wfile->srcUrl->Release();
			wfile->location->Release();
			MemFree(wfile);
		}
		DEL_CLASS(sp);
	}
	this->spMap.Clear();
	this->spNameMap.Clear();
}

void SSWR::OrganWeb::OrganWebEnv::FreeGroups()
{
	CategoryInfo *cate;
	GroupInfo *group;
	UOSInt i;
	i = this->cateMap.GetCount();
	while (i-- > 0)
	{
		cate = this->cateMap.GetItem(i);
		cate->groups.Clear();
	}

	i = this->groupMap.GetCount();
	while (i-- > 0)
	{
		group = this->groupMap.GetItem(i);
		FreeGroup(group);
	}
	this->groupMap.Clear();
}

void SSWR::OrganWeb::OrganWebEnv::FreeGroup(GroupInfo *group)
{
	group->engName->Release();
	group->chiName->Release();
	group->descript->Release();
	SDEL_STRING(group->idKey);
	DEL_CLASS(group);
}

void SSWR::OrganWeb::OrganWebEnv::FreeBooks()
{
	BookInfo *book;
	BookSpInfo *bookSp;
	UOSInt i;
	UOSInt j;

	i = this->bookMap.GetCount();
	while (i-- > 0)
	{
		book = this->bookMap.GetItem(i);
		book->title->Release();
		book->author->Release();
		book->press->Release();
		SDEL_STRING(book->url);
		j = book->species.GetCount();
		while (j-- > 0)
		{
			bookSp = book->species.GetItem(j);
			bookSp->dispName->Release();
			MemFree(bookSp);
		}
		DEL_CLASS(book);
	}
	this->bookMap.Clear();
}

void SSWR::OrganWeb::OrganWebEnv::FreeUsers()
{
	WebUserInfo *user;
	UserFileInfo *userFile;
	DataFileInfo *dataFile;
	const Data::FastMap<Int64, TripInfo*> *tripCate;
	TripInfo *trip;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	i = this->userMap.GetCount();
	while (i-- > 0)
	{
		user = this->userMap.GetItem(i);
		user->userName->Release();
		user->watermark->Release();
		SDEL_STRING(user->pwd);

		j = user->userFileObj.GetCount();
		while (j-- > 0)
		{
			userFile = user->userFileObj.GetItem(j);
			userFile->oriFileName->Release();
			userFile->dataFileName->Release();
			SDEL_STRING(userFile->descript);
			SDEL_STRING(userFile->location);
			MemFree(userFile);
		}

		j = user->tripCates.GetCount();
		while (j-- > 0)
		{
			tripCate = user->tripCates.GetItem(j);
			k = tripCate->GetCount();
			while (k-- > 0)
			{
				trip = tripCate->GetItem(k);
				MemFree(trip);
			}
			DEL_CLASS(tripCate);
		}
		DEL_CLASS(user);
	}
	this->userMap.Clear();
	this->userNameMap.Clear();
	this->userFileMap.Clear();

	i = this->dataFileMap.GetCount();
	while (i-- > 0)
	{
		dataFile = this->dataFileMap.GetItem(i);
		SDEL_STRING(dataFile->oriFileName);
		SDEL_STRING(dataFile->dataFileName);
		MemFree(dataFile);
	}
	this->dataFileMap.Clear();
}

void SSWR::OrganWeb::OrganWebEnv::ClearUsers()
{
	WebUserInfo *user;
	UserFileInfo *userFile;
	DataFileInfo *dataFile;
	UOSInt i;
	UOSInt j;
	i = this->userMap.GetCount();
	while (i-- > 0)
	{
		user = this->userMap.GetItem(i);

		j = user->userFileObj.GetCount();
		while (j-- > 0)
		{
			userFile = user->userFileObj.GetItem(j);
			userFile->oriFileName->Release();
			userFile->dataFileName->Release();
			SDEL_STRING(userFile->descript);
			MemFree(userFile);
		}
		user->userFileIndex.Clear();
		user->userFileObj.Clear();
		user->gpsDataFiles.Clear();
		user->tempDataFiles.Clear();
	}
	this->userFileMap.Clear();

	i = this->dataFileMap.GetCount();
	while (i-- > 0)
	{
		dataFile = this->dataFileMap.GetItem(i);
		SDEL_STRING(dataFile->oriFileName);
		SDEL_STRING(dataFile->dataFileName);
		MemFree(dataFile);
	}
	this->dataFileMap.Clear();
}

SSWR::OrganWeb::OrganWebEnv::OrganWebEnv(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, IO::LogTool *log, DB::DBTool *db, Text::String *imageDir, UInt16 port, UInt16 sslPort, Text::String *cacheDir, Text::String *dataDir, UInt32 scnSize, Text::String *reloadPwd, Int32 unorganizedGroupId, NotNullPtr<Media::DrawEngine> eng, Text::CString osmCachePath)
{
	this->imageDir = SCOPY_STRING(imageDir);
	this->sockf = sockf;
	this->ssl = ssl;
	this->log = log;
	this->scnSize = scnSize;
	this->dataDir = SCOPY_STRING(dataDir);
	this->unorganizedGroupId = unorganizedGroupId;
	this->cacheDir = SCOPY_STRING(cacheDir);
	this->reloadPwd = SCOPY_STRING(reloadPwd);
	this->webHdlr = 0;
	this->selectedBook = 0;

	UTF8Char sbuff[512];
	UTF8Char *sptr;
	this->colorSess = this->colorMgr.CreateSess(0);
	this->eng = eng;

	NEW_CLASS(this->osmHdlr, Map::OSM::OSMCacheHandler(CSTR("http://a.tile.openstreetmap.org/"), osmCachePath, 18, this->sockf, this->ssl));
	this->osmHdlr->AddAlternateURL(CSTR("http://b.tile.openstreetmap.org/"));
	this->osmHdlr->AddAlternateURL(CSTR("http://c.tile.openstreetmap.org/"));
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("Map"));
	NEW_CLASS(this->mapDirHdlr, Net::WebServer::HTTPDirectoryHandler(CSTRP(sbuff, sptr), false, 0, false));

	this->db = db;
	if (this->db == 0)
	{
		this->listener = 0;
		this->sslListener = 0;
		return;
	}
	this->LoadLangs();

	if (port == 0)
	{
		this->listener = 0;
		this->sslListener = 0;
	}
	else
	{
		NEW_CLASS(this->webHdlr, SSWR::OrganWeb::OrganWebHandler(this, this->scnSize));
		this->webHdlr->HandlePath(CSTR("/map"), this->mapDirHdlr, false);
		this->webHdlr->HandlePath(CSTR("/osm"), this->osmHdlr, false);

		NEW_CLASS(this->listener, Net::WebServer::WebListener(this->sockf, 0, this->webHdlr, port, 30, 10, CSTR("OrganWeb/1.0"), false, Net::WebServer::KeepAlive::Default, true));
		if (this->ssl && sslPort)
		{
			NEW_CLASS(this->sslListener, Net::WebServer::WebListener(this->sockf, this->ssl, this->webHdlr, sslPort, 30, 10, CSTR("OrganWeb/1.0"), false, Net::WebServer::KeepAlive::Default, true));
		}
		else
		{
			this->sslListener = 0;
		}
		this->Reload();
	}
}

SSWR::OrganWeb::OrganWebEnv::~OrganWebEnv()
{
	CategoryInfo *cate;
	GroupTypeInfo *grpType;
	LocationInfo *loc;
	IO::ConfigFile *lang;
	UOSInt i;
	UOSInt j;

	SDEL_CLASS(this->listener);
	SDEL_CLASS(this->sslListener);
	SDEL_CLASS(this->webHdlr);
	SDEL_CLASS(this->db);
	DEL_CLASS(this->osmHdlr);
	this->mapDirHdlr->Release();

	FreeGroups();
	FreeSpecies();
	FreeBooks();
	FreeUsers();

	i = this->cateMap.GetCount();
	while (i-- > 0)
	{
		cate = this->cateMap.GetItem(i);
		cate->chiName->Release();
		cate->dirName->Release();
		cate->srcDir->Release();

		j = cate->groupTypes.GetCount();
		while (j-- > 0)
		{
			grpType = cate->groupTypes.GetItem(j);
			grpType->chiName->Release();
			grpType->engName->Release();
			MemFree(grpType);
		}

		DEL_CLASS(cate);
	}

	i = this->langMap.GetCount();
	while (i-- > 0)
	{
		lang = this->langMap.GetItem(i);
		DEL_CLASS(lang);
	}

	i = this->locMap.GetCount();
	while (i-- > 0)
	{
		loc = this->locMap.GetItem(i);
		SDEL_STRING(loc->cname);
		SDEL_STRING(loc->ename);
		MemFree(loc);
	}
	this->colorMgr.DeleteSess(this->colorSess);
	this->eng.Delete();

	this->imageDir->Release();
	this->dataDir->Release();
	SDEL_STRING(this->cacheDir);
	SDEL_STRING(this->reloadPwd);
}

Bool SSWR::OrganWeb::OrganWebEnv::IsError()
{
	if (this->listener == 0)
		return true;
	if (this->listener->IsError())
		return true;
	if (this->sslListener != 0 && this->sslListener->IsError())
		return true;
	return false;
}

void SSWR::OrganWeb::OrganWebEnv::Reload()
{
	Sync::RWMutexUsage mutUsage(this->dataMut, true);
	this->LoadCategory();
	this->LoadLocations();
	this->LoadSpecies();
	this->LoadGroups();
	this->LoadBooks();
	this->LoadUsers(mutUsage);
}

void SSWR::OrganWeb::OrganWebEnv::Restart()
{
	///////////////////////////
}

IO::ParsedObject *SSWR::OrganWeb::OrganWebEnv::ParseFileType(NotNullPtr<IO::StreamData> fd, IO::ParserType targetType)
{
	Sync::MutexUsage mutUsage(this->parserMut);
	return this->parsers.ParseFileType(fd, targetType);
}

Bool SSWR::OrganWeb::OrganWebEnv::HasReloadPwd() const
{
	return this->reloadPwd != 0;
}

Bool SSWR::OrganWeb::OrganWebEnv::ReloadPwdMatches(Text::String *pwd) const
{
	return this->reloadPwd->Equals(pwd);
}

Text::String *SSWR::OrganWeb::OrganWebEnv::GetCacheDir() const
{
	return this->cacheDir;
}

Text::String *SSWR::OrganWeb::OrganWebEnv::GetDataDir() const
{
	return this->dataDir;
}

Media::ColorManagerSess *SSWR::OrganWeb::OrganWebEnv::GetColorSess() const
{
	return this->colorSess;
}

NotNullPtr<Media::DrawEngine> SSWR::OrganWeb::OrganWebEnv::GetDrawEngine() const
{
	return this->eng;
}

void SSWR::OrganWeb::OrganWebEnv::CalcGroupCount(NotNullPtr<Sync::RWMutexUsage> mutUsage, GroupInfo *group)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	UOSInt i;
	SpeciesInfo *sp;
	GroupInfo *sgroup;
	if (group->myPhotoCount != (UOSInt)-1)
		return;

	group->myPhotoCount = 0;
	group->photoCount = 0;
	group->totalCount = 0;

	group->totalCount += group->species.GetCount();
	i = group->species.GetCount();
	while (i-- > 0)
	{
		sp = group->species.GetItem(i);
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

	i = group->groups.GetCount();
	while (i-- > 0)
	{
		sgroup = group->groups.GetItem(i);
		this->CalcGroupCount(mutUsage, sgroup);
		group->myPhotoCount += sgroup->myPhotoCount;
		group->photoCount += sgroup->photoCount;
		group->totalCount += sgroup->totalCount;
		if (group->photoSpObj == 0 || group->photoGroup == sgroup->id)
		{
			group->photoSpObj = sgroup->photoSpObj;
		}
	}
}

void SSWR::OrganWeb::OrganWebEnv::GetGroupSpecies(NotNullPtr<Sync::RWMutexUsage> mutUsage, GroupInfo *group, Data::DataMap<Text::String*, SpeciesInfo*> *spMap, WebUserInfo *user)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	UOSInt i;
	UOSInt j;
	SpeciesInfo *sp;
	GroupInfo *sgroup;
	i = 0;
	j = group->species.GetCount();
	while (i < j)
	{
		sp = group->species.GetItem(i);
		spMap->Put(sp->sciName.Ptr(), sp);
		i++;
	}
	i = group->groups.GetCount();
	while (i-- > 0)
	{
		sgroup = group->groups.GetItem(i);
		if ((sgroup->flags & 1) == 0 || user != 0)
		{
			GetGroupSpecies(mutUsage, sgroup, spMap, user);
		}
	}
}

void SSWR::OrganWeb::OrganWebEnv::SearchInGroup(NotNullPtr<Sync::RWMutexUsage> mutUsage, GroupInfo *group, const UTF8Char *searchStr, UOSInt searchStrLen, Data::ArrayListDbl *speciesIndice, Data::ArrayList<SpeciesInfo*> *speciesObjs, Data::ArrayListDbl *groupIndice, Data::ArrayList<GroupInfo*> *groupObjs, WebUserInfo *user)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	SpeciesInfo *species;
	BookSpInfo *bookSp;
	GroupInfo *subGroup;
	Double rating;
	Double currRating;
	UOSInt i;
	UOSInt j;
/*
h = b
o = a
i = l
e = c
*/
	i = group->species.GetCount();
	while (i-- > 0)
	{
		rating = 0;
		species = group->species.GetItem(i);
		if (species->sciName->Equals(searchStr, searchStrLen) || species->chiName->Equals(searchStr, searchStrLen))
		{
			speciesIndice->Add(1.0);
			speciesObjs->Add(species);
		}
		else
		{
			if (rating < (currRating = species->sciName->MatchRating(searchStr, searchStrLen)))
				rating = currRating;
			if (rating < (currRating = species->chiName->MatchRating(searchStr, searchStrLen)))
				rating = currRating;
			if (rating < (currRating = species->engName->MatchRating(searchStr, searchStrLen)))
				rating = currRating;
			if (rating < (currRating = species->descript->MatchRating(searchStr, searchStrLen)))
				rating = currRating;
			j = species->books.GetCount();
			while (j-- > 0)
			{
				bookSp = species->books.GetItem(j);
				if (bookSp->dispName->Equals(searchStr, searchStrLen))
				{
					rating = 1.0;
					break;
				}
				else
				{
					if (rating < (currRating = bookSp->dispName->MatchRating(searchStr, searchStrLen)))
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
	i = group->groups.GetCount();
	while (i-- > 0)
	{
		rating = 0;
		subGroup = group->groups.GetItem(i);
		if (user == 0 && (subGroup->flags & 1))
		{

		}
		else
		{
			if (subGroup->engName->Equals(searchStr, searchStrLen) || subGroup->chiName->Equals(searchStr, searchStrLen))
			{
				groupIndice->Add(1.0);
				groupObjs->Add(subGroup);
			}
			else
			{
				if (rating < (currRating = subGroup->engName->MatchRating(searchStr, searchStrLen)))
					rating = currRating;
				if (rating < (currRating = subGroup->chiName->MatchRating(searchStr, searchStrLen)))
					rating = currRating;
				if (rating > 0)
				{
					j = groupIndice->SortedInsert(rating);
					groupObjs->Insert(j, subGroup);
				}
			}
			SearchInGroup(mutUsage, subGroup, searchStr, searchStrLen, speciesIndice, speciesObjs, groupIndice, groupObjs, user);
		}
	}
}

Bool SSWR::OrganWeb::OrganWebEnv::GroupIsAdmin(GroupInfo *group)
{
	while (group)
	{
		if (group->flags & 1)
		{
			return true;
		}
		group = this->groupMap.Get(group->parentId);
	}
	return false;
}

UTF8Char *SSWR::OrganWeb::OrganWebEnv::PasswordEnc(UTF8Char *buff, Text::CString pwd)
{
	UInt8 md5Val[16];
	Crypto::Hash::MD5 md5;
	md5.Calc(pwd.v, pwd.leng);
	md5.GetValue(md5Val);
	return Text::StrHexBytes(buff, md5Val, 16, 0);
}

SSWR::OrganWeb::BookInfo *SSWR::OrganWeb::OrganWebEnv::BookGet(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 id)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	return this->bookMap.Get(id);
}

SSWR::OrganWeb::BookInfo *SSWR::OrganWeb::OrganWebEnv::BookGetSelected(NotNullPtr<Sync::RWMutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	return this->selectedBook;
}

void SSWR::OrganWeb::OrganWebEnv::BookSelect(BookInfo *book)
{
	this->selectedBook = book;
}

UTF8Char *SSWR::OrganWeb::OrganWebEnv::BookGetPath(UTF8Char *sbuff, Int32 bookId)
{
	sbuff = this->dataDir->ConcatTo(sbuff);
	if (sbuff[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sbuff++ = IO::Path::PATH_SEPERATOR;
	}
	sbuff = Text::StrConcatC(sbuff, UTF8STRC("BookFile"));
	*sbuff++ = IO::Path::PATH_SEPERATOR;
	sbuff = Text::StrInt32(sbuff, bookId);
	sbuff = Text::StrConcatC(sbuff, UTF8STRC(".pdf"));
	return sbuff;
}

void SSWR::OrganWeb::OrganWebEnv::BookGetList(NotNullPtr<Sync::RWMutexUsage> mutUsage, Data::ArrayList<BookInfo*> *bookList)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	bookList->AddAll(this->bookMap);
}

Bool SSWR::OrganWeb::OrganWebEnv::BookFileExist(BookInfo *book)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = this->dataDir->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = Text::StrConcatC(sptr, UTF8STRC("BookFile"));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, book->id);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".pdf"));
	return IO::Path::GetPathType(CSTRP(sbuff, sptr)) == IO::Path::PathType::File;
}

Bool SSWR::OrganWeb::OrganWebEnv::BookSetPhoto(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 bookId, Int32 userfileId)
{
	BookInfo *book = this->BookGet(mutUsage, bookId);
	UserFileInfo *userFile = this->UserfileGet(mutUsage, userfileId);
	if (book == 0 || userFile == 0)
		return false;
	mutUsage->ReplaceMutex(this->dataMut, true);

	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("update book set userfile_id = "));
	sql.AppendInt32(userfileId);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(book->id);
	if (this->db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		book->userfileId = userfileId;
		return true;
	}
	else
	{
		return false;
	}
}

SSWR::OrganWeb::BookInfo *SSWR::OrganWeb::OrganWebEnv::BookAdd(NotNullPtr<Sync::RWMutexUsage> mutUsage, Text::String *title, Text::String *author, Text::String *press, Data::Timestamp pubDate, Text::String *url)
{
	if (title == 0 || title->leng == 0 ||
		author == 0 || author->leng == 0 ||
		press == 0 || press->leng == 0 ||
		pubDate.IsNull() ||
		(url != 0 && !(url->leng == 0 || url->StartsWith(UTF8STRC("http://")) || url->StartsWith(UTF8STRC("https://")))))
	{
		return 0;
	}
	mutUsage->ReplaceMutex(this->dataMut, true);

	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("insert into book (title, dispAuthor, press, publishDate, groupId, url) values ("));
	sql.AppendStr(title);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStr(author);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStr(press);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendTS(pubDate);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(0);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStr(url);
	sql.AppendCmdC(CSTR(")"));
	if (this->db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		BookInfo *book;
		NEW_CLASS(book, BookInfo());
		book->id = this->db->GetLastIdentity32();
		book->title = title->Clone();
		book->author = author->Clone();
		book->press = press->Clone();
		book->publishDate = pubDate.ToTicks();
		book->url = SCOPY_STRING(url);
		book->userfileId = 0;

		this->bookMap.Put(book->id, book);
		this->selectedBook = book;
		return book;
	}
	else
	{
		return 0;
	}
}

Bool SSWR::OrganWeb::OrganWebEnv::BookAddSpecies(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 speciesId, Text::String *bookspecies, Bool allowDuplicate)
{
	BookInfo *book = this->selectedBook;
	if (book == 0)
		return false;
	BookSpInfo *bookSp;
	SpeciesInfo *species = this->SpeciesGet(mutUsage, speciesId);
	if (species == 0)
		return false;
	UOSInt i;
	if (!allowDuplicate)
	{
		i = species->books.GetCount();
		while (i-- > 0)
		{
			if (species->books.GetItem(i)->bookId == book->id)
				return false;
		}
	}
	mutUsage->ReplaceMutex(this->dataMut, true);

	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("insert into species_book (species_id, book_id, dispName) values ("));
	sql.AppendInt32(species->speciesId);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(book->id);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStr(bookspecies);
	sql.AppendCmdC(CSTR(")"));
	if (this->db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		bookSp = MemAlloc(BookSpInfo, 1);
		bookSp->speciesId = species->speciesId;
		bookSp->bookId = book->id;
		bookSp->dispName = bookspecies->Clone();
		book->species.Add(bookSp);
		species->books.Add(bookSp);
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::OrganWeb::OrganWebEnv::UserGPSGetPos(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 userId, const Data::Timestamp &t, Double *lat, Double *lon)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
/*	OSInt i;
	WebUserInfo *webUser;
	DataFileInfo *dataFile;
	UTF8Char sbuff[512];
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
			sptr = Text::StrConcat(sbuff, this->dataDir);
			if (sptr[-1] != IO::Path::PATH_SEPERATOR)
			{
				*sptr++ = IO::Path::PATH_SEPERATOR;
			}
			sptr = Text::StrConcatC(sptr, UTF8STRC("DataFile"));
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrConcat(sptr, dataFile->fileName);
			NEW_CLASS(fd, IO::StmData::FileData(sbuff, false));
			Map::MapDrawLayer *lyr = (Map::MapDrawLayer*)this->parsers->ParseFileType(fd, IO::ParserType::MapLayer);
			DEL_CLASS(fd);
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

SSWR::OrganWeb::WebUserInfo *SSWR::OrganWeb::OrganWebEnv::UserGet(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 id)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	return this->userMap.Get(id);
}

SSWR::OrganWeb::WebUserInfo *SSWR::OrganWeb::OrganWebEnv::UserGetByName(NotNullPtr<Sync::RWMutexUsage> mutUsage, Text::String *name)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	return this->userNameMap.Get(name);
}

SSWR::OrganWeb::SpeciesInfo *SSWR::OrganWeb::OrganWebEnv::SpeciesGet(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 id)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	return this->spMap.Get(id);
}

SSWR::OrganWeb::SpeciesInfo *SSWR::OrganWeb::OrganWebEnv::SpeciesGetByName(NotNullPtr<Sync::RWMutexUsage> mutUsage, Text::String *sname)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	return this->spNameMap.Get(sname);
}

Int32 SSWR::OrganWeb::OrganWebEnv::SpeciesAdd(NotNullPtr<Sync::RWMutexUsage> mutUsage, Text::CString engName, Text::CString chiName, Text::CString sciName, Int32 groupId, Text::CString description, Text::CString dirName, Text::CString idKey, Int32 cateId)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("insert into species (eng_name, chi_name, sci_name, group_id, description, dirName, idKey, cate_id, mapColor) values ("));
	sql.AppendStrC(engName);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrC(chiName);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrC(sciName);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(groupId);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrC(description);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrC(dirName);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrC(idKey);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(cateId);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32((Int32)0xff4040ff);
	sql.AppendCmdC(CSTR(")"));
	if (this->db->ExecuteNonQuery(sql.ToCString()) == 1)
	{
		SpeciesInfo *species;
		NEW_CLASS(species, SpeciesInfo());
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

		this->spMap.Put(species->speciesId, species);
		this->spNameMap.PutC(species->sciName->ToCString(), species);

		GroupInfo *group = this->groupMap.Get(species->groupId);
		if (group)
		{
			group->species.Add(species);
			this->GroupAddCounts(mutUsage, group->id, 1, 0, 0);
		}
		return species->speciesId;
	}
	else
	{
		return 0;
	}
}

Bool SSWR::OrganWeb::OrganWebEnv::SpeciesUpdateDefPhoto(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 speciesId)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	SpeciesInfo *species = this->spMap.Get(speciesId);
	if (species == 0)
		return false;
	if (species->files.GetCount() > 0)
	{
		UserFileInfo *file = species->files.GetItem(0);
		if (species->photoId != file->id)
		{
			this->SpeciesSetPhotoId(mutUsage, speciesId, file->id);
		}
	}
	else
	{
		if (species->photoId != 0)
		{
			this->SpeciesSetPhotoId(mutUsage, speciesId, 0);
			if (species->flags & SF_HAS_MYPHOTO)
			{
				this->SpeciesSetFlags(mutUsage, speciesId, (SpeciesFlags)(species->flags & ~SF_HAS_MYPHOTO));
				this->GroupAddCounts(mutUsage, species->groupId, 0, (species->flags & SSWR::OrganWeb::SF_HAS_WEBPHOTO)?0:(UOSInt)-1, (UOSInt)-1);
			}
			GroupInfo *group = this->GroupGet(mutUsage, species->groupId);
			Int32 parentGroupId = 0;
			Int32 photoSpId = 0;
			SpeciesInfo *phSp;
			GroupInfo *phGroup;
			Bool found;
			UOSInt i;
			UOSInt j;
			while (group)
			{
				if (group->photoSpecies != species->speciesId)
					break;
				
				if (parentGroupId == 0)
				{
					found = false;
					i = 0;
					j = group->species.GetCount();
					while (i < j)
					{
						phSp = group->species.GetItem(i);
						if (phSp->flags != 0)
						{
							found = true;
							photoSpId = phSp->speciesId;
							this->GroupSetPhotoSpecies(mutUsage, group->id, photoSpId);
							break;
						}
						i++;
					}
					if (!found)
					{
						this->GroupSetPhotoSpecies(mutUsage, group->id, 0);
					}
				}
				else
				{
					if (photoSpId == 0)
					{
						UOSInt k = 0;
						UOSInt l = group->groups.GetCount();
						while (k < l)
						{
							phGroup = group->groups.GetItem(k);
							if (phGroup->photoSpecies != 0)
							{
								photoSpId = phGroup->photoSpecies;
								this->GroupSetPhotoGroup(mutUsage, group->id, phGroup->id);
								break;
							}
							k++;
						}
						this->GroupSetPhotoSpecies(mutUsage, group->id, photoSpId);
					}
					else
					{
						this->GroupSetPhotoSpecies(mutUsage, group->id, photoSpId);
					}
				}
				parentGroupId = group->id;
				group = this->GroupGet(mutUsage, group->parentId);
			}
		}
	}
	return true;
}

Bool SSWR::OrganWeb::OrganWebEnv::SpeciesSetPhotoId(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 speciesId, Int32 photoId)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	SpeciesInfo *species = this->spMap.Get(speciesId);
	if (species == 0)
		return false;
	if (species->photoId == photoId)
	{
		return true;
	}
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("update species set photoId = "));
	sql.AppendInt32(photoId);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(speciesId);
	if (this->db->ExecuteNonQuery(sql.ToCString()) == 1)
	{
		species->photoId = photoId;
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::OrganWeb::OrganWebEnv::SpeciesSetPhotoWId(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 speciesId, Int32 photoWId, Bool removePhotoId)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	SpeciesInfo *species = this->spMap.Get(speciesId);
	if (species == 0)
		return false;
	if (species->photoWId == photoWId)
	{
		return true;
	}
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("update species set photoWId = "));
	sql.AppendInt32(photoWId);
	if (removePhotoId && species->photoId != 0)
	{
		sql.AppendCmdC(CSTR(", photoId = 0"));
	}
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(speciesId);
	if (this->db->ExecuteNonQuery(sql.ToCString()) == 1)
	{
		species->photoWId = photoWId;
		if (removePhotoId && species->photoId != 0)
		{
			species->photoId = 0;
		}
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::OrganWeb::OrganWebEnv::SpeciesSetFlags(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 speciesId, SpeciesFlags flags)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	SpeciesInfo *species = this->spMap.Get(speciesId);
	if (species == 0)
		return false;
	if (species->flags == flags)
	{
		return true;
	}
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("update species set flags = "));
	sql.AppendInt32(flags);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(speciesId);
	if (this->db->ExecuteNonQuery(sql.ToCString()) == 1)
	{
		species->flags = flags;
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::OrganWeb::OrganWebEnv::SpeciesMove(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 speciesId, Int32 groupId, Int32 cateId)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	SpeciesInfo *species = this->spMap.Get(speciesId);
	if (species == 0)
		return false;
	if (species->groupId == groupId)
	{
		return true;
	}
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("update species set group_id = "));
	sql.AppendInt32(groupId);
	sql.AppendCmdC(CSTR(", cate_id = "));
	sql.AppendInt32(cateId);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(speciesId);
	if (this->db->ExecuteNonQuery(sql.ToCString()) == 1)
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
		GroupInfo *group = this->groupMap.Get(species->groupId);
		if (group)
		{
			group->species.Remove(species);
			if (group->photoSpecies == species->speciesId)
			{
				group->photoSpObj = 0;
				this->GroupSetPhotoSpecies(mutUsage, group->id, 0);
			}
			this->GroupAddCounts(mutUsage, group->id, -totalCount, -photoCount, -myPhotoCount);
		}
		species->groupId = groupId;
		species->cateId = cateId;
		group = this->groupMap.Get(species->groupId);
		if (group)
		{
			group->species.Add(species);
			if (group->photoSpObj == 0 && (species->photoId != 0 || species->photo != 0 || species->photoWId != 0))
			{
				group->photoSpObj = species;
			}
			this->GroupAddCounts(mutUsage, group->id, totalCount, photoCount, myPhotoCount);
		}
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::OrganWeb::OrganWebEnv::SpeciesModify(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 speciesId, Text::CString engName, Text::CString chiName, Text::CStringNN sciName, Text::CString description, Text::CString dirName)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	SpeciesInfo *species = this->spMap.Get(speciesId);
	if (species == 0)
		return false;
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("update species set eng_name = "));
	sql.AppendStrC(engName);
	sql.AppendCmdC(CSTR(", chi_name = "));
	sql.AppendStrC(chiName);
	sql.AppendCmdC(CSTR(", sci_name = "));
	sql.AppendStrC(sciName);
	sql.AppendCmdC(CSTR(", description = "));
	sql.AppendStrC(description);
	sql.AppendCmdC(CSTR(", dirName = "));
	sql.AppendStrC(dirName);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(speciesId);
	if (this->db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		if (!species->sciName->Equals(sciName.v, sciName.leng))
		{
			this->spNameMap.RemoveNN(species->sciName);
			this->spNameMap.PutC(sciName, species);
		}
		species->engName->Release();
		species->engName = Text::String::New(engName);
		species->chiName->Release();
		species->chiName = Text::String::New(chiName);
		species->sciName->Release();
		species->sciName = Text::String::New(sciName);
		species->descript->Release();
		species->descript = Text::String::New(description);
		species->dirName->Release();
		species->dirName = Text::String::New(dirName);
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::OrganWeb::OrganWebEnv::SpeciesDelete(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 speciesId)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	SpeciesInfo *species = this->spMap.Get(speciesId);
	if (species == 0 || species->books.GetCount() != 0 || species->files.GetCount() != 0 || species->wfiles.GetCount() != 0)
		return false;
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("delete from species where id = "));
	sql.AppendInt32(speciesId);
	if (this->db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		this->spNameMap.RemoveNN(species->sciName);
		species->engName->Release();
		species->chiName->Release();
		species->sciName->Release();
		species->descript->Release();
		species->dirName->Release();
		SDEL_STRING(species->photo);
		species->idKey->Release();
		SDEL_STRING(species->poiImg);
		Int32 groupId = species->groupId;
		this->spMap.Remove(speciesId);
		GroupInfo *group = this->groupMap.Get(groupId);
		group->species.Remove(species);
		DEL_CLASS(species);
		GroupAddCounts(mutUsage, groupId, (UOSInt)-1, 0, 0);
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::OrganWeb::OrganWebEnv::SpeciesMerge(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 srcSpeciesId, Int32 destSpeciesId, Int32 cateId)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	if (srcSpeciesId == destSpeciesId)
		return false;
	SpeciesInfo *srcSpecies = this->spMap.Get(srcSpeciesId);
	SpeciesInfo *destSpecies = this->spMap.Get(destSpeciesId);
	if (srcSpecies == 0 || destSpecies == 0)
		return false;
	UOSInt i;
	DB::SQLBuilder sql(this->db);
	Bool hasFiles = false;
	Bool hasWFiles = false;
	Bool flagChg = false;
	SpeciesFlags newFlags = destSpecies->flags;
	if (srcSpecies->files.GetCount() > 0)
	{
		sql.Clear();
		sql.AppendCmdC(CSTR("update userfile set species_id = "));
		sql.AppendInt32(destSpeciesId);
		sql.AppendCmdC(CSTR(" where species_id = "));
		sql.AppendInt32(srcSpeciesId);
		if (this->db->ExecuteNonQuery(sql.ToCString()) >= 0)
		{
			i = srcSpecies->files.GetCount();
			while (i-- > 0)
			{
				srcSpecies->files.GetItem(i)->speciesId = destSpeciesId;
			}
			destSpecies->files.AddAll(srcSpecies->files);
			srcSpecies->files.Clear();
			if ((newFlags & SF_HAS_MYPHOTO) == 0)
			{
				newFlags = (SpeciesFlags)(newFlags | SF_HAS_MYPHOTO);
				flagChg = true;
			}
			hasFiles = true;
		}
		else
		{
			return false;
		}
	}
	if (srcSpecies->wfiles.GetCount() > 0)
	{
		sql.Clear();
		sql.AppendCmdC(CSTR("update webfile set species_id = "));
		sql.AppendInt32(destSpeciesId);
		sql.AppendCmdC(CSTR(" where species_id = "));
		sql.AppendInt32(srcSpeciesId);
		if (this->db->ExecuteNonQuery(sql.ToCString()) >= 0)
		{
			destSpecies->wfiles.PutAll(&srcSpecies->wfiles);
			srcSpecies->wfiles.Clear();
			if ((newFlags & SF_HAS_MYPHOTO) == 0)
			{
				newFlags = (SpeciesFlags)(newFlags | SF_HAS_WEBPHOTO);
				flagChg = true;
			}
			hasWFiles = true;
		}
		else
		{
			return false;
		}
	}

	if (srcSpecies->books.GetCount() > 0)
	{
		sql.Clear();
		sql.AppendCmdC(CSTR("update species_book set species_id = "));
		sql.AppendInt32(destSpeciesId);
		sql.AppendCmdC(CSTR(" where species_id = "));
		sql.AppendInt32(srcSpeciesId);
		if (this->db->ExecuteNonQuery(sql.ToCString()) >= 0)
		{
			i = srcSpecies->books.GetCount();
			while (i-- > 0)
			{
				srcSpecies->books.GetItem(i)->speciesId = destSpeciesId;
			}
			destSpecies->books.AddAll(srcSpecies->books);
			srcSpecies->books.Clear();
		}
		else
		{
			return false;
		}
	}
	if (flagChg)
	{
		this->SpeciesSetFlags(mutUsage, destSpecies->speciesId, newFlags);
	}
	if (destSpecies->photoId == 0 && destSpecies->photo == 0 && destSpecies->photoWId == 0)
	{
		this->SpeciesUpdateDefPhoto(mutUsage, destSpecies->speciesId);
	}
	GroupInfo *group = this->groupMap.Get(destSpecies->groupId);
	if (group && group->photoSpObj == 0)
	{
		group->photoSpObj = destSpecies;
	}
	if (hasFiles || hasWFiles)
	{
		this->GroupAddCounts(mutUsage, srcSpecies->groupId, 0, (UOSInt)-1, hasFiles?(UOSInt)-1:0);
	}
	return this->SpeciesDelete(mutUsage, srcSpeciesId);
}

Bool SSWR::OrganWeb::OrganWebEnv::SpeciesAddWebfile(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 speciesId, Text::CStringNN imgURL, Text::CStringNN sourceURL, Text::CString location)
{
	if (!imgURL.StartsWith(UTF8STRC("http://")) && !imgURL.StartsWith(UTF8STRC("https://")))
		return false;
	if (!sourceURL.StartsWith(UTF8STRC("http://")) && !sourceURL.StartsWith(UTF8STRC("https://")))
		return false;
	mutUsage->ReplaceMutex(this->dataMut, true);
	SpeciesInfo *species = this->spMap.Get(speciesId);
	if (species == 0)
	{
		return false;
	}
	UOSInt i = species->wfiles.GetCount();
	while (i-- > 0)
	{
		if (species->wfiles.GetItem(i)->imgUrl->Equals(imgURL))
			return false;
	}
	NotNullPtr<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, imgURL, Net::WebUtil::RequestMethod::HTTP_GET, true);
	if (cli->IsError() || cli->GetRespStatus() != Net::WebStatus::SC_OK)
	{
		cli.Delete();
		return false;
	}
	IO::MemoryStream mstm;
	if (!cli->ReadAllContent(mstm, 65536, 10485760))
	{
		cli.Delete();
		return false;
	}
	cli.Delete();
	Crypto::Hash::CRC32RIEEE crc;
	UInt32 crcVal = crc.CalcDirect(imgURL.v, imgURL.leng);
	
	IO::StmData::MemoryDataRef fd(mstm.GetBuff(), mstm.GetLength());
	Media::ImageList *imgList = (Media::ImageList*)this->parsers.ParseFileType(fd, IO::ParserType::ImageList);
	if (imgList == 0)
		return false;
	DEL_CLASS(imgList);

	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("insert into webfile (species_id, crcVal, imgUrl, srcUrl, prevUpdated, cropLeft, cropTop, cropRight, cropBottom, location) values ("));
	sql.AppendInt32(speciesId);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32((Int32)crcVal);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrC(imgURL);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrC(sourceURL);
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
	sql.AppendStrC(location);
	sql.AppendCmdC(CSTR(")"));
	if (this->db->ExecuteNonQuery(sql.ToCString()) > 0)
	{
		Int32 id = this->db->GetLastIdentity32();
		
		WebFileInfo *wfile = MemAlloc(WebFileInfo, 1);
		wfile->id = id;
		wfile->imgUrl = Text::String::New(imgURL);
		wfile->srcUrl = Text::String::New(sourceURL);
		wfile->location = Text::String::New(location);
		wfile->crcVal = (Int32)crcVal;
		wfile->cropLeft = 0;
		wfile->cropTop = 0;
		wfile->cropRight = 0;
		wfile->cropBottom = 0;

		UTF8Char sbuff2[512];
		UTF8Char *sptr2;
		sptr2 = this->dataDir->ConcatTo(sbuff2);
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

		species->wfiles.Put(wfile->id, wfile);
		if ((species->flags & SpeciesFlags::SF_HAS_WEBPHOTO) == 0)
		{
			this->SpeciesSetFlags(mutUsage, speciesId, (SpeciesFlags)(species->flags | SpeciesFlags::SF_HAS_WEBPHOTO));
			if ((species->flags & SpeciesFlags::SF_HAS_MYPHOTO) == 0)
			{
				this->GroupAddCounts(mutUsage, species->groupId, 0, 1, 0);
			}
		}
		if (species->photoWId == 0)
		{
			this->SpeciesSetPhotoWId(mutUsage, speciesId, wfile->id, false);
		}
		return true;
	}
	else
	{
		return false;
	}
}

SSWR::OrganWeb::UserFileInfo *SSWR::OrganWeb::OrganWebEnv::UserfileGetCheck(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 userfileId, Int32 speciesId, Int32 cateId, WebUserInfo *currUser, UTF8Char **filePathOut)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	SpeciesInfo *sp = this->spMap.Get(speciesId);
	UserFileInfo *userFile = this->userFileMap.Get(userfileId);
	if (sp && sp->cateId == cateId && currUser && userFile && (currUser->userType == 0 || userFile->webuserId == currUser->id))
	{
		Data::DateTime dt;
		dt.SetTicks(userFile->fileTimeTicks);
		dt.ToUTCTime();

		UTF8Char *sptr = *filePathOut;
		sptr = this->dataDir->ConcatTo(sptr);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC("UserFile"));
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrInt32(sptr, userFile->webuserId);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = dt.ToString(sptr, "yyyyMM");
		*sptr++ = IO::Path::PATH_SEPERATOR;
		if (userFile->fileType == 3)
		{
			sptr = Text::StrInt64(sptr, userFile->fileTimeTicks);
			sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
			sptr = Text::StrHexVal32(sptr, userFile->crcVal);
			sptr = Text::StrConcatC(sptr, UTF8STRC(".png"));
		}
		else
		{
			sptr = userFile->dataFileName->ConcatTo(sptr);
		}
		*filePathOut = sptr;
		return userFile;
	}
	else
	{
		return 0;
	}
}

SSWR::OrganWeb::UserFileInfo *SSWR::OrganWeb::OrganWebEnv::UserfileGet(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 id)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	return this->userFileMap.Get(id);
}

UTF8Char *SSWR::OrganWeb::OrganWebEnv::UserfileGetPath(UTF8Char *sbuff, const UserFileInfo *userFile)
{
	Data::DateTime dt;
	sbuff = this->dataDir->ConcatTo(sbuff);
	if (sbuff[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sbuff++ = IO::Path::PATH_SEPERATOR;
	}
	sbuff = Text::StrConcatC(sbuff, UTF8STRC("UserFile"));
	*sbuff++ = IO::Path::PATH_SEPERATOR;
	sbuff = Text::StrInt32(sbuff, userFile->webuserId);
	*sbuff++ = IO::Path::PATH_SEPERATOR;
	dt.SetTicks(userFile->fileTimeTicks);
	sbuff = dt.ToString(sbuff, "yyyyMM");
	*sbuff++ = IO::Path::PATH_SEPERATOR;
	sbuff = userFile->dataFileName->ConcatTo(sbuff);
	return sbuff;
}

Int32 SSWR::OrganWeb::OrganWebEnv::UserfileAdd(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 userId, Int32 spId, Text::CStringNN fileName, const UInt8 *fileCont, UOSInt fileSize, Bool mustHaveCamera, Text::String *location)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	UOSInt j;
	UOSInt i;
	Int32 fileType = 0;
	UOSInt fileNameLen = fileName.leng;
	i = fileName.LastIndexOf('.');
	if (i == INVALID_INDEX)
	{
		return 0;
	}
	if (Text::StrEqualsICaseC(&fileName.v[i + 1], fileNameLen - i - 1, UTF8STRC("JPG")))
	{
		fileType = 1;
	}
	else if (Text::StrEqualsICaseC(&fileName.v[i + 1], fileNameLen - i - 1, UTF8STRC("TIF")))
	{
		fileType = 1;
	}
	else if (Text::StrEqualsICaseC(&fileName.v[i + 1], fileNameLen - i - 1, UTF8STRC("PCX")))
	{
		fileType = 1;
	}
	else if (Text::StrEqualsICaseC(&fileName.v[i + 1], fileNameLen - i - 1, UTF8STRC("GIF")))
	{
		fileType = 1;
	}
	else if (Text::StrEqualsICaseC(&fileName.v[i + 1], fileNameLen - i - 1, UTF8STRC("PNG")))
	{
		fileType = 1;
	}
	else if (Text::StrEqualsICaseC(&fileName.v[i + 1], fileNameLen - i - 1, UTF8STRC("HEIC")))
	{
		fileType = 1;
	}
	else if (Text::StrEqualsICaseC(&fileName.v[i + 1], fileNameLen - i - 1, UTF8STRC("HEIF")))
	{
		fileType = 1;
	}
	else if (Text::StrEqualsICaseC(&fileName.v[i + 1], fileNameLen - i - 1, UTF8STRC("AVI")))
	{
		fileType = 2;
	}
	else if (Text::StrEqualsICaseC(&fileName.v[i + 1], fileNameLen - i - 1, UTF8STRC("MOV")))
	{
		fileType = 2;
	}
	else if (Text::StrEqualsICaseC(&fileName.v[i + 1], fileNameLen - i - 1, UTF8STRC("WAV")))
	{
		fileType = 3;
	}
	else
	{
		return 0;
	}
	if (fileType == 1)
	{
		IO::ParserType t;
		IO::ParsedObject *pobj;
		Bool valid = false;
		Data::Timestamp fileTime = Data::Timestamp(0, Data::DateTimeUtil::GetLocalTzQhr());
		Double lat = 0;
		Double lon = 0;
		Int32 rotType = 0;
		UserFileInfo *userFile;
		Text::String *camera = 0;
		UInt32 crcVal = 0;

		{
			IO::StmData::MemoryDataRef md(fileCont, fileSize);
			pobj = this->parsers.ParseFile(md, &t);
		}
		if (pobj)
		{
			if (t == IO::ParserType::ImageList)
			{
				valid = !mustHaveCamera;

				Media::ImageList *imgList = (Media::ImageList*)pobj;
				Media::Image *img = imgList->GetImage(0, 0);
				if (img)
				{
					Media::EXIFData *exif = img->exif;
					if (exif)
					{
						valid = true;
						exif->GetPhotoDate(&fileTime);
						fileTime = fileTime.SetTimeZoneQHR(Data::DateTimeUtil::GetLocalTzQhr());
						if (fileTime.ToUnixTimestamp() >= 946684800) //Y2000
						{
							this->UserGPSGetPos(mutUsage, userId, fileTime, &lat, &lon);
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
								camera = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
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
						else if (mustHaveCamera)
						{
							valid = false;
						}
						Double altitude;
						Int64 gpsTimeTick;
						exif->GetPhotoLocation(&lat, &lon, &altitude, &gpsTimeTick);
						rotType = (Int32)exif->GetRotateType();
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
			WebUserInfo *webUser = this->userMap.Get(userId);
			Int64 ticks = fileTime.ToTicks();
			UOSInt k;
			OSInt si;
			si = webUser->userFileIndex.SortedIndexOf(ticks);
			if (si >= 0)
			{
				while (si > 0)
				{
					if (webUser->userFileIndex.GetItem((UOSInt)si - 1) == ticks)
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
					if (webUser->userFileIndex.GetItem(j) != ticks)
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
				sptr = this->dataDir->ConcatTo(sbuff);
				if (sptr[-1] != IO::Path::PATH_SEPERATOR)
				{
					*sptr++ = IO::Path::PATH_SEPERATOR;
				}
				sptr = Text::StrConcatC(sptr, UTF8STRC("UserFile"));
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = Text::StrInt32(sptr, userId);
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = fileTime.ToUTCTime().ToString(sptr, "yyyyMM");
				IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
				*sptr++ = IO::Path::PATH_SEPERATOR;
				dataFileName = sptr;
				sptr = Text::StrInt64(sptr, ticks);
				sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
				sptr = Text::StrHexVal32(sptr, crcVal);
				i = Text::StrLastIndexOfCharC(fileName.v, fileNameLen, '.');
				if (i != INVALID_INDEX)
				{
					sptr = Text::StrConcatC(sptr, &fileName.v[i], fileNameLen - i);
				}

				Bool succ;
				{
					IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
					succ = (fs.Write(fileCont, fileSize) == fileSize);
				}
				if (succ)
				{
					DB::SQLBuilder sql(this->db);
					sql.AppendCmdC(CSTR("insert into userfile (fileType, oriFileName, fileTime, lat, lon, webuser_id, species_id, captureTime, dataFileName, crcVal, rotType, camera, cropLeft, cropTop, cropRight, cropBottom, location) values ("));
					sql.AppendInt32(fileType);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendStrC(fileName);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendTS(fileTime);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendDbl(lat);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendDbl(lon);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendInt32(userId);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendInt32(spId);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendTS(fileTime);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendStrUTF8(dataFileName);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendInt32((Int32)crcVal);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendInt32(rotType);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendStr(camera);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendDbl(0);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendDbl(0);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendDbl(0);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendDbl(0);
					sql.AppendCmdC(CSTR(", "));
					if (location && location->leng > 0)
					{
						sql.AppendStr(location);
					}
					else
					{
						sql.AppendStr(0);
					}
					sql.AppendCmdC(CSTR(")"));
					if (this->db->ExecuteNonQuery(sql.ToCString()) > 0)
					{
						userFile = MemAlloc(UserFileInfo, 1);
						userFile->id = this->db->GetLastIdentity32();
						userFile->fileType = fileType;
						userFile->oriFileName = Text::String::New(fileName);
						userFile->fileTimeTicks = fileTime.ToTicks();
						userFile->lat = lat;
						userFile->lon = lon;
						userFile->webuserId = userId;
						userFile->speciesId = spId;
						userFile->captureTimeTicks = userFile->fileTimeTicks;
						userFile->dataFileName = Text::String::NewP(dataFileName, sptr);
						userFile->crcVal = crcVal;
						userFile->rotType = rotType;
						userFile->prevUpdated = 0;
						//userFile->camera = camera;
						userFile->cropLeft = 0;
						userFile->cropTop = 0;
						userFile->cropRight = 0;
						userFile->cropBottom = 0;
						userFile->descript = 0;
						if (location && location->leng > 0)
							userFile->location = location->Clone().Ptr();
						else
							userFile->location = 0;
						this->userFileMap.Put(userFile->id, userFile);

						SpeciesInfo *species = this->spMap.Get(userFile->speciesId);
						if (species)
						{
							species->files.Add(userFile);
							if (species->photoId == 0)
							{
								this->SpeciesSetPhotoId(mutUsage, species->speciesId, userFile->id);
							}
						}

						webUser = this->userMap.Get(userFile->webuserId);
						j = webUser->userFileIndex.SortedInsert(userFile->fileTimeTicks);
						webUser->userFileObj.Insert(j, userFile);
						
						SDEL_STRING(camera);
						return userFile->id;
					}
					else
					{
						SDEL_STRING(camera);
						return 0;
					}
				}
				else
				{
					SDEL_STRING(camera);
					return 0;
				}
			}
			else
			{
				SDEL_STRING(camera);
				return 0;
			}
		}
		else
		{
			SDEL_STRING(camera);
			return 0;
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
		UInt8 crcBuff[4];
		crc.Calc(fileCont, fileSize);
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
			WebUserInfo *webUser = this->userMap.Get(userId);
			Int64 ticks = 0;
			UOSInt k;
			OSInt si;
			si = webUser->userFileIndex.SortedIndexOf(ticks);
			if (si >= 0)
			{
				while (si > 0)
				{
					if (webUser->userFileIndex.GetItem((UOSInt)si - 1) == ticks)
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
					if (webUser->userFileIndex.GetItem(j) != ticks)
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
				sptr = this->dataDir->ConcatTo(sbuff);
				if (sptr[-1] != IO::Path::PATH_SEPERATOR)
				{
					*sptr++ = IO::Path::PATH_SEPERATOR;
				}
				sptr = Text::StrConcatC(sptr, UTF8STRC("UserFile"));
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = Text::StrInt32(sptr, userId);
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = fileTime.ToUTCTime().ToString(sptr, "yyyyMM");
				IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
				*sptr++ = IO::Path::PATH_SEPERATOR;
				dataFileName = sptr;
				sptr = Text::StrInt64(sptr, ticks);
				sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
				sptr = Text::StrHexVal32(sptr, crcVal);
				i = Text::StrLastIndexOfCharC(fileName.v, fileNameLen, '.');
				if (i != INVALID_INDEX)
				{
					sptr = Text::StrConcatC(sptr, &fileName.v[i], fileNameLen - i);
				}
				Bool succ;
				{
					IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
					succ = (fs.Write(fileCont, fileSize) == fileSize);
				}
				if (succ)
				{
					DB::SQLBuilder sql(this->db);
					sql.AppendCmdC(CSTR("insert into userfile (fileType, oriFileName, fileTime, lat, lon, webuser_id, species_id, captureTime, dataFileName, crcVal, camera, location) values ("));
					sql.AppendInt32(fileType);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendStrC(fileName);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendTS(fileTime);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendDbl(0);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendDbl(0);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendInt32(userId);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendInt32(spId);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendTS(fileTime);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendStrUTF8(dataFileName);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendInt32((Int32)crcVal);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendStrUTF8(0);
					if (location && location->leng > 0)
					{
						sql.AppendStr(location);
					}
					else
					{
						sql.AppendStr(0);
					}
					sql.AppendCmdC(CSTR(")"));
					if (this->db->ExecuteNonQuery(sql.ToCString()) > 0)
					{
						userFile = MemAlloc(UserFileInfo, 1);
						userFile->id = this->db->GetLastIdentity32();
						userFile->fileType = fileType;
						userFile->oriFileName = Text::String::New(fileName);
						userFile->fileTimeTicks = fileTime.ToTicks();
						userFile->lat = 0;
						userFile->lon = 0;
						userFile->webuserId = userId;
						userFile->speciesId = spId;
						userFile->captureTimeTicks = userFile->fileTimeTicks;
						userFile->dataFileName = Text::String::NewP(dataFileName, sptr);
						userFile->crcVal = crcVal;
						userFile->rotType = 0;
						//userFile->camera = 0;
						userFile->descript = 0;
						userFile->cropLeft = 0;
						userFile->cropTop = 0;
						userFile->cropRight = 0;
						userFile->cropBottom = 0;
						if (location && location->leng > 0)
							userFile->location = location->Clone().Ptr();
						else
							userFile->location = 0;
						this->userFileMap.Put(userFile->id, userFile);

						SpeciesInfo *species = this->spMap.Get(userFile->speciesId);
						if (species)
						{
							species->files.Add(userFile);
							if (species->photoId == 0)
							{
								this->SpeciesSetPhotoId(mutUsage, species->speciesId, userFile->id);
							}
						}

						webUser = this->userMap.Get(userFile->webuserId);
						j = webUser->userFileIndex.SortedInsert(userFile->fileTimeTicks);
						webUser->userFileObj.Insert(j, userFile);
						
						sptr = this->dataDir->ConcatTo(sbuff);
						if (sptr[-1] != IO::Path::PATH_SEPERATOR)
						{
							*sptr++ = IO::Path::PATH_SEPERATOR;
						}
						sptr = Text::StrConcatC(sptr, UTF8STRC("UserFile"));
						*sptr++ = IO::Path::PATH_SEPERATOR;
						sptr = Text::StrInt32(sptr, userId);
						*sptr++ = IO::Path::PATH_SEPERATOR;
						sptr = fileTime.ToUTCTime().ToString(sptr, "yyyyMM");
						IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
						*sptr++ = IO::Path::PATH_SEPERATOR;
						sptr = Text::StrInt64(sptr, ticks);
						sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
						sptr = Text::StrHexVal32(sptr, crcVal);
						sptr = Text::StrConcatC(sptr, UTF8STRC(".png"));
						{
							IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
							graphImg->SavePng(fs);
						}
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

Bool SSWR::OrganWeb::OrganWebEnv::UserfileMove(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 userfileId, Int32 speciesId, Int32 cateId)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	UserFileInfo *userFile = this->userFileMap.Get(userfileId);
	if (userFile == 0)
	{
		return false;
	}
	if (userFile->speciesId == speciesId)
	{
		return true;
	}
	SpeciesInfo *srcSpecies = this->spMap.Get(userFile->speciesId);
	SpeciesInfo *destSpecies = this->spMap.Get(speciesId);
	if (srcSpecies == 0 || destSpecies == 0)
	{
		return false;
	}
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("update userfile set species_id = "));
	sql.AppendInt32(speciesId);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(userfileId);
	if (this->db->ExecuteNonQuery(sql.ToCString()) > 0)
	{
		userFile->speciesId = speciesId;

		UOSInt i = srcSpecies->files.GetCount();
		while (i-- > 0)
		{
			if (srcSpecies->files.GetItem(i) == userFile)
			{
				srcSpecies->files.RemoveAt(i);
				if (srcSpecies->files.GetCount() == 0 || srcSpecies->photoId == userfileId)
				{
					this->SpeciesUpdateDefPhoto(mutUsage, srcSpecies->speciesId);
				}
				break;
			}
		}

		destSpecies->files.Add(userFile);
		if ((destSpecies->flags & SF_HAS_MYPHOTO) == 0)
		{
			this->SpeciesSetFlags(mutUsage, destSpecies->speciesId, (SpeciesFlags)(destSpecies->flags | SF_HAS_MYPHOTO));
			this->GroupAddCounts(mutUsage, destSpecies->groupId, 0, 1, 1);
			GroupInfo *group = this->groupMap.Get(destSpecies->groupId);
			while (group && group->photoSpObj == 0)
			{
				group->photoSpObj = destSpecies;
				group = this->groupMap.Get(group->parentId);
			}
		}
		if (destSpecies->photoId == 0)
		{
			this->SpeciesSetPhotoId(mutUsage, destSpecies->speciesId, userFile->id);
		}
		return true;
	}
	return false;
}

Bool SSWR::OrganWeb::OrganWebEnv::UserfileUpdateDesc(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 userfileId, Text::CString descr)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	UserFileInfo *userFile = this->userFileMap.Get(userfileId);
	if (userFile == 0)
	{
		return false;
	}
	if (descr.v && descr.leng == 0)
	{
		descr.v = 0;
	}
	if (userFile->descript == 0 && descr.v == 0)
	{
		return true;
	}
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("update userfile set descript = "));
	sql.AppendStrC(descr);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(userfileId);
	if (this->db->ExecuteNonQuery(sql.ToCString()) > 0)
	{
		SDEL_STRING(userFile->descript);
		userFile->descript = Text::String::NewOrNull(descr);
		return true;
	}
	return false;
}

Bool SSWR::OrganWeb::OrganWebEnv::UserfileUpdateRotType(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 userfileId, Int32 rotType)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	UserFileInfo *userFile = this->userFileMap.Get(userfileId);
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
	sql.AppendCmdC(CSTR("update userfile set rotType = "));
	sql.AppendInt32(rotType);
	sql.AppendCmdC(CSTR(", prevUpdated = "));
	sql.AppendInt32(1);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(userfileId);
	if (this->db->ExecuteNonQuery(sql.ToCString()) > 0)
	{
		userFile->rotType = rotType;
		userFile->prevUpdated = 1;
		return true;
	}
	return false;
}

Bool SSWR::OrganWeb::OrganWebEnv::SpeciesBookIsExist(NotNullPtr<Sync::RWMutexUsage> mutUsage, Text::CString speciesName, NotNullPtr<Text::StringBuilderUTF8> bookNameOut)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	BookInfo *book;
	BookSpInfo *bookSp;
	UOSInt nameLen = speciesName.leng;
	UOSInt i = 0;
	UOSInt j = this->bookMap.GetCount();
	UOSInt k;
	while (i < j)
	{
		book = this->bookMap.GetItem(i);
		k = book->species.GetCount();
		while (k-- > 0)
		{
			bookSp = book->species.GetItem(k);
			if (bookSp->dispName->Equals(speciesName.v, nameLen))
			{
				bookNameOut->Append(book->title);
				return true;
			}
		}
		i++;
	}
	return false;
}

void SSWR::OrganWeb::OrganWebEnv::UserFilePrevUpdated(NotNullPtr<Sync::RWMutexUsage> mutUsage, UserFileInfo *userFile)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	if (userFile->prevUpdated)
	{
		DB::SQLBuilder sql(this->db);
		sql.AppendCmdC(CSTR("update userfile set prevUpdated = 0 where id = "));
		sql.AppendInt32(userFile->id);
		if (this->db->ExecuteNonQuery(sql.ToCString()) < 0)
		{
			this->db->ExecuteNonQuery(sql.ToCString());
		}
		userFile->prevUpdated = 0;
	}
}

void SSWR::OrganWeb::OrganWebEnv::WebFilePrevUpdated(NotNullPtr<Sync::RWMutexUsage> mutUsage, WebFileInfo *wfile)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	if (wfile->prevUpdated)
	{
		DB::SQLBuilder sql(this->db);
		sql.AppendCmdC(CSTR("update webfile set prevUpdated = 0 where id = "));
		sql.AppendInt32(wfile->id);
		if (this->db->ExecuteNonQuery(sql.ToCString()) < 0)
		{
			this->db->ExecuteNonQuery(sql.ToCString());
		}
		wfile->prevUpdated = 0;
	}
}

SSWR::OrganWeb::GroupInfo *SSWR::OrganWeb::OrganWebEnv::GroupGet(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 id)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	return this->groupMap.Get(id);
}

Int32 SSWR::OrganWeb::OrganWebEnv::GroupAdd(NotNullPtr<Sync::RWMutexUsage> mutUsage, Text::CString engName, Text::CString chiName, Int32 parentId, Text::CString descr, Int32 groupTypeId, Int32 cateId, GroupFlags flags)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	GroupInfo *group = this->groupMap.Get(parentId);
	if (group == 0)
		return 0;
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("insert into "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" (group_type, eng_name, chi_name, description, parent_id, idKey, cate_id, flags) values ("));
	sql.AppendInt32(groupTypeId);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrC(engName);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrC(chiName);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrC(descr);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(parentId);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrUTF8(0);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(cateId);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(flags);
	sql.AppendCmdC(CSTR(")"));
	if (this->db->ExecuteNonQuery(sql.ToCString()) == 1)
	{
		GroupInfo *newGroup;
		NEW_CLASS(newGroup, GroupInfo());
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

		newGroup->photoCount = (UOSInt)-1;
		newGroup->myPhotoCount = (UOSInt)-1;
		newGroup->totalCount = (UOSInt)-1;
		newGroup->photoSpObj = 0;
		this->groupMap.Put(newGroup->id, newGroup);
		group->groups.Add(newGroup);

		return newGroup->id;
	}
	return 0;
}

Bool SSWR::OrganWeb::OrganWebEnv::GroupModify(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 id, Text::CString engName, Text::CString chiName, Text::CString descr, Int32 groupTypeId, GroupFlags flags)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	GroupInfo *group = this->groupMap.Get(id);
	if (group == 0)
		return false;
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("update "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" set group_type = "));
	sql.AppendInt32(groupTypeId);
	sql.AppendCmdC(CSTR(", eng_name = "));
	sql.AppendStrC(engName);
	sql.AppendCmdC(CSTR(", chi_name = "));
	sql.AppendStrC(chiName);
	sql.AppendCmdC(CSTR(", description = "));
	sql.AppendStrC(descr);
	sql.AppendCmdC(CSTR(", flags = "));
	sql.AppendInt32(flags);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(id);
	if (this->db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		group->groupType = groupTypeId;
		group->engName->Release();
		group->engName = Text::String::New(engName);
		group->chiName->Release();
		group->chiName = Text::String::New(chiName);
		group->descript->Release();;
		group->descript = Text::String::New(descr);
		group->flags = flags;
		return true;
	}
	return false;
}

Bool SSWR::OrganWeb::OrganWebEnv::GroupDelete(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 id)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	GroupInfo *group = this->groupMap.Get(id);
	if (group == 0)
		return false;
	if (group->groups.GetCount() > 0)
		return false;
	if (group->species.GetCount() > 0)
		return false;
	CategoryInfo *cate = this->cateMap.Get(group->cateId);
	GroupInfo *parentGroup = this->groupMap.Get(group->parentId);
	if (parentGroup == 0)
		return false;
	if (cate == 0)
		return false;

	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("delete from "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(id);
	if (this->db->ExecuteNonQuery(sql.ToCString()) == 1)
	{
		parentGroup->groups.Remove(group);
		cate->groups.Remove(group);
		this->groupMap.Remove(group->id);
		FreeGroup(group);
		return true;
	}
	return false;
}

Bool SSWR::OrganWeb::OrganWebEnv::GroupMove(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 groupId, Int32 destGroupId, Int32 cateId)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	GroupInfo *group = this->groupMap.Get(groupId);
	if (group == 0)
		return false;
	if (group->parentId == destGroupId)
	{
		return true;
	}
	GroupInfo *parentGroup = group;
	while (parentGroup)
	{
		if (parentGroup->id == destGroupId)
		{
			return false;
		}
		parentGroup = this->groupMap.Get(parentGroup->parentId);
	}
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("update "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" set parent_id = "));
	sql.AppendInt32(destGroupId);
	sql.AppendCmdC(CSTR(", cate_id = "));
	sql.AppendInt32(cateId);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(groupId);
	if (this->db->ExecuteNonQuery(sql.ToCString()) == 1)
	{
		parentGroup = this->groupMap.Get(group->parentId);
		if (parentGroup)
		{
			parentGroup->groups.Remove(group);
			if (parentGroup->photoGroup == group->id)
			{
				this->GroupSetPhotoGroup(mutUsage, parentGroup->id, 0);
			}
			if (parentGroup->groups.GetCount() == 0)
			{
				parentGroup->photoSpObj = 0;
			}
			if (group->myPhotoCount != (UOSInt)-1)
			{
				this->GroupAddCounts(mutUsage, parentGroup->id, -group->totalCount, -group->photoCount, -group->myPhotoCount);
			}
		}
		group->parentId = destGroupId;
		group->cateId = cateId;
		parentGroup = this->groupMap.Get(group->parentId);
		if (parentGroup)
		{
			parentGroup->groups.Add(group);
			if (group->myPhotoCount != (UOSInt)-1)
			{
				this->GroupAddCounts(mutUsage, parentGroup->id, group->totalCount, group->photoCount, group->myPhotoCount);
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::OrganWeb::OrganWebEnv::GroupAddCounts(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 groupId, UOSInt totalCount, UOSInt photoCount, UOSInt myPhotoCount)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	GroupInfo *group = this->groupMap.Get(groupId);
	if (group == 0)
		return false;
	if (group->myPhotoCount != (UOSInt)-1)
	{
		group->totalCount += totalCount;
		group->myPhotoCount += myPhotoCount;
		group->photoCount += photoCount;
		GroupAddCounts(mutUsage, group->parentId, totalCount, photoCount, myPhotoCount);
		return true;
	}
	return false;
}


Bool SSWR::OrganWeb::OrganWebEnv::GroupSetPhotoSpecies(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 groupId, Int32 photoSpeciesId)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	GroupInfo *group = this->groupMap.Get(groupId);
	if (group == 0)
		return false;
	SpeciesInfo *photoSpecies = this->spMap.Get(photoSpeciesId);
	if (photoSpeciesId != 0 && photoSpecies == 0)
		return false;

	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("update "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" set photo_species = "));
	sql.AppendInt32(photoSpeciesId);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(groupId);
	if (this->db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		group->photoSpecies = photoSpeciesId;
		if (photoSpecies == 0)
		{
			group->photoSpObj = 0;
			this->CalcGroupCount(mutUsage, group);
		}
		else
		{
			group->photoSpObj = photoSpecies;
		}
		return true;
	}
	return false;
}

Bool SSWR::OrganWeb::OrganWebEnv::GroupSetPhotoGroup(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 groupId, Int32 photoGroupId)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	GroupInfo *group = this->groupMap.Get(groupId);
	if (group == 0)
		return false;
	GroupInfo *photoGroup = this->groupMap.Get(photoGroupId);
	if (photoGroupId != 0 && photoGroup == 0)
		return false;

	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("update "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" set photo_group = "));
	sql.AppendInt32(photoGroupId);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(groupId);
	if (this->db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		group->photoGroup = photoGroupId;
		if (photoGroup == 0)
		{
			group->photoSpObj = 0;
			this->CalcGroupCount(mutUsage, group);
		}
		else
		{
			group->photoSpObj = photoGroup->photoSpObj;
		}
		return true;
	}
	return false;
}

Bool SSWR::OrganWeb::OrganWebEnv::GroupIsPublic(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 groupId)
{
	if (groupId == 0)
	{
		return false;
	}
	else if (groupId == 21593)
	{
		return true;
	}
	mutUsage->ReplaceMutex(this->dataMut, false);
	GroupInfo *group = this->groupMap.Get(groupId);
	if (group == 0)
	{
		return false;
	}
	return GroupIsPublic(mutUsage, group->parentId);
}

SSWR::OrganWeb::CategoryInfo *SSWR::OrganWeb::OrganWebEnv::CateGet(NotNullPtr<Sync::RWMutexUsage> mutUsage, Int32 id)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	return this->cateMap.Get(id);
}

SSWR::OrganWeb::CategoryInfo *SSWR::OrganWeb::OrganWebEnv::CateGetByName(NotNullPtr<Sync::RWMutexUsage> mutUsage, Text::String *name)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	return this->cateSMap.Get(name);
}

Data::ReadingList<SSWR::OrganWeb::CategoryInfo*> *SSWR::OrganWeb::OrganWebEnv::CateGetList(NotNullPtr<Sync::RWMutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	return &this->cateMap;
}

IO::ConfigFile *SSWR::OrganWeb::OrganWebEnv::LangGet(NotNullPtr<Net::WebServer::IWebRequest> req)
{
	Text::StringBuilderUTF8 sb;
	IO::ConfigFile *lang;
	Text::PString sarr[2];
	Text::PString sarr2[2];
	UOSInt i;
	Text::Locale::LocaleEntry *ent;
	if (req->GetHeaderC(sb, CSTR("Accept-Language")))
	{
		sarr[1] = sb;
		i = 2;
		while (i == 2)
		{
			i = Text::StrSplitP(sarr, 2, sarr[1], ',');
			Text::StrSplitP(sarr2, 2, sarr[0], ';');
			ent = this->locale.GetLocaleEntryByName(sarr2[0].ToCString());
			if (ent)
			{
				lang = this->langMap.Get(ent->lcid);
				if (lang)
					return lang;
			}
		}
	}
	lang = this->langMap.Get(0x409);
	if (lang)
		return lang;
	return this->langMap.GetItem(0);
}
