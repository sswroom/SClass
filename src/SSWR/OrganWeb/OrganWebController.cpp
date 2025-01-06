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

Optional<Net::WebServer::IWebSession> SSWR::OrganWeb::OrganWebController::ParseRequestEnv(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, NN<RequestEnv> env, Bool keepSess)
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
	NN<Net::WebServer::IWebSession> sess;
	if (this->sessMgr->GetSession(req, resp).SetTo(sess))
	{
		Data::DateTime *t;
		env->user = (WebUserInfo*)sess->GetValuePtr(CSTR("User"));
		env->pickObjType = (PickObjType)sess->GetValueInt32(CSTR("PickObjType"));
		env->pickObjs = (Data::ArrayListInt32*)sess->GetValuePtr(CSTR("PickObjs"));
		t = (Data::DateTime*)sess->GetValuePtr(CSTR("LastUseTime"));
		t->SetCurrTimeUTC();
		if (keepSess)
		{
			return sess.Ptr();
		}
		sess->EndUse();
	}
	else
	{
		env->pickObjType = PickObjType::POT_UNKNOWN;
		env->pickObjs = 0;
	}
	return 0;
}

void SSWR::OrganWeb::OrganWebController::ResponseMstm(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, NN<IO::MemoryStream> mstm, Text::CStringNN contType)
{
	resp->AddDefHeaders(req);
	resp->AddContentType(contType);
	mstm->SeekFromBeginning(0);
	Net::WebServer::HTTPServerUtil::SendContent(req, resp, contType, mstm->GetLength(), mstm);
}


void SSWR::OrganWeb::OrganWebController::WriteHeaderPart1(NN<IO::Writer> writer, UnsafeArray<const UTF8Char> title, Bool isMobile)
{
	NN<Text::String> s;
	writer->WriteLine(CSTR("<HTML>"));
	writer->WriteLine(CSTR("<HEAD>"));
	writer->WriteLine(CSTR("<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=utf8\">"));
	if (isMobile)
	{
		writer->WriteLine(CSTR("<meta name=\"viewport\" content=\"width=1024\">"));
	}
	writer->Write(CSTR("<title>"));
	s = Text::XML::ToNewHTMLElementText(title);
	writer->Write(s->ToCString());
	s->Release();
	writer->WriteLine(CSTR("</title>"));
}

void SSWR::OrganWeb::OrganWebController::WriteHeaderPart2(NN<IO::Writer> writer, Optional<WebUserInfo> user, UnsafeArrayOpt<const UTF8Char> onLoadFunc)
{
	NN<Text::String> s;
	writer->WriteLine(CSTR("</HEAD>"));
	writer->WriteLine();
	writer->Write(CSTR("<BODY TEXT=\"#c0e0ff\" LINK=\"#6080ff\" VLINK=\"#4060ff\" ALINK=\"#4040FF\" bgcolor=\"#000000\""));
	UnsafeArray<const UTF8Char> nnonLoadFunc;
	if (onLoadFunc.SetTo(nnonLoadFunc))
	{
		writer->Write(CSTR(" onLoad="));
		s = Text::XML::ToNewAttrText(nnonLoadFunc);
		writer->Write(s->ToCString());
		s->Release();
	}
	writer->WriteLine(CSTR(">"));
	NN<WebUserInfo> nnuser;
	if (user.SetTo(nnuser))
	{
		writer->Write(CSTR("<p align=\"right\">"));
		s = Text::XML::ToNewHTMLBodyText(nnuser->userName->v);
		writer->Write(s->ToCString());
		s->Release();
		writer->WriteLine(CSTR("<a href=\"logout\">Logout</a></p>"));
	}
}

void SSWR::OrganWeb::OrganWebController::WriteHeader(NN<IO::Writer> writer, UnsafeArray<const UTF8Char> title, Optional<WebUserInfo> user, Bool isMobile)
{
	this->WriteHeaderPart1(writer, title, isMobile);
	this->WriteHeaderPart2(writer, user, 0);
}

void SSWR::OrganWeb::OrganWebController::WriteFooter(NN<IO::Writer> writer)
{
	writer->WriteLine(CSTR("</BODY>"));
	writer->WriteLine(CSTR("</HTML>"));
}

