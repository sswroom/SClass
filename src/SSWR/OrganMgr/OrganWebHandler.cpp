#include "Stdafx.h"
#include "Crypto/Hash/CRC32R.h"
#include "Crypto/Hash/MD5.h"
#include "Data/ArrayListICaseString.h"
#include "Data/ArrayListString.h"
#include "Data/ByteTool.h"
#include "Data/FastMap.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "DB/DBReader.h"
#include "Exporter/GUIJPGExporter.h"
#include "IO/FileStream.h"
#include "IO/IniFile.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/StreamReader.h"
#include "IO/StmData/FileData.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Map/IMapDrawLayer.h"
#include "Math/Math.h"
#include "Media/FrequencyGraph.h"
#include "Media/IAudioSource.h"
#include "Media/ICCProfile.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "Media/MediaFile.h"
#include "Media/PhotoInfo.h"
#include "Net/WebServer/WebSessionUsage.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Parser/FullParserList.h"
#include "SSWR/OrganMgr/OrganWebHandler.h"
#include "Sync/MutexUsage.h"
#include "Text/JSText.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "Text/XML.h"
#include "Text/TextBinEnc/URIEncoding.h"

#define SP_PER_PAGE_DESKTOP 100
#define SP_PER_PAGE_MOBILE 90
#define PREVIEW_SIZE 320

SSWR::OrganMgr::OrganWebHandler::SpeciesSciNameComparator::~SpeciesSciNameComparator()
{
}

OSInt SSWR::OrganMgr::OrganWebHandler::SpeciesSciNameComparator::Compare(SpeciesInfo *a, SpeciesInfo *b) const
{
	if (a->sciNameHash > b->sciNameHash)
	{
		return 1;
	}
	else if (a->sciNameHash < b->sciNameHash)
	{
		return -1;
	}
	else
	{
		return a->sciName->CompareToFast(b->sciName->ToCString());
	}
}

SSWR::OrganMgr::OrganWebHandler::UserFileTimeComparator::~UserFileTimeComparator()
{

}

OSInt SSWR::OrganMgr::OrganWebHandler::UserFileTimeComparator::Compare(UserFileInfo *a, UserFileInfo *b) const
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

SSWR::OrganMgr::OrganWebHandler::UserFileDescComparator::UserFileDescComparator(RequestEnv *env)
{
	this->env = env;
}

SSWR::OrganMgr::OrganWebHandler::UserFileDescComparator::~UserFileDescComparator()
{
}

OSInt SSWR::OrganMgr::OrganWebHandler::UserFileDescComparator::Compare(UserFileInfo *a, UserFileInfo *b) const
{
	Bool aDesc = false;
	Bool bDesc = false;
	if (env->user != 0)
	{
		if (a->descript != 0 && a->descript->leng > 0 && (env->user->userType == 0 || a->webuserId == env->user->id))
			aDesc = true;
		if (b->descript != 0 && b->descript->leng > 0 && (env->user->userType == 0 || b->webuserId == env->user->id))
			bDesc = true;
	}
	if (aDesc && bDesc)
	{
		OSInt ret = a->descript->CompareTo(b->descript);
		if (ret != 0)
			return ret;
	}
	else if (aDesc)
	{
		return -1;
	}
	else if (bDesc)
	{
		return 1;
	}
	if (a->captureTimeTicks > b->captureTimeTicks)
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

void SSWR::OrganMgr::OrganWebHandler::LoadCategory()
{
	Text::StringBuilderUTF8 sb;
	SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
	SSWR::OrganMgr::OrganWebHandler::GroupTypeInfo *grpType;
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
				NEW_CLASS(cate, SSWR::OrganMgr::OrganWebHandler::CategoryInfo());
				cate->cateId = cateId;
				cate->chiName = r->GetNewStrB(1, &sb, false);
				cate->dirName = r->GetNewStrB(2, &sb, false);
				sb.ClearStr();
				r->GetStr(3, &sb);
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
				grpType = MemAlloc(SSWR::OrganMgr::OrganWebHandler::GroupTypeInfo, 1);
				grpType->id = r->GetInt32(0);
				grpType->engName = r->GetNewStrB(1, &sb, false);
				grpType->chiName = r->GetNewStrB(2, &sb, false);
				cate->groupTypes.Put(grpType->id, grpType);
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
	Text::StringBuilderUTF8 sb;
	DB::DBReader *r = this->db->ExecuteReader(CSTR("select id, eng_name, chi_name, sci_name, group_id, description, dirName, photo, idKey, cate_id, flags, photoId, photoWId, poiImg from species"));
	if (r != 0)
	{
		while (r->ReadNext())
		{
			NEW_CLASS(sp, SSWR::OrganMgr::OrganWebHandler::SpeciesInfo());
			sp->speciesId = r->GetInt32(0);
			sp->engName = r->GetNewStrB(1, &sb, true);
			sp->chiName = r->GetNewStrB(2, &sb, true);
			sp->sciName = r->GetNewStrB(3, &sb, false);
			sp->groupId = r->GetInt32(4);
			sp->descript = r->GetNewStrB(5, &sb, true);
			sp->dirName = r->GetNewStrB(6, &sb, true);
			sp->photo = r->GetNewStrB(7, &sb, false);
			sp->idKey = r->GetNewStrB(8, &sb, true);
			sp->cateId = r->GetInt32(9);
			sp->flags = (SpeciesFlags)r->GetInt32(10);
			sp->photoId = r->GetInt32(11);
			sp->photoWId = r->GetInt32(12);
			sp->poiImg = r->GetNewStrB(13, &sb, false);

			this->spMap.Put(sp->speciesId, sp);
			sp->sciNameHash = this->spNameMap.CalcHash(sp->sciName->v, sp->sciName->leng);
		}
		this->db->CloseReader(r);

		SpeciesSciNameComparator comparator;
		Data::ArrayList<SpeciesInfo*> speciesList(this->spMap.GetCount());
		speciesList.AddAll(&this->spMap);
		Data::Sort::ArtificialQuickSort::Sort(&speciesList, &comparator);
		UOSInt i = 0;
		UOSInt j = speciesList.GetCount();
		while (i < j)
		{
			sp = speciesList.GetItem(i);
			this->spNameMap.Put(sp->sciName, sp);
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
				wfile = MemAlloc(SSWR::OrganMgr::OrganWebHandler::WebFileInfo, 1);
				wfile->id = r->GetInt32(0);
				wfile->crcVal = r->GetInt32(2);
				wfile->imgUrl = r->GetNewStrB(3, &sb, false);
				wfile->srcUrl = r->GetNewStrB(4, &sb, false);
				wfile->prevUpdated = r->GetBool(5);
				wfile->cropLeft = r->GetDbl(6);
				wfile->cropTop = r->GetDbl(7);
				wfile->cropRight = r->GetDbl(8);
				wfile->cropBottom = r->GetDbl(9);
				wfile->location = r->GetNewStrB(10, &sb, false);
				sp->wfiles.Put(wfile->id, wfile);
			}
		}
		this->db->CloseReader(r);
	}
}

void SSWR::OrganMgr::OrganWebHandler::LoadGroups()
{
	FreeGroups();

	Text::StringBuilderUTF8 sb;
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *pGroup;
	SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
	UOSInt i;
	DB::DBReader *r = this->db->ExecuteReader(CSTR("select id, group_type, eng_name, chi_name, description, parent_id, photo_group, photo_species, idKey, cate_id, flags from groups"));
	if (r != 0)
	{
		while (r->ReadNext())
		{
			NEW_CLASS(group, SSWR::OrganMgr::OrganWebHandler::GroupInfo());
			group->id = r->GetInt32(0);
			group->groupType = r->GetInt32(1);
			group->engName = r->GetNewStrB(2, &sb, false);
			group->chiName = r->GetNewStrB(3, &sb, false);
			group->descript = r->GetNewStrB(4, &sb, true);
			group->parentId = r->GetInt32(5);
			group->photoGroup = r->GetInt32(6);
			group->photoSpecies = r->GetInt32(7);
			group->idKey = r->GetNewStrB(8, &sb, true);
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

void SSWR::OrganMgr::OrganWebHandler::LoadBooks()
{
	FreeBooks();

	Text::StringBuilderUTF8 sb;
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
	SSWR::OrganMgr::OrganWebHandler::BookInfo *book;
	SSWR::OrganMgr::OrganWebHandler::BookSpInfo *bookSp;
	Data::DateTime dt;

	DB::DBReader *r = this->db->ExecuteReader(CSTR("select id, title, dispAuthor, press, publishDate, url from book"));
	if (r != 0)
	{
		while (r->ReadNext())
		{
			NEW_CLASS(book, SSWR::OrganMgr::OrganWebHandler::BookInfo());
			book->id = r->GetInt32(0);
			book->title = r->GetNewStrB(1, &sb, false);
			book->author = r->GetNewStrB(2, &sb, false);
			book->press = r->GetNewStrB(3, &sb, false);
			book->publishDate = r->GetTimestamp(4).ToTicks();
			book->url = r->GetNewStrB(5, &sb, false);

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
				bookSp = MemAlloc(SSWR::OrganMgr::OrganWebHandler::BookSpInfo, 1);
				bookSp->bookId = book->id;
				bookSp->speciesId = sp->speciesId;
				bookSp->dispName = r->GetNewStrB(2, &sb, false);
				book->species.Add(bookSp);
				sp->books.Add(bookSp);
			}
		}
		this->db->CloseReader(r);
	}
}

void SSWR::OrganMgr::OrganWebHandler::LoadUsers()
{
	this->ClearUsers();

	Int32 userId;
	Text::StringBuilderUTF8 sb;
	SSWR::OrganMgr::OrganWebHandler::WebUserInfo *user;
	DB::DBReader *r = this->db->ExecuteReader(CSTR("select id, userName, pwd, watermark, userType from webuser"));
	if (r != 0)
	{
		while (r->ReadNext())
		{
			userId = r->GetInt32(0);
			user = this->userMap.Get(userId);
			if (user)
			{
				this->userNameMap.Remove(user->userName);
				SDEL_STRING(user->userName);
				user->userName = r->GetNewStrB(1, &sb, false);
				SDEL_STRING(user->pwd);
				user->pwd = r->GetNewStrB(2, &sb, false);
				SDEL_STRING(user->watermark);
				user->watermark = r->GetNewStrB(3, &sb, false);
				user->userType = r->GetInt32(4);
				this->userNameMap.Put(user->userName, user);
			}
			else
			{
				NEW_CLASS(user, SSWR::OrganMgr::OrganWebHandler::WebUserInfo());
				user->id = userId;
				user->userName = r->GetNewStrB(1, &sb, false);
				user->pwd = r->GetNewStrB(2, &sb, false);
				user->watermark = r->GetNewStrB(3, &sb, false);
				user->userType = r->GetInt32(4);
				user->unorganSpId = 0;
				this->userMap.Put(user->id, user);
				this->userNameMap.Put(user->userName, user);
			}
		}
		this->db->CloseReader(r);
	}

	r = this->db->ExecuteReader(CSTR("select id, fileType, oriFileName, fileTime, lat, lon, webuser_id, species_id, captureTime, dataFileName, crcVal, rotType, prevUpdated, cropLeft, cropTop, cropRight, cropBottom, descript, location from userfile"));
	if (r != 0)
	{
		SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile;
		SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species;
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
				userFile = MemAlloc(SSWR::OrganMgr::OrganWebHandler::UserFileInfo, 1);
				userFile->id = r->GetInt32(0);
				userFile->fileType = r->GetInt32(1);
				userFile->oriFileName = r->GetNewStrB(2, &sb, false);
				userFile->fileTimeTicks = r->GetTimestamp(3).ToTicks();
				userFile->lat = r->GetDbl(4);
				userFile->lon = r->GetDbl(5);
				userFile->webuserId = userId;
				userFile->speciesId = r->GetInt32(7);
				userFile->captureTimeTicks = r->GetTimestamp(8).ToTicks();
				userFile->dataFileName = r->GetNewStrB(9, &sb, false);
				userFile->crcVal = (UInt32)r->GetInt32(10);
				userFile->rotType = r->GetInt32(11);
				userFile->prevUpdated = r->GetInt32(12);
				userFile->cropLeft = r->GetDbl(13);
				userFile->cropTop = r->GetDbl(14);
				userFile->cropRight = r->GetDbl(15);
				userFile->cropBottom = r->GetDbl(16);
				userFile->descript = r->GetNewStrB(17, &sb, false);
				userFile->location = r->GetNewStrB(18, &sb, false);
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
		userFileList.AddAll(&this->userFileMap);
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

	r = this->db->ExecuteReader(CSTR("select fromDate, toDate, locId, cate_id, webuser_id from trip"));
	if (r != 0)
	{
		Int32 cateId;
		Int64 fromDate;
		Data::FastMap<Int64, SSWR::OrganMgr::OrganWebHandler::TripInfo*> *tripCate;
		SSWR::OrganMgr::OrganWebHandler::TripInfo *trip;
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
					NEW_CLASS(tripCate, Data::Int64FastMap<SSWR::OrganMgr::OrganWebHandler::TripInfo*>());
					user->tripCates.Put(cateId, tripCate);
				}
				trip = tripCate->Get(fromDate);
				if (trip == 0)
				{
					trip = MemAlloc(SSWR::OrganMgr::OrganWebHandler::TripInfo, 1);
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
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
		group = this->groupMap.Get(this->unorganizedGroupId);
		if (group != 0)
		{
			UOSInt i = this->userMap.GetCount();
			UOSInt j;
			SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species;
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
						user->unorganSpId = this->SpeciesAdd(CSTR(""), user->userName->ToCString(), sbSName.ToCString(), group->id, CSTR(""), sb.ToCString(), CSTR(""), group->cateId);
					}
				}
			}
		}
	}
}

