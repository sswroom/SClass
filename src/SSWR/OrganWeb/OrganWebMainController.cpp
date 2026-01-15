#include "Stdafx.h"
#include "Data/ArrayListICaseStringNN.h"
#include "Data/ByteBuffer.h"
#include "Data/StringMapNN.hpp"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Exporter/GUIJPGExporter.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Media/AudioSource.h"
#include "Media/ICCProfile.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "Media/MediaFile.h"
#include "Media/PhotoInfo.h"
#include "Net/MIME.h"
#include "SSWR/OrganWeb/OrganWebEnv.h"
#include "SSWR/OrganWeb/OrganWebMainController.h"
#include "SSWR/OrganWeb/OrganWebSession.h"
#include "Text/JSText.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "Text/XML.h"

#include <stdio.h>

Bool __stdcall SSWR::OrganWeb::OrganWebMainController::SvcGroup(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebMainController> me = NN<SSWR::OrganWeb::OrganWebMainController>::ConvertFrom(parent);
	RequestEnv env;
	OrganWebSession webSess(me->ParseRequestEnv(req, resp, env, true));

	NN<Data::ArrayListInt32> pickObjs;
	Int32 id;
	Int32 cateId;
	if (req->GetQueryValueI32(CSTR("id"), id) &&
		req->GetQueryValueI32(CSTR("cateId"), cateId))
	{
		NN<Text::String> s;
		UIntOS i;
		UIntOS j;
		Text::StringBuilderUTF8 sb;
		NN<GroupInfo> group;
		NN<CategoryInfo> cate;
		Optional<IO::ConfigFile> lang = me->env->LangGet(req);
		NN<WebUserInfo> user;
		Sync::RWMutexUsage mutUsage;
		Bool notAdmin = (!env.user.SetTo(user) || user->userType != UserType::Admin);
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

		if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST && env.user.SetTo(user) && user->userType == UserType::Admin)
		{
			req->ParseHTTPForm();
			NN<Text::String> action;
			Int32 itemId;
			if (req->GetHTTPFormStr(CSTR("action")).SetTo(action))
			{
				if (action->Equals(UTF8STRC("pickall")) && env.pickObjs.SetTo(pickObjs))
				{
					if (group->groups.GetCount() > 0)
					{
						env.pickObjType = POT_GROUP;
						webSess.SetPickObjType(env.pickObjType);
						pickObjs->Clear();
						i = 0;
						j = group->groups.GetCount();
						while (i < j)
						{
							pickObjs->SortedInsert(group->groups.GetItemNoCheck(i)->id);
							i++;
						}
					}
					else if (group->species.GetCount() > 0)
					{
						env.pickObjType = POT_SPECIES;
						webSess.SetPickObjType(env.pickObjType);
						pickObjs->Clear();
						i = 0;
						j = group->species.GetCount();
						while (i < j)
						{
							pickObjs->SortedInsert(group->species.GetItemNoCheck(i)->speciesId);
							i++;
						}
					}
				}
				else if (action->Equals(UTF8STRC("picksel")) && env.pickObjs.SetTo(pickObjs))
				{
					if (group->groups.GetCount() > 0)
					{
						env.pickObjType = POT_GROUP;
						webSess.SetPickObjType(env.pickObjType);
						pickObjs->Clear();
						i = 0;
						j = group->groups.GetCount();
						while (i < j)
						{
							itemId = group->groups.GetItemNoCheck(i)->id;
							sb.ClearStr();
							sb.AppendC(UTF8STRC("group"));
							sb.AppendI32(itemId);
							if (req->GetHTTPFormStr(sb.ToCString()).SetTo(s) && s->v[0] == '1')
							{
								pickObjs->SortedInsert(itemId);
							}
							i++;
						}
					}
					else if (group->species.GetCount() > 0)
					{
						env.pickObjType = POT_SPECIES;
						webSess.SetPickObjType(env.pickObjType);
						pickObjs->Clear();
						i = 0;
						j = group->species.GetCount();
						while (i < j)
						{
							itemId = group->species.GetItemNoCheck(i)->speciesId;
							sb.ClearStr();
							sb.AppendC(UTF8STRC("species"));
							sb.AppendI32(itemId);
							if (req->GetHTTPFormStr(sb.ToCString()).SetTo(s) && s->v[0] == '1')
							{
								pickObjs->SortedInsert(itemId);
							}
							i++;
						}
					}
				}
				else if (action->Equals(UTF8STRC("place")) && env.pickObjs.SetTo(pickObjs))
				{
					if (env.pickObjType == POT_GROUP && group->species.GetCount() == 0)
					{
						i = 0;
						j = pickObjs->GetCount();
						while (i < j)
						{
							itemId = pickObjs->GetItem(i);
							sb.ClearStr();
							sb.AppendC(UTF8STRC("group"));
							sb.AppendI32(itemId);
							if (req->GetHTTPFormStr(sb.ToCString()).SetTo(s) && s->v[0] == '1')
							{
								if (me->env->GroupMove(mutUsage, itemId, id, cateId))
								{
									pickObjs->RemoveAt(i);
									i--;
								}
							}
							i++;
						}
						if (pickObjs->GetCount() == 0)
						{
							env.pickObjType = POT_UNKNOWN;
							webSess.SetPickObjType(env.pickObjType);
						}
					}
					else if (env.pickObjType == POT_SPECIES && group->groups.GetCount() == 0)
					{
						i = 0;
						j = pickObjs->GetCount();
						while (i < j)
						{
							itemId = pickObjs->GetItem(i);
							sb.ClearStr();
							sb.AppendC(UTF8STRC("species"));
							sb.AppendI32(itemId);
							if (req->GetHTTPFormStr(sb.ToCString()).SetTo(s) && s->v[0] == '1')
							{
								if (me->env->SpeciesMove(mutUsage, itemId, id, cateId))
								{
									pickObjs->RemoveAt(i);
									i--;
								}
							}
							i++;
						}
						if (pickObjs->GetCount() == 0)
						{
							env.pickObjType = POT_UNKNOWN;
							webSess.SetPickObjType(env.pickObjType);
						}
					}
				}
				else if (action->Equals(UTF8STRC("placeall")) && env.pickObjs.SetTo(pickObjs))
				{
					if (env.pickObjType == POT_GROUP && group->species.GetCount() == 0)
					{
						i = 0;
						j = pickObjs->GetCount();
						while (i < j)
						{
							itemId = pickObjs->GetItem(i);
							if (me->env->GroupMove(mutUsage, itemId, id, cateId))
							{
								pickObjs->RemoveAt(i);
								i--;
							}
							i++;
						}
						if (pickObjs->GetCount() == 0)
						{
							env.pickObjType = POT_UNKNOWN;
							webSess.SetPickObjType(env.pickObjType);
						}
					}
					else if (env.pickObjType == POT_SPECIES && group->groups.GetCount() == 0)
					{
						i = 0;
						j = pickObjs->GetCount();
						while (i < j)
						{
							itemId = pickObjs->GetItem(i);
							if (me->env->SpeciesMove(mutUsage, itemId, id, cateId))
							{
								pickObjs->RemoveAt(i);
								i--;
							}
							i++;
						}
						if (pickObjs->GetCount() == 0)
						{
							env.pickObjType = POT_UNKNOWN;
							webSess.SetPickObjType(env.pickObjType);
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
		me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
		writer.Write(CSTR("<center><h1>"));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.Write(s->ToCString());
		s->Release();
		writer.WriteLine(CSTR("</h1></center>"));

		writer.WriteLine(CSTR("<table border=\"0\" width=\"100%\">"));
		writer.WriteLine(CSTR("<tr>"));
		writer.Write(CSTR("<td><form method=\"POST\" action=\"searchinside.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&amp;cateId="));
		sb.AppendI32(group->cateId);
		writer.Write(sb.ToCString());
		writer.Write(CSTR("\">"));
		writer.Write(CSTR("Search inside: <input type=\"text\" name=\"searchStr\"/><input type=\"submit\"/>"));
		writer.WriteLine(CSTR("</form></td>"));
		writer.WriteLine(CSTR("</tr>"));
		writer.WriteLine(CSTR("</table>"));

		me->WriteLocator(mutUsage, writer, group, cate);
		writer.WriteLine(CSTR("<br/>"));
		s = Text::XML::ToNewHTMLBodyText(group->descript->v);
		writer.Write(s->ToCString());
		s->Release();
		if (!notAdmin)
		{
			writer.WriteLine(CSTR("<br/>"));
			if (group->groups.GetCount() == 0)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<a href=\"speciesmod.html?id="));
				sb.AppendI32(id);
				sb.AppendC(UTF8STRC("&amp;cateId="));
				sb.AppendI32(cateId);
				sb.AppendC(UTF8STRC("\">New Species</a>"));
				writer.WriteLine(sb.ToCString());
			}
			if (group->species.GetCount() == 0)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<a href=\"groupmod.html?id="));
				sb.AppendI32(id);
				sb.AppendC(UTF8STRC("&amp;cateId="));
				sb.AppendI32(cateId);
				sb.AppendC(UTF8STRC("\">New Group</a>"));
				writer.WriteLine(sb.ToCString());
			}
			writer.Write(CSTR("<a href=\"groupmod.html?id="));
			sb.ClearStr();
			sb.AppendI32(group->parentId);
			sb.AppendC(UTF8STRC("&amp;cateId="));
			sb.AppendI32(group->cateId);
			sb.AppendC(UTF8STRC("&amp;groupId="));
			sb.AppendI32(group->id);
			writer.Write(sb.ToCString());
			writer.Write(CSTR("\">"));
			writer.Write(CSTR("Modify Group"));
			writer.Write(CSTR("</a>"));
			writer.WriteLine(CSTR("<br/>"));
		}
		sb.ClearStr();
		sb.AppendC(UTF8STRC("<a href=\"list.html?id="));
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&amp;cateId="));
		sb.AppendI32(group->cateId);
		sb.AppendC(UTF8STRC("\">"));
		sb.Append(LangGetValue(lang, CSTR("ListAll")));
		sb.AppendC(UTF8STRC("</a><br/>"));
		writer.WriteLine(sb.ToCString());

		sb.ClearStr();
		sb.AppendC(UTF8STRC("<a href=\"listimage.html?id="));
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&amp;cateId="));
		sb.AppendI32(group->cateId);
		sb.AppendC(UTF8STRC("\">"));
		sb.Append(LangGetValue(lang, CSTR("ListImage")));
		sb.AppendC(UTF8STRC("</a><br/>"));
		writer.WriteLine(sb.ToCString());

		if (env.user.NotNull() || me->env->GroupIsPublic(mutUsage, group->id))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<a href=\"map/index.html?group="));
			sb.AppendI32(group->id);
			sb.AppendC(UTF8STRC("\">"));
			sb.Append(LangGetValue(lang, CSTR("ShowMap")));
			sb.AppendC(UTF8STRC("</a><br/>"));
			writer.WriteLine(sb.ToCString());
		}
		writer.WriteLine(CSTR("<hr/>"));

		Bool found = false;
		if (!notAdmin)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<form name=\"groupform\" method=\"POST\" action=\"group.html?id="));
			sb.AppendI32(id);
			sb.AppendC(UTF8STRC("&amp;cateId="));
			sb.AppendI32(cateId);
			sb.AppendC(UTF8STRC("\">"));
			writer.WriteLine(sb.ToCString());
			writer.WriteLine(CSTR("<input type=\"hidden\" name=\"action\"/>"));
		}
		if (group->groups.GetCount())
		{
			NN<GroupInfo> sgroup;
			Data::StringMapNN<GroupInfo> groups;
			i = group->groups.GetCount();
			while (i-- > 0)
			{
				sgroup = group->groups.GetItemNoCheck(i);
				if ((sgroup->flags & 1) == 0 || !notAdmin)
				{
					groups.PutNN(sgroup->engName, sgroup);
				}
			}
			if (groups.GetCount() > 0)
			{
				me->WriteGroupTable(mutUsage, writer, groups.GetValues(), env.scnWidth, !notAdmin, false);
				writer.WriteLine(CSTR("<hr/>"));
				found = true;
			}
		}
		if (group->species.GetCount())
		{
			NN<SpeciesInfo> sp;
			Data::StringMapNN<SpeciesInfo> species;
			i = group->species.GetCount();
			while (i-- > 0)
			{
				sp = group->species.GetItemNoCheck(i);
				species.PutNN(sp->sciName, sp);
			}
			me->WriteSpeciesTable(mutUsage, writer, species.GetValues(), env.scnWidth, group->cateId, !notAdmin, !notAdmin);
			writer.WriteLine(CSTR("<hr/>"));
			found = true;
		}
		if (!found)
		{
			writer.WriteLine(LangGetValue(lang, CSTR("NoItems")));
			writer.WriteLine(CSTR("<hr/>"));
		}
		if (!notAdmin)
		{
			writer.WriteLine(CSTR("</form>"));
		}
		if (env.user.SetTo(user) && user->userType == UserType::Admin)
		{
			writer.WriteLine(CSTR("Pick Objects:<br/>"));
			writer.WriteLine(CSTR("<input type=\"button\" value=\"Pick Selected\" onclick=\"document.forms.groupform.action.value='picksel';document.forms.groupform.submit();\"/>"));
			writer.WriteLine(CSTR("<input type=\"button\" value=\"Pick All\" onclick=\"document.forms.groupform.action.value='pickall';document.forms.groupform.submit();\"/>"));
			writer.WriteLine(CSTR("<hr/>"));
			writer.WriteLine(CSTR("Picked Objects:<br/>"));
			sb.ClearStr();
			sb.AppendC(UTF8STRC("group.html?id="));
			sb.AppendI32(id);
			sb.AppendC(UTF8STRC("&cateId="));
			sb.AppendI32(cateId);
			me->WritePickObjs(mutUsage, writer, env, sb.ToString(), false);
		}

		if (group->parentId == 0)
		{
			writer.Write(CSTR("<a href="));
			sb.ClearStr();
			sb.AppendC(UTF8STRC("cate.html?cateName="));
			sb.Append(cate->dirName);
			s = Text::XML::ToNewAttrText(sb.ToString());
			writer.Write(s->ToCString());
			s->Release();
			writer.Write(CSTR(">"));
			writer.Write(LangGetValue(lang, CSTR("Back")));
			writer.Write(CSTR("</a>"));
		}
		else
		{
			writer.Write(CSTR("<a href=\"group.html?id="));
			sb.ClearStr();
			sb.AppendI32(group->parentId);
			sb.AppendC(UTF8STRC("&amp;cateId="));
			sb.AppendI32(group->cateId);
			writer.Write(sb.ToCString());
			writer.Write(CSTR("\">"));
			writer.Write(LangGetValue(lang, CSTR("Back")));
			writer.Write(CSTR("</a>"));
		}
		if (env.user.SetTo(user) && user->userType == UserType::Admin)
		{
			writer.WriteLine(CSTR("<br/>"));

			writer.WriteLine(CSTR("<input type=\"button\" value=\"Set Parent Photo\" onclick=\"document.forms.groupform.action.value='setphoto';document.forms.groupform.submit();\"/>"));
		}


		me->WriteFooter(writer);
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


