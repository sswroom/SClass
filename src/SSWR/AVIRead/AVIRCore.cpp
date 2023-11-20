#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/EXEFile.h"
#include "IO/FileCheck.h"
#include "IO/FileStream.h"
#include "IO/ISectorData.h"
#include "IO/MemoryStream.h"
#include "IO/ModemController.h"
#include "IO/Path.h"
#include "Math/Math.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Net/WebBrowser.h"
#include "Parser/FullParserList.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRGISForm.h"
#include "SSWR/AVIRead/AVIRGSMModemForm.h"
#include "SSWR/AVIRead/AVIRHexViewerForm.h"
#include "SSWR/AVIRead/AVIRSelStreamForm.h"
#include "Text/MyStringW.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"
#include "UI/GUIForm.h"

void __stdcall SSWR::AVIRead::AVIRCore::FormClosed(void *userObj, UI::GUIForm *frm)
{
	SSWR::AVIRead::AVIRCore *me = (SSWR::AVIRead::AVIRCore *)userObj;
	me->frms.RemoveAt(me->frms.IndexOf(frm));
	if (me->gisForm == frm)
	{
		me->gisForm = 0;
	}
}

void SSWR::AVIRead::AVIRCore::InitForm(UI::GUIForm *frm)
{
	frm->HandleFormClosed(FormClosed, this);
	this->frms.Add(frm);
}

SSWR::AVIRead::AVIRCore::AVIRCore(NotNullPtr<UI::GUICore> ui) : vioPinMgr(4)
{
	WChar wbuff[512];
	WChar wbuff2[32];
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	this->ui = ui;
	this->forwardedUI = this->ui->IsForwarded();
	this->currCodePage = 0;
	this->eng = ui->CreateDrawEngine();
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("CacheDir"));
	NEW_CLASS(this->parsers, Parser::FullParserList());
	NEW_CLASSNN(this->sockf, Net::OSSocketFactory(true));
	this->ssl = Net::SSLEngineFactory::Create(this->sockf, true);
	NEW_CLASS(this->browser, Net::WebBrowser(sockf, this->ssl, CSTRP(sbuff, sptr)));
	NEW_CLASS(this->gpioCtrl, IO::GPIOControl());
	if (this->gpioCtrl->IsError())
	{
		DEL_CLASS(this->gpioCtrl);
		this->gpioCtrl = 0;
	}
	NEW_CLASS(this->siLabDriver, IO::SiLabDriver());
	if (this->siLabDriver->IsError())
	{
		DEL_CLASS(this->siLabDriver);
		this->siLabDriver = 0;
	}
	this->parsers->SetEncFactory(&this->encFact);
	this->parsers->SetMapManager(&this->mapMgr);
	this->parsers->SetWebBrowser(this->browser);
	this->parsers->SetSocketFactory(this->sockf);
	this->parsers->SetSSLEngine(this->ssl);
	this->parsers->SetLogTool(&this->log);
	this->batchLyrs = 0;
	this->batchLoad = false;
	this->gisForm = 0;
	this->ui->SetMonitorMgr(&this->monMgr);

	IO::Registry *reg = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"AVIRead");
	if (reg)
	{
		OSInt i = 0;
		while (true)
		{
			Text::StrOSInt(Text::StrConcatC(wbuff2, L"AudioDevice", 11), i);
			if (reg->GetValueStr(wbuff2, wbuff) != 0)
			{
				NotNullPtr<Text::String> devName = Text::String::NewNotNull(wbuff);
				this->audDevice.AddDevice(devName->ToCString());
				this->audDevList.Add(devName);
			}
			else
			{
				break;
			}
			i++;
		}
		IO::Registry::CloseRegistry(reg);
	}
}

SSWR::AVIRead::AVIRCore::~AVIRCore()
{
	UOSInt i;
	this->CloseAllForm();
	DEL_CLASS(this->parsers);
	DEL_CLASS(this->browser);
	SDEL_CLASS(this->ssl);
	this->sockf.Delete();
	this->eng.Delete();
	this->ui->SetMonitorMgr(0);
	SDEL_CLASS(this->gpioCtrl);
	SDEL_CLASS(this->siLabDriver);
	i = this->audDevList.GetCount();
	while (i-- > 0)
	{
		this->audDevList.GetItem(i)->Release();
	}
}

