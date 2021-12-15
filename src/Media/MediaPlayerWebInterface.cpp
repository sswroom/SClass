#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSortC.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "Media/MediaPlayerWebInterface.h"
#include "Media/CS/TransferFunc.h"
#include "Net/MIME.h"
#include "Text/MyStringFloat.h"
#include "Text/UTF8Writer.h"
#include "Text/XML.h"
#include "Text/TextEnc/URIEncoding.h"

OSInt __stdcall Media::MediaPlayerWebInterface::VideoFileCompare(void *file1, void *file2)
{
	VideoFileInfo *vfile1 = (VideoFileInfo*)file1;
	VideoFileInfo *vfile2 = (VideoFileInfo*)file2;
	return Text::StrCompare(vfile1->fileName, vfile2->fileName);
}

Media::MediaPlayerWebInterface::MediaPlayerWebInterface(Media::MediaPlayerInterface *iface, Bool autoRelease)
{
	this->iface = iface;
	this->autoRelease = autoRelease;
}

Media::MediaPlayerWebInterface::~MediaPlayerWebInterface()
{

}

void Media::MediaPlayerWebInterface::BrowseRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	Text::String *fname = req->GetQueryValue((const UTF8Char*)"fname");
	if (this->iface->GetOpenedFile() == 0)
	{
		resp->RedirectURL(req, (const UTF8Char*)"/", 0);
		return;
	}
	UTF8Char sbuff[1024];
	UTF8Char sbuff2[1024];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	this->iface->GetOpenedFile()->GetSourceNameObj()->ConcatTo(sbuff);
	i = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
	sptr = &sbuff[i + 1];

	if (fname)
	{
		fname->ConcatTo(sptr);
		if (this->iface->OpenFile(sbuff))
		{
			this->iface->PBStart();
			resp->RedirectURL(req, (const UTF8Char*)"/", 0);
			return;
		}
	}
	IO::MemoryStream *mstm;
	IO::Writer *writer;
	IO::Path::PathType pt;
	IO::Path::FindFileSession *sess;
	UInt8 *buff;
	Text::String *s;
	const UTF8Char *u8ptr;
	UOSInt size;
	UInt64 fileSize;

	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SP.GPSWeb.GPSWebHandler.LoginFunc"));
	NEW_CLASS(writer, Text::UTF8Writer(mstm));

	writer->WriteLine((const UTF8Char*)"<html>");
	writer->WriteLine((const UTF8Char*)"<head><title>HQMP Control</title>");
	writer->WriteLine((const UTF8Char*)"</head>");
	writer->WriteLine((const UTF8Char*)"<body>");
	writer->WriteLine((const UTF8Char*)"<a href=\"/\">Back</a><br/><br/>");
	writer->Write((const UTF8Char*)"<b>Current File: </b>");
	s = Text::XML::ToNewHTMLText(this->iface->GetOpenedFile()->GetSourceNameObj()->v);
	writer->Write(s->v, s->leng);
	s->Release();
	writer->WriteLine((const UTF8Char*)"<hr/>");

	writer->WriteLine((const UTF8Char*)"<table border=\"0\">");
	writer->WriteLine((const UTF8Char*)"<tr><td>Name</td><td>Size</td><td>MIME Type</td></tr>");

	Text::StrConcat(sptr, IO::Path::ALL_FILES);
	sess = IO::Path::FindFile(sbuff);
	if (sess)
	{
		Data::ArrayList<VideoFileInfo *> fileList;
		VideoFileInfo *vfile;

		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, &fileSize))
		{
			if (pt == IO::Path::PathType::File)
			{
				vfile = MemAlloc(VideoFileInfo, 1);
				vfile->fileName = Text::StrCopyNew(sptr);
				vfile->fileSize = fileSize;
				fileList.Add(vfile);
			}
		}
		IO::Path::FindFileClose(sess);

		void **arr = (void**)fileList.GetArray(&j);
		ArtificialQuickSort_SortCmp(arr, VideoFileCompare, 0, (OSInt)j - 1);

		i = 0;
		j = fileList.GetCount();
		while (i < j)
		{
			vfile = fileList.GetItem(i);

			writer->Write((const UTF8Char*)"<tr><td>");
			writer->Write((const UTF8Char*)"<a href=\"/browse?fname=");
			Text::TextEnc::URIEncoding::URIEncode(sbuff2, vfile->fileName);
			s = Text::XML::ToNewXMLText(sbuff2);
			writer->Write(s->v, s->leng);
			s->Release();
			writer->Write((const UTF8Char*)"\">");

			s = Text::XML::ToNewHTMLText(vfile->fileName);
			writer->Write(s->v, s->leng);
			s->Release();
			writer->Write((const UTF8Char*)"</a></td><td>");
			Text::StrUInt64(sbuff2, vfile->fileSize);
			writer->Write(sbuff2);
			writer->Write((const UTF8Char*)"</td><td>");

			IO::Path::GetFileExt(sbuff2, vfile->fileName);
			u8ptr = Net::MIME::GetMIMEFromExt(sbuff2);
			writer->Write(u8ptr);
			writer->WriteLine((const UTF8Char*)"</td></tr>");

			Text::StrDelNew(vfile->fileName);
			MemFree(vfile);
			i++;
		}
	}
	writer->WriteLine((const UTF8Char*)"</table>");

	writer->WriteLine((const UTF8Char*)"</body>");
	writer->WriteLine((const UTF8Char*)"</html>");
	DEL_CLASS(writer);

	resp->AddDefHeaders(req);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	buff = mstm->GetBuff(&size);
	resp->AddContentLength(size);
	resp->AddContentType((const UTF8Char*)"text/html;charset=UTF-8");
	resp->Write(buff, size);
	DEL_CLASS(mstm);
}