void SSWR::OrganWeb::OrganWebController::WriteLocator(NN<Sync::RWMutexUsage> mutUsage, NN<IO::Writer> writer, NN<GroupInfo> group, NN<CategoryInfo> cate)
{
	NN<GroupTypeInfo> grpType;
	NN<Text::String> s;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[12];
	UnsafeArray<UTF8Char> sptr;
	Data::ArrayListNN<GroupInfo> groupList;
	UOSInt i;
	while (true)
	{
		groupList.Add(group);
		if (!this->env->GroupGet(mutUsage, group->parentId).SetTo(group))
			break;
	}


	writer->WriteLine(CSTR("<center><table border=\"0\" cellpadding=\"0\" cellspacing=\"4\">"));
	writer->Write(CSTR("<tr><td></td><td></td><td><a href=\"/\">Index"));
	writer->Write(CSTR("</a></td></tr>"));

	writer->Write(CSTR("<tr><td></td><td></td><td><a href="));
	sb.ClearStr();
	sb.AppendC(UTF8STRC("cate.html?cateName="));
	sb.Append(cate->dirName);
	s = Text::XML::ToNewAttrText(sb.ToString());
	writer->Write(s->ToCString());
	s->Release();
	writer->Write(CSTR(">"));
	s = Text::XML::ToNewHTMLBodyText(cate->chiName->v);
	writer->Write(s->ToCString());
	s->Release();
	writer->Write(CSTR("</a></td></tr>"));

	i = groupList.GetCount();
	while (i-- > 0)
	{
		if (groupList.GetItem(i).SetTo(group))
		{
			writer->WriteLine(CSTR("<tr>"));
			if (cate->groupTypes.Get(group->groupType).SetTo(grpType))
			{
				writer->Write(CSTR("<td>"));
				s = Text::XML::ToNewHTMLBodyText(grpType->chiName->v);
				writer->Write(s->ToCString());
				s->Release();
				writer->WriteLine(CSTR("</td>"));
				writer->Write(CSTR("<td>"));
				s = Text::XML::ToNewHTMLBodyText(grpType->engName->v);
				writer->Write(s->ToCString());
				s->Release();
				writer->WriteLine(CSTR("</td>"));
			}
			else
			{
				writer->WriteLine(CSTR("<td>?</td>"));
				writer->WriteLine(CSTR("<td>?</td>"));
			}
			writer->Write(CSTR("<td><a href=\"group.html?id="));
			sptr = Text::StrInt32(sbuff, group->id);
			writer->Write(CSTRP(sbuff, sptr));
			writer->Write(CSTR("&amp;cateId="));
			sptr = Text::StrInt32(sbuff, group->cateId);
			writer->Write(CSTRP(sbuff, sptr));
			writer->Write(CSTR("\">"));
			sb.ClearStr();
			sb.Append(group->engName);
			sb.AppendC(UTF8STRC(" "));
			sb.Append(group->chiName);
			s = Text::XML::ToNewHTMLBodyText(sb.ToString());
			writer->Write(s->ToCString());
			s->Release();
			writer->WriteLine(CSTR("</a></td>"));
			writer->WriteLine(CSTR("</tr>"));
		}
	}
	writer->WriteLine(CSTR("</table>"));
	WriteLocatorText(mutUsage, writer, group, cate);
	writer->WriteLine(CSTR("</center>"));
}

void SSWR::OrganWeb::OrganWebController::WriteLocatorText(NN<Sync::RWMutexUsage> mutUsage, NN<IO::Writer> writer, NN<GroupInfo> group, NN<CategoryInfo> cate)
{
	Text::StringBuilderUTF8 sb;
	Data::ArrayListNN<GroupInfo> groupList;
	UOSInt i;
	Bool found = false;
	while (true)
	{
		groupList.Add(group);
		if (group->groupType == 21)
		{
			found = true;
			break;
		}
		if (!this->env->GroupGet(mutUsage, group->parentId).SetTo(group))
		{
			break;
		}
	}

	if (!found)
		return;

	found = false;
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Text Form: "));
	i = groupList.GetCount();
	while (i-- > 0)
	{
		if (groupList.GetItem(i).SetTo(group))
		{
			if (found)
			{
				sb.AppendC(UTF8STRC(", "));
			}
			sb.Append(group->engName);
			found = true;
		}
	}
	sb.AppendC(UTF8STRC("<br/>"));
	writer->WriteLine(sb.ToCString());
}