void SSWR::AVIRead::AVIRCore::OpenGSMModem(IO::Stream *modemPort)
{
	SSWR::AVIRead::AVIRGSMModemForm *frm;
	NEW_CLASS(frm, SSWR::AVIRead::AVIRGSMModemForm(0, ui, *this, modemPort));
	InitForm(frm);
	frm->Show();
}

IO::Stream *SSWR::AVIRead::AVIRCore::OpenStream(IO::StreamType *st, UI::GUIForm *ownerFrm, Int32 defBaudRate, Bool allowReadOnly)
{
	IO::Stream *retStm = 0;
	SSWR::AVIRead::AVIRSelStreamForm frm(0, this->ui, *this, allowReadOnly, this->ssl);
	if (defBaudRate != 0)
	{
		frm.SetInitBaudRate(defBaudRate);
	}
	if (frm.ShowDialog(ownerFrm) == UI::GUIForm::DR_OK)
	{
		retStm = frm.GetStream().Ptr();
		if (st)
		{
			*st = frm.GetStreamType();
		}
	}
	return retStm;
}

void SSWR::AVIRead::AVIRCore::OpenHex(NotNullPtr<IO::StreamData> fd, IO::FileAnalyse::IFileAnalyse *fileAnalyse)
{
	SSWR::AVIRead::AVIRHexViewerForm *frm;
	NEW_CLASS(frm, SSWR::AVIRead::AVIRHexViewerForm(0, ui, *this));
	InitForm(frm);
	frm->SetData(fd, fileAnalyse);
	frm->Show();
}

void SSWR::AVIRead::AVIRCore::BeginLoad()
{
	this->batchLoad = true;
}

void SSWR::AVIRead::AVIRCore::EndLoad()
{
	this->batchLoad = false;
	if (this->batchLyrs)
	{
		AVIRead::AVIRGISForm *gisForm;
		NotNullPtr<Map::MapEnv> env;
		NotNullPtr<Map::MapView> view;
		NotNullPtr<Math::CoordinateSystem> csys = this->batchLyrs->GetItem(0)->GetCoordinateSystem();
		NEW_CLASSNN(env, Map::MapEnv(CSTR("Untitled"), 0xffc0c0ff, csys->Clone()));
		if (this->batchLyrs->GetCount() > 0)
		{
			view = this->batchLyrs->GetItem(0)->CreateMapView(Math::Size2DDbl(320, 240));
		}
		else
		{
			view = env->CreateMapView(Math::Size2DDbl(320, 240));
		}
		NEW_CLASS(gisForm, AVIRead::AVIRGISForm(0, this->ui, *this, env, view));
		gisForm->AddLayers(this->batchLyrs);
		DEL_CLASS(this->batchLyrs);
		this->batchLyrs = 0;
		InitForm(gisForm);
		gisForm->Show();
	}
}

