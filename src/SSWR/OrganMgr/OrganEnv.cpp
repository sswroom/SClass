#include "Stdafx.h"
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
#include "Map/GPSTrack.h"
#include "Media/DrawEngineFactory.h"
#include "Media/FrequencyGraph.h"
#include "Media/JPEGFile.h"
#include "Media/MediaFile.h"
#include "Media/StaticImage.h"
#include "Media/Resizer/LanczosResizerH8_8.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
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

OSInt SSWR::OrganMgr::UserFileComparator::Compare(UserFileInfo *a, UserFileInfo *b) const
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

OSInt SSWR::OrganMgr::UserFileTimeComparator::Compare(UserFileInfo *a, UserFileInfo *b) const
{
	if (a->fileTime > b->fileTime)
	{
		return 1;
	}
	else if (a->fileTime < b->fileTime)
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

OSInt SSWR::OrganMgr::UserFileSpeciesComparator::Compare(UserFileInfo *a, UserFileInfo *b) const
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

OSInt SSWR::OrganMgr::WebFileSpeciesComparator::Compare(WebFileInfo *a, WebFileInfo *b) const
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
	this->drawEng = Media::DrawEngineFactory::CreateDrawEngine();
	NEW_CLASSNN(this->sockf, Net::OSSocketFactory(true));
	this->ssl = Net::SSLEngineFactory::Create(this->sockf, true);
	this->currCate = 0;
	this->cateIsFullDir = false;
	this->bookIds = 0;
	this->bookObjs = 0;
	this->gpsTrk = 0;
	this->gpsUserId = 0;
	this->errType = ERR_NONE;

	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("Lang"));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(sptr, UTF8STRC("zh-hk.txt"));
	this->langFile = IO::IniFile::Parse(CSTRP(sbuff, sptr), 65001);
}

SSWR::OrganMgr::OrganEnv::~OrganEnv()
{
	UOSInt i;
	i = this->categories.GetCount();
	while (i-- > 0)
	{
		this->FreeCategory(this->categories.RemoveAt(i));
	}
	i = this->grpTypes.GetCount();
	while (i-- > 0)
	{
		OrganGroupType *grpType;
		grpType = this->grpTypes.GetItem(i);
		DEL_CLASS(grpType);
	}
	DataFileInfo *dataFile;
	this->BooksDeinit();
	i = this->dataFiles.GetCount();
	while (i-- > 0)
	{
		dataFile = this->dataFiles.GetItem(i);
		this->ReleaseDataFile(dataFile);
	}
	SpeciesInfo *species;
	UserFileInfo *userFile;
	WebUserInfo *webUser;
	i = this->speciesMap.GetCount();
	while (i-- > 0)
	{
		species = this->speciesMap.GetItem(i);
		this->ReleaseSpecies(species);
	}
	i = this->userFileMap.GetCount();
	while (i-- > 0)
	{
		userFile = this->userFileMap.GetItem(i);
		this->ReleaseUserFile(userFile);
	}
	i = this->userMap.GetCount();
	while (i-- > 0)
	{
		webUser = this->userMap.GetItem(i);
		DEL_CLASS(webUser);
	}
	this->TripRelease();
	SDEL_CLASS(this->gpsTrk);
	SDEL_CLASS(this->langFile);
	DEL_CLASS(this->drawEng);
	SDEL_CLASS(this->ssl);
	this->sockf.Delete();
}

Media::DrawEngine *SSWR::OrganMgr::OrganEnv::GetDrawEngine()
{
	return this->drawEng;
}

Parser::ParserList *SSWR::OrganMgr::OrganEnv::GetParserList()
{
	return &this->parsers;
}

NotNullPtr<Net::SocketFactory> SSWR::OrganMgr::OrganEnv::GetSocketFactory()
{
	return this->sockf;
}

Net::SSLEngine *SSWR::OrganMgr::OrganEnv::GetSSLEngine()
{
	return this->ssl;
}

Media::ColorManager *SSWR::OrganMgr::OrganEnv::GetColorMgr()
{
	return &this->colorMgr;
}

