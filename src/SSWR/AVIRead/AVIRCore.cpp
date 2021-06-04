#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/EXEFile.h"
#include "IO/FileCheck.h"
#include "IO/FileStream.h"
#include "IO/ISectorData.h"
#include "IO/MemoryStream.h"
#include "IO/ModemController.h"
#include "IO/Path.h"
#include "IO/StmData/MemoryData2.h"
#include "Math/Math.h"
#include "Net/OSSocketFactory.h"
#include "Net/WebBrowser.h"
#include "Parser/FullParserList.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRGISForm.h"
#include "SSWR/AVIRead/AVIRGSMModemForm.h"
#include "SSWR/AVIRead/AVIRSelStreamForm.h"
#include "Text/MyStringW.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"
#include "UI/GUIForm.h"

void __stdcall SSWR::AVIRead::AVIRCore::FormClosed(void *userObj, UI::GUIForm *frm)
{
	SSWR::AVIRead::AVIRCore *me = (SSWR::AVIRead::AVIRCore *)userObj;
	me->frms->RemoveAt((UOSInt)me->frms->IndexOf(frm));
	if (me->gisForm == frm)
	{
		me->gisForm = 0;
	}
}

void SSWR::AVIRead::AVIRCore::InitForm(UI::GUIForm *frm)
{
	frm->HandleFormClosed(FormClosed, this);
	this->frms->Add(frm);
}

SSWR::AVIRead::AVIRCore::AVIRCore(UI::GUICore *ui)
{
	WChar sbuff[512];
	WChar sbuff2[32];
	UTF8Char u8buff[512];
	this->ui = ui;
	this->currCodePage = 0;
	this->eng = ui->CreateDrawEngine();
	IO::Path::GetProcessFileName(u8buff);
	IO::Path::AppendPath(u8buff, (const UTF8Char*)"CacheDir");
	NEW_CLASS(this->parsers, Parser::FullParserList());
	NEW_CLASS(this->mapMgr, Map::MapManager());
	NEW_CLASS(this->colorMgr, Media::ColorManager());
	NEW_CLASS(this->sockf, Net::OSSocketFactory(true));
	NEW_CLASS(this->encFact, Text::EncodingFactory());
	NEW_CLASS(this->exporters, Exporter::ExporterList());
	NEW_CLASS(this->browser, Net::WebBrowser(sockf, u8buff));
	NEW_CLASS(this->frms, Data::ArrayList<UI::GUIForm*>());
	NEW_CLASS(this->log, IO::LogTool());
	NEW_CLASS(this->monMgr, Media::MonitorMgr());
	NEW_CLASS(this->vioPinMgr, IO::VirtualIOPinMgr(4));
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
	this->parsers->SetEncFactory(this->encFact);
	this->parsers->SetMapManager(this->mapMgr);
	this->parsers->SetWebBrowser(this->browser);
	this->parsers->SetSocketFactory(this->sockf);
	this->batchLyrs = 0;
	this->batchLoad = false;
	NEW_CLASS(this->audDevList, Data::ArrayList<const UTF8Char *>());
	this->audDevice = 0;
	this->gisForm = 0;
	this->ui->SetMonitorMgr(this->monMgr);

	IO::Registry *reg = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"sswr", L"AVIRead");
	NEW_CLASS(this->audDevice, Media::AudioDevice());
	if (reg)
	{
		OSInt i = 0;
		while (true)
		{
			Text::StrOSInt(Text::StrConcat(sbuff2, L"AudioDevice"), i);
			if (reg->GetValueStr(sbuff2, sbuff) != 0)
			{
				const UTF8Char *devName = Text::StrToUTF8New(sbuff);
				this->audDevice->AddDevice(devName);
				this->audDevList->Add(devName);
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
	DEL_CLASS(this->frms);
	DEL_CLASS(this->log);
	DEL_CLASS(this->exporters);
	DEL_CLASS(this->parsers);
	DEL_CLASS(this->browser);
	DEL_CLASS(this->encFact);
	DEL_CLASS(this->mapMgr);
	DEL_CLASS(this->colorMgr);
	DEL_CLASS(this->sockf);
	DEL_CLASS(this->eng);
	this->ui->SetMonitorMgr(0);
	DEL_CLASS(this->monMgr);
	DEL_CLASS(this->vioPinMgr);
	SDEL_CLASS(this->gpioCtrl);
	SDEL_CLASS(this->siLabDriver);
	SDEL_CLASS(this->audDevice);
	i = this->audDevList->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->audDevList->GetItem(i));
	}
	SDEL_CLASS(this->audDevList);
}

