#include "Stdafx.h"
#include "SSWR/OrganWeb/OrganWebBookController.h"
#include "SSWR/OrganWeb/OrganWebEnv.h"
#include "Text/UTF8Writer.h"
#include "Text/XML.h"

Bool __stdcall SSWR::OrganWeb::OrganWebBookController::SvcBookView(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebBookController *me = (SSWR::OrganWeb::OrganWebBookController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	Int32 id;
	if (req->GetQueryValueI32(CSTR("id"), id))
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		BookInfo *book;
		Sync::RWMutexUsage mutUsage;
		book = me->env->BookGet(mutUsage, id);
		if (env.user == 0 || env.user->userType != UserType::Admin)
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
			readSize = fs.Read(BYTEARR(sbuff));
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

Bool __stdcall SSWR::OrganWeb::OrganWebBookController::SvcBookPhoto(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebBookController *me = (SSWR::OrganWeb::OrganWebBookController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	Int32 id;
	Int32 cateId;
	if (req->GetQueryValueI32(CSTR("id"), id) &&
		req->GetQueryValueI32(CSTR("cateId"), cateId))
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		BookInfo *book;
		NotNullPtr<CategoryInfo> cate;
		Sync::RWMutexUsage mutUsage;
		book = me->env->BookGet(mutUsage, id);
		if (env.user == 0 || env.user->userType != UserType::Admin)
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_UNAUTHORIZED);
			return true;
		}
		else if (book == 0 || !me->env->CateGet(mutUsage, cateId).SetTo(cate) || env.pickObjType != PickObjType::POT_USERFILE)
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		Text::StringBuilderUTF8 sb;
		Int32 fileId;
		if (req->GetQueryValueI32(CSTR("fileId"), fileId))
		{
			if (me->env->BookSetPhoto(mutUsage, id, fileId))
			{
				sb.AppendC(UTF8STRC("book.html?id="));
				sb.AppendI32(book->id);
				sb.AppendC(UTF8STRC("&cateId="));
				sb.AppendI32(cate->cateId);
				resp->RedirectURL(req, sb.ToCString(), 0);
			}
		}
		IO::MemoryStream mstm;
		Text::UTF8Writer writer(mstm);
		NotNullPtr<Text::String> s;
		NotNullPtr<Text::String> s2;

		sb.ClearStr();
		sb.Append(cate->chiName);
		sb.AppendC(UTF8STRC(" - "));
		sb.Append(book->title);
		me->WriteHeader(&writer, sb.ToString(), env.user, env.isMobile);
		writer.WriteStrC(UTF8STRC("<center><h1>"));
		s = Text::XML::ToNewHTMLBodyText(sb.ToString());
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("</h1></center>"));

		if (book->userfileId != 0)
		{
			UserFileInfo *userFile = me->env->UserfileGet(mutUsage, book->userfileId);
			if (userFile)
			{
				NotNullPtr<SpeciesInfo> sp;
				if (me->env->SpeciesGet(mutUsage, userFile->speciesId).SetTo(sp))
				{
					writer.WriteStrC(UTF8STRC("<img src="));
					sb.ClearStr();
					sb.AppendC(UTF8STRC("photo.html?id="));
					sb.AppendI32(userFile->speciesId);
					sb.AppendC(UTF8STRC("&cateId="));
					sb.AppendI32(sp->cateId);
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

		writer.WriteStrC(UTF8STRC("<b>Publish Date:</b> "));
		sptr = Data::Timestamp(book->publishDate, 0).ToString(sbuff, "yyyy-MM-dd");
		writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer.WriteLineC(UTF8STRC("<br/>"));

		if (book->url.SetTo(s2))
		{
			writer.WriteStrC(UTF8STRC("<b>URL:</b> <a href="));
			s = Text::XML::ToNewAttrText(s2->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
			writer.WriteStrC(UTF8STRC(">"));
			s = Text::XML::ToNewHTMLBodyText(s2->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();
			writer.WriteLineC(UTF8STRC("</a><br/>"));
		}

		writer.WriteLineC(UTF8STRC("<hr/>"));
		UserFileInfo *userFile;
		UOSInt colCount = env.scnWidth / GetPreviewSize();
		UOSInt colWidth = 100 / colCount;
		UInt32 currColumn;
		UOSInt i;
		UOSInt j;
		i = 0;
		j = env.pickObjs->GetCount();
		if (j > 0)
		{
			writer.WriteLineC(UTF8STRC("<table border=\"0\" width=\"100%\">"));
			currColumn = 0;
			while (i < j)
			{
				userFile = me->env->UserfileGet(mutUsage, env.pickObjs->GetItem(i));
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
				sb.AppendC(UTF8STRC("<center><a href=\"bookphoto.html?id="));
				sb.AppendI32(book->id);
				sb.AppendC(UTF8STRC("&amp;cateId="));
				sb.AppendI32(cate->cateId);
				sb.AppendC(UTF8STRC("&amp;fileId="));
				sb.AppendI32(userFile->id);
				sb.AppendC(UTF8STRC("\">"));
				writer.WriteLineC(sb.ToString(), sb.GetLength());

				writer.WriteStrC(UTF8STRC("<img src="));
				NotNullPtr<SpeciesInfo> sp;
				sb.ClearStr();
				sb.AppendC(UTF8STRC("photo.html?id="));
				sb.AppendI32(userFile->speciesId);
				sb.AppendC(UTF8STRC("&cateId="));
				if (me->env->SpeciesGet(mutUsage, userFile->speciesId).SetTo(sp))
					sb.AppendI32(sp->cateId);
				else
					sb.AppendI32(cate->cateId);
				sb.AppendC(UTF8STRC("&width="));
				sb.AppendUOSInt(GetPreviewSize());
				sb.AppendC(UTF8STRC("&height="));
				sb.AppendUOSInt(GetPreviewSize());
				sb.AppendC(UTF8STRC("&fileId="));
				sb.AppendI32(userFile->id);
				s = Text::XML::ToNewAttrText(sb.ToString());
				writer.WriteStrC(s->v, s->leng);
				s->Release();
				writer.WriteStrC(UTF8STRC(" border=\"0\""));
				writer.WriteLineC(UTF8STRC("><br/>"));

				writer.WriteLineC(UTF8STRC("</a>"));
				writer.WriteLineC(UTF8STRC("</center></td>"));

				currColumn++;
				if (currColumn >= colCount)
				{
					writer.WriteLineC(UTF8STRC("</tr>"));
					currColumn = 0;
				}
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
			writer.WriteLineC(UTF8STRC("</table>"));
		}
		writer.WriteLineC(UTF8STRC("<hr/>"));

		writer.WriteStrC(UTF8STRC("<a href=\"booklist.html?id="));
		sptr = Text::StrInt32(sbuff, cate->cateId);
		writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer.WriteStrC(UTF8STRC("\">"));
		writer.WriteStrC(UTF8STRC("Book List</a>"));

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

Bool __stdcall SSWR::OrganWeb::OrganWebBookController::SvcBookAdd(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebBookController *me = (SSWR::OrganWeb::OrganWebBookController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	Int32 cateId;
	if (req->GetQueryValueI32(CSTR("id"), cateId))
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		NotNullPtr<CategoryInfo> cate;
		Sync::RWMutexUsage mutUsage;
		if (env.user == 0 || env.user->userType != UserType::Admin)
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_UNAUTHORIZED);
			return true;
		}
		else if (!me->env->CateGet(mutUsage, cateId).SetTo(cate))
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
		NotNullPtr<Text::String> title = Text::String::NewEmpty();
		NotNullPtr<Text::String> author = title;
		NotNullPtr<Text::String> press = title;
		NotNullPtr<Text::String> pubDate = title;
		NotNullPtr<Text::String> url = title;
		Text::CString errMsg = CSTR_NULL;
		if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
		{
			req->ParseHTTPForm();
			Data::Timestamp ts;
			if (!req->GetHTTPFormStr(CSTR("title")).SetTo(title) || title->leng == 0)
			{
				errMsg = CSTR("Book Name is empty");
				req->GetHTTPFormStr(CSTR("author")).SetTo(author);
				req->GetHTTPFormStr(CSTR("press")).SetTo(press);
				req->GetHTTPFormStr(CSTR("pubDate")).SetTo(pubDate);
				req->GetHTTPFormStr(CSTR("url")).SetTo(url);
			}
			else if (!req->GetHTTPFormStr(CSTR("author")).SetTo(author) || author->leng == 0)
			{
				errMsg = CSTR("Author is empty");
				req->GetHTTPFormStr(CSTR("press")).SetTo(press);
				req->GetHTTPFormStr(CSTR("pubDate")).SetTo(pubDate);
				req->GetHTTPFormStr(CSTR("url")).SetTo(url);
			}
			else if (!req->GetHTTPFormStr(CSTR("press")).SetTo(press) || press->leng == 0)
			{
				errMsg = CSTR("Press is empty");
				req->GetHTTPFormStr(CSTR("pubDate")).SetTo(pubDate);
				req->GetHTTPFormStr(CSTR("url")).SetTo(url);
			}
			else if (!req->GetHTTPFormStr(CSTR("pubDate")).SetTo(pubDate) || (ts = Data::Timestamp(pubDate->ToCString(), 0)).IsNull())
			{
				errMsg = CSTR("Publish Date is not valid");
				req->GetHTTPFormStr(CSTR("url")).SetTo(url);
			}
			else if (!req->GetHTTPFormStr(CSTR("url")).SetTo(url) || (url->leng > 0 && !url->StartsWith(UTF8STRC("http://")) && !url->StartsWith(UTF8STRC("https://"))))
			{
				errMsg = CSTR("URL is not valid");
			}
			else
			{
				if (me->env->BookAdd(mutUsage, title, author, press, ts, url))
				{
					sptr = Text::StrConcatC(sbuff, UTF8STRC("booklist.html?id="));
					sptr = Text::StrInt32(sptr, cate->cateId);
					return resp->RedirectURL(req, CSTRP(sbuff, sptr), 0);
				}
				else
				{
					errMsg = CSTR("Error in adding book");
				}
			}
		}
		IO::MemoryStream mstm;
		Text::UTF8Writer writer(mstm);
		NotNullPtr<Text::String> s;

		me->WriteHeader(&writer, cate->chiName->v, env.user, env.isMobile);
		writer.WriteStrC(UTF8STRC("<center><h1>New Book"));
		writer.WriteLineC(UTF8STRC("</h1></center>"));

		writer.WriteStrC(UTF8STRC("<form name=\"newBook\" method=\"POST\" action=\"bookadd.html?id="));
		sptr = Text::StrInt32(sbuff, cate->cateId);
		writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer.WriteStrC(UTF8STRC("\">"));

		writer.WriteStrC(UTF8STRC("<b>Book Name:</b> <input type=\"text\" name=\"title\""));
		if (title->leng > 0)
		{
			s = Text::XML::ToNewAttrText(title->v);
			writer.WriteStrC(UTF8STRC(" value="));
			writer.WriteStrC(s->v, s->leng);
			s->Release();
		}
		writer.WriteLineC(UTF8STRC(" /><br/>"));

		writer.WriteStrC(UTF8STRC("<b>Author:</b> <input type=\"text\" name=\"author\""));
		if (author->leng > 0)
		{
			s = Text::XML::ToNewAttrText(author->v);
			writer.WriteStrC(UTF8STRC(" value="));
			writer.WriteStrC(s->v, s->leng);
			s->Release();
		}
		writer.WriteLineC(UTF8STRC(" /><br/>"));

		writer.WriteStrC(UTF8STRC("<b>Publish Date:</b> <input type=\"text\" name=\"pubDate\""));
		if (pubDate->leng > 0)
		{
			s = Text::XML::ToNewAttrText(pubDate->v);
			writer.WriteStrC(UTF8STRC(" value="));
			writer.WriteStrC(s->v, s->leng);
			s->Release();
		}
		else
		{
			sptr = Data::Timestamp::Now().ToString(sbuff, "yyyy-MM-dd");
			writer.WriteStrC(UTF8STRC(" value=\""));
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteStrC(UTF8STRC("\""));
		}
		writer.WriteLineC(UTF8STRC(" /><br/>"));

		writer.WriteStrC(UTF8STRC("<b>Press:</b> <input type=\"text\" name=\"press\""));
		if (press->leng > 0)
		{
			s = Text::XML::ToNewAttrText(press->v);
			writer.WriteStrC(UTF8STRC(" value="));
			writer.WriteStrC(s->v, s->leng);
			s->Release();
		}
		writer.WriteLineC(UTF8STRC(" /><br/>"));

		writer.WriteStrC(UTF8STRC("<b>URL:</b> <input type=\"text\" name=\"url\""));
		if (url->leng > 0)
		{
			s = Text::XML::ToNewAttrText(url->v);
			writer.WriteStrC(UTF8STRC(" value="));
			writer.WriteStrC(s->v, s->leng);
			s->Release();
		}
		writer.WriteLineC(UTF8STRC(" /><br/>"));

		writer.WriteLineC(UTF8STRC("<input type=\"submit\"/></form>"));
		if (errMsg.leng > 0)
		{
			writer.WriteLineC(errMsg.v, errMsg.leng);
			writer.WriteLineC(UTF8STRC("<br/>"));
		}
		writer.WriteStrC(UTF8STRC("<a href=\"booklist.html?id="));
		sptr = Text::StrInt32(sbuff, cate->cateId);
		writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		writer.WriteStrC(UTF8STRC("\">"));
		writer.WriteStrC(UTF8STRC("Book List</a>"));

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

SSWR::OrganWeb::OrganWebBookController::OrganWebBookController(Net::WebServer::MemoryWebSessionManager *sessMgr, OrganWebEnv *env, UInt32 scnSize) : OrganWebController(sessMgr, env, scnSize)
{
	this->AddService(CSTR("/bookview.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcBookView);
	this->AddService(CSTR("/bookphoto.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcBookPhoto);
	this->AddService(CSTR("/bookadd.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcBookAdd);
	this->AddService(CSTR("/bookadd.html"), Net::WebUtil::RequestMethod::HTTP_POST, SvcBookAdd);
}

SSWR::OrganWeb::OrganWebBookController::~OrganWebBookController()
{
}
