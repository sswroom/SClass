#include "Stdafx.h"
#include "Core/DefaultDrawEngine.h"
#include "Crypto/Hash/CRC32R.h"
#include "Crypto/Hash/MD5.h"
#include "Data/ByteTool.h"
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
#include "Net/DefaultSSLEngine.h"
#include "Net/OSSocketFactory.h"
#include "Parser/FullParserList.h"
#include "SSWR/OrganMgr/OrganImageItem.h"
#include "SSWR/OrganMgr/OrganImages.h"
#include "SSWR/OrganMgr/OrganEnv.h"
#include "SSWR/OrganMgr/OrganTripForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "Text/XML.h"
#include "UI/MessageDialog.h"
#include "UI/GUICore.h"

SSWR::OrganMgr::UserFileComparator::~UserFileComparator()
{
}

OSInt SSWR::OrganMgr::UserFileComparator::Compare(UserFileInfo *a, UserFileInfo *b)
{
	if (a->id > b->id)
	{
		return 1;
	}
	else if (a->id < b->id)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

SSWR::OrganMgr::UserFileTimeComparator::~UserFileTimeComparator()
{
}

OSInt SSWR::OrganMgr::UserFileTimeComparator::Compare(UserFileInfo *a, UserFileInfo *b)
{
	if (a->fileTimeTicks > b->fileTimeTicks)
	{
		return 1;
	}
	else if (a->fileTimeTicks < b->fileTimeTicks)
	{
		return -1;
	}
	else if (a->id > b->id)
	{
		return 1;
	}
	else if (a->id < b->id)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

SSWR::OrganMgr::UserFileSpeciesComparator::~UserFileSpeciesComparator()
{
}

OSInt SSWR::OrganMgr::UserFileSpeciesComparator::Compare(UserFileInfo *a, UserFileInfo *b)
{
	if (a->speciesId > b->speciesId)
	{
		return 1;
	}
	else if (a->speciesId < b->speciesId)
	{
		return -1;
	}
	else if (a->id > b->id)
	{
		return 1;
	}
	else if (a->id < b->id)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

SSWR::OrganMgr::WebFileSpeciesComparator::~WebFileSpeciesComparator()
{
}

OSInt SSWR::OrganMgr::WebFileSpeciesComparator::Compare(WebFileInfo *a, WebFileInfo *b)
{
	if (a->speciesId > b->speciesId)
	{
		return 1;
	}
	else if (a->speciesId < b->speciesId)
	{
		return -1;
	}
	else if (a->id > b->id)
	{
		return 1;
	}
	else if (a->id < b->id)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

SSWR::OrganMgr::OrganEnv::OrganEnv()
{
	NEW_CLASS(this->parsers, Parser::FullParserList());
	NEW_CLASS(this->colorMgr, Media::ColorManager());
	this->drawEng = Core::DefaultDrawEngine::CreateDrawEngine();
	NEW_CLASS(this->sockf, Net::OSSocketFactory(true));
	this->ssl = Net::DefaultSSLEngine::Create(this->sockf, true);
	NEW_CLASS(this->monMgr, Media::MonitorMgr());
	this->currCate = 0;
	this->trips = 0;
	this->cateIsFullDir = false;
	this->bookIds = 0;
	this->bookObjs = 0;
	NEW_CLASS(this->dataFiles, Data::ArrayList<DataFileInfo*>());
	NEW_CLASS(this->categories, Data::ArrayList<Category*>());
	NEW_CLASS(this->grpTypes, Data::ArrayList<OrganGroupType*>());
	NEW_CLASS(this->speciesMap, Data::Int32Map<SpeciesInfo*>());
	NEW_CLASS(this->userFileMap, Data::Int32Map<UserFileInfo*>());
	NEW_CLASS(this->userMap, Data::Int32Map<WebUserInfo*>());
	NEW_CLASS(this->gpsStartTime, Data::DateTime());
	NEW_CLASS(this->gpsEndTime, Data::DateTime());
	NEW_CLASS(this->trips, Data::ArrayList<Trip*>());
	NEW_CLASS(this->locs, Data::ArrayList<Location*>());
	NEW_CLASS(this->locType, Data::ArrayList<LocationType*>());
	this->gpsTrk = 0;
	this->gpsUserId = 0;
	this->errType = ERR_NONE;

	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, (const UTF8Char*)"Lang");
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"zh-hk.txt");
	this->langFile = IO::IniFile::Parse(sbuff, 65001);
}

SSWR::OrganMgr::OrganEnv::~OrganEnv()
{
	UOSInt i;
	i = this->categories->GetCount();
	while (i-- > 0)
	{
		this->FreeCategory(this->categories->RemoveAt(i));
	}
	i = this->grpTypes->GetCount();
	while (i-- > 0)
	{
		OrganGroupType *grpType;
		grpType = this->grpTypes->GetItem(i);
		DEL_CLASS(grpType);
	}
	DEL_CLASS(this->grpTypes);
	DataFileInfo *dataFile;
	this->BooksDeinit();
	i = this->dataFiles->GetCount();
	while (i-- > 0)
	{
		dataFile = this->dataFiles->GetItem(i);
		this->ReleaseDataFile(dataFile);
	}
	DEL_CLASS(this->dataFiles);
	SpeciesInfo *species;
	UserFileInfo *userFile;
	WebUserInfo *webUser;
	Data::ArrayList<SpeciesInfo*> *speciesList;
	Data::ArrayList<UserFileInfo*> *userFileList;
	Data::ArrayList<WebUserInfo*> *userList;
	speciesList = this->speciesMap->GetValues();
	i = speciesList->GetCount();
	while (i-- > 0)
	{
		species = speciesList->GetItem(i);
		this->ReleaseSpecies(species);
	}
	DEL_CLASS(this->speciesMap);
	userFileList = this->userFileMap->GetValues();
	i = userFileList->GetCount();
	while (i-- > 0)
	{
		userFile = userFileList->GetItem(i);
		this->ReleaseUserFile(userFile);
	}
	DEL_CLASS(this->userFileMap);
	userList = this->userMap->GetValues();
	i = userList->GetCount();
	while (i-- > 0)
	{
		webUser = userList->GetItem(i);
		DEL_CLASS(webUser->gpsFileIndex);
		DEL_CLASS(webUser->gpsFileObj);
		DEL_CLASS(webUser->userFileIndex);
		DEL_CLASS(webUser->userFileObj);
		MemFree(webUser);
	}
	DEL_CLASS(this->userMap);
	if (this->trips)
	{
		this->TripRelease();
		DEL_CLASS(this->trips);
	}
	DEL_CLASS(this->locs);
	DEL_CLASS(this->locType);

	DEL_CLASS(this->gpsStartTime);
	DEL_CLASS(this->gpsEndTime);
	SDEL_CLASS(this->gpsTrk);
	SDEL_CLASS(this->langFile);
	DEL_CLASS(this->categories);
	DEL_CLASS(this->drawEng);
	DEL_CLASS(this->parsers);
	SDEL_CLASS(this->ssl);
	DEL_CLASS(this->sockf);
	DEL_CLASS(this->colorMgr);
	DEL_CLASS(this->monMgr);
}

Media::DrawEngine *SSWR::OrganMgr::OrganEnv::GetDrawEngine()
{
	return this->drawEng;
}

Parser::ParserList *SSWR::OrganMgr::OrganEnv::GetParserList()
{
	return this->parsers;
}

Net::SocketFactory *SSWR::OrganMgr::OrganEnv::GetSocketFactory()
{
	return this->sockf;
}

Net::SSLEngine *SSWR::OrganMgr::OrganEnv::GetSSLEngine()
{
	return this->ssl;
}

Media::ColorManager *SSWR::OrganMgr::OrganEnv::GetColorMgr()
{
	return this->colorMgr;
}

Media::MonitorMgr *SSWR::OrganMgr::OrganEnv::GetMonitorMgr()
{
	return this->monMgr;
}

SSWR::OrganMgr::OrganEnv::ErrorType SSWR::OrganMgr::OrganEnv::GetErrorType()
{
	return this->errType;
}

const UTF8Char *SSWR::OrganMgr::OrganEnv::GetLang(const UTF8Char *name)
{
	if (this->langFile == 0)
		return name;
	const UTF8Char *ret = this->langFile->GetValue(name);
	if (ret == 0)
		return name;
	return ret;
}

UOSInt SSWR::OrganMgr::OrganEnv::GetCategories(Data::ArrayList<Category*> *categories)
{
	categories->AddAll(this->categories);
	return this->categories->GetCount();
}

Data::ArrayList<SSWR::OrganMgr::OrganGroupType*> *SSWR::OrganMgr::OrganEnv::GetGroupTypes()
{
	return this->grpTypes;
}

Bool SSWR::OrganMgr::OrganEnv::SetSpeciesImg(OrganSpecies *sp, OrganImageItem *img)
{
	UTF8Char sbuff[512];
	UOSInt i;
	if (img->GetFileType() == OrganImageItem::FT_USERFILE)
	{
		sp->SetPhotoId(img->GetUserFile()->id);
		sp->SetPhotoWId(0);
	}
	else if (img->GetFileType() == OrganImageItem::FT_WEBFILE)
	{
		sp->SetPhotoId(0);
		sp->SetPhotoWId(img->GetWebFile()->id);
	}
	else
	{
		img->GetDispName()->ConcatTo(sbuff);
		i = Text::StrLastIndexOf(sbuff, '.');
		if (i != INVALID_INDEX)
		{
			sbuff[i] = 0;
		}
		sp->SetPhoto(sbuff);
		sp->SetPhotoId(0);
		sp->SetPhotoWId(0);
	}
	SaveSpecies(sp);
	return true;
}

Bool SSWR::OrganMgr::OrganEnv::SetSpeciesMapColor(OrganSpecies *sp, UInt32 mapColor)
{
	sp->SetMapColor(mapColor);
	SaveSpecies(sp);
	return true;
}

UOSInt SSWR::OrganMgr::OrganEnv::GetBooksAll(Data::ArrayList<OrganBook*> *items)
{
	items->AddAll(this->bookObjs);
	return this->bookObjs->GetCount();
}

UOSInt SSWR::OrganMgr::OrganEnv::GetBooksOfYear(Data::ArrayList<OrganBook*> *items, Int32 year)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	OrganBook *book;
	k = 0;
	i = 0;
	j = this->bookObjs->GetCount();
	while (i < j)
	{
		book = this->bookObjs->GetItem(i);
		if (book->GetPublishDate()->GetYear() == year)
		{
			items->Add(book);
			k++;
		}
		i++;
	}
	return k;
}

SSWR::OrganMgr::WebUserInfo *SSWR::OrganMgr::OrganEnv::GetWebUser(Int32 userId)
{
	WebUserInfo *webUser = this->userMap->Get(userId);
	if (webUser == 0)
	{
		webUser = MemAlloc(WebUserInfo, 1);
		webUser->id = userId;
		NEW_CLASS(webUser->gpsFileIndex, Data::ArrayListInt64());
		NEW_CLASS(webUser->gpsFileObj, Data::ArrayList<DataFileInfo*>());
		NEW_CLASS(webUser->userFileIndex, Data::ArrayListInt64());
		NEW_CLASS(webUser->userFileObj, Data::ArrayList<UserFileInfo*>());
		this->userMap->Put(webUser->id, webUser);
	}
	return webUser;
}

Data::ArrayList<SSWR::OrganMgr::DataFileInfo*> *SSWR::OrganMgr::OrganEnv::GetDataFiles()
{
	return this->dataFiles;
}

void SSWR::OrganMgr::OrganEnv::ReleaseDataFile(DataFileInfo *dataFile)
{
	dataFile->oriFileName->Release();
	dataFile->fileName->Release();
	MemFree(dataFile);
}

void SSWR::OrganMgr::OrganEnv::ReleaseSpecies(SpeciesInfo *species)
{
	UOSInt i;
	UOSInt j;
	WebFileInfo *webFile;
	Data::ArrayList<WebFileInfo*> *webFiles = species->wfileMap->GetValues();
	i = 0;
	j = webFiles->GetCount();
	while (i < j)
	{
		webFile = webFiles->GetItem(i);
		webFile->location->Release();
		webFile->imgUrl->Release();
		webFile->srcUrl->Release();
		MemFree(webFile);
		i++;
	}
	DEL_CLASS(species->wfileMap);
	DEL_CLASS(species->files);
	MemFree(species);
}

void SSWR::OrganMgr::OrganEnv::ReleaseUserFile(UserFileInfo *userFile)
{
	userFile->oriFileName->Release();
	userFile->dataFileName->Release();
	SDEL_STRING(userFile->camera);
	SDEL_STRING(userFile->descript);
	SDEL_STRING(userFile->location);
	MemFree(userFile);
}

UOSInt SSWR::OrganMgr::OrganEnv::GetUserFiles(Data::ArrayList<UserFileInfo*> *userFiles, Int64 fromTimeTicks, Int64 toTimeTicks)
{
	Data::ArrayList<UserFileInfo *> *userFileList = this->userFileMap->GetValues();
	UserFileInfo *userFile;
	UOSInt initCnt = userFiles->GetCount();
	UOSInt i;
	UOSInt j;
	i = 0;
	j = userFileList->GetCount();
	while (i < j)
	{
		userFile = userFileList->GetItem(i);
		if (userFile->webuserId == this->userId && userFile->fileTimeTicks >= fromTimeTicks && userFile->fileTimeTicks <= toTimeTicks)
		{
			userFiles->Add(userFile);
		}
		i++;
	}
	return userFiles->GetCount() - initCnt;
}

void SSWR::OrganMgr::OrganEnv::TripRelease()
{
	UOSInt i;
	Trip *trip;
	Location *loc;
	LocationType *locTyp;
	i = this->trips->GetCount();
	while (i-- > 0)
	{
		trip = this->trips->RemoveAt(i);
		DEL_CLASS(trip);
	}
	i = this->locs->GetCount();
	while (i-- > 0)
	{
		loc = this->locs->RemoveAt(i);
		DEL_CLASS(loc);
	}
	i = this->locType->GetCount();
	while (i-- > 0)
	{
		locTyp = this->locType->RemoveAt(i);
		DEL_CLASS(locTyp);
	}
}

OSInt SSWR::OrganMgr::OrganEnv::TripGetIndex(Data::DateTime *d)
{
	Int64 ts = d->ToUnixTimestamp();
	OSInt i = 0;
	OSInt j = (OSInt)this->trips->GetCount() - 1;
	OSInt k;
	Trip *t;
	while (i <= j)
	{
		k = (i + j) >> 1;
		t = this->trips->GetItem((UOSInt)k);
		if (t->fromDate > ts)
		{
			j = k - 1;
		}
		else if (t->toDate >= ts)
		{
			return k;
		}
		else
		{
			i = k + 1;
		}
	}
	return -i - 1;
}

SSWR::OrganMgr::Trip *SSWR::OrganMgr::OrganEnv::TripGet(Int32 userId, Data::DateTime *d)
{
	OSInt i = this->TripGetIndex(d);
	if (i < 0)
		return 0;
	else
		return this->trips->GetItem((UOSInt)i);
}

Data::ArrayList<SSWR::OrganMgr::Trip*> *SSWR::OrganMgr::OrganEnv::TripGetList()
{
	return this->trips;
}

OSInt SSWR::OrganMgr::OrganEnv::LocationGetIndex(Int32 locId)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->locs->GetCount() - 1;
	OSInt k;
	Int32 l;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = this->locs->GetItem((UOSInt)k)->id;
		if (locId > l)
		{
			i = k + 1;
		}
		else if (locId < l)
		{
			j = k - 1;
		}
		else
		{
			return k;
		}
	}
	return -i - 1;
}