Bool __stdcall SSWR::OrganWeb::OrganWebMainController::SvcGroupMod(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebMainController> me = NN<SSWR::OrganWeb::OrganWebMainController>::ConvertFrom(parent);
	RequestEnv env;
	NN<WebUserInfo> user;
	UIntOS i;
	UIntOS j;
	me->ParseRequestEnv(req, resp, env, false);

	if (!env.user.SetTo(user) || user->userType != UserType::Admin)
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
		NN<GroupInfo> group;
		NN<CategoryInfo> cate;
		Text::StringBuilderUTF8 sb;
		NN<Text::String> s;
		NN<Text::String> txt;
		Optional<IO::ConfigFile> lang = me->env->LangGet(req);

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
		NN<Text::String> ename = Text::String::NewEmpty();
		NN<Text::String> cname = ename;
		NN<Text::String> descr = ename;
		GroupFlags groupFlags = GF_NONE;
		Int32 groupTypeId = 0;
		Optional<GroupInfo> modGroup = nullptr;
		NN<GroupInfo> nnmodGroup;
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
			NN<Text::String> task;
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
						if (group->groups.GetItemNoCheck(i)->engName->Equals(ename))
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
						if (group->groups.GetItem(i) != nnmodGroup.Ptr() && group->groups.GetItemNoCheck(i)->engName->Equals(ename))
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
		me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
		writer.Write(CSTR("<center><h1>"));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.Write(s->ToCString());
		s->Release();
		writer.WriteLine(CSTR("</h1></center>"));

		writer.Write(CSTR("<form name=\"newgroup\" method=\"POST\" action=\"groupmod.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&amp;cateId="));
		sb.AppendI32(group->cateId);
		if (groupId != 0)
		{
			sb.AppendC(UTF8STRC("&amp;groupId="));
			sb.AppendI32(groupId);
		}
		writer.Write(sb.ToCString());
		writer.WriteLine(CSTR("\">"));
		writer.WriteLine(CSTR("<input type=\"hidden\" name=\"task\"/>"));
		writer.WriteLine(CSTR("<table border=\"0\">"));
		writer.WriteLine(CSTR("<tr><td>Category</td><td><select name=\"groupType\">"));
		NN<GroupTypeInfo> groupType;
		i = 0;
		j = cate->groupTypes.GetCount();
		while (i < j)
		{
			groupType = cate->groupTypes.GetItemNoCheck(i);
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
			writer.WriteLine(sb.ToCString());
			i++;
		}
		writer.WriteLine(CSTR("</select></td></tr>"));
		writer.Write(CSTR("<tr><td>English Name</td><td><input type=\"text\" name=\"ename\""));
		if (ename->leng > 0)
		{
			writer.Write(CSTR(" value="));
			s = Text::XML::ToNewAttrText(ename->v);
			writer.Write(s->ToCString());
			s->Release();
		}
		writer.WriteLine(CSTR("/></td></tr>"));
		writer.Write(CSTR("<tr><td>Chinese Name</td><td><input type=\"text\" name=\"cname\""));
		if (cname->leng > 0)
		{
			writer.Write(CSTR(" value="));
			s = Text::XML::ToNewAttrText(cname->v);
			writer.Write(s->ToCString());
			s->Release();
		}
		writer.WriteLine(CSTR("/></td></tr>"));
		writer.Write(CSTR("<tr><td>Flags</td><td><input type=\"checkbox\" id=\"adminOnly\" name=\"adminOnly\" value=\"1\""));
		if (groupFlags & GF_ADMIN_ONLY)
		{
			writer.Write(CSTR(" checked"));
		}
		writer.WriteLine(CSTR("/><label for=\"adminOnly\">Admin Only</label></td></tr>"));
		writer.Write(CSTR("<tr><td>Description</td><td><textarea name=\"descr\" rows=\"4\" cols=\"40\">"));
		if (descr->leng > 0)
		{
			s = Text::XML::ToNewHTMLElementText(descr->v);
			writer.Write(s->ToCString());
			s->Release();
		}
		writer.WriteLine(CSTR("</textarea></td></tr>"));
		writer.Write(CSTR("<tr><td></td><td>"));
		if (groupId != 0)
		{
			writer.Write(CSTR("<input type=\"button\" value=\"Modify\" onclick=\"document.forms.newgroup.task.value='modify';document.forms.newgroup.submit();\"/>"));
		}
		writer.Write(CSTR("<input type=\"button\" value=\"New\" onclick=\"document.forms.newgroup.task.value='new';document.forms.newgroup.submit();\"/>"));
		if (groupId != 0)
		{
			NN<GroupInfo> modGroup;
			if (me->env->GroupGet(mutUsage, groupId).SetTo(modGroup) && modGroup->species.GetCount() == 0 && modGroup->groups.GetCount() == 0)
			{
				writer.Write(CSTR("<input type=\"button\" value=\"Delete\" onclick=\"document.forms.newgroup.task.value='delete';document.forms.newgroup.submit();\"/>"));
			}
		}
		writer.Write(CSTR("<a href=\"group.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&amp;cateId="));
		sb.AppendI32(group->cateId);
		writer.Write(sb.ToCString());
		writer.Write(CSTR("\">"));
		writer.Write(LangGetValue(lang, CSTR("Back")));
		writer.Write(CSTR("</a>"));
		writer.WriteLine(CSTR("</td></tr>"));
		writer.WriteLine(CSTR("</table></form>"));
		me->WriteFooter(writer);
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

Bool __stdcall SSWR::OrganWeb::OrganWebMainController::SvcSpecies(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebMainController> me = NN<SSWR::OrganWeb::OrganWebMainController>::ConvertFrom(parent);
	RequestEnv env;
	OrganWebSession webSess(me->ParseRequestEnv(req, resp, env, true));
	NN<Data::ArrayListInt32> pickObjs;

	Int32 id;
	Int32 cateId;
	if (req->GetQueryValueI32(CSTR("id"), id) &&
		req->GetQueryValueI32(CSTR("cateId"), cateId))
	{
		NN<Text::String> s;
		UIntOS i;
		UIntOS j;
		UTF8Char sbuff[512];
		UTF8Char sbuff2[512];
		UnsafeArray<UTF8Char> sptr;
		UnsafeArray<UTF8Char> sptr2;
		NN<IO::Path::FindFileSession> sess;
		IO::Path::PathType pt;
		Text::StringBuilderUTF8 sb;
		NN<SpeciesInfo> species;
		NN<GroupInfo> group;
		NN<CategoryInfo> cate;
		NN<BookSpInfo> bookSp;
		NN<BookInfo> book;
		NN<UserFileInfo> userFile;
		NN<WebFileInfo> wfile;
		NN<WebUserInfo> user;
		Optional<IO::ConfigFile> lang = me->env->LangGet(req);
		Data::DateTime dt;

		Sync::RWMutexUsage mutUsage;
		if (!me->env->SpeciesGet(mutUsage, id).SetTo(species))
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		Bool notAdmin = (!env.user.SetTo(user) || user->userType != UserType::Admin);
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

		if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST && env.user.SetTo(user) && user->userType == UserType::Admin)
		{
			req->ParseHTTPForm();
			NN<Text::String> action;
			NN<Text::String> s;
			Int32 userfileId;
			if (req->GetHTTPFormStr(CSTR("action")).SetTo(action))
			{
				if (action->Equals(UTF8STRC("pickall")) && env.pickObjs.SetTo(pickObjs))
				{
					env.pickObjType = POT_USERFILE;
					webSess.SetPickObjType(env.pickObjType);
					pickObjs->Clear();
					i = 0;
					j = species->files.GetCount();
					while (i < j)
					{
						pickObjs->SortedInsert(species->files.GetItemNoCheck(i)->id);
						i++;
					}
				}
				else if (action->Equals(UTF8STRC("picksel")) && env.pickObjs.SetTo(pickObjs))
				{
					env.pickObjType = POT_USERFILE;
					webSess.SetPickObjType(env.pickObjType);
					pickObjs->Clear();
					i = 0;
					j = species->files.GetCount();
					while (i < j)
					{
						userfileId = species->files.GetItemNoCheck(i)->id;
						sb.ClearStr();
						sb.AppendC(UTF8STRC("userfile"));
						sb.AppendI32(userfileId);
						if (req->GetHTTPFormStr(sb.ToCString()).SetTo(s) && s->v[0] == '1')
						{
							pickObjs->SortedInsert(userfileId);
						}
						i++;
					}
				}
				else if (action->Equals(UTF8STRC("place")) && env.pickObjs.SetTo(pickObjs))
				{
					if (env.pickObjType == POT_USERFILE)
					{
						i = 0;
						j = pickObjs->GetCount();
						while (i < j)
						{
							userfileId = pickObjs->GetItem(i);
							sb.ClearStr();
							sb.AppendC(UTF8STRC("userfile"));
							sb.AppendI32(userfileId);
							if (req->GetHTTPFormStr(sb.ToCString()).SetTo(s) && s->v[0] == '1')
							{
								if (me->env->UserfileMove(mutUsage, userfileId, id, cateId))
								{
									pickObjs->RemoveAt(i);
									i--;
								}
							}
							i++;
						}
						if (pickObjs->GetCount() == 0)
						{
							env.pickObjType = POT_UNKNOWN;
							webSess.SetPickObjType(env.pickObjType);
						}
					}
				}
				else if (action->Equals(UTF8STRC("placeall")) && env.pickObjs.SetTo(pickObjs))
				{
					if (env.pickObjType == POT_USERFILE)
					{
						i = 0;
						j = pickObjs->GetCount();
						while (i < j)
						{
							userfileId = pickObjs->GetItem(i);
							if (me->env->UserfileMove(mutUsage, userfileId, id, cateId))
							{
								pickObjs->RemoveAt(i);
								i--;
							}
							i++;
						}
						if (pickObjs->GetCount() == 0)
						{
							env.pickObjType = POT_UNKNOWN;
							webSess.SetPickObjType(env.pickObjType);
						}
					}
				}
				else if (action->Equals(UTF8STRC("placemerge")) && env.pickObjs.SetTo(pickObjs))
				{
					if (env.pickObjType == POT_SPECIES)
					{
						i = 0;
						j = pickObjs->GetCount();
						while (i < j)
						{
							Int32 speciesId = pickObjs->GetItem(i);
							sb.ClearStr();
							sb.AppendC(UTF8STRC("species"));
							sb.AppendI32(speciesId);
							if (req->GetHTTPFormStr(sb.ToCString()).SetTo(s) && s->v[0] == '1')
							{
								if (me->env->SpeciesMerge(mutUsage, speciesId, id, cateId))
								{
									pickObjs->RemoveAt(i);
									i--;
								}
							}
							i++;
						}
						if (pickObjs->GetCount() == 0)
						{
							env.pickObjType = POT_UNKNOWN;
							webSess.SetPickObjType(env.pickObjType);
						}
					}
				}
				else if (action->Equals(UTF8STRC("setphoto")))
				{
					me->env->GroupSetPhotoSpecies(mutUsage, species->groupId, species->speciesId);
				}
				else if (action->Equals(UTF8STRC("bookspecies")))
				{
					NN<Text::String> dispName;
					UInt32 bookAllowDup = 0;
					req->GetHTTPFormUInt32(CSTR("bookAllowDup"), bookAllowDup);
					if (req->GetHTTPFormStr(CSTR("speciesname")).SetTo(dispName) && dispName->leng > 0)
					{
						me->env->BookAddSpecies(mutUsage, species->speciesId, dispName, bookAllowDup != 0);
					}
				}
				else if (action->Equals(UTF8STRC("webfile")))
				{
					NN<Text::String> srcURL;
					NN<Text::String> imgURL;
					Optional<Text::String> location = req->GetHTTPFormStr(CSTR("location"));
					if (req->GetHTTPFormStr(CSTR("srcURL")).SetTo(srcURL) && req->GetHTTPFormStr(CSTR("imgURL")).SetTo(imgURL) && srcURL->leng > 0 && imgURL->leng > 0)
					{
						me->env->SpeciesAddWebfile(mutUsage, species->speciesId, imgURL->ToCString(), srcURL->ToCString(), Text::String::OrEmpty(location)->ToCString());
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
		me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
		writer.Write(CSTR("<center><h1>"));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.Write(s->ToCString());
		s->Release();
		writer.WriteLine(CSTR("</h1></center>"));

		writer.WriteLine(CSTR("<table border=\"0\"><tr><td>"));
		s = Text::XML::ToNewHTMLBodyText(species->descript->v);
		writer.Write(s->ToCString());
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
				userFile = species->files.GetItemNoCheck(i);
				if (userFile->captureTimeTicks)
				{
					dt.SetTicks(userFile->captureTimeTicks);
					months[dt.GetMonth() - 1] = true;
				}
			}
			writer.WriteLine(CSTR("<br/><br/>Months"));
			writer.WriteLine(CSTR("<table><tr>"));
			i = 0;
			while (i < 12)
			{
				if (months[i])
				{
					writer.Write(CSTR("<td bgcolor=\"#4060ff\">"));
				}
				else
				{
					writer.Write(CSTR("<td>"));
				}
				sptr = Text::StrUIntOS(sbuff, i + 1);
				writer.Write(CSTRP(sbuff, sptr));
				writer.Write(CSTR("</td>"));
				i++;
			}
			writer.WriteLine(CSTR("</tr></table>"));
		}
		if (species->books.GetCount() > 0)
		{
			Data::DateTime dt;
			writer.WriteLine(CSTR("<br/><br/>"));
			writer.Write(LangGetValue(lang, CSTR("Document")));
			writer.WriteLine(CSTR(":<br/>"));
			writer.WriteLine(CSTR("<table border=\"0\">"));
			i = 0;
			j = species->books.GetCount();
			while (i < j)
			{
				bookSp = species->books.GetItemNoCheck(i);
				if (me->env->BookGet(mutUsage, bookSp->bookId).SetTo(book))
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
					writer.Write(sb.ToCString());
					s = Text::XML::ToNewHTMLBodyText(bookSp->dispName->v);
					writer.Write(s->ToCString());
					s->Release();
					writer.Write(CSTR("</i></a></td><td>"));
					sb.ClearStr();
					sb.Append(book->author);
					sb.AppendC(UTF8STRC(" ("));
					dt.SetTicks(book->publishDate);
					sb.AppendI32(Data::DateTimeUtil::DispYearI32(dt.GetYear()));
					sb.AppendC(UTF8STRC(")"));
					s = Text::XML::ToNewHTMLBodyText(sb.ToString());
					writer.Write(s->ToCString());
					s->Release();
					writer.WriteLine(CSTR("</td></tr>"));
				}
				i++;
			}
			writer.WriteLine(CSTR("</table>"));
		}
		writer.WriteLine(CSTR("</td><td>"));
		me->WriteLocator(mutUsage, writer, group, cate);
		writer.WriteLine(CSTR("</td></tr></table>"));
		if (env.user.NotNull() || me->env->GroupIsPublic(mutUsage, group->id))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<a href=\"map/index.html?species="));
			sb.AppendI32(id);
			sb.AppendC(UTF8STRC("\">"));
			sb.Append(LangGetValue(lang, CSTR("ShowMap")));
			sb.AppendC(UTF8STRC("</a><br/>"));
			writer.WriteLine(sb.ToCString());
		}
		writer.WriteLine(CSTR("<hr/>"));
		if (env.user.SetTo(user) && user->userType == UserType::Admin && me->env->BookGetSelected(mutUsage).SetTo(book))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<form name=\"bookspecies\" action=\"species.html?id="));
			sb.AppendI32(id);
			sb.AppendC(UTF8STRC("&amp;cateId="));
			sb.AppendI32(cateId);
			sb.AppendC(UTF8STRC("\" method=\"POST\"/>"));
			writer.WriteLine(sb.ToCString());
			writer.WriteLine(CSTR("<input type=\"hidden\" name=\"action\" value=\"bookspecies\"/>"));
			writer.Write(CSTR("Selected Book: "));
			s = Text::XML::ToNewHTMLBodyText(book->title->v);
			writer.Write(s->ToCString());
			s->Release();
			writer.Write(CSTR("<br/><input type=\"text\" name=\"speciesname\" value="));
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
			writer.Write(s->ToCString());
			s->Release();
			writer.Write(CSTR(" /><input type=\"checkbox\" name=\"bookAllowDup\" id=\"bookAllowDup\" value=\"1\"/><label for=\"bookAllowDup\">Allow Duplicate</label>"));
			writer.Write(CSTR("<input type=\"submit\" /><br/>"));
			writer.WriteLine(CSTR("</form><hr/>"));
		}

		Data::ArrayListICaseStringNN fileNameList;
		Data::ArrayListStringNN refURLList;
		sptr = cate->srcDir->ConcatTo(sbuff);
		if (IO::Path::PATH_SEPERATOR != '\\')
		{
			Text::StrReplace(sbuff, '\\', IO::Path::PATH_SEPERATOR);
		}
		sptr = species->dirName->ConcatTo(sptr);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
		if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
		{
			while (IO::Path::FindNextFile(sptr, sess, 0, pt, 0).SetTo(sptr2))
			{
				if (pt == IO::Path::PathType::File)
				{
					if (Text::StrEndsWithICaseC(sptr, (UIntOS)(sptr2 - sptr), UTF8STRC(".JPG")) || Text::StrEndsWithICaseC(sptr, (UIntOS)(sptr2 - sptr), UTF8STRC(".PCX")) || Text::StrEndsWithICaseC(sptr, (UIntOS)(sptr2 - sptr), UTF8STRC(".TIF")))
					{
						sptr2[-4] = 0;
						fileNameList.SortedInsert(Text::String::New(sptr, (UIntOS)(sptr2 - sptr - 4)));
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
					i = Text::StrLastIndexOfCharC(sptr, (UIntOS)(sptr2 - sptr), '.');
					if (i != INVALID_INDEX)
					{
						sptr[i] = 0;
						sptr2 = &sptr[i];
					}
					fileNameList.Add(Text::String::New(sptr, (UIntOS)(sptr2 - sptr)));
				}
				sb.ClearStr();
			}
		}

		UIntOS colCount = env.scnWidth / GetPreviewSize();
		UIntOS colWidth = 100 / colCount;
		UInt32 currColumn;
		if (fileNameList.GetCount() > 0 || species->files.GetCount() > 0 || species->wfiles.GetCount() > 0)
		{
			currColumn = 0;
			if (env.user.SetTo(user) && user->userType == UserType::Admin)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<form name=\"userfiles\" action=\"species.html?id="));
				sb.AppendI32(id);
				sb.AppendC(UTF8STRC("&amp;cateId="));
				sb.AppendI32(cateId);
				sb.AppendC(UTF8STRC("\" method=\"POST\"/>"));
				writer.WriteLine(sb.ToCString());
				writer.WriteLine(CSTR("<input type=\"hidden\" name=\"action\"/>"));
			}
			writer.WriteLine(CSTR("<table border=\"0\" width=\"100%\">"));

			Data::ArrayListNN<UserFileInfo> fileList;
			fileList.AddAll(species->files);
			if (env.user.NotNull())
			{
				UserFileDescComparator comparator(env);
				Data::Sort::ArtificialQuickSort::Sort<NN<UserFileInfo>>(fileList, comparator);
			}
			i = 0;
			j = fileList.GetCount();
			while (i < j)
			{
				userFile = fileList.GetItemNoCheck(i);
				if (currColumn == 0)
				{
					writer.WriteLine(CSTR("<tr>"));
				}
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<td width=\""));
				sb.AppendUIntOS(colWidth);
				sb.AppendC(UTF8STRC("%\">"));
				writer.WriteLine(sb.ToCString());
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
				writer.WriteLine(sb.ToCString());
				s->Release();

				writer.Write(CSTR("<img src="));
				sb.ClearStr();
				sb.AppendC(UTF8STRC("photo.html?id="));
				sb.AppendI32(species->speciesId);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(species->cateId);
				sb.AppendC(UTF8STRC("&width="));
				sb.AppendUIntOS(GetPreviewSize());
				sb.AppendC(UTF8STRC("&height="));
				sb.AppendUIntOS(GetPreviewSize());
				sb.AppendC(UTF8STRC("&fileId="));
				sb.AppendI32(userFile->id);
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer.Write(s->ToCString());
				s->Release();
				writer.Write(CSTR(" border=\"0\">"));
				writer.Write(CSTR("</a><br/>"));
				dt.SetTicks(userFile->fileTimeTicks);
				dt.ToLocalTime();
				if (env.user.SetTo(user) && (user->userType == UserType::Admin || userFile->webuserId == user->id))
				{
					if (user->userType == UserType::Admin)
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("<input type=\"checkbox\" id=\"userfile"));
						sb.AppendI32(userFile->id);
						sb.AppendC(UTF8STRC("\" name=\"userfile"));
						sb.AppendI32(userFile->id);
						sb.AppendC(UTF8STRC("\" value=\"1\"/><label for=\"userfile"));
						sb.AppendI32(userFile->id);
						sb.AppendC(UTF8STRC("\">"));
						writer.Write(sb.ToCString());
					}
					sptr2 = dt.ToString(sbuff2, "yyyy-MM-dd HH:mm:ss zzzz");
					writer.Write(CSTRP(sbuff2, sptr2));
					if (userFile->webuserId == user->id)
					{
						if (userFile->location.SetTo(s))
						{
							writer.Write(CSTR(" "));
							s = Text::XML::ToNewHTMLBodyText(s->v);
							writer.Write(s->ToCString());
							s->Release();
						}
					}
					if (userFile->descript.SetTo(s) && s->leng > 0)
					{
						writer.Write(CSTR("<br/>"));
						s = Text::XML::ToNewHTMLBodyText(s->v);
						writer.Write(s->ToCString());
						s->Release();
					}
					if (userFile->webuserId == user->id)
					{
						writer.Write(CSTR("<br/>"));
						s = Text::XML::ToNewHTMLBodyText(userFile->oriFileName->v);
						writer.Write(s->ToCString());
						s->Release();
					}
					if (user->userType == UserType::Admin)
					{
						writer.Write(CSTR("</label>"));
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
						writer.Write(sb.ToCString());
					}
				}
				else
				{
					sptr2 = dt.ToString(sbuff2, "yyyy-MM-dd zzzz");
					writer.Write(CSTRP(sbuff2, sptr2));
				}
				writer.WriteLine(CSTR("</center></td>"));

				currColumn++;
				if (currColumn >= colCount)
				{
					writer.WriteLine(CSTR("</tr>"));
					currColumn = 0;
				}
				i++;
			}

			i = 0;
			j = species->wfiles.GetCount();
			while (i < j)
			{
				wfile = species->wfiles.GetItemNoCheck(i);
				if (currColumn == 0)
				{
					writer.WriteLine(CSTR("<tr>"));
				}
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<td width=\""));
				sb.AppendUIntOS(colWidth);
				sb.AppendC(UTF8STRC("%\">"));
				writer.WriteLine(sb.ToCString());
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
				writer.WriteLine(sb.ToCString());
				s->Release();

				writer.Write(CSTR("<img src="));
				sb.ClearStr();
				sb.AppendC(UTF8STRC("photo.html?id="));
				sb.AppendI32(species->speciesId);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(species->cateId);
				sb.AppendC(UTF8STRC("&width="));
				sb.AppendUIntOS(GetPreviewSize());
				sb.AppendC(UTF8STRC("&height="));
				sb.AppendUIntOS(GetPreviewSize());
				sb.AppendC(UTF8STRC("&fileWId="));
				sb.AppendI32(wfile->id);
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer.Write(s->ToCString());
				s->Release();
				writer.Write(CSTR(" border=\"0\">"));
				writer.Write(CSTR("</a>"));
				if (env.user.SetTo(user) && user->userType == UserType::Admin)
				{
					if (wfile->location->leng > 0)
					{
						writer.Write(CSTR("<br/>"));
						s = Text::XML::ToNewHTMLBodyText(wfile->location->v);
						writer.Write(s->ToCString());
						s->Release();
					}
				}
				writer.WriteLine(CSTR("</center></td>"));

				currColumn++;
				if (currColumn >= colCount)
				{
					writer.WriteLine(CSTR("</tr>"));
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
				sptr2 = Text::TextBinEnc::URIEncoding::URIEncode(sbuff2, fileNameList.GetItemNoCheck(i)->v);
				if (currColumn == 0)
				{
					writer.WriteLine(CSTR("<tr>"));
				}
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<td width=\""));
				sb.AppendUIntOS(colWidth);
				sb.AppendC(UTF8STRC("%\">"));
				writer.WriteLine(sb.ToCString());
				sb.ClearStr();
				sb.AppendC(UTF8STRC("photodetail.html?id="));
				sb.AppendI32(species->speciesId);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(species->cateId);
				sb.AppendC(UTF8STRC("&file="));
				sb.AppendC(sbuff2, (UIntOS)(sptr2 - sbuff2));
				s = Text::XML::ToNewAttrText(sb.ToString());
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<center><a href="));
				sb.Append(s);
				sb.AppendC(UTF8STRC(">"));
				writer.WriteLine(sb.ToCString());
				s->Release();

				writer.Write(CSTR("<img src="));
				sb.ClearStr();
				sb.AppendC(UTF8STRC("photo.html?id="));
				sb.AppendI32(species->speciesId);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(species->cateId);
				sb.AppendC(UTF8STRC("&width="));
				sb.AppendUIntOS(GetPreviewSize());
				sb.AppendC(UTF8STRC("&height="));
				sb.AppendUIntOS(GetPreviewSize());
				sb.AppendC(UTF8STRC("&file="));
				sb.AppendC(sbuff2, (UIntOS)(sptr2 - sbuff2));
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer.Write(s->ToCString());
				s->Release();
				writer.Write(CSTR(" border=\"0\">"));
				writer.WriteLine(CSTR("</a></center></td>"));

				currColumn++;
				if (currColumn >= colCount)
				{
					writer.WriteLine(CSTR("</tr>"));
					currColumn = 0;
				}

				fileNameList.GetItemNoCheck(i)->Release();
				i++;
			}

			if (currColumn != 0)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<td width=\""));
				sb.AppendUIntOS(colWidth);
				sb.AppendC(UTF8STRC("%\"></td>"));
				while (currColumn < colCount)
				{
					writer.WriteLine(sb.ToCString());
					currColumn++;
				}
				writer.WriteLine(CSTR("</tr>"));
			}
			writer.WriteLine(CSTR("</table><hr/>"));
			if (env.user.SetTo(user) && user->userType == UserType::Admin)
			{
				writer.WriteLine(CSTR("</form>"));
			}
		}
		if (refURLList.GetCount() > 0)
		{
			writer.WriteLine(CSTR("Reference URL:<br/>"));
			NN<Text::String> url;
			Data::ArrayIterator<NN<Text::String>> it = refURLList.Iterator();
			while (it.HasNext())
			{
				url = it.Next();

				writer.Write(CSTR("<a href="));
				s = Text::XML::ToNewAttrText(url->v);
				writer.Write(s->ToCString());
				s->Release();
				writer.Write(CSTR(">"));
				s = Text::XML::ToNewHTMLBodyText(url->v);
				writer.Write(s->ToCString());
				s->Release();
				writer.WriteLine(CSTR("</a><br/>"));

				url->Release();
			}
			writer.WriteLine(CSTR("<hr/>"));
		}
		if (env.user.SetTo(user) && user->userType == UserType::Admin)
		{
			writer.WriteLine(CSTR("Pick Objects:<br/>"));
			writer.WriteLine(CSTR("<input type=\"button\" value=\"Pick Selected\" onclick=\"document.forms.userfiles.action.value='picksel';document.forms.userfiles.submit();\"/>"));
			writer.WriteLine(CSTR("<input type=\"button\" value=\"Pick All\" onclick=\"document.forms.userfiles.action.value='pickall';document.forms.userfiles.submit();\"/>"));
			writer.WriteLine(CSTR("<hr/>"));
			writer.WriteLine(CSTR("Picked Objects:<br/>"));
			sb.ClearStr();
			sb.AppendC(UTF8STRC("species.html?id="));
			sb.AppendI32(id);
			sb.AppendC(UTF8STRC("&cateId="));
			sb.AppendI32(cateId);
			me->WritePickObjs(mutUsage, writer, env, sb.ToString(), true);
		}

		writer.WriteLine(CSTR("<br/>"));
		writer.Write(CSTR("<a href=\"group.html?id="));
		sb.ClearStr();
		sb.AppendI32(species->groupId);
		sb.AppendC(UTF8STRC("&amp;cateId="));
		sb.AppendI32(group->cateId);
		writer.Write(sb.ToCString());
		writer.Write(CSTR("\">"));
		writer.Write(LangGetValue(lang, CSTR("Back")));
		writer.Write(CSTR("</a>"));

		if (env.user.SetTo(user) && user->userType == UserType::Admin)
		{
			writer.WriteLine(CSTR("<br/>"));
			writer.WriteLine(CSTR("<hr/>"));
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<form name=\"webfile\" action=\"species.html?id="));
			sb.AppendI32(id);
			sb.AppendC(UTF8STRC("&amp;cateId="));
			sb.AppendI32(cateId);
			sb.AppendC(UTF8STRC("\" method=\"POST\"/>"));
			writer.WriteLine(sb.ToCString());
			writer.WriteLine(CSTR("<input type=\"hidden\" name=\"action\" value=\"webfile\"/>"));
			writer.WriteLine(CSTR("Image URL: <input type=\"text\" name=\"imgURL\" value=\"\"/><br/>"));
			writer.WriteLine(CSTR("Source URL: <input type=\"text\" name=\"srcURL\" value=\"\"/><br/>"));
			writer.WriteLine(CSTR("Location: <input type=\"text\" name=\"location\" value=\"\"/><br/>"));
			writer.WriteLine(CSTR("<input type=\"submit\"/><br/>"));
			writer.WriteLine(CSTR("</form>"));
			writer.WriteLine(CSTR("<hr/>"));
			writer.WriteLine(CSTR("<input type=\"button\" value=\"Set Group Photo\" onclick=\"document.forms.userfiles.action.value='setphoto';document.forms.userfiles.submit();\"/>"));
		}

		me->WriteFooter(writer);
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

