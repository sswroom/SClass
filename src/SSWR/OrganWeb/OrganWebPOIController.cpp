#include "Stdafx.h"
#include "SSWR/OrganWeb/OrganWebEnv.h"
#include "SSWR/OrganWeb/OrganWebPOIController.h"
#include "Text/JSText.h"

#include <stdio.h>


Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcPublicPOI(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	Sync::RWMutexUsage mutUsage;
	NotNullPtr<GroupInfo> poiGroup;
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	Data::ArrayListNN<GroupInfo> groups;
	Data::ArrayListNN<SpeciesInfo> speciesList;
	json.ObjectBeginArray(CSTR("poi"));
	if (poiGroup.Set(me->env->GroupGet(mutUsage, 21593)))
	{
		me->AddGroupPOI(mutUsage, json, poiGroup, 0, groups, speciesList);
	}
	json.ArrayEnd();
	json.ObjectBeginArray(CSTR("group"));
	me->AddGroups(json, groups);
	json.ArrayEnd();
	json.ObjectBeginArray(CSTR("species"));
	me->AddSpeciesList(json, speciesList);
	return resp->ResponseJSONStr(req, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcGroupPOI(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	Data::ArrayListNN<GroupInfo> groups;
	Data::ArrayListNN<SpeciesInfo> speciesList;
	Int32 groupId;
	json.ObjectBeginArray(CSTR("poi"));
	if (req->GetQueryValueI32(CSTR("id"), groupId))
	{
		Sync::RWMutexUsage mutUsage;
		NotNullPtr<GroupInfo> poiGroup;
		if (poiGroup.Set(me->env->GroupGet(mutUsage, groupId)))
		{
			groups.Add(poiGroup);
			me->AddGroupPOI(mutUsage, json, poiGroup, env.user?env.user->id:0, groups, speciesList);
		}
	}
	json.ArrayEnd();
	json.ObjectBeginArray(CSTR("group"));
	me->AddGroups(json, groups);
	json.ArrayEnd();
	json.ObjectBeginArray(CSTR("species"));
	me->AddSpeciesList(json, speciesList);
	return resp->ResponseJSONStr(req, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcSpeciesPOI(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	Data::ArrayListNN<SpeciesInfo> speciesList;
	Int32 speciesId;
	json.ObjectBeginArray(CSTR("poi"));
	if (req->GetQueryValueI32(CSTR("id"), speciesId))
	{
		Sync::RWMutexUsage mutUsage;
		NotNullPtr<SpeciesInfo> poiSpecies;
		if (poiSpecies.Set(me->env->SpeciesGet(mutUsage, speciesId)))
		{
			speciesList.Add(poiSpecies);
			me->AddSpeciesPOI(mutUsage, json, poiSpecies, env.user?env.user->id:0, me->env->GroupIsPublic(mutUsage, poiSpecies->groupId));
		}
	}
	json.ArrayEnd();
	json.ObjectBeginArray(CSTR("group"));
	json.ArrayEnd();
	json.ObjectBeginArray(CSTR("species"));
	me->AddSpeciesList(json, speciesList);
	return resp->ResponseJSONStr(req, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcDayPOI(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	Int32 dayId;
	json.ObjectBeginArray(CSTR("poi"));
	if (env.user == 0)
	{
		printf("SvcDayPOI: user == null\r\n");
	}
	else if (req->GetQueryValueI32(CSTR("id"), dayId))
	{
		Sync::RWMutexUsage mutUsage;
		Int64 startTime;
		Int64 endTime;
		OSInt startIndex;
		OSInt endIndex;
		startTime = dayId * 86400000LL;
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
		NotNullPtr<UserFileInfo> userFile;
		NotNullPtr<SpeciesInfo> sp;

		while (startIndex < endIndex)
		{
			if (userFile.Set(env.user->userFileObj.GetItem((UOSInt)startIndex)) && sp.Set(me->env->SpeciesGet(mutUsage, userFile->speciesId)))
			{
				me->AddUserfilePOI(json, sp, userFile);
			}
			startIndex++;
		}
	}
	json.ArrayEnd();
	json.ObjectBeginArray(CSTR("group"));
	json.ArrayEnd();
	json.ObjectBeginArray(CSTR("species"));
	return resp->ResponseJSONStr(req, 0, json.Build());
}


void SSWR::OrganWeb::OrganWebPOIController::AddGroupPOI(NotNullPtr<Sync::RWMutexUsage> mutUsage, NotNullPtr<Text::JSONBuilder> json, NotNullPtr<GroupInfo> group, Int32 userId, NotNullPtr<Data::ArrayListNN<GroupInfo>> groups, NotNullPtr<Data::ArrayListNN<SpeciesInfo>> speciesList)
{
	NotNullPtr<SpeciesInfo> species;
	NotNullPtr<GroupInfo> subgroup;
	UOSInt i = 0;
	UOSInt j = group->groups.GetCount();
	while (i < j)
	{
		if (subgroup.Set(group->groups.GetItem(i)))
		{
			groups->Add(subgroup);
			AddGroupPOI(mutUsage, json, subgroup, userId, groups, speciesList);
		}
		i++;
	}
	i = 0;
	j = group->species.GetCount();
	while (i < j)
	{
		if (species.Set(group->species.GetItem(i)))
		{
			speciesList->Add(species);
			AddSpeciesPOI(mutUsage, json, species, userId, this->env->GroupIsPublic(mutUsage, group->id));
		}
		i++;
	}
}

void SSWR::OrganWeb::OrganWebPOIController::AddSpeciesPOI(NotNullPtr<Sync::RWMutexUsage> mutUsage, NotNullPtr<Text::JSONBuilder> json, NotNullPtr<SpeciesInfo> species, Int32 userId, Bool publicGroup)
{
	NotNullPtr<UserFileInfo> file;
	UOSInt k;
	UOSInt l;
	WebUserInfo *user = 0;
	Bool adminUser = false;
	if (userId != 0)
	{
		user = this->env->UserGet(mutUsage, userId);
		if (user && user->userType == 0)
		{
			adminUser = true;
		}
	}
	k = 0;
	l = species->files.GetCount();
	while (k < l)
	{
		if (file.Set(species->files.GetItem(k)))
		{
			if ((file->lat != 0 || file->lon != 0) && (publicGroup || adminUser || file->webuserId == userId))
			{
				this->AddUserfilePOI(json, species, file);
			}
		}
		k++;
	}
}

void SSWR::OrganWeb::OrganWebPOIController::AddUserfilePOI(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<SpeciesInfo> species, NotNullPtr<UserFileInfo> file)
{
	json->ArrayBeginObject();
	json->ObjectAddInt32(CSTR("id"), file->id);
	json->ObjectAddStr(CSTR("descript"), file->descript);
	json->ObjectAddTSStr(CSTR("captureTime"), Data::Timestamp(file->captureTimeTicks, 32));
	json->ObjectAddFloat64(CSTR("lat"), file->lat);
	json->ObjectAddFloat64(CSTR("lon"), file->lon);
	json->ObjectAddStr(CSTR("oriFileName"), file->oriFileName);
	json->ObjectAddInt32(CSTR("imgSize"), (Int32)GetPreviewSize());
	json->ObjectAddStr(CSTR("sciName"), species->sciName);
	json->ObjectAddInt32(CSTR("speciesId"), species->speciesId);
	json->ObjectAddInt32(CSTR("groupId"), species->groupId);
	json->ObjectAddInt32(CSTR("cateId"), species->cateId);
	json->ObjectAddStr(CSTR("poiImg"), species->poiImg);
	json->ObjectEnd();
/*	sb->AppendUTF8Char('{');
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
	sb->AppendUOSInt(GetPreviewSize());
	sb->AppendC(UTF8STRC("&height="));
	sb->AppendUOSInt(GetPreviewSize());
	sb->AppendC(UTF8STRC("&fileId="));
	sb->AppendI32(file->id);
	sb->AppendC(UTF8STRC("\\\" /><br/>"));
	sb->AppendTS(Data::Timestamp(file->captureTimeTicks, 32));
	sb->AppendC(UTF8STRC("\",\"lat\":"));
	sb->AppendDouble(file->lat);
	sb->AppendC(UTF8STRC(",\"lon\":"));
	sb->AppendDouble(file->lon);
	sb->AppendC(UTF8STRC(",\"imgUrl\":\"/photo.html?id="));
	sb->AppendI32(species->speciesId);
	sb->AppendC(UTF8STRC("&cateId="));
	sb->AppendI32(species->cateId);
	sb->AppendC(UTF8STRC("&width="));
	sb->AppendUOSInt(GetPreviewSize());
	sb->AppendC(UTF8STRC("&height="));
	sb->AppendUOSInt(GetPreviewSize());
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
	sb->AppendC(UTF8STRC("\"},\r\n"));*/
}

void SSWR::OrganWeb::OrganWebPOIController::AddGroups(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<Data::ArrayListNN<GroupInfo>> groups)
{
	NotNullPtr<GroupInfo> group;
	Data::ArrayIterator<NotNullPtr<GroupInfo>> it = groups->Iterator();
	while (it.HasNext())
	{
		group = it.Next();
		json->ArrayBeginObject();
		json->ObjectAddInt32(CSTR("id"), group->id);
		json->ObjectAddInt32(CSTR("parentId"), group->parentId);
		json->ObjectAddStr(CSTR("engName"), group->engName);
		json->ObjectAddStr(CSTR("chiName"), group->chiName);
		json->ObjectAddStr(CSTR("descript"), group->descript);
		json->ObjectAddInt64(CSTR("photoCount"), (Int64)group->photoCount);
		json->ObjectAddInt64(CSTR("myPhotoCount"), (Int64)group->myPhotoCount);
		json->ObjectAddInt64(CSTR("totalCount"), (Int64)group->totalCount);
		json->ObjectAddInt32(CSTR("flags"), group->flags);
		json->ObjectAddInt32(CSTR("groupType"), group->groupType);
		json->ObjectEnd();
	}
}

void SSWR::OrganWeb::OrganWebPOIController::AddSpeciesList(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<Data::ArrayListNN<SpeciesInfo>> speciesList)
{
	Sync::RWMutexUsage mutUsage;
	NotNullPtr<SpeciesInfo> species;
	BookSpInfo *bookSp;
	BookInfo *book;
	UOSInt i;
	UOSInt j;
	Data::ArrayIterator<NotNullPtr<SpeciesInfo>> it = speciesList->Iterator();
	while (it.HasNext())
	{
		species = it.Next();
		json->ArrayBeginObject();
		json->ObjectAddInt32(CSTR("id"), species->speciesId);
		json->ObjectAddInt32(CSTR("groupId"), species->groupId);
		json->ObjectAddStr(CSTR("sciName"), species->sciName);
		json->ObjectAddStr(CSTR("chiName"), species->chiName);
		json->ObjectAddStr(CSTR("engName"), species->engName);
		json->ObjectAddStr(CSTR("descript"), species->descript);
		json->ObjectAddStr(CSTR("poiImg"), species->poiImg);
		json->ObjectAddInt32(CSTR("flags"), species->flags);
		json->ObjectBeginArray(CSTR("books"));
		i = 0;
		j = species->books.GetCount();
		while (i < j)
		{
			bookSp = species->books.GetItem(i);
			book = this->env->BookGet(mutUsage, bookSp->bookId);
			json->ArrayBeginObject();
			json->ObjectAddStr(CSTR("dispName"), bookSp->dispName);
			if (book)
			{
				json->ObjectAddStr(CSTR("bookTitle"), book->title);
				json->ObjectAddStr(CSTR("bookAuthor"), book->author);
				json->ObjectAddTSStr(CSTR("publishDate"), Data::Timestamp(book->publishDate, 32));
			}
			json->ObjectEnd();
			i++;
		}
		json->ArrayEnd();
		json->ObjectEnd();
	}
}

SSWR::OrganWeb::OrganWebPOIController::OrganWebPOIController(Net::WebServer::MemoryWebSessionManager *sessMgr, OrganWebEnv *env, UInt32 scnSize) : OrganWebController(sessMgr, env, scnSize)
{
	this->AddService(CSTR("/api/publicpoi"), Net::WebUtil::RequestMethod::HTTP_GET, SvcPublicPOI);
	this->AddService(CSTR("/api/grouppoi"), Net::WebUtil::RequestMethod::HTTP_GET, SvcGroupPOI);
	this->AddService(CSTR("/api/speciespoi"), Net::WebUtil::RequestMethod::HTTP_GET, SvcSpeciesPOI);
	this->AddService(CSTR("/api/daypoi"), Net::WebUtil::RequestMethod::HTTP_GET, SvcDayPOI);
}

SSWR::OrganWeb::OrganWebPOIController::~OrganWebPOIController()
{
}