void SSWR::AVIRead::AVIRCore::OpenGSMModem(IO::Stream *modemPort)
{
	SSWR::AVIRead::AVIRGSMModemForm *frm;
	IO::GSMModemController *modem;
	IO::ATCommandChannel *channel;
	NEW_CLASS(channel, IO::ATCommandChannel(modemPort, false));
	NEW_CLASS(modem, IO::GSMModemController(channel, false));

	NEW_CLASS(frm, SSWR::AVIRead::AVIRGSMModemForm(0, ui, this, modem, channel, modemPort));
	InitForm(frm);
	frm->Show();
}

IO::Stream *SSWR::AVIRead::AVIRCore::OpenStream(StreamType *st, UI::GUIForm *ownerFrm, Int32 defBaudRate, Bool allowReadOnly)
{
	IO::Stream *retStm = 0;
	SSWR::AVIRead::AVIRSelStreamForm *frm;
	NEW_CLASS(frm, SSWR::AVIRead::AVIRSelStreamForm(0, this->ui, this, allowReadOnly));
	if (defBaudRate != 0)
	{
		frm->SetInitBaudRate(defBaudRate);
	}
	if (frm->ShowDialog(ownerFrm) == UI::GUIForm::DR_OK)
	{
		retStm = frm->stm;
		if (st)
		{
			*st = frm->stmType;
		}
	}
	DEL_CLASS(frm);
	return retStm;
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
		Map::MapEnv *env;
		Map::MapView *view;
		Math::CoordinateSystem *csys = this->batchLyrs->GetItem(0)->GetCoordinateSystem();
		NEW_CLASS(env, Map::MapEnv((const UTF8Char*)"Untitled", 0xffc0c0ff, csys?(csys->Clone()):0));
		if (this->batchLyrs->GetCount() > 0)
		{
			view = this->batchLyrs->GetItem(0)->CreateMapView(320, 240);
		}
		else
		{
			view = env->CreateMapView(320, 240);
		}
		NEW_CLASS(gisForm, AVIRead::AVIRGISForm(0, this->ui, this, env, view));
		gisForm->AddLayers(this->batchLyrs);
		DEL_CLASS(this->batchLyrs);
		this->batchLyrs = 0;
		InitForm(gisForm);
		gisForm->Show();
	}
}

