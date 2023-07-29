#include "Stdafx.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "SSWR/OrganWeb/OrganWebController.h"
#include "SSWR/OrganWeb/OrganWebEnv.h"
#include "Text/XML.h"

#define PREVIEW_SIZE 320
#define SP_PER_PAGE_DESKTOP 100
#define SP_PER_PAGE_MOBILE 90

UOSInt SSWR::OrganWeb::OrganWebController::GetPreviewSize()
{
	return PREVIEW_SIZE;
}

UOSInt SSWR::OrganWeb::OrganWebController::GetPerPage(Bool isMobile)
{
	if (isMobile)
	{
		return SP_PER_PAGE_MOBILE;
	}
	else
	{
		return SP_PER_PAGE_DESKTOP;
	}
}

Net::WebServer::IWebSession *SSWR::OrganWeb::OrganWebController::ParseRequestEnv(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, RequestEnv *env, Bool keepSess)
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

void SSWR::OrganWeb::OrganWebController::ResponseMstm(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, IO::MemoryStream *mstm, Text::CString contType)
{
	resp->AddDefHeaders(req);
	resp->AddContentType(contType);
	mstm->SeekFromBeginning(0);
	Net::WebServer::HTTPServerUtil::SendContent(req, resp, contType, mstm->GetLength(), mstm);
}