void SSWR::OrganWeb::OrganWebController::WriteGroupTable(NN<Sync::RWMutexUsage> mutUsage, NN<IO::Writer> writer, NN<const Data::ReadingList<Optional<GroupInfo>>> groupList, UInt32 scnWidth, Bool showSelect, Bool showAll)
{
	NN<GroupInfo> group;
	NN<Text::String> s;
	NN<SpeciesInfo> photoSpObj;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
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
		writer->WriteLine(CSTR("<table border=\"0\" width=\"100%\">"));
		currColumn = 0;
		while (i < j)
		{
			if (groupList->GetItem(i).SetTo(group))
			{
				this->env->CalcGroupCount(mutUsage, group);
				if (showAll || group->totalCount != 0 || showSelect)
				{
					if (currColumn == 0)
					{
						writer->WriteLine(CSTR("<tr>"));
					}
					sb.ClearStr();
					sb.AppendC(UTF8STRC("<td width=\""));
					sb.AppendU32(colWidth);
					sb.AppendC(UTF8STRC("%\">"));
					writer->WriteLine(sb.ToCString());
					sb.ClearStr();
					sb.AppendC(UTF8STRC("<center><a href=\"group.html?id="));
					sb.AppendI32(group->id);
					sb.AppendC(UTF8STRC("&amp;cateId="));
					sb.AppendI32(group->cateId);
					sb.AppendC(UTF8STRC("\">"));
					writer->WriteLine(sb.ToCString());

					if (group->photoSpObj.SetTo(photoSpObj) && (!photoSpObj->photo.IsNull() || photoSpObj->photoId != 0 || photoSpObj->photoWId != 0))
					{
						if (photoSpObj->photoId != 0)
						{
							writer->Write(CSTR("<img src="));
							sb.ClearStr();
							sb.AppendC(UTF8STRC("photo.html?id="));
							sb.AppendI32(photoSpObj->speciesId);
							sb.AppendC(UTF8STRC("&cateId="));
							sb.AppendI32(group->cateId);
							sb.AppendC(UTF8STRC("&width="));
							sb.AppendI32(PREVIEW_SIZE);
							sb.AppendC(UTF8STRC("&height="));
							sb.AppendI32(PREVIEW_SIZE);
							sb.AppendC(UTF8STRC("&fileId="));
							sb.AppendI32(photoSpObj->photoId);
							s = Text::XML::ToNewAttrText(sb.ToString());
							writer->Write(s->ToCString());
							s->Release();
							writer->Write(CSTR(" border=\"0\" ALT="));
							s = Text::XML::ToNewAttrText(group->engName->v);
							writer->Write(s->ToCString());
							s->Release();
							writer->WriteLine(CSTR("><br/>"));
						}
						else if (photoSpObj->photoWId != 0)
						{
							writer->Write(CSTR("<img src="));
							sb.ClearStr();
							sb.AppendC(UTF8STRC("photo.html?id="));
							sb.AppendI32(photoSpObj->speciesId);
							sb.AppendC(UTF8STRC("&cateId="));
							sb.AppendI32(group->cateId);
							sb.AppendC(UTF8STRC("&width="));
							sb.AppendI32(PREVIEW_SIZE);
							sb.AppendC(UTF8STRC("&height="));
							sb.AppendI32(PREVIEW_SIZE);
							sb.AppendC(UTF8STRC("&fileWId="));
							sb.AppendI32(photoSpObj->photoWId);
							s = Text::XML::ToNewAttrText(sb.ToString());
							writer->Write(s->ToCString());
							s->Release();
							writer->Write(CSTR(" border=\"0\" ALT="));
							s = Text::XML::ToNewAttrText(group->engName->v);
							writer->Write(s->ToCString());
							s->Release();
							writer->WriteLine(CSTR("><br/>"));
						}
						else
						{
							writer->Write(CSTR("<img src="));
							sb.ClearStr();
							sb.AppendC(UTF8STRC("photo.html?id="));
							sb.AppendI32(photoSpObj->speciesId);
							sb.AppendC(UTF8STRC("&cateId="));
							sb.AppendI32(group->cateId);
							sb.AppendC(UTF8STRC("&width="));
							sb.AppendI32(PREVIEW_SIZE);
							sb.AppendC(UTF8STRC("&height="));
							sb.AppendI32(PREVIEW_SIZE);
							sb.AppendC(UTF8STRC("&file="));
							sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, Text::String::OrEmpty(photoSpObj->photo)->v);
							sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
							s = Text::XML::ToNewAttrText(sb.ToString());
							writer->Write(s->ToCString());
							s->Release();
							writer->Write(CSTR(" border=\"0\" ALT="));
							s = Text::XML::ToNewAttrText(group->engName->v);
							writer->Write(s->ToCString());
							s->Release();
							writer->WriteLine(CSTR("><br/>"));
						}
					}
					else
					{
						s = Text::XML::ToNewHTMLBodyText(group->engName->v);
						writer->Write(s->ToCString());
						s->Release();
					}
					if (showSelect)
					{
						writer->WriteLine(CSTR("</a>"));
						
						sb.ClearStr();
						sb.AppendC(UTF8STRC("<input type=\"checkbox\" name=\"group"));
						sb.AppendI32(group->id);
						sb.AppendC(UTF8STRC("\" id=\"group"));
						sb.AppendI32(group->id);
						sb.AppendC(UTF8STRC("\" value=\"1\"/><label for=\"group"));
						sb.AppendI32(group->id);
						sb.AppendC(UTF8STRC("\">"));
						writer->Write(sb.ToCString());
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
						writer->Write(s->ToCString());
						writer->WriteLine(CSTR("</label></center></td>"));
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
						writer->Write(s->ToCString());
						writer->WriteLine(CSTR("</a></center></td>"));
						s->Release();
					}

					currColumn++;
					if (currColumn >= colCount)
					{
						writer->WriteLine(CSTR("</tr>"));
						currColumn = 0;
					}
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
				writer->WriteLine(sb.ToCString());
				currColumn++;
			}
			writer->WriteLine(CSTR("</tr>"));
		}
		writer->WriteLine(CSTR("</table>"));
	}
}