Bool __stdcall SSWR::OrganWeb::OrganWebMainController::SvcSpeciesMod(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebMainController> me = NN<SSWR::OrganWeb::OrganWebMainController>::ConvertFrom(parent);
	RequestEnv env;
	NN<WebUserInfo> user;
	me->ParseRequestEnv(req, resp, env, false);

	if (!env.user.SetTo(user) || user->userType != UserType::Admin)
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
		NN<GroupInfo> group;
		Text::StringBuilderUTF8 sb;
		NN<Text::String> s;
		Optional<IO::ConfigFile> lang = me->env->LangGet(req);

		Sync::RWMutexUsage mutUsage;
		if (!me->env->GroupGet(mutUsage, id).SetTo(group))
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}

		Text::StringBuilderUTF8 msg;
		NN<Text::String> cname = Text::String::NewEmpty();
		NN<Text::String> sname = cname;
		NN<Text::String> ename = cname;
		NN<Text::String> descr = cname;
		Bool canDelete = false;
		UnsafeArrayOpt<const UTF8Char> bookIgn = nullptr;
		UnsafeArray<const UTF8Char> nnbookIgn;
		Optional<SpeciesInfo> species = nullptr;
		NN<SpeciesInfo> nnspecies;
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
			NN<Text::String> task;
			NN<Text::String> sbookIgn;
			if (req->GetQueryValue(CSTR("bookIgn")).SetTo(sbookIgn))
				bookIgn = UnsafeArray<const UTF8Char>(sbookIgn->v);
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
					else if ((!bookIgn.SetTo(nnbookIgn) || nnbookIgn[0] != '1') && me->env->SpeciesBookIsExist(mutUsage, sname->ToCString(), sb))
					{
						msg.AppendC(UTF8STRC("Species already exist in book: "));
						msg.AppendC(sb.ToString(), sb.GetLength());
						msg.AppendC(UTF8STRC(", continue?"));
						bookIgn = U8STR("1");
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
					else if (nameChg && (!bookIgn.SetTo(nnbookIgn) || nnbookIgn[0] != '1') && me->env->SpeciesBookIsExist(mutUsage, sname->ToCString(), sb))
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
		me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
		writer.Write(CSTR("<center><h1>"));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.Write(s->ToCString());
		s->Release();
		writer.Write(CSTR("</h1>"));
		if (msg.GetLength() > 0)
		{
			writer.Write(CSTR("<h2>"));
			writer.Write(msg.ToCString());
			writer.Write(CSTR("</h2>"));
		}
		writer.WriteLine(CSTR("</center>"));

		writer.Write(CSTR("<form name=\"newspecies\" method=\"POST\" action=\"speciesmod.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&amp;cateId="));
		sb.AppendI32(group->cateId);
		if (spId != 0)
		{
			sb.AppendC(UTF8STRC("&amp;spId="));
			sb.AppendI32(spId);
		}
		if (bookIgn.SetTo(nnbookIgn) && nnbookIgn[0] == '1')
		{
			sb.AppendC(UTF8STRC("&amp;bookIgn=1"));
		}
		writer.Write(sb.ToCString());
		writer.WriteLine(CSTR("\">"));
		writer.WriteLine(CSTR("<input type=\"hidden\" name=\"task\"/>"));
		writer.WriteLine(CSTR("<table border=\"0\">"));
		writer.Write(CSTR("<tr><td>Chinese Name</td><td><input type=\"text\" name=\"cname\""));
		if (cname->leng > 0)
		{
			writer.Write(CSTR(" value="));
			s = Text::XML::ToNewAttrText(cname->v);
			writer.Write(s->ToCString());
			s->Release();
		}
		writer.WriteLine(CSTR("/></td></tr>"));
		writer.Write(CSTR("<tr><td>Science Name</td><td><input type=\"text\" name=\"sname\""));
		if (sname->leng > 0)
		{
			writer.Write(CSTR(" value="));
			s = Text::XML::ToNewAttrText(sname->v);
			writer.Write(s->ToCString());
			s->Release();
		}
		writer.WriteLine(CSTR("/></td></tr>"));
		writer.Write(CSTR("<tr><td>English Name</td><td><input type=\"text\" name=\"ename\""));
		if (ename->leng > 0)
		{
			writer.Write(CSTR(" value="));
			s = Text::XML::ToNewAttrText(ename->v);
			writer.Write(s->ToCString());
			s->Release();
		}
		writer.WriteLine(CSTR("/></td></tr>"));
		writer.Write(CSTR("<tr><td>Description</td><td><textarea name=\"descr\" rows=\"4\" cols=\"40\">"));
		if (descr->leng > 0)
		{
			s = Text::XML::ToNewHTMLElementText(descr->v);
			writer.Write(s->ToCString());
			s->Release();
		}
		writer.WriteLine(CSTR("</textarea></td></tr>"));
		writer.Write(CSTR("<tr><td></td><td>"));
		if (spId != 0)
		{
			writer.Write(CSTR("<input type=\"button\" value=\"Modify\" onclick=\"document.forms.newspecies.task.value='modify';document.forms.newspecies.submit();\"/>"));
		}
		writer.Write(CSTR("<input type=\"button\" value=\"New\" onclick=\"document.forms.newspecies.task.value='new';document.forms.newspecies.submit();\"/>"));

		writer.Write(CSTR("<a href=\"group.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&amp;cateId="));
		sb.AppendI32(group->cateId);
		writer.Write(sb.ToCString());
		writer.Write(CSTR("\">"));
		writer.Write(LangGetValue(lang, CSTR("Back")));
		writer.Write(CSTR("</a>"));
		if (canDelete)
		{
			writer.Write(CSTR("<br/><br/>"));
			writer.Write(CSTR("<input type=\"button\" value=\"Delete\" onclick=\"document.forms.newspecies.task.value='delete';document.forms.newspecies.submit();\"/>"));
		}
		writer.WriteLine(CSTR("</td></tr>"));
		writer.WriteLine(CSTR("</table></form>"));
		me->WriteFooter(writer);
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

Bool __stdcall SSWR::OrganWeb::OrganWebMainController::SvcList(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebMainController> me = NN<SSWR::OrganWeb::OrganWebMainController>::ConvertFrom(parent);
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
		NN<Text::String> s;
		UIntOS i;
		UIntOS j;
		Text::StringBuilderUTF8 sb;
		NN<GroupInfo> group;
		NN<CategoryInfo> cate;
		Optional<IO::ConfigFile> lang = me->env->LangGet(req);
		NN<WebUserInfo> user;
		Sync::RWMutexUsage mutUsage;
		Bool notAdmin = (!env.user.SetTo(user) || user->userType != UserType::Admin);
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
		me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
		writer.Write(CSTR("<center><h1>"));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.Write(s->ToCString());
		s->Release();
		writer.WriteLine(CSTR("</h1></center>"));

		me->WriteLocator(mutUsage, writer, group, cate);
		writer.WriteLine(CSTR("<br/>"));
		s = Text::XML::ToNewHTMLBodyText(group->descript->v);
		writer.Write(s->ToCString());
		s->Release();
		writer.WriteLine(CSTR("<br/>"));
		writer.WriteLine(CSTR("<hr/>"));

		Data::StringMapNN<SpeciesInfo> spMap;
		me->env->GetGroupSpecies(mutUsage, group, spMap, env.user);
		Data::ArrayListNN<SpeciesInfo> speciesTmp;
		NN<const Data::ReadingListNN<SpeciesInfo>> spList;
		spList = spMap.GetValues();
		if (imageOnly)
		{
			NN<SpeciesInfo> sp;
			i = 0;
			j = spList->GetCount();
			while (i < j)
			{
				sp = spList->GetItemNoCheck(i);
				if ((sp->flags & 9) != 0)
				{
					speciesTmp.Add(sp);
				}
				i++;
			}
			spList = speciesTmp;
		}
		UIntOS perPage = GetPerPage(env.isMobile);
		i = page * perPage;
		j = i + perPage;
		if (j > spList->GetCount())
		{
			j = spList->GetCount();
		}
		Data::ArrayListNN<SpeciesInfo> species;
		NN<SpeciesInfo> sp;
		while (i < j)
		{
			sp = spList->GetItemNoCheck(i);
			species.Add(sp);
			i++;
		}
		me->WriteSpeciesTable(mutUsage, writer, species, env.scnWidth, group->cateId, false, (env.user.SetTo(user) && user->userType == UserType::Admin));
		writer.WriteLine(CSTR("<hr/>"));

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
				sb.AppendUIntOS(perPage);
				sb.Append(LangGetValue(lang, CSTR("Items")));
				sb.AppendC(UTF8STRC("</a>"));
				writer.WriteLine(sb.ToCString());
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
				sb.AppendUIntOS(perPage);
				sb.Append(LangGetValue(lang, CSTR("Items")));
				sb.AppendC(UTF8STRC("&gt;</a>"));
				writer.WriteLine(sb.ToCString());
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
				sb.AppendUIntOS(perPage);
				sb.Append(LangGetValue(lang, CSTR("Items")));
				sb.AppendC(UTF8STRC("</a>"));
				writer.WriteLine(sb.ToCString());
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
				sb.AppendUIntOS(perPage);
				sb.Append(LangGetValue(lang, CSTR("Items")));
				sb.AppendC(UTF8STRC("&gt;</a>"));
				writer.WriteLine(sb.ToCString());
			}
		}
		writer.WriteLine(CSTR("<br/>"));
		writer.WriteLine(CSTR("<br/>"));
		writer.Write(CSTR("<a href=\"group.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&cateId="));
		sb.AppendI32(group->cateId);
		writer.Write(sb.ToCString());
		writer.Write(CSTR("\">"));
		writer.Write(LangGetValue(lang, CSTR("Back")));
		writer.Write(CSTR("</a>"));

		me->WriteFooter(writer);
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

