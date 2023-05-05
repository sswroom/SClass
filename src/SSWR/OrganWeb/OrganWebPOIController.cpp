#include "Stdafx.h"
#include "SSWR/OrganWeb/OrganWebEnv.h"
#include "SSWR/OrganWeb/OrganWebPOIController.h"
#include "Text/JSText.h"

#include <stdio.h>


Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcPublicPOI(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	Sync::RWMutexUsage mutUsage;
	GroupInfo *poiGroup = me->env->GroupGet(&mutUsage, 21593);
	Text::StringBuilderUTF8 sb;
	sb.AppendUTF8Char('[');
	me->AddGroupPOI(&mutUsage, &sb, poiGroup, 0);
	mutUsage.EndUse();
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

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcGroupPOI(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);
	Text::StringBuilderUTF8 sb;
	sb.AppendUTF8Char('[');
	Int32 groupId;
	if (req->GetQueryValueI32(CSTR("id"), &groupId))
	{
		Sync::RWMutexUsage mutUsage;
		GroupInfo *poiGroup = me->env->GroupGet(&mutUsage, groupId);
		if (poiGroup)
		{
			if (env.user != 0)
				me->AddGroupPOI(&mutUsage, &sb, poiGroup, env.user->id);
			else
				me->AddGroupPOI(&mutUsage, &sb, poiGroup, 0);
			if (sb.GetLength() > 1)
			{
				sb.RemoveChars(3);
			}
		}
	}
	sb.AppendUTF8Char(']');
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("application/json"));
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToString(), sb.GetLength());
	return true;
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcSpeciesPOI(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);
	Text::StringBuilderUTF8 sb;
	sb.AppendUTF8Char('[');
	Int32 speciesId;
	if (req->GetQueryValueI32(CSTR("id"), &speciesId))
	{
		Sync::RWMutexUsage mutUsage;
		SpeciesInfo *poiSpecies = me->env->SpeciesGet(&mutUsage, speciesId);
		if (poiSpecies)
		{
			if (env.user != 0)
				me->AddSpeciesPOI(&mutUsage, &sb, poiSpecies, env.user->id, me->env->GroupIsPublic(&mutUsage, poiSpecies->groupId));
			else
				me->AddSpeciesPOI(&mutUsage, &sb, poiSpecies, 0, me->env->GroupIsPublic(&mutUsage, poiSpecies->groupId));
			if (sb.GetLength() > 1)
			{
				sb.RemoveChars(3);
			}
		}
	}
	sb.AppendUTF8Char(']');
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("application/json"));
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToString(), sb.GetLength());
	return true;
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcDayPOI(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);
	Text::StringBuilderUTF8 sb;
	sb.AppendUTF8Char('[');
	Int32 dayId;
	if (env.user == 0)
	{
		printf("SvcDayPOI: user == null\r\n");
	}
	else if (req->GetQueryValueI32(CSTR("id"), &dayId))
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
		UserFileInfo *userFile;
		SpeciesInfo *sp;

		while (startIndex < endIndex)
		{
			userFile = env.user->userFileObj.GetItem((UOSInt)startIndex);
			sp = me->env->SpeciesGet(&mutUsage, userFile->speciesId);
			me->AddUserfilePOI(&sb, sp, userFile);
			startIndex++;
		}
		if (sb.GetLength() > 1)
		{
			sb.RemoveChars(3);
		}
	}
	sb.AppendUTF8Char(']');
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("application/json"));
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToString(), sb.GetLength());
	return true;
}


void SSWR::OrganWeb::OrganWebPOIController::AddGroupPOI(Sync::RWMutexUsage *mutUsage, Text::StringBuilderUTF8 *sb, GroupInfo *group, Int32 userId)
{
	UOSInt i = 0;
	UOSInt j = group->groups.GetCount();
	while (i < j)
	{
		AddGroupPOI(mutUsage, sb, group->groups.GetItem(i), userId);
		i++;
	}
	i = 0;
	j = group->species.GetCount();
	while (i < j)
	{
		AddSpeciesPOI(mutUsage, sb, group->species.GetItem(i), userId, this->env->GroupIsPublic(mutUsage, group->id));
		i++;
	}
}

void SSWR::OrganWeb::OrganWebPOIController::AddSpeciesPOI(Sync::RWMutexUsage *mutUsage, Text::StringBuilderUTF8 *sb, SpeciesInfo *species, Int32 userId, Bool publicGroup)
{
	UserFileInfo *file;
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
		file = species->files.GetItem(k);
		if ((file->lat != 0 || file->lon != 0) && (publicGroup || adminUser || file->webuserId == userId))
		{
			this->AddUserfilePOI(sb, species, file);
		}
		k++;
	}
}

void SSWR::OrganWeb::OrganWebPOIController::AddUserfilePOI(Text::StringBuilderUTF8 *sb, SpeciesInfo *species, UserFileInfo *file)
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
	sb->AppendC(UTF8STRC("\"},\r\n"));
}

SSWR::OrganWeb::OrganWebPOIController::OrganWebPOIController(Net::WebServer::MemoryWebSessionManager *sessMgr, OrganWebEnv *env, UInt32 scnSize) : OrganWebController(sessMgr, env, scnSize)
{
	this->AddService(CSTR("/publicpoi"), Net::WebUtil::RequestMethod::HTTP_GET, SvcPublicPOI);
	this->AddService(CSTR("/grouppoi"), Net::WebUtil::RequestMethod::HTTP_GET, SvcGroupPOI);
	this->AddService(CSTR("/speciespoi"), Net::WebUtil::RequestMethod::HTTP_GET, SvcSpeciesPOI);
	this->AddService(CSTR("/daypoi"), Net::WebUtil::RequestMethod::HTTP_GET, SvcDayPOI);
}

SSWR::OrganWeb::OrganWebPOIController::~OrganWebPOIController()
{
}