void SSWR::OrganWeb::OrganWebController::WriteSpeciesTable(NN<Sync::RWMutexUsage> mutUsage, NN<IO::Writer> writer, NN<const Data::ReadingList<Optional<SpeciesInfo>>> spList, UInt32 scnWidth, Int32 cateId, Bool showSelect, Bool showModify)
{
	NN<SpeciesInfo> sp;
	NN<Text::String> s;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Text::StringBuilderUTF8 sb;
	UInt32 colCount = scnWidth / PREVIEW_SIZE;
	UInt32 colWidth = 100 / colCount;
	UInt32 currColumn;
	UOSInt i = 0;
	UOSInt j = spList->GetCount();
	if (j > 0)
	{
		writer->WriteLine(CSTR("<table border=\"0\" width=\"100%\">"));
		currColumn = 0;
		while (i < j)
		{
			if (spList->GetItem(i).SetTo(sp))
			{
				if (currColumn == 0)
				{
					writer->WriteLine(CSTR("<tr>"));
				}
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<td width=\""));
				sb.AppendU32(colWidth);
				sb.AppendC(UTF8STRC("%\">"));
				writer->WriteLine(sb.ToCString());
				sb.ClearStr();
				if (sp->cateId == cateId || showSelect)
				{
					sb.AppendC(UTF8STRC("<center><a href=\"species.html?id="));
					sb.AppendI32(sp->speciesId);
					sb.AppendC(UTF8STRC("&amp;cateId="));
					sb.AppendI32(sp->cateId);
					sb.AppendC(UTF8STRC("\">"));
					writer->WriteLine(sb.ToCString());
				}
				else
				{
					writer->WriteLine(CSTR("<center>"));
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
					writer->Write(CSTR("<img src="));
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
					writer->Write(s->ToCString());
					s->Release();
					writer->Write(CSTR(" border=\"0\" ALT="));
					s = Text::XML::ToNewAttrText(sp->sciName->v);
					writer->Write(s->ToCString());
					s->Release();
					writer->WriteLine(CSTR("><br/>"));
				}
				else if (sp->photoWId)
				{
					writer->Write(CSTR("<img src="));
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
					writer->Write(s->ToCString());
					s->Release();
					writer->Write(CSTR(" border=\"0\" ALT="));
					s = Text::XML::ToNewAttrText(sp->sciName->v);
					writer->Write(s->ToCString());
					s->Release();
					writer->WriteLine(CSTR("><br/>"));
				}
				else if (sp->photo.SetTo(s) && s->leng > 0)
				{
					writer->Write(CSTR("<img src="));
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
					sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, s->v);
					sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
					s = Text::XML::ToNewAttrText(sb.ToString());
					writer->Write(s->ToCString());
					s->Release();
					writer->Write(CSTR(" border=\"0\" ALT="));
					s = Text::XML::ToNewAttrText(sp->sciName->v);
					writer->Write(s->ToCString());
					s->Release();
					writer->WriteLine(CSTR("><br/>"));
				}
				else
				{
					s = Text::XML::ToNewHTMLBodyText(sp->sciName->v);
					writer->Write(s->ToCString());
					s->Release();
					writer->WriteLine(CSTR("<br/>"));
				}
				if (showSelect || showModify)
				{
					writer->WriteLine(CSTR("</a>"));
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
						writer->Write(sb.ToCString());
					}
					sb.ClearStr();
					sb.Append(sp->sciName);
					sb.AppendC(UTF8STRC(" "));
					sb.Append(sp->chiName);
					sb.AppendC(UTF8STRC(" "));
					sb.Append(sp->engName);
					s = Text::XML::ToNewHTMLBodyText(sb.ToString());
					writer->Write(s->ToCString());
					s->Release();
					if (showSelect)
					{
						writer->WriteLine(CSTR("</label>"));
					}

					sb.ClearStr();
					sb.AppendC(UTF8STRC("<br/><a href=\"speciesmod.html?id="));
					sb.AppendI32(sp->groupId);
					sb.AppendC(UTF8STRC("&amp;cateId="));
					sb.AppendI32(sp->cateId);
					sb.AppendC(UTF8STRC("&amp;spId="));
					sb.AppendI32(sp->speciesId);
					sb.AppendC(UTF8STRC("\">Modify</a>"));
					writer->WriteLine(sb.ToCString());
					writer->WriteLine(CSTR("</center></td>"));
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
					writer->Write(s->ToCString());
					s->Release();
					if (sp->cateId == cateId)
					{
						writer->WriteLine(CSTR("</a></center></td>"));
					}
					else
					{
						writer->WriteLine(CSTR("</center></td>"));
					}
				}

				currColumn++;
				if (currColumn >= colCount)
				{
					writer->WriteLine(CSTR("</tr>"));
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
				writer->WriteLine(sb.ToCString());
				currColumn++;
			}
			writer->WriteLine(CSTR("</tr>"));
		}
		writer->WriteLine(CSTR("</table>"));
	}
}