SSWR::OrganMgr::Location *SSWR::OrganMgr::OrganEnv::LocationGet(Int32 locId)
{
	OSInt i = LocationGetIndex(locId);
	if (i < 0)
		return 0;
	else
		return this->locs->GetItem((UOSInt)i);
}

Data::ArrayList<SSWR::OrganMgr::Location*> *SSWR::OrganMgr::OrganEnv::LocationGetSub(Int32 locId)
{
	Data::ArrayList<Location *> *outArr;
	NEW_CLASS(outArr, Data::ArrayList<Location*>());
	UOSInt i = 0;
	UOSInt j = this->locs->GetCount();
	Location *loc;
	while (i < j)
	{
		loc = this->locs->GetItem(i);
		if (loc->parId == locId)
			outArr->Add(loc);
		i += 1;
	}
	return outArr;
}

OSInt SSWR::OrganMgr::OrganEnv::LocationGetTypeIndex(Int32 lType)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->locType->GetCount() - 1;
	OSInt k;
	Int32 l;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = this->locType->GetItem((UOSInt)k)->id;
		if (l > lType)
		{
			j = k - 1;
		}
		else if (l < lType)
		{
			i = k + 1;
		}
		else
		{
			return k;
		}
	}
	return -i - 1;
}

SSWR::OrganMgr::SpeciesInfo *SSWR::OrganMgr::OrganEnv::GetSpeciesInfo(Int32 speciesId, Bool createNew)
{
	SpeciesInfo *sp;
	sp = this->speciesMap->Get(speciesId);
	if (sp == 0 && createNew)
	{
		sp = MemAlloc(SpeciesInfo, 1);
		sp->id = speciesId;
		NEW_CLASS(sp->files, Data::ArrayList<UserFileInfo*>());
		NEW_CLASS(sp->wfileMap, Data::Int32Map<WebFileInfo*>());
		this->speciesMap->Put(sp->id, sp);
	}
	return sp;
}

