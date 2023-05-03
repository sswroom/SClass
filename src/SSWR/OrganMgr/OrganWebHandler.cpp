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
#include "Map/MapDrawLayer.h"
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
#include "SSWR/OrganMgr/OrganWebEnv.h"
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

#include <stdio.h>

#define SP_PER_PAGE_DESKTOP 100
#define SP_PER_PAGE_MOBILE 90
#define PREVIEW_SIZE 320

Net::WebServer::IWebSession *SSWR::OrganMgr::OrganWebHandler::ParseRequestEnv(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, RequestEnv *env, Bool keepSess)
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
		env->user = (WebUserInfo*)sess->GetValuePtr(UTF8STRC("User"));
		env->pickObjType = (PickObjType)sess->GetValueInt32(UTF8STRC("PickObjType"));
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
	RequestEnv env;
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
	RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 spId;
	Int32 cateId;
	Int32 fileId;
	if (req->GetQueryValueI32(CSTR("id"), &spId) &&
		req->GetQueryValueI32(CSTR("cateId"), &cateId) &&
		req->GetQueryValueI32(CSTR("fileId"), &fileId))
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		UserFileInfo *userFile;
		Sync::RWMutexUsage mutUsage;
		sptr = sbuff;
		userFile = me->env->UserfileGetCheck(&mutUsage, fileId, spId, cateId, env.user, &sptr);
		if (userFile)
		{
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
			mutUsage.EndUse();
			resp->Write(buff, buffSize);
			return true;
		}
		else
		{
			mutUsage.EndUse();
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
	RequestEnv env;
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
		GroupInfo *group;
		CategoryInfo *cate;
		IO::ConfigFile *lang = me->env->LangGet(req);
		Sync::RWMutexUsage mutUsage;
		group = me->env->GroupGet(&mutUsage, id);
		Bool notAdmin = (env.user == 0 || env.user->userType != 0);
		if (group == 0 || group->cateId != cateId)
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->env->CateGet(&mutUsage, group->cateId);
		if (cate == 0 || ((cate->flags & 1) && notAdmin))
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		if (me->env->GroupIsAdmin(group) && notAdmin)
		{
			mutUsage.EndUse();
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
							if (me->env->GroupMove(&mutUsage, itemId, id, cateId))
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
							if (me->env->SpeciesMove(&mutUsage, itemId, id, cateId))
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
						if (me->env->GroupMove(&mutUsage, itemId, id, cateId))
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
						if (me->env->SpeciesMove(&mutUsage, itemId, id, cateId))
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
				me->env->GroupSetPhotoGroup(&mutUsage, group->parentId, group->id);
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

		me->WriteLocator(&mutUsage, &writer, group, cate);
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

		if (env.user != 0 || me->env->GroupIsPublic(&mutUsage, group->id))
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
			GroupInfo *sgroup;
			Data::StringMap<GroupInfo*> groups;
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
				me->WriteGroupTable(&mutUsage, &writer, groups.GetValues(), env.scnWidth, !notAdmin);
				writer.WriteLineC(UTF8STRC("<hr/>"));
				found = true;
			}
		}
		if (group->species.GetCount())
		{
			SpeciesInfo *sp;
			Data::StringMap<SpeciesInfo*> species;
			i = group->species.GetCount();
			while (i-- > 0)
			{
				sp = group->species.GetItem(i);
				species.Put(sp->sciName, sp);
			}
			me->WriteSpeciesTable(&mutUsage, &writer, species.GetValues(), env.scnWidth, group->cateId, !notAdmin);
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
			me->WritePickObjs(&mutUsage, &writer, &env, sb.ToString(), false);
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
		mutUsage.EndUse();
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
	RequestEnv env;
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
		GroupInfo *group;
		CategoryInfo *cate;
		Text::StringBuilderUTF8 sb;
		Text::String *s;
		Text::String *txt;
		IO::ConfigFile *lang = me->env->LangGet(req);

		Sync::RWMutexUsage mutUsage;
		group = me->env->GroupGet(&mutUsage, id);
		if (group == 0)
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->env->CateGet(&mutUsage, cateId);
		if (cate == 0)
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		Text::StringBuilderUTF8 msg;
		Text::String *ename = 0;
		Text::String *cname = 0;
		Text::String *descr = 0;
		GroupFlags groupFlags = GF_NONE;
		Int32 groupTypeId = 0;
		GroupInfo *modGroup = 0;
		if (req->GetQueryValueI32(CSTR("groupId"), &groupId))
		{
			modGroup = me->env->GroupGet(&mutUsage, groupId);
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
						Int32 newGroupId = me->env->GroupAdd(&mutUsage, ename->ToCString(), cname->ToCString(), id, descr->ToCString(), groupTypeId, cateId, groupFlags);
						if (newGroupId)
						{
							mutUsage.EndUse();
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
						if (me->env->GroupModify(&mutUsage, modGroup->id, STR_CSTR(ename), STR_CSTR(cname), STR_CSTR(descr), groupTypeId, groupFlags))
						{
							mutUsage.EndUse();
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
					}
				}
				else if (task->Equals(UTF8STRC("delete")) && modGroup != 0 && modGroup->groups.GetCount() == 0 && modGroup->species.GetCount() == 0)
				{
					Int32 parentId = modGroup->parentId;
					Int32 cateId = modGroup->cateId;
					if (me->env->GroupDelete(&mutUsage, modGroup->id))
					{
						mutUsage.EndUse();
						sb.ClearStr();
						sb.AppendC(UTF8STRC("group.html?id="));
						sb.AppendI32(parentId);
						sb.AppendC(UTF8STRC("&cateId="));
						sb.AppendI32(cateId);

						resp->RedirectURL(req, sb.ToCString(), 0);
						return true;
					}
					else
					{
						msg.AppendC(UTF8STRC("Error in deleting group"));
					}
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
		GroupTypeInfo *groupType;
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
		if (groupId != 0)
		{
			GroupInfo *modGroup = me->env->GroupGet(&mutUsage, groupId);
			if (modGroup && modGroup->species.GetCount() == 0 && modGroup->groups.GetCount() == 0)
			{
				writer.WriteStrC(UTF8STRC("<input type=\"button\" value=\"Delete\" onclick=\"document.forms.newgroup.task.value='delete';document.forms.newgroup.submit();\"/>"));
			}
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
		mutUsage.EndUse();
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
	RequestEnv env;
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
		SpeciesInfo *species;
		GroupInfo *group;
		CategoryInfo *cate;
		BookSpInfo *bookSp;
		BookInfo *book;
		UserFileInfo *userFile;
		WebFileInfo *wfile;
		IO::ConfigFile *lang = me->env->LangGet(req);
		Data::DateTime dt;

		Sync::RWMutexUsage mutUsage;
		species = me->env->SpeciesGet(&mutUsage, id);
		if (species == 0)
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		Bool notAdmin = (env.user == 0 || env.user->userType != 0);
		group = me->env->GroupGet(&mutUsage, species->groupId);
		if (group == 0 || group->cateId != cateId || (me->env->GroupIsAdmin(group) && notAdmin))
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->env->CateGet(&mutUsage, group->cateId);
		if (cate == 0 || ((cate->flags & 1) && notAdmin))
		{
			mutUsage.EndUse();
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
							if (me->env->UserfileMove(&mutUsage, userfileId, id, cateId))
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
						if (me->env->UserfileMove(&mutUsage, userfileId, id, cateId))
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
			else if (action && action->Equals(UTF8STRC("placemerge")))
			{
				if (env.pickObjType == POT_SPECIES)
				{
					i = 0;
					j = env.pickObjs->GetCount();
					while (i < j)
					{
						Int32 speciesId = env.pickObjs->GetItem(i);
						sb.ClearStr();
						sb.AppendC(UTF8STRC("species"));
						sb.AppendI32(speciesId);
						s = req->GetHTTPFormStr(sb.ToCString());
						if (s && s->v[0] == '1')
						{
							if (me->env->SpeciesMerge(&mutUsage, speciesId, id, cateId))
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
			else if (action && action->Equals(UTF8STRC("setphoto")))
			{
				me->env->GroupSetPhotoSpecies(&mutUsage, species->groupId, species->speciesId);
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
				book = me->env->BookGet(&mutUsage, bookSp->bookId);
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
		me->WriteLocator(&mutUsage, &writer, group, cate);
		writer.WriteLineC(UTF8STRC("</td></tr></table>"));
		if (env.user != 0 || me->env->GroupIsPublic(&mutUsage, group->id))
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
			me->WritePickObjs(&mutUsage, &writer, &env, sb.ToString(), true);
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

		if (env.user != 0 && env.user->userType == 0)
		{
			writer.WriteLineC(UTF8STRC("<br/>"));

			writer.WriteLineC(UTF8STRC("<input type=\"button\" value=\"Set Group Photo\" onclick=\"document.forms.userfiles.action.value='setphoto';document.forms.userfiles.submit();\"/>"));
		}

		me->WriteFooter(&writer);
		mutUsage.EndUse();
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
	RequestEnv env;
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
		GroupInfo *group;
		Text::StringBuilderUTF8 sb;
		Text::String *s;
		IO::ConfigFile *lang = me->env->LangGet(req);

		Sync::RWMutexUsage mutUsage;
		group = me->env->GroupGet(&mutUsage, id);
		if (group == 0)
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		Text::StringBuilderUTF8 msg;
		Text::String *cname = 0;
		Text::String *sname = 0;
		Text::String *ename = 0;
		Text::String *descr = 0;
		Bool canDelete = false;
		const UTF8Char *bookIgn = 0;
		SpeciesInfo *species = 0;
		if (req->GetQueryValueI32(CSTR("spId"), &spId))
		{
			species = me->env->SpeciesGet(&mutUsage, spId);
			if (species)
			{
				cname = species->chiName;
				sname = species->sciName;
				ename = species->engName;
				descr = species->descript;
				canDelete = (species->files.GetCount() == 0 && species->books.GetCount() == 0 && species->wfiles.GetCount() == 0);
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
					if (me->env->SpeciesGetByName(&mutUsage, sname) != 0)
					{
						msg.AppendC(UTF8STRC("Species already exist"));
					}
					else if ((bookIgn == 0 || bookIgn[0] != '1') && me->env->SpeciesBookIsExist(&mutUsage, sname->ToCString(), &sb))
					{
						msg.AppendC(UTF8STRC("Species already exist in book: "));
						msg.AppendC(sb.ToString(), sb.GetLength());
						msg.AppendC(UTF8STRC(", continue?"));
						bookIgn = (const UTF8Char*)"1";
					}
					else
					{
						sb.ClearStr();
						sb.Append(sname);
						sb.ToLower();
						sb.ReplaceStr(UTF8STRC(" "), UTF8STRC("_"));
						sb.ReplaceStr(UTF8STRC("."), UTF8STRC(""));
						Int32 spId = me->env->SpeciesAdd(&mutUsage, STR_CSTR(ename), STR_CSTR(cname), STR_CSTR(sname), id, STR_CSTR(descr), sb.ToCString(), CSTR(""), cateId);
						if (spId)
						{
							mutUsage.EndUse();
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
					}
				}
				else if (task->Equals(UTF8STRC("modify")) && species != 0)
				{
					Bool nameChg = !species->sciName->Equals(sname);
					sb.ClearStr();
					if (nameChg && me->env->SpeciesGetByName(&mutUsage, sname) != 0)
					{
						msg.AppendC(UTF8STRC("Species already exist"));
					}
					else if (nameChg && (bookIgn == 0 || bookIgn[0] != '1') && me->env->SpeciesBookIsExist(&mutUsage, STR_CSTR(sname), &sb))
					{
						msg.AppendC(UTF8STRC("Species already exist in book: "));
						msg.AppendC(sb.ToString(), sb.GetLength());
						msg.AppendC(UTF8STRC(", continue?"));
						bookIgn = (const UTF8Char*)"1";
					}
					else
					{
						sb.ClearStr();
						sb.Append(sname);
						sb.ToLower();
						sb.ReplaceStr(UTF8STRC(" "), UTF8STRC("_"));
						sb.ReplaceStr(UTF8STRC("."), UTF8STRC(""));
						if (me->env->SpeciesModify(&mutUsage, spId, STR_CSTR(ename), STR_CSTR(cname), STR_CSTR(sname), STR_CSTR(descr), sb.ToCString()))
						{
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
					}
				}
				else if (task->Equals(UTF8STRC("delete")) && species != 0 && species->files.GetCount() == 0 && species->books.GetCount() == 0 && species->wfiles.GetCount() == 0)
				{
					Int32 groupId = species->groupId;
					if (me->env->SpeciesDelete(&mutUsage, species->speciesId))
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("group.html?id="));
						sb.AppendI32(groupId);
						sb.AppendC(UTF8STRC("&cateId="));
						sb.AppendI32(cateId);

						resp->RedirectURL(req, sb.ToCString(), 0);
						return true;
					}
					else
					{
						msg.AppendC(UTF8STRC("Error in modifying species"));
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
		if (canDelete)
		{
			writer.WriteStrC(UTF8STRC("<br/><br/>"));
			writer.WriteStrC(UTF8STRC("<input type=\"button\" value=\"Delete\" onclick=\"document.forms.newspecies.task.value='delete';document.forms.newspecies.submit();\"/>"));
		}
		writer.WriteLineC(UTF8STRC("</td></tr>"));
		writer.WriteLineC(UTF8STRC("</table></form>"));
		me->WriteFooter(&writer);
		mutUsage.EndUse();
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
	RequestEnv env;
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
		GroupInfo *group;
		CategoryInfo *cate;
		IO::ConfigFile *lang = me->env->LangGet(req);
		Sync::RWMutexUsage mutUsage;
		group = me->env->GroupGet(&mutUsage, id);
		Bool notAdmin = (env.user == 0 || env.user->userType != 0);
		if (group == 0 || group->cateId != cateId)
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->env->CateGet(&mutUsage, group->cateId);
		if (cate == 0 || ((cate->flags & 1) && notAdmin))
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		if (me->env->GroupIsAdmin(group) && notAdmin)
		{
			mutUsage.EndUse();
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

		me->WriteLocator(&mutUsage, &writer, group, cate);
		writer.WriteLineC(UTF8STRC("<br/>"));
		if (group->descript)
		{
			s = Text::XML::ToNewHTMLBodyText(group->descript->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
		}
		writer.WriteLineC(UTF8STRC("<br/>"));
		writer.WriteLineC(UTF8STRC("<hr/>"));

		Data::StringMap<SpeciesInfo*> spMap;
		me->env->GetGroupSpecies(&mutUsage, group, &spMap, env.user);
		Data::ArrayList<SpeciesInfo*> speciesTmp;
		const Data::ReadingList<SpeciesInfo*> *spList;
		spList = spMap.GetValues();
		if (imageOnly)
		{
			SpeciesInfo *sp;
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
		Data::ArrayList<SpeciesInfo*> species;
		while (i < j)
		{
			species.Add(spList->GetItem(i));
			i++;
		}
		me->WriteSpeciesTable(&mutUsage, &writer, &species, env.scnWidth, group->cateId, false);
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
		mutUsage.EndUse();
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
	RequestEnv env;
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
		SpeciesInfo *species;
		GroupInfo *group;
		CategoryInfo *cate;
		Text::PString sarr[4];
		WebFileInfo *wfile;
		IO::ConfigFile *lang = me->env->LangGet(req);

		Sync::RWMutexUsage mutUsage;
		species = me->env->SpeciesGet(&mutUsage, id);
		if (species == 0 || species->cateId != cateId)
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		group = me->env->GroupGet(&mutUsage, species->groupId);
		if (group == 0)
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->env->CateGet(&mutUsage, group->cateId);
		if (cate == 0)
		{
			mutUsage.EndUse();
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
				if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST && env.user && (env.user->userType == 0 || env.user->id == userFile->webuserId))
				{
					req->ParseHTTPForm();
					Text::String *action = req->GetHTTPFormStr(CSTR("action"));
					if (action && action->Equals(UTF8STRC("setdefault")) && env.user->userType == 0)
					{
						me->env->SpeciesSetPhotoId(&mutUsage, id, fileId);
					}
					else if (action && action->Equals(UTF8STRC("setname")))
					{
						Text::String *desc = req->GetHTTPFormStr(CSTR("descr"));
						if (desc)
						{
							me->env->UserfileUpdateDesc(&mutUsage, fileId, desc->ToCString());
						}
					}
					else if (action && action->Equals(UTF8STRC("rotate")))
					{
						me->env->UserfileUpdateRotType(&mutUsage, fileId, (userFile->rotType + 1) & 3);
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
					sptr = me->env->UserfileGetPath(sbuff, userFile);
					UInt64 fileSize = 0;
					Media::MediaFile *mediaFile;
					{
						IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
						fileSize = fd.GetDataSize();
						mediaFile = (Media::MediaFile*)me->env->ParseFileType(&fd, IO::ParserType::MediaFile);
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
						writer.WriteStrC(UTF8STRC("<b>"));
						writer.WriteStr(LangGetValue(lang, UTF8STRC("PhotoDate")));
						writer.WriteStrC(UTF8STRC("</b> "));
						sptr2 = Data::Timestamp(userFile->captureTimeTicks, Data::DateTimeUtil::GetLocalTzQhr()).ToString(sbuff2, "yyyy-MM-dd HH:mm:ss zzzz");
						writer.WriteStrC(sbuff2, (UOSInt)(sptr2 - sbuff2));
						writer.WriteStrC(UTF8STRC("<br/>"));
					}
				}
				else
				{
					sptr = me->env->UserfileGetPath(sbuff, userFile);
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

						writer.WriteStrC(UTF8STRC("<b>"));
						writer.WriteStr(LangGetValue(lang, UTF8STRC("PhotoDate")));
						writer.WriteStrC(UTF8STRC("</b> "));
						sptr2 = Data::Timestamp(userFile->captureTimeTicks, Data::DateTimeUtil::GetLocalTzQhr()).ToString(sbuff2, "yyyy-MM-dd HH:mm:ss zzzz");
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
				mutUsage.EndUse();
				ResponseMstm(req, resp, &mstm, CSTR("text/html"));
				return true;
			}
			else
			{
				resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
				mutUsage.EndUse();
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
				mutUsage.EndUse();
				ResponseMstm(req, resp, &mstm, CSTR("text/html"));
				return true;
			}
			else
			{
				resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
				mutUsage.EndUse();
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
					mutUsage.EndUse();
					ResponseMstm(req, resp, &mstm, CSTR("text/html"));

					srcURL->Release();
					imgURL->Release();
					return true;
				}
				else
				{
					resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
					mutUsage.EndUse();
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
						mutUsage.EndUse();
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
					mutUsage.EndUse();
					ResponseMstm(req, resp, &mstm, CSTR("text/html"));

					LIST_FREE_STRING(&fileNameList);
					return true;
				}
				else
				{
					resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
					mutUsage.EndUse();
					return true;
				}
			}
		}
		mutUsage.EndUse();
	}
	resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcPhotoDetailD(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	RequestEnv env;
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
		SpeciesInfo *species;
		Data::DateTime dt;
		IO::ConfigFile *lang = me->env->LangGet(req);

		Sync::RWMutexUsage mutUsage;
		UserFileInfo *userFile = 0;
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
			UserFileInfo *userFile2;
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
			species = me->env->SpeciesGet(&mutUsage, userFile->speciesId);

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
				sptr = me->env->UserfileGetPath(sbuff, userFile);
				UInt64 fileSize = 0;
				Media::MediaFile *mediaFile;
				{
					IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
					fileSize = fd.GetDataSize();
					mediaFile = (Media::MediaFile*)me->env->ParseFileType(&fd, IO::ParserType::MediaFile);
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
				sptr = me->env->UserfileGetPath(sbuff, userFile);
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
			mutUsage.EndUse();
			ResponseMstm(req, resp, &mstm, CSTR("text/html"));
			return true;
		}
		else
		{
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			mutUsage.EndUse();
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
	RequestEnv env;
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

		Sync::RWMutexUsage mutUsage;
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
		UserFileInfo *userFile;
		SpeciesInfo *sp;
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
					sp = me->env->SpeciesGet(&mutUsage, userFile->speciesId);

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
			Data::Int64Map<TripInfo*> *tripCate = user->tripCates->Get(sp->cateId);
			if (tripCate)
			{
				OSInt ind = tripCate->GetIndex(userFile->captureTime);
				if (ind < 0)
				{
					ind = ~ind - 1;
				}
				TripInfo *trip = tripCate->GetValues()->GetItem(ind);
				if (trip != 0 && trip->fromDate <= userFile->captureTime && trip->toDate > userFile->captureTime)
				{
					LocationInfo *loc = this->locMap->Get(trip->locId);
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
			sp = me->env->SpeciesGet(&mutUsage, userFile->speciesId);
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
		mutUsage.EndUse();

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
	RequestEnv env;
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

		Sync::RWMutexUsage mutUsage;
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
		UserFileInfo *userFile;
		SpeciesInfo *sp;
		UInt32 colCount = env.scnWidth / PREVIEW_SIZE;
		UInt32 colWidth = 100 / colCount;
		UInt32 currColumn;
		currColumn = 0;
		writer.WriteLineC(UTF8STRC("<table border=\"0\" width=\"100%\">"));
			
		while (startIndex < endIndex)
		{
			userFile = env.user->userFileObj.GetItem((UOSInt)startIndex);
			sp = me->env->SpeciesGet(&mutUsage, userFile->speciesId);
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
			if (env.user->userType == 0 && sp)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<br/><a href=\"species.html?id="));
				sb.AppendI32(sp->speciesId);
				sb.AppendC(UTF8STRC("&amp;cateId="));
				sb.AppendI32(sp->cateId);
				sb.AppendC(UTF8STRC("\">"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());
				sb.ClearStr();
				sb.Append(sp->sciName);
				sb.AppendC(UTF8STRC(" "));
				sb.Append(sp->chiName);
				sb.AppendC(UTF8STRC(" "));
				sb.Append(sp->engName);
				s = Text::XML::ToNewHTMLBodyText(sb.ToString());
				writer.WriteStrC(s->v, s->leng);
				s->Release();
				writer.WriteLineC(UTF8STRC("</a>"));
			}

	/*		Data::Int64Map<TripInfo*> *tripCate = user->tripCates->Get(sp->cateId);
			if (tripCate)
			{
				OSInt ind = tripCate->GetIndex(userFile->captureTime);
				if (ind < 0)
				{
					ind = ~ind - 1;
				}
				TripInfo *trip = tripCate->GetValues()->GetItem(ind);
				if (trip != 0 && trip->fromDate <= userFile->captureTime && trip->toDate > userFile->captureTime)
				{
					LocationInfo *loc = this->locMap->Get(trip->locId);
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

		writer.WriteStrC(UTF8STRC("Data Files:<br/>"));
		me->WriteDataFiles(&writer, &env.user->gpsDataFiles, startTime, endTime);
		me->WriteDataFiles(&writer, &env.user->tempDataFiles, startTime, endTime);
		writer.WriteLineC(UTF8STRC("<hr/>"));

		sb.ClearStr();
		sb.AppendC(UTF8STRC("<a href=\"map/index.html?day="));
		sb.AppendI32(d);
		sb.AppendC(UTF8STRC("\">"));
		sb.AppendC(UTF8STRC("ShowMap"));
		sb.AppendC(UTF8STRC("</a><br/>"));
		writer.WriteLineC(sb.ToString(), sb.GetLength());

		sptr = Text::StrUInt32(sbuff, dt.GetYear());
		writer.WriteStrC(UTF8STRC("<a href=\"photoyear.html?y="));
		writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer.WriteStrC(UTF8STRC("\">Year "));
		writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer.WriteStrC(UTF8STRC("</a><br/>"));
		writer.WriteLineC(UTF8STRC("<a href=\"/\">Index</a><br/>"));
		mutUsage.EndUse();

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
	RequestEnv env;
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
	Sync::RWMutexUsage mutUsage;

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
		Int32 ret = me->env->UserfileAdd(&mutUsage, env.user->id, env.user->unorganSpId, CSTRP(fileName, fileNameEnd), fileCont, fileSize, true);
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
	mutUsage.EndUse();
	writer.WriteLineC(UTF8STRC("</table>"));
	me->WriteFooter(&writer);

	ResponseMstm(req, resp, &mstm, CSTR("text/html"));
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcPhotoUpload2(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

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
	Bool succ = true;
	req->ParseHTTPForm();

	while (true)
	{
		fileCont = req->GetHTTPFormFile(CSTR("file"), i, fileName, sizeof(fileName), &fileNameEnd, &fileSize);
		if (fileCont == 0)
		{
			break;
		}
		if (!me->env->UserfileAdd(&mutUsage, env.user->id, env.user->unorganSpId, CSTRP(fileName, fileNameEnd), fileCont, fileSize, true))
			succ = false;

		i++;
	}
	mutUsage.EndUse();

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
	RequestEnv env;
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

	Sync::RWMutexUsage mutUsage;
	Int32 ret = me->env->UserfileAdd(&mutUsage, env.user->id, env.user->unorganSpId, sb.ToCString(), imgData, dataSize, true);
	mutUsage.EndUse();

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
	RequestEnv env;
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
		GroupInfo *group;
		CategoryInfo *cate;
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		IO::ConfigFile *lang = me->env->LangGet(req);
		Sync::RWMutexUsage mutUsage;
		group = me->env->GroupGet(&mutUsage, id);
		Bool notAdmin = (env.user == 0 || env.user->userType != 0);
		if (group == 0 || group->cateId != cateId || (me->env->GroupIsAdmin(group) && notAdmin))
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->env->CateGet(&mutUsage, group->cateId);
		if (cate == 0 || ((cate->flags & 1) && notAdmin))
		{
			mutUsage.EndUse();
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

		me->WriteLocator(&mutUsage, &writer, group, cate);
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
		Data::ArrayList<SpeciesInfo*> speciesObjs;
		Data::ArrayListDbl groupIndice;
		Data::ArrayList<GroupInfo*> groupObjs;
		sb.ClearStr();
		sb.Append(searchStr);
		sb.Trim();
		writer.WriteStrC(UTF8STRC("Result for \""));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("\"<br/>"));
		me->env->SearchInGroup(&mutUsage, group, sb.ToString(), sb.GetLength(), &speciesIndice, &speciesObjs, &groupIndice, &groupObjs, env.user);

		Bool found = false;

		if (speciesObjs.GetCount() > 0)
		{
			Data::ArrayList<SpeciesInfo*> speciesList;
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
			me->WriteSpeciesTable(&mutUsage, &writer, &speciesList, env.scnWidth, group->cateId, false);
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
			Data::ArrayList<GroupInfo*> groupList;
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
			me->WriteGroupTable(&mutUsage, &writer, &groupList, env.scnWidth, false);
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
		mutUsage.EndUse();
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
	RequestEnv env;
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
		GroupInfo *group;
		CategoryInfo *cate;
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		IO::ConfigFile *lang = me->env->LangGet(req);
		Sync::RWMutexUsage mutUsage;
		group = me->env->GroupGet(&mutUsage, id);
		Bool notAdmin = (env.user == 0 || env.user->userType != 0);
		if (group == 0 || group->cateId != cateId || (me->env->GroupIsAdmin(group) && notAdmin))
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->env->CateGet(&mutUsage, group->cateId);
		if (cate == 0 || ((cate->flags & 1) && notAdmin))
		{
			mutUsage.EndUse();
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

		me->WriteLocator(&mutUsage, &writer, group, cate);
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
		Data::ArrayList<SpeciesInfo*> speciesObjs;
		Data::ArrayListDbl groupIndice;
		Data::ArrayList<GroupInfo*> groupObjs;
		sb.ClearStr();
		sb.Append(searchStr);
		sb.Trim();
		writer.WriteStrC(UTF8STRC("Result for \""));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("\"<br/>"));
		me->env->SearchInGroup(&mutUsage, group, sb.ToString(), sb.GetLength(), &speciesIndice, &speciesObjs, &groupIndice, &groupObjs, env.user);

		Bool found = false;

		if (speciesObjs.GetCount() > pageNo * 50)
		{
			Data::ArrayList<SpeciesInfo*> speciesList;
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
			me->WriteSpeciesTable(&mutUsage, &writer, &speciesList, env.scnWidth, group->cateId, false);
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
		mutUsage.EndUse();
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
	RequestEnv env;
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
		GroupInfo *group;
		CategoryInfo *cate;
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		IO::ConfigFile *lang = me->env->LangGet(req);
		Sync::RWMutexUsage mutUsage;
		group = me->env->GroupGet(&mutUsage, id);
		Bool notAdmin = (env.user == 0 || env.user->userType != 0);
		if (group == 0 || group->cateId != cateId || (me->env->GroupIsAdmin(group) && notAdmin))
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		cate = me->env->CateGet(&mutUsage, group->cateId);
		if (cate == 0 || ((cate->flags & 1) && notAdmin))
		{
			mutUsage.EndUse();
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

		me->WriteLocator(&mutUsage, &writer, group, cate);
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
		Data::ArrayList<SpeciesInfo*> speciesObjs;
		Data::ArrayListDbl groupIndice;
		Data::ArrayList<GroupInfo*> groupObjs;
		sb.ClearStr();
		sb.Append(searchStr);
		sb.Trim();
		writer.WriteStrC(UTF8STRC("Result for \""));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("\"<br/>"));
		me->env->SearchInGroup(&mutUsage, group, sb.ToString(), sb.GetLength(), &speciesIndice, &speciesObjs, &groupIndice, &groupObjs, env.user);

		Bool found = false;

		if (groupObjs.GetCount() > 0)
		{
			Data::ArrayList<GroupInfo*> groupList;
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
			me->WriteGroupTable(&mutUsage, &writer, &groupList, env.scnWidth, false);
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
		mutUsage.EndUse();
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
	RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 id;
	if (req->GetQueryValueI32(CSTR("id"), &id))
	{
		Text::String *s;
		Data::FastMap<Int64, BookInfo*> sortBookMap;
		Data::DateTime dt;
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		BookInfo *book;
		CategoryInfo *cate;
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		IO::ConfigFile *lang = me->env->LangGet(req);
		Sync::RWMutexUsage mutUsage;
		cate = me->env->CateGet(&mutUsage, id);
		if (cate == 0)
		{
			mutUsage.EndUse();
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

		me->env->BookGetDateMap(&mutUsage, &sortBookMap);

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
		mutUsage.EndUse();
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
	RequestEnv env;
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
		CategoryInfo *cate;
		BookSpInfo *bookSp;
		SpeciesInfo *species;
		Data::FastStringMap<SpeciesInfo*> speciesMap;
		Data::ArrayList<SpeciesInfo*> tempList;
		UOSInt i;
		UOSInt j;
		IO::ConfigFile *lang = me->env->LangGet(req);
		Text::StringBuilderUTF8 sb;
		Sync::RWMutexUsage mutUsage;
		cate = me->env->CateGet(&mutUsage, cateId);
		if (cate == 0)
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		book = me->env->BookGet(&mutUsage, id);
		if (book == 0)
		{
			mutUsage.EndUse();
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
			if (me->env->BookFileExist(book))
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
			species = me->env->SpeciesGet(&mutUsage, bookSp->speciesId);
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
		me->WriteSpeciesTable(&mutUsage, &writer, &tempList, env.scnWidth, cateId, false);
		writer.WriteLineC(UTF8STRC("<hr/>"));

		writer.WriteStrC(UTF8STRC("<a href=\"booklist.html?id="));
		sptr = Text::StrInt32(sbuff, cate->cateId);
		writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer.WriteStrC(UTF8STRC("\">"));
		writer.WriteStrC(UTF8STRC("Book List</a>"));

		me->WriteFooter(&writer);
		mutUsage.EndUse();
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
	RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Int32 id;
	if (req->GetQueryValueI32(CSTR("id"), &id))
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		BookInfo *book;
		Sync::RWMutexUsage mutUsage;
		book = me->env->BookGet(&mutUsage, id);
		if (env.user == 0 || env.user->userType != 0)
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_UNAUTHORIZED);
			return true;
		}
		else if (book == 0)
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		sptr = me->env->BookGetPath(sbuff, book->id);
		IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		UInt64 fileLen = fs.GetLength();
		if (fileLen <= 16)
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;

		}
		mutUsage.EndUse();

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
	RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Data::DateTime dt;
	UTF8Char sbuff[128];
	UTF8Char *sptr;

	if (env.user || !req->IsSecure())
	{
		resp->RedirectURL(req, CSTR("/"), 0);
		return true;
	}
	Sync::RWMutexUsage mutUsage;
	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		Text::String *userName = req->GetHTTPFormStr(CSTR("userName"));
		Text::String *pwd = req->GetHTTPFormStr(CSTR("password"));
		if (userName && pwd)
		{
			sptr = me->env->PasswordEnc(sbuff, pwd->ToCString());
			env.user = me->env->UserGetByName(&mutUsage, userName);
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

				IO::MemoryStream mstm;
				Text::UTF8Writer writer(&mstm);

				me->WriteHeaderPart1(&writer, (const UTF8Char*)"Index", env.isMobile);
				writer.WriteLineC(UTF8STRC("<script type=\"text/javascript\">"));
				writer.WriteLineC(UTF8STRC("function afterLoad()"));
				writer.WriteLineC(UTF8STRC("{"));
				writer.WriteLineC(UTF8STRC("	document.location.replace('/');"));
				writer.WriteLineC(UTF8STRC("}"));
				writer.WriteLineC(UTF8STRC("</script>"));
				me->WriteHeaderPart2(&writer, 0, (const UTF8Char*)"afterLoad()");
				writer.WriteLineC(UTF8STRC("Login succeeded"));

				me->WriteFooter(&writer);
				ResponseMstm(req, resp, &mstm, CSTR("text/html"));
				return true;
			}
			env.user = 0;
		}
	}

	IO::MemoryStream mstm;
	Text::UTF8Writer writer(&mstm);

	me->WriteHeader(&writer, (const UTF8Char*)"Index", env.user, env.isMobile);
	writer.WriteLineC(UTF8STRC("<center><h1>Login</h1></center>"));

	writer.WriteLineC(UTF8STRC("<form method=\"POST\" action=\"login.html\">"));
	writer.WriteLineC(UTF8STRC("User Name: <input type=\"text\" name=\"userName\"/><br/>"));
	writer.WriteLineC(UTF8STRC("Password: <input type=\"password\" name=\"password\"/><br/>"));
	writer.WriteLineC(UTF8STRC("<input type=\"submit\" /><br/>"));
	writer.WriteLineC(UTF8STRC("</form>"));
	mutUsage.EndUse();

	me->WriteFooter(&writer);
	ResponseMstm(req, resp, &mstm, CSTR("text/html"));
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcLogout(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	me->sessMgr->DeleteSession(req, resp);
	resp->RedirectURL(req, CSTR("/"), 0);
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcReload(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	if (me->env->HasReloadPwd())
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
				if (me->env->ReloadPwdMatches(pwd))
				{
					writer.WriteLineC(UTF8STRC("Reloaded<br/>"));
					showPwd = false;
					me->env->Reload();
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
	RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	if (me->env->HasReloadPwd())
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
				if (me->env->ReloadPwdMatches(pwd))
				{
					writer.WriteLineC(UTF8STRC("Restarting<br/>"));
					showPwd = false;
					me->env->Restart();
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
	RequestEnv env;
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
								"\t\t}\r\n"
								"\t\ti++;\r\n"
								"\t}\r\n"
								"\tif (failList.length > 0) {\r\n"
								"\t\tstatusText = \"Failed Files:<br/>\"+failList.join(\"<br/>\");\r\n"
								"\t} else {\r\n"
								"\t\tstatusText = \"Upload Success\";\r\n"
								"\t\tfileupload.value = null;\r\n"
								"\t}\r\n"
								"\tuploadStatus.innerHTML = statusText;\r\n"
								"\tdocument.getElementById(\"uploadStatus\").disabled = false;\r\n"
								"}\r\n"
								"</script>"));
	writer.WriteLineC(UTF8STRC("<center><h1>Index</h1></center>"));

	Sync::RWMutexUsage mutUsage;
	CategoryInfo *cate;
	CategoryInfo *firstCate = 0;
	UOSInt i;
	UOSInt j;
	Text::String *s;
	Text::StringBuilderUTF8 sb;
	Bool notAdmin = (env.user == 0 || env.user->userType != 0);
	Data::ReadingList<CategoryInfo*> *cateList = me->env->CateGetList(&mutUsage);
	i = 0;
	j = cateList->GetCount();
	while (i < j)
	{
		cate = cateList->GetItem(i);
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
	mutUsage.EndUse();

	me->WriteFooter(&writer);
	ResponseMstm(req, resp, &mstm, CSTR("text/html"));
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcCate(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	Sync::RWMutexUsage mutUsage;
	CategoryInfo *cate;
	Text::String *cateName = req->GetQueryValue(CSTR("cateName"));
	if (cateName != 0 && (cate = me->env->CateGetByName(&mutUsage, cateName)) != 0)
	{
		Text::String *s;
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		GroupInfo *group;
		Data::ArrayList<GroupInfo*> groups;
		IO::ConfigFile *lang = me->env->LangGet(req);

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
			me->env->CalcGroupCount(&mutUsage, group);
			if ((group->totalCount > 0 && (group->flags & 1) == 0) || !notAdmin)
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
		me->WriteLocator(&mutUsage, &writer, 0, 0);
		writer.WriteLineC(UTF8STRC("<hr/>"));
		me->WriteGroupTable(&mutUsage, &writer, &groups, env.scnWidth, false);
		writer.WriteLineC(UTF8STRC("<hr/>"));
		writer.WriteStrC(UTF8STRC("<a href=\"/\">"));
		writer.WriteStr(LangGetValue(lang, UTF8STRC("Back")));
		writer.WriteLineC(UTF8STRC("</a>"));
		me->WriteFooter(&writer);
		mutUsage.EndUse();
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
	RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcPublicPOI(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
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

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcGroupPOI(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
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

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcSpeciesPOI(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
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

Bool __stdcall SSWR::OrganMgr::OrganWebHandler::SvcDayPOI(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent)
{
	SSWR::OrganMgr::OrganWebHandler *me = (SSWR::OrganMgr::OrganWebHandler*)parent;
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

void SSWR::OrganMgr::OrganWebHandler::AddGroupPOI(Sync::RWMutexUsage *mutUsage, Text::StringBuilderUTF8 *sb, GroupInfo *group, Int32 userId)
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

void SSWR::OrganMgr::OrganWebHandler::AddSpeciesPOI(Sync::RWMutexUsage *mutUsage, Text::StringBuilderUTF8 *sb, SpeciesInfo *species, Int32 userId, Bool publicGroup)
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

void SSWR::OrganMgr::OrganWebHandler::AddUserfilePOI(Text::StringBuilderUTF8 *sb, SpeciesInfo *species, UserFileInfo *file)
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

void SSWR::OrganMgr::OrganWebHandler::ResponsePhoto(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, WebUserInfo *user, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, const UTF8Char *fileName)
{
	CategoryInfo *cate;
	SpeciesInfo *sp;
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr2;
	Int32 rotateType = 0;
	UTF8Char *sptr;
	UTF8Char *sptrEnd = sbuff;
	Sync::RWMutexUsage mutUsage;
	sp = this->env->SpeciesGet(&mutUsage, speciesId);
	Bool notAdmin = (user == 0 || user->userType != 0);
	if (sp && sp->cateId == cateId)
	{
		cate = this->env->CateGet(&mutUsage, sp->cateId);
		if (cate && ((cate->flags & 1) == 0 || !notAdmin))
		{
			Text::StringBuilderUTF8 sb;
			Text::String *cacheDir = this->env->GetCacheDir();
			if (cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE)
			{
				sptr = cacheDir->ConcatTo(sbuff);
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
						mutUsage.EndUse();
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
			mutUsage.EndUse();
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
			{
				IO::StmData::FileData fd(sb.ToCString(), false);
				imgList = (Media::ImageList*)this->env->ParseFileType(&fd, IO::ParserType::ImageList);
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
					this->csconv = Media::CS::CSConverter::NewConverter(this->csconvFCC, this->csconvBpp, this->csconvPF, &this->csconvColor, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, &color, Media::ColorProfile::YUVT_UNKNOWN, this->env->GetColorSess());
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

						if (cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE && mstm.GetLength() > 0)
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
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return;
		}
	}
	else
	{
		mutUsage.EndUse();
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return;
	}
}

void SSWR::OrganMgr::OrganWebHandler::ResponsePhotoId(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, WebUserInfo *reqUser, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, Int32 fileId)
{
	SpeciesInfo *sp;
	UTF8Char sbuff2[512];
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UserFileInfo *userFile;
	Int32 rotateType = 0;
	Sync::RWMutexUsage mutUsage;
	sp = this->env->SpeciesGet(&mutUsage, speciesId);
	userFile = this->env->UserfileGet(&mutUsage, fileId);
	if (sp && sp->cateId == cateId && userFile && (userFile->fileType == 1 || userFile->fileType == 3))
	{
		if (sp->photoId == fileId && userFile->speciesId != sp->speciesId)
		{
			this->env->SpeciesUpdateDefPhoto(&mutUsage, sp->speciesId);
		}

		Text::String *cacheDir = this->env->GetCacheDir();
		Data::DateTime dt;
		WebUserInfo *user;
		user = this->env->UserGet(&mutUsage, userFile->webuserId);
		dt.SetTicks(userFile->fileTimeTicks);
		dt.ToUTCTime();
		rotateType = userFile->rotType;

		sptr = cacheDir->ConcatTo(sbuff2);
		sptr = IO::Path::AppendPath(sbuff2, sptr, CSTR("UserFile"));
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrInt32(sptr, userFile->webuserId);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = dt.ToString(sptr, "yyyyMM");
		IO::Path::CreateDirectory(CSTRP(sbuff2, sptr));
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr2 = userFile->dataFileName->ConcatTo(sptr);

		if (imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE && userFile->prevUpdated == 0)
		{
			Data::DateTime dt2;
			Data::DateTime dt3;
			if (req->GetIfModifiedSince(&dt2) && IO::Path::GetFileTime(sbuff2, &dt3, 0, 0))
			{
				Int64 tdiff = dt2.ToTicks() - dt3.ToTicks();
				if (tdiff >= -1000 && tdiff <= 1000)
				{
					mutUsage.EndUse();
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
				mutUsage.EndUse();
				MemFree(buff);
				return;
			}
		}

		sptr = this->env->UserfileGetPath(sbuff, userFile);
		mutUsage.EndUse();

		Media::ImageList *imgList;
		Media::StaticImage *simg;
		Media::StaticImage *lrimg;
		Media::StaticImage *dimg;
		{
			IO::StmData::FileData fd({sbuff, (UOSInt)(sptr - sbuff)}, false);
			imgList = (Media::ImageList*)this->env->ParseFileType(&fd, IO::ParserType::ImageList);
		}
		if (imgList)
		{
			simg = imgList->GetImage(0, 0)->CreateStaticImage();
			DEL_CLASS(imgList);
			Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
			NEW_CLASS(lrimg, Media::StaticImage(simg->info.dispWidth, simg->info.dispHeight, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, 0, &color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			{
				Sync::MutexUsage mutUsage(&this->csconvMut);
				if (this->csconv == 0 || this->csconvFCC != simg->info.fourcc || this->csconvBpp != simg->info.storeBPP || this->csconvPF != simg->info.pf || !simg->info.color->Equals(&this->csconvColor))
				{
					SDEL_CLASS(this->csconv);
					this->csconvFCC = simg->info.fourcc;
					this->csconvBpp = simg->info.storeBPP;
					this->csconvPF = simg->info.pf;
					this->csconvColor.Set(simg->info.color);
					this->csconv = Media::CS::CSConverter::NewConverter(this->csconvFCC, this->csconvBpp, this->csconvPF, &this->csconvColor, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, &color, Media::ColorProfile::YUVT_UNKNOWN, this->env->GetColorSess());
				}
				if (this->csconv)
				{
					this->csconv->ConvertV2(&simg->data, lrimg->data, simg->info.dispWidth, simg->info.dispHeight, simg->info.storeWidth, simg->info.storeHeight, (OSInt)lrimg->GetDataBpl(), Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);
				}
				else
				{
					SDEL_CLASS(lrimg);
				}
			}
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
					Media::DrawImage *gimg = this->env->GetDrawEngine()->ConvImage(dimg);
					if ((cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE) || user != reqUser)
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
								gimg2 = this->env->GetDrawEngine()->CreateImage32(iWidth, iHeight, Media::AT_NO_ALPHA);
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
								this->env->GetDrawEngine()->DeleteImage(gimg2);
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

					if (cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE)
					{
						IO::FileStream fs({sbuff2, (UOSInt)(sptr2 - sbuff2)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
						buff = mstm.GetBuff(&buffSize);
						fs.Write(buff, buffSize);
						if (userFile->prevUpdated)
						{
							this->env->UserFilePrevUpdated(&mutUsage, userFile);
						}
					}

					DEL_CLASS(dimg);
					this->env->GetDrawEngine()->DeleteImage(gimg);
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

					if (cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE)
					{
						IO::FileStream fs({sbuff2, (UOSInt)(sptr2 - sbuff2)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
						buff = mstm.GetBuff(&buffSize);
						fs.Write(buff, buffSize);
						if (userFile->prevUpdated)
						{
							this->env->UserFilePrevUpdated(&mutUsage, userFile);
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
		mutUsage.EndUse();
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return;
	}
}

void SSWR::OrganMgr::OrganWebHandler::ResponsePhotoWId(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, WebUserInfo *reqUser, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, Int32 fileWId)
{
	SpeciesInfo *sp;
	UTF8Char sbuff2[512];
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	WebFileInfo *wfile;
	Int32 rotateType = 0;
	Sync::RWMutexUsage mutUsage;
	sp = this->env->SpeciesGet(&mutUsage, speciesId);
	if (sp && sp->cateId == cateId)
	{
		wfile = sp->wfiles.Get(fileWId);
		if (wfile)
		{
			Data::DateTime dt;
			Text::String *cacheDir = this->env->GetCacheDir();
			sptr = cacheDir->ConcatTo(sbuff2);
			sptr = IO::Path::AppendPath(sbuff2, sptr, CSTR("WebFile"));
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrInt32(sptr, wfile->id >> 10);
			IO::Path::CreateDirectory(CSTRP(sbuff2, sptr));
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrInt32(sptr, wfile->id);
			sptr2 = Text::StrConcatC(sptr, UTF8STRC(".jpg"));

			if (imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE && wfile->prevUpdated == 0)
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
					mutUsage.EndUse();
					MemFree(buff);
					return;
				}
			}

			sptr = this->env->GetDataDir()->ConcatTo(sbuff);
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

			Media::ImageList *imgList;
			Media::StaticImage *simg;
			Media::StaticImage *lrimg;
			Media::StaticImage *dimg;
			{
				IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
				imgList = (Media::ImageList*)this->env->ParseFileType(&fd, IO::ParserType::ImageList);
			}
			if (imgList)
			{
				simg = imgList->GetImage(0, 0)->CreateStaticImage();
				DEL_CLASS(imgList);
				Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
				NEW_CLASS(lrimg, Media::StaticImage(simg->info.dispWidth, simg->info.dispHeight, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, 0, &color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
				{
					Sync::MutexUsage mutUsage(&this->csconvMut);
					if (this->csconv == 0 || this->csconvFCC != simg->info.fourcc || this->csconvBpp != simg->info.storeBPP || this->csconvPF != simg->info.pf || !simg->info.color->Equals(&this->csconvColor))
					{
						SDEL_CLASS(this->csconv);
						this->csconvFCC = simg->info.fourcc;
						this->csconvBpp = simg->info.storeBPP;
						this->csconvPF = simg->info.pf;
						this->csconvColor.Set(simg->info.color);
						this->csconv = Media::CS::CSConverter::NewConverter(this->csconvFCC, this->csconvBpp, this->csconvPF, &this->csconvColor, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, &color, Media::ColorProfile::YUVT_UNKNOWN, this->env->GetColorSess());
					}
					if (this->csconv)
					{
						this->csconv->ConvertV2(&simg->data, lrimg->data, simg->info.dispWidth, simg->info.dispHeight, simg->info.storeWidth, simg->info.storeHeight, (OSInt)lrimg->GetDataBpl(), Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);
					}
					else
					{
						SDEL_CLASS(lrimg);
					}
				}
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

					if (cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE)
					{
						IO::FileStream fs({sbuff2, (UOSInt)(sptr2 - sbuff2)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
						buff = mstm.GetBuff(&buffSize);
						fs.Write(buff, buffSize);
						if (wfile->prevUpdated)
						{
							this->env->WebFilePrevUpdated(&mutUsage, wfile);
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
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return;
		}
	}
	else
	{
		mutUsage.EndUse();
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

void SSWR::OrganMgr::OrganWebHandler::WriteHeaderPart2(IO::Writer *writer, WebUserInfo *user, const UTF8Char *onLoadFunc)
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

void SSWR::OrganMgr::OrganWebHandler::WriteHeader(IO::Writer *writer, const UTF8Char *title, WebUserInfo *user, Bool isMobile)
{
	this->WriteHeaderPart1(writer, title, isMobile);
	this->WriteHeaderPart2(writer, user, 0);
}

void SSWR::OrganMgr::OrganWebHandler::WriteFooter(IO::Writer *writer)
{
	writer->WriteLineC(UTF8STRC("</BODY>"));
	writer->WriteLineC(UTF8STRC("</HTML>"));
}

void SSWR::OrganMgr::OrganWebHandler::WriteLocator(Sync::RWMutexUsage *mutUsage, IO::Writer *writer, GroupInfo *group, CategoryInfo *cate)
{
	GroupTypeInfo *grpType;
	Text::String *s;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[12];
	UTF8Char *sptr;
	Data::ArrayList<GroupInfo *> groupList;
	UOSInt i;
	while (group)
	{
		groupList.Add(group);
		group = this->env->GroupGet(mutUsage, group->parentId);
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
	WriteLocatorText(mutUsage, writer, group, cate);
	writer->WriteLineC(UTF8STRC("</center>"));
}

void SSWR::OrganMgr::OrganWebHandler::WriteLocatorText(Sync::RWMutexUsage *mutUsage, IO::Writer *writer, GroupInfo *group, CategoryInfo *cate)
{
	Text::StringBuilderUTF8 sb;
	Data::ArrayList<GroupInfo *> groupList;
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
		group = this->env->GroupGet(mutUsage, group->parentId);
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

void SSWR::OrganMgr::OrganWebHandler::WriteGroupTable(Sync::RWMutexUsage *mutUsage, IO::Writer *writer, const Data::ReadingList<GroupInfo *> *groupList, UInt32 scnWidth, Bool showSelect)
{
	GroupInfo *group;
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
			this->env->CalcGroupCount(mutUsage, group);
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

void SSWR::OrganMgr::OrganWebHandler::WriteSpeciesTable(Sync::RWMutexUsage *mutUsage, IO::Writer *writer, const Data::ArrayList<SpeciesInfo *> *spList, UInt32 scnWidth, Int32 cateId, Bool showSelect)
{
	SpeciesInfo *sp;
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

			if (sp->files.GetCount() > 0)
			{
				if ((sp->flags & SSWR::OrganMgr::SF_HAS_MYPHOTO) == 0)
				{
					this->env->SpeciesSetFlags(mutUsage, sp->speciesId, (SSWR::OrganMgr::SpeciesFlags)(sp->flags | SSWR::OrganMgr::SF_HAS_MYPHOTO));
					this->env->GroupAddCounts(mutUsage, sp->groupId, 0, (sp->flags & SSWR::OrganMgr::SF_HAS_WEBPHOTO)?0:1, 1);
				}
			}
			else
			{
				if (sp->flags & SSWR::OrganMgr::SF_HAS_MYPHOTO)
				{
					this->env->SpeciesSetFlags(mutUsage, sp->speciesId, (SSWR::OrganMgr::SpeciesFlags)(sp->flags & ~SSWR::OrganMgr::SF_HAS_MYPHOTO));
					this->env->GroupAddCounts(mutUsage, sp->groupId, 0, (sp->flags & SSWR::OrganMgr::SF_HAS_WEBPHOTO)?0:(UOSInt)-1, (UOSInt)-1);
				}
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

void SSWR::OrganMgr::OrganWebHandler::WritePickObjs(Sync::RWMutexUsage *mutUsage, IO::Writer *writer, RequestEnv *env, const UTF8Char *url, Bool allowMerge)
{
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;
	Text::String *s;
	UInt32 colCount = env->scnWidth / PREVIEW_SIZE;
	UInt32 colWidth = 100 / colCount;
	UInt32 currColumn;
	UserFileInfo *userFile;
	SpeciesInfo *species;
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
			userFile = this->env->UserfileGet(mutUsage, env->pickObjs->GetItem(i));
			species = 0;
			if (userFile) species = this->env->SpeciesGet(mutUsage, userFile->speciesId);
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
		SpeciesInfo *species;
		Data::ArrayList<SpeciesInfo*> spList;
		i = 0;
		j = env->pickObjs->GetCount();
		while (i < j)
		{
			species = this->env->SpeciesGet(mutUsage, env->pickObjs->GetItem(i));
			if (species)
			{
				spList.Add(species);
			}
			i++;
		}
		WriteSpeciesTable(mutUsage, writer, &spList, scnSize, 0, true);
		writer->WriteLineC(UTF8STRC("<input type=\"submit\" value=\"Place Selected\"/>"));
		writer->WriteLineC(UTF8STRC("<input type=\"button\" value=\"Place All\" onclick=\"document.forms.pickfiles.action.value='placeall';document.forms.pickfiles.submit();\"/>"));
		if (allowMerge)
		{
			writer->WriteLineC(UTF8STRC("<input type=\"button\" value=\"Place Merge\" onclick=\"document.forms.pickfiles.action.value='placemerge';document.forms.pickfiles.submit();\"/>"));
		}
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
		GroupInfo *group;
		Data::ArrayList<GroupInfo*> groupList;
		i = 0;
		j = env->pickObjs->GetCount();
		while (i < j)
		{
			group = this->env->GroupGet(mutUsage, env->pickObjs->GetItem(i));
			if (group)
			{
				groupList.Add(group);
			}
			i++;
		}
		WriteGroupTable(mutUsage, writer, &groupList, scnSize, true);
		writer->WriteLineC(UTF8STRC("<input type=\"submit\" value=\"Place Selected\"/>"));
		writer->WriteLineC(UTF8STRC("<input type=\"button\" value=\"Place All\" onclick=\"document.forms.pickfiles.action.value='placeall';document.forms.pickfiles.submit();\"/>"));
		writer->WriteLineC(UTF8STRC("</form>"));
		writer->WriteLineC(UTF8STRC("<hr/>"));
	}
}

void SSWR::OrganMgr::OrganWebHandler::WriteDataFiles(IO::Writer *writer, Data::FastMap<Data::Timestamp, DataFileInfo*> *fileMap, Int64 startTimeTicks, Int64 endTimeTicks)
{
	OSInt startIndex;
	OSInt endIndex;
	Text::StringBuilderUTF8 sb;
	startIndex = fileMap->GetIndex(Data::Timestamp(startTimeTicks, 0));
	if (startIndex < 0)
		startIndex = ~startIndex;
	if (startIndex > 0 && fileMap->GetItem((UOSInt)startIndex - 1)->endTime.ToTicks() > startTimeTicks)
		startIndex--;
	endIndex = fileMap->GetIndex(Data::Timestamp(endTimeTicks, 0));
	if (endIndex < 0)
		endIndex = ~endIndex;
	while (startIndex < endIndex)
	{
		DataFileInfo *dataFile = fileMap->GetItem((UOSInt)startIndex);
		sb.ClearStr();
		//sb.AppendC(UTF8STRC("<a href=\"datafile.html\">"));
		if (dataFile->fileType == DataFileType::GPSTrack)
		{
			sb.AppendC(UTF8STRC("GPS: "));
		}
		else if (dataFile->fileType == DataFileType::Temperature)
		{
			sb.AppendC(UTF8STRC("Temp: "));
		}
		else
		{
			sb.AppendC(UTF8STRC("Other: "));
		}
		sb.AppendTS(dataFile->startTime);
		sb.AppendC(UTF8STRC(" - "));
		sb.AppendTS(dataFile->endTime);
		sb.AppendC(UTF8STRC("<br/>"));
		writer->WriteLineC(sb.ToString(), sb.GetLength());
		startIndex++;
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

Text::CString SSWR::OrganMgr::OrganWebHandler::LangGetValue(IO::ConfigFile *lang, const UTF8Char *name, UOSInt nameLen)
{
	if (lang == 0)
		return {name, nameLen};
	Text::String *s = lang->GetValue({name, nameLen});
	if (s)
		return s->ToCString();
	return {name, nameLen};
}

SSWR::OrganMgr::OrganWebHandler::OrganWebHandler(OrganWebEnv *env, UInt32 scnSize) : csconvColor(Media::ColorProfile::CPT_SRGB)
{
	this->env = env;
	this->scnSize = scnSize;
	Media::ColorProfile destProfile(Media::ColorProfile::CPT_SRGB);
	NEW_CLASS(this->resizerLR, Media::Resizer::LanczosResizerLR_C32(3, 3, &destProfile, this->env->GetColorSess(), Media::AT_NO_ALPHA, 0, Media::PF_B8G8R8A8));
	this->csconv = 0;
	this->csconvFCC = 0;
	this->csconvBpp = 0;
	this->csconvPF = Media::PF_UNKNOWN;

	NEW_CLASS(this->sessMgr, Net::WebServer::MemoryWebSessionManager(CSTR("/"), OnSessionDel, this, 30000, OnSessionCheck, this, CSTR("OrganSessId")));
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
	this->AddService(CSTR("/daypoi"), Net::WebUtil::RequestMethod::HTTP_GET, SvcDayPOI);
}

SSWR::OrganMgr::OrganWebHandler::~OrganWebHandler()
{
	DEL_CLASS(this->resizerLR);
	SDEL_CLASS(this->csconv);
	SDEL_CLASS(this->sessMgr);
}