void SSWR::OrganWeb::OrganWebController::WritePickObjs(NN<Sync::RWMutexUsage> mutUsage, NN<IO::Writer> writer, NN<RequestEnv> env, UnsafeArray<const UTF8Char> url, Bool allowMerge)
{
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;
	NN<Text::String> s;
	UInt32 colCount = env->scnWidth / PREVIEW_SIZE;
	UInt32 colWidth = 100 / colCount;
	UInt32 currColumn;
	NN<WebUserInfo> user;
	NN<UserFileInfo> userFile;
	NN<SpeciesInfo> species;
	Data::DateTime dt;
	UTF8Char sbuff2[64];
	UnsafeArray<UTF8Char> sptr2;
	if (env->pickObjType == POT_USERFILE && env->pickObjs->GetCount() > 0 && env->user.SetTo(user))
	{
		currColumn = 0;
		sb.ClearStr();
		sb.AppendC(UTF8STRC("<form name=\"pickfiles\" action="));
		s = Text::XML::ToNewAttrText(url);
		sb.Append(s);
		s->Release();
		sb.AppendC(UTF8STRC(" method=\"POST\"/>"));
		writer->WriteLine(sb.ToCString());
		writer->WriteLine(CSTR("<input type=\"hidden\" name=\"action\" value=\"place\"/>"));
		writer->WriteLine(CSTR("<table border=\"0\" width=\"100%\">"));

		i = 0;
		j = env->pickObjs->GetCount();
		while (i < j)
		{
			if (this->env->UserfileGet(mutUsage, env->pickObjs->GetItem(i)).SetTo(userFile) && this->env->SpeciesGet(mutUsage, userFile->speciesId).SetTo(species))
			{
				if (currColumn == 0)
				{
					writer->WriteLine(CSTR("<tr>"));
				}
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<td width=\""));
				sb.AppendU32(colWidth);
				sb.AppendC(UTF8STRC("%\">"));
				writer->WriteLine(sb.ToCString());
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
				writer->WriteLine(sb.ToCString());
				s->Release();

				writer->Write(CSTR("<img src="));
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
				if (userFile->rotType != 0)
				{
					sb.AppendC(UTF8STRC("&r="));
					sb.AppendI32(userFile->rotType);
				}
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer->Write(s->ToCString());
				s->Release();
				writer->Write(CSTR(" border=\"0\">"));
				writer->Write(CSTR("</a><br/>"));
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
				writer->Write(sb.ToCString());

				sptr2 = dt.ToString(sbuff2, "yyyy-MM-dd HH:mm:ss zzzz");
				writer->Write(CSTRP(sbuff2, sptr2));
				if (userFile->webuserId == user->id)
				{
					if (userFile->location.SetTo(s))
					{
						writer->Write(CSTR(" "));
						s = Text::XML::ToNewHTMLBodyText(s->v);
						writer->Write(s->ToCString());
						s->Release();
					}
				}
				if (userFile->descript.SetTo(s) && s->leng > 0)
				{
					writer->Write(CSTR("<br/>"));
					s = Text::XML::ToNewHTMLBodyText(s->v);
					writer->Write(s->ToCString());
					s->Release();
				}
				if (userFile->webuserId == user->id)
				{
					writer->Write(CSTR("<br/>"));
					s = Text::XML::ToNewHTMLBodyText(userFile->oriFileName->v);
					writer->Write(s->ToCString());
					s->Release();
				}
				writer->Write(CSTR("</label>"));
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
					writer->Write(sb.ToCString());
				}
				writer->WriteLine(CSTR("</center></td>"));

				currColumn++;
				if (currColumn >= colCount)
				{
					writer->WriteLine(CSTR("</tr>"));
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
				writer->WriteLine(sb.ToCString());
				currColumn++;
			}
			writer->WriteLine(CSTR("</tr>"));
		}
		writer->WriteLine(CSTR("</table>"));
		writer->WriteLine(CSTR("<input type=\"submit\" value=\"Place Selected\"/>"));
		writer->WriteLine(CSTR("<input type=\"button\" value=\"Place All\" onclick=\"document.forms.pickfiles.action.value='placeall';document.forms.pickfiles.submit();\"/>"));
		writer->WriteLine(CSTR("</form>"));
		writer->WriteLine(CSTR("<hr/>"));
	}
	else if (env->pickObjType == POT_SPECIES && env->pickObjs->GetCount() > 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("<form name=\"pickfiles\" action="));
		s = Text::XML::ToNewAttrText(url);
		sb.Append(s);
		s->Release();
		sb.AppendC(UTF8STRC(" method=\"POST\"/>"));
		writer->WriteLine(sb.ToCString());
		writer->WriteLine(CSTR("<input type=\"hidden\" name=\"action\" value=\"place\"/>"));
		NN<SpeciesInfo> species;
		Data::ArrayListNN<SpeciesInfo> spList;
		i = 0;
		j = env->pickObjs->GetCount();
		while (i < j)
		{
			if (this->env->SpeciesGet(mutUsage, env->pickObjs->GetItem(i)).SetTo(species))
			{
				spList.Add(species);
			}
			i++;
		}
		WriteSpeciesTable(mutUsage, writer, spList, scnSize, 0, true, true);
		writer->WriteLine(CSTR("<input type=\"submit\" value=\"Place Selected\"/>"));
		writer->WriteLine(CSTR("<input type=\"button\" value=\"Place All\" onclick=\"document.forms.pickfiles.action.value='placeall';document.forms.pickfiles.submit();\"/>"));
		if (allowMerge)
		{
			writer->WriteLine(CSTR("<input type=\"button\" value=\"Place Merge\" onclick=\"document.forms.pickfiles.action.value='placemerge';document.forms.pickfiles.submit();\"/>"));
		}
		writer->WriteLine(CSTR("</form>"));
		writer->WriteLine(CSTR("<hr/>"));
	}
	else if (env->pickObjType == POT_GROUP && env->pickObjs->GetCount() > 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("<form name=\"pickfiles\" action="));
		s = Text::XML::ToNewAttrText(url);
		sb.Append(s);
		s->Release();
		sb.AppendC(UTF8STRC(" method=\"POST\"/>"));
		writer->WriteLine(sb.ToCString());
		writer->WriteLine(CSTR("<input type=\"hidden\" name=\"action\" value=\"place\"/>"));
		NN<GroupInfo> group;
		Data::ArrayListNN<GroupInfo> groupList;
		i = 0;
		j = env->pickObjs->GetCount();
		while (i < j)
		{
			if (this->env->GroupGet(mutUsage, env->pickObjs->GetItem(i)).SetTo(group))
			{
				groupList.Add(group);
			}
			i++;
		}
		WriteGroupTable(mutUsage, writer, groupList, scnSize, true, true);
		writer->WriteLine(CSTR("<input type=\"submit\" value=\"Place Selected\"/>"));
		writer->WriteLine(CSTR("<input type=\"button\" value=\"Place All\" onclick=\"document.forms.pickfiles.action.value='placeall';document.forms.pickfiles.submit();\"/>"));
		writer->WriteLine(CSTR("</form>"));
		writer->WriteLine(CSTR("<hr/>"));
	}
}