void SSWR::OrganWeb::OrganWebController::WriteHeaderPart1(IO::Writer *writer, const UTF8Char *title, Bool isMobile)
{
	NotNullPtr<Text::String> s;
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

void SSWR::OrganWeb::OrganWebController::WriteHeaderPart2(IO::Writer *writer, WebUserInfo *user, const UTF8Char *onLoadFunc)
{
	NotNullPtr<Text::String> s;
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

void SSWR::OrganWeb::OrganWebController::WriteHeader(IO::Writer *writer, const UTF8Char *title, WebUserInfo *user, Bool isMobile)
{
	this->WriteHeaderPart1(writer, title, isMobile);
	this->WriteHeaderPart2(writer, user, 0);
}

void SSWR::OrganWeb::OrganWebController::WriteFooter(IO::Writer *writer)
{
	writer->WriteLineC(UTF8STRC("</BODY>"));
	writer->WriteLineC(UTF8STRC("</HTML>"));
}

void SSWR::OrganWeb::OrganWebController::WriteLocator(NotNullPtr<Sync::RWMutexUsage> mutUsage, IO::Writer *writer, GroupInfo *group, CategoryInfo *cate)
{
	GroupTypeInfo *grpType;
	NotNullPtr<Text::String> s;
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

void SSWR::OrganWeb::OrganWebController::WriteLocatorText(NotNullPtr<Sync::RWMutexUsage> mutUsage, IO::Writer *writer, GroupInfo *group, CategoryInfo *cate)
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

void SSWR::OrganWeb::OrganWebController::WriteGroupTable(NotNullPtr<Sync::RWMutexUsage> mutUsage, IO::Writer *writer, NotNullPtr<const Data::ReadingList<GroupInfo *>> groupList, UInt32 scnWidth, Bool showSelect, Bool showAll)
{
	GroupInfo *group;
	NotNullPtr<Text::String> s;
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
			if (showAll || group->totalCount != 0 || showSelect)
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

void SSWR::OrganWeb::OrganWebController::WriteSpeciesTable(NotNullPtr<Sync::RWMutexUsage> mutUsage, IO::Writer *writer, NotNullPtr<const Data::ArrayList<SpeciesInfo *>> spList, UInt32 scnWidth, Int32 cateId, Bool showSelect, Bool showModify)
{
	SpeciesInfo *sp;
	NotNullPtr<Text::String> s;
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
				if ((sp->flags & SSWR::OrganWeb::SF_HAS_MYPHOTO) == 0)
				{
					this->env->SpeciesSetFlags(mutUsage, sp->speciesId, (SSWR::OrganWeb::SpeciesFlags)(sp->flags | SSWR::OrganWeb::SF_HAS_MYPHOTO));
					this->env->GroupAddCounts(mutUsage, sp->groupId, 0, (sp->flags & SSWR::OrganWeb::SF_HAS_WEBPHOTO)?0:1, 1);
				}
			}
			else
			{
				if (sp->flags & SSWR::OrganWeb::SF_HAS_MYPHOTO)
				{
					this->env->SpeciesSetFlags(mutUsage, sp->speciesId, (SSWR::OrganWeb::SpeciesFlags)(sp->flags & ~SSWR::OrganWeb::SF_HAS_MYPHOTO));
					this->env->GroupAddCounts(mutUsage, sp->groupId, 0, (sp->flags & SSWR::OrganWeb::SF_HAS_WEBPHOTO)?0:(UOSInt)-1, (UOSInt)-1);
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
			else if (sp->photo && sp->photo->leng > 0)
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
			if (showSelect || showModify)
			{
				writer->WriteLineC(UTF8STRC("</a>"));
				if (showSelect)
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("<input type=\"checkbox\" name=\"species"));
					sb.AppendI32(sp->speciesId);
					sb.AppendC(UTF8STRC("\" id=\"species"));
					sb.AppendI32(sp->speciesId);
					sb.AppendC(UTF8STRC("\" value=\"1\"/><label for=\"species"));
					sb.AppendI32(sp->speciesId);
					sb.AppendC(UTF8STRC("\">"));
					writer->WriteStrC(sb.ToString(), sb.GetLength());
				}
				sb.ClearStr();
				sb.Append(sp->sciName);
				sb.AppendC(UTF8STRC(" "));
				sb.Append(sp->chiName);
				sb.AppendC(UTF8STRC(" "));
				sb.Append(sp->engName);
				s = Text::XML::ToNewHTMLBodyText(sb.ToString());
				writer->WriteStrC(s->v, s->leng);
				s->Release();
				if (showSelect)
				{
					writer->WriteLineC(UTF8STRC("</label>"));
				}

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

void SSWR::OrganWeb::OrganWebController::WritePickObjs(NotNullPtr<Sync::RWMutexUsage> mutUsage, IO::Writer *writer, RequestEnv *env, const UTF8Char *url, Bool allowMerge)
{
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;
	NotNullPtr<Text::String> s;
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
					sb.AppendDouble(userFile->lat);
					sb.AppendC(UTF8STRC(","));
					sb.AppendDouble(userFile->lon);
					sb.AppendC(UTF8STRC("/@"));
					sb.AppendDouble(userFile->lat);
					sb.AppendC(UTF8STRC(","));
					sb.AppendDouble(userFile->lon);
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
		WriteSpeciesTable(mutUsage, writer, spList, scnSize, 0, true, true);
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
		WriteGroupTable(mutUsage, writer, groupList, scnSize, true, true);
		writer->WriteLineC(UTF8STRC("<input type=\"submit\" value=\"Place Selected\"/>"));
		writer->WriteLineC(UTF8STRC("<input type=\"button\" value=\"Place All\" onclick=\"document.forms.pickfiles.action.value='placeall';document.forms.pickfiles.submit();\"/>"));
		writer->WriteLineC(UTF8STRC("</form>"));
		writer->WriteLineC(UTF8STRC("<hr/>"));
	}
}

void SSWR::OrganWeb::OrganWebController::WriteDataFiles(IO::Writer *writer, Data::FastMap<Data::Timestamp, DataFileInfo*> *fileMap, Int64 startTimeTicks, Int64 endTimeTicks)
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

Text::CString SSWR::OrganWeb::OrganWebController::LangGetValue(IO::ConfigFile *lang, const UTF8Char *name, UOSInt nameLen)
{
	if (lang == 0)
		return {name, nameLen};
	Text::String *s = lang->GetValue({name, nameLen});
	if (s)
		return s->ToCString();
	return {name, nameLen};
}

SSWR::OrganWeb::OrganWebController::OrganWebController(Net::WebServer::MemoryWebSessionManager *sessMgr, OrganWebEnv *env, UInt32 scnSize) : Net::WebServer::WebController(CSTR(""))
{
	this->sessMgr = sessMgr;
	this->env = env;
	this->scnSize = scnSize;
}

SSWR::OrganWeb::OrganWebController::~OrganWebController()
{

}