Media::MonitorMgr *SSWR::OrganMgr::OrganEnv::GetMonitorMgr()
{
	return &this->monMgr;
}

SSWR::OrganMgr::OrganEnv::ErrorType SSWR::OrganMgr::OrganEnv::GetErrorType()
{
	return this->errType;
}

Text::CString SSWR::OrganMgr::OrganEnv::GetLang(const UTF8Char *name, UOSInt nameLen)
{
	if (this->langFile == 0)
		return {name, nameLen};
	Text::String *ret = this->langFile->GetValue({name, nameLen});
	if (ret == 0)
		return {name, nameLen};
	return ret->ToCString();
}

UOSInt SSWR::OrganMgr::OrganEnv::GetCategories(Data::ArrayList<Category*> *categories)
{
	categories->AddAll(&this->categories);
	return this->categories.GetCount();
}

Data::ArrayList<SSWR::OrganMgr::OrganGroupType*> *SSWR::OrganMgr::OrganEnv::GetGroupTypes()
{
	return &this->grpTypes;
}

Bool SSWR::OrganMgr::OrganEnv::SetSpeciesImg(OrganSpecies *sp, OrganImageItem *img)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	if (img->GetFileType() == OrganImageItem::FileType::UserFile)
	{
		sp->SetPhotoId(img->GetUserFile()->id);
		sp->SetPhotoWId(0);
	}
	else if (img->GetFileType() == OrganImageItem::FileType::WebFile)
	{
		sp->SetPhotoId(0);
		sp->SetPhotoWId(img->GetWebFile()->id);
	}
	else
	{
		sptr = img->GetDispName()->ConcatTo(sbuff);
		i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '.');
		if (i != INVALID_INDEX)
		{
			sbuff[i] = 0;
			sptr = &sbuff[i];
		}
		sp->SetPhoto(CSTRP(sbuff, sptr));
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
		if (book->GetPublishDate().GetTimeValue().year == year)
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
	WebUserInfo *webUser = this->userMap.Get(userId);
	if (webUser == 0)
	{
		NEW_CLASS(webUser, WebUserInfo());
		webUser->id = userId;
		this->userMap.Put(webUser->id, webUser);
	}
	return webUser;
}