Bool SSWR::AVIRead::AVIRCore::LoadData(IO::IStreamData *data, IO::PackageFile *pkgFile)
{
	IO::ParsedObject::ParserType pt;
	IO::ParsedObject *pobj;
	pobj = this->parsers->ParseFile(data, pkgFile, &pt);
	if (pobj)
	{
		OpenObject(pobj);
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::AVIRead::AVIRCore::LoadDataType(IO::IStreamData *data, IO::ParsedObject::ParserType targetType)
{
	IO::ParsedObject *pobj;
	pobj = this->parsers->ParseFileType(data, targetType);
	if (pobj)
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
	return this->mapMgr;
}

Media::ColorManager *SSWR::AVIRead::AVIRCore::GetColorMgr()
{
	return this->colorMgr;
}

Net::SocketFactory *SSWR::AVIRead::AVIRCore::GetSocketFactory()
{
	return this->sockf;
}

Media::DrawEngine *SSWR::AVIRead::AVIRCore::GetDrawEngine()
{
	return this->eng;
}

Text::EncodingFactory *SSWR::AVIRead::AVIRCore::GetEncFactory()
{
	return this->encFact;
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
	return this->vioPinMgr;
}

IO::GPIOControl *SSWR::AVIRead::AVIRCore::GetGPIOControl()
{
	return this->gpioCtrl;
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
	this->exporters->SetCodePage(codePage);
}

IO::LogTool *SSWR::AVIRead::AVIRCore::GetLog()
{
	return this->log;
}

Double SSWR::AVIRead::AVIRCore::GetMonitorHDPI(void *hMonitor)
{
	return this->monMgr->GetMonitorHDPI(hMonitor);
}

void SSWR::AVIRead::AVIRCore::SetMonitorHDPI(void *hMonitor, Double monitorHDPI)
{
	this->monMgr->SetMonitorHDPI(hMonitor, monitorHDPI);
}

Double SSWR::AVIRead::AVIRCore::GetMonitorDDPI(void *hMonitor)
{
	return this->monMgr->GetMonitorDDPI(hMonitor);
}

void SSWR::AVIRead::AVIRCore::SetMonitorDDPI(void *hMonitor, Double monitorDDPI)
{
	this->monMgr->SetMonitorDDPI(hMonitor, monitorDDPI);
}

Media::MonitorMgr *SSWR::AVIRead::AVIRCore::GetMonitorMgr()
{
	return this->monMgr;
}

void SSWR::AVIRead::AVIRCore::SetAudioDevice(Data::ArrayList<const UTF8Char *> *devList)
{
	IO::Registry *reg = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"sswr", L"AVIRead");
	WChar sbuff[32];
	UOSInt i;
	UOSInt j;
	i = this->audDevList->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->audDevList->GetItem(i));
	}
	this->audDevList->Clear();
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
				Text::StrUOSInt(Text::StrConcat(sbuff, L"AudioDevice"), i);
				const WChar *wptr = Text::StrToWCharNew(devList->GetItem(i));
				reg->SetValue(sbuff, wptr);
				Text::StrDelNew(wptr);
				i++;
			}
			Text::StrUOSInt(Text::StrConcat(sbuff, L"AudioDevice"), j);
			reg->DelValue(sbuff);
		}
		IO::Registry::CloseRegistry(reg);
	}
	SDEL_CLASS(this->audDevice);
	NEW_CLASS(this->audDevice, Media::AudioDevice());
	if (devList)
	{
		i = 0;
		j = devList->GetCount();
		while (i < j)
		{
			this->audDevice->AddDevice(devList->GetItem(i));
			this->audDevList->Add(Text::StrCopyNew(devList->GetItem(i)));
			i++;
		}
	}
}

Data::ArrayList<const UTF8Char *> *SSWR::AVIRead::AVIRCore::GetAudioDevices()
{
	return this->audDevList;
}

Int32 SSWR::AVIRead::AVIRCore::GetAudioAPIType()
{
	return this->audAPIType;
}

Media::IAudioRenderer *SSWR::AVIRead::AVIRCore::BindAudio(Media::IAudioSource *audSrc)
{
	if (this->audDevice == 0)
		return 0;
	return this->audDevice->BindAudio(audSrc);
}

Bool SSWR::AVIRead::AVIRCore::GenLinePreview(Media::DrawImage *img, Media::DrawEngine *eng, UOSInt lineThick, UInt32 lineColor, Media::ColorConv *colorConv)
{
	Media::DrawPen *p;
	Media::DrawBrush *b;
	Double dpi = img->GetHDPI();
	b = img->NewBrushARGB(colorConv->ConvRGB8(0xffffffff));
	img->DrawRect(0, 0, Math::UOSInt2Double(img->GetWidth()), Math::UOSInt2Double(img->GetHeight()), 0, b);
	img->DelBrush(b);

	p = img->NewPenARGB(colorConv->ConvRGB8(lineColor), Math::UOSInt2Double(lineThick) * dpi / 96.0, 0, 0);
	img->DrawLine(0, Math::UOSInt2Double(img->GetHeight()) * 0.5, Math::UOSInt2Double(img->GetWidth()), Math::UOSInt2Double(img->GetHeight()) * 0.5, p);
	img->DelPen(p);
	return true;
}