Bool SSWR::AVIRead::AVIRCore::LoadData(NotNullPtr<IO::StreamData> data, IO::PackageFile *pkgFile)
{
	IO::ParserType pt;
	NotNullPtr<IO::ParsedObject> pobj;
	if (pobj.Set(this->parsers->ParseFile(data, pkgFile, &pt)))
	{
		OpenObject(pobj);
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::AVIRead::AVIRCore::LoadDataType(NotNullPtr<IO::StreamData> data, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	NotNullPtr<IO::ParsedObject> pobj;
	IO::ParserType pt;
	if (pobj.Set(this->parsers->ParseFile(data, pkgFile, &pt, targetType)))
	{
		OpenObject(pobj);
		return true;
	}
	else
	{
		return false;
	}
}

Parser::ParserList *SSWR::AVIRead::AVIRCore::GetParserList()
{
	return this->parsers;
}

Map::MapManager *SSWR::AVIRead::AVIRCore::GetMapManager()
{
	return &this->mapMgr;
}

Media::ColorManager *SSWR::AVIRead::AVIRCore::GetColorMgr()
{
	return &this->colorMgr;
}

NotNullPtr<Net::SocketFactory> SSWR::AVIRead::AVIRCore::GetSocketFactory()
{
	return this->sockf;
}

NotNullPtr<Media::DrawEngine> SSWR::AVIRead::AVIRCore::GetDrawEngine()
{
	return this->eng;
}

Text::EncodingFactory *SSWR::AVIRead::AVIRCore::GetEncFactory()
{
	return &this->encFact;
}

IO::SiLabDriver *SSWR::AVIRead::AVIRCore::GetSiLabDriver()
{
	return this->siLabDriver;
}

Net::WebBrowser *SSWR::AVIRead::AVIRCore::GetWebBrowser()
{
	return this->browser;
}

IO::VirtualIOPinMgr *SSWR::AVIRead::AVIRCore::GetVirtualIOPinMgr()
{
	return &this->vioPinMgr;
}

IO::GPIOControl *SSWR::AVIRead::AVIRCore::GetGPIOControl()
{
	return this->gpioCtrl;
}

Media::AudioDevice *SSWR::AVIRead::AVIRCore::GetAudioDevice()
{
	return &this->audDevice;
}

UInt32 SSWR::AVIRead::AVIRCore::GetCurrCodePage()
{
	if (this->currCodePage == 0)
		return Text::EncodingFactory::GetSystemCodePage();
	else
		return this->currCodePage;
}
void SSWR::AVIRead::AVIRCore::SetCodePage(UInt32 codePage)
{
	this->currCodePage = codePage;
	this->parsers->SetCodePage(codePage);
	this->exporters.SetCodePage(codePage);
}

NotNullPtr<IO::LogTool> SSWR::AVIRead::AVIRCore::GetLog()
{
	return this->log;
}

Double SSWR::AVIRead::AVIRCore::GetMonitorHDPI(MonitorHandle *hMonitor)
{
	if (this->forwardedUI)
	{
		return this->ui->GetMagnifyRatio(hMonitor) * 96.0;
	}
	else
	{
		return this->monMgr.GetMonitorHDPI(hMonitor);
	}
}

void SSWR::AVIRead::AVIRCore::SetMonitorHDPI(MonitorHandle *hMonitor, Double monitorHDPI)
{
	if (!this->forwardedUI)
	{
		this->monMgr.SetMonitorHDPI(hMonitor, monitorHDPI);
	}
}

Double SSWR::AVIRead::AVIRCore::GetMonitorDDPI(MonitorHandle *hMonitor)
{
	if (this->forwardedUI)
	{
		return 96.0;
	}
	else
	{
		return this->monMgr.GetMonitorDDPI(hMonitor);
	}
}

void SSWR::AVIRead::AVIRCore::SetMonitorDDPI(MonitorHandle *hMonitor, Double monitorDDPI)
{
	if (!this->forwardedUI)
	{
		this->monMgr.SetMonitorDDPI(hMonitor, monitorDDPI);
	}
}

Media::MonitorMgr *SSWR::AVIRead::AVIRCore::GetMonitorMgr()
{
	return &this->monMgr;
}

void SSWR::AVIRead::AVIRCore::SetAudioDeviceList(Data::ArrayListNN<Text::String> *devList)
{
	IO::Registry *reg = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"AVIRead");
	WChar wbuff[32];
	UOSInt i;
	UOSInt j;
	i = this->audDevList.GetCount();
	while (i-- > 0)
	{
		this->audDevList.GetItem(i)->Release();
	}
	this->audDevList.Clear();
	if (reg)
	{
		if (devList == 0)
		{
			reg->DelValue(L"AudioDevice0");
		}
		else
		{
			i = 0;
			j = devList->GetCount();
			while (i < j)
			{
				Text::StrUOSInt(Text::StrConcat(wbuff, L"AudioDevice"), i);
				const WChar *wptr = Text::StrToWCharNew(devList->GetItem(i)->v);
				reg->SetValue(wbuff, wptr);
				Text::StrDelNew(wptr);
				i++;
			}
			Text::StrUOSInt(Text::StrConcat(wbuff, L"AudioDevice"), j);
			reg->DelValue(wbuff);
		}
		IO::Registry::CloseRegistry(reg);
	}
	this->audDevice.ClearDevices();
	if (devList)
	{
		i = 0;
		j = devList->GetCount();
		while (i < j)
		{
			this->audDevice.AddDevice(devList->GetItem(i)->ToCString());
			this->audDevList.Add(devList->GetItem(i)->Clone());
			i++;
		}
	}
}

Data::ArrayListNN<Text::String> *SSWR::AVIRead::AVIRCore::GetAudioDeviceList()
{
	return &this->audDevList;
}

Int32 SSWR::AVIRead::AVIRCore::GetAudioAPIType()
{
	return this->audAPIType;
}

Media::IAudioRenderer *SSWR::AVIRead::AVIRCore::BindAudio(Media::IAudioSource *audSrc)
{
	return this->audDevice.BindAudio(audSrc);
}

Bool SSWR::AVIRead::AVIRCore::GenLinePreview(NotNullPtr<Media::DrawImage> img, NotNullPtr<Media::DrawEngine> eng, UOSInt lineThick, UInt32 lineColor, Media::ColorConv *colorConv)
{
	Media::DrawPen *p;
	NotNullPtr<Media::DrawBrush> b;
	Double dpi = img->GetHDPI();
	b = img->NewBrushARGB(colorConv->ConvRGB8(0xffffffff));
	img->DrawRect(Math::Coord2DDbl(0, 0), img->GetSize().ToDouble(), 0, b);
	img->DelBrush(b);

	p = img->NewPenARGB(colorConv->ConvRGB8(lineColor), UOSInt2Double(lineThick) * dpi / 96.0, 0, 0);
	img->DrawLine(0, UOSInt2Double(img->GetHeight()) * 0.5, UOSInt2Double(img->GetWidth()), UOSInt2Double(img->GetHeight()) * 0.5, p);
	img->DelPen(p);
	return true;
}

Bool SSWR::AVIRead::AVIRCore::GenLineStylePreview(NotNullPtr<Media::DrawImage> img, NotNullPtr<Media::DrawEngine> eng, NotNullPtr<Map::MapEnv> env, UOSInt lineStyle, Media::ColorConv *colorConv)
{
	Math::Size2D<UOSInt> size = img->GetSize();
	Double dpi = img->GetHDPI();
	if (lineStyle >= env->GetLineStyleCount())
	{
		NotNullPtr<Media::DrawFont> f = img->NewFontPt(CSTR("Arial"), 9, Media::DrawEngine::DFS_ANTIALIAS, 0);
		NotNullPtr<Media::DrawBrush> b = img->NewBrushARGB(colorConv->ConvRGB8(0xffffffff));
		img->DrawRect(Math::Coord2DDbl(0, 0), size.ToDouble(), 0, b);
		img->DelBrush(b);
		b = img->NewBrushARGB(colorConv->ConvRGB8(0xff000000));
		img->DrawString(Math::Coord2DDbl(0, 0), CSTR("No line style"), f, b);
		img->DelBrush(b);
		img->DelFont(f);
		return false;
	}

	Media::DrawPen *p;
	NotNullPtr<Media::DrawBrush> b;
	b = img->NewBrushARGB(colorConv->ConvRGB8(0xffc0c0c0));
	img->DrawRect(Math::Coord2DDbl(0, 0), size.ToDouble(), 0, b);
	img->DelBrush(b);

	UInt32 color;
	UOSInt layerId = 0;
	UOSInt thick;
	UInt8 *pattern;
	UOSInt npattern;

	while (env->GetLineStyleLayer(lineStyle, layerId++, color, thick, pattern, npattern))
	{
		p = img->NewPenARGB(colorConv->ConvRGB8(color), UOSInt2Double(thick) * dpi / 96.0, pattern, npattern);
		img->DrawLine(0, UOSInt2Double(size.y >> 1), UOSInt2Double(size.x), UOSInt2Double(size.y >> 1), p);
		img->DelPen(p);
	}
	return true;
}

Bool SSWR::AVIRead::AVIRCore::GenFontStylePreview(NotNullPtr<Media::DrawImage> img, NotNullPtr<Media::DrawEngine> eng, NotNullPtr<Map::MapEnv> env, UOSInt fontStyle, Media::ColorConv *colorConv)
{
	Math::Size2D<UOSInt> size = img->GetSize();
	Double dpi = img->GetHDPI();
	
	if (fontStyle >= env->GetFontStyleCount())
	{
		NotNullPtr<Media::DrawFont> f = img->NewFontPt(CSTR("Arial"), 9.0, Media::DrawEngine::DFS_ANTIALIAS, 0);
		NotNullPtr<Media::DrawBrush> b = img->NewBrushARGB(colorConv->ConvRGB8(0xffffffff));
		img->DrawRect(Math::Coord2DDbl(0, 0), size.ToDouble(), 0, b);
		img->DelBrush(b);
		b = img->NewBrushARGB(colorConv->ConvRGB8(0xff000000));
		img->DrawString(Math::Coord2DDbl(0, 0), CSTR("No font style"), f, b);
		img->DelBrush(b);
		img->DelFont(f);
		return false;
	}

	NotNullPtr<Media::DrawFont> f;
	NotNullPtr<Media::DrawBrush> b;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Math::Coord2DDbl refPos;
	b = img->NewBrushARGB(colorConv->ConvRGB8(0xffffffff));
	img->DrawRect(Math::Coord2DDbl(0, 0), size.ToDouble(), 0, b);
	img->DelBrush(b);

	NotNullPtr<Text::String> fontName;
	Double fontSizePt;
	Bool bold;
	UInt32 fontColor;
	UOSInt buffSize;
	UInt32 buffColor;
	Math::Size2DDbl sz;

	if (env->GetFontStyle(fontStyle, fontName, fontSizePt, bold, fontColor, buffSize, buffColor))
	{
		buffSize = (UOSInt)Double2Int32(UOSInt2Double(buffSize) * dpi / 96.0);
		if ((sptr = env->GetFontStyleName(fontStyle, sbuff)) == 0)
		{
			sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Style ")), fontStyle);
		}
		f = img->NewFontPt(fontName->ToCString(), fontSizePt, bold?((Media::DrawEngine::DrawFontStyle)(Media::DrawEngine::DFS_BOLD | Media::DrawEngine::DFS_ANTIALIAS)):Media::DrawEngine::DFS_ANTIALIAS, this->currCodePage);
		sz = img->GetTextSize(f, CSTRP(sbuff, sptr));
		refPos = (size.ToDouble() - sz) * 0.5;
		if (buffSize > 0)
		{
			b = img->NewBrushARGB(colorConv->ConvRGB8(buffColor));
			img->DrawStringB(refPos, CSTRP(sbuff, sptr), f, b, buffSize);
			img->DelBrush(b);
		}
		b = img->NewBrushARGB(colorConv->ConvRGB8(fontColor));
		img->DrawString(refPos, CSTRP(sbuff, sptr), f, b);
		img->DelBrush(b);

		img->DelFont(f);
	}
	return true;
}

Bool SSWR::AVIRead::AVIRCore::GenFontPreview(NotNullPtr<Media::DrawImage> img, NotNullPtr<Media::DrawEngine> eng, Text::CStringNN fontName, Double fontSizePt, UInt32 fontColor, Media::ColorConv *colorConv)
{
	if (fontName.leng == 0)
	{
		fontName = CSTR("Arial");
	}
	Math::Size2DDbl sz;
	NotNullPtr<Media::DrawFont> f;
	NotNullPtr<Media::DrawBrush> b;
	b = img->NewBrushARGB(colorConv->ConvRGB8(0xffffffff));
	img->DrawRect(Math::Coord2DDbl(0, 0), img->GetSize().ToDouble(), 0, b);
	img->DelBrush(b);

	b = img->NewBrushARGB(colorConv->ConvRGB8(fontColor));
	f = img->NewFontPt(fontName, fontSizePt, Media::DrawEngine::DFS_ANTIALIAS, this->currCodePage);
	sz = img->GetTextSize(f, fontName);
	img->DrawString(Math::Coord2DDbl((UOSInt2Double(img->GetWidth()) - sz.x) * 0.5, (UOSInt2Double(img->GetHeight()) - sz.y) * 0.5), fontName, f, b);
	img->DelFont(f);
	img->DelBrush(b);
	return true;
}

void SSWR::AVIRead::AVIRCore::ShowForm(UI::GUIForm *frm)
{
	frm->Show();
	this->InitForm(frm);
}

void SSWR::AVIRead::AVIRCore::CloseAllForm()
{
	UOSInt i = this->frms.GetCount();
	while (i-- > 0)
	{
		this->frms.GetItem(i)->Close();
	}
}

void SSWR::AVIRead::AVIRCore::SetGISForm(SSWR::AVIRead::AVIRGISForm *frm)
{
	this->gisForm = frm;
}

SSWR::AVIRead::AVIRGISForm *SSWR::AVIRead::AVIRCore::GetGISForm()
{
	return this->gisForm;
}

Text::CStringNN SSWR::AVIRead::AVIRCore::IOPinTypeGetName(IOPinType iopt)
{
	switch (iopt)
	{
	case AVIRCore::IOPT_GPIO:
		return CSTR("GPIO");
	case AVIRCore::IOPT_VIOPIN:
		return CSTR("VirtualIOPin");
	default:
		return CSTR("Unknown");
	}
}
