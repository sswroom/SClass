#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Media/IAudioSource.h"
#include "Media/MediaFile.h"
#include "Media/PhotoInfo.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "SSWR/OrganWeb/OrganWebEnv.h"
#include "SSWR/OrganWeb/OrganWebPOIController.h"
#include "Text/JSText.h"

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcLang(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	IO::ConfigFile *lang = me->env->LangGet(req);
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	if (lang)
	{
		Data::ArrayListNN<Text::String> keys;
		NotNullPtr<Text::String> key;
		lang->GetKeys(CSTR(""), keys);
		Data::ArrayIterator<NotNullPtr<Text::String>> it = keys.Iterator();
		while (it.HasNext())
		{
			key = it.Next();
			json.ObjectAddStr(key->ToCString(), lang->GetValue(key));
		}
	}
	return me->ResponseJSON(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcLoginInfo(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	NotNullPtr<WebUserInfo> user;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	json.ObjectAddBool(CSTR("isMobile"), env.isMobile);
	json.ObjectAddInt32(CSTR("scnWidth"), (Int32)env.scnWidth);
	json.ObjectAddUInt64(CSTR("previewSize"), GetPreviewSize());
	json.ObjectAddInt32(CSTR("pickObjType"), env.pickObjType);
	json.ObjectAddArrayInt32(CSTR("pickObjs"), env.pickObjs);
	if (user.Set(env.user))
	{
		json.ObjectBeginObject(CSTR("user"));
		AppendUser(json, user);
		json.ObjectEnd();
		if (user->userType == UserType::Admin)
		{
			Sync::RWMutexUsage mutUsage;
			BookInfo *book = me->env->BookGetSelected(mutUsage);
			json.ObjectAddInt32(CSTR("selectedBook"), book?book->id:0);
		}
	}
	return me->ResponseJSON(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcLogin(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	Text::CStringNN msg;
	if (env.user)
	{
		msg = CSTR("ok");
	}
	else
	{
		UTF8Char sbuff[128];
		UTF8Char *sptr;
		req->ParseHTTPForm();
		Text::String *userName = req->GetHTTPFormStr(CSTR("userName"));
		Text::String *pwd = req->GetHTTPFormStr(CSTR("password"));
		if (userName && pwd && userName->leng > 0 && pwd->leng > 0)
		{
			Sync::RWMutexUsage mutUsage;
			sptr = me->env->PasswordEnc(sbuff, pwd->ToCString());
			env.user = me->env->UserGetByName(mutUsage, userName);
			if (env.user && env.user->pwd->Equals(sbuff, (UOSInt)(sptr - sbuff)))
			{
				mutUsage.EndUse();
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
				msg = CSTR("ok");
			}
			else
			{
				msg = CSTR("Invalid username or password");
			}
		}
		else if (userName == 0 || userName->leng == 0)
		{
			msg = CSTR("Username is missing");
		}
		else
		{
			msg = CSTR("Password is missing");
		}
	}
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	json.ObjectAddStr(CSTR("status"), msg);
	return me->ResponseJSON(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcLogout(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	me->sessMgr->DeleteSession(req, resp);
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	json.ObjectAddStr(CSTR("status"), CSTR("ok"));
	return me->ResponseJSON(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcCateList(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	Sync::RWMutexUsage mutUsage;
	Bool notAdmin = (env.user == 0 || env.user->userType != UserType::Admin);
	Data::ReadingList<CategoryInfo*> *cateList = me->env->CateGetList(mutUsage);
	CategoryInfo *cate;
	Text::JSONBuilder json(Text::JSONBuilder::OT_ARRAY);
	UOSInt i = 0;
	UOSInt j = cateList->GetCount();
	while (i < j)
	{
		cate = cateList->GetItem(i);
		if ((cate->flags & 1) == 0 || !notAdmin)
		{
			json.ArrayBeginObject();
			json.ObjectAddInt32(CSTR("cateId"), cate->cateId);
			json.ObjectAddStr(CSTR("chiName"), cate->chiName);
			json.ObjectAddStr(CSTR("dirName"), cate->dirName);
			json.ObjectAddInt32(CSTR("flags"), cate->flags);
			json.ObjectEnd();
		}
		i++;
	}
	return me->ResponseJSON(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcCate(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	Sync::RWMutexUsage mutUsage;
	CategoryInfo *cate;
	NotNullPtr<GroupInfo> group;
	Text::String *cateName = req->GetQueryValue(CSTR("cateName"));
	if (cateName != 0 && (cate = me->env->CateGetByName(mutUsage, cateName)) != 0)
	{
		Bool notAdmin = (env.user == 0 || env.user->userType != UserType::Admin);
		if ((cate->flags & 1) && notAdmin)
		{
			return resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		}
		Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
		json.ObjectAddInt32(CSTR("cateId"), cate->cateId);
		json.ObjectAddStr(CSTR("chiName"), cate->chiName);
		json.ObjectBeginArray(CSTR("groups"));
		UOSInt i = 0;
		UOSInt j = cate->groups.GetCount();
		while (i < j)
		{
			if (group.Set(cate->groups.GetItem(i)))
			{
				me->env->CalcGroupCount(mutUsage, group);
				if (!notAdmin || (group->totalCount > 0 && (group->flags & 1) == 0))
				{
					json.ArrayBeginObject();
					me->AddGroup(json, group);
					json.ObjectEnd();
				}
			}
			i++;
		}
		json.ArrayEnd();
		return me->ResponseJSON(req, resp, 0, json.Build());
	}
	else
	{
		return resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcYearList(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	Text::JSONBuilder json(Text::JSONBuilder::OT_ARRAY);
	if (env.user)
	{
		Data::DateTime dt;
		OSInt endIndex = (OSInt)env.user->userFileIndex.GetCount();
		OSInt startIndex;
		Int64 currTime = env.user->userFileIndex.GetItem((UOSInt)endIndex - 1);
		Int64 thisTicks;
		if (endIndex > 0)
		{
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
				json.ArrayAddInt32((Int32)dt.GetYear());
				if (startIndex <= 0)
					break;
				endIndex = startIndex;
				currTime = env.user->userFileIndex.GetItem((UOSInt)endIndex - 1);
			}
		}
	}
	return me->ResponseJSON(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcDayList(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	req->ParseHTTPForm();
	Int32 year;
	Int32 tzQhr = 0;
	req->GetHTTPFormInt32(CSTR("tzQhr"), tzQhr);
	if (req->GetHTTPFormInt32(CSTR("year"), year))
	{
		Data::DateTime dt;
		Data::DateTime dt2;
		Text::JSONBuilder json(Text::JSONBuilder::OT_ARRAY);

		Sync::RWMutexUsage mutUsage;
		Int64 startTime;
		Int64 endTime;
		OSInt startIndex;
		OSInt endIndex;
		dt.SetTimeZoneQHR((Int8)tzQhr);
		dt.SetValue((UInt16)(year + 1), 1, 1, 0, 0, 0, 0);
		endTime = dt.ToTicks();
		dt.SetValue(year, 1, 1, 0, 0, 0, 0);
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
		UserFileInfo *userFile;
		SpeciesInfo *sp;
		Text::StringBuilderUTF8 sb;
		Data::ArrayListStringNN locList;
		NotNullPtr<Text::String> unkLoc = Text::String::New(UTF8STRC("?"));
		OSInt si;
		UOSInt i;
		UOSInt j;

		while (startIndex < endIndex)
		{
			dt.SetTicks(env.user->userFileIndex.GetItem((UOSInt)startIndex));
			if (dt.GetMonth() != month || dt.GetDay() != day)
			{
				if (month != 0 && day != 0)
				{
					userFile = env.user->userFileObj.GetItem((UOSInt)(dayStartIndex + startIndex) >> 1);
					sp = me->env->SpeciesGet(mutUsage, userFile->speciesId);

					json.ArrayBeginObject();
					json.ObjectAddInt64(CSTR("day"), Data::Date(year, month, day).GetTotalDays());
					json.ObjectAddInt32(CSTR("photoSpId"), userFile->speciesId);
					json.ObjectAddInt32(CSTR("photoCateId"), sp->cateId);
					json.ObjectAddInt32(CSTR("photoFileId"), userFile->id);
					json.ObjectAddInt32(CSTR("count"), (Int32)(startIndex - dayStartIndex));
					json.ObjectBeginArray(CSTR("locs"));
					i = 0;
					j = locList.GetCount();
					while (i < j)
					{
						json.ArrayAddStr(locList.GetItem(i));
						i++;
					}
					json.ArrayEnd();
					json.ObjectEnd();
				}

				month = dt.GetMonth();
				day = dt.GetDay();
				dayStartIndex = startIndex;
				locList.Clear();
			}

			NotNullPtr<Text::String> locName = unkLoc;
			userFile = env.user->userFileObj.GetItem((UOSInt)startIndex);
			locName.Set(userFile->location);
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
			sp = me->env->SpeciesGet(mutUsage, userFile->speciesId);

			json.ArrayBeginObject();
			json.ObjectAddInt64(CSTR("day"), Data::Date(year, month, day).GetTotalDays());
			json.ObjectAddInt32(CSTR("photoSpId"), userFile->speciesId);
			json.ObjectAddInt32(CSTR("photoCateId"), sp->cateId);
			json.ObjectAddInt32(CSTR("photoFileId"), userFile->id);
			json.ObjectAddInt32(CSTR("count"), (Int32)(startIndex - dayStartIndex));
			json.ObjectBeginArray(CSTR("locs"));
			i = 0;
			j = locList.GetCount();
			while (i < j)
			{
				json.ArrayAddStr(locList.GetItem(i));
				i++;
			}
			json.ArrayEnd();
			json.ObjectEnd();
		}
		mutUsage.EndUse();
		unkLoc->Release();
		return me->ResponseJSON(req, resp, 0, json.Build());
	}
	else
	{
		return me->ResponseJSON(req, resp, 0, CSTR("[]"));
	}
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcDayDetail(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	req->ParseHTTPForm();
	Int32 d;
	Int32 tzQhr = 0;
	req->GetHTTPFormInt32(CSTR("tzQhr"), tzQhr);
	if (env.user != 0 && req->GetHTTPFormInt32(CSTR("d"), d))
	{
		Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
		Data::DateTime dt;
		dt.SetTicks(d * 86400000LL);
		dt.SetTimeZoneQHR((Int8)tzQhr);

		Sync::RWMutexUsage mutUsage;
		Int64 startTime;
		Int64 endTime;
		OSInt startIndex;
		OSInt endIndex;
		Text::StringBuilderUTF8 sb;
		startTime = dt.ToTicks();
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
		NotNullPtr<SpeciesInfo> sp;
		Data::ArrayListInt32 spList;
		OSInt si;
		json.ObjectBeginArray(CSTR("userFiles"));
			
		while (startIndex < endIndex)
		{
			userFile = env.user->userFileObj.GetItem((UOSInt)startIndex);
			if (sp.Set(me->env->SpeciesGet(mutUsage, userFile->speciesId)))
			{
				si = spList.SortedIndexOf(userFile->speciesId);
				if (si < 0)
				{
					spList.Insert((UOSInt)~si, userFile->speciesId);
				}

				json.ArrayBeginObject();
				json.ObjectAddInt32(CSTR("id"), userFile->id);
				json.ObjectAddInt32(CSTR("index"), (Int32)startIndex);
				json.ObjectAddStr(CSTR("oriFileName"), userFile->oriFileName);
				json.ObjectAddInt64(CSTR("fileTimeTicks"), userFile->fileTimeTicks);
				json.ObjectAddFloat64(CSTR("lat"), userFile->lat);
				json.ObjectAddFloat64(CSTR("lon"), userFile->lon);
				json.ObjectAddInt32(CSTR("webuserId"), userFile->webuserId);
				json.ObjectAddInt32(CSTR("speciesId"), userFile->speciesId);
				json.ObjectAddInt64(CSTR("captureTimeTicks"), userFile->captureTimeTicks);
				json.ObjectAddInt32(CSTR("rotType"), userFile->rotType);
				json.ObjectAddFloat64(CSTR("cropLeft"), userFile->cropLeft);
				json.ObjectAddFloat64(CSTR("cropTop"), userFile->cropTop);
				json.ObjectAddFloat64(CSTR("cropRight"), userFile->cropRight);
				json.ObjectAddFloat64(CSTR("cropBottom"), userFile->cropBottom);
				json.ObjectAddStr(CSTR("descript"), userFile->descript);
				json.ObjectAddStr(CSTR("location"), userFile->location);
				json.ObjectAddInt32(CSTR("cateId"), sp->cateId);
				json.ObjectEnd();
			}
			startIndex++;
		}
		json.ArrayEnd();
		json.ObjectBeginArray(CSTR("dataFiles"));
		me->AppendDataFiles(json, env.user->gpsDataFiles, startTime, endTime);
		me->AppendDataFiles(json, env.user->tempDataFiles, startTime, endTime);
		json.ArrayEnd();
		if (env.user->userType == UserType::Admin)
		{
			json.ObjectBeginArray(CSTR("spList"));
			UOSInt i = 0;
			UOSInt j = spList.GetCount();
			while (i < j)
			{
				if (sp.Set(me->env->SpeciesGet(mutUsage, spList.GetItem(i))))
				{
					json.ArrayBeginObject();
					me->AppendSpecies(json, sp, mutUsage);
					json.ObjectEnd();
				}
				i++;
			}
			json.ArrayEnd();
		}
		return me->ResponseJSON(req, resp, 0, json.Build());
	}
	else
	{
		return me->ResponseJSON(req, resp, 0, CSTR("{}"));
	}

}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcBookList(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	Sync::RWMutexUsage mutUsage;
	Data::ArrayList<BookInfo*> bookList;
	BookInfo *book;
	Text::JSONBuilder json(Text::JSONBuilder::OT_ARRAY);
	me->env->BookGetList(mutUsage, bookList);

	UOSInt i = 0;
	UOSInt j = bookList.GetCount();
	while (i < j)
	{
		book = bookList.GetItem(i);
		json.ArrayBeginObject();
		json.ObjectAddInt32(CSTR("id"), book->id);
		json.ObjectAddUInt64(CSTR("speciesCount"), book->species.GetCount());
		json.ObjectAddInt32(CSTR("userfileId"), book->userfileId);
		json.ObjectAddInt64(CSTR("publishDateTicks"), book->publishDate);
		json.ObjectAddStr(CSTR("author"), book->author);
		json.ObjectAddStr(CSTR("title"), book->title);
		json.ObjectAddStr(CSTR("press"), book->press);
		json.ObjectAddStr(CSTR("url"), book->url);
		json.ObjectEnd();

		i++;
	}
	return me->ResponseJSON(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcBookSelect(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	Text::CStringNN json;
	if (env.user && env.user->userType == UserType::Admin)
	{
		req->ParseHTTPForm();
		Sync::RWMutexUsage mutUsage;
		Int32 id;
		if (req->GetHTTPFormInt32(CSTR("id"), id))
		{
			BookInfo *book = me->env->BookGet(mutUsage, id);
			if (book)
			{
				me->env->BookSelect(book);
				json = CSTR("{\"status\":\"ok\"}");
			}
			else
			{
				json = CSTR("{\"status\":\"id invalid\"}");
			}
		}
		else
		{
			json = CSTR("{\"status\":\"id not found\"}");
		}
	}
	else
	{
		json = CSTR("{\"status\":\"access denied\"}");
	}
	return me->ResponseJSON(req, resp, 0, json);
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcBookUnselect(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	if (env.user && env.user->userType == UserType::Admin)
	{
		me->env->BookSelect(0);
	}
	return me->ResponseJSON(req, resp, 0, CSTR("{}"));
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcBookAdd(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	Text::CStringNN msg;
	if (env.user == 0 || env.user->userType != UserType::Admin)
	{
		msg = CSTR("Access denied");
	}
	else
	{
		Text::String *title = 0;
		Text::String *author = 0;
		Text::String *press = 0;
		Text::String *pubDate = 0;
		Text::String *url = 0;
		req->ParseHTTPForm();
		title = req->GetHTTPFormStr(CSTR("title"));
		author = req->GetHTTPFormStr(CSTR("author"));
		press = req->GetHTTPFormStr(CSTR("press"));
		pubDate = req->GetHTTPFormStr(CSTR("pubDate"));
		url = req->GetHTTPFormStr(CSTR("url"));
		Data::Timestamp ts;
		if (title == 0 || title->leng == 0)
		{
			msg = CSTR("Book Name is empty");
		}
		else if (author == 0 || author->leng == 0)
		{
			msg = CSTR("Author is empty");
		}
		else if (press == 0 || press->leng == 0)
		{
			msg = CSTR("Press is empty");
		}
		else if (pubDate == 0 || (ts = Data::Timestamp(pubDate->ToCString(), 0)).IsNull())
		{
			msg = CSTR("Publish Date is not valid");
		}
		else if (url == 0 || (url->leng > 0 && !url->StartsWith(UTF8STRC("http://")) && !url->StartsWith(UTF8STRC("https://"))))
		{
			msg = CSTR("URL is not valid");
		}
		else
		{
			Sync::RWMutexUsage mutUsage;
			if (me->env->BookAdd(mutUsage, title, author, press, ts, url))
			{
				msg = CSTR("ok");
			}
			else
			{
				msg = CSTR("Error in adding book");
			}
		}
	}
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	json.ObjectAddStr(CSTR("status"), msg);
	return me->ResponseJSON(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcBookDetail(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	Sync::RWMutexUsage mutUsage;
	Int32 bookId;
	NotNullPtr<BookInfo> book;
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	if (req->GetQueryValueI32(CSTR("id"), bookId) && book.Set(me->env->BookGet(mutUsage, bookId)))
	{
		json.ObjectAddInt32(CSTR("id"), book->id);
		json.ObjectAddUInt64(CSTR("speciesCount"), book->species.GetCount());
		json.ObjectAddInt32(CSTR("userfileId"), book->userfileId);
		json.ObjectAddInt64(CSTR("publishDateTicks"), book->publishDate);
		json.ObjectAddStr(CSTR("author"), book->author);
		json.ObjectAddStr(CSTR("title"), book->title);
		json.ObjectAddStr(CSTR("press"), book->press);
		json.ObjectAddStr(CSTR("url"), book->url);
		if (book->userfileId != 0)
		{
			UserFileInfo *userFile = me->env->UserfileGet(mutUsage, book->userfileId);
			if (userFile)
			{
				SpeciesInfo *sp = me->env->SpeciesGet(mutUsage, userFile->speciesId);
				if (sp)
				{
					json.ObjectBeginObject(CSTR("userfile"));
					json.ObjectAddInt32(CSTR("id"), userFile->id);
					json.ObjectAddInt32(CSTR("speciesId"), userFile->speciesId);
					json.ObjectAddInt32(CSTR("cateId"), sp->cateId);
					json.ObjectEnd();
				}
			}
		}
		if (env.user && env.user->userType == UserType::Admin)
		{
			json.ObjectAddBool(CSTR("hasFile"), me->env->BookFileExist(book));
		}

		json.ObjectBeginArray(CSTR("species"));
		UOSInt i = 0;
		UOSInt j = book->species.GetCount();
		while (i < j)
		{
			BookSpInfo *bookSp = book->species.GetItem(i);
			NotNullPtr<SpeciesInfo> species;
			if (species.Set(me->env->SpeciesGet(mutUsage, bookSp->speciesId)))
			{
				json.ArrayBeginObject();
				json.ObjectAddStr(CSTR("dispName"), bookSp->dispName);
				me->AppendSpecies(json, species, mutUsage);
				json.ObjectEnd();
			}
			i++;
		}
		json.ArrayEnd();
	}
	return me->ResponseJSON(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcPhotoDetail(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	req->ParseHTTPForm();
	Int32 id;
	Int32 cateId;
	Int32 fileId;
	if (req->GetHTTPFormInt32(CSTR("id"), id) &&
		req->GetHTTPFormInt32(CSTR("cateId"), cateId))
	{
		SpeciesInfo *species;
		GroupInfo *group;
		CategoryInfo *cate;
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		UOSInt i;
		UOSInt j;
		Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
		Sync::RWMutexUsage mutUsage;
		species = me->env->SpeciesGet(mutUsage, id);
		if (species == 0 || species->cateId != cateId)
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		group = me->env->GroupGet(mutUsage, species->groupId);
		if (group == 0)
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->env->CateGet(mutUsage, group->cateId);
		if (cate == 0)
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		if (req->GetHTTPFormInt32(CSTR("fileId"), fileId))
		{
			Bool found = false;
			UserFileInfo *userFile;
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

				if (userFile->fileType == FileType::Audio)
				{
					sptr = me->env->UserfileGetPath(sbuff, userFile);
					UInt64 fileSize = 0;
					Media::MediaFile *mediaFile;
					{
						IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
						fileSize = fd.GetDataSize();
						mediaFile = (Media::MediaFile*)me->env->ParseFileType(fd, IO::ParserType::MediaFile);
					}

					if (mediaFile)
					{
						json.ObjectAddUInt64(CSTR("fileSize"), fileSize);
						Media::IMediaSource *msrc = mediaFile->GetStream(0, 0);
						Data::Duration stmTime;
						if (msrc)
						{
							stmTime = msrc->GetStreamTime();
							json.ObjectAddInt64(CSTR("stmTime"), stmTime.GetTotalMS());

							if (msrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
							{
								Media::IAudioSource *asrc = (Media::IAudioSource*)msrc;
								Media::AudioFormat format;
								asrc->GetFormat(format);
								json.ObjectAddUInt64(CSTR("frequency"), format.frequency);
								json.ObjectAddUInt64(CSTR("bitPerSample"), format.bitpersample);
								json.ObjectAddUInt64(CSTR("nChannels"), format.nChannels);
							}
						}
						DEL_CLASS(mediaFile);
					}
				}
				else
				{
					sptr = me->env->UserfileGetPath(sbuff, userFile);
					IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
					Media::PhotoInfo info(fd);
					if (info.HasInfo())
					{
						Text::StringBuilderUTF8 sb;
						info.ToString(sb);
						json.ObjectAddStr(CSTR("photoInfo"), sb.ToCString());
						json.ObjectAddUInt64(CSTR("imgWidth"), info.GetWidth());
						json.ObjectAddUInt64(CSTR("imgHeight"), info.GetHeight());
					}
				}

			}
		}
		else if (req->GetHTTPFormInt32(CSTR("fileWId"), fileId))
		{
			WebFileInfo *wfile = species->wfiles.Get(fileId);
			if (wfile)
			{
				json.ObjectAddStr(CSTR("imgUrl"), wfile->imgUrl);
				json.ObjectAddStr(CSTR("srcUrl"), wfile->srcUrl);

				sptr = me->env->GetDataDir()->ConcatTo(sbuff);
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
				mutUsage.EndUse();;

				IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
				Media::PhotoInfo info(fd);
				if (info.HasInfo())
				{
					Text::StringBuilderUTF8 sb;
					info.ToString(sb);
					json.ObjectAddStr(CSTR("photoInfo"), sb.ToCString());
					json.ObjectAddUInt64(CSTR("imgWidth"), info.GetWidth());
					json.ObjectAddUInt64(CSTR("imgHeight"), info.GetHeight());
				}
			}
		}

		return me->ResponseJSON(req, resp, 0, json.Build());
	}
	else
	{
		return me->ResponseJSON(req, resp, 0, CSTR("{}"));
	}
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcPhotoUpload(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	if (env.user == 0)
	{
		resp->ResponseError(req, Net::WebStatus::SC_FORBIDDEN);
		return true;
	}
	Sync::RWMutexUsage mutUsage;
	UOSInt i = 0;
	UOSInt fileSize;
	UTF8Char fileName[512];
	UTF8Char *fileNameEnd;
	const UInt8 *fileCont;
	Text::String *location;
	Bool succ = true;
	req->ParseHTTPForm();

	while (true)
	{
		fileCont = req->GetHTTPFormFile(CSTR("file"), i, fileName, sizeof(fileName), &fileNameEnd, fileSize);
		if (fileCont == 0)
		{
			break;
		}
		location = req->GetHTTPFormStr(CSTR("location"));
		if (!me->env->UserfileAdd(mutUsage, env.user->id, env.user->unorganSpId, CSTRP(fileName, fileNameEnd), fileCont, fileSize, true, location))
			succ = false;

		i++;
	}
	return me->ResponseJSON(req, resp, 0, succ?CSTR("{\"status\":\"ok\"}"):CSTR("{\"status\":\"fail\"}"));
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcReload(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	if (me->env->HasReloadPwd() && env.user && env.user->userType == UserType::Admin)
	{
		NotNullPtr<Text::String> pwd;
		req->ParseHTTPForm();
		if (pwd.Set(req->GetHTTPFormStr(CSTR("pwd"))) && me->env->ReloadPwdMatches(pwd))
		{
			me->env->Reload();
			return me->ResponseJSON(req, resp, 0, CSTR("{\"status\":\"Reloaded\"}"));
		}
		else
		{
			return me->ResponseJSON(req, resp, 0, CSTR("{\"status\":\"Password Error\"}"));
		}
	}
	else
	{
		return resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
}

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
	me->AddSpeciesList(json, speciesList, mutUsage);
	return me->ResponseJSON(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcGroupPOI(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	Sync::RWMutexUsage mutUsage;
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	Data::ArrayListNN<GroupInfo> groups;
	Data::ArrayListNN<SpeciesInfo> speciesList;
	Int32 groupId;
	json.ObjectBeginArray(CSTR("poi"));
	if (req->GetQueryValueI32(CSTR("id"), groupId))
	{
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
	me->AddSpeciesList(json, speciesList, mutUsage);
	return me->ResponseJSON(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcSpeciesPOI(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	Sync::RWMutexUsage mutUsage;
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	Data::ArrayListNN<SpeciesInfo> speciesList;
	Int32 speciesId;
	json.ObjectBeginArray(CSTR("poi"));
	if (req->GetQueryValueI32(CSTR("id"), speciesId))
	{
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
	me->AddSpeciesList(json, speciesList, mutUsage);
	return me->ResponseJSON(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcDayPOI(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPOIController *me = (SSWR::OrganWeb::OrganWebPOIController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
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
	return me->ResponseJSON(req, resp, 0, json.Build());
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
		if (user && user->userType == UserType::Admin)
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
}

void SSWR::OrganWeb::OrganWebPOIController::AddGroups(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<Data::ArrayListNN<GroupInfo>> groups)
{
	NotNullPtr<GroupInfo> group;
	Data::ArrayIterator<NotNullPtr<GroupInfo>> it = groups->Iterator();
	while (it.HasNext())
	{
		group = it.Next();
		json->ArrayBeginObject();
		this->AddGroup(json, group);
		json->ObjectEnd();
	}
}

void SSWR::OrganWeb::OrganWebPOIController::AddGroup(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<GroupInfo> group)
{
	json->ObjectAddInt32(CSTR("id"), group->id);
	json->ObjectAddInt32(CSTR("parentId"), group->parentId);
	json->ObjectAddStr(CSTR("engName"), group->engName);
	json->ObjectAddStr(CSTR("chiName"), group->chiName);
	json->ObjectAddStr(CSTR("descript"), group->descript);
	json->ObjectAddInt32(CSTR("cateId"), group->cateId);
	json->ObjectAddInt64(CSTR("photoCount"), (Int64)group->photoCount);
	json->ObjectAddInt64(CSTR("myPhotoCount"), (Int64)group->myPhotoCount);
	json->ObjectAddInt64(CSTR("totalCount"), (Int64)group->totalCount);
	json->ObjectAddInt32(CSTR("flags"), group->flags);
	json->ObjectAddInt32(CSTR("groupType"), group->groupType);
	if (group->photoSpObj)
	{
		json->ObjectAddInt32(CSTR("photoSpId"), group->photoSpObj->speciesId);
		if (group->photoSpObj->photoId != 0)
		{
			json->ObjectAddInt32(CSTR("photoId"), group->photoSpObj->photoId);
		}
		else if (group->photoSpObj->photoWId != 0)
		{
			json->ObjectAddInt32(CSTR("photoWId"), group->photoSpObj->photoWId);
		}
		else if (group->photoSpObj->photo && group->photoSpObj->photo->leng > 0)
		{
			json->ObjectAddStr(CSTR("photoWId"), group->photoSpObj->photo);
		}
	}
}

void SSWR::OrganWeb::OrganWebPOIController::AddSpeciesList(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<Data::ArrayListNN<SpeciesInfo>> speciesList, NotNullPtr<Sync::RWMutexUsage> mutUsage)
{
	NotNullPtr<SpeciesInfo> species;
	Data::ArrayIterator<NotNullPtr<SpeciesInfo>> it = speciesList->Iterator();
	while (it.HasNext())
	{
		species = it.Next();
		json->ArrayBeginObject();
		AppendSpecies(json, species, mutUsage);
		json->ObjectEnd();
	}
}

void SSWR::OrganWeb::OrganWebPOIController::AppendUser(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<WebUserInfo> user)
{
	json->ObjectAddInt32(CSTR("id"), user->id);
	json->ObjectAddInt32(CSTR("userType"), (Int32)user->userType);
	json->ObjectAddStr(CSTR("userName"), user->userName);
	json->ObjectAddStr(CSTR("watermark"), user->watermark);
}

void SSWR::OrganWeb::OrganWebPOIController::AppendSpecies(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<SpeciesInfo> species, NotNullPtr<Sync::RWMutexUsage> mutUsage)
{
	json->ObjectAddInt32(CSTR("id"), species->speciesId);
	json->ObjectAddInt32(CSTR("groupId"), species->groupId);
	json->ObjectAddStr(CSTR("sciName"), species->sciName);
	json->ObjectAddStr(CSTR("chiName"), species->chiName);
	json->ObjectAddStr(CSTR("engName"), species->engName);
	json->ObjectAddStr(CSTR("descript"), species->descript);
	json->ObjectAddStr(CSTR("poiImg"), species->poiImg);
	json->ObjectAddInt32(CSTR("flags"), species->flags);
	json->ObjectAddStr(CSTR("idKey"), species->idKey);
	json->ObjectAddInt32(CSTR("cateId"), species->cateId);
	if (species->files.GetCount() > 0)
	{
		if ((species->flags & SSWR::OrganWeb::SF_HAS_MYPHOTO) == 0)
		{
			this->env->SpeciesSetFlags(mutUsage, species->speciesId, (SSWR::OrganWeb::SpeciesFlags)(species->flags | SSWR::OrganWeb::SF_HAS_MYPHOTO));
			this->env->GroupAddCounts(mutUsage, species->groupId, 0, (species->flags & SSWR::OrganWeb::SF_HAS_WEBPHOTO)?0:1, 1);
		}
	}
	else
	{
		if (species->flags & SSWR::OrganWeb::SF_HAS_MYPHOTO)
		{
			this->env->SpeciesSetFlags(mutUsage, species->speciesId, (SSWR::OrganWeb::SpeciesFlags)(species->flags & ~SSWR::OrganWeb::SF_HAS_MYPHOTO));
			this->env->GroupAddCounts(mutUsage, species->groupId, 0, (species->flags & SSWR::OrganWeb::SF_HAS_WEBPHOTO)?0:(UOSInt)-1, (UOSInt)-1);
		}
	}
	if (species->photoId != 0)
	{
		json->ObjectAddInt32(CSTR("photoId"), species->photoId);
	}
	else if (species->photoWId != 0)
	{
		json->ObjectAddInt32(CSTR("photoWId"), species->photoWId);
	}
	else if (species->photo && species->photo->leng > 0)
	{
		json->ObjectAddStr(CSTR("photo"), species->photo);
	}
	json->ObjectBeginArray(CSTR("books"));
	UOSInt i = 0;
	UOSInt j = species->books.GetCount();
	while (i < j)
	{
		BookSpInfo *bookSp = species->books.GetItem(i);
		BookInfo *book = this->env->BookGet(mutUsage, bookSp->bookId);
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
}

void SSWR::OrganWeb::OrganWebPOIController::AppendDataFiles(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<Data::FastMap<Data::Timestamp, DataFileInfo*>> dataFiles, Int64 startTime, Int64 endTime)
{
	OSInt startIndex;
	OSInt endIndex;
	Text::StringBuilderUTF8 sb;
	startIndex = dataFiles->GetIndex(Data::Timestamp(startTime, 0));
	if (startIndex < 0)
		startIndex = ~startIndex;
	if (startIndex > 0 && dataFiles->GetItem((UOSInt)startIndex - 1)->endTime.ToTicks() > startTime)
		startIndex--;
	endIndex = dataFiles->GetIndex(Data::Timestamp(endTime, 0));
	if (endIndex < 0)
		endIndex = ~endIndex;
	while (startIndex < endIndex)
	{
		DataFileInfo *dataFile = dataFiles->GetItem((UOSInt)startIndex);
		json->ArrayBeginObject();
		json->ObjectAddInt32(CSTR("id"), dataFile->id);
		json->ObjectAddInt32(CSTR("fileType"), (Int32)dataFile->fileType);
		json->ObjectAddInt64(CSTR("startTime"), dataFile->startTime.ToTicks());
		json->ObjectAddInt64(CSTR("endTime"), dataFile->endTime.ToTicks());
		json->ObjectAddStr(CSTR("oriFileName"), dataFile->oriFileName);
		json->ObjectAddInt32(CSTR("webuserId"), dataFile->webuserId);
		json->ObjectEnd();
		startIndex++;
	}
}

Bool SSWR::OrganWeb::OrganWebPOIController::ResponseJSON(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, OSInt cacheAge, Text::CStringNN json)
{
	resp->EnableWriteBuffer();
	resp->AddDefHeaders(req);
	resp->AddCacheControl(cacheAge);
	resp->AddContentType(CSTR("application/json;charset=UTF-8"));
	return Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("application/json"), json);
}

SSWR::OrganWeb::OrganWebPOIController::OrganWebPOIController(Net::WebServer::MemoryWebSessionManager *sessMgr, OrganWebEnv *env, UInt32 scnSize) : OrganWebController(sessMgr, env, scnSize)
{
	this->AddService(CSTR("/api/lang"), Net::WebUtil::RequestMethod::HTTP_GET, SvcLang);
	this->AddService(CSTR("/api/logininfo"), Net::WebUtil::RequestMethod::HTTP_GET, SvcLoginInfo);
	this->AddService(CSTR("/api/login"), Net::WebUtil::RequestMethod::HTTP_POST, SvcLogin);
	this->AddService(CSTR("/api/logout"), Net::WebUtil::RequestMethod::HTTP_GET, SvcLogout);
	this->AddService(CSTR("/api/catelist"), Net::WebUtil::RequestMethod::HTTP_GET, SvcCateList);
	this->AddService(CSTR("/api/cate"), Net::WebUtil::RequestMethod::HTTP_GET, SvcCate);
	this->AddService(CSTR("/api/yearlist"), Net::WebUtil::RequestMethod::HTTP_GET, SvcYearList);
	this->AddService(CSTR("/api/daylist"), Net::WebUtil::RequestMethod::HTTP_POST, SvcDayList);
	this->AddService(CSTR("/api/daydetail"), Net::WebUtil::RequestMethod::HTTP_POST, SvcDayDetail);
	this->AddService(CSTR("/api/booklist"), Net::WebUtil::RequestMethod::HTTP_GET, SvcBookList);
	this->AddService(CSTR("/api/bookselect"), Net::WebUtil::RequestMethod::HTTP_POST, SvcBookSelect);
	this->AddService(CSTR("/api/bookunselect"), Net::WebUtil::RequestMethod::HTTP_GET, SvcBookUnselect);
	this->AddService(CSTR("/api/bookadd"), Net::WebUtil::RequestMethod::HTTP_GET, SvcBookAdd);
	this->AddService(CSTR("/api/bookdetail"), Net::WebUtil::RequestMethod::HTTP_GET, SvcBookDetail);
	this->AddService(CSTR("/api/photodetail"), Net::WebUtil::RequestMethod::HTTP_POST, SvcPhotoDetail);
	this->AddService(CSTR("/api/photoupload"), Net::WebUtil::RequestMethod::HTTP_POST, SvcPhotoUpload);
	this->AddService(CSTR("/api/reload"), Net::WebUtil::RequestMethod::HTTP_POST, SvcReload);
	this->AddService(CSTR("/api/publicpoi"), Net::WebUtil::RequestMethod::HTTP_GET, SvcPublicPOI);
	this->AddService(CSTR("/api/grouppoi"), Net::WebUtil::RequestMethod::HTTP_GET, SvcGroupPOI);
	this->AddService(CSTR("/api/speciespoi"), Net::WebUtil::RequestMethod::HTTP_GET, SvcSpeciesPOI);
	this->AddService(CSTR("/api/daypoi"), Net::WebUtil::RequestMethod::HTTP_GET, SvcDayPOI);
}

SSWR::OrganWeb::OrganWebPOIController::~OrganWebPOIController()
{
}

