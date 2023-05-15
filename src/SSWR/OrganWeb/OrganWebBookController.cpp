#include "Stdafx.h"
#include "SSWR/OrganWeb/OrganWebBookController.h"
#include "SSWR/OrganWeb/OrganWebEnv.h"
#include "Text/UTF8Writer.h"
#include "Text/XML.h"

Bool __stdcall SSWR::OrganWeb::OrganWebBookController::SvcBookList(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebBookController *me = (SSWR::OrganWeb::OrganWebBookController*)parent;
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
		writer.WriteLineC(UTF8STRC("<td>Count</td>"));
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
			writer.WriteStrC(UTF8STRC("<td>"));
			sptr = Text::StrUOSInt(sbuff, book->species.GetCount());
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteStrC(UTF8STRC("</td>"));
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

Bool __stdcall SSWR::OrganWeb::OrganWebBookController::SvcBook(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebBookController *me = (SSWR::OrganWeb::OrganWebBookController*)parent;
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
		sb.AppendC(UTF8STRC(" - "));
		sb.Append(book->title);
		sb.AppendC(UTF8STRC(" - Page "));
		sb.AppendUOSInt(pageNo + 1);
		me->WriteHeader(&writer, sb.ToString(), env.user, env.isMobile);
		if (book->userfileId != 0)
		{
			UserFileInfo *userFile = me->env->UserfileGet(&mutUsage, book->userfileId);
			if (userFile)
			{
				SpeciesInfo *sp = me->env->SpeciesGet(&mutUsage, userFile->speciesId);
				if (sp)
				{
					writer.WriteStrC(UTF8STRC("<img src="));
					sb.ClearStr();
					sb.AppendC(UTF8STRC("photo.html?id="));
					sb.AppendI32(userFile->speciesId);
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
					writer.WriteStrC(UTF8STRC(" border=\"0\" style=\"float: left;\"/>"));
				}
			}
		}
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
		UOSInt perPage = GetPerPage(env.isMobile);
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
			sb.AppendUOSInt(perPage);
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
			sb.AppendUOSInt(perPage);
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
		me->WriteSpeciesTable(&mutUsage, &writer, &tempList, env.scnWidth, cateId, false, (env.user && env.user->userType == 0));
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

Bool __stdcall SSWR::OrganWeb::OrganWebBookController::SvcBookView(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebBookController *me = (SSWR::OrganWeb::OrganWebBookController*)parent;
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

SSWR::OrganWeb::OrganWebBookController::OrganWebBookController(Net::WebServer::MemoryWebSessionManager *sessMgr, OrganWebEnv *env, UInt32 scnSize) : OrganWebController(sessMgr, env, scnSize)
{
	this->AddService(CSTR("/booklist.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcBookList);
	this->AddService(CSTR("/book.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcBook);
	this->AddService(CSTR("/bookview.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcBookView);
}

SSWR::OrganWeb::OrganWebBookController::~OrganWebBookController()
{
}
