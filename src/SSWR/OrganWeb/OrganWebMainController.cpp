#include "Stdafx.h"
#include "Data/ArrayListICaseString.h"
#include "Data/ByteBuffer.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Exporter/GUIJPGExporter.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Media/IAudioSource.h"
#include "Media/ICCProfile.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "Media/MediaFile.h"
#include "Media/PhotoInfo.h"
#include "Net/MIME.h"
#include "Net/WebServer/WebSessionUsage.h"
#include "SSWR/OrganWeb/OrganWebEnv.h"
#include "SSWR/OrganWeb/OrganWebMainController.h"
#include "Text/JSText.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "Text/XML.h"

#include <stdio.h>

Bool __stdcall SSWR::OrganWeb::OrganWebMainController::SvcGroup(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<Net::WebServer::WebController> parent)
{
	NotNullPtr<SSWR::OrganWeb::OrganWebMainController> me = NotNullPtr<SSWR::OrganWeb::OrganWebMainController>::ConvertFrom(parent);
	RequestEnv env;
	Net::WebServer::WebSessionUsage webSess(me->ParseRequestEnv(req, resp, env, true));

	Int32 id;
	Int32 cateId;
	if (req->GetQueryValueI32(CSTR("id"), id) &&
		req->GetQueryValueI32(CSTR("cateId"), cateId))
	{
		NotNullPtr<Text::String> s;
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		NotNullPtr<GroupInfo> group;
		NotNullPtr<CategoryInfo> cate;
		IO::ConfigFile *lang = me->env->LangGet(req);
		Sync::RWMutexUsage mutUsage;
		Bool notAdmin = (env.user == 0 || env.user->userType != UserType::Admin);
		if (!me->env->GroupGet(mutUsage, id).SetTo(group) || group->cateId != cateId)
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		if (!me->env->CateGet(mutUsage, group->cateId).SetTo(cate) || ((cate->flags & 1) && notAdmin))
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

		if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST && env.user != 0 && env.user->userType == UserType::Admin)
		{
			req->ParseHTTPForm();
			NotNullPtr<Text::String> action;
			Int32 itemId;
			if (req->GetHTTPFormStr(CSTR("action")).SetTo(action))
			{
				if (action->Equals(UTF8STRC("pickall")))
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
				else if (action->Equals(UTF8STRC("picksel")))
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
							if (req->GetHTTPFormStr(sb.ToCString()).SetTo(s) && s->v[0] == '1')
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
							if (req->GetHTTPFormStr(sb.ToCString()).SetTo(s) && s->v[0] == '1')
							{
								env.pickObjs->SortedInsert(itemId);
							}
							i++;
						}
					}
				}
				else if (action->Equals(UTF8STRC("place")))
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
							if (req->GetHTTPFormStr(sb.ToCString()).SetTo(s) && s->v[0] == '1')
							{
								if (me->env->GroupMove(mutUsage, itemId, id, cateId))
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
							if (req->GetHTTPFormStr(sb.ToCString()).SetTo(s) && s->v[0] == '1')
							{
								if (me->env->SpeciesMove(mutUsage, itemId, id, cateId))
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
				else if (action->Equals(UTF8STRC("placeall")))
				{
					if (env.pickObjType == POT_GROUP && group->species.GetCount() == 0)
					{
						i = 0;
						j = env.pickObjs->GetCount();
						while (i < j)
						{
							itemId = env.pickObjs->GetItem(i);
							if (me->env->GroupMove(mutUsage, itemId, id, cateId))
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
							if (me->env->SpeciesMove(mutUsage, itemId, id, cateId))
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
				else if (action->Equals(UTF8STRC("setphoto")))
				{
					me->env->GroupSetPhotoGroup(mutUsage, group->parentId, group->id);
				}
			}
		}

		IO::MemoryStream mstm;
		Text::UTF8Writer writer(mstm);

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

		me->WriteLocator(mutUsage, &writer, group, cate);
		writer.WriteLineC(UTF8STRC("<br/>"));
		s = Text::XML::ToNewHTMLBodyText(group->descript->v);
		writer.WriteStrC(s->v, s->leng);
		s->Release();
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
		sb.Append(LangGetValue(lang, CSTR("ListAll")));
		sb.AppendC(UTF8STRC("</a><br/>"));
		writer.WriteLineC(sb.ToString(), sb.GetCharCnt());

		sb.ClearStr();
		sb.AppendC(UTF8STRC("<a href=\"listimage.html?id="));
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&amp;cateId="));
		sb.AppendI32(group->cateId);
		sb.AppendC(UTF8STRC("\">"));
		sb.Append(LangGetValue(lang, CSTR("ListImage")));
		sb.AppendC(UTF8STRC("</a><br/>"));
		writer.WriteLineC(sb.ToString(), sb.GetLength());

		if (env.user != 0 || me->env->GroupIsPublic(mutUsage, group->id))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<a href=\"map/index.html?group="));
			sb.AppendI32(group->id);
			sb.AppendC(UTF8STRC("\">"));
			sb.Append(LangGetValue(lang, CSTR("ShowMap")));
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
			Data::StringMap<Optional<GroupInfo>> groups;
			i = group->groups.GetCount();
			while (i-- > 0)
			{
				sgroup = group->groups.GetItem(i);
				if ((sgroup->flags & 1) == 0 || !notAdmin)
				{
					groups.PutNN(sgroup->engName, sgroup);
				}
			}
			if (groups.GetCount() > 0)
			{
				me->WriteGroupTable(mutUsage, &writer, groups.GetValues(), env.scnWidth, !notAdmin, false);
				writer.WriteLineC(UTF8STRC("<hr/>"));
				found = true;
			}
		}
		if (group->species.GetCount())
		{
			SpeciesInfo *sp;
			Data::StringMap<Optional<SpeciesInfo>> species;
			i = group->species.GetCount();
			while (i-- > 0)
			{
				sp = group->species.GetItem(i);
				species.PutNN(sp->sciName, sp);
			}
			me->WriteSpeciesTable(mutUsage, &writer, species.GetValues(), env.scnWidth, group->cateId, !notAdmin, !notAdmin);
			writer.WriteLineC(UTF8STRC("<hr/>"));
			found = true;
		}
		if (!found)
		{
			writer.WriteLineCStr(LangGetValue(lang, CSTR("NoItems")));
			writer.WriteLineC(UTF8STRC("<hr/>"));
		}
		if (!notAdmin)
		{
			writer.WriteLineC(UTF8STRC("</form>"));
		}
		if (env.user != 0 && env.user->userType == UserType::Admin)
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
			me->WritePickObjs(mutUsage, &writer, &env, sb.ToString(), false);
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
			writer.WriteStr(LangGetValue(lang, CSTR("Back")));
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
			writer.WriteStr(LangGetValue(lang, CSTR("Back")));
			writer.WriteStrC(UTF8STRC("</a>"));
		}
		if (env.user != 0 && env.user->userType == UserType::Admin)
		{
			writer.WriteLineC(UTF8STRC("<br/>"));

			writer.WriteLineC(UTF8STRC("<input type=\"button\" value=\"Set Parent Photo\" onclick=\"document.forms.groupform.action.value='setphoto';document.forms.groupform.submit();\"/>"));
		}


		me->WriteFooter(&writer);
		mutUsage.EndUse();
		ResponseMstm(req, resp, mstm, CSTR("text/html"));
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}


