#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSortFunc.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "Media/MediaPlayerWebInterface.h"
#include "Media/CS/TransferFunc.h"
#include "Net/MIME.h"
#include "Text/MyStringFloat.h"
#include "Text/UTF8Writer.h"
#include "Text/XML.h"
#include "Text/TextBinEnc/URIEncoding.h"

OSInt __stdcall Media::MediaPlayerWebInterface::VideoFileCompare(NN<VideoFileInfo> file1, NN<VideoFileInfo> file2)
{
	return Text::StrCompare(file1->fileName->v, file2->fileName->v);
}

Media::MediaPlayerWebInterface::MediaPlayerWebInterface(NN<Media::MediaPlayerInterface> iface, Bool autoRelease)
{
	this->iface = iface;
	this->autoRelease = autoRelease;
}

Media::MediaPlayerWebInterface::~MediaPlayerWebInterface()
{

}

void Media::MediaPlayerWebInterface::BrowseRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Optional<Text::String> fname = req->GetQueryValue(CSTR("fname"));
	NN<Media::MediaFile> openedFile;
	if (!this->iface->GetOpenedFile().SetTo(openedFile))
	{
		resp->RedirectURL(req, CSTR("/"), 0);
		return;
	}
	UTF8Char sbuff[1024];
	UTF8Char sbuff2[1024];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	NN<Text::String> s;
	UOSInt i;
	UOSInt j;
	sptr = openedFile->GetSourceNameObj()->ConcatTo(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	sptr = &sbuff[i + 1];

	if (fname.SetTo(s))
	{
		sptr2 = s->ConcatTo(sptr);
		if (this->iface->OpenFile(CSTRP(sbuff, sptr2), IO::ParserType::MediaFile))
		{
			this->iface->PBStart();
			resp->RedirectURL(req, CSTR("/"), 0);
			return;
		}
	}
	IO::Path::PathType pt;
	NN<IO::Path::FindFileSession> sess;
	UnsafeArray<UInt8> buff;
	UOSInt size;
	UInt64 fileSize;

	IO::MemoryStream mstm;
	{
		Text::UTF8Writer writer(mstm);

		writer.WriteLine(CSTR("<html>"));
		writer.WriteLine(CSTR("<head><title>HQMP Control</title>"));
		writer.WriteLine(CSTR("</head>"));
		writer.WriteLine(CSTR("<body>"));
		writer.WriteLine(CSTR("<a href=\"/\">Back</a><br/><br/>"));
		writer.Write(CSTR("<b>Current File: </b>"));
		s = Text::XML::ToNewHTMLBodyText(openedFile->GetSourceNameObj()->v);
		writer.Write(s->ToCString());
		s->Release();
		writer.WriteLine(CSTR("<hr/>"));

		writer.WriteLine(CSTR("<table border=\"0\">"));
		writer.WriteLine(CSTR("<tr><td>Name</td><td>Size</td><td>MIME Type</td></tr>"));

		sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
		if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
		{
			Data::ArrayListNN<VideoFileInfo> fileList;
			NN<VideoFileInfo> vfile;

			while (IO::Path::FindNextFile(sptr, sess, 0, pt, fileSize).SetTo(sptr2))
			{
				if (pt == IO::Path::PathType::File)
				{
					vfile = MemAllocNN(VideoFileInfo);
					vfile->fileName = Text::String::New(sptr, (UOSInt)(sptr2 - sptr));
					vfile->fileSize = fileSize;
					fileList.Add(vfile);
				}
			}
			IO::Path::FindFileClose(sess);

			Data::Sort::ArtificialQuickSortFunc<NN<VideoFileInfo>>::Sort(fileList, VideoFileCompare);

			i = 0;
			j = fileList.GetCount();
			while (i < j)
			{
				vfile = fileList.GetItemNoCheck(i);

				writer.Write(CSTR("<tr><td>"));
				writer.Write(CSTR("<a href=\"/browse?fname="));
				Text::TextBinEnc::URIEncoding::URIEncode(sbuff2, vfile->fileName->v);
				s = Text::XML::ToNewXMLText(sbuff2);
				writer.Write(s->ToCString());
				s->Release();
				writer.Write(CSTR("\">"));

				s = Text::XML::ToNewHTMLBodyText(vfile->fileName->v);
				writer.Write(s->ToCString());
				s->Release();
				writer.Write(CSTR("</a></td><td>"));
				sptr2 = Text::StrUInt64(sbuff2, vfile->fileSize);
				writer.Write(CSTRP(sbuff2, sptr2));
				writer.Write(CSTR("</td><td>"));

				sptr2 = IO::Path::GetFileExt(sbuff2, vfile->fileName->v, vfile->fileName->leng);
				Text::CStringNN mime = Net::MIME::GetMIMEFromExt(CSTRP(sbuff2, sptr2));
				writer.Write(mime);
				writer.WriteLine(CSTR("</td></tr>"));

				vfile->fileName->Release();
				MemFreeNN(vfile);
				i++;
			}
		}
		writer.WriteLine(CSTR("</table>"));

		writer.WriteLine(CSTR("</body>"));
		writer.WriteLine(CSTR("</html>"));
	}

	resp->AddDefHeaders(req);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	buff = mstm.GetBuff(size);
	resp->AddContentLength(size);
	resp->AddContentType(CSTR("text/html; charset=UTF-8"));
	resp->Write(Data::ByteArrayR(buff, size));
}

