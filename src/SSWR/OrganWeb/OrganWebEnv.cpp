#include "Stdafx.h"
#include "Crypto/Hash/CRC32R.h"
#include "Crypto/Hash/CRC32RIEEE.h"
#include "Crypto/Hash/MD5.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "DB/DBReader.h"
#include "IO/FileUtil.h"
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
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	NN<IO::Path::FindFileSession> sess;
	UInt32 langId;
	IO::Path::PathType pt;
	UOSInt i;
	NN<IO::ConfigFile> lang;

	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("Langs"));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
	{
		while (IO::Path::FindNextFile(sptr, sess, 0, pt, 0).SetTo(sptr2))
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
						if (IO::IniFile::Parse(CSTRP(sbuff, sptr), 65001).SetTo(lang))
						{
							if (this->langMap.Put(langId, lang).SetTo(lang))
							{
								lang.Delete();
							}
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
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return;
	Text::StringBuilderUTF8 sb;
	NN<CategoryInfo> cate;
	NN<GroupTypeInfo> grpType;
	Int32 cateId;
	UOSInt i;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<DB::DBReader> r;
	if (db->ExecuteReader(CSTR("select cate_id, chi_name, dirName, srcDir, flags from category")).SetTo(r))
	{
		Text::StringBuilderUTF8 sb;
		while (r->ReadNext())
		{
			cateId = r->GetInt32(0);
			if (!this->cateMap.Get(cateId).SetTo(cate))
			{
				NEW_CLASSNN(cate, CategoryInfo());
				cate->cateId = cateId;
				cate->chiName = r->GetNewStrBNN(1, sb);
				cate->dirName = r->GetNewStrBNN(2, sb);
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
				this->cateSMap.PutNN(cate->dirName, cate);
			}
			else
			{
				i = cate->groupTypes.GetCount();
				while (i-- > 0)
				{
					grpType = cate->groupTypes.GetItemNoCheck(i);
					grpType->chiName->Release();
					grpType->engName->Release();
					MemFreeNN(grpType);
				}
				cate->groupTypes.Clear();
			}
		}
		db->CloseReader(r);
	}
	if (db->ExecuteReader(CSTR("select seq, eng_name, chi_name, cate_id from group_type")).SetTo(r))
	{
		while (r->ReadNext())
		{
			cateId = r->GetInt32(3);
			if (!this->cateMap.Get(cateId).SetTo(cate))
			{
			}
			else
			{
				grpType = MemAllocNN(GroupTypeInfo);
				grpType->id = r->GetInt32(0);
				grpType->engName = r->GetNewStrBNN(1, sb);
				grpType->chiName = r->GetNewStrBNN(2, sb);
				cate->groupTypes.Put(grpType->id, grpType);
			}
		}
		db->CloseReader(r);
	}

}

void SSWR::OrganWeb::OrganWebEnv::LoadSpecies()
{
	FreeSpecies();
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return;

	NN<SpeciesInfo> sp;
	NN<WebFileInfo> wfile;
	Text::StringBuilderUTF8 sb;
	NN<DB::DBReader> r;
	if (db->ExecuteReader(CSTR("select id, eng_name, chi_name, sci_name, group_id, description, dirName, photo, idKey, cate_id, flags, photoId, photoWId, poiImg from species")).SetTo(r))
	{
		while (r->ReadNext())
		{
			NEW_CLASSNN(sp, SpeciesInfo());
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
		db->CloseReader(r);

		SpeciesSciNameComparator comparator;
		Data::ArrayListNN<SpeciesInfo> speciesList(this->spMap.GetCount());
		speciesList.AddAll(this->spMap);
		Data::Sort::ArtificialQuickSort::Sort<NN<SpeciesInfo>>(speciesList, comparator);
		UOSInt i = 0;
		UOSInt j = speciesList.GetCount();
		while (i < j)
		{
			sp = speciesList.GetItemNoCheck(i);
			this->spNameMap.PutNN(sp->sciName, sp);
			i++;
		}
	}

	if (db->ExecuteReader(CSTR("select id, species_id, crcVal, imgUrl, srcUrl, prevUpdated, cropLeft, cropTop, cropRight, cropBottom, location from webfile")).SetTo(r))
	{
		while (r->ReadNext())
		{
			if (this->spMap.Get(r->GetInt32(1)).SetTo(sp))
			{
				wfile = MemAllocNN(WebFileInfo);
				wfile->id = r->GetInt32(0);
				wfile->crcVal = r->GetInt32(2);
				wfile->imgUrl = r->GetNewStrBNN(3, sb);
				wfile->srcUrl = r->GetNewStrBNN(4, sb);
				wfile->prevUpdated = r->GetBool(5);
				wfile->cropLeft = r->GetDblOr(6, 0);
				wfile->cropTop = r->GetDblOr(7, 0);
				wfile->cropRight = r->GetDblOr(8, 0);
				wfile->cropBottom = r->GetDblOr(9, 0);
				wfile->location = r->GetNewStrBNN(10, sb);
				sp->wfiles.Put(wfile->id, wfile);
			}
		}
		db->CloseReader(r);
	}
}

void SSWR::OrganWeb::OrganWebEnv::LoadGroups()
{
	FreeGroups();

	Text::StringBuilderUTF8 sb;
	NN<SpeciesInfo> sp;
	NN<GroupInfo> group;
	NN<GroupInfo> pGroup;
	NN<CategoryInfo> cate;
	UOSInt i;
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("select id, group_type, eng_name, chi_name, description, parent_id, photo_group, photo_species, idKey, cate_id, flags from "));
	sql.AppendCol((const UTF8Char*)"groups");
	NN<DB::DBReader> r;
	if (db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		while (r->ReadNext())
		{
			NEW_CLASSNN(group, GroupInfo());
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
		db->CloseReader(r);

		i = this->spMap.GetCount();
		while (i-- > 0)
		{
			sp = this->spMap.GetItemNoCheck(i);
			if (this->groupMap.Get(sp->groupId).SetTo(group))
			{
				group->species.Add(sp);
			}
		}

		i = this->groupMap.GetCount();
		while (i-- > 0)
		{
			group = this->groupMap.GetItemNoCheck(i);
			if (group->parentId)
			{
				if (this->groupMap.Get(group->parentId).SetTo(pGroup))
				{
					pGroup->groups.Add(group);
				}
			}
			else
			{
				if (this->cateMap.Get(group->cateId).SetTo(cate))
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
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return;

	this->selectedBook = 0;
	Text::StringBuilderUTF8 sb;
	NN<SpeciesInfo> sp;
	NN<BookInfo> book;
	NN<BookSpInfo> bookSp;
	Data::DateTime dt;

	NN<DB::DBReader> r;
	if (db->ExecuteReader(CSTR("select id, title, dispAuthor, press, publishDate, url, userfile_id from book")).SetTo(r))
	{
		while (r->ReadNext())
		{
			NEW_CLASSNN(book, BookInfo());
			book->id = r->GetInt32(0);
			book->title = r->GetNewStrBNN(1, sb);
			book->author = r->GetNewStrBNN(2, sb);
			book->press = r->GetNewStrBNN(3, sb);
			book->publishDate = r->GetTimestamp(4).ToTicks();
			book->url = r->GetNewStrB(5, sb);
			book->userfileId = r->GetInt32(6);

			this->bookMap.Put(book->id, book);
		}
		db->CloseReader(r);
	}

	if (db->ExecuteReader(CSTR("select species_id, book_id, dispName from species_book")).SetTo(r))
	{
		while (r->ReadNext())
		{
			if (this->spMap.Get(r->GetInt32(0)).SetTo(sp) && this->bookMap.Get(r->GetInt32(1)).SetTo(book))
			{
				bookSp = MemAllocNN(BookSpInfo);
				bookSp->bookId = book->id;
				bookSp->speciesId = sp->speciesId;
				bookSp->dispName = r->GetNewStrBNN(2, sb);
				book->species.Add(bookSp);
				sp->books.Add(bookSp);
			}
		}
		db->CloseReader(r);
	}
}

void SSWR::OrganWeb::OrganWebEnv::LoadUsers(NN<Sync::RWMutexUsage> mutUsage)
{
	this->ClearUsers();
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return;

	Int32 userId;
	Text::StringBuilderUTF8 sb;
	NN<WebUserInfo> user;
	Optional<WebUserInfo> optuser;
	NN<DB::DBReader> r;
	if (db->ExecuteReader(CSTR("select id, userName, pwd, watermark, userType from webuser")).SetTo(r))
	{
		while (r->ReadNext())
		{
			userId = r->GetInt32(0);
			if (this->userMap.Get(userId).SetTo(user))
			{
				this->userNameMap.RemoveNN(user->userName);
				user->userName->Release();
				user->userName = r->GetNewStrBNN(1, sb);
				OPTSTR_DEL(user->pwd);
				user->pwd = r->GetNewStrB(2, sb);
				user->watermark->Release();
				user->watermark = r->GetNewStrBNN(3, sb);
				user->userType = (UserType)r->GetInt32(4);
				this->userNameMap.PutNN(user->userName, user);
			}
			else
			{
				NEW_CLASSNN(user, WebUserInfo());
				user->id = userId;
				user->userName = r->GetNewStrBNN(1, sb);
				user->pwd = r->GetNewStrB(2, sb);
				user->watermark = r->GetNewStrBNN(3, sb);
				user->userType = (UserType)r->GetInt32(4);
				user->unorganSpId = 0;
				this->userMap.Put(user->id, user);
				this->userNameMap.PutNN(user->userName, user);
			}
		}
		db->CloseReader(r);
	}

	if (db->ExecuteReader(CSTR("select id, fileType, oriFileName, fileTime, lat, lon, webuser_id, species_id, captureTime, dataFileName, crcVal, rotType, prevUpdated, cropLeft, cropTop, cropRight, cropBottom, descript, location, camera, locType from userfile")).SetTo(r))
	{
		NN<UserFileInfo> userFile;
		NN<SpeciesInfo> species;
		optuser = 0;
		UOSInt i;
		UOSInt j;
		UOSInt k;
		while (r->ReadNext())
		{
			userId = r->GetInt32(6);
			if (!optuser.SetTo(user) || user->id != userId)
			{
				optuser = this->userMap.Get(userId);
			}
			if (optuser.SetTo(user))
			{
				userFile = MemAllocNN(UserFileInfo);
				userFile->id = r->GetInt32(0);
				userFile->fileType = (FileType)r->GetInt32(1);
				userFile->oriFileName = r->GetNewStrBNN(2, sb);
				userFile->fileTimeTicks = r->GetTimestamp(3).ToTicks();
				userFile->lat = r->GetDblOr(4, 0);
				userFile->lon = r->GetDblOr(5, 0);
				userFile->webuserId = userId;
				userFile->speciesId = r->GetInt32(7);
				userFile->captureTimeTicks = r->GetTimestamp(8).ToTicks();
				userFile->dataFileName = r->GetNewStrBNN(9, sb);
				userFile->crcVal = (UInt32)r->GetInt32(10);
				userFile->rotType = r->GetInt32(11);
				userFile->prevUpdated = r->GetInt32(12);
				userFile->cropLeft = r->GetDblOr(13, 0);
				userFile->cropTop = r->GetDblOr(14, 0);
				userFile->cropRight = r->GetDblOr(15, 0);
				userFile->cropBottom = r->GetDblOr(16, 0);
				userFile->descript = r->GetNewStrB(17, sb);
				userFile->location = r->GetNewStrB(18, sb);
				userFile->camera = r->GetNewStrB(19, sb);
				userFile->locType = (LocType)r->GetInt32(20);
				if (this->spMap.Get(userFile->speciesId).SetTo(species))
				{
					species->files.Add(userFile);
				}
				this->userFileMap.Put(userFile->id, userFile);
			}
		}
		db->CloseReader(r);

		UserFileTimeComparator comparator;
		Data::ArrayListNN<UserFileInfo> userFileList(this->userFileMap.GetCount());
		userFileList.AddAll(this->userFileMap);
		Data::Sort::ArtificialQuickSort::Sort<NN<UserFileInfo>>(userFileList, comparator);
		i = 0;
		j = userFileList.GetCount();
		while (i < j)
		{
			userFile = userFileList.GetItemNoCheck(i);
			if (!optuser.SetTo(user) || user->id != userFile->webuserId)
			{
				optuser = this->userMap.Get(userFile->webuserId);
			}
			if (optuser.SetTo(user))
			{
				k = user->userFileIndex.SortedInsert(userFile->captureTimeTicks);
				user->userFileObj.Insert(k, userFile);
			}
			i++;
		}
	}

	if (db->ExecuteReader(CSTR("select id, fileType, startTime, endTime, oriFileName, dataFileName, webuser_id from datafile order by webuser_id, startTime")).SetTo(r))
	{
		NN<DataFileInfo> dataFile;
		optuser = 0;
		while (r->ReadNext())
		{
			userId = r->GetInt32(6);
			if (!optuser.SetTo(user) || user->id != userId)
			{
				optuser = this->userMap.Get(userId);
			}
			dataFile = MemAllocNN(DataFileInfo);
			dataFile->id = r->GetInt32(0);
			dataFile->fileType = (DataFileType)r->GetInt32(1);
			dataFile->startTime = r->GetTimestamp(2);
			dataFile->endTime = r->GetTimestamp(3);
			dataFile->oriFileName = r->GetNewStrNN(4);
			dataFile->dataFileName = r->GetNewStrNN(5);
			dataFile->webuserId = userId;
			this->dataFileMap.Put(dataFile->id, dataFile);
			if (optuser.SetTo(user))
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
		db->CloseReader(r);
	}

	if (db->ExecuteReader(CSTR("select fromDate, toDate, locId, cate_id, webuser_id from trip")).SetTo(r))
	{
		Int32 cateId;
		Int64 fromDate;
		NN<Data::FastMapNN<Int64, TripInfo>> tripCate;
		NN<TripInfo> trip;
		optuser = 0;
		while (r->ReadNext())
		{
			userId = r->GetInt32(4);
			cateId = r->GetInt32(3);
			fromDate = r->GetTimestamp(0).ToTicks();
			if (!optuser.SetTo(user) || user->id != userId)
			{
				optuser = this->userMap.Get(userId);
			}
			if (optuser.SetTo(user))
			{
				if (!user->tripCates.Get(cateId).SetTo(tripCate))
				{
					NEW_CLASSNN(tripCate, Data::Int64FastMapNN<TripInfo>());
					user->tripCates.Put(cateId, tripCate);
				}
				if (!tripCate->Get(fromDate).SetTo(trip))
				{
					trip = MemAllocNN(TripInfo);
					trip->fromDate = fromDate;
					trip->toDate = r->GetTimestamp(1).ToTicks();
					trip->cateId = cateId;
					trip->locId = r->GetInt32(2);
					tripCate->Put(fromDate, trip);
				}
			}
		}
		db->CloseReader(r);
	}

	if (this->unorganizedGroupId)
	{
		NN<GroupInfo> group;
		if (this->groupMap.Get(this->unorganizedGroupId).SetTo(group))
		{
			UOSInt i = this->userMap.GetCount();
			UOSInt j;
			NN<SpeciesInfo> species;
			while (i-- > 0)
			{
				user = this->userMap.GetItemNoCheck(i);
				if (user->unorganSpId == 0)
				{
					Text::StringBuilderUTF8 sbSName;
					sbSName.AppendC(UTF8STRC("Unorganized "));
					sbSName.Append(user->userName);
					j = group->species.GetCount();
					while (j-- > 0)
					{
						species = group->species.GetItemNoCheck(j);
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
	NN<LocationInfo> loc;
	NN<DB::DBReader> r;
	Int32 id;
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return;
	if (db->ExecuteReader(CSTR("select id, parentId, cname, ename, lat, lon, cate_id, locType from location")).SetTo(r))
	{
		Text::StringBuilderUTF8 sb;
		while (r->ReadNext())
		{
			id = r->GetInt32(0);
			if (!this->locMap.Get(id).SetTo(loc))
			{
				loc = MemAllocNN(LocationInfo);
				loc->id = id;
				loc->parentId = r->GetInt32(1);
				loc->cname = r->GetNewStrB(2, sb);
				loc->ename = r->GetNewStrB(3, sb);
				loc->lat = r->GetDblOr(4, 0);
				loc->lon = r->GetDblOr(5, 0);
				loc->cateId = r->GetInt32(6);
				loc->locType = r->GetInt32(7);
				this->locMap.Put(id, loc);
			}
		}
		db->CloseReader(r);
	}
}

void SSWR::OrganWeb::OrganWebEnv::FreeSpecies()
{
	NN<SpeciesInfo> sp;
	NN<WebFileInfo> wfile;
	UOSInt i;
	UOSInt j;

	i = this->spMap.GetCount();
	while (i-- > 0)
	{
		sp = this->spMap.GetItemNoCheck(i);
		sp->engName->Release();
		sp->chiName->Release();
		sp->sciName->Release();
		sp->descript->Release();
		sp->dirName->Release();
		OPTSTR_DEL(sp->photo);
		sp->idKey->Release();
		OPTSTR_DEL(sp->poiImg);

		j = sp->wfiles.GetCount();
		while (j-- > 0)
		{
			wfile = sp->wfiles.GetItemNoCheck(j);
			wfile->imgUrl->Release();
			wfile->srcUrl->Release();
			wfile->location->Release();
			MemFreeNN(wfile);
		}
		sp.Delete();
	}
	this->spMap.Clear();
	this->spNameMap.Clear();
}

void SSWR::OrganWeb::OrganWebEnv::FreeGroups()
{
	NN<CategoryInfo> cate;
	NN<GroupInfo> group;
	UOSInt i;
	i = this->cateMap.GetCount();
	while (i-- > 0)
	{
		cate = this->cateMap.GetItemNoCheck(i);
		cate->groups.Clear();
	}

	i = this->groupMap.GetCount();
	while (i-- > 0)
	{
		group = this->groupMap.GetItemNoCheck(i);
		FreeGroup(group);
	}
	this->groupMap.Clear();
}

void SSWR::OrganWeb::OrganWebEnv::FreeGroup(NN<GroupInfo> group)
{
	group->engName->Release();
	group->chiName->Release();
	group->descript->Release();
	OPTSTR_DEL(group->idKey);
	group.Delete();
}

void SSWR::OrganWeb::OrganWebEnv::FreeBooks()
{
	NN<BookInfo> book;
	NN<BookSpInfo> bookSp;
	UOSInt i;
	UOSInt j;

	i = this->bookMap.GetCount();
	while (i-- > 0)
	{
		book = this->bookMap.GetItemNoCheck(i);
		book->title->Release();
		book->author->Release();
		book->press->Release();
		OPTSTR_DEL(book->url);
		j = book->species.GetCount();
		while (j-- > 0)
		{
			bookSp = book->species.GetItemNoCheck(j);
			bookSp->dispName->Release();
			MemFreeNN(bookSp);
		}
		book.Delete();
	}
	this->bookMap.Clear();
}

void SSWR::OrganWeb::OrganWebEnv::FreeUsers()
{
	NN<WebUserInfo> user;
	NN<UserFileInfo> userFile;
	NN<DataFileInfo> dataFile;
	NN<const Data::FastMapNN<Int64, TripInfo>> tripCate;
	NN<TripInfo> trip;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	i = this->userMap.GetCount();
	while (i-- > 0)
	{
		user = this->userMap.GetItemNoCheck(i);
		user->userName->Release();
		user->watermark->Release();
		OPTSTR_DEL(user->pwd);

		j = user->userFileObj.GetCount();
		while (j-- > 0)
		{
			userFile = user->userFileObj.GetItemNoCheck(j);
			userFile->oriFileName->Release();
			userFile->dataFileName->Release();
			OPTSTR_DEL(userFile->descript);
			OPTSTR_DEL(userFile->location);
			OPTSTR_DEL(userFile->camera);
			MemFreeNN(userFile);
		}

		j = user->tripCates.GetCount();
		while (j-- > 0)
		{
			tripCate = user->tripCates.GetItemNoCheck(j);
			k = tripCate->GetCount();
			while (k-- > 0)
			{
				trip = tripCate->GetItemNoCheck(k);
				MemFreeNN(trip);
			}
			tripCate.Delete();
		}
		user.Delete();
	}
	this->userMap.Clear();
	this->userNameMap.Clear();
	this->userFileMap.Clear();

	i = this->dataFileMap.GetCount();
	while (i-- > 0)
	{
		dataFile = this->dataFileMap.GetItemNoCheck(i);
		dataFile->oriFileName->Release();
		dataFile->dataFileName->Release();
		MemFreeNN(dataFile);
	}
	this->dataFileMap.Clear();
}

void SSWR::OrganWeb::OrganWebEnv::ClearUsers()
{
	NN<WebUserInfo> user;
	NN<UserFileInfo> userFile;
	NN<DataFileInfo> dataFile;
	UOSInt i;
	UOSInt j;
	i = this->userMap.GetCount();
	while (i-- > 0)
	{
		user = this->userMap.GetItemNoCheck(i);

		j = user->userFileObj.GetCount();
		while (j-- > 0)
		{
			userFile = user->userFileObj.GetItemNoCheck(j);
			userFile->oriFileName->Release();
			userFile->dataFileName->Release();
			OPTSTR_DEL(userFile->descript);
			OPTSTR_DEL(userFile->location);
			OPTSTR_DEL(userFile->camera);
			MemFreeNN(userFile);
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
		dataFile = this->dataFileMap.GetItemNoCheck(i);
		dataFile->oriFileName->Release();
		dataFile->dataFileName->Release();
		MemFreeNN(dataFile);
	}
	this->dataFileMap.Clear();
}

SSWR::OrganWeb::OrganWebEnv::OrganWebEnv(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, NN<IO::LogTool> log, Optional<DB::DBTool> db, NN<Text::String> imageDir, UInt16 port, UInt16 sslPort, Optional<Text::String> cacheDir, NN<Text::String> dataDir, UInt32 scnSize, Optional<Text::String> reloadPwd, Int32 unorganizedGroupId, NN<Media::DrawEngine> eng, Text::CStringNN osmCachePath)
{
	this->imageDir = imageDir->Clone();
	this->clif = clif;
	this->ssl = ssl;
	this->log = log;
	this->scnSize = scnSize;
	this->dataDir = dataDir->Clone();
	this->unorganizedGroupId = unorganizedGroupId;
	this->cacheDir = Text::String::CopyOrNull(cacheDir);
	this->reloadPwd = Text::String::CopyOrNull(reloadPwd);
	this->webHdlr = 0;
	this->selectedBook = 0;
	this->gpsTrk = 0;
	this->gpsUserId = 0;
	this->gpsStartTime = 0;
	this->gpsEndTime = 0;

	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	this->colorSess = this->colorMgr.CreateSess(0);
	this->eng = eng;

	NEW_CLASSNN(this->osmHdlr, Map::OSM::OSMCacheHandler(CSTR("http://a.tile.openstreetmap.org/"), osmCachePath, 18, this->clif, this->ssl));
	this->osmHdlr->AddAlternateURL(CSTR("http://b.tile.openstreetmap.org/"));
	this->osmHdlr->AddAlternateURL(CSTR("http://c.tile.openstreetmap.org/"));
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("node_modules"));
	NEW_CLASSNN(this->nodeHdlr, Net::WebServer::NodeModuleHandler(CSTRP(sbuff, sptr), 0));

	this->db = db;
	if (this->db.IsNull())
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
		NN<SSWR::OrganWeb::OrganWebHandler> webHdlr;
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("web"));
		NEW_CLASSNN(webHdlr, SSWR::OrganWeb::OrganWebHandler(*this, this->scnSize, CSTRP(sbuff, sptr)));
		webHdlr->HandlePath(CSTR("/js"), this->nodeHdlr, false);
		webHdlr->HandlePath(CSTR("/osm"), this->osmHdlr, false);

		this->webHdlr = webHdlr;
		NEW_CLASSOPT(this->listener, Net::WebServer::WebListener(this->clif, 0, webHdlr, port, 30, 1, 10, CSTR("OrganWeb/1.0"), false, Net::WebServer::KeepAlive::Default, true));
		if (!this->ssl.IsNull() && sslPort)
		{
			NEW_CLASSOPT(this->sslListener, Net::WebServer::WebListener(this->clif, this->ssl, webHdlr, sslPort, 30, 1, 10, CSTR("OrganWeb/1.0"), false, Net::WebServer::KeepAlive::Default, true));
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
	NN<CategoryInfo> cate;
	NN<GroupTypeInfo> grpType;
	NN<LocationInfo> loc;
	NN<IO::ConfigFile> lang;
	UOSInt i;
	UOSInt j;

	this->listener.Delete();
	this->sslListener.Delete();
	this->webHdlr.Delete();
	this->db.Delete();
	this->osmHdlr.Delete();
	this->nodeHdlr.Delete();

	FreeGroups();
	FreeSpecies();
	FreeBooks();
	FreeUsers();

	i = this->cateMap.GetCount();
	while (i-- > 0)
	{
		cate = this->cateMap.GetItemNoCheck(i);
		cate->chiName->Release();
		cate->dirName->Release();
		cate->srcDir->Release();

		j = cate->groupTypes.GetCount();
		while (j-- > 0)
		{
			grpType = cate->groupTypes.GetItemNoCheck(j);
			grpType->chiName->Release();
			grpType->engName->Release();
			MemFreeNN(grpType);
		}

		cate.Delete();
	}

	i = this->langMap.GetCount();
	while (i-- > 0)
	{
		lang = this->langMap.GetItemNoCheck(i);
		lang.Delete();
	}

	i = this->locMap.GetCount();
	while (i-- > 0)
	{
		loc = this->locMap.GetItemNoCheck(i);
		OPTSTR_DEL(loc->cname);
		OPTSTR_DEL(loc->ename);
		MemFreeNN(loc);
	}
	this->eng->EndColorSess(this->colorSess);
	this->colorMgr.DeleteSess(this->colorSess);
	this->eng.Delete();

	this->imageDir->Release();
	this->dataDir->Release();
	OPTSTR_DEL(this->cacheDir);
	OPTSTR_DEL(this->reloadPwd);
}

Bool SSWR::OrganWeb::OrganWebEnv::IsError()
{
	NN<Net::WebServer::WebListener> listener;
	if (!this->listener.SetTo(listener))
		return true;
	if (listener->IsError())
		return true;
	if (this->sslListener.SetTo(listener) && listener->IsError())
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

Optional<IO::ParsedObject> SSWR::OrganWeb::OrganWebEnv::ParseFileType(NN<IO::StreamData> fd, IO::ParserType targetType)
{
	Sync::MutexUsage mutUsage(this->parserMut);
	return this->parsers.ParseFileType(fd, targetType);
}

Bool SSWR::OrganWeb::OrganWebEnv::HasReloadPwd() const
{
	return !this->reloadPwd.IsNull();
}

Bool SSWR::OrganWeb::OrganWebEnv::ReloadPwdMatches(NN<Text::String> pwd) const
{
	NN<Text::String> s;
	return this->reloadPwd.SetTo(s) && s->Equals(pwd);
}

Optional<Text::String> SSWR::OrganWeb::OrganWebEnv::GetCacheDir() const
{
	return this->cacheDir;
}

NN<Text::String> SSWR::OrganWeb::OrganWebEnv::GetDataDir() const
{
	return this->dataDir;
}

NN<Media::ColorManagerSess> SSWR::OrganWeb::OrganWebEnv::GetColorSess() const
{
	return this->colorSess;
}

NN<Media::DrawEngine> SSWR::OrganWeb::OrganWebEnv::GetDrawEngine() const
{
	return this->eng;
}

void SSWR::OrganWeb::OrganWebEnv::SetUpgradeInsecureURL(Text::CStringNN upgradeInsecureURL)
{
	NN<SSWR::OrganWeb::OrganWebHandler> webHdlr;
	if (this->webHdlr.SetTo(webHdlr))
	{
		webHdlr->SetUpgradeInsecureURL(upgradeInsecureURL);
	}
}

void SSWR::OrganWeb::OrganWebEnv::CalcGroupCount(NN<Sync::RWMutexUsage> mutUsage, NN<GroupInfo> group)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	UOSInt i;
	NN<SpeciesInfo> sp;
	NN<GroupInfo> sgroup;
	if (group->myPhotoCount != (UOSInt)-1)
		return;

	group->myPhotoCount = 0;
	group->photoCount = 0;
	group->totalCount = 0;

	group->totalCount += group->species.GetCount();
	i = group->species.GetCount();
	while (i-- > 0)
	{
		sp = group->species.GetItemNoCheck(i);
		if (sp->flags & 9)
		{
			group->photoCount++;
			if (sp->photoId != 0 || sp->photoWId != 0 || !sp->photo.IsNull())
			{
				if (group->photoSpObj.IsNull() || group->photoSpecies == sp->speciesId)
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
		sgroup = group->groups.GetItemNoCheck(i);
		this->CalcGroupCount(mutUsage, sgroup);
		group->myPhotoCount += sgroup->myPhotoCount;
		group->photoCount += sgroup->photoCount;
		group->totalCount += sgroup->totalCount;
		if (group->photoSpObj.IsNull() || group->photoGroup == sgroup->id)
		{
			group->photoSpObj = sgroup->photoSpObj;
		}
	}
}

void SSWR::OrganWeb::OrganWebEnv::GetGroupSpecies(NN<Sync::RWMutexUsage> mutUsage, NN<GroupInfo> group, NN<Data::DataMapNN<Text::String*, SpeciesInfo>> spMap, Optional<WebUserInfo> user)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	UOSInt i;
	UOSInt j;
	NN<SpeciesInfo> sp;
	NN<GroupInfo> sgroup;
	i = 0;
	j = group->species.GetCount();
	while (i < j)
	{
		sp = group->species.GetItemNoCheck(i);
		spMap->Put(sp->sciName.Ptr(), sp);
		i++;
	}
	i = group->groups.GetCount();
	while (i-- > 0)
	{
		sgroup = group->groups.GetItemNoCheck(i);
		if (((sgroup->flags & 1) == 0 || user.NotNull()))
		{
			GetGroupSpecies(mutUsage, sgroup, spMap, user);
		}
	}
}

void SSWR::OrganWeb::OrganWebEnv::SearchInGroup(
	NN<Sync::RWMutexUsage> mutUsage,
	NN<GroupInfo> group,
	UnsafeArray<const UTF8Char> searchStr, UOSInt searchStrLen,
	NN<Data::ArrayListDbl> speciesIndice,
	NN<Data::ArrayListNN<SpeciesInfo>> speciesObjs,
	NN<Data::ArrayListDbl> groupIndice,
	NN<Data::ArrayListNN<GroupInfo>> groupObjs,
	Optional<WebUserInfo> user)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	NN<SpeciesInfo> species;
	NN<BookSpInfo> bookSp;
	NN<GroupInfo> subGroup;
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
		species = group->species.GetItemNoCheck(i);
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
				bookSp = species->books.GetItemNoCheck(j);
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
		subGroup = group->groups.GetItemNoCheck(i);
		if (user.IsNull() && (subGroup->flags & 1))
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

Bool SSWR::OrganWeb::OrganWebEnv::GroupIsAdmin(NN<GroupInfo> group)
{
	while (true)
	{
		if (group->flags & 1)
		{
			return true;
		}
		if (!this->groupMap.Get(group->parentId).SetTo(group))
			return false;
	}
}

UnsafeArray<UTF8Char> SSWR::OrganWeb::OrganWebEnv::PasswordEnc(UnsafeArray<UTF8Char> buff, Text::CStringNN pwd)
{
	UInt8 md5Val[16];
	Crypto::Hash::MD5 md5;
	md5.Calc(pwd.v, pwd.leng);
	md5.GetValue(md5Val);
	return Text::StrHexBytes(buff, md5Val, 16, 0);
}

Optional<SSWR::OrganWeb::BookInfo> SSWR::OrganWeb::OrganWebEnv::BookGet(NN<Sync::RWMutexUsage> mutUsage, Int32 id)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	return this->bookMap.Get(id);
}

Optional<SSWR::OrganWeb::BookInfo> SSWR::OrganWeb::OrganWebEnv::BookGetSelected(NN<Sync::RWMutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	return this->selectedBook;
}

void SSWR::OrganWeb::OrganWebEnv::BookSelect(Optional<BookInfo> book)
{
	this->selectedBook = book;
}

UnsafeArray<UTF8Char> SSWR::OrganWeb::OrganWebEnv::BookGetPath(UnsafeArray<UTF8Char> sbuff, Int32 bookId)
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

void SSWR::OrganWeb::OrganWebEnv::BookGetList(NN<Sync::RWMutexUsage> mutUsage, NN<Data::ArrayListNN<BookInfo>> bookList)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	bookList->AddAll(this->bookMap);
}

Bool SSWR::OrganWeb::OrganWebEnv::BookFileExist(NN<BookInfo> book)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
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

Bool SSWR::OrganWeb::OrganWebEnv::BookSetPhoto(NN<Sync::RWMutexUsage> mutUsage, Int32 bookId, Int32 userfileId)
{
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return false;
	NN<BookInfo> book;
	NN<UserFileInfo> userFile;
	if (!this->BookGet(mutUsage, bookId).SetTo(book) || !this->UserfileGet(mutUsage, userfileId).SetTo(userFile))
		return false;
	mutUsage->ReplaceMutex(this->dataMut, true);

	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("update book set userfile_id = "));
	sql.AppendInt32(userfileId);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(book->id);
	if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		book->userfileId = userfileId;
		return true;
	}
	else
	{
		return false;
	}
}

Optional<SSWR::OrganWeb::BookInfo> SSWR::OrganWeb::OrganWebEnv::BookAdd(NN<Sync::RWMutexUsage> mutUsage, NN<Text::String> title, NN<Text::String> author, NN<Text::String> press, Data::Timestamp pubDate, NN<Text::String> url)
{
	if (title->leng == 0 ||
		author->leng == 0 ||
		press->leng == 0 ||
		pubDate.IsNull() ||
		(!(url->leng == 0 || url->StartsWith(UTF8STRC("http://")) || url->StartsWith(UTF8STRC("https://")))))
	{
		return 0;
	}
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return 0;
	mutUsage->ReplaceMutex(this->dataMut, true);

	DB::SQLBuilder sql(db);
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
	if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		NN<BookInfo> book;
		NEW_CLASSNN(book, BookInfo());
		book->id = db->GetLastIdentity32();
		book->title = title->Clone();
		book->author = author->Clone();
		book->press = press->Clone();
		book->publishDate = pubDate.ToTicks();
		book->url = url->Clone().Ptr();
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

Bool SSWR::OrganWeb::OrganWebEnv::BookAddSpecies(NN<Sync::RWMutexUsage> mutUsage, Int32 speciesId, NN<Text::String> bookspecies, Bool allowDuplicate)
{
	NN<BookInfo> book;
	if (!this->selectedBook.SetTo(book))
		return false;
	NN<BookSpInfo> bookSp;
	NN<SpeciesInfo> species;
	if (!this->SpeciesGet(mutUsage, speciesId).SetTo(species))
		return false;
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return false;
	UOSInt i;
	if (!allowDuplicate)
	{
		i = species->books.GetCount();
		while (i-- > 0)
		{
			if (species->books.GetItemNoCheck(i)->bookId == book->id)
				return false;
		}
	}
	mutUsage->ReplaceMutex(this->dataMut, true);

	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("insert into species_book (species_id, book_id, dispName) values ("));
	sql.AppendInt32(species->speciesId);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(book->id);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStr(bookspecies);
	sql.AppendCmdC(CSTR(")"));
	if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		bookSp = MemAllocNN(BookSpInfo);
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

Bool SSWR::OrganWeb::OrganWebEnv::UserGPSGetPos(NN<Sync::RWMutexUsage> mutUsage, Int32 userId, const Data::Timestamp &t, OutParam<Double> lat, OutParam<Double> lon)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	OSInt i;
	NN<WebUserInfo> webUser;
	NN<DataFileInfo> dataFile;
	NN<Map::GPSTrack> gpsTrk;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	if (this->gpsTrk.IsNull() || this->gpsUserId != userId || this->gpsStartTime > t || this->gpsEndTime < t)
	{
		this->gpsTrk.Delete();
		this->gpsUserId = userId;
		if (this->userMap.Get(userId).SetTo(webUser))
		{
			i = webUser->gpsDataFiles.GetIndex(t);
			if (i < 0)
			{
				i = ~i - 1;
			}
			if (webUser->gpsDataFiles.GetItem((UOSInt)i).SetTo(dataFile))
			{
				this->gpsStartTime = dataFile->startTime;
				this->gpsEndTime = dataFile->endTime;
				sptr = this->dataDir->ConcatTo(sbuff);
				if (sptr[-1] != IO::Path::PATH_SEPERATOR)
				{
					*sptr++ = IO::Path::PATH_SEPERATOR;
				}
				sptr = Text::StrConcatC(sptr, UTF8STRC("DataFile"));
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = dataFile->dataFileName->ConcatTo(sptr);
				IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
				NN<Map::MapDrawLayer> lyr;
				if (Optional<Map::MapDrawLayer>::ConvertFrom(this->parsers.ParseFileType(fd, IO::ParserType::MapLayer)).SetTo(lyr))
				{
					if (lyr->GetObjectClass() == Map::MapDrawLayer::OC_GPS_TRACK)
					{
						this->gpsTrk = NN<Map::GPSTrack>::ConvertFrom(lyr);
					}
					else
					{
						lyr.Delete();
					}
				}
			}
		}
	}

	if (this->gpsTrk.SetTo(gpsTrk))
	{
		Math::Coord2DDbl pos = gpsTrk->GetPosByTime(t);
		lat.Set(pos.GetLat());
		lon.Set(pos.GetLon());
		return true;
	}
	else
	{
		lat.Set(0);
		lon.Set(0);
		return false;
	}
}

Optional<SSWR::OrganWeb::WebUserInfo> SSWR::OrganWeb::OrganWebEnv::UserGet(NN<Sync::RWMutexUsage> mutUsage, Int32 id)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	return this->userMap.Get(id);
}

Optional<SSWR::OrganWeb::WebUserInfo> SSWR::OrganWeb::OrganWebEnv::UserGetByName(NN<Sync::RWMutexUsage> mutUsage, NN<Text::String> name)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	return this->userNameMap.GetNN(name);
}

Optional<SSWR::OrganWeb::SpeciesInfo> SSWR::OrganWeb::OrganWebEnv::SpeciesGet(NN<Sync::RWMutexUsage> mutUsage, Int32 id)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	return this->spMap.Get(id);
}

Optional<SSWR::OrganWeb::SpeciesInfo> SSWR::OrganWeb::OrganWebEnv::SpeciesGetByName(NN<Sync::RWMutexUsage> mutUsage, NN<Text::String> sname)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	return this->spNameMap.GetNN(sname);
}

Int32 SSWR::OrganWeb::OrganWebEnv::SpeciesAdd(NN<Sync::RWMutexUsage> mutUsage, Text::CStringNN engName, Text::CStringNN chiName, Text::CStringNN sciName, Int32 groupId, Text::CStringNN description, Text::CStringNN dirName, Text::CStringNN idKey, Int32 cateId)
{
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return 0;
	mutUsage->ReplaceMutex(this->dataMut, true);
	DB::SQLBuilder sql(db);
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
	if (db->ExecuteNonQuery(sql.ToCString()) == 1)
	{
		NN<SpeciesInfo> species;
		NEW_CLASSNN(species, SpeciesInfo());
		species->speciesId = db->GetLastIdentity32();
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

		NN<GroupInfo> group;
		if (this->groupMap.Get(species->groupId).SetTo(group))
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

Bool SSWR::OrganWeb::OrganWebEnv::SpeciesUpdateDefPhoto(NN<Sync::RWMutexUsage> mutUsage, Int32 speciesId)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	NN<SpeciesInfo> species;
	if (!this->spMap.Get(speciesId).SetTo(species))
		return false;
	if (species->files.GetCount() > 0)
	{
		NN<UserFileInfo> file = species->files.GetItemNoCheck(0);
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
			GroupInfo *group = this->GroupGet(mutUsage, species->groupId).OrNull();
			Int32 parentGroupId = 0;
			Int32 photoSpId = 0;
			NN<SpeciesInfo> phSp;
			NN<GroupInfo> phGroup;
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
						phSp = group->species.GetItemNoCheck(i);
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
							phGroup = group->groups.GetItemNoCheck(k);
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
				group = this->GroupGet(mutUsage, group->parentId).OrNull();
			}
		}
	}
	return true;
}

Bool SSWR::OrganWeb::OrganWebEnv::SpeciesSetPhotoId(NN<Sync::RWMutexUsage> mutUsage, Int32 speciesId, Int32 photoId)
{
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return false;
	mutUsage->ReplaceMutex(this->dataMut, true);
	NN<SpeciesInfo> species;
	if (!this->spMap.Get(speciesId).SetTo(species))
		return false;
	if (species->photoId == photoId)
	{
		return true;
	}
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("update species set photoId = "));
	sql.AppendInt32(photoId);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(speciesId);
	if (db->ExecuteNonQuery(sql.ToCString()) == 1)
	{
		species->photoId = photoId;
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::OrganWeb::OrganWebEnv::SpeciesSetPhotoWId(NN<Sync::RWMutexUsage> mutUsage, Int32 speciesId, Int32 photoWId, Bool removePhotoId)
{
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return false;
	mutUsage->ReplaceMutex(this->dataMut, true);
	NN<SpeciesInfo> species;
	if (!this->spMap.Get(speciesId).SetTo(species))
		return false;
	if (species->photoWId == photoWId)
	{
		return true;
	}
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("update species set photoWId = "));
	sql.AppendInt32(photoWId);
	if (removePhotoId && species->photoId != 0)
	{
		sql.AppendCmdC(CSTR(", photoId = 0"));
	}
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(speciesId);
	if (db->ExecuteNonQuery(sql.ToCString()) == 1)
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

Bool SSWR::OrganWeb::OrganWebEnv::SpeciesSetFlags(NN<Sync::RWMutexUsage> mutUsage, Int32 speciesId, SpeciesFlags flags)
{
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return false;
	mutUsage->ReplaceMutex(this->dataMut, true);
	NN<SpeciesInfo> species;
	if (!this->spMap.Get(speciesId).SetTo(species))
		return false;
	if (species->flags == flags)
	{
		return true;
	}
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("update species set flags = "));
	sql.AppendInt32(flags);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(speciesId);
	if (db->ExecuteNonQuery(sql.ToCString()) == 1)
	{
		species->flags = flags;
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::OrganWeb::OrganWebEnv::SpeciesMove(NN<Sync::RWMutexUsage> mutUsage, Int32 speciesId, Int32 groupId, Int32 cateId)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	NN<SpeciesInfo> species;
	if (!this->spMap.Get(speciesId).SetTo(species))
		return false;
	if (species->groupId == groupId)
	{
		return true;
	}
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return false;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("update species set group_id = "));
	sql.AppendInt32(groupId);
	sql.AppendCmdC(CSTR(", cate_id = "));
	sql.AppendInt32(cateId);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(speciesId);
	if (db->ExecuteNonQuery(sql.ToCString()) == 1)
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
		NN<GroupInfo> group;
		if (this->groupMap.Get(species->groupId).SetTo(group))
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
		if (this->groupMap.Get(species->groupId).SetTo(group))
		{
			group->species.Add(species);
			if (group->photoSpObj.IsNull() && (species->photoId != 0 || !species->photo.IsNull() || species->photoWId != 0))
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

Bool SSWR::OrganWeb::OrganWebEnv::SpeciesModify(NN<Sync::RWMutexUsage> mutUsage, Int32 speciesId, Text::CStringNN engName, Text::CStringNN chiName, Text::CStringNN sciName, Text::CStringNN description, Text::CStringNN dirName)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	NN<SpeciesInfo> species;
	if (!this->spMap.Get(speciesId).SetTo(species))
		return false;
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return false;
	DB::SQLBuilder sql(db);
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
	if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
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

Bool SSWR::OrganWeb::OrganWebEnv::SpeciesDelete(NN<Sync::RWMutexUsage> mutUsage, Int32 speciesId)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	NN<SpeciesInfo> species;
	if (!this->spMap.Get(speciesId).SetTo(species) || species->books.GetCount() != 0 || species->files.GetCount() != 0 || species->wfiles.GetCount() != 0)
		return false;
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return false;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("delete from species where id = "));
	sql.AppendInt32(speciesId);
	if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		this->spNameMap.RemoveNN(species->sciName);
		species->engName->Release();
		species->chiName->Release();
		species->sciName->Release();
		species->descript->Release();
		species->dirName->Release();
		OPTSTR_DEL(species->photo);
		species->idKey->Release();
		OPTSTR_DEL(species->poiImg);
		Int32 groupId = species->groupId;
		this->spMap.Remove(speciesId);
		NN<GroupInfo> group;
		if (this->groupMap.Get(groupId).SetTo(group))
		{
			group->species.Remove(species);
		}
		species.Delete();
		GroupAddCounts(mutUsage, groupId, (UOSInt)-1, 0, 0);
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::OrganWeb::OrganWebEnv::SpeciesMerge(NN<Sync::RWMutexUsage> mutUsage, Int32 srcSpeciesId, Int32 destSpeciesId, Int32 cateId)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	if (srcSpeciesId == destSpeciesId)
		return false;
	NN<SpeciesInfo> srcSpecies;
	NN<SpeciesInfo> destSpecies;
	if (!this->spMap.Get(srcSpeciesId).SetTo(srcSpecies) || !this->spMap.Get(destSpeciesId).SetTo(destSpecies))
		return false;
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return false;
	UOSInt i;
	DB::SQLBuilder sql(db);
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
		if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
		{
			i = srcSpecies->files.GetCount();
			while (i-- > 0)
			{
				srcSpecies->files.GetItemNoCheck(i)->speciesId = destSpeciesId;
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
		if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
		{
			destSpecies->wfiles.PutAll(srcSpecies->wfiles);
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
		if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
		{
			i = srcSpecies->books.GetCount();
			while (i-- > 0)
			{
				srcSpecies->books.GetItemNoCheck(i)->speciesId = destSpeciesId;
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
	if (destSpecies->photoId == 0 && destSpecies->photo.IsNull() && destSpecies->photoWId == 0)
	{
		this->SpeciesUpdateDefPhoto(mutUsage, destSpecies->speciesId);
	}
	NN<GroupInfo> group;
	if (this->groupMap.Get(destSpecies->groupId).SetTo(group) && group->photoSpObj.IsNull())
	{
		group->photoSpObj = destSpecies;
	}
	if (hasFiles || hasWFiles)
	{
		this->GroupAddCounts(mutUsage, srcSpecies->groupId, 0, (UOSInt)-1, hasFiles?(UOSInt)-1:0);
	}
	return this->SpeciesDelete(mutUsage, srcSpeciesId);
}

Bool SSWR::OrganWeb::OrganWebEnv::SpeciesAddWebfile(NN<Sync::RWMutexUsage> mutUsage, Int32 speciesId, Text::CStringNN imgURL, Text::CStringNN sourceURL, Text::CStringNN location)
{
	if (!imgURL.StartsWith(UTF8STRC("http://")) && !imgURL.StartsWith(UTF8STRC("https://")))
		return false;
	if (!sourceURL.StartsWith(UTF8STRC("http://")) && !sourceURL.StartsWith(UTF8STRC("https://")))
		return false;
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return false;
	mutUsage->ReplaceMutex(this->dataMut, true);
	NN<SpeciesInfo> species;
	if (!this->spMap.Get(speciesId).SetTo(species))
	{
		return false;
	}
	UOSInt i = species->wfiles.GetCount();
	while (i-- > 0)
	{
		if (species->wfiles.GetItemNoCheck(i)->imgUrl->Equals(imgURL))
			return false;
	}
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, imgURL, Net::WebUtil::RequestMethod::HTTP_GET, true);
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
	NN<Media::ImageList> imgList;
	if (!Optional<Media::ImageList>::ConvertFrom(this->parsers.ParseFileType(fd, IO::ParserType::ImageList)).SetTo(imgList))
		return false;
	imgList.Delete();

	DB::SQLBuilder sql(db);
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
	if (db->ExecuteNonQuery(sql.ToCString()) > 0)
	{
		Int32 id = db->GetLastIdentity32();
		
		NN<WebFileInfo> wfile = MemAllocNN(WebFileInfo);
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
		UnsafeArray<UTF8Char> sptr2;
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

		UnsafeArray<UInt8> buff = mstm.GetBuff(i);
		{
			IO::FileStream fs(CSTRP(sbuff2, sptr2), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			fs.Write(Data::ByteArrayR(buff, i));
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

Optional<SSWR::OrganWeb::UserFileInfo> SSWR::OrganWeb::OrganWebEnv::UserfileGetCheck(NN<Sync::RWMutexUsage> mutUsage, Int32 userfileId, Int32 speciesId, Int32 cateId, Optional<WebUserInfo> currUser, InOutParam<UnsafeArray<UTF8Char>> filePathOut)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	NN<SpeciesInfo> sp;
	NN<UserFileInfo> userFile;
	NN<WebUserInfo> nnuser;
	if (this->spMap.Get(speciesId).SetTo(sp) && sp->cateId == cateId && this->userFileMap.Get(userfileId).SetTo(userFile) && currUser.SetTo(nnuser) && (nnuser->userType == UserType::Admin || userFile->webuserId == nnuser->id))
	{
		Data::DateTime dt;
		dt.SetTicks(userFile->fileTimeTicks);
		dt.ToUTCTime();

		UnsafeArray<UTF8Char> sptr = filePathOut.Get();
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
		if (userFile->fileType == FileType::Audio)
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
		filePathOut.Set(sptr);
		return userFile;
	}
	else
	{
		return 0;
	}
}

Optional<SSWR::OrganWeb::UserFileInfo> SSWR::OrganWeb::OrganWebEnv::UserfileGet(NN<Sync::RWMutexUsage> mutUsage, Int32 id)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	return this->userFileMap.Get(id);
}

UnsafeArray<UTF8Char> SSWR::OrganWeb::OrganWebEnv::UserfileGetPath(UnsafeArray<UTF8Char> sbuff, NN<const UserFileInfo> userFile)
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

Int32 SSWR::OrganWeb::OrganWebEnv::UserfileAdd(NN<Sync::RWMutexUsage> mutUsage, Int32 userId, Int32 spId, Text::CStringNN fileName, UnsafeArray<const UInt8> fileCont, UOSInt fileSize, Bool mustHaveCamera, Optional<Text::String> location)
{
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return 0;
	mutUsage->ReplaceMutex(this->dataMut, true);
	NN<Text::String> s;
	UOSInt j;
	UOSInt i;
	FileType fileType = FileType::Unknown;
	UOSInt fileNameLen = fileName.leng;
	i = fileName.LastIndexOf('.');
	if (i == INVALID_INDEX)
	{
		return 0;
	}
	if (Text::StrEqualsICaseC(&fileName.v[i + 1], fileNameLen - i - 1, UTF8STRC("JPG")))
	{
		fileType = FileType::Image;
	}
	else if (Text::StrEqualsICaseC(&fileName.v[i + 1], fileNameLen - i - 1, UTF8STRC("TIF")))
	{
		fileType = FileType::Image;
	}
	else if (Text::StrEqualsICaseC(&fileName.v[i + 1], fileNameLen - i - 1, UTF8STRC("PCX")))
	{
		fileType = FileType::Image;
	}
	else if (Text::StrEqualsICaseC(&fileName.v[i + 1], fileNameLen - i - 1, UTF8STRC("GIF")))
	{
		fileType = FileType::Image;
	}
	else if (Text::StrEqualsICaseC(&fileName.v[i + 1], fileNameLen - i - 1, UTF8STRC("PNG")))
	{
		fileType = FileType::Image;
	}
	else if (Text::StrEqualsICaseC(&fileName.v[i + 1], fileNameLen - i - 1, UTF8STRC("HEIC")))
	{
		fileType = FileType::Image;
	}
	else if (Text::StrEqualsICaseC(&fileName.v[i + 1], fileNameLen - i - 1, UTF8STRC("HEIF")))
	{
		fileType = FileType::Image;
	}
	else if (Text::StrEqualsICaseC(&fileName.v[i + 1], fileNameLen - i - 1, UTF8STRC("AVI")))
	{
		fileType = FileType::Video;
	}
	else if (Text::StrEqualsICaseC(&fileName.v[i + 1], fileNameLen - i - 1, UTF8STRC("MOV")))
	{
		fileType = FileType::Video;
	}
	else if (Text::StrEqualsICaseC(&fileName.v[i + 1], fileNameLen - i - 1, UTF8STRC("WAV")))
	{
		fileType = FileType::Audio;
	}
	else
	{
		return 0;
	}
	if (fileType == FileType::Image)
	{
		NN<IO::ParsedObject> pobj;
		Bool valid = false;
		Data::Timestamp fileTime = Data::Timestamp(0, Data::DateTimeUtil::GetLocalTzQhr());
		Double lat = 0;
		Double lon = 0;
		LocType locType = LocType::Unknown;
		Int32 rotType = 0;
		NN<UserFileInfo> userFile;
		Text::String *camera = 0;
		UInt32 crcVal = 0;

		{
			IO::StmData::MemoryDataRef md(fileCont, fileSize);
			if (this->parsers.ParseFile(md).SetTo(pobj))
			{
				if (pobj->GetParserType() == IO::ParserType::ImageList)
				{
					valid = !mustHaveCamera;

					NN<Media::ImageList> imgList = NN<Media::ImageList>::ConvertFrom(pobj);
					NN<Media::RasterImage> img;
					if (imgList->GetImage(0, 0).SetTo(img))
					{
						NN<Media::EXIFData> exif;
						if (img->exif.SetTo(exif))
						{
							valid = true;
							exif->GetPhotoDate(fileTime);
							fileTime = fileTime.SetTimeZoneQHR(Data::DateTimeUtil::GetLocalTzQhr());
							if (fileTime.ToUnixTimestamp() >= 946684800) //Y2000
							{
								if (this->UserGPSGetPos(mutUsage, userId, fileTime, lat, lon))
								{
									locType = LocType::GPSTrack;
								}
							}
							Text::CString cstr;
							Text::CString cstr2;
							Text::CStringNN nncstr;
							Text::CStringNN nncstr2;
							cstr = exif->GetPhotoMake();
							cstr2 = exif->GetPhotoModel();
							if (cstr.SetTo(nncstr) && cstr2.SetTo(nncstr2))
							{
								if (nncstr2.StartsWithICase(nncstr.v, nncstr.leng))
								{
									camera = Text::String::New(nncstr2).Ptr();
								}
								else
								{
									Text::StringBuilderUTF8 sb;
									sb.Append(nncstr);
									sb.AppendC(UTF8STRC(" "));
									sb.Append(nncstr2);
									camera = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
								}
							}
							else if (cstr.SetTo(nncstr))
							{
								camera = Text::String::New(nncstr).Ptr();
							}
							else if (cstr2.SetTo(nncstr2))
							{
								camera = Text::String::New(nncstr2).Ptr();
							}
							else if (mustHaveCamera)
							{
								valid = false;
							}
							Double altitude;
							Int64 gpsTimeTick;
							if (exif->GetPhotoLocation(lat, lon, altitude, gpsTimeTick))
							{
								locType = LocType::PhotoExif;
							}
							rotType = (Int32)exif->GetRotateType();
						}
					}

					UInt8 crcBuff[4];
					Crypto::Hash::CRC32R crc;
					crc.Calc(fileCont, fileSize);
					crc.GetValue(crcBuff);
					crcVal = ReadMUInt32(crcBuff);
				}
				pobj.Delete();
			}
		}
		NN<WebUserInfo> webUser;
		if (valid && this->userMap.Get(userId).SetTo(webUser))
		{
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
				UnsafeArray<UTF8Char> sptr;
				UnsafeArray<UTF8Char> dataFileName;
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
					succ = (fs.Write(Data::ByteArrayR(fileCont, fileSize)) == fileSize);
				}
				if (succ)
				{
					DB::SQLBuilder sql(db);
					sql.AppendCmdC(CSTR("insert into userfile (fileType, oriFileName, fileTime, lat, lon, webuser_id, species_id, captureTime, dataFileName, crcVal, rotType, camera, locType, cropLeft, cropTop, cropRight, cropBottom, location) values ("));
					sql.AppendInt32((Int32)fileType);
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
					sql.AppendStrUTF8(UnsafeArray<const UTF8Char>(dataFileName));
					sql.AppendCmdC(CSTR(", "));
					sql.AppendInt32((Int32)crcVal);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendInt32(rotType);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendStr(camera);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendInt32((Int32)locType);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendDbl(0);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendDbl(0);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendDbl(0);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendDbl(0);
					sql.AppendCmdC(CSTR(", "));
					if (location.SetTo(s) && s->leng > 0)
					{
						sql.AppendStr(s);
					}
					else
					{
						sql.AppendStr(0);
					}
					sql.AppendCmdC(CSTR(")"));
					if (db->ExecuteNonQuery(sql.ToCString()) > 0)
					{
						userFile = MemAllocNN(UserFileInfo);
						userFile->id = db->GetLastIdentity32();
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
						userFile->camera = camera;
						userFile->locType = locType;
						userFile->cropLeft = 0;
						userFile->cropTop = 0;
						userFile->cropRight = 0;
						userFile->cropBottom = 0;
						userFile->descript = 0;
						if (location.SetTo(s) && s->leng > 0)
							userFile->location = s->Clone();
						else
							userFile->location = 0;
						this->userFileMap.Put(userFile->id, userFile);

						NN<SpeciesInfo> species;
						if (this->spMap.Get(userFile->speciesId).SetTo(species))
						{
							species->files.Add(userFile);
							if (species->photoId == 0)
							{
								this->SpeciesSetPhotoId(mutUsage, species->speciesId, userFile->id);
							}
						}

						if (this->userMap.Get(userFile->webuserId).SetTo(webUser))
						{
							j = webUser->userFileIndex.SortedInsert(userFile->fileTimeTicks);
							webUser->userFileObj.Insert(j, userFile);
						}
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
	else if (fileType == FileType::Audio)
	{
		Crypto::Hash::CRC32R crc;
		UInt32 crcVal;
		NN<IO::ParsedObject> pobj;
		Data::Timestamp fileTime = 0;
		NN<UserFileInfo> userFile;
		Bool valid = false;
		Optional<Media::DrawImage> graphImg = 0;
		NN<Media::DrawImage> nngraphImg;
		UInt8 crcBuff[4];
		crc.Calc(fileCont, fileSize);
		crc.GetValue(crcBuff);
		crcVal = ReadMUInt32(crcBuff);

		{
			IO::StmData::FileData fd(fileName, false);
			if (this->parsers.ParseFile(fd).SetTo(pobj))
			{
				if (pobj->GetParserType() == IO::ParserType::MediaFile)
				{
					NN<Media::MediaFile> mediaFile = NN<Media::MediaFile>::ConvertFrom(pobj);
					NN<Media::MediaSource> msrc;
					if (mediaFile->GetStream(0, 0).SetTo(msrc) && msrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
					{
						graphImg = Media::FrequencyGraph::CreateGraph(this->eng, NN<Media::AudioSource>::ConvertFrom(msrc), 2048, 2048, Math::FFTCalc::WT_BLACKMANN_HARRIS, 12);
						if (graphImg.NotNull())
						{
							valid = true;
						}
					}
				}
				pobj.Delete();
			}
		}
		NN<WebUserInfo> webUser;
		if (valid && graphImg.SetTo(nngraphImg) && this->userMap.Get(userId).SetTo(webUser))
		{
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
				UnsafeArray<UTF8Char> sptr;
				UnsafeArray<UTF8Char> dataFileName;
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
					succ = (fs.Write(Data::ByteArrayR(fileCont, fileSize)) == fileSize);
				}
				if (succ)
				{
					DB::SQLBuilder sql(db);
					sql.AppendCmdC(CSTR("insert into userfile (fileType, oriFileName, fileTime, lat, lon, webuser_id, species_id, captureTime, dataFileName, crcVal, camera, location) values ("));
					sql.AppendInt32((Int32)fileType);
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
					sql.AppendStrUTF8(UnsafeArray<const UTF8Char>(dataFileName));
					sql.AppendCmdC(CSTR(", "));
					sql.AppendInt32((Int32)crcVal);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendStrUTF8(0);
					if (location.SetTo(s) && s->leng > 0)
					{
						sql.AppendStr(s);
					}
					else
					{
						sql.AppendStr(0);
					}
					sql.AppendCmdC(CSTR(")"));
					if (db->ExecuteNonQuery(sql.ToCString()) > 0)
					{
						userFile = MemAllocNN(UserFileInfo);
						userFile->id = db->GetLastIdentity32();
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
						if (location.SetTo(s) && s->leng > 0)
							userFile->location = s->Clone();
						else
							userFile->location = 0;
						this->userFileMap.Put(userFile->id, userFile);

						NN<SpeciesInfo> species;
						if (this->spMap.Get(userFile->speciesId).SetTo(species))
						{
							species->files.Add(userFile);
							if (species->photoId == 0)
							{
								this->SpeciesSetPhotoId(mutUsage, species->speciesId, userFile->id);
							}
						}

						if (this->userMap.Get(userFile->webuserId).SetTo(webUser))
						{
							j = webUser->userFileIndex.SortedInsert(userFile->fileTimeTicks);
							webUser->userFileObj.Insert(j, userFile);
						}
						
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
							nngraphImg->SavePng(fs);
						}
						this->eng->DeleteImage(nngraphImg);
						return userFile->id;
					}
					else
					{
						this->eng->DeleteImage(nngraphImg);
						return 0;
					}
				}
				else
				{
					this->eng->DeleteImage(nngraphImg);
					return 0;
				}
			}
			else
			{
				this->eng->DeleteImage(nngraphImg);
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
		UnsafeArray<UTF8Char> sptr;
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

Bool SSWR::OrganWeb::OrganWebEnv::UserfileMove(NN<Sync::RWMutexUsage> mutUsage, Int32 userfileId, Int32 speciesId, Int32 cateId)
{
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return false;
	mutUsage->ReplaceMutex(this->dataMut, true);
	NN<UserFileInfo> userFile;
	if (!this->userFileMap.Get(userfileId).SetTo(userFile))
	{
		return false;
	}
	if (userFile->speciesId == speciesId)
	{
		return true;
	}
	NN<SpeciesInfo> srcSpecies;
	NN<SpeciesInfo> destSpecies;
	if (!this->spMap.Get(userFile->speciesId).SetTo(srcSpecies) || !this->spMap.Get(speciesId).SetTo(destSpecies))
	{
		return false;
	}
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("update userfile set species_id = "));
	sql.AppendInt32(speciesId);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(userfileId);
	if (db->ExecuteNonQuery(sql.ToCString()) > 0)
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
			Optional<GroupInfo> group = this->groupMap.Get(destSpecies->groupId);
			NN<GroupInfo> nngroup;
			while (group.SetTo(nngroup) && nngroup->photoSpObj.IsNull())
			{
				nngroup->photoSpObj = destSpecies;
				group = this->groupMap.Get(nngroup->parentId);
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

Bool SSWR::OrganWeb::OrganWebEnv::UserfileUpdateDesc(NN<Sync::RWMutexUsage> mutUsage, Int32 userfileId, Text::CString descr)
{
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return false;
	mutUsage->ReplaceMutex(this->dataMut, true);
	NN<UserFileInfo> userFile;
	if (!this->userFileMap.Get(userfileId).SetTo(userFile))
	{
		return false;
	}
	if (descr.leng == 0)
	{
		descr.v = 0;
	}
	if (userFile->descript.IsNull() && descr.v == 0)
	{
		return true;
	}
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("update userfile set descript = "));
	sql.AppendStrC(descr);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(userfileId);
	if (db->ExecuteNonQuery(sql.ToCString()) > 0)
	{
		OPTSTR_DEL(userFile->descript);
		userFile->descript = Text::String::NewOrNull(descr);
		return true;
	}
	return false;
}

Bool SSWR::OrganWeb::OrganWebEnv::UserfileUpdateRotType(NN<Sync::RWMutexUsage> mutUsage, Int32 userfileId, Int32 rotType)
{
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return false;
	mutUsage->ReplaceMutex(this->dataMut, true);
	NN<UserFileInfo> userFile;
	if (!this->userFileMap.Get(userfileId).SetTo(userFile))
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
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("update userfile set rotType = "));
	sql.AppendInt32(rotType);
	sql.AppendCmdC(CSTR(", prevUpdated = "));
	sql.AppendInt32(1);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(userfileId);
	if (db->ExecuteNonQuery(sql.ToCString()) > 0)
	{
		userFile->rotType = rotType;
		userFile->prevUpdated = 1;
		return true;
	}
	return false;
}

Bool SSWR::OrganWeb::OrganWebEnv::UserfileUpdatePos(NN<Sync::RWMutexUsage> mutUsage, Int32 userfileId, Data::Timestamp captureTime, Double lat, Double lon, LocType locType)
{
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return false;
	mutUsage->ReplaceMutex(this->dataMut, true);
	NN<UserFileInfo> userFile;
	if (!this->userFileMap.Get(userfileId).SetTo(userFile))
	{
		return false;
	}
	if (userFile->captureTimeTicks == captureTime.ToTicks() && Math::NearlyEqualsDbl(userFile->lat, lat) && Math::NearlyEqualsDbl(userFile->lon, lon))
	{
		return true;
	}
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("update userfile set captureTime = "));
	sql.AppendTS(captureTime);
	sql.AppendCmdC(CSTR(", lat = "));
	sql.AppendDbl(lat);
	sql.AppendCmdC(CSTR(", lon = "));
	sql.AppendDbl(lon);
	sql.AppendCmdC(CSTR(", locType = "));
	sql.AppendInt32((Int32)locType);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(userFile->id);
	if (db->ExecuteNonQuery(sql.ToCString()) > 0)
	{
		userFile->captureTimeTicks = captureTime.ToTicks();
		userFile->lat = lat;
		userFile->lon = lon;
		return true;
	}
	return false;
}

Bool SSWR::OrganWeb::OrganWebEnv::SpeciesBookIsExist(NN<Sync::RWMutexUsage> mutUsage, Text::CStringNN speciesName, NN<Text::StringBuilderUTF8> bookNameOut)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	NN<BookInfo> book;
	NN<BookSpInfo> bookSp;
	UOSInt nameLen = speciesName.leng;
	UOSInt i = 0;
	UOSInt j = this->bookMap.GetCount();
	UOSInt k;
	while (i < j)
	{
		book = this->bookMap.GetItemNoCheck(i);
		k = book->species.GetCount();
		while (k-- > 0)
		{
			bookSp = book->species.GetItemNoCheck(k);
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

void SSWR::OrganWeb::OrganWebEnv::UserFilePrevUpdated(NN<Sync::RWMutexUsage> mutUsage, NN<UserFileInfo> userFile)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	if (userFile->prevUpdated)
	{
		NN<DB::DBTool> db;
		if (!this->db.SetTo(db))
			return;
		DB::SQLBuilder sql(db);
		sql.AppendCmdC(CSTR("update userfile set prevUpdated = 0 where id = "));
		sql.AppendInt32(userFile->id);
		if (db->ExecuteNonQuery(sql.ToCString()) < 0)
		{
			db->ExecuteNonQuery(sql.ToCString());
		}
		userFile->prevUpdated = 0;
	}
}

void SSWR::OrganWeb::OrganWebEnv::WebFilePrevUpdated(NN<Sync::RWMutexUsage> mutUsage, NN<WebFileInfo> wfile)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	if (wfile->prevUpdated)
	{
		NN<DB::DBTool> db;
		if (!this->db.SetTo(db))
			return;
		DB::SQLBuilder sql(db);
		sql.AppendCmdC(CSTR("update webfile set prevUpdated = 0 where id = "));
		sql.AppendInt32(wfile->id);
		if (db->ExecuteNonQuery(sql.ToCString()) < 0)
		{
			db->ExecuteNonQuery(sql.ToCString());
		}
		wfile->prevUpdated = 0;
	}
}

Bool SSWR::OrganWeb::OrganWebEnv::GPSFileAdd(NN<Sync::RWMutexUsage> mutUsage, Int32 webuserId, Text::CStringNN fileName, Data::Timestamp startTime, Data::Timestamp endTime, UnsafeArray<const UInt8> fileCont, UOSInt fileSize, NN<Map::GPSTrack> gpsTrk, OutParam<Text::CString> errMsg)
{
	NN<WebUserInfo> webUser;
	if (DataFileAdd(mutUsage, webuserId, fileName, startTime, endTime, DataFileType::GPSTrack, fileCont, fileSize, errMsg) && this->userMap.Get(webuserId).SetTo(webUser))
	{
		OSInt startIndex = webUser->userFileIndex.SortedIndexOf(startTime.ToTicks());
		OSInt endIndex = webUser->userFileIndex.SortedIndexOf(endTime.ToTicks());
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
			NN<UserFileInfo> userFile = webUser->userFileObj.GetItemNoCheck((UOSInt)startIndex);
			if (userFile->lat == 0 && userFile->lon == 0)
			{
				Math::Coord2DDbl pos = Math::Coord2DDbl(0, 0);
				Data::Timestamp captureTime = Data::Timestamp(userFile->captureTimeTicks, 0);
				pos = gpsTrk->GetPosByTime(captureTime);
				this->UserfileUpdatePos(mutUsage, userFile->id, captureTime, pos.GetLat(), pos.GetLon(), LocType::GPSTrack);
			}
			startIndex++;
		}
		return true;
	}
	return false;
}

Bool SSWR::OrganWeb::OrganWebEnv::DataFileAdd(NN<Sync::RWMutexUsage> mutUsage, Int32 webuserId, Text::CStringNN fileName, Data::Timestamp startTime, Data::Timestamp endTime, DataFileType fileType, UnsafeArray<const UInt8> fileCont, UOSInt fileSize, OutParam<Text::CString> errMsg)
{
	if (startTime.IsNull())
	{
		errMsg.Set(CSTR("StartTime is null"));
		return false;
	}
	else if (endTime.IsNull())
	{
		errMsg.Set(CSTR("EndTime is null"));
	}
	NN<WebUserInfo> user;
	if (!this->userMap.Get(webuserId).SetTo(user))
	{
		errMsg.Set(CSTR("User not found"));
		return false;
	}
	switch (fileType)
	{
	case DataFileType::GPSTrack:
		if (user->gpsDataFiles.GetIndex(startTime) != user->gpsDataFiles.GetIndex(endTime))
		{
			errMsg.Set(CSTR("GPS file already exist"));
			return false;
		}
		break;
	case DataFileType::Temperature:
		if (user->tempDataFiles.GetIndex(startTime) != user->tempDataFiles.GetIndex(endTime))
		{
			errMsg.Set(CSTR("Temp file already exist"));
			return false;
		}
		break;
	case DataFileType::Unknown:
	default:
		errMsg.Set(CSTR("File type not supported"));
		return false;
	}
	NN<DB::DBTool> db;
	mutUsage->ReplaceMutex(this->dataMut, true);
	if (!this->db.SetTo(db))
	{
		errMsg.Set(CSTR("Database not found"));
		return false;
	}
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> dataFileName;
	UnsafeArray<UTF8Char> sptr = this->dataDir->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = Text::StrConcatC(sptr, UTF8STRC("DataFile"));
	IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	dataFileName = sptr;
	sptr = Text::StrInt32(sptr, webuserId);
	sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
	sptr = Text::StrInt64(sptr, startTime.ToTicks());
	UOSInt i = fileName.LastIndexOf('.');
	if (i != INVALID_INDEX)
	{
		sptr = Text::StrConcatC(sptr, &fileName.v[i], fileName.leng - i);
	}
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("insert into datafile (fileType, startTime, endTime, oriFileName, dataFileName, webuser_id) values ("));
	sql.AppendInt32((Int32)fileType);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendTS(startTime);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendTS(endTime);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrC(fileName);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrUTF8(UnsafeArray<const UTF8Char>(dataFileName));
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(webuserId);
	sql.AppendCmdC(CSTR(")"));
	if (db->ExecuteNonQuery(sql.ToCString()) >= 1)
	{
		Bool succ = true;
		if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) != IO::Path::PathType::Unknown)
		{
			errMsg.Set(CSTR("Datafile already exist in disk"));
			succ = false;
		}
		else
		{
			IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			succ = (fs.WriteCont(fileCont, fileSize) == fileSize);
			if (!succ)
			{
				errMsg.Set(CSTR("Error in writing Datafile to disk"));
			}
		}
		if (succ)
		{
			NN<DataFileInfo> dataFile;
			dataFile = MemAllocNN(DataFileInfo);
			dataFile->id = db->GetLastIdentity32();
			dataFile->fileType = fileType;
			dataFile->startTime = startTime;
			dataFile->endTime = endTime;
			dataFile->webuserId = webuserId;
			dataFile->oriFileName = Text::String::New(fileName);
			dataFile->dataFileName = Text::String::NewP(dataFileName, sptr);
			this->dataFileMap.Put(dataFile->id, dataFile);
			switch (fileType)
			{
			case DataFileType::GPSTrack:
				user->gpsDataFiles.Put(startTime, dataFile);
				break;
			case DataFileType::Temperature:
				user->tempDataFiles.Put(startTime, dataFile);
				break;
			case DataFileType::Unknown:
			default:
				errMsg.Set(CSTR("File type not supported2"));
				return false;
			}
			return true;
		}
		else
		{
			Int32 id = db->GetLastIdentity32();
			sql.Clear();
			sql.AppendCmdC(CSTR("delete from datafile where id = "));
			sql.AppendInt32(id);
			db->ExecuteNonQuery(sql.ToCString());
			return false;
		}
	}
	else
	{
		errMsg.Set(CSTR("Error in writing to database"));
		return false;
	}
}

Optional<IO::ParsedObject> SSWR::OrganWeb::OrganWebEnv::DataFileParse(NN<DataFileInfo> dataFile)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr = this->dataDir->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = Text::StrConcatC(sptr, UTF8STRC("DataFile"));
	IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = dataFile->dataFileName->ConcatTo(sptr);

	IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
	switch (dataFile->fileType)
	{
	case DataFileType::GPSTrack:
		return this->parsers.ParseFileType(fd, IO::ParserType::MapLayer);
	case DataFileType::Temperature:
	case DataFileType::Unknown:
	default:
		return 0;
	}
}

Optional<SSWR::OrganWeb::DataFileInfo> SSWR::OrganWeb::OrganWebEnv::DataFileGet(NN<Sync::RWMutexUsage> mutUsage, Int32 datafileId)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	return this->dataFileMap.Get(datafileId);
}

Optional<SSWR::OrganWeb::GroupInfo> SSWR::OrganWeb::OrganWebEnv::GroupGet(NN<Sync::RWMutexUsage> mutUsage, Int32 id)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	return this->groupMap.Get(id);
}

Int32 SSWR::OrganWeb::OrganWebEnv::GroupAdd(NN<Sync::RWMutexUsage> mutUsage, Text::CStringNN engName, Text::CStringNN chiName, Int32 parentId, Text::CStringNN descr, Int32 groupTypeId, Int32 cateId, GroupFlags flags)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	NN<GroupInfo> group;
	if (!this->groupMap.Get(parentId).SetTo(group))
		return 0;
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return 0;
	DB::SQLBuilder sql(db);
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
	if (db->ExecuteNonQuery(sql.ToCString()) == 1)
	{
		NN<GroupInfo> newGroup;
		NEW_CLASSNN(newGroup, GroupInfo());
		newGroup->id = db->GetLastIdentity32();
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

Bool SSWR::OrganWeb::OrganWebEnv::GroupModify(NN<Sync::RWMutexUsage> mutUsage, Int32 id, Text::CStringNN engName, Text::CStringNN chiName, Text::CStringNN descr, Int32 groupTypeId, GroupFlags flags)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	NN<GroupInfo> group;
	if (!this->groupMap.Get(id).SetTo(group))
		return false;
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return false;
	DB::SQLBuilder sql(db);
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
	if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
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

Bool SSWR::OrganWeb::OrganWebEnv::GroupDelete(NN<Sync::RWMutexUsage> mutUsage, Int32 id)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	NN<GroupInfo> group;
	if (!this->groupMap.Get(id).SetTo(group))
		return false;
	if (group->groups.GetCount() > 0)
		return false;
	if (group->species.GetCount() > 0)
		return false;
	NN<CategoryInfo> cate;
	NN<GroupInfo> parentGroup;
	if (!this->groupMap.Get(group->parentId).SetTo(parentGroup))
		return false;
	if (!this->cateMap.Get(group->cateId).SetTo(cate))
		return false;
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return false;

	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("delete from "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(id);
	if (db->ExecuteNonQuery(sql.ToCString()) == 1)
	{
		parentGroup->groups.Remove(group);
		cate->groups.Remove(group);
		this->groupMap.Remove(group->id);
		FreeGroup(group);
		return true;
	}
	return false;
}

Bool SSWR::OrganWeb::OrganWebEnv::GroupMove(NN<Sync::RWMutexUsage> mutUsage, Int32 groupId, Int32 destGroupId, Int32 cateId)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	NN<GroupInfo> group;
	if (!this->groupMap.Get(groupId).SetTo(group))
		return false;
	if (group->parentId == destGroupId)
	{
		return true;
	}
	Optional<GroupInfo> parentGroup = group;
	NN<GroupInfo> nnparentGroup;
	while (parentGroup.SetTo(nnparentGroup))
	{
		if (nnparentGroup->id == destGroupId)
		{
			return false;
		}
		parentGroup = this->groupMap.Get(nnparentGroup->parentId);
	}
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return false;
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("update "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" set parent_id = "));
	sql.AppendInt32(destGroupId);
	sql.AppendCmdC(CSTR(", cate_id = "));
	sql.AppendInt32(cateId);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(groupId);
	if (db->ExecuteNonQuery(sql.ToCString()) == 1)
	{
		parentGroup = this->groupMap.Get(group->parentId);
		if (parentGroup.SetTo(nnparentGroup))
		{
			nnparentGroup->groups.Remove(group);
			if (nnparentGroup->photoGroup == group->id)
			{
				this->GroupSetPhotoGroup(mutUsage, nnparentGroup->id, 0);
			}
			if (nnparentGroup->groups.GetCount() == 0)
			{
				nnparentGroup->photoSpObj = 0;
			}
			if (group->myPhotoCount != (UOSInt)-1)
			{
				this->GroupAddCounts(mutUsage, nnparentGroup->id, -group->totalCount, -group->photoCount, -group->myPhotoCount);
			}
		}
		group->parentId = destGroupId;
		group->cateId = cateId;
		parentGroup = this->groupMap.Get(group->parentId);
		if (parentGroup.SetTo(nnparentGroup))
		{
			nnparentGroup->groups.Add(group);
			if (group->myPhotoCount != (UOSInt)-1)
			{
				this->GroupAddCounts(mutUsage, nnparentGroup->id, group->totalCount, group->photoCount, group->myPhotoCount);
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::OrganWeb::OrganWebEnv::GroupAddCounts(NN<Sync::RWMutexUsage> mutUsage, Int32 groupId, UOSInt totalCount, UOSInt photoCount, UOSInt myPhotoCount)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	NN<GroupInfo> group;
	if (!this->groupMap.Get(groupId).SetTo(group))
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


Bool SSWR::OrganWeb::OrganWebEnv::GroupSetPhotoSpecies(NN<Sync::RWMutexUsage> mutUsage, Int32 groupId, Int32 photoSpeciesId)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	NN<GroupInfo> group;
	if (!this->groupMap.Get(groupId).SetTo(group))
		return false;
	Optional<SpeciesInfo> photoSpecies = this->spMap.Get(photoSpeciesId);
	if (photoSpeciesId != 0 && photoSpecies.IsNull())
		return false;
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return false;

	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("update "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" set photo_species = "));
	sql.AppendInt32(photoSpeciesId);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(groupId);
	if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		group->photoSpecies = photoSpeciesId;
		if (photoSpecies.IsNull())
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

Bool SSWR::OrganWeb::OrganWebEnv::GroupSetPhotoGroup(NN<Sync::RWMutexUsage> mutUsage, Int32 groupId, Int32 photoGroupId)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	NN<GroupInfo> group;
	if (!this->groupMap.Get(groupId).SetTo(group))
		return false;
	Optional<GroupInfo> photoGroup = this->groupMap.Get(photoGroupId);
	NN<GroupInfo> nnphotoGroup;
	if (photoGroupId != 0 && photoGroup.IsNull())
		return false;
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return false;

	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("update "));
	sql.AppendCol((const UTF8Char*)"groups");
	sql.AppendCmdC(CSTR(" set photo_group = "));
	sql.AppendInt32(photoGroupId);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(groupId);
	if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
	{
		group->photoGroup = photoGroupId;
		if (!photoGroup.SetTo(nnphotoGroup))
		{
			group->photoSpObj = 0;
			this->CalcGroupCount(mutUsage, group);
		}
		else
		{
			group->photoSpObj = nnphotoGroup->photoSpObj;
		}
		return true;
	}
	return false;
}

Bool SSWR::OrganWeb::OrganWebEnv::GroupIsPublic(NN<Sync::RWMutexUsage> mutUsage, Int32 groupId)
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
	NN<GroupInfo> group;
	if (!this->groupMap.Get(groupId).SetTo(group))
	{
		return false;
	}
	return GroupIsPublic(mutUsage, group->parentId);
}

Optional<SSWR::OrganWeb::CategoryInfo> SSWR::OrganWeb::OrganWebEnv::CateGet(NN<Sync::RWMutexUsage> mutUsage, Int32 id)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	return this->cateMap.Get(id);
}

Optional<SSWR::OrganWeb::CategoryInfo> SSWR::OrganWeb::OrganWebEnv::CateGetByName(NN<Sync::RWMutexUsage> mutUsage, NN<Text::String> name)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	return this->cateSMap.GetNN(name);
}

NN<Data::ReadingListNN<SSWR::OrganWeb::CategoryInfo>> SSWR::OrganWeb::OrganWebEnv::CateGetList(NN<Sync::RWMutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->dataMut, false);
	return this->cateMap;
}

UOSInt SSWR::OrganWeb::OrganWebEnv::PeakGetUnfin(NN<Sync::RWMutexUsage> mutUsage, NN<Data::ArrayListNN<PeakInfo>> peaks)
{
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
		return 0;
	mutUsage->ReplaceMutex(this->dataMut, true);
	NN<DB::DBReader> r;
	NN<PeakInfo> peak;
	UOSInt ret = 0;
	if (db->ExecuteReader(CSTR("select id, RefId, District, MapX, MapY, MarkedHeight, csys, status, name, type from peak where status = 0")).SetTo(r))
	{
		while (r->ReadNext())
		{
			peak = MemAllocNN(PeakInfo);
			peak->id = r->GetInt32(0);
			peak->refId = r->GetNewStrNN(1);
			peak->district = r->GetNewStrNN(2);
			peak->mapX = r->GetDblOr(3, 0);
			peak->mapY = r->GetDblOr(4, 0);
			peak->markedHeight = r->GetDblOr(5, 0);
			peak->csys = r->GetInt32(6);
			peak->status = r->GetInt32(7);
			peak->name = r->GetNewStr(8);
			peak->type = r->GetNewStr(9);
			peaks->Add(peak);
			ret++;
		}
		db->CloseReader(r);
	}
	return ret;
}

Bool SSWR::OrganWeb::OrganWebEnv::PeakUpdateStatus(NN<Sync::RWMutexUsage> mutUsage, Int32 id, Int32 status)
{
	mutUsage->ReplaceMutex(this->dataMut, true);
	NN<DB::DBTool> db;
	if (this->db.SetTo(db))
	{
		DB::SQLBuilder sql(db);
		sql.AppendCmdC(CSTR("update peak set status = "));
		sql.AppendInt32(status);
		sql.AppendCmdC(CSTR(" where id = "));
		sql.AppendInt32(id);
		sql.AppendCmdC(CSTR(" and status = 0"));
		return db->ExecuteNonQuery(sql.ToCString()) > 0;
	}
	return false;
}

void SSWR::OrganWeb::OrganWebEnv::PeakFreeAll(NN<Data::ArrayListNN<PeakInfo>> peaks)
{
	NN<PeakInfo> peak;
	Data::ArrayIterator<NN<PeakInfo>> it = peaks->Iterator();
	while (it.HasNext())
	{
		peak = it.Next();
		peak->refId->Release();
		peak->district->Release();
		OPTSTR_DEL(peak->name);
		OPTSTR_DEL(peak->type);
		MemFreeNN(peak);
	}
	peaks->Clear();
}

Optional<IO::ConfigFile> SSWR::OrganWeb::OrganWebEnv::LangGet(NN<Net::WebServer::WebRequest> req)
{
	Text::StringBuilderUTF8 sb;
	NN<IO::ConfigFile> lang;
	Text::PString sarr[2];
	Text::PString sarr2[2];
	UOSInt i;
	NN<Text::Locale::LocaleEntry> ent;
	if (req->GetHeaderC(sb, CSTR("Accept-Language")))
	{
		sarr[1] = sb;
		i = 2;
		while (i == 2)
		{
			i = Text::StrSplitP(sarr, 2, sarr[1], ',');
			Text::StrSplitP(sarr2, 2, sarr[0], ';');
			if (this->locale.GetLocaleEntryByName(sarr2[0].ToCString()).SetTo(ent))
			{
				if (this->langMap.Get(ent->lcid).SetTo(lang))
					return lang;
			}
		}
	}
	if (this->langMap.Get(0x409).SetTo(lang))
		return lang;
	return this->langMap.GetItem(0);
}