Bool __stdcall SSWR::OrganWeb::OrganWebMainController::SvcGroupMod(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<Net::WebServer::WebController> parent)
{
	NotNullPtr<SSWR::OrganWeb::OrganWebMainController> me = NotNullPtr<SSWR::OrganWeb::OrganWebMainController>::ConvertFrom(parent);
	RequestEnv env;
	UOSInt i;
	UOSInt j;
	me->ParseRequestEnv(req, resp, env, false);

	if (env.user == 0 || env.user->userType != UserType::Admin)
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}

	Int32 id;
	Int32 cateId;
	Int32 groupId = 0;
	if (req->GetQueryValueI32(CSTR("id"), id) &&
		req->GetQueryValueI32(CSTR("cateId"), cateId))
	{
		NotNullPtr<GroupInfo> group;
		NotNullPtr<CategoryInfo> cate;
		Text::StringBuilderUTF8 sb;
		NotNullPtr<Text::String> s;
		NotNullPtr<Text::String> txt;
		IO::ConfigFile *lang = me->env->LangGet(req);

		Sync::RWMutexUsage mutUsage;
		if (!me->env->GroupGet(mutUsage, id).SetTo(group))
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		if (!me->env->CateGet(mutUsage, cateId).SetTo(cate))
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		Text::StringBuilderUTF8 msg;
		NotNullPtr<Text::String> ename = Text::String::NewEmpty();
		NotNullPtr<Text::String> cname = ename;
		NotNullPtr<Text::String> descr = ename;
		GroupFlags groupFlags = GF_NONE;
		Int32 groupTypeId = 0;
		Optional<GroupInfo> modGroup = 0;
		NotNullPtr<GroupInfo> nnmodGroup;
		if (req->GetQueryValueI32(CSTR("groupId"), groupId))
		{
			modGroup = me->env->GroupGet(mutUsage, groupId);
			if (modGroup.SetTo(nnmodGroup))
			{
				cname = nnmodGroup->chiName;
				ename = nnmodGroup->engName;
				descr = nnmodGroup->descript;
				groupTypeId = nnmodGroup->groupType;
			}
		}
		if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
		{
			req->ParseHTTPForm();
			NotNullPtr<Text::String> task;
			if (req->GetHTTPFormStr(CSTR("adminOnly")).SetTo(txt) && txt->v[0] == '1')
			{
				groupFlags = (GroupFlags)(groupFlags | GF_ADMIN_ONLY);
			}
			if (req->GetHTTPFormStr(CSTR("task")).SetTo(task) &&
				req->GetHTTPFormStr(CSTR("cname")).SetTo(cname) &&
				req->GetHTTPFormInt32(CSTR("groupType"), groupTypeId) &&
				req->GetHTTPFormStr(CSTR("ename")).SetTo(ename) &&
				req->GetHTTPFormStr(CSTR("descr")).SetTo(descr) && ename->v[0] != 0 && cname->v[0] != 0)
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
						Int32 newGroupId = me->env->GroupAdd(mutUsage, ename->ToCString(), cname->ToCString(), id, descr->ToCString(), groupTypeId, cateId, groupFlags);
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
				else if (task->Equals(UTF8STRC("modify")) && modGroup.SetTo(nnmodGroup) && nnmodGroup->cateId == cateId)
				{
					Bool found = false;
					i = group->groups.GetCount();
					while (i-- > 0)
					{
						if (group->groups.GetItem(i) != nnmodGroup.Ptr() && group->groups.GetItem(i)->engName->Equals(ename))
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
						if (me->env->GroupModify(mutUsage, nnmodGroup->id, ename->ToCString(), cname->ToCString(), descr->ToCString(), groupTypeId, groupFlags))
						{
							mutUsage.EndUse();
							sb.ClearStr();
							sb.AppendC(UTF8STRC("group.html?id="));
							sb.AppendI32(nnmodGroup->id);
							sb.AppendC(UTF8STRC("&cateId="));
							sb.AppendI32(nnmodGroup->cateId);

							resp->RedirectURL(req, sb.ToCString(), 0);
							return true;
						}
						else
						{
							msg.AppendC(UTF8STRC("Error in modifying group"));
						}
					}
				}
				else if (task->Equals(UTF8STRC("delete")) && modGroup.SetTo(nnmodGroup) && nnmodGroup->groups.GetCount() == 0 && nnmodGroup->species.GetCount() == 0)
				{
					Int32 parentId = nnmodGroup->parentId;
					Int32 cateId = nnmodGroup->cateId;
					if (me->env->GroupDelete(mutUsage, nnmodGroup->id))
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
		Text::UTF8Writer writer(mstm);

		sb.ClearStr();
		if (!modGroup.IsNull())
		{
			sb.AppendC(UTF8STRC("Modify group in "));
		}
		else
		{
			sb.AppendC(UTF8STRC("New group in "));
		}
		sb.Append(group->chiName);
		sb.AppendC(UTF8STRC(" "));
		sb.Append(group->engName);
		if (modGroup.SetTo(nnmodGroup))
		{
			sb.AppendC(UTF8STRC(" of "));
			sb.Append(nnmodGroup->chiName);
			sb.AppendC(UTF8STRC(" "));
			sb.Append(nnmodGroup->engName);
		}
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
		if (ename->leng > 0)
		{
			writer.WriteStrC(UTF8STRC(" value="));
			s = Text::XML::ToNewAttrText(ename->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
		}
		writer.WriteLineC(UTF8STRC("/></td></tr>"));
		writer.WriteStrC(UTF8STRC("<tr><td>Chinese Name</td><td><input type=\"text\" name=\"cname\""));
		if (cname->leng > 0)
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
		if (descr->leng > 0)
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
			NotNullPtr<GroupInfo> modGroup;
			if (me->env->GroupGet(mutUsage, groupId).SetTo(modGroup) && modGroup->species.GetCount() == 0 && modGroup->groups.GetCount() == 0)
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
		writer.WriteStr(LangGetValue(lang, CSTR("Back")));
		writer.WriteStrC(UTF8STRC("</a>"));
		writer.WriteLineC(UTF8STRC("</td></tr>"));
		writer.WriteLineC(UTF8STRC("</table></form>"));
		me->WriteFooter(&writer);
		mutUsage.EndUse();
		ResponseMstm(req, resp, mstm, CSTR("text/html"));
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganWeb::OrganWebMainController::SvcSpecies(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<Net::WebServer::WebController> parent)
{
	NotNullPtr<SSWR::OrganWeb::OrganWebMainController> me = NotNullPtr<SSWR::OrganWeb::OrganWebMainController>::ConvertFrom(parent);
	RequestEnv env;
	Net::WebServer::WebSessionUsage webSess(me->ParseRequestEnv(req, resp, env, true));

	Int32 id;
	Int32 cateId;
	if (req->GetQueryValueI32(CSTR("id"), id) &&
		req->GetQueryValueI32(CSTR("cateId"), cateId))
	{
		NotNullPtr<Text::String> s;
		UOSInt i;
		UOSInt j;
		UTF8Char sbuff[512];
		UTF8Char sbuff2[512];
		UTF8Char *sptr;
		UTF8Char *sptr2;
		IO::Path::FindFileSession *sess;
		IO::Path::PathType pt;
		Text::StringBuilderUTF8 sb;
		NotNullPtr<SpeciesInfo> species;
		NotNullPtr<GroupInfo> group;
		NotNullPtr<CategoryInfo> cate;
		BookSpInfo *bookSp;
		BookInfo *book;
		UserFileInfo *userFile;
		WebFileInfo *wfile;
		IO::ConfigFile *lang = me->env->LangGet(req);
		Data::DateTime dt;

		Sync::RWMutexUsage mutUsage;
		if (!me->env->SpeciesGet(mutUsage, id).SetTo(species))
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		Bool notAdmin = (env.user == 0 || env.user->userType != UserType::Admin);
		if (!me->env->GroupGet(mutUsage, species->groupId).SetTo(group) || group->cateId != cateId || (me->env->GroupIsAdmin(group) && notAdmin))
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		if (!me->env->CateGet(mutUsage, group->cateId).SetTo(cate) || ((cate->flags & 1) && notAdmin))
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST && env.user != 0 && env.user->userType == UserType::Admin)
		{
			req->ParseHTTPForm();
			NotNullPtr<Text::String> action;
			NotNullPtr<Text::String> s;
			Int32 userfileId;
			if (req->GetHTTPFormStr(CSTR("action")).SetTo(action))
			{
				if (action->Equals(UTF8STRC("pickall")))
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
				else if (action->Equals(UTF8STRC("picksel")))
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
						if (req->GetHTTPFormStr(sb.ToCString()).SetTo(s) && s->v[0] == '1')
						{
							env.pickObjs->SortedInsert(userfileId);
						}
						i++;
					}
				}
				else if (action->Equals(UTF8STRC("place")))
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
							if (req->GetHTTPFormStr(sb.ToCString()).SetTo(s) && s->v[0] == '1')
							{
								if (me->env->UserfileMove(mutUsage, userfileId, id, cateId))
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
				else if (action->Equals(UTF8STRC("placeall")))
				{
					if (env.pickObjType == POT_USERFILE)
					{
						i = 0;
						j = env.pickObjs->GetCount();
						while (i < j)
						{
							userfileId = env.pickObjs->GetItem(i);
							if (me->env->UserfileMove(mutUsage, userfileId, id, cateId))
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
				else if (action->Equals(UTF8STRC("placemerge")))
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
							if (req->GetHTTPFormStr(sb.ToCString()).SetTo(s) && s->v[0] == '1')
							{
								if (me->env->SpeciesMerge(mutUsage, speciesId, id, cateId))
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
				else if (action->Equals(UTF8STRC("setphoto")))
				{
					me->env->GroupSetPhotoSpecies(mutUsage, species->groupId, species->speciesId);
				}
				else if (action->Equals(UTF8STRC("bookspecies")))
				{
					NotNullPtr<Text::String> dispName;
					UInt32 bookAllowDup = 0;
					req->GetHTTPFormUInt32(CSTR("bookAllowDup"), bookAllowDup);
					if (req->GetHTTPFormStr(CSTR("speciesname")).SetTo(dispName) && dispName->leng > 0)
					{
						me->env->BookAddSpecies(mutUsage, species->speciesId, dispName, bookAllowDup != 0);
					}
				}
				else if (action->Equals(UTF8STRC("webfile")))
				{
					NotNullPtr<Text::String> srcURL;
					NotNullPtr<Text::String> imgURL;
					Optional<Text::String> location = req->GetHTTPFormStr(CSTR("location"));
					if (req->GetHTTPFormStr(CSTR("srcURL")).SetTo(srcURL) && req->GetHTTPFormStr(CSTR("imgURL")).SetTo(imgURL) && srcURL->leng > 0 && imgURL->leng > 0)
					{
						me->env->SpeciesAddWebfile(mutUsage, species->speciesId, imgURL->ToCString(), srcURL->ToCString(), OPTSTR_CSTR(location));
					}
				}
			}
		}

		IO::MemoryStream mstm;
		Text::UTF8Writer writer(mstm);

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
		s = Text::XML::ToNewHTMLBodyText(species->descript->v);
		writer.WriteStrC(s->v, s->leng);
		s->Release();
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
			writer.WriteStr(LangGetValue(lang, CSTR("Document")));
			writer.WriteLineC(UTF8STRC(":<br/>"));
			writer.WriteLineC(UTF8STRC("<table border=\"0\">"));
			i = 0;
			j = species->books.GetCount();
			while (i < j)
			{
				bookSp = species->books.GetItem(i);
				book = me->env->BookGet(mutUsage, bookSp->bookId);
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
					sb.AppendI32(Data::DateTimeUtil::DispYearI32(dt.GetYear()));
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
		me->WriteLocator(mutUsage, &writer, group, cate);
		writer.WriteLineC(UTF8STRC("</td></tr></table>"));
		if (env.user != 0 || me->env->GroupIsPublic(mutUsage, group->id))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<a href=\"map/index.html?species="));
			sb.AppendI32(id);
			sb.AppendC(UTF8STRC("\">"));
			sb.Append(LangGetValue(lang, CSTR("ShowMap")));
			sb.AppendC(UTF8STRC("</a><br/>"));
			writer.WriteLineC(sb.ToString(), sb.GetLength());
		}
		writer.WriteLineC(UTF8STRC("<hr/>"));
		if (env.user && env.user->userType == UserType::Admin && (book = me->env->BookGetSelected(mutUsage)) != 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<form name=\"bookspecies\" action=\"species.html?id="));
			sb.AppendI32(id);
			sb.AppendC(UTF8STRC("&amp;cateId="));
			sb.AppendI32(cateId);
			sb.AppendC(UTF8STRC("\" method=\"POST\"/>"));
			writer.WriteLineC(sb.ToString(), sb.GetLength());
			writer.WriteLineC(UTF8STRC("<input type=\"hidden\" name=\"action\" value=\"bookspecies\"/>"));
			writer.WriteStrC(UTF8STRC("Selected Book: "));
			s = Text::XML::ToNewHTMLBodyText(book->title->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
			writer.WriteStrC(UTF8STRC("<br/><input type=\"text\" name=\"speciesname\" value="));
			if (species->sciName->EndsWith(UTF8STRC(" male")))
			{
				sb.ClearStr();
				sb.Append(species->sciName);
				sb.RemoveChars(5);
				s = Text::XML::ToNewAttrText(sb.v);
			}
			else if (species->sciName->EndsWith(UTF8STRC(" female")))
			{
				sb.ClearStr();
				sb.Append(species->sciName);
				sb.RemoveChars(7);
				s = Text::XML::ToNewAttrText(sb.v);
			}
			else
			{
				s = Text::XML::ToNewAttrText(species->sciName->v);
			}
			writer.WriteStrC(s->v, s->leng);
			s->Release();
			writer.WriteStrC(UTF8STRC(" /><input type=\"checkbox\" name=\"bookAllowDup\" id=\"bookAllowDup\" value=\"1\"/><label for=\"bookAllowDup\">Allow Duplicate</label>"));
			writer.WriteStrC(UTF8STRC("<input type=\"submit\" /><br/>"));
			writer.WriteLineC(UTF8STRC("</form><hr/>"));
		}

		Data::ArrayListICaseString fileNameList;
		Data::ArrayListStringNN refURLList;
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
						fileNameList.SortedInsert(Text::String::New(sptr, (UOSInt)(sptr2 - sptr - 4)).Ptr());
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
			Text::UTF8Reader reader(fs);
			sb.ClearStr();
			while (reader.ReadLine(sb, 4096))
			{
				if (Text::StrSplitP(sarr, 4, sb, '\t') == 3)
				{
					if (refURLList.SortedIndexOfC(sarr[2].ToCString()) < 0)
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
					fileNameList.Add(Text::String::New(sptr, (UOSInt)(sptr2 - sptr)).Ptr());
				}
				sb.ClearStr();
			}
		}

		UOSInt colCount = env.scnWidth / GetPreviewSize();
		UOSInt colWidth = 100 / colCount;
		UInt32 currColumn;
		if (fileNameList.GetCount() > 0 || species->files.GetCount() > 0 || species->wfiles.GetCount() > 0)
		{
			currColumn = 0;
			if (env.user != 0 && env.user->userType == UserType::Admin)
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
			fileList.AddAll(species->files);
			if (env.user != 0)
			{
				UserFileDescComparator comparator(&env);
				Data::Sort::ArtificialQuickSort::Sort<UserFileInfo*>(&fileList, comparator);
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
				sb.AppendUOSInt(colWidth);
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
				sb.AppendUOSInt(GetPreviewSize());
				sb.AppendC(UTF8STRC("&height="));
				sb.AppendUOSInt(GetPreviewSize());
				sb.AppendC(UTF8STRC("&fileId="));
				sb.AppendI32(userFile->id);
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer.WriteStrC(s->v, s->leng);
				s->Release();
				writer.WriteStrC(UTF8STRC(" border=\"0\">"));
				writer.WriteStrC(UTF8STRC("</a><br/>"));
				dt.SetTicks(userFile->fileTimeTicks);
				dt.ToLocalTime();
				if (env.user != 0 && (env.user->userType == UserType::Admin || userFile->webuserId == env.user->id))
				{
					if (env.user->userType == UserType::Admin)
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
						if (userFile->location.SetTo(s))
						{
							writer.WriteStrC(UTF8STRC(" "));
							s = Text::XML::ToNewHTMLBodyText(s->v);
							writer.WriteStrC(s->v, s->leng);
							s->Release();
						}
					}
					if (userFile->descript.SetTo(s) && s->leng > 0)
					{
						writer.WriteStrC(UTF8STRC("<br/>"));
						s = Text::XML::ToNewHTMLBodyText(s->v);
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
					if (env.user->userType == UserType::Admin)
					{
						writer.WriteStrC(UTF8STRC("</label>"));
					}
					if (userFile->lat != 0 || userFile->lon != 0)
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("<br/>"));
						sb.AppendC(UTF8STRC("<a href=\"https://www.google.com/maps/place/"));
						sb.AppendDouble(userFile->lat);
						sb.AppendC(UTF8STRC(","));
						sb.AppendDouble(userFile->lon);
						sb.AppendC(UTF8STRC("/@"));
						sb.AppendDouble(userFile->lat);
						sb.AppendC(UTF8STRC(","));
						sb.AppendDouble(userFile->lon);
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
				sb.AppendUOSInt(colWidth);
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
				sb.AppendUOSInt(GetPreviewSize());
				sb.AppendC(UTF8STRC("&height="));
				sb.AppendUOSInt(GetPreviewSize());
				sb.AppendC(UTF8STRC("&fileWId="));
				sb.AppendI32(wfile->id);
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer.WriteStrC(s->v, s->leng);
				s->Release();
				writer.WriteStrC(UTF8STRC(" border=\"0\">"));
				writer.WriteStrC(UTF8STRC("</a>"));
				if (env.user && env.user->userType == UserType::Admin)
				{
					if (wfile->location->leng > 0)
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
				sb.AppendUOSInt(colWidth);
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
				sb.AppendUOSInt(GetPreviewSize());
				sb.AppendC(UTF8STRC("&height="));
				sb.AppendUOSInt(GetPreviewSize());
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
				sb.AppendUOSInt(colWidth);
				sb.AppendC(UTF8STRC("%\"></td>"));
				while (currColumn < colCount)
				{
					writer.WriteLineC(sb.ToString(), sb.GetLength());
					currColumn++;
				}
				writer.WriteLineC(UTF8STRC("</tr>"));
			}
			writer.WriteLineC(UTF8STRC("</table><hr/>"));
			if (env.user != 0 && env.user->userType == UserType::Admin)
			{
				writer.WriteLineC(UTF8STRC("</form>"));
			}
		}
		if (refURLList.GetCount() > 0)
		{
			writer.WriteLineC(UTF8STRC("Reference URL:<br/>"));
			NotNullPtr<Text::String> url;
			Data::ArrayIterator<NotNullPtr<Text::String>> it = refURLList.Iterator();
			while (it.HasNext())
			{
				url = it.Next();

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
			}
			writer.WriteLineC(UTF8STRC("<hr/>"));
		}
		if (env.user != 0 && env.user->userType == UserType::Admin)
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
			me->WritePickObjs(mutUsage, &writer, &env, sb.ToString(), true);
		}

		writer.WriteLineC(UTF8STRC("<br/>"));
		writer.WriteStrC(UTF8STRC("<a href=\"group.html?id="));
		sb.ClearStr();
		sb.AppendI32(species->groupId);
		sb.AppendC(UTF8STRC("&amp;cateId="));
		sb.AppendI32(group->cateId);
		writer.WriteStrC(sb.ToString(), sb.GetLength());
		writer.WriteStrC(UTF8STRC("\">"));
		writer.WriteStr(LangGetValue(lang, CSTR("Back")));
		writer.WriteStrC(UTF8STRC("</a>"));

		if (env.user != 0 && env.user->userType == UserType::Admin)
		{
			writer.WriteLineC(UTF8STRC("<br/>"));
			writer.WriteLineC(UTF8STRC("<hr/>"));
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<form name=\"webfile\" action=\"species.html?id="));
			sb.AppendI32(id);
			sb.AppendC(UTF8STRC("&amp;cateId="));
			sb.AppendI32(cateId);
			sb.AppendC(UTF8STRC("\" method=\"POST\"/>"));
			writer.WriteLineC(sb.ToString(), sb.GetLength());
			writer.WriteLineC(UTF8STRC("<input type=\"hidden\" name=\"action\" value=\"webfile\"/>"));
			writer.WriteLineC(UTF8STRC("Image URL: <input type=\"text\" name=\"imgURL\" value=\"\"/><br/>"));
			writer.WriteLineC(UTF8STRC("Source URL: <input type=\"text\" name=\"srcURL\" value=\"\"/><br/>"));
			writer.WriteLineC(UTF8STRC("Location: <input type=\"text\" name=\"location\" value=\"\"/><br/>"));
			writer.WriteLineC(UTF8STRC("<input type=\"submit\"/><br/>"));
			writer.WriteLineC(UTF8STRC("</form>"));
			writer.WriteLineC(UTF8STRC("<hr/>"));
			writer.WriteLineC(UTF8STRC("<input type=\"button\" value=\"Set Group Photo\" onclick=\"document.forms.userfiles.action.value='setphoto';document.forms.userfiles.submit();\"/>"));
		}

		me->WriteFooter(&writer);
		mutUsage.EndUse();
		ResponseMstm(req, resp, mstm, CSTR("text/html"));
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganWeb::OrganWebMainController::SvcSpeciesMod(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<Net::WebServer::WebController> parent)
{
	NotNullPtr<SSWR::OrganWeb::OrganWebMainController> me = NotNullPtr<SSWR::OrganWeb::OrganWebMainController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	if (env.user == 0 || env.user->userType != UserType::Admin)
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}

	Int32 id;
	Int32 cateId;
	Int32 spId = 0;
	if (req->GetQueryValueI32(CSTR("id"), id) &&
		req->GetQueryValueI32(CSTR("cateId"), cateId))
	{
		NotNullPtr<GroupInfo> group;
		Text::StringBuilderUTF8 sb;
		NotNullPtr<Text::String> s;
		IO::ConfigFile *lang = me->env->LangGet(req);

		Sync::RWMutexUsage mutUsage;
		if (!me->env->GroupGet(mutUsage, id).SetTo(group))
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		Text::StringBuilderUTF8 msg;
		NotNullPtr<Text::String> cname = Text::String::NewEmpty();
		NotNullPtr<Text::String> sname = cname;
		NotNullPtr<Text::String> ename = cname;
		NotNullPtr<Text::String> descr = cname;
		Bool canDelete = false;
		const UTF8Char *bookIgn = 0;
		Optional<SpeciesInfo> species = 0;
		NotNullPtr<SpeciesInfo> nnspecies;
		if (req->GetQueryValueI32(CSTR("spId"), spId))
		{
			species = me->env->SpeciesGet(mutUsage, spId);
			if (species.SetTo(nnspecies))
			{
				cname = nnspecies->chiName;
				sname = nnspecies->sciName;
				ename = nnspecies->engName;
				descr = nnspecies->descript;
				canDelete = (nnspecies->files.GetCount() == 0 && nnspecies->books.GetCount() == 0 && nnspecies->wfiles.GetCount() == 0);
			}
		}
		if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
		{
			req->ParseHTTPForm();
			NotNullPtr<Text::String> task;
			bookIgn = STR_PTR(req->GetQueryValue(CSTR("bookIgn")).OrNull());
			if (req->GetHTTPFormStr(CSTR("task")).SetTo(task) &&
				req->GetHTTPFormStr(CSTR("cname")).SetTo(cname) &&
				req->GetHTTPFormStr(CSTR("sname")).SetTo(sname) &&
				req->GetHTTPFormStr(CSTR("ename")).SetTo(ename) &&
				req->GetHTTPFormStr(CSTR("descr")).SetTo(descr) && cname->v[0] != 0 && sname->v[0] != 0)
			{
				if (task->Equals(UTF8STRC("new")))
				{
					sb.ClearStr();
					if (!me->env->SpeciesGetByName(mutUsage, sname).IsNull())
					{
						msg.AppendC(UTF8STRC("Species already exist"));
					}
					else if ((bookIgn == 0 || bookIgn[0] != '1') && me->env->SpeciesBookIsExist(mutUsage, sname->ToCString(), sb))
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
						Int32 spId = me->env->SpeciesAdd(mutUsage, ename->ToCString(), cname->ToCString(), sname->ToCString(), id, descr->ToCString(), sb.ToCString(), CSTR(""), cateId);
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
				else if (task->Equals(UTF8STRC("modify")) && species.SetTo(nnspecies))
				{
					Bool nameChg = !sname->Equals(nnspecies->sciName);
					sb.ClearStr();
					if (nameChg && !me->env->SpeciesGetByName(mutUsage, sname).IsNull())
					{
						msg.AppendC(UTF8STRC("Species already exist"));
					}
					else if (nameChg && (bookIgn == 0 || bookIgn[0] != '1') && me->env->SpeciesBookIsExist(mutUsage, sname->ToCString(), sb))
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
						if (me->env->SpeciesModify(mutUsage, spId, ename->ToCString(), cname->ToCString(), Text::String::OrEmpty(sname)->ToCString(), descr->ToCString(), sb.ToCString()))
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
				else if (task->Equals(UTF8STRC("delete")) && species.SetTo(nnspecies) && nnspecies->files.GetCount() == 0 && nnspecies->books.GetCount() == 0 && nnspecies->wfiles.GetCount() == 0)
				{
					Int32 groupId = nnspecies->groupId;
					if (me->env->SpeciesDelete(mutUsage, nnspecies->speciesId))
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
		Text::UTF8Writer writer(mstm);

		sb.ClearStr();
		if (!species.IsNull())
		{
			sb.AppendC(UTF8STRC("Modify Species in "));
		}
		else
		{
			sb.AppendC(UTF8STRC("New Species in "));
		}
		sb.Append(group->chiName);
		sb.AppendC(UTF8STRC(" "));
		sb.Append(group->engName);
		if (species.SetTo(nnspecies))
		{
			sb.AppendC(UTF8STRC(" of "));
			sb.Append(nnspecies->chiName);
			sb.AppendC(UTF8STRC(" "));
			sb.Append(nnspecies->sciName);
			if (nnspecies->engName->leng > 0)
			{
				sb.AppendC(UTF8STRC(" "));
				sb.Append(nnspecies->engName);
			}
		}
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
		if (cname->leng > 0)
		{
			writer.WriteStrC(UTF8STRC(" value="));
			s = Text::XML::ToNewAttrText(cname->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
		}
		writer.WriteLineC(UTF8STRC("/></td></tr>"));
		writer.WriteStrC(UTF8STRC("<tr><td>Science Name</td><td><input type=\"text\" name=\"sname\""));
		if (sname->leng > 0)
		{
			writer.WriteStrC(UTF8STRC(" value="));
			s = Text::XML::ToNewAttrText(sname->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
		}
		writer.WriteLineC(UTF8STRC("/></td></tr>"));
		writer.WriteStrC(UTF8STRC("<tr><td>English Name</td><td><input type=\"text\" name=\"ename\""));
		if (ename->leng > 0)
		{
			writer.WriteStrC(UTF8STRC(" value="));
			s = Text::XML::ToNewAttrText(ename->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
		}
		writer.WriteLineC(UTF8STRC("/></td></tr>"));
		writer.WriteStrC(UTF8STRC("<tr><td>Description</td><td><textarea name=\"descr\" rows=\"4\" cols=\"40\">"));
		if (descr->leng > 0)
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
		writer.WriteStr(LangGetValue(lang, CSTR("Back")));
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
		ResponseMstm(req, resp, mstm, CSTR("text/html"));
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganWeb::OrganWebMainController::SvcList(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<Net::WebServer::WebController> parent)
{
	NotNullPtr<SSWR::OrganWeb::OrganWebMainController> me = NotNullPtr<SSWR::OrganWeb::OrganWebMainController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	Int32 id;
	UInt32 page = 0;
	Int32 cateId;
	req->GetQueryValueU32(CSTR("page"), page);
	if (req->GetQueryValueI32(CSTR("id"), id) &&
		req->GetQueryValueI32(CSTR("cateId"), cateId))
	{
		Bool imageOnly = subReq.Equals(UTF8STRC("/listimage.html"));
		NotNullPtr<Text::String> s;
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		NotNullPtr<GroupInfo> group;
		NotNullPtr<CategoryInfo> cate;
		IO::ConfigFile *lang = me->env->LangGet(req);
		Sync::RWMutexUsage mutUsage;
		Bool notAdmin = (env.user == 0 || env.user->userType != UserType::Admin);
		if (!me->env->GroupGet(mutUsage, id).SetTo(group) || group->cateId != cateId)
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		if (!me->env->CateGet(mutUsage, group->cateId).SetTo(cate) || ((cate->flags & 1) && notAdmin))
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
		Text::UTF8Writer writer(mstm);

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

		me->WriteLocator(mutUsage, &writer, group, cate);
		writer.WriteLineC(UTF8STRC("<br/>"));
		s = Text::XML::ToNewHTMLBodyText(group->descript->v);
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("<br/>"));
		writer.WriteLineC(UTF8STRC("<hr/>"));

		Data::StringMap<SpeciesInfo*> spMap;
		me->env->GetGroupSpecies(mutUsage, group, &spMap, env.user);
		Data::ArrayList<SpeciesInfo*> speciesTmp;
		NotNullPtr<const Data::ReadingList<SpeciesInfo*>> spList;
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
			spList = speciesTmp;
		}
		UOSInt perPage = GetPerPage(env.isMobile);
		i = page * perPage;
		j = i + perPage;
		if (j > spList->GetCount())
		{
			j = spList->GetCount();
		}
		Data::ArrayListNN<SpeciesInfo> species;
		NotNullPtr<SpeciesInfo> sp;
		while (i < j)
		{
			if (sp.Set(spList->GetItem(i)))
			{
				species.Add(sp);
			}
			i++;
		}
		me->WriteSpeciesTable(mutUsage, &writer, species, env.scnWidth, group->cateId, false, (env.user && env.user->userType == UserType::Admin));
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
				sb.Append(LangGetValue(lang, CSTR("Previous")));
				sb.AppendUOSInt(perPage);
				sb.Append(LangGetValue(lang, CSTR("Items")));
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
				sb.Append(LangGetValue(lang, CSTR("Next")));
				sb.AppendUOSInt(perPage);
				sb.Append(LangGetValue(lang, CSTR("Items")));
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
				sb.Append(LangGetValue(lang, CSTR("Previous")));
				sb.AppendUOSInt(perPage);
				sb.Append(LangGetValue(lang, CSTR("Items")));
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
				sb.Append(LangGetValue(lang, CSTR("Next")));
				sb.AppendUOSInt(perPage);
				sb.Append(LangGetValue(lang, CSTR("Items")));
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
		writer.WriteStr(LangGetValue(lang, CSTR("Back")));
		writer.WriteStrC(UTF8STRC("</a>"));

		me->WriteFooter(&writer);
		mutUsage.EndUse();
		ResponseMstm(req, resp, mstm, CSTR("text/html"));
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganWeb::OrganWebMainController::SvcPhotoDetail(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<Net::WebServer::WebController> parent)
{
	NotNullPtr<SSWR::OrganWeb::OrganWebMainController> me = NotNullPtr<SSWR::OrganWeb::OrganWebMainController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	Int32 id;
	Int32 cateId;
	if (req->GetQueryValueI32(CSTR("id"), id) &&
		req->GetQueryValueI32(CSTR("cateId"), cateId))
	{
		UTF8Char fileName[512];
		UTF8Char *fileNameEnd;
		Int32 fileId;
		NotNullPtr<Text::String> s;
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
		NotNullPtr<SpeciesInfo> species;
		NotNullPtr<GroupInfo> group;
		NotNullPtr<CategoryInfo> cate;
		Text::PString sarr[4];
		WebFileInfo *wfile;
		IO::ConfigFile *lang = me->env->LangGet(req);

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

		sptr = cate->srcDir->ConcatTo(sbuff);
		if (IO::Path::PATH_SEPERATOR != '\\')
		{
			Text::StrReplace(sbuff, '\\', IO::Path::PATH_SEPERATOR);
		}
		sptr = species->dirName->ConcatTo(sptr);
		*sptr++ = IO::Path::PATH_SEPERATOR;

		if (req->GetQueryValueI32(CSTR("fileId"), fileId))
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
				if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST && env.user && (env.user->userType == UserType::Admin || env.user->id == userFile->webuserId))
				{
					req->ParseHTTPForm();
					NotNullPtr<Text::String> action;
					if (req->GetHTTPFormStr(CSTR("action")).SetTo(action))
					{
						if (action->Equals(UTF8STRC("setdefault")) && env.user->userType == UserType::Admin)
						{
							me->env->SpeciesSetPhotoId(mutUsage, id, fileId);
						}
						else if (action->Equals(UTF8STRC("setname")))
						{
							NotNullPtr<Text::String> desc;
							if (req->GetHTTPFormStr(CSTR("descr")).SetTo(desc))
							{
								me->env->UserfileUpdateDesc(mutUsage, fileId, desc->ToCString());
							}
						}
						else if (action->Equals(UTF8STRC("rotate")))
						{
							me->env->UserfileUpdateRotType(mutUsage, fileId, (userFile->rotType + 1) & 3);
						}
					}
				}

				IO::MemoryStream mstm;
				Text::UTF8Writer writer(mstm);

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
									fileNameList.SortedInsert(Text::String::New(sptr, (UOSInt)(sptr2 - sptr - 4)).Ptr());
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
							Text::UTF8Reader reader(fs);
							sb.ClearStr();
							found = false;
							while (reader.ReadLine(sb, 4096))
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
						sb.ClearStr();
						sb.AppendU64(fileSize);
						sb.AppendC(UTF8STRC(" bytes"));
						Media::IMediaSource *msrc = mediaFile->GetStream(0, 0);
						Data::Duration stmTime;
						if (msrc)
						{
							stmTime = msrc->GetStreamTime();
							sb.AppendC(UTF8STRC(", Length: "));
							sb.AppendDur(stmTime);

							if (msrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
							{
								Media::IAudioSource *asrc = (Media::IAudioSource*)msrc;
								Media::AudioFormat format;
								asrc->GetFormat(format);
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
						writer.WriteStr(LangGetValue(lang, CSTR("PhotoSpec")));
						writer.WriteStrC(UTF8STRC("</b> "));
						writer.WriteStrC(sb.ToString(), sb.GetLength());
						writer.WriteStrC(UTF8STRC("<br/>"));
						DEL_CLASS(mediaFile);
					}
					if (userFile->captureTimeTicks != 0)
					{
						writer.WriteStrC(UTF8STRC("<b>"));
						writer.WriteStr(LangGetValue(lang, CSTR("PhotoDate")));
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
					Media::PhotoInfo info(fd);
					if (info.HasInfo())
					{
						sb.ClearStr();
						writer.WriteStrC(UTF8STRC("<b>"));
						writer.WriteStr(LangGetValue(lang, CSTR("PhotoSpec")));
						writer.WriteStrC(UTF8STRC("</b> "));
						info.ToString(sb);
						writer.WriteStrC(sb.ToString(), sb.GetLength());
						writer.WriteStrC(UTF8STRC("<br/>"));

						writer.WriteStrC(UTF8STRC("<b>"));
						writer.WriteStr(LangGetValue(lang, CSTR("PhotoDate")));
						writer.WriteStrC(UTF8STRC("</b> "));
						sptr2 = Data::Timestamp(userFile->captureTimeTicks, Data::DateTimeUtil::GetLocalTzQhr()).ToString(sbuff2, "yyyy-MM-dd HH:mm:ss zzzz");
						writer.WriteStrC(sbuff2, (UOSInt)(sptr2 - sbuff2));
						writer.WriteStrC(UTF8STRC("<br/>"));
					}
				}

				if (env.user && (userFile->webuserId == env.user->id || env.user->userType == UserType::Admin))
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
					sb.Append(LangGetValue(lang, CSTR("Download")));
					sb.AppendC(UTF8STRC("</a>"));
					writer.WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("<form name=\"photo\" method=\"POST\" action=\"photodetail.html?id="));
					sb.AppendI32(id);
					sb.AppendC(UTF8STRC("&amp;cateId="));
					sb.AppendI32(cateId);
					sb.AppendC(UTF8STRC("&amp;fileId="));
					sb.AppendI32(fileId);
					sb.AppendC(UTF8STRC("\"><input type=\"hidden\" name=\"action\" value=\"setname\"/>"));
					if (env.user->userType == UserType::Admin)
					{
						sb.AppendC(UTF8STRC("<input type=\"button\" value=\"Set As Species Photo\" onclick=\"document.forms.photo.action.value='setdefault';document.forms.photo.submit();\"/>"));
					}
					sb.AppendC(UTF8STRC("<input type=\"button\" value=\"Rotate\" onclick=\"document.forms.photo.action.value='rotate';document.forms.photo.submit();\"/>"));
					sb.AppendC(UTF8STRC("<br/>"));
					writer.WriteLineC(sb.ToString(), sb.GetLength());
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Description<input type=\"text\" name=\"descr\""));
					if (userFile->descript.SetTo(s))
					{
						sb.AppendC(UTF8STRC(" value="));
						s = Text::XML::ToNewAttrText(s->v);
						sb.Append(s);
						s->Release();
					}
					sb.AppendC(UTF8STRC("/><input type=\"button\" value=\"Update\" onclick=\"document.forms.photo.action.value='setname';document.forms.photo.submit();\"/>"));
					sb.AppendC(UTF8STRC("</form>"));
					writer.WriteLineC(sb.ToString(), sb.GetLength());
				}

				if (env.user && env.user->userType == UserType::Admin)
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
				sb.Append(LangGetValue(lang, CSTR("Back")));
				sb.AppendC(UTF8STRC("</a>"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());
				writer.WriteLineC(UTF8STRC("</td></tr>"));
				writer.WriteLineC(UTF8STRC("</table>"));
				writer.WriteLineC(UTF8STRC("</center>"));

				me->WriteFooter(&writer);
				mutUsage.EndUse();
				ResponseMstm(req, resp, mstm, CSTR("text/html"));
				return true;
			}
			else
			{
				resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
				mutUsage.EndUse();
				return true;
			}
		}
		else if (req->GetQueryValueI32(CSTR("fileWId"), fileId))
		{
			wfile = species->wfiles.Get(fileId);
			if (wfile)
			{
				if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST && env.user && env.user->userType == UserType::Admin)
				{
					req->ParseHTTPForm();
					NotNullPtr<Text::String> action;
					if (req->GetHTTPFormStr(CSTR("action")).SetTo(action) && action->Equals(UTF8STRC("setdefault")))
					{
						me->env->SpeciesSetPhotoWId(mutUsage, id, fileId, true);
					}
				}
				IO::MemoryStream mstm;
				Text::UTF8Writer writer(mstm);

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
									fileNameList.SortedInsert(Text::String::New(sptr, (UOSInt)(sptr2 - sptr - 4)).Ptr());
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
							Text::UTF8Reader reader(fs);
							sb.ClearStr();
							found = false;
							while (reader.ReadLine(sb, 4096))
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

				if (env.user && env.user->userType == UserType::Admin)
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("<form name=\"photo\" method=\"POST\" action=\"photodetail.html?id="));
					sb.AppendI32(id);
					sb.AppendC(UTF8STRC("&amp;cateId="));
					sb.AppendI32(cateId);
					sb.AppendC(UTF8STRC("&amp;fileWId="));
					sb.AppendI32(fileId);
					sb.AppendC(UTF8STRC("\"><input type=\"hidden\" name=\"action\" value=\"setname\"/>"));
					sb.AppendC(UTF8STRC("<input type=\"button\" value=\"Set As Species Photo\" onclick=\"document.forms.photo.action.value='setdefault';document.forms.photo.submit();\"/>"));
					sb.AppendC(UTF8STRC("</form>"));
					writer.WriteLineC(sb.ToString(), sb.GetLength());
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
				sb.AppendUTF8Char('>');
				s->Release();
				sb.Append(LangGetValue(lang, CSTR("Back")));
				sb.AppendC(UTF8STRC("</a>"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());
				writer.WriteLineC(UTF8STRC("</td></tr>"));
				writer.WriteLineC(UTF8STRC("</table>"));
				writer.WriteLineC(UTF8STRC("</center>"));

				me->WriteFooter(&writer);
				mutUsage.EndUse();
				ResponseMstm(req, resp, mstm, CSTR("text/html"));
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
					Text::UTF8Reader reader(fs);
					sb.ClearStr();
					found = false;
					foundNext = false;
					while (reader.ReadLine(sb, 4096))
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
									srcURL = Text::String::New(sarr[2].v, sarr[2].leng).Ptr();
									imgURL = Text::String::New(sarr[1].v, sarr[1].leng).Ptr();
								}
							}
						}
						sb.ClearStr();
					}
				}

				if (found)
				{
					IO::MemoryStream mstm;
					Text::UTF8Writer writer(mstm);

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
					sb.Append(LangGetValue(lang, CSTR("Back")));
					sb.AppendC(UTF8STRC("</a>"));
					writer.WriteLineC(sb.ToString(), sb.GetLength());
					writer.WriteLineC(UTF8STRC("</td></tr>"));
					writer.WriteLineC(UTF8STRC("</table>"));
					writer.WriteLineC(UTF8STRC("</center>"));

					me->WriteFooter(&writer);
					mutUsage.EndUse();
					ResponseMstm(req, resp, mstm, CSTR("text/html"));

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
								fileNameList.SortedInsert(Text::String::New(sptr, (UOSInt)(sptr2 - sptr - 4)).Ptr());
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
					Text::UTF8Writer writer(mstm);

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
							Text::UTF8Reader reader(fs);
							sb.ClearStr();
							found = false;
							while (reader.ReadLine(sb, 4096))
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
						Media::PhotoInfo info(fd);
						if (info.HasInfo())
						{
							Data::DateTime dt;
							sb.ClearStr();
							writer.WriteStrC(UTF8STRC("<b>"));
							writer.WriteStr(LangGetValue(lang, CSTR("PhotoSpec")));
							writer.WriteStrC(UTF8STRC("</b> "));
							info.ToString(sb);
							writer.WriteStrC(sb.ToString(), sb.GetLength());
							writer.WriteStrC(UTF8STRC("<br/>"));
							if (info.GetPhotoDate(dt))
							{
								writer.WriteStrC(UTF8STRC("<b>"));
								writer.WriteStr(LangGetValue(lang, CSTR("PhotoDate")));
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
					sb.Append(LangGetValue(lang, CSTR("Back")));
					sb.AppendC(UTF8STRC("</a>"));
					writer.WriteLineC(sb.ToString(), sb.GetLength());
					writer.WriteLineC(UTF8STRC("</td></tr>"));
					writer.WriteLineC(UTF8STRC("</table>"));
					writer.WriteLineC(UTF8STRC("</center>"));

					me->WriteFooter(&writer);
					mutUsage.EndUse();
					ResponseMstm(req, resp, mstm, CSTR("text/html"));

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

Bool __stdcall SSWR::OrganWeb::OrganWebMainController::SvcSearchInside(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<Net::WebServer::WebController> parent)
{
	NotNullPtr<SSWR::OrganWeb::OrganWebMainController> me = NotNullPtr<SSWR::OrganWeb::OrganWebMainController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	Int32 id;
	Int32 cateId;
	NotNullPtr<Text::String> searchStr;
	req->ParseHTTPForm();
	if (req->GetQueryValueI32(CSTR("id"), id) &&
		req->GetQueryValueI32(CSTR("cateId"), cateId) &&
		req->GetHTTPFormStr(CSTR("searchStr")).SetTo(searchStr))
	{
		NotNullPtr<Text::String> s;
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		NotNullPtr<GroupInfo> group;
		NotNullPtr<CategoryInfo> cate;
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		IO::ConfigFile *lang = me->env->LangGet(req);
		Sync::RWMutexUsage mutUsage;
		Bool notAdmin = (env.user == 0 || env.user->userType != UserType::Admin);
		if (!me->env->GroupGet(mutUsage, id).SetTo(group) || group->cateId != cateId || (me->env->GroupIsAdmin(group) && notAdmin))
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		if (!me->env->CateGet(mutUsage, group->cateId).SetTo(cate) || ((cate->flags & 1) && notAdmin))
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		IO::MemoryStream mstm;
		Text::UTF8Writer writer(mstm);

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

		me->WriteLocator(mutUsage, &writer, group, cate);
		writer.WriteLineC(UTF8STRC("<br/>"));
		s = Text::XML::ToNewHTMLBodyText(group->descript->v);
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("<br/>"));
		writer.WriteLineC(UTF8STRC("<hr/>"));

		Data::ArrayListDbl speciesIndice;
		Data::ArrayListNN<SpeciesInfo> speciesObjs;
		Data::ArrayListDbl groupIndice;
		Data::ArrayListNN<GroupInfo> groupObjs;
		sb.ClearStr();
		sb.Append(searchStr);
		sb.Trim();
		writer.WriteStrC(UTF8STRC("Result for \""));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("\"<br/>"));
		me->env->SearchInGroup(mutUsage, group, sb.ToString(), sb.GetLength(), speciesIndice, speciesObjs, groupIndice, groupObjs, env.user);

		Bool found = false;

		if (speciesObjs.GetCount() > 0)
		{
			Data::ArrayListNN<SpeciesInfo> speciesList;
			NotNullPtr<SpeciesInfo> sp;
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
				if (speciesObjs.GetItem(i).SetTo(sp))
					speciesList.Add(sp);
			}
			me->WriteSpeciesTable(mutUsage, &writer, speciesList, env.scnWidth, group->cateId, false, (env.user && env.user->userType == UserType::Admin));
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
			Data::ArrayListNN<GroupInfo> groupList;
			NotNullPtr<GroupInfo> group;
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
				if (groupObjs.GetItem(i).SetTo(group))
					groupList.Add(group);
			}
			me->WriteGroupTable(mutUsage, &writer, groupList, env.scnWidth, false, env.user != 0 && env.user->userType == UserType::Admin);
			if (groupList.GetLast().SetTo(group))
			{
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
		writer.WriteStr(LangGetValue(lang, CSTR("Back")));
		writer.WriteStrC(UTF8STRC("</a>"));

		me->WriteFooter(&writer);
		mutUsage.EndUse();
		ResponseMstm(req, resp, mstm, CSTR("text/html"));
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganWeb::OrganWebMainController::SvcSearchInsideMoreS(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<Net::WebServer::WebController> parent)
{
	NotNullPtr<SSWR::OrganWeb::OrganWebMainController> me = NotNullPtr<SSWR::OrganWeb::OrganWebMainController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	Int32 id;
	UInt32 pageNo;
	Int32 cateId;
	NotNullPtr<Text::String> searchStr;
	if (req->GetQueryValueI32(CSTR("id"), id) &&
		req->GetQueryValueI32(CSTR("cateId"), cateId) &&
		req->GetQueryValueU32(CSTR("pageNo"), pageNo) &&
		req->GetQueryValue(CSTR("searchStr")).SetTo(searchStr))
	{
		NotNullPtr<Text::String> s;
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		NotNullPtr<GroupInfo> group;
		NotNullPtr<CategoryInfo> cate;
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		IO::ConfigFile *lang = me->env->LangGet(req);
		Sync::RWMutexUsage mutUsage;
		Bool notAdmin = (env.user == 0 || env.user->userType != UserType::Admin);
		if (!me->env->GroupGet(mutUsage, id).SetTo(group) || group->cateId != cateId || (me->env->GroupIsAdmin(group) && notAdmin))
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		if (!me->env->CateGet(mutUsage, group->cateId).SetTo(cate) || ((cate->flags & 1) && notAdmin))
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		IO::MemoryStream mstm;
		Text::UTF8Writer writer(mstm);

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

		me->WriteLocator(mutUsage, &writer, group, cate);
		writer.WriteLineC(UTF8STRC("<br/>"));
		s = Text::XML::ToNewHTMLBodyText(group->descript->v);
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("<br/>"));
		writer.WriteLineC(UTF8STRC("<hr/>"));

		Data::ArrayListDbl speciesIndice;
		Data::ArrayListNN<SpeciesInfo> speciesObjs;
		Data::ArrayListDbl groupIndice;
		Data::ArrayListNN<GroupInfo> groupObjs;
		sb.ClearStr();
		sb.Append(searchStr);
		sb.Trim();
		writer.WriteStrC(UTF8STRC("Result for \""));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("\"<br/>"));
		me->env->SearchInGroup(mutUsage, group, sb.ToString(), sb.GetLength(), speciesIndice, speciesObjs, groupIndice, groupObjs, env.user);

		Bool found = false;

		if (speciesObjs.GetCount() > pageNo * 50)
		{
			Data::ArrayListNN<SpeciesInfo> speciesList;
			NotNullPtr<SpeciesInfo> sp;
			j = speciesObjs.GetCount() - pageNo * 50;
			i = j - 50;
			if (j < 50)
			{
				i = 0;
			}
			while (j-- > i)
			{
				if (speciesObjs.GetItem(j).SetTo(sp))
					speciesList.Add(sp);
			}
			me->WriteSpeciesTable(mutUsage, &writer, speciesList, env.scnWidth, group->cateId, false, (env.user && env.user->userType == UserType::Admin));
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
		writer.WriteStr(LangGetValue(lang, CSTR("Back")));
		writer.WriteStrC(UTF8STRC("</a>"));

		me->WriteFooter(&writer);
		mutUsage.EndUse();
		ResponseMstm(req, resp, mstm, CSTR("text/html"));
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganWeb::OrganWebMainController::SvcSearchInsideMoreG(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<Net::WebServer::WebController> parent)
{
	NotNullPtr<SSWR::OrganWeb::OrganWebMainController> me = NotNullPtr<SSWR::OrganWeb::OrganWebMainController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	Int32 id;
	UInt32 pageNo;
	Int32 cateId;
	NotNullPtr<Text::String> searchStr;
	if (req->GetQueryValueI32(CSTR("id"), id) &&
		req->GetQueryValueI32(CSTR("cateId"), cateId) &&
		req->GetQueryValueU32(CSTR("pageNo"), pageNo) &&
		req->GetQueryValue(CSTR("searchStr")).SetTo(searchStr))
	{
		NotNullPtr<Text::String> s;
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		NotNullPtr<GroupInfo> group;
		NotNullPtr<CategoryInfo> cate;
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		IO::ConfigFile *lang = me->env->LangGet(req);
		Sync::RWMutexUsage mutUsage;
		Bool notAdmin = (env.user == 0 || env.user->userType != UserType::Admin);
		if (!me->env->GroupGet(mutUsage, id).SetTo(group) || group->cateId != cateId || (me->env->GroupIsAdmin(group) && notAdmin))
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		if (!me->env->CateGet(mutUsage, group->cateId).SetTo(cate) || ((cate->flags & 1) && notAdmin))
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		IO::MemoryStream mstm;
		Text::UTF8Writer writer(mstm);

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

		me->WriteLocator(mutUsage, &writer, group, cate);
		writer.WriteLineC(UTF8STRC("<br/>"));
		s = Text::XML::ToNewHTMLBodyText(group->descript->v);
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("<br/>"));
		writer.WriteLineC(UTF8STRC("<hr/>"));

		Data::ArrayListDbl speciesIndice;
		Data::ArrayListNN<SpeciesInfo> speciesObjs;
		Data::ArrayListDbl groupIndice;
		Data::ArrayListNN<GroupInfo> groupObjs;
		sb.ClearStr();
		sb.Append(searchStr);
		sb.Trim();
		writer.WriteStrC(UTF8STRC("Result for \""));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("\"<br/>"));
		me->env->SearchInGroup(mutUsage, group, sb.ToString(), sb.GetLength(), speciesIndice, speciesObjs, groupIndice, groupObjs, env.user);

		Bool found = false;

		if (groupObjs.GetCount() > 0)
		{
			Data::ArrayListNN<GroupInfo> groupList;
			NotNullPtr<GroupInfo> group;
			j = groupObjs.GetCount() - pageNo * 50;
			i = j - 50;
			if (i < 0)
			{
				i = 0;
			}
			while (j-- > i)
			{
				if (groupObjs.GetItem(j).SetTo(group))
					groupList.Add(group);
			}
			me->WriteGroupTable(mutUsage, &writer, groupList, env.scnWidth, false, env.user && env.user->userType == UserType::Admin);
			if (groupList.GetLast().SetTo(group))
			{
				if (pageNo > 0)
				{
					sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, searchStr->v);
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
					sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, searchStr->v);
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
		writer.WriteStr(LangGetValue(lang, CSTR("Back")));
		writer.WriteStrC(UTF8STRC("</a>"));

		me->WriteFooter(&writer);
		mutUsage.EndUse();
		ResponseMstm(req, resp, mstm, CSTR("text/html"));
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganWeb::OrganWebMainController::SvcLogout(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<Net::WebServer::WebController> parent)
{
	NotNullPtr<SSWR::OrganWeb::OrganWebMainController> me = NotNullPtr<SSWR::OrganWeb::OrganWebMainController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	me->sessMgr->DeleteSession(req, resp);
	resp->RedirectURL(req, CSTR("/"), 0);
	return true;
}

SSWR::OrganWeb::OrganWebMainController::OrganWebMainController(Net::WebServer::MemoryWebSessionManager *sessMgr, OrganWebEnv *env, UInt32 scnSize) : OrganWebController(sessMgr, env, scnSize)
{
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
	this->AddService(CSTR("/searchinside.html"), Net::WebUtil::RequestMethod::HTTP_POST, SvcSearchInside);
	this->AddService(CSTR("/searchinsidemores.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcSearchInsideMoreS);
	this->AddService(CSTR("/searchinsidemoreg.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcSearchInsideMoreG);
	this->AddService(CSTR("/logout"), Net::WebUtil::RequestMethod::HTTP_GET, SvcLogout);
}

SSWR::OrganWeb::OrganWebMainController::~OrganWebMainController()
{
}
