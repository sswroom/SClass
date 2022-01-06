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
	Text::String *fname = req->GetQueryValue(UTF8STRC("fname"));
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
	UTF8Char *sptr2;
	UOSInt size;
	UInt64 fileSize;

	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SP.GPSWeb.GPSWebHandler.LoginFunc"));
	NEW_CLASS(writer, Text::UTF8Writer(mstm));

	writer->WriteLineC(UTF8STRC("<html>"));
	writer->WriteLineC(UTF8STRC("<head><title>HQMP Control</title>"));
	writer->WriteLineC(UTF8STRC("</head>"));
	writer->WriteLineC(UTF8STRC("<body>"));
	writer->WriteLineC(UTF8STRC("<a href=\"/\">Back</a><br/><br/>"));
	writer->WriteStrC(UTF8STRC("<b>Current File: </b>"));
	s = Text::XML::ToNewHTMLText(this->iface->GetOpenedFile()->GetSourceNameObj()->v);
	writer->WriteStrC(s->v, s->leng);
	s->Release();
	writer->WriteLineC(UTF8STRC("<hr/>"));

	writer->WriteLineC(UTF8STRC("<table border=\"0\">"));
	writer->WriteLineC(UTF8STRC("<tr><td>Name</td><td>Size</td><td>MIME Type</td></tr>"));

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

			writer->WriteStrC(UTF8STRC("<tr><td>"));
			writer->WriteStrC(UTF8STRC("<a href=\"/browse?fname="));
			Text::TextEnc::URIEncoding::URIEncode(sbuff2, vfile->fileName);
			s = Text::XML::ToNewXMLText(sbuff2);
			writer->WriteStrC(s->v, s->leng);
			s->Release();
			writer->WriteStrC(UTF8STRC("\">"));

			s = Text::XML::ToNewHTMLText(vfile->fileName);
			writer->WriteStrC(s->v, s->leng);
			s->Release();
			writer->WriteStrC(UTF8STRC("</a></td><td>"));
			sptr2 = Text::StrUInt64(sbuff2, vfile->fileSize);
			writer->WriteStrC(sbuff2, (UOSInt)(sptr2 - sbuff));
			writer->WriteStrC(UTF8STRC("</td><td>"));

			IO::Path::GetFileExt(sbuff2, vfile->fileName);
			u8ptr = Net::MIME::GetMIMEFromExt(sbuff2);
			writer->WriteStr(u8ptr);
			writer->WriteLineC(UTF8STRC("</td></tr>"));

			Text::StrDelNew(vfile->fileName);
			MemFree(vfile);
			i++;
		}
	}
	writer->WriteLineC(UTF8STRC("</table>"));

	writer->WriteLineC(UTF8STRC("</body>"));
	writer->WriteLineC(UTF8STRC("</html>"));
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

	writer->WriteLineC(UTF8STRC("<html>"));
	writer->WriteLineC(UTF8STRC("<head><title>HQMP Control</title>"));
	writer->WriteLineC(UTF8STRC("</head>"));
	writer->WriteLineC(UTF8STRC("<body>"));
	writer->WriteLineC(UTF8STRC("<a href=\"/\">Refresh</a><br/><br/>"));
	writer->WriteStrC(UTF8STRC("<b>Current File: </b>"));
	if (this->iface->GetOpenedFile())
	{
		s = Text::XML::ToNewHTMLText(this->iface->GetOpenedFile()->GetSourceNameObj()->v);
		writer->WriteStrC(s->v, s->leng);
		s->Release();

		writer->WriteStrC(UTF8STRC(" <a href=\"/browse\">Browse</a>"));
	}
	else
	{
		writer->WriteStrC(UTF8STRC("-"));
	}
	writer->WriteLineC(UTF8STRC("<br/>"));

	writer->WriteLineC(UTF8STRC("<input type=\"button\" value=\"Stop\" onclick=\"document.location.replace('/stop')\"/>"));
	writer->WriteLineC(UTF8STRC("<input type=\"button\" value=\"Start\" onclick=\"document.location.replace('/start')\"/>"));
	writer->WriteLineC(UTF8STRC("<input type=\"button\" value=\"Pause\" onclick=\"document.location.replace('/pause')\"/>"));
	writer->WriteLineC(UTF8STRC("<br/>"));
	writer->WriteLineC(UTF8STRC("<a href=\"/backward60\">Backward 1 Minute</a>"));
	writer->WriteLineC(UTF8STRC("<a href=\"/backward10\">Backward 10 Seconds</a>"));
	writer->WriteLineC(UTF8STRC("<a href=\"/forward10\">Forward 10 Seconds</a>"));
	writer->WriteLineC(UTF8STRC("<a href=\"/forward60\">Forward 1 Minute</a>"));
	writer->WriteLineC(UTF8STRC("<br/>"));
	writer->WriteLineC(UTF8STRC("<a href=\"/prevchap\">Previous Chapter</a>"));
	writer->WriteLineC(UTF8STRC("<a href=\"/nextchap\">Next Chapter</a>"));
	writer->WriteLineC(UTF8STRC("<a href=\"/avofstdec\">A/V Offset Decrease</a>"));
	writer->WriteLineC(UTF8STRC("<a href=\"/avofstinc\">A/V Offset Increase</a>"));
	{
		Text::StringBuilderUTF8 sb;
		Media::VideoRenderer::RendererStatus status;
		UInt32 currTime;
		UInt32 v;

		writer->WriteLineC(UTF8STRC("<hr/>"));
		NEW_CLASS(status.color, Media::ColorProfile());
		this->iface->GetVideoRenderer()->GetStatus(&status);
		sb.AppendC(UTF8STRC("Curr Time: "));
		sb.AppendU32(status.currTime);
		currTime = status.currTime;
		v = currTime / 3600000;
		sb.AppendC(UTF8STRC(" ("));
		sb.AppendU32(v);
		sb.AppendC(UTF8STRC(":"));
		currTime -= v * 3600000;
		v = currTime / 60000;
		if (v < 10)
		{
			sb.AppendC(UTF8STRC("0"));
		}
		sb.AppendU32(v);
		sb.AppendC(UTF8STRC(":"));
		currTime -= v * 60000;
		v = currTime / 1000;
		if (v < 10)
		{
			sb.AppendC(UTF8STRC("0"));
		}
		sb.AppendU32(v);
		sb.AppendC(UTF8STRC("."));
		currTime -= v * 1000;
		if (currTime < 10)
		{
			sb.AppendC(UTF8STRC("00"));
			sb.AppendU32(currTime);
		}
		else if (currTime < 100)
		{
			sb.AppendC(UTF8STRC("0"));
			sb.AppendU32(currTime);
		}
		else
		{
			sb.AppendU32(currTime);
		}
		sb.AppendC(UTF8STRC(")<br/>\r\n"));
		sb.AppendC(UTF8STRC("Disp Frame Time: "));
		sb.AppendU32(status.dispFrameTime);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("Disp Frame Num: "));
		sb.AppendU32(status.dispFrameNum);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("Proc Delay: "));
		sb.AppendI32(status.procDelay);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("Disp Delay: "));
		sb.AppendI32(status.dispDelay);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("Disp Jitter: "));
		sb.AppendI32(status.dispJitter);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("Video Delay: "));
		sb.AppendI32(status.videoDelay);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("Source Delay: "));
		sb.AppendI32(status.srcDelay);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("A/V Offset: "));
		sb.AppendI32(status.avOfst);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("Seek Count: "));
		sb.AppendUOSInt(status.seekCnt);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("Frame Displayed: "));
		sb.AppendU32(status.frameDispCnt);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("Frame Skip before process: "));
		sb.AppendU32(status.frameSkipBefore);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("Frame Skip after process: "));
		sb.AppendU32(status.frameSkipAfter);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("ProcTimes H: "));
		Text::SBAppendF64(&sb, status.hTime);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("ProcTimes V: "));
		Text::SBAppendF64(&sb, status.vTime);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("ProcTimes C: "));
		Text::SBAppendF64(&sb, status.csTime);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("Buff: "));
		sb.AppendI32(status.buffProc);
		sb.AppendC(UTF8STRC(","));
		sb.AppendI32(status.buffReady);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("Src Size: "));
		sb.AppendUOSInt(status.srcWidth);
		sb.AppendC(UTF8STRC(" x "));
		sb.AppendUOSInt(status.srcHeight);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("Disp Size: "));
		sb.AppendUOSInt(status.dispWidth);
		sb.AppendC(UTF8STRC(" x "));
		sb.AppendUOSInt(status.dispHeight);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("PAR: "));
		Text::SBAppendF64(&sb, status.par);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("Decoder: "));
		if (status.decoderName)
		{
			sb.Append(status.decoderName);
		}
		else
		{
			sb.AppendC(UTF8STRC("-"));
		}
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("Format: "));
		sb.Append(Media::CS::CSConverter::GetFormatName(status.format));
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("Output Bitdepth: "));
		sb.AppendU32(status.dispBitDepth);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("Src YUV Type: "));
		sb.Append(Media::ColorProfile::YUVTypeGetName(status.srcYUVType));
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("Src R Transfer: "));
		sb.Append(Media::CS::TransferFunc::GetTransferFuncName(status.color->GetRTranParam()->GetTranType()));
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("Src G Transfer: "));
		sb.Append(Media::CS::TransferFunc::GetTransferFuncName(status.color->GetGTranParam()->GetTranType()));
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("Src B Transfer: "));
		sb.Append(Media::CS::TransferFunc::GetTransferFuncName(status.color->GetBTranParam()->GetTranType()));
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("Src Gamma: "));
		Text::SBAppendF64(&sb, status.color->GetRTranParam()->GetGamma());
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		Media::ColorProfile::ColorPrimaries *primaries = status.color->GetPrimaries(); 
		sb.AppendC(UTF8STRC("Src RGB Primary: "));
		sb.Append(Media::ColorProfile::ColorTypeGetName(primaries->colorType));
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("-Red:   "));
		Text::SBAppendF64(&sb, primaries->rx);
		sb.AppendC(UTF8STRC(", "));
		Text::SBAppendF64(&sb, primaries->ry);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("-Green: "));
		Text::SBAppendF64(&sb, primaries->gx);
		sb.AppendC(UTF8STRC(", "));
		Text::SBAppendF64(&sb, primaries->gy);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("-Blue:  "));
		Text::SBAppendF64(&sb, primaries->bx);
		sb.AppendC(UTF8STRC(", "));
		Text::SBAppendF64(&sb, primaries->by);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("-White: "));
		Text::SBAppendF64(&sb, primaries->wx);
		sb.AppendC(UTF8STRC(", "));
		Text::SBAppendF64(&sb, primaries->wy);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		DEL_CLASS(status.color);
		writer->WriteStrC(sb.ToString(), sb.GetLength());
	}

	writer->WriteLineC(UTF8STRC("</body>"));
	writer->WriteLineC(UTF8STRC("</html>"));
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
