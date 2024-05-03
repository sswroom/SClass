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
#include "UI/GUICore.h"

SSWR::OrganMgr::UserFileComparator::~UserFileComparator()
{
}

OSInt SSWR::OrganMgr::UserFileComparator::Compare(NN<UserFileInfo> a, NN<UserFileInfo> b) const
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

OSInt SSWR::OrganMgr::UserFileTimeComparator::Compare(NN<UserFileInfo> a, NN<UserFileInfo> b) const
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

OSInt SSWR::OrganMgr::UserFileSpeciesComparator::Compare(NN<UserFileInfo> a, NN<UserFileInfo> b) const
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

OSInt SSWR::OrganMgr::WebFileSpeciesComparator::Compare(NN<WebFileInfo> a, NN<WebFileInfo> b) const
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
	this->categories.FreeAll(FreeCategory);
	this->grpTypes.DeleteAll();
	this->BooksDeinit();
	NN<DataFileInfo> dataFile;
	i = this->dataFiles.GetCount();
	while (i-- > 0)
	{
		dataFile = this->dataFiles.GetItemNoCheck(i);
		this->ReleaseDataFile(dataFile);
	}
	this->speciesMap.FreeAll(ReleaseSpecies);
	this->userFileMap.FreeAll(ReleaseUserFile);
	this->userMap.DeleteAll();
	this->TripRelease();
	SDEL_CLASS(this->gpsTrk);
	SDEL_CLASS(this->langFile);
	this->drawEng.Delete();
	this->ssl.Delete();
	this->sockf.Delete();
}

NN<Media::DrawEngine> SSWR::OrganMgr::OrganEnv::GetDrawEngine()
{
	return this->drawEng;
}

NN<Parser::ParserList> SSWR::OrganMgr::OrganEnv::GetParserList()
{
	return this->parsers;
}

NN<Net::SocketFactory> SSWR::OrganMgr::OrganEnv::GetSocketFactory()
{
	return this->sockf;
}

Optional<Net::SSLEngine> SSWR::OrganMgr::OrganEnv::GetSSLEngine()
{
	return this->ssl;
}

NN<Media::ColorManager> SSWR::OrganMgr::OrganEnv::GetColorMgr()
{
	return this->colorMgr;
}

NN<Media::MonitorMgr> SSWR::OrganMgr::OrganEnv::GetMonitorMgr()
{
	return this->monMgr;
}

SSWR::OrganMgr::OrganEnv::ErrorType SSWR::OrganMgr::OrganEnv::GetErrorType()
{
	return this->errType;
}

Text::CStringNN SSWR::OrganMgr::OrganEnv::GetLang(Text::CStringNN name)
{
	if (this->langFile == 0)
		return name;
	NN<Text::String> ret;
	if (!this->langFile->GetValue(name).SetTo(ret))
		return name;
	return ret->ToCString();
}

UOSInt SSWR::OrganMgr::OrganEnv::GetCategories(NN<Data::ArrayListNN<Category>> categories)
{
	categories->AddAll(this->categories);
	return this->categories.GetCount();
}

NN<Data::ArrayListNN<SSWR::OrganMgr::OrganGroupType>> SSWR::OrganMgr::OrganEnv::GetGroupTypes()
{
	return this->grpTypes;
}

Bool SSWR::OrganMgr::OrganEnv::SetSpeciesImg(NN<OrganSpecies> sp, NN<OrganImageItem> img)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	NN<UserFileInfo> userFile;
	NN<WebFileInfo> webFile;
	if (img->GetFileType() == OrganImageItem::FileType::UserFile && img->GetUserFile().SetTo(userFile))
	{
		sp->SetPhotoId(userFile->id);
		sp->SetPhotoWId(0);
	}
	else if (img->GetFileType() == OrganImageItem::FileType::WebFile && img->GetWebFile().SetTo(webFile))
	{
		sp->SetPhotoId(0);
		sp->SetPhotoWId(webFile->id);
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

Bool SSWR::OrganMgr::OrganEnv::SetSpeciesMapColor(NN<OrganSpecies> sp, UInt32 mapColor)
{
	sp->SetMapColor(mapColor);
	SaveSpecies(sp);
	return true;
}

UOSInt SSWR::OrganMgr::OrganEnv::GetBooksAll(NN<Data::ArrayListNN<OrganBook>> items)
{
	NN<Data::ArrayListNN<OrganBook>> books;
	if (books.Set(this->bookObjs))
	{
		items->AddAll(books);
		return books->GetCount();
	}
	return 0;
}

UOSInt SSWR::OrganMgr::OrganEnv::GetBooksOfYear(NN<Data::ArrayListNN<OrganBook>> items, Int32 year)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	NN<OrganBook> book;
	k = 0;
	i = 0;
	j = this->bookObjs->GetCount();
	while (i < j)
	{
		book = this->bookObjs->GetItemNoCheck(i);
		if (book->GetPublishDate().GetTimeValue().year == year)
		{
			items->Add(book);
			k++;
		}
		i++;
	}
	return k;
}

