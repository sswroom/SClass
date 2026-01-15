#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Map/GPSTrack.h"
#include "Map/MapDrawLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Media/AudioSource.h"
#include "Media/MediaFile.h"
#include "Media/PhotoInfo.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "SSWR/OrganWeb/OrganWebEnv.h"
#include "SSWR/OrganWeb/OrganWebPOIController.h"
#include "Text/JSONUtil.h"
#include "Text/JSText.h"
#include "Text/UTF8Reader.h"

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcLang(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	NN<IO::ConfigFile> lang;
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	if (me->env->LangGet(req).SetTo(lang))
	{
		Data::ArrayListStringNN keys;
		NN<Text::String> key;
		lang->GetKeys(CSTR(""), keys);
		Data::ArrayIterator<NN<Text::String>> it = keys.Iterator();
		while (it.HasNext())
		{
			key = it.Next();
			json.ObjectAddStrOpt(key->ToCString(), lang->GetValue(key));
		}
	}
	return me->ResponseJSON(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcLoginInfo(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	NN<WebUserInfo> user;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	json.ObjectAddBool(CSTR("isMobile"), env.isMobile);
	json.ObjectAddInt32(CSTR("scnWidth"), (Int32)env.scnWidth);
	json.ObjectAddUInt64(CSTR("previewSize"), GetPreviewSize());
	json.ObjectAddInt32(CSTR("pickObjType"), env.pickObjType);
	json.ObjectAddArrayInt32(CSTR("pickObjs"), env.pickObjs);
	if (env.user.SetTo(user))
	{
		json.ObjectBeginObject(CSTR("user"));
		AppendUser(json, user);
		json.ObjectEnd();
		if (user->userType == UserType::Admin)
		{
			Sync::RWMutexUsage mutUsage;
			NN<BookInfo> book;
			json.ObjectAddInt32(CSTR("selectedBook"), me->env->BookGetSelected(mutUsage).SetTo(book)?book->id:0);
		}
	}
	return me->ResponseJSON(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcLogin(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	Text::CStringNN msg;
	if (env.user.NotNull())
	{
		msg = CSTR("ok");
	}
	else
	{
		UTF8Char sbuff[128];
		UnsafeArray<UTF8Char> sptr;
		req->ParseHTTPForm();
		NN<Text::String> userName;
		NN<Text::String> pwd;
		if (!req->GetHTTPFormStr(CSTR("userName")).SetTo(userName) || userName->leng == 0)
		{
			msg = CSTR("Username is missing");
		}
		else if (!req->GetHTTPFormStr(CSTR("password")).SetTo(pwd) || pwd->leng == 0)
		{
			msg = CSTR("Password is missing");
		}
		else
		{
			Sync::RWMutexUsage mutUsage;
			NN<WebUserInfo> user;
			sptr = me->env->PasswordEnc(sbuff, pwd->ToCString());
			env.user = me->env->UserGetByName(mutUsage, userName).OrNull();
			if (env.user.SetTo(user) && user->pwd.SetTo(pwd) && pwd->Equals(sbuff, (UOSInt)(sptr - sbuff)))
			{
				mutUsage.EndUse();
				NN<Net::WebServer::WebSession> sess = me->sessMgr->CreateSession(req, resp);
				Data::DateTime *t;
				Data::ArrayListInt32 *pickObjs;
				NEW_CLASS(t, Data::DateTime());
				NEW_CLASS(pickObjs, Data::ArrayListInt32());
				sess->SetValuePtr(CSTR("LastUseTime"), t);
				sess->SetValuePtr(CSTR("User"), user.Ptr());
				sess->SetValuePtr(CSTR("PickObjs"), pickObjs);
				sess->SetValueInt32(CSTR("PickObjType"), 0);
				sess->EndUse();
				msg = CSTR("ok");
			}
			else
			{
				msg = CSTR("Invalid username or password");
			}
		}
	}
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	json.ObjectAddStr(CSTR("status"), msg);
	return me->ResponseJSON(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcLogout(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	me->sessMgr->DeleteSession(req, resp);
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	json.ObjectAddStr(CSTR("status"), CSTR("ok"));
	return me->ResponseJSON(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcCateList(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	Sync::RWMutexUsage mutUsage;
	NN<WebUserInfo> user;
	Bool notAdmin = (!env.user.SetTo(user) || user->userType != UserType::Admin);
	NN<Data::ReadingListNN<CategoryInfo>> cateList = me->env->CateGetList(mutUsage);
	NN<CategoryInfo> cate;
	Text::JSONBuilder json(Text::JSONBuilder::OT_ARRAY);
	UOSInt i = 0;
	UOSInt j = cateList->GetCount();
	while (i < j)
	{
		cate = cateList->GetItemNoCheck(i);
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

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcCate(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	Sync::RWMutexUsage mutUsage;
	NN<CategoryInfo> cate;
	NN<GroupInfo> group;
	NN<Text::String> cateName;
	NN<WebUserInfo> user;
	if (req->GetQueryValue(CSTR("cateName")).SetTo(cateName) && me->env->CateGetByName(mutUsage, cateName).SetTo(cate))
	{
		Bool notAdmin = (!env.user.SetTo(user) || user->userType != UserType::Admin);
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
			group = cate->groups.GetItemNoCheck(i);
			me->env->CalcGroupCount(mutUsage, group);
			if (!notAdmin || (group->totalCount > 0 && (group->flags & 1) == 0))
			{
				json.ArrayBeginObject();
				me->AddGroup(json, group);
				json.ObjectEnd();
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

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcYearList(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	Text::JSONBuilder json(Text::JSONBuilder::OT_ARRAY);
	NN<WebUserInfo> user;
	if (env.user.SetTo(user))
	{
		Data::DateTime dt;
		OSInt endIndex = (OSInt)user->userFileIndex.GetCount();
		OSInt startIndex;
		Int64 currTime = user->userFileIndex.GetItem((UOSInt)endIndex - 1);
		Int64 thisTicks;
		if (endIndex > 0)
		{
			dt.ToUTCTime();
			while (true)
			{
				dt.SetTicks(currTime);
				dt.SetValue(dt.GetYear(), 1, 1, 0, 0, 0, 0);
				thisTicks = dt.ToTicks();
				startIndex = user->userFileIndex.SortedIndexOf(thisTicks);
				if (startIndex < 0)
				{
					startIndex = ~startIndex;
				}
				else
				{
					while (startIndex > 0 && user->userFileIndex.GetItem((UOSInt)startIndex - 1) == thisTicks)
					{
						startIndex--;
					}
				}
				json.ArrayAddInt32((Int32)dt.GetYear());
				if (startIndex <= 0)
					break;
				endIndex = startIndex;
				currTime = user->userFileIndex.GetItem((UOSInt)endIndex - 1);
			}
		}
	}
	return me->ResponseJSON(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcDayList(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	NN<WebUserInfo> user;
	me->ParseRequestEnv(req, resp, env, false);
	req->ParseHTTPForm();
	Int32 year;
	Int32 tzQhr = 0;
	req->GetHTTPFormInt32(CSTR("tzQhr"), tzQhr);
	if (req->GetHTTPFormInt32(CSTR("year"), year) && env.user.SetTo(user))
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

		startIndex = user->userFileIndex.SortedIndexOf(startTime);
		if (startIndex < 0)
		{
			startIndex = ~startIndex;
		}
		else
		{
			while (startIndex > 0 && user->userFileIndex.GetItem((UOSInt)startIndex - 1) == startTime)
			{
				startIndex--;
			}
		}
		endIndex = user->userFileIndex.SortedIndexOf(endTime);
		if (endIndex < 0)
		{
			endIndex = ~endIndex;
		}
		else
		{
			while (endIndex > 0 && user->userFileIndex.GetItem((UOSInt)endIndex - 1) == endTime)
			{
				endIndex--;
			}
		}
		UInt8 month = 0;
		UInt8 day = 0;
		OSInt dayStartIndex = 0;
		NN<UserFileInfo> userFile;
		NN<SpeciesInfo> sp;
		Text::StringBuilderUTF8 sb;
		Data::ArrayListStringNN locList;
		NN<Text::String> unkLoc = Text::String::New(UTF8STRC("?"));
		OSInt si;

		while (startIndex < endIndex)
		{
			dt.SetTicks(user->userFileIndex.GetItem((UOSInt)startIndex));
			if (dt.GetMonth() != month || dt.GetDay() != day)
			{
				if (month != 0 && day != 0)
				{
					userFile = user->userFileObj.GetItemNoCheck((UOSInt)(dayStartIndex + startIndex) >> 1);
					if (me->env->SpeciesGet(mutUsage, userFile->speciesId).SetTo(sp))
					{
						json.ArrayBeginObject();
						json.ObjectAddInt64(CSTR("day"), Data::Date(year, month, day).GetTotalDays());
						json.ObjectAddInt32(CSTR("photoSpId"), userFile->speciesId);
						json.ObjectAddInt32(CSTR("photoCateId"), sp->cateId);
						json.ObjectAddInt32(CSTR("photoFileId"), userFile->id);
						json.ObjectAddInt32(CSTR("count"), (Int32)(startIndex - dayStartIndex));
						json.ObjectBeginArray(CSTR("locs"));
						Data::ArrayIterator<NN<Text::String>> it = locList.Iterator();
						while (it.HasNext())
						{
							json.ArrayAddStr(it.Next());
						}
						json.ArrayEnd();
						json.ObjectEnd();
					}
				}

				month = dt.GetMonth();
				day = dt.GetDay();
				dayStartIndex = startIndex;
				locList.Clear();
			}

			NN<Text::String> locName = unkLoc;
			userFile = user->userFileObj.GetItemNoCheck((UOSInt)startIndex);
			userFile->location.SetTo(locName);
			si = locList.SortedIndexOf(locName);
			if (si < 0)
			{
				locList.SortedInsert(locName);
			}

			startIndex++;
		}
		if (month != 0 && day != 0)
		{
			userFile = user->userFileObj.GetItemNoCheck((UOSInt)(dayStartIndex + startIndex) >> 1);
			if (me->env->SpeciesGet(mutUsage, userFile->speciesId).SetTo(sp))
			{
				json.ArrayBeginObject();
				json.ObjectAddInt64(CSTR("day"), Data::Date(year, month, day).GetTotalDays());
				json.ObjectAddInt32(CSTR("photoSpId"), userFile->speciesId);
				json.ObjectAddInt32(CSTR("photoCateId"), sp->cateId);
				json.ObjectAddInt32(CSTR("photoFileId"), userFile->id);
				json.ObjectAddInt32(CSTR("count"), (Int32)(startIndex - dayStartIndex));
				json.ObjectBeginArray(CSTR("locs"));
				Data::ArrayIterator<NN<Text::String>> it = locList.Iterator();
				while (it.HasNext())
				{
					json.ArrayAddStr(it.Next());
				}
				json.ArrayEnd();
				json.ObjectEnd();
			}
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

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcDayDetail(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	req->ParseHTTPForm();
	NN<WebUserInfo> user;
	Int32 d;
	Int32 tzQhr = 0;
	req->GetHTTPFormInt32(CSTR("tzQhr"), tzQhr);
	if (env.user.SetTo(user) && req->GetHTTPFormInt32(CSTR("d"), d))
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

		startIndex = user->userFileIndex.SortedIndexOf(startTime);
		if (startIndex < 0)
		{
			startIndex = ~startIndex;
		}
		else
		{
			while (startIndex > 0 && user->userFileIndex.GetItem((UOSInt)startIndex - 1) == startTime)
			{
				startIndex--;
			}
		}
		endIndex = user->userFileIndex.SortedIndexOf(endTime);
		if (endIndex < 0)
		{
			endIndex = ~endIndex;
		}
		else
		{
			while (endIndex > 0 && user->userFileIndex.GetItem((UOSInt)endIndex - 1) == endTime)
			{
				endIndex--;
			}
		}
		NN<UserFileInfo> userFile;
		NN<SpeciesInfo> sp;
		Data::ArrayListInt32 spList;
		OSInt si;
		json.ObjectBeginArray(CSTR("userFiles"));
			
		while (startIndex < endIndex)
		{
			userFile = user->userFileObj.GetItemNoCheck((UOSInt)startIndex);
			if (me->env->SpeciesGet(mutUsage, userFile->speciesId).SetTo(sp))
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
				json.ObjectAddStrOpt(CSTR("descript"), userFile->descript);
				json.ObjectAddStrOpt(CSTR("location"), userFile->location);
				json.ObjectAddStrOpt(CSTR("camera"), userFile->camera);
				json.ObjectAddInt32(CSTR("locType"), (Int32)userFile->locType);
				json.ObjectAddInt32(CSTR("cateId"), sp->cateId);
				json.ObjectEnd();
			}
			startIndex++;
		}
		json.ArrayEnd();
		json.ObjectBeginArray(CSTR("dataFiles"));
		me->AppendDataFiles(json, user->gpsDataFiles, startTime, endTime, false);
		me->AppendDataFiles(json, user->tempDataFiles, startTime, endTime, false);
		json.ArrayEnd();
		if (user->userType == UserType::Admin)
		{
			json.ObjectBeginArray(CSTR("spList"));
			UOSInt i = 0;
			UOSInt j = spList.GetCount();
			while (i < j)
			{
				if (me->env->SpeciesGet(mutUsage, spList.GetItem(i)).SetTo(sp))
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

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcBookList(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	Sync::RWMutexUsage mutUsage;
	Data::ArrayListNN<BookInfo> bookList;
	NN<BookInfo> book;
	Text::JSONBuilder json(Text::JSONBuilder::OT_ARRAY);
	me->env->BookGetList(mutUsage, bookList);

	UOSInt i = 0;
	UOSInt j = bookList.GetCount();
	while (i < j)
	{
		book = bookList.GetItemNoCheck(i);
		json.ArrayBeginObject();
		json.ObjectAddInt32(CSTR("id"), book->id);
		json.ObjectAddUInt64(CSTR("speciesCount"), book->species.GetCount());
		json.ObjectAddInt32(CSTR("userfileId"), book->userfileId);
		json.ObjectAddInt64(CSTR("publishDateTicks"), book->publishDate);
		json.ObjectAddStr(CSTR("author"), book->author);
		json.ObjectAddStr(CSTR("title"), book->title);
		json.ObjectAddStr(CSTR("press"), book->press);
		json.ObjectAddStrOpt(CSTR("url"), book->url);
		json.ObjectEnd();

		i++;
	}
	return me->ResponseJSON(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcBookSelect(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	Text::CStringNN json;
	NN<WebUserInfo> user;
	if (env.user.SetTo(user) && user->userType == UserType::Admin)
	{
		req->ParseHTTPForm();
		Sync::RWMutexUsage mutUsage;
		Int32 id;
		if (req->GetHTTPFormInt32(CSTR("id"), id))
		{
			NN<BookInfo> book;
			if (me->env->BookGet(mutUsage, id).SetTo(book))
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

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcBookUnselect(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	NN<WebUserInfo> user;
	me->ParseRequestEnv(req, resp, env, false);
	if (env.user.SetTo(user) && user->userType == UserType::Admin)
	{
		me->env->BookSelect(nullptr);
	}
	return me->ResponseJSON(req, resp, 0, CSTR("{}"));
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcBookAdd(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	NN<WebUserInfo> user;
	me->ParseRequestEnv(req, resp, env, false);
	Text::CStringNN msg;
	if (!env.user.SetTo(user) || user->userType != UserType::Admin)
	{
		msg = CSTR("Access denied");
	}
	else
	{
		NN<Text::String> title;
		NN<Text::String> author;
		NN<Text::String> press;
		NN<Text::String> pubDate;
		NN<Text::String> url;
		req->ParseHTTPForm();
		Data::Timestamp ts;
		if (!req->GetHTTPFormStr(CSTR("title")).SetTo(title) || title->leng == 0)
		{
			msg = CSTR("Book Name is empty");
		}
		else if (!req->GetHTTPFormStr(CSTR("author")).SetTo(author) || author->leng == 0)
		{
			msg = CSTR("Author is empty");
		}
		else if (!req->GetHTTPFormStr(CSTR("press")).SetTo(press) || press->leng == 0)
		{
			msg = CSTR("Press is empty");
		}
		else if (!req->GetHTTPFormStr(CSTR("pubDate")).SetTo(pubDate) || (ts = Data::Timestamp(pubDate->ToCString(), 0)).IsNull())
		{
			msg = CSTR("Publish Date is not valid");
		}
		else if (!req->GetHTTPFormStr(CSTR("url")).SetTo(url) || (url->leng > 0 && !url->StartsWith(UTF8STRC("http://")) && !url->StartsWith(UTF8STRC("https://"))))
		{
			msg = CSTR("URL is not valid");
		}
		else
		{
			Sync::RWMutexUsage mutUsage;
			if (me->env->BookAdd(mutUsage, title, author, press, ts, url).NotNull())
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

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcBookDetail(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	Sync::RWMutexUsage mutUsage;
	Int32 bookId;
	NN<BookInfo> book;
	NN<WebUserInfo> user;
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	if (req->GetQueryValueI32(CSTR("id"), bookId) && me->env->BookGet(mutUsage, bookId).SetTo(book))
	{
		json.ObjectAddInt32(CSTR("id"), book->id);
		json.ObjectAddUInt64(CSTR("speciesCount"), book->species.GetCount());
		json.ObjectAddInt32(CSTR("userfileId"), book->userfileId);
		json.ObjectAddInt64(CSTR("publishDateTicks"), book->publishDate);
		json.ObjectAddStr(CSTR("author"), book->author);
		json.ObjectAddStr(CSTR("title"), book->title);
		json.ObjectAddStr(CSTR("press"), book->press);
		json.ObjectAddStrOpt(CSTR("url"), book->url);
		if (book->userfileId != 0)
		{
			NN<UserFileInfo> userFile;
			if (me->env->UserfileGet(mutUsage, book->userfileId).SetTo(userFile))
			{
				NN<SpeciesInfo> sp;
				if (me->env->SpeciesGet(mutUsage, userFile->speciesId).SetTo(sp))
				{
					json.ObjectBeginObject(CSTR("userfile"));
					json.ObjectAddInt32(CSTR("id"), userFile->id);
					json.ObjectAddInt32(CSTR("speciesId"), userFile->speciesId);
					json.ObjectAddInt32(CSTR("cateId"), sp->cateId);
					json.ObjectEnd();
				}
			}
		}
		if (env.user.SetTo(user) && user->userType == UserType::Admin)
		{
			json.ObjectAddBool(CSTR("hasFile"), me->env->BookFileExist(book));
		}

		json.ObjectBeginArray(CSTR("species"));
		UOSInt i = 0;
		UOSInt j = book->species.GetCount();
		while (i < j)
		{
			NN<BookSpInfo> bookSp = book->species.GetItemNoCheck(i);
			NN<SpeciesInfo> species;
			if (me->env->SpeciesGet(mutUsage, bookSp->speciesId).SetTo(species))
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

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcSpecies(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	Int32 id;
	Int32 cateId;
	if (req->GetQueryValueI32(CSTR("id"), id) &&
		req->GetQueryValueI32(CSTR("cateId"), cateId))
	{
		UOSInt i;
		UOSInt j;
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		UnsafeArray<UTF8Char> sptr2;
		Text::StringBuilderUTF8 sb;
		NN<SpeciesInfo> species;
		NN<GroupInfo> group;
		NN<CategoryInfo> cate;
		NN<BookInfo> book;
		NN<UserFileInfo> userFile;
		NN<WebFileInfo> wfile;
		NN<WebUserInfo> user;

		Sync::RWMutexUsage mutUsage;
		if (!me->env->SpeciesGet(mutUsage, id).SetTo(species))
		{
			mutUsage.EndUse();
			return resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		}

		Bool notAdmin = (!env.user.SetTo(user) || user->userType != UserType::Admin);
		if (!me->env->GroupGet(mutUsage, species->groupId).SetTo(group) || group->cateId != cateId || (me->env->GroupIsAdmin(group) && notAdmin))
		{
			mutUsage.EndUse();
			return resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		}
		if (!me->env->CateGet(mutUsage, group->cateId).SetTo(cate) || ((cate->flags & 1) && notAdmin))
		{
			mutUsage.EndUse();
			return resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		}

		Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
		json.ObjectBeginObject(CSTR("cate"));
		json.ObjectAddInt32(CSTR("cateId"), cate->cateId);
		json.ObjectAddStr(CSTR("chiName"), cate->chiName);
		json.ObjectAddStr(CSTR("dirName"), cate->dirName);
		json.ObjectEnd();

		json.ObjectBeginObject(CSTR("species"));
		me->AppendSpecies(json, species, mutUsage);
		json.ObjectBeginArray(CSTR("files"));
		i = 0;
		j = species->files.GetCount();
		while (i < j)
		{
			userFile = species->files.GetItemNoCheck(i);
			json.ArrayBeginObject();
			json.ObjectAddInt32(CSTR("id"), userFile->id);
			json.ObjectAddInt64(CSTR("captureTimeTicks"), userFile->captureTimeTicks);
			json.ObjectAddInt64(CSTR("fileTimeTicks"), userFile->fileTimeTicks);
			if (env.user.SetTo(user) && (user->userType == UserType::Admin || userFile->webuserId == user->id))
			{
				json.ObjectAddStrOpt(CSTR("descript"), userFile->descript);
				json.ObjectAddStrOpt(CSTR("location"), userFile->location);
				json.ObjectAddStrOpt(CSTR("oriFileName"), userFile->oriFileName);
				json.ObjectAddFloat64(CSTR("lat"), userFile->lat);
				json.ObjectAddFloat64(CSTR("lon"), userFile->lon);
			}
			json.ObjectEnd();
			i++;
		}
		json.ArrayEnd();

		json.ObjectBeginArray(CSTR("wfiles"));
		i = 0;
		j = species->wfiles.GetCount();
		while (i < j)
		{
			wfile = species->wfiles.GetItemNoCheck(i);
			json.ArrayBeginObject();
			json.ObjectAddInt32(CSTR("id"), wfile->id);
			if (env.user.SetTo(user) && user->userType == UserType::Admin)
			{
				json.ObjectAddStr(CSTR("location"), wfile->location);
			}
			json.ObjectAddStr(CSTR("srcUrl"), wfile->srcUrl);
			json.ObjectEnd();

			i++;
		}
		json.ArrayEnd();

		json.ObjectBeginArray(CSTR("webfiles"));
		sptr = cate->srcDir->ConcatTo(sbuff);
		if (IO::Path::PATH_SEPERATOR != '\\')
		{
			Text::StrReplace(sbuff, '\\', IO::Path::PATH_SEPERATOR);
		}
		sptr = species->dirName->ConcatTo(sptr);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr2 = Text::StrConcatC(sptr, UTF8STRC("web.txt"));
		if (IO::Path::GetPathType(CSTRP(sbuff, sptr2)) == IO::Path::PathType::File)
		{
			Text::PString sarr[4];
			IO::FileStream fs(CSTRP(sbuff, sptr2), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
			Text::UTF8Reader reader(fs);
			sb.ClearStr();
			while (reader.ReadLine(sb, 4096))
			{
				if (Text::StrSplitP(sarr, 4, sb, '\t') == 3)
				{
					json.ArrayBeginObject();
					json.ObjectAddStr(CSTR("refUrl"), sarr[2].ToCString());
					sptr2 = Text::StrConcatC(sptr, UTF8STRC("web"));
					*sptr2++ = IO::Path::PATH_SEPERATOR;
					sptr2 = Text::StrConcatC(sptr2, sarr[0].v, sarr[0].leng);
					i = Text::StrLastIndexOfCharC(sptr, (UOSInt)(sptr2 - sptr), '.');
					if (i != INVALID_INDEX)
					{
						sptr[i] = 0;
						sptr2 = &sptr[i];
					}
					json.ObjectAddStr(CSTR("fileName"), CSTRP(sptr, sptr2));
					json.ObjectEnd();
				}
			}
		}
		json.ArrayEnd();

		json.ObjectEnd();

		json.ObjectAddBool(CSTR("isPublic"), me->env->GroupIsPublic(mutUsage, group->id));
		json.ObjectBeginArray(CSTR("groups"));
		me->AppendLocator(json, mutUsage, group, cate);
		json.ArrayEnd();
		if (env.user.SetTo(user) && user->userType == UserType::Admin && me->env->BookGetSelected(mutUsage).SetTo(book))
		{
			json.ObjectBeginObject(CSTR("selectedBook"));
			json.ObjectAddInt32(CSTR("id"), book->id);
			json.ObjectAddStr(CSTR("title"), book->title);
			json.ObjectEnd();
		}
		mutUsage.EndUse();
		return me->ResponseJSON(req, resp, 0, json.Build());
	}
	else
	{
		return resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
	}
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcPhotoDetail(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	req->ParseHTTPForm();
	Int32 id;
	Int32 cateId;
	Int32 fileId;
	if (req->GetHTTPFormInt32(CSTR("id"), id) &&
		req->GetHTTPFormInt32(CSTR("cateId"), cateId))
	{
		NN<SpeciesInfo> species;
		NN<GroupInfo> group;
		NN<CategoryInfo> cate;
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		UOSInt i;
		UOSInt j;
		Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
		Sync::RWMutexUsage mutUsage;
		if (!me->env->SpeciesGet(mutUsage, id).SetTo(species) || species->cateId != cateId)
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		if (!me->env->GroupGet(mutUsage, species->groupId).SetTo(group))
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		if (!me->env->CateGet(mutUsage, group->cateId).SetTo(cate))
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		if (req->GetHTTPFormInt32(CSTR("fileId"), fileId))
		{
			Bool found = false;
			NN<UserFileInfo> userFile;
			i = 0;
			j = species->files.GetCount();
			while (i < j)
			{
				userFile = species->files.GetItemNoCheck(i);
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
					NN<Media::MediaFile> mediaFile;
					{
						IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
						fileSize = fd.GetDataSize();
						if (Optional<Media::MediaFile>::ConvertFrom(me->env->ParseFileType(fd, IO::ParserType::MediaFile)).SetTo(mediaFile))
						{
							json.ObjectAddUInt64(CSTR("fileSize"), fileSize);
							NN<Media::MediaSource> msrc;
							Data::Duration stmTime;
							if (mediaFile->GetStream(0, 0).SetTo(msrc))
							{
								stmTime = msrc->GetStreamTime();
								json.ObjectAddInt64(CSTR("stmTime"), stmTime.GetTotalMS());

								if (msrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
								{
									NN<Media::AudioSource> asrc = NN<Media::AudioSource>::ConvertFrom(msrc);
									Media::AudioFormat format;
									asrc->GetFormat(format);
									json.ObjectAddUInt64(CSTR("frequency"), format.frequency);
									json.ObjectAddUInt64(CSTR("bitPerSample"), format.bitpersample);
									json.ObjectAddUInt64(CSTR("nChannels"), format.nChannels);
								}
							}
							mediaFile.Delete();
						}
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
			NN<WebFileInfo> wfile;
			if (species->wfiles.Get(fileId).SetTo(wfile))
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

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcPhotoUpload(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	NN<WebUserInfo> user;
	me->ParseRequestEnv(req, resp, env, false);

	if (!env.user.SetTo(user))
	{
		resp->ResponseError(req, Net::WebStatus::SC_FORBIDDEN);
		return true;
	}
	Sync::RWMutexUsage mutUsage;
	UOSInt i = 0;
	UOSInt fileSize;
	UTF8Char fileName[512];
	UnsafeArray<UTF8Char> fileNameEnd;
	UnsafeArray<const UInt8> fileCont;
	NN<Text::String> location;
	Text::CString msg = nullptr;
	Bool succ = true;
	req->ParseHTTPForm();

	while (true)
	{
		if (!req->GetHTTPFormFile(CSTR("file"), i, fileName, sizeof(fileName), fileNameEnd, fileSize).SetTo(fileCont))
		{
			break;
		}
		location = Text::String::OrEmpty(req->GetHTTPFormStr(CSTR("location")));
		IO::StmData::MemoryDataRef md(fileCont, fileSize);
		md.SetName(CSTRP(fileName, fileNameEnd));
		NN<Map::MapDrawLayer> layer;
		if (Optional<Map::MapDrawLayer>::ConvertFrom(me->env->ParseFileType(md, IO::ParserType::MapLayer)).SetTo(layer))
		{
			if (layer->GetObjectClass() == Map::MapDrawLayer::OC_GPS_TRACK)
			{
				NN<Map::GPSTrack> gps = NN<Map::GPSTrack>::ConvertFrom(layer);
				succ = me->env->GPSFileAdd(mutUsage, user->id, CSTRP(fileName, fileNameEnd), gps->GetTrackStartTime(0), gps->GetTrackEndTime(0), fileCont, fileSize, gps, msg);
				layer.Delete();
			}
			else
			{
				layer.Delete();
				succ = false;
				msg = CSTR("Not GPS Track");
			}
		}
		else if (!me->env->UserfileAdd(mutUsage, user->id, user->unorganSpId, CSTRP(fileName, fileNameEnd), fileCont, fileSize, true, (location->leng == 0)?nullptr:Optional<Text::String>(location)))
		{
			msg = CSTR("Userfile problem");
			succ = false;
		}

		i++;
	}
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	if (succ)
	{
		json.ObjectAddStr(CSTR("status"), CSTR("ok"));
	}
	else
	{
		json.ObjectAddStr(CSTR("status"), CSTR("fail"));
		Text::CStringNN nnmsg;
		if (msg.SetTo(nnmsg))
		{
			json.ObjectAddStr(CSTR("msg"), nnmsg);
		}
		json.ObjectAddUInt64(CSTR("fileSize"), fileSize);
	}
	return me->ResponseJSON(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcPhotoName(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	NN<WebUserInfo> user;
	me->ParseRequestEnv(req, resp, env, false);

	if (env.user.SetTo(user))
	{
		Int32 fileId;
		NN<Text::String> desc;
		req->ParseHTTPForm();
		if (req->GetHTTPFormStr(CSTR("desc")).SetTo(desc) && req->GetHTTPFormInt32(CSTR("fileId"), fileId))
		{
			Sync::RWMutexUsage mutUsage;
			NN<UserFileInfo> file;
			if (me->env->UserfileGet(mutUsage, fileId).SetTo(file) && (user->userType == UserType::Admin || file->webuserId == user->id))
			{
				if (me->env->UserfileUpdateDesc(mutUsage, fileId, desc->ToCString()))
				{
					return me->ResponseJSON(req, resp, 0, CSTR("{\"status\": \"ok\"}"));
				}
			}
		}
	}
	return me->ResponseJSON(req, resp, 0, CSTR("{\"status\": \"failed\"}"));
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcPhotoPos(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	Int32 id;
	Double lat;
	Double lon;
	Int32 locType;
	Int64 captureTime;
	NN<WebUserInfo> user;
	req->ParseHTTPForm();
	if (env.user.SetTo(user) && req->GetHTTPFormInt32(CSTR("id"), id) && req->GetHTTPFormDouble(CSTR("lat"), lat) && req->GetHTTPFormDouble(CSTR("lon"), lon) && req->GetHTTPFormInt32(CSTR("locType"), locType) && req->GetHTTPFormInt64(CSTR("captureTime"), captureTime))
	{
		Sync::RWMutexUsage mutUsage;
		NN<UserFileInfo> file;
		if (me->env->UserfileGet(mutUsage, id).SetTo(file) && file->webuserId == user->id)
		{
			Int64 tdiff = file->fileTimeTicks - captureTime;
			if (tdiff >= -900000 && tdiff <= 900000)
			{
				if (me->env->UserfileUpdatePos(mutUsage, id, Data::Timestamp(captureTime, 0), lat, lon, (LocType)locType))
				{
					return me->ResponseJSON(req, resp, 0, CSTR("{\"status\": \"ok\"}"));
				}
			}
		}
	}
	return me->ResponseJSON(req, resp, 0, CSTR("{\"status\": \"failed\"}"));
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcUnfinPeak(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	Sync::RWMutexUsage mutUsage;
	Data::ArrayListNN<PeakInfo> peaks;
	Text::JSONBuilder json(Text::JSONBuilder::ObjectType::OT_ARRAY);
	NN<PeakInfo> peak;
	Math::Vector3 pt;
	me->env->PeakGetUnfin(mutUsage, peaks);
	if (peaks.GetCount() > 0)
	{
		Optional<Math::CoordinateSystem> csysHK = Math::CoordinateSystemManager::CreateProjCoordinateSystemDefName(Math::CoordinateSystemManager::PCST_HK80);
		Optional<Math::CoordinateSystem> csysMO = Math::CoordinateSystemManager::CreateProjCoordinateSystemDefName(Math::CoordinateSystemManager::PCST_MACAU_GRID);
		NN<Math::CoordinateSystem> csysWGS84 = Math::CoordinateSystemManager::CreateWGS84Csys();
		NN<Math::CoordinateSystem> csys;
		Data::ArrayIterator<NN<PeakInfo>> it = peaks.Iterator();
		while (it.HasNext())
		{
			peak = it.Next();
			json.ArrayBeginObject();
			json.ObjectAddInt32(CSTR("id"), peak->id);
			json.ObjectAddStr(CSTR("refId"), peak->refId);
			json.ObjectAddStr(CSTR("district"), peak->district);
			if (peak->csys == 1 && csysHK.SetTo(csys))
			{
				pt = Math::CoordinateSystem::Convert3D(csys, csysWGS84, Math::Vector3(peak->mapX, peak->mapY, peak->markedHeight));
			}
			else if (peak->csys == 2 && csysMO.SetTo(csys))
			{
				pt = Math::CoordinateSystem::Convert3D(csys, csysWGS84, Math::Vector3(peak->mapX, peak->mapY, peak->markedHeight));
			}
			else
			{
				pt = Math::Vector3(peak->mapX, peak->mapY, peak->markedHeight);
			}
			json.ObjectAddFloat64(CSTR("lon"), pt.GetLon());
			json.ObjectAddFloat64(CSTR("lat"), pt.GetLat());
			json.ObjectAddFloat64(CSTR("height"), pt.GetZ());
			json.ObjectAddInt32(CSTR("status"), peak->status);
			json.ObjectAddStrOpt(CSTR("name"), peak->name);
			json.ObjectAddStrOpt(CSTR("type"), peak->type);
			json.ObjectEnd();
		}
		csysHK.Delete();
		csysMO.Delete();
		csysWGS84.Delete();
	}
	me->env->PeakFreeAll(peaks);
	return me->ResponseJSON(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcUpdatePeak(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	Int32 id;
	Int32 status;
	NN<WebUserInfo> user;
	req->ParseHTTPForm();
	if (env.user.SetTo(user) && user->userType == UserType::Admin && req->GetHTTPFormInt32(CSTR("id"), id) && req->GetHTTPFormInt32(CSTR("status"), status))
	{
		Sync::RWMutexUsage mutUsage;
		if (me->env->PeakUpdateStatus(mutUsage, id, status))
		{
			return me->ResponseJSON(req, resp, 0, CSTR("{\"status\": \"ok\"}"));
		}
	}
	return me->ResponseJSON(req, resp, 0, CSTR("{\"status\": \"failed\"}"));
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcReload(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	NN<WebUserInfo> user;
	me->ParseRequestEnv(req, resp, env, false);

	if (me->env->HasReloadPwd() && env.user.SetTo(user) && user->userType == UserType::Admin)
	{
		NN<Text::String> pwd;
		req->ParseHTTPForm();
		if (req->GetHTTPFormStr(CSTR("pwd")).SetTo(pwd) && me->env->ReloadPwdMatches(pwd))
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

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcPublicPOI(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	Sync::RWMutexUsage mutUsage;
	NN<GroupInfo> poiGroup;
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	Data::ArrayListNN<GroupInfo> groups;
	Data::ArrayListNN<SpeciesInfo> speciesList;
	json.ObjectBeginArray(CSTR("poi"));
	if (me->env->GroupGet(mutUsage, 21593).SetTo(poiGroup))
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

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcGroupPOI(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	Sync::RWMutexUsage mutUsage;
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	Data::ArrayListNN<GroupInfo> groups;
	Data::ArrayListNN<SpeciesInfo> speciesList;
	NN<WebUserInfo> user;
	Int32 groupId;
	json.ObjectBeginArray(CSTR("poi"));
	if (req->GetQueryValueI32(CSTR("id"), groupId))
	{
		NN<GroupInfo> poiGroup;
		if (me->env->GroupGet(mutUsage, groupId).SetTo(poiGroup))
		{
			groups.Add(poiGroup);
			me->AddGroupPOI(mutUsage, json, poiGroup, env.user.SetTo(user)?user->id:0, groups, speciesList);
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

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcSpeciesPOI(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	Sync::RWMutexUsage mutUsage;
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	Data::ArrayListNN<SpeciesInfo> speciesList;
	NN<WebUserInfo> user;
	Int32 speciesId;
	json.ObjectBeginArray(CSTR("poi"));
	if (req->GetQueryValueI32(CSTR("id"), speciesId))
	{
		NN<SpeciesInfo> poiSpecies;
		if (me->env->SpeciesGet(mutUsage, speciesId).SetTo(poiSpecies))
		{
			speciesList.Add(poiSpecies);
			me->AddSpeciesPOI(mutUsage, json, poiSpecies, env.user.SetTo(user)?user->id:0, me->env->GroupIsPublic(mutUsage, poiSpecies->groupId));
		}
	}
	json.ArrayEnd();
	json.ObjectBeginArray(CSTR("group"));
	json.ArrayEnd();
	json.ObjectBeginArray(CSTR("species"));
	me->AddSpeciesList(json, speciesList, mutUsage);
	return me->ResponseJSON(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcDayPOI(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	Int32 dayId;
	NN<WebUserInfo> user;
	if (!env.user.SetTo(user))
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

		json.ObjectBeginArray(CSTR("poi"));
		startIndex = user->userFileIndex.SortedIndexOf(startTime);
		if (startIndex < 0)
		{
			startIndex = ~startIndex;
		}
		else
		{
			while (startIndex > 0 && user->userFileIndex.GetItem((UOSInt)startIndex - 1) == startTime)
			{
				startIndex--;
			}
		}
		endIndex = user->userFileIndex.SortedIndexOf(endTime);
		if (endIndex < 0)
		{
			endIndex = ~endIndex;
		}
		else
		{
			while (endIndex > 0 && user->userFileIndex.GetItem((UOSInt)endIndex - 1) == endTime)
			{
				endIndex--;
			}
		}
		NN<UserFileInfo> userFile;
		NN<SpeciesInfo> sp;

		while (startIndex < endIndex)
		{
			if (user->userFileObj.GetItem((UOSInt)startIndex).SetTo(userFile) && me->env->SpeciesGet(mutUsage, userFile->speciesId).SetTo(sp))
			{
				me->AddUserfilePOI(json, sp, userFile);
			}
			startIndex++;
		}
		json.ArrayEnd();
		json.ObjectBeginArray(CSTR("datafiles"));
		me->AppendDataFiles(json, user->gpsDataFiles, startTime, endTime, true);
		json.ArrayEnd();
	}
	json.ObjectBeginArray(CSTR("group"));
	json.ArrayEnd();
	json.ObjectBeginArray(CSTR("species"));
	return me->ResponseJSON(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::OrganWeb::OrganWebPOIController::SvcDatafilePOI(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPOIController> me = NN<SSWR::OrganWeb::OrganWebPOIController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	Int32 fileId;
	NN<WebUserInfo> user;
	if (!env.user.SetTo(user))
	{
		printf("SvcDatafilePOI: user == null\r\n");
	}
	else if (req->GetQueryValueI32(CSTR("id"), fileId))
	{
		Sync::RWMutexUsage mutUsage;
		NN<DataFileInfo> file;
		if (me->env->DataFileGet(mutUsage, fileId).SetTo(file) && file->webuserId == user->id)
		{
			Int64 startTime;
			Int64 endTime;
			OSInt startIndex;
			OSInt endIndex;
			startTime = file->startTime.ToTicks() - 300000;
			endTime = file->endTime.ToTicks() + 300000;

			json.ObjectBeginArray(CSTR("poi"));
			startIndex = user->userFileIndex.SortedIndexOf(startTime);
			if (startIndex < 0)
			{
				startIndex = ~startIndex;
			}
			else
			{
				while (startIndex > 0 && user->userFileIndex.GetItem((UOSInt)startIndex - 1) == startTime)
				{
					startIndex--;
				}
			}
			endIndex = user->userFileIndex.SortedIndexOf(endTime);
			if (endIndex < 0)
			{
				endIndex = ~endIndex;
			}
			else
			{
				while (endIndex > 0 && user->userFileIndex.GetItem((UOSInt)endIndex - 1) == endTime)
				{
					endIndex--;
				}
			}
			NN<UserFileInfo> userFile;
			NN<SpeciesInfo> sp;

			while (startIndex < endIndex)
			{
				if (user->userFileObj.GetItem((UOSInt)startIndex).SetTo(userFile) && me->env->SpeciesGet(mutUsage, userFile->speciesId).SetTo(sp))
				{
					me->AddUserfilePOI(json, sp, userFile);
				}
				startIndex++;
			}
			json.ArrayEnd();
			json.ObjectBeginArray(CSTR("datafiles"));
			me->AppendDataFiles(json, user->gpsDataFiles, startTime, endTime, true);
			json.ArrayEnd();
		}
	}
	json.ObjectBeginArray(CSTR("group"));
	json.ArrayEnd();
	json.ObjectBeginArray(CSTR("species"));
	return me->ResponseJSON(req, resp, 0, json.Build());
}

void SSWR::OrganWeb::OrganWebPOIController::AddGroupPOI(NN<Sync::RWMutexUsage> mutUsage, NN<Text::JSONBuilder> json, NN<GroupInfo> group, Int32 userId, NN<Data::ArrayListNN<GroupInfo>> groups, NN<Data::ArrayListNN<SpeciesInfo>> speciesList)
{
	NN<SpeciesInfo> species;
	NN<GroupInfo> subgroup;
	UOSInt i = 0;
	UOSInt j = group->groups.GetCount();
	while (i < j)
	{
		subgroup = group->groups.GetItemNoCheck(i);
		groups->Add(subgroup);
		AddGroupPOI(mutUsage, json, subgroup, userId, groups, speciesList);
		i++;
	}
	i = 0;
	j = group->species.GetCount();
	while (i < j)
	{
		species = group->species.GetItemNoCheck(i);
		speciesList->Add(species);
		AddSpeciesPOI(mutUsage, json, species, userId, this->env->GroupIsPublic(mutUsage, group->id));
		i++;
	}
}

void SSWR::OrganWeb::OrganWebPOIController::AddSpeciesPOI(NN<Sync::RWMutexUsage> mutUsage, NN<Text::JSONBuilder> json, NN<SpeciesInfo> species, Int32 userId, Bool publicGroup)
{
	NN<UserFileInfo> file;
	UOSInt k;
	UOSInt l;
	NN<WebUserInfo> user;
	Bool adminUser = false;
	if (userId != 0)
	{
		if (this->env->UserGet(mutUsage, userId).SetTo(user) && user->userType == UserType::Admin)
		{
			adminUser = true;
		}
	}
	k = 0;
	l = species->files.GetCount();
	while (k < l)
	{
		file = species->files.GetItemNoCheck(k);
		if ((file->lat != 0 || file->lon != 0) && (publicGroup || adminUser || file->webuserId == userId))
		{
			this->AddUserfilePOI(json, species, file);
		}
		k++;
	}
}

void SSWR::OrganWeb::OrganWebPOIController::AddUserfilePOI(NN<Text::JSONBuilder> json, NN<SpeciesInfo> species, NN<UserFileInfo> file)
{
	json->ArrayBeginObject();
	json->ObjectAddInt32(CSTR("id"), file->id);
	json->ObjectAddStrOpt(CSTR("descript"), file->descript);
	json->ObjectAddTSStr(CSTR("captureTime"), Data::Timestamp(file->captureTimeTicks, 32));
	json->ObjectAddTSStr(CSTR("fileTime"), Data::Timestamp(file->fileTimeTicks, 32));
	json->ObjectAddFloat64(CSTR("lat"), file->lat);
	json->ObjectAddFloat64(CSTR("lon"), file->lon);
	json->ObjectAddStr(CSTR("oriFileName"), file->oriFileName);
	json->ObjectAddInt32(CSTR("imgSize"), (Int32)GetPreviewSize());
	json->ObjectAddStr(CSTR("sciName"), species->sciName);
	json->ObjectAddInt32(CSTR("speciesId"), species->speciesId);
	json->ObjectAddInt32(CSTR("groupId"), species->groupId);
	json->ObjectAddInt32(CSTR("cateId"), species->cateId);
	json->ObjectAddStrOpt(CSTR("poiImg"), species->poiImg);
	json->ObjectAddStrOpt(CSTR("camera"), file->camera);
	json->ObjectAddInt32(CSTR("locType"), (Int32)file->locType);
	json->ObjectEnd();
}

void SSWR::OrganWeb::OrganWebPOIController::AddGroups(NN<Text::JSONBuilder> json, NN<Data::ArrayListNN<GroupInfo>> groups)
{
	NN<GroupInfo> group;
	Data::ArrayIterator<NN<GroupInfo>> it = groups->Iterator();
	while (it.HasNext())
	{
		group = it.Next();
		json->ArrayBeginObject();
		this->AddGroup(json, group);
		json->ObjectEnd();
	}
}

void SSWR::OrganWeb::OrganWebPOIController::AddGroup(NN<Text::JSONBuilder> json, NN<GroupInfo> group)
{
	NN<Text::String> s;
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
	NN<SpeciesInfo> photoSpObj;
	if (group->photoSpObj.SetTo(photoSpObj))
	{
		json->ObjectAddInt32(CSTR("photoSpId"), photoSpObj->speciesId);
		if (photoSpObj->photoId != 0)
		{
			json->ObjectAddInt32(CSTR("photoId"), photoSpObj->photoId);
		}
		else if (photoSpObj->photoWId != 0)
		{
			json->ObjectAddInt32(CSTR("photoWId"), photoSpObj->photoWId);
		}
		else if (photoSpObj->photo.SetTo(s) && s->leng > 0)
		{
			json->ObjectAddStr(CSTR("photo"), s);
		}
	}
}

void SSWR::OrganWeb::OrganWebPOIController::AddSpeciesList(NN<Text::JSONBuilder> json, NN<Data::ArrayListNN<SpeciesInfo>> speciesList, NN<Sync::RWMutexUsage> mutUsage)
{
	NN<SpeciesInfo> species;
	Data::ArrayIterator<NN<SpeciesInfo>> it = speciesList->Iterator();
	while (it.HasNext())
	{
		species = it.Next();
		json->ArrayBeginObject();
		AppendSpecies(json, species, mutUsage);
		json->ObjectEnd();
	}
}

void SSWR::OrganWeb::OrganWebPOIController::AppendUser(NN<Text::JSONBuilder> json, NN<WebUserInfo> user)
{
	json->ObjectAddInt32(CSTR("id"), user->id);
	json->ObjectAddInt32(CSTR("userType"), (Int32)user->userType);
	json->ObjectAddStr(CSTR("userName"), user->userName);
	json->ObjectAddStr(CSTR("watermark"), user->watermark);
}

void SSWR::OrganWeb::OrganWebPOIController::AppendSpecies(NN<Text::JSONBuilder> json, NN<SpeciesInfo> species, NN<Sync::RWMutexUsage> mutUsage)
{
	UOSInt i;
	UOSInt j;
	NN<Text::String> s;
	json->ObjectAddInt32(CSTR("id"), species->speciesId);
	json->ObjectAddInt32(CSTR("groupId"), species->groupId);
	json->ObjectAddStr(CSTR("sciName"), species->sciName);
	json->ObjectAddStr(CSTR("chiName"), species->chiName);
	json->ObjectAddStr(CSTR("engName"), species->engName);
	json->ObjectAddStr(CSTR("descript"), species->descript);
	json->ObjectAddStrOpt(CSTR("poiImg"), species->poiImg);
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
	else if (species->photo.SetTo(s) && s->leng > 0)
	{
		json->ObjectAddStr(CSTR("photo"), s);
	}
	json->ObjectBeginArray(CSTR("books"));
	i = 0;
	j = species->books.GetCount();
	while (i < j)
	{
		NN<BookSpInfo> bookSp = species->books.GetItemNoCheck(i);
		NN<BookInfo> book;
		json->ArrayBeginObject();
		json->ObjectAddStr(CSTR("dispName"), bookSp->dispName);
		if (this->env->BookGet(mutUsage, bookSp->bookId).SetTo(book))
		{
			json->ObjectAddInt32(CSTR("id"), book->id);
			json->ObjectAddStr(CSTR("bookTitle"), book->title);
			json->ObjectAddStr(CSTR("bookAuthor"), book->author);
			json->ObjectAddTSStr(CSTR("publishDate"), Data::Timestamp(book->publishDate, 32));
		}
		json->ObjectEnd();
		i++;
	}
	json->ArrayEnd();
}

void SSWR::OrganWeb::OrganWebPOIController::AppendDataFiles(NN<Text::JSONBuilder> json, NN<Data::FastMapNN<Data::Timestamp, DataFileInfo>> dataFiles, Int64 startTime, Int64 endTime, Bool includeCont)
{
	OSInt startIndex;
	OSInt endIndex;
	Text::StringBuilderUTF8 sb;
	startIndex = dataFiles->GetIndex(Data::Timestamp(startTime, 0));
	if (startIndex < 0)
		startIndex = ~startIndex;
	if (startIndex > 0 && dataFiles->GetItemNoCheck((UOSInt)startIndex - 1)->endTime.ToTicks() > startTime)
		startIndex--;
	endIndex = dataFiles->GetIndex(Data::Timestamp(endTime, 0));
	if (endIndex < 0)
		endIndex = ~endIndex;
	while (startIndex < endIndex)
	{
		NN<DataFileInfo> dataFile;
		if (dataFiles->GetItem((UOSInt)startIndex).SetTo(dataFile))
		{
			json->ArrayBeginObject();
			json->ObjectAddInt32(CSTR("id"), dataFile->id);
			json->ObjectAddInt32(CSTR("fileType"), (Int32)dataFile->fileType);
			json->ObjectAddInt64(CSTR("startTime"), dataFile->startTime.ToTicks());
			json->ObjectAddInt64(CSTR("endTime"), dataFile->endTime.ToTicks());
			json->ObjectAddStr(CSTR("oriFileName"), dataFile->oriFileName);
			json->ObjectAddInt32(CSTR("webuserId"), dataFile->webuserId);
			if (includeCont)
			{
				NN<Map::GPSTrack> trk;
				if (dataFile->fileType == DataFileType::GPSTrack && Optional<Map::GPSTrack>::ConvertFrom(this->env->DataFileParse(dataFile)).SetTo(trk))
				{
					json->ObjectBeginArray(CSTR("track"));
					Text::JSONUtil::ArrayGPSTrack(json, trk);
					json->ArrayEnd();
					trk.Delete();
				}
			}
			json->ObjectEnd();
		}
		startIndex++;
	}
}

void SSWR::OrganWeb::OrganWebPOIController::AppendLocator(NN<Text::JSONBuilder> json, NN<Sync::RWMutexUsage> mutUsage, NN<GroupInfo> group, NN<CategoryInfo> cate)
{
	NN<GroupTypeInfo> grpType;
	while (true)
	{
		json->ArrayBeginObject();
		json->ObjectAddInt32(CSTR("id"), group->id);
		json->ObjectAddInt32(CSTR("cateId"), group->cateId);
		json->ObjectAddInt32(CSTR("groupType"), group->groupType);
		json->ObjectAddStr(CSTR("engName"), group->engName);
		json->ObjectAddStr(CSTR("chiName"), group->chiName);
		if (cate->groupTypes.Get(group->groupType).SetTo(grpType))
		{
			json->ObjectAddStr(CSTR("grpTypeChi"), grpType->chiName);
			json->ObjectAddStr(CSTR("grpTypeEng"), grpType->engName);
		}
		json->ObjectEnd();
		if (!this->env->GroupGet(mutUsage, group->parentId).SetTo(group))
			return;
	}
}

Bool SSWR::OrganWeb::OrganWebPOIController::ResponseJSON(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, OSInt cacheAge, Text::CStringNN json)
{
	resp->EnableWriteBuffer();
	resp->AddDefHeaders(req);
	resp->AddCacheControl(cacheAge);
	resp->AddContentType(CSTR("application/json;charset=UTF-8"));
	return Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("application/json"), json);
}

SSWR::OrganWeb::OrganWebPOIController::OrganWebPOIController(NN<Net::WebServer::MemoryWebSessionManager> sessMgr, NN<OrganWebEnv> env, UInt32 scnSize) : OrganWebController(sessMgr, env, scnSize)
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
	this->AddService(CSTR("/api/photoname"), Net::WebUtil::RequestMethod::HTTP_POST, SvcPhotoName);
	this->AddService(CSTR("/api/photopos"), Net::WebUtil::RequestMethod::HTTP_POST, SvcPhotoPos);
	this->AddService(CSTR("/api/unfinpeak"), Net::WebUtil::RequestMethod::HTTP_GET, SvcUnfinPeak);
	this->AddService(CSTR("/api/updatepeak"), Net::WebUtil::RequestMethod::HTTP_POST, SvcUpdatePeak);
	this->AddService(CSTR("/api/reload"), Net::WebUtil::RequestMethod::HTTP_POST, SvcReload);
	this->AddService(CSTR("/api/publicpoi"), Net::WebUtil::RequestMethod::HTTP_GET, SvcPublicPOI);
	this->AddService(CSTR("/api/grouppoi"), Net::WebUtil::RequestMethod::HTTP_GET, SvcGroupPOI);
	this->AddService(CSTR("/api/speciespoi"), Net::WebUtil::RequestMethod::HTTP_GET, SvcSpeciesPOI);
	this->AddService(CSTR("/api/daypoi"), Net::WebUtil::RequestMethod::HTTP_GET, SvcDayPOI);
	this->AddService(CSTR("/api/datafilepoi"), Net::WebUtil::RequestMethod::HTTP_GET, SvcDatafilePOI);
}

SSWR::OrganWeb::OrganWebPOIController::~OrganWebPOIController()
{
}