void SSWR::OrganWeb::OrganWebController::WriteDataFiles(NN<IO::Writer> writer, NN<Data::FastMapNN<Data::Timestamp, DataFileInfo>> fileMap, Int64 startTimeTicks, Int64 endTimeTicks)
{
	OSInt startIndex;
	OSInt endIndex;
	Text::StringBuilderUTF8 sb;
	startIndex = fileMap->GetIndex(Data::Timestamp(startTimeTicks, 0));
	if (startIndex < 0)
		startIndex = ~startIndex;
	if (startIndex > 0 && fileMap->GetItemNoCheck((UOSInt)startIndex - 1)->endTime.ToTicks() > startTimeTicks)
		startIndex--;
	endIndex = fileMap->GetIndex(Data::Timestamp(endTimeTicks, 0));
	if (endIndex < 0)
		endIndex = ~endIndex;
	while (startIndex < endIndex)
	{
		NN<DataFileInfo> dataFile = fileMap->GetItemNoCheck((UOSInt)startIndex);
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
		sb.AppendTSNoZone(dataFile->startTime);
		sb.AppendC(UTF8STRC(" - "));
		sb.AppendTSNoZone(dataFile->endTime);
		sb.AppendC(UTF8STRC("<br/>"));
		writer->WriteLine(sb.ToCString());
		startIndex++;
	}
}

Text::CStringNN SSWR::OrganWeb::OrganWebController::LangGetValue(Optional<IO::ConfigFile> lang, Text::CStringNN name)
{
	NN<IO::ConfigFile> nnlang;
	if (!lang.SetTo(nnlang))
		return name;
	NN<Text::String> s;
	if (nnlang->GetValue(name).SetTo(s))
		return s->ToCString();
	return name;
}

SSWR::OrganWeb::OrganWebController::OrganWebController(NN<Net::WebServer::MemoryWebSessionManager> sessMgr, NN<OrganWebEnv> env, UInt32 scnSize) : Net::WebServer::WebController(CSTR(""))
{
	this->sessMgr = sessMgr;
	this->env = env;
	this->scnSize = scnSize;
}

SSWR::OrganWeb::OrganWebController::~OrganWebController()
{

}