NN<SSWR::OrganMgr::WebUserInfo> SSWR::OrganMgr::OrganEnv::GetWebUser(Int32 userId)
{
	NN<WebUserInfo> webUser;
	if (!this->userMap.Get(userId).SetTo(webUser))
	{
		NEW_CLASSNN(webUser, WebUserInfo());
		webUser->id = userId;
		this->userMap.Put(webUser->id, webUser);
	}
	return webUser;
}

NN<Data::ArrayListNN<SSWR::OrganMgr::DataFileInfo>> SSWR::OrganMgr::OrganEnv::GetDataFiles()
{
	return this->dataFiles;
}

void SSWR::OrganMgr::OrganEnv::ReleaseDataFile(NN<DataFileInfo> dataFile)
{
	dataFile->oriFileName->Release();
	dataFile->fileName->Release();
	MemFreeNN(dataFile);
}

void SSWR::OrganMgr::OrganEnv::ReleaseSpecies(NN<SpeciesInfo> species)
{
	UOSInt i;
	UOSInt j;
	NN<WebFileInfo> webFile;
	i = 0;
	j = species->wfileMap.GetCount();
	while (i < j)
	{
		webFile = species->wfileMap.GetItemNoCheck(i);
		webFile->location->Release();
		webFile->imgUrl->Release();
		webFile->srcUrl->Release();
		MemFreeNN(webFile);
		i++;
	}
	species.Delete();
}

void SSWR::OrganMgr::OrganEnv::ReleaseUserFile(NN<UserFileInfo> userFile)
{
	userFile->oriFileName->Release();
	userFile->dataFileName->Release();
	OPTSTR_DEL(userFile->camera);
	OPTSTR_DEL(userFile->descript);
	OPTSTR_DEL(userFile->location);
	MemFreeNN(userFile);
}