void SSWR::OrganMgr::OrganWebHandler::LoadLocations()
{
	SSWR::OrganMgr::OrganWebHandler::LocationInfo *loc;
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
				loc = MemAlloc(SSWR::OrganMgr::OrganWebHandler::LocationInfo, 1);
				loc->id = id;
				loc->parentId = r->GetInt32(1);
				loc->cname = r->GetNewStrB(2, &sb, false);
				loc->ename = r->GetNewStrB(3, &sb, false);
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

void SSWR::OrganMgr::OrganWebHandler::FreeSpecies()
{
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
	SSWR::OrganMgr::OrganWebHandler::WebFileInfo *wfile;
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

void SSWR::OrganMgr::OrganWebHandler::FreeGroups()
{
	SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
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

void SSWR::OrganMgr::OrganWebHandler::FreeGroup(GroupInfo *group)
{
	group->engName->Release();
	group->chiName->Release();
	group->descript->Release();
	SDEL_STRING(group->idKey);
	DEL_CLASS(group);
}

void SSWR::OrganMgr::OrganWebHandler::FreeBooks()
{
	SSWR::OrganMgr::OrganWebHandler::BookInfo *book;
	SSWR::OrganMgr::OrganWebHandler::BookSpInfo *bookSp;
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

void SSWR::OrganMgr::OrganWebHandler::FreeUsers()
{
	SSWR::OrganMgr::OrganWebHandler::WebUserInfo *user;
	SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile;
	const Data::FastMap<Int64, SSWR::OrganMgr::OrganWebHandler::TripInfo*> *tripCate;
	SSWR::OrganMgr::OrganWebHandler::TripInfo *trip;
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
}

void SSWR::OrganMgr::OrganWebHandler::ClearUsers()
{
	SSWR::OrganMgr::OrganWebHandler::WebUserInfo *user;
	SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile;
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
	}
	this->userFileMap.Clear();
}

void SSWR::OrganMgr::OrganWebHandler::UserFilePrevUpdated(SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile)
{
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

void SSWR::OrganMgr::OrganWebHandler::WebFilePrevUpdated(SSWR::OrganMgr::OrganWebHandler::WebFileInfo *wfile)
{
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

void SSWR::OrganMgr::OrganWebHandler::GetGroupSpecies(SSWR::OrganMgr::OrganWebHandler::GroupInfo *group, Data::DataMap<Text::String*, SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> *spMap, SSWR::OrganMgr::OrganWebHandler::WebUserInfo *user)
{
	UOSInt i;
	UOSInt j;
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *sgroup;
	i = 0;
	j = group->species.GetCount();
	while (i < j)
	{
		sp = group->species.GetItem(i);
		spMap->Put(sp->sciName, sp);
		i++;
	}
	i = group->groups.GetCount();
	while (i-- > 0)
	{
		sgroup = group->groups.GetItem(i);
		if ((sgroup->flags & 1) == 0 || user != 0)
		{
			GetGroupSpecies(sgroup, spMap, user);
		}
	}
}

void SSWR::OrganMgr::OrganWebHandler::SearchInGroup(SSWR::OrganMgr::OrganWebHandler::GroupInfo *group, const UTF8Char *searchStr, UOSInt searchStrLen, Data::ArrayListDbl *speciesIndice, Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> *speciesObjs, Data::ArrayListDbl *groupIndice, Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupInfo*> *groupObjs, SSWR::OrganMgr::OrganWebHandler::WebUserInfo *user)
{
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species;
	SSWR::OrganMgr::OrganWebHandler::BookSpInfo *bookSp;
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *subGroup;
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
			SearchInGroup(subGroup, searchStr, searchStrLen, speciesIndice, speciesObjs, groupIndice, groupObjs, user);
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
		group = this->groupMap.Get(group->parentId);
	}
	return false;
}

UTF8Char *SSWR::OrganMgr::OrganWebHandler::PasswordEnc(UTF8Char *buff, Text::CString pwd)
{
	UInt8 md5Val[16];
	Crypto::Hash::MD5 md5;
	md5.Calc(pwd.v, pwd.leng);
	md5.GetValue(md5Val);
	return Text::StrHexBytes(buff, md5Val, 16, 0);
}

Bool SSWR::OrganMgr::OrganWebHandler::BookFileExist(BookInfo *book)
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

Bool SSWR::OrganMgr::OrganWebHandler::UserGPSGetPos(Int32 userId, const Data::Timestamp &t, Double *lat, Double *lon)
{
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

Int32 SSWR::OrganMgr::OrganWebHandler::SpeciesAdd(Text::CString engName, Text::CString chiName, Text::CString sciName, Int32 groupId, Text::CString description, Text::CString dirName, Text::CString idKey, Int32 cateId)
{
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
		SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species;
		NEW_CLASS(species, SSWR::OrganMgr::OrganWebHandler::SpeciesInfo());
		species->speciesId = this->db->GetLastIdentity32();
		species->engName = Text::String::NewOrNull(engName);
		species->chiName = Text::String::NewOrNull(chiName);
		species->sciName = Text::String::NewOrNull(sciName);
		species->groupId = groupId;
		species->descript = Text::String::NewOrNull(description);
		species->dirName = Text::String::NewOrNull(dirName);
		species->photo = 0;
		species->idKey = Text::String::NewOrNull(idKey);
		species->cateId = cateId;
		species->flags = SF_NONE;
		species->photoId = 0;
		species->photoWId = 0;

		this->spMap.Put(species->speciesId, species);
		this->spNameMap.PutC(species->sciName->ToCString(), species);

		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group = this->groupMap.Get(species->groupId);
		if (group)
		{
			group->species.Add(species);
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
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species = this->spMap.Get(speciesId);
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

Bool SSWR::OrganMgr::OrganWebHandler::SpeciesSetFlags(Int32 speciesId, SpeciesFlags flags)
{

	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species = this->spMap.Get(speciesId);
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

Bool SSWR::OrganMgr::OrganWebHandler::SpeciesMove(Int32 speciesId, Int32 groupId, Int32 cateId)
{
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species = this->spMap.Get(speciesId);
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
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group = this->groupMap.Get(species->groupId);
		if (group)
		{
			group->species.Remove(species);
			if (group->photoSpecies == species->speciesId)
			{
				group->photoSpObj = 0;
				this->GroupSetPhotoSpecies(group->id, 0);
			}
			this->GroupAddCounts(group->id, -totalCount, -photoCount, -myPhotoCount);
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
			this->GroupAddCounts(group->id, totalCount, photoCount, myPhotoCount);
		}
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::OrganMgr::OrganWebHandler::SpeciesModify(Int32 speciesId, Text::CString engName, Text::CString chiName, Text::CString sciName, Text::CString description, Text::CString dirName)
{
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species = this->spMap.Get(speciesId);
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
			this->spNameMap.Remove(species->sciName);
			this->spNameMap.PutC(sciName, species);
		}
		SDEL_STRING(species->engName);
		species->engName = Text::String::NewOrNull(engName);
		SDEL_STRING(species->chiName);
		species->chiName = Text::String::NewOrNull(chiName);
		SDEL_STRING(species->sciName);
		species->sciName = Text::String::NewOrNull(sciName);
		SDEL_STRING(species->descript);
		species->descript = Text::String::NewOrNull(description);
		SDEL_STRING(species->dirName);
		species->dirName = Text::String::NewOrNull(dirName);
		return true;
	}
	else
	{
		return false;
	}
}

Int32 SSWR::OrganMgr::OrganWebHandler::UserfileAdd(Int32 userId, Int32 spId, Text::CString fileName, const UInt8 *fileCont, UOSInt fileSize, Bool mustHaveCamera)
{
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
		SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile;
		Text::String *camera = 0;
		UInt32 crcVal = 0;

		{
			IO::StmData::MemoryDataRef md(fileCont, fileSize);
			pobj = this->parsers.ParseFile(&md, &t);
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
							this->UserGPSGetPos(userId, fileTime, &lat, &lon);
						}
						Text::CString cstr;
						Text::CString cstr2;
						cstr = exif->GetPhotoMake();
						cstr2 = exif->GetPhotoModel();
						if (cstr.v && cstr2.v)
						{
							if (cstr2.StartsWithICase(cstr.v, cstr.leng))
							{
								camera = Text::String::New(cstr2);
							}
							else
							{
								Text::StringBuilderUTF8 sb;
								sb.Append(cstr);
								sb.AppendC(UTF8STRC(" "));
								sb.Append(cstr2);
								camera = Text::String::New(sb.ToString(), sb.GetLength());
							}
						}
						else if (cstr.v)
						{
							camera = Text::String::New(cstr);
						}
						else if (cstr2.v)
						{
							camera = Text::String::New(cstr2);
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
			SSWR::OrganMgr::OrganWebHandler::WebUserInfo *webUser = this->userMap.Get(userId);
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
					sql.AppendCmdC(CSTR("insert into userfile (fileType, oriFileName, fileTime, lat, lon, webuser_id, species_id, captureTime, dataFileName, crcVal, rotType, camera, cropLeft, cropTop, cropRight, cropBottom) values ("));
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
					sql.AppendCmdC(CSTR(")"));
					if (this->db->ExecuteNonQuery(sql.ToCString()) > 0)
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
						userFile->location = 0;
						this->userFileMap.Put(userFile->id, userFile);

						SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species = this->spMap.Get(userFile->speciesId);
						if (species)
						{
							species->files.Add(userFile);
							if (species->photoId == 0)
							{
								this->SpeciesSetPhotoId(species->speciesId, userFile->id);
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
		SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile;
		Bool valid = false;
		Media::DrawImage *graphImg = 0;
		UInt8 crcBuff[4];
		crc.Calc(fileCont, fileSize);
		crc.GetValue(crcBuff);
		crcVal = ReadMUInt32(crcBuff);

		{
			IO::StmData::FileData fd(fileName, false);
			pobj = this->parsers.ParseFile(&fd, &t);
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
			SSWR::OrganMgr::OrganWebHandler::WebUserInfo *webUser = this->userMap.Get(userId);
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
					sql.AppendCmdC(CSTR("insert into userfile (fileType, oriFileName, fileTime, lat, lon, webuser_id, species_id, captureTime, dataFileName, crcVal, camera) values ("));
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
					sql.AppendCmdC(CSTR(")"));
					if (this->db->ExecuteNonQuery(sql.ToCString()) > 0)
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
						userFile->dataFileName = Text::String::NewP(dataFileName, sptr);
						userFile->crcVal = crcVal;
						userFile->rotType = 0;
						//userFile->camera = 0;
						userFile->descript = 0;
						userFile->cropLeft = 0;
						userFile->cropTop = 0;
						userFile->cropRight = 0;
						userFile->cropBottom = 0;
						this->userFileMap.Put(userFile->id, userFile);

						SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species = this->spMap.Get(userFile->speciesId);
						if (species)
						{
							species->files.Add(userFile);
							if (species->photoId == 0)
							{
								this->SpeciesSetPhotoId(species->speciesId, userFile->id);
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
							graphImg->SavePng(&fs);
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

Bool SSWR::OrganMgr::OrganWebHandler::UserfileMove(Int32 userfileId, Int32 speciesId, Int32 cateId)
{
	UserFileInfo *userFile = this->userFileMap.Get(userfileId);
	if (userFile == 0)
	{
		return false;
	}
	if (userFile->speciesId == speciesId)
	{
		return true;
	}
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *srcSpecies = this->spMap.Get(userFile->speciesId);
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *destSpecies = this->spMap.Get(speciesId);
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
				break;
			}
		}

		destSpecies->files.Add(userFile);
		if ((destSpecies->flags & 1) == 0)
		{
			this->SpeciesSetFlags(destSpecies->speciesId, (SpeciesFlags)(destSpecies->flags | SF_HAS_MYPHOTO));
			this->GroupAddCounts(destSpecies->groupId, 0, 1, 1);
			SSWR::OrganMgr::OrganWebHandler::GroupInfo *group = this->groupMap.Get(destSpecies->groupId);
			while (group && group->photoSpObj == 0)
			{
				group->photoSpObj = destSpecies;
				group = this->groupMap.Get(group->parentId);
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

Bool SSWR::OrganMgr::OrganWebHandler::UserfileUpdateDesc(Int32 userfileId, Text::CString descr)
{
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

Bool SSWR::OrganMgr::OrganWebHandler::UserfileUpdateRotType(Int32 userfileId, Int32 rotType)
{
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

Bool SSWR::OrganMgr::OrganWebHandler::SpeciesBookIsExist(Text::CString speciesName, Text::StringBuilderUTF8 *bookNameOut)
{
	SSWR::OrganMgr::OrganWebHandler::BookInfo *book;
	SSWR::OrganMgr::OrganWebHandler::BookSpInfo *bookSp;
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
			if (bookSp->dispName && bookSp->dispName->Equals(speciesName.v, nameLen))
			{
				bookNameOut->Append(book->title);
				return true;
			}
		}
		i++;
	}
	return false;
}

Int32 SSWR::OrganMgr::OrganWebHandler::GroupAdd(Text::CString engName, Text::CString chiName, Int32 parentId, Text::CString descr, Int32 groupTypeId, Int32 cateId, GroupFlags flags)
{
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *group = this->groupMap.Get(parentId);
	if (group == 0)
		return 0;
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("insert into groups (group_type, eng_name, chi_name, description, parent_id, idKey, cate_id, flags) values ("));
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
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *newGroup;
		NEW_CLASS(newGroup, SSWR::OrganMgr::OrganWebHandler::GroupInfo());
		newGroup->id = this->db->GetLastIdentity32();
		newGroup->groupType = groupTypeId;
		newGroup->engName = Text::String::NewOrNull(engName);
		newGroup->chiName = Text::String::NewOrNull(chiName);
		newGroup->descript = Text::String::NewOrNull(descr);
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

Bool SSWR::OrganMgr::OrganWebHandler::GroupModify(Int32 id, Text::CString engName, Text::CString chiName, Text::CString descr, Int32 groupTypeId, GroupFlags flags)
{
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *group = this->groupMap.Get(id);
	if (group == 0)
		return false;
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("update groups set group_type = "));
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
		SDEL_STRING(group->engName);
		group->engName = Text::String::NewOrNull(engName);
		SDEL_STRING(group->chiName);
		group->chiName = Text::String::NewOrNull(chiName);
		SDEL_STRING(group->descript);
		group->descript = Text::String::NewOrNull(descr);
		group->flags = flags;
		return true;
	}
	return false;
}

Bool SSWR::OrganMgr::OrganWebHandler::GroupDelete(Int32 id)
{
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *group = this->groupMap.Get(id);
	if (group == 0)
		return false;
	if (group->groups.GetCount() > 0)
		return false;
	if (group->species.GetCount() > 0)
		return false;
	SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate = this->cateMap.Get(group->cateId);
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *parentGroup = this->groupMap.Get(group->parentId);
	if (parentGroup == 0)
		return false;
	if (cate == 0)
		return false;

	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("delete from groups where id = "));
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

Bool SSWR::OrganMgr::OrganWebHandler::GroupMove(Int32 groupId, Int32 destGroupId, Int32 cateId)
{
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *group = this->groupMap.Get(groupId);
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
		parentGroup = this->groupMap.Get(parentGroup->parentId);
	}
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("update groups set parent_id = "));
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
				this->GroupSetPhotoGroup(parentGroup->id, 0);
			}
			if (parentGroup->groups.GetCount() == 0)
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
		parentGroup = this->groupMap.Get(group->parentId);
		if (parentGroup)
		{
			parentGroup->groups.Add(group);
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
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *group = this->groupMap.Get(groupId);
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
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *group = this->groupMap.Get(groupId);
	if (group == 0)
		return false;
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *photoSpecies = this->spMap.Get(photoSpeciesId);
	if (photoSpeciesId != 0 && photoSpecies == 0)
		return false;

	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("update groups set photo_species = "));
	sql.AppendInt32(photoSpeciesId);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(groupId);
	if (this->db->ExecuteNonQuery(sql.ToCString()) >= 0)
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
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *group = this->groupMap.Get(groupId);
	if (group == 0)
		return false;
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *photoGroup = this->groupMap.Get(photoGroupId);
	if (photoGroupId != 0 && photoGroup == 0)
		return false;

	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("update groups set photo_group = "));
	sql.AppendInt32(photoGroupId);
	sql.AppendCmdC(CSTR(" where id = "));
	sql.AppendInt32(groupId);
	if (this->db->ExecuteNonQuery(sql.ToCString()) >= 0)
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

Bool SSWR::OrganMgr::OrganWebHandler::GroupIsPublic(Int32 groupId)
{
	if (groupId == 0)
	{
		return false;
	}
	else if (groupId == 21593)
	{
		return true;
	}
	GroupInfo *group = this->groupMap.Get(groupId);
	if (group == 0)
	{
		return false;
	}
	return GroupIsPublic(group->parentId);
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
		env->user = (SSWR::OrganMgr::OrganWebHandler::WebUserInfo*)sess->GetValuePtr(UTF8STRC("User"));
		env->pickObjType = (SSWR::OrganMgr::OrganWebHandler::PickObjType)sess->GetValueInt32(UTF8STRC("PickObjType"));
		env->pickObjs = (Data::ArrayListInt32*)sess->GetValuePtr(UTF8STRC("PickObjs"));
		t = (Data::DateTime*)sess->GetValuePtr(UTF8STRC("LastUseTime"));
		t->SetCurrTimeUTC();
		if (keepSess)
		{
			return sess;
		}
		sess->EndUse();
	}
	return 0;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcPhoto(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
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
	if (req->GetQueryValueI32(CSTR("id"), &spId) &&
		req->GetQueryValueI32(CSTR("cateId"), &cateId) &&
		req->GetQueryValueU32(CSTR("width"), &width) &&
		req->GetQueryValueU32(CSTR("height"), &height) &&
		spId > 0 && width > 0 && height > 0 && cateId > 0 && width <= 10000 && height <= 10000
		)
	{
		if (req->GetQueryValueI32(CSTR("fileId"), &id))
		{
			me->ResponsePhotoId(req, resp, env.user, env.isMobile, spId, cateId, width, height, id);
			return true;
		}
		else if (req->GetQueryValueI32(CSTR("fileWId"), &id))
		{
			me->ResponsePhotoWId(req, resp, env.user, env.isMobile, spId, cateId, width, height, id);
			return true;
		}
		else if (req->GetQueryValueStr(CSTR("file"), sbuff, 512))
		{
			me->ResponsePhoto(req, resp, env.user, env.isMobile, spId, cateId, width, height, sbuff);
			return true;
		}
	}
	resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcPhotoDown(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 spId;
	Int32 cateId;
	Int32 fileId;
	if (req->GetQueryValueI32(CSTR("id"), &spId) &&
		req->GetQueryValueI32(CSTR("cateId"), &cateId) &&
		req->GetQueryValueI32(CSTR("fileId"), &fileId))
	{
		SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile;
		me->dataMut.LockRead();
		sp = me->spMap.Get(spId);
		userFile = me->userFileMap.Get(fileId);
		if (sp && sp->cateId == cateId && env.user && userFile && (env.user->userType == 0 || userFile->webuserId == env.user->id))
		{
			Data::DateTime dt;
			dt.SetTicks(userFile->fileTimeTicks);
			dt.ToUTCTime();

			sptr = me->dataDir->ConcatTo(sbuff);
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
			me->dataMut.UnlockRead();

			UInt8 *buff;
			UOSInt buffSize;
			IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
			
			buffSize = (UOSInt)fd.GetDataSize();
			buff = MemAlloc(UInt8, buffSize);
			fd.GetRealData(0, buffSize, buff);
			resp->AddDefHeaders(req);
			resp->AddContentDisposition(false, userFile->oriFileName->v, req->GetBrowser());
			resp->AddContentLength(buffSize);
			if (userFile->fileType == 3)
			{
				resp->AddContentType(CSTR("image/png"));
			}
			else
			{
				resp->AddContentType(CSTR("image/jpeg"));
			}
			resp->Write(buff, buffSize);
			return true;
		}
		else
		{
			me->dataMut.UnlockRead();
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

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcGroup(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	Net::WebServer::WebSessionUsage webSess(me->ParseRequestEnv(req, resp, &env, true));

	Int32 id;
	Int32 cateId;
	if (req->GetQueryValueI32(CSTR("id"), &id) &&
		req->GetQueryValueI32(CSTR("cateId"), &cateId))
	{
		Text::String *s;
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
		SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
		IO::ConfigFile *lang = me->LangGet(req);
		me->dataMut.LockRead();
		group = me->groupMap.Get(id);
		Bool notAdmin = (env.user == 0 || env.user->userType != 0);
		if (group == 0 || group->cateId != cateId)
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->cateMap.Get(group->cateId);
		if (cate == 0 || ((cate->flags & 1) && notAdmin))
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		if (me->GroupIsAdmin(group) && notAdmin)
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST && env.user != 0 && env.user->userType == 0)
		{
			req->ParseHTTPForm();
			Text::String *action = req->GetHTTPFormStr(CSTR("action"));
			Text::String *s;
			Int32 itemId;
			if (action && action->Equals(UTF8STRC("pickall")))
			{
				if (group->groups.GetCount() > 0)
				{
					env.pickObjType = POT_GROUP;
					webSess.GetSess()->SetValueInt32(UTF8STRC("PickObjType"), env.pickObjType);
					env.pickObjs->Clear();
					i = 0;
					j = group->groups.GetCount();
					while (i < j)
					{
						env.pickObjs->SortedInsert(group->groups.GetItem(i)->id);
						i++;
					}
				}
				else if (group->species.GetCount() > 0)
				{
					env.pickObjType = POT_SPECIES;
					webSess.GetSess()->SetValueInt32(UTF8STRC("PickObjType"), env.pickObjType);
					env.pickObjs->Clear();
					i = 0;
					j = group->species.GetCount();
					while (i < j)
					{
						env.pickObjs->SortedInsert(group->species.GetItem(i)->speciesId);
						i++;
					}
				}
			}
			else if (action && action->Equals(UTF8STRC("picksel")))
			{
				if (group->groups.GetCount() > 0)
				{
					env.pickObjType = POT_GROUP;
					webSess.GetSess()->SetValueInt32(UTF8STRC("PickObjType"), env.pickObjType);
					env.pickObjs->Clear();
					i = 0;
					j = group->groups.GetCount();
					while (i < j)
					{
						itemId = group->groups.GetItem(i)->id;
						sb.ClearStr();
						sb.AppendC(UTF8STRC("group"));
						sb.AppendI32(itemId);
						s = req->GetHTTPFormStr(sb.ToCString());
						if (s && s->v[0] == '1')
						{
							env.pickObjs->SortedInsert(itemId);
						}
						i++;
					}
				}
				else if (group->species.GetCount() > 0)
				{
					env.pickObjType = POT_SPECIES;
					webSess.GetSess()->SetValueInt32(UTF8STRC("PickObjType"), env.pickObjType);
					env.pickObjs->Clear();
					i = 0;
					j = group->species.GetCount();
					while (i < j)
					{
						itemId = group->species.GetItem(i)->speciesId;
						sb.ClearStr();
						sb.AppendC(UTF8STRC("species"));
						sb.AppendI32(itemId);
						s = req->GetHTTPFormStr(sb.ToCString());
						if (s && s->v[0] == '1')
						{
							env.pickObjs->SortedInsert(itemId);
						}
						i++;
					}
				}
			}
			else if (action && action->Equals(UTF8STRC("place")))
			{
				if (env.pickObjType == POT_GROUP && group->species.GetCount() == 0)
				{
					i = 0;
					j = env.pickObjs->GetCount();
					while (i < j)
					{
						itemId = env.pickObjs->GetItem(i);
						sb.ClearStr();
						sb.AppendC(UTF8STRC("group"));
						sb.AppendI32(itemId);
						s = req->GetHTTPFormStr(sb.ToCString());
						if (s && s->v[0] == '1')
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
						webSess.GetSess()->SetValueInt32(UTF8STRC("PickObjType"), env.pickObjType);
					}
				}
				else if (env.pickObjType == POT_SPECIES && group->groups.GetCount() == 0)
				{
					i = 0;
					j = env.pickObjs->GetCount();
					while (i < j)
					{
						itemId = env.pickObjs->GetItem(i);
						sb.ClearStr();
						sb.AppendC(UTF8STRC("species"));
						sb.AppendI32(itemId);
						s = req->GetHTTPFormStr(sb.ToCString());
						if (s && s->v[0] == '1')
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
						webSess.GetSess()->SetValueInt32(UTF8STRC("PickObjType"), env.pickObjType);
					}
				}
			}
			else if (action && action->Equals(UTF8STRC("placeall")))
			{
				if (env.pickObjType == POT_GROUP && group->species.GetCount() == 0)
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
						webSess.GetSess()->SetValueInt32(UTF8STRC("PickObjType"), env.pickObjType);
					}
				}
				else if (env.pickObjType == POT_SPECIES && group->groups.GetCount() == 0)
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
						webSess.GetSess()->SetValueInt32(UTF8STRC("PickObjType"), env.pickObjType);
					}
				}
			}
			else if (action && action->Equals(UTF8STRC("setphoto")))
			{
				me->GroupSetPhotoGroup(group->parentId, group->id);
			}
		}

		IO::MemoryStream mstm;
		Text::UTF8Writer writer(&mstm);

		sb.ClearStr();
		sb.Append(cate->chiName);
		sb.AppendC(UTF8STRC(" - "));
		sb.Append(group->chiName);
		sb.AppendC(UTF8STRC(" "));
		sb.Append(group->engName);
		me->WriteHeader(&writer, sb.ToString(), env.user, env.isMobile);
		writer.WriteStrC(UTF8STRC("<center><h1>"));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("</h1></center>"));

		writer.WriteLineC(UTF8STRC("<table border=\"0\" width=\"100%\">"));
		writer.WriteLineC(UTF8STRC("<tr>"));
		writer.WriteStrC(UTF8STRC("<td><form method=\"POST\" action=\"searchinside.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&amp;cateId="));
		sb.AppendI32(group->cateId);
		writer.WriteStrC(sb.ToString(), sb.GetCharCnt());
		writer.WriteStrC(UTF8STRC("\">"));
		writer.WriteStrC(UTF8STRC("Search inside: <input type=\"text\" name=\"searchStr\"/><input type=\"submit\"/>"));
		writer.WriteLineC(UTF8STRC("</form></td>"));
		writer.WriteLineC(UTF8STRC("</tr>"));
		writer.WriteLineC(UTF8STRC("</table>"));

		me->WriteLocator(&writer, group, cate);
		writer.WriteLineC(UTF8STRC("<br/>"));
		if (group->descript)
		{
			s = Text::XML::ToNewHTMLBodyText(group->descript->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
		}
		if (!notAdmin)
		{
			writer.WriteLineC(UTF8STRC("<br/>"));
			if (group->groups.GetCount() == 0)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<a href=\"speciesmod.html?id="));
				sb.AppendI32(id);
				sb.AppendC(UTF8STRC("&amp;cateId="));
				sb.AppendI32(cateId);
				sb.AppendC(UTF8STRC("\">New Species</a>"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());
			}
			if (group->species.GetCount() == 0)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<a href=\"groupmod.html?id="));
				sb.AppendI32(id);
				sb.AppendC(UTF8STRC("&amp;cateId="));
				sb.AppendI32(cateId);
				sb.AppendC(UTF8STRC("\">New Group</a>"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());
			}
			writer.WriteStrC(UTF8STRC("<a href=\"groupmod.html?id="));
			sb.ClearStr();
			sb.AppendI32(group->parentId);
			sb.AppendC(UTF8STRC("&amp;cateId="));
			sb.AppendI32(group->cateId);
			sb.AppendC(UTF8STRC("&amp;groupId="));
			sb.AppendI32(group->id);
			writer.WriteStrC(sb.ToString(), sb.GetCharCnt());
			writer.WriteStrC(UTF8STRC("\">"));
			writer.WriteStrC(UTF8STRC("Modify Group"));
			writer.WriteStrC(UTF8STRC("</a>"));
			writer.WriteLineC(UTF8STRC("<br/>"));
		}
		sb.ClearStr();
		sb.AppendC(UTF8STRC("<a href=\"list.html?id="));
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&amp;cateId="));
		sb.AppendI32(group->cateId);
		sb.AppendC(UTF8STRC("\">"));
		sb.Append(LangGetValue(lang, UTF8STRC("ListAll")));
		sb.AppendC(UTF8STRC("</a><br/>"));
		writer.WriteLineC(sb.ToString(), sb.GetCharCnt());

		sb.ClearStr();
		sb.AppendC(UTF8STRC("<a href=\"listimage.html?id="));
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&amp;cateId="));
		sb.AppendI32(group->cateId);
		sb.AppendC(UTF8STRC("\">"));
		sb.Append(LangGetValue(lang, UTF8STRC("ListImage")));
		sb.AppendC(UTF8STRC("</a><br/>"));
		writer.WriteLineC(sb.ToString(), sb.GetLength());

		if (env.user != 0 || me->GroupIsPublic(group->id))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<a href=\"map/index.html?group="));
			sb.AppendI32(group->id);
			sb.AppendC(UTF8STRC("\">"));
			sb.Append(LangGetValue(lang, UTF8STRC("ShowMap")));
			sb.AppendC(UTF8STRC("</a><br/>"));
			writer.WriteLineC(sb.ToString(), sb.GetLength());
		}
		writer.WriteLineC(UTF8STRC("<hr/>"));

		Bool found = false;
		if (!notAdmin)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<form name=\"groupform\" method=\"POST\" action=\"group.html?id="));
			sb.AppendI32(id);
			sb.AppendC(UTF8STRC("&amp;cateId="));
			sb.AppendI32(cateId);
			sb.AppendC(UTF8STRC("\">"));
			writer.WriteLineC(sb.ToString(), sb.GetLength());
			writer.WriteLineC(UTF8STRC("<input type=\"hidden\" name=\"action\"/>"));
		}
		if (group->groups.GetCount())
		{
			SSWR::OrganMgr::OrganWebHandler::GroupInfo *sgroup;
			Data::StringMap<SSWR::OrganMgr::OrganWebHandler::GroupInfo*> groups;
			i = group->groups.GetCount();
			while (i-- > 0)
			{
				sgroup = group->groups.GetItem(i);
				if ((sgroup->flags & 1) == 0 || !notAdmin)
				{
					groups.Put(sgroup->engName, sgroup);
				}
			}
			if (groups.GetCount() > 0)
			{
				me->WriteGroupTable(&writer, groups.GetValues(), env.scnWidth, !notAdmin);
				writer.WriteLineC(UTF8STRC("<hr/>"));
				found = true;
			}
		}
		if (group->species.GetCount())
		{
			SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
			Data::StringMap<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> species;
			i = group->species.GetCount();
			while (i-- > 0)
			{
				sp = group->species.GetItem(i);
				species.Put(sp->sciName, sp);
			}
			me->WriteSpeciesTable(&writer, species.GetValues(), env.scnWidth, group->cateId, !notAdmin);
			writer.WriteLineC(UTF8STRC("<hr/>"));
			found = true;
		}
		if (!found)
		{
			writer.WriteLineCStr(LangGetValue(lang, UTF8STRC("NoItems")));
			writer.WriteLineC(UTF8STRC("<hr/>"));
		}
		if (!notAdmin)
		{
			writer.WriteLineC(UTF8STRC("</form>"));
		}
		if (env.user != 0 && env.user->userType == 0)
		{
			writer.WriteLineC(UTF8STRC("Pick Objects:<br/>"));
			writer.WriteLineC(UTF8STRC("<input type=\"button\" value=\"Pick Selected\" onclick=\"document.forms.groupform.action.value='picksel';document.forms.groupform.submit();\"/>"));
			writer.WriteLineC(UTF8STRC("<input type=\"button\" value=\"Pick All\" onclick=\"document.forms.groupform.action.value='pickall';document.forms.groupform.submit();\"/>"));
			writer.WriteLineC(UTF8STRC("<hr/>"));
			writer.WriteLineC(UTF8STRC("Picked Objects:<br/>"));
			sb.ClearStr();
			sb.AppendC(UTF8STRC("group.html?id="));
			sb.AppendI32(id);
			sb.AppendC(UTF8STRC("&cateId="));
			sb.AppendI32(cateId);
			me->WritePickObjs(&writer, &env, sb.ToString());
		}

		if (group->parentId == 0)
		{
			writer.WriteStrC(UTF8STRC("<a href="));
			sb.ClearStr();
			sb.AppendC(UTF8STRC("cate.html?cateName="));
			sb.Append(cate->dirName);
			s = Text::XML::ToNewAttrText(sb.ToString());
			writer.WriteStrC(s->v, s->leng);
			s->Release();
			writer.WriteStrC(UTF8STRC(">"));
			writer.WriteStr(LangGetValue(lang, UTF8STRC("Back")));
			writer.WriteStrC(UTF8STRC("</a>"));
		}
		else
		{
			writer.WriteStrC(UTF8STRC("<a href=\"group.html?id="));
			sb.ClearStr();
			sb.AppendI32(group->parentId);
			sb.AppendC(UTF8STRC("&amp;cateId="));
			sb.AppendI32(group->cateId);
			writer.WriteStrC(sb.ToString(), sb.GetCharCnt());
			writer.WriteStrC(UTF8STRC("\">"));
			writer.WriteStr(LangGetValue(lang, UTF8STRC("Back")));
			writer.WriteStrC(UTF8STRC("</a>"));
		}
		if (env.user != 0 && env.user->userType == 0)
		{
			writer.WriteLineC(UTF8STRC("<br/>"));

			writer.WriteLineC(UTF8STRC("<input type=\"button\" value=\"Set Parent Photo\" onclick=\"document.forms.groupform.action.value='setphoto';document.forms.groupform.submit();\"/>"));
		}


		me->WriteFooter(&writer);
		me->dataMut.UnlockRead();
		ResponseMstm(req, resp, &mstm, CSTR("text/html"));
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}


Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcGroupMod(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
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
	if (req->GetQueryValueI32(CSTR("id"), &id) &&
		req->GetQueryValueI32(CSTR("cateId"), &cateId))
	{
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
		SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
		Text::StringBuilderUTF8 sb;
		Text::String *s;
		Text::String *txt;
		IO::ConfigFile *lang = me->LangGet(req);

		me->dataMut.LockRead();
		group = me->groupMap.Get(id);
		if (group == 0)
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->cateMap.Get(cateId);
		if (cate == 0)
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		Text::StringBuilderUTF8 msg;
		Text::String *ename = 0;
		Text::String *cname = 0;
		Text::String *descr = 0;
		GroupFlags groupFlags = GF_NONE;
		Int32 groupTypeId = 0;
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *modGroup = 0;
		if (req->GetQueryValueI32(CSTR("groupId"), &groupId))
		{
			modGroup = me->groupMap.Get(groupId);
			if (modGroup)
			{
				cname = modGroup->chiName;
				ename = modGroup->engName;
				descr = modGroup->descript;
				groupTypeId = modGroup->groupType;
			}
		}
		if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
		{
			req->ParseHTTPForm();
			Text::String *task = req->GetHTTPFormStr(CSTR("task"));
			cname = req->GetHTTPFormStr(CSTR("cname"));
			ename = req->GetHTTPFormStr(CSTR("ename"));
			descr = req->GetHTTPFormStr(CSTR("descr"));
			txt = req->GetHTTPFormStr(CSTR("adminOnly"));
			if (txt && txt->v[0] == '1')
			{
				groupFlags = (GroupFlags)(groupFlags | GF_ADMIN_ONLY);
			}
			if (task != 0 && cname != 0 && req->GetHTTPFormInt32(CSTR("groupType"), &groupTypeId) && ename != 0 && descr != 0 && ename->v[0] != 0 && cname->v[0] != 0)
			{
				if (task->Equals(UTF8STRC("new")))
				{
					sb.ClearStr();
					Bool found = false;
					i = group->groups.GetCount();
					while (i-- > 0)
					{
						if (group->groups.GetItem(i)->engName->Equals(ename))
						{
							found = true;
							break;
						}
					}
					if (found)
					{
						msg.AppendC(UTF8STRC("Group already exist"));
					}
					else
					{
						me->dataMut.UnlockRead();
						me->dataMut.LockWrite();
						Int32 newGroupId = me->GroupAdd(ename->ToCString(), cname->ToCString(), id, descr->ToCString(), groupTypeId, cateId, groupFlags);
						if (newGroupId)
						{
							me->dataMut.UnlockWrite();
							sb.ClearStr();
							sb.AppendC(UTF8STRC("group.html?id="));
							sb.AppendI32(newGroupId);
							sb.AppendC(UTF8STRC("&cateId="));
							sb.AppendI32(cateId);

							resp->RedirectURL(req, sb.ToCString(), 0);
							return true;
						}
						else
						{
							msg.AppendC(UTF8STRC("Error in adding group"));
						}
						me->dataMut.UnlockWrite();
						me->dataMut.LockRead();
					}
				}
				else if (task->Equals(UTF8STRC("modify")) && modGroup != 0 && modGroup->cateId == cateId)
				{
					Bool found = false;
					i = group->groups.GetCount();
					while (i-- > 0)
					{
						if (group->groups.GetItem(i) != modGroup && group->groups.GetItem(i)->engName->Equals(ename))
						{
							found = true;
							break;
						}
					}
					if (found)
					{
						msg.AppendC(UTF8STRC("Group name already exist"));
					}
					else
					{
						me->dataMut.UnlockRead();
						me->dataMut.LockWrite();
						if (me->GroupModify(modGroup->id, STR_CSTR(ename), STR_CSTR(cname), STR_CSTR(descr), groupTypeId, groupFlags))
						{
							me->dataMut.UnlockWrite();
							sb.ClearStr();
							sb.AppendC(UTF8STRC("group.html?id="));
							sb.AppendI32(modGroup->id);
							sb.AppendC(UTF8STRC("&cateId="));
							sb.AppendI32(modGroup->cateId);

							resp->RedirectURL(req, sb.ToCString(), 0);
							return true;
						}
						else
						{
							msg.AppendC(UTF8STRC("Error in modifying group"));
						}
						me->dataMut.UnlockWrite();
						me->dataMut.LockRead();
					}
				}
				else if (task->Equals(UTF8STRC("delete")) && modGroup != 0 && modGroup->groups.GetCount() == 0 && modGroup->species.GetCount() == 0)
				{
					Int32 id = modGroup->id;
					Int32 cateId = modGroup->cateId;
					me->dataMut.UnlockRead();
					me->dataMut.LockWrite();
					if (me->GroupDelete(modGroup->id))
					{
						me->dataMut.UnlockWrite();
						sb.ClearStr();
						sb.AppendC(UTF8STRC("group.html?id="));
						sb.AppendI32(id);
						sb.AppendC(UTF8STRC("&cateId="));
						sb.AppendI32(cateId);

						resp->RedirectURL(req, sb.ToCString(), 0);
						return true;
					}
					else
					{
						msg.AppendC(UTF8STRC("Error in deleting group"));
					}
					me->dataMut.UnlockWrite();
					me->dataMut.LockRead();
				}
			}
		}

		IO::MemoryStream mstm;
		Text::UTF8Writer writer(&mstm);

		sb.ClearStr();
		sb.AppendC(UTF8STRC("New group in "));
		sb.Append(group->chiName);
		sb.AppendC(UTF8STRC(" "));
		sb.Append(group->engName);
		me->WriteHeader(&writer, sb.ToString(), env.user, env.isMobile);
		writer.WriteStrC(UTF8STRC("<center><h1>"));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("</h1></center>"));

		writer.WriteStrC(UTF8STRC("<form name=\"newgroup\" method=\"POST\" action=\"groupmod.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&amp;cateId="));
		sb.AppendI32(group->cateId);
		if (groupId != 0)
		{
			sb.AppendC(UTF8STRC("&amp;groupId="));
			sb.AppendI32(groupId);
		}
		writer.WriteStrC(sb.ToString(), sb.GetLength());
		writer.WriteLineC(UTF8STRC("\">"));
		writer.WriteLineC(UTF8STRC("<input type=\"hidden\" name=\"task\"/>"));
		writer.WriteLineC(UTF8STRC("<table border=\"0\">"));
		writer.WriteLineC(UTF8STRC("<tr><td>Category</td><td><select name=\"groupType\">"));
		SSWR::OrganMgr::OrganWebHandler::GroupTypeInfo *groupType;
		i = 0;
		j = cate->groupTypes.GetCount();
		while (i < j)
		{
			groupType = cate->groupTypes.GetItem(i);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<option value=\""));
			sb.AppendI32(groupType->id);
			sb.AppendC(UTF8STRC("\""));
			if (groupType->id == groupTypeId)
			{
				sb.AppendC(UTF8STRC(" selected"));
			}
			sb.AppendC(UTF8STRC(">"));
			s = Text::XML::ToNewHTMLElementText(groupType->chiName->v);
			sb.Append(s);
			s->Release();
			sb.AppendC(UTF8STRC("</option>"));
			writer.WriteLineC(sb.ToString(), sb.GetLength());
			i++;
		}
		writer.WriteLineC(UTF8STRC("</select></td></tr>"));
		writer.WriteStrC(UTF8STRC("<tr><td>English Name</td><td><input type=\"text\" name=\"ename\""));
		if (ename)
		{
			writer.WriteStrC(UTF8STRC(" value="));
			s = Text::XML::ToNewAttrText(ename->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
		}
		writer.WriteLineC(UTF8STRC("/></td></tr>"));
		writer.WriteStrC(UTF8STRC("<tr><td>Chinese Name</td><td><input type=\"text\" name=\"cname\""));
		if (cname)
		{
			writer.WriteStrC(UTF8STRC(" value="));
			s = Text::XML::ToNewAttrText(cname->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
		}
		writer.WriteLineC(UTF8STRC("/></td></tr>"));
		writer.WriteStrC(UTF8STRC("<tr><td>Flags</td><td><input type=\"checkbox\" id=\"adminOnly\" name=\"adminOnly\" value=\"1\""));
		if (groupFlags & GF_ADMIN_ONLY)
		{
			writer.WriteStrC(UTF8STRC(" checked"));
		}
		writer.WriteLineC(UTF8STRC("/><label for=\"adminOnly\">Admin Only</label></td></tr>"));
		writer.WriteStrC(UTF8STRC("<tr><td>Description</td><td><textarea name=\"descr\" rows=\"4\" cols=\"40\">"));
		if (descr)
		{
			s = Text::XML::ToNewHTMLElementText(descr->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
		}
		writer.WriteLineC(UTF8STRC("</textarea></td></tr>"));
		writer.WriteStrC(UTF8STRC("<tr><td></td><td>"));
		if (groupId != 0)
		{
			writer.WriteStrC(UTF8STRC("<input type=\"button\" value=\"Modify\" onclick=\"document.forms.newgroup.task.value='modify';document.forms.newgroup.submit();\"/>"));
		}
		writer.WriteStrC(UTF8STRC("<input type=\"button\" value=\"New\" onclick=\"document.forms.newgroup.task.value='new';document.forms.newgroup.submit();\"/>"));
		if (group->species.GetCount() == 0 && group->groups.GetCount() == 0)
		{
			writer.WriteStrC(UTF8STRC("<input type=\"button\" value=\"Delete\" onclick=\"document.forms.newgroup.task.value='delete';document.forms.newgroup.submit();\"/>"));
		}
		writer.WriteStrC(UTF8STRC("<a href=\"group.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&amp;cateId="));
		sb.AppendI32(group->cateId);
		writer.WriteStrC(sb.ToString(), sb.GetLength());
		writer.WriteStrC(UTF8STRC("\">"));
		writer.WriteStr(LangGetValue(lang, UTF8STRC("Back")));
		writer.WriteStrC(UTF8STRC("</a>"));
		writer.WriteLineC(UTF8STRC("</td></tr>"));
		writer.WriteLineC(UTF8STRC("</table></form>"));
		me->WriteFooter(&writer);
		me->dataMut.UnlockRead();
		ResponseMstm(req, resp, &mstm, CSTR("text/html"));
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcSpecies(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	Net::WebServer::WebSessionUsage webSess(me->ParseRequestEnv(req, resp, &env, true));

	Int32 id;
	Int32 cateId;
	if (req->GetQueryValueI32(CSTR("id"), &id) &&
		req->GetQueryValueI32(CSTR("cateId"), &cateId))
	{
		Text::String *s;
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
		SSWR::OrganMgr::OrganWebHandler::WebFileInfo *wfile;
		IO::ConfigFile *lang = me->LangGet(req);
		Data::DateTime dt;

		me->dataMut.LockRead();
		species = me->spMap.Get(id);
		if (species == 0)
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		Bool notAdmin = (env.user == 0 || env.user->userType != 0);
		group = me->groupMap.Get(species->groupId);
		if (group == 0 || group->cateId != cateId || (me->GroupIsAdmin(group) && notAdmin))
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->cateMap.Get(group->cateId);
		if (cate == 0 || ((cate->flags & 1) && notAdmin))
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST && env.user != 0 && env.user->userType == 0)
		{
			req->ParseHTTPForm();
			Text::String *action = req->GetHTTPFormStr(CSTR("action"));
			Text::String *s;
			Int32 userfileId;
			if (action && action->Equals(UTF8STRC("pickall")))
			{
				env.pickObjType = POT_USERFILE;
				webSess.GetSess()->SetValueInt32(UTF8STRC("PickObjType"), env.pickObjType);
				env.pickObjs->Clear();
				i = 0;
				j = species->files.GetCount();
				while (i < j)
				{
					env.pickObjs->SortedInsert(species->files.GetItem(i)->id);
					i++;
				}
			}
			else if (action && action->Equals(UTF8STRC("picksel")))
			{
				env.pickObjType = POT_USERFILE;
				webSess.GetSess()->SetValueInt32(UTF8STRC("PickObjType"), env.pickObjType);
				env.pickObjs->Clear();
				i = 0;
				j = species->files.GetCount();
				while (i < j)
				{
					userfileId = species->files.GetItem(i)->id;
					sb.ClearStr();
					sb.AppendC(UTF8STRC("userfile"));
					sb.AppendI32(userfileId);
					s = req->GetHTTPFormStr(sb.ToCString());
					if (s && s->v[0] == '1')
					{
						env.pickObjs->SortedInsert(userfileId);
					}
					i++;
				}
			}
			else if (action && action->Equals(UTF8STRC("place")))
			{
				if (env.pickObjType == POT_USERFILE)
				{
					i = 0;
					j = env.pickObjs->GetCount();
					while (i < j)
					{
						userfileId = env.pickObjs->GetItem(i);
						sb.ClearStr();
						sb.AppendC(UTF8STRC("userfile"));
						sb.AppendI32(userfileId);
						s = req->GetHTTPFormStr(sb.ToCString());
						if (s && s->v[0] == '1')
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
						webSess.GetSess()->SetValueInt32(UTF8STRC("PickObjType"), env.pickObjType);
					}
				}
			}
			else if (action && action->Equals(UTF8STRC("placeall")))
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
						webSess.GetSess()->SetValueInt32(UTF8STRC("PickObjType"), env.pickObjType);
					}
				}
			}
		}

		IO::MemoryStream mstm;
		Text::UTF8Writer writer(&mstm);

		sb.ClearStr();
		sb.Append(cate->chiName);
		sb.AppendC(UTF8STRC(" - "));
		sb.AppendC(species->sciName->v, species->sciName->leng);
		sb.AppendC(UTF8STRC(" "));
		sb.AppendC(species->chiName->v, species->chiName->leng);
		sb.AppendC(UTF8STRC(" "));
		sb.AppendC(species->engName->v, species->engName->leng);
		me->WriteHeader(&writer, sb.ToString(), env.user, env.isMobile);
		writer.WriteStrC(UTF8STRC("<center><h1>"));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("</h1></center>"));

		writer.WriteLineC(UTF8STRC("<table border=\"0\"><tr><td>"));
		if (species->descript)
		{
			s = Text::XML::ToNewHTMLBodyText(species->descript->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
		}
		if (species->files.GetCount() > 0)
		{
			Bool months[12];
			i = 12;
			while (i-- > 0)
			{
				months[i] = false;
			}
			i = species->files.GetCount();
			while (i-- > 0)
			{
				userFile = species->files.GetItem(i);
				if (userFile->captureTimeTicks)
				{
					dt.SetTicks(userFile->captureTimeTicks);
					months[dt.GetMonth() - 1] = true;
				}
			}
			writer.WriteLineC(UTF8STRC("<br/><br/>Months"));
			writer.WriteLineC(UTF8STRC("<table><tr>"));
			i = 0;
			while (i < 12)
			{
				if (months[i])
				{
					writer.WriteStrC(UTF8STRC("<td bgcolor=\"#4060ff\">"));
				}
				else
				{
					writer.WriteStrC(UTF8STRC("<td>"));
				}
				sptr = Text::StrUOSInt(sbuff, i + 1);
				writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
				writer.WriteStrC(UTF8STRC("</td>"));
				i++;
			}
			writer.WriteLineC(UTF8STRC("</tr></table>"));
		}
		if (species->books.GetCount() > 0)
		{
			Data::DateTime dt;
			writer.WriteLineC(UTF8STRC("<br/><br/>"));
			writer.WriteStr(LangGetValue(lang, UTF8STRC("Document")));
			writer.WriteLineC(UTF8STRC(":<br/>"));
			writer.WriteLineC(UTF8STRC("<table border=\"0\">"));
			i = 0;
			j = species->books.GetCount();
			while (i < j)
			{
				bookSp = species->books.GetItem(i);
				book = me->bookMap.Get(bookSp->bookId);
				if (book != 0)
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("<tr><td><a href=\"book.html?id="));
					sb.AppendI32(book->id);
					sb.AppendC(UTF8STRC("&cateId="));
					sb.AppendI32(cate->cateId);
					sb.AppendC(UTF8STRC("\" title="));
					s = Text::XML::ToNewAttrText(book->title->v);
					sb.Append(s);
					s->Release();
					sb.AppendC(UTF8STRC("><i>"));
					writer.WriteStrC(sb.ToString(), sb.GetLength());
					s = Text::XML::ToNewHTMLBodyText(bookSp->dispName->v);
					writer.WriteStrC(s->v, s->leng);
					s->Release();
					writer.WriteStrC(UTF8STRC("</i></a></td><td>"));
					sb.ClearStr();
					sb.Append(book->author);
					sb.AppendC(UTF8STRC(" ("));
					dt.SetTicks(book->publishDate);
					sb.AppendU32(dt.GetYear());
					sb.AppendC(UTF8STRC(")"));
					s = Text::XML::ToNewHTMLBodyText(sb.ToString());
					writer.WriteStrC(s->v, s->leng);
					s->Release();
					writer.WriteLineC(UTF8STRC("</td></tr>"));
				}
				i++;
			}
			writer.WriteLineC(UTF8STRC("</table>"));
		}
		writer.WriteLineC(UTF8STRC("</td><td>"));
		me->WriteLocator(&writer, group, cate);
		writer.WriteLineC(UTF8STRC("</td></tr></table>"));
		if (env.user != 0 || me->GroupIsPublic(group->id))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<a href=\"map/index.html?species="));
			sb.AppendI32(id);
			sb.AppendC(UTF8STRC("\">"));
			sb.Append(LangGetValue(lang, UTF8STRC("ShowMap")));
			sb.AppendC(UTF8STRC("</a><br/>"));
			writer.WriteLineC(sb.ToString(), sb.GetLength());
		}
		writer.WriteLineC(UTF8STRC("<hr/>"));

		Data::ArrayListICaseString fileNameList;
		Data::ArrayListString refURLList;
		sptr = cate->srcDir->ConcatTo(sbuff);
		if (IO::Path::PATH_SEPERATOR != '\\')
		{
			Text::StrReplace(sbuff, '\\', IO::Path::PATH_SEPERATOR);
		}
		sptr = species->dirName->ConcatTo(sptr);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
		sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
		if (sess)
		{
			while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
			{
				if (pt == IO::Path::PathType::File)
				{
					if (Text::StrEndsWithICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC(".JPG")) || Text::StrEndsWithICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC(".PCX")) || Text::StrEndsWithICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC(".TIF")))
					{
						sptr2[-4] = 0;
						fileNameList.SortedInsert(Text::String::New(sptr, (UOSInt)(sptr2 - sptr - 4)));
					}
				}
			}
			IO::Path::FindFileClose(sess);
		}
		sptr2 = Text::StrConcatC(sptr, UTF8STRC("web.txt"));
		if (IO::Path::GetPathType(CSTRP(sbuff, sptr2)) == IO::Path::PathType::File)
		{
			Text::PString sarr[4];
			IO::FileStream fs(CSTRP(sbuff, sptr2), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
			Text::UTF8Reader reader(&fs);
			sb.ClearStr();
			while (reader.ReadLine(&sb, 4096))
			{
				if (Text::StrSplitP(sarr, 4, sb, '\t') == 3)
				{
					if (refURLList.SortedIndexOfPtr(sarr[2].v, sarr[2].leng) < 0)
					{
						refURLList.SortedInsert(Text::String::New(sarr[2].v, sarr[2].leng));
					}
					sptr2 = Text::StrConcatC(sptr, UTF8STRC("web"));
					*sptr2++ = IO::Path::PATH_SEPERATOR;
					sptr2 = Text::StrConcatC(sptr2, sarr[0].v, sarr[0].leng);
					i = Text::StrLastIndexOfCharC(sptr, (UOSInt)(sptr2 - sptr), '.');
					if (i != INVALID_INDEX)
					{
						sptr[i] = 0;
						sptr2 = &sptr[i];
					}
					fileNameList.Add(Text::String::New(sptr, (UOSInt)(sptr2 - sptr)));
				}
				sb.ClearStr();
			}
		}

		UInt32 colCount = env.scnWidth / PREVIEW_SIZE;
		UInt32 colWidth = 100 / colCount;
		UInt32 currColumn;
		if (fileNameList.GetCount() > 0 || species->files.GetCount() > 0 || species->wfiles.GetCount() > 0)
		{
			currColumn = 0;
			if (env.user != 0 && env.user->userType == 0)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<form name=\"userfiles\" action=\"species.html?id="));
				sb.AppendI32(id);
				sb.AppendC(UTF8STRC("&amp;cateId="));
				sb.AppendI32(cateId);
				sb.AppendC(UTF8STRC("\" method=\"POST\"/>"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());
				writer.WriteLineC(UTF8STRC("<input type=\"hidden\" name=\"action\"/>"));
			}
			writer.WriteLineC(UTF8STRC("<table border=\"0\" width=\"100%\">"));

			Data::ArrayList<UserFileInfo*> fileList;
			fileList.AddAll(&species->files);
			if (env.user != 0)
			{
				UserFileDescComparator comparator(&env);
				Data::Sort::ArtificialQuickSort::Sort(&fileList, &comparator);
			}
			i = 0;
			j = fileList.GetCount();
			while (i < j)
			{
				userFile = fileList.GetItem(i);
				if (currColumn == 0)
				{
					writer.WriteLineC(UTF8STRC("<tr>"));
				}
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<td width=\""));
				sb.AppendU32(colWidth);
				sb.AppendC(UTF8STRC("%\">"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());
				sb.ClearStr();
				sb.AppendC(UTF8STRC("photodetail.html?id="));
				sb.AppendI32(species->speciesId);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(species->cateId);
				sb.AppendC(UTF8STRC("&fileId="));
				sb.AppendI32(userFile->id);
				s = Text::XML::ToNewAttrText(sb.ToString());
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<center><a href="));
				sb.Append(s);
				sb.AppendC(UTF8STRC(">"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());
				s->Release();

				writer.WriteStrC(UTF8STRC("<img src="));
				sb.ClearStr();
				sb.AppendC(UTF8STRC("photo.html?id="));
				sb.AppendI32(species->speciesId);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(species->cateId);
				sb.AppendC(UTF8STRC("&width="));
				sb.AppendI32(PREVIEW_SIZE);
				sb.AppendC(UTF8STRC("&height="));
				sb.AppendI32(PREVIEW_SIZE);
				sb.AppendC(UTF8STRC("&fileId="));
				sb.AppendI32(userFile->id);
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer.WriteStrC(s->v, s->leng);
				s->Release();
				writer.WriteStrC(UTF8STRC(" border=\"0\">"));
				writer.WriteStrC(UTF8STRC("</a><br/>"));
				dt.SetTicks(userFile->fileTimeTicks);
				dt.ToLocalTime();
				if (env.user != 0 && (env.user->userType == 0 || userFile->webuserId == env.user->id))
				{
					if (env.user->userType == 0)
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("<input type=\"checkbox\" id=\"userfile"));
						sb.AppendI32(userFile->id);
						sb.AppendC(UTF8STRC("\" name=\"userfile"));
						sb.AppendI32(userFile->id);
						sb.AppendC(UTF8STRC("\" value=\"1\"/><label for=\"userfile"));
						sb.AppendI32(userFile->id);
						sb.AppendC(UTF8STRC("\">"));
						writer.WriteStrC(sb.ToString(), sb.GetLength());
					}
					sptr2 = dt.ToString(sbuff2, "yyyy-MM-dd HH:mm:ss zzzz");
					writer.WriteStrC(sbuff2, (UOSInt)(sptr2 - sbuff2));
					if (userFile->webuserId == env.user->id)
					{
						if (userFile->location)
						{
							writer.WriteStrC(UTF8STRC(" "));
							s = Text::XML::ToNewHTMLBodyText(userFile->location->v);
							writer.WriteStrC(s->v, s->leng);
							s->Release();
						}
					}
					if (userFile->descript && userFile->descript->leng > 0)
					{
						writer.WriteStrC(UTF8STRC("<br/>"));
						s = Text::XML::ToNewHTMLBodyText(userFile->descript->v);
						writer.WriteStrC(s->v, s->leng);
						s->Release();
					}
					if (userFile->webuserId == env.user->id)
					{
						writer.WriteStrC(UTF8STRC("<br/>"));
						s = Text::XML::ToNewHTMLBodyText(userFile->oriFileName->v);
						writer.WriteStrC(s->v, s->leng);
						s->Release();
					}
					if (env.user->userType == 0)
					{
						writer.WriteStrC(UTF8STRC("</label>"));
					}
					if (userFile->lat != 0 || userFile->lon != 0)
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("<br/>"));
						sb.AppendC(UTF8STRC("<a href=\"https://www.google.com/maps/place/"));
						Text::SBAppendF64(&sb, userFile->lat);
						sb.AppendC(UTF8STRC(","));
						Text::SBAppendF64(&sb, userFile->lon);
						sb.AppendC(UTF8STRC("/@"));
						Text::SBAppendF64(&sb, userFile->lat);
						sb.AppendC(UTF8STRC(","));
						Text::SBAppendF64(&sb, userFile->lon);
						sb.AppendC(UTF8STRC(",19z\">"));
						sb.AppendC(UTF8STRC("Google Map</a>"));
						writer.WriteStrC(sb.ToString(), sb.GetLength());
					}
				}
				else
				{
					sptr2 = dt.ToString(sbuff2, "yyyy-MM-dd zzzz");
					writer.WriteStrC(sbuff2, (UOSInt)(sptr2 - sbuff2));
				}
				writer.WriteLineC(UTF8STRC("</center></td>"));

				currColumn++;
				if (currColumn >= colCount)
				{
					writer.WriteLineC(UTF8STRC("</tr>"));
					currColumn = 0;
				}
				i++;
			}

			i = 0;
			j = species->wfiles.GetCount();
			while (i < j)
			{
				wfile = species->wfiles.GetItem(i);
				if (currColumn == 0)
				{
					writer.WriteLineC(UTF8STRC("<tr>"));
				}
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<td width=\""));
				sb.AppendU32(colWidth);
				sb.AppendC(UTF8STRC("%\">"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());
				sb.ClearStr();
				sb.AppendC(UTF8STRC("photodetail.html?id="));
				sb.AppendI32(species->speciesId);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(species->cateId);
				sb.AppendC(UTF8STRC("&fileWId="));
				sb.AppendI32(wfile->id);
				s = Text::XML::ToNewAttrText(sb.ToString());
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<center><a href="));
				sb.Append(s);
				sb.AppendC(UTF8STRC(">"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());
				s->Release();

				writer.WriteStrC(UTF8STRC("<img src="));
				sb.ClearStr();
				sb.AppendC(UTF8STRC("photo.html?id="));
				sb.AppendI32(species->speciesId);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(species->cateId);
				sb.AppendC(UTF8STRC("&width="));
				sb.AppendI32(PREVIEW_SIZE);
				sb.AppendC(UTF8STRC("&height="));
				sb.AppendI32(PREVIEW_SIZE);
				sb.AppendC(UTF8STRC("&fileWId="));
				sb.AppendI32(wfile->id);
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer.WriteStrC(s->v, s->leng);
				s->Release();
				writer.WriteStrC(UTF8STRC(" border=\"0\">"));
				writer.WriteStrC(UTF8STRC("</a>"));
				if (env.user && env.user->userType == 0)
				{
					if (wfile->location && wfile->location->leng > 0)
					{
						writer.WriteStrC(UTF8STRC("<br/>"));
						s = Text::XML::ToNewHTMLBodyText(wfile->location->v);
						writer.WriteStrC(s->v, s->leng);
						s->Release();
					}
				}
				writer.WriteLineC(UTF8STRC("</center></td>"));

				currColumn++;
				if (currColumn >= colCount)
				{
					writer.WriteLineC(UTF8STRC("</tr>"));
					currColumn = 0;
				}

				if (refURLList.SortedIndexOf(wfile->srcUrl) < 0)
				{
					refURLList.SortedInsert(wfile->srcUrl->Clone());
				}

				i++;
			}

			i = 0;
			j = fileNameList.GetCount();
			while (i < j)
			{
				sptr2 = Text::TextBinEnc::URIEncoding::URIEncode(sbuff2, fileNameList.GetItem(i)->v);
				if (currColumn == 0)
				{
					writer.WriteLineC(UTF8STRC("<tr>"));
				}
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<td width=\""));
				sb.AppendU32(colWidth);
				sb.AppendC(UTF8STRC("%\">"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());
				sb.ClearStr();
				sb.AppendC(UTF8STRC("photodetail.html?id="));
				sb.AppendI32(species->speciesId);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(species->cateId);
				sb.AppendC(UTF8STRC("&file="));
				sb.AppendC(sbuff2, (UOSInt)(sptr2 - sbuff2));
				s = Text::XML::ToNewAttrText(sb.ToString());
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<center><a href="));
				sb.Append(s);
				sb.AppendC(UTF8STRC(">"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());
				s->Release();

				writer.WriteStrC(UTF8STRC("<img src="));
				sb.ClearStr();
				sb.AppendC(UTF8STRC("photo.html?id="));
				sb.AppendI32(species->speciesId);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(species->cateId);
				sb.AppendC(UTF8STRC("&width="));
				sb.AppendI32(PREVIEW_SIZE);
				sb.AppendC(UTF8STRC("&height="));
				sb.AppendI32(PREVIEW_SIZE);
				sb.AppendC(UTF8STRC("&file="));
				sb.AppendC(sbuff2, (UOSInt)(sptr2 - sbuff2));
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer.WriteStrC(s->v, s->leng);
				s->Release();
				writer.WriteStrC(UTF8STRC(" border=\"0\">"));
				writer.WriteLineC(UTF8STRC("</a></center></td>"));

				currColumn++;
				if (currColumn >= colCount)
				{
					writer.WriteLineC(UTF8STRC("</tr>"));
					currColumn = 0;
				}

				fileNameList.GetItem(i)->Release();
				i++;
			}

			if (currColumn != 0)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<td width=\""));
				sb.AppendU32(colWidth);
				sb.AppendC(UTF8STRC("%\"></td>"));
				while (currColumn < colCount)
				{
					writer.WriteLineC(sb.ToString(), sb.GetLength());
					currColumn++;
				}
				writer.WriteLineC(UTF8STRC("</tr>"));
			}
			writer.WriteLineC(UTF8STRC("</table><hr/>"));
			if (env.user != 0 && env.user->userType == 0)
			{
				writer.WriteLineC(UTF8STRC("</form>"));
			}
		}
		if (refURLList.GetCount() > 0)
		{
			writer.WriteLineC(UTF8STRC("Reference URL:<br/>"));
			Text::String *url;
			i = 0;
			j = refURLList.GetCount();
			while (i < j)
			{
				url = refURLList.GetItem(i);

				writer.WriteStrC(UTF8STRC("<a href="));
				s = Text::XML::ToNewAttrText(url->v);
				writer.WriteStrC(s->v, s->leng);
				s->Release();
				writer.WriteStrC(UTF8STRC(">"));
				s = Text::XML::ToNewHTMLBodyText(url->v);
				writer.WriteStrC(s->v, s->leng);
				s->Release();
				writer.WriteLineC(UTF8STRC("</a><br/>"));

				url->Release();
				i++;
			}
			writer.WriteLineC(UTF8STRC("<hr/>"));
		}
		if (env.user != 0 && env.user->userType == 0)
		{
			writer.WriteLineC(UTF8STRC("Pick Objects:<br/>"));
			writer.WriteLineC(UTF8STRC("<input type=\"button\" value=\"Pick Selected\" onclick=\"document.forms.userfiles.action.value='picksel';document.forms.userfiles.submit();\"/>"));
			writer.WriteLineC(UTF8STRC("<input type=\"button\" value=\"Pick All\" onclick=\"document.forms.userfiles.action.value='pickall';document.forms.userfiles.submit();\"/>"));
			writer.WriteLineC(UTF8STRC("<hr/>"));
			writer.WriteLineC(UTF8STRC("Picked Objects:<br/>"));
			sb.ClearStr();
			sb.AppendC(UTF8STRC("species.html?id="));
			sb.AppendI32(id);
			sb.AppendC(UTF8STRC("&cateId="));
			sb.AppendI32(cateId);
			me->WritePickObjs(&writer, &env, sb.ToString());
		}

		writer.WriteLineC(UTF8STRC("<br/>"));
		writer.WriteStrC(UTF8STRC("<a href=\"group.html?id="));
		sb.ClearStr();
		sb.AppendI32(species->groupId);
		sb.AppendC(UTF8STRC("&amp;cateId="));
		sb.AppendI32(group->cateId);
		writer.WriteStrC(sb.ToString(), sb.GetLength());
		writer.WriteStrC(UTF8STRC("\">"));
		writer.WriteStr(LangGetValue(lang, UTF8STRC("Back")));
		writer.WriteStrC(UTF8STRC("</a>"));

		me->WriteFooter(&writer);
		me->dataMut.UnlockRead();
		ResponseMstm(req, resp, &mstm, CSTR("text/html"));
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcSpeciesMod(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	if (env.user == 0 || env.user->userType != 0)
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}

	Int32 id;
	Int32 cateId;
	Int32 spId = 0;
	if (req->GetQueryValueI32(CSTR("id"), &id) &&
		req->GetQueryValueI32(CSTR("cateId"), &cateId))
	{
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
		Text::StringBuilderUTF8 sb;
		Text::String *s;
		IO::ConfigFile *lang = me->LangGet(req);

		me->dataMut.LockRead();
		group = me->groupMap.Get(id);
		if (group == 0)
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		Text::StringBuilderUTF8 msg;
		Text::String *cname = 0;
		Text::String *sname = 0;
		Text::String *ename = 0;
		Text::String *descr = 0;
		const UTF8Char *bookIgn = 0;
		SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species = 0;
		if (req->GetQueryValueI32(CSTR("spId"), &spId))
		{
			species = me->spMap.Get(spId);
			if (species)
			{
				cname = species->chiName;
				sname = species->sciName;
				ename = species->engName;
				descr = species->descript;
			}
		}
		if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
		{
			req->ParseHTTPForm();
			Text::String *task = req->GetHTTPFormStr(CSTR("task"));
			cname = req->GetHTTPFormStr(CSTR("cname"));
			sname = req->GetHTTPFormStr(CSTR("sname"));
			ename = req->GetHTTPFormStr(CSTR("ename"));
			descr = req->GetHTTPFormStr(CSTR("descr"));
			bookIgn = STR_PTR(req->GetHTTPFormStr(CSTR("bookIgn")));
			if (task != 0 && cname != 0 && sname != 0 && ename != 0 && descr != 0 && cname->v[0] != 0 && sname->v[0] != 0)
			{
				if (task->Equals(UTF8STRC("new")))
				{
					sb.ClearStr();
					if (me->spNameMap.Get(sname) != 0)
					{
						msg.AppendC(UTF8STRC("Species already exist"));
					}
					else if ((bookIgn == 0 || bookIgn[0] != '1') && me->SpeciesBookIsExist(sname->ToCString(), &sb))
					{
						msg.AppendC(UTF8STRC("Species already exist in book: "));
						msg.AppendC(sb.ToString(), sb.GetLength());
						msg.AppendC(UTF8STRC(", continue?"));
						bookIgn = (const UTF8Char*)"1";
					}
					else
					{
						me->dataMut.UnlockRead();
						me->dataMut.LockWrite();
						sb.ClearStr();
						sb.Append(sname);
						sb.ToLower();
						sb.ReplaceStr(UTF8STRC(" "), UTF8STRC("_"));
						sb.ReplaceStr(UTF8STRC("."), UTF8STRC(""));
						Int32 spId = me->SpeciesAdd(STR_CSTR(ename), STR_CSTR(cname), STR_CSTR(sname), id, STR_CSTR(descr), sb.ToCString(), CSTR(""), cateId);
						if (spId)
						{
							me->dataMut.UnlockWrite();
							sb.ClearStr();
							sb.AppendC(UTF8STRC("species.html?id="));
							sb.AppendI32(spId);
							sb.AppendC(UTF8STRC("&cateId="));
							sb.AppendI32(cateId);

							resp->RedirectURL(req, sb.ToCString(), 0);
							return true;
						}
						else
						{
							msg.AppendC(UTF8STRC("Error in adding species"));
						}
						me->dataMut.UnlockWrite();
						me->dataMut.LockRead();
					}
				}
				else if (task->Equals(UTF8STRC("modify")) && species != 0)
				{
					Bool nameChg = !species->sciName->Equals(sname);
					sb.ClearStr();
					if (nameChg && me->spNameMap.Get(sname) != 0)
					{
						msg.AppendC(UTF8STRC("Species already exist"));
					}
					else if (nameChg && (bookIgn == 0 || bookIgn[0] != '1') && me->SpeciesBookIsExist(STR_CSTR(sname), &sb))
					{
						msg.AppendC(UTF8STRC("Species already exist in book: "));
						msg.AppendC(sb.ToString(), sb.GetLength());
						msg.AppendC(UTF8STRC(", continue?"));
						bookIgn = (const UTF8Char*)"1";
					}
					else
					{
						me->dataMut.UnlockRead();
						me->dataMut.LockWrite();
						sb.ClearStr();
						sb.Append(sname);
						sb.ToLower();
						sb.ReplaceStr(UTF8STRC(" "), UTF8STRC("_"));
						sb.ReplaceStr(UTF8STRC("."), UTF8STRC(""));
						if (me->SpeciesModify(spId, STR_CSTR(ename), STR_CSTR(cname), STR_CSTR(sname), STR_CSTR(descr), sb.ToCString()))
						{
							me->dataMut.UnlockWrite();
							sb.ClearStr();
							sb.AppendC(UTF8STRC("species.html?id="));
							sb.AppendI32(spId);
							sb.AppendC(UTF8STRC("&cateId="));
							sb.AppendI32(cateId);

							resp->RedirectURL(req, sb.ToCString(), 0);
							return true;
						}
						else
						{
							msg.AppendC(UTF8STRC("Error in modifying species"));
						}
						me->dataMut.UnlockWrite();
						me->dataMut.LockRead();
					}
				}
			}
		}

		IO::MemoryStream mstm;
		Text::UTF8Writer writer(&mstm);

		sb.ClearStr();
		sb.AppendC(UTF8STRC("New Species in "));
		sb.Append(group->chiName);
		sb.AppendC(UTF8STRC(" "));
		sb.Append(group->engName);
		me->WriteHeader(&writer, sb.ToString(), env.user, env.isMobile);
		writer.WriteStrC(UTF8STRC("<center><h1>"));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteStrC(UTF8STRC("</h1>"));
		if (msg.GetLength() > 0)
		{
			writer.WriteStrC(UTF8STRC("<h2>"));
			writer.WriteStr(msg.ToCString());
			writer.WriteStrC(UTF8STRC("</h2>"));
		}
		writer.WriteLineC(UTF8STRC("</center>"));

		writer.WriteStrC(UTF8STRC("<form name=\"newspecies\" method=\"POST\" action=\"speciesmod.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&amp;cateId="));
		sb.AppendI32(group->cateId);
		if (spId != 0)
		{
			sb.AppendC(UTF8STRC("&amp;spId="));
			sb.AppendI32(spId);
		}
		if (bookIgn && bookIgn[0] == '1')
		{
			sb.AppendC(UTF8STRC("&amp;bookIgn=1"));
		}
		writer.WriteStrC(sb.ToString(), sb.GetLength());
		writer.WriteLineC(UTF8STRC("\">"));
		writer.WriteLineC(UTF8STRC("<input type=\"hidden\" name=\"task\"/>"));
		writer.WriteLineC(UTF8STRC("<table border=\"0\">"));
		writer.WriteStrC(UTF8STRC("<tr><td>Chinese Name</td><td><input type=\"text\" name=\"cname\""));
		if (cname)
		{
			writer.WriteStrC(UTF8STRC(" value="));
			s = Text::XML::ToNewAttrText(cname->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
		}
		writer.WriteLineC(UTF8STRC("/></td></tr>"));
		writer.WriteStrC(UTF8STRC("<tr><td>Science Name</td><td><input type=\"text\" name=\"sname\""));
		if (sname)
		{
			writer.WriteStrC(UTF8STRC(" value="));
			s = Text::XML::ToNewAttrText(sname->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
		}
		writer.WriteLineC(UTF8STRC("/></td></tr>"));
		writer.WriteStrC(UTF8STRC("<tr><td>English Name</td><td><input type=\"text\" name=\"ename\""));
		if (ename)
		{
			writer.WriteStrC(UTF8STRC(" value="));
			s = Text::XML::ToNewAttrText(ename->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
		}
		writer.WriteLineC(UTF8STRC("/></td></tr>"));
		writer.WriteStrC(UTF8STRC("<tr><td>Description</td><td><textarea name=\"descr\" rows=\"4\" cols=\"40\">"));
		if (descr)
		{
			s = Text::XML::ToNewHTMLElementText(descr->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
		}
		writer.WriteLineC(UTF8STRC("</textarea></td></tr>"));
		writer.WriteStrC(UTF8STRC("<tr><td></td><td>"));
		if (spId != 0)
		{
			writer.WriteStrC(UTF8STRC("<input type=\"button\" value=\"Modify\" onclick=\"document.forms.newspecies.task.value='modify';document.forms.newspecies.submit();\"/>"));
		}
		writer.WriteStrC(UTF8STRC("<input type=\"button\" value=\"New\" onclick=\"document.forms.newspecies.task.value='new';document.forms.newspecies.submit();\"/>"));

		writer.WriteStrC(UTF8STRC("<a href=\"group.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&amp;cateId="));
		sb.AppendI32(group->cateId);
		writer.WriteStrC(sb.ToString(), sb.GetLength());
		writer.WriteStrC(UTF8STRC("\">"));
		writer.WriteStr(LangGetValue(lang, UTF8STRC("Back")));
		writer.WriteStrC(UTF8STRC("</a>"));
		writer.WriteLineC(UTF8STRC("</td></tr>"));
		writer.WriteLineC(UTF8STRC("</table></form>"));
		me->WriteFooter(&writer);
		me->dataMut.UnlockRead();
		ResponseMstm(req, resp, &mstm, CSTR("text/html"));
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcList(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 id;
	UInt32 page = 0;
	Int32 cateId;
	req->GetQueryValueU32(CSTR("page"), &page);
	if (req->GetQueryValueI32(CSTR("id"), &id) &&
		req->GetQueryValueI32(CSTR("cateId"), &cateId))
	{
		Bool imageOnly = subReq.Equals(UTF8STRC("/listimage.html"));
		Text::String *s;
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
		SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
		IO::ConfigFile *lang = me->LangGet(req);
		me->dataMut.LockRead();
		group = me->groupMap.Get(id);
		Bool notAdmin = (env.user == 0 || env.user->userType != 0);
		if (group == 0 || group->cateId != cateId)
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->cateMap.Get(group->cateId);
		if (cate == 0 || ((cate->flags & 1) && notAdmin))
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		if (me->GroupIsAdmin(group) && notAdmin)
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		IO::MemoryStream mstm;
		Text::UTF8Writer writer(&mstm);

		sb.ClearStr();
		sb.Append(cate->chiName);
		sb.AppendC(UTF8STRC(" - "));
		sb.Append(group->chiName);
		sb.AppendC(UTF8STRC(" "));
		sb.Append(group->engName);
		me->WriteHeader(&writer, sb.ToString(), env.user, env.isMobile);
		writer.WriteStrC(UTF8STRC("<center><h1>"));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("</h1></center>"));

		me->WriteLocator(&writer, group, cate);
		writer.WriteLineC(UTF8STRC("<br/>"));
		if (group->descript)
		{
			s = Text::XML::ToNewHTMLBodyText(group->descript->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
		}
		writer.WriteLineC(UTF8STRC("<br/>"));
		writer.WriteLineC(UTF8STRC("<hr/>"));

		Data::StringMap<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> spMap;
		me->GetGroupSpecies(group, &spMap, env.user);
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> speciesTmp;
		const Data::ReadingList<SpeciesInfo*> *spList;
		spList = spMap.GetValues();
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
		me->WriteSpeciesTable(&writer, &species, env.scnWidth, group->cateId, false);
		writer.WriteLineC(UTF8STRC("<hr/>"));

		if (imageOnly)
		{
			if (page > 0)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<a href=\"listimage.html?id="));
				sb.AppendI32(id);
				sb.AppendC(UTF8STRC("&amp;cateId="));
				sb.AppendI32(group->cateId);
				sb.AppendC(UTF8STRC("&amp;page="));
				sb.AppendU32(page - 1);
				sb.AppendC(UTF8STRC("\">&lt;"));
				sb.Append(LangGetValue(lang, UTF8STRC("Previous")));
				sb.AppendUOSInt(perPage);
				sb.Append(LangGetValue(lang, UTF8STRC("Items")));
				sb.AppendC(UTF8STRC("</a>"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());
			}
			if ((page + 1) * perPage < spList->GetCount())
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<a href=\"listimage.html?id="));
				sb.AppendI32(id);
				sb.AppendC(UTF8STRC("&amp;cateId="));
				sb.AppendI32(group->cateId);
				sb.AppendC(UTF8STRC("&amp;page="));
				sb.AppendU32(page + 1);
				sb.AppendC(UTF8STRC("\">"));
				sb.Append(LangGetValue(lang, UTF8STRC("Next")));
				sb.AppendUOSInt(perPage);
				sb.Append(LangGetValue(lang, UTF8STRC("Items")));
				sb.AppendC(UTF8STRC("&gt;</a>"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());
			}
		}
		else
		{
			if (page > 0)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<a href=\"list.html?id="));
				sb.AppendI32(id);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(group->cateId);
				sb.AppendC(UTF8STRC("&page="));
				sb.AppendU32(page - 1);
				sb.AppendC(UTF8STRC("\">&lt;"));
				sb.Append(LangGetValue(lang, UTF8STRC("Previous")));
				sb.AppendUOSInt(perPage);
				sb.Append(LangGetValue(lang, UTF8STRC("Items")));
				sb.AppendC(UTF8STRC("</a>"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());
			}
			if ((page + 1) * perPage < spMap.GetCount())
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<a href=\"list.html?id="));
				sb.AppendI32(id);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(group->cateId);
				sb.AppendC(UTF8STRC("&page="));
				sb.AppendU32(page + 1);
				sb.AppendC(UTF8STRC("\">"));
				sb.Append(LangGetValue(lang, UTF8STRC("Next")));
				sb.AppendUOSInt(perPage);
				sb.Append(LangGetValue(lang, UTF8STRC("Items")));
				sb.AppendC(UTF8STRC("&gt;</a>"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());
			}
		}
		writer.WriteLineC(UTF8STRC("<br/>"));
		writer.WriteLineC(UTF8STRC("<br/>"));
		writer.WriteStrC(UTF8STRC("<a href=\"group.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&cateId="));
		sb.AppendI32(group->cateId);
		writer.WriteStrC(sb.ToString(), sb.GetLength());
		writer.WriteStrC(UTF8STRC("\">"));
		writer.WriteStr(LangGetValue(lang, UTF8STRC("Back")));
		writer.WriteStrC(UTF8STRC("</a>"));

		me->WriteFooter(&writer);
		me->dataMut.UnlockRead();
		ResponseMstm(req, resp, &mstm, CSTR("text/html"));
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcPhotoDetail(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 id;
	Int32 cateId;
	if (req->GetQueryValueI32(CSTR("id"), &id) &&
		req->GetQueryValueI32(CSTR("cateId"), &cateId))
	{
		UTF8Char fileName[512];
		UTF8Char *fileNameEnd;
		Int32 fileId;
		Text::String *s;
		UOSInt i;
		UOSInt j;
		UTF8Char sbuff[512];
		UTF8Char sbuff2[512];
		UTF8Char *sptr;
		UTF8Char *sptrEnd;
		UTF8Char *sptr2;
		IO::Path::FindFileSession *sess;
		IO::Path::PathType pt;
		Text::StringBuilderUTF8 sb;
		SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species;
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
		SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
		Text::PString sarr[4];
		SSWR::OrganMgr::OrganWebHandler::WebFileInfo *wfile;
		IO::ConfigFile *lang = me->LangGet(req);

		me->dataMut.LockRead();
		species = me->spMap.Get(id);
		if (species == 0 || species->cateId != cateId)
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		group = me->groupMap.Get(species->groupId);
		if (group == 0)
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->cateMap.Get(group->cateId);
		if (cate == 0)
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		sptr = cate->srcDir->ConcatTo(sbuff);
		if (IO::Path::PATH_SEPERATOR != '\\')
		{
			Text::StrReplace(sbuff, '\\', IO::Path::PATH_SEPERATOR);
		}
		sptr = species->dirName->ConcatTo(sptr);
		*sptr++ = IO::Path::PATH_SEPERATOR;

		if (req->GetQueryValueI32(CSTR("fileId"), &fileId))
		{
			Bool found = false;
			SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile;
			i = 0;
			j = species->files.GetCount();
			while (i < j)
			{
				userFile = species->files.GetItem(i);
				if (userFile->id == fileId)
				{
					found = true;
					break;
				}
				i++;
			}
			if (found)
			{
				if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST && env.user && (env.user->userType == 0 || env.user->id == userFile->webuserId))
				{
					req->ParseHTTPForm();
					Text::String *action = req->GetHTTPFormStr(CSTR("action"));
					if (action && action->Equals(UTF8STRC("setdefault")) && env.user->userType == 0)
					{
						me->SpeciesSetPhotoId(id, fileId);
					}
					else if (action && action->Equals(UTF8STRC("setname")))
					{
						Text::String *desc = req->GetHTTPFormStr(CSTR("descr"));
						if (desc)
						{
							me->UserfileUpdateDesc(fileId, desc->ToCString());
						}
					}
					else if (action && action->Equals(UTF8STRC("rotate")))
					{
						me->UserfileUpdateRotType(fileId, (userFile->rotType + 1) & 3);
					}
				}

				IO::MemoryStream mstm;
				Text::UTF8Writer writer(&mstm);

				sb.ClearStr();
				sb.Append(cate->chiName);
				sb.AppendC(UTF8STRC(" - "));
				sb.Append(species->sciName);
				sb.AppendC(UTF8STRC(" "));
				sb.Append(species->chiName);
				sb.AppendC(UTF8STRC(" "));
				sb.Append(species->engName);
				me->WriteHeader(&writer, sb.ToString(), env.user, env.isMobile);
				writer.WriteStrC(UTF8STRC("<center><h1>"));
				s = Text::XML::ToNewHTMLBodyText(sb.ToString());
				writer.WriteStrC(s->v, s->leng);
				s->Release();
				writer.WriteLineC(UTF8STRC("</h1></center>"));

				writer.WriteLineC(UTF8STRC("<center>"));
				writer.WriteLineC(UTF8STRC("<table border=\"0\" width=\"100%\">"));
				writer.WriteLineC(UTF8STRC("<tr><td align=\"center\">"));

				if (i < j - 1)
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("photodetail.html?id="));
					sb.AppendI32(species->speciesId);
					sb.AppendC(UTF8STRC("&cateId="));
					sb.AppendI32(species->cateId);
					sb.AppendC(UTF8STRC("&fileId="));
					sb.AppendI32(species->files.GetItem(i + 1)->id);
				}
				else if (species->wfiles.GetCount() != 0)
				{
					wfile = species->wfiles.GetItem(0);
					sb.ClearStr();
					sb.AppendC(UTF8STRC("photodetail.html?id="));
					sb.AppendI32(species->speciesId);
					sb.AppendC(UTF8STRC("&cateId="));
					sb.AppendI32(species->cateId);
					sb.AppendC(UTF8STRC("&fileWId="));
					sb.AppendI32(wfile->id);
				}
				else
				{
					Data::ArrayListICaseString fileNameList;

					sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
					sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
					if (sess)
					{

						while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
						{
							if (pt == IO::Path::PathType::File)
							{
								if (Text::StrEndsWithICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC(".JPG")) || Text::StrEndsWithICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC(".PCX")) || Text::StrEndsWithICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC(".TIF")))
								{
									sptr2[-4] = 0;
									fileNameList.SortedInsert(Text::String::New(sptr, (UOSInt)(sptr2 - sptr - 4)));
								}
							}
						}
						IO::Path::FindFileClose(sess);
					}
					if (fileNameList.GetCount() > 0)
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("photodetail.html?id="));
						sb.AppendI32(species->speciesId);
						sb.AppendC(UTF8STRC("&cateId="));
						sb.AppendI32(species->cateId);
						sb.AppendC(UTF8STRC("&file="));
						sptr2 = Text::TextBinEnc::URIEncoding::URIEncode(sbuff2, fileNameList.GetItem(0)->v);
						sb.AppendC(sbuff2, (UOSInt)(sptr2 - sbuff2));
					}
					else
					{
						Bool found;
						{
							sptrEnd = Text::StrConcatC(sptr, UTF8STRC("web.txt"));
							IO::FileStream fs({sbuff, (UOSInt)(sptrEnd - sbuff)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
							Text::UTF8Reader reader(&fs);
							sb.ClearStr();
							found = false;
							while (reader.ReadLine(&sb, 4096))
							{
								if (Text::StrSplitP(sarr, 4, sb, '\t') == 3)
								{
									found = true;
									sptr2 = Text::StrConcatC(sbuff2, UTF8STRC("web"));
									*sptr2++ = IO::Path::PATH_SEPERATOR;
									sptr2 = Text::StrConcatC(sptr2, sarr[0].v, sarr[0].leng);
									i = Text::StrLastIndexOfCharC(sbuff2, (UOSInt)(sptr2 - sbuff2), '.');
									if (i != INVALID_INDEX)
									{
										sbuff2[i] = 0;
										sptr2 = &sbuff2[i];
									}
									break;
								}
								sb.ClearStr();
							}
						}

						if (found)
						{
							sb.ClearStr();
							sb.AppendC(sbuff2, (UOSInt)(sptr2 - sbuff2));
							sptr2 = Text::TextBinEnc::URIEncoding::URIEncode(sbuff2, sb.ToString());
							sb.ClearStr();
							sb.AppendC(UTF8STRC("photodetail.html?id="));
							sb.AppendI32(species->speciesId);
							sb.AppendC(UTF8STRC("&cateId="));
							sb.AppendI32(species->cateId);
							sb.AppendC(UTF8STRC("&file="));
							sb.AppendC(sbuff2, (UOSInt)(sptr2 - sbuff2));
						}
						else
						{
							sb.ClearStr();
							sb.AppendC(UTF8STRC("species.html?id="));
							sb.AppendI32(species->speciesId);
							sb.AppendC(UTF8STRC("&cateId="));
							sb.AppendI32(species->cateId);
						}
					}
					LIST_FREE_STRING(&fileNameList);
				}
				s = Text::XML::ToNewAttrText(sb.ToString());
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<a href="));
				sb.Append(s);
				s->Release();
				sb.AppendC(UTF8STRC(">"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());

				sb.ClearStr();
				sb.AppendC(UTF8STRC("photo.html?id="));
				sb.AppendI32(species->speciesId);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(species->cateId);
				sb.AppendC(UTF8STRC("&width="));
				sb.AppendU32(env.scnWidth);
				sb.AppendC(UTF8STRC("&height="));
				sb.AppendU32(env.scnWidth);
				sb.AppendC(UTF8STRC("&fileId="));
				sb.AppendI32(fileId);
				s = Text::XML::ToNewAttrText(sb.ToString());
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<img src="));
				sb.Append(s);
				s->Release();
				sb.AppendC(UTF8STRC(" border=\"0\"></a>"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());
				writer.WriteLineC(UTF8STRC("</td></tr>"));
				writer.WriteLineC(UTF8STRC("<tr><td align=\"center\">"));


				if (userFile->fileType == 3)
				{
					Data::DateTime dt;
					UInt64 fileSize = 0;
					Media::MediaFile *mediaFile;
					sptr = me->dataDir->ConcatTo(sbuff);
					if (sptr[-1] != IO::Path::PATH_SEPERATOR)
					{
						*sptr++ = IO::Path::PATH_SEPERATOR;
					}
					sptr = Text::StrConcatC(sptr, UTF8STRC("UserFile"));
					*sptr++ = IO::Path::PATH_SEPERATOR;
					sptr = Text::StrInt32(sptr, userFile->webuserId);
					*sptr++ = IO::Path::PATH_SEPERATOR;
					dt.SetTicks(userFile->fileTimeTicks);
					sptr = dt.ToString(sptr, "yyyyMM");
					*sptr++ = IO::Path::PATH_SEPERATOR;
					sptr = userFile->dataFileName->ConcatTo(sptr);
					{
						Sync::MutexUsage mutUsage(&me->parserMut);
						IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
						fileSize = fd.GetDataSize();
						mediaFile = (Media::MediaFile*)me->parsers.ParseFileType(&fd, IO::ParserType::MediaFile);
					}

					if (mediaFile)
					{
						sb.ClearStr();
						sb.AppendU64(fileSize);
						sb.AppendC(UTF8STRC(" bytes"));
						Media::IMediaSource *msrc = mediaFile->GetStream(0, 0);
						Int32 stmTime;
						if (msrc)
						{
							stmTime = msrc->GetStreamTime();
							sb.AppendC(UTF8STRC(", Length: "));
							sb.AppendI32(stmTime / 60000);
							sb.AppendC(UTF8STRC(":"));
							stmTime = stmTime % 60000;
							if (stmTime < 10000)
							{
								sb.AppendC(UTF8STRC("0"));
							}
							Text::SBAppendF64(&sb, stmTime * 0.001);

							if (msrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
							{
								Media::IAudioSource *asrc = (Media::IAudioSource*)msrc;
								Media::AudioFormat format;
								asrc->GetFormat(&format);
								sb.AppendC(UTF8STRC(" "));
								sb.AppendU32(format.frequency);
								sb.AppendC(UTF8STRC("Hz, "));
								sb.AppendU32(format.bitpersample);
								sb.AppendC(UTF8STRC("bits, "));
								sb.AppendU32(format.nChannels);
								sb.AppendC(UTF8STRC(" ch"));
							}
						}
						writer.WriteStrC(UTF8STRC("<b>"));
						writer.WriteStr(LangGetValue(lang, UTF8STRC("PhotoSpec")));
						writer.WriteStrC(UTF8STRC("</b> "));
						writer.WriteStrC(sb.ToString(), sb.GetLength());
						writer.WriteStrC(UTF8STRC("<br/>"));
						DEL_CLASS(mediaFile);
					}
					if (userFile->captureTimeTicks != 0)
					{
						dt.SetTicks(userFile->captureTimeTicks);
						dt.ToLocalTime();
						writer.WriteStrC(UTF8STRC("<b>"));
						writer.WriteStr(LangGetValue(lang, UTF8STRC("PhotoDate")));
						writer.WriteStrC(UTF8STRC("</b> "));
						sptr2 = dt.ToString(sbuff2, "yyyy-MM-dd HH:mm:ss zzzz");
						writer.WriteStrC(sbuff2, (UOSInt)(sptr2 - sbuff2));
						writer.WriteStrC(UTF8STRC("<br/>"));
					}
				}
				else
				{
					Data::DateTime dt;
					sptr = me->dataDir->ConcatTo(sbuff);
					if (sptr[-1] != IO::Path::PATH_SEPERATOR)
					{
						*sptr++ = IO::Path::PATH_SEPERATOR;
					}
					sptr = Text::StrConcatC(sptr, UTF8STRC("UserFile"));
					*sptr++ = IO::Path::PATH_SEPERATOR;
					sptr = Text::StrInt32(sptr, userFile->webuserId);
					*sptr++ = IO::Path::PATH_SEPERATOR;
					dt.SetTicks(userFile->fileTimeTicks);
					sptr = dt.ToString(sptr, "yyyyMM");
					*sptr++ = IO::Path::PATH_SEPERATOR;
					sptr = userFile->dataFileName->ConcatTo(sptr);

					IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
					Media::PhotoInfo info(&fd);
					if (info.HasInfo())
					{
						sb.ClearStr();
						writer.WriteStrC(UTF8STRC("<b>"));
						writer.WriteStr(LangGetValue(lang, UTF8STRC("PhotoSpec")));
						writer.WriteStrC(UTF8STRC("</b> "));
						info.ToString(&sb);
						writer.WriteStrC(sb.ToString(), sb.GetLength());
						writer.WriteStrC(UTF8STRC("<br/>"));

						dt.SetTicks(userFile->captureTimeTicks);
						dt.ToLocalTime();
						writer.WriteStrC(UTF8STRC("<b>"));
						writer.WriteStr(LangGetValue(lang, UTF8STRC("PhotoDate")));
						writer.WriteStrC(UTF8STRC("</b> "));
						sptr2 = dt.ToString(sbuff2, "yyyy-MM-dd HH:mm:ss zzzz");
						writer.WriteStrC(sbuff2, (UOSInt)(sptr2 - sbuff2));
						writer.WriteStrC(UTF8STRC("<br/>"));
					}
				}

				if (env.user && (userFile->webuserId == env.user->id || env.user->userType == 0))
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("photodown.html?id="));
					sb.AppendI32(species->speciesId);
					sb.AppendC(UTF8STRC("&cateId="));
					sb.AppendI32(species->cateId);
					sb.AppendC(UTF8STRC("&fileId="));
					sb.AppendI32(userFile->id);
					s = Text::XML::ToNewAttrText(sb.ToString());
					sb.ClearStr();
					sb.AppendC(UTF8STRC("<a href="));
					sb.Append(s);
					s->Release();
					sb.AppendC(UTF8STRC(">"));
					sb.Append(LangGetValue(lang, UTF8STRC("Download")));
					sb.AppendC(UTF8STRC("</a>"));
					writer.WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("<form name=\"photo\" method=\"POST\" action=\"photodetail.html?id="));
					sb.AppendI32(id);
					sb.AppendC(UTF8STRC("&amp;cateId="));
					sb.AppendI32(cateId);
					sb.AppendC(UTF8STRC("&amp;fileId="));
					sb.AppendI32(fileId);
					sb.AppendC(UTF8STRC("\"><input type=\"hidden\" name=\"action\"/>"));
					if (env.user->userType == 0)
					{
						sb.AppendC(UTF8STRC("<input type=\"button\" value=\"Set As Species Photo\" onclick=\"document.forms.photo.action.value='setdefault';document.forms.photo.submit();\"/>"));
					}
					sb.AppendC(UTF8STRC("<input type=\"button\" value=\"Rotate\" onclick=\"document.forms.photo.action.value='rotate';document.forms.photo.submit();\"/>"));
					sb.AppendC(UTF8STRC("<br/>"));
					writer.WriteLineC(sb.ToString(), sb.GetLength());
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Description<input type=\"text\" name=\"descr\""));
					if (userFile->descript)
					{
						sb.AppendC(UTF8STRC(" value="));
						s = Text::XML::ToNewAttrText(userFile->descript->v);
						sb.Append(s);
						s->Release();
					}
					sb.AppendC(UTF8STRC("/><input type=\"button\" value=\"Update\" onclick=\"document.forms.photo.action.value='setname';document.forms.photo.submit();\"/>"));
					sb.AppendC(UTF8STRC("</form>"));
					writer.WriteLineC(sb.ToString(), sb.GetLength());
				}

				if (env.user && env.user->userType == 0)
				{
					
				}
				sb.ClearStr();
				sb.AppendC(UTF8STRC("species.html?id="));
				sb.AppendI32(species->speciesId);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(species->cateId);
				s = Text::XML::ToNewAttrText(sb.ToString());
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<a href="));
				sb.Append(s);
				s->Release();
				sb.AppendC(UTF8STRC(">"));
				sb.Append(LangGetValue(lang, UTF8STRC("Back")));
				sb.AppendC(UTF8STRC("</a>"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());
				writer.WriteLineC(UTF8STRC("</td></tr>"));
				writer.WriteLineC(UTF8STRC("</table>"));
				writer.WriteLineC(UTF8STRC("</center>"));

				me->WriteFooter(&writer);
				me->dataMut.UnlockRead();
				ResponseMstm(req, resp, &mstm, CSTR("text/html"));
				return true;
			}
			else
			{
				resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
				me->dataMut.UnlockRead();
				return true;
			}
		}
		else if (req->GetQueryValueI32(CSTR("fileWId"), &fileId))
		{
			wfile = species->wfiles.Get(fileId);
			if (wfile)
			{
				IO::MemoryStream mstm;
				Text::UTF8Writer writer(&mstm);

				sb.ClearStr();
				sb.Append(cate->chiName);
				sb.AppendC(UTF8STRC(" - "));
				sb.Append(species->sciName);
				sb.AppendC(UTF8STRC(" "));
				sb.Append(species->chiName);
				sb.AppendC(UTF8STRC(" "));
				sb.Append(species->engName);
				me->WriteHeader(&writer, sb.ToString(), env.user, env.isMobile);
				writer.WriteStrC(UTF8STRC("<center><h1>"));
				s = Text::XML::ToNewHTMLBodyText(sb.ToString());
				writer.WriteStrC(s->v, s->leng);
				s->Release();
				writer.WriteLineC(UTF8STRC("</h1></center>"));

				writer.WriteLineC(UTF8STRC("<center>"));
				writer.WriteLineC(UTF8STRC("<table border=\"0\" width=\"100%\">"));
				writer.WriteLineC(UTF8STRC("<tr><td align=\"center\">"));

				i = (UOSInt)species->wfiles.GetIndex(fileId);
				j = species->wfiles.GetCount();
				if (i + 1 < j)
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("photodetail.html?id="));
					sb.AppendI32(species->speciesId);
					sb.AppendC(UTF8STRC("&cateId="));
					sb.AppendI32(species->cateId);
					sb.AppendC(UTF8STRC("&fileWId="));
					sb.AppendI32(species->wfiles.GetKey(i + 1));
				}
				else
				{
					Data::ArrayListICaseString fileNameList;

					sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
					sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
					if (sess)
					{

						while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
						{
							if (pt == IO::Path::PathType::File)
							{
								if (Text::StrEndsWithICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC(".JPG")) || Text::StrEndsWithICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC(".PCX")) || Text::StrEndsWithICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC(".TIF")))
								{
									sptr2[-4] = 0;
									fileNameList.SortedInsert(Text::String::New(sptr, (UOSInt)(sptr2 - sptr - 4)));
								}
							}
						}
						IO::Path::FindFileClose(sess);
					}
					if (fileNameList.GetCount() > 0)
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("photodetail.html?id="));
						sb.AppendI32(species->speciesId);
						sb.AppendC(UTF8STRC("&cateId="));
						sb.AppendI32(species->cateId);
						sb.AppendC(UTF8STRC("&file="));
						sptr2 = Text::TextBinEnc::URIEncoding::URIEncode(sbuff2, fileNameList.GetItem(0)->v);
						sb.AppendC(sbuff2, (UOSInt)(sptr2 - sbuff2));
					}
					else
					{
						Bool found;
						{
							sptrEnd = Text::StrConcatC(sptr, UTF8STRC("web.txt"));
							IO::FileStream fs({sbuff, (UOSInt)(sptrEnd - sbuff)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
							Text::UTF8Reader reader(&fs);
							sb.ClearStr();
							found = false;
							while (reader.ReadLine(&sb, 4096))
							{
								if (Text::StrSplitP(sarr, 4, sb, '\t') == 3)
								{
									found = true;
									sptr2 = Text::StrConcatC(sbuff2, UTF8STRC("web"));
									*sptr2++ = IO::Path::PATH_SEPERATOR;
									sptr2 = Text::StrConcatC(sptr2, sarr[0].v, sarr[0].leng);
									i = Text::StrLastIndexOfCharC(sbuff2, (UOSInt)(sptr2 - sbuff2), '.');
									if (i != INVALID_INDEX)
									{
										sbuff2[i] = 0;
										sptr2 = &sbuff2[i];
									}
									break;
								}
								sb.ClearStr();
							}
						}

						if (found)
						{
							sb.ClearStr();
							sb.AppendC(sbuff2, (UOSInt)(sptr2 - sbuff2));
							sptr2 = Text::TextBinEnc::URIEncoding::URIEncode(sbuff2, sb.ToString());
							sb.ClearStr();
							sb.AppendC(UTF8STRC("photodetail.html?id="));
							sb.AppendI32(species->speciesId);
							sb.AppendC(UTF8STRC("&cateId="));
							sb.AppendI32(species->cateId);
							sb.AppendC(UTF8STRC("&file="));
							sb.AppendC(sbuff2, (UOSInt)(sptr2 - sbuff2));
						}
						else
						{
							sb.ClearStr();
							sb.AppendC(UTF8STRC("species.html?id="));
							sb.AppendI32(species->speciesId);
							sb.AppendC(UTF8STRC("&cateId="));
							sb.AppendI32(species->cateId);
						}
					}
					LIST_FREE_STRING(&fileNameList);
				}
				s = Text::XML::ToNewAttrText(sb.ToString());
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<a href="));
				sb.Append(s);
				s->Release();
				sb.AppendC(UTF8STRC(">"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());

				sb.ClearStr();
				sb.AppendC(UTF8STRC("photo.html?id="));
				sb.AppendI32(species->speciesId);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(species->cateId);
				sb.AppendC(UTF8STRC("&width="));
				sb.AppendU32(env.scnWidth);
				sb.AppendC(UTF8STRC("&height="));
				sb.AppendU32(env.scnWidth);
				sb.AppendC(UTF8STRC("&fileWId="));
				sb.AppendI32(fileId);
				s = Text::XML::ToNewAttrText(sb.ToString());
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<img src="));
				sb.Append(s);
				s->Release();
				sb.AppendC(UTF8STRC(" border=\"0\"></a>"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());
				writer.WriteLineC(UTF8STRC("</td></tr>"));
				writer.WriteLineC(UTF8STRC("<tr><td align=\"center\">"));

				writer.WriteStrC(UTF8STRC("<b>Image URL:</b> <a href="));
				s = Text::XML::ToNewAttrText(wfile->imgUrl->v);
				writer.WriteStrC(s->v, s->leng);
				s->Release();
				writer.WriteStrC(UTF8STRC(">"));
				writer.WriteStrC(wfile->imgUrl->v, wfile->imgUrl->leng);
				writer.WriteStrC(UTF8STRC("</a><br/>"));

				writer.WriteStrC(UTF8STRC("<b>Source URL:</b> <a href="));
				s = Text::XML::ToNewAttrText(wfile->srcUrl->v);
				writer.WriteStrC(s->v, s->leng);
				s->Release();
				writer.WriteStrC(UTF8STRC(">"));
				writer.WriteStrC(wfile->srcUrl->v, wfile->srcUrl->leng);
				writer.WriteStrC(UTF8STRC("</a><br/>"));

				sb.ClearStr();
				sb.AppendC(UTF8STRC("species.html?id="));
				sb.AppendI32(species->speciesId);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(species->cateId);
				s = Text::XML::ToNewAttrText(sb.ToString());
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<a href="));
				sb.Append(s);
				s->Release();
				sb.Append(LangGetValue(lang, UTF8STRC("Back")));
				sb.AppendC(UTF8STRC("</a>"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());
				writer.WriteLineC(UTF8STRC("</td></tr>"));
				writer.WriteLineC(UTF8STRC("</table>"));
				writer.WriteLineC(UTF8STRC("</center>"));

				me->WriteFooter(&writer);
				me->dataMut.UnlockRead();
				ResponseMstm(req, resp, &mstm, CSTR("text/html"));
				return true;
			}
			else
			{
				resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
				me->dataMut.UnlockRead();
				return true;
			}
		}
		else if ((fileNameEnd = req->GetQueryValueStr(CSTR("file"), fileName, 512)) != 0)
		{
			if (Text::StrStartsWithC(fileName, (UOSInt)(fileNameEnd - fileName), UTF8STRC("web")) && fileName[3] == IO::Path::PATH_SEPERATOR)
			{
				Text::String *srcURL = 0;
				Text::String *imgURL = 0;
				Bool found;
				Bool foundNext;
				{
					sptrEnd = Text::StrConcatC(sptr, UTF8STRC("web.txt"));
					sptr2 = Text::StrConcatC(Text::StrConcat(sbuff2, &fileName[4]), UTF8STRC("."));
					Text::StrToUpperC(sbuff2, sbuff2, (UOSInt)(sptr2 - sbuff2));
					IO::FileStream fs({sbuff, (UOSInt)(sptrEnd - sbuff)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
					Text::UTF8Reader reader(&fs);
					sb.ClearStr();
					found = false;
					foundNext = false;
					while (reader.ReadLine(&sb, 4096))
					{
						if (Text::StrSplitP(sarr, 4, sb, '\t') == 3)
						{
							if (found)
							{
								foundNext = true;
								Text::StrConcatC(sbuff2, sarr[0].v, sarr[0].leng);
								break;
							}
							else
							{
								Text::StrToUpperC(sarr[0].v, sarr[0].v, sarr[0].leng);
								if (Text::StrStartsWithC(sarr[0].v, sarr[0].leng, sbuff2, (UOSInt)(sptr2 - sbuff2)))
								{
									found = true;
									foundNext = false;
									srcURL = Text::String::New(sarr[2].v, sarr[2].leng);
									imgURL = Text::String::New(sarr[1].v, sarr[1].leng);
								}
							}
						}
						sb.ClearStr();
					}
				}

				if (found)
				{
					IO::MemoryStream mstm;
					Text::UTF8Writer writer(&mstm);

					sb.ClearStr();
					sb.Append(cate->chiName);
					sb.AppendC(UTF8STRC(" - "));
					sb.Append(species->sciName);
					sb.AppendC(UTF8STRC(" "));
					sb.Append(species->chiName);
					sb.AppendC(UTF8STRC(" "));
					sb.Append(species->engName);
					me->WriteHeader(&writer, sb.ToString(), env.user, env.isMobile);
					writer.WriteStrC(UTF8STRC("<center><h1>"));
					s = Text::XML::ToNewHTMLBodyText(sb.ToString());
					writer.WriteStrC(s->v, s->leng);
					s->Release();
					writer.WriteLineC(UTF8STRC("</h1></center>"));

					writer.WriteLineC(UTF8STRC("<center>"));
					writer.WriteLineC(UTF8STRC("<table border=\"0\" width=\"100%\">"));
					writer.WriteLineC(UTF8STRC("<tr><td align=\"center\">"));
					if (foundNext)
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("photodetail.html?id="));
						sb.AppendI32(species->speciesId);
						sb.AppendC(UTF8STRC("&cateId="));
						sb.AppendI32(species->cateId);
						sb.AppendC(UTF8STRC("&file="));
						sptr2 = Text::StrConcatC(sbuff, UTF8STRC("web"));
						*sptr2++ = IO::Path::PATH_SEPERATOR;
						sptr2 = Text::StrConcat(sptr2, sbuff2);
						i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr2 - sbuff), '.');
						if (i != INVALID_INDEX)
							sbuff[i] = 0;
						sptr2 = Text::TextBinEnc::URIEncoding::URIEncode(sbuff2, sbuff);
						sb.AppendC(sbuff2, (UOSInt)(sptr2 - sbuff2));
					}
					else
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("species.html?id="));
						sb.AppendI32(species->speciesId);
						sb.AppendC(UTF8STRC("&cateId="));
						sb.AppendI32(species->cateId);
					}
					s = Text::XML::ToNewAttrText(sb.ToString());
					sb.ClearStr();
					sb.AppendC(UTF8STRC("<a href="));
					sb.Append(s);
					s->Release();
					sb.AppendC(UTF8STRC(">"));
					writer.WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("photo.html?id="));
					sb.AppendI32(species->speciesId);
					sb.AppendC(UTF8STRC("&cateId="));
					sb.AppendI32(species->cateId);
					sb.AppendC(UTF8STRC("&width="));
					sb.AppendU32(env.scnWidth);
					sb.AppendC(UTF8STRC("&height="));
					sb.AppendU32(env.scnWidth);
					sb.AppendC(UTF8STRC("&file="));
					sb.AppendSlow(fileName);
					s = Text::XML::ToNewAttrText(sb.ToString());
					sb.ClearStr();
					sb.AppendC(UTF8STRC("<img src="));
					sb.Append(s);
					s->Release();
					sb.AppendC(UTF8STRC(" border=\"0\"></a>"));
					writer.WriteLineC(sb.ToString(), sb.GetLength());
					writer.WriteLineC(UTF8STRC("</td></tr>"));
					writer.WriteLineC(UTF8STRC("<tr><td align=\"center\">"));

					writer.WriteStrC(UTF8STRC("<b>Image URL:</b> <a href="));
					s = Text::XML::ToNewAttrText(imgURL->v);
					writer.WriteStrC(s->v, s->leng);
					s->Release();
					writer.WriteStrC(UTF8STRC(">"));
					writer.WriteStrC(imgURL->v, imgURL->leng);
					writer.WriteStrC(UTF8STRC("</a><br/>"));

					writer.WriteStrC(UTF8STRC("<b>Source URL:</b> <a href="));
					s = Text::XML::ToNewAttrText(srcURL->v);
					writer.WriteStrC(s->v, s->leng);
					s->Release();
					writer.WriteStrC(UTF8STRC(">"));
					writer.WriteStrC(srcURL->v, srcURL->leng);
					writer.WriteStrC(UTF8STRC("</a><br/>"));

					sb.ClearStr();
					sb.AppendC(UTF8STRC("species.html?id="));
					sb.AppendI32(species->speciesId);
					sb.AppendC(UTF8STRC("&cateId="));
					sb.AppendI32(species->cateId);
					s = Text::XML::ToNewAttrText(sb.ToString());
					sb.ClearStr();
					sb.AppendC(UTF8STRC("<a href="));
					sb.Append(s);
					s->Release();
					sb.Append(LangGetValue(lang, UTF8STRC("Back")));
					sb.AppendC(UTF8STRC("</a>"));
					writer.WriteLineC(sb.ToString(), sb.GetLength());
					writer.WriteLineC(UTF8STRC("</td></tr>"));
					writer.WriteLineC(UTF8STRC("</table>"));
					writer.WriteLineC(UTF8STRC("</center>"));

					me->WriteFooter(&writer);
					me->dataMut.UnlockRead();
					ResponseMstm(req, resp, &mstm, CSTR("text/html"));

					srcURL->Release();
					imgURL->Release();
					return true;
				}
				else
				{
					resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
					me->dataMut.UnlockRead();
					return true;
				}
			}
			else
			{
				Data::ArrayListICaseString fileNameList;

				sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
				sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
				if (sess)
				{
					while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
					{
						if (pt == IO::Path::PathType::File)
						{
							if (Text::StrEndsWithICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC(".JPG")) || Text::StrEndsWithICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC(".PCX")) || Text::StrEndsWithICaseC(sptr, (UOSInt)(sptr2 - sptr), UTF8STRC(".TIF")))
							{
								sptr2[-4] = 0;
								fileNameList.SortedInsert(Text::String::New(sptr, (UOSInt)(sptr2 - sptr - 4)));
							}
						}
					}
					IO::Path::FindFileClose(sess);
					i = (UOSInt)fileNameList.SortedIndexOfPtr(fileName, (UOSInt)(fileNameEnd - fileName));
					if ((OSInt)i < 0)
					{
						LIST_FREE_STRING(&fileNameList);
						resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
						me->dataMut.UnlockRead();
						return true;
					}

					IO::MemoryStream mstm;
					Text::UTF8Writer writer(&mstm);

					sb.ClearStr();
					sb.Append(cate->chiName);
					sb.AppendC(UTF8STRC(" - "));
					sb.Append(species->sciName);
					sb.AppendC(UTF8STRC(" "));
					sb.Append(species->chiName);
					sb.AppendC(UTF8STRC(" "));
					sb.Append(species->engName);
					me->WriteHeader(&writer, sb.ToString(), env.user, env.isMobile);
					writer.WriteStrC(UTF8STRC("<center><h1>"));
					s = Text::XML::ToNewHTMLBodyText(sb.ToString());
					writer.WriteStrC(s->v, s->leng);
					s->Release();
					writer.WriteLineC(UTF8STRC("</h1></center>"));

					writer.WriteLineC(UTF8STRC("<center>"));
					writer.WriteLineC(UTF8STRC("<table border=\"0\" width=\"100%\">"));
					writer.WriteLineC(UTF8STRC("<tr><td align=\"center\">"));
					if (i < fileNameList.GetCount() - 1)
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("photodetail.html?id="));
						sb.AppendI32(species->speciesId);
						sb.AppendC(UTF8STRC("&cateId="));
						sb.AppendI32(species->cateId);
						sb.AppendC(UTF8STRC("&file="));
						sptr2 = Text::TextBinEnc::URIEncoding::URIEncode(sbuff2, fileNameList.GetItem(i + 1)->v);
						sb.AppendC(sbuff2, (UOSInt)(sptr2 - sbuff2));
					}
					else
					{
						Bool found;
						{
							sptrEnd = Text::StrConcatC(sptr, UTF8STRC("web.txt"));
							IO::FileStream fs({sbuff, (UOSInt)(sptrEnd - sbuff)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
							Text::UTF8Reader reader(&fs);
							sb.ClearStr();
							found = false;
							while (reader.ReadLine(&sb, 4096))
							{
								if (Text::StrSplitP(sarr, 4, sb, '\t') == 3)
								{
									found = true;
									sptr2 = Text::StrConcatC(sbuff2, UTF8STRC("web"));
									*sptr2++ = IO::Path::PATH_SEPERATOR;
									sptr2 = Text::StrConcatC(sptr2, sarr[0].v, sarr[0].leng);
									i = Text::StrLastIndexOfCharC(sbuff2, (UOSInt)(sptr2 - sbuff2), '.');
									if (i != INVALID_INDEX)
									{
										sbuff2[i] = 0;
										sptr2 = &sbuff2[i];
									}
									break;
								}
								sb.ClearStr();
							}
						}

						if (found)
						{
							sb.ClearStr();
							sb.AppendC(sbuff2, (UOSInt)(sptr2 - sbuff2));
							sptr2 = Text::TextBinEnc::URIEncoding::URIEncode(sbuff2, sb.ToString());
							sb.ClearStr();
							sb.AppendC(UTF8STRC("photodetail.html?id="));
							sb.AppendI32(species->speciesId);
							sb.AppendC(UTF8STRC("&cateId="));
							sb.AppendI32(species->cateId);
							sb.AppendC(UTF8STRC("&file="));
							sb.AppendC(sbuff2, (UOSInt)(sptr2 - sbuff2));
						}
						else
						{
							sb.ClearStr();
							sb.AppendC(UTF8STRC("species.html?id="));
							sb.AppendI32(species->speciesId);
							sb.AppendC(UTF8STRC("&cateId="));
							sb.AppendI32(species->cateId);
						}
					}
					s = Text::XML::ToNewAttrText(sb.ToString());
					sb.ClearStr();
					sb.AppendC(UTF8STRC("<a href="));
					sb.Append(s);
					s->Release();
					sb.AppendC(UTF8STRC(">"));
					writer.WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("photo.html?id="));
					sb.AppendI32(species->speciesId);
					sb.AppendC(UTF8STRC("&cateId="));
					sb.AppendI32(species->cateId);
					sb.AppendC(UTF8STRC("&width="));
					sb.AppendU32(env.scnWidth);
					sb.AppendC(UTF8STRC("&height="));
					sb.AppendU32(env.scnWidth);
					sb.AppendC(UTF8STRC("&file="));
					sb.AppendSlow(fileName);
					s = Text::XML::ToNewAttrText(sb.ToString());
					sb.ClearStr();
					sb.AppendC(UTF8STRC("<img src="));
					sb.Append(s);
					s->Release();
					sb.AppendC(UTF8STRC(" border=\"0\"></a>"));
					writer.WriteLineC(sb.ToString(), sb.GetLength());
					writer.WriteLineC(UTF8STRC("</td></tr>"));
					writer.WriteLineC(UTF8STRC("<tr><td align=\"center\">"));

					sptrEnd = Text::StrConcatC(Text::StrConcat(sptr, fileName), UTF8STRC(".jpg"));
					{
						IO::StmData::FileData fd({sbuff, (UOSInt)(sptrEnd - sbuff)}, false);
						Media::PhotoInfo info(&fd);
						if (info.HasInfo())
						{
							Data::DateTime dt;
							sb.ClearStr();
							writer.WriteStrC(UTF8STRC("<b>"));
							writer.WriteStr(LangGetValue(lang, UTF8STRC("PhotoSpec")));
							writer.WriteStrC(UTF8STRC("</b> "));
							info.ToString(&sb);
							writer.WriteStrC(sb.ToString(), sb.GetLength());
							writer.WriteStrC(UTF8STRC("<br/>"));
							if (info.GetPhotoDate(&dt))
							{
								writer.WriteStrC(UTF8STRC("<b>"));
								writer.WriteStr(LangGetValue(lang, UTF8STRC("PhotoDate")));
								writer.WriteStrC(UTF8STRC("</b> "));
								sptr2 = dt.ToString(sbuff2, "yyyy-MM-dd HH:mm:ss zzzz");
								writer.WriteStrC(sbuff2, (UOSInt)(sptr2 - sbuff2));
								writer.WriteStrC(UTF8STRC("<br/>"));
							}
						}
					}

					sb.ClearStr();
					sb.AppendC(UTF8STRC("species.html?id="));
					sb.AppendI32(species->speciesId);
					sb.AppendC(UTF8STRC("&cateId="));
					sb.AppendI32(species->cateId);
					s = Text::XML::ToNewAttrText(sb.ToString());
					sb.ClearStr();
					sb.AppendC(UTF8STRC("<a href="));
					sb.Append(s);
					s->Release();
					sb.Append(LangGetValue(lang, UTF8STRC("Back")));
					sb.AppendC(UTF8STRC("</a>"));
					writer.WriteLineC(sb.ToString(), sb.GetLength());
					writer.WriteLineC(UTF8STRC("</td></tr>"));
					writer.WriteLineC(UTF8STRC("</table>"));
					writer.WriteLineC(UTF8STRC("</center>"));

					me->WriteFooter(&writer);
					me->dataMut.UnlockRead();
					ResponseMstm(req, resp, &mstm, CSTR("text/html"));

					LIST_FREE_STRING(&fileNameList);
					return true;
				}
				else
				{
					resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
					me->dataMut.UnlockRead();
					return true;
				}
			}
		}
		me->dataMut.UnlockRead();
	}
	resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcPhotoDetailD(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 fileId;
	UInt32 index;
	if (env.user != 0 &&
		req->GetQueryValueI32(CSTR("fileId"), &fileId) &&
		req->GetQueryValueU32(CSTR("index"), &index))
	{
		Text::String *s;

		UTF8Char sbuff[512];
		UTF8Char sbuff2[512];
		UTF8Char *sptr;
		UTF8Char *sptr2;
		Text::StringBuilderUTF8 sb;
		SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species;
		Data::DateTime dt;
		IO::ConfigFile *lang = me->LangGet(req);

		me->dataMut.LockRead();
		SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile = 0;
		if (env.user)
		{
			userFile = env.user->userFileObj.GetItem(index);
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
			IO::MemoryStream mstm;
			Text::UTF8Writer writer(&mstm);

			dt.SetTicks(userFile->captureTimeTicks);
			dt.ToString(sbuff, "yyyy-MM-dd");

			me->WriteHeader(&writer, sbuff, env.user, env.isMobile);
			writer.WriteStrC(UTF8STRC("<center><h1>"));
			s = Text::XML::ToNewHTMLBodyText(sbuff);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
			writer.WriteLineC(UTF8STRC("</h1></center>"));

			writer.WriteLineC(UTF8STRC("<center>"));
			writer.WriteLineC(UTF8STRC("<table border=\"0\" width=\"100%\">"));
			writer.WriteLineC(UTF8STRC("<tr><td align=\"center\">"));
			species = me->spMap.Get(userFile->speciesId);

			userFile2 = env.user->userFileObj.GetItem(index + 1);
			if (userFile2 && (userFile->captureTimeTicks / 86400000LL) == (userFile2->captureTimeTicks / 86400000LL))
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("photodetaild.html?fileId="));
				sb.AppendI32(userFile2->id);
				sb.AppendC(UTF8STRC("&index="));
				sb.AppendU32(index + 1);
			}
			else
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("photoday.html?d="));
				sb.AppendI64(userFile->captureTimeTicks / 86400000LL);
			}
			s = Text::XML::ToNewAttrText(sb.ToString());
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<a href="));
			sb.Append(s);
			s->Release();
			sb.AppendC(UTF8STRC(">"));
			writer.WriteLineC(sb.ToString(), sb.GetLength());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("photo.html?id="));
			sb.AppendI32(species->speciesId);
			sb.AppendC(UTF8STRC("&cateId="));
			sb.AppendI32(species->cateId);
			sb.AppendC(UTF8STRC("&width="));
			sb.AppendU32(env.scnWidth);
			sb.AppendC(UTF8STRC("&height="));
			sb.AppendU32(env.scnWidth);
			sb.AppendC(UTF8STRC("&fileId="));
			sb.AppendI32(fileId);
			s = Text::XML::ToNewAttrText(sb.ToString());
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<img src="));
			sb.Append(s);
			s->Release();
			sb.AppendC(UTF8STRC(" border=\"0\"></a>"));
			writer.WriteLineC(sb.ToString(), sb.GetLength());
			writer.WriteLineC(UTF8STRC("</td></tr>"));
			writer.WriteLineC(UTF8STRC("<tr><td align=\"center\">"));


			if (userFile->fileType == 3)
			{
				Data::DateTime dt;
				UInt64 fileSize = 0;
				Media::MediaFile *mediaFile;
				sptr = me->dataDir->ConcatTo(sbuff);
				if (sptr[-1] != IO::Path::PATH_SEPERATOR)
				{
					*sptr++ = IO::Path::PATH_SEPERATOR;
				}
				sptr = Text::StrConcatC(sptr, UTF8STRC("UserFile"));
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = Text::StrInt32(sptr, userFile->webuserId);
				*sptr++ = IO::Path::PATH_SEPERATOR;
				dt.SetTicks(userFile->fileTimeTicks);
				sptr = dt.ToString(sptr, "yyyyMM");
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = userFile->dataFileName->ConcatTo(sptr);
				{
					Sync::MutexUsage mutUsage(&me->parserMut);
					IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
					fileSize = fd.GetDataSize();
					mediaFile = (Media::MediaFile*)me->parsers.ParseFileType(&fd, IO::ParserType::MediaFile);
				}
				if (mediaFile)
				{
					sb.ClearStr();
					sb.AppendU64(fileSize);
					sb.AppendC(UTF8STRC(" bytes"));
					Media::IMediaSource *msrc = mediaFile->GetStream(0, 0);
					Int32 stmTime;
					if (msrc)
					{
						stmTime = msrc->GetStreamTime();
						sb.AppendC(UTF8STRC(", Length: "));
						sb.AppendI32(stmTime / 60000);
						sb.AppendC(UTF8STRC(":"));
						stmTime = stmTime % 60000;
						if (stmTime < 10000)
						{
							sb.AppendC(UTF8STRC("0"));
						}
						Text::SBAppendF64(&sb, stmTime * 0.001);

						if (msrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
						{
							Media::IAudioSource *asrc = (Media::IAudioSource*)msrc;
							Media::AudioFormat format;
							asrc->GetFormat(&format);
							sb.AppendC(UTF8STRC(" "));
							sb.AppendU32(format.frequency);
							sb.AppendC(UTF8STRC("Hz, "));
							sb.AppendU32(format.bitpersample);
							sb.AppendC(UTF8STRC("bits, "));
							sb.AppendU32(format.nChannels);
							sb.AppendC(UTF8STRC(" ch"));
						}
					}
					writer.WriteStrC(UTF8STRC("<b>"));
					writer.WriteStr(LangGetValue(lang, UTF8STRC("PhotoSpec")));
					writer.WriteStrC(UTF8STRC("</b> "));
					writer.WriteStrC(sb.ToString(), sb.GetLength());
					writer.WriteStrC(UTF8STRC("<br/>"));
					DEL_CLASS(mediaFile);
				}
				if (userFile->captureTimeTicks != 0)
				{
					dt.SetTicks(userFile->captureTimeTicks);
					dt.ToLocalTime();
					writer.WriteStrC(UTF8STRC("<b>"));
					writer.WriteStr(LangGetValue(lang, UTF8STRC("PhotoDate")));
					writer.WriteStrC(UTF8STRC("</b> "));
					sptr2 = dt.ToString(sbuff2, "yyyy-MM-dd HH:mm:ss zzzz");
					writer.WriteStrC(sbuff2, (UOSInt)(sptr2 - sbuff2));
					writer.WriteStrC(UTF8STRC("<br/>"));
				}
			}
			else
			{
				sptr = me->dataDir->ConcatTo(sbuff);
				if (sptr[-1] != IO::Path::PATH_SEPERATOR)
				{
					*sptr++ = IO::Path::PATH_SEPERATOR;
				}
				sptr = Text::StrConcatC(sptr, UTF8STRC("UserFile"));
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = Text::StrInt32(sptr, userFile->webuserId);
				*sptr++ = IO::Path::PATH_SEPERATOR;
				dt.SetTicks(userFile->fileTimeTicks);
				sptr = dt.ToString(sptr, "yyyyMM");
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = userFile->dataFileName->ConcatTo(sptr);

				IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
				Media::PhotoInfo info(&fd);
				if (info.HasInfo())
				{
					sb.ClearStr();
					writer.WriteStrC(UTF8STRC("<b>"));
					writer.WriteStr(LangGetValue(lang, UTF8STRC("PhotoSpec")));
					writer.WriteStrC(UTF8STRC("</b> "));
					info.ToString(&sb);
					writer.WriteStrC(sb.ToString(), sb.GetLength());
					writer.WriteStrC(UTF8STRC("<br/>"));

					dt.SetTicks(userFile->captureTimeTicks);
					dt.ToLocalTime();
					writer.WriteStrC(UTF8STRC("<b>"));
					writer.WriteStr(LangGetValue(lang, UTF8STRC("PhotoDate")));
					writer.WriteStrC(UTF8STRC("</b> "));
					sptr2 = dt.ToString(sbuff2, "yyyy-MM-dd HH:mm:ss zzzz");
					writer.WriteStrC(sbuff2, (UOSInt)(sptr2 - sbuff2));
					writer.WriteStrC(UTF8STRC("<br/>"));
				}
			}

			sb.ClearStr();
			sb.AppendC(UTF8STRC("photoday.html?d="));
			sb.AppendI64(userFile->captureTimeTicks / 86400000LL);
			s = Text::XML::ToNewAttrText(sb.ToString());
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<a href="));
			sb.Append(s);
			s->Release();
			sb.Append(LangGetValue(lang, UTF8STRC("Back")));
			sb.AppendC(UTF8STRC("</a>"));
			writer.WriteLineC(sb.ToString(), sb.GetLength());
			writer.WriteLineC(UTF8STRC("</td></tr>"));
			writer.WriteLineC(UTF8STRC("</table>"));
			writer.WriteLineC(UTF8STRC("</center>"));

			me->WriteFooter(&writer);
			me->dataMut.UnlockRead();
			ResponseMstm(req, resp, &mstm, CSTR("text/html"));
			return true;
		}
		else
		{
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			me->dataMut.UnlockRead();
			return true;
		}
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcPhotoYear(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	UInt16 y;
	if (env.user != 0 &&
		req->GetQueryValueU16(CSTR("y"), &y))
	{
		Data::DateTime dt;
		Data::DateTime dt2;
		UTF8Char sbuff[32];
		UTF8Char *sptr;

		IO::MemoryStream mstm;
		Text::UTF8Writer writer(&mstm);

		sptr = Text::StrUInt16(Text::StrConcatC(sbuff, UTF8STRC("Year ")), y);
		me->WriteHeader(&writer, sbuff, env.user, env.isMobile);
		writer.WriteStrC(UTF8STRC("<center><h1>"));
		writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer.WriteLineC(UTF8STRC("</h1></center>"));

		me->dataMut.LockRead();
		Int64 startTime;
		Int64 endTime;
		OSInt startIndex;
		OSInt endIndex;
		dt.ToUTCTime();
		dt.SetValue((UInt16)(y + 1), 1, 1, 0, 0, 0, 0);
		endTime = dt.ToTicks();
		dt.SetValue(y, 1, 1, 0, 0, 0, 0);
		startTime = dt.ToTicks();

		startIndex = env.user->userFileIndex.SortedIndexOf(startTime);
		if (startIndex < 0)
		{
			startIndex = ~startIndex;
		}
		else
		{
			while (startIndex > 0 && env.user->userFileIndex.GetItem((UOSInt)startIndex - 1) == startTime)
			{
				startIndex--;
			}
		}
		endIndex = env.user->userFileIndex.SortedIndexOf(endTime);
		if (endIndex < 0)
		{
			endIndex = ~endIndex;
		}
		else
		{
			while (endIndex > 0 && env.user->userFileIndex.GetItem((UOSInt)endIndex - 1) == endTime)
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
		writer.WriteLineC(UTF8STRC("<table border=\"0\" width=\"100%\">"));

		while (startIndex < endIndex)
		{
			dt.SetTicks(env.user->userFileIndex.GetItem((UOSInt)startIndex));
			if (dt.GetMonth() != month || dt.GetDay() != day)
			{
				if (month != 0 && day != 0)
				{
					userFile = env.user->userFileObj.GetItem((UOSInt)(dayStartIndex + startIndex) >> 1);
					sp = me->spMap.Get(userFile->speciesId);

					if (currColumn == 0)
					{
						writer.WriteLineC(UTF8STRC("<tr>"));
					}

					sb.ClearStr();
					sb.AppendC(UTF8STRC("<td width=\""));
					sb.AppendU32(colWidth);
					sb.AppendC(UTF8STRC("%\"><center>"));
					writer.WriteStrC(sb.ToString(), sb.GetLength());

					writer.WriteStrC(UTF8STRC("<a href=\"photoday.html?d="));
					dt2.SetValue(y, month, day, 0, 0, 0, 0);
					sptr = Text::StrInt64(sbuff, dt2.ToTicks() / 86400000LL);
					writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer.WriteStrC(UTF8STRC("\">"));
					writer.WriteStrC(UTF8STRC("<img src=\"photo.html?id="));
					sptr = Text::StrInt32(sbuff, userFile->speciesId);
					writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer.WriteStrC(UTF8STRC("&amp;cateId="));
					sptr = Text::StrInt32(sbuff, sp->cateId);
					writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer.WriteStrC(UTF8STRC("&amp;width="));
					sptr = Text::StrInt32(sbuff, PREVIEW_SIZE);
					writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer.WriteStrC(UTF8STRC("&amp;height="));
					sptr = Text::StrInt32(sbuff, PREVIEW_SIZE);
					writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer.WriteStrC(UTF8STRC("&amp;fileId="));
					sptr = Text::StrInt32(sbuff, userFile->id);
					writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer.WriteStrC(UTF8STRC("\" border=\"0\" ALT=\""));
					sptr = Text::StrUInt32(Text::StrConcatC(Text::StrUInt32(Text::StrConcatC(Text::StrInt32(sbuff, y), UTF8STRC("-")), month), UTF8STRC("-")), day);
					writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer.WriteStrC(UTF8STRC("\" title=\""));
					writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer.WriteStrC(UTF8STRC("\" /></a><br/>"));
					writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					i = 0;
					j = locList.GetCount();
					while (i < j)
					{
						Text::String *s = Text::XML::ToNewHTMLBodyText(locList.GetItem(i));
						writer.WriteStrC(UTF8STRC(" "));
						writer.WriteStrC(s->v, s->leng);
						s->Release();
						i++;
					}
					writer.WriteStrC(UTF8STRC(" ("));
					sptr = Text::StrOSInt(sbuff, (startIndex - dayStartIndex));
					writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer.WriteLineC(UTF8STRC(")</center></td>"));

					currColumn++;
					if (currColumn >= colCount)
					{
						writer.WriteLineC(UTF8STRC("</tr>"));
						currColumn = 0;
					}
				}

				if (month != dt.GetMonth())
				{
					month = dt.GetMonth();
					if (currColumn != 0)
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("<td width=\""));
						sb.AppendU32(colWidth);
						sb.AppendC(UTF8STRC("%\"></td>"));
						while (currColumn < colCount)
						{
							writer.WriteLineC(sb.ToString(), sb.GetLength());
							currColumn++;
						}
						writer.WriteLineC(UTF8STRC("</tr>"));
					}
					writer.WriteLineC(UTF8STRC("</table><hr/>"));
					writer.WriteStrC(UTF8STRC("<h2>"));
					sptr = Text::StrUInt32(Text::StrConcatC(Text::StrInt32(sbuff, y), UTF8STRC("-")), month);
					writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
					writer.WriteLineC(UTF8STRC("</h2>"));
					writer.WriteLineC(UTF8STRC("<table border=\"0\" width=\"100%\">"));
					currColumn = 0;
				}
				day = dt.GetDay();
				dayStartIndex = startIndex;
				locList.Clear();
			}

			const UTF8Char *locName = (const UTF8Char*)"?";
			userFile = env.user->userFileObj.GetItem((UOSInt)startIndex);
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
			userFile = env.user->userFileObj.GetItem((UOSInt)(dayStartIndex + startIndex) >> 1);
			sp = me->spMap.Get(userFile->speciesId);
			if (currColumn == 0)
			{
				writer.WriteLineC(UTF8STRC("<tr>"));
			}

			sb.ClearStr();
			sb.AppendC(UTF8STRC("<td width=\""));
			sb.AppendU32(colWidth);
			sb.AppendC(UTF8STRC("%\"><center>"));
			writer.WriteStrC(sb.ToString(), sb.GetLength());

			writer.WriteStrC(UTF8STRC("<a href=\"photoday.html?d="));
			dt2.SetValue(y, month, day, 0, 0, 0, 0);
			sptr = Text::StrInt64(sbuff, dt2.ToTicks() / 86400000LL);
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteStrC(UTF8STRC("\">"));
			writer.WriteStrC(UTF8STRC("<img src=\"photo.html?id="));
			sptr = Text::StrInt32(sbuff, userFile->speciesId);
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteStrC(UTF8STRC("&amp;cateId="));
			sptr = Text::StrInt32(sbuff, sp->cateId);
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteStrC(UTF8STRC("&amp;width="));
			sptr = Text::StrInt32(sbuff, PREVIEW_SIZE);
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteStrC(UTF8STRC("&amp;height="));
			sptr = Text::StrInt32(sbuff, PREVIEW_SIZE);
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteStrC(UTF8STRC("&amp;fileId="));
			sptr = Text::StrInt32(sbuff, userFile->id);
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteStrC(UTF8STRC("\" border=\"0\" ALT=\""));
			sptr = Text::StrUInt32(Text::StrConcatC(Text::StrUInt32(Text::StrConcatC(Text::StrInt32(sbuff, y), UTF8STRC("-")), month), UTF8STRC("-")), day);
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteStrC(UTF8STRC("\" title=\""));
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteStrC(UTF8STRC("\" /></a><br/>"));
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			i = 0;
			j = locList.GetCount();
			while (i < j)
			{
				Text::String *s = Text::XML::ToNewHTMLBodyText(locList.GetItem(i));
				writer.WriteStrC(UTF8STRC(" "));
				writer.WriteStrC(s->v, s->leng);
				s->Release();
				i++;
			}
			writer.WriteStrC(UTF8STRC(" ("));
			sptr = Text::StrOSInt(sbuff, (startIndex - dayStartIndex));
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteLineC(UTF8STRC(")</center></td>"));

			currColumn++;
			if (currColumn >= colCount)
			{
				writer.WriteLineC(UTF8STRC("</tr>"));
				currColumn = 0;
			}
		}
		if (currColumn != 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<td width=\""));
			sb.AppendU32(colWidth);
			sb.AppendC(UTF8STRC("%\"></td>"));
			while (currColumn < colCount)
			{
				writer.WriteLineC(sb.ToString(), sb.GetLength());
				currColumn++;
			}
			writer.WriteLineC(UTF8STRC("</tr>"));
		}
		writer.WriteLineC(UTF8STRC("</table><hr/>"));
		writer.WriteLineC(UTF8STRC("<a href=\"/\">Index</a><br/>"));
		me->dataMut.UnlockRead();

		me->WriteFooter(&writer);
		ResponseMstm(req, resp, &mstm, CSTR("text/html"));
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcPhotoDay(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 d;
	if (env.user != 0 &&
		req->GetQueryValueI32(CSTR("d"), &d))
	{
		Data::DateTime dt;
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		Text::String *s;

		IO::MemoryStream mstm;
		Text::UTF8Writer writer(&mstm);

		dt.SetTicks(d * 86400000LL);
		sptr = dt.ToString(sbuff, "yyyy-MM-dd");
		me->WriteHeader(&writer, sbuff, env.user, env.isMobile);
		writer.WriteStrC(UTF8STRC("<center><h1>"));
		writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer.WriteLineC(UTF8STRC("</h1></center>"));

		me->dataMut.LockRead();
		Int64 startTime;
		Int64 endTime;
		OSInt startIndex;
		OSInt endIndex;
		Text::StringBuilderUTF8 sb;
		startTime = d * 86400000LL;
		endTime = startTime + 86400000LL;

		startIndex = env.user->userFileIndex.SortedIndexOf(startTime);
		if (startIndex < 0)
		{
			startIndex = ~startIndex;
		}
		else
		{
			while (startIndex > 0 && env.user->userFileIndex.GetItem((UOSInt)startIndex - 1) == startTime)
			{
				startIndex--;
			}
		}
		endIndex = env.user->userFileIndex.SortedIndexOf(endTime);
		if (endIndex < 0)
		{
			endIndex = ~endIndex;
		}
		else
		{
			while (endIndex > 0 && env.user->userFileIndex.GetItem((UOSInt)endIndex - 1) == endTime)
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
		writer.WriteLineC(UTF8STRC("<table border=\"0\" width=\"100%\">"));
			
		while (startIndex < endIndex)
		{
			userFile = env.user->userFileObj.GetItem((UOSInt)startIndex);
			sp = me->spMap.Get(userFile->speciesId);
			if (currColumn == 0)
			{
				writer.WriteLineC(UTF8STRC("<tr>"));
			}

			sb.ClearStr();
			sb.AppendC(UTF8STRC("<td width=\""));
			sb.AppendU32(colWidth);
			sb.AppendC(UTF8STRC("%\">"));
			writer.WriteLineC(sb.ToString(), sb.GetLength());
			sb.ClearStr();
			sb.AppendC(UTF8STRC("photodetaild.html?fileId="));
			sb.AppendI32(userFile->id);
			sb.AppendC(UTF8STRC("&index="));
			sb.AppendOSInt(startIndex);
			s = Text::XML::ToNewAttrText(sb.ToString());
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<center><a href="));
			sb.Append(s);
			sb.AppendC(UTF8STRC(">"));
			writer.WriteLineC(sb.ToString(), sb.GetLength());
			s->Release();

			writer.WriteStrC(UTF8STRC("<img src="));
			sb.ClearStr();
			sb.AppendC(UTF8STRC("photo.html?id="));
			sb.AppendI32(sp->speciesId);
			sb.AppendC(UTF8STRC("&cateId="));
			sb.AppendI32(sp->cateId);
			sb.AppendC(UTF8STRC("&width="));
			sb.AppendI32(PREVIEW_SIZE);
			sb.AppendC(UTF8STRC("&height="));
			sb.AppendI32(PREVIEW_SIZE);
			sb.AppendC(UTF8STRC("&fileId="));
			sb.AppendI32(userFile->id);
			s = Text::XML::ToNewAttrText(sb.ToString());
			writer.WriteStrC(s->v, s->leng);
			s->Release();
			writer.WriteStrC(UTF8STRC(" border=\"0\">"));
			writer.WriteStrC(UTF8STRC("</a>"));

			dt.SetTicks(userFile->captureTimeTicks);
			sptr = dt.ToString(sbuff, "HH:mm:ss");
			writer.WriteStrC(UTF8STRC("<br/>"));
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));

			if (userFile->location)
			{
				writer.WriteStrC(UTF8STRC(" "));
				s = Text::XML::ToNewHTMLBodyText(userFile->location->v);
				writer.WriteStrC(s->v, s->leng);
				s->Release();
			}
			writer.WriteStrC(UTF8STRC("<br/>"));
			writer.WriteStr(userFile->oriFileName->ToCString());

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
						writer.WriteStrC(UTF8STRC(" "));
						txt = Text::XML::ToNewHTMLText(loc->cname);
						writer.Write(txt);
						Text::XML::FreeNewText(txt);
					}
				}
			}*/

			if (userFile->descript)
			{
				writer.WriteStrC(UTF8STRC("<br/>"));
				s = Text::XML::ToNewHTMLBodyText(userFile->descript->v);
				writer.WriteStrC(s->v, s->leng);
				s->Release();
			}
			if (userFile->lat != 0 || userFile->lon != 0)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<br/>"));
				sb.AppendC(UTF8STRC("<a href=\"https://www.google.com/maps/place/"));
				Text::SBAppendF64(&sb, userFile->lat);
				sb.AppendC(UTF8STRC(","));
				Text::SBAppendF64(&sb, userFile->lon);
				sb.AppendC(UTF8STRC("/@"));
				Text::SBAppendF64(&sb, userFile->lat);
				sb.AppendC(UTF8STRC(","));
				Text::SBAppendF64(&sb, userFile->lon);
				sb.AppendC(UTF8STRC(",19z\">"));
				sb.AppendC(UTF8STRC("Google Map</a>"));
				writer.WriteStrC(sb.ToString(), sb.GetLength());
			}
			writer.WriteLineC(UTF8STRC("</center></td>"));

			currColumn++;
			if (currColumn >= colCount)
			{
				writer.WriteLineC(UTF8STRC("</tr>"));
				currColumn = 0;
			}
			startIndex++;
		}

		if (currColumn != 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<td width=\""));
			sb.AppendU32(colWidth);
			sb.AppendC(UTF8STRC("%\"></td>"));
			while (currColumn < colCount)
			{
				writer.WriteLineC(sb.ToString(), sb.GetLength());
				currColumn++;
			}
			writer.WriteLineC(UTF8STRC("</tr>"));
		}
		writer.WriteLineC(UTF8STRC("</table><hr/>"));
		sptr = Text::StrUInt32(sbuff, dt.GetYear());
		writer.WriteStrC(UTF8STRC("<a href=\"photoyear.html?y="));
		writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer.WriteStrC(UTF8STRC("\">Year "));
		writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer.WriteStrC(UTF8STRC("</a><br/>"));
		writer.WriteLineC(UTF8STRC("<a href=\"/\">Index</a><br/>"));
		me->dataMut.UnlockRead();

		me->WriteFooter(&writer);
		ResponseMstm(req, resp, &mstm, CSTR("text/html"));
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcPhotoUpload(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
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
	UTF8Char *fileNameEnd;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	const UInt8 *fileCont;
	Text::String *s;
	req->ParseHTTPForm();

	IO::MemoryStream mstm;
	Text::UTF8Writer writer(&mstm);

	me->WriteHeader(&writer, (const UTF8Char*)"Photo Upload", env.user, env.isMobile);
	writer.WriteLineC(UTF8STRC("<table border=\"1\">"));
	writer.WriteLineC(UTF8STRC("<tr><td>File Name</td><td>File Size</td><td>Image Size</td></tr>"));
	while (true)
	{
		fileCont = req->GetHTTPFormFile(CSTR("file"), i, fileName, sizeof(fileName), &fileNameEnd, &fileSize);
		if (fileCont == 0)
		{
			break;
		}
		writer.WriteStrC(UTF8STRC("<tr><td>"));
		s = Text::XML::ToNewHTMLBodyText(fileName);
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteStrC(UTF8STRC("</td><td>"));
		sptr = Text::StrUOSInt(sbuff, fileSize);
		writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer.WriteStrC(UTF8STRC("</td><td>"));
		me->dataMut.LockWrite();
		Int32 ret = me->UserfileAdd(env.user->id, env.user->unorganSpId, CSTRP(fileName, fileNameEnd), fileCont, fileSize, true);
		me->dataMut.UnlockWrite();
		if (ret == 0)
		{
			writer.WriteStrC(UTF8STRC("Failed"));
		}
		else
		{
			writer.WriteStrC(UTF8STRC("Success"));
		}
/*
		Media::ImageList *imgList;
		{
			IO::StmData::MemoryDataRef fd(fileCont, fileSize);
			me->parserMut->Lock();
			imgList = (Media::ImageList*)me->parsers->ParseFileType(&fd, IO::ParserType::ImageList);
			me->parserMut->Unlock();
		}
		if (imgList)
		{
			Int32 imgDelay;
			Media::Image *img = imgList->GetImage(0, &imgDelay);
			if (img)
			{
				Text::StrUOSInt(Text::StrConcatC(Text::StrUOSInt(sbuff, img->info.dispWidth), UTF8STRC(" x ")), img->info.dispHeight);
				writer.Write(sbuff);
			}
			else
			{
				writer.WriteStrC(UTF8STRC("-"));
			}
			DEL_CLASS(imgList);
		}
		else
		{
			writer.WriteStrC(UTF8STRC("-"));
		}*/
		writer.WriteLineC(UTF8STRC("</td></tr>"));
		i++;
	}
	writer.WriteLineC(UTF8STRC("</table>"));
	me->WriteFooter(&writer);

	ResponseMstm(req, resp, &mstm, CSTR("text/html"));
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcPhotoUpload2(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
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
	UTF8Char *fileNameEnd;
	const UInt8 *fileCont;
	Bool succ = true;
	req->ParseHTTPForm();

	while (true)
	{
		fileCont = req->GetHTTPFormFile(CSTR("file"), i, fileName, sizeof(fileName), &fileNameEnd, &fileSize);
		if (fileCont == 0)
		{
			break;
		}
		me->dataMut.LockWrite();
		if (!me->UserfileAdd(env.user->id, env.user->unorganSpId, CSTRP(fileName, fileNameEnd), fileCont, fileSize, true))
			succ = false;
		me->dataMut.UnlockWrite();

		i++;
	}

	IO::MemoryStream mstm;
	if (succ)
	{
		mstm.Write((const UInt8*)"ok", 2);
	}
	else
	{
		mstm.Write((const UInt8*)"fail", 4);
	}
	ResponseMstm(req, resp, &mstm, CSTR("text/plain"));
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcPhotoUploadD(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
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
	if (!req->GetHeaderC(&sb, CSTR("X-FileName")))
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

	me->dataMut.LockWrite();
	Int32 ret = me->UserfileAdd(env.user->id, env.user->unorganSpId, sb.ToCString(), imgData, dataSize, true);
	me->dataMut.UnlockWrite();

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
	resp->AddContentType(CSTR("text/plain"));
	resp->Write(sbuff, (UOSInt)(sptr - sbuff));
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcSearchInside(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 id;
	Int32 cateId;
	Text::String *searchStr;
	req->ParseHTTPForm();
	if (req->GetQueryValueI32(CSTR("id"), &id) &&
		req->GetQueryValueI32(CSTR("cateId"), &cateId) &&
		(searchStr = req->GetHTTPFormStr(CSTR("searchStr"))) != 0)
	{
		Text::String *s;
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
		SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		IO::ConfigFile *lang = me->LangGet(req);
		me->dataMut.LockRead();
		group = me->groupMap.Get(id);
		Bool notAdmin = (env.user == 0 || env.user->userType != 0);
		if (group == 0 || group->cateId != cateId || (me->GroupIsAdmin(group) && notAdmin))
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->cateMap.Get(group->cateId);
		if (cate == 0 || ((cate->flags & 1) && notAdmin))
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		IO::MemoryStream mstm;
		Text::UTF8Writer writer(&mstm);

		sb.ClearStr();
		sb.Append(cate->chiName);
		sb.AppendC(UTF8STRC(" - "));
		sb.Append(group->chiName);
		sb.AppendC(UTF8STRC(" "));
		sb.Append(group->engName);
		me->WriteHeader(&writer, sb.ToString(), env.user, env.isMobile);
		writer.WriteStrC(UTF8STRC("<center><h1>"));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("</h1></center>"));

		writer.WriteLineC(UTF8STRC("<table border=\"0\" width=\"100%\">"));
		writer.WriteLineC(UTF8STRC("<tr>"));
		writer.WriteStrC(UTF8STRC("<td><form method=\"POST\" action=\"searchinside.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&cateId="));
		sb.AppendI32(group->cateId);
		writer.WriteStrC(sb.ToString(), sb.GetLength());
		writer.WriteStrC(UTF8STRC("\">"));
		writer.WriteStrC(UTF8STRC("Search inside: <input type=\"text\" name=\"searchStr\"/><input type=\"submit\"/>"));
		writer.WriteLineC(UTF8STRC("</form></td>"));
		writer.WriteLineC(UTF8STRC("</tr>"));
		writer.WriteLineC(UTF8STRC("</table>"));

		me->WriteLocator(&writer, group, cate);
		writer.WriteLineC(UTF8STRC("<br/>"));
		if (group->descript)
		{
			s = Text::XML::ToNewHTMLBodyText(group->descript->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
		}
		writer.WriteLineC(UTF8STRC("<br/>"));
		writer.WriteLineC(UTF8STRC("<hr/>"));

		Data::ArrayListDbl speciesIndice;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> speciesObjs;
		Data::ArrayListDbl groupIndice;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupInfo*> groupObjs;
		sb.ClearStr();
		sb.Append(searchStr);
		sb.Trim();
		writer.WriteStrC(UTF8STRC("Result for \""));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("\"<br/>"));
		me->SearchInGroup(group, sb.ToString(), sb.GetLength(), &speciesIndice, &speciesObjs, &groupIndice, &groupObjs, env.user);

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
			me->WriteSpeciesTable(&writer, &speciesList, env.scnWidth, group->cateId, false);
			if (j > 0)
			{
				sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, searchStr->v);
				writer.WriteStrC(UTF8STRC("<a href="));
				sb.ClearStr();
				sb.AppendC(UTF8STRC("searchinsidemores.html?id="));
				sb.AppendI32(group->id);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(group->cateId);
				sb.AppendC(UTF8STRC("&pageNo=1"));
				sb.AppendC(UTF8STRC("&searchStr="));
				sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer.WriteStrC(s->v, s->leng);
				s->Release();
				writer.WriteStrC(UTF8STRC(">More</a><br/>"));
			}
			writer.WriteLineC(UTF8STRC("<hr/>"));
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
			me->WriteGroupTable(&writer, &groupList, env.scnWidth, false);
			if (j > 0)
			{
				sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, searchStr->v);
				writer.WriteStrC(UTF8STRC("<a href="));
				sb.ClearStr();
				sb.AppendC(UTF8STRC("searchinsidemoreg.html?id="));
				sb.AppendI32(group->id);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(group->cateId);
				sb.AppendC(UTF8STRC("&searchStr="));
				sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer.WriteStrC(s->v, s->leng);
				s->Release();
				writer.WriteStrC(UTF8STRC(">More</a><br/>"));
			}
			writer.WriteLineC(UTF8STRC("<hr/>"));
			found = true;
		}
		if (!found)
		{
			writer.WriteLineC(UTF8STRC("No object found<br/>"));
		}
		writer.WriteLineC(UTF8STRC("<br/>"));
		writer.WriteStrC(UTF8STRC("<a href=\"group.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&cateId="));
		sb.AppendI32(group->cateId);
		writer.WriteStrC(sb.ToString(), sb.GetLength());
		writer.WriteStrC(UTF8STRC("\">"));
		writer.WriteStr(LangGetValue(lang, UTF8STRC("Back")));
		writer.WriteStrC(UTF8STRC("</a>"));

		me->WriteFooter(&writer);
		me->dataMut.UnlockRead();
		ResponseMstm(req, resp, &mstm, CSTR("text/html"));
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcSearchInsideMoreS(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 id;
	UInt32 pageNo;
	Int32 cateId;
	Text::String *searchStr;
	if (req->GetQueryValueI32(CSTR("id"), &id) &&
		req->GetQueryValueI32(CSTR("cateId"), &cateId) &&
		req->GetQueryValueU32(CSTR("pageNo"), &pageNo) &&
		(searchStr = req->GetQueryValue(CSTR("searchStr"))) != 0)
	{
		Text::String *s;
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
		SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		IO::ConfigFile *lang = me->LangGet(req);
		me->dataMut.LockRead();
		group = me->groupMap.Get(id);
		Bool notAdmin = (env.user == 0 || env.user->userType != 0);
		if (group == 0 || group->cateId != cateId || (me->GroupIsAdmin(group) && notAdmin))
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->cateMap.Get(group->cateId);
		if (cate == 0 || ((cate->flags & 1) && notAdmin))
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		IO::MemoryStream mstm;
		Text::UTF8Writer writer(&mstm);

		sb.ClearStr();
		sb.Append(cate->chiName);
		sb.AppendC(UTF8STRC(" - "));
		sb.Append(group->chiName);
		sb.AppendC(UTF8STRC(" "));
		sb.Append(group->engName);
		me->WriteHeader(&writer, sb.ToString(), env.user, env.isMobile);
		writer.WriteStrC(UTF8STRC("<center><h1>"));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("</h1></center>"));

		writer.WriteLineC(UTF8STRC("<table border=\"0\" width=\"100%\">"));
		writer.WriteLineC(UTF8STRC("<tr>"));
		writer.WriteStrC(UTF8STRC("<td><form method=\"POST\" action=\"searchinside.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&cateId="));
		sb.AppendI32(group->cateId);
		writer.WriteStrC(sb.ToString(), sb.GetLength());
		writer.WriteStrC(UTF8STRC("\">"));
		writer.WriteStrC(UTF8STRC("Search inside: <input type=\"text\" name=\"searchStr\"/><input type=\"submit\"/>"));
		writer.WriteLineC(UTF8STRC("</form></td>"));
		writer.WriteLineC(UTF8STRC("</tr>"));
		writer.WriteLineC(UTF8STRC("</table>"));

		me->WriteLocator(&writer, group, cate);
		writer.WriteLineC(UTF8STRC("<br/>"));
		if (group->descript)
		{
			s = Text::XML::ToNewHTMLBodyText(group->descript->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
		}
		writer.WriteLineC(UTF8STRC("<br/>"));
		writer.WriteLineC(UTF8STRC("<hr/>"));

		Data::ArrayListDbl speciesIndice;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> speciesObjs;
		Data::ArrayListDbl groupIndice;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupInfo*> groupObjs;
		sb.ClearStr();
		sb.Append(searchStr);
		sb.Trim();
		writer.WriteStrC(UTF8STRC("Result for \""));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("\"<br/>"));
		me->SearchInGroup(group, sb.ToString(), sb.GetLength(), &speciesIndice, &speciesObjs, &groupIndice, &groupObjs, env.user);

		Bool found = false;

		if (speciesObjs.GetCount() > pageNo * 50)
		{
			Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> speciesList;
			j = speciesObjs.GetCount() - pageNo * 50;
			i = j - 50;
			if (j < 50)
			{
				i = 0;
			}
			while (j-- > i)
			{
				speciesList.Add(speciesObjs.GetItem(j));
			}
			me->WriteSpeciesTable(&writer, &speciesList, env.scnWidth, group->cateId, false);
			if (pageNo > 0)
			{
				sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, searchStr->v);
				writer.WriteStrC(UTF8STRC("<a href="));
				sb.ClearStr();
				sb.AppendC(UTF8STRC("searchinsidemores.html?id="));
				sb.AppendI32(group->id);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(group->cateId);
				sb.AppendC(UTF8STRC("&pageNo="));
				sb.AppendU32(pageNo - 1);
				sb.AppendC(UTF8STRC("&searchStr="));
				sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer.WriteStrC(s->v, s->leng);
				s->Release();
				writer.WriteStrC(UTF8STRC(">Prev</a> "));
			}
			if (i > 0)
			{
				sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, searchStr->v);
				writer.WriteStrC(UTF8STRC(" <a href="));
				sb.ClearStr();
				sb.AppendC(UTF8STRC("searchinsidemores.html?id="));
				sb.AppendI32(group->id);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(group->cateId);
				sb.AppendC(UTF8STRC("&pageNo="));
				sb.AppendU32(pageNo + 1);
				sb.AppendC(UTF8STRC("&searchStr="));
				sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer.WriteStrC(s->v, s->leng);
				s->Release();
				writer.WriteStrC(UTF8STRC(">Next</a>"));
			}
			writer.WriteLineC(UTF8STRC("<br/><hr/>"));
			found = true;
		}
		if (!found)
		{
			writer.WriteLineC(UTF8STRC("No object found<br/>"));
		}
		writer.WriteLineC(UTF8STRC("<br/>"));
		writer.WriteStrC(UTF8STRC("<a href=\"group.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&cateId="));
		sb.AppendI32(group->cateId);
		writer.WriteStrC(sb.ToString(), sb.GetLength());
		writer.WriteStrC(UTF8STRC("\">"));
		writer.WriteStr(LangGetValue(lang, UTF8STRC("Back")));
		writer.WriteStrC(UTF8STRC("</a>"));

		me->WriteFooter(&writer);
		me->dataMut.UnlockRead();
		ResponseMstm(req, resp, &mstm, CSTR("text/html"));
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcSearchInsideMoreG(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 id;
	UInt32 pageNo;
	Int32 cateId;
	Text::String *searchStr;
	if (req->GetQueryValueI32(CSTR("id"), &id) &&
		req->GetQueryValueI32(CSTR("cateId"), &cateId) &&
		req->GetQueryValueU32(CSTR("pageNo"), &pageNo) &&
		(searchStr = req->GetQueryValue(CSTR("searchStr"))) != 0)
	{
		Text::String *s;
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
		SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		IO::ConfigFile *lang = me->LangGet(req);
		me->dataMut.LockRead();
		group = me->groupMap.Get(id);
		Bool notAdmin = (env.user == 0 || env.user->userType != 0);
		if (group == 0 || group->cateId != cateId || (me->GroupIsAdmin(group) && notAdmin))
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->cateMap.Get(group->cateId);
		if (cate == 0 || ((cate->flags & 1) && notAdmin))
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		IO::MemoryStream mstm;
		Text::UTF8Writer writer(&mstm);

		sb.ClearStr();
		sb.Append(cate->chiName);
		sb.AppendC(UTF8STRC(" - "));
		sb.Append(group->chiName);
		sb.AppendC(UTF8STRC(" "));
		sb.Append(group->engName);
		me->WriteHeader(&writer, sb.ToString(), env.user, env.isMobile);
		writer.WriteStrC(UTF8STRC("<center><h1>"));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("</h1></center>"));

		writer.WriteLineC(UTF8STRC("<table border=\"0\" width=\"100%\">"));
		writer.WriteLineC(UTF8STRC("<tr>"));
		writer.WriteStrC(UTF8STRC("<td><form method=\"POST\" action=\"searchinside.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&cateId="));
		sb.AppendI32(group->cateId);
		writer.WriteStrC(sb.ToString(), sb.GetLength());
		writer.WriteStrC(UTF8STRC("\">"));
		writer.WriteStrC(UTF8STRC("Search inside: <input type=\"text\" name=\"searchStr\"/><input type=\"submit\"/>"));
		writer.WriteLineC(UTF8STRC("</form></td>"));
		writer.WriteLineC(UTF8STRC("</tr>"));
		writer.WriteLineC(UTF8STRC("</table>"));

		me->WriteLocator(&writer, group, cate);
		writer.WriteLineC(UTF8STRC("<br/>"));
		if (group->descript)
		{
			s = Text::XML::ToNewHTMLBodyText(group->descript->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
		}
		writer.WriteLineC(UTF8STRC("<br/>"));
		writer.WriteLineC(UTF8STRC("<hr/>"));

		Data::ArrayListDbl speciesIndice;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> speciesObjs;
		Data::ArrayListDbl groupIndice;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupInfo*> groupObjs;
		sb.ClearStr();
		sb.Append(searchStr);
		sb.Trim();
		writer.WriteStrC(UTF8STRC("Result for \""));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("\"<br/>"));
		me->SearchInGroup(group, sb.ToString(), sb.GetLength(), &speciesIndice, &speciesObjs, &groupIndice, &groupObjs, env.user);

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
			me->WriteGroupTable(&writer, &groupList, env.scnWidth, false);
			if (pageNo > 0)
			{
				sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, STR_PTR(searchStr));
				writer.WriteStrC(UTF8STRC("<a href="));
				sb.ClearStr();
				sb.AppendC(UTF8STRC("searchinsidemoreg.html?id="));
				sb.AppendI32(group->id);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(group->cateId);
				sb.AppendC(UTF8STRC("&pageNo="));
				sb.AppendU32(pageNo - 1);
				sb.AppendC(UTF8STRC("&searchStr="));
				sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer.WriteStrC(s->v, s->leng);
				s->Release();
				writer.WriteStrC(UTF8STRC(">Prev</a> "));
			}
			if (i > 0)
			{
				sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, STR_PTR(searchStr));
				writer.WriteStrC(UTF8STRC(" <a href="));
				sb.ClearStr();
				sb.AppendC(UTF8STRC("searchinsidemoreg.html?id="));
				sb.AppendI32(group->id);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(group->cateId);
				sb.AppendC(UTF8STRC("&pageNo="));
				sb.AppendU32(pageNo + 1);
				sb.AppendC(UTF8STRC("&searchStr="));
				sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer.WriteStrC(s->v, s->leng);
				s->Release();
				writer.WriteStrC(UTF8STRC(">Next</a>"));
			}
			writer.WriteLineC(UTF8STRC("<hr/>"));
			found = true;
		}
		if (!found)
		{
			writer.WriteLineC(UTF8STRC("No object found<br/>"));
		}
		writer.WriteLineC(UTF8STRC("<br/>"));
		writer.WriteStrC(UTF8STRC("<a href=\"group.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&cateId="));
		sb.AppendI32(group->cateId);
		writer.WriteStrC(sb.ToString(), sb.GetLength());
		writer.WriteStrC(UTF8STRC("\">"));
		writer.WriteStr(LangGetValue(lang, UTF8STRC("Back")));
		writer.WriteStrC(UTF8STRC("</a>"));

		me->WriteFooter(&writer);
		me->dataMut.UnlockRead();
		ResponseMstm(req, resp, &mstm, CSTR("text/html"));
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcBookList(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 id;
	if (req->GetQueryValueI32(CSTR("id"), &id))
	{
		Text::String *s;
		Data::FastMap<Int64, BookInfo*> sortBookMap;
		Data::DateTime dt;
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		SSWR::OrganMgr::OrganWebHandler::BookInfo *book;
		SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		IO::ConfigFile *lang = me->LangGet(req);
		me->dataMut.LockRead();
		cate = me->cateMap.Get(id);
		if (cate == 0)
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		IO::MemoryStream mstm;
		Text::UTF8Writer writer(&mstm);

		sb.ClearStr();
		sb.Append(cate->chiName);
		sb.AppendC(UTF8STRC(" - "));
		sb.AppendC(UTF8STRC("Book List"));
		me->WriteHeader(&writer, sb.ToString(), env.user, env.isMobile);
		writer.WriteStrC(UTF8STRC("<center><h1>"));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("</h1></center>"));

		writer.WriteLineC(UTF8STRC("<table border=\"0\" width=\"100%\">"));
		writer.WriteLineC(UTF8STRC("<tr>"));
		writer.WriteLineC(UTF8STRC("<td>Book Name</td>"));
		writer.WriteLineC(UTF8STRC("<td>Author</td>"));
		writer.WriteLineC(UTF8STRC("<td>Press</td>"));
		writer.WriteLineC(UTF8STRC("<td>Publish Date</td>"));
		writer.WriteLineC(UTF8STRC("</tr>"));

		i = 0;
		j = me->bookMap.GetCount();
		while (i < j)
		{
			book = me->bookMap.GetItem(i);
			sortBookMap.Put(book->publishDate, book);
			i++;
		}

		i = 0;
		j = sortBookMap.GetCount();
		while (i < j)
		{
			book = sortBookMap.GetItem(i);

			writer.WriteLineC(UTF8STRC("<tr>"));
			writer.WriteStrC(UTF8STRC("<td><a href=\"book.html?id="));
			sptr = Text::StrInt32(sbuff, book->id);
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteStrC(UTF8STRC("&amp;cateId="));
			sptr = Text::StrInt32(sbuff, cate->cateId);
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteStrC(UTF8STRC("\">"));
			sb.ClearStr();
			s = Text::XML::ToNewHTMLBodyText(book->title->v);
			sb.Append(s);
			sb.ReplaceStr(UTF8STRC("[i]"), UTF8STRC("<i>"));
			sb.ReplaceStr(UTF8STRC("[/i]"), UTF8STRC("</i>"));
			writer.WriteStrC(sb.ToString(), sb.GetLength());
			s->Release();
			writer.WriteLineC(UTF8STRC("</a></td>"));
			writer.WriteStrC(UTF8STRC("<td>"));
			s = Text::XML::ToNewHTMLBodyText(book->author->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
			writer.WriteLineC(UTF8STRC("</td>"));
			writer.WriteStrC(UTF8STRC("<td>"));
			s = Text::XML::ToNewHTMLBodyText(book->press->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
			writer.WriteLineC(UTF8STRC("</td>"));
			writer.WriteStrC(UTF8STRC("<td>"));
			dt.SetTicks(book->publishDate);
			sptr = dt.ToString(sbuff, "yyyy-MM-dd");
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteLineC(UTF8STRC("</td>"));
			writer.WriteLineC(UTF8STRC("</tr>"));

			i++;
		}


		writer.WriteLineC(UTF8STRC("</table>"));
		writer.WriteLineC(UTF8STRC("<br/>"));
		writer.WriteStrC(UTF8STRC("<a href="));
		sb.ClearStr();
		sb.AppendC(UTF8STRC("cate.html?cateName="));
		sb.Append(cate->dirName);
		s = Text::XML::ToNewAttrText(sb.ToString());
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteStrC(UTF8STRC(">"));
		writer.WriteStr(LangGetValue(lang, UTF8STRC("Back")));
		writer.WriteStrC(UTF8STRC("</a>"));

		me->WriteFooter(&writer);
		me->dataMut.UnlockRead();
		ResponseMstm(req, resp, &mstm, CSTR("text/html"));
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcBook(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 id;
	UInt32 pageNo = 0;
	Int32 cateId;
	req->GetQueryValueU32(CSTR("page"), &pageNo);
	if (req->GetQueryValueI32(CSTR("id"), &id) &&
		req->GetQueryValueI32(CSTR("cateId"), &cateId))
	{
		Text::String *s;
		Data::FastMap<Int64, BookInfo*> sortBookMap;
		Data::DateTime dt;
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		BookInfo *book;
		SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
		BookSpInfo *bookSp;
		SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species;
		Data::FastStringMap<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> speciesMap;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> tempList;
		UOSInt i;
		UOSInt j;
		IO::ConfigFile *lang = me->LangGet(req);
		Text::StringBuilderUTF8 sb;
		me->dataMut.LockRead();
		cate = me->cateMap.Get(cateId);
		if (cate == 0)
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		book = me->bookMap.Get(id);
		if (book == 0)
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		IO::MemoryStream mstm;
		Text::UTF8Writer writer(&mstm);

		sb.ClearStr();
		sb.Append(cate->chiName);
		me->WriteHeader(&writer, sb.ToString(), env.user, env.isMobile);
		writer.WriteStrC(UTF8STRC("<center><h1>"));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("</h1></center>"));

		writer.WriteStrC(UTF8STRC("<b>Book Name:</b> "));
		s = Text::XML::ToNewHTMLBodyText(book->title->v);
		sb.ClearStr();
		sb.Append(s);
		s->Release();
		sb.ReplaceStr(UTF8STRC("[i]"), UTF8STRC("<i>"));
		sb.ReplaceStr(UTF8STRC("[/i]"), UTF8STRC("</i>"));
		writer.WriteStrC(sb.ToString(), sb.GetLength());
		writer.WriteLineC(UTF8STRC("<br/>"));

		writer.WriteStrC(UTF8STRC("<b>Author:</b> "));
		s = Text::XML::ToNewHTMLBodyText(book->author->v);
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("<br/>"));

		writer.WriteStrC(UTF8STRC("<b>Press:</b> "));
		s = Text::XML::ToNewHTMLBodyText(book->press->v);
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("<br/>"));

		if (book->url)
		{
			writer.WriteStrC(UTF8STRC("<b>URL:</b> <a href="));
			s = Text::XML::ToNewAttrText(book->url->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
			writer.WriteStrC(UTF8STRC(">"));
			s = Text::XML::ToNewHTMLBodyText(book->url->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
			writer.WriteLineC(UTF8STRC("</a><br/>"));
		}

		if (env.user && env.user->userType == 0)
		{
			if (me->BookFileExist(book))
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<b>View:</b> <a href=\"bookview.html?id="));
				sb.AppendI32(id);
				sb.AppendC(UTF8STRC("\">here</a><br/>"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());
			}
		}

		i = 0;
		j = book->species.GetCount();
		while (i < j)
		{
			bookSp = book->species.GetItem(i);
			species = me->spMap.Get(bookSp->speciesId);
			if (species)
			{
				speciesMap.Put(species->sciName, species);
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
		writer.WriteLineC(UTF8STRC("<br/>"));
		if (pageNo > 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<a href=\"book.html?id="));
			sb.AppendI32(id);
			sb.AppendC(UTF8STRC("&amp;cateId="));
			sb.AppendI32(cateId);
			sb.AppendC(UTF8STRC("&amp;page="));
			sb.AppendU32(pageNo - 1);
			sb.AppendC(UTF8STRC("\">&lt;"));
			sb.Append(LangGetValue(lang, UTF8STRC("Previous")));
			sb.AppendU32(perPage);
			sb.Append(LangGetValue(lang, UTF8STRC("Items")));
			sb.AppendC(UTF8STRC("</a>"));
			writer.WriteLineC(sb.ToString(), sb.GetLength());
		}
		if ((pageNo + 1) * perPage < speciesMap.GetCount())
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<a href=\"book.html?id="));
			sb.AppendI32(id);
			sb.AppendC(UTF8STRC("&amp;cateId="));
			sb.AppendI32(cateId);
			sb.AppendC(UTF8STRC("&amp;page="));
			sb.AppendU32(pageNo + 1);
			sb.AppendC(UTF8STRC("\">"));
			sb.Append(LangGetValue(lang, UTF8STRC("Next")));
			sb.AppendU32(perPage);
			sb.Append(LangGetValue(lang, UTF8STRC("Items")));
			sb.AppendC(UTF8STRC("&gt;</a>"));
			writer.WriteLineC(sb.ToString(), sb.GetLength());
		}
		i = pageNo * perPage;
		j = i + perPage;
		if (j > speciesMap.GetCount())
		{
			j = speciesMap.GetCount();
		}
		while (i < j)
		{
			tempList.Add(speciesMap.GetItem(i));
			i++;
		}
		writer.WriteLineC(UTF8STRC("<hr/>"));
		me->WriteSpeciesTable(&writer, &tempList, env.scnWidth, cateId, false);
		writer.WriteLineC(UTF8STRC("<hr/>"));

		writer.WriteStrC(UTF8STRC("<a href=\"booklist.html?id="));
		sptr = Text::StrInt32(sbuff, cate->cateId);
		writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer.WriteStrC(UTF8STRC("\">"));
		writer.WriteStrC(UTF8STRC("Book List</a>"));

		me->WriteFooter(&writer);
		me->dataMut.UnlockRead();
		ResponseMstm(req, resp, &mstm, CSTR("text/html"));
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcBookView(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 id;
	if (req->GetQueryValueI32(CSTR("id"), &id))
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		BookInfo *book;
		me->dataMut.LockRead();
		book = me->bookMap.Get(id);
		if (env.user == 0 || env.user->userType != 0)
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_UNAUTHORIZED);
			return true;
		}
		else if (book == 0)
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		sptr = me->dataDir->ConcatTo(sbuff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC("BookFile"));
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrInt32(sptr, book->id);
		sptr = Text::StrConcatC(sptr, UTF8STRC(".pdf"));
		IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		UInt64 fileLen = fs.GetLength();
		if (fileLen <= 16)
		{
			me->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;

		}

		me->dataMut.UnlockRead();

		resp->AddDefHeaders(req);
		resp->AddContentType(CSTR("application/pdf"));
		resp->AddContentLength(fileLen);
		UOSInt readSize;
		UInt64 sizeLeft = fileLen;
		while (sizeLeft > 0)
		{
			readSize = fs.Read(sbuff, 512);
			if (readSize <= 0)
			{
				break;
			}

			resp->Write(sbuff, readSize);
			sizeLeft -= readSize;
		}
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcLogin(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Data::DateTime dt;
	UTF8Char sbuff[128];
	UTF8Char *sptr;

	if (env.user || !req->IsSecure())
	{
		resp->RedirectURL(req, CSTR("/"), 0);
		return true;
	}
	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		Text::String *userName = req->GetHTTPFormStr(CSTR("userName"));
		Text::String *pwd = req->GetHTTPFormStr(CSTR("password"));
		if (userName && pwd)
		{
			sptr = me->PasswordEnc(sbuff, pwd->ToCString());
			me->dataMut.LockRead();
			env.user = me->userNameMap.Get(userName);
			if (env.user && env.user->pwd->Equals(sbuff, (UOSInt)(sptr - sbuff)))
			{
				me->dataMut.UnlockRead();
				Net::WebServer::IWebSession *sess = me->sessMgr->CreateSession(req, resp);
				Data::DateTime *t;
				Data::ArrayListInt32 *pickObjs;
				NEW_CLASS(t, Data::DateTime());
				NEW_CLASS(pickObjs, Data::ArrayListInt32());
				sess->SetValuePtr(UTF8STRC("LastUseTime"), t);
				sess->SetValuePtr(UTF8STRC("User"), env.user);
				sess->SetValuePtr(UTF8STRC("PickObjs"), pickObjs);
				sess->SetValueInt32(UTF8STRC("PickObjType"), 0);
				sess->EndUse();

				IO::MemoryStream mstm;
				Text::UTF8Writer writer(&mstm);

				me->dataMut.LockRead();
				me->WriteHeaderPart1(&writer, (const UTF8Char*)"Index", env.isMobile);
				writer.WriteLineC(UTF8STRC("<script type=\"text/javascript\">"));
				writer.WriteLineC(UTF8STRC("function afterLoad()"));
				writer.WriteLineC(UTF8STRC("{"));
				writer.WriteLineC(UTF8STRC("	document.location.replace('/');"));
				writer.WriteLineC(UTF8STRC("}"));
				writer.WriteLineC(UTF8STRC("</script>"));
				me->WriteHeaderPart2(&writer, 0, (const UTF8Char*)"afterLoad()");
				writer.WriteLineC(UTF8STRC("Login succeeded"));
				me->dataMut.UnlockRead();

				me->WriteFooter(&writer);
				ResponseMstm(req, resp, &mstm, CSTR("text/html"));
				return true;
			}
			env.user = 0;
			me->dataMut.UnlockRead();
		}
	}

	IO::MemoryStream mstm;
	Text::UTF8Writer writer(&mstm);

	me->dataMut.LockRead();
	me->WriteHeader(&writer, (const UTF8Char*)"Index", env.user, env.isMobile);
	writer.WriteLineC(UTF8STRC("<center><h1>Login</h1></center>"));

	writer.WriteLineC(UTF8STRC("<form method=\"POST\" action=\"login.html\">"));
	writer.WriteLineC(UTF8STRC("User Name: <input type=\"text\" name=\"userName\"/><br/>"));
	writer.WriteLineC(UTF8STRC("Password: <input type=\"password\" name=\"password\"/><br/>"));
	writer.WriteLineC(UTF8STRC("<input type=\"submit\" /><br/>"));
	writer.WriteLineC(UTF8STRC("</form>"));
	me->dataMut.UnlockRead();

	me->WriteFooter(&writer);
	ResponseMstm(req, resp, &mstm, CSTR("text/html"));
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcLogout(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	me->sessMgr->DeleteSession(req, resp);
	resp->RedirectURL(req, CSTR("/"), 0);
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcReload(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	if (me->reloadPwd)
	{
		Data::DateTime dt;

		IO::MemoryStream mstm;
		Text::UTF8Writer writer(&mstm);

		me->WriteHeader(&writer, (const UTF8Char*)"Reload", env.user, env.isMobile);
		writer.WriteLineC(UTF8STRC("<center><h1>Reload</h1></center>"));

		Bool showPwd = true;;
		if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
		{
			Text::String *pwd;
			req->ParseHTTPForm();
			pwd = req->GetHTTPFormStr(CSTR("pwd"));
			if (pwd)
			{
				if (me->reloadPwd->Equals(pwd))
				{
					writer.WriteLineC(UTF8STRC("Reloaded<br/>"));
					showPwd = false;
					me->Reload();
				}
				else
				{
					writer.WriteLineC(UTF8STRC("Password Error<br/>"));
				}
			}
		}
		if (showPwd)
		{
			writer.WriteLineC(UTF8STRC("<form name=\"pwd\" method=\"POST\" action=\"reload\">"));
			writer.WriteLineC(UTF8STRC("Reload Password:"));
			writer.WriteLineC(UTF8STRC("<input name=\"pwd\" type=\"password\" /><br/>"));
			writer.WriteLineC(UTF8STRC("<input type=\"submit\" />"));
			writer.WriteLineC(UTF8STRC("</form>"));
		}

		me->WriteFooter(&writer);
		ResponseMstm(req, resp, &mstm, CSTR("text/html"));
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}
}


Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcRestart(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	if (me->reloadPwd)
	{
		Data::DateTime dt;
		IO::MemoryStream mstm;
		Text::UTF8Writer writer(&mstm);

		me->WriteHeader(&writer, (const UTF8Char*)"Restart", env.user, env.isMobile);
		writer.WriteLineC(UTF8STRC("<center><h1>Restart</h1></center>"));

		Bool showPwd = true;;
		if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
		{
			Text::String *pwd;
			req->ParseHTTPForm();
			pwd = req->GetHTTPFormStr(CSTR("pwd"));
			if (pwd)
			{
				if (me->reloadPwd->Equals(pwd))
				{
					writer.WriteLineC(UTF8STRC("Restarting<br/>"));
					showPwd = false;
					me->Restart();
				}
				else
				{
					writer.WriteLineC(UTF8STRC("Password Error<br/>"));
				}
			}
		}
		if (showPwd)
		{
			writer.WriteLineC(UTF8STRC("<form name=\"pwd\" method=\"POST\" action=\"restart\">"));
			writer.WriteLineC(UTF8STRC("Restart Password:"));
			writer.WriteLineC(UTF8STRC("<input name=\"pwd\" type=\"password\" /><br/>"));
			writer.WriteLineC(UTF8STRC("<input type=\"submit\" />"));
			writer.WriteLineC(UTF8STRC("</form>"));
		}

		me->WriteFooter(&writer);
		ResponseMstm(req, resp, &mstm, CSTR("text/html"));
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcIndex(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Data::DateTime dt;
	UTF8Char sbuff[32];
	UTF8Char *sptr;

	IO::MemoryStream mstm;
	Text::UTF8Writer writer(&mstm);

	me->WriteHeader(&writer, (const UTF8Char*)"Index", env.user, env.isMobile);
	writer.WriteLineC(UTF8STRC("<script type=\"application/javascript\">\r\n"
								"async function submitFile() {\r\n"
								"\tdocument.getElementById(\"uploadStatus\").disabled = true;\r\n"
								"\tvar url = \"photoupload2.html\";\r\n"
								"\tvar fileupload = document.getElementById(\"file\");\r\n"
								"\tvar uploadStatus = document.getElementById(\"uploadStatus\");\r\n"
								"\tvar failList = new Array();\r\n"
								"\tvar failFiles = new Array();\r\n"
								"\tvar statusText;\r\n"
								"\tvar i = 0;\r\n"
								"\tvar j = fileupload.files.length;\r\n"
								"\twhile (i < j) {\r\n"
								"\t\tstatusText = \"Uploading \"+(i + 1)+\" of \"+j;\r\n"
								"\t\tif (failList.length > 0) statusText = statusText+\"<br/>Failed Files:<br/>\"+failList.join(\"<br/>\");\r\n"
								"\t\tuploadStatus.innerHTML = statusText;\r\n"
								"\t\tvar formData = new FormData();\r\n"
								"\t\tformData.append(\"file\", fileupload.files[i]);\r\n"
								"\t\tconst resp = await fetch(url, {\r\n"
								"\t\t\tmethod: \"POST\", \r\n"
								"\t\t\tbody: formData\r\n"
								"\t\t});\r\n"
								"\t\tconst respText = await resp.text();\r\n"
								"\t\tif (respText != \"ok\") {\r\n"
								"\t\t\tfailList.push(fileupload.files[i].name);\r\n"
								"\t\t\tfailFiles.push(fileupload.files[i]);\r\n"
								"\t\t}\r\n"
								"\t\ti++;\r\n"
								"\t}\r\n"
								"\tif (failList.length > 0) {\r\n"
								"\t\tstatusText = \"Failed Files:<br/>\"+failList.join(\"<br/>\");\r\n"
								"\t\tfileupload.files = failFiles;\r\n"
								"\t} else {\r\n"
								"\t\tstatusText = \"Upload Success\";\r\n"
								"\t\tfileupload.value = null;\r\n"
								"\t}\r\n"
								"\tuploadStatus.innerHTML = statusText;\r\n"
								"\tdocument.getElementById(\"uploadStatus\").disabled = false;\r\n"
								"}\r\n"
								"</script>"));
	writer.WriteLineC(UTF8STRC("<center><h1>Index</h1></center>"));

	me->dataMut.LockRead();
	SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
	SSWR::OrganMgr::OrganWebHandler::CategoryInfo *firstCate = 0;
	UOSInt i;
	UOSInt j;
	Text::String *s;
	Text::StringBuilderUTF8 sb;
	Bool notAdmin = (env.user == 0 || env.user->userType != 0);
	i = 0;
	j = me->cateMap.GetCount();
	while (i < j)
	{
		cate = me->cateMap.GetItem(i);
		if ((cate->flags & 1) == 0 || !notAdmin)
		{
			writer.WriteStrC(UTF8STRC("<a href="));
			sb.ClearStr();
			sb.AppendC(UTF8STRC("cate.html?cateName="));
			sb.Append(cate->dirName);
			s = Text::XML::ToNewAttrText(sb.ToString());
			writer.WriteStrC(s->v, s->leng);
			writer.WriteStrC(UTF8STRC(">"));
			s->Release();
			s = Text::XML::ToNewHTMLBodyText(cate->chiName->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
			writer.WriteLineC(UTF8STRC("</a><br/>"));

			if (firstCate == 0)
				firstCate = cate;
		}
		i++;
	}
	if (env.user)
	{
		OSInt endIndex = (OSInt)env.user->userFileIndex.GetCount();
		OSInt startIndex;
		Int64 currTime = env.user->userFileIndex.GetItem((UOSInt)endIndex - 1);
		Int64 thisTicks;
		if (endIndex > 0)
		{
			writer.WriteLineC(UTF8STRC("<hr/>"));
			dt.ToUTCTime();
			while (true)
			{
				dt.SetTicks(currTime);
				dt.SetValue(dt.GetYear(), 1, 1, 0, 0, 0, 0);
				thisTicks = dt.ToTicks();
				startIndex = env.user->userFileIndex.SortedIndexOf(thisTicks);
				if (startIndex < 0)
				{
					startIndex = ~startIndex;
				}
				else
				{
					while (startIndex > 0 && env.user->userFileIndex.GetItem((UOSInt)startIndex - 1) == thisTicks)
					{
						startIndex--;
					}
				}
//				printf("Index startIndex = %d, endIndex = %d, currTime = %ld, year = %d\r\n", startIndex, endIndex, currTime, dt.GetYear());
				sptr = Text::StrUInt32(sbuff, dt.GetYear());
				writer.WriteStrC(UTF8STRC("<a href=\"photoyear.html?y="));
				writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
				writer.WriteStrC(UTF8STRC("\">Year "));
				writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
				writer.WriteLineC(UTF8STRC("</a><br/>"));
				if (startIndex <= 0)
					break;
				endIndex = startIndex;
				currTime = env.user->userFileIndex.GetItem((UOSInt)endIndex - 1);
			}
		}
		writer.WriteLineC(UTF8STRC("<hr/>"));
		writer.WriteLineC(UTF8STRC("<h3>Photo Upload</h3>"));
		writer.WriteLineC(UTF8STRC("<form name=\"upload\" method=\"POST\" action=\"photoupload.html\" enctype=\"multipart/form-data\">Files:<input type=\"file\" id=\"file\" name=\"file\" multiple/>"));
		writer.WriteLineC(UTF8STRC("<input type=\"button\" id=\"uploadButton\" value=\"Upload\" onclick=\"submitFile()\"/></form>"));
		writer.WriteLineC(UTF8STRC("<div id=\"uploadStatus\"></div>"));
	}
	writer.WriteLineC(UTF8STRC("<hr/>"));
	writer.WriteStrC(UTF8STRC("<a href=\"booklist.html?id="));
	sb.ClearStr();
	sb.AppendI32(firstCate->cateId);
	writer.WriteStrC(sb.ToString(), sb.GetLength());
	writer.WriteLineC(UTF8STRC("\">Book List</a><br/>"));
	writer.WriteStrC(UTF8STRC("<a href=\"map/index.html\">ShowMap</a><br/>"));
	writer.WriteStrC(UTF8STRC("<a href=\"map/index2d.html\">ShowMap2D</a><br/>"));
	if (env.user == 0 && req->IsSecure())
	{
		writer.WriteStrC(UTF8STRC("<a href=\"login.html\">Login</a><br/>"));
	}
	me->dataMut.UnlockRead();

	me->WriteFooter(&writer);
	ResponseMstm(req, resp, &mstm, CSTR("text/html"));
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcCate(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
	Text::String *cateName = req->GetQueryValue(CSTR("cateName"));
	if (cateName != 0 && (cate = me->cateSMap.Get(cateName)) != 0)
	{
		Text::String *s;
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupInfo*> groups;
		IO::ConfigFile *lang = me->LangGet(req);

		IO::MemoryStream mstm;
		Text::UTF8Writer writer(&mstm);

		me->WriteHeader(&writer, cate->chiName->v, env.user, env.isMobile);
		writer.WriteStrC(UTF8STRC("<center><h1>"));
		s = Text::XML::ToNewHTMLBodyText(cate->chiName->v);
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("</h1></center>"));

		Bool notAdmin = (env.user == 0 || env.user->userType != 0);
		i = 0;
		j = cate->groups.GetCount();
		while (i < j)
		{
			group = cate->groups.GetItem(i);
			me->CalcGroupCount(group);
			if (group->totalCount > 0 && ((group->flags & 1) == 0 || !notAdmin))
			{
	/*			sb.ClearStr();
				sb.AppendC(UTF8STRC("<a href=\"group.html?id="));
				sb.AppendI32(group->id);
				sb.AppendC(UTF8STRC("&amp;cateId="));
				sb.AppendI32(group->cateId);
				sb.AppendC(UTF8STRC("\">"));
				txt = Text::XML::ToNewHTMLText(group->chiName);
				sb.Append(txt);
				sb.AppendC(UTF8STRC(" "));
				Text::XML::FreeNewText(txt);
				txt = Text::XML::ToNewHTMLText(group->engName);
				sb.Append(txt);
				Text::XML::FreeNewText(txt);
				sb.AppendC(UTF8STRC(" ("));
				sb.AppendOSInt(group->myPhotoCount);
				sb.AppendC(UTF8STRC("/"));
				sb.AppendOSInt(group->photoCount);
				sb.AppendC(UTF8STRC("/"));
				sb.AppendOSInt(group->totalCount);
				sb.AppendC(UTF8STRC(")</a><br/>"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());*/
				groups.Add(group);
			}
			i++;
		}

	/*	writer.WriteLineC(UTF8STRC("<br/>"));
		writer.WriteStrC(UTF8STRC("<a href=\"booklist.html?id="));
		sb.ClearStr();
		sb.AppendI32(cate->cateId);
		writer.WriteStrC(sb.ToString(), sb.GetLength());
		writer.WriteLineC(UTF8STRC("\">Book List</a>"));*/
		me->WriteLocator(&writer, 0, 0);
		writer.WriteLineC(UTF8STRC("<hr/>"));
		me->WriteGroupTable(&writer, &groups, env.scnWidth, false);
		writer.WriteLineC(UTF8STRC("<hr/>"));
		writer.WriteStrC(UTF8STRC("<a href=\"/\">"));
		writer.WriteStr(LangGetValue(lang, UTF8STRC("Back")));
		writer.WriteLineC(UTF8STRC("</a>"));
		me->WriteFooter(&writer);
		ResponseMstm(req, resp, &mstm, CSTR("text/html"));
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcFavicon(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcPublicPOI(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	me->dataMut.LockRead();
	GroupInfo *poiGroup = me->groupMap.Get(21593);
	Text::StringBuilderUTF8 sb;
	sb.AppendUTF8Char('[');
	me->AddGroupPOI(&sb, poiGroup, 0);
	me->dataMut.UnlockRead();
	if (sb.GetLength() > 1)
	{
		sb.RemoveChars(3);
	}
	sb.AppendUTF8Char(']');
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("application/json"));
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToString(), sb.GetLength());
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcGroupPOI(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);
	Text::StringBuilderUTF8 sb;
	sb.AppendUTF8Char('[');
	Int32 groupId;
	if (req->GetQueryValueI32(CSTR("id"), &groupId))
	{
		me->dataMut.LockRead();
		GroupInfo *poiGroup = me->groupMap.Get(groupId);
		if (poiGroup)
		{
			if (env.user != 0)
				me->AddGroupPOI(&sb, poiGroup, env.user->id);
			else
				me->AddGroupPOI(&sb, poiGroup, 0);
			if (sb.GetLength() > 1)
			{
				sb.RemoveChars(3);
			}
		}
		me->dataMut.UnlockRead();
	}
	sb.AppendUTF8Char(']');
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("application/json"));
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToString(), sb.GetLength());
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcSpeciesPOI(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	SSWR::OrganMgr::OrganWebHandler::RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);
	Text::StringBuilderUTF8 sb;
	sb.AppendUTF8Char('[');
	Int32 speciesId;
	if (req->GetQueryValueI32(CSTR("id"), &speciesId))
	{
		me->dataMut.LockRead();
		SpeciesInfo *poiSpecies = me->spMap.Get(speciesId);
		if (poiSpecies)
		{
			if (env.user != 0)
				me->AddSpeciesPOI(&sb, poiSpecies, env.user->id, me->GroupIsPublic(poiSpecies->groupId));
			else
				me->AddSpeciesPOI(&sb, poiSpecies, 0, me->GroupIsPublic(poiSpecies->groupId));
			if (sb.GetLength() > 1)
			{
				sb.RemoveChars(3);
			}
		}
		me->dataMut.UnlockRead();
	}
	sb.AppendUTF8Char(']');
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("application/json"));
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToString(), sb.GetLength());
	return true;
}

void SSWR::OrganMgr::OrganWebHandler::AddGroupPOI(Text::StringBuilderUTF8 *sb, GroupInfo *group, Int32 userId)
{
	UOSInt i = 0;
	UOSInt j = group->groups.GetCount();
	while (i < j)
	{
		AddGroupPOI(sb, group->groups.GetItem(i), userId);
		i++;
	}
	i = 0;
	j = group->species.GetCount();
	while (i < j)
	{
		AddSpeciesPOI(sb, group->species.GetItem(i), userId, this->GroupIsPublic(group->id));
		i++;
	}
}

void SSWR::OrganMgr::OrganWebHandler::AddSpeciesPOI(Text::StringBuilderUTF8 *sb, SpeciesInfo *species, Int32 userId, Bool publicGroup)
{
	Data::DateTime dt;
	UserFileInfo *file;
	UOSInt k;
	UOSInt l;
	WebUserInfo *user = 0;
	Bool adminUser = false;
	if (userId != 0)
	{
		user = this->userMap.Get(userId);
		if (user && user->userType == 0)
		{
			adminUser = true;
		}
	}
	k = 0;
	l = species->files.GetCount();
	while (k < l)
	{
		file = species->files.GetItem(k);
		if ((file->lat != 0 || file->lon != 0) && (publicGroup || adminUser || file->webuserId == userId))
		{
			sb->AppendUTF8Char('{');
			sb->AppendC(UTF8STRC("\"id\":\""));
			sb->AppendI32(file->id);
			sb->AppendC(UTF8STRC("\",\"name\":"));
			if (file->descript && file->descript->leng > 0)
			{
				Text::JSText::ToJSTextDQuote(sb, file->descript->v);
			}
			else
			{
				Text::JSText::ToJSTextDQuote(sb, species->sciName->v);
			}
			sb->AppendC(UTF8STRC(",\"description\":\"<img src=\\\"/photo.html?id="));
			sb->AppendI32(species->speciesId);
			sb->AppendC(UTF8STRC("&cateId="));
			sb->AppendI32(species->cateId);
			sb->AppendC(UTF8STRC("&width="));
			sb->AppendI32(PREVIEW_SIZE);
			sb->AppendC(UTF8STRC("&height="));
			sb->AppendI32(PREVIEW_SIZE);
			sb->AppendC(UTF8STRC("&fileId="));
			sb->AppendI32(file->id);
			sb->AppendC(UTF8STRC("\\\" /><br/>"));
			dt.SetTicks(file->captureTimeTicks);
			dt.SetTimeZoneQHR(32);
			sb->AppendDate(&dt);
			sb->AppendC(UTF8STRC("\",\"lat\":"));
			sb->AppendDouble(file->lat);
			sb->AppendC(UTF8STRC(",\"lon\":"));
			sb->AppendDouble(file->lon);
			sb->AppendC(UTF8STRC(",\"imgUrl\":\"/photo.html?id="));
			sb->AppendI32(species->speciesId);
			sb->AppendC(UTF8STRC("&cateId="));
			sb->AppendI32(species->cateId);
			sb->AppendC(UTF8STRC("&width="));
			sb->AppendI32(PREVIEW_SIZE);
			sb->AppendC(UTF8STRC("&height="));
			sb->AppendI32(PREVIEW_SIZE);
			sb->AppendC(UTF8STRC("&fileId="));
			sb->AppendI32(file->id);
			sb->AppendC(UTF8STRC("\",\"poiUrl\":\"img/"));
			if (species->poiImg)
			{
				sb->Append(species->poiImg);
			}
			else
			{
				sb->AppendC(UTF8STRC("poi.png"));
			}
			sb->AppendC(UTF8STRC("\"},\r\n"));
		}
		k++;
	}
}

void SSWR::OrganMgr::OrganWebHandler::ResponsePhoto(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, SSWR::OrganMgr::OrganWebHandler::WebUserInfo *user, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, const UTF8Char *fileName)
{
	SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr2;
	Int32 rotateType = 0;
	UTF8Char *sptr;
	UTF8Char *sptrEnd = sbuff;
	this->dataMut.LockRead();
	sp = this->spMap.Get(speciesId);
	Bool notAdmin = (user == 0 || user->userType != 0);
	if (sp && sp->cateId == cateId)
	{
		cate = this->cateMap.Get(sp->cateId);
		if (cate && ((cate->flags & 1) == 0 || !notAdmin))
		{
			Text::StringBuilderUTF8 sb;
			if (this->cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE)
			{
				sptr = this->cacheDir->ConcatTo(sbuff);
				sptr2 = Text::StrInt32(sbuff2, cate->cateId);
				sptr = IO::Path::AppendPath(sbuff, sptr, CSTRP(sbuff2, sptr2));
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = sp->dirName->ConcatTo(sptr);
				IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
				*sptr++ = IO::Path::PATH_SEPERATOR;
				if (Text::StrStartsWith(fileName, (const UTF8Char*)"web") && (fileName[3] == IO::Path::PATH_SEPERATOR || fileName[3] == '\\'))
				{
					sptr = Text::StrConcatC(sptr, UTF8STRC("web"));
					IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
					*sptr++ = IO::Path::PATH_SEPERATOR;
					sptrEnd = Text::StrConcatC(Text::StrConcat(sptr, &fileName[4]), UTF8STRC(".jpg"));
				}
				else
				{
					sptrEnd = Text::StrConcatC(Text::StrConcat(sptr, fileName), UTF8STRC(".jpg"));
				}

				IO::FileStream fs({sbuff, (UOSInt)(sptrEnd - sbuff)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				if (fs.IsError())
				{
				}
				else
				{
					UInt8 *buff;
					UOSInt buffSize = (UOSInt)fs.GetLength();
					if (buffSize > 0)
					{
						buff = MemAlloc(UInt8, buffSize);
						fs.Read(buff, buffSize);
						resp->AddDefHeaders(req);
						resp->AddContentLength(buffSize);
						resp->AddContentType(CSTR("image/jpeg"));
						resp->Write(buff, buffSize);
						this->dataMut.UnlockRead();
						MemFree(buff);
						return;
					}
					else
					{
					}
				}
			}

			{
				sb.ClearStr();
				sb.Append(cate->srcDir);
				sb.Append(sp->dirName);
				sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
				sb.AppendC(UTF8STRC("setting.txt"));
				IO::FileStream fs(sb.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
				if (!fs.IsError())
				{
					Text::PString sarr[3];
					sb.ClearStr();
					sb.AppendSlow(fileName);
					sb.AppendC(UTF8STRC("."));

					Text::UTF8Reader reader(&fs);
					while ((sptr2 = reader.ReadLine(sbuff2, 511)) != 0)
					{
						if (Text::StrSplitP(sarr, 3, {sbuff2, (UOSInt)(sptr2 - sbuff2)}, '\t') == 2)
						{
							if (Text::StrStartsWithICaseC(sarr[0].v, sarr[0].leng, sb.ToString(), sb.GetLength()))
							{
								if (sarr[1].v[0] == 'R')
								{
									rotateType = Text::StrToInt32(&sarr[1].v[1]);
									break;
								}
							}
						}
					}
				}
			}

			sb.ClearStr();
			sb.Append(cate->srcDir);
			sb.Append(sp->dirName);
			sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
			sb.AppendSlow(fileName);
			if (IO::Path::PATH_SEPERATOR == '/')
			{
				sb.Replace('\\', '/');
			}
			sb.AppendC(UTF8STRC(".jpg"));
			this->dataMut.UnlockRead();
			if (IO::Path::GetPathType(sb.ToCString()) != IO::Path::PathType::File)
			{
				sb.RemoveChars(4);
				sb.AppendC(UTF8STRC(".pcx"));
				if (IO::Path::GetPathType(sb.ToCString()) != IO::Path::PathType::File)
				{
					sb.RemoveChars(4);
					sb.AppendC(UTF8STRC(".tif"));
					if (IO::Path::GetPathType(sb.ToCString()) != IO::Path::PathType::File)
					{
						sb.RemoveChars(4);
						sb.AppendC(UTF8STRC(".png"));
					}
				}
			}

			Media::ImageList *imgList;
			Media::StaticImage *simg;
			Media::StaticImage *lrimg;
			Media::StaticImage *dimg;
			Sync::MutexUsage mutUsage(&this->parserMut);
			{
				IO::StmData::FileData fd(sb.ToCString(), false);
				imgList = (Media::ImageList*)this->parsers.ParseFileType(&fd, IO::ParserType::ImageList);
			}
			mutUsage.EndUse();
			if (imgList)
			{
				simg = imgList->GetImage(0, 0)->CreateStaticImage();
				DEL_CLASS(imgList);
				Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
				NEW_CLASS(lrimg, Media::StaticImage(simg->info.dispWidth, simg->info.dispHeight, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, 0, &color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
				Sync::MutexUsage mutUsage(&this->csconvMut);
				if (this->csconv == 0 || this->csconvFCC != simg->info.fourcc || this->csconvBpp != simg->info.storeBPP || this->csconvPF != simg->info.pf || !simg->info.color->Equals(&this->csconvColor))
				{
					SDEL_CLASS(this->csconv);
					this->csconvFCC = simg->info.fourcc;
					this->csconvBpp = simg->info.storeBPP;
					this->csconvPF = simg->info.pf;
					this->csconvColor.Set(simg->info.color);
					this->csconv = Media::CS::CSConverter::NewConverter(this->csconvFCC, this->csconvBpp, this->csconvPF, &this->csconvColor, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, &color, Media::ColorProfile::YUVT_UNKNOWN, this->colorSess);
				}
				if (this->csconv)
				{
					this->csconv->ConvertV2(&simg->data, lrimg->data, simg->info.dispWidth, simg->info.dispHeight, simg->info.storeWidth, simg->info.storeHeight, (OSInt)lrimg->GetDataBpl(), Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);
				}
				else
				{
					SDEL_CLASS(lrimg);
				}
				mutUsage.EndUse();
				DEL_CLASS(simg);

				if (lrimg)
				{
					this->lrgbLimiter.LimitImageLRGB(lrimg->data, lrimg->info.dispWidth, lrimg->info.dispHeight);
					Sync::MutexUsage mutUsage(&this->resizerMut);
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
					UInt8 *buff;
					UOSInt buffSize;
					dimg->info.color->SetRAWICC(Media::ICCProfile::GetSRGBICCData());
					if (rotateType == 1)
					{
						dimg->RotateImage(Media::StaticImage::RotateType::CW90);
					}
					else if (rotateType == 2)
					{
						dimg->RotateImage(Media::StaticImage::RotateType::CW180);
					}
					else if (rotateType == 3)
					{
						dimg->RotateImage(Media::StaticImage::RotateType::CW270);
					}

					if (false)//this->watermark && !Text::StrStartsWith(fileName, (const UTF8Char*)"web") && fileName[3] != IO::Path::PATH_SEPERATOR && fileName[3] != '\\')
					{
/*						Int32 xRand;
						Int32 yRand;
						Int16 fontSize = imgWidth / 12;
						OSInt leng = this->watermark->leng;
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
							if (sz[0] <= dimg->info.dispWidth && sz[1] <= dimg->info.dispHeight)
							{
								xRand = Double2Int32(dimg->info.dispWidth - sz[0]);
								yRand = Double2Int32(dimg->info.dispHeight - sz[1]);
								iWidth = Double2Int32(sz[0]);
								iHeight = Double2Int32(sz[1]);
								gimg2 = this->eng->CreateImage32(iWidth, iHeight, Media::AT_NO_ALPHA);
								gimg2->DrawString(0, 0, this->watermark, f, b);
								gimg2->SetAlphaType(Media::AT_ALPHA);
								{
									Bool revOrder;
									UInt8 *bits = gimg2->GetImgBits(&revOrder);
									Int32 col = (this->random->NextInt30() & 0xffffff) | 0x5f808080;
									ImageUtil_ColorReplace32(bits, iWidth, iHeight, col);
								}
								gimg->DrawImagePt(gimg2, Double2Int32(this->random->NextDouble() * xRand), Double2Int32(this->random->NextDouble() * yRand));
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
						NEW_CLASS(mstm, IO::MemoryStream(UTF8STRC("SSWR::OrganMgr::OrganWebHandler.WebRequest"));
						gimg->SaveJPG(mstm);
						buff = mstm->GetBuff(&buffSize);
						resp->AddDefHeaders(req);
						resp->AddContentLength(buffSize);
						resp->AddContentType((const UTF8Char*)"image/jpeg"));
						resp->Write(buff, buffSize);

						if (this->cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE && buffSize > 0)
						{
							IO::FileStream fs(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
							fs.Write(buff, buffSize);
						}

						DEL_CLASS(mstm);
						DEL_CLASS(dimg);
						this->eng->DeleteImage(gimg);*/
					}
					else
					{
						void *param;
						Media::ImageList nimgList(CSTR("Temp"));
						IO::MemoryStream mstm;
						nimgList.AddImage(dimg, 0);
						Exporter::GUIJPGExporter exporter;
						param = exporter.CreateParam(&nimgList);
						exporter.SetParamInt32(param, 0, 95);
						exporter.ExportFile(&mstm, CSTR(""), &nimgList, param);
						exporter.DeleteParam(param);
						ResponseMstm(req, resp, &mstm, CSTR("image/jpeg"));

						if (this->cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE && mstm.GetLength() > 0)
						{
							IO::FileStream fs({sbuff, (UOSInt)(sptrEnd - sbuff)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
							buff = mstm.GetBuff(&buffSize);
							fs.Write(buff, buffSize);
						}
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
			this->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return;
		}
	}
	else
	{
		this->dataMut.UnlockRead();
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return;
	}
}

void SSWR::OrganMgr::OrganWebHandler::ResponsePhotoId(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, SSWR::OrganMgr::OrganWebHandler::WebUserInfo *reqUser, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, Int32 fileId)
{
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
	UTF8Char sbuff2[512];
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile;
	Int32 rotateType = 0;
	this->dataMut.LockRead();
	sp = this->spMap.Get(speciesId);
	userFile = this->userFileMap.Get(fileId);
	if (sp && sp->cateId == cateId && userFile && (userFile->fileType == 1 || userFile->fileType == 3))
	{
		Data::DateTime dt;
		SSWR::OrganMgr::OrganWebHandler::WebUserInfo *user;
		user = this->userMap.Get(userFile->webuserId);
		dt.SetTicks(userFile->fileTimeTicks);
		dt.ToUTCTime();
		rotateType = userFile->rotType;

		sptr = this->cacheDir->ConcatTo(sbuff2);
		sptr = IO::Path::AppendPath(sbuff2, sptr, CSTR("UserFile"));
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrInt32(sptr, userFile->webuserId);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = dt.ToString(sptr, "yyyyMM");
		IO::Path::CreateDirectory(CSTRP(sbuff2, sptr));
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr2 = userFile->dataFileName->ConcatTo(sptr);

		if (this->cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE && userFile->prevUpdated == 0)
		{
			Data::DateTime dt2;
			Data::DateTime dt3;
			if (req->GetIfModifiedSince(&dt2) && IO::Path::GetFileTime(sbuff2, &dt3, 0, 0))
			{
				Int64 tdiff = dt2.ToTicks() - dt3.ToTicks();
				if (tdiff >= -1000 && tdiff <= 1000)
				{
					this->dataMut.UnlockRead();
					resp->ResponseNotModified(req, -1);
					return;
				}
			}
			IO::FileStream fs({sbuff2, (UOSInt)(sptr2 - sbuff2)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			UOSInt buffSize = (UOSInt)fs.GetLength();
			if (fs.IsError() || buffSize == 0)
			{
			}
			else
			{
				UInt8 *buff;
				buff = MemAlloc(UInt8, buffSize);
				fs.Read(buff, buffSize);
				fs.GetFileTimes(0, 0, &dt2);
				resp->AddDefHeaders(req);
				resp->AddContentLength(buffSize);
				resp->AddContentType(CSTR("image/jpeg"));
				resp->AddLastModified(&dt2);
				resp->Write(buff, buffSize);
				this->dataMut.UnlockRead();
				MemFree(buff);
				return;
			}
		}

		sptr = this->dataDir->ConcatTo(sbuff);
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
		this->dataMut.UnlockRead();

		Media::ImageList *imgList;
		Media::StaticImage *simg;
		Media::StaticImage *lrimg;
		Media::StaticImage *dimg;
		Sync::MutexUsage mutUsage(&this->parserMut);
		{
			IO::StmData::FileData fd({sbuff, (UOSInt)(sptr - sbuff)}, false);
			imgList = (Media::ImageList*)this->parsers.ParseFileType(&fd, IO::ParserType::ImageList);
		}
		mutUsage.EndUse();
		if (imgList)
		{
			simg = imgList->GetImage(0, 0)->CreateStaticImage();
			DEL_CLASS(imgList);
			Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
			NEW_CLASS(lrimg, Media::StaticImage(simg->info.dispWidth, simg->info.dispHeight, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, 0, &color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			Sync::MutexUsage mutUsage(&this->csconvMut);
			if (this->csconv == 0 || this->csconvFCC != simg->info.fourcc || this->csconvBpp != simg->info.storeBPP || this->csconvPF != simg->info.pf || !simg->info.color->Equals(&this->csconvColor))
			{
				SDEL_CLASS(this->csconv);
				this->csconvFCC = simg->info.fourcc;
				this->csconvBpp = simg->info.storeBPP;
				this->csconvPF = simg->info.pf;
				this->csconvColor.Set(simg->info.color);
				this->csconv = Media::CS::CSConverter::NewConverter(this->csconvFCC, this->csconvBpp, this->csconvPF, &this->csconvColor, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, &color, Media::ColorProfile::YUVT_UNKNOWN, this->colorSess);
			}
			if (this->csconv)
			{
				this->csconv->ConvertV2(&simg->data, lrimg->data, simg->info.dispWidth, simg->info.dispHeight, simg->info.storeWidth, simg->info.storeHeight, (OSInt)lrimg->GetDataBpl(), Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);
			}
			else
			{
				SDEL_CLASS(lrimg);
			}
			mutUsage.EndUse();
			DEL_CLASS(simg);

			if (lrimg)
			{
				this->lrgbLimiter.LimitImageLRGB(lrimg->data, lrimg->info.dispWidth, lrimg->info.dispHeight);
				if (imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE)
				{
					Sync::MutexUsage mutUsage(&this->resizerMut);
					resizerLR->SetResizeAspectRatio(Media::IImgResizer::RAR_SQUAREPIXEL);
					resizerLR->SetTargetWidth(imgWidth);
					resizerLR->SetTargetHeight(imgHeight);
					Double x1 = userFile->cropLeft;
					Double y1 = userFile->cropTop;
					Double x2 = UOSInt2Double(lrimg->info.dispWidth) - userFile->cropRight;
					Double y2 = UOSInt2Double(lrimg->info.dispHeight) - userFile->cropBottom;
					if (userFile->cropLeft < 0)
					{
						x1 = 0;
						x2 = UOSInt2Double(lrimg->info.dispWidth) - userFile->cropRight - userFile->cropLeft;
					}
					else if (userFile->cropRight < 0)
					{
						x1 = userFile->cropLeft + userFile->cropRight;
						x2 = UOSInt2Double(lrimg->info.dispWidth);
					}
					if (userFile->cropTop < 0)
					{
						y1 = 0;
						y2 = UOSInt2Double(lrimg->info.dispHeight) - userFile->cropBottom - userFile->cropTop;
					}
					else if (userFile->cropBottom < 0)
					{
						y1 = userFile->cropBottom + userFile->cropTop;
						y2 = UOSInt2Double(lrimg->info.dispHeight);
					}
					dimg = resizerLR->ProcessToNewPartial(lrimg, x1, y1, x2, y2);
					mutUsage.EndUse();
				}
				else
				{
					Sync::MutexUsage mutUsage(&this->resizerMut);
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
				UInt8 *buff;
				UOSInt buffSize;
				dimg->info.color->SetRAWICC(Media::ICCProfile::GetSRGBICCData());

				if (rotateType == 1)
				{
					dimg->RotateImage(Media::StaticImage::RotateType::CW90);
				}
				else if (rotateType == 2)
				{
					dimg->RotateImage(Media::StaticImage::RotateType::CW180);
				}
				else if (rotateType == 3)
				{
					dimg->RotateImage(Media::StaticImage::RotateType::CW270);
				}

				if (user && user->watermark)
				{
					Media::DrawImage *gimg = this->eng->ConvImage(dimg);
					if ((this->cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE) || user != reqUser)
					{
						Int32 xRand;
						Int32 yRand;
						UInt32 fontSizePx = imgWidth / 12;
						Math::Size2D<Double> sz;
						UInt32 iWidth;
						UInt32 iHeight;
						Media::DrawImage *gimg2;
						Media::DrawBrush *b = gimg->NewBrushARGB(0xffffffff);
						Media::DrawFont *f;
						while (true)
						{
							f = gimg->NewFontPx(CSTR("Arial"), fontSizePx, Media::DrawEngine::DFS_NORMAL, 0);
							sz = gimg->GetTextSize(f, user->watermark->ToCString());
							if (!sz.HasArea())
							{
								gimg->DelFont(f);
								break;
							}
							if (sz.width <= UOSInt2Double(dimg->info.dispWidth) && sz.height <= UOSInt2Double(dimg->info.dispHeight))
							{
								xRand = Double2Int32(UOSInt2Double(dimg->info.dispWidth) - sz.width);
								yRand = Double2Int32(UOSInt2Double(dimg->info.dispHeight) - sz.height);
								iWidth = (UInt32)Double2Int32(sz.width);
								iHeight = (UInt32)Double2Int32(sz.height);
								gimg2 = this->eng->CreateImage32(iWidth, iHeight, Media::AT_NO_ALPHA);
								gimg2->DrawString(0, 0, user->watermark->ToCString(), f, b);
								gimg2->SetAlphaType(Media::AT_ALPHA);
								{
									Bool revOrder;
									UInt8 *bits = gimg2->GetImgBits(&revOrder);
									UInt32 col = (this->random.NextInt30() & 0xffffff) | 0x5f808080;
									if (bits)
									{
										ImageUtil_ColorReplace32(bits, iWidth, iHeight, col);
									}
								}
								gimg->DrawImagePt(gimg2, Double2Int32(this->random.NextDouble() * xRand), Double2Int32(this->random.NextDouble() * yRand));
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

					IO::MemoryStream mstm;
					gimg->SaveJPG(&mstm);
					ResponseMstm(req, resp, &mstm, CSTR("image/jpeg"));

					if (this->cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE)
					{
						IO::FileStream fs({sbuff2, (UOSInt)(sptr2 - sbuff2)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
						buff = mstm.GetBuff(&buffSize);
						fs.Write(buff, buffSize);
						if (userFile->prevUpdated)
						{
							this->UserFilePrevUpdated(userFile);
						}
					}

					DEL_CLASS(dimg);
					this->eng->DeleteImage(gimg);
				}
				else
				{
					void *param;
					Media::ImageList nimgList(CSTR("Temp"));
					IO::MemoryStream mstm;
					nimgList.AddImage(dimg, 0);
					Exporter::GUIJPGExporter exporter;
					param = exporter.CreateParam(&nimgList);
					exporter.SetParamInt32(param, 0, 95);
					exporter.ExportFile(&mstm, CSTR(""), &nimgList, param);
					exporter.DeleteParam(param);
					ResponseMstm(req, resp, &mstm, CSTR("image/jpeg"));

					if (this->cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE)
					{
						IO::FileStream fs({sbuff2, (UOSInt)(sptr2 - sbuff2)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
						buff = mstm.GetBuff(&buffSize);
						fs.Write(buff, buffSize);
						if (userFile->prevUpdated)
						{
							this->UserFilePrevUpdated(userFile);
						}
					}
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
		this->dataMut.UnlockRead();
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return;
	}
}

void SSWR::OrganMgr::OrganWebHandler::ResponsePhotoWId(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, SSWR::OrganMgr::OrganWebHandler::WebUserInfo *reqUser, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, Int32 fileWId)
{
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
	UTF8Char sbuff2[512];
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	SSWR::OrganMgr::OrganWebHandler::WebFileInfo *wfile;
	Int32 rotateType = 0;
	this->dataMut.LockRead();
	sp = this->spMap.Get(speciesId);
	if (sp && sp->cateId == cateId)
	{
		wfile = sp->wfiles.Get(fileWId);
		if (wfile)
		{
			Data::DateTime dt;

			sptr = this->cacheDir->ConcatTo(sbuff2);
			sptr = IO::Path::AppendPath(sbuff2, sptr, CSTR("WebFile"));
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrInt32(sptr, wfile->id >> 10);
			IO::Path::CreateDirectory(CSTRP(sbuff2, sptr));
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrInt32(sptr, wfile->id);
			sptr2 = Text::StrConcatC(sptr, UTF8STRC(".jpg"));

			if (this->cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE && wfile->prevUpdated == 0)
			{
				IO::FileStream fs({sbuff2, (UOSInt)(sptr2 - sbuff2)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				UOSInt buffSize = (UOSInt)fs.GetLength();
				if (fs.IsError() || buffSize == 0)
				{
				}
				else
				{
					UInt8 *buff;
					buff = MemAlloc(UInt8, buffSize);
					fs.Read(buff, buffSize);
					resp->AddDefHeaders(req);
					resp->AddContentLength(buffSize);
					resp->AddContentType(CSTR("image/jpeg"));
					resp->Write(buff, buffSize);
					this->dataMut.UnlockRead();
					MemFree(buff);
					return;
				}
			}

			sptr = this->dataDir->ConcatTo(sbuff);
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
			this->dataMut.UnlockRead();

			Media::ImageList *imgList;
			Media::StaticImage *simg;
			Media::StaticImage *lrimg;
			Media::StaticImage *dimg;
			{
				Sync::MutexUsage mutUsage(&this->parserMut);
				IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
				imgList = (Media::ImageList*)this->parsers.ParseFileType(&fd, IO::ParserType::ImageList);
			}
			if (imgList)
			{
				simg = imgList->GetImage(0, 0)->CreateStaticImage();
				DEL_CLASS(imgList);
				Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
				NEW_CLASS(lrimg, Media::StaticImage(simg->info.dispWidth, simg->info.dispHeight, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, 0, &color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
				Sync::MutexUsage mutUsage(&this->csconvMut);
				if (this->csconv == 0 || this->csconvFCC != simg->info.fourcc || this->csconvBpp != simg->info.storeBPP || this->csconvPF != simg->info.pf || !simg->info.color->Equals(&this->csconvColor))
				{
					SDEL_CLASS(this->csconv);
					this->csconvFCC = simg->info.fourcc;
					this->csconvBpp = simg->info.storeBPP;
					this->csconvPF = simg->info.pf;
					this->csconvColor.Set(simg->info.color);
					this->csconv = Media::CS::CSConverter::NewConverter(this->csconvFCC, this->csconvBpp, this->csconvPF, &this->csconvColor, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, &color, Media::ColorProfile::YUVT_UNKNOWN, this->colorSess);
				}
				if (this->csconv)
				{
					this->csconv->ConvertV2(&simg->data, lrimg->data, simg->info.dispWidth, simg->info.dispHeight, simg->info.storeWidth, simg->info.storeHeight, (OSInt)lrimg->GetDataBpl(), Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);
				}
				else
				{
					SDEL_CLASS(lrimg);
				}
				mutUsage.EndUse();
				DEL_CLASS(simg);

				if (lrimg)
				{
					this->lrgbLimiter.LimitImageLRGB(lrimg->data, lrimg->info.dispWidth, lrimg->info.dispHeight);
					if (imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE)
					{
						Sync::MutexUsage mutUsage(&this->resizerMut);
						resizerLR->SetResizeAspectRatio(Media::IImgResizer::RAR_SQUAREPIXEL);
						resizerLR->SetTargetWidth(imgWidth);
						resizerLR->SetTargetHeight(imgHeight);
						Double x1 = wfile->cropLeft;
						Double y1 = wfile->cropTop;
						Double x2 = UOSInt2Double(lrimg->info.dispWidth) - wfile->cropRight;
						Double y2 = UOSInt2Double(lrimg->info.dispHeight) - wfile->cropBottom;
						if (wfile->cropLeft < 0)
						{
							x1 = 0;
							x2 = UOSInt2Double(lrimg->info.dispWidth) - wfile->cropRight - wfile->cropLeft;
						}
						else if (wfile->cropRight < 0)
						{
							x1 = wfile->cropLeft + wfile->cropRight;
							x2 = UOSInt2Double(lrimg->info.dispWidth);
						}
						if (wfile->cropTop < 0)
						{
							y1 = 0;
							y2 = UOSInt2Double(lrimg->info.dispHeight) - wfile->cropBottom - wfile->cropTop;
						}
						else if (wfile->cropBottom < 0)
						{
							y1 = wfile->cropBottom + wfile->cropTop;
							y2 = UOSInt2Double(lrimg->info.dispHeight);
						}
						dimg = resizerLR->ProcessToNewPartial(lrimg, x1, y1, x2, y2);
						mutUsage.EndUse();
					}
					else
					{
						Sync::MutexUsage mutUsage(&this->resizerMut);
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
					UInt8 *buff;
					UOSInt buffSize;
					dimg->info.color->SetRAWICC(Media::ICCProfile::GetSRGBICCData());

					if (rotateType == 1)
					{
						dimg->RotateImage(Media::StaticImage::RotateType::CW90);
					}
					else if (rotateType == 2)
					{
						dimg->RotateImage(Media::StaticImage::RotateType::CW180);
					}
					else if (rotateType == 3)
					{
						dimg->RotateImage(Media::StaticImage::RotateType::CW270);
					}

					void *param;
					Media::ImageList nimgList(CSTR("Temp"));
					IO::MemoryStream mstm;
					nimgList.AddImage(dimg, 0);
					Exporter::GUIJPGExporter exporter;
					param = exporter.CreateParam(&nimgList);
					exporter.SetParamInt32(param, 0, 95);
					exporter.ExportFile(&mstm, CSTR(""), &nimgList, param);
					exporter.DeleteParam(param);
					ResponseMstm(req, resp, &mstm, CSTR("image/jpeg"));

					if (this->cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE)
					{
						IO::FileStream fs({sbuff2, (UOSInt)(sptr2 - sbuff2)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
						buff = mstm.GetBuff(&buffSize);
						fs.Write(buff, buffSize);
						if (wfile->prevUpdated)
						{
							this->WebFilePrevUpdated(wfile);
						}
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
			this->dataMut.UnlockRead();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return;
		}
	}
	else
	{
		this->dataMut.UnlockRead();
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return;
	}
}

void SSWR::OrganMgr::OrganWebHandler::ResponseMstm(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, IO::MemoryStream *mstm, Text::CString contType)
{
	resp->AddDefHeaders(req);
	resp->AddContentType(contType);
	mstm->SeekFromBeginning(0);
	Net::WebServer::HTTPServerUtil::SendContent(req, resp, contType, mstm->GetLength(), mstm);
}

void SSWR::OrganMgr::OrganWebHandler::WriteHeaderPart1(IO::Writer *writer, const UTF8Char *title, Bool isMobile)
{
	Text::String *s;
	writer->WriteLineC(UTF8STRC("<HTML>"));
	writer->WriteLineC(UTF8STRC("<HEAD>"));
	writer->WriteLineC(UTF8STRC("<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=utf8\">"));
	if (isMobile)
	{
		writer->WriteLineC(UTF8STRC("<meta name=\"viewport\" content=\"width=1024\">"));
	}
	writer->WriteStrC(UTF8STRC("<title>"));
	s = Text::XML::ToNewHTMLElementText(title);
	writer->WriteStrC(s->v, s->leng);
	s->Release();
	writer->WriteLineC(UTF8STRC("</title>"));
}

void SSWR::OrganMgr::OrganWebHandler::WriteHeaderPart2(IO::Writer *writer, SSWR::OrganMgr::OrganWebHandler::WebUserInfo *user, const UTF8Char *onLoadFunc)
{
	Text::String *s;
	writer->WriteLineC(UTF8STRC("</HEAD>"));
	writer->WriteLine();
	writer->WriteStrC(UTF8STRC("<BODY TEXT=\"#c0e0ff\" LINK=\"#6080ff\" VLINK=\"#4060ff\" ALINK=\"#4040FF\" bgcolor=\"#000000\""));
	if (onLoadFunc)
	{
		writer->WriteStrC(UTF8STRC(" onLoad="));
		s = Text::XML::ToNewAttrText(onLoadFunc);
		writer->WriteStrC(s->v, s->leng);
		s->Release();
	}
	writer->WriteLineC(UTF8STRC(">"));
	if (user)
	{
		writer->WriteStrC(UTF8STRC("<p align=\"right\">"));
		s = Text::XML::ToNewHTMLBodyText(user->userName->v);
		writer->WriteStrC(s->v, s->leng);
		s->Release();
		writer->WriteLineC(UTF8STRC("<a href=\"logout\">Logout</a></p>"));
	}
}

void SSWR::OrganMgr::OrganWebHandler::WriteHeader(IO::Writer *writer, const UTF8Char *title, SSWR::OrganMgr::OrganWebHandler::WebUserInfo *user, Bool isMobile)
{
	this->WriteHeaderPart1(writer, title, isMobile);
	this->WriteHeaderPart2(writer, user, 0);
}

void SSWR::OrganMgr::OrganWebHandler::WriteFooter(IO::Writer *writer)
{
	writer->WriteLineC(UTF8STRC("</BODY>"));
	writer->WriteLineC(UTF8STRC("</HTML>"));
}

void SSWR::OrganMgr::OrganWebHandler::WriteLocator(IO::Writer *writer, SSWR::OrganMgr::OrganWebHandler::GroupInfo *group, SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate)
{
	SSWR::OrganMgr::OrganWebHandler::GroupTypeInfo *grpType;
	Text::String *s;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[12];
	UTF8Char *sptr;
	Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupInfo *> groupList;
	UOSInt i;
	while (group)
	{
		groupList.Add(group);
		group = this->groupMap.Get(group->parentId);
	}


	writer->WriteLineC(UTF8STRC("<center><table border=\"0\" cellpadding=\"0\" cellspacing=\"4\">"));
	writer->WriteStrC(UTF8STRC("<tr><td></td><td></td><td><a href=\"/\">Index"));
	writer->WriteStrC(UTF8STRC("</a></td></tr>"));
	if (cate)
	{
		writer->WriteStrC(UTF8STRC("<tr><td></td><td></td><td><a href="));
		sb.ClearStr();
		sb.AppendC(UTF8STRC("cate.html?cateName="));
		sb.Append(cate->dirName);
		s = Text::XML::ToNewAttrText(sb.ToString());
		writer->WriteStrC(s->v, s->leng);
		s->Release();
		writer->WriteStrC(UTF8STRC(">"));
		s = Text::XML::ToNewHTMLBodyText(cate->chiName->v);
		writer->WriteStrC(s->v, s->leng);
		s->Release();
		writer->WriteStrC(UTF8STRC("</a></td></tr>"));
	}

	i = groupList.GetCount();
	while (i-- > 0)
	{
		group = groupList.GetItem(i);
		grpType = cate->groupTypes.Get(group->groupType);
		writer->WriteLineC(UTF8STRC("<tr>"));
		if (grpType)
		{
			writer->WriteStrC(UTF8STRC("<td>"));
			s = Text::XML::ToNewHTMLBodyText(grpType->chiName->v);
			writer->WriteStrC(s->v, s->leng);
			s->Release();
			writer->WriteLineC(UTF8STRC("</td>"));
			writer->WriteStrC(UTF8STRC("<td>"));
			s = Text::XML::ToNewHTMLBodyText(grpType->engName->v);
			writer->WriteStrC(s->v, s->leng);
			s->Release();
			writer->WriteLineC(UTF8STRC("</td>"));
		}
		else
		{
			writer->WriteLineC(UTF8STRC("<td>?</td>"));
			writer->WriteLineC(UTF8STRC("<td>?</td>"));
		}
		writer->WriteStrC(UTF8STRC("<td><a href=\"group.html?id="));
		sptr = Text::StrInt32(sbuff, group->id);
		writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer->WriteStrC(UTF8STRC("&amp;cateId="));
		sptr = Text::StrInt32(sbuff, group->cateId);
		writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer->WriteStrC(UTF8STRC("\">"));
		sb.ClearStr();
		sb.Append(group->engName);
		sb.AppendC(UTF8STRC(" "));
		sb.Append(group->chiName);
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer->WriteStrC(s->v, s->leng);
		s->Release();
		writer->WriteLineC(UTF8STRC("</a></td>"));
		writer->WriteLineC(UTF8STRC("</tr>"));

	}
	writer->WriteLineC(UTF8STRC("</table>"));
	WriteLocatorText(writer, group, cate);
	writer->WriteLineC(UTF8STRC("</center>"));
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
		group = this->groupMap.Get(group->parentId);
	}

	if (!found)
		return;

	found = false;
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Text Form: "));
	i = groupList.GetCount();
	while (i-- > 0)
	{
		if (found)
		{
			sb.AppendC(UTF8STRC(", "));
		}
		group = groupList.GetItem(i);
		sb.Append(group->engName);
		found = true;
	}
	sb.AppendC(UTF8STRC("<br/>"));
	writer->WriteLineC(sb.ToString(), sb.GetLength());
}

void SSWR::OrganMgr::OrganWebHandler::WriteGroupTable(IO::Writer *writer, const Data::ReadingList<SSWR::OrganMgr::OrganWebHandler::GroupInfo *> *groupList, UInt32 scnWidth, Bool showSelect)
{
	SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
	Text::String *s;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
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
		writer->WriteLineC(UTF8STRC("<table border=\"0\" width=\"100%\">"));
		currColumn = 0;
		while (i < j)
		{
			group = groupList->GetItem(i);
			this->CalcGroupCount(group);
			if (group->totalCount != 0 || showSelect)
			{
				if (currColumn == 0)
				{
					writer->WriteLineC(UTF8STRC("<tr>"));
				}
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<td width=\""));
				sb.AppendU32(colWidth);
				sb.AppendC(UTF8STRC("%\">"));
				writer->WriteLineC(sb.ToString(), sb.GetCharCnt());
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<center><a href=\"group.html?id="));
				sb.AppendI32(group->id);
				sb.AppendC(UTF8STRC("&amp;cateId="));
				sb.AppendI32(group->cateId);
				sb.AppendC(UTF8STRC("\">"));
				writer->WriteLineC(sb.ToString(), sb.GetCharCnt());

				if (group->photoSpObj && (group->photoSpObj->photo != 0 || group->photoSpObj->photoId != 0 || group->photoSpObj->photoWId != 0))
				{
					if (group->photoSpObj->photoId != 0)
					{
						writer->WriteStrC(UTF8STRC("<img src="));
						sb.ClearStr();
						sb.AppendC(UTF8STRC("photo.html?id="));
						sb.AppendI32(group->photoSpObj->speciesId);
						sb.AppendC(UTF8STRC("&cateId="));
						sb.AppendI32(group->cateId);
						sb.AppendC(UTF8STRC("&width="));
						sb.AppendI32(PREVIEW_SIZE);
						sb.AppendC(UTF8STRC("&height="));
						sb.AppendI32(PREVIEW_SIZE);
						sb.AppendC(UTF8STRC("&fileId="));
						sb.AppendI32(group->photoSpObj->photoId);
						s = Text::XML::ToNewAttrText(sb.ToString());
						writer->WriteStrC(s->v, s->leng);
						s->Release();
						writer->WriteStrC(UTF8STRC(" border=\"0\" ALT="));
						s = Text::XML::ToNewAttrText(group->engName->v);
						writer->WriteStrC(s->v, s->leng);
						s->Release();
						writer->WriteLineC(UTF8STRC("><br/>"));
					}
					else if (group->photoSpObj->photoWId != 0)
					{
						writer->WriteStrC(UTF8STRC("<img src="));
						sb.ClearStr();
						sb.AppendC(UTF8STRC("photo.html?id="));
						sb.AppendI32(group->photoSpObj->speciesId);
						sb.AppendC(UTF8STRC("&cateId="));
						sb.AppendI32(group->cateId);
						sb.AppendC(UTF8STRC("&width="));
						sb.AppendI32(PREVIEW_SIZE);
						sb.AppendC(UTF8STRC("&height="));
						sb.AppendI32(PREVIEW_SIZE);
						sb.AppendC(UTF8STRC("&fileWId="));
						sb.AppendI32(group->photoSpObj->photoWId);
						s = Text::XML::ToNewAttrText(sb.ToString());
						writer->WriteStrC(s->v, s->leng);
						s->Release();
						writer->WriteStrC(UTF8STRC(" border=\"0\" ALT="));
						s = Text::XML::ToNewAttrText(group->engName->v);
						writer->WriteStrC(s->v, s->leng);
						s->Release();
						writer->WriteLineC(UTF8STRC("><br/>"));
					}
					else
					{
						writer->WriteStrC(UTF8STRC("<img src="));
						sb.ClearStr();
						sb.AppendC(UTF8STRC("photo.html?id="));
						sb.AppendI32(group->photoSpObj->speciesId);
						sb.AppendC(UTF8STRC("&cateId="));
						sb.AppendI32(group->cateId);
						sb.AppendC(UTF8STRC("&width="));
						sb.AppendI32(PREVIEW_SIZE);
						sb.AppendC(UTF8STRC("&height="));
						sb.AppendI32(PREVIEW_SIZE);
						sb.AppendC(UTF8STRC("&file="));
						sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, group->photoSpObj->photo->v);
						sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
						s = Text::XML::ToNewAttrText(sb.ToString());
						writer->WriteStrC(s->v, s->leng);
						s->Release();
						writer->WriteStrC(UTF8STRC(" border=\"0\" ALT="));
						s = Text::XML::ToNewAttrText(group->engName->v);
						writer->WriteStrC(s->v, s->leng);
						s->Release();
						writer->WriteLineC(UTF8STRC("><br/>"));
					}
				}
				else
				{
					s = Text::XML::ToNewHTMLBodyText(group->engName->v);
					writer->WriteStrC(s->v, s->leng);
					s->Release();
				}
				if (showSelect)
				{
					writer->WriteLineC(UTF8STRC("</a>"));
					
					sb.ClearStr();
					sb.AppendC(UTF8STRC("<input type=\"checkbox\" name=\"group"));
					sb.AppendI32(group->id);
					sb.AppendC(UTF8STRC("\" id=\"group"));
					sb.AppendI32(group->id);
					sb.AppendC(UTF8STRC("\" value=\"1\"/><label for=\"group"));
					sb.AppendI32(group->id);
					sb.AppendC(UTF8STRC("\">"));
					writer->WriteStrC(sb.ToString(), sb.GetLength());
					sb.ClearStr();
					sb.Append(group->chiName);
					sb.AppendC(UTF8STRC(" "));
					sb.Append(group->engName);
					sb.AppendC(UTF8STRC(" ("));
					sb.AppendUOSInt(group->myPhotoCount);
					sb.AppendC(UTF8STRC("/"));
					sb.AppendUOSInt(group->photoCount);
					sb.AppendC(UTF8STRC("/"));
					sb.AppendUOSInt(group->totalCount);
					sb.AppendC(UTF8STRC(")"));
					s = Text::XML::ToNewHTMLBodyText(sb.ToString());
					writer->WriteStrC(s->v, s->leng);
					writer->WriteLineC(UTF8STRC("</label></center></td>"));
					s->Release();
				}
				else
				{
					sb.ClearStr();
					sb.Append(group->chiName);
					sb.AppendC(UTF8STRC(" "));
					sb.Append(group->engName);
					sb.AppendC(UTF8STRC(" ("));
					sb.AppendUOSInt(group->myPhotoCount);
					sb.AppendC(UTF8STRC("/"));
					sb.AppendUOSInt(group->photoCount);
					sb.AppendC(UTF8STRC("/"));
					sb.AppendUOSInt(group->totalCount);
					sb.AppendC(UTF8STRC(")"));
					s = Text::XML::ToNewHTMLBodyText(sb.ToString());
					writer->WriteStrC(s->v, s->leng);
					writer->WriteLineC(UTF8STRC("</a></center></td>"));
					s->Release();
				}

				currColumn++;
				if (currColumn >= colCount)
				{
					writer->WriteLineC(UTF8STRC("</tr>"));
					currColumn = 0;
				}
			}
			i++;
		}
		if (currColumn != 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<td width=\""));
			sb.AppendU32(colWidth);
			sb.AppendC(UTF8STRC("%\"></td>"));
			while (currColumn < colCount)
			{
				writer->WriteLineC(sb.ToString(), sb.GetCharCnt());
				currColumn++;
			}
			writer->WriteLineC(UTF8STRC("</tr>"));
		}
		writer->WriteLineC(UTF8STRC("</table>"));
	}
}

void SSWR::OrganMgr::OrganWebHandler::WriteSpeciesTable(IO::Writer *writer, const Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *> *spList, UInt32 scnWidth, Int32 cateId, Bool showSelect)
{
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *sp;
	Text::String *s;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
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
		writer->WriteLineC(UTF8STRC("<table border=\"0\" width=\"100%\">"));
		currColumn = 0;
		while (i < j)
		{
			sp = spList->GetItem(i);
			if (currColumn == 0)
			{
				writer->WriteLineC(UTF8STRC("<tr>"));
			}
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<td width=\""));
			sb.AppendU32(colWidth);
			sb.AppendC(UTF8STRC("%\">"));
			writer->WriteLineC(sb.ToString(), sb.GetLength());
			sb.ClearStr();
			if (sp->cateId == cateId || showSelect)
			{
				sb.AppendC(UTF8STRC("<center><a href=\"species.html?id="));
				sb.AppendI32(sp->speciesId);
				sb.AppendC(UTF8STRC("&amp;cateId="));
				sb.AppendI32(sp->cateId);
				sb.AppendC(UTF8STRC("\">"));
				writer->WriteLineC(sb.ToString(), sb.GetLength());
			}
			else
			{
				writer->WriteLineC(UTF8STRC("<center>"));
			}

			if (sp->photoId != 0)
			{
				writer->WriteStrC(UTF8STRC("<img src="));
				sb.ClearStr();
				sb.AppendC(UTF8STRC("photo.html?id="));
				sb.AppendI32(sp->speciesId);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(sp->cateId);
				sb.AppendC(UTF8STRC("&width="));
				sb.AppendI32(PREVIEW_SIZE);
				sb.AppendC(UTF8STRC("&height="));
				sb.AppendI32(PREVIEW_SIZE);
				sb.AppendC(UTF8STRC("&fileId="));
				sb.AppendI32(sp->photoId);
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer->WriteStrC(s->v, s->leng);
				s->Release();
				writer->WriteStrC(UTF8STRC(" border=\"0\" ALT="));
				s = Text::XML::ToNewAttrText(sp->sciName->v);
				writer->WriteStrC(s->v, s->leng);
				s->Release();
				writer->WriteLineC(UTF8STRC("><br/>"));
			}
			else if (sp->photoWId)
			{
				writer->WriteStrC(UTF8STRC("<img src="));
				sb.ClearStr();
				sb.AppendC(UTF8STRC("photo.html?id="));
				sb.AppendI32(sp->speciesId);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(sp->cateId);
				sb.AppendC(UTF8STRC("&width="));
				sb.AppendI32(PREVIEW_SIZE);
				sb.AppendC(UTF8STRC("&height="));
				sb.AppendI32(PREVIEW_SIZE);
				sb.AppendC(UTF8STRC("&fileWId="));
				sb.AppendI32(sp->photoWId);
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer->WriteStrC(s->v, s->leng);
				s->Release();
				writer->WriteStrC(UTF8STRC(" border=\"0\" ALT="));
				s = Text::XML::ToNewAttrText(sp->sciName->v);
				writer->WriteStrC(s->v, s->leng);
				s->Release();
				writer->WriteLineC(UTF8STRC("><br/>"));
			}
			else if (sp->photo)
			{
				writer->WriteStrC(UTF8STRC("<img src="));
				sb.ClearStr();
				sb.AppendC(UTF8STRC("photo.html?id="));
				sb.AppendI32(sp->speciesId);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(sp->cateId);
				sb.AppendC(UTF8STRC("&width="));
				sb.AppendI32(PREVIEW_SIZE);
				sb.AppendC(UTF8STRC("&height="));
				sb.AppendI32(PREVIEW_SIZE);
				sb.AppendC(UTF8STRC("&file="));
				sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, sp->photo->v);
				sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer->WriteStrC(s->v, s->leng);
				s->Release();
				writer->WriteStrC(UTF8STRC(" border=\"0\" ALT="));
				s = Text::XML::ToNewAttrText(sp->sciName->v);
				writer->WriteStrC(s->v, s->leng);
				s->Release();
				writer->WriteLineC(UTF8STRC("><br/>"));
			}
			else
			{
				s = Text::XML::ToNewHTMLBodyText(sp->sciName->v);
				writer->WriteStrC(s->v, s->leng);
				s->Release();
				writer->WriteLineC(UTF8STRC("<br/>"));
			}
			if (showSelect)
			{
				writer->WriteLineC(UTF8STRC("</a>"));
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<input type=\"checkbox\" name=\"species"));
				sb.AppendI32(sp->speciesId);
				sb.AppendC(UTF8STRC("\" id=\"species"));
				sb.AppendI32(sp->speciesId);
				sb.AppendC(UTF8STRC("\" value=\"1\"/><label for=\"species"));
				sb.AppendI32(sp->speciesId);
				sb.AppendC(UTF8STRC("\">"));
				writer->WriteStrC(sb.ToString(), sb.GetLength());

				sb.ClearStr();
				sb.Append(sp->sciName);
				sb.AppendC(UTF8STRC(" "));
				sb.Append(sp->chiName);
				sb.AppendC(UTF8STRC(" "));
				sb.Append(sp->engName);
				s = Text::XML::ToNewHTMLBodyText(sb.ToString());
				writer->WriteStrC(s->v, s->leng);
				writer->WriteLineC(UTF8STRC("</label>"));
				s->Release();
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<br/><a href=\"speciesmod.html?id="));
				sb.AppendI32(sp->groupId);
				sb.AppendC(UTF8STRC("&amp;cateId="));
				sb.AppendI32(sp->cateId);
				sb.AppendC(UTF8STRC("&amp;spId="));
				sb.AppendI32(sp->speciesId);
				sb.AppendC(UTF8STRC("\">Modify</a>"));
				writer->WriteLineC(sb.ToString(), sb.GetLength());
				writer->WriteLineC(UTF8STRC("</center></td>"));
			}
			else
			{
				sb.ClearStr();
				sb.Append(sp->sciName);
				sb.AppendC(UTF8STRC(" "));
				sb.Append(sp->chiName);
				sb.AppendC(UTF8STRC(" "));
				sb.Append(sp->engName);
				s = Text::XML::ToNewHTMLBodyText(sb.ToString());
				writer->WriteStrC(s->v, s->leng);
				s->Release();
				if (sp->cateId == cateId)
				{
					writer->WriteLineC(UTF8STRC("</a></center></td>"));
				}
				else
				{
					writer->WriteLineC(UTF8STRC("</center></td>"));
				}
			}

			currColumn++;
			if (currColumn >= colCount)
			{
				writer->WriteLineC(UTF8STRC("</tr>"));
				currColumn = 0;
			}
			i++;
		}
		if (currColumn != 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<td width=\""));
			sb.AppendU32(colWidth);
			sb.AppendC(UTF8STRC("%\"></td>"));
			while (currColumn < colCount)
			{
				writer->WriteLineC(sb.ToString(), sb.GetLength());
				currColumn++;
			}
			writer->WriteLineC(UTF8STRC("</tr>"));
		}
		writer->WriteLineC(UTF8STRC("</table>"));
	}
}

void SSWR::OrganMgr::OrganWebHandler::WritePickObjs(IO::Writer *writer, SSWR::OrganMgr::OrganWebHandler::RequestEnv *env, const UTF8Char *url)
{
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;
	Text::String *s;
	UInt32 colCount = env->scnWidth / PREVIEW_SIZE;
	UInt32 colWidth = 100 / colCount;
	UInt32 currColumn;
	SSWR::OrganMgr::OrganWebHandler::UserFileInfo *userFile;
	SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species;
	Data::DateTime dt;
	UTF8Char sbuff2[64];
	UTF8Char *sptr2;
	if (env->pickObjType == POT_USERFILE && env->pickObjs->GetCount() > 0)
	{
		currColumn = 0;
		sb.ClearStr();
		sb.AppendC(UTF8STRC("<form name=\"pickfiles\" action="));
		s = Text::XML::ToNewAttrText(url);
		sb.Append(s);
		s->Release();
		sb.AppendC(UTF8STRC(" method=\"POST\"/>"));
		writer->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(UTF8STRC("<input type=\"hidden\" name=\"action\" value=\"place\"/>"));
		writer->WriteLineC(UTF8STRC("<table border=\"0\" width=\"100%\">"));

		i = 0;
		j = env->pickObjs->GetCount();
		while (i < j)
		{
			userFile = this->userFileMap.Get(env->pickObjs->GetItem(i));
			species = 0;
			if (userFile) species = this->spMap.Get(userFile->speciesId);
			if (userFile && species)
			{
				if (currColumn == 0)
				{
					writer->WriteLineC(UTF8STRC("<tr>"));
				}
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<td width=\""));
				sb.AppendU32(colWidth);
				sb.AppendC(UTF8STRC("%\">"));
				writer->WriteLineC(sb.ToString(), sb.GetLength());
				sb.ClearStr();
				sb.AppendC(UTF8STRC("photodetail.html?id="));
				sb.AppendI32(userFile->speciesId);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(species->cateId);
				sb.AppendC(UTF8STRC("&fileId="));
				sb.AppendI32(userFile->id);
				s = Text::XML::ToNewAttrText(sb.ToString());
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<center><a href="));
				sb.Append(s);
				sb.AppendC(UTF8STRC(">"));
				writer->WriteLineC(sb.ToString(), sb.GetLength());
				s->Release();

				writer->WriteStrC(UTF8STRC("<img src="));
				sb.ClearStr();
				sb.AppendC(UTF8STRC("photo.html?id="));
				sb.AppendI32(species->speciesId);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(species->cateId);
				sb.AppendC(UTF8STRC("&width="));
				sb.AppendI32(PREVIEW_SIZE);
				sb.AppendC(UTF8STRC("&height="));
				sb.AppendI32(PREVIEW_SIZE);
				sb.AppendC(UTF8STRC("&fileId="));
				sb.AppendI32(userFile->id);
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer->WriteStrC(s->v, s->leng);
				s->Release();
				writer->WriteStrC(UTF8STRC(" border=\"0\">"));
				writer->WriteStrC(UTF8STRC("</a><br/>"));
				dt.SetTicks(userFile->fileTimeTicks);
				dt.ToLocalTime();

				sb.ClearStr();
				sb.AppendC(UTF8STRC("<input type=\"checkbox\" id=\"userfile"));
				sb.AppendI32(userFile->id);
				sb.AppendC(UTF8STRC("\" name=\"userfile"));
				sb.AppendI32(userFile->id);
				sb.AppendC(UTF8STRC("\" value=\"1\"/><label for=\"userfile"));
				sb.AppendI32(userFile->id);
				sb.AppendC(UTF8STRC("\">"));
				writer->WriteStrC(sb.ToString(), sb.GetLength());

				sptr2 = dt.ToString(sbuff2, "yyyy-MM-dd HH:mm:ss zzzz");
				writer->WriteStrC(sbuff2, (UOSInt)(sptr2 - sbuff2));
				if (userFile->webuserId == env->user->id)
				{
					if (userFile->location)
					{
						writer->WriteStrC(UTF8STRC(" "));
						s = Text::XML::ToNewHTMLBodyText(userFile->location->v);
						writer->WriteStrC(s->v, s->leng);
						s->Release();
					}
				}
				if (userFile->descript && userFile->descript->leng > 0)
				{
					writer->WriteStrC(UTF8STRC("<br/>"));
					s = Text::XML::ToNewHTMLBodyText(userFile->descript->v);
					writer->WriteStrC(s->v, s->leng);
					s->Release();
				}
				if (userFile->webuserId == env->user->id)
				{
					writer->WriteStrC(UTF8STRC("<br/>"));
					s = Text::XML::ToNewHTMLBodyText(userFile->oriFileName->v);
					writer->WriteStrC(s->v, s->leng);
					s->Release();
				}
				writer->WriteStrC(UTF8STRC("</label>"));
				if (userFile->lat != 0 || userFile->lon != 0)
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("<br/>"));
					sb.AppendC(UTF8STRC("<a href=\"https://www.google.com/maps/place/"));
					Text::SBAppendF64(&sb, userFile->lat);
					sb.AppendC(UTF8STRC(","));
					Text::SBAppendF64(&sb, userFile->lon);
					sb.AppendC(UTF8STRC("/@"));
					Text::SBAppendF64(&sb, userFile->lat);
					sb.AppendC(UTF8STRC(","));
					Text::SBAppendF64(&sb, userFile->lon);
					sb.AppendC(UTF8STRC(",19z\">"));
					sb.AppendC(UTF8STRC("Google Map</a>"));
					writer->WriteStrC(sb.ToString(), sb.GetLength());
				}
				writer->WriteLineC(UTF8STRC("</center></td>"));

				currColumn++;
				if (currColumn >= colCount)
				{
					writer->WriteLineC(UTF8STRC("</tr>"));
					currColumn = 0;
				}
			}

			i++;
		}

		if (currColumn != 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<td width=\""));
			sb.AppendU32(colWidth);
			sb.AppendC(UTF8STRC("%\"></td>"));
			while (currColumn < colCount)
			{
				writer->WriteLineC(sb.ToString(), sb.GetLength());
				currColumn++;
			}
			writer->WriteLineC(UTF8STRC("</tr>"));
		}
		writer->WriteLineC(UTF8STRC("</table>"));
		writer->WriteLineC(UTF8STRC("<input type=\"submit\" value=\"Place Selected\"/>"));
		writer->WriteLineC(UTF8STRC("<input type=\"button\" value=\"Place All\" onclick=\"document.forms.pickfiles.action.value='placeall';document.forms.pickfiles.submit();\"/>"));
		writer->WriteLineC(UTF8STRC("</form>"));
		writer->WriteLineC(UTF8STRC("<hr/>"));
	}
	else if (env->pickObjType == POT_SPECIES && env->pickObjs->GetCount() > 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("<form name=\"pickfiles\" action="));
		s = Text::XML::ToNewAttrText(url);
		sb.Append(s);
		s->Release();
		sb.AppendC(UTF8STRC(" method=\"POST\"/>"));
		writer->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(UTF8STRC("<input type=\"hidden\" name=\"action\" value=\"place\"/>"));
		SSWR::OrganMgr::OrganWebHandler::SpeciesInfo *species;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::SpeciesInfo*> spList;
		i = 0;
		j = env->pickObjs->GetCount();
		while (i < j)
		{
			species = this->spMap.Get(env->pickObjs->GetItem(i));
			if (species)
			{
				spList.Add(species);
			}
			i++;
		}
		WriteSpeciesTable(writer, &spList, scnSize, 0, true);
		writer->WriteLineC(UTF8STRC("<input type=\"submit\" value=\"Place Selected\"/>"));
		writer->WriteLineC(UTF8STRC("<input type=\"button\" value=\"Place All\" onclick=\"document.forms.pickfiles.action.value='placeall';document.forms.pickfiles.submit();\"/>"));
		writer->WriteLineC(UTF8STRC("</form>"));
		writer->WriteLineC(UTF8STRC("<hr/>"));
	}
	else if (env->pickObjType == POT_GROUP && env->pickObjs->GetCount() > 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("<form name=\"pickfiles\" action="));
		s = Text::XML::ToNewAttrText(url);
		sb.Append(s);
		s->Release();
		sb.AppendC(UTF8STRC(" method=\"POST\"/>"));
		writer->WriteLineC(sb.ToString(), sb.GetLength());
		writer->WriteLineC(UTF8STRC("<input type=\"hidden\" name=\"action\" value=\"place\"/>"));
		SSWR::OrganMgr::OrganWebHandler::GroupInfo *group;
		Data::ArrayList<SSWR::OrganMgr::OrganWebHandler::GroupInfo*> groupList;
		i = 0;
		j = env->pickObjs->GetCount();
		while (i < j)
		{
			group = this->groupMap.Get(env->pickObjs->GetItem(i));
			if (group)
			{
				groupList.Add(group);
			}
			i++;
		}
		WriteGroupTable(writer, &groupList, scnSize, true);
		writer->WriteLineC(UTF8STRC("<input type=\"submit\" value=\"Place Selected\"/>"));
		writer->WriteLineC(UTF8STRC("<input type=\"button\" value=\"Place All\" onclick=\"document.forms.pickfiles.action.value='placeall';document.forms.pickfiles.submit();\"/>"));
		writer->WriteLineC(UTF8STRC("</form>"));
		writer->WriteLineC(UTF8STRC("<hr/>"));
	}
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::OnSessionDel(Net::WebServer::IWebSession* sess, void *userObj)
{
	Data::DateTime *t;
	Data::ArrayListInt32 *pickObjs;
	t = (Data::DateTime *)sess->GetValuePtr(UTF8STRC("LastUseTime"));
	pickObjs = (Data::ArrayListInt32*)sess->GetValuePtr(UTF8STRC("PickObjs"));
	DEL_CLASS(t);
	DEL_CLASS(pickObjs);
	return false;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::OnSessionCheck(Net::WebServer::IWebSession* sess, void *userObj)
{
	Data::DateTime *t;
	Data::DateTime currTime;
	t = (Data::DateTime*)sess->GetValuePtr(UTF8STRC("LastUseTime"));
	currTime.SetCurrTimeUTC();
	if (currTime.DiffMS(t) >= 1800000)
		return true;
	return false;
}

IO::ConfigFile *SSWR::OrganMgr::OrganWebHandler::LangGet(Net::WebServer::IWebRequest *req)
{
	Text::StringBuilderUTF8 sb;
	IO::ConfigFile *lang;
	Text::PString sarr[2];
	Text::PString sarr2[2];
	UOSInt i;
	Text::Locale::LocaleEntry *ent;
	if (req->GetHeaderC(&sb, CSTR("Accept-Language")))
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

Text::CString SSWR::OrganMgr::OrganWebHandler::LangGetValue(IO::ConfigFile *lang, const UTF8Char *name, UOSInt nameLen)
{
	if (lang == 0)
		return {name, nameLen};
	Text::String *s = lang->GetValue({name, nameLen});
	if (s)
		return s->ToCString();
	return {name, nameLen};
}

SSWR::OrganMgr::OrganWebHandler::OrganWebHandler(Net::SocketFactory *sockf, Net::SSLEngine *ssl, IO::LogTool *log, DB::DBTool *db, Text::String *imageDir, UInt16 port, UInt16 sslPort, Text::String *cacheDir, Text::String *dataDir, UInt32 scnSize, Text::String *reloadPwd, Int32 unorganizedGroupId, Media::DrawEngine *eng, Text::CString osmCachePath) : csconvColor(Media::ColorProfile::CPT_SRGB)
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

	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Media::ColorProfile destProfile(Media::ColorProfile::CPT_SRGB);
	this->colorSess = this->colorMgr.CreateSess(0);
	NEW_CLASS(this->resizerLR, Media::Resizer::LanczosResizerLR_C32(3, 3, &destProfile, this->colorSess, Media::AT_NO_ALPHA, 0, Media::PF_B8G8R8A8));
	this->csconv = 0;
	this->csconvFCC = 0;
	this->csconvBpp = 0;
	this->csconvPF = Media::PF_UNKNOWN;
	this->eng = eng;

	NEW_CLASS(this->sessMgr, Net::WebServer::MemoryWebSessionManager(CSTR("/"), OnSessionDel, this, 30000, OnSessionCheck, this));
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
		this->HandlePath(CSTR("/map"), this->mapDirHdlr, false);
		this->HandlePath(CSTR("/osm"), this->osmHdlr, false);
		this->AddService(CSTR("/photo.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcPhoto);
		this->AddService(CSTR("/photodown.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcPhotoDown);
		this->AddService(CSTR("/group.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcGroup);
		this->AddService(CSTR("/group.html"), Net::WebUtil::RequestMethod::HTTP_POST, SvcGroup);
		this->AddService(CSTR("/groupmod.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcGroupMod);
		this->AddService(CSTR("/groupmod.html"), Net::WebUtil::RequestMethod::HTTP_POST, SvcGroupMod);
		this->AddService(CSTR("/species.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcSpecies);
		this->AddService(CSTR("/species.html"), Net::WebUtil::RequestMethod::HTTP_POST, SvcSpecies);
		this->AddService(CSTR("/speciesmod.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcSpeciesMod);
		this->AddService(CSTR("/speciesmod.html"), Net::WebUtil::RequestMethod::HTTP_POST, SvcSpeciesMod);
		this->AddService(CSTR("/list.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcList);
		this->AddService(CSTR("/listimage.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcList);
		this->AddService(CSTR("/photodetail.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcPhotoDetail);
		this->AddService(CSTR("/photodetail.html"), Net::WebUtil::RequestMethod::HTTP_POST, SvcPhotoDetail);
		this->AddService(CSTR("/photodetaild.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcPhotoDetailD);
		this->AddService(CSTR("/photoyear.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcPhotoYear);
		this->AddService(CSTR("/photoday.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcPhotoDay);
		this->AddService(CSTR("/photoupload.html"), Net::WebUtil::RequestMethod::HTTP_POST, SvcPhotoUpload);
		this->AddService(CSTR("/photoupload2.html"), Net::WebUtil::RequestMethod::HTTP_POST, SvcPhotoUpload2);
		this->AddService(CSTR("/photouploadd.html"), Net::WebUtil::RequestMethod::HTTP_POST, SvcPhotoUploadD);
		this->AddService(CSTR("/searchinside.html"), Net::WebUtil::RequestMethod::HTTP_POST, SvcSearchInside);
		this->AddService(CSTR("/searchinsidemores.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcSearchInsideMoreS);
		this->AddService(CSTR("/searchinsidemoreg.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcSearchInsideMoreG);
		this->AddService(CSTR("/booklist.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcBookList);
		this->AddService(CSTR("/book.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcBook);
		this->AddService(CSTR("/bookview.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcBookView);
		this->AddService(CSTR("/login.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcLogin);
		this->AddService(CSTR("/login.html"), Net::WebUtil::RequestMethod::HTTP_POST, SvcLogin);
		this->AddService(CSTR("/logout"), Net::WebUtil::RequestMethod::HTTP_GET, SvcLogout);
		this->AddService(CSTR("/reload"), Net::WebUtil::RequestMethod::HTTP_GET, SvcReload);
		this->AddService(CSTR("/reload"), Net::WebUtil::RequestMethod::HTTP_POST, SvcReload);
		this->AddService(CSTR("/restart"), Net::WebUtil::RequestMethod::HTTP_GET, SvcRestart);
		this->AddService(CSTR("/restart"), Net::WebUtil::RequestMethod::HTTP_POST, SvcRestart);
		this->AddService(CSTR("/"), Net::WebUtil::RequestMethod::HTTP_GET, SvcIndex);
		this->AddService(CSTR("/index.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcIndex);
		this->AddService(CSTR("/cate.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcCate);
		this->AddService(CSTR("/favicon.ico"), Net::WebUtil::RequestMethod::HTTP_GET, SvcFavicon);
		this->AddService(CSTR("/publicpoi"), Net::WebUtil::RequestMethod::HTTP_GET, SvcPublicPOI);
		this->AddService(CSTR("/grouppoi"), Net::WebUtil::RequestMethod::HTTP_GET, SvcGroupPOI);
		this->AddService(CSTR("/speciespoi"), Net::WebUtil::RequestMethod::HTTP_GET, SvcSpeciesPOI);

		NEW_CLASS(this->listener, Net::WebServer::WebListener(this->sockf, 0, this, port, 30, 10, CSTR("OrganWeb/1.0"), false, Net::WebServer::KeepAlive::Default, true));
		if (this->ssl && sslPort)
		{
			NEW_CLASS(this->sslListener, Net::WebServer::WebListener(this->sockf, this->ssl, this, sslPort, 30, 10, CSTR("OrganWeb/1.0"), false, Net::WebServer::KeepAlive::Default, true));
		}
		else
		{
			this->sslListener = 0;
		}
		this->Reload();
	}
}

SSWR::OrganMgr::OrganWebHandler::~OrganWebHandler()
{
	SSWR::OrganMgr::OrganWebHandler::CategoryInfo *cate;
	SSWR::OrganMgr::OrganWebHandler::GroupTypeInfo *grpType;
	SSWR::OrganMgr::OrganWebHandler::LocationInfo *loc;
	IO::ConfigFile *lang;
	UOSInt i;
	UOSInt j;

	SDEL_CLASS(this->listener);
	SDEL_CLASS(this->sslListener);
	SDEL_CLASS(this->db);
	SDEL_CLASS(this->sessMgr);
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
	DEL_CLASS(this->resizerLR);
	SDEL_CLASS(this->csconv);
	this->colorMgr.DeleteSess(this->colorSess);
	DEL_CLASS(this->eng);

	this->imageDir->Release();
	this->dataDir->Release();
	SDEL_STRING(this->cacheDir);
	SDEL_STRING(this->reloadPwd);
}

Bool SSWR::OrganMgr::OrganWebHandler::IsError()
{
	if (this->listener == 0)
		return true;
	if (this->listener->IsError())
		return true;
	if (this->sslListener != 0 && this->sslListener->IsError())
		return true;
	return false;
}

void SSWR::OrganMgr::OrganWebHandler::Reload()
{
	this->dataMut.LockWrite();
	this->LoadCategory();
	this->LoadLocations();
	this->LoadSpecies();
	this->LoadGroups();
	this->LoadBooks();
	this->LoadUsers();
	this->dataMut.UnlockWrite();
}

void SSWR::OrganMgr::OrganWebHandler::Restart()
{
	///////////////////////////
}