void SSWR::OrganMgr::OrganEnv::BooksDeinit()
{
	OrganBook *book;
	UOSInt i;
	if (this->bookObjs)
	{
		i = this->bookObjs->GetCount();
		while (i-- > 0)
		{
			book = this->bookObjs->GetItem(i);
			DEL_CLASS(book);
		}
		DEL_CLASS(this->bookObjs);
		DEL_CLASS(this->bookIds);
		this->bookIds = 0;
		this->bookObjs = 0;
	}
}

Text::String *SSWR::OrganMgr::OrganEnv::GetLocName(Int32 userId, Data::DateTime *dt, UI::GUIForm *ownerFrm, UI::GUICore *ui)
{
	Trip *tr = this->TripGet(userId, dt);
	if (tr)
	{
		return this->LocationGet(tr->locId)->cname;
	}
	else if (userId == this->userId)
	{
		Text::StringBuilderUTF8 sb;
		OrganTripForm *frm;
		Data::DateTime dt2(dt);
		Data::DateTime dt3(dt);
		dt2.ClearTime();
		dt3.ClearTime();
		dt3.AddDay(1);
		NEW_CLASS(frm, OrganTripForm(0, ui, this));
		sb.Append((const UTF8Char*)"Trip not found at ");
		sb.AppendDate(dt);
		frm->SetText(sb.ToString());
		frm->SetTimes(&dt2, &dt3);
		frm->ShowDialog(ownerFrm);
		DEL_CLASS(frm);
		tr = this->TripGet(userId, dt);
		if (tr)
		{
			return this->LocationGet(tr->locId)->cname;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

void SSWR::OrganMgr::OrganEnv::SetCurrCategory(Category *currCate)
{
	if (currCate)
	{
		this->currCate = currCate;
		this->cateIsFullDir = this->currCate->srcDir->IndexOf((const UTF8Char*)":\\") != INVALID_INDEX;
		this->TripReload(this->currCate->cateId);
		this->LoadGroupTypes();
	}
}


void SSWR::OrganMgr::OrganEnv::FreeCategory(Category *cate)
{
	SDEL_STRING(cate->chiName);
	SDEL_STRING(cate->dirName);
	SDEL_STRING(cate->srcDir);
	MemFree(cate);
}

Media::EXIFData *SSWR::OrganMgr::OrganEnv::ParseJPGExif(const UTF8Char *fileName)
{
	IO::StmData::FileData *fd;
	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	Media::EXIFData *exif = Media::JPEGFile::ParseJPEGExif(fd);
	DEL_CLASS(fd);
	return exif;
}

Media::EXIFData *SSWR::OrganMgr::OrganEnv::ParseTIFExif(const UTF8Char *fileName)
{
	return 0;
	//////////////////////////////////
}

void SSWR::OrganMgr::OrganEnv::ExportWeb(const UTF8Char *exportDir, Bool includeWebPhoto, Bool includeNoPhoto, Int32 locId, UOSInt *photoCnt, UOSInt *speciesCnt)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr = Text::StrConcat(sbuff, exportDir);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	Text::StrConcat(sptr, (const UTF8Char*)"indexhd");
	IO::Path::CreateDirectory(sbuff);

	Text::StrConcat(sptr, (const UTF8Char*)"indexhd.html");

	Text::UTF8Writer *writer;
	IO::FileStream *fs;
	UOSInt photoParsed = 0;
	UOSInt speciesParsed = 0;
	UOSInt thisPhotoCnt;
	UOSInt thisSpeciesCnt;
	UOSInt thisPhSpeciesCnt;
	Text::StringBuilderUTF8 *sb;

	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(writer, Text::UTF8Writer(fs));

	ExportBeginPage(writer, this->currCate->chiName->v);
	
	OrganGroup *grp;
	UOSInt i;
	UOSInt j;
	const UTF8Char *csptr;
	Data::Int32Map<Data::ArrayList<OrganGroup*>*> *grpTree;
	Data::Int32Map<Data::ArrayList<OrganSpecies*>*> *spTree;
	Data::ArrayList<OrganGroup*> *grps;

	NEW_CLASS(sb, Text::StringBuilderUTF8());

	grpTree = GetGroupTree();
	spTree = GetSpeciesTree();

	grps = grpTree->Get(0);

	if (grps)
	{
		i = 0;
		j = grps->GetCount();
		while (i < j)
		{
			grp = grps->GetItem(i);
			ExportGroup(grp, grpTree, spTree, (const UTF8Char*)"../../index.html", sbuff, sptr, includeWebPhoto, includeNoPhoto, locId, &thisPhotoCnt, &thisSpeciesCnt, &thisPhSpeciesCnt);
			if (thisSpeciesCnt > 0)
			{
				photoParsed += thisPhotoCnt;
				speciesParsed += thisSpeciesCnt;

				writer->Write((const UTF8Char*)"<a href=");
				sb->ClearStr();
				sb->Append((const UTF8Char*)"indexhd/grp");
				sb->AppendI32(grp->GetGroupId());
				sb->Append((const UTF8Char*)"/index.html");
				csptr = Text::XML::ToNewAttrText(sb->ToString());
				writer->Write(csptr);
				Text::XML::FreeNewText(csptr);
				writer->Write((const UTF8Char*)">");

				sb->ClearStr();
				sb->Append(grp->GetCName());
				sb->Append((const UTF8Char*)" ");
				sb->Append(grp->GetEName());
				sb->Append((const UTF8Char*)" (");
				sb->AppendUOSInt(thisPhSpeciesCnt);
				sb->Append((const UTF8Char*)"/");
				sb->AppendUOSInt(thisSpeciesCnt);
				sb->Append((const UTF8Char*)")");
				csptr = Text::XML::ToNewXMLText(sb->ToString());
				writer->Write(csptr);
				Text::XML::FreeNewText(csptr);
				writer->WriteLine((const UTF8Char*)"</a><br/>");
			}

			i++;
		}
	}
	FreeSpeciesTree(spTree);
	FreeGroupTree(grpTree);
	DEL_CLASS(sb);

	ExportEndPage(writer);
	DEL_CLASS(writer);
	DEL_CLASS(fs);
	*photoCnt = photoParsed;
	*speciesCnt = speciesParsed;
}

void SSWR::OrganMgr::OrganEnv::FreeGroupTree(Data::Int32Map<Data::ArrayList<OrganGroup*>*> *grpTree)
{
	OrganGroup *grp;
	Data::ArrayList<OrganGroup*> *grps;
	Data::ArrayList<Data::ArrayList<OrganGroup*>*> *grpsList;
	UOSInt i;
	UOSInt j;

	grpsList = grpTree->GetValues();
	i = grpsList->GetCount();
	while (i-- > 0)
	{
		grps = grpsList->GetItem(i);
		j = grps->GetCount();
		while (j-- > 0)
		{
			grp = grps->GetItem(j);
			DEL_CLASS(grp);
		}
		DEL_CLASS(grps);
	}
	DEL_CLASS(grpTree);
}

void SSWR::OrganMgr::OrganEnv::FreeSpeciesTree(Data::Int32Map<Data::ArrayList<OrganSpecies*>*> *spTree)
{
	OrganSpecies *sp;
	Data::ArrayList<OrganSpecies*> *sps;
	Data::ArrayList<Data::ArrayList<OrganSpecies*>*> *spsList;
	UOSInt i;
	UOSInt j;

	spsList = spTree->GetValues();
	i = spsList->GetCount();
	while (i-- > 0)
	{
		sps = spsList->GetItem(i);
		j = sps->GetCount();
		while (j-- > 0)
		{
			sp = sps->GetItem(j);
			DEL_CLASS(sp);
		}
		DEL_CLASS(sps);
	}
	DEL_CLASS(spTree);
}

void SSWR::OrganMgr::OrganEnv::ExportBeginPage(IO::Writer *writer, const UTF8Char *title)
{
	const UTF8Char *csptr;
	writer->WriteLine((const UTF8Char*)"<HTML>");
	writer->WriteLine((const UTF8Char*)"<HEAD>");
	writer->WriteLine((const UTF8Char*)"<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=utf8\">");
	writer->Write((const UTF8Char*)"<title>");
	csptr = Text::XML::ToNewXMLText(title);
	writer->Write(csptr);
	writer->WriteLine((const UTF8Char*)"</title>");
	writer->WriteLine((const UTF8Char*)"</HEAD>");
	writer->WriteLine();
	writer->WriteLine((const UTF8Char*)"<BODY TEXT=\"#c0e0ff\" LINK=\"#6080ff\" VLINK=\"#4060ff\" ALINK=\"#4040FF\" bgcolor=\"#000000\">");
	writer->Write((const UTF8Char*)"<center><h1>");
	writer->Write(csptr);
	writer->WriteLine((const UTF8Char*)"</h1></center>");
	Text::XML::FreeNewText(csptr);
}

void SSWR::OrganMgr::OrganEnv::ExportEndPage(IO::Writer *writer)
{
	writer->WriteLine((const UTF8Char*)"</BODY>");
	writer->WriteLine((const UTF8Char*)"</HTML>");
}

void SSWR::OrganMgr::OrganEnv::ExportGroup(OrganGroup *grp, Data::Int32Map<Data::ArrayList<OrganGroup*>*> *grpTree, Data::Int32Map<Data::ArrayList<OrganSpecies*>*> *spTree, const UTF8Char *backURL, UTF8Char *fullPath, UTF8Char *pathAppend, Bool includeWebPhoto, Bool includeNoPhoto, Int32 locId, UOSInt *photoCnt, UOSInt *speciesCnt, UOSInt *phSpeciesCnt)
{
	UOSInt totalPhoto = 0;
	UOSInt totalSpecies = 0;
	UOSInt totalPhSpecies = 0;
	UOSInt thisPhoto;
	UOSInt thisSpecies;
	UOSInt thisPhSpecies;
	UOSInt i;
	UOSInt j;
	OrganGroup *myGrp;
	OrganSpecies *sp;

	IO::FileStream *fs = 0;
	Text::UTF8Writer *writer = 0;
	Text::StringBuilderUTF8 *sb;
	UTF8Char *sptr;
	const UTF8Char *u8ptr;
	UTF8Char backBuff[64];
	Text::StrConcat(Text::StrInt32(Text::StrConcat(backBuff, (const UTF8Char*)"../../indexhd/grp"), grp->GetGroupId()), (const UTF8Char*)"/index.html");

	Data::ArrayList<OrganGroup*> *grps;
	Data::ArrayList<OrganSpecies*> *sps;

	if ((grps = grpTree->Get(grp->GetGroupId())) != 0)
	{
		i = 0;
		j = grps->GetCount();
		while (i < j)
		{
			myGrp = grps->GetItem(i);
			ExportGroup(myGrp, grpTree, spTree, backBuff, fullPath, pathAppend, includeWebPhoto, includeNoPhoto, locId, &thisPhoto, &thisSpecies, &thisPhSpecies);

			if (thisSpecies > 0)
			{
				totalPhoto += thisPhoto;
				totalSpecies += thisSpecies;
				totalPhSpecies += thisPhSpecies;

				if (fs == 0)
				{
					sptr = Text::StrInt32(Text::StrConcat(pathAppend, (const UTF8Char*)"indexhd\\grp"), grp->GetGroupId());
					IO::Path::CreateDirectory(fullPath);
					Text::StrConcat(sptr, (const UTF8Char*)"\\index.html");

					NEW_CLASS(fs, IO::FileStream(fullPath, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
					NEW_CLASS(writer, Text::UTF8Writer(fs));
					NEW_CLASS(sb, Text::StringBuilderUTF8());
					sb->AppendC(this->currCate->chiName->v, this->currCate->chiName->leng);
					sb->Append((const UTF8Char*)" - ");
					sb->Append(grp->GetCName());
					sb->Append((const UTF8Char*)" ");
					sb->Append(grp->GetEName());
					ExportBeginPage(writer, sb->ToString());
				}

				writer->Write((const UTF8Char*)"<a href=");
				sb->ClearStr();
				sb->Append((const UTF8Char*)"../grp");
				sb->AppendI32(myGrp->GetGroupId());
				sb->Append((const UTF8Char*)"/index.html");
				u8ptr = Text::XML::ToNewAttrText(sb->ToString());
				writer->Write(u8ptr);
				Text::XML::FreeNewText(u8ptr);
				writer->Write((const UTF8Char*)">");

				sb->ClearStr();
				sb->Append(myGrp->GetCName());
				sb->Append((const UTF8Char*)" ");
				sb->Append(myGrp->GetEName());
				sb->Append((const UTF8Char*)" (");
				sb->AppendUOSInt(thisPhSpecies);
				sb->Append((const UTF8Char*)"/");
				sb->AppendUOSInt(thisSpecies);
				sb->Append((const UTF8Char*)")");
				u8ptr = Text::XML::ToNewXMLText(sb->ToString());
				writer->Write(u8ptr);
				Text::XML::FreeNewText(u8ptr);
				writer->WriteLine((const UTF8Char*)"</a><br/>");
			}

			i++;
		}
	}
	else if ((sps = spTree->Get(grp->GetGroupId())) != 0)
	{
		Bool hasMyPhoto;
		i = 0;
		j = sps->GetCount();
		while (i < j)
		{
			sp = sps->GetItem(i);
			if (ExportSpecies(sp, backBuff, fullPath, pathAppend, includeWebPhoto, includeNoPhoto, locId, &thisPhoto, &hasMyPhoto))
			{
				if (fs == 0)
				{
					sptr = Text::StrInt32(Text::StrConcat(pathAppend, (const UTF8Char*)"indexhd\\grp"), grp->GetGroupId());
					IO::Path::CreateDirectory(fullPath);
					Text::StrConcat(sptr, (const UTF8Char*)"\\index.html");

					NEW_CLASS(fs, IO::FileStream(fullPath, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
					NEW_CLASS(writer, Text::UTF8Writer(fs));
					NEW_CLASS(sb, Text::StringBuilderUTF8());
					sb->AppendC(this->currCate->chiName->v, this->currCate->chiName->leng);
					sb->Append((const UTF8Char*)" - ");
					sb->Append(grp->GetCName());
					sb->Append((const UTF8Char*)" ");
					sb->Append(grp->GetEName());
					ExportBeginPage(writer, sb->ToString());
				}
				
				u8ptr = sp->GetDirName();
				sptr = pathAppend;
				*sptr++ = u8ptr[0];
				*sptr++ = u8ptr[1];
				*sptr = 0;
				sb->ClearStr();
				sb->Append((const UTF8Char*)"../../");
				sb->Append(pathAppend);
				sb->Append((const UTF8Char*)"/");
				sb->Append(u8ptr);
				sb->Append((const UTF8Char*)"/index.html");
				writer->Write((const UTF8Char*)"<a href=");
				u8ptr = Text::XML::ToNewAttrText(sb->ToString());
				writer->Write(u8ptr);
				writer->Write((const UTF8Char*)">");
				Text::XML::FreeNewText(u8ptr);
				sb->ClearStr();
				sb->Append(sp->GetSName());
				sb->Append((const UTF8Char*)" ");
				sb->Append(sp->GetCName());
				if (sp->GetEName())
				{
					sb->Append((const UTF8Char*)" ");
					sb->Append(sp->GetEName());
				}
				u8ptr = Text::XML::ToNewXMLText(sb->ToString());
				writer->Write(u8ptr);
				Text::XML::FreeNewText(u8ptr);
				writer->WriteLine((const UTF8Char*)"</a><br/>");

				totalPhoto += thisPhoto;
				totalSpecies++;
				if (hasMyPhoto)
				{
					totalPhSpecies++;
				}
			}
			i++;
		}
	}

	if (fs)
	{
		writer->WriteLineW(L"<br/><a href=\"list.html\">すべて</a><br>");
		writer->Write((const UTF8Char*)"<br><a href=");
		u8ptr = Text::XML::ToNewAttrText(backURL);
		writer->Write(u8ptr);
		Text::XML::FreeNewText(u8ptr);
		writer->WriteLineW(L">戻る</a>");

		ExportEndPage(writer);
		DEL_CLASS(writer);
		DEL_CLASS(fs);
		DEL_CLASS(sb);
	}

	*photoCnt = totalPhoto;
	*speciesCnt = totalSpecies;
	*phSpeciesCnt = totalPhSpecies;
}

Bool SSWR::OrganMgr::OrganEnv::ExportSpecies(OrganSpecies *sp, const UTF8Char *backURL, UTF8Char *fullPath, UTF8Char *pathAppend, Bool includeWebPhoto, Bool includeNoPhoto, Int32 locId, UOSInt *photoCnt, Bool *hasMyPhoto)
{
	UOSInt i;
	UOSInt j;
	OrganImageItem *imgItem;
	OrganImageItem::FileType ft;
	Data::ArrayList<OrganImageItem*> items;
	Bool myPhotoExist = false;
	const UTF8Char *csptr;
	UTF8Char *sptr;
	GetSpeciesImages(&items, sp);

	i = items.GetCount();
	while (i-- > 0)
	{
		imgItem = items.GetItem(i);
		ft = imgItem->GetFileType();
		if (ft == OrganImageItem::FT_JPG || ft == OrganImageItem::FT_TIF)
		{
			myPhotoExist = true;
		}
		else if (ft == OrganImageItem::FT_WEB_IMAGE)
		{
			if (!includeWebPhoto)
			{
				DEL_CLASS(imgItem);
				items.RemoveAt(i);
			}
		}
		else
		{
			DEL_CLASS(imgItem);
			items.RemoveAt(i);
		}
	}

	if (items.GetCount() == 0 && !includeNoPhoto)
	{
		return false;
	}

	csptr = sp->GetDirName();
	sptr = pathAppend;
	*sptr++ = csptr[0];
	*sptr++ = csptr[1];
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcat(sptr, csptr);
	IO::Path::CreateDirectory(fullPath);

	*sptr++ = IO::Path::PATH_SEPERATOR;
	Text::StrConcat(sptr, (const UTF8Char*)"index.html");

	IO::FileStream *fs;
	Text::UTF8Writer *writer;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(fs, IO::FileStream(fullPath, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(writer, Text::UTF8Writer(fs));
	sb.AppendC(this->currCate->chiName->v, this->currCate->chiName->leng);
	sb.Append((const UTF8Char*)" - ");
	sb.Append(sp->GetSName());
	sb.Append((const UTF8Char*)" ");
	sb.Append(sp->GetCName());
	if (sp->GetEName())
	{
		sb.Append((const UTF8Char*)" ");
		sb.Append(sp->GetEName());
	}
	ExportBeginPage(writer, sb.ToString());

	i = 0;
	j = items.GetCount();
	while (i < j)
	{
		i++;
	}


	writer->Write((const UTF8Char*)"<a href=");
	csptr = Text::XML::ToNewAttrText(backURL);
	writer->Write(csptr);
	Text::XML::FreeNewText(csptr);
	writer->WriteW(L">戻る</a><br/>");

	ExportEndPage(writer);

	DEL_CLASS(writer);
	DEL_CLASS(fs);

	*photoCnt = items.GetCount();
	*hasMyPhoto = myPhotoExist;

	i = items.GetCount();
	while (i-- > 0)
	{
		imgItem = items.GetItem(i);
		DEL_CLASS(imgItem);
	}

	return true;
}

Double SSWR::OrganMgr::OrganEnv::GetMonitorHDPI(MonitorHandle *hMonitor)
{
	return this->monMgr->GetMonitorHDPI(hMonitor);
}

Double SSWR::OrganMgr::OrganEnv::GetMonitorDDPI(MonitorHandle *hMonitor)
{
	return this->monMgr->GetMonitorDDPI(hMonitor);
}

void SSWR::OrganMgr::OrganEnv::SetMonitorHDPI(MonitorHandle *hMonitor, Double monitorHDPI)
{
	this->monMgr->SetMonitorHDPI(hMonitor, monitorHDPI);
}
