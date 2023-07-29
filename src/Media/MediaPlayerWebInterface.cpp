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
#include "Text/TextBinEnc/URIEncoding.h"

OSInt __stdcall Media::MediaPlayerWebInterface::VideoFileCompare(void *file1, void *file2)
{
	VideoFileInfo *vfile1 = (VideoFileInfo*)file1;
	VideoFileInfo *vfile2 = (VideoFileInfo*)file2;
	return Text::StrCompare(vfile1->fileName->v, vfile2->fileName->v);
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
	Text::String *fname = req->GetQueryValue(CSTR("fname"));
	if (this->iface->GetOpenedFile() == 0)
	{
		resp->RedirectURL(req, CSTR("/"), 0);
		return;
	}
	UTF8Char sbuff[1024];
	UTF8Char sbuff2[1024];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UOSInt i;
	UOSInt j;
	sptr = this->iface->GetOpenedFile()->GetSourceNameObj()->ConcatTo(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	sptr = &sbuff[i + 1];

	if (fname)
	{
		sptr2 = fname->ConcatTo(sptr);
		if (this->iface->OpenFile(CSTRP(sbuff, sptr2), IO::ParserType::MediaFile))
		{
			this->iface->PBStart();
			resp->RedirectURL(req, CSTR("/"), 0);
			return;
		}
	}
	IO::Path::PathType pt;
	IO::Path::FindFileSession *sess;
	UInt8 *buff;
	NotNullPtr<Text::String> s;
	UOSInt size;
	UInt64 fileSize;

	IO::MemoryStream mstm;
	{
		Text::UTF8Writer writer(mstm);

		writer.WriteLineC(UTF8STRC("<html>"));
		writer.WriteLineC(UTF8STRC("<head><title>HQMP Control</title>"));
		writer.WriteLineC(UTF8STRC("</head>"));
		writer.WriteLineC(UTF8STRC("<body>"));
		writer.WriteLineC(UTF8STRC("<a href=\"/\">Back</a><br/><br/>"));
		writer.WriteStrC(UTF8STRC("<b>Current File: </b>"));
		s = Text::XML::ToNewHTMLBodyText(this->iface->GetOpenedFile()->GetSourceNameObj()->v);
		writer.WriteStrC(s->v, s->leng);
		s->Release();
		writer.WriteLineC(UTF8STRC("<hr/>"));

		writer.WriteLineC(UTF8STRC("<table border=\"0\">"));
		writer.WriteLineC(UTF8STRC("<tr><td>Name</td><td>Size</td><td>MIME Type</td></tr>"));

		sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
		sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
		if (sess)
		{
			Data::ArrayList<VideoFileInfo *> fileList;
			VideoFileInfo *vfile;

			while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, &fileSize)) != 0)
			{
				if (pt == IO::Path::PathType::File)
				{
					vfile = MemAlloc(VideoFileInfo, 1);
					vfile->fileName = Text::String::New(sptr, (UOSInt)(sptr2 - sptr));
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

				writer.WriteStrC(UTF8STRC("<tr><td>"));
				writer.WriteStrC(UTF8STRC("<a href=\"/browse?fname="));
				Text::TextBinEnc::URIEncoding::URIEncode(sbuff2, vfile->fileName->v);
				s = Text::XML::ToNewXMLText(sbuff2);
				writer.WriteStrC(s->v, s->leng);
				s->Release();
				writer.WriteStrC(UTF8STRC("\">"));

				s = Text::XML::ToNewHTMLBodyText(vfile->fileName->v);
				writer.WriteStrC(s->v, s->leng);
				s->Release();
				writer.WriteStrC(UTF8STRC("</a></td><td>"));
				sptr2 = Text::StrUInt64(sbuff2, vfile->fileSize);
				writer.WriteStrC(sbuff2, (UOSInt)(sptr2 - sbuff));
				writer.WriteStrC(UTF8STRC("</td><td>"));

				sptr2 = IO::Path::GetFileExt(sbuff2, vfile->fileName->v, vfile->fileName->leng);
				Text::CString mime = Net::MIME::GetMIMEFromExt(CSTRP(sbuff2, sptr2));
				writer.WriteStrC(mime.v, mime.leng);
				writer.WriteLineC(UTF8STRC("</td></tr>"));

				vfile->fileName->Release();
				MemFree(vfile);
				i++;
			}
		}
		writer.WriteLineC(UTF8STRC("</table>"));

		writer.WriteLineC(UTF8STRC("</body>"));
		writer.WriteLineC(UTF8STRC("</html>"));
	}

	resp->AddDefHeaders(req);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	buff = mstm.GetBuff(&size);
	resp->AddContentLength(size);
	resp->AddContentType(CSTR("text/html; charset=UTF-8"));
	resp->Write(buff, size);
}