Bool __stdcall SSWR::OrganWeb::OrganWebMainController::SvcPhotoDetail(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebMainController> me = NN<SSWR::OrganWeb::OrganWebMainController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	Int32 id;
	Int32 cateId;
	if (req->GetQueryValueI32(CSTR("id"), id) &&
		req->GetQueryValueI32(CSTR("cateId"), cateId))
	{
		UTF8Char fileName[512];
		UnsafeArray<UTF8Char> fileNameEnd;
		Int32 fileId;
		NN<Text::String> s;
		UIntOS i;
		UIntOS j;
		UTF8Char sbuff[512];
		UTF8Char sbuff2[512];
		UnsafeArray<UTF8Char> sptr;
		UnsafeArray<UTF8Char> sptrEnd;
		UnsafeArray<UTF8Char> sptr2;
		NN<IO::Path::FindFileSession> sess;
		IO::Path::PathType pt;
		Text::StringBuilderUTF8 sb;
		NN<SpeciesInfo> species;
		NN<GroupInfo> group;
		NN<CategoryInfo> cate;
		NN<WebUserInfo> user;
		Text::PString sarr[4];
		NN<WebFileInfo> wfile;
		Optional<IO::ConfigFile> lang = me->env->LangGet(req);

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
				if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST && env.user.SetTo(user) && (user->userType == UserType::Admin || user->id == userFile->webuserId))
				{
					req->ParseHTTPForm();
					NN<Text::String> action;
					if (req->GetHTTPFormStr(CSTR("action")).SetTo(action))
					{
						if (action->Equals(UTF8STRC("setdefault")) && user->userType == UserType::Admin)
						{
							me->env->SpeciesSetPhotoId(mutUsage, id, fileId);
						}
						else if (action->Equals(UTF8STRC("setname")))
						{
							NN<Text::String> desc;
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
				me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
				writer.Write(CSTR("<center><h1>"));
				s = Text::XML::ToNewHTMLBodyText(sb.ToString());
				writer.Write(s->ToCString());
				s->Release();
				writer.WriteLine(CSTR("</h1></center>"));

				writer.WriteLine(CSTR("<center>"));
				writer.WriteLine(CSTR("<table border=\"0\" width=\"100%\">"));
				writer.WriteLine(CSTR("<tr><td align=\"center\">"));

				if (i < j - 1)
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("photodetail.html?id="));
					sb.AppendI32(species->speciesId);
					sb.AppendC(UTF8STRC("&cateId="));
					sb.AppendI32(species->cateId);
					sb.AppendC(UTF8STRC("&fileId="));
					sb.AppendI32(species->files.GetItemNoCheck(i + 1)->id);
				}
				else if (species->wfiles.GetCount() != 0)
				{
					wfile = species->wfiles.GetItemNoCheck(0);
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
					Data::ArrayListICaseStringNN fileNameList;

					sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
					if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
					{

						while (IO::Path::FindNextFile(sptr, sess, 0, pt, 0).SetTo(sptr2))
						{
							if (pt == IO::Path::PathType::File)
							{
								if (Text::StrEndsWithICaseC(sptr, (UIntOS)(sptr2 - sptr), UTF8STRC(".JPG")) || Text::StrEndsWithICaseC(sptr, (UIntOS)(sptr2 - sptr), UTF8STRC(".PCX")) || Text::StrEndsWithICaseC(sptr, (UIntOS)(sptr2 - sptr), UTF8STRC(".TIF")))
								{
									sptr2[-4] = 0;
									fileNameList.SortedInsert(Text::String::New(sptr, (UIntOS)(sptr2 - sptr - 4)));
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
						sptr2 = Text::TextBinEnc::URIEncoding::URIEncode(sbuff2, fileNameList.GetItemNoCheck(0)->v);
						sb.AppendC(sbuff2, (UIntOS)(sptr2 - sbuff2));
					}
					else
					{
						Bool found;
						{
							sptrEnd = Text::StrConcatC(sptr, UTF8STRC("web.txt"));
							IO::FileStream fs({sbuff, (UIntOS)(sptrEnd - sbuff)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
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
									i = Text::StrLastIndexOfCharC(sbuff2, (UIntOS)(sptr2 - sbuff2), '.');
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
							sb.AppendC(sbuff2, (UIntOS)(sptr2 - sbuff2));
							sptr2 = Text::TextBinEnc::URIEncoding::URIEncode(sbuff2, sb.ToString());
							sb.ClearStr();
							sb.AppendC(UTF8STRC("photodetail.html?id="));
							sb.AppendI32(species->speciesId);
							sb.AppendC(UTF8STRC("&cateId="));
							sb.AppendI32(species->cateId);
							sb.AppendC(UTF8STRC("&file="));
							sb.AppendC(sbuff2, (UIntOS)(sptr2 - sbuff2));
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
					fileNameList.FreeAll();
				}
				s = Text::XML::ToNewAttrText(sb.ToString());
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<a href="));
				sb.Append(s);
				s->Release();
				sb.AppendC(UTF8STRC(">"));
				writer.WriteLine(sb.ToCString());

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
				writer.WriteLine(sb.ToCString());
				writer.WriteLine(CSTR("</td></tr>"));
				writer.WriteLine(CSTR("<tr><td align=\"center\">"));


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
							sb.ClearStr();
							sb.AppendU64(fileSize);
							sb.AppendC(UTF8STRC(" bytes"));
							NN<Media::MediaSource> msrc;
							Data::Duration stmTime;
							if (mediaFile->GetStream(0, 0).SetTo(msrc))
							{
								stmTime = msrc->GetStreamTime();
								sb.AppendC(UTF8STRC(", Length: "));
								sb.AppendDur(stmTime);

								if (msrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
								{
									NN<Media::AudioSource> asrc = NN<Media::AudioSource>::ConvertFrom(msrc);
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
							writer.Write(CSTR("<b>"));
							writer.Write(LangGetValue(lang, CSTR("PhotoSpec")));
							writer.Write(CSTR("</b> "));
							writer.Write(sb.ToCString());
							writer.Write(CSTR("<br/>"));
							mediaFile.Delete();
						}
					}
					if (userFile->captureTimeTicks != 0)
					{
						writer.Write(CSTR("<b>"));
						writer.Write(LangGetValue(lang, CSTR("PhotoDate")));
						writer.Write(CSTR("</b> "));
						sptr2 = Data::Timestamp(userFile->captureTimeTicks, Data::DateTimeUtil::GetLocalTzQhr()).ToString(sbuff2, "yyyy-MM-dd HH:mm:ss zzzz");
						writer.Write(CSTRP(sbuff2, sptr2));
						writer.Write(CSTR("<br/>"));
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
						writer.Write(CSTR("<b>"));
						writer.Write(LangGetValue(lang, CSTR("PhotoSpec")));
						writer.Write(CSTR("</b> "));
						info.ToString(sb);
						writer.Write(sb.ToCString());
						writer.Write(CSTR("<br/>"));

						writer.Write(CSTR("<b>"));
						writer.Write(LangGetValue(lang, CSTR("PhotoDate")));
						writer.Write(CSTR("</b> "));
						sptr2 = Data::Timestamp(userFile->captureTimeTicks, Data::DateTimeUtil::GetLocalTzQhr()).ToString(sbuff2, "yyyy-MM-dd HH:mm:ss zzzz");
						writer.Write(CSTRP(sbuff2, sptr2));
						writer.Write(CSTR("<br/>"));
					}
				}

				if (env.user.SetTo(user) && (userFile->webuserId == user->id || user->userType == UserType::Admin))
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
					writer.WriteLine(sb.ToCString());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("<form name=\"photo\" method=\"POST\" action=\"photodetail.html?id="));
					sb.AppendI32(id);
					sb.AppendC(UTF8STRC("&amp;cateId="));
					sb.AppendI32(cateId);
					sb.AppendC(UTF8STRC("&amp;fileId="));
					sb.AppendI32(fileId);
					sb.AppendC(UTF8STRC("\"><input type=\"hidden\" name=\"action\" value=\"setname\"/>"));
					if (user->userType == UserType::Admin)
					{
						sb.AppendC(UTF8STRC("<input type=\"button\" value=\"Set As Species Photo\" onclick=\"document.forms.photo.action.value='setdefault';document.forms.photo.submit();\"/>"));
					}
					sb.AppendC(UTF8STRC("<input type=\"button\" value=\"Rotate\" onclick=\"document.forms.photo.action.value='rotate';document.forms.photo.submit();\"/>"));
					sb.AppendC(UTF8STRC("<br/>"));
					writer.WriteLine(sb.ToCString());
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
					writer.WriteLine(sb.ToCString());
				}

				if (env.user.SetTo(user) && user->userType == UserType::Admin)
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
				writer.WriteLine(sb.ToCString());
				writer.WriteLine(CSTR("</td></tr>"));
				writer.WriteLine(CSTR("</table>"));
				writer.WriteLine(CSTR("</center>"));

				me->WriteFooter(writer);
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
			if (species->wfiles.Get(fileId).SetTo(wfile))
			{
				if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST && env.user.SetTo(user) && user->userType == UserType::Admin)
				{
					req->ParseHTTPForm();
					NN<Text::String> action;
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
				me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
				writer.Write(CSTR("<center><h1>"));
				s = Text::XML::ToNewHTMLBodyText(sb.ToString());
				writer.Write(s->ToCString());
				s->Release();
				writer.WriteLine(CSTR("</h1></center>"));

				writer.WriteLine(CSTR("<center>"));
				writer.WriteLine(CSTR("<table border=\"0\" width=\"100%\">"));
				writer.WriteLine(CSTR("<tr><td align=\"center\">"));

				i = (UIntOS)species->wfiles.GetIndex(fileId);
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
					Data::ArrayListICaseStringNN fileNameList;

					sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
					if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
					{

						while (IO::Path::FindNextFile(sptr, sess, 0, pt, 0).SetTo(sptr2))
						{
							if (pt == IO::Path::PathType::File)
							{
								if (Text::StrEndsWithICaseC(sptr, (UIntOS)(sptr2 - sptr), UTF8STRC(".JPG")) || Text::StrEndsWithICaseC(sptr, (UIntOS)(sptr2 - sptr), UTF8STRC(".PCX")) || Text::StrEndsWithICaseC(sptr, (UIntOS)(sptr2 - sptr), UTF8STRC(".TIF")))
								{
									sptr2[-4] = 0;
									fileNameList.SortedInsert(Text::String::New(sptr, (UIntOS)(sptr2 - sptr - 4)));
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
						sptr2 = Text::TextBinEnc::URIEncoding::URIEncode(sbuff2, fileNameList.GetItemNoCheck(0)->v);
						sb.AppendC(sbuff2, (UIntOS)(sptr2 - sbuff2));
					}
					else
					{
						Bool found;
						{
							sptrEnd = Text::StrConcatC(sptr, UTF8STRC("web.txt"));
							IO::FileStream fs({sbuff, (UIntOS)(sptrEnd - sbuff)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
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
									i = Text::StrLastIndexOfCharC(sbuff2, (UIntOS)(sptr2 - sbuff2), '.');
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
							sb.AppendC(sbuff2, (UIntOS)(sptr2 - sbuff2));
							sptr2 = Text::TextBinEnc::URIEncoding::URIEncode(sbuff2, sb.ToString());
							sb.ClearStr();
							sb.AppendC(UTF8STRC("photodetail.html?id="));
							sb.AppendI32(species->speciesId);
							sb.AppendC(UTF8STRC("&cateId="));
							sb.AppendI32(species->cateId);
							sb.AppendC(UTF8STRC("&file="));
							sb.AppendC(sbuff2, (UIntOS)(sptr2 - sbuff2));
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
					fileNameList.FreeAll();
				}
				s = Text::XML::ToNewAttrText(sb.ToString());
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<a href="));
				sb.Append(s);
				s->Release();
				sb.AppendC(UTF8STRC(">"));
				writer.WriteLine(sb.ToCString());

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
				writer.WriteLine(sb.ToCString());
				writer.WriteLine(CSTR("</td></tr>"));
				writer.WriteLine(CSTR("<tr><td align=\"center\">"));

				writer.Write(CSTR("<b>Image URL:</b> <a href="));
				s = Text::XML::ToNewAttrText(wfile->imgUrl->v);
				writer.Write(s->ToCString());
				s->Release();
				writer.Write(CSTR(">"));
				writer.Write(wfile->imgUrl->ToCString());
				writer.Write(CSTR("</a><br/>"));

				writer.Write(CSTR("<b>Source URL:</b> <a href="));
				s = Text::XML::ToNewAttrText(wfile->srcUrl->v);
				writer.Write(s->ToCString());
				s->Release();
				writer.Write(CSTR(">"));
				writer.Write(wfile->srcUrl->ToCString());
				writer.Write(CSTR("</a><br/>"));

				if (env.user.SetTo(user) && user->userType == UserType::Admin)
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
					writer.WriteLine(sb.ToCString());
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
				writer.WriteLine(sb.ToCString());
				writer.WriteLine(CSTR("</td></tr>"));
				writer.WriteLine(CSTR("</table>"));
				writer.WriteLine(CSTR("</center>"));

				me->WriteFooter(writer);
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
		else if (req->GetQueryValueStr(CSTR("file"), fileName, 512).SetTo(fileNameEnd))
		{
			if (Text::StrStartsWithC(fileName, (UIntOS)(fileNameEnd - fileName), UTF8STRC("web")) && fileName[3] == IO::Path::PATH_SEPERATOR)
			{
				Text::String *srcURL = 0;
				Text::String *imgURL = 0;
				Bool found;
				Bool foundNext;
				{
					sptrEnd = Text::StrConcatC(sptr, UTF8STRC("web.txt"));
					sptr2 = Text::StrConcatC(Text::StrConcat(sbuff2, &fileName[4]), UTF8STRC("."));
					Text::StrToUpperC(sbuff2, sbuff2, (UIntOS)(sptr2 - sbuff2));
					IO::FileStream fs({sbuff, (UIntOS)(sptrEnd - sbuff)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
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
								if (Text::StrStartsWithC(sarr[0].v, sarr[0].leng, sbuff2, (UIntOS)(sptr2 - sbuff2)))
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
					me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
					writer.Write(CSTR("<center><h1>"));
					s = Text::XML::ToNewHTMLBodyText(sb.ToString());
					writer.Write(s->ToCString());
					s->Release();
					writer.WriteLine(CSTR("</h1></center>"));

					writer.WriteLine(CSTR("<center>"));
					writer.WriteLine(CSTR("<table border=\"0\" width=\"100%\">"));
					writer.WriteLine(CSTR("<tr><td align=\"center\">"));
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
						i = Text::StrLastIndexOfCharC(sbuff, (UIntOS)(sptr2 - sbuff), '.');
						if (i != INVALID_INDEX)
							sbuff[i] = 0;
						sptr2 = Text::TextBinEnc::URIEncoding::URIEncode(sbuff2, sbuff);
						sb.AppendC(sbuff2, (UIntOS)(sptr2 - sbuff2));
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
					writer.WriteLine(sb.ToCString());

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
					writer.WriteLine(sb.ToCString());
					writer.WriteLine(CSTR("</td></tr>"));
					writer.WriteLine(CSTR("<tr><td align=\"center\">"));

					writer.Write(CSTR("<b>Image URL:</b> <a href="));
					s = Text::XML::ToNewAttrText(imgURL->v);
					writer.Write(s->ToCString());
					s->Release();
					writer.Write(CSTR(">"));
					writer.Write(imgURL->ToCString());
					writer.Write(CSTR("</a><br/>"));

					writer.Write(CSTR("<b>Source URL:</b> <a href="));
					s = Text::XML::ToNewAttrText(srcURL->v);
					writer.Write(s->ToCString());
					s->Release();
					writer.Write(CSTR(">"));
					writer.Write(srcURL->ToCString());
					writer.Write(CSTR("</a><br/>"));

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
					writer.WriteLine(sb.ToCString());
					writer.WriteLine(CSTR("</td></tr>"));
					writer.WriteLine(CSTR("</table>"));
					writer.WriteLine(CSTR("</center>"));

					me->WriteFooter(writer);
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
				Data::ArrayListICaseStringNN fileNameList;

				sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
				if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
				{
					while (IO::Path::FindNextFile(sptr, sess, 0, pt, 0).SetTo(sptr2))
					{
						if (pt == IO::Path::PathType::File)
						{
							if (Text::StrEndsWithICaseC(sptr, (UIntOS)(sptr2 - sptr), UTF8STRC(".JPG")) || Text::StrEndsWithICaseC(sptr, (UIntOS)(sptr2 - sptr), UTF8STRC(".PCX")) || Text::StrEndsWithICaseC(sptr, (UIntOS)(sptr2 - sptr), UTF8STRC(".TIF")))
							{
								sptr2[-4] = 0;
								fileNameList.SortedInsert(Text::String::New(sptr, (UIntOS)(sptr2 - sptr - 4)));
							}
						}
					}
					IO::Path::FindFileClose(sess);
					i = (UIntOS)fileNameList.SortedIndexOfC(Text::CStringNN(fileName, (UIntOS)(fileNameEnd - fileName)));
					if ((IntOS)i < 0)
					{
						fileNameList.FreeAll();
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
					me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
					writer.Write(CSTR("<center><h1>"));
					s = Text::XML::ToNewHTMLBodyText(sb.ToString());
					writer.Write(s->ToCString());
					s->Release();
					writer.WriteLine(CSTR("</h1></center>"));

					writer.WriteLine(CSTR("<center>"));
					writer.WriteLine(CSTR("<table border=\"0\" width=\"100%\">"));
					writer.WriteLine(CSTR("<tr><td align=\"center\">"));
					if (i < fileNameList.GetCount() - 1)
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("photodetail.html?id="));
						sb.AppendI32(species->speciesId);
						sb.AppendC(UTF8STRC("&cateId="));
						sb.AppendI32(species->cateId);
						sb.AppendC(UTF8STRC("&file="));
						sptr2 = Text::TextBinEnc::URIEncoding::URIEncode(sbuff2, fileNameList.GetItemNoCheck(i + 1)->v);
						sb.AppendC(sbuff2, (UIntOS)(sptr2 - sbuff2));
					}
					else
					{
						Bool found;
						{
							sptrEnd = Text::StrConcatC(sptr, UTF8STRC("web.txt"));
							IO::FileStream fs({sbuff, (UIntOS)(sptrEnd - sbuff)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
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
									i = Text::StrLastIndexOfCharC(sbuff2, (UIntOS)(sptr2 - sbuff2), '.');
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
							sb.AppendC(sbuff2, (UIntOS)(sptr2 - sbuff2));
							sptr2 = Text::TextBinEnc::URIEncoding::URIEncode(sbuff2, sb.ToString());
							sb.ClearStr();
							sb.AppendC(UTF8STRC("photodetail.html?id="));
							sb.AppendI32(species->speciesId);
							sb.AppendC(UTF8STRC("&cateId="));
							sb.AppendI32(species->cateId);
							sb.AppendC(UTF8STRC("&file="));
							sb.AppendC(sbuff2, (UIntOS)(sptr2 - sbuff2));
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
					writer.WriteLine(sb.ToCString());

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
					writer.WriteLine(sb.ToCString());
					writer.WriteLine(CSTR("</td></tr>"));
					writer.WriteLine(CSTR("<tr><td align=\"center\">"));

					sptrEnd = Text::StrConcatC(Text::StrConcat(sptr, fileName), UTF8STRC(".jpg"));
					{
						IO::StmData::FileData fd({sbuff, (UIntOS)(sptrEnd - sbuff)}, false);
						Media::PhotoInfo info(fd);
						if (info.HasInfo())
						{
							Data::DateTime dt;
							sb.ClearStr();
							writer.Write(CSTR("<b>"));
							writer.Write(LangGetValue(lang, CSTR("PhotoSpec")));
							writer.Write(CSTR("</b> "));
							info.ToString(sb);
							writer.Write(sb.ToCString());
							writer.Write(CSTR("<br/>"));
							if (info.GetPhotoDate(dt))
							{
								writer.Write(CSTR("<b>"));
								writer.Write(LangGetValue(lang, CSTR("PhotoDate")));
								writer.Write(CSTR("</b> "));
								sptr2 = dt.ToString(sbuff2, "yyyy-MM-dd HH:mm:ss zzzz");
								writer.Write(CSTRP(sbuff2, sptr2));
								writer.Write(CSTR("<br/>"));
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
					writer.WriteLine(sb.ToCString());
					writer.WriteLine(CSTR("</td></tr>"));
					writer.WriteLine(CSTR("</table>"));
					writer.WriteLine(CSTR("</center>"));

					me->WriteFooter(writer);
					mutUsage.EndUse();
					ResponseMstm(req, resp, mstm, CSTR("text/html"));

					fileNameList.FreeAll();
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

Bool __stdcall SSWR::OrganWeb::OrganWebMainController::SvcSearchInside(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebMainController> me = NN<SSWR::OrganWeb::OrganWebMainController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	Int32 id;
	Int32 cateId;
	NN<Text::String> searchStr;
	req->ParseHTTPForm();
	if (req->GetQueryValueI32(CSTR("id"), id) &&
		req->GetQueryValueI32(CSTR("cateId"), cateId) &&
		req->GetHTTPFormStr(CSTR("searchStr")).SetTo(searchStr))
	{
		NN<Text::String> s;
		UIntOS i;
		UIntOS j;
		Text::StringBuilderUTF8 sb;
		NN<GroupInfo> group;
		NN<CategoryInfo> cate;
		NN<WebUserInfo> user;
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		Optional<IO::ConfigFile> lang = me->env->LangGet(req);
		Sync::RWMutexUsage mutUsage;
		Bool notAdmin = (!env.user.SetTo(user) || user->userType != UserType::Admin);
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
		me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
		writer.Write(CSTR("<center><h1>"));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.Write(s->ToCString());
		s->Release();
		writer.WriteLine(CSTR("</h1></center>"));

		writer.WriteLine(CSTR("<table border=\"0\" width=\"100%\">"));
		writer.WriteLine(CSTR("<tr>"));
		writer.Write(CSTR("<td><form method=\"POST\" action=\"searchinside.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&cateId="));
		sb.AppendI32(group->cateId);
		writer.Write(sb.ToCString());
		writer.Write(CSTR("\">"));
		writer.Write(CSTR("Search inside: <input type=\"text\" name=\"searchStr\"/><input type=\"submit\"/>"));
		writer.WriteLine(CSTR("</form></td>"));
		writer.WriteLine(CSTR("</tr>"));
		writer.WriteLine(CSTR("</table>"));

		me->WriteLocator(mutUsage, writer, group, cate);
		writer.WriteLine(CSTR("<br/>"));
		s = Text::XML::ToNewHTMLBodyText(group->descript->v);
		writer.Write(s->ToCString());
		s->Release();
		writer.WriteLine(CSTR("<br/>"));
		writer.WriteLine(CSTR("<hr/>"));

		Data::ArrayListDbl speciesIndice;
		Data::ArrayListNN<SpeciesInfo> speciesObjs;
		Data::ArrayListDbl groupIndice;
		Data::ArrayListNN<GroupInfo> groupObjs;
		sb.ClearStr();
		sb.Append(searchStr);
		sb.Trim();
		writer.Write(CSTR("Result for \""));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.Write(s->ToCString());
		s->Release();
		writer.WriteLine(CSTR("\"<br/>"));
		me->env->SearchInGroup(mutUsage, group, sb.ToString(), sb.GetLength(), speciesIndice, speciesObjs, groupIndice, groupObjs, env.user);

		Bool found = false;

		if (speciesObjs.GetCount() > 0)
		{
			Data::ArrayListNN<SpeciesInfo> speciesList;
			NN<SpeciesInfo> sp;
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
			me->WriteSpeciesTable(mutUsage, writer, speciesList, env.scnWidth, group->cateId, false, (env.user.SetTo(user) && user->userType == UserType::Admin));
			if (j > 0)
			{
				sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, searchStr->v);
				writer.Write(CSTR("<a href="));
				sb.ClearStr();
				sb.AppendC(UTF8STRC("searchinsidemores.html?id="));
				sb.AppendI32(group->id);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(group->cateId);
				sb.AppendC(UTF8STRC("&pageNo=1"));
				sb.AppendC(UTF8STRC("&searchStr="));
				sb.AppendC(sbuff, (UIntOS)(sptr - sbuff));
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer.Write(s->ToCString());
				s->Release();
				writer.Write(CSTR(">More</a><br/>"));
			}
			writer.WriteLine(CSTR("<hr/>"));
			found = true;
		}
		if (groupObjs.GetCount() > 0)
		{
			Data::ArrayListNN<GroupInfo> groupList;
			NN<GroupInfo> group;
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
			me->WriteGroupTable(mutUsage, writer, groupList, env.scnWidth, false, env.user.SetTo(user) && user->userType == UserType::Admin);
			if (groupList.GetLast().SetTo(group))
			{
				if (j > 0)
				{
					sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, searchStr->v);
					writer.Write(CSTR("<a href="));
					sb.ClearStr();
					sb.AppendC(UTF8STRC("searchinsidemoreg.html?id="));
					sb.AppendI32(group->id);
					sb.AppendC(UTF8STRC("&cateId="));
					sb.AppendI32(group->cateId);
					sb.AppendC(UTF8STRC("&searchStr="));
					sb.AppendC(sbuff, (UIntOS)(sptr - sbuff));
					s = Text::XML::ToNewAttrText(sb.ToString());
					writer.Write(s->ToCString());
					s->Release();
					writer.Write(CSTR(">More</a><br/>"));
				}
			}
			writer.WriteLine(CSTR("<hr/>"));
			found = true;
		}
		if (!found)
		{
			writer.WriteLine(CSTR("No object found<br/>"));
		}
		writer.WriteLine(CSTR("<br/>"));
		writer.Write(CSTR("<a href=\"group.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&cateId="));
		sb.AppendI32(group->cateId);
		writer.Write(sb.ToCString());
		writer.Write(CSTR("\">"));
		writer.Write(LangGetValue(lang, CSTR("Back")));
		writer.Write(CSTR("</a>"));

		me->WriteFooter(writer);
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

Bool __stdcall SSWR::OrganWeb::OrganWebMainController::SvcSearchInsideMoreS(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebMainController> me = NN<SSWR::OrganWeb::OrganWebMainController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	Int32 id;
	UInt32 pageNo;
	Int32 cateId;
	NN<Text::String> searchStr;
	if (req->GetQueryValueI32(CSTR("id"), id) &&
		req->GetQueryValueI32(CSTR("cateId"), cateId) &&
		req->GetQueryValueU32(CSTR("pageNo"), pageNo) &&
		req->GetQueryValue(CSTR("searchStr")).SetTo(searchStr))
	{
		NN<Text::String> s;
		UIntOS i;
		UIntOS j;
		Text::StringBuilderUTF8 sb;
		NN<GroupInfo> group;
		NN<CategoryInfo> cate;
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		NN<WebUserInfo> user;
		Optional<IO::ConfigFile> lang = me->env->LangGet(req);
		Sync::RWMutexUsage mutUsage;
		Bool notAdmin = (!env.user.SetTo(user) || user->userType != UserType::Admin);
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
		me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
		writer.Write(CSTR("<center><h1>"));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.Write(s->ToCString());
		s->Release();
		writer.WriteLine(CSTR("</h1></center>"));

		writer.WriteLine(CSTR("<table border=\"0\" width=\"100%\">"));
		writer.WriteLine(CSTR("<tr>"));
		writer.Write(CSTR("<td><form method=\"POST\" action=\"searchinside.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&cateId="));
		sb.AppendI32(group->cateId);
		writer.Write(sb.ToCString());
		writer.Write(CSTR("\">"));
		writer.Write(CSTR("Search inside: <input type=\"text\" name=\"searchStr\"/><input type=\"submit\"/>"));
		writer.WriteLine(CSTR("</form></td>"));
		writer.WriteLine(CSTR("</tr>"));
		writer.WriteLine(CSTR("</table>"));

		me->WriteLocator(mutUsage, writer, group, cate);
		writer.WriteLine(CSTR("<br/>"));
		s = Text::XML::ToNewHTMLBodyText(group->descript->v);
		writer.Write(s->ToCString());
		s->Release();
		writer.WriteLine(CSTR("<br/>"));
		writer.WriteLine(CSTR("<hr/>"));

		Data::ArrayListDbl speciesIndice;
		Data::ArrayListNN<SpeciesInfo> speciesObjs;
		Data::ArrayListDbl groupIndice;
		Data::ArrayListNN<GroupInfo> groupObjs;
		sb.ClearStr();
		sb.Append(searchStr);
		sb.Trim();
		writer.Write(CSTR("Result for \""));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.Write(s->ToCString());
		s->Release();
		writer.WriteLine(CSTR("\"<br/>"));
		me->env->SearchInGroup(mutUsage, group, sb.ToString(), sb.GetLength(), speciesIndice, speciesObjs, groupIndice, groupObjs, env.user);

		Bool found = false;

		if (speciesObjs.GetCount() > pageNo * 50)
		{
			Data::ArrayListNN<SpeciesInfo> speciesList;
			NN<SpeciesInfo> sp;
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
			me->WriteSpeciesTable(mutUsage, writer, speciesList, env.scnWidth, group->cateId, false, (env.user.SetTo(user) && user->userType == UserType::Admin));
			if (pageNo > 0)
			{
				sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, searchStr->v);
				writer.Write(CSTR("<a href="));
				sb.ClearStr();
				sb.AppendC(UTF8STRC("searchinsidemores.html?id="));
				sb.AppendI32(group->id);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(group->cateId);
				sb.AppendC(UTF8STRC("&pageNo="));
				sb.AppendU32(pageNo - 1);
				sb.AppendC(UTF8STRC("&searchStr="));
				sb.AppendC(sbuff, (UIntOS)(sptr - sbuff));
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer.Write(s->ToCString());
				s->Release();
				writer.Write(CSTR(">Prev</a> "));
			}
			if (i > 0)
			{
				sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, searchStr->v);
				writer.Write(CSTR(" <a href="));
				sb.ClearStr();
				sb.AppendC(UTF8STRC("searchinsidemores.html?id="));
				sb.AppendI32(group->id);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(group->cateId);
				sb.AppendC(UTF8STRC("&pageNo="));
				sb.AppendU32(pageNo + 1);
				sb.AppendC(UTF8STRC("&searchStr="));
				sb.AppendC(sbuff, (UIntOS)(sptr - sbuff));
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer.Write(s->ToCString());
				s->Release();
				writer.Write(CSTR(">Next</a>"));
			}
			writer.WriteLine(CSTR("<br/><hr/>"));
			found = true;
		}
		if (!found)
		{
			writer.WriteLine(CSTR("No object found<br/>"));
		}
		writer.WriteLine(CSTR("<br/>"));
		writer.Write(CSTR("<a href=\"group.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&cateId="));
		sb.AppendI32(group->cateId);
		writer.Write(sb.ToCString());
		writer.Write(CSTR("\">"));
		writer.Write(LangGetValue(lang, CSTR("Back")));
		writer.Write(CSTR("</a>"));

		me->WriteFooter(writer);
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

Bool __stdcall SSWR::OrganWeb::OrganWebMainController::SvcSearchInsideMoreG(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebMainController> me = NN<SSWR::OrganWeb::OrganWebMainController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	Int32 id;
	UInt32 pageNo;
	Int32 cateId;
	NN<Text::String> searchStr;
	if (req->GetQueryValueI32(CSTR("id"), id) &&
		req->GetQueryValueI32(CSTR("cateId"), cateId) &&
		req->GetQueryValueU32(CSTR("pageNo"), pageNo) &&
		req->GetQueryValue(CSTR("searchStr")).SetTo(searchStr))
	{
		NN<Text::String> s;
		UIntOS i;
		UIntOS j;
		Text::StringBuilderUTF8 sb;
		NN<GroupInfo> group;
		NN<CategoryInfo> cate;
		NN<WebUserInfo> user;
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		Optional<IO::ConfigFile> lang = me->env->LangGet(req);
		Sync::RWMutexUsage mutUsage;
		Bool notAdmin = (!env.user.SetTo(user) || user->userType != UserType::Admin);
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
		me->WriteHeader(writer, sb.ToString(), env.user, env.isMobile);
		writer.Write(CSTR("<center><h1>"));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.Write(s->ToCString());
		s->Release();
		writer.WriteLine(CSTR("</h1></center>"));

		writer.WriteLine(CSTR("<table border=\"0\" width=\"100%\">"));
		writer.WriteLine(CSTR("<tr>"));
		writer.Write(CSTR("<td><form method=\"POST\" action=\"searchinside.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&cateId="));
		sb.AppendI32(group->cateId);
		writer.Write(sb.ToCString());
		writer.Write(CSTR("\">"));
		writer.Write(CSTR("Search inside: <input type=\"text\" name=\"searchStr\"/><input type=\"submit\"/>"));
		writer.WriteLine(CSTR("</form></td>"));
		writer.WriteLine(CSTR("</tr>"));
		writer.WriteLine(CSTR("</table>"));

		me->WriteLocator(mutUsage, writer, group, cate);
		writer.WriteLine(CSTR("<br/>"));
		s = Text::XML::ToNewHTMLBodyText(group->descript->v);
		writer.Write(s->ToCString());
		s->Release();
		writer.WriteLine(CSTR("<br/>"));
		writer.WriteLine(CSTR("<hr/>"));

		Data::ArrayListDbl speciesIndice;
		Data::ArrayListNN<SpeciesInfo> speciesObjs;
		Data::ArrayListDbl groupIndice;
		Data::ArrayListNN<GroupInfo> groupObjs;
		sb.ClearStr();
		sb.Append(searchStr);
		sb.Trim();
		writer.Write(CSTR("Result for \""));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.Write(s->ToCString());
		s->Release();
		writer.WriteLine(CSTR("\"<br/>"));
		me->env->SearchInGroup(mutUsage, group, sb.ToString(), sb.GetLength(), speciesIndice, speciesObjs, groupIndice, groupObjs, env.user);

		Bool found = false;

		if (groupObjs.GetCount() > 0)
		{
			Data::ArrayListNN<GroupInfo> groupList;
			NN<GroupInfo> group;
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
			me->WriteGroupTable(mutUsage, writer, groupList, env.scnWidth, false, env.user.SetTo(user) && user->userType == UserType::Admin);
			if (groupList.GetLast().SetTo(group))
			{
				if (pageNo > 0)
				{
					sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, searchStr->v);
					writer.Write(CSTR("<a href="));
					sb.ClearStr();
					sb.AppendC(UTF8STRC("searchinsidemoreg.html?id="));
					sb.AppendI32(group->id);
					sb.AppendC(UTF8STRC("&cateId="));
					sb.AppendI32(group->cateId);
					sb.AppendC(UTF8STRC("&pageNo="));
					sb.AppendU32(pageNo - 1);
					sb.AppendC(UTF8STRC("&searchStr="));
					sb.AppendC(sbuff, (UIntOS)(sptr - sbuff));
					s = Text::XML::ToNewAttrText(sb.ToString());
					writer.Write(s->ToCString());
					s->Release();
					writer.Write(CSTR(">Prev</a> "));
				}
				if (i > 0)
				{
					sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, searchStr->v);
					writer.Write(CSTR(" <a href="));
					sb.ClearStr();
					sb.AppendC(UTF8STRC("searchinsidemoreg.html?id="));
					sb.AppendI32(group->id);
					sb.AppendC(UTF8STRC("&cateId="));
					sb.AppendI32(group->cateId);
					sb.AppendC(UTF8STRC("&pageNo="));
					sb.AppendU32(pageNo + 1);
					sb.AppendC(UTF8STRC("&searchStr="));
					sb.AppendC(sbuff, (UIntOS)(sptr - sbuff));
					s = Text::XML::ToNewAttrText(sb.ToString());
					writer.Write(s->ToCString());
					s->Release();
					writer.Write(CSTR(">Next</a>"));
				}
			}
			writer.WriteLine(CSTR("<hr/>"));
			found = true;
		}
		if (!found)
		{
			writer.WriteLine(CSTR("No object found<br/>"));
		}
		writer.WriteLine(CSTR("<br/>"));
		writer.Write(CSTR("<a href=\"group.html?id="));
		sb.ClearStr();
		sb.AppendI32(group->id);
		sb.AppendC(UTF8STRC("&cateId="));
		sb.AppendI32(group->cateId);
		writer.Write(sb.ToCString());
		writer.Write(CSTR("\">"));
		writer.Write(LangGetValue(lang, CSTR("Back")));
		writer.Write(CSTR("</a>"));

		me->WriteFooter(writer);
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

Bool __stdcall SSWR::OrganWeb::OrganWebMainController::SvcLogout(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebMainController> me = NN<SSWR::OrganWeb::OrganWebMainController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	me->sessMgr->DeleteSession(req, resp);
	resp->RedirectURL(req, CSTR("/"), 0);
	return true;
}

SSWR::OrganWeb::OrganWebMainController::OrganWebMainController(NN<Net::WebServer::MemoryWebSessionManager> sessMgr, NN<OrganWebEnv> env, UInt32 scnSize) : OrganWebController(sessMgr, env, scnSize)
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