UOSInt SSWR::OrganMgr::OrganEnv::GetUserFiles(NN<Data::ArrayListNN<UserFileInfo>> userFiles, const Data::Timestamp &fromTime, const Data::Timestamp &toTime)
{
	NN<UserFileInfo> userFile;
	UOSInt initCnt = userFiles->GetCount();
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->userFileMap.GetCount();
	while (i < j)
	{
		userFile = this->userFileMap.GetItemNoCheck(i);
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
	this->trips.DeleteAll();
	this->locs.DeleteAll();
	this->locType.DeleteAll();
}

OSInt SSWR::OrganMgr::OrganEnv::TripGetIndex(const Data::Timestamp &ts)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->trips.GetCount() - 1;
	OSInt k;
	NN<Trip> t;
	while (i <= j)
	{
		k = (i + j) >> 1;
		t = this->trips.GetItemNoCheck((UOSInt)k);
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

Optional<SSWR::OrganMgr::Trip> SSWR::OrganMgr::OrganEnv::TripGet(Int32 userId, const Data::Timestamp &ts)
{
	OSInt i = this->TripGetIndex(ts);
	if (i < 0)
		return 0;
	else
		return this->trips.GetItem((UOSInt)i);
}

NN<Data::ArrayListNN<SSWR::OrganMgr::Trip>> SSWR::OrganMgr::OrganEnv::TripGetList()
{
	return this->trips;
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
		l = this->locs.GetItemNoCheck((UOSInt)k)->id;
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

Optional<SSWR::OrganMgr::Location> SSWR::OrganMgr::OrganEnv::LocationGet(Int32 locId)
{
	OSInt i = LocationGetIndex(locId);
	if (i < 0)
		return 0;
	else
		return this->locs.GetItem((UOSInt)i);
}

NN<Data::ArrayListNN<SSWR::OrganMgr::Location>> SSWR::OrganMgr::OrganEnv::LocationGetSub(Int32 locId)
{
	NN<Data::ArrayListNN<Location>> outArr;
	NEW_CLASSNN(outArr, Data::ArrayListNN<Location>());
	UOSInt i = 0;
	UOSInt j = this->locs.GetCount();
	NN<Location> loc;
	while (i < j)
	{
		loc = this->locs.GetItemNoCheck(i);
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
		l = this->locType.GetItemNoCheck((UOSInt)k)->id;
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

Optional<SSWR::OrganMgr::SpeciesInfo> SSWR::OrganMgr::OrganEnv::GetSpeciesInfo(Int32 speciesId)
{
	return this->speciesMap.Get(speciesId);
}

NN<SSWR::OrganMgr::SpeciesInfo> SSWR::OrganMgr::OrganEnv::GetSpeciesInfoCreate(Int32 speciesId)
{
	NN<SpeciesInfo> sp;
	if (!this->speciesMap.Get(speciesId).SetTo(sp))
	{
		NEW_CLASSNN(sp, SpeciesInfo());
		sp->id = speciesId;
		this->speciesMap.Put(sp->id, sp);
	}
	return sp;
}

void SSWR::OrganMgr::OrganEnv::BooksDeinit()
{
	if (this->bookObjs)
	{
		this->bookObjs->DeleteAll();
		DEL_CLASS(this->bookObjs);
		DEL_CLASS(this->bookIds);
		this->bookIds = 0;
		this->bookObjs = 0;
	}
}

Optional<Text::String> SSWR::OrganMgr::OrganEnv::GetLocName(Int32 userId, const Data::Timestamp &ts, UI::GUIForm *ownerFrm, NN<UI::GUICore> ui)
{
	NN<Trip> tr;
	NN<Location> loc;
	if (this->TripGet(userId, ts).SetTo(tr) && this->LocationGet(tr->locId).SetTo(loc))
	{
		return loc->cname;
	}
	else if (userId == this->userId)
	{
		Text::StringBuilderUTF8 sb;
		Data::Timestamp ts2 = ts.ClearTimeLocal();
		Data::Timestamp ts3 = ts2.AddDay(1);
		{
			OrganTripForm frm(0, ui, *this);
			sb.AppendC(UTF8STRC("Trip not found at "));
			sb.AppendTSNoZone(ts);
			frm.SetText(sb.ToCString());
			frm.SetTimes(ts, ts2, ts3);
			frm.ShowDialog(ownerFrm);
		}
		if (this->TripGet(userId, ts).SetTo(tr) && this->LocationGet(tr->locId).SetTo(loc))
		{
			return loc->cname;
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

void SSWR::OrganMgr::OrganEnv::SetCurrCategory(NN<Category> currCate)
{
	this->currCate = currCate.Ptr();
	this->cateIsFullDir = Text::String::OrEmpty(this->currCate->srcDir)->IndexOf(UTF8STRC(":\\")) != INVALID_INDEX;
	this->TripReload(this->currCate->cateId);
	this->LoadGroupTypes();
}


void SSWR::OrganMgr::OrganEnv::FreeCategory(NN<Category> cate)
{
	cate->chiName->Release();
	cate->dirName->Release();
	OPTSTR_DEL(cate->srcDir);
	MemFreeNN(cate);
}

Optional<Media::EXIFData> SSWR::OrganMgr::OrganEnv::ParseJPGExif(Text::CStringNN fileName)
{
	IO::StmData::FileData fd(fileName, false);
	return Media::JPEGFile::ParseJPEGExif(fd);
}

Optional<Media::EXIFData> SSWR::OrganMgr::OrganEnv::ParseTIFExif(Text::CStringNN fileName)
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

	ExportBeginPage(writer, this->currCate->chiName->v);
	
	NN<OrganGroup> grp;
	UOSInt i;
	UOSInt j;
	NN<Text::String> s;
	NN<Data::FastMapNN<Int32, Data::ArrayListNN<OrganGroup>>> grpTree;
	NN<Data::FastMapNN<Int32, Data::ArrayListNN<OrganSpecies>>> spTree;
	NN<Data::ArrayListNN<OrganGroup>> grps;

	Text::StringBuilderUTF8 sb;
	{
		grpTree = GetGroupTree();
		spTree = GetSpeciesTree();
		if (grpTree->Get(0).SetTo(grps))
		{
			i = 0;
			j = grps->GetCount();
			while (i < j)
			{
				grp = grps->GetItemNoCheck(i);
				ExportGroup(grp, grpTree, spTree, (const UTF8Char*)"../../index.html", sbuff, sptr, includeWebPhoto, includeNoPhoto, locId, &thisPhotoCnt, &thisSpeciesCnt, &thisPhSpeciesCnt);
				if (thisSpeciesCnt > 0)
				{
					photoParsed += thisPhotoCnt;
					speciesParsed += thisSpeciesCnt;

					writer.Write(CSTR("<a href="));
					sb.ClearStr();
					sb.AppendC(UTF8STRC("indexhd/grp"));
					sb.AppendI32(grp->GetGroupId());
					sb.AppendC(UTF8STRC("/index.html"));
					s = Text::XML::ToNewAttrText(sb.ToString());
					writer.Write(s->ToCString());
					s->Release();
					writer.Write(CSTR(">"));

					sb.ClearStr();
					sb.AppendOpt(grp->GetCName());
					sb.AppendC(UTF8STRC(" "));
					sb.AppendOpt(grp->GetEName());
					sb.AppendC(UTF8STRC(" ("));
					sb.AppendUOSInt(thisPhSpeciesCnt);
					sb.AppendC(UTF8STRC("/"));
					sb.AppendUOSInt(thisSpeciesCnt);
					sb.AppendC(UTF8STRC(")"));
					s = Text::XML::ToNewXMLText(sb.ToString());
					writer.Write(s->ToCString());
					s->Release();
					writer.WriteLine(CSTR("</a><br/>"));
				}

				i++;
			}
		}
		FreeSpeciesTree(spTree);
		FreeGroupTree(grpTree);
	}

	ExportEndPage(writer);
	*photoCnt = photoParsed;
	*speciesCnt = speciesParsed;
}

void SSWR::OrganMgr::OrganEnv::FreeGroupTree(NN<Data::FastMapNN<Int32, Data::ArrayListNN<OrganGroup>>> grpTree)
{
	NN<Data::ArrayListNN<OrganGroup>> grps;
	UOSInt i;

	i = grpTree->GetCount();
	while (i-- > 0)
	{
		grps = grpTree->GetItemNoCheck(i);
		grps->DeleteAll();
		grps.Delete();
	}
	grpTree.Delete();
}

void SSWR::OrganMgr::OrganEnv::FreeSpeciesTree(NN<Data::FastMapNN<Int32, Data::ArrayListNN<OrganSpecies>>> spTree)
{
	NN<Data::ArrayListNN<OrganSpecies>> sps;
	UOSInt i;

	i = spTree->GetCount();
	while (i-- > 0)
	{
		sps = spTree->GetItemNoCheck(i);
		sps->DeleteAll();
		sps.Delete();
	}
	spTree.Delete();
}

void SSWR::OrganMgr::OrganEnv::ExportBeginPage(NN<IO::Writer> writer, const UTF8Char *title)
{
	NN<Text::String> s;
	writer->WriteLine(CSTR("<HTML>"));
	writer->WriteLine(CSTR("<HEAD>"));
	writer->WriteLine(CSTR("<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=utf8\">"));
	writer->Write(CSTR("<title>"));
	s = Text::XML::ToNewXMLText(title);
	writer->Write(s->ToCString());
	writer->WriteLine(CSTR("</title>"));
	writer->WriteLine(CSTR("</HEAD>"));
	writer->WriteLine();
	writer->WriteLine(CSTR("<BODY TEXT=\"#c0e0ff\" LINK=\"#6080ff\" VLINK=\"#4060ff\" ALINK=\"#4040FF\" bgcolor=\"#000000\">"));
	writer->Write(CSTR("<center><h1>"));
	writer->Write(s->ToCString());
	writer->WriteLine(CSTR("</h1></center>"));
	s->Release();
}

void SSWR::OrganMgr::OrganEnv::ExportEndPage(NN<IO::Writer> writer)
{
	writer->WriteLine(CSTR("</BODY>"));
	writer->WriteLine(CSTR("</HTML>"));
}

void SSWR::OrganMgr::OrganEnv::ExportGroup(NN<OrganGroup> grp, NN<Data::FastMapNN<Int32, Data::ArrayListNN<OrganGroup>>> grpTree, NN<Data::FastMapNN<Int32, Data::ArrayListNN<OrganSpecies>>> spTree, const UTF8Char *backURL, UTF8Char *fullPath, UTF8Char *pathAppend, Bool includeWebPhoto, Bool includeNoPhoto, Int32 locId, UOSInt *photoCnt, UOSInt *speciesCnt, UOSInt *phSpeciesCnt)
{
	UOSInt totalPhoto = 0;
	UOSInt totalSpecies = 0;
	UOSInt totalPhSpecies = 0;
	UOSInt thisPhoto;
	UOSInt thisSpecies;
	UOSInt thisPhSpecies;
	UOSInt i;
	UOSInt j;
	NN<OrganGroup> myGrp;
	NN<OrganSpecies> sp;

	IO::FileStream *fs = 0;
	Text::UTF8Writer *writer = 0;
	NN<Text::UTF8Writer> nnwriter;
	Text::StringBuilderUTF8 sb;
	UTF8Char *sptr;
	NN<Text::String> s;
	UTF8Char backBuff[64];
	Text::StrConcatC(Text::StrInt32(Text::StrConcatC(backBuff, UTF8STRC("../../indexhd/grp")), grp->GetGroupId()), UTF8STRC("/index.html"));

	NN<Data::ArrayListNN<OrganGroup>> grps;
	NN<Data::ArrayListNN<OrganSpecies>> sps;

	if (grpTree->Get(grp->GetGroupId()).SetTo(grps))
	{
		i = 0;
		j = grps->GetCount();
		while (i < j)
		{
			myGrp = grps->GetItemNoCheck(i);
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

					NN<IO::FileStream> nnfs;
					NEW_CLASSNN(nnfs, IO::FileStream({fullPath, (UOSInt)(sptr - fullPath)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
					fs = nnfs.Ptr();
					NEW_CLASSNN(nnwriter, Text::UTF8Writer(nnfs));
					writer = nnwriter.Ptr();
					sb.ClearStr();
					sb.AppendC(this->currCate->chiName->v, this->currCate->chiName->leng);
					sb.AppendC(UTF8STRC(" - "));
					sb.AppendOpt(grp->GetCName());
					sb.AppendC(UTF8STRC(" "));
					sb.AppendOpt(grp->GetEName());
					ExportBeginPage(nnwriter, sb.ToString());
				}

				writer->Write(CSTR("<a href="));
				sb.ClearStr();
				sb.AppendC(UTF8STRC("../grp"));
				sb.AppendI32(myGrp->GetGroupId());
				sb.AppendC(UTF8STRC("/index.html"));
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer->Write(s->ToCString());
				s->Release();
				writer->Write(CSTR(">"));

				sb.ClearStr();
				sb.AppendOpt(myGrp->GetCName());
				sb.AppendC(UTF8STRC(" "));
				sb.AppendOpt(myGrp->GetEName());
				sb.AppendC(UTF8STRC(" ("));
				sb.AppendUOSInt(thisPhSpecies);
				sb.AppendC(UTF8STRC("/"));
				sb.AppendUOSInt(thisSpecies);
				sb.AppendC(UTF8STRC(")"));
				s = Text::XML::ToNewXMLText(sb.ToString());
				writer->Write(s->ToCString());
				s->Release();
				writer->WriteLine(CSTR("</a><br/>"));
			}

			i++;
		}
	}
	else if (spTree->Get(grp->GetGroupId()).SetTo(sps))
	{
		Bool hasMyPhoto;
		i = 0;
		j = sps->GetCount();
		while (i < j)
		{
			sp = sps->GetItemNoCheck(i);
			if (ExportSpecies(sp, backBuff, fullPath, pathAppend, includeWebPhoto, includeNoPhoto, locId, thisPhoto, hasMyPhoto))
			{
				if (fs == 0)
				{
					sptr = Text::StrInt32(Text::StrConcatC(pathAppend, UTF8STRC("indexhd\\grp")), grp->GetGroupId());
					IO::Path::CreateDirectory(CSTRP(fullPath, sptr));
					sptr = Text::StrConcatC(sptr, UTF8STRC("\\index.html"));

					NN<IO::FileStream> nnfs;
					NEW_CLASSNN(nnfs, IO::FileStream({fullPath, (UOSInt)(sptr - fullPath)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
					fs = nnfs.Ptr();
					NEW_CLASSNN(nnwriter, Text::UTF8Writer(nnfs));
					writer = nnwriter.Ptr();
					sb.ClearStr();
					sb.AppendC(this->currCate->chiName->v, this->currCate->chiName->leng);
					sb.AppendC(UTF8STRC(" - "));
					sb.AppendOpt(grp->GetCName());
					sb.AppendC(UTF8STRC(" "));
					sb.AppendOpt(grp->GetEName());
					ExportBeginPage(nnwriter, sb.ToString());
				}
				
				NN<Text::String> str = Text::String::OrEmpty(sp->GetDirName());
				sptr = pathAppend;
				*sptr++ = str->v[0];
				*sptr++ = str->v[1];
				*sptr = 0;
				sb.ClearStr();
				sb.AppendC(UTF8STRC("../../"));
				sb.AppendC(pathAppend, 2);
				sb.AppendC(UTF8STRC("/"));
				sb.Append(str);
				sb.AppendC(UTF8STRC("/index.html"));
				writer->Write(CSTR("<a href="));
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer->Write(s->ToCString());
				writer->Write(CSTR(">"));
				s->Release();
				sb.ClearStr();
				sb.AppendOpt(sp->GetSName());
				sb.AppendC(UTF8STRC(" "));
				sb.AppendOpt(sp->GetCName());
				if (sp->GetEName().SetTo(s))
				{
					sb.AppendC(UTF8STRC(" "));
					sb.Append(s);
				}
				s = Text::XML::ToNewXMLText(sb.ToString());
				writer->Write(s->ToCString());
				s->Release();
				writer->WriteLine(CSTR("</a><br/>"));

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

	if (fs && nnwriter.Set(writer))
	{
		writer->WriteLineW(L"<br/><a href=\"list.html\">すべて</a><br>");
		writer->Write(CSTR("<br><a href="));
		s = Text::XML::ToNewAttrText(backURL);
		writer->Write(s->ToCString());
		s->Release();
		writer->WriteLineW(L">戻る</a>");

		ExportEndPage(nnwriter);
		DEL_CLASS(writer);
		DEL_CLASS(fs);
	}

	*photoCnt = totalPhoto;
	*speciesCnt = totalSpecies;
	*phSpeciesCnt = totalPhSpecies;
}

Bool SSWR::OrganMgr::OrganEnv::ExportSpecies(NN<OrganSpecies> sp, const UTF8Char *backURL, UTF8Char *fullPath, UTF8Char *pathAppend, Bool includeWebPhoto, Bool includeNoPhoto, Int32 locId, OutParam<UOSInt> photoCnt, OutParam<Bool> hasMyPhoto)
{
	UOSInt i;
	UOSInt j;
	NN<OrganImageItem> imgItem;
	OrganImageItem::FileType ft;
	Data::ArrayListNN<OrganImageItem> items;
	Bool myPhotoExist = false;
	NN<Text::String> s;
	UTF8Char *sptr;
	GetSpeciesImages(items, sp);

	i = items.GetCount();
	while (i-- > 0)
	{
		imgItem = items.GetItemNoCheck(i);
		ft = imgItem->GetFileType();
		if (ft == OrganImageItem::FileType::JPEG || ft == OrganImageItem::FileType::TIFF)
		{
			myPhotoExist = true;
		}
		else if (ft == OrganImageItem::FileType::Webimage)
		{
			if (!includeWebPhoto)
			{
				imgItem.Delete();
				items.RemoveAt(i);
			}
		}
		else
		{
			imgItem.Delete();
			items.RemoveAt(i);
		}
	}

	if (items.GetCount() == 0 && !includeNoPhoto)
	{
		return false;
	}

	NN<Text::String> str = Text::String::OrEmpty(sp->GetDirName());
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
	sb.AppendOpt(sp->GetSName());
	sb.AppendC(UTF8STRC(" "));
	sb.AppendOpt(sp->GetCName());
	if (sp->GetEName().SetTo(s))
	{
		sb.AppendC(UTF8STRC(" "));
		sb.Append(s);
	}
	ExportBeginPage(writer, sb.ToString());

	i = 0;
	j = items.GetCount();
	while (i < j)
	{
		i++;
	}


	writer.Write(CSTR("<a href="));
	s = Text::XML::ToNewAttrText(backURL);
	writer.Write(s->ToCString());
	s->Release();
	writer.WriteW(L">戻る</a><br/>");

	ExportEndPage(writer);

	photoCnt.Set(items.GetCount());
	hasMyPhoto.Set(myPhotoExist);
	items.DeleteAll();
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