Data::ArrayList<SSWR::OrganMgr::DataFileInfo*> *SSWR::OrganMgr::OrganEnv::GetDataFiles()
{
	return &this->dataFiles;
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
	i = 0;
	j = species->wfileMap.GetCount();
	while (i < j)
	{
		webFile = species->wfileMap.GetItem(i);
		webFile->location->Release();
		webFile->imgUrl->Release();
		webFile->srcUrl->Release();
		MemFree(webFile);
		i++;
	}
	DEL_CLASS(species);
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

UOSInt SSWR::OrganMgr::OrganEnv::GetUserFiles(Data::ArrayList<UserFileInfo*> *userFiles, const Data::Timestamp &fromTime, const Data::Timestamp &toTime)
{
	UserFileInfo *userFile;
	UOSInt initCnt = userFiles->GetCount();
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->userFileMap.GetCount();
	while (i < j)
	{
		userFile = this->userFileMap.GetItem(i);
		if (userFile->webuserId == this->userId && userFile->fileTime >= fromTime && userFile->fileTime <= toTime)
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
	i = this->trips.GetCount();
	while (i-- > 0)
	{
		trip = this->trips.RemoveAt(i);
		DEL_CLASS(trip);
	}
	i = this->locs.GetCount();
	while (i-- > 0)
	{
		loc = this->locs.RemoveAt(i);
		DEL_CLASS(loc);
	}
	i = this->locType.GetCount();
	while (i-- > 0)
	{
		locTyp = this->locType.RemoveAt(i);
		DEL_CLASS(locTyp);
	}
}

OSInt SSWR::OrganMgr::OrganEnv::TripGetIndex(const Data::Timestamp &ts)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->trips.GetCount() - 1;
	OSInt k;
	Trip *t;
	while (i <= j)
	{
		k = (i + j) >> 1;
		t = this->trips.GetItem((UOSInt)k);
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

SSWR::OrganMgr::Trip *SSWR::OrganMgr::OrganEnv::TripGet(Int32 userId, const Data::Timestamp &ts)
{
	OSInt i = this->TripGetIndex(ts);
	if (i < 0)
		return 0;
	else
		return this->trips.GetItem((UOSInt)i);
}

Data::ArrayList<SSWR::OrganMgr::Trip*> *SSWR::OrganMgr::OrganEnv::TripGetList()
{
	return &this->trips;
}

OSInt SSWR::OrganMgr::OrganEnv::LocationGetIndex(Int32 locId)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->locs.GetCount() - 1;
	OSInt k;
	Int32 l;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = this->locs.GetItem((UOSInt)k)->id;
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
		return this->locs.GetItem((UOSInt)i);
}

Data::ArrayList<SSWR::OrganMgr::Location*> *SSWR::OrganMgr::OrganEnv::LocationGetSub(Int32 locId)
{
	Data::ArrayList<Location *> *outArr;
	NEW_CLASS(outArr, Data::ArrayList<Location*>());
	UOSInt i = 0;
	UOSInt j = this->locs.GetCount();
	Location *loc;
	while (i < j)
	{
		loc = this->locs.GetItem(i);
		if (loc->parId == locId)
			outArr->Add(loc);
		i += 1;
	}
	return outArr;
}

OSInt SSWR::OrganMgr::OrganEnv::LocationGetTypeIndex(Int32 lType)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->locType.GetCount() - 1;
	OSInt k;
	Int32 l;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = this->locType.GetItem((UOSInt)k)->id;
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
	sp = this->speciesMap.Get(speciesId);
	if (sp == 0 && createNew)
	{
		NEW_CLASS(sp, SpeciesInfo());
		sp->id = speciesId;
		this->speciesMap.Put(sp->id, sp);
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

Text::String *SSWR::OrganMgr::OrganEnv::GetLocName(Int32 userId, const Data::Timestamp &ts, UI::GUIForm *ownerFrm, UI::GUICore *ui)
{
	Trip *tr = this->TripGet(userId, ts);
	if (tr)
	{
		return this->LocationGet(tr->locId)->cname.Ptr();
	}
	else if (userId == this->userId)
	{
		Text::StringBuilderUTF8 sb;
		Data::Timestamp ts2 = ts.ClearTimeLocal();
		Data::Timestamp ts3 = ts2.AddDay(1);
		{
			OrganTripForm frm(0, ui, this);
			sb.AppendC(UTF8STRC("Trip not found at "));
			sb.AppendTS(ts);
			frm.SetText(sb.ToCString());
			frm.SetTimes(ts, ts2, ts3);
			frm.ShowDialog(ownerFrm);
		}
		tr = this->TripGet(userId, ts);
		if (tr)
		{
			return this->LocationGet(tr->locId)->cname.Ptr();
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
		this->cateIsFullDir = this->currCate->srcDir->IndexOf(UTF8STRC(":\\")) != INVALID_INDEX;
		this->TripReload(this->currCate->cateId);
		this->LoadGroupTypes();
	}
}


void SSWR::OrganMgr::OrganEnv::FreeCategory(Category *cate)
{
	cate->chiName->Release();
	cate->dirName->Release();
	SDEL_STRING(cate->srcDir);
	MemFree(cate);
}

Media::EXIFData *SSWR::OrganMgr::OrganEnv::ParseJPGExif(Text::CString fileName)
{
	IO::StmData::FileData *fd;
	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	Media::EXIFData *exif = Media::JPEGFile::ParseJPEGExif(fd);
	DEL_CLASS(fd);
	return exif;
}

Media::EXIFData *SSWR::OrganMgr::OrganEnv::ParseTIFExif(Text::CString fileName)
{
	return 0;
	//////////////////////////////////
}

void SSWR::OrganMgr::OrganEnv::ExportWeb(const UTF8Char *exportDir, Bool includeWebPhoto, Bool includeNoPhoto, Int32 locId, UOSInt *photoCnt, UOSInt *speciesCnt)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr = Text::StrConcat(sbuff, exportDir);
	UTF8Char *sptrEnd;
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptrEnd = Text::StrConcatC(sptr, UTF8STRC("indexhd"));
	IO::Path::CreateDirectory(CSTRP(sbuff, sptrEnd));

	sptrEnd = Text::StrConcatC(sptr, UTF8STRC("indexhd.html"));

	UOSInt photoParsed = 0;
	UOSInt speciesParsed = 0;
	UOSInt thisPhotoCnt;
	UOSInt thisSpeciesCnt;
	UOSInt thisPhSpeciesCnt;

	IO::FileStream fs({sbuff, (UOSInt)(sptrEnd - sbuff)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	Text::UTF8Writer writer(fs);

	ExportBeginPage(&writer, this->currCate->chiName->v);
	
	OrganGroup *grp;
	UOSInt i;
	UOSInt j;
	NotNullPtr<Text::String> s;
	Data::FastMap<Int32, Data::ArrayList<OrganGroup*>*> *grpTree;
	Data::FastMap<Int32, Data::ArrayList<OrganSpecies*>*> *spTree;
	Data::ArrayList<OrganGroup*> *grps;

	Text::StringBuilderUTF8 sb;
	{
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

					writer.WriteStrC(UTF8STRC("<a href="));
					sb.ClearStr();
					sb.AppendC(UTF8STRC("indexhd/grp"));
					sb.AppendI32(grp->GetGroupId());
					sb.AppendC(UTF8STRC("/index.html"));
					s = Text::XML::ToNewAttrText(sb.ToString());
					writer.WriteStrC(s->v, s->leng);
					s->Release();
					writer.WriteStrC(UTF8STRC(">"));

					sb.ClearStr();
					sb.Append(grp->GetCName());
					sb.AppendC(UTF8STRC(" "));
					sb.Append(grp->GetEName());
					sb.AppendC(UTF8STRC(" ("));
					sb.AppendUOSInt(thisPhSpeciesCnt);
					sb.AppendC(UTF8STRC("/"));
					sb.AppendUOSInt(thisSpeciesCnt);
					sb.AppendC(UTF8STRC(")"));
					s = Text::XML::ToNewXMLText(sb.ToString());
					writer.WriteStrC(s->v, s->leng);
					s->Release();
					writer.WriteLineC(UTF8STRC("</a><br/>"));
				}

				i++;
			}
		}
		FreeSpeciesTree(spTree);
		FreeGroupTree(grpTree);
	}

	ExportEndPage(&writer);
	*photoCnt = photoParsed;
	*speciesCnt = speciesParsed;
}

void SSWR::OrganMgr::OrganEnv::FreeGroupTree(Data::FastMap<Int32, Data::ArrayList<OrganGroup*>*> *grpTree)
{
	OrganGroup *grp;
	Data::ArrayList<OrganGroup*> *grps;
	UOSInt i;
	UOSInt j;

	i = grpTree->GetCount();
	while (i-- > 0)
	{
		grps = grpTree->GetItem(i);
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

void SSWR::OrganMgr::OrganEnv::FreeSpeciesTree(Data::FastMap<Int32, Data::ArrayList<OrganSpecies*>*> *spTree)
{
	OrganSpecies *sp;
	Data::ArrayList<OrganSpecies*> *sps;
	UOSInt i;
	UOSInt j;

	i = spTree->GetCount();
	while (i-- > 0)
	{
		sps = spTree->GetItem(i);
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
	NotNullPtr<Text::String> s;
	writer->WriteLineC(UTF8STRC("<HTML>"));
	writer->WriteLineC(UTF8STRC("<HEAD>"));
	writer->WriteLineC(UTF8STRC("<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=utf8\">"));
	writer->WriteStrC(UTF8STRC("<title>"));
	s = Text::XML::ToNewXMLText(title);
	writer->WriteStrC(s->v, s->leng);
	writer->WriteLineC(UTF8STRC("</title>"));
	writer->WriteLineC(UTF8STRC("</HEAD>"));
	writer->WriteLine();
	writer->WriteLineC(UTF8STRC("<BODY TEXT=\"#c0e0ff\" LINK=\"#6080ff\" VLINK=\"#4060ff\" ALINK=\"#4040FF\" bgcolor=\"#000000\">"));
	writer->WriteStrC(UTF8STRC("<center><h1>"));
	writer->WriteStrC(s->v, s->leng);
	writer->WriteLineC(UTF8STRC("</h1></center>"));
	s->Release();
}

void SSWR::OrganMgr::OrganEnv::ExportEndPage(IO::Writer *writer)
{
	writer->WriteLineC(UTF8STRC("</BODY>"));
	writer->WriteLineC(UTF8STRC("</HTML>"));
}

void SSWR::OrganMgr::OrganEnv::ExportGroup(OrganGroup *grp, Data::FastMap<Int32, Data::ArrayList<OrganGroup*>*> *grpTree, Data::FastMap<Int32, Data::ArrayList<OrganSpecies*>*> *spTree, const UTF8Char *backURL, UTF8Char *fullPath, UTF8Char *pathAppend, Bool includeWebPhoto, Bool includeNoPhoto, Int32 locId, UOSInt *photoCnt, UOSInt *speciesCnt, UOSInt *phSpeciesCnt)
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
	NotNullPtr<Text::String> s;
	UTF8Char backBuff[64];
	Text::StrConcatC(Text::StrInt32(Text::StrConcatC(backBuff, UTF8STRC("../../indexhd/grp")), grp->GetGroupId()), UTF8STRC("/index.html"));

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
					sptr = Text::StrInt32(Text::StrConcatC(pathAppend, UTF8STRC("indexhd\\grp")), grp->GetGroupId());
					IO::Path::CreateDirectory(CSTRP(fullPath, sptr));
					sptr = Text::StrConcatC(sptr, UTF8STRC("\\index.html"));

					NotNullPtr<IO::FileStream> nnfs;
					NEW_CLASSNN(nnfs, IO::FileStream({fullPath, (UOSInt)(sptr - fullPath)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
					fs = nnfs.Ptr();
					NEW_CLASS(writer, Text::UTF8Writer(nnfs));
					NEW_CLASS(sb, Text::StringBuilderUTF8());
					sb->AppendC(this->currCate->chiName->v, this->currCate->chiName->leng);
					sb->AppendC(UTF8STRC(" - "));
					sb->Append(grp->GetCName());
					sb->AppendC(UTF8STRC(" "));
					sb->Append(grp->GetEName());
					ExportBeginPage(writer, sb->ToString());
				}

				writer->WriteStrC(UTF8STRC("<a href="));
				sb->ClearStr();
				sb->AppendC(UTF8STRC("../grp"));
				sb->AppendI32(myGrp->GetGroupId());
				sb->AppendC(UTF8STRC("/index.html"));
				s = Text::XML::ToNewAttrText(sb->ToString());
				writer->WriteStrC(s->v, s->leng);
				s->Release();
				writer->WriteStrC(UTF8STRC(">"));

				sb->ClearStr();
				sb->Append(myGrp->GetCName());
				sb->AppendC(UTF8STRC(" "));
				sb->Append(myGrp->GetEName());
				sb->AppendC(UTF8STRC(" ("));
				sb->AppendUOSInt(thisPhSpecies);
				sb->AppendC(UTF8STRC("/"));
				sb->AppendUOSInt(thisSpecies);
				sb->AppendC(UTF8STRC(")"));
				s = Text::XML::ToNewXMLText(sb->ToString());
				writer->WriteStrC(s->v, s->leng);
				s->Release();
				writer->WriteLineC(UTF8STRC("</a><br/>"));
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
					sptr = Text::StrInt32(Text::StrConcatC(pathAppend, UTF8STRC("indexhd\\grp")), grp->GetGroupId());
					IO::Path::CreateDirectory(CSTRP(fullPath, sptr));
					sptr = Text::StrConcatC(sptr, UTF8STRC("\\index.html"));

					NotNullPtr<IO::FileStream> nnfs;
					NEW_CLASSNN(nnfs, IO::FileStream({fullPath, (UOSInt)(sptr - fullPath)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
					fs = nnfs.Ptr();
					NEW_CLASS(writer, Text::UTF8Writer(nnfs));
					NEW_CLASS(sb, Text::StringBuilderUTF8());
					sb->AppendC(this->currCate->chiName->v, this->currCate->chiName->leng);
					sb->AppendC(UTF8STRC(" - "));
					sb->Append(grp->GetCName());
					sb->AppendC(UTF8STRC(" "));
					sb->Append(grp->GetEName());
					ExportBeginPage(writer, sb->ToString());
				}
				
				Text::String *str = sp->GetDirName();
				sptr = pathAppend;
				*sptr++ = str->v[0];
				*sptr++ = str->v[1];
				*sptr = 0;
				sb->ClearStr();
				sb->AppendC(UTF8STRC("../../"));
				sb->AppendC(pathAppend, 2);
				sb->AppendC(UTF8STRC("/"));
				sb->Append(str);
				sb->AppendC(UTF8STRC("/index.html"));
				writer->WriteStrC(UTF8STRC("<a href="));
				s = Text::XML::ToNewAttrText(sb->ToString());
				writer->WriteStrC(s->v, s->leng);
				writer->WriteStrC(UTF8STRC(">"));
				s->Release();
				sb->ClearStr();
				sb->Append(sp->GetSName());
				sb->AppendC(UTF8STRC(" "));
				sb->Append(sp->GetCName());
				if (sp->GetEName())
				{
					sb->AppendC(UTF8STRC(" "));
					sb->Append(sp->GetEName());
				}
				s = Text::XML::ToNewXMLText(sb->ToString());
				writer->WriteStrC(s->v, s->leng);
				s->Release();
				writer->WriteLineC(UTF8STRC("</a><br/>"));

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
		writer->WriteStrC(UTF8STRC("<br><a href="));
		s = Text::XML::ToNewAttrText(backURL);
		writer->WriteStrC(s->v, s->leng);
		s->Release();
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
	NotNullPtr<Text::String> s;
	UTF8Char *sptr;
	GetSpeciesImages(&items, sp);

	i = items.GetCount();
	while (i-- > 0)
	{
		imgItem = items.GetItem(i);
		ft = imgItem->GetFileType();
		if (ft == OrganImageItem::FileType::JPEG || ft == OrganImageItem::FileType::TIFF)
		{
			myPhotoExist = true;
		}
		else if (ft == OrganImageItem::FileType::Webimage)
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

	Text::String *str = sp->GetDirName();
	sptr = pathAppend;
	*sptr++ = str->v[0];
	*sptr++ = str->v[1];
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = str->ConcatTo(sptr);
	IO::Path::CreateDirectory(CSTRP(fullPath, sptr));

	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(sptr, UTF8STRC("index.html"));

	Text::StringBuilderUTF8 sb;
	IO::FileStream fs({fullPath, (UOSInt)(sptr - fullPath)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	Text::UTF8Writer writer(fs);
	sb.AppendC(this->currCate->chiName->v, this->currCate->chiName->leng);
	sb.AppendC(UTF8STRC(" - "));
	sb.Append(sp->GetSName());
	sb.AppendC(UTF8STRC(" "));
	sb.Append(sp->GetCName());
	if (sp->GetEName())
	{
		sb.AppendC(UTF8STRC(" "));
		sb.Append(sp->GetEName());
	}
	ExportBeginPage(&writer, sb.ToString());

	i = 0;
	j = items.GetCount();
	while (i < j)
	{
		i++;
	}


	writer.WriteStrC(UTF8STRC("<a href="));
	s = Text::XML::ToNewAttrText(backURL);
	writer.WriteStrC(s->v, s->leng);
	s->Release();
	writer.WriteW(L">戻る</a><br/>");

	ExportEndPage(&writer);

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
	return this->monMgr.GetMonitorHDPI(hMonitor);
}

Double SSWR::OrganMgr::OrganEnv::GetMonitorDDPI(MonitorHandle *hMonitor)
{
	return this->monMgr.GetMonitorDDPI(hMonitor);
}

void SSWR::OrganMgr::OrganEnv::SetMonitorHDPI(MonitorHandle *hMonitor, Double monitorHDPI)
{
	this->monMgr.SetMonitorHDPI(hMonitor, monitorHDPI);
}