void Media::MediaPlayerWebInterface::DoWebRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	NN<Text::String> reqURI = req->GetRequestURI();
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
	UnsafeArray<UInt8> buff;
	NN<Text::String> s;
	UOSInt size;

	IO::MemoryStream mstm;
	{
		Text::UTF8Writer writer(mstm);

		writer.WriteLine(CSTR("<html>"));
		writer.WriteLine(CSTR("<head><title>HQMP Control</title>"));
		writer.WriteLine(CSTR("</head>"));
		writer.WriteLine(CSTR("<body>"));
		writer.WriteLine(CSTR("<a href=\"/\">Refresh</a><br/><br/>"));
		writer.Write(CSTR("<b>Current File: </b>"));
		NN<Media::MediaFile> openedFile;
		if (this->iface->GetOpenedFile().SetTo(openedFile))
		{
			s = Text::XML::ToNewHTMLBodyText(openedFile->GetSourceNameObj()->v);
			writer.Write(s->ToCString());
			s->Release();

			writer.Write(CSTR(" <a href=\"/browse\">Browse</a>"));
		}
		else
		{
			writer.Write(CSTR("-"));
		}
		writer.WriteLine(CSTR("<br/>"));

		writer.WriteLine(CSTR("<input type=\"button\" value=\"Stop\" onclick=\"document.location.replace('/stop')\"/>"));
		writer.WriteLine(CSTR("<input type=\"button\" value=\"Start\" onclick=\"document.location.replace('/start')\"/>"));
		writer.WriteLine(CSTR("<input type=\"button\" value=\"Pause\" onclick=\"document.location.replace('/pause')\"/>"));
		writer.WriteLine(CSTR("<br/>"));
		writer.WriteLine(CSTR("<a href=\"/backward60\">Backward 1 Minute</a>"));
		writer.WriteLine(CSTR("<a href=\"/backward10\">Backward 10 Seconds</a>"));
		writer.WriteLine(CSTR("<a href=\"/forward10\">Forward 10 Seconds</a>"));
		writer.WriteLine(CSTR("<a href=\"/forward60\">Forward 1 Minute</a>"));
		writer.WriteLine(CSTR("<br/>"));
		writer.WriteLine(CSTR("<a href=\"/prevchap\">Previous Chapter</a>"));
		writer.WriteLine(CSTR("<a href=\"/nextchap\">Next Chapter</a>"));
		writer.WriteLine(CSTR("<a href=\"/avofstdec\">A/V Offset Decrease</a>"));
		writer.WriteLine(CSTR("<a href=\"/avofstinc\">A/V Offset Increase</a>"));
		NN<Media::VideoRenderer> vrenderer;
		if (this->iface->GetVideoRenderer().SetTo(vrenderer))
		{
			Text::StringBuilderUTF8 sb;
			Media::VideoRenderer::RendererStatus2 status;

			writer.WriteLine(CSTR("<hr/>"));
			vrenderer->GetStatus(status);
			sb.AppendC(UTF8STRC("Curr Time: "));
			sb.AppendDur(status.currTime);
			sb.AppendC(UTF8STRC("<br/>\r\n"));
			sb.AppendC(UTF8STRC("Disp Frame Time: "));
			sb.AppendDur(status.dispFrameTime);
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
			sb.AppendDur(status.videoDelay);
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
			sb.AppendC(UTF8STRC("Rotate Type: "));
			sb.AppendOpt(Media::RotateTypeGetName(status.rotateType));
			sb.AppendC(UTF8STRC("<br/>\r\n"));
			sb.AppendC(UTF8STRC("Decoder: "));
			Text::CStringNN nns;
			if (status.decoderName.SetTo(nns))
			{
				sb.Append(nns);
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
			NN<Media::ColorProfile::ColorPrimaries> primaries = status.color.GetPrimaries(); 
			sb.AppendC(UTF8STRC("Src RGB Primary: "));
			sb.Append(Media::ColorProfile::ColorTypeGetName(primaries->colorType));
			sb.AppendC(UTF8STRC("<br/>\r\n"));
			sb.AppendC(UTF8STRC("-Red:   "));
			sb.AppendDouble(primaries->r.x);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendDouble(primaries->r.y);
			sb.AppendC(UTF8STRC("<br/>\r\n"));
			sb.AppendC(UTF8STRC("-Green: "));
			sb.AppendDouble(primaries->g.x);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendDouble(primaries->g.y);
			sb.AppendC(UTF8STRC("<br/>\r\n"));
			sb.AppendC(UTF8STRC("-Blue:  "));
			sb.AppendDouble(primaries->b.x);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendDouble(primaries->b.y);
			sb.AppendC(UTF8STRC("<br/>\r\n"));
			sb.AppendC(UTF8STRC("-White: "));
			sb.AppendDouble(primaries->w.x);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendDouble(primaries->w.y);
			sb.AppendC(UTF8STRC("<br/>\r\n"));
			writer.Write(sb.ToCString());
		}

		writer.WriteLine(CSTR("</body>"));
		writer.WriteLine(CSTR("</html>"));
	}

	resp->AddDefHeaders(req);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	buff = mstm.GetBuff(size);
	resp->AddContentLength(size);
	resp->AddContentType(CSTR("text/html; charset=UTF-8"));
	resp->Write(Data::ByteArrayR(buff, size));
}

void Media::MediaPlayerWebInterface::Release()
{
	if (this->autoRelease)
	{
		DEL_CLASS(this);
	}
}