void Media::MediaPlayerWebInterface::WebRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	Text::String *reqURI = req->GetRequestURI();
	if (reqURI->Equals((const UTF8Char*)"/browse") || reqURI->StartsWith((const UTF8Char*)"/browse?"))
	{
		this->BrowseRequest(req, resp);
		return;
	}
	if (reqURI->Equals((const UTF8Char*)"/start"))
	{
		this->iface->PBStart();
	}
	else if (reqURI->Equals((const UTF8Char*)"/stop"))
	{
		this->iface->PBStop();
	}
	else if (reqURI->Equals((const UTF8Char*)"/pause"))
	{
		this->iface->PBPause();
	}
	else if (reqURI->Equals((const UTF8Char*)"/backward60"))
	{
		this->iface->PBJumpOfst(-60000);
	}
	else if (reqURI->Equals((const UTF8Char*)"/backward10"))
	{
		this->iface->PBJumpOfst(-10000);
	}
	else if (reqURI->Equals((const UTF8Char*)"/forward10"))
	{
		this->iface->PBJumpOfst(10000);
	}
	else if (reqURI->Equals((const UTF8Char*)"/forward60"))
	{
		this->iface->PBJumpOfst(60000);
	}
	else if (reqURI->Equals((const UTF8Char*)"/prevchap"))
	{
		this->iface->PBPrevChapter();
	}
	else if (reqURI->Equals((const UTF8Char*)"/nextchap"))
	{
		this->iface->PBNextChapter();
	}
	else if (reqURI->Equals((const UTF8Char*)"/avofstdec"))
	{
		this->iface->PBDecAVOfst();
	}
	else if (reqURI->Equals((const UTF8Char*)"/avofstinc"))
	{
		this->iface->PBIncAVOfst();
	}
	IO::MemoryStream *mstm;
	IO::Writer *writer;
	UInt8 *buff;
	Text::String *s;
	UOSInt size;

	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SP.GPSWeb.GPSWebHandler.LoginFunc"));
	NEW_CLASS(writer, Text::UTF8Writer(mstm));

	writer->WriteLine((const UTF8Char*)"<html>");
	writer->WriteLine((const UTF8Char*)"<head><title>HQMP Control</title>");
	writer->WriteLine((const UTF8Char*)"</head>");
	writer->WriteLine((const UTF8Char*)"<body>");
	writer->WriteLine((const UTF8Char*)"<a href=\"/\">Refresh</a><br/><br/>");
	writer->Write((const UTF8Char*)"<b>Current File: </b>");
	if (this->iface->GetOpenedFile())
	{
		s = Text::XML::ToNewHTMLText(this->iface->GetOpenedFile()->GetSourceNameObj()->v);
		writer->Write(s->v, s->leng);
		s->Release();

		writer->Write((const UTF8Char*)" <a href=\"/browse\">Browse</a>");
	}
	else
	{
		writer->Write((const UTF8Char*)"-");
	}
	writer->WriteLine((const UTF8Char*)"<br/>");

	writer->WriteLine((const UTF8Char*)"<input type=\"button\" value=\"Stop\" onclick=\"document.location.replace('/stop')\"/>");
	writer->WriteLine((const UTF8Char*)"<input type=\"button\" value=\"Start\" onclick=\"document.location.replace('/start')\"/>");
	writer->WriteLine((const UTF8Char*)"<input type=\"button\" value=\"Pause\" onclick=\"document.location.replace('/pause')\"/>");
	writer->WriteLine((const UTF8Char*)"<br/>");
	writer->WriteLine((const UTF8Char*)"<a href=\"/backward60\">Backward 1 Minute</a>");
	writer->WriteLine((const UTF8Char*)"<a href=\"/backward10\">Backward 10 Seconds</a>");
	writer->WriteLine((const UTF8Char*)"<a href=\"/forward10\">Forward 10 Seconds</a>");
	writer->WriteLine((const UTF8Char*)"<a href=\"/forward60\">Forward 1 Minute</a>");
	writer->WriteLine((const UTF8Char*)"<br/>");
	writer->WriteLine((const UTF8Char*)"<a href=\"/prevchap\">Previous Chapter</a>");
	writer->WriteLine((const UTF8Char*)"<a href=\"/nextchap\">Next Chapter</a>");
	writer->WriteLine((const UTF8Char*)"<a href=\"/avofstdec\">A/V Offset Decrease</a>");
	writer->WriteLine((const UTF8Char*)"<a href=\"/avofstinc\">A/V Offset Increase</a>");
	{
		Text::StringBuilderUTF8 sb;
		Media::VideoRenderer::RendererStatus status;
		UInt32 currTime;
		UInt32 v;

		writer->WriteLine((const UTF8Char*)"<hr/>");
		NEW_CLASS(status.color, Media::ColorProfile());
		this->iface->GetVideoRenderer()->GetStatus(&status);
		sb.Append((const UTF8Char*)"Curr Time: ");
		sb.AppendU32(status.currTime);
		currTime = status.currTime;
		v = currTime / 3600000;
		sb.Append((const UTF8Char*)" (");
		sb.AppendU32(v);
		sb.Append((const UTF8Char*)":");
		currTime -= v * 3600000;
		v = currTime / 60000;
		if (v < 10)
		{
			sb.Append((const UTF8Char*)"0");
		}
		sb.AppendU32(v);
		sb.Append((const UTF8Char*)":");
		currTime -= v * 60000;
		v = currTime / 1000;
		if (v < 10)
		{
			sb.Append((const UTF8Char*)"0");
		}
		sb.AppendU32(v);
		sb.Append((const UTF8Char*)".");
		currTime -= v * 1000;
		if (currTime < 10)
		{
			sb.Append((const UTF8Char*)"00");
			sb.AppendU32(currTime);
		}
		else if (currTime < 100)
		{
			sb.Append((const UTF8Char*)"0");
			sb.AppendU32(currTime);
		}
		else
		{
			sb.AppendU32(currTime);
		}
		sb.Append((const UTF8Char*)")<br/>\r\n");
		sb.Append((const UTF8Char*)"Disp Frame Time: ");
		sb.AppendU32(status.dispFrameTime);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Disp Frame Num: ");
		sb.AppendU32(status.dispFrameNum);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Proc Delay: ");
		sb.AppendI32(status.procDelay);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Disp Delay: ");
		sb.AppendI32(status.dispDelay);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Disp Jitter: ");
		sb.AppendI32(status.dispJitter);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Video Delay: ");
		sb.AppendI32(status.videoDelay);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Source Delay: ");
		sb.AppendI32(status.srcDelay);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"A/V Offset: ");
		sb.AppendI32(status.avOfst);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Seek Count: ");
		sb.AppendUOSInt(status.seekCnt);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Frame Displayed: ");
		sb.AppendU32(status.frameDispCnt);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Frame Skip before process: ");
		sb.AppendU32(status.frameSkipBefore);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Frame Skip after process: ");
		sb.AppendU32(status.frameSkipAfter);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"ProcTimes H: ");
		Text::SBAppendF64(&sb, status.hTime);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"ProcTimes V: ");
		Text::SBAppendF64(&sb, status.vTime);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"ProcTimes C: ");
		Text::SBAppendF64(&sb, status.csTime);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Buff: ");
		sb.AppendI32(status.buffProc);
		sb.Append((const UTF8Char*)",");
		sb.AppendI32(status.buffReady);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Src Size: ");
		sb.AppendUOSInt(status.srcWidth);
		sb.Append((const UTF8Char*)" x ");
		sb.AppendUOSInt(status.srcHeight);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Disp Size: ");
		sb.AppendUOSInt(status.dispWidth);
		sb.Append((const UTF8Char*)" x ");
		sb.AppendUOSInt(status.dispHeight);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"PAR: ");
		Text::SBAppendF64(&sb, status.par);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Decoder: ");
		if (status.decoderName)
		{
			sb.Append(status.decoderName);
		}
		else
		{
			sb.Append((const UTF8Char*)"-");
		}
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Format: ");
		sb.Append(Media::CS::CSConverter::GetFormatName(status.format));
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Output Bitdepth: ");
		sb.AppendU32(status.dispBitDepth);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Src YUV Type: ");
		sb.Append(Media::ColorProfile::YUVTypeGetName(status.srcYUVType));
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Src R Transfer: ");
		sb.Append(Media::CS::TransferFunc::GetTransferFuncName(status.color->GetRTranParam()->GetTranType()));
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Src G Transfer: ");
		sb.Append(Media::CS::TransferFunc::GetTransferFuncName(status.color->GetGTranParam()->GetTranType()));
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Src B Transfer: ");
		sb.Append(Media::CS::TransferFunc::GetTransferFuncName(status.color->GetBTranParam()->GetTranType()));
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Src Gamma: ");
		Text::SBAppendF64(&sb, status.color->GetRTranParam()->GetGamma());
		sb.Append((const UTF8Char*)"<br/>\r\n");
		Media::ColorProfile::ColorPrimaries *primaries = status.color->GetPrimaries(); 
		sb.Append((const UTF8Char*)"Src RGB Primary: ");
		sb.Append(Media::ColorProfile::ColorTypeGetName(primaries->colorType));
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"-Red:   ");
		Text::SBAppendF64(&sb, primaries->rx);
		sb.Append((const UTF8Char*)", ");
		Text::SBAppendF64(&sb, primaries->ry);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"-Green: ");
		Text::SBAppendF64(&sb, primaries->gx);
		sb.Append((const UTF8Char*)", ");
		Text::SBAppendF64(&sb, primaries->gy);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"-Blue:  ");
		Text::SBAppendF64(&sb, primaries->bx);
		sb.Append((const UTF8Char*)", ");
		Text::SBAppendF64(&sb, primaries->by);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"-White: ");
		Text::SBAppendF64(&sb, primaries->wx);
		sb.Append((const UTF8Char*)", ");
		Text::SBAppendF64(&sb, primaries->wy);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		DEL_CLASS(status.color);
		writer->Write(sb.ToString());
	}

	writer->WriteLine((const UTF8Char*)"</body>");
	writer->WriteLine((const UTF8Char*)"</html>");
	DEL_CLASS(writer);

	resp->AddDefHeaders(req);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	buff = mstm->GetBuff(&size);
	resp->AddContentLength(size);
	resp->AddContentType((const UTF8Char*)"text/html;charset=UTF-8");
	resp->Write(buff, size);
	DEL_CLASS(mstm);
}

void Media::MediaPlayerWebInterface::Release()
{
	if (this->autoRelease)
	{
		DEL_CLASS(this);
	}
}