Bool SSWR::AVIRead::AVIRCore::GenLineStylePreview(Media::DrawImage *img, Media::DrawEngine *eng, Map::MapEnv *env, UOSInt lineStyle, Media::ColorConv *colorConv)
{
	UOSInt w = img->GetWidth();
	UOSInt h = img->GetHeight();
	Double dpi = img->GetHDPI();
	if (lineStyle >= env->GetLineStyleCount())
	{
		Media::DrawFont *f = img->NewFontPt((const UTF8Char*)"Arial", 9, Media::DrawEngine::DFS_ANTIALIAS, 0);
		Media::DrawBrush *b = img->NewBrushARGB(colorConv->ConvRGB8(0xffffffff));
		img->DrawRect(0, 0, Math::UOSInt2Double(w), Math::UOSInt2Double(h), 0, b);
		img->DelBrush(b);
		b = img->NewBrushARGB(colorConv->ConvRGB8(0xff000000));
		img->DrawString(0, 0, (const UTF8Char*)"No line style", f, b);
		img->DelBrush(b);
		img->DelFont(f);
		return false;
	}

	Media::DrawPen *p;
	Media::DrawBrush *b;
	b = img->NewBrushARGB(colorConv->ConvRGB8(0xffc0c0c0));
	img->DrawRect(0, 0, Math::UOSInt2Double(w), Math::UOSInt2Double(h), 0, b);
	img->DelBrush(b);

	UInt32 color;
	UOSInt layerId = 0;
	UOSInt thick;
	UInt8 *pattern;
	UOSInt npattern;

	while (env->GetLineStyleLayer(lineStyle, layerId++, &color, &thick, &pattern, &npattern))
	{
		p = img->NewPenARGB(colorConv->ConvRGB8(color), Math::UOSInt2Double(thick) * dpi / 96.0, pattern, npattern);
		img->DrawLine(0, Math::UOSInt2Double(h >> 1), Math::UOSInt2Double(w), Math::UOSInt2Double(h >> 1), p);
		img->DelPen(p);
	}
	return true;
}

Bool SSWR::AVIRead::AVIRCore::GenFontStylePreview(Media::DrawImage *img, Media::DrawEngine *eng, Map::MapEnv *env, UOSInt fontStyle, Media::ColorConv *colorConv)
{
	UOSInt w = img->GetWidth();
	UOSInt h = img->GetHeight();
	Double dpi = img->GetHDPI();
	
	if (fontStyle >= env->GetFontStyleCount())
	{
		Media::DrawFont *f = img->NewFontPt((const UTF8Char*)"Arial", 9.0, Media::DrawEngine::DFS_ANTIALIAS, 0);
		Media::DrawBrush *b = img->NewBrushARGB(colorConv->ConvRGB8(0xffffffff));
		img->DrawRect(0, 0, Math::UOSInt2Double(w), Math::UOSInt2Double(h), 0, b);
		img->DelBrush(b);
		b = img->NewBrushARGB(colorConv->ConvRGB8(0xff000000));
		img->DrawString(0, 0, (const UTF8Char*)"No font style", f, b);
		img->DelBrush(b);
		img->DelFont(f);
		return false;
	}

	Media::DrawFont *f;
	Media::DrawBrush *b;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Double refX;
	Double refY;
	b = img->NewBrushARGB(colorConv->ConvRGB8(0xffffffff));
	img->DrawRect(0, 0, Math::UOSInt2Double(w), Math::UOSInt2Double(h), 0, b);
	img->DelBrush(b);

	const UTF8Char *fontName;
	Double fontSizePt;
	Bool bold;
	UInt32 fontColor;
	UOSInt buffSize;
	UInt32 buffColor;
	Double sz[2];

	if (env->GetFontStyle(fontStyle, &fontName, &fontSizePt, &bold, &fontColor, &buffSize, &buffColor))
	{
		buffSize = (UOSInt)Math::Double2Int32(Math::UOSInt2Double(buffSize) * dpi / 96.0);
		if ((sptr = env->GetFontStyleName(fontStyle, sbuff)) == 0)
		{
			sptr = Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Style "), fontStyle);
		}
		f = img->NewFontPt(fontName, fontSizePt, bold?((Media::DrawEngine::DrawFontStyle)(Media::DrawEngine::DFS_BOLD | Media::DrawEngine::DFS_ANTIALIAS)):Media::DrawEngine::DFS_ANTIALIAS, this->currCodePage);
		img->GetTextSize(f, sbuff, (sptr - sbuff), sz);
		refX = (Math::UOSInt2Double(w) - sz[0]) * 0.5;
		refY = (Math::UOSInt2Double(h) - sz[1]) * 0.5;
		if (buffSize > 0)
		{
			b = img->NewBrushARGB(colorConv->ConvRGB8(buffColor));
			img->DrawStringB(refX, refY, sbuff, f, b, buffSize);
			img->DelBrush(b);
		}
		b = img->NewBrushARGB(colorConv->ConvRGB8(fontColor));
		img->DrawString(refX, refY, sbuff, f, b);
		img->DelBrush(b);

		img->DelFont(f);
	}
	return true;
}