void Media::MediaPlayerWebInterface::WebRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	NotNullPtr<Text::String> reqURI = req->GetRequestURI();
	if (reqURI->Equals(UTF8STRC("/browse")) || reqURI->StartsWith(UTF8STRC("/browse?")))
	{
		this->BrowseRequest(req, resp);
		return;
	}
	if (reqURI->Equals(UTF8STRC("/start")))
	{
		this->iface->PBStart();
	}
	else if (reqURI->Equals(UTF8STRC("/stop")))
	{
		this->iface->PBStop();
	}
	else if (reqURI->Equals(UTF8STRC("/pause")))
	{
		this->iface->PBPause();
	}
	else if (reqURI->Equals(UTF8STRC("/backward60")))
	{
		this->iface->PBJumpOfst(-60000);
	}
	else if (reqURI->Equals(UTF8STRC("/backward10")))
	{
		this->iface->PBJumpOfst(-10000);
	}
	else if (reqURI->Equals(UTF8STRC("/forward10")))
	{
		this->iface->PBJumpOfst(10000);
	}
	else if (reqURI->Equals(UTF8STRC("/forward60")))
	{
		this->iface->PBJumpOfst(60000);
	}
	else if (reqURI->Equals(UTF8STRC("/prevchap")))
	{
		this->iface->PBPrevChapter();
	}
	else if (reqURI->Equals(UTF8STRC("/nextchap")))
	{
		this->iface->PBNextChapter();
	}
	else if (reqURI->Equals(UTF8STRC("/avofstdec")))
	{
		this->iface->PBDecAVOfst();
	}
	else if (reqURI->Equals(UTF8STRC("/avofstinc")))
	{
		this->iface->PBIncAVOfst();
	}
	UInt8 *buff;
	NotNullPtr<Text::String> s;
	UOSInt size;

	IO::MemoryStream mstm;
	{
		Text::UTF8Writer writer(mstm);

		writer.WriteLineC(UTF8STRC("<html>"));
		writer.WriteLineC(UTF8STRC("<head><title>HQMP Control</title>"));
		writer.WriteLineC(UTF8STRC("</head>"));
		writer.WriteLineC(UTF8STRC("<body>"));
		writer.WriteLineC(UTF8STRC("<a href=\"/\">Refresh</a><br/><br/>"));
		writer.WriteStrC(UTF8STRC("<b>Current File: </b>"));
		if (this->iface->GetOpenedFile())
		{
			s = Text::XML::ToNewHTMLBodyText(this->iface->GetOpenedFile()->GetSourceNameObj()->v);
			writer.WriteStrC(s->v, s->leng);
			s->Release();

			writer.WriteStrC(UTF8STRC(" <a href=\"/browse\">Browse</a>"));
		}
		else
		{
			writer.WriteStrC(UTF8STRC("-"));
		}
		writer.WriteLineC(UTF8STRC("<br/>"));

		writer.WriteLineC(UTF8STRC("<input type=\"button\" value=\"Stop\" onclick=\"document.location.replace('/stop')\"/>"));
		writer.WriteLineC(UTF8STRC("<input type=\"button\" value=\"Start\" onclick=\"document.location.replace('/start')\"/>"));
		writer.WriteLineC(UTF8STRC("<input type=\"button\" value=\"Pause\" onclick=\"document.location.replace('/pause')\"/>"));
		writer.WriteLineC(UTF8STRC("<br/>"));
		writer.WriteLineC(UTF8STRC("<a href=\"/backward60\">Backward 1 Minute</a>"));
		writer.WriteLineC(UTF8STRC("<a href=\"/backward10\">Backward 10 Seconds</a>"));
		writer.WriteLineC(UTF8STRC("<a href=\"/forward10\">Forward 10 Seconds</a>"));
		writer.WriteLineC(UTF8STRC("<a href=\"/forward60\">Forward 1 Minute</a>"));
		writer.WriteLineC(UTF8STRC("<br/>"));
		writer.WriteLineC(UTF8STRC("<a href=\"/prevchap\">Previous Chapter</a>"));
		writer.WriteLineC(UTF8STRC("<a href=\"/nextchap\">Next Chapter</a>"));
		writer.WriteLineC(UTF8STRC("<a href=\"/avofstdec\">A/V Offset Decrease</a>"));
		writer.WriteLineC(UTF8STRC("<a href=\"/avofstinc\">A/V Offset Increase</a>"));
		{
			Text::StringBuilderUTF8 sb;
			Media::VideoRenderer::RendererStatus status;
			UInt32 currTime;
			UInt32 v;

			writer.WriteLineC(UTF8STRC("<hr/>"));
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
			sb.AppendDouble(status.hTime);
			sb.AppendC(UTF8STRC("<br/>\r\n"));
			sb.AppendC(UTF8STRC("ProcTimes V: "));
			sb.AppendDouble(status.vTime);
			sb.AppendC(UTF8STRC("<br/>\r\n"));
			sb.AppendC(UTF8STRC("ProcTimes C: "));
			sb.AppendDouble(status.csTime);
			sb.AppendC(UTF8STRC("<br/>\r\n"));
			sb.AppendC(UTF8STRC("Buff: "));
			sb.AppendI32(status.buffProc);
			sb.AppendC(UTF8STRC(","));
			sb.AppendI32(status.buffReady);
			sb.AppendC(UTF8STRC("<br/>\r\n"));
			sb.AppendC(UTF8STRC("Src Size: "));
			sb.AppendUOSInt(status.srcSize.x);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendUOSInt(status.srcSize.y);
			sb.AppendC(UTF8STRC("<br/>\r\n"));
			sb.AppendC(UTF8STRC("Disp Size: "));
			sb.AppendUOSInt(status.dispSize.x);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendUOSInt(status.dispSize.y);
			sb.AppendC(UTF8STRC("<br/>\r\n"));
			sb.AppendC(UTF8STRC("PAR: "));
			sb.AppendDouble(status.par);
			sb.AppendC(UTF8STRC("<br/>\r\n"));
			sb.AppendC(UTF8STRC("Decoder: "));
			if (status.decoderName.v)
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
			sb.Append(Media::CS::TransferTypeGetName(status.color.GetRTranParam()->GetTranType()));
			sb.AppendC(UTF8STRC("<br/>\r\n"));
			sb.AppendC(UTF8STRC("Src G Transfer: "));
			sb.Append(Media::CS::TransferTypeGetName(status.color.GetGTranParam()->GetTranType()));
			sb.AppendC(UTF8STRC("<br/>\r\n"));
			sb.AppendC(UTF8STRC("Src B Transfer: "));
			sb.Append(Media::CS::TransferTypeGetName(status.color.GetBTranParam()->GetTranType()));
			sb.AppendC(UTF8STRC("<br/>\r\n"));
			sb.AppendC(UTF8STRC("Src Gamma: "));
			sb.AppendDouble(status.color.GetRTranParam()->GetGamma());
			sb.AppendC(UTF8STRC("<br/>\r\n"));
			Media::ColorProfile::ColorPrimaries *primaries = status.color.GetPrimaries(); 
			sb.AppendC(UTF8STRC("Src RGB Primary: "));
			sb.Append(Media::ColorProfile::ColorTypeGetName(primaries->colorType));
			sb.AppendC(UTF8STRC("<br/>\r\n"));
			sb.AppendC(UTF8STRC("-Red:   "));
			sb.AppendDouble(primaries->rx);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendDouble(primaries->ry);
			sb.AppendC(UTF8STRC("<br/>\r\n"));
			sb.AppendC(UTF8STRC("-Green: "));
			sb.AppendDouble(primaries->gx);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendDouble(primaries->gy);
			sb.AppendC(UTF8STRC("<br/>\r\n"));
			sb.AppendC(UTF8STRC("-Blue:  "));
			sb.AppendDouble(primaries->bx);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendDouble(primaries->by);
			sb.AppendC(UTF8STRC("<br/>\r\n"));
			sb.AppendC(UTF8STRC("-White: "));
			sb.AppendDouble(primaries->wx);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendDouble(primaries->wy);
			sb.AppendC(UTF8STRC("<br/>\r\n"));
			writer.WriteStrC(sb.ToString(), sb.GetLength());
		}

		writer.WriteLineC(UTF8STRC("</body>"));
		writer.WriteLineC(UTF8STRC("</html>"));
	}

	resp->AddDefHeaders(req);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	buff = mstm.GetBuff(&size);
	resp->AddContentLength(size);
	resp->AddContentType(CSTR("text/html; charset=UTF-8"));
	resp->Write(buff, size);
}

void Media::MediaPlayerWebInterface::Release()
{
	if (this->autoRelease)
	{
		DEL_CLASS(this);
	}
}