Bool SSWR::AVIRead::AVIRCore::GenFontPreview(Media::DrawImage *img, Media::DrawEngine *eng, const UTF8Char *fontName, Double fontSizePt, UInt32 fontColor, Media::ColorConv *colorConv)
{
	if (fontName == 0)
	{
		fontName = (const UTF8Char*)"Arial";
	}
	Double sz[2];
	UOSInt strLeng;
	Media::DrawFont *f;
	Media::DrawBrush *b;
	b = img->NewBrushARGB(colorConv->ConvRGB8(0xffffffff));
	img->DrawRect(0, 0, Math::UOSInt2Double(img->GetWidth()), Math::UOSInt2Double(img->GetHeight()), 0, b);
	img->DelBrush(b);

	b = img->NewBrushARGB(colorConv->ConvRGB8(fontColor));
	f = img->NewFontPt(fontName, fontSizePt, Media::DrawEngine::DFS_ANTIALIAS, this->currCodePage);
	strLeng = Text::StrCharCnt(fontName);
	img->GetTextSize(f, fontName, (OSInt)strLeng, sz);
	img->DrawString((Math::UOSInt2Double(img->GetWidth()) - sz[0]) * 0.5, (Math::UOSInt2Double(img->GetHeight()) - sz[1]) * 0.5, fontName, f, b);
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
	UOSInt i = this->frms->GetCount();
	while (i-- > 0)
	{
		this->frms->GetItem(i)->Close();
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

const UTF8Char *SSWR::AVIRead::AVIRCore::GetStreamTypeName(StreamType st)
{
	switch (st)
	{
	case AVIRCore::ST_SERIAL_PORT:
		return (const UTF8Char*)"Serial Port";
	case AVIRCore::ST_USBXPRESS:
		return (const UTF8Char*)"Silicon Laboratories USBXpress";
	case AVIRCore::ST_TCPSERVER:
		return (const UTF8Char*)"TCP Server";
	case AVIRCore::ST_TCPCLIENT:
		return (const UTF8Char*)"TCP Client";
	case AVIRCore::ST_FILE:
		return (const UTF8Char*)"File (Readonly)";
	case AVIRCore::ST_HID:
		return (const UTF8Char*)"HID";
	default:
		return (const UTF8Char*)"Unknown";
	}
}

const UTF8Char *SSWR::AVIRead::AVIRCore::GetIOPinTypeName(IOPinType iopt)
{
	switch (iopt)
	{
	case AVIRCore::IOPT_GPIO:
		return (const UTF8Char*)"GPIO";
	case AVIRCore::IOPT_VIOPIN:
		return (const UTF8Char*)"VirtualIOPin";
	default:
		return (const UTF8Char*)"Unknown";
	}
}
